/* Recursive-descent parser emitting x86-64 directly (no AST).
 * Scope: M1-M10 — ints, chars, char/string literals, control flow,
 * calls, globals, arrays/pointers with scaling, structs (./->, struct
 * pointer params, self-referential tags), printf lowering, compound
 * assignment, bitwise and shift operators, comma, address-of,
 * deref, bitwise NOT, inc/dec. External absolute addresses (globals,
 * strings, arena bases) route through CCQuadAbs so M10 can relocate.
 * Codegen model: expression value in RAX unless cc->is_addr says RAX
 * holds a pending ADDRESS (see CCCtrl comment in CC.HH). Binaries via
 * PUSH/POP; comparisons materialize 0/1 with SETcc.
 * HolyC gotchas honored: no `for(;;)` in THIS source, no `continue`,
 * every &&/|| operand parenthesized, forward references need `extern`
 * prototypes. */

extern U0 CCExpr(CCCtrl *cc);
extern U0 CCExprNC(CCCtrl *cc);
extern U0 CCExprLogOr(CCCtrl *cc);
extern U0 CCExprBitOr(CCCtrl *cc);
extern U0 CCExprEquality(CCCtrl *cc);
extern U0 CCExprRelational(CCCtrl *cc);
extern U0 CCExprShift(CCCtrl *cc);
extern U0 CCExprAdd(CCCtrl *cc);
extern U0 CCExprMul(CCCtrl *cc);
extern U0 CCExprUnary(CCCtrl *cc);
extern U0 CCPrintf(CCCtrl *cc);
extern U0 CCAssignTail(CCCtrl *cc);
extern U0 CCCompoundAssign(CCCtrl *cc,I64 op);
extern U0 CCPrintf(CCCtrl *cc);
extern U0 CCStmt(CCCtrl *cc);
extern U0 CCDeclStmt(CCCtrl *cc);
//CCStagFind/CCMemberFind come from CC_SYM.HC (included first) and
//CCPostfix is defined below before its first use: extern re-declaring a
//symbol that is ALREADY defined in this unit links as "Undefined Extern".

Bool CCAccept(CCCtrl *cc,I64 ch)
{
  if ((cc->tk==TK_PUNCT)&&(cc->tk_num==ch))
  {
    CCLex(cc);
    return TRUE;
  }
  return FALSE;
}

Bool CCAcceptKw(I64 kw,CCCtrl *cc)
{
  if ((cc->tk==TK_IDENT)&&(cc->tk_kw==kw))
  {
    CCLex(cc);
    return TRUE;
  }
  return FALSE;
}

Bool CCExpect(CCCtrl *cc,I64 ch,U8 *msg)
{
  if (CCAccept(cc,ch))
    return TRUE;
  CCError(cc,msg);
  return FALSE;
}

Bool CCTkIsPunct(CCCtrl *cc,I64 ch)
{
  return ((cc->tk==TK_PUNCT)&&(cc->tk_num==ch));
}

/* Copy a string literal into the globals arena (re-MAlloced each
 * compile, never Free()d) and return its address. */
U8 *CCStrAlloc(CCCtrl *cc,U8 *bytes)
{
  I64 n=StrLen(bytes);
  U8 *dst;
  if ((cc->goff+n+1)>CC_LOCAL_SIZE)
  {
    CCError(cc,"string arena exhausted");
    return cc->locals;
  }
  dst=cc->locals+cc->goff;
  MemCpy(dst,bytes,n+1);
  cc->goff+=(n+8)&~7;		//rounded allocation; NUL included
  return dst;
}

/* Materialize a pending address: RAX must be a VALUE from here on.
 * A struct (ety==TY_STRUCT) is never loaded as a value: it keeps the
 * decayed address, so consumers see `&v` implicitly (see limitations). */
U0 CCVal(CCCtrl *cc)
{
  if ((cc->is_addr==1)&&(cc->ety!=TY_STRUCT))
  {
    if (cc->aty)
    {
      CCByte(cc,0x48);CCByte(cc,0x0F);CCByte(cc,0xB6);CCByte(cc,0x00);
      //MOVZX RAX,BYTE[RAX]
    }
    else
    {
      CCByte(cc,0x48);CCByte(cc,0x8B);CCByte(cc,0x00);	//MOV RAX,[RAX]
    }
  }
  if (cc->ety!=TY_STRUCT)
    cc->is_addr=0;	//is_addr==2 (array decay): already an address value
  cc->aty=0;
}

/* Element size an expression addresses/points to: 1 byte or 8. */
I64 CCPtrScale(I64 ty)
{
  if (ty==TY_CHARP)
    return 1;
  if (ty==TY_PTR)
    return 8;
  return 0;		//not pointer-like
}

/* M13 type layer. The compiler has ONE int width (8-byte slots) and a
 * byte model for char, so unsigned/long/signed fold to TY_INT, const
 * is a no-op qualifier, and void maps to the byte (char) model —
 * `void *` then behaves exactly like memcpy's byte-pointer contract.
 * typedef names bind a TY_* base and are consulted ONLY in type
 * position: an existing symbol of any kind shadows a typedef name
 * (C's dominance rule, approximately). */
I64 CCTypedefFind(CCCtrl *cc,U8 *nm)
{
  I64 i;
  if (CCSymFind(cc,nm))
    return -1;		//a real variable/function owns the name
  for (i=0;i!=cc->tn;i++)
    if (!StrCmp(cc->tname[i],nm))
      return i;
  return -1;
}

/* TRUE when the current token can START a declaration/function type.
 * Drives the dispatch gates that used to test for int/char/struct. */
I64 CCTokTypeStart(CCCtrl *cc)
{
  I64 kw;
  if (cc->tk!=TK_IDENT)
    return FALSE;
  kw=cc->tk_kw;
  if ((kw==KW_INT)||(kw==KW_CHAR))
    return TRUE;
  if ((kw==KW_STRUCT)||(kw==KW_CONST))
    return TRUE;
  if ((kw==KW_VOID)||(kw==KW_UNSIGNED))
    return TRUE;
  if ((kw==KW_SIGNED)||(kw==KW_LONG))
    return TRUE;
  if (kw==KW_NONE)
    return CCTypedefFind(cc,cc->tk_str)>=0;
  return FALSE;
}

/* Consume a full type-specifier sequence and return its base type, or
 * -1 when the current token starts none (so the caller can fall back
 * to an expression). *pstag receives the struct tag id, else -1.
 * Sequences like `unsigned long int` collapse; mixing char with the
 * int family, or void with anything, is an error. */
I64 CCAcceptBase(CCCtrl *cc,I64 *pstag)
{
  I64 ty=-1,ti;
  *pstag=-1;
  //No `continue` in HolyC (the keyword does not exist in this dialect):
  //the fold is one if/else chain per token, and each arm advances or
  //breaks. See PLAN.
  while (cc->tk==TK_IDENT)
  {
    if ((cc->tk_kw==KW_CONST)||(cc->tk_kw==KW_SIGNED))
      CCLex(cc);
    else if ((cc->tk_kw==KW_UNSIGNED)||(cc->tk_kw==KW_LONG)||(cc->tk_kw==KW_INT))
    {
      if (ty==TY_CHAR)
      {
        CCError(cc,"type keyword conflict (char with int/long/unsigned)");
        return -1;
      }
      ty=TY_INT;
      CCLex(cc);
    }
    else if (cc->tk_kw==KW_CHAR)
    {
      if ((ty==TY_INT)||(ty==TY_VOID))
      {
        CCError(cc,"type keyword conflict");
        return -1;
      }
      ty=TY_CHAR;
      CCLex(cc);
    }
    else if (cc->tk_kw==KW_VOID)
    {
      if (ty>=0)
      {
        CCError(cc,"void cannot combine with another type");
        return -1;
      }
      ty=TY_VOID;
      CCLex(cc);
    }
    else if (cc->tk_kw==KW_STRUCT)
    {
      if (ty>=0)
      {
        CCError(cc,"struct cannot combine with another type");
        return -1;
      }
      CCLex(cc);
      if (cc->tk!=TK_IDENT)
      {
        CCError(cc,"expected struct tag name");
        return -1;
      }
      ti=CCStagFind(cc,cc->tk_str);
      if (ti<0)
      {
        CCError(cc,"undefined struct tag");
        return -1;
      }
      *pstag=ti;
      ty=TY_STRUCT;
      CCLex(cc);
      break;		//tag consumed: the declarator follows
    }
    else
    {
      //Plain identifier: only a typedef name here is a type; anything
      //else (a variable, a call target) leaves the declarator/expression
      //for the caller.
      ti=CCTypedefFind(cc,cc->tk_str);
      if (ti<0)
        break;
      ty=cc->tty[ti];
      CCLex(cc);
      break;		//the typedef supplies the whole base type
    }
  }
  return ty;
}

/* typedef <base> [*] Name ; — registers Name as a type alias. Struct
 * typedefs are not implemented (the tag system is name-keyed and
 * single-sourced at file scope; see M9). */
U0 CCParseTypedef(CCCtrl *cc)
{
  I64 ty,stag;
  CCLex(cc);			//consume 'typedef'
  ty=CCAcceptBase(cc,&stag);
  if (ty<0)
  {
    CCError(cc,"typedef needs a supported base type");
    return;
  }
  if (ty==TY_VOID)
    ty=TY_CHAR;		//typedef void *vp: vp is a byte pointer
  while (CCAccept(cc,'*'))
  {
    if (ty==TY_CHAR)
      ty=TY_CHARP;
    else
      ty=TY_PTR;
  }
  if (cc->tk!=TK_IDENT)
  {
    CCError(cc,"expected typedef name");
    return;
  }
  if (cc->tn>=CC_MAX_TYPEDEF)
  {
    CCError(cc,"too many typedefs");
    return;
  }
  StrCpy(cc->tname[cc->tn],cc->tk_str);
  cc->tty[cc->tn]=ty;
  cc->tn++;
  CCLex(cc);
  CCExpect(cc,';',"expected ';' after typedef");
}

