U0 EditGet(CServer *srv,CDyadStream *stream,CURL *url,CHTTPRequest *req) {
  U8 *page=GetQueryValue(url->query,"PAGENAME"),*link,*ptr,*ftxt,*t1,*ln;
  if(!page) {
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
  } else {
    WriteLn(stream,,"HTTP/1.1 200 OK");
    WriteLn(stream,,"Server: Bungis");
    WriteLn(stream,,"Content-Type: text/html");
    WriteLn(stream,,"");
    if(page) {
      link=MStrPrint(WIKI_ROOT"/%s",page);
    }
    WikiHeader(stream,NULL,link,FALSE);
    WriteLn(stream,NULL,"<FORM ACTION=\""WIKI_EDIT"?PAGENAME=%s\"CLASS=\"form-group\" METHOD=\"POST\">",page);
    WriteLn(stream,NULL,"<TEXTAREA NAME=\"PAGETEXT\" COLS=\"70\" ROWS=\"100\" TYPE=\"textarea\">");
    if(FileFind(link)) {
      ptr=ftxt=FileRead(link);
      while(ln=ReadLine(ptr,&ptr)) {
        if(StrFirstOcc(ln,"\x0d\n"))
	  *StrFirstOcc(ln,"\x0d\n")=0;
        t1=HTMLify(ln);
        WriteLn(stream,NULL,t1);
        Free(ln);Free(t1);
      }
      Free(ftxt);
    }
    Free(link);
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
  }
}
U0 EditPost(CServer *srv,CDyadStream *stream,CURL *url,CHTTPRequest *req,CHashTable *tab) {
  WriteLn(stream,,"HTTP/1.1 200 OK");
  WriteLn(stream,,"Server: Bungis");
  WriteLn(stream,,"Content-Type: text/html");
  WriteLn(stream,,"");
  CHashGeneric *hash=HashFind("PAGENAME",tab,-1),*hash2=HashFind("PAGETEXT",tab,-1),*hash3=HashFind("PREVIEW",tab,-1);
  U8 *link,*ftxt,*ptr,*t1,*ln,*plink;
  Bool first_run=TRUE;
  if(!hash&&!(t1=GetQueryValue(url->query,"PAGENAME"))) return;
  if(hash) {
    link=MStrPrint(WIKI_ROOT"/%s",hash->user_data1);
    plink=MStrPrint(WIKI_PREVIEW"/%s",hash->user_data1);
  } else {
    link=MStrPrint(WIKI_ROOT"/%s",t1);
    plink=MStrPrint(WIKI_PREVIEW"/%s",t1);
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
    if(hash3&&!StrICmp(hash3->user_data1,"PREVIEW")) {
		//Is preview
		FileWrite(plink,ftxt,t1-ftxt);
	} else {
		//TODO profanity check.
		FileWrite(link,ftxt,t1-ftxt);
	}
    Free(ftxt);
    EditGet(srv,stream,url,req);
    goto en;
  }
  if(FileFind(link)) {
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
      WriteLn(stream,NULL,t1);
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
