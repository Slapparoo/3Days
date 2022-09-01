class CFileListEnt:CQue {
  U8 *filename;
  I64 hits;
};
//https://en.wikipedia.org/wiki/Ukkonen's_algorithm
class CUkkonen {
  I64 node_lock;
  //First 128 charactors
  CUkkonen *tree[0x80];
  CQue head; //CFileListEnt
};
//Here is the deal,you supply the search_tree_hc and search_tree_root
//That way we can generate a new index while we use the old one
I64 index_lock=0;

//TempleOS wants a block pool,3Days will take a NULL
//Im using a heap control to free the entire memory on re-index
CHeapCtrl *_search_tree_hc=HeapCtrlInit(,Fs,NULL),*_search_tree_hc2;
CUkkonen *_search_root=CAlloc(sizeof(CUkkonen),_search_tree_hc),*_search_root2;
QueInit(&_search_root->head);
//This dude will re-index  files on request
CTask *index_task=Spawn(&SrvTaskCont,0,"Indexer",0,Fs);
CDate last_index_time=0;
CUkkonen *UkkonenFind(U8 *str,I64 cap=-1,CUkkonen *search_root) {
  CUkkonen *root=search_root,*old;
  I64 i,len=cap;
  if(cap==-1)
    len=StrLen(str);
  for(i=0;i!=len;i++) {
    while(LBts(&root->node_lock,0))
      PAUSE;
    if(!root->tree[ToUpper(str[i]&0x7f)]) {
      LBtr(&root->node_lock,0);
      return NULL;
    }
    old=root;
    root=root->tree[ToUpper(str[i]&0x7f)];
    LBtr(&old->node_lock,0);
  }
  return root;
}
U0 AddToSuffixTree(U8 *str,U8 *file,I64 len=-1,CUkkonen *search_root,CUkkonen *search_tree_hc) {
  CUkkonen *root=search_root,*cur,*old;
  CFileListEnt *ent;
  I64 i,j,k;
  if(len==-1)
    len=StrLen(str);
  if(cur=UkkonenFind(str,len,search_root)) {
    while(LBts(&cur->node_lock,0))
      PAUSE;
    for(ent=cur->head.next;ent!=&cur->head;ent=ent->next) {
      if(!StrCmp(ent->filename,file)) {
        LBtr(&cur->node_lock,0);
        return;
      }
    }
    LBtr(&cur->node_lock,0);
  }
  for(i=0;i<=len;i++) {
    for(j=i-1;j>=0;j--) {
      if(!(cur=UkkonenFind(str+j,i-j,search_root))) {
l_ent:
        for(k=0;TRUE;k++) {
	  cur=UkkonenFind(str+j,i-j-k,search_root);
	  if(cur) {
	    while(LBts(&cur->node_lock,0))
	      PAUSE;
	    if(cur->tree[ToUpper(str[i-j-k]&0x7f)]) {
	      LBtr(&cur->node_lock,0);
	      goto ins;
	    }
	    old=cur;
	    cur->tree[ToUpper(str[i-j-k]&0x7f)]=CAlloc(sizeof(CUkkonen),search_tree_hc);
	    cur=cur->tree[ToUpper(str[i-j-k]&0x7f)];
	    QueInit(&cur->head);
	    LBtr(&old->node_lock,0);
	    goto l_ent;
	  }
        }
      }
ins:
      while(LBts(&cur->node_lock,0))
        PAUSE;
      for(ent=cur->head.next;ent!=&cur->head;ent=ent->next) {
        if(!StrCmp(ent->filename,file)) {
	  ent->hits++;
	  goto next;
        }
      }
      ent=MAlloc(sizeof CFileListEnt,search_tree_hc);
      ent->filename=StrNew(file,search_tree_hc);
      QueInit(ent);
      QueIns(ent,cur->head.last);
next:;
      LBtr(&cur->node_lock,0);
    }
  }
}
U0 IndexFile(U8 *filename) {
  U8 *ftxt=FileRead(filename);
  I64 i,consec;
  for(i=0;ftxt[i];) {
    consec=0;
    while(ftxt[i]&&Bt(char_bmp_white_space,ftxt[i]))
      i++;
    while(ftxt[i+consec]&&!Bt(char_bmp_white_space,ftxt[i+consec])) {
      consec++;
    }
    AddToSuffixTree(ftxt+i,filename,consec,_search_root2,_search_tree_hc2);
    i+=consec;
  }
  Free(ftxt);
}
U0 ResetFileIndex() {
  if(last_index_time>Unix2CDate(FUnixTime(WIKI_ROOT)))
    return;
  last_index_time=Now;
  CDirEntry *first=FilesFind(WIKI_ROOT"/*",FUF_RECURSE),*cur;
  U8 *dot;
  I64 c=mp_cnt-1,i;
  CTask *wait_for[mp_cnt]; 
  MemSetI64(wait_for,0,mp_cnt);
  _search_tree_hc2=HeapCtrlInit(,Fs,NULL);
  _search_root2=CAlloc(sizeof(CUkkonen),_search_tree_hc2);
  QueInit(&_search_root2->head);
  cur=first;
enter:
  while(cur) {
    dot=FileExtDot(cur->full_name);
    if(dot)
      if(!StrICmp(dot,".WIKI")||!StrICmp(dot,".WIKI.Z")) {
        wait_for[c]=Spawn(&IndexFile,cur->full_name,"Indexer",c);
        c=(c+1)%mp_cnt; 
      }
wait:
    for(;c!=mp_cnt;c++) {
      if(wait_for[c]) {
        if(!TaskValidate(wait_for[c])) {
	  cur=cur->next;
	  goto enter;
        }
      } else {
        cur=cur->next;
        goto enter;
      }
    }
    c=0;
    Sleep(2);
    if(cur)
      goto wait;
  }
  for(c=0;c!=mp_cnt;c++) {
    if(wait_for[c])
      if(TaskValidate(wait_for[c]))
        DeathWait(wait_for[c]);
  }
  DirEntryDel(first);
  while(LBts(&index_lock,0))
    PAUSE;
  HeapCtrlDel(_search_tree_hc);
  _search_tree_hc=_search_tree_hc2;
  _search_root=_search_root2;
  LBtr(&index_lock,0);
}
U0 FileListEntDel(CFileListEnt *head) {
  CFileListEnt *cur;
  if(!head) return;
  for(cur=head->next;cur!=head;cur=cur->next)
    Free(cur->filename);
  QueDel(head);
}
//Be sure to delete the ques  FileListEntDel
CFileListEnt *FindIndexedStr(U8 *str,CUkkonen *search_root) {
  I64 consec,i;
  CFileListEnt *head2,*c2,*head,*ret=NULL,*second,*shead,*c,*tmp;
  CUkkonen *find;
//Here's the deal,I sepreate the items by whitespace so...
  //...if str contains whitespace,we seperate str by whitespace...
  //... and we make sure each seperated part is found
  consec=i=0;
  while(str[i]&&Bt(char_bmp_white_space,str[i]))
    i++;
  while(str[i+consec]&&!Bt(char_bmp_white_space,str[i+consec]))
    consec++;
  find=UkkonenFind(str+i,consec,search_root);
  if(!find) goto r;
  if(find->head.next==&find->head) goto r;
  c=find->head.next;
  ret=MAlloc(sizeof CQue);
  QueInit(ret);
  for(c=c;c!=&find->head;c=c->next) {
    tmp=MAlloc(sizeof CFileListEnt);;
    QueInit(tmp);
    tmp->filename=StrNew(c->filename);
    tmp->hits=c->hits;
    QueIns(tmp,ret->last);
  }
  if(str[i+consec]) {
    while(LBts(&index_lock,0))
      PAUSE;
    second=FindIndexedStr(str+i+consec,search_root);
    LBtr(&index_lock,0);
    if(!second) {
      FileListEntDel(ret);
      ret=NULL;
      goto r;
    }
    for(c=(head=ret)->next;c!=head;c=c->next) {
      for(c2=(head2=second)->next;c2!=head2;c2=c2->next) {
        if(!StrCmp(c->filename,c2->filename)) {
	  c->hits+=c2->hits;
	  goto next;
        }
      }
f:
      Free(c->filename);
      QueRem(c);
      Free(c);
next:;
    }
    if(head->next==head) {
      Free(head);
      ret=NULL;
      goto r;
    }
    FileListEntDel(second);
  }
r:
  return ret;
}
I64 FindResultSort(I64 a,I64 b) {
    return StrCmp(a,b); //TODO TODO sort by relevance
}
U0 FindGet(CServer *srv,CDyadStream *stream,CURL *url,CHTTPRequest *req) {
  U8 **sorted,*ftxt,*ptr,*ln_txt,*search_for=GetQueryValue(url->query,"s"),*h,*title,*dot;
  title=MStrPrint("FIND:%s",search_for);
  I64 len=0,*len_ptr=&len,mat,fcnt,f,trim;
  ptr=FileNameAbs(WIKI_ROOT);
  trim=StrLen(ptr);
  Free(ptr);
  CFifoI64 *matches=FifoI64New(0x80);
  CDirEntry c_ent;
  CFileListEnt *head,*ent;
loop:
  WikiHeader(stream,len_ptr,title);
  WriteLn(stream,len_ptr,"<FORM CLASS=\"form-group\" ACTION=\""WIKI_SEARCH"\">");
  WriteLn(stream,len_ptr,"<LABEL FOR=\"s\">SEARCH:</LABEL>");
  WriteLn(stream,len_ptr,"<INPUT NAME=\"s\" ID=\"s\">");
  WriteLn(stream,len_ptr,"<INPUT TYPE=\"Submit\" VALUE=\"Submit\">");
  WriteLn(stream,len_ptr,"</FORM>");
  if(search_for) {
    head=FindIndexedStr(search_for,_search_root);
    if(head)
      for(ent=head->next;ent!=head;ent=ent->next) {
        ftxt=FileRead(ent->filename);
        if(StrIMatch(search_for,ftxt)) {
	  FifoI64Ins(matches,StrNew(ent->filename));
        }
        Free(ftxt);
      }
    FileListEntDel(head);
  }
  sorted=MAlloc(FifoI64Cnt(matches)*sizeof(U8 **));
  fcnt=FifoI64Cnt(matches);
  while(mat=FifoI64Cnt(matches)) {
    FifoI64Rem(matches,&sorted[mat-1]);
  }
  QSortI64(sorted,fcnt,&FindResultSort);
  for(f=0;f!=fcnt;f++) {
    FileFind(sorted[f],&c_ent);
    ptr=StrNew(c_ent.full_name+trim);
    Free(c_ent.full_name);
    c_ent.full_name=ptr;
    WriteLn(stream,len_ptr,"<ARITCLE CLASS=\"article\">",c_ent.full_name);
    WriteLn(stream,len_ptr,"<H4 CLASS=\"article-title\"><A HREF=\"%s\">%s</A></H4>",c_ent.full_name,c_ent.full_name);    
    WriteLn(stream,len_ptr,"<P CLASS=\"article-meta\">Last edited at %D(%T).</P>",c_ent.full_name,c_ent.datetime);
    WriteLn(stream,len_ptr,"<P>");
    ptr=ftxt=FileRead(sorted[f]);
    for(mat=0;(ln_txt=ReadLine(ptr,&ptr));) {
      if(mat<5&&StrIMatch(search_for,ln_txt)) {
        h=HTMLify(ln_txt);
        WriteLn(stream,len_ptr,"%s<BR>",h);
        Free(h);
        mat++;
      }
      Free(ln_txt);
    }
    Free(sorted[f]);
    Free(c_ent.full_name);
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
  FifoI64Del(matches);
}
U0 ReIndex() {
  TaskExe(index_task,Fs,"ResetFileIndex;;\n",0);
}
ReIndex;