#define CDIR_FILENAME_LEN 1024
class CDirEntry
{
  CDirEntry *next,*parent,*prev;
 U16 attr;	//Unused in Cubit
 U8 name[CDIR_FILENAME_LEN];
 I64 size;	//In bytes
 I64 datetime; //In Unix time(not used yet)
};
//Checks a signle mask
static Bool __FileFitsMask(U8 *fn,U8 *mask) {
  I64 maskchr=StrLen(mask)-1;
  I64 fptr=StrLen(fn)-1;
  for(;maskchr>=0&&fptr>=0;) {
    switch(mask[maskchr]) {
      case '*':
      if(--maskchr<0) return TRUE;
      while(fn[fptr]!=mask[maskchr]) {
        if(--fptr<0) return FALSE;
      }
      break;
      case '?':
      maskchr--;
      fptr--;
      break;
      default:
      if(fn[fptr--]!=mask[maskchr--]) return FALSE;
    }
  }
  return TRUE;
}
static Bool __IsEscaped(U8 *str,U8 *at) {
    if(IsWindows) return FALSE;
  if(at-1>=str) {
    if(at[-1]=='\\') {
      return !__IsEscaped(str,at-1);
    }
  }
  return FALSE;
}
//From the TempleOS,Epic code cramming stuff into a single line(RIP Terry Davis).
U8 *FileExtDot(U8 *src) {
    U8 delim='/';
    if(IsWindows()) delim='\\';
  I64 ch;
  while(ch=*src++)
    if(ch=='.'&&*src!='/'&&*src!='.')
      return src-1;
  return NULL;
}
#define FUF_RECURSE (1<<0)
#define FUF_SINGLE (1<<1)
#define FUF_JUST_DIRS (1<<3)
#define FUF_JUST_DIRS (1<<4)
#define FUF_JUST_FILES (1<<5)
#define FUF_JUST_TXT (1<<6)
#define FUF_JUST_DD (1<<7)
#define FUF_JUST_SRC (1<<9)
static I64 __DirRecur(U8 *dir,U8 **retbuf,I64 max=1024) {
  U8 **ret=__Dir(dir);
  I64 found=0;
  I64 cnt=0;
  while(ret[cnt]) {
    if(0==StrCmp(ret[cnt],".")||0==StrCmp(ret[cnt],"..")) {
      Free(ret[cnt]);
      goto next;
    }
    if(IsDir(ret[cnt])) {
      U8 *buffer=MAlloc(StrLen(dir)+StrLen(ret[cnt])+1+1);
      I64 off=0;
      StrCpy(buffer+StrLen(buffer),dir);
      if(!IsWindows())
        StrCpy(buffer+StrLen(buffer),"/");
      else
        StrCpy(buffer+StrLen(buffer),"\\");
      StrCpy(buffer+StrLen(buffer),ret[cnt]);
      I64 found2=__DirRecur(buffer,retbuf,max);
      found+=found2;
      retbuf+=found2;
      max-=found2;
      Free(buffer);
    } else {
      if(max-->=0) {
        if(!IsWindows())
            U8 *fmted=MStrPrint("%s/%s",dir,ret[cnt]);
        else
            fmted=MStrPrint("%s\\%s",dir,ret[cnt]);
        found++,*retbuf++=fmted;
      }
    }
    Free(ret[cnt]);
    next:
    cnt++;
  }
  Free(ret);
  return found;
}
CDirEntry *FilesFind(U8 *mask,I64 flags=0) {
  mask=StrNew(mask);
  U8 delim_chr='/';
  if(IsWindows())
  delim_chr='\\';
  //Find last unescaped "/"
  I64 idx=StrLen(mask);
  if(StrFirstOcc(mask,";"))
    idx=StrFirstOcc(mask,";")-mask;
  while(--idx>=0) {
    if(mask[idx]==delim_chr) {
      if(!__IsEscaped(mask,mask+idx))
        goto found;
    }
  }
  found:
  U8 *file_masks_st=NULL;
  if(idx==-1)
    file_masks_st=mask;
  else
    file_masks_st=mask+idx;
  I64 fmask_cnt=1;
  U8 *semi=mask;
  while(semi=StrFirstOcc(semi,";")) {
    fmask_cnt++;
    semi++;
  }
  U8 **masks=MAlloc(fmask_cnt*sizeof(U8*));
  if(mask[idx]==delim_chr)
    semi=mask+idx+1;
  else
    semi=mask;
  I64 fmask_cnt2=fmask_cnt;
  while(--fmask_cnt>=0) {
    masks[fmask_cnt]=semi;
    if(StrLen(masks[fmask_cnt])==0) masks[fmask_cnt]="*";
    semi=StrFirstOcc(semi,";");
    if(semi) *semi++=0;
  }
  fmask_cnt=fmask_cnt2;
  U8 *root=NULL;
  if(idx==-1) {
    root=".";
  } else {
    mask[idx]=0;
    root=mask;
  }
  U8 **files=NULL;
  if(flags&FUF_RECURSE) {
    U8 *recurfs[1024];
    I64 found=__DirRecur(root,recurfs,1024);
    files=MAlloc((1+found)*sizeof(U8 *));
    files[found]=NULL;
    while(--found>=0) {
      /*
      //Remove prefix
      U8 *prefix_rem=StrNew(recurfs[found]+StrLen(root)+1); //+1 Removes delim("/" on linux)
      files[found]=prefix_rem;
      Free(recurfs[found]);
      */
    }
  } else {
    files=__Dir(root);
    U8 **origf=files;
    while(*files) {
      U8 *withpath=MStrPrint("%s%c%s",root,delim_chr,*files);
      Free(*files);
      *files=withpath;
      files++;
    }
    files=origf;
  }
  if(!files) return NULL;
  I64 fcnt=0;
  I64 keep_cnt=0;
  for(;files[fcnt];fcnt++) {
    if(flags&FUF_JUST_DIRS)
      if(!IsDir(files[fcnt])) {
        skip:
        files[fcnt]=NULL;
        goto next;
      }
    if(flags&FUF_JUST_FILES) {
      if(IsDir(files[fcnt]))
        goto skip;
    }
    U8 *ext=FileExtDot(files[fcnt]);
    if(flags&FUF_JUST_TXT) {
      if(!ext)
        goto skip;
      if(0!=StrCmp(ext,".TXT"))
        goto skip;
    }
    if(flags&FUF_JUST_DD) {
      if(!ext)
        goto skip;
      if(0!=StrCmp(ext,".DD"))
        goto skip;
    }
    if(flags&FUF_JUST_SRC) {
      if(!ext)
        goto skip;
      if(0!=StrCmp(ext,".HC")&&0!=StrCmp(ext,".HH"))
        goto skip;
    }
    I64 mi=fmask_cnt;
    Bool keep=TRUE;
    while(--mi>=0) {
      if(masks[mi][0]=='!'){
        if(__FileFitsMask(files[fcnt],masks[mi]))
          keep=FALSE;
      } if(__FileFitsMask(files[fcnt],masks[mi])) {
        keep&=TRUE;
      } else {
        keep=FALSE;
      }
    }
    if(!keep) files[fcnt]=NULL;
    else keep_cnt++;
    next:
  }
  U8 path[CDIR_FILENAME_LEN];
  StrCpy(path,root);
  path[StrLen(root)]=delim_chr;
  path[StrLen(root)+1]=0;
  I64 fnstart=StrLen(path);
  CDirEntry *prev=NULL;
  while(--fcnt>=0) {
    if(files[fcnt]) {
      CDirEntry *ent=MAlloc(sizeof(CDirEntry));
      StrCpy(path+fnstart,files[fcnt]);
      StrCpy(ent.name,files[fcnt]);
      CFile *f=FOpen(path,"rb",-1);
      if(f) {
        ent.size=FSize(f);
        FClose(f);
      }
      if(prev) {
        ent->prev=prev;
        prev->next=ent;
      }
      prev=ent;
    }
  }
  if(prev&&!(flags&FUF_SINGLE))
    while(prev->prev) prev=prev->prev;
  Free(mask);
  return prev;
}
U0 DirEntryFree(CDirEntry *ents) {
  if(ents->next)
    DirEntryFree(ents->next);
  Free(ents);
}
U0 Dir(U8 *path=".") {
  U8 delim='/';
  if(IsWindows) delim='\\';
  U8 *fpath=MStrPrint("%s%c*",path,delim);
  CDirEntry *ents=FilesFind(fpath);
  if(ents) {
    CDirEntry *orig=ents;
    while(ents) {
      "\t%s(%d bytes)\n",ents->name,ents->size;
      ents=ents->next;
    }
    DirEntryFree(orig);
  }
  Free(fpath);
}
U0 Zip(U8 *f,I64 flags=0) {
  Debugger;
  CDirEntry *ents=FilesFind(f,flags);
  loop:
  if(ents) {
    "%s\n",ents->name;
    I64 len;
    if(IsDir(ents->name)) goto skip;
    U8 *text=FileRead(ents->name,&len);
    CArcCompress *cmp=CompressBuf(text,len);
    U8 *dotz=MStrPrint("%s.Z",ents->name);
    FileWrite(dotz,cmp,cmp->compressed_size);
    Free(text);
    Free(cmp);
    skip:
    ents=ents->next;
    goto loop;
  }
}
U0 Unzip(U8 *f,I64 flags=0) {
  Debugger;
  CDirEntry *ents=FilesFind(f,flags);
  loop:
  if(ents) {
  "%s\n",ents->name;
    I64 len;
    if(IsDir(ents->name)) goto skip;
    CArcCompress *cmp=FileRead(ents->name,&len);
    I64 last=StrLen(ents->name);
    while(last>=0) if(ents->name[--last]=='.') {ents->name[last]=0;break;}
    U8 *text=ExpandBuf(cmp);
    FileWrite(ents->name,text,cmp->expanded_size);
    Free(text);
    Free(cmp);
    skip:
    ents=ents->next;
    goto loop;
  }
}
U0 Move(U8 *mask,U8 *to) {
  U8 delim='/';
  if(IsWindows) delim='\\';
  CDirEntry *ents=FilesFind(mask);
  if(ents) {
    CDirEntry *orig=ents;
    loop:
    I64 ptr=StrLen(ents->name);
    while(--ptr>=0) if(ents->name[ptr]==delim) {ents->name[ptr]=0; break;}
    U8 *fn=MStrPrint("%s%c%s",to,delim,ents->name+ptr+1);
    "%s\n",fn;
    __Move(ents->name,fn);
    Free(fn);
    if(ents=ents->next) goto loop;
    DirEntryFree(orig);
  }
}
