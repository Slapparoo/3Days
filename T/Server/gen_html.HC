#include "http.HC"
#define WIKI_ROOT "/Wiki/"
#define WIKI_NAME "TempleOS Tiki"
#define WIKI_HOME "/"
#define WIKI_INDEX "/INDEX"
#define WIKI_SEARCH "/FIND"
#include "header_footer.HC";
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
  TOSPrint("RL:%s\n",r);
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
    if(!StrICmp(dot,"JPG")) return TRUE;
    if(!StrICmp(dot,"PNG")) return TRUE;
    if(!StrICmp(dot,"BMP")) return TRUE;
  }
  return FALSE;
}
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
        ptr++;
        r=t;
    }
    str=ptr;
  }
  return r;
}
#define WGf_CODE 1
#define WGf_PARA 2
#define WGf_LIST 3
#define WGf_TABLE 4
U0 FileGet(CServer *srv,CDyadStream*stream,CURL *url,CHTTPRequest *req) {
  U8 *link=MStrPrint(WIKI_ROOT"/%s",url->abs_path);
  U8 *f,*mime,*dot=FileExtDot(link);
  I64 len;
  if(dot&&FileFind(link)){
    if(!StrICmp(dot,".GIF")) mime="image/gif";
    else if(!StrICmp(dot,".JPEG")) mime="image/jpeg";
    else if(!StrICmp(dot,".JPG")) mime="image/jpg";
    else if(!StrICmp(dot,".PNG"))  mime="image/png";
    else if(!StrICmp(dot,".BMP"))  mime="image/bmp";
    else if(!StrICmp(dot,".CSS"))  mime="text/css";
    else if(!StrICmp(dot,".MIN.CSS"))  mime="text/css";
    else mime="text/plain";
    f=FileRead(link,&len);
    WriteLn(stream,,"HTTP/1.1 200 OK");
    WriteLn(stream,,"Server: Bungis");
    WriteLn(stream,,"Content-Type: %s",mime);
    WriteLn(stream,,"");
    DyadWrite(stream,f,len);
    Free(f);
    DyadEnd(stream);
  } else {
    R404(stream,&len,url->abs_path);
    WriteLn(stream,,"HTTP/1.1 404 OK");
    WriteLn(stream,,"Server: Bungis");
    WriteLn(stream,,"Content-Type: text/html");
    WriteLn(stream,,"");
    R404(stream,NULL,url->abs_path);
  }
en:
  Free(link);
}
class CHTMLPair:CQue {
	U64 tag;
};
CHTMLPair *EnterPair(CHTMLPair *last,CDyadStream *s,I64 *len,U64 tag) {
	CHTMLPair *r=MAlloc(sizeof(CHTMLPair));
	QueInit(r);
	QueIns(r,last);
	r->tag=tag;
	Write(s,len,"<%c>",r->tag);
	return r;
}
CHTMLPair *ClosePair(CHTMLPair *last,CDyadStream *s,I64 *len,I64 cnt=1) {
	loop:;
	CHTMLPair *r=last->last;
	if(last->next==last->last)
		return r;
	QueRem(last);
	WriteLn(s,len,"</%c>",last->tag);
	Free(last);
	last=r;
	if(--cnt) goto loop;
	return r;
}
class CHeaderItem:CQue {
	U8 *text;
	I64 depth;
	I64 ident;
};
U8 *MakeIndex(CDyadStream *stream,I64 *len_ptr,CHeaderItem *index) {
	CHeaderItem *cur;
	CHTMLPair *pairs=MAlloc(sizeof(CHTMLPair)),*cpair=pairs;
	QueInit(pairs);
	cpair=pairs;
	WriteLn(stream,len_ptr,"<DIV CLASS=\"paper\"><H3>Table of contents</H3>");
	I64 last_idx=0;
	for(cur=index->next;TRUE;) {
		if(cur->depth>last_idx){
			while(last_idx++<cur->depth)
				cpair=EnterPair(cpair,stream,len_ptr,'OL');
		}
		if(cur->depth<last_idx){
			while(last_idx-- <cur->depth)
				cpair=ClosePair(cpair,stream,len_ptr);
		}
		WriteLn(stream,len_ptr,"<LI><A HREF=\"#%d\">%s</A></LI>",cur->ident,cur->text);
		cur=cur->next;
		if(cur==index) goto en;
	}
	en:
	while(cpair!=pairs)
		cpair=ClosePair(cpair,stream,len_ptr);
	QueDel(pairs);
	WriteLn(stream,len_ptr,"</DIV>");
}
U0 WikiGet(CServer *srv,CDyadStream *stream,CURL *url,CHTTPRequest *req) {
  U8 *path=MStrPrint(WIKI_ROOT"/%s",url->abs_path),*ftxt,*fptr,
        *ln,*oln,*ln2,*t1,*t2,*t3;
  I64 len=0,*len_ptr,flags,i,depth,ident=0;
  I64 list_depths[0x100],list_depths_i;
  CHTMLPair *markup=MAlloc(sizeof(CHTMLPair)),*last;
  CHeaderItem *headers=MAlloc(sizeof(CHeaderItem)),*cheader,*theader;
  QueInit(markup);
  QueInit(headers);
  cheader=headers;
  last=markup;
  if(!StrLen(url->abs_path)||!StrCmp(url->abs_path,"/"))
	path=StrNew(WIKI_ROOT"/WikiHome");
  if(FileFind(path)) {
    flags=0;
    ftxt=FileRead(path);
    len_ptr=&len;
loop:
    ident=0;
    list_depths_i=0;
    fptr=ftxt;
    WikiHeader(stream,len_ptr,url->abs_path);
    if(!len_ptr) { //On second run len_ptr is NULL,but header is already populated
	MakeIndex(stream,len_ptr,headers);
	for(cheader=headers->next;cheader!=headers;cheader=cheader->next)
		Free(cheader->text);
	QueDel(headers);
    }
    while(oln=ln=ReadLine(fptr,&fptr)) {
//Is code if starts with whitespace
      if(StrOcc("\t ",*ln)) {
        if(!LBts(&flags,WGf_CODE)) {
	  last=EnterPair(last,stream,len_ptr,'PRE');
	  last=EnterPair(last,stream,len_ptr,'CODE');
        }
        WriteLn(stream,len_ptr,"%s",t1=HTMLify(ln));
        Free(t1);
        goto next;
      } else if(LBtr(&flags,WGf_CODE)) {
        last=ClosePair(last,stream,len_ptr);
        last=ClosePair(last,stream,len_ptr);
      }
      if(*ln=='=') {
        last=ClosePair(last,stream,len_ptr);
        for(i=0;ln[i]=='=';i++);
        WriteLn(stream,len_ptr,"<H%d ID=\"%d\">%s</H%d>",i,++ident,t1=HTMLify(ln+i),i);
	if(len_ptr){
        	theader=MAlloc(sizeof(CHeaderItem));
		QueInit(theader);
		theader->text=t1;
		theader->depth=i;
		theader->ident=ident;
		QueIns(theader,cheader);
		cheader=theader;
	} else
		Free(t1);
        goto next;
      }
      if(*ln=='-') {
        for(i=0;ln[i]=='-';i++);
        if(i>=4) {
	  last=ClosePair(last,stream,len_ptr);
	  WriteLn(stream,len_ptr,"<HR>");
	  goto next;
        }
      }
      if(*ln=='*') {
        for(i=0;ln[i]=='*';i++);
        ln+=i;
        depth=list_depths[list_depths_i];
        if(!list_depths_i) {
	  last=EnterPair(last,stream,len_ptr,'UL');
	  list_depths[list_depths_i++]=i;
        } else if(depth==i) {
//Do nothing
        } else if(depth>i) {
	  last=ClosePair(last,stream,len_ptr,depth-i);
	  list_depths_i--;
        } else if(depth<i) {
	  last=EnterPair(last,stream,len_ptr,'UL');
	  list_depths[list_depths_i++]=i;
        } else
	  ;
        last=EnterPair(last,stream,len_ptr,'LI');
        LBts(&flags,WGf_LIST);
        goto fmt_line;
      } else if(list_depths_i) {
        last=ClosePair(last,stream,len_ptr,list_depths_i);
        list_depths_i=0;
      }
      if(!Bts(&flags,WGf_PARA))
        last=EnterPair(last,stream,len_ptr,'P');
      if(*ln=='|') {
        if(LBts(&flags,WGf_TABLE)) {
row_start:
	  last=EnterPair(last,stream,len_ptr,'TR');
	  last=EnterPair(last,stream,len_ptr,'TD');
        } else {
	  last=EnterPair(last,stream,len_ptr,'TABLE');
	  goto row_start;
        }
      } else if(LBtr(&flags,WGf_TABLE)) {
        last=ClosePair(last,stream,len_ptr);
      }
fmt_line:
      for(;*ln;) {
        switch(*ln) {
	  start:
	    t1=t2=NULL;
	    case '|':
	      ln2=ln;
	      last=ClosePair(last,stream,len_ptr);
	      last=EnterPair(last,stream,len_ptr,'TD');
	      break;
	    case '[':
	      ln2=StrFirstOcc(ln+1,"]");
	      if(!ln2) ln2=ln+StrLen(ln);
	      t1=StrSlice(ln+1,ln2);
	      t3=StrNew(t1);//Trimming mutates t1
	      if(IsImage(Trim(t1))) {
	        Write(stream,len_ptr,"<IMG SRC=\"%s\">",Trim(t1));
	      } else if(!StrFirstOcc(t3," "))
	        Write(stream,len_ptr,"<A HREF=\"""/%s\">%s</A>",Trim(t1),Trim(t1));
	      else {
	        t2=StrFirstOcc(t3," ");
	        *t2++=0;
	        t2=HTMLify(t2);
	        Write(stream,len_ptr,"<A HREF=\"""/%s\">%s</A>",Trim(t1),Trim(t2));
	      }
	      Free(t3);
	      break;
	    case '*':
	      ln2=StrFirstOcc(ln+1,"*");
	      if(!ln2) ln2=ln+StrLen(ln);
	      Write(stream,len_ptr,"<B>%s</B>",t2=HTMLify(t1=StrSlice(ln+1,ln2)));
	      break;
	    case '/':
	      ln2=StrFirstOcc(ln+1,"/");
	      if(!ln2) ln2=ln+StrLen(ln);
	      Write(stream,len_ptr,"<I>%s</I>",t2=HTMLify(t1=StrSlice(ln+1,ln2)));
	      break;
	    case '_':
	      ln2=StrFirstOcc(ln+1,"_");
	      if(!ln2) ln2=ln+StrLen(ln);
	      Write(stream,len_ptr,"<U>%s</U>",t2=HTMLify(t1=StrSlice(ln+1,ln2)));
	      break;
	    case '-':
	      ln2=StrFirstOcc(ln+1,"-");
	      if(!ln2) ln2=ln+StrLen(ln);
	      Write(stream,len_ptr,"<S>%s</S>",t2=HTMLify(t1=StrSlice(ln+1,ln2)));
	      break;
	    case '+':
	      ln2=StrFirstOcc(ln+1,"+");
	      if(!ln2) ln2=ln+StrLen(ln);
	      Write(stream,len_ptr,"<MARK>%s</MARK>",t2=HTMLify(t1=StrSlice(ln+1,ln2)));
	      break;
	  end:
	    if(*ln2)
	      ln=ln2+1;
	    else
	      ln=ln2;
	    Free(t2),Free(t1);
	    break;
	  default:
	    ln2=StrFirstOcc(ln,"-_/*[|");
	    if(!ln2) ln2=ln+StrLen(ln);
	    Write(stream,len_ptr,"%s",t2=HTMLify(t1=StrSlice(ln,ln2)));
	    Free(t1),Free(t2);
	    ln=ln2;
        }
      }
      if(LBtr(&flags,WGf_LIST))
        last=ClosePair(last,stream,len_ptr); //Close list item
      else if(Bt(&flags,WGf_TABLE)) {
        last=ClosePair(last,stream,len_ptr); //Close data
        last=ClosePair(last,stream,len_ptr); //Close row
      }
      else if(Bt(&flags,WGf_PARA))
        Write(stream,len_ptr,"<BR>");
next:
      Free(oln);
    }
finish:
    while(markup!=(last=ClosePair(last,stream,len_ptr)));
    WikiFooter(stream,len_ptr,url->abs_path);
    if(len_ptr) {
	//dummy write an index to compute its length
	MakeIndex(stream,len_ptr,headers);
      WriteLn(stream,,"HTTP/1.1 200 OK");
      WriteLn(stream,,"Server: Bungis");
      WriteLn(stream,,"Content-Type: text/html");
      WriteLn(stream,,"");
      len_ptr=NULL;
      goto loop;
    }
    Free(ftxt);
  }
  Free(path);
  DyadEnd(stream);
}
#include "find.HC"
U0 Get(CServer *srv,CDyadStream *stream,CURL *url,CHTTPRequest *req)  {
	U8 *dot=FileExtDot(url->abs_path);
	if(!StrICmp(url->abs_path,WIKI_SEARCH)) {
		FindGet(srv,stream,url,req);
		return;
	}
	if(!dot) {
		WikiGet(srv,stream,url,req);
		return;
	}
	if(dot&&!StrICmp(dot,".WIKI")) {
		WikiGet(srv,stream,url,req);
		return;
	}
	FileGet(srv,stream,url,req);
}
CServer *srv=CreateServer(8080);
srv->get=&Get;
Serve(srv);
