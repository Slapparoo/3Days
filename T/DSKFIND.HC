U0 DirContextDel(CDirContext *dirc,Bool restore=TRUE)
{//Change back to old cur_dir and drv.
  CBlkDev *bd;
  if (!dirc) return;
  if (restore) {
  Cd(dirc->old_dir);
  /* TODO RESTORE???
    bd=dirc->old_dv->bd;
    if (!(bd->flags & BDF_INIT_IN_PROGRESS)) {
      if (dirc->old_dir) {
        
      }
    } else {
      Fs->cur_dv=dirc->old_dv;
      Free(Fs->cur_dir);
      Fs->cur_dir=StrNew("/");
    }
    */
  }
  Free(dirc->old_dir);
  Free(dirc->mask);
  Free(dirc);
}
Bool Cd2(U8 *to,Bool make=FALSE) {
	if(make&&!FileFind(to))
		DirMk(to);
	return Cd(to);
}

#define Cd Cd2
CDirContext *DirContextNew(U8 *_mask,Bool make_mask=FALSE,
	Bool make_dirs=FALSE,Bool no_mask=FALSE)
{//Save cur_dir and drv. Change to new dir.
  Bool valid=TRUE,old_silent;
  I64 mask_len=StrLen(_mask);
  U8 *buf,*mask,*tmp_mask,*semicolon_mask;
  CDirContext *dirc=CAlloc(sizeof(CDirContext));
  dirc->old_dir=DirCur;
  mask=MStrUtil(_mask,SUF_REM_LEADING|SUF_REM_TRAILING|SUF_REM_CTRL_CHARS);
  tmp_mask=mask;
  if (*mask && mask[1]==':') {
  /*TODO RESTORE
    if (Fs->cur_dv!=Let2Drv(*mask) && !Drv(*mask))
      valid=FALSE;
      */
    mask+=2;
  }
  /* TODO RESTORE
  if (*mask=='~' && Fs->cur_dv!=Let2Drv('~') && !Drv('~'))
    valid=FALSE;
  */
  //dirc->dv=Fs->cur_dv;
  //DrvChk(dirc->dv);
  buf=MAlloc(mask_len+2);
  StrCpy(buf,mask);

  dirc->mask=MAlloc(mask_len+2);
  if (no_mask)
    *dirc->mask=0;
  else if (StrOcc(buf,';')) {
    semicolon_mask=MAlloc(mask_len+2);
    StrCpy(semicolon_mask,mask);
    StrFirstRem(semicolon_mask,";",buf);
    StrLastRem(buf,"/",dirc->mask);
    CatPrint(dirc->mask,";%s",semicolon_mask);
    Free(semicolon_mask);
  } else
    StrLastRem(buf,"/",dirc->mask);

  if (*mask=='/' && !*buf)
    StrCpy(buf,"/");
//If began with Dir, change to Dir.
  if (*buf && !Cd(buf ,make_dirs))
    valid=FALSE;
  if (valid && make_mask) {
    if (!*dirc->mask) {
      Free(dirc->mask);
      dirc->mask=StrNew("*");
    } else {
      if (!make_dirs || FileNameChk(dirc->mask)) {
	//old_silent=Silent; TODO RESTORE
//Try mask to see if Dir. If Dir, change to dir and set to "*".
	if (Cd(dirc->mask ,make_dirs)) { //URGENT RESTORE
	  Free(dirc->mask);
	  dirc->mask=StrNew("*");
	}
	//Silent(old_silent); TODO RESTORE
      }
    }
  }
  Free(buf);
  Free(tmp_mask);
  if (!valid) {
    DirContextDel(dirc);
    dirc=NULL;
  }
  return dirc;
}

