public U0 FunRep(U8 *st,U8 *rbp=NULL,I64 max_depth=2,CTask *task=NULL)
{//Shows names and vals of a fun's local vars using compiler's info.
  I64 size;
  U8 *img;
  CHashFun *tmpf=HashFind(st,Fs->hash_table,HTT_FUN);
  CMemberLst *tmpm;
  if (tmpf) {
    if(1) {
      tmpm=tmpf->member_lst_and_root;
      while (tmpm) {
	if (0<=tmpm->reg<REG_REGS_NUM)
	  "%08Z %s\n",tmpm->reg,"ST_U64_REGS",tmpm->str;
	else
	  "%08tX %s\n",tmpm->offset,tmpm->str;
	tmpm=tmpm->next;
      }
      "%08tX Stk Size\n",tmpf->size;
    }
  }
}

#help_index "Debugging/Unassemble"
public U0 Uf(U8 *st)
{//Unassembles a named fun
  I64 i;
  CHashSrcSym *tmpf;
  CDbgInfo *dbg_info;
  if (tmpf=HashFind(st,Fs->hash_table,HTT_FUN|HTT_EXPORT_SYS_SYM)) {
    if (tmpf->type&HTT_FUN)
      FunRep(st);
    if (dbg_info=tmpf->dbg_info) {
      i=dbg_info->body[dbg_info->max_line+1-dbg_info->min_line]
	    -dbg_info->body[0];
      Un(dbg_info->body[0],i);
      "Code Size:%04X\n",i;
    } else
      U(HashVal(tmpf));
  }
}
#help_index ""
