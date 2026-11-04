/* Symbol table. M3: one I64 slot per symbol.
 * Locals live at [RBP-8*offset]; globals at absolute address
 * cc->locals+offset (the MOVABS form keeps us 64-bit safe). */

CSym *CCSymFind(CCCtrl *cc,U8 *name)
{
  I64 i;
  //Reverse scan: the most recently declared symbol wins. Function
  //parameters/locals are created immediately before their body, so
  //each function sees its own, and prelude symbols stay visible to
  //user code without shadowing same-named user declarations.
  for (i=cc->sym_cnt-1;i>=0;i--)
    if (!StrCmp(cc->syms[i].name,name))
      return &cc->syms[i];
  return NULL;
}

CSym *CCSymNew(CCCtrl *cc,U8 *name,I64 kind)
{
  CSym *s;
  I64 i;
  if (cc->sym_cnt>=CC_SYMS)
  {
    CCError(cc,"symbol table full");
    return NULL;
  }
  if (((kind==SC_LOCAL)||(kind==SC_PARAM))&&(!cc->noemit))
  {
    //M21: C forbids redeclaring a name in the SAME scope; a nested
    //scope may redeclare freely (that is shadowing, and reverse-scan
    //CCSymFind resolves to the inner one). The scan is bounded by the
    //open scope's waterline — outer names are shadowed, not clashes.
    for (i=cc->sym_cnt-1;i!=cc->sym_base-1;i--)
    {
      if (!StrCmp(cc->syms[i].name,name))
      {
        //C-faithful diagnostic, then hand back the EXISTING symbol:
        //callers keep dereferencing the return value, and returning
        //NULL here would turn one clean error into a host fault. errs
        //is set, so the translation unwinds anyway.
        CCError(cc,"redeclaration in this scope");
        return &cc->syms[i];
      }
      if (i==0)
        break;
    }
  }
  //M21: scope waterlines TRUNCATE the table, so these indices are
  //recycled — a fresh symbol must not inherit the dead one's dim/persist/
  //patch queues. Zero the whole entry, then set what matters.
  s=&cc->syms[cc->sym_cnt++];
  MemSet(s,0,sizeof(CSym));
  StrCpy(s->name,name);
  s->kind=kind;
  s->sty=-1;		//struct decls overwrite when relevant
  if (kind==SC_LOCAL)
  {
    cc->lofs++;
    s->offset=cc->lofs;		//slot number; byte offset is -8*offset
  }
  else if (kind==SC_GLOBAL)
  {
    s->offset=cc->goff;
    cc->goff+=8;
  }
  else if (kind==SC_FUN)
    s->offset=-1;		//defined later sets it
  return s;
}

/* Find or lazily declare at file scope (used for globals referenced
 * before their declaration; declarations call CCSymNew explicitly). */
CSym *CCSymVar(CCCtrl *cc,U8 *name)
{
  CSym *s=CCSymFind(cc,name);
  if (s)
    return s;
  return CCSymNew(cc,name,SC_GLOBAL);
}

//RBP-relative byte displacement for locals/params.
I64 CCRbpDisp(CCCtrl *cc,CSym *s)
{
  if (s->kind==SC_LOCAL)
    return -8*s->offset;
  //Params were pushed left-to-right, so the leftmost sits highest.
  return 16+8*(cc->cur_argc-1-s->offset);
}

/* Struct tag id by name, -1 if undefined. */
I64 CCStagFind(CCCtrl *cc,U8 *name)
{
  I64 i;
  for (i=cc->nst-1;i>=0;i--)
    if (!StrCmp(cc->st[i].name,name))
      return i;
  return -1;
}

/* Member index within tag t by name, -1 if none. */
I64 CCMemberFind(CCCtrl *cc,I64 t,U8 *mname)
{
  I64 i;
  for (i=0;i!=cc->st[t].nmem;i++)
    if (!StrCmp(cc->st[t].mname+i*CC_TOK_MAX,mname))
      return i;
  return -1;
}

/* RAX = address of the variable's storage. Loads/stores are the
 * parser's job (pending-lvalue model: see CCCtrl is_addr). */
U0 CCVarAddr(CCCtrl *cc,CSym *s)
{
  if ((s->kind==SC_GLOBAL)&&(!cc->noemit))
    s->refs=1;		//M34: this is the one place a name lookup turns
  if (s->persist)	//into storage access, so it is where "something used
			//this global" gets recorded. C asks that question
			//only of objects a program actually touches, which
			//is why an unreferenced `extern int g;` is fine and
			//a referenced one is not; a silent sizeof scan must
			//not answer it, hence the noemit test.
  {
    //M21: `static` block object: absolute arena addressing like a
    //global; the NAME is block-scoped through the symbol table.
    CCByte(cc,0x48);CCByte(cc,0xB8);			//MOVABS RAX,addr
    CCQuadAbs(cc,CC_B_LOCALS,cc->locals+s->offset);
  }
  else if ((s->kind==SC_LOCAL)||(s->kind==SC_PARAM))
  {
    CCByte(cc,0x48);CCByte(cc,0x8D);CCByte(cc,0x85);	//LEA RAX,[RBP+d32]
    CCWord(cc,CCRbpDisp(cc,s));
  }
  else if (s->kind==SC_FUN)
  {
    //M18: in C a function name IS an address value. Offset known?
    //emit the code-buffer absolute (relocatable via CC_B_CODE, so a
    //saved object rebases identically on reload). is_addr=2: it is
    //already a value (never loads) and never assignable ("assignment
    //to array" is the reuse of that state — C rejects `f=x` too).
    //Forward reference (prototype seen, body later): queue the operand
    //position with the symbol; CCParseFun writes the final address at
    //definition and records the save/reload fixup then. The code
    //buffer is allocated once per compile (CCInit), so the queued
    //position never slides under a buffer move. Under noemit nothing
    //is emitted AND nothing queued — a silent sizeof scan must not
    //register a site whose 10 bytes were never written.
    if (s->offset<0)
    {
      if (!cc->noemit)
      {
        if (s->napatch>=CC_PATCHES)
          CCError(cc,"too many forward references to this function");
        else
        {
          CCByte(cc,0x48);CCByte(cc,0xB8);	//MOVABS RAX,placeholder
          s->apatch[s->napatch++]=cc->ep-cc->code;
          CCQuad(cc,0);
        }
      }
    }
    else
    {
      CCByte(cc,0x48);CCByte(cc,0xB8);		//MOVABS RAX,code+off
      CCQuadAbs(cc,CC_B_CODE,cc->code+s->offset);
    }
  }
  else
  {
    CCByte(cc,0x48);CCByte(cc,0xB8);			//MOVABS RAX,addr
    CCQuadAbs(cc,CC_B_LOCALS,cc->locals+s->offset);
  }
}
