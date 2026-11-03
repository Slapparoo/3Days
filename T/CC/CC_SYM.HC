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
  if (cc->sym_cnt>=CC_SYMS)
  {
    CCError(cc,"symbol table full");
    return NULL;
  }
  s=&cc->syms[cc->sym_cnt++];
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
  if ((s->kind==SC_LOCAL)||(s->kind==SC_PARAM))
  {
    CCByte(cc,0x48);CCByte(cc,0x8D);CCByte(cc,0x85);	//LEA RAX,[RBP+d32]
    CCWord(cc,CCRbpDisp(cc,s));
  }
  else if (s->kind==SC_FUN)
    CCError(cc,"function name used as variable");
  else
  {
    CCByte(cc,0x48);CCByte(cc,0xB8);			//MOVABS RAX,addr
    CCQuadAbs(cc,CC_B_LOCALS,cc->locals+s->offset);
  }
}
