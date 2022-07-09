DyadInit;
class CURL {
	U8 hostname[STR_LEN];
	U8 abs_path[STR_LEN];
	U8 query[STR_LEN];
	I64 port;
};
class CHTTPRequest {
	#define HTTP_GET 1
	#define HTTP_POST 2
	
	I64 method;
	U8 content_type[STR_LEN];
	I64 content_length;
	U8 *data;
};
class CServer {
	CDyadStream *s_sock;
	U0(*get)(CServer*,CDyadStream*,CURL*,CHTTPRequest*);
	U0(*post)(CServer*,CDyadStream*,CURL*,CHTTPRequest*);
};
CURL *ParseURL(U8 *url) {
	CURL *ret=CAlloc(sizeof(CURL));
	U8 *r,*q,*p,chr;
	I64 i,l;
	if(!(url=StrFind("//",url)))
		goto fail;
	url+=2;
	r=StrFirstOcc(url,"/");
	q=StrFirstOcc(url,"?");
	p=StrFirstOcc(url,":");
	if(!p) p=StrLen(url)+url;
	if(!r) r=StrLen(url)+url;
	if(!q) q=StrLen(url)+url;
	if(p<r||p<q) {
		MemCpy(ret->hostname,url,p-url);
		url=p+1;
		ret->port=Str2I64(url,10,&url);
	} else {
		MemCpy(ret->hostname,url,MinI64(p,r)-url);
		url=MinI64(p,r)+1;
	}
	if(!*url) goto retn;
	if(r<q) {
		MemCpy(ret->abs_path,r+1,q-r-1);
	}
	if(*q) {
		l=StrLen(++q);
		for(i=0;i!=l;i++) {
			ret->query[i]=*q++;
		}
	}
	retn:
	return ret;
	fail:
	Free(ret);
	return NULL;
}

