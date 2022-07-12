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
};
class CServer {
	CDyadStream *s_sock;
	U0(*get)(CServer*,CDyadStream*,CURL*,CHTTPRequest*);
	U0(*post)(CServer*,CDyadStream*,CURL*,CHTTPRequest*,CHashTable *);
};
class CConnection:CQue {
	//These occupy .u8[0]
	#define CON_S_METHOD 1
	#define CON_S_FIELDS 2
	#define CON_S_BLANK_LINE 3
	#define CON_S_RAW_DATA 4
	//These are flags
	#define CONf_AWAIT_BOUNDARY 8
	#define CONf_URL_ENCODED 9
	#define CONf_DATA 10
	I64 state,method;
	I64 b_ptr;
	CServer *server;
	CDyadStream *stream;
	CHashTable *post_data;
	I64 field_ptr,field_length;
	U8 *fbuffer;
	CURL *url;
	U8 boundary[STR_LEN];
	U8 field[STR_LEN];
	U8 name[STR_LEN];
	U8 _fbuffer[0x20000];
	U8 buffer[0x10000];
};
U8 *GetQueryValue(U8 *q,U8 *f) {
	I64 len=StrLen(q),bptr=0;
	U8 *buf,three[3],name[STR_LEN];
	U8 *ptr=q,*en;
	loop:
	buf=name;
	while(*ptr&&!StrOcc("&=",*ptr)) {
		buf[bptr++]=*ptr++;
	}
	buf[bptr]=0;
	bptr=0;
	if(!StrCmp(f,buf)) {
		if(*ptr=='=') {
			en=StrFirstOcc(ptr,"&");
			if(!en) en=ptr+StrLen(ptr);
			buf=CAlloc(en-ptr+1);
			ptr++;
			while(*ptr&&*ptr!='&') {
				if(*ptr=='%') {
					MemCpy(three,++ptr,2);
					three[2]=0;
					buf[bptr++]=Str2I64(three,16);
					ptr+=StrLen(three);
				} else if(*ptr=='+') {
					buf[bptr++]=' ';
					ptr++;
				} else
					buf[bptr++]=*ptr++;
			}
			buf[bptr++]=0;
			return buf;
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
}
U8 *SkipWhitespace(U8 *s) {
	while(*s&&StrOcc(" \t\n",*s)) s++;
	return s;
}
U8 *Trim(U8 *buf) {
	if(StrFirstOcc(buf," \t\n\x0d"))
			*StrFirstOcc(buf," \t\n\x0d")=0;
	return SkipWhitespace(buf);
}
U0 ParseRequest(CDyadStream *stream,CConnection *con) {
	U8 *http2,*t1,buf[STR_LEN],buf2[STR_LEN],*t2,*ptr,*optr;
	CHashGeneric *hash;
	CHTTPRequest req;
	I64 len;
	MemSet(&req,0,sizeof(CHTTPRequest));
	req.method=con->method;
	CURL *url=con->url;
	switch(con->method) {
		case HTTP_GET:
		(*con->server->get)(con->server,stream,url,&req);
		DyadEnd(stream);
		break;
		case HTTP_POST:
		(*con->server->post)(con->server,stream,url,&req,con->post_data);
		DyadWrite(stream,"\x0d\n",2);
		DyadEnd(stream);
		break;
	}
}
U0 WriteLn(CDyadStream *s,I64 *len=NULL,U8 *fmt,...) {
	U8 *str=StrPrintJoin(NULL,fmt,argc,argv);
	TOSPrint("D%p:%s\n",s,str);
	if(len) {
		*len+=StrLen(str)+2;
		Free(str);
		return;
	}
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
}
CServer *CreateServer(I64 port) {
	CServer *srv=CAlloc(sizeof(CServer));
	srv->s_sock=DyadNewStream;
	return srv;
}
CConnection *connects=MAlloc(sizeof(CQue));
QueInit(connects);
I64 con_locked;
U0 ReadLine(CDyadStream *s,U8 *data,I64 len,CServer *serv) {
  TOSPrint("%p,%s\n",s,data);
  while(Bts(&con_locked,0))
    PAUSE;
  CConnection *first,*con;
  CHashGeneric *hash;
  U8 buf[STR_LEN],buf2[STR_LEN];
  U8 *fb,*http2,*t1,*t2,*odata;
  first=connects;
  for(con=first->next;con!=first;con=con->next) {
    if(con->stream==s)
      goto found;
  }
  con=CAlloc(sizeof(CConnection));
  QueInit(con);
  QueIns(con,connects->last);
  con->server=serv;
  con->stream=s;
  con->state.u8[0]=CON_S_METHOD;
  con->post_data=HashTableNew(0x10);
  con->fbuffer=con->_fbuffer;
found:
  if(len==2) con->state.u8[0]=CON_S_BLANK_LINE;
//Pray that header is smaller than 128kb.
  if(con->state.u8[0]==CON_S_METHOD&&!StrNCmp(data,"GET",3)) {
    con->method.u8[0]=HTTP_GET;
    t1=MStrPrint("http://potato.com/%s",Trim(SkipWhitespace(data+3)));
    con->url=ParseURL(t1);
    Free(t1);
  } else if(con->state.u8[0]==CON_S_METHOD&&!StrNCmp(data,"POST",4)) {
    con->method.u8[0]=HTTP_POST;
    t1=MStrPrint("http://potato.com/%s",Trim(SkipWhitespace(data+4)));
    con->url=ParseURL(t1);
    Free(t1);
  } else if(!StrNCmp(data,"Content-Length",StrLen("Content-Length"))) {
    data=SkipWhitespace(StrFirstOcc(data,":")+1);
    con->fbuffer=MAlloc(Str2I64(data));
  }
  if(con->state.u8[0]==CON_S_BLANK_LINE&&len>2&&Bt(&con->state,CONf_URL_ENCODED)) {
    TOSPrint("HERE\n");
    odata=data;
qloop:
    t2=StrFirstOcc(data,"=");
    if(!t2) t2=data+StrLen(data);
    MemCpy(buf,data,t2-data);
    buf[t2-data]=0;
    data=t2;
    t1=GetQueryValue(odata,buf);
    hash=CAlloc(sizeof(CHashGeneric)+StrLen(t1)+1);
    hash->str=StrNew(buf);
    hash->user_data0=StrLen(t1);
    hash->user_data1=hash+1;
    StrCpy(hash->user_data1,t1);
    hash->type=HTT_DICT_WORD;
    HashAdd(hash,con->post_data);
    Free(t1);
    if(*data&&StrFirstOcc(data,"&")) {
      data=StrFirstOcc(data,"&")+1;
      goto qloop;
    }
    Btr(&con->state,CONf_URL_ENCODED);
    LBtr(&con_locked,0);
    goto process;
  } else if(con->method==HTTP_POST&&con->state.u8[0]!=CON_S_BLANK_LINE&&StrOcc(data,':')) {
    t1=buf,t2=buf2;
    data=StrScan(data,"%s:%s",&t1,&t2);
    if(!StrICmp(Trim(buf),"Content-Type")) {
      if(StrIMatch("multipart/form-data",buf2)) {
        t2=con->boundary;
        StrScan(StrFirstOcc(buf2,";")+1,"%sboundary=%s\x0d\n",&t1,&t2);
        LBts(&con->state,CONf_AWAIT_BOUNDARY);
        TOSPrint("BOUNDARY:%s\n",con->boundary);
      } else if(StrIMatch("urlencoded",buf2)) {
	TOSPrint("HERE\n");
	LBts(&con->state,CONf_URL_ENCODED);
	}
    }
  }
  if(Bt(&con->state,CONf_AWAIT_BOUNDARY)) {
    TOSPrint("AWIAT BOUNDARY\n");
    TOSPrint("%d,%d\n",con->state.u8[0]!=CON_S_BLANK_LINE,!StrNCmp(data,"Content-Disposition",StrLen("Content-Disposition")));
    t1=buf;
    if(!StrNCmp(data,"--",2)&&!StrCmp(Trim(data+2),con->boundary)) {
      TOSPrint("FOUND\n");
      if(con->field[0]) {
        hash=CAlloc(1+con->field_ptr+1+StrLen(con->name)+sizeof(CHashGeneric));
        hash->str=StrNew(con->field);
        if(con->field_length==-1)
	  hash->user_data0=con->field_ptr-2;
        else
	  hash->user_data0=con->field_length;
        hash->user_data1=hash+1;
        hash->user_data2=hash->user_data1+con->field_ptr+1;
        StrCpy(hash->user_data2,con->name);
        MemCpy(hash->user_data1,con->fbuffer,con->field_ptr);
        hash->type=HTT_DICT_WORD;
        HashAdd(hash,con->post_data);
      }
      con->field[0]=0;
      con->field_ptr=0;
      con->field_length=-1;
      con->name[0]=0;
      Btr(&con->state,CONf_DATA);
    } else if(!StrNCmp(data,"--",2) && !StrNCmp(data+2,con->boundary,StrLen(con->boundary))
	  &&!StrCmp(Trim(data+2+StrLen(con->boundary)),"--")
    ) {
      LBtr(&con_locked,0);
      if(con->field[0]) {
        hash=CAlloc(1+con->field_ptr+1+StrLen(con->name)+sizeof(CHashGeneric));
        hash->str=StrNew(con->field);
        if(con->field_length==-1)
	  hash->user_data0=con->field_ptr-2;
        else
	  hash->user_data0=con->field_length;
        hash->user_data1=hash+1;
        hash->user_data2=hash->user_data1+con->field_ptr+1;
        StrCpy(hash->user_data2,con->name);
        MemCpy(hash->user_data1,con->fbuffer,con->field_ptr);
        hash->type=HTT_DICT_WORD;
        HashAdd(hash,con->post_data);
      }
      goto process;
    } else if(!Bt(&con->state,CONf_DATA)&&!StrNCmp(data,"Content-Length",StrLen("Content-Length"))) {
      con->field_length=Str2I64(SkipWhitespace(StrFirstOcc(data,":")+1));
    } else if(!Bt(&con->state,CONf_DATA)&&!StrNCmp(data,"Content-Disposition",StrLen("Content-Disposition"))) {
      data=SkipWhitespace(StrFirstOcc(data,":")+1);
      if(!StrNCmp(data,"form-data",StrLen("form-data"))) {
loop:
        if(data) {
	  data=SkipWhitespace(data);
	  if(!StrNCmp(data,"name",StrLen("name"))) {
	    t2=con->field;
	    data=StrScan(StrFirstOcc(data,"\""),"\"%s\"",&t2);
	    TOSPrint("FIELD:%s\n",con->field);
	  } else if(!StrNCmp(data,"filename",StrLen("filename"))) {
	    t2=con->name;
	    data=StrScan(StrFirstOcc(data,"\""),"\"%s\"",&t2);
	    TOSPrint("FILE_NAME:%s\n",con->name);
	  }
	  if(data) data=StrFirstOcc(data,";");
	  if(data) {
	    data++;
	    goto loop;
	  }
	  LBtr(&con_locked,0);
	  return;
        }
      } 
    } else if(!Bt(&con->state,CONf_DATA)&&len==2) { //+2 for \x0d\n
      Bts(&con->state,CONf_DATA);
    } else if(Bt(&con->state,CONf_DATA)) {
      MemCpy(con->fbuffer+con->field_ptr,data,len);
      con->field_ptr+=len;
      goto en;
    }	
  }
read:
  StrCpy(con->buffer+con->b_ptr,data);
  con->b_ptr+=StrLen(data);
en:
  LBtr(&con_locked,0);
//2 is \x0d\n
  if(len==2&&!Bt(&con->state,CONf_AWAIT_BOUNDARY)&&!Bt(&con->state,CONf_URL_ENCODED))
    goto process;
  return;
process:
  TOSPrint("PROCESS\n");
  con->buffer[con->b_ptr]=0;
  ParseRequest(s,con);
  if(con->post_data)
    HashTableDel(con->post_data);
  QueRem(con);
  Free(con->url);
  if(con->fbuffer!=con->_fbuffer)
    Free(con->fbuffer);
  Free(con);
}
U0 ListenCallback(CDyadStream *s,CServer *srv) {
	DyadSetReadCallback(s,&ReadLine,srv);
}
U0 Serve(CServer *srv) {
	DyadListen(srv->s_sock,8080);
	DyadSetOnListenCallback(srv->s_sock,&ListenCallback,srv);
	"Press a button to quit serving\n";
	while(!ScanKey) {
		Sleep(10);
		DyadUpdate;
	}
	DyadShutdown;
}