CDirEntry *FilesFind__(U8 *files_find_mask,I64 fuf_flags=0,
                       CDirEntry *parent=NULL)
{
    CDirEntry *res=NULL,*tmpde;
    U8 **dir=__Dir(".");
    I64 ch,idx;
    if (fuf_flags&~FUG_FILES_FIND)
        throw('FUF');
    try {
        for (idx=0; dir&&dir[idx]; idx++) {
            tmpde=CAlloc(sizeof(CDirEntry));
            StrCpy(&tmpde->name,dir[idx]);
            tmpde->datetime=Unix2CDate(FUnixTime(dir[idx]));
            tmpde->size=FSize(dir[idx]);
            tmpde->parent=parent;
            if (Bt(&fuf_flags,FUf_RECURSE) && IsDir(dir[idx]) &&
                    *tmpde->name!='.'&&*tmpde->name!='..') {
                tmpde->next=res;
                res=tmpde;
                tmpde->full_name=DirNameAbs(tmpde->name);
                if (Cd(tmpde->name)) {
                    tmpde->sub=FilesFind__(files_find_mask,fuf_flags,tmpde);
                    Cd("..");
                }
            } else {
                tmpde->full_name=FileNameAbs(tmpde->name);
                if ((IsDir(dir[idx]) ||
                        !Bt(&fuf_flags,FUf_JUST_DIRS)) &&
                        !(Bt(&fuf_flags,FUf_RECURSE) && *tmpde->name=='.' &&
                          IsDir(dir[idx])) &&
                        FilesFindMatch(tmpde->full_name,files_find_mask,fuf_flags)) {
                    tmpde->next=res;
                    res=tmpde;
                } else if(!FilesFindMatch(tmpde->full_name,files_find_mask,fuf_flags)) {
                    DirEntryDel(tmpde);
                } else {
                    tmpde->next=res;
                    res=tmpde;
                } 
            }
        }
    } catch
        ;
    for(idx=0; dir&&dir[idx]; idx++)
        Free(dir[idx]);
    Free(dir);
    return res;
}
CDirEntry *FilesFind2(U8 *files_find_mask,I64 fuf_flags)
{
  CDirEntry *res=FilesFind__(files_find_mask,fuf_flags);
  if (res) {
    DirFilesSort(&res,SK_NAME);
    if (fuf_flags&(FUF_FLATTEN_TREE|FUF_JUST_FILES))
      DirFilesFlatten(res,&res,fuf_flags);
    if (fuf_flags&FUF_CLUS_ORDER)
      DirFilesSort(&res,SK_CLUS);
  }
  return res;
}
CDirEntry *FilesFind(U8 *files_find_mask,I64 fuf_flags=0)
{/* See \dLK,"::/Doc/FileUtils.DD"\d.

Find files and make a directory tree in memory.

When done, you free with \dLK,"DirEntryDel",A="MN:DirEntryDel"\d(),\dLK,"DirEntryDel2",A="MN:DirEntryDel2"\d(),
\dLK,"DirTreeDel",A="MN:DirTreeDel"\d() or \dLK,"DirTreeDel2",A="MN:DirTreeDel2"\d().

*/
  CDirEntry *res;
  CDirContext *dirc;
  if (fuf_flags&~FUG_FILES_FIND)
    throw('FUF');
  if (fuf_flags&FUF_SINGLE) {
    res=MAlloc(sizeof(CDirEntry));
    if (!FileFind(files_find_mask,res)) {
      Free(res);
      return NULL;
    }
  } else if (dirc=DirContextNew(files_find_mask,TRUE)) {
    res=FilesFind2(dirc->mask,fuf_flags);
    DirContextDel(dirc);
  } else
    return NULL;
  return res;
}
static Bool IsAtRoot() {
    U8 *r=DirCur;
    Bool ret=r[2]=='/'&&!r[3];
    Free(r);
    return ret; 
}
Bool FileFind(U8 *filename,CDirEntry *_de=NULL,I64 fuf_flags=FUF_Z_OR_NOT_Z)
{//\dLK,"FUF_JUST_DIRS",A="MN:FUF_JUST_DIRS"\d, \dLK,"FUF_JUST_FILES",A="MN:FUF_JUST_FILES"\d, \dLK,"FUF_Z_OR_NOT_Z",A="MN:FUF_Z_OR_NOT_Z"\d, \dLK,"FUF_SCAN_PARENTS",A="MN:FUF_SCAN_PARENTS"\d
//If you pass _de, you must Free(_de->full_name);
  I64 i,j,cur_dir_clus;
  U8 *altname,*curname,*full_name=NULL,*dn;
  CDirEntry de,*_de2;
  CDirContext *dirc;
  Bool res=FALSE,old_silent;
  if (fuf_flags&~FUG_FILE_FIND)
    throw('FUF');
  if (!filename  /* || *filename && filename[1]==':'   &&!Let2Drv(*filename,FALSE)*/) //TODO RESTORE
    return FALSE;
  altname=ToggleZorNotZ(filename);
  if (fuf_flags&FUF_Z_OR_NOT_Z)
    j=2;
  else
    j=1;
  for (i=0;i<j && !res;i++) {
    if (!i)
      curname=filename;
    else
      curname=altname;
    //old_silent=Silent; TODO RESTORE
    if (!(dirc=DirContextNew(curname)))
      ;//Silent(old_silent);
    else {
      ;//Silent(old_silent);
      //cur_dir_clus=Name2DirClus(dirc->dv,Fs->cur_dir);
        _de2=FilesFind__(dirc->mask);
        res=NULL!=_de2;
        if(res) {
            MemCpy(&de,_de2,sizeof(de));
            de.full_name=StrNew(_de2->full_name);
        }
        DirEntryDel2(_de2);
      if (res && _de) {
		full_name=DirNameAbs(de.name);
      }
      DirContextDel(dirc);
    }
  }
  for (i=0;i<j && !res && fuf_flags&FUF_SCAN_PARENTS;i++) {
    if (!i)
      curname=filename;
    else
      curname=altname;
    //old_silent=Silent; TODO RESTORE
    if (!(dirc=DirContextNew(curname)))
      ;//Silent(old_silent);
    else {
      ;//Silent(old_silent);
      //cur_dir_clus=FileNameAbs(dirc->dv,Fs->cur_dir);
      //TODO CHECK IF AT ROOT ON WINDOWS
      while (!res && !IsAtRoot) {
	Cd("..");
	//cur_dir_clus=FileNameAbs(dirc->dv,Fs->cur_dir);
        _de2=FilesFind__(dirc->mask);
        res=NULL!=_de2; 
        if(res) {
            MemCpy(&de,_de2,sizeof(de));
            de.full_name=StrNew(_de2->full_name);
        }
        DirEntryDel2(_de2);
      }
      if (res && _de) {
		full_name=DirNameAbs(de.name);
      }
      DirContextDel(dirc);
    }
  }
  if (_de) {
    if (res) {
      MemCpy(_de,&de,sizeof(CDirEntry));
      _de->full_name=full_name;
    } else
      MemSet(_de,0,sizeof(CDirEntry));
  }
  Free(altname);
  return res;
}
U0 PutFileLink(U8 *filename,U8 *full_name=NULL,I64 line=0,Bool plain_text=FALSE)
{//Put \dLK,"DolDoc",A="FI:::/Doc/DolDocOverview.DD"\d file,line link to StdOut, \dLK,"DocPut",A="MN:DocPut"\d.
  U8 *st;
  if (!filename) return;
  if (IsRaw) {
    if (line)
      "%s,%04d",filename,line;
    else
      "%s",filename;
  } else {
//LK_DOC,LK_DOC_ANCHOR,LK_DOC_FIND,LK_DOC_LINE
    if (filename[0]=='A'&&filename[2]==':') {
      if (line) //See \dLK,"SpriteEdText",A="MN:SpriteEdText"\d()
	"\dLK,\"%s,%04d\",A=\"AL:%s,%d\"\d",filename+3,line,filename+3,line;
      else
	"\dLK,\"%s\",A=\"AI:%s\"\d",filename+3,filename+3;
    } else {
      if (!full_name)
	full_name=st=FileNameAbs(filename);
      else
	st=NULL;
      if (plain_text) {
	if (line)
	  "\dLK,\"%s,%04d\",A=\"PL:%s,%d\"\d",filename,line,full_name,line;
	else
	  "\dLK,\"%s\",A=\"PI:%s\"\d",filename,full_name;
      } else {
	if (line)
	  "\dLK,\"%s,%04d\",A=\"FL:%s,%d\"\d",filename,line,full_name,line;
	else
	  "\dLK,\"%s\",A=\"FI:%s\"\d",filename,full_name;
      }
      Free(st);
    }
  }
}

