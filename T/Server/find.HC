I64 FindResultSort(I64 a,I64 b) {
	return b(CDirEntry*)->user_data-a(CDirEntry*)->user_data;
}
U0 FindGet(CServer *srv,CDyadStream *stream,CURL *url,CHTTPRequest *req) {
	CDirEntry *ents,*c_ent,**sorted;
	U8 *ftxt,*ptr,*ln_txt,*search_for=GetQueryValue(url->query,"s"),*h,*title,*dot;
	title=MStrPrint("FIND:%s",search_for);
	I64 len=0,*len_ptr=&len,mat,fcnt,f;
	CFifoI64 *matches=FifoI64New(0x80);
	ents=FilesFind(WIKI_ROOT"/*",FUF_RECURSE);
	loop:
	WikiHeader(stream,len_ptr,title);
	WriteLn(stream,len_ptr,"<FORM CLASS=\"form-group\" ACTION=\""WIKI_SEARCH"\">");
	WriteLn(stream,len_ptr,"<LABEL FOR=\"s\">SEARCH:</LABEL>");
	WriteLn(stream,len_ptr,"<INPUT NAME=\"s\" ID=\"s\">");
	WriteLn(stream,len_ptr,"<INPUT TYPE=\"Submit\" VALUE=\"Submit\">");
	WriteLn(stream,len_ptr,"</FORM>");
	if(search_for) {
		for(c_ent=ents;c_ent;c_ent=c_ent->next) {
			if(c_ent==c_ent->next)
				break;
			if(!c_ent->full_name)
				goto next;
			if(!(dot=FileExtDot(c_ent->name))) {
				;
			} else if(!StrICmp(dot,".WIKI")||!StrICmp(dot,".Z")||!StrICmp(dot,".WIKI.Z")) {
                        } else
				goto next;
			Yield;
			ptr=ftxt=FileRead(c_ent->full_name);
			if(StrIMatch(search_for,ftxt)) {
				FifoI64Ins(matches,c_ent);
				for(mat=0;ln_txt=ReadLine(ptr,&ptr);) {
					if(StrIMatch(search_for,ln_txt))
						c_ent->user_data=++mat;
					Free(ln_txt);
				}
			}
			Free(ftxt);
			next:;
		}
	}
	sorted=MAlloc(FifoI64Cnt(matches)*sizeof(CDirEntry*));
	fcnt=FifoI64Cnt(matches);
	while(mat=FifoI64Cnt(matches)) {
		FifoI64Rem(matches,&sorted[mat-1]);
	}
	QSortI64(sorted,fcnt,&FindResultSort);
	for(f=0;f!=fcnt;f++) {
		c_ent=sorted[f];
		WriteLn(stream,len_ptr,"<ARITCLE CLASS=\"article\">",c_ent->name);
		WriteLn(stream,len_ptr,"<H4 CLASS=\"article-title\"><A HREF=\"%s\">%s</A></H4>",c_ent->name,c_ent->name);
		WriteLn(stream,len_ptr,"<P CLASS=\"article-meta\">Last edited at %D(%T).</P>",c_ent->datetime,c_ent->datetime);
		WriteLn(stream,len_ptr,"<P>");
		ptr=ftxt=FileRead(c_ent->full_name);
		for(mat=0;(ln_txt=ReadLine(ptr,&ptr));) {
			if(mat<5&&StrIMatch(search_for,ln_txt)) {
				h=HTMLify(ln_txt);
				WriteLn(stream,len_ptr,"%s<BR>",h);
				Free(h);
				mat++;
			}
			Free(ln_txt);
		}
		Free(ftxt);
		WriteLn(stream,len_ptr,"</P>");
		WriteLn(stream,len_ptr,"</ARITCLE>");
			
	}
	WikiFooter(stream,len_ptr,url);
	if(len_ptr) {
		WriteLn(stream,,"HTTP/1.1 200 OK");
   	    WriteLn(stream,,"Server: Bungis");
	    WriteLn(stream,,"Content-Type: text/html");
	    WriteLn(stream,,"");
	    len_ptr=NULL;
	    goto loop;
	}
	Free(title);
	DirTreeDel(ents);
	FifoI64Del(matches);
}