U0 CCBinEmit(CCCtrl *cc,I64 op)
{
  I64 rem;
  //RAX=right operand; fetch left back from the stack into RCX.
  switch (op)
  {
    case '+':
      CCByte(cc,0x59);		//POP RCX
      CCByte(cc,0x48);CCByte(cc,0x01);CCByte(cc,0xC8);	//ADD RAX,RCX
      break;
    case '-':
      CCByte(cc,0x59);		//POP RCX
      CCByte(cc,0x48);CCByte(cc,0x29);CCByte(cc,0xC1);	//SUB RCX,RAX
      CCByte(cc,0x48);CCByte(cc,0x89);CCByte(cc,0xC8);	//MOV RAX,RCX
      break;
    case '*':
      CCByte(cc,0x59);		//POP RCX
      CCByte(cc,0x48);CCByte(cc,0x0F);CCByte(cc,0xAF);CCByte(cc,0xC1);	//IMUL RAX,RCX
      break;
    case '/':
    case '%':
      rem=(op=='%');
      CCByte(cc,0x59);		//POP RCX (left operand)
      CCByte(cc,0x48);CCByte(cc,0x91);		//XCHG RAX,RCX
      CCByte(cc,0x48);CCByte(cc,0x99);		//CQO
      CCByte(cc,0x48);CCByte(cc,0xF7);CCByte(cc,0xF9);	//IDIV RCX
      if (rem)
      {
        CCByte(cc,0x48);CCByte(cc,0x89);CCByte(cc,0xD0);	//MOV RAX,RDX
      }
      break;
    case '&':
      CCByte(cc,0x59);		//POP RCX
      CCByte(cc,0x48);CCByte(cc,0x21);CCByte(cc,0xC8);	//AND RAX,RCX
      break;
    case '|':
      CCByte(cc,0x59);		//POP RCX
      CCByte(cc,0x48);CCByte(cc,0x09);CCByte(cc,0xC8);	//OR RAX,RCX
      break;
    case '^':
      CCByte(cc,0x59);		//POP RCX
      CCByte(cc,0x48);CCByte(cc,0x31);CCByte(cc,0xC8);	//XOR RAX,RCX
      break;
    case OP_SHL:
    case OP_SHR:
      //Shift distance must sit in CL: POP RCX(left), XCHG so RAX holds
      //the value being shifted, then shift by CL.
      CCByte(cc,0x59);		//POP RCX (left operand)
      CCByte(cc,0x48);CCByte(cc,0x91);		//XCHG RAX,RCX
      CCByte(cc,0x48);CCByte(cc,0xD3);
      if (op==OP_SHL)
        CCByte(cc,0xE0);	//SHL RAX,CL
      else
        CCByte(cc,0xF8);	//SAR RAX,CL (arithmetic, signed model)
      break;
    default:
      CCError(cc,"bad binary op");
      break;
  }
  cc->is_addr=0;
  cc->aty=0;
  cc->sty=-1;		//arithmetic results are never struct-related
}

//CMP RCX,RAX leaves LEFT-RIGHT flags for the SETcc.
U0 CCCompareEmit(CCCtrl *cc,I64 op)
{
  CCByte(cc,0x59);		//POP RCX
  //NOTE: 0x3B not 0x3A — 3A is CMP r8 (BYTE opcode, REX.W is ignored,
  //so `48 3A` silently compared only the low byte). 3B/RAX rm=RAX...
  //modrm C8: reg=RCX, rm=RAX -> RCX-RAX = left-right ✓.
  CCByte(cc,0x48);CCByte(cc,0x3B);CCByte(cc,0xC8);	//CMP RCX,RAX
  switch (op)
  {
    case OP_EQ:
      CCSetccRax(cc,CC_OPC_EQ);
      break;
    case OP_NE:
      CCSetccRax(cc,CC_OPC_NE);
      break;
    case '<':
      CCSetccRax(cc,CC_OPC_L);
      break;
    case OP_LE:
      CCSetccRax(cc,CC_OPC_LE);
      break;
    case '>':
      CCSetccRax(cc,CC_OPC_G);
      break;
    case OP_GE:
      CCSetccRax(cc,CC_OPC_GE);
      break;
    default:
      CCError(cc,"bad compare op");
      break;
  }
  cc->is_addr=0;
  cc->aty=0;
  cc->ety=TY_INT;
  cc->sty=-1;
}

/* Postfix chain on the current expression: `[i]`, `.m`, `->m`, mixed
 * freely (`vs[i].a`, `p->a`, `vs[i].next->k` if it were chained).
 * Invariant in: pending-lvalue state describes the base object.
 * `[i]` scales by the element size; struct elements scale by the tag
 * size and stay TY_STRUCT (tag preserved); members take their declared
 * TY and drop structness (nested structs are not supported). */
/* One member of tag bsty resolves to its declared type; a
 * `struct T *m` member keeps tag T so `n->next->v` chains. */
U0 CCPostfix(CCCtrl *cc)
{
  I64 ekind,off,mi,bsty;
  while ((!cc->errs)&&(cc->tk==TK_PUNCT)&&
      (((cc->tk_num=='[')||(cc->tk_num=='.'))||(cc->tk_num==OP_ARROW)))
  {
    if (cc->tk_num=='[')
    {
      bsty=-1;
      if (cc->ety==TY_STRUCT)
      {
        bsty=cc->sty;
        ekind=cc->st[bsty].size;	//element == whole struct
      }
      else if ((cc->ety==TY_PTR)&&(cc->sty>=0))
      {
        //`ps[i]` where ps is a struct pointer (incl. `struct T ps[]`
        //params): elements are whole structs; CCVal loads the pointer.
        bsty=cc->sty;
        ekind=cc->st[bsty].size;
      }
      else
      {
        ekind=CCPtrScale(cc->ety);
        if (!ekind)
          ekind=8;		//lenient: index a plain int as slots
      }
      CCVal(cc);
      CCLex(cc);
      CCByte(cc,0x50);		//PUSH RAX (base address)
      CCExpr(cc);		//index (commas allowed inside [])
      CCExpect(cc,']',"expected ']' after index");
      CCByte(cc,0x59);		//POP RCX (base)
      if (ekind==8)
      {
        CCByte(cc,0x48);CCByte(cc,0x6B);CCByte(cc,0xC0);CCByte(cc,0x08);
        //IMUL RAX,RAX,8
      }
      else if (ekind!=1)
      {
        CCByte(cc,0x48);CCByte(cc,0x69);CCByte(cc,0xC0);
        CCWord(cc,ekind);	//IMUL RAX,RAX,imm32 (struct element size)
      }
      CCByte(cc,0x48);CCByte(cc,0x01);CCByte(cc,0xC8);	//ADD RAX,RCX
      cc->is_addr=1;		//address of the element
      cc->aty=0;
      if (bsty>=0)
      {
        cc->ety=TY_STRUCT;	//a struct element is still the struct
        cc->sty=bsty;
      }
      else
      {
        cc->sty=-1;
        cc->aty=(ekind==1);
        if (ekind==1)
          cc->ety=TY_CHAR;
        else
          cc->ety=TY_INT;		//the ELEMENT's type
      }
    }
    else
    {
      if (cc->tk_num==OP_ARROW)
      {
        if ((cc->ety!=TY_PTR)||(cc->sty<0))
        {
          CCError(cc,"'->' needs a struct pointer");
          return;
        }
        CCVal(cc);		//the pointer VALUE
      }
      else if (cc->ety!=TY_STRUCT)
      {
        CCError(cc,"'.' needs a struct object");
        return;
      }
      bsty=cc->sty;
      CCLex(cc);		//consume '.' / '->'
      if (cc->tk!=TK_IDENT)
      {
        CCError(cc,"expected member name");
        return;
      }
      mi=CCMemberFind(cc,bsty,cc->tk_str);
      if (mi<0)
      {
        CCError(cc,"no such struct member");
        return;
      }
      CCLex(cc);
      off=cc->st[bsty].moff[mi];
      if (off)
      {
        CCByte(cc,0x48);CCByte(cc,0x05);	//ADD RAX,imm32
        CCWord(cc,off);
      }
      cc->ety=cc->st[bsty].mty[mi];
      cc->aty=(cc->ety==TY_CHAR);
      cc->sty=cc->st[bsty].msty[mi];	//struct-pointer members chain
      cc->is_addr=1;
    }
  }
}

/* M12: a call to a delegated stdlib function. Arguments are evaluated
 * left-to-right and staged into cc->ext[] (not pushed), then the
 * zero-arg kernel-backed helper is CALLed; it reads cc->ext, calls the
 * kernel, and returns the value in RAX. This keeps the activation
 * zero-arg — the HolyC JIT's bookkeeping around multi-arg indirect
 * calls is what M11 documented as unreliable, and the main-entry
 * argc/argv bridge already proved the staging pattern end to end. */
U0 CCLibCall(CCCtrl *cc,I64 slot,I64 argc,I64 rty)
{
  I64 i;
  for (i=0;i!=argc;i++)
  {
    if (i)
      CCExpect(cc,',' ,"expected ',' in stdlib call");
    CCExprNC(cc);
    CCPushExt(cc,i);
  }
  CCExpect(cc,')',"expected ')' after stdlib args");
  CCEmitLibCall(cc,slot);
  cc->is_addr=0;
  cc->aty=0;
  cc->ety=rty;
  cc->sty=-1;
}