U0 DumpURL(CURL *url) {
	if(!url) return;
	TOSPrint("%s:%d/%s?%s\n",url->hostname,
	url->port,
	url->abs_path,
	url->query);
}
U8 *SkipWhitespace(U8 *s) {
	while(*s&&StrOcc(" \t\n",*s)) s++;
	return s;
}
U8 *Trim(U8 *buf) {
	if(StrFirstOcc(buf," \t\n"))
			*StrFirstOcc(buf," \t\n")=0;
	return SkipWhitespace(buf);
}
U0 ParseRequest(CServer *s,CDyadStream *stream,U8 *http) {
	U8 *http2,*t1,buf[STR_LEN],buf2[STR_LEN],*t2;
	CHTTPRequest req;
	MemSet(&req,0,sizeof(CHTTPRequest));
	CURL *url=NULL;
	if(!StrNCmp(http,"GET",3)) {
		req.method=HTTP_GET;
		get_like:
		http=SkipWhitespace(http+3);
		http2=StrFind("HTTP",http);
		MemCpy(buf,http,http2-http);
		buf[http2-http]=0;
		t2=Trim(buf);
		t1=MStrPrint("http://bungis.com:123/%s",t2);
		http2=StrFirstOcc(http2,"\n");
		url=ParseURL(t1);
		Free(t1);
		http=http2+1;
	}
	while(http&&http(U16*)[0]!='\x0d\n') {
		t1=buf,t2=buf2;
		http=StrScan(http,"%s:%s\x0d\n",&t1,&t2);
		if(!StrICmp(Trim(buf),"Content-Type")) {
			StrCpy(req.content_type,Trim(buf2));
		}
		if(!StrICmp(Trim(buf),"Content-Length")) {
			req.content_length=Str2I64(buf2);
		}
	}
	if(http) {
		http+=2;
		req.data=CAlloc(req.content_length+1);
		MemCpy(req.data,http,req.content_length);
	}
	switch(req.method) {
		case HTTP_GET:
		(*s->get)(s,stream,url,&req);
		break;
		case HTTP_POST:
		break;
	}
	Free(url);
}
U0 WriteLn(CDyadStream *s,I64 *len=NULL,U8 *fmt,...) {
	U8 *str=StrPrintJoin(NULL,fmt,argc,argv);
	if(len) {
		*len+=StrLen(str)+2;
		Free(str);
		return;
	}
	TOSPrint("%s\n",str);
	DyadWrite(s,str,StrLen(str));
	DyadWrite(s,"\x0d\n",2);
	Free(str);
}
U0 Write(CDyadStream *s,I64 *len=NULL,U8 *fmt,...) {
	U8 *str=StrPrintJoin(NULL,fmt,argc,argv);
	if(len) {
		*len+=StrLen(str);
		Free(str);
		return;
	}
	TOSPrint("%s\n",str);
	DyadWrite(s,str,StrLen(str));
	Free(str);
}
U0 R200(CDyadStream *s,I64 *ptr,U8 *msg) {
	U8 *ftxt=FileRead(msg),*ptr2,*next;
	WriteLn(s,ptr,"<html>");
	WriteLn(s,ptr,"	<body>");
	WriteLn(s,ptr,"		<H1>File %s found!!!</H1>",msg);
	WriteLn(s,ptr,"			<PRE>");
	for(ptr2=ftxt;*ptr2;) {
		if(StrFirstOcc(ptr2,"\n"))
			*StrFirstOcc(ptr2,"\n")=0,next=1+ptr2+StrLen(ptr2);
		else
			next=ptr2+StrLen(ptr2);
		WriteLn(s,ptr,"%s",ptr2);
		ptr2=next;
	}
	WriteLn(s,ptr,"			</PRE>");
	WriteLn(s,ptr,"	</body>");
	WriteLn(s,ptr,"</html>");
	Free(ftxt);
}
U0 R404(CDyadStream *s,I64 *ptr,U8 *msg) {
	WriteLn(s,ptr,"<html>");
	WriteLn(s,ptr,"	<body>");
	WriteLn(s,ptr,"		<H1>File %s not found</H1>",msg);
	WriteLn(s,ptr,"	</body>");
	WriteLn(s,ptr,"</html>");
}
U0 GenericGet(CServer *serv,CDyadStream *s,CURL *url,CHTTPRequest *req) {
	CDirEntry ent;
	I64 len=0;
	TOSPrint("Bungis\n");
	DumpURL(url);
	if(FileFind(url->abs_path,&ent,FUF_JUST_FILES)) {
		WriteLn(s,,"HTTP/1.1 200 OK");
		WriteLn(s,,"Server: Bungis");
		WriteLn(s,,"Content-Type: text/html");
		R200(s,&len,url->abs_path);
		WriteLn(s,,"Content-Length: %d",len);
		WriteLn(s,,"");
		R200(s,NULL,url->abs_path);
	} else {
		WriteLn(s,,"HTTP/1.1 404 Not Found");
		WriteLn(s,,"Server: Bungis");
		WriteLn(s,,"Content-Type: text/html");
		R404(s,&len,url->abs_path);
		WriteLn(s,,"Content-Length: %d",len);
		WriteLn(s,,"");
		R404(s,NULL,url->abs_path);
	}
	DyadEnd(s);
}
CServer *CreateServer(I64 port) {
	CServer *srv=CAlloc(sizeof(CServer));
	srv->s_sock=DyadNewStream;
	return srv;
}
class CConnection:CQue {
	#define CON_S_METHOD 1
	#define CON_S_FIELDS 2
	#define CON_S_BLANK_LINE 3
	I64 state;
	I64 b_ptr;
	CServer *server;
	CDyadStream *stream;
	U8 buffer[0x20000];
};
CConnection *connects=MAlloc(sizeof(CQue));
QueInit(connects);
I64 con_locked;
U0 ReadLine(CDyadStream *s,U8 *data,I64 len,CServer *serv) {
	while(Bts(&con_locked,0))
		PAUSE;
	CConnection *first,*con;
	first=connects;
	for(con=first->next;con!=first;con=con->next) {
		if(con->stream==s)
			goto found;
	}
	con=MAlloc(sizeof(CConnection));
	QueInit(con);
	QueIns(con,connects->last);
	con->state=CON_S_METHOD;
	con->server=serv;
	con->stream=s;
	found:
	//Pray that header is smaller than 128kb.
	if(len==0) {
		con->state=CON_S_BLANK_LINE;
		goto read;
	}
	if(con->state==CON_S_METHOD) {
		con->state=CON_S_FIELDS;
		goto read;
	} else if(con->state==CON_S_FIELDS) {
		goto read;
	}
	read:
	TOSPrint("%p:%s\n",s,data);
	StrCpy(con->buffer+con->b_ptr,data);
	con->b_ptr+=StrLen(data);
	StrCpy(con->buffer+con->b_ptr,"\x0d\n");
	con->b_ptr+=2;
	if(!len) {
		con->buffer[con->b_ptr]=0;
		TOSPrint("%s\n",con->buffer);
		ParseRequest(con->server,s,con->buffer);
		QueRem(con);
		Free(con);
	}
	LBtr(&con_locked,0);
}
U0 ListenCallback(CDyadStream *s,CServer *srv) {
	DyadSetReadCallback(s,&ReadLine,srv);
}
U0 Serve(CServer *srv) {
	DyadListen(srv->s_sock,8080);
	DyadSetOnListenCallback(srv->s_sock,&ListenCallback,srv);
	"Press a button to quit serving\n";
	while(!ScanKey) {
		Sleep(1);
		DyadUpdate;
	}
	DyadShutdown;
}
U8 *GetQueryValue(U8 *q,U8 *f) {
	I64 len=StrLen(q),bptr=0;
	U8 buf[1024],three[3];
	U8 *ptr=q;
	loop:
	while(*ptr&&!StrOcc("&=",*ptr)) {
		buf[bptr++]=*ptr++;
	}
	buf[bptr]=0;
	bptr=0;
	if(!StrCmp(f,buf)) {
		if(*ptr=='=') {
			ptr++;
			while(*ptr&&*ptr!='&') {
				if(*ptr=='%') {
					MemCpy(three,++ptr,2);
					three[2]=0;
					buf[bptr++]=Str2I64(three,16);
					ptr+=StrLen(three);
				} else
					buf[bptr++]=*ptr++;
			}
			buf[bptr++]=0;
			return StrNew(buf);
		} else 
			return StrNew("");
	} else if(*ptr) {
		if(StrFirstOcc(ptr,"&")) {
			ptr=1+StrFirstOcc(ptr,"&");
			goto loop;
		}
	}
	return NULL;
}
