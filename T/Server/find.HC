U0 FindGet(CServer *srv,CDyadStream *stream,CURL *url,CHTTPRequest *req) {
	CDirEntry *ents,*c_ent;
	TOSPrint("bingis,%s\n",url->query);
	U8 *ftxt,*ptr,*ln_txt,*search_for=GetQueryValue(url->query,"s"),*h,*title;
	TOSPrint("bingis2\n");
	title=MStrPrint("FIND:%s",search_for);
	TOSPrint("bingis2\n");
	I64 len=0,*len_ptr=&len,mat;
	ents=FilesFind(WIKI_ROOT"/*.WIKI",FUF_RECURSE);
	loop:
	WikiHeader(stream,len_ptr,title);
	WriteLn(stream,len_ptr,"<FORM CLASS=\"form-group\" ACTION=\""WIKI_SEARCH"\">");
	WriteLn(stream,len_ptr,"<LABEL FOR=\"s\">SEARCH:</LABEL>");
	WriteLn(stream,len_ptr,"<INPUT NAME=\"s\" ID=\"s\">");
	WriteLn(stream,len_ptr,"<INPUT TYPE=\"Submit\" VALUE=\"Submit\">");
	WriteLn(stream,len_ptr,"</FORM>");
	if(search_for) {
		for(c_ent=ents;c_ent;c_ent=c_ent->next) {
			WriteLn(stream,len_ptr,"<ARITCLE CLASS=\"article\">",c_ent->name);
			WriteLn(stream,len_ptr,"<H4 CLASS=\"article-title\"><A HREF=\"%s\">%s</A></H4>",c_ent->name,c_ent->name);
			WriteLn(stream,len_ptr,"<P CLASS=\"article-meta\">Last edited at %D(%T).</P>",c_ent->datetime,c_ent->datetime);
			WriteLn(stream,len_ptr,"<P>");
			ptr=ftxt=FileRead(c_ent->full_name);
			if(StrIMatch(search_for,ftxt)) {
				for(mat=0;mat<5&&(ln_txt=ReadLine(ptr,&ptr));) {
					if(StrIMatch(search_for,ln_txt)) {
						mat++;
						h=HTMLify(ln_txt);
						WriteLn(stream,len_ptr,h);
						Free(h);
					}
					Free(ln_txt);
				}
			}
			WriteLn(stream,len_ptr,"</P>");
			WriteLn(stream,len_ptr,"</ARITCLE>");
			Free(ftxt);
		}
	}
	TOSPrint("bingis3\n");
	WikiFooter(stream,len_ptr,url);
	if(len_ptr) {
		WriteLn(stream,,"HTTP/1.1 200 OK");
   	    WriteLn(stream,,"Server: Bungis");
	    WriteLn(stream,,"Content-Type: text/html");
	    WriteLn(stream,,"Content-Length: %d",len);
	    WriteLn(stream,,"");
	    len_ptr=NULL;
	    goto loop;
	}
	Free(title);
	DirTreeDel(ents);
}