U0 CCExprPrimary(CCCtrl *cc)
{
  CSym *s,*hs;
  I64 inc,n,delta,scale,byte_aty;
  U8 save_name[CC_TOK_MAX];
  U8 *save_pc;
  U8 *sp;
  I64 save_tk,save_kw,save_num;

  if (cc->errs)
    return;
  if (cc->tk==TK_NUM)
  {
    CCMovImmRax(cc,cc->tk_num);
    cc->is_addr=0;
    cc->aty=0;
    cc->ety=TY_INT;
    cc->sty=-1;
    CCLex(cc);
    return;
  }
  if (cc->tk==TK_STR)
  {
    sp=CCStrAlloc(cc,cc->tk_str);
    CCByte(cc,0x48);CCByte(cc,0xB8);	//MOVABS RAX,string
    CCQuadAbs(cc,CC_B_LOCALS,sp);
    cc->is_addr=0;
    cc->aty=0;
    cc->ety=TY_CHARP;
    cc->sty=-1;
    CCLex(cc);
    return;
  }
  if (cc->tk==TK_IDENT)
  {
    //Peek one token: IDENT '(' is a call, else a variable.
    StrCpy(save_name,cc->tk_str);
    save_pc=cc->pc;
    save_tk=cc->tk;
    save_kw=cc->tk_kw;
    save_num=cc->tk_num;
    CCLex(cc);
    if (CCTkIsPunct(cc,'('))
    {
      CCLex(cc);
      if (!StrCmp(save_name,"printf"))
      {
        //compile-time format lowering into prelude helper calls.
        CCPrintf(cc);
        return;
      }
      if (!StrCmp(save_name,"putchar"))
      {
        CCExprNC(cc);
        CCExpect(cc,')',"expected ')' after putchar arg");
        CCEmitPutchar(cc);
        cc->is_addr=0;
        cc->aty=0;
        cc->ety=TY_INT;
        cc->sty=-1;
        return;
      }
      if (!StrCmp(save_name,"exit"))
      {
        //exit(code): return to the harness from any depth, RAX=code.
        CCExprNC(cc);
        CCExpect(cc,')',"expected ')' after exit code");
        CCFunRet(cc,TRUE);
        cc->is_addr=0;
        cc->aty=0;
        cc->ety=TY_INT;
        cc->sty=-1;
        return;
      }
      /* M12: kernel-delegated stdlib (CC_LIB.HC helpers). Results are
       * byte-oriented pointers (TY_CHARP): arrays and literals are
       * byte-addressed in this model; malloc yields byte storage. */
      if (!StrCmp(save_name,"strlen"))
      {
        CCLibCall(cc,CC_LIB_STRLEN,1,TY_INT);
        return;
      }
      if (!StrCmp(save_name,"memcpy"))
      {
        CCLibCall(cc,CC_LIB_MEMCPY,3,TY_CHARP);
        return;
      }
      if (!StrCmp(save_name,"memset"))
      {
        CCLibCall(cc,CC_LIB_MEMSET,3,TY_CHARP);
        return;
      }
      if (!StrCmp(save_name,"memcmp"))
      {
        CCLibCall(cc,CC_LIB_MEMCMP,3,TY_INT);
        return;
      }
      if (!StrCmp(save_name,"malloc"))
      {
        CCLibCall(cc,CC_LIB_MALLOC,1,TY_CHARP);
        return;
      }
      if (!StrCmp(save_name,"getchar"))
      {
        //Zero-arg: no staging, just the helper call. fgetc/getc are
        //prelude wrappers calling this builtin (see CC_PRELUDE).
        CCLibCall(cc,CC_LIB_GETCHAR,0,TY_INT);
        return;
      }
      if (!StrCmp(save_name,"abort"))
      {
        //abort(): exit-shaped return to the harness with 134.
        CCExpect(cc,')',"expected ')' after abort");
        CCMovImmRax(cc,134);
        CCFunRet(cc,TRUE);
        cc->is_addr=0;
        cc->aty=0;
        cc->ety=TY_INT;
        cc->sty=-1;
        return;
      }
      if (!StrCmp(save_name,"sprintf"))
      {
        //Unlike printf, the FORMAT is interpreted at RUNTIME by the
        //prelude engine — the user may pass a variable. dst, fmt, then
        //up to 4 conversion args, zero-padded to the fixed 6-arg frame
        //the engine declares (extra user args are a diagnostic).
        hs=CCSymFind(cc,"cc_sprintf");
        if (!hs)
        {
          CCError(cc,"sprintf: prelude missing");
          return;
        }
        n=0;
        while ((!cc->errs)&&(n<6))
        {
          CCExprNC(cc);		//dst, fmt, then conversion args
          CCByte(cc,0x50);	//PUSH RAX
          n++;
          if (!CCAccept(cc,','))
            break;
        }
        while (n<6)
        {
          CCMovImmRax(cc,0);	//pad the engine's fixed frame
          CCByte(cc,0x50);
          n++;
        }
        CCExpect(cc,')',"expected ')' after sprintf args");
        CCCall(cc,hs);
        CCPopArgs(cc,6);
        cc->is_addr=0;
        cc->aty=0;
        cc->ety=TY_INT;
        cc->sty=-1;
        return;
      }
      n=0;
      if (!CCTkIsPunct(cc,')'))
      {
        while ((!cc->errs)&&(cc->tk!=TK_EOF))
        {
          CCExprNC(cc);		//commas here are ARGUMENT separators
          CCByte(cc,0x50);		//PUSH RAX (arg)
          n++;
          if (CCAccept(cc,','))
          {
            //Next argument.
          }
          else
            break;
        }
      }
      CCExpect(cc,')',"expected ')' after args");
      s=CCSymFind(cc,save_name);
      if (!s)
        s=CCSymNew(cc,save_name,SC_FUN);	//forward reference
      if (s->kind!=SC_FUN)
        CCError(cc,"not a function");
      else
        CCCall(cc,s);		//queues if defined later
      CCPopArgs(cc,n);
      cc->is_addr=0;
      cc->aty=0;
      cc->ety=TY_INT;
      cc->sty=-1;
      return;
    }
    //Not a call: rewind and treat as a variable.
    StrCpy(cc->tk_str,save_name);
    cc->pc=save_pc;
    cc->tk=save_tk;
    cc->tk_kw=save_kw;
    cc->tk_num=save_num;
    s=CCSymFind(cc,cc->tk_str);
    if (!s)
    {
      CCError(cc,"undefined variable");
      return;
    }
    CCVarAddr(cc,s);		//RAX = address of storage
    if ((s->dim>1)||(s->ty==TY_STRUCT))
    {
      //Array or struct name: value IS the address (decay); never load.
      cc->is_addr=2;
      cc->ety=s->ty;
      cc->aty=(s->ty==TY_CHARP);
    }
    else
    {
      cc->is_addr=1;
      cc->ety=s->ty;
      cc->aty=(s->ty==TY_CHAR);
    }
    cc->sty=s->sty;
    CCLex(cc);
    CCPostfix(cc);
    if ((!cc->errs)&&((CCTkIsPunct(cc,OP_INC))||(CCTkIsPunct(cc,OP_DEC))))
    {
      inc=(CCTkIsPunct(cc,OP_INC));
      CCLex(cc);
      if (cc->is_addr!=1)
        CCError(cc,"++/-- needs a modifiable object");
      else
      {
        byte_aty=cc->aty;
        scale=CCPtrScale(cc->ety);
        delta=1;
        if (scale>0)
          delta=scale;
        CCByte(cc,0x48);CCByte(cc,0x89);CCByte(cc,0xC1);	//MOV RCX,RAX
        if (byte_aty)
        {
          CCByte(cc,0x48);CCByte(cc,0x0F);CCByte(cc,0xB6);CCByte(cc,0x00);
          //MOVZX RAX,BYTE[RAX] (old value)
        }
        else
        {
          CCByte(cc,0x48);CCByte(cc,0x8B);CCByte(cc,0x00);	//MOV RAX,[RAX]
        }
        CCByte(cc,0x50);		//PUSH RAX (old: the expression result)
        CCByte(cc,0x48);CCByte(cc,0x83);
        if (inc)
        {
          CCByte(cc,0xC0);CCByte(cc,delta);	//ADD RAX,delta
        }
        else
        {
          CCByte(cc,0xE8);CCByte(cc,delta);	//SUB RAX,delta
        }
        if (byte_aty)
        {
          CCByte(cc,0x88);CCByte(cc,0x01);	//MOV [RCX],AL
        }
        else
        {
          CCByte(cc,0x48);CCByte(cc,0x89);CCByte(cc,0x01);	//MOV [RCX],RAX
        }
        CCByte(cc,0x58);		//POP RAX (old value back)
        cc->is_addr=0;
        cc->aty=0;
        cc->sty=-1;
      }
    }
    return;
  }
  if (CCAccept(cc,'('))
  {
    //Parenthesized: keep any pending lvalue status so `(*p)=x` works;
    //commas inside are the operator, so the last operand's status wins.
    CCExprLogOr(cc);
    while ((!cc->errs)&&(CCTkIsPunct(cc,',')))
    {
      CCLex(cc);
      CCVal(cc);		//discard the left operand's value
      CCExprLogOr(cc);
    }
    CCExpect(cc,')',"expected ')'");
    CCPostfix(cc);		//enables `(*p).m`
    return;
  }
  CCError(cc,"expected number, string, variable or '('");
}

U0 CCExprUnary(CCCtrl *cc)
{
  if (CCAccept(cc,'-'))
  {
    CCExprUnary(cc);
    CCVal(cc);
    CCByte(cc,0x48);CCByte(cc,0xF7);CCByte(cc,0xD8);	//NEG RAX
    cc->ety=TY_INT;
    return;
  }
  if (CCAccept(cc,'!'))
  {
    CCExprUnary(cc);
    CCVal(cc);
    CCTestRax(cc);
    CCSetccRax(cc,CC_OPC_EQ);		//!x
    cc->ety=TY_INT;
    return;
  }
  if (CCAccept(cc,'~'))
  {
    CCExprUnary(cc);
    CCVal(cc);
    CCByte(cc,0x48);CCByte(cc,0xF7);CCByte(cc,0xD0);	//NOT RAX
    cc->ety=TY_INT;
    return;
  }
  if (CCAccept(cc,'&'))
  {
    //Operand parse leaves RAX = its address (is_addr 1 or 2); the
    //address itself becomes the value — no code needed. &struct makes
    //a struct pointer and KEEPS the tag, so the result supports '->'.
    CCExprUnary(cc);
    if (!cc->is_addr)
      CCError(cc,"cannot take address of a value");
    else if (cc->ety==TY_STRUCT)
    {
      cc->ety=TY_PTR;		//sty already names the tag
      cc->is_addr=0;
      cc->aty=0;
    }
    else
    {
      if (cc->aty)
        cc->ety=TY_CHARP;
      else
        cc->ety=TY_PTR;
      cc->is_addr=0;
      cc->aty=0;
      cc->sty=-1;
    }
    return;
  }
  if (CCAccept(cc,'*'))
  {
    //Rvalue use of *e: the address IS the operand's value. Dereferencing
    //a struct pointer yields the struct (address semantics, tag kept),
    //so `(*p).m` and `*p=m` style member paths work.
    CCExprUnary(cc);
    if ((cc->ety==TY_PTR)&&(cc->sty>=0))
    {
      CCVal(cc);
      cc->ety=TY_STRUCT;	//sty survives: still the same tag
      cc->is_addr=1;
      cc->aty=0;
    }
    else
    {
      CCVal(cc);
      cc->aty=((cc->ety==TY_CHARP)||(cc->ety==TY_CHAR));
      if (cc->aty)
        cc->ety=TY_CHAR;
      else
        cc->ety=TY_INT;
      cc->is_addr=1;
      cc->sty=-1;
    }
    return;
  }
  CCExprPrimary(cc);
}

