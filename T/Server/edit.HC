U0 __EditGet(CServer *srv,CDyadStream *stream,CURL *url,CHTTPRequest *req,U8 *file) {
  U8 *page=GetQueryValue(url->query,"PAGENAME"),*ptr,*ftxt,*t1,*ln;
  U8 *profan;
  if(!page||!file) {
new_page:
    WriteLn(stream,,"HTTP/1.1 200 OK");
    WriteLn(stream,,"Server: Bungis");
    WriteLn(stream,,"Content-Type: text/html");
    WriteLn(stream,,"");
    WikiHeader(stream,NULL,"Make a New Page!!!",FALSE);
    WriteLn(stream,NULL,"<FORM ACTION=\""WIKI_EDIT"?PAGENAME=%s\"CLASS=\"form-group\" METHOD=\"POST\">",page);
    WriteLn(stream,NULL,"<LABEL FOR=\"PAGENAME\">New page name</LABEL>");
    WriteLn(stream,NULL,"<INPUT NAME=\"PAGENAME\" TYPE=\"text\">");
    WriteLn(stream,NULL,"<INPUT TYPE=\"submit\" VALUE=\"submit\">");
    WriteLn(stream,NULL,"</FORM>");
    WikiFooter(stream,NULL,url);
	return;
  } else {
    if(FileFind(file)&&IsDir(file)) {
		WriteLn(stream,,"HTTP/1.1 404 OK");
		WriteLn(stream,,"Server: Bungis");
		WriteLn(stream,,"Content-Type: text/html");
		WriteLn(stream,,"");    
		WikiHeader(stream,NULL,"Attempted Directory Edit.",FALSE);
		WriteLn(stream,NULL,"<H1>You attemped to edit a directory!!!</H1>");
		WikiFooter(stream,NULL,url);
		return;
    }
    WriteLn(stream,,"HTTP/1.1 200 OK");
    WriteLn(stream,,"Server: Bungis");
    WriteLn(stream,,"Content-Type: text/html");
    WriteLn(stream,,"");
    WikiHeader(stream,NULL,page,FALSE);
    WriteLn(stream,NULL,"<FORM ACTION=\""WIKI_EDIT"?PAGENAME=%s\"CLASS=\"form-group\" METHOD=\"POST\">",page);
    WriteLn(stream,NULL,"<TEXTAREA NAME=\"PAGETEXT\" COLS=\"70\" ROWS=\"100\" TYPE=\"textarea\">");
    if(FileFind(file)) {
      ptr=ftxt=FileRead(file);
      profan=HasProfanity(ftxt);
      while(ln=ReadLine(ptr,&ptr)) {
        if(StrFirstOcc(ln,"\x0d\n"))
	  *StrFirstOcc(ln,"\x0d\n")=0;
        t1=HTMLify(ln);
        WriteLn(stream,NULL,"%s",t1);
        Free(ln);Free(t1);
      }
      Free(ftxt);
    }
    WriteLn(stream,NULL,"</TEXTAREA>");
    if(profan) {
      WriteLn(stream,NULL,"<DIV CLASS=\"alert alert-danger\">Use of profranity \"%s\"</DIV>",profan);
    }
    Free(profan);
    WriteLn(stream,NULL,"<FIELDSET CLASS=\"form-group\">");
    WriteLn(stream,NULL,"<LEGEND>Preview or Publish?</LEGEND>");
    WriteLn(stream,NULL,"<LABEL CLASS=\"paper-radio\" FOR=\"PREVIEW\">");
    WriteLn(stream,NULL,"<INPUT TYPE=\"radio\" ID=\"PREVIEW\" NAME=\"PREVIEW\" VALUE=\"PREVIEW\" CHECKED=\"checked\">");
    WriteLn(stream,NULL,"<SPAN>Preview</SPAN></LABEL>");
    WriteLn(stream,NULL,"<LABEL CLASS=\"paper-radio\" FOR=\"PUBLISH\">");
    WriteLn(stream,NULL,"<INPUT TYPE=\"radio\" ID=\"PUBLISH\" NAME=\"PREVIEW\" VALUE=\"PUBLISH\">");
    WriteLn(stream,NULL,"<SPAN>Publish</SPAN></LABEL>");
    WriteLn(stream,NULL,"</FIELDSET>");
    WriteLn(stream,NULL,"<INPUT TYPE=\"submit\" VALUE=\"submit\">");
    WriteLn(stream,NULL,"</FORM>");
    if(FileFind(file)) {
      ftxt=FileRead(file);
      FmtText(ftxt,stream,url);
      Free(ftxt);
    }
    WikiFooter(stream,NULL,url);
  }
}
U0 EditGet(CServer *srv,CDyadStream *stream,CURL *url,CHTTPRequest *req) {
	U8 *page=GetQueryValue(url->query,"PAGENAME"),*link=NULL;
	if(page)
		link=ChrootFile(page);
	__EditGet(srv,stream,url,req,link);
	Free(page),Free(link);
}
U0 EditPost(CServer *srv,CDyadStream *stream,CURL *url,CHTTPRequest *req,CHashTable *tab) {
  CHashGeneric *hash=HashFind("PAGENAME",tab,-1),*hash2=HashFind("PAGETEXT",tab,-1),*hash3=HashFind("PREVIEW",tab,-1);
  U8 *link,*ftxt,*ptr,*t1,*ln,*plink,*t2,*profan;
  Bool first_run=TRUE;
  if(!hash&&!(t1=GetQueryValue(url->query,"PAGENAME"))) return;
  if(hash) {
    link=ChrootFile(hash->user_data1);
    plink=ChrootFile(hash->user_data1,WIKI_PREVIEW);
  } else {
    link=ChrootFile(t1);
    plink=ChrootFile(t1,WIKI_PREVIEW);
    Free(t1);
  }
loop:
  if(hash2) {
    ftxt=MAlloc(hash2->user_data0+1+1); //+1 for trailing newline
    ptr=hash2->user_data1;
    t1=ftxt;
    while(ln=ReadLine(ptr,&ptr)) {
      if(!first_run)
        *t1++='\n';
      first_run=FALSE;
      if(StrFirstOcc(ln,"\x0d\n"))
        *StrFirstOcc(ln,"\x0d\n")=0;
      StrCpy(t1,ln);
      t1+=StrLen(ln);
      Free(ln);
    }
    profan=HasProfanity(ftxt);
    if(hash3&&!StrICmp(hash3->user_data1,"PREVIEW")) {
//Is preview
      if(StrLastOcc(plink,"/")) {
        t2=StrNew(plink);
        *StrLastOcc(t2,"/")=0;
        DirMk(t2);
        Free(t2);
      }
      FileWrite(plink,ftxt,t1-ftxt);
    } else {
      if(StrLastOcc(link,"/")) {
        t2=StrNew(link);
        *StrLastOcc(t2,"/")=0;
        DirMk(t2);
        Free(t2);
      }
      if(!profan) {
	BackupFile(link+StrLen(WIKI_ROOT),ftxt);
        FileWrite(link,ftxt,t1-ftxt);
     }
    }
    Free(profan);
    Free(ftxt);
    if(hash3&&!StrICmp(hash3->user_data1,"PREVIEW"))
      __EditGet(srv,stream,url,req,plink);
    else
      __EditGet(srv,stream,url,req,link);
    goto en;
  }
  if(FileFind(link)) {
    WriteLn(stream,,"HTTP/1.1 200 OK");
    WriteLn(stream,,"Server: Bungis");
    WriteLn(stream,,"Content-Type: text/html");
    WriteLn(stream,,"");

    WikiHeader(stream,NULL,link,FALSE);
    WriteLn(stream,NULL,"<FORM ACTION=\""WIKI_EDIT"?PAGENAME=%s\"CLASS=\"form-group\" METHOD=\"POST\">",hash->user_data1);
    WriteLn(stream,NULL,"<LABEL FOR=\"PAGENAME\">%s</LABEL>",hash->user_data1);
    WriteLn(stream,NULL,"<INPUT NAME=\"PAGENAME\" TYPE=\"text\"><BR>");
    WriteLn(stream,NULL,"<TEXTAREA NAME=\"PAGETEXT\" COLS=\"70\" ROWS=\"100\" TYPE=\"textarea\">");
    ptr=ftxt=FileRead(link);
    while(ln=ReadLine(ptr,&ptr)) {
      if(StrFirstOcc(ln,"\x0d\n"))
        *StrFirstOcc(ln,"\x0d\n")=0;
      t1=HTMLify(ln);
      WriteLn(stream,NULL,"%s",t1);
      Free(ln);Free(t1);
    }

    Free(ftxt);
    WriteLn(stream,NULL,"</TEXTAREA>");
    WriteLn(stream,NULL,"<FIELDSET CLASS=\"form-group\">");
    WriteLn(stream,NULL,"<LEGEND>Preview or Publish?</LEGEND>");
    WriteLn(stream,NULL,"<LABEL CLASS=\"paper-radio\" FOR=\"PREVIEW\">");
    WriteLn(stream,NULL,"<INPUT TYPE=\"radio\" ID=\"PREVIEW\" NAME=\"PREVIEW\" VALUE=\"PREVIEW\" CHECKED=\"checked\">");
    WriteLn(stream,NULL,"<SPAN>Preview</SPAN></LABEL>");
    WriteLn(stream,NULL,"<LABEL CLASS=\"paper-radio\" FOR=\"PUBLISH\">");
    WriteLn(stream,NULL,"<INPUT TYPE=\"radio\" ID=\"PUBLISH\" NAME=\"PREVIEW\" VALUE=\"PUBLISH\">");
    WriteLn(stream,NULL,"<SPAN>Publish</SPAN></LABEL>");
    WriteLn(stream,NULL,"</FIELDSET>");
    WriteLn(stream,NULL,"<INPUT TYPE=\"submit\" VALUE=\"submit\">");
    WriteLn(stream,NULL,"</FORM>");
    if(FileFind(link)) {
      ftxt=FileRead(link);
      FmtText(ftxt,stream,url);
      Free(ftxt);
    }
    WikiFooter(stream,NULL,url);
  } else {
    FileWrite(link,"",0);
    goto loop;
  }
en:
  Free(plink);
  Free(link);
}
