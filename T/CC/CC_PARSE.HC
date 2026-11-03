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
extern U0 CCExprConditional(CCCtrl *cc);
extern I64 CCParseFunPtrDecl(CCCtrl *cc,U8 *name);
/* M20: the constant-eval chain + enum machinery need the SAME extern
 * declarations every other parser chain carries — this JIT misbinds
 * same-unit calls without them (CCConstUnary's call into CCConstPrim
 * silently executed nothing: no value, no token consumed, no error).
 * See the unit rule at the top of CC.HH. */
extern I64 CCConstFind(CCCtrl *cc,U8 *nm,I64 *pval);
extern U0 CCConstPrim(CCCtrl *cc,I64 *pv);
extern U0 CCConstUnary(CCCtrl *cc,I64 *pv);
extern U0 CCConstMul(CCCtrl *cc,I64 *pv);
extern U0 CCConstAdd(CCCtrl *cc,I64 *pv);
extern U0 CCConstShift(CCCtrl *cc,I64 *pv);
extern U0 CCConstRel(CCCtrl *cc,I64 *pv);
extern U0 CCConstEq(CCCtrl *cc,I64 *pv);
extern U0 CCConstBitAnd(CCCtrl *cc,I64 *pv);
extern U0 CCConstBitXor(CCCtrl *cc,I64 *pv);
extern U0 CCConstBitOr(CCCtrl *cc,I64 *pv);
extern U0 CCConstLogAnd(CCCtrl *cc,I64 *pv);
extern U0 CCConstLogOr(CCCtrl *cc,I64 *pv);
extern I64 CCConstExpr(CCCtrl *cc);
extern U0 CCEraseScope(CCCtrl *cc,I64 mark,I64 base);
extern U0 CCMakeStatic(CCCtrl *cc,CSym *s);
extern I64 CCEnumTagFind(CCCtrl *cc,U8 *nm);
extern U0 CCEnumDef(CCCtrl *cc,U8 *tag);
extern U0 CCSnap(CCCtrl *cc,U8 **pp,I64 *ptk,I64 *pkw,I64 *pnum,U8 *pstr);
extern I64 CCStructDef(CCCtrl *cc,U8 *tag,I64 is_union);
extern U0 CCRestore(CCCtrl *cc,U8 *p,I64 tk,I64 kw,I64 num,U8 *str);
extern U0 CCIndirectCallAt(CCCtrl *cc);
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
  cc->esz2=0;		//M17: any CCVal turns the value into a plain
}				//pointer — its row scales are spent (the
				//Postfix index arm reads esz2 BEFORE this
				//runs on the base; see CC.HH).

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
/* ---- M20: compile-time constant folding + enum constants -----------
 * The evaluator is a mini recursive-descent parser over the same
 * precedence ladder the code-emitting parser uses — but it PRODUCES
 * I64 VALUES instead of code, over literals and enum constants only.
 * It feeds: enum bodies (`B = PREV*2`), case labels (`case GREEN:`
 * and `case 1+1:`), global initializers, and array sizes. Division
 * by zero is a diagnostic — the HOST JIT would fault on it, and a
 * compiler crash on legal-ish input is never acceptable here.
 * Enumerator names live in their own flat table; a variable/function
 * of the same name wins everywhere they compete (C shares the
 * namespace), and CCConstFind refuses to answer for those names at
 * all. Enum TAGS are a separate tiny table: `enum Color` folds to
 * the int model, so a tag only exists to catch redefinition. */
/* ---- M20: compile-time constant folding + enum constants -----------
 * The evaluator is a mini recursive-descent parser over the same
 * precedence ladder the code-emitting parser uses — but it PRODUCES
 * I64 VALUES instead of code, over literals and enum constants only.
 * It feeds: enum bodies (`B = PREV*2`), case labels (`case GREEN:`
 * and `case 1+1:`), global initializers, and array sizes. Division
 * by zero is a diagnostic — the HOST JIT would fault on it, and a
 * compiler crash on a legal-ish input is never acceptable here.
 * Enumerator names live in their own flat table; a variable/function
 * of the same name wins everywhere they compete (C shares the
 * namespace), and CCConstFind refuses to answer for those names at
 * all. Enum TAGS are a separate tiny table: `enum Color` folds to
 * the int model, so a tag only exists to catch redefinition. */
I64 CCConstFind(CCCtrl *cc,U8 *nm,I64 *pval)
{
  I64 i;
  if (CCSymFind(cc,nm))
    return FALSE;		//a real variable/function owns the name
  for (i=0;i!=cc->ncnst;i++)
  {
    if (!StrCmp(cc->cname[i],nm))
    {
      *pval=cc->cvalue[i];
      return TRUE;
    }
  }
  return FALSE;
}

U0 CCConstPrim(CCCtrl *cc,I64 *pv)
{
  I64 v;
  if (cc->tk==TK_NUM)
  {
    *pv=cc->tk_num;
    CCLex(cc);
    return;
  }
  if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_NONE))
  {
    if (CCConstFind(cc,cc->tk_str,&v))
    {
      *pv=v;
      CCLex(cc);
      return;
    }
    CCError(cc,"not a compile-time constant");
    return;
  }
  if (CCTkIsPunct(cc,'('))
  {
    CCLex(cc);
    *pv=CCConstExpr(cc);	//defined below; extern at the top
    if (cc->errs)
      return;
    CCExpect(cc,')',"expected ')' in constant expression");
    return;
  }
  CCError(cc,"not a compile-time constant");
}

U0 CCConstUnary(CCCtrl *cc,I64 *pv)
{
  if (CCTkIsPunct(cc,'-'))
  {
    CCLex(cc);
    CCConstUnary(cc,pv);	//unary chains fold: `- -3` is 3
    if (cc->errs)
      return;
    *pv=-*pv;
    return;
  }
  if (CCTkIsPunct(cc,'+'))
  {
    CCLex(cc);
    CCConstUnary(cc,pv);
    return;
  }
  if ((cc->tk==TK_PUNCT)&&(cc->tk_num=='!'))
  {
    CCLex(cc);
    CCConstUnary(cc,pv);
    if (cc->errs)
      return;
    if (*pv)
      *pv=0;
    else
      *pv=1;
    return;
  }
  if ((cc->tk==TK_PUNCT)&&(cc->tk_num=='~'))
  {
    CCLex(cc);
    CCConstUnary(cc,pv);
    if (cc->errs)
      return;
    *pv=(0-*pv)-1;		//~v == -v-1 in two's complement;
    return;			//spelled so the fold never depends on
  }				//host ~ semantics
  CCConstPrim(cc,pv);
}

U0 CCConstMul(CCCtrl *cc,I64 *pv)
{
  I64 r,op;
  CCConstUnary(cc,pv);
  while ((!cc->errs)&&(cc->tk==TK_PUNCT)&&(((cc->tk_num=='*')||(cc->tk_num=='/'))||(cc->tk_num=='%')))
  {
    op=cc->tk_num;
    CCLex(cc);
    CCConstUnary(cc,&r);
    if (cc->errs)
      return;
    if (op=='*')
      *pv=*pv*r;
    else if (op=='/')
    {
      if (!r)
      {
        CCError(cc,"constant expression divides by zero");
        return;
      }
      *pv=*pv/r;
    }
    else
    {
      if (!r)
      {
        CCError(cc,"constant expression divides by zero");
        return;
      }
      *pv=*pv%r;
    }
  }
}

U0 CCConstAdd(CCCtrl *cc,I64 *pv)
{
  I64 r,op;
  CCConstMul(cc,pv);
  while ((!cc->errs)&&(cc->tk==TK_PUNCT)&&((cc->tk_num=='+')||(cc->tk_num=='-')))
  {
    op=cc->tk_num;
    CCLex(cc);
    CCConstMul(cc,&r);
    if (cc->errs)
      return;
    if (op=='+')
      *pv=*pv+r;
    else
      *pv=*pv-r;
  }
}

U0 CCConstShift(CCCtrl *cc,I64 *pv)
{
  I64 r,op;
  CCConstAdd(cc,pv);
  while ((!cc->errs)&&(cc->tk==TK_PUNCT)&&((cc->tk_num==OP_SHL)||(cc->tk_num==OP_SHR)))
  {
    op=cc->tk_num;
    CCLex(cc);
    CCConstAdd(cc,&r);
    if (cc->errs)
      return;
    if (op==OP_SHL)
      *pv=*pv<<r;
    else
      *pv=*pv>>r;		//arithmetic shift: matches the signed
  }				//I64 model everywhere else
}

U0 CCConstRel(CCCtrl *cc,I64 *pv)
{
  I64 r,op;
  CCConstShift(cc,pv);
  while ((!cc->errs)&&(cc->tk==TK_PUNCT)&&((((cc->tk_num=='<')||(cc->tk_num=='>'))||(cc->tk_num==OP_LE))||(cc->tk_num==OP_GE)))
  {
    op=cc->tk_num;
    CCLex(cc);
    CCConstShift(cc,&r);
    if (cc->errs)
      return;
    if (op=='<')
    {
      if (*pv<r)
        *pv=1;
      else
        *pv=0;
    }
    else if (op=='>')
    {
      if (*pv>r)
        *pv=1;
      else
        *pv=0;
    }
    else if (op==OP_LE)
    {
      if (*pv<=r)
        *pv=1;
      else
        *pv=0;
    }
    else
    {
      if (*pv>=r)
        *pv=1;
      else
        *pv=0;
    }
  }
}

U0 CCConstEq(CCCtrl *cc,I64 *pv)
{
  I64 r,op;
  CCConstRel(cc,pv);
  while ((!cc->errs)&&(cc->tk==TK_PUNCT)&&((cc->tk_num==OP_EQ)||(cc->tk_num==OP_NE)))
  {
    op=cc->tk_num;
    CCLex(cc);
    CCConstRel(cc,&r);
    if (cc->errs)
      return;
    if ((op==OP_EQ)&&(*pv==r))
      *pv=1;
    else if ((op==OP_NE)&&(*pv!=r))
      *pv=1;
    else
      *pv=0;
  }
}

