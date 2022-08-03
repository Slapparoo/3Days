#define WHF_EDIT (1<<0)
#define WHF_CHANGES (1<<1)

U8 *WikiHeader(CDyadStream *s,I64 *len_ptr,U8 *points_to,I64 flags=TRUE) {
	WriteLn(s,len_ptr,"<!DOCTYPE html>");
	WriteLn(s,len_ptr,"<HTML>");
	WriteLn(s,len_ptr,"<HEAD>");
		WriteLn(s,len_ptr,"<LINK REL=\"stylesheet\" HREF=\"/paper.css\">");
		WriteLn(s,len_ptr,"<TITLE>%s</TITLE>",points_to);
	WriteLn(s,len_ptr,"</HEAD>");
	WriteLn(s,len_ptr,"<BODY>");
	WriteLn(s,len_ptr,"<NAV CLASS=\"fixed border split-nav\">");
		WriteLn(s,len_ptr,"<DIV class=\"nav-brand\"><H3><A HREF=\"/WikiHome\">" WIKI_NAME "</A></H3></DIV>");
		WriteLn(s,len_ptr,"<DIV><UL CLASS=\"inline\">");
			WriteLn(s,len_ptr,"<LI><A HREF=\""WIKI_INDEX"\">INDEX</A></LI>");
			if(flags&WHF_EDIT)
				WriteLn(s,len_ptr,"<LI><A HREF=\""WIKI_EDIT"?PAGENAME=%s\">EDIT</A></LI>",points_to);
			if(flags&WHF_CHANGES)
				WriteLn(s,len_ptr,"<LI><A HREF=\""WIKI_CHANGES"?FILE=%s\">CHANGE-LOG</A></LI>",points_to);
			WriteLn(s,len_ptr,"<LI><A HREF=\""WIKI_SEARCH"\">SEARCH</A></LI>",points_to);
		WriteLn(s,len_ptr,"</UL></DIV>");
	WriteLn(s,len_ptr,"</NAV>");
	WriteLn(s,len_ptr,"<DIV CLASS=\"sm-12 md-12 col\"><DIV CLASS=\"paper\">");
}
U8 *WikiFooter(CDyadStream *s,I64 *len_ptr,CURL *url) {
	WriteLn(s,len_ptr,"</DIV></DIV></BODY></HTML>");
}
