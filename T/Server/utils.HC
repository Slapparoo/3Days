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
	I64 port;
	CDyadStream *s_sock;
	U0(*get)(CServer*,CDyadStream*,CURL*,CHTTPRequest*);
	U0(*post)(CServer*,CDyadStream*,CURL*,CHTTPRequest*,CHashTable *);
};
U8 *ReadLine(U8 *t,U8 **en) {
  U8 *r,*e;
  if(!*t) {
    if(en) *en=NULL;
	return NULL;
  }
  if(e=StrFirstOcc(t,"\n")) {
chug:
    r=MAlloc(e-t+1);
    MemCpy(r,t,e-t);
    r[e-t]=0;
  } else {
    e=StrLen(t)+t;
    goto chug;
  }
  if(en) {
    if(*e=='\n')
      *en=e+1;
    else
      *en=e;
  }
  return r;
}
U8 *StrSlice(U8 *ptr,U8 *en) {
  U8 *r=MAlloc(en-ptr+1);
  MemCpy(r,ptr,en-ptr);
  r[en-ptr]=0;
  return r;
}
Bool IsImage(U8 *name) {
  U8 *dot=FileExtDot(name);
  if(dot++) {
    if(!StrICmp(dot,"GIF")) return TRUE;
    if(!StrICmp(dot,"JPEG")) return TRUE;
    if(!StrICmp(dot,"JPG")) return TRUE;
    if(!StrICmp(dot,"PNG")) return TRUE;
    if(!StrICmp(dot,"BMP")) return TRUE;
  }
  return FALSE;
$WW,1$}
U8 *HTMLify(U8 *str) {
  U8 *ptr,*r=StrNew(""),*t,*t2;
  for(;*str;){
    ptr=StrFirstOcc(str,"<>;&\"");
    if(!ptr)
      ptr=str+StrLen(str);
    switch(*ptr) {
      start:
        t=MStrPrint("%s%s",r,t2=StrSlice(str,ptr));
        Free(r),Free(t2);
        r=t;
        case ';': //?
        t=MStrPrint("%s&#59;",r);
		break;
        case '<':
	  t=MStrPrint("%s&lt",r);
	  break;
        case '>':
	  t=MStrPrint("%s&gt",r);
	  break;
        case '&':
	  t=MStrPrint("%s&amp",r);
	  break;
        case '"':
	  t=MStrPrint("%s&quot",r);
	  break;
		case 0:
		t=StrNew(r);
		break;
      end:
        Free(r);
        if(*ptr) ptr++;
        r=t;
    }
    str=ptr;
  }
  return r;
}
U8 *ChrootFile(U8 *fn,U8 *prefix=WIKI_ROOT) {
  U8 *virt=MStrPrint("/%s/%s",prefix,fn);
  U8 *abs=FileNameAbs(virt),*abs2=FileNameAbs(prefix);
  if(!StrNCmp(abs,abs2,StrLen(abs2))) {
    Free(abs),Free(abs2);
    return virt;
  }
  Free(abs),Free(abs2),Free(virt);
  if(StrLastOcc(fn,"/")) {
    return MStrPrint("%s/%s",prefix,StrLastOcc(fn,"/")+1);
  }
  return MStrPrint("%s/%s",prefix,fn);
}
extern I64 FmtText(U8 *ftxt,CDyadStream *stream,CURL *url);
U8 *DotZify(U8 *t) {
  U8 *r=t;
  I64 len=StrLen(t);
  if(StrOcc(t,'.'))
    if(!(t[len-2]=='.'&&t[len-1]=='Z')) {
      r=MStrPrint("%s.Z",t);
      Free(t);
    }
  return r;
}