U0 CCConstBitAnd(CCCtrl *cc,I64 *pv)
{
  I64 r;
  CCConstEq(cc,pv);
  while ((!cc->errs)&&(cc->tk==TK_PUNCT)&&(cc->tk_num=='&'))
  {
    CCLex(cc);
    CCConstEq(cc,&r);
    if (cc->errs)
      return;
    *pv=*pv&r;
  }
}

U0 CCConstBitXor(CCCtrl *cc,I64 *pv)
{
  I64 r;
  CCConstBitAnd(cc,pv);
  while ((!cc->errs)&&(cc->tk==TK_PUNCT)&&(cc->tk_num=='^'))
  {
    CCLex(cc);
    CCConstBitAnd(cc,&r);
    if (cc->errs)
      return;
    *pv=*pv^r;
  }
}

U0 CCConstBitOr(CCCtrl *cc,I64 *pv)
{
  I64 r;
  CCConstBitXor(cc,pv);
  while ((!cc->errs)&&(cc->tk==TK_PUNCT)&&(cc->tk_num=='|'))
  {
    CCLex(cc);
    CCConstBitXor(cc,&r);
    if (cc->errs)
      return;
    *pv=*pv|r;
  }
}

U0 CCConstLogAnd(CCCtrl *cc,I64 *pv)
{
  I64 r;
  CCConstBitOr(cc,pv);
  while ((!cc->errs)&&(cc->tk==TK_PUNCT)&&(cc->tk_num==OP_AND))
  {
    CCLex(cc);
    CCConstBitOr(cc,&r);
    if (cc->errs)
      return;
    if (*pv&&r)		//both sides are always evaluable here, so
      *pv=1;		//skipping a short-circuit fold is safe
    else
      *pv=0;
  }
}

U0 CCConstLogOr(CCCtrl *cc,I64 *pv)
{
  I64 r;
  CCConstLogAnd(cc,pv);
  while ((!cc->errs)&&(cc->tk==TK_PUNCT)&&(cc->tk_num==OP_OR))
  {
    CCLex(cc);
    CCConstLogAnd(cc,&r);
    if (cc->errs)
      return;
    if (*pv||r)
      *pv=1;
    else
      *pv=0;
  }
}

/* Entry: fold a full constant expression; returns the value and
 * leaves cc->errs set on any diagnostic. */
I64 CCConstExpr(CCCtrl *cc)
{
  I64 v;
  v=0;
  CCConstLogOr(cc,&v);
  return v;
}

I64 CCEnumTagFind(CCCtrl *cc,U8 *nm)
{
  I64 i;
  for (i=0;i!=cc->netag;i++)
  {
    if (!StrCmp(cc->etag[i],nm))
      return i;
  }
  return -1;
}

/* `enum [Tag] { NAME [= const-expr], ... }` with '{' current. The tag
 * is optional in definition position. Auto-increments from the previous
 * value (starting at 0) exactly like C. Trailing commas are C-legal.
 * Enumerator names must be unique in the translation unit AND must not
 * collide with a variable/function name (C shares the namespace: we
 * enforce it in both directions — see CCConstFind for the other). */