U0 CCExprMul(CCCtrl *cc)
{
  I64 op;
  CCExprUnary(cc);
  if ((!cc->errs)&&CCTkIsPunct(cc,'='))
  {
    if (!cc->is_addr)
      CCError(cc,"assignment to non-lvalue");
    else if (cc->is_addr==2)
      CCError(cc,"assignment to array");
    else
      CCAssignTail(cc);
    return;
  }
  //Compound assignment ops are the contiguous OP_PLUSEQ..OP_SHREQ block.
  if ((!cc->errs)&&(cc->tk==TK_PUNCT)&&
      ((cc->tk_num>=OP_PLUSEQ)&&(cc->tk_num<=OP_SHREQ)))
  {
    if (!cc->is_addr)
      CCError(cc,"compound assignment to non-lvalue");
    else if (cc->is_addr==2)
      CCError(cc,"compound assignment to array");
    else
      CCCompoundAssign(cc,cc->tk_num);
    return;
  }
  while ((!cc->errs)&&(cc->tk==TK_PUNCT)&&
      (((cc->tk_num=='*')||(cc->tk_num=='/'))||(cc->tk_num=='%')))
  {
    op=cc->tk_num;
    CCLex(cc);
    CCVal(cc);
    CCByte(cc,0x50);		//PUSH RAX
    CCExprUnary(cc);
    CCVal(cc);
    CCBinEmit(cc,op);
    cc->ety=TY_INT;
  }
}

U0 CCExprAdd(CCCtrl *cc)
{
  I64 op,lty,rty,scale;
  CCExprMul(cc);
  while ((!cc->errs)&&(cc->tk==TK_PUNCT)&&
      ((cc->tk_num=='+')||(cc->tk_num=='-')))
  {
    op=cc->tk_num;
    lty=cc->ety;
    scale=CCPtrScale(lty);
    CCLex(cc);
    CCVal(cc);
    CCByte(cc,0x50);		//PUSH RAX
    CCExprMul(cc);
    rty=cc->ety;
    CCVal(cc);
    if ((scale!=0)&&((rty==TY_INT)||(rty==TY_CHAR)))
    {
      //pointer + integer scales the integer by the element size.
      if (scale==8)
      {
        CCByte(cc,0x48);CCByte(cc,0x6B);CCByte(cc,0xC0);CCByte(cc,0x08);
        //IMUL RAX,RAX,8
      }
      cc->ety=lty;		//pointer arithmetic stays pointer-typed
    }
    else
      cc->ety=TY_INT;
    CCBinEmit(cc,op);
    if (scale!=0)
      cc->ety=lty;
  }
}

//C precedence: shifts bind tighter than relational ops.
U0 CCExprShift(CCCtrl *cc)
{
  I64 op;
  CCExprAdd(cc);
  while ((!cc->errs)&&(cc->tk==TK_PUNCT)&&
      ((cc->tk_num==OP_SHL)||(cc->tk_num==OP_SHR)))
  {
    op=cc->tk_num;
    CCLex(cc);
    CCVal(cc);
    CCByte(cc,0x50);		//PUSH RAX
    CCExprAdd(cc);
    CCVal(cc);
    CCBinEmit(cc,op);
    cc->ety=TY_INT;
  }
}

U0 CCExprRelational(CCCtrl *cc)
{
  I64 op;
  CCExprShift(cc);
  while ((!cc->errs)&&((cc->tk==TK_PUNCT)&&
      (((cc->tk_num=='<')||(cc->tk_num=='>'))||
      ((cc->tk_num==OP_LE)||(cc->tk_num==OP_GE)))))
  {
    op=cc->tk_num;
    CCLex(cc);
    CCVal(cc);
    CCByte(cc,0x50);		//PUSH RAX
    CCExprShift(cc);
    CCVal(cc);
    CCCompareEmit(cc,op);
  }
}

U0 CCExprEquality(CCCtrl *cc)
{
  I64 op;
  CCExprRelational(cc);
  while ((!cc->errs)&&((cc->tk==TK_PUNCT)&&
      ((cc->tk_num==OP_EQ)||(cc->tk_num==OP_NE))))
  {
    op=cc->tk_num;
    CCLex(cc);
    CCVal(cc);
    CCByte(cc,0x50);		//PUSH RAX
    CCExprRelational(cc);
    CCVal(cc);
    CCCompareEmit(cc,op);
  }
}

//Bitwise levels, in C order: & tighter than ^ tighter than |, all
//looser than equality and tighter than &&. Single '&' here is always
//binary — unary '&' sits in prefix position (CCExprUnary).
U0 CCExprBitAnd(CCCtrl *cc)
{
  CCExprEquality(cc);
  while ((!cc->errs)&&CCTkIsPunct(cc,'&'))
  {
    CCLex(cc);
    CCVal(cc);
    CCByte(cc,0x50);		//PUSH RAX
    CCExprEquality(cc);
    CCVal(cc);
    CCBinEmit(cc,'&');
    cc->ety=TY_INT;
  }
}

U0 CCExprBitXor(CCCtrl *cc)
{
  CCExprBitAnd(cc);
  while ((!cc->errs)&&CCTkIsPunct(cc,'^'))
  {
    CCLex(cc);
    CCVal(cc);
    CCByte(cc,0x50);		//PUSH RAX
    CCExprBitAnd(cc);
    CCVal(cc);
    CCBinEmit(cc,'^');
    cc->ety=TY_INT;
  }
}

U0 CCExprBitOr(CCCtrl *cc)
{
  CCExprBitXor(cc);
  while ((!cc->errs)&&CCTkIsPunct(cc,'|'))
  {
    CCLex(cc);
    CCVal(cc);
    CCByte(cc,0x50);		//PUSH RAX
    CCExprBitXor(cc);
    CCVal(cc);
    CCBinEmit(cc,'|');
    cc->ety=TY_INT;
  }
}

//a && b:  false left short-circuits (RAX already 0);
//otherwise RAX = normalized right.
U0 CCExprLogAnd(CCCtrl *cc)
{
  I64 pz;
  CCExprBitOr(cc);
  CCVal(cc);
  while ((!cc->errs)&&((cc->tk==TK_PUNCT)&&(cc->tk_num==OP_AND)))
  {
    CCLex(cc);
    CCTestRax(cc);
    pz=CCJz(cc);
    CCExprBitOr(cc);
    CCVal(cc);
    CCTestRax(cc);
    CCSetccRax(cc,CC_OPC_NE);
    CCPatchJmp(cc,pz);
  }
  cc->ety=TY_INT;
  cc->sty=-1;
}

//a || b:  true left yields 1; otherwise normalized right.
U0 CCExprLogOr(CCCtrl *cc)
{
  I64 pz,pj;
  CCExprLogAnd(cc);
  while ((!cc->errs)&&((cc->tk==TK_PUNCT)&&(cc->tk_num==OP_OR)))
  {
    CCLex(cc);
    CCTestRax(cc);
    pz=CCJz(cc);
    CCMovImmRax(cc,1);
    pj=CCJmp(cc);
    CCPatchJmp(cc,pz);
    CCExprLogAnd(cc);
    CCVal(cc);
    CCTestRax(cc);
    CCSetccRax(cc,CC_OPC_NE);
    CCPatchJmp(cc,pj);
  }
  cc->ety=TY_INT;
  cc->sty=-1;
}

/* RAX holds the destination address (is_addr==1); the current token is
 * `=`. Parse the RHS and store. Byte-sized when aty was set. */
U0 CCAssignTail(CCCtrl *cc)
{
  I64 saty,lty,lsty;
  saty=cc->aty;
  lty=cc->ety;		//lhs type, before the RHS parse overwrites it
  lsty=cc->sty;		//lhs struct tag (valid when lty==TY_PTR)
  CCLex(cc);		//consume '='
  CCByte(cc,0x50);	//PUSH RAX (destination address)
  CCExprNC(cc);		//RHS value (another '=' chains right-associatively)
  CCByte(cc,0x59);	//POP RCX (address)
  if (saty)
  {
    CCByte(cc,0x88);CCByte(cc,0x01);	//MOV [RCX],AL
  }
  else
  {
    CCByte(cc,0x48);CCByte(cc,0x89);CCByte(cc,0x01);	//MOV [RCX],RAX
  }
  cc->is_addr=0;
  cc->aty=0;
  //The assign-expression takes the LHS type. Typing every '=' result as
  //a pointer made `(a=5)+1` scale the 1 by 8; only real pointers may.
  if ((lty==TY_PTR)||(lty==TY_CHARP))
    cc->ety=lty;
  else
    cc->ety=TY_INT;
  //Struct-pointer-ness comes from the lhs declaration (q=&v keeps q's tag).
  if (lty==TY_PTR)
    cc->sty=lsty;
  else
    cc->sty=-1;
}

/* lhs op= rhs with the lhs address in RAX (is_addr==1).
 * Layout: PUSH the address (it must survive CALLs inside the RHS), load
 * the old value and PUSH it, parse the RHS, then CCBinEmit pops the old
 * value as its left operand — leaving the address to POP+store. RAX
 * already holds the new value, which is also the expression's value. */
U0 CCCompoundAssign(CCCtrl *cc,I64 op)
{
  I64 base,lty,saty,lsty,scale;
  switch (op)
  {
    case OP_PLUSEQ:
      base='+';
      break;
    case OP_MINUSEQ:
      base='-';
      break;
    case OP_STAREQ:
      base='*';
      break;
    case OP_SLASHEQ:
      base='/';
      break;
    case OP_PCTEQ:
      base='%';
      break;
    case OP_AMPEQ:
      base='&';
      break;
    case OP_PIPEEQ:
      base='|';
      break;
    case OP_CARETEQ:
      base='^';
      break;
    case OP_SHLEQ:
      base=OP_SHL;
      break;
    case OP_SHREQ:
      base=OP_SHR;
      break;
    default:
      CCError(cc,"bad compound assignment op");
      return;
  }
  lty=cc->ety;
  saty=cc->aty;
  lsty=cc->sty;
  CCByte(cc,0x50);		//PUSH RAX (address)
  if (saty)
  {
    CCByte(cc,0x48);CCByte(cc,0x0F);CCByte(cc,0xB6);CCByte(cc,0x00);
    //MOVZX RAX,BYTE[RAX] — old value; RAX was the address until here
  }
  else
  {
    CCByte(cc,0x48);CCByte(cc,0x8B);CCByte(cc,0x00);	//MOV RAX,[RAX]
  }
  CCByte(cc,0x50);		//PUSH RAX (old value, above the address)
  CCLex(cc);			//consume op=
  CCExprNC(cc);			//RHS value
  if ((base=='+')||(base=='-'))
  {
    //`p+=n` scales n by the element size, like CCExprAdd does.
    scale=CCPtrScale(lty);
    if ((scale==8)&&((cc->ety==TY_INT)||(cc->ety==TY_CHAR)))
    {
      CCByte(cc,0x48);CCByte(cc,0x6B);CCByte(cc,0xC0);CCByte(cc,0x08);
      //IMUL RAX,RAX,8
    }
  }
  CCBinEmit(cc,base);		//pops the OLD value as left operand
  CCByte(cc,0x59);		//POP RCX (address)
  if (saty)
  {
    CCByte(cc,0x88);CCByte(cc,0x01);	//MOV [RCX],AL (RAX preserved)
  }
  else
  {
    CCByte(cc,0x48);CCByte(cc,0x89);CCByte(cc,0x01);	//MOV [RCX],RAX
  }
  cc->is_addr=0;
  cc->aty=0;
  if ((lty==TY_PTR)||(lty==TY_CHARP))
    cc->ety=lty;
  else
    cc->ety=TY_INT;
  if (lty==TY_PTR)
    cc->sty=lsty;		//p+=1 stays a struct pointer
  else
    cc->sty=-1;
}

