I64 IndexSort(CDirEntry *a,CDirEntry *b) {
	if(IsDir(a->full_name)&&!IsDir(b->full_name)) 
		return -1;
	if(!IsDir(a->full_name)&&IsDir(b->full_name)) 
		return 1;
	return StrCmp(a->name,b->name);
}
U0 WIKI_FILE_UPLOAD_HTML(CDyadStream *stream,CURL *url,U8 *folder="/") {
	U8 *dest_folder=GetQueryValue(url->query,"FOLDER");
	if(!dest_folder) dest_folder=StrNew("/");
	WriteLn(stream,NULL,"<FORM ACTION=\""WIKI_UPLOAD"?FOLDER=%s\" CLASS=\"form-group\" ENCTYPE=\"multipart/form-data\" METHOD=\"POST\">",folder);
	WriteLn(stream,NULL,"<INPUT TYPE=\"file\" NAME=\"UPLOAD\" ID=\"UPLOAD\">");
	WriteLn(stream,NULL,"<INPUT TYPE=\"submit\" NAME=\"SUBMIT\" ID=\"SUBMIT\">");
	WriteLn(stream,NULL,"</FORM>");
	Free(dest_folder);
}
U0 UploadGet(CServer *srv,CDyadStream *stream,CURL *url,CHTTPRequest *req) {
	WriteLn(stream,,"HTTP/1.1 200 OK");
	WriteLn(stream,,"Server: Bungis");
	WriteLn(stream,,"Content-Type: text/html");
	WriteLn(stream,,"");
	WikiHeader(stream,NULL,"<H1>Upload a file here:</H1>",FALSE);
	WIKI_FILE_UPLOAD_HTML(stream,url);
	WikiFooter(stream,NULL,url);
}
U0 UploadPost(CServer *srv,CDyadStream *stream,CURL *url,CHTTPRequest *req,CHashTable *tab) {
	CHashGeneric *hash=HashFind("UPLOAD",tab,-1);
	U8 *blurb,*where;
	if(hash) {
		WriteLn(stream,,"HTTP/1.1 200 OK");
		WriteLn(stream,,"Server: Bungis");
		WriteLn(stream,,"Content-Type: text/html");
		WriteLn(stream,,"");
		where=DotZify(ChrootFile(hash->user_data2));
		FileWrite(where,hash->user_data1,hash->user_data0);
		blurb=MStrPrint("File %s uploaded.",hash->user_data2);
		WikiHeader(stream,NULL,blurb,FALSE);
		Free(blurb);
		Free(where);
		WriteLn(stream,NULL,"<H1>File \"%s\" uploaded.</H1>",hash->user_data2);
		WriteLn(stream,NULL,"<P>Size is %d byte(s).</P>",hash->user_data0);
		WikiFooter(stream,NULL,url);
	} else {
		WriteLn(stream,,"HTTP/1.1 200 OK");
		WriteLn(stream,,"Server: Bungis");
		WriteLn(stream,,"Content-Type: text/html");
		WriteLn(stream,,"");
		WikiHeader(stream,NULL,"<H1>Upload a file here:</H1>",FALSE);
		WIKI_FILE_UPLOAD_HTML(stream,url,"/");
		WikiFooter(stream,NULL,url);
	}
}
U0 IndexGet(CServer *srv,CDyadStream *stream,CURL *url,CHTTPRequest *req) {
	U8 *index_link=MStrPrint("INDEX:/%s",url->abs_path);
	U8 *dir="/",*rel_dir="/",*chrooted;
	CDirEntry *ents,*cent,**sorted;
	I64 ent_cnt,i;
	if(!StrNCmp(WIKI_INDEX,url->abs_path,StrLen(WIKI_INDEX)));
		rel_dir=dir=url->abs_path+StrLen(WIKI_INDEX);
	chrooted=ChrootFile(dir);
	dir=MStrPrint("%s/*",chrooted);
	Free(chrooted);
	WriteLn(stream,,"HTTP/1.1 200 OK");
	WriteLn(stream,,"Server: Bungis");
	WriteLn(stream,,"Content-Type: text/html");
	WriteLn(stream,,"");
	WikiHeader(stream,NULL,index_link,FALSE);
	ents=FilesFind(dir);
	for(cent=ents;cent;cent=cent->next)
		ent_cnt++;
	sorted=MAlloc(sizeof(CDirEntry*)*ent_cnt);
	for(cent=ents,ent_cnt=0;cent;cent=cent->next)
		sorted[ent_cnt++]=cent;
	QSortI64(sorted,ent_cnt,&IndexSort);
	WriteLn(stream,NULL,"<H1>Upload a new file to %s</H1>",rel_dir);
	WIKI_FILE_UPLOAD_HTML(stream,url,"/");
	WriteLn(stream,NULL,"<H1>Make a new page</H1>");
        WriteLn(stream,NULL,"<FORM ACTION=\""WIKI_EDIT"\" CLASS=\"form-group>\">");
	WriteLn(stream,NULL,"<LABELFOR=\"PAGENAME\">New Page:</LABEL>");
	WriteLn(stream,NULL,"<INPUT TYPE=\"TEXT\" NAME=\"PAGENAME\" ID=\"PAGENAME\">");
	WriteLn(stream,NULL,"</FORM>");
	WriteLn(stream,NULL,"<H1>Contents of %s</H1>",rel_dir);
	WriteLn(stream,NULL,"<UL>");
	for(i=0;i!=ent_cnt;i++) {
		if(!StrCmp(".",sorted[i]->name)||!StrCmp("..",sorted[i]->name))
			goto next;
		if(IsDir(sorted[i]->full_name)) {
			WriteLn(stream,NULL,"<LI><A HREF=\""WIKI_INDEX"%s/%s\">%s</A><SPAN CLASS=\"badge secondary\">Folder</SPAN></LI>",rel_dir,sorted[i]->name,sorted[i]->name);
		} else {
			WriteLn(stream,NULL,"<LI><A HREF=\"%s/%s\">%s</A>(Last edit at %D-%T)</LI>",rel_dir,sorted[i]->name,sorted[i]->name,sorted[i]->datetime,sorted[i]->datetime);
		}
		next:
	}
	WriteLn(stream,NULL,"</UL>");
	DirEntryDel(ents);
	WikiFooter(stream,NULL,url);
	Free(index_link);
	Free(dir);
	Free(sorted);
}