U0 CCEnumDef(CCCtrl *cc,U8 *tag)
{
  I64 next,val;
  U8 nm[CC_TOK_MAX];
  if (tag[0])
  {
    if (CCEnumTagFind(cc,tag)>=0)
    {
      CCError(cc,"enum tag redefined");
      return;
    }
    if (cc->netag>=CC_MAX_ENUMTAGS)
    {
      CCError(cc,"too many enum tags");
      return;
    }
    StrCpy(cc->etag[cc->netag],tag);
    cc->netag++;
  }
  CCLex(cc);		//consume '{'
  next=0;
  while ((!cc->errs)&&(cc->tk!=TK_EOF)&&!CCTkIsPunct(cc,'}'))
  {
    if (cc->tk!=TK_IDENT)
    {
      CCError(cc,"expected enumerator name");
      return;
    }
    StrCpy(nm,cc->tk_str);
    CCLex(cc);
    val=next;
    if (CCAccept(cc,'='))
    {
      val=CCConstExpr(cc);
      if (cc->errs)
        return;
    }
    if (CCSymFind(cc,nm))
    {
      CCError(cc,"enumerator conflicts with a variable or function name");
      return;
    }
    if (CCConstFind(cc,nm,&val))
    {
      CCError(cc,"enumerator redefined");
      return;
    }
    if (cc->ncnst>=CC_MAX_CONSTS)
    {
      CCError(cc,"too many enum constants");
      return;
    }
    StrCpy(cc->cname[cc->ncnst],nm);
    cc->cvalue[cc->ncnst]=val;
    cc->ncnst++;
    next=val+1;
    if (!CCAccept(cc,','))
      break;
  }
  CCExpect(cc,'}',"expected '}' after enum members");
}

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
  if ((kw==KW_UNION)||((kw==KW_EXTERN)||((kw==KW_STATIC)||(kw==KW_REGISTER))))
    return TRUE;
  if ((kw==KW_AUTO)||(kw==KW_VOLATILE))
    return TRUE;
  if (kw==KW_INLINE)
    return TRUE;
  if (kw==KW_ENUM)
    return TRUE;
  if ((kw==KW_VOID)||(kw==KW_UNSIGNED))
    return TRUE;
  if ((kw==KW_SIGNED)||(kw==KW_LONG))
    return TRUE;
  if (kw==KW_SHORT)
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
  I64 ty=-1,ti,had_int,had_char,tent;
  *pstag=-1;
  had_int=0;
  had_char=0;
  tent=0;
  cc->spec_static=0;		//M21: the static PROMISE is per-spec;
  //No `continue` in HolyC (the keyword does not exist in this dialect):
  //the fold is one if/else chain per token, and each arm advances or
  //breaks. See PLAN.
  //M19 fold rules, C-truth in the single-width byte/slot model:
  //  qualifiers (const signed extern static register auto volatile
  //  inline) are no-ops that PROVISIONALLY promise int — exactly C's
  //  default-int when nothing else shows up — but a real base (char/
  //  void/struct/typedef) overwrites the promise;
  //  had_int (int/long/short ONLY) blocks char both ways (`unsigned
  //  char` and `char unsigned` fold to the byte model because char IS
  //  the unsigned byte model here); had_char blocks int/long/short.
  while (cc->tk==TK_IDENT)
  {
    if ((cc->tk_kw==KW_CONST)||((cc->tk_kw==KW_SIGNED)||((cc->tk_kw==KW_EXTERN)||((cc->tk_kw==KW_STATIC)||((cc->tk_kw==KW_REGISTER)||((cc->tk_kw==KW_AUTO)||((cc->tk_kw==KW_VOLATILE)||(cc->tk_kw==KW_INLINE))))))))
    {
      if (cc->tk_kw==KW_STATIC)
        cc->spec_static=1;	//M21: CCDeclOne reads this after the
      if (ty<0)			//base folds — static block objects get
      {				//arena slots (see CCDeclOne)
        ty=TY_INT;		//default-int promise; bases overwrite
        tent=1;			//...but it is a PROMISE only: void
        CCLex(cc);		//and struct take it over below
      }
      else
        CCLex(cc);
    }
    else if ((cc->tk_kw==KW_UNSIGNED)||((cc->tk_kw==KW_LONG)||((cc->tk_kw==KW_INT)||(cc->tk_kw==KW_SHORT))))
    {
      if (cc->tk_kw==KW_UNSIGNED)
      {
        if (ty<0)
          ty=TY_INT;		//bare `unsigned x` = unsigned int
      }
      else if (had_char)
      {
        CCError(cc,"type keyword conflict (char with int/long)");
        return -1;
      }
      else
      {
        had_int=1;
        ty=TY_INT;
      }
      CCLex(cc);
    }
    else if (cc->tk_kw==KW_CHAR)
    {
      if (had_int||(ty==TY_VOID))
      {
        CCError(cc,"type keyword conflict");
        return -1;
      }
      if (had_char)
      {
        CCError(cc,"duplicate char");
        return -1;
      }
      had_char=1;
      ty=TY_CHAR;		//unsigned/signed char both land here:
      CCLex(cc);		//the model's char already zero-extends
    }
    else if (cc->tk_kw==KW_VOID)
    {
      if ((ty>=0)&&(tent==0))
      {
        CCError(cc,"void cannot combine with another type");
        return -1;
      }
      ty=TY_VOID;		//M19: a bare-qualifier promise yields
      tent=0;			//to void — `const void *v` is the
      CCLex(cc);		//headers' idiom and must fold
    }
    else if (cc->tk_kw==KW_ENUM)
    {
      if ((ty>=0)&&(tent==0))
      {
        CCError(cc,"enum cannot combine with another type");
        return -1;
      }
      tent=0;
      CCLex(cc);		//consume 'enum'
      if (cc->tk==TK_IDENT)
      {
        CCLex(cc);		//its tag, when present — a separate
      }				//namespace from structs', incomplete
      ty=TY_INT;		//tags C-legal, and we fold to the int
    }				//model anyway, so a tag need not exist
    else if ((cc->tk_kw==KW_STRUCT)||(cc->tk_kw==KW_UNION))
    {
      if ((ty>=0)&&(tent==0))
      {
        CCError(cc,"struct cannot combine with another type");
        return -1;
      }
      tent=0;
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
  I64 ty,stag,fpok;
  I64 stk,skw,snum;
  U8 *spc;
  U8 sstr[CC_TOK_MAX];
  U8 fpname[CC_TOK_MAX];
  CCLex(cc);			//consume 'typedef'
  ty=CCAcceptBase(cc,&stag);
  if (ty<0)
  {
    CCError(cc,"typedef needs a supported base type");
    return;
  }
  if (ty==TY_VOID)
    ty=TY_CHAR;		//typedef void *vp: vp is a byte pointer
  fpok=0;
  if (CCTkIsPunct(cc,'('))		//M18: `typedef int (*cmp_t)(int,
  {
    CCSnap(cc,&spc,&stk,&skw,&snum,sstr);	//int);` — the name sits
    fpok=CCParseFunPtrDecl(cc,fpname);		//INSIDE the parens; the
    if (!fpok)
      CCRestore(cc,spc,stk,skw,snum,sstr);	//binding is a plain
  }
  if (fpok)
  {
    if (cc->tn>=CC_MAX_TYPEDEF)
    {
      CCError(cc,"too many typedefs");
      return;
    }
    StrCpy(cc->tname[cc->tn],fpname);
    cc->tty[cc->tn]=TY_PTR;		//fp values are slot pointers;
    cc->tn++;				//the arity they promised is the
    CCExpect(cc,';',"expected ';' after typedef");	//call site's business
    return;
  }
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

/* M15: element size in bytes under C truth: char is 1, a struct is
 * its tag's byte total, every other scalar/pointer is the 8-byte
 * quad. sizeof reports C's answer, not the frame footprint (char
 * arrays occupy 8 bytes per byte in the frame — a separate matter). */
I64 CCSizeOfEty(CCCtrl *cc,I64 ty,I64 sty)
{
  if (ty==TY_CHAR)
    return 1;
  if ((ty==TY_STRUCT)&&(sty>=0))
    return cc->st[sty].size;
  return 8;
}

/* Silent speculative type-specifier scan, shared by sizeof(type) and
 * casts (CCExprUnary). Like CCAcceptBase but returns -1 IN SILENCE
 * when the token run is not a clean type; the caller rewinds with the
 * standard full token-state snapshot. */
I64 CCTrySpec(CCCtrl *cc,I64 *pstag)
{
  I64 ty=-1,ti,had_int,had_char,tent;
  *pstag=-1;
  had_int=0;
  had_char=0;
  tent=0;
  //M19: exact silent mirror of CCAcceptBase's fold rules (see there).
  while (cc->tk==TK_IDENT)
  {
    if ((cc->tk_kw==KW_CONST)||((cc->tk_kw==KW_SIGNED)||((cc->tk_kw==KW_EXTERN)||((cc->tk_kw==KW_STATIC)||((cc->tk_kw==KW_REGISTER)||((cc->tk_kw==KW_AUTO)||((cc->tk_kw==KW_VOLATILE)||(cc->tk_kw==KW_INLINE))))))))
    {
      if (ty<0)
      {
        ty=TY_INT;
        tent=1;
        CCLex(cc);
      }
      else
        CCLex(cc);
    }
    else if ((cc->tk_kw==KW_UNSIGNED)||((cc->tk_kw==KW_LONG)||((cc->tk_kw==KW_INT)||(cc->tk_kw==KW_SHORT))))
    {
      if (cc->tk_kw==KW_UNSIGNED)
      {
        if (ty<0)
          ty=TY_INT;
      }
      else if (had_char)
        return -1;
      else
      {
        had_int=1;
        ty=TY_INT;
      }
      CCLex(cc);
    }
    else if (cc->tk_kw==KW_CHAR)
    {
      if (had_int||(ty==TY_VOID)||(had_char))
        return -1;
      had_char=1;
      ty=TY_CHAR;
      CCLex(cc);
    }
    else if (cc->tk_kw==KW_VOID)
    {
      if ((ty>=0)&&(tent==0))
        return -1;
      ty=TY_VOID;
      tent=0;
      CCLex(cc);
    }
    else if (cc->tk_kw==KW_ENUM)
    {
      if ((ty>=0)&&(tent==0))
        return -1;
      tent=0;
      CCLex(cc);
      if (cc->tk==TK_IDENT)
        CCLex(cc);
      ty=TY_INT;
    }
    else if ((cc->tk_kw==KW_STRUCT)||(cc->tk_kw==KW_UNION))
    {
      if ((ty>=0)&&(tent==0))
        return -1;
      tent=0;
      CCLex(cc);
      if (cc->tk!=TK_IDENT)
        return -1;
      ti=CCStagFind(cc,cc->tk_str);
      if (ti<0)
        return -1;
      *pstag=ti;
      ty=TY_STRUCT;
      CCLex(cc);
      break;
    }
    else
    {
      ti=CCTypedefFind(cc,cc->tk_str);
      if (ti<0)
        break;
      ty=cc->tty[ti];
      CCLex(cc);
      break;
    }
  }
  return ty;
}

/* Rewind support for the speculative scans: the FULL lexer state
 * (PLAN: pc alone swallows the first token past the rewind). */
U0 CCSnap(CCCtrl *cc,U8 **pp,I64 *ptk,I64 *pkw,I64 *pnum,U8 *pstr)
{
  *pp=cc->pc;
  *ptk=cc->tk;
  *pkw=cc->tk_kw;
  *pnum=cc->tk_num;
  StrCpy(pstr,cc->tk_str);
}

U0 CCRestore(CCCtrl *cc,U8 *p,I64 tk,I64 kw,I64 num,U8 *str)
{
  cc->pc=p;
  cc->tk=tk;
  cc->tk_kw=kw;
  cc->tk_num=num;
  StrCpy(cc->tk_str,str);
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
/* M18: match `(* name ) ( param-list )` — the function-pointer
 * declarator. The caller has seen '(' and snapshotted the lexer; on
 * FALSE nothing is consumed (caller restores). On TRUE the name is
 * captured (possibly empty: anonymous fp params), tokens sit past the
 * parameter list's ')', and the caller registers a TY_PTR slot. Arity
 * is deliberately DROPPED: an indirect call takes its arg count from
 * the call site (see CCIndirectCallAt), and C only requires a match —
 * mismatched arity through an fp is as broken here as it is machine-
 * dependent in C. */
I64 CCParseFunPtrDecl(CCCtrl *cc,U8 *name)
{
  I64 depth;
  name[0]=0;
  CCLex(cc);			//consume '('
  if (!CCAccept(cc,'*'))
    return FALSE;		//`(x)` etc: caller rewinds the one token
  if (cc->tk==TK_IDENT)
  {
    StrCpy(name,cc->tk_str);
    CCLex(cc);
  }
  if (!CCTkIsPunct(cc,')'))
  {
    CCError(cc,"expected ')' in function-pointer declarator");
    return FALSE;
  }
  CCLex(cc);
  if (CCTkIsPunct(cc,'['))
  {
    CCError(cc,"function-pointer arrays not supported: keep one (*f)");
    return FALSE;
  }
  if (!CCTkIsPunct(cc,'('))
  {
    CCError(cc,"expected parameter list in function-pointer declarator");
    return FALSE;
  }
  CCLex(cc);
  depth=1;			//swallow params wholesale: fp params may
  while ((!cc->errs)&&(cc->tk!=TK_EOF)&&depth)
  {
    if (CCTkIsPunct(cc,'('))
      depth++;
    else if (CCTkIsPunct(cc,')'))
      depth--;
    if (depth)
      CCLex(cc);		//nested declarators inside parse their
  }				//own parens — the counter keeps the
  CCExpect(cc,')',"expected ')' after function-pointer parameter list");	//swallow honest
  if (cc->errs)
    return FALSE;
  return TRUE;
}

/* M18: `expr(args)` when expr is a chain-produced pointer VALUE in
 * hand; '(' already consumed. Normalize RAX to the TARGET ADDRESS:
 *   is_addr==1 with ety TY_PTR/TY_CHARP — RAX is the ADDRESS OF the
 *     pointer slot; the target is its value -> CCVal loads it (the
 *     member case: `n.fn(2)`).
 *   is_addr==1 with ety TY_INT — the object IS a function whose
 *     address the chain already produced (`(*fp)(3)`: the deref marked
 *     a pending load, but a function has no further load — C adjusts
 *     the lvalue to exactly this address) -> use RAX as-is.
 *   is_addr 0/2 — RAX already holds the value (or a decayed base) ✓.
 * Spill the target to a hidden local (lofs grows; the frame imm32 is
 * patched at the function's end — switch's proven trick), evaluate
 * args (RAX free to clobber), reload, CALL RAX, pop args. The callee
 * is CC-compiled C: plain `ret` + caller-side pops, exactly like
 * C-to-C calls; HolyC-JIT functions have a different epilogue (see
 * PLAN) so calling one through a C fp is not supported. */
U0 CCIndirectCallAt(CCCtrl *cc)
{
  I64 n,disp;
  if ((cc->is_addr==1)&&((cc->ety==TY_PTR)||(cc->ety==TY_CHARP)))
    CCVal(cc);
  cc->lofs++;
  disp=-8*cc->lofs;
  CCByte(cc,0x48);CCByte(cc,0x89);CCByte(cc,0x85);	//MOV [RBP+d32],RAX
  CCWord(cc,disp);
  n=0;
  if (!CCTkIsPunct(cc,')'))
  {
    while ((!cc->errs)&&(cc->tk!=TK_EOF))
    {
      CCExprNC(cc);		//commas here are ARGUMENT separators
      CCByte(cc,0x50);		//PUSH RAX (arg)
      n++;
      if (!CCAccept(cc,','))
        break;
    }
  }
  CCExpect(cc,')',"expected ')' after indirect call args");
  if (cc->errs)
    return;
  CCByte(cc,0x48);CCByte(cc,0x8B);CCByte(cc,0x85);	//MOV RAX,[RBP+d32]
  CCWord(cc,disp);
  CCByte(cc,0xFF);CCByte(cc,0xD0);			//CALL RAX
  CCPopArgs(cc,n);
  cc->is_addr=0;
  cc->aty=0;
  cc->ety=TY_INT;
  cc->sty=-1;
  cc->esz2=0;
}

U0 CCPostfix(CCCtrl *cc)
{
  I64 ekind,off,mi,bsty,hijack,bty,sty0;
  while ((!cc->errs)&&(cc->tk==TK_PUNCT)&&
      ((((cc->tk_num=='[')||(cc->tk_num=='.'))||(cc->tk_num==OP_ARROW))||(cc->tk_num=='(')))
  {
    if (cc->tk_num=='(')
    {
      //M18: call through what the chain produced: `n.fn(2)`,
      //`(*fp)(3)`, `(bp)(1)`. Ident-head calls are handled upstream
      //by Primary's peek (which parses args itself), so only chain
      //results and groups reach this arm.
      CCLex(cc);		//consume '('
      CCIndirectCallAt(cc);
    }
    else if (cc->tk_num=='[')
    {
      bsty=-1;
      hijack=0;
      bty=cc->ety;		//the decayed base type: a row keeps it
      sty0=cc->sty;
      if (cc->esz2)
      {
        //M17: first index of a 2-D decay scales a WHOLE ROW
        //(cc->esz); must be read before the CCVal below, which is
        //exactly what consumes esz2 for every other consumer.
        ekind=cc->esz;
        hijack=1;
      }
      else if (cc->ety==TY_STRUCT)
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
      if (hijack)
      {
        cc->esz=cc->esz2;
        cc->esz2=0;
        cc->is_addr=2;
        cc->ety=bty;
        cc->aty=0;
        if (bsty>=0)
          cc->sty=bsty;
        else
          cc->sty=sty0;
      }
      else
      {
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
  I64 inc,n,delta,scale,byte_aty,cv,cn,cnext,ci;
  U8 catbuf[CC_TOK_MAX];
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
    //M20: C concatenates ADJACENT literals at parse time. Collect in a
    //scratch buffer and allocate the pool entry ONCE — appending into
    //the first literal's entry would scribble on whatever the pool
    //cursor placed next to it (and dedupe could hand back an unrelated
    //address for the second literal anyway).
    StrCpy(catbuf,cc->tk_str);
    cn=StrLen(catbuf);
    CCLex(cc);
    while (cc->tk==TK_STR)
    {
      cnext=StrLen(cc->tk_str);
      if (cn+cnext+1>CC_TOK_MAX)
      {
        CCError(cc,"concatenated string literal too long");
        return;
      }
      //HolyC has no StrCat in this build: copy bytes by hand into the
      //scratch buffer (the length check above bounds it; the NUL rides
      //along on purpose).
      for (ci=0;ci!=cnext+1;ci++)
        catbuf[cn+ci]=cc->tk_str[ci];
      cn=cn+cnext;
      CCLex(cc);
    }
    sp=CCStrAlloc(cc,catbuf);
    CCByte(cc,0x48);CCByte(cc,0xB8);	//MOVABS RAX,string
    CCQuadAbs(cc,CC_B_LOCALS,sp);
    cc->is_addr=0;
    cc->aty=0;
    cc->ety=TY_CHARP;
    cc->sty=-1;
    CCPostfix(cc);		//M19: `"ab"[1]` and friends — the literal
    return;			//is a pointer VALUE; Postfix indexes it
  }				//with byte scale, no new machinery.
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
      {
        //M18: `fp(3)` on a pointer-shaped variable (global/local/
        //param, or a 2-D... no: arrays are TY_CHARP too — calling an
        //array base is as much UB here as in C). Args are ALREADY
        //pushed: load the target LAST into RAX (nothing runs after
        //to clobber it) and CALL RAX.
        if ((s->ty==TY_PTR)||(s->ty==TY_CHARP))
        {
          if (s->dim>1)
          {
            CCError(cc,"cannot call through an array of function pointers");
          }
          else
          {
            CCVarAddr(cc,s);
            cc->is_addr=1;			//describe the slot we
            cc->ety=s->ty;			//just located — the
            cc->aty=0;				//STATE is what CCVal
            cc->sty=-1;				//reads, and the last
            cc->esz2=0;				//arg expression left
            CCVal(cc);				//its own there (calling
            CCByte(cc,0xFF);CCByte(cc,0xD0);	//CALL RAX	&slot was the
          }					//first bug here)
        }
        else
          CCError(cc,"not a function");
      }
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
      //M20: enum constant — a compile-time VALUE, not storage. No
      //emit gate needed: CCMovImmRax is gated like every emitter.
      cv=0;
      if ((cc->tk_kw==KW_NONE)&&(CCConstFind(cc,cc->tk_str,&cv)))
      {
        CCMovImmRax(cc,cv);
        cc->is_addr=0;
        cc->aty=0;
        cc->ety=TY_INT;
        cc->sty=-1;
        cc->esz=0;
        cc->esz2=0;
        CCLex(cc);
        CCPostfix(cc);		//RED[0] is real C — subscripts commute
        return;
      }
      CCError(cc,"undefined variable");
      return;
    }
    CCVarAddr(cc,s);		//RAX = address of storage (M18: or the
    if (s->kind==SC_FUN)	//function's OWN address for SC_FUN —
    {
      cc->is_addr=2;		//it is a value already, never loads,
      cc->ety=TY_PTR;		//never assigns (`f=x` stays a C error)
      cc->aty=0;
      cc->sty=-1;
    }
    else if ((s->dim>1)||(s->ty==TY_STRUCT))
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
    if (s->dim2)		//M17: two-dimensional decay carries its
    {
      if (s->ty==TY_CHARP)	//row scales (see the CC.HH esz/esz2 note):
      {
        cc->esz2=1;		//esz2 = bytes per inner element, esz =
      }
      else if ((s->ty==TY_STRUCT)||((s->ty==TY_PTR)&&(s->sty>=0)))
      {
        //Struct rows — from a struct array OR a `struct T g[][2]`
        //parameter (which decays to a struct POINTER and must carry
        //the tag to reach this branch, or rows stride by 8.
        cc->esz2=cc->st[s->sty].size;
      }
      else
      {
        cc->esz2=8;		//int/pointer elements: slots.
      }
      cc->esz=cc->esz2*s->dim2;	//row base, so the load-on-value path
    }
    else
    {
      cc->esz=0;		//is right; the scales make indexing match
      cc->esz2=0;		//the caller's row layout either way.
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
    //Conditional (not LogOr): C permits `(c?a:b)` and that is where a
    //parenthesized ternary appears inside bigger expressions.
    CCExprConditional(cc);
    while ((!cc->errs)&&(CCTkIsPunct(cc,',')))
    {
      CCLex(cc);
      CCVal(cc);		//discard the left operand's value
      CCExprConditional(cc);
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
  if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_SIZEOF))
  {
    //M15 sizeof: compile-time constant, C-truth sizes. Three routes:
    //  sizeof(type)      speculative clean token run -> sizes table
    //  sizeof(identifier) exact symbol answer (arrays! structs!)
    //  sizeof(expr)      silent noemit parse, size of the RESULT type
    //                    (arrays here decay to pointer size — the bare
    //                    identifier path is what reports 128 for
    //                    `char b[128]`; sizeof needs parentheses).
    I64 sz,ty,stag,ty2,n,stars,ti;
    I64 stk,skw,snum;
    I64 stk2,skw2,snum2;
    U8 *spc;
    U8 *spc2;
    U8 sstr[CC_TOK_MAX];
    U8 sstr2[CC_TOK_MAX];
    CSym *sym;
    CCLex(cc);		//consume 'sizeof'
    if (!CCTkIsPunct(cc,'('))
    {
      CCError(cc,"sizeof needs parentheses");
      return;
    }
    CCLex(cc);		//consume '('
    CCSnap(cc,&spc,&stk,&skw,&snum,sstr);
    ty=CCTrySpec(cc,&stag);
    if (ty>=0)
    {
      stars=0;
      ty2=ty;
      while (CCAccept(cc,'*'))
      {
        stars=1;
        if (ty2==TY_CHAR)
          ty2=TY_CHARP;
        else
          ty2=TY_PTR;
      }
      n=1;
      if (CCTkIsPunct(cc,'['))
      {
        CCLex(cc);
        if (cc->tk!=TK_NUM)
        {
          CCError(cc,"sizeof array dimension must be a number");
          return;
        }
        n=cc->tk_num;
        CCLex(cc);
        CCExpect(cc,']',"expected ']' in sizeof type");
      }
      if (CCTkIsPunct(cc,')'))
      {
        CCLex(cc);
        sz=CCSizeOfEty(cc,ty,stag);
        if (stars)
          sz=8;		//a pointer element, whatever it pointed at
        sz=sz*n;		//sizeof(char[64])==64, sizeof(int[4])==32,
        		//sizeof(char*[4])==32
        CCMovImmRax(cc,sz);
        cc->is_addr=0;
        cc->aty=0;
        cc->ety=TY_INT;
        cc->sty=-1;
        return;
      }
      //Not a clean `sizeof(type)`: rewind and read it as an expression.
      CCRestore(cc,spc,stk,skw,snum,sstr);
    }
    //Exact answer for a bare identifier: array dims and struct sizes
    //live on the symbol, where no expression parse can recover them.
    if (cc->tk==TK_IDENT)
    {
      //Snapshot (token = the identifier) so a non-matching shape can
      //rewind and fall through to the silent expression parse.
      CCSnap(cc,&spc2,&stk2,&skw2,&snum2,sstr2);
      CCLex(cc);
      if (CCTkIsPunct(cc,')'))
      {
        sym=CCSymFind(cc,sstr2);
        if (sym)
        {
          CCLex(cc);
          if (sym->dim>1)
          {
            //Arrays are stored POINTER-TYPED on the symbol (s->ty
            //decayed to CHARP/PTR at declaration; dim carries the
            //count), so element size must come from C truth here:
            //a char array is 1 byte per element (the frame spends 8
            //per byte — a separate, documented matter), a struct
            //array is size*n, anything else 8. This is the ONLY
            //route that sees dims: sizeof(b)=128 for char b[128];
            //sizeof(msg) stays 8 because msg really is a pointer.
            if (sym->ty==TY_CHARP)
              sz=1;
            else
              sz=CCSizeOfEty(cc,sym->ty,sym->sty);
            sz=sym->dim*sz;
            if (sym->dim2)
              sz=sz*sym->dim2;
          }
          else
            sz=CCSizeOfEty(cc,sym->ty,sym->sty);	//scalars + pointers
          CCMovImmRax(cc,sz);
          cc->is_addr=0;
          cc->aty=0;
          cc->ety=TY_INT;
          cc->sty=-1;
          return;
        }
        ti=CCTypedefFind(cc,sstr2);	//sizeof(size_t): C reads it as the TYPE
        if (ti>=0)
        {
          CCLex(cc);
          CCMovImmRax(cc,CCSizeOfEty(cc,cc->tty[ti],-1));
          cc->is_addr=0;
          cc->aty=0;
          cc->ety=TY_INT;
          cc->sty=-1;
          return;
        }
      }
      CCRestore(cc,spc2,stk2,skw2,snum2,sstr2);
    }
    //Silent type-analysis parse: emits nothing (see cc->noemit).
    cc->noemit=1;
    CCExprNC(cc);
    sz=CCSizeOfEty(cc,cc->ety,cc->sty);
    cc->noemit=0;
    CCExpect(cc,')',"expected ')' after sizeof expression");
    CCMovImmRax(cc,sz);
    cc->is_addr=0;
    cc->aty=0;
    cc->ety=TY_INT;
    cc->sty=-1;
    return;
  }
  if (CCTkIsPunct(cc,'('))
  {
    //M15 cast: `(type) operand`. Speculative silent scan decides
    //cast-vs-grouping: a clean spec (+stars) run ending in ')' is a
    //cast; anything else rewinds and Primary handles the paren group.
    //Names must not collide with the sizeof arm above: HolyC local
    //scope is function-flat ("Duplicate member" otherwise).
    I64 cty,cstag,cty2,cstars;
    I64 ctk,ckw,cnum;
    U8 *cpc;
    U8 cstr[CC_TOK_MAX];
    CCSnap(cc,&cpc,&ctk,&ckw,&cnum,cstr);
    CCLex(cc);		//tentative: past '('
    cty=CCTrySpec(cc,&cstag);
    cstars=0;
    if (cty>=0)
    {
      cty2=cty;
      while (CCAccept(cc,'*'))
      {
        cstars=cstars+1;	//counted: `(struct T**)` must NOT keep
        if (cty2==TY_CHAR)	//the tag, only single struct* does
          cty2=TY_CHARP;
        else
          cty2=TY_PTR;
      }
      if (!CCTkIsPunct(cc,')'))
      {
        cty=-1;		//not a cast after all
        cstars=0;
      }
    }
    if (cty<0)
    {
      CCRestore(cc,cpc,ctk,ckw,cnum,cstr);
      //CCExprPrimary already applies postfix chains on every arm,
      //so the rewind must NOT add another CCPostfix (double-parsing
      //the same `[0][1]` tail).
      CCExprPrimary(cc);
      return;
    }
    CCLex(cc);		//consume ')' of the cast
    CCExprUnary(cc);
    CCVal(cc);
    cc->is_addr=0;
    cc->aty=0;
    cc->sty=-1;
    if (cstars)
    {
      cc->ety=cty2;		//char* stays a byte pointer; deeper is a slot pointer
      if ((cty==TY_STRUCT)&&(cstars==1))
        cc->sty=cstag;		//(struct T *)p keeps the tag: `->` must work
      return;
    }
    if (cty==TY_CHAR)
    {
      CCByte(cc,0x0F);CCByte(cc,0xB6);CCByte(cc,0xC0);	//MOVZX RAX,AL
      cc->ety=TY_CHAR;		//C truncates: (char)321 == 65
      return;
    }
    if (cty==TY_STRUCT)
    {
      cc->ety=TY_INT;		//a struct-typed value in this model is its base address
      return;
    }
    if (cty==TY_VOID)
    {
      cc->ety=TY_INT;		//`(void)x` discards; the value is inert
      return;
    }
    cc->ety=TY_INT;
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
  I64 pz,fired;
  fired=0;
  CCExprBitOr(cc);
  if ((!cc->errs)&&((cc->tk==TK_PUNCT)&&(cc->tk_num==OP_AND)))
    CCVal(cc);
  while ((!cc->errs)&&((cc->tk==TK_PUNCT)&&(cc->tk_num==OP_AND)))
  {
    fired=1;
    CCLex(cc);
    CCTestRax(cc);
    pz=CCJz(cc);
    CCExprBitOr(cc);
    CCVal(cc);
    CCTestRax(cc);
    CCSetccRax(cc,CC_OPC_NE);
    CCPatchJmp(cc,pz);
  }
  if (fired)		//M15: normalization ONLY when && actually ran —
  {			//sizeof(expr) parses through this chain and needs
    cc->ety=TY_INT;	//the operand's own type intact when there was
    cc->sty=-1;		//no operator at all.
  }
}

//a || b:  true left yields 1; otherwise normalized right.
U0 CCExprLogOr(CCCtrl *cc)
{
  I64 pz,pj,fired;
  fired=0;
  CCExprLogAnd(cc);
  if ((!cc->errs)&&((cc->tk==TK_PUNCT)&&(cc->tk_num==OP_OR)))
    CCVal(cc);
  while ((!cc->errs)&&((cc->tk==TK_PUNCT)&&(cc->tk_num==OP_OR)))
  {
    fired=1;
    CCLex(cc);
    CCTestRax(cc);		//MUST set flags before the short-circuit JZ —
    pz=CCJz(cc);		//my (F) rewrite silently DROPPED this line and
      			//every `a || b` branched on stale flags (the atoi
      			//`(x<48)||(x>57)` mis-break — three suite checks
      			//failed while every micro-probe stayed green).
    CCMovImmRax(cc,1);
    pj=CCJmp(cc);
    CCPatchJmp(cc,pz);
    CCExprLogAnd(cc);
    CCVal(cc);
    CCTestRax(cc);
    CCSetccRax(cc,CC_OPC_NE);
    CCPatchJmp(cc,pj);
  }
  if (fired)		//same rule as CCExprLogAnd (see M15 note there)
  {
    cc->ety=TY_INT;
    cc->sty=-1;
  }
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

/* M16: `cond ? a : b`. Above the logical chain, below comma. Arms:
 * middle is an assignment-expression (via NC: comma-free, value in
 * RAX), third recurses into THIS level for right-associativity
 * (`a?b:c?d:e`). Codegen is the textbook shape — value already in
 * RAX, JZ to the third arm, JMP over it after the second. Under
 * noemit (sizeof) CCJmp/CCJz return -1 and CCPatchJmp ignores it:
 * the analysis tracks arm types exactly like the emit pass. Result
 * type: the arms' if identical, else the int model. */
U0 CCExprConditional(CCCtrl *cc)
{
  I64 pz,pj,mty,msty;
  CCExprLogOr(cc);
  if (cc->errs)
    return;
  if (!CCTkIsPunct(cc,'?'))
    return;
  CCLex(cc);
  CCVal(cc);		//condition value (LogOr leaves addresses pending)
  CCTestRax(cc);
  pz=CCJz(cc);
  CCExprNC(cc);		//second arm
  mty=cc->ety;
  msty=cc->sty;
  CCExpect(cc,':',"expected ':' in conditional");
  if (cc->errs)
    return;
  pj=CCJmp(cc);
  CCPatchJmp(cc,pz);
  CCExprConditional(cc);	//third arm, right-associative
  CCVal(cc);		//NC normalized the middle arm; normalize this one
  if ((mty==cc->ety)&&(msty==cc->sty))
  {
    //Same type both arms: it stands (pointers keep their element kind
    //and struct tags, so `(c?p:q)->m` works when both are struct*).
  }
  else
  {
    cc->ety=TY_INT;
    cc->sty=-1;
  }
  cc->is_addr=0;
  cc->aty=0;
  CCPatchJmp(cc,pj);
}

/* Comma-free expression: the value consumers (return, if/while cond,
 * function arguments) use. `f(a,b)` must NOT see commas as operators. */
U0 CCExprNC(CCCtrl *cc)
{
  CCExprConditional(cc);
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
  I64 disp,djz,eoj,save_b,i,def,nc,v,rel,fits,smark,sbase,slofs;
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
  smark=cc->sym_cnt;			//M21: the switch body is its
  sbase=cc->sym_base;			//own scope (C: declarations
  cc->sym_base=smark;			//between `{` and the first
  slofs=cc->lofs;			//case live there)
  while ((!cc->errs)&&(cc->tk!=TK_EOF)&&!CCTkIsPunct(cc,'}'))
  {
    if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_CASE))
    {
      CCLex(cc);
      v=CCConstExpr(cc);	//M20: literals, enum constants, and
      if (cc->errs)		//folded expressions (`case ROW*2:`)
        return;			//all qualify — anything else diagnoses
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
  CCEraseScope(cc,smark,sbase);		//M21: erase switch-body declarations
  if (cc->lofs>cc->lofs_peak)
    cc->lofs_peak=cc->lofs;
  cc->lofs=slofs;
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
  I64 save_tk,save_kw,save_num,lmark,lbase,llofs;
  U8 save_str[CC_TOK_MAX];
  U8 *pc_step,*pc_after,*pc_body;
  pc_after=NULL;
  CCLex(cc);		//consume 'for'
  CCExpect(cc,'(',"expected '(' after for");
  //M21: C99 for-init declarations are scoped to the LOOP: mark before
  //the init clause, erase after the whole statement (the step re-scan
  //still sees j; the code after the loop must not).
  lmark=cc->sym_cnt;
  lbase=cc->sym_base;
  llofs=cc->lofs;
  cc->sym_base=lmark;
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
  CCEraseScope(cc,lmark,lbase);		//M21: the for-init scope closes here
  if (cc->lofs>cc->lofs_peak)
    cc->lofs_peak=cc->lofs;
  cc->lofs=llofs;
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

/* M16: `goto name;`. A defined label jumps directly (E9 rel32; the
 * displacement is relative to the operand's END, so compute rel =
 * target-(operand_pos+4) — the emitted-opcode-first discipline of M14
 * in its purest form: nothing is written between the position read
 * and CCWord). An undefined label queues an E9 with a zero operand
 * that CCLabelHere patches when the definition arrives; leftovers are
 * diagnosed at the function's closing brace. */
U0 CCGotoStmt(CCCtrl *cc)
{
  I64 i,rel,lab;
  U8 nm[CC_TOK_MAX];
  CCLex(cc);		//consume 'goto'
  if (cc->tk!=TK_IDENT)
  {
    CCError(cc,"goto needs a label name");
    return;
  }
  StrCpy(nm,cc->tk_str);
  CCLex(cc);
  CCExpect(cc,';',"expected ';' after goto");
  lab=-1;
  for (i=0;i!=cc->nlab;i++)
  {
    if (!StrCmp(cc->lab_name[i],nm))
      lab=cc->lab_pos[i];
  }
  if (lab>=0)
  {
    CCByte(cc,0xE9);
    rel=lab-(cc->ep-cc->code)-4;
    CCWord(cc,rel);
    return;
  }
  if (cc->ngoto>=CC_LABELS)
  {
    CCError(cc,"too many unresolved gotos");
    return;
  }
  StrCpy(cc->goto_name[cc->ngoto],nm);
  CCByte(cc,0xE9);
  cc->goto_pos[cc->ngoto]=cc->ep-cc->code;
  CCWord(cc,0);		//patched by CCLabelHere (or reported dead)
  cc->ngoto++;
}

/* Define nm at the current code position: duplicates are an error;
 * queued gotos naming it get their rel32 written into the placeholder
 * (CCPatchCallPos pattern) and their queue entry parked at -1 so they
 * are neither re-patched nor flagged at function end. */
U0 CCLabelHere(CCCtrl *cc,U8 *nm)
{
  I64 i,pos,rel;
  U8 *p;
  for (i=0;i!=cc->nlab;i++)
  {
    if (!StrCmp(cc->lab_name[i],nm))
    {
      CCError(cc,"label redefined");
      return;
    }
  }
  if (cc->nlab>=CC_LABELS)
  {
    CCError(cc,"too many labels");
    return;
  }
  pos=cc->ep-cc->code;
  StrCpy(cc->lab_name[cc->nlab],nm);
  cc->lab_pos[cc->nlab]=pos;
  cc->nlab++;
  for (i=0;i!=cc->ngoto;i++)
  {
    if ((cc->goto_pos[i]>=0)&&(!StrCmp(cc->goto_name[i],nm)))
    {
      rel=pos-(cc->goto_pos[i]+4);
      p=cc->code+cc->goto_pos[i];
      p[0]=rel;
      p[1]=rel>>8;
      p[2]=rel>>16;
      p[3]=rel>>24;
      cc->goto_pos[i]=-1;
    }
  }
}

/* M21: close a scope. Params, locals, block statics, typedefs and tags
 * all vanish with the brace (the waterline truncation IS the scope
 * mechanism), but a FUNCTION symbol is a special case: a call to a name
 * not yet declared auto-creates its stub, and that stub is created
 * INSIDE whatever window the call happened in. C treats such a function
 * as a file-scope entity — its identity, its queued rel32 call patches
 * and its M18 address queue must outlive the brace, or a later
 * definition creates a SECOND symbol and never patches the queued call
 * (mutual recursion without prototypes: is_even/is_odd, each body
 * stubbing the other — the placeholder rel32 was left dangling and the
 * fixture returned garbage). Compact the SC_FUN entries down onto the
 * mark and keep them. */
U0 CCEraseScope(CCCtrl *cc,I64 mark,I64 base)
{
  I64 w,i;
  w=mark;
  i=mark;
  while (i!=cc->sym_cnt)
  {
    if (cc->syms[i].kind==SC_FUN)
    {
      if (i!=w)
        MemCpy(&cc->syms[w],&cc->syms[i],sizeof(CSym));
      w++;
    }
    i++;
  }
  cc->sym_cnt=w;
  cc->sym_base=base;
}

/* M21: the `static` block-object conversion, shared by the ordinary
 * declarator path and the function-pointer path: hand back the frame
 * slot CCSymNew reserved and take an arena slot instead (C's contract
 * is that the value survives between calls). */
U0 CCMakeStatic(CCCtrl *cc,CSym *s)
{
  cc->lofs--;
  s->persist=1;
  s->offset=cc->goff;
  cc->goff+=8;
}

U0 CCBlockStmt(CCCtrl *cc)
{
  //M21: block scope. Every table this parser appends to is ERASED at
  //the closing brace by restoring its waterline — symbols, frame-slot
  //waterline, typedefs, enum constants+tags, struct/union tags. That
  //IS the scope mechanism: reverse-scan lookup already resolves to
  //the newest name, so erasure alone gives correct shadowing AND
  //makes out-of-scope names undefined again. The frame high-water
  //mark lets a block that peaked deep release its slots while SUB RSP
  //at function end still covers the deepest point the function hit.
  I64 m_sym,m_lofs,m_tn,m_ncnst,m_netag,m_nst,m_base;
  m_sym=cc->sym_cnt;		m_lofs=cc->lofs;
  m_base=cc->sym_base;		cc->sym_base=m_sym;
  m_tn=cc->tn;			m_ncnst=cc->ncnst;
  m_netag=cc->netag;		m_nst=cc->nst;
  CCLex(cc);		//consume '{'
  while ((!cc->errs)&&((cc->tk!=TK_EOF)&&!CCTkIsPunct(cc,'}')))
    CCStmt(cc);
  CCExpect(cc,'}',"expected '}'");
  CCEraseScope(cc,m_sym,m_base);	//truncate: declarations vanish
  if (cc->lofs>cc->lofs_peak)
    cc->lofs_peak=cc->lofs;	//slots released, but the frame pays
  cc->lofs=m_lofs;		//for the peak
  cc->tn=m_tn;
  cc->ncnst=m_ncnst;
  cc->netag=m_netag;
  cc->nst=m_nst;
}

/* One declarator within an `int|char` (or `struct Tag`) declaration:
 * [*] name [n] [= init]. Stars are parsed per declarator so `int a,*b;`
 * types b correctly and `int *a,b;` leaves b a plain int, exactly as C
 * does. base==TY_STRUCT makes a starless declarator a struct object
 * (sized by the tag) and a starred one a struct pointer. */
U0 CCDeclOne(CCCtrl *cc,I64 base,I64 stag)
{
  CSym *s;
  I64 n,n2,fpok,sstat,val,vi;
  I64 ty=base;
  I64 stk,skw,snum;
  U8 *spc;
  U8 sstr[CC_TOK_MAX];
  U8 fpname[CC_TOK_MAX];

  fpok=0;
  if (CCTkIsPunct(cc,'('))		//M18: `int (*fp)(int)` — the
  {
    CCSnap(cc,&spc,&stk,&skw,&snum,sstr);	//ONLY shape that begins
    fpok=CCParseFunPtrDecl(cc,fpname);	//with '(' after a base type
    if (!fpok)
      CCRestore(cc,spc,stk,skw,snum,sstr);
  }
  if (fpok)
  {
    s=CCSymNew(cc,fpname,SC_LOCAL);
    if (!s)
      return;
    if (cc->spec_static)
    {
      //M21: a `static` function pointer needs arena storage like any
      //other block static. An initializer here would be an EMITTED
      //store (function addresses are not foldable constants), which
      //would re-assign the target on every call — C initializes a
      //static exactly once — so it is refused rather than approximated.
      CCMakeStatic(cc,s);
      if (CCTkIsPunct(cc,'='))
      {
        CCError(cc,"static function-pointer initializers not supported");
        return;
      }
    }
    s->ty=TY_PTR;
    s->sty=-1;
    s->dim=1;
    s->dim2=0;
    if (CCTkIsPunct(cc,'='))		//fp=add; — the RHS yields the
    {
      CCVarAddr(cc,s);			//code address (M18 SC_FUN
      cc->is_addr=1;			//value), stored one quad
      cc->aty=0;
      CCAssignTail(cc);
    }
    return;
  }
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
  sstat=cc->spec_static;	//M21: `static` block object — arena
  if (sstat)			//storage, block-scoped name (CCMakeStatic
    CCMakeStatic(cc,s);		//notes). Initializers must fold (checked
				//at the init site).
  CCLex(cc);
  s->ty=ty;
  if (base==TY_STRUCT)
    if ((ty==TY_STRUCT)||(ty==TY_PTR))
      s->sty=stag;
  s->dim=1;
  s->dim2=0;
  if (CCAccept(cc,'['))
  {
    n=CCConstExpr(cc);		//M20: constant expressions (enum
    if (cc->errs)			//sizes) fold here
      return;
    CCExpect(cc,']',"expected ']' after array size");
    if ((n<1)||(n>2048))
    {
      CCError(cc,"array size out of range");
      return;
    }
    s->dim=n;
    if (CCTkIsPunct(cc,'['))		//M17: second dimension (row-major)
    {
      CCLex(cc);
      n2=CCConstExpr(cc);		//M20: constant expressions (enum
      if (cc->errs)			//sizes) fold here
        return;
      CCExpect(cc,']',"expected ']' after second array size");
      if ((n2<1)||(n2>2048))
      {
        CCError(cc,"second array size out of range");
        return;
      }
      s->dim2=n2;
      n=n*n2;		//allocation and indexing cover ALL elements
    }
    if (n>1)
    {
      if (sstat)
      {
        //Arena arrays ascend from the base (global layout): element
        //0 IS s->offset, exactly what the decay+index paths expect.
        if (base==TY_STRUCT)
          cc->goff+=cc->st[stag].size*n-8;
        else
          cc->goff+=8*(n-1);
      }
      else
      {
        //Frame grows DOWN from the base slot: element 0 sits at the
        //highest slot number so element i lands at base+8*i.
        if (base==TY_STRUCT)
          cc->lofs+=n*(cc->st[stag].size/8)-1;	//structs: size*n slots
        else
          cc->lofs+=n-1;
        s->offset=cc->lofs;
      }
    }
    if (s->ty==TY_CHAR)
      s->ty=TY_CHARP;
    else if (s->ty!=TY_STRUCT)
      s->ty=TY_PTR;
  }
  else if (s->ty==TY_STRUCT)
  {
    //Bare struct object: reserve its slots, base at the highest.
    if (sstat)
      cc->goff+=cc->st[stag].size-8;	//persist: arena (global layout)
    else
    {
      cc->lofs+=cc->st[stag].size/8-1;
      s->offset=cc->lofs;
    }
  }
  if (CCTkIsPunct(cc,'='))
  {
    if ((s->dim>1)||(s->ty==TY_STRUCT))
    {
      CCError(cc,"array/struct initializers not supported");
      return;
    }
    if (s->persist)
    {
      //C requires a constant for static init — the folder enforces
      //it; the bytes go straight into the arena (the initial value
      //is there before any call runs, and reloads with the object).
      CCLex(cc);
      val=CCConstExpr(cc);
      if (cc->errs)
        return;
      for (vi=0;vi!=8;vi++)
        cc->locals[s->offset+vi]=val>>(8*vi);
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
  I64 ty,stag,t,isu,isenu;
  I64 stk,skw,snum;
  U8 *spc;
  U8 sstr[CC_TOK_MAX];
  U8 tag[CC_TOK_MAX];

  //M19: block-scope `struct|union Tag { ... };` — mirror of the top-level
  //definition path (CCDeclOne reserves frame slots for declarators after
  //the body). Peeked with the full snapshot: `struct L l;` (a USE) must
  //rewind to run through CCAcceptBase unchanged.
  if ((cc->tk==TK_IDENT)&&((cc->tk_kw==KW_STRUCT)||((cc->tk_kw==KW_UNION)||(cc->tk_kw==KW_ENUM))))
  {
    CCSnap(cc,&spc,&stk,&skw,&snum,sstr);
    isu=(cc->tk_kw==KW_UNION);
    isenu=(cc->tk_kw==KW_ENUM);
    CCLex(cc);
    tag[0]=0;
    if ((cc->tk==TK_IDENT)||isenu)	//struct/union REQUIRE a tag here
    {				//(anonymous definitions rewind to the
      if (cc->tk==TK_IDENT)	//ordinary error path); enum's tag is
      {				//optional — `enum { A } x;` is the
        StrCpy(tag,cc->tk_str);	//common shape
        CCLex(cc);
      }
      if (CCTkIsPunct(cc,'{'))
      {
        if (isenu)
        {
          CCEnumDef(cc,tag);
          if (cc->errs)
            return;
          if (CCAccept(cc,';'))
            return;		//pure definition; constants are table-bound
          while ((!cc->errs)&&(cc->tk!=TK_EOF))
          {
            CCDeclOne(cc,TY_INT,-1);	//`enum E { A } x;` — the int model
            if (cc->errs)
              return;
            if (!CCAccept(cc,','))
              break;
          }
          CCExpect(cc,';',"expected ';' after declaration");
          return;
        }
        t=CCStructDef(cc,tag,isu);
        if (t<0)
          return;
        stag=t;
        if (CCAccept(cc,';'))
          return;		//pure tag definition
        ty=TY_STRUCT;
        while ((!cc->errs)&&(cc->tk!=TK_EOF))
        {
          CCDeclOne(cc,ty,stag);
          if (cc->errs)
            return;
          if (!CCAccept(cc,','))
            break;
        }
        CCExpect(cc,';',"expected ';' after declaration");
        return;
      }
    }
    CCRestore(cc,spc,stk,skw,snum,sstr);	//a use: normal path below
  }
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
  //M16: label-gate peek state (see the snapshot note: pc alone lies).
  U8 *spc;
  I64 stk,skw,snum;
  U8 sstr[CC_TOK_MAX];

  if (cc->errs)
    return;
  if (CCTkIsPunct(cc,'{'))
  {
    CCBlockStmt(cc);
    return;
  }
  if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_GOTO))
  {
    CCGotoStmt(cc);
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
  if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_NONE))
  {
    //M16: statement label `name:` — peek with the FULL lexer state
    //restore (PLAN: a pc-only rewind swallows the token after the
    //rewound run; CCSnap/CCRestore are the sanctioned pair). Keyword-
    //typed identifiers are excluded up front. A typedef name used as
    //a label (`size_t:`) is C-legal: labels are their own namespace,
    //and the ':' peek never collides with a declaration.
    CCSnap(cc,&spc,&stk,&skw,&snum,sstr);
    CCLex(cc);
    if (CCTkIsPunct(cc,':'))
    {
      CCLex(cc);		//consume ':'
      CCLabelHere(cc,sstr);
      CCStmt(cc);		//the labeled statement (another label,
      return;			//or any statement — C's shape)
    }
    CCRestore(cc,spc,stk,skw,snum,sstr);
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
  I64 base,stag,i,fpok;
  I64 stk,skw,snum;
  U8 *spc;
  U8 sstr[CC_TOK_MAX];
  U8 fpname[CC_TOK_MAX];

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
      fpok=0;
      if (CCTkIsPunct(cc,'('))		//M18: `run(int (*f)(int))` and
      {
        CCSnap(cc,&spc,&stk,&skw,&snum,sstr);	//the anonymous
        fpok=CCParseFunPtrDecl(cc,fpname);	//`int (*)(int)`
        if (!fpok)
          CCRestore(cc,spc,stk,skw,snum,sstr);
      }
      if (!fpok)
      {
      while (CCAccept(cc,'*'))
      {
        //char* -> byte pointer; any further pointer (char**, int**) —
        //and struct pointers — are modeled as pointers to 8-byte slots.
        if (cc->pty[cc->nparam]==TY_CHAR)
          cc->pty[cc->nparam]=TY_CHARP;
        else
          cc->pty[cc->nparam]=TY_PTR;
      }
      }
      i=cc->nparam;
      cc->pname[i*CC_TOK_MAX]=0;	//empty name = anonymous parameter
      cc->pdim2[i]=0;			//M17: cleared until proven otherwise
      if (fpok)
      {
        StrCpy(cc->pname+i*CC_TOK_MAX,fpname);	//"" stays anonymous
        cc->pty[i]=TY_PTR;
        cc->psty[i]=-1;
      }
      else if (cc->tk==TK_IDENT)
      {
        StrCpy(cc->pname+i*CC_TOK_MAX,cc->tk_str);
        CCLex(cc);
      }
      cc->nparam++;
      if (CCAccept(cc,'['))
      {
        //Array parameter. The first size is ALWAYS negotiable (the
        //caller owns the count) and ignored. Element scales come from
        //what the elements ARE: a char t[] element is a byte (the old
        //blanket TY_PTR made `char s[]` params stride by quads — C
        //truth is 1; latent until the M17 probes exercised it); an
        //int a[] element is a slot; and `t[][64]` (or [8][64]) rows
        //scale by pdim2 — the second dim is the one that must MATCH
        //the caller's layout, which is why it is the one recorded.
        if (cc->tk==TK_NUM)
          CCLex(cc);
        CCExpect(cc,']',"expected ']' after parameter array size");
        if (cc->pty[i]==TY_CHAR)
          cc->pty[i]=TY_CHARP;
        else
          cc->pty[i]=TY_PTR;
        if (CCTkIsPunct(cc,'['))
        {
          CCLex(cc);
          if (!CCTkIsPunct(cc,']'))	//M20: an OFFERED second size must
          {				//fold (enum ROW sizes work now;
            cc->pdim2[i]=CCConstExpr(cc);	//`t[]` stays negotiable)
            if (cc->errs)
              return;
          }
          CCExpect(cc,']',"expected ']' after second parameter array size");
        }
      }
      if (cc->pty[i]==TY_STRUCT)
      {
        CCError(cc,"struct parameters must be pointers: struct T *p");
        return;
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
  I64 sub_pos,i,entryflag,t,apv,fmark,fbase,nframe;
  U8 *app;
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
  //M21: the FUNCTION symbol is registered ABOVE the scope waterline.
  //Params and locals are erased at '}'; the function's own name must
  //SURVIVE (forward calls and M18 address queues were resolved against
  //it, and every later function/reference needs it — erasing it made
  //each subsequent use re-queue, which is what broke cross-function
  //code after the waterline landed). The mark is taken after this
  //registration, so params clash only against each other and a param
  //may legally shadow the function's own name.
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
  //M18: forward function-ADDRESS references (fp=add before add is
  //defined): write the final absolute into each deferred MOVABS
  //operand — the buffer never moved, so the position is exact — and
  //record the save/reload fixup here (delta = bytes from the CODE
  //base, exactly what CCLoad's generic CCBase+delta patch expects).
  for (i=0;i!=f->napatch;i++)
  {
    app=cc->code+f->apatch[i];
    apv=cc->code+entry;
    app[0]=apv;
    app[1]=apv>>8;
    app[2]=apv>>16;
    app[3]=apv>>24;
    app[4]=apv>>32;
    app[5]=apv>>40;
    app[6]=apv>>48;
    app[7]=apv>>56;
    if (cc->nfix>=CC_FIXUPS)
      CCError(cc,"too many absolute-address sites");
    else
    {
      cc->fix_pos[cc->nfix]=f->apatch[i];
      cc->fix_kind[cc->nfix]=CC_B_CODE;
      cc->fix_delta[cc->nfix]=entry;
      cc->nfix++;
    }
  }
  f->napatch=0;
  //M21: scope mark for the BODY: params+locals are erased at '}';
  //the function symbol registered above stays. The dup-check bound
  //during param binding starts here, so a param may shadow any
  //file-scope name (and even the function itself) but not another
  //param.
  fmark=cc->sym_cnt;
  fbase=cc->sym_base;
  cc->sym_base=fmark;
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
        s->dim2=cc->pdim2[i];	//M17: row scale source (1st dim unknown)
      }
    }
  }
  cc->cur_argc=cc->nparam;
  cc->lofs=0;		//locals restart per function
  cc->lofs_peak=0;
  cc->nlab=0;		//M16: label scope is per-function too (C functions
  cc->ngoto=0;		//never nest, so one table per function is exact)
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
  CCEraseScope(cc,fmark,fbase);	//params + locals leave the table
  if (cc->lofs>cc->lofs_peak)
    cc->lofs_peak=cc->lofs;
  //M16: a goto whose label never arrived in this function (or names a
  //label in ANOTHER function — labels are function-scoped, the queue
  //reset proves it) is a compile error. Patched entries sit at -1.
  for (i=0;i!=cc->ngoto;i++)
  {
    if (cc->goto_pos[i]>=0)
    {
      "CC ERROR: goto to undefined label %s\n",cc->goto_name[i];
      cc->errs++;
    }
  }
  nframe=cc->lofs;
  if (cc->lofs_peak>nframe)
    nframe=cc->lofs_peak;	//M21: blocks released slots, but the
  CCPatchImm32(cc,sub_pos,(nframe*8+15)&~15);	//deepest one was paid for
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
  I64 ty=base,val,neg,n,n2,i,entry,fpok;
  I64 stk,skw,snum;
  U8 *spc;
  U8 sstr[CC_TOK_MAX];
  U8 fpname[CC_TOK_MAX];
  U8 *gp;
  CSym *s;

  fpok=0;
  if (CCTkIsPunct(cc,'('))		//M18: global `int (*gf)(int)`
  {
    CCSnap(cc,&spc,&stk,&skw,&snum,sstr);
    fpok=CCParseFunPtrDecl(cc,fpname);
    if (!fpok)
      CCRestore(cc,spc,stk,skw,snum,sstr);
  }
  if (fpok)
  {
    s=CCSymNew(cc,fpname,SC_GLOBAL);
    if (!s)
      return FALSE;
    s->ty=TY_PTR;
    s->sty=-1;
    s->dim=1;
    s->dim2=0;
    if (CCTkIsPunct(cc,'='))
    {
      //Global fp initializers stay in the constant-only club: a
      //function address is known at compile time but the arena-write
      //path takes TK_NUM; assign in main instead (documented).
      CCError(cc,"global initializer must be a constant");
      return FALSE;
    }
    return FALSE;		//FALSE = "declarator done, ';' follows"
  }
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
  s->dim2=0;
  if (CCAccept(cc,'['))
  {
    n=CCConstExpr(cc);		//M20: constant expressions (enum
    if (cc->errs)			//sizes) fold here
      return FALSE;
    CCExpect(cc,']',"expected ']' after global array size");
    if ((n<1)||(n>2048))
    {
      CCError(cc,"array size out of range");
      return FALSE;
    }
    s->dim=n;
    if (CCTkIsPunct(cc,'['))		//M17: second dimension
    {
      CCLex(cc);
      n2=CCConstExpr(cc);		//M20: constant expressions (enum
      if (cc->errs)			//sizes) fold here
        return FALSE;
      CCExpect(cc,']',"expected ']' after second global array size");
      if ((n2<1)||(n2>2048))
      {
        CCError(cc,"second array size out of range");
        return FALSE;
      }
      s->dim2=n2;
      n=n*n2;		//arena advance covers ALL elements
    }
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
      //M20: literals, enum constants, and folded expressions
      //(`g=RED*2+1`) initialize globals. Anything that is not a
      //compile-time constant — a variable, a call, a function
      //address — still diagnoses loudly here.
      val=CCConstExpr(cc);
      if (cc->errs)
        return FALSE;
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
I64 CCStructDef(CCCtrl *cc,U8 *tag,I64 is_union)
{
  I64 t,mty,msty,mi,fpok,ui;
  I64 stk,skw,snum;
  U8 *spc;
  U8 sstr[CC_TOK_MAX];
  U8 fpname[CC_TOK_MAX];
  U8 *nm;
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
    fpok=0;
    if ((cc->tk==TK_IDENT)&&((cc->tk_kw==KW_STRUCT)||(cc->tk_kw==KW_UNION)))
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
      if (CCTkIsPunct(cc,'('))		//M18: `struct T (*fn)()` member
      {
        CCSnap(cc,&spc,&stk,&skw,&snum,sstr);
        fpok=CCParseFunPtrDecl(cc,fpname);
        if (!fpok)
          CCRestore(cc,spc,stk,skw,snum,sstr);
      }
      if (fpok)
      {
        mty=TY_PTR;		//an fp member is a plain slot
        msty=-1;		//address: no tag involved
      }
      else if (CCTkIsPunct(cc,'*'))
      {
        while (CCAccept(cc,'*'))
        {
          //`struct T **m` is still a slot pointer.
        }
        mty=TY_PTR;
      }
      else if (cc->st[msty].size==8)
      {
        //M19: BY-VALUE member of a one-slot tag — every union (all
        //members are slots, so a union is always exactly one slot)
        //and any single-member struct. The member IS a slot: no copy
        //semantics needed, and the Postfix '.' arm chains through
        //msty for free. Multi-slot structs stay pointer-only (C's
        //by-value copy needs member memcpy this model lacks); a tag
        //mid-definition still has size 0 here, which is why C's
        //illegal self-by-value `struct S { struct S s; }` rejects
        //itself with the pointer message.
        mty=TY_STRUCT;		//value semantics; msty carries the tag
      }
      else
      {
        CCError(cc,"struct members must be pointers: struct T *m");
        return -1;
      }
    }
    else if ((cc->tk==TK_IDENT)&&((cc->tk_kw==KW_INT)||(cc->tk_kw==KW_CHAR)))
    {
      if (cc->tk_kw==KW_CHAR)
        mty=TY_CHAR;
      else
        mty=TY_INT;
      CCLex(cc);
      fpok=0;
      if (CCTkIsPunct(cc,'('))		//M18: `int (*fn)(int)` member
      {
        CCSnap(cc,&spc,&stk,&skw,&snum,sstr);
        fpok=CCParseFunPtrDecl(cc,fpname);
        if (!fpok)
          CCRestore(cc,spc,stk,skw,snum,sstr);
      }
      if (fpok)
        mty=TY_PTR;
      else
      {
        while (CCAccept(cc,'*'))
        {
          if (mty==TY_CHAR)
            mty=TY_CHARP;
          else
            mty=TY_PTR;
        }
      }
    }
    else
    {
      CCError(cc,"struct members are int/char (optionally starred) or struct pointers");
      return -1;
    }
    if (fpok)
    {
      //M18: the declarator swallowed the name (`(*fn)`); an anonymous
      //member would be unreachable, so C-legal or not it is an error.
      if (!fpname[0])
      {
        CCError(cc,"function-pointer members need a name");
        return -1;
      }
      nm=fpname;
    }
    else
    {
      if (cc->tk!=TK_IDENT)
      {
        CCError(cc,"expected member name");
        return -1;
      }
      nm=cc->tk_str;
      CCLex(cc);
    }
    mi=cc->st[t].nmem;
    if (mi>=CC_MAX_MEMBERS)
    {
      CCError(cc,"too many struct members");
      return -1;
    }
    StrCpy(cc->st[t].mname+mi*CC_TOK_MAX,nm);
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
  if (is_union)
  {
    //M19: the single-slot union. Every member type this model admits
    //is one 8-byte slot (int/char fold/pointer/struct pointer/fp), so
    //every member IS the slot: all offsets 0, size exactly one slot.
    //That is byte-truth too: a char member reads its quad's low byte
    //and the arena/frames are zeroed, so int->char->int round trips
    //deterministically (C would call the stale upper bits garbage).
    for (ui=0;ui!=cc->st[t].nmem;ui++)
      cc->st[t].moff[ui]=0;
    cc->st[t].size=8;
  }
  else
    cc->st[t].size=8*cc->st[t].nmem;
  return t;
}

/* Top-level: `struct` definition/declaration, function definition, or
 * `int|char decl [, decl]* ;` */
U0 CCParseTop(CCCtrl *cc)
{
  I64 ty=TY_INT,first=TRUE,stag=-1,t,isunion;
  I64 stk,skw,snum;
  U8 *spc;
  U8 sstr[CC_TOK_MAX];
  U8 tag[CC_TOK_MAX];

  if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_TYPEDEF))
  {
    CCParseTypedef(cc);
    return;
  }
  //M20: top-level enum DEFINITION (`enum [Tag] { ... }` — tag
  //optional). The use form (`enum E x;` — foldable through the
  //CCAcceptBase enum arm below) rewinds via the snapshot; declarators
  //after a body land in the ordinary global-declarator loop with the
  //int model as their base.
  if ((cc->tk==TK_IDENT)&&(cc->tk_kw==KW_ENUM))
  {
    CCSnap(cc,&spc,&stk,&skw,&snum,sstr);
    CCLex(cc);		//consume 'enum'
    tag[0]=0;
    if (cc->tk==TK_IDENT)
    {
      StrCpy(tag,cc->tk_str);
      CCLex(cc);
    }
    if (CCTkIsPunct(cc,'{'))
    {
      CCEnumDef(cc,tag);
      if (cc->errs)
        return;
      if (CCAccept(cc,';'))
        return;		//pure definition
      while ((!cc->errs)&&(cc->tk!=TK_EOF))
      {
        CCGlobalOne(cc,TY_INT,FALSE,-1);	//functions after an enum body
        if (cc->errs)			//keep their own statement
          return;
        if (!CCAccept(cc,','))
          break;
      }
      CCExpect(cc,';',"expected ';' after enum declaration");
      return;
    }
    CCRestore(cc,spc,stk,skw,snum,sstr);	//use form: CCAcceptBase below
  }
  if ((cc->tk==TK_IDENT)&&((cc->tk_kw==KW_STRUCT)||(cc->tk_kw==KW_UNION)))
  {
    isunion=(cc->tk_kw==KW_UNION);
    CCLex(cc);			//consume 'struct' or 'union'
    if (cc->tk!=TK_IDENT)
    {
      CCError(cc,"expected tag name");
      return;
    }
    StrCpy(tag,cc->tk_str);
    CCLex(cc);
    if (CCTkIsPunct(cc,'{'))
    {
      t=CCStructDef(cc,tag,isunion);
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
    if ((s->kind==SC_FUN)&&(s->offset<0))
    {
      if (s->npatch)
        CCError(cc,"call to undefined function");
      if (s->napatch)		//M18: prototype promised an address
        CCError(cc,"address of undefined function");	//nobody ever defined
    }
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