/* printf: format must be a string literal; `%d %s %c %%` are lowered
 * at compile time to calls into the prelude helpers (CCItoa, CCStr,
 * CCOutByte). Arguments are evaluated left-to-right onto the stack,
 * moved into a fixed 8-slot frame area, then read per directive.
 * Up to 8 args. Returns 0 (C's byte count is not tracked). */
U0 CCPrintf(CCCtrl *cc)
{
  I64 i,j,l,n,argp,at;
  U8 fmt[CC_TOK_MAX];
  U8 run[CC_TOK_MAX];
  U8 *sp;
  CSym *hi,*hs,*ho;
  hi=CCSymFind(cc,"CCItoa");
  hs=CCSymFind(cc,"CCStr");
  ho=CCSymFind(cc,"CCOutByte");
  if ((hi==NULL)||((hs==NULL)||(ho==NULL)))
  {
    CCError(cc,"printf prelude missing");
    return;
  }
  if (cc->tk!=TK_STR)
  {
    CCError(cc,"printf: format must be a string literal");
    return;
  }
  StrCpy(fmt,cc->tk_str);
  CCLex(cc);
  n=0;
  while ((!cc->errs)&&(CCAccept(cc,',')))
  {
    if (n>=8)
    {
      CCError(cc,"too many printf args");
      return;
    }
    CCExprNC(cc);		//commas here separate args, not operators
    CCByte(cc,0x50);		//PUSH RAX
    n++;
  }
  CCExpect(cc,')',"expected ')' after printf args");
  if (cc->errs)
    return;
  //Frame: 64 bytes of slots below the n pushed args.
  CCByte(cc,0x48);CCByte(cc,0x81);CCByte(cc,0xEC);
  CCWord(cc,64);		//SUB RSP,64
  for (i=0;i!=n;i++)
  {
    //arg i sits at [RSP+64+8*(n-1-i)] now.
    CCByte(cc,0x48);CCByte(cc,0x8B);CCByte(cc,0x84);CCByte(cc,0x24);
    CCWord(cc,64+8*(n-1-i));	//MOV RAX,[RSP+d32]
    CCByte(cc,0x48);CCByte(cc,0x89);CCByte(cc,0x84);CCByte(cc,0x24);
    CCWord(cc,8*i);		//MOV [RSP+d32],RAX
  }
  argp=0;
  i=0;
  while ((!cc->errs)&&(fmt[i]))
  {
    if (fmt[i]!='%')
    {
      //Literal run: copy, pool-allocate, emit one CCStr call.
      j=i;
      while ((fmt[j])&&(fmt[j]!='%'))
        j++;
      if ((j-i)>CC_TOK_MAX-1)
      {
        CCError(cc,"internal: literal run");
        return;
      }
      for (l=0;l!=j-i;l++)
        run[l]=fmt[i+l];
      run[l]=0;
      sp=CCStrAlloc(cc,run);
      CCByte(cc,0x48);CCByte(cc,0xB8);	//MOVABS RAX,literal
      CCQuadAbs(cc,CC_B_LOCALS,sp);
      i=j;
      CCByte(cc,0x50);		//PUSH RAX
      CCCall(cc,hs);
      CCPopArgs(cc,1);
    }
    else
    {
      at=fmt[i+1];
      if (!at)
      {
        CCError(cc,"printf: trailing %");
        return;
      }
      if (at=='%')
      {
        CCMovImmRax(cc,37);
        CCByte(cc,0x50);
        CCCall(cc,ho);
        CCPopArgs(cc,1);
      }
      else if ((at=='d')||((at=='s')||(at=='c')))
      {
        if (argp>=n)
        {
          CCError(cc,"printf: not enough args");
          return;
        }
        CCByte(cc,0x48);CCByte(cc,0x8B);CCByte(cc,0x84);CCByte(cc,0x24);
        CCWord(cc,8*argp);	//MOV RAX,[RSP+8*slot]
        CCByte(cc,0x50);	//PUSH RAX
        if (at=='d')
          CCCall(cc,hi);
        else if (at=='s')
          CCCall(cc,hs);
        else
          CCCall(cc,ho);
        CCPopArgs(cc,1);
        argp++;
      }
      else
      {
        CCError(cc,"printf: unsupported format");
        return;
      }
      i=i+2;
    }
  }
  //Pop the slot frame AND the n argument pushes.
  CCByte(cc,0x48);CCByte(cc,0x81);CCByte(cc,0xC4);
  CCWord(cc,64+8*n);		//ADD RSP, imm32
  CCMovImmRax(cc,0);
  cc->is_addr=0;
  cc->aty=0;
  cc->ety=TY_INT;
  cc->sty=-1;
}

/* Comma-free expression: the value consumers (return, if/while cond,
 * function arguments) use. `f(a,b)` must NOT see commas as operators. */
U0 CCExprNC(CCCtrl *cc)
{
  CCExprLogOr(cc);
  CCVal(cc);
}

/* Expression with the comma operator (statements, for-clauses, []). */
U0 CCExpr(CCCtrl *cc)
{
  CCExprNC(cc);
  while ((!cc->errs)&&CCTkIsPunct(cc,','))
  {
    CCLex(cc);
    CCExprNC(cc);
  }
}

U0 CCIfStmt(CCCtrl *cc)
{
  I64 pz,pj;
  CCLex(cc);		//consume 'if'
  CCExpect(cc,'(',"expected '(' after if");
  CCExpr(cc);
  CCExpect(cc,')',"expected ')' after condition");
  CCTestRax(cc);
  pz=CCJz(cc);
  CCStmt(cc);
  if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_ELSE))
  {
    pj=CCJmp(cc);
    CCPatchJmp(cc,pz);
    CCLex(cc);
    CCStmt(cc);
    CCPatchJmp(cc,pj);
  }
  else
    CCPatchJmp(cc,pz);
}

U0 CCWhileStmt(CCCtrl *cc)
{
  I64 top,pz,save_b,save_c,i;
  CCLex(cc);		//consume 'while'
  CCExpect(cc,'(',"expected '(' after while");
  top=cc->ep-cc->code;
  CCExpr(cc);
  CCExpect(cc,')',"expected ')' after condition");
  CCTestRax(cc);
  pz=CCJz(cc);
  save_b=cc->brk_cnt;
  cc->brk_cnt=0;
  save_c=cc->cont_cnt;
  cc->cont_cnt=0;
  cc->in_loop++;		//M14: continue-validity context (CCContStmt)
  CCStmt(cc);
  cc->in_loop--;
  //continue re-tests the condition: land on the JMP-to-top.
  for (i=0;i!=cc->cont_cnt;i++)
    CCPatchJmp(cc,cc->cont[i]);
  cc->cont_cnt=0;
  CCByte(cc,0xE9);		//JMP top (rel32)
  CCWord(cc,top-(cc->ep-cc->code)-4);
  CCPatchJmp(cc,pz);
  for (i=0;i!=cc->brk_cnt;i++)
    CCPatchJmp(cc,cc->brk[i]);
  cc->brk_cnt=save_b;
  cc->cont_cnt=save_c;
}

/* M14: `do body while (cond);` — body first, condition re-tested at
 * the tail. continue jumps to the CONDITION (the do-while contract:
 * the guard always runs); break exits the loop. */
U0 CCDoStmt(CCCtrl *cc)
{
  I64 save_b,save_c,i,top,rel;
  CCLex(cc);		//consume 'do'
  save_b=cc->brk_cnt;
  cc->brk_cnt=0;
  save_c=cc->cont_cnt;
  cc->cont_cnt=0;
  top=cc->ep-cc->code;		//body top: the JNZ target
  cc->in_loop++;
  CCStmt(cc);		//body ({ } block, or one statement)
  cc->in_loop--;
  //continue lands here: on the condition.
  for (i=0;i!=cc->cont_cnt;i++)
    CCPatchJmp(cc,cc->cont[i]);
  cc->cont_cnt=0;
  if (!((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_WHILE)))
  {
    CCError(cc,"expected 'while' after do body");
    return;
  }
  CCLex(cc);		//consume 'while'
  CCExpect(cc,'(',"expected '(' after while");
  CCExpr(cc);
  CCExpect(cc,')',"expected ')' after do-while condition");
  CCExpect(cc,';',"expected ';' after do-while condition");
  CCTestRax(cc);
  //JNZ rel32 back to the body top: target known, emit directly.
  //The rel value must be computed AFTER the two opcode bytes are
  //emitted — rel is relative to the instruction AFTER the displacement
  //(pre-opcode arithmetic landed jumps 2 bytes late: the do-while
  //SIGILL).
  CCByte(cc,0x0F);
  CCByte(cc,0x85);
  rel=top-(cc->ep-cc->code)-4;
  CCWord(cc,rel);
  for (i=0;i!=cc->brk_cnt;i++)
    CCPatchJmp(cc,cc->brk[i]);
  cc->brk_cnt=save_b;
  cc->cont_cnt=save_c;
}

/* M14: `switch (expr) { case K: ... default: ... }`.
 * Single pass, no rewind: the value is parked in a hidden local slot,
 * an entry JMP leaps over the body to a dispatch chain emitted AFTER
 * it, so every je target is known when emitted. Values matching no
 * case go to `default` if present, else fall out of the switch.
 * Breaks belong to the switch; `continue` inside a switch body still
 * targets an ENCLOSING loop (the cont frame is deliberately untouched
 * here; CCContStmt already rejects continue with no loop at all).
 * Fallthrough is C's: labels emit no jump-out, code flows on. */
