I64 GenerateChangeText(CDiffItem *diff,U8 *to=NULL) {
  I64 len=0,i,items=MSize(diff)/sizeof(CDiffItem);
  for(i=0;i!=items;i++) {
    switch (diff[i].type) {
      case DIFF_SAME:
        break;
      start:
        case DIFF_DELETE:
	  if(to) *to++='-';
	  len++;
          break;
        case DIFF_INSERT:
	  if(to) *to++='+';
	  len++;
	  break;
      end:
        if(to) StrCpy(to,diff[i].str);
        len+=StrLen(diff[i].str);
        if(to) to+=StrLen(diff[i].str);
        if(to) *to++='\n';
        break;
      default:
        goto fin;
    }
  }
fin:
  if(to) *to++=0;
  len++;
  return len;
}
U0 BackupFile(U8 *file,U8 *ntxt) {
  U8 *cnam1=ChrootFile(file),*fmted=NULL,*cnam2=NULL,*clone=StrNew(file),*otxt,
        *dpath,*dnam,*timestamp,*ftxt,*opath;
  I64 len,len2,len3;
  CDirEntry dent;
  CDiffItem *diff;
  if(FileFind(cnam1)) {
    otxt=FileRead(cnam1,&len);
    opath=StrLastOcc(clone,"/");
    cnam2=opath+1;
    FileFind(cnam1,&dent);
    timestamp=MStrPrint("%D_%T_",dent.datetime,dent.datetime);
    while(StrFirstOcc(timestamp,"/"))
      *StrFirstOcc(timestamp,"/")='-';
    while(StrFirstOcc(timestamp,":"))
      *StrFirstOcc(timestamp,":")='.';
    if(opath) {
      fmted=MStrPrint("%s/%s__%s.Z",clone,timestamp,opath+1);
      cnam2=ChrootFile(fmted,WIKI_BACKUP);
      Free(clone);
      clone=StrNew(cnam2);
      opath=StrLastOcc(clone,"/");
      *opath=0;
      DirMk(clone);
      *opath='/';
      dnam=clone;
    } else {
      fmted=MStrPrint("%s__%s.Z",timestamp,clone);
      cnam2=ChrootFile(fmted,WIKI_BACKUP);
      dnam=cnam2;
    }
    diff=Diff(ntxt,otxt);
    len3=GenerateChangeText(diff,NULL);
    len2=StrLen(timestamp);
    ftxt=MAlloc(len2+1+len+1+len3);
    StrCpy(ftxt,timestamp);
    MemCpy(ftxt+len2+1,otxt,len);
    GenerateChangeText(diff,ftxt+len+len2+1+1);
    FileWrite(dnam,ftxt,len2+1+len+1+len3);
    DiffItemsDel(diff);
    Free(otxt);
    Free(ftxt);
    Free(timestamp);
  }
  Free(cnam1);
  Free(cnam2);
  Free(fmted);
  Free(clone);
}
CDirEntry *GetFileRevisions(U8 *name) {
  CDirEntry *ents;
  U8 *clone=StrNew(name),*mask,*cnam=ChrootFile(name,WIKI_BACKUP),
    *l=StrLastOcc(cnam,"/"),*od=DirCur;
  *l=0;
  Cd(cnam);
  Cd(l+1);
  ents=FilesFind(mask=MStrPrint("*__%s.Z",l+1));
  Cd(od);
  Free(od);
  Free(cnam);
  Free(mask);
  Free(clone);
  return ents;
}
I64 DateSort(CDirEntry *a,CDirEntry *b) {
   return b->datetime-a->datetime;
}
U0 GenerateChangeLog(CDyadStream *s,U8 *name) {
  CDirEntry *ents=GetFileRevisions(name),*cur,**sorted,*root_abs;
  I64 i,len;
  U8 *link,*link2,*ftxt,*ptr,*ln,*h;
  WriteLn(s,NULL,"<H1>Revisions of \"%s\"</H1>",name);
  WriteLn(s,NULL,"<OL>");
  for(cur=ents;cur;cur=cur->next)
    len++;
  sorted=MAlloc(sizeof(CDirEntry*)*len);
  len=0;
  for(cur=ents;cur;cur=cur->next) {
    sorted[len++]=cur;
  }
  QSortI64(sorted,len,&DateSort);
  for(i=0;i!=len;i++) {
    root_abs=FileNameAbs(WIKI_BACKUP);
    link=EncodeQueryValue(sorted[i]->full_name+StrLen(root_abs));
    WriteLn(s,NULL,"<DIV CLASS=\"paper\"><H2><A HREF=\""WIKI_CHANGES_FOR_FILE"?FILE=%s\">%s</A></H2><PRE>",link,sorted[i]->full_name+StrLen(root_abs));
    Free(root_abs);
    ptr=ftxt=FileRead(sorted[i]->full_name);
    ptr+=StrLen(ptr)+1; //Skip Timestamp
    ptr+=StrLen(ptr)+1; //Skip file contents
    while(ln=ReadLine(ptr,&ptr)) {
      h=HTMLify(ln);
      WriteLn(s,NULL,"%s\n",h);
      Free(ln),Free(h); 
    }
    Free(ftxt);
    WriteLn(s,NULL,"</PRE></DIV>");
    Free(link);
  }
  WriteLn(s,NULL,"</OL>");
  DirEntryDel(ents);
}
U0 ChangeLogGet(CServer *s,CDyadStream *stream,CURL *url,CHTTPRequest *req) {
  U8 *f=GetQueryValue(url->query,"FILE"),*header=MStrPrint("Changelog for %s",f);
  WriteLn(stream,,"HTTP/1.1 200 OK");
  WriteLn(stream,,"Server: Bungis");
  WriteLn(stream,,"Content-Type: text/html");
  WriteLn(stream,,"");
  WikiHeader(stream,NULL,header,FALSE); 
  GenerateChangeLog(stream,f);
  WikiFooter(stream,NULL,url);
  Free(f);
  Free(header);
}
U0 RestoreGet(CServer *s,CDyadStream *stream,CURL *url,CHTTPRequest *req) {
  U8 *f=GetQueryValue(url->query,"FILE"),*lasts,*chrooted,*ftxt,*chrooted2,*oftxt;
  if(!f) return;
  WriteLn(stream,,"HTTP/1.1 200 OK");
  WriteLn(stream,,"Server: Bungis");
  WriteLn(stream,,"Content-Type: text/html");
  WriteLn(stream,,"");
  lasts=StrLastOcc(f,"/");
  if(!lasts) {
    err:
    R404(stream,NULL,f);
    Free(f);
    return;
  }
  chrooted=ChrootFile(f,WIKI_BACKUP);
  *lasts=0;
  if(!FileFind(chrooted)) {
    Free(chrooted);
    goto err;
  }
  chrooted2=ChrootFile(f,WIKI_ROOT);
  oftxt=ftxt=FileRead(chrooted);
  ftxt+=StrLen(ftxt)+1; //Skip timestamp
  FileWrite(chrooted2,ftxt,StrLen(ftxt));
  Free(oftxt);
  WikiHeader(stream,NULL,"File reverted");
  WriteLn(stream,NULL,"<H1>Reverted &quot%s&quot</H1>",chrooted);
  WikiFooter(stream,NULL,url);
  Free(f),Free(chrooted),Free(chrooted2);
}
U0 ChangeForFileGet(CServer *s,CDyadStream *stream,CURL *url,CHTTPRequest *req) {
  U8 *f=GetQueryValue(url->query,"FILE"),*header,*chrooted,*ftxt,*oftxt,*ln,*ptr,*h,*enc;
  if(!f) return ;
  chrooted=ChrootFile(f,WIKI_BACKUP);
  oftxt=ftxt=FileRead(chrooted);
  header=MStrPrint("Changes for %s(revision %s)",f,ftxt);
  WriteLn(stream,,"HTTP/1.1 200 OK");
  WriteLn(stream,,"Server: Bungis");
  WriteLn(stream,,"Content-Type: text/html");
  WriteLn(stream,,"");
  WikiHeader(stream,NULL,header,FALSE); 
  ftxt+=StrLen(ftxt)+1;
  WriteLn(stream,NULL,"<H1>Full-text</H1><P>");
  FmtText(ftxt,stream,url);
  ftxt+=StrLen(ftxt)+1;
  WriteLn(stream,NULL,"<H1>Changes</H1><PRE><CODE>");
  ptr=ftxt;
  while(ln=ReadLine(ptr,&ptr)) {
    h=HTMLify(ln);
    WriteLn(stream,NULL,"%s",h);
    Free(ln);
  }
  WriteLn(stream,NULL,"</CODE></PRE>");
  WriteLn(stream,NULL,"<A HREF=\""WIKI_RESTORE"?FILE=%s\" CLASS=\"form-group\">Restore</A>",enc=EncodeQueryValue(f));
  Free(enc);
  WikiFooter(stream,NULL,url);
  Free(f);
  Free(header);
  Free(chrooted);
  Free(oftxt);
}