U0 PutDirLink(U8 *dirname,U8 *full_name=NULL)
{//Put \dLK,"DolDoc",A="FI:::/Doc/DolDocOverview.DD"\d dir macro to StdOut, \dLK,"DocPut",A="MN:DocPut"\d.
  U8 *st;
  if (!dirname) return;
  if(IsRaw) {
	"%s",dirname;
  } else {
    if (!full_name)
      full_name=st=DirNameAbs(dirname);
    else
      st=NULL;
    "\dMA,T=\"%s\",LM=\"Cd(\\\"%s\\\");Dir;\n\"\d",dirname,full_name;
    Free(st);
  }
}

I64 Dir(U8 *files_find_mask,Bool full)
{//List directory.
  CDirEntry *tmpde1=NULL,*tmpde2;
  U8 *st;
  CDateStruct ds;
  I64 csize=0xFFFF,c=0xFFFF,res=0;
  tmpde1=FilesFind(files_find_mask);
  if (!(st=DirCur))
    PrintErr("Invalid Drive\n");
  else {
    if (tmpde1) {
      Free(st);
      st=MAllocIdent(tmpde1->full_name);
      StrLastRem(st,"/");
      if (!st[2])
	StrCpy(st+2,"/");
//Find max columns
      tmpde2=tmpde1;
      while (tmpde2) {
	if (tmpde2->size>csize)
	  csize=tmpde2->size;
	if (tmpde2->clus>c)
	  c=tmpde2->clus;
	tmpde2=tmpde2->next;
      }
      csize=Bsr(csize)/4+1;
      c=Bsr(c)/4+1;

      "\dMA,T=\"Directory\",LM=\"PopUpCd;Dir;\n\"\d of %s\n",st;
      if (full)
	"__DATE__ __TIME__ %*ts %*ts\n",
	      csize,"SIZE",c,"BLK";
      else
	"DATE_ TIME_ %*ts\n",csize,"SIZE";
      while (tmpde1) {
	tmpde2=tmpde1->next;
	res++;
	  "%D %T %0*tX %0*tX ",tmpde1->datetime,tmpde1->datetime,
		csize,tmpde1->size,c,tmpde1->clus;
	if (IsDir(tmpde1->full_name))
	  PutDirLink(tmpde1->name,tmpde1->full_name);
	else
	  PutFileLink(tmpde1->name,tmpde1->full_name);
	"\n";
	DirEntryDel(tmpde1);
	tmpde1=tmpde2;
      }
    } else
      "No matching entries\n";
    Free(st);
  }
  return res;
}