U0 CCSwitchStmt(CCCtrl *cc)
{
  I64 disp,djz,eoj,save_b,i,def,nc,v,rel,fits;
  //Per-invocation (NESTED switches must not share a table: a ctrl-
  //level array let an inner switch overwrite its parent's case values
  //— cls(1,x) silently matched nothing).
  I64 cval[CC_SWITCH_CASES];
  I64 ctarg[CC_SWITCH_CASES];
  CCLex(cc);		//consume 'switch'
  CCExpect(cc,'(',"expected '(' after switch");
  CCExpr(cc);
  CCExpect(cc,')',"expected ')' after switch expression");
  //Park the value in its own local slot (the frame size is patched
  //at function end, so growing lofs here is safe).
  cc->lofs++;
  disp=-8*cc->lofs;
  CCByte(cc,0x48);CCByte(cc,0x89);CCByte(cc,0x85);	//MOV [RBP+d32],RAX
  CCWord(cc,disp);
  djz=CCJmp(cc);		//to the dispatch chain (patched at the end)
  save_b=cc->brk_cnt;		//breaks belong to this switch
  cc->brk_cnt=0;
  nc=0;
  def=-1;
  CCExpect(cc,'{',"expected '{' after switch header");
  while ((!cc->errs)&&(cc->tk!=TK_EOF)&&!CCTkIsPunct(cc,'}'))
  {
    if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_CASE))
    {
      CCLex(cc);
      v=1;
      if (CCAccept(cc,'-'))
        v=-1;
      if (cc->tk!=TK_NUM)
      {
        CCError(cc,"case needs an integer constant");
        return;
      }
      v=v*cc->tk_num;
      CCLex(cc);
      CCExpect(cc,':',"expected ':' after case value");
      if (nc>=CC_SWITCH_CASES)
      {
        CCError(cc,"too many case labels");
        return;
      }
      cval[nc]=v;
      ctarg[nc]=cc->ep-cc->code;
      nc++;
    }
    else if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_DEFAULT))
    {
      CCLex(cc);
      CCExpect(cc,':',"expected ':' after default");
      def=cc->ep-cc->code;
    }
    else
      CCStmt(cc);
  }
  CCExpect(cc,'}',"expected '}' after switch body");
  if (cc->errs)
    return;
  //Body execution ends here: a fall-off out of the last case must NOT
  //run the dispatch chain sitting physically after the body (it would
  //match its own case and loop forever). Jump to the switch end.
  eoj=CCJmp(cc);
  //The entry JMP lands AFTER this body-end jump — at the chain's
  //first compare. Patching it after the chain instead made the entry
  //leap over its own dispatch (every switch silently matched nothing).
  CCPatchJmp(cc,djz);
  //Dispatch chain, emitted after the body: every target is behind us.
  for (i=0;i!=nc;i++)
  {
    CCByte(cc,0x48);CCByte(cc,0x8B);CCByte(cc,0x85);	//MOV RAX,[RBP+d32]
    CCWord(cc,disp);
    v=cval[i];
    //HolyC integer literals are I64 already; no LL suffix exists.
    fits=(v>=(-2147483648))&&(v<=2147483647);
    if (fits)
    {
      CCByte(cc,0x48);CCByte(cc,0x3D);			//CMP RAX,imm32
      CCWord(cc,v);
    }
    else
    {
      CCByte(cc,0x48);CCByte(cc,0xB9);			//MOVABS RCX,v
      CCQuad(cc,v);
      CCByte(cc,0x48);CCByte(cc,0x39);CCByte(cc,0xC8);	//CMP RAX,RCX
    }
    CCByte(cc,0x0F);CCByte(cc,0x84);			//JE rel32 -> label
    rel=ctarg[i]-(cc->ep-cc->code)-4;
    CCWord(cc,rel);
  }
  if (def>=0)
  {
    CCByte(cc,0xE9);				//JMP default (rel32)
    rel=def-(cc->ep-cc->code)-4;
    CCWord(cc,rel);
  }
  //Here = switch end: the body-end jump and all breaks land here.
  CCPatchJmp(cc,eoj);
  for (i=0;i!=cc->brk_cnt;i++)
    CCPatchJmp(cc,cc->brk[i]);
  cc->brk_cnt=save_b;
}

/* for(init;cond;step) body — init may be a declaration; empty cond
 * means forever; empty step emits nothing. continue jumps past the
 * body INTO the step; break exits the loop skipping the step.
 * Emission is single-pass, but the step text precedes the body while
 * its code must follow it — so the step clause is LOCATED by a pure
 * lexer scan, the body parsed, then the step re-lexed from its text
 * and emitted into place (jump/call patches are correct because the
 * code lands at its final offsets). */
U0 CCForStmt(CCCtrl *cc)
{
  I64 top,pz,save_b,save_c,i,depth;
  I64 save_tk,save_kw,save_num;
  U8 save_str[CC_TOK_MAX];
  U8 *pc_step,*pc_after,*pc_body;
  pc_after=NULL;
  CCLex(cc);		//consume 'for'
  CCExpect(cc,'(',"expected '(' after for");
  if (CCTokTypeStart(cc))
    CCDeclStmt(cc);		//consumes its ';'
  else if (CCAccept(cc,';'))
  {
    //Empty init clause.
  }
  else
  {
    CCExpr(cc);
    CCExpect(cc,';',"expected ';' after for-init");
  }
  top=cc->ep-cc->code;
  if (CCTkIsPunct(cc,';'))
  {
    pc_step=cc->pc;		//past this ';' = start of step text
    CCLex(cc);
    pz=-1;			//no condition: loop forever
  }
  else
  {
    CCExpr(cc);
    pc_step=cc->pc;		//before the ';' lex: start of step text
    CCTestRax(cc);
    pz=CCJz(cc);
    CCExpect(cc,';',"expected ';' after for-condition");
  }
  //Scan from the already-lexed first step token to the closing ')'.
  depth=1;
  while ((!cc->errs)&&(cc->tk!=TK_EOF)&&(depth!=0))
  {
    if (cc->tk==TK_PUNCT)
    {
      if (cc->tk_num=='(')
        depth++;
      else if (cc->tk_num==')')
        depth--;
    }
    if ((depth!=0)&&(!cc->errs))
      CCLex(cc);
  }
  if (depth!=0)
  {
    CCError(cc,"unterminated for-header");
    return;
  }
  pc_after=cc->pc;		//just past ')': the body text
  save_b=cc->brk_cnt;
  cc->brk_cnt=0;
  save_c=cc->cont_cnt;
  cc->cont_cnt=0;
  cc->pc=pc_after;
  CCLex(cc);
  cc->in_loop++;		//M14: continue-validity context (CCContStmt)
  CCStmt(cc);
  cc->in_loop--;
  pc_body=cc->pc;		//after the body: resume point
  /* pc alone points PAST the already-lexed first token after the body
   * (CCLex leaves it in tk/tk_str with pc beyond it), so resuming with
   * a bare `pc=pc_body; CCLex` would silently swallow that token.
   * Snapshot the full token state and restore it instead. */
  save_tk=cc->tk;
  save_kw=cc->tk_kw;
  save_num=cc->tk_num;
  StrCpy(save_str,cc->tk_str);
  for (i=0;i!=cc->cont_cnt;i++)
    CCPatchJmp(cc,cc->cont[i]);	//continue -> step
  cc->cont_cnt=0;
  cc->pc=pc_step;
  CCLex(cc);			//first step token (or ')')
  if (!CCTkIsPunct(cc,')'))
    CCExpr(cc);			//emitted AFTER the body: correct patches
  cc->pc=pc_body;
  cc->tk=save_tk;
  cc->tk_kw=save_kw;
  cc->tk_num=save_num;
  StrCpy(cc->tk_str,save_str);
  CCByte(cc,0xE9);		//JMP top (rel32)
  CCWord(cc,top-(cc->ep-cc->code)-4);
  if (pz>=0)
    CCPatchJmp(cc,pz);		//condition false -> exit (past the step)
  for (i=0;i!=cc->brk_cnt;i++)
    CCPatchJmp(cc,cc->brk[i]);
  cc->brk_cnt=save_b;
  cc->cont_cnt=save_c;
}

U0 CCBrkStmt(CCCtrl *cc)
{
  CCLex(cc);		//consume 'break'
  if (cc->brk_cnt>=64)
    CCError(cc,"too many breaks");
  else
    cc->brk[cc->brk_cnt++]=CCJmp(cc);
  CCExpect(cc,';',"expected ';' after break");
}

U0 CCContStmt(CCCtrl *cc)
{
  CCLex(cc);		//consume 'continue'
  if (!cc->in_loop)
  {
    //M14: a continue in a switch body but in NO loop (or at block
    //level) would emit a jump that nothing ever patches.
    CCError(cc,"continue outside of a loop");
    CCExpect(cc,';',"expected ';' after continue");
    return;
  }
  if (cc->cont_cnt>=64)
    CCError(cc,"too many continues");
  else
    cc->cont[cc->cont_cnt++]=CCJmp(cc);
  CCExpect(cc,';',"expected ';' after continue");
}

U0 CCBlockStmt(CCCtrl *cc)
{
  CCLex(cc);		//consume '{'
  while ((!cc->errs)&&((cc->tk!=TK_EOF)&&!CCTkIsPunct(cc,'}')))
    CCStmt(cc);
  CCExpect(cc,'}',"expected '}'");
}

/* One declarator within an `int|char` (or `struct Tag`) declaration:
 * [*] name [n] [= init]. Stars are parsed per declarator so `int a,*b;`
 * types b correctly and `int *a,b;` leaves b a plain int, exactly as C
 * does. base==TY_STRUCT makes a starless declarator a struct object
 * (sized by the tag) and a starred one a struct pointer. */
U0 CCDeclOne(CCCtrl *cc,I64 base,I64 stag)
{
  CSym *s;
  I64 n;
  I64 ty=base;
  while (CCAccept(cc,'*'))
  {
    if (ty==TY_CHAR)
      ty=TY_CHARP;
    else
      ty=TY_PTR;
  }
  if (cc->tk!=TK_IDENT)
  {
    CCError(cc,"expected variable name");
    return;
  }
  s=CCSymNew(cc,cc->tk_str,SC_LOCAL);
  if (!s)
    return;
  CCLex(cc);
  s->ty=ty;
  if (base==TY_STRUCT)
    if ((ty==TY_STRUCT)||(ty==TY_PTR))
      s->sty=stag;
  s->dim=1;
  if (CCAccept(cc,'['))
  {
    if (cc->tk!=TK_NUM)
    {
      CCError(cc,"array size must be a number");
      return;
    }
    n=cc->tk_num;
    CCLex(cc);
    CCExpect(cc,']',"expected ']' after array size");
    if ((n<1)||(n>2048))
    {
      CCError(cc,"array size out of range");
      return;
    }
    s->dim=n;
    if (n>1)
    {
      //Frame grows DOWN from the base slot: element 0 sits at the
      //highest slot number so element i lands at base+8*i.
      if (base==TY_STRUCT)
        cc->lofs+=n*(cc->st[stag].size/8)-1;	//structs: size*n slots
      else
        cc->lofs+=n-1;
      s->offset=cc->lofs;
    }
    if (s->ty==TY_CHAR)
      s->ty=TY_CHARP;
    else if (s->ty!=TY_STRUCT)
      s->ty=TY_PTR;
  }
  else if (s->ty==TY_STRUCT)
  {
    //Bare struct object: reserve its slots, base at the highest.
    cc->lofs+=cc->st[stag].size/8-1;
    s->offset=cc->lofs;
  }
  if (CCTkIsPunct(cc,'='))
  {
    if ((s->dim>1)||(s->ty==TY_STRUCT))
    {
      CCError(cc,"array/struct initializers not supported");
      return;
    }
    CCVarAddr(cc,s);
    cc->is_addr=1;
    cc->aty=(s->ty==TY_CHAR);
    CCAssignTail(cc);		//consumes '='; ety typed from lhs
  }
}

/* <base> [*] name [n] [, ...] ;  — local declaration(s). M13: the base
 * comes from CCAcceptBase, so const/unsigned/long/void/typedef names
 * parse here exactly like at file scope. */
U0 CCDeclStmt(CCCtrl *cc)
{
  I64 ty,stag;
  ty=CCAcceptBase(cc,&stag);
  if (ty<0)
  {
    CCError(cc,"bad declaration");
    return;
  }
  if (ty==TY_VOID)
    ty=TY_CHAR;		//void scalar / void* in the byte model
  while ((!cc->errs)&&(cc->tk!=TK_EOF))
  {
    CCDeclOne(cc,ty,stag);
    if (cc->errs)
      return;
    if (!CCAccept(cc,','))
      break;
  }
  CCExpect(cc,';',"expected ';' after declaration");
}

U0 CCStmt(CCCtrl *cc)
{
  if (cc->errs)
    return;
  if (CCTkIsPunct(cc,'{'))
  {
    CCBlockStmt(cc);
    return;
  }
  if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_IF))
  {
    CCIfStmt(cc);
    return;
  }
  if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_SWITCH))
  {
    CCSwitchStmt(cc);
    return;
  }
  if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_DO))
  {
    CCDoStmt(cc);
    return;
  }
  if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_WHILE))
  {
    CCWhileStmt(cc);
    return;
  }
  if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_FOR))
  {
    CCForStmt(cc);
    return;
  }
  if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_BREAK))
  {
    CCBrkStmt(cc);
    return;
  }
  if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_CONTINUE))
  {
    CCContStmt(cc);
    return;
  }
  if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_RETURN))
  {
    CCLex(cc);
    if (!CCTkIsPunct(cc,';'))
      CCExpr(cc);
    CCExpect(cc,';',"expected ';'");
    CCFunRet(cc,cc->cur_entry);
    return;
  }
  if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_TYPEDEF))
  {
    CCParseTypedef(cc);		//block-scope typedef: legal in C
    return;
  }
  if (CCTokTypeStart(cc))
  {
    CCDeclStmt(cc);
    return;
  }
  if (CCAccept(cc,';'))
    return;		//empty statement
  CCExpr(cc);		//expression statement (also handles `lvalue = e`)
  CCExpect(cc,';',"expected ';'");
}

/* Parse a function parameter list; '(' already consumed, stops after
 * ')'. M13: bases fold through CCAcceptBase (const/unsigned/long/void/
 * typedef names); `f(void)` is the zero-parameter idiom C headers use;
 * anonymous parameters and a trailing `...` (printf-style prototypes)
 * are accepted — arity records the NAMED count, and every variadic
 * name on the header surface is a parser-dispatched builtin anyway. */
U0 CCParseParams(CCCtrl *cc)
{
  I64 base,stag,i;

  cc->nparam=0;
  if (!CCTkIsPunct(cc,')'))
  {
    while ((!cc->errs)&&(cc->tk!=TK_EOF))
    {
      stag=-1;
      base=-1;
      if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_VOID))
      {
        CCLex(cc);
        if (CCTkIsPunct(cc,')'))
          break;		//`f(void)` idiom: zero parameters
        base=TY_CHAR;		//`void *p` as first param: byte pointer
      }
      if ((cc->tk==TK_PUNCT)&&(cc->tk_num=='.'))
      {
        //`...`: consume the remaining two dots; named arity stands.
        CCLex(cc);
        if ((cc->tk!=TK_PUNCT)||(cc->tk_num!='.'))
        {
          CCError(cc,"bad '...' in parameter list");
          return;
        }
        CCLex(cc);
        if ((cc->tk!=TK_PUNCT)||(cc->tk_num!='.'))
        {
          CCError(cc,"bad '...' in parameter list");
          return;
        }
        CCLex(cc);
        break;
      }
      if (cc->nparam>=CC_MAX_PARAMS)
      {
        CCError(cc,"too many parameters");
        return;
      }
      if (base<0)
      {
        base=CCAcceptBase(cc,&stag);
        if (base<0)
        {
          CCError(cc,"unsupported parameter type");
          return;
        }
        if (base==TY_VOID)
          base=TY_CHAR;		//`void *p`: byte pointer (M13 model)
      }
      cc->psty[cc->nparam]=stag;
      cc->pty[cc->nparam]=base;
      while (CCAccept(cc,'*'))
      {
        //char* -> byte pointer; any further pointer (char**, int**) —
        //and struct pointers — are modeled as pointers to 8-byte slots.
        if (cc->pty[cc->nparam]==TY_CHAR)
          cc->pty[cc->nparam]=TY_CHARP;
        else
          cc->pty[cc->nparam]=TY_PTR;
      }
      if (cc->pty[cc->nparam]==TY_STRUCT)
      {
        CCError(cc,"struct parameters must be pointers: struct T *p");
        return;
      }
      i=cc->nparam;
      cc->pname[i*CC_TOK_MAX]=0;	//empty name = anonymous parameter
      if (cc->tk==TK_IDENT)
      {
        StrCpy(cc->pname+i*CC_TOK_MAX,cc->tk_str);
        CCLex(cc);
      }
      cc->nparam++;
      if (CCAccept(cc,'['))
      {
        //Array parameter (char *argv[]): adjusts to a slot pointer, so
        //a[i] indexes 8-byte elements — what an argv really is.
        if (cc->tk==TK_NUM)
          CCLex(cc);
        CCExpect(cc,']',"expected ']' after parameter array size");
        cc->pty[i]=TY_PTR;
      }
      if (CCAccept(cc,','))
      {
        //Next parameter.
      }
      else
        break;
    }
  }
  CCExpect(cc,')',"expected ')' after parameters");
}

/* Body of `int name(...) {...}` — or a prototype `int name(...);`
 * (M13: the path every #include'd C declaration takes). '(' already
 * consumed. entry is the code offset; the private-stack stub is
 * emitted only for 'main'. */
U0 CCParseFun(CCCtrl *cc,U8 *name,I64 entry)
{
  I64 sub_pos,i,entryflag,t;
  CSym *f,*s;

  CCParseParams(cc);
  if (cc->errs)
    return;
  if (!CCTkIsPunct(cc,'{'))
  {
    //Prototype declaration: no body. Register (or merge with a
    //forward-call stub) so calls and CCFind see the arity; a real
    //definition may still follow and wins. A prototype after a
    //definition is C-legal and inert. main() declared but never
    //defined does NOT satisfy the harness (entry_off stays unset).
    CCExpect(cc,';',"expected ';' after function declaration");
    if (cc->errs)
      return;
    f=CCSymFind(cc,name);
    if (!f)
    {
      f=CCSymNew(cc,name,SC_FUN);
      f->offset=-1;		//CCSymNew leaves 0; -1 = undefined yet
    }
    if (f->kind!=SC_FUN)
      CCError(cc,"name already a variable");
    else
    {
      if (f->offset<0)
        f->fargc=cc->nparam;	//definition overwrites on merge
    }
    return;
  }
  CCLex(cc);			//consume '{'
  for (i=0;i!=cc->nparam;i++)
  {
    if (cc->pname[i*CC_TOK_MAX])	//anonymous parameters bind no name
    {
      s=CCSymNew(cc,cc->pname+i*CC_TOK_MAX,SC_PARAM);
      if (s)
      {
        s->offset=i;	//CCSymNew leaves 0; index drives the RBP disp
        s->ty=cc->pty[i];
        s->sty=cc->psty[i];	//struct pointers keep their tag
        s->dim=1;
      }
    }
  }
  cc->cur_argc=cc->nparam;
  cc->lofs=0;		//locals restart per function
  f=CCSymFind(cc,name);
  if (!f)
    f=CCSymNew(cc,name,SC_FUN);
  if (f->kind!=SC_FUN)
  {
    CCError(cc,"name already a variable");
    return;
  }
  if (f->offset>=0)
  {
    CCError(cc,"function redefined");
    return;
  }
  f->offset=entry;
  f->fargc=cc->nparam;		//CCFind builds arg-copying thunks
  for (i=0;i!=f->npatch;i++)
    CCPatchCallPos(cc,f->patches[i],entry);
  f->npatch=0;
  entryflag=!StrCmp(name,"main");
  cc->cur_entry=entryflag;
  //main(int argc,char *argv[]...): the harness bridge fills those slots.
  cc->entry_inject=0;
  if (entryflag)
    if (cc->nparam>=1)
      cc->entry_inject=1;
  CCPrologue(cc,entryflag);
  //SUB RSP,imm32 for locals; patched once the body is parsed.
  CCByte(cc,0x48);CCByte(cc,0x81);CCByte(cc,0xEC);
  sub_pos=cc->ep-cc->code;
  CCWord(cc,0);
  while ((!cc->errs)&&(cc->tk!=TK_EOF)&&!CCTkIsPunct(cc,'}'))
    CCStmt(cc);
  CCExpect(cc,'}',"expected '}'");
  CCPatchImm32(cc,sub_pos,(cc->lofs*8+15)&~15);
  CCFunRet(cc,entryflag);	//fall-off end: implicit return
}

/* One declarator of an `int|char|struct Tag` top-level statement:
 * [*] name [[n]] [= const-init], or `name(...) {` when allow_fun.
 * base==TY_STRUCT: starless == struct object (sized by the tag),
 * starred == struct pointer. Returns TRUE if a function definition
 * was parsed (statement over). */
Bool CCGlobalOne(CCCtrl *cc,I64 base,I64 allow_fun,I64 stag)
{
  U8 name[CC_TOK_MAX];
  I64 ty=base,val,neg,n,i,entry;
  U8 *gp;
  CSym *s;

  while (CCAccept(cc,'*'))
  {
    if (ty==TY_CHAR)
      ty=TY_CHARP;
    else
      ty=TY_PTR;
  }
  if (cc->tk!=TK_IDENT)
  {
    CCError(cc,"expected name after type");
    return FALSE;
  }
  StrCpy(name,cc->tk_str);
  CCLex(cc);
  if (CCTkIsPunct(cc,'('))
  {
    if (!allow_fun)
    {
      CCError(cc,"functions get their own declaration");
      return FALSE;
    }
    CCLex(cc);
    entry=cc->ep-cc->code;
    CCParseFun(cc,name,entry);
    if (!StrCmp(name,"main"))
      cc->entry_off=entry;
    return TRUE;
  }
  s=CCSymFind(cc,name);
  if (s)
  {
    CCError(cc,"global redefined");
    return FALSE;
  }
  s=CCSymNew(cc,name,SC_GLOBAL);
  s->ty=ty;
  if (base==TY_STRUCT)
    if ((ty==TY_STRUCT)||(ty==TY_PTR))
      s->sty=stag;
  s->dim=1;
  if (CCAccept(cc,'['))
  {
    if (cc->tk!=TK_NUM)
    {
      CCError(cc,"global array size must be a number");
      return FALSE;
    }
    n=cc->tk_num;
    CCLex(cc);
    CCExpect(cc,']',"expected ']' after global array size");
    if ((n<1)||(n>2048))
    {
      CCError(cc,"array size out of range");
      return FALSE;
    }
    s->dim=n;
    if (n>1)
    {
      //CCSymNew already claimed the first 8 bytes.
      if (base==TY_STRUCT)
        cc->goff+=cc->st[stag].size*n-8;
      else
        cc->goff+=8*(n-1);
    }
    if (s->ty==TY_CHAR)
      s->ty=TY_CHARP;
    else if (s->ty!=TY_STRUCT)
      s->ty=TY_PTR;
  }
  else if (s->ty==TY_STRUCT)
    cc->goff+=cc->st[stag].size-8;	//claim the struct's bytes
  if (CCAccept(cc,'='))
  {
    if ((s->dim>1)||(s->ty==TY_STRUCT))
    {
      CCError(cc,"global array/struct initializers not supported");
      return FALSE;
    }
    if (cc->tk==TK_STR)
    {
      //`char *g = "text";` — store the pooled address as the value.
      if ((s->ty!=TY_PTR)&&(s->ty!=TY_CHARP))
      {
        CCError(cc,"only pointer globals take string initializers");
        return FALSE;
      }
      val=CCStrAlloc(cc,cc->tk_str);
      if (s->ty==TY_PTR)
        s->ty=TY_CHARP;		//string content implies byte pointers
      CCLex(cc);
    }
    else
    {
      neg=FALSE;
      if (CCAccept(cc,'-'))
        neg=TRUE;
      if (cc->tk!=TK_NUM)
      {
        CCError(cc,"global initializer must be a constant");
        return FALSE;
      }
      val=cc->tk_num;
      if (neg)
        val=-val;
      CCLex(cc);
    }
    s->init=val;
    s->has_init=TRUE;
    //HolyC needs postfix casts, so just store the 8 bytes directly.
    gp=cc->locals+s->offset;
    for (i=0;i!=8;i++)
      gp[i]=val>>(8*i);
  }
  return FALSE;
}

/* `struct Tag { ... };` with '{' as the current token. Members are
 * `int|char [*] name ;` — one 8-byte slot each, byte semantics kept for
 * char — or `struct T *name ;` (T's tag must already be registered:
 * the tag being DEFINED is registered before its '{', so self-referential
 * `struct Node { struct Node *next; }` links work). No member arrays,
 * by-value nested structs, or initializers. Returns the tag id, -1 error. */
I64 CCStructDef(CCCtrl *cc,U8 *tag)
{
  I64 t,mty,msty,mi;
  if (cc->nst>=CC_STRUCTS)
  {
    CCError(cc,"too many structs");
    return -1;
  }
  if (CCStagFind(cc,tag)>=0)
  {
    CCError(cc,"struct tag redefined");
    return -1;
  }
  t=cc->nst;
  StrCpy(cc->st[t].name,tag);
  cc->st[t].nmem=0;
  cc->nst++;			//register BEFORE the members: self-refs work
  CCLex(cc);			//consume '{'
  while ((!cc->errs)&&(cc->tk!=TK_EOF)&&!CCTkIsPunct(cc,'}'))
  {
    msty=-1;
    if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_STRUCT))
    {
      CCLex(cc);
      if (cc->tk!=TK_IDENT)
      {
        CCError(cc,"expected tag name in struct member");
        return -1;
      }
      msty=CCStagFind(cc,cc->tk_str);
      if (msty<0)
      {
        CCError(cc,"undefined struct tag in member");
        return -1;
      }
      CCLex(cc);
      if (!CCAccept(cc,'*'))
      {
        CCError(cc,"struct members must be pointers: struct T *m");
        return -1;
      }
      while (CCAccept(cc,'*'))
      {
        //`struct T **m` is still a slot pointer.
      }
      mty=TY_PTR;
    }
    else if ((cc->tk==TK_IDENT)&&((cc->tk_kw==KW_INT)||(cc->tk_kw==KW_CHAR)))
    {
      if (cc->tk_kw==KW_CHAR)
        mty=TY_CHAR;
      else
        mty=TY_INT;
      CCLex(cc);
      while (CCAccept(cc,'*'))
      {
        if (mty==TY_CHAR)
          mty=TY_CHARP;
        else
          mty=TY_PTR;
      }
    }
    else
    {
      CCError(cc,"struct members are int/char (optionally starred) or struct pointers");
      return -1;
    }
    if (cc->tk!=TK_IDENT)
    {
      CCError(cc,"expected member name");
      return -1;
    }
    mi=cc->st[t].nmem;
    if (mi>=CC_MAX_MEMBERS)
    {
      CCError(cc,"too many struct members");
      return -1;
    }
    StrCpy(cc->st[t].mname+mi*CC_TOK_MAX,cc->tk_str);
    CCLex(cc);
    if (CCTkIsPunct(cc,'['))
    {
      CCError(cc,"struct member arrays not supported");
      return -1;
    }
    CCExpect(cc,';',"expected ';' after struct member");
    cc->st[t].moff[mi]=8*mi;
    cc->st[t].mty[mi]=mty;
    cc->st[t].msty[mi]=msty;
    cc->st[t].nmem=mi+1;
  }
  CCExpect(cc,'}',"expected '}' after struct members");
  if (cc->errs)
    return -1;
  if (!cc->st[t].nmem)
  {
    CCError(cc,"empty struct");
    return -1;
  }
  cc->st[t].size=8*cc->st[t].nmem;
  return t;
}

/* Top-level: `struct` definition/declaration, function definition, or
 * `int|char decl [, decl]* ;` */
U0 CCParseTop(CCCtrl *cc)
{
  I64 ty=TY_INT,first=TRUE,stag=-1,t;
  U8 tag[CC_TOK_MAX];

  if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_TYPEDEF))
  {
    CCParseTypedef(cc);
    return;
  }
  if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_STRUCT))
  {
    CCLex(cc);			//consume 'struct'
    if (cc->tk!=TK_IDENT)
    {
      CCError(cc,"expected struct tag name");
      return;
    }
    StrCpy(tag,cc->tk_str);
    CCLex(cc);
    if (CCTkIsPunct(cc,'{'))
    {
      t=CCStructDef(cc,tag);
      if (t<0)
        return;
      stag=t;
      if (CCAccept(cc,';'))
        return;			//pure tag definition, no declarators
    }
    else
    {
      stag=CCStagFind(cc,tag);
      if (stag<0)
      {
        CCError(cc,"undefined struct tag");
        return;
      }
    }
    while ((!cc->errs)&&(cc->tk!=TK_EOF))
    {
      CCGlobalOne(cc,TY_STRUCT,FALSE,stag);	//no functions here
      if (cc->errs)
        return;
      if (!CCAccept(cc,','))
        break;
    }
    CCExpect(cc,';',"expected ';' after struct declaration");
    return;
  }
  //M13: any foldable type sequence opens a top-level statement; the
  //struct-with-body case was already handled above, so CCAcceptBase
  //here only ever folds keywords/typedef names (TY_STRUCT cannot).
  ty=CCAcceptBase(cc,&stag);
  if (ty<0)
  {
    CCError(cc,"expected a type at top level");
    return;
  }
  if (ty==TY_VOID)
    ty=TY_CHAR;		//`void *f();` etc: byte-pointer model
  while ((!cc->errs)&&(cc->tk!=TK_EOF))
  {
    //Only the first declarator of a statement may become a function:
    //`int a,b();` is rejected rather than silently guessed.
    if (CCGlobalOne(cc,ty,first,-1))
      return;
    if (cc->errs)
      return;
    if (!CCAccept(cc,','))
      break;
    first=FALSE;
  }
  CCExpect(cc,';',"expected ';' after global");
}

/* Parse a whole translation unit; returns main's offset or -1. */
I64 CCParse(CCCtrl *cc)
{
  I64 i;
  CSym *s;

  cc->entry_off=-1;
  while ((!cc->errs)&&(cc->tk!=TK_EOF))
    CCParseTop(cc);
  for (i=0;i!=cc->sym_cnt;i++)
  {
    s=&cc->syms[i];
    if ((s->kind==SC_FUN)&&(s->offset<0)&&(s->npatch))
      CCError(cc,"call to undefined function");
  }
  if (cc->errs)
    return -1;
  if (cc->entry_off<0)
  {
    CCError(cc,"no main function");
    return -1;
  }
  return cc->entry_off;
}
