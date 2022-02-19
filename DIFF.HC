class CDiffItem {
    #define DIFF_SAME '='
    #define DIFF_DELETE '-'
    #define DIFF_INSERT '+'
    U32 type;
    U32 line;
    U8 *str;
};
class CLine {
    I64 idx;
    U8 *text;
};
CLine *SplitLines(U8 *text,I64 *line_cnt) {
    I64 lines=StrOcc(text,'\n')+1;
    CLine *ret=MAlloc(sizeof(CLine)*lines),*ret_ptr;
    U8 *ln_start=text,*end_ptr,*line;
    ret_ptr=ret;
    loop:
    end_ptr=StrFirstOcc(ln_start,"\n");    
    if(!end_ptr) end_ptr=ln_start+StrLen(ln_start);
    if(*end_ptr)
        line=StrNCpy(MAlloc(end_ptr-ln_start+2),ln_start,end_ptr-ln_start+1);
    else {
        line=StrNCpy(MAlloc(end_ptr-ln_start+2),ln_start,end_ptr-ln_start);
    }
    ret_ptr->idx=ret_ptr-ret;
    ret_ptr++ ->text=line;
    if(*end_ptr=='\n') {
        ln_start=++end_ptr;
        goto loop;
    }
    if(line_cnt) *line_cnt=lines;
    return ret;
}
I64 SCmp(CLine *a,CLine *b) {
    return StrCmp(a->text,b->text);
}
CDiffItem *DiffSub(CDiffItem *res,CLine *str1,CLine *str2,I64 N,I64 M,CLine *sorted_a,CLine *sorted_b,I64 i1_lo,I64 i1_hi,I64 i2_lo,I64 i2_hi) {
    static I64 cnt;
    I64 i1=0,i2=0,i2b,i1b,cmp,score,j1,j2,n,best1,best2,best_score=0;
    while(i1<N&&i2<M) {
        cmp=StrCmp(sorted_a[i1]->text,sorted_b[i2]->text);
        if(cmp>0)
            i2++;
        else if(cmp<0)
            i1++;
        else {
            i2b=i2;
            while(!StrCmp(sorted_a[i1]->text,sorted_b[i2]->text)) {    
                if(i2_lo<=sorted_b[i2]->idx<i2_hi&&i1_lo<=sorted_a[i1]->idx<i1_hi) {
                    score=0;
                    j1=sorted_a[i1]->idx;
                    j2=sorted_b[i2]->idx;
                    n=i1_hi-j1;
                    if(i2_hi-j2<n)
                        n=i2_hi-j2;
                    while(score<n) {
                        if(!StrCmp(str1[j1+score]->text,str2[j2+score]->text))
                            score++;
                        else
                            break;
                    }
                    if(score>best_score) {
                        best_score=score;    
                        best1=j1;
                        best2=j2;
                        if(i1_lo>best1||i1_hi<best1) throw('tsrdfdf');
                        if(i2_lo>best2||i2_hi<best2) throw('tsrdfdf');
                    }
                }
                i2++;
                if(i2>=M)
                    break;
            }
            i2=i2b;
            i1++;
        }
    }
    if(!best_score) {
        for(i1=0;i1<i1_hi-i1_lo;i1++) {
            res->line=i2_lo;
            res->type=DIFF_INSERT;
            res->str=StrNew(str1[i1+i1_lo]->text);
            res++;
        }    
        for(i2=0;i2<i2_hi-i2_lo;i2++) {
            res->line=i2_lo;
            res->type=DIFF_DELETE;
            res->str=StrNew(str2[i2+i2_lo]->text);
            res++;
        }
        return res;
    } 
    //"%d,%d,%d\n",best_score,best1,best2;
    res=DiffSub(res,str1,str2,N,M,sorted_a,sorted_b,i1_lo,best1,i2_lo,best2);
    for(i1=0;i1!=best_score;i1++) {
        res->type=DIFF_SAME;
        res->line=str2[best2+i1]->idx;
        res->str=StrNew(str1[best1+i1]->text);
        res++;
    }
    res=DiffSub(res,str1,str2,N,M,sorted_a,sorted_b,best1+best_score,i1_hi,best2+best_score,i2_hi);
    return res;
}
CDiffItem *Diff(U8 *f1,U8 *f2) {
    I64 N,M;
    CLine *str1=SplitLines(f1,&N);
    CLine *str2=SplitLines(f2,&M);
    CDiffItem *items=MAlloc(sizeof(CDiffItem)*(N+M)),*trimmed_items,*item_ptr;
    CLine *sorted_a=MemCpy(MAlloc(N*sizeof(CLine)),str1,N*sizeof(CLine));
    CLine *sorted_b=MemCpy(MAlloc(M*sizeof(CLine)),str2,M*sizeof(CLine));
    QSort(sorted_a,N,sizeof(CLine),&SCmp);
    QSort(sorted_b,M,sizeof(CLine),&SCmp);
    item_ptr=DiffSub(items,str1,str2,N,M,sorted_a,sorted_b,0,N,0,M);
    "%d,%d\n",item_ptr-items,N+M;
    Free(sorted_a),Free(sorted_b);
    trimmed_items=MAlloc(sizeof(CDiffItem)*(item_ptr-items));
    MemCpy(trimmed_items,items,MSize(trimmed_items));
    Free(items);
    while(--N>=0)
        Free(str1[N]->text);
    while(--M>=0)
        Free(str2[M]->text);
    Free(str1),Free(str2);
    return trimmed_items;
}
I64 cnt1,cnt2,lc1,lc2;
U8 *f1=FileRead("PARSER.HC",&cnt1);
U8 *f2=FileRead("../Documents/3Days2/PARSER.HC",&cnt2);
CDiffItem *diff=Diff(f1,f2);
Free(f1),Free(f2);
U0 ViewDiff(CDiffItem *diff) {
    I64 code,char,ptype=0,idx=0,cnt=MSize(diff)/sizeof(*diff);
    while(idx!=cnt) {
            code=char=0;
        if(diff->type!=ptype) {
            "\x1b[39m";
            if(diff->type==DIFF_SAME) {
                "\n=======[SAME@%d]======\n\x1b[36m",diff->line+1;
            } else if(diff->type==DIFF_DELETE) {
                "\n=======[DELETE:%d]======\n\x1b[31m",diff->line+1;
            } else 
                "\n=======[INSERT:%d]======\n\x1b[32m",diff->line+1;
            ptype=diff->type;
        }
        "%s",diff++ ->str;
        idx++;
    }
    "\x1b[39m";
    "\n=======[END]======\n";
}
ViewDiff(diff);
U0 SaveDiff(U8 *to,CDiffItem *item) {
    I64 idx,len2=0,items=MSize(item)/sizeof(*item);
    U8 *ret,*oret;
    CArcCompress *comped;
    for(idx=0;idx!=items;idx++) {
        if(item[idx]->type!=DIFF_SAME) {
            len2+=2*sizeof(U32);
            len2+=StrLen(item[idx]->str)+1;
        }
    }
    oret=ret=MAlloc(len2);
    for(idx=0;idx!=items;idx++) {
        if(item[idx]->type!=DIFF_SAME) {
            ret(U32*)[0]=item[idx]->type;
            ret+=sizeof(U32);
            ret(U32*)[0]=item[idx]->line;
            ret+=sizeof(U32);
            StrCpy(ret,item[idx]->str);
            ret+=StrLen(ret)+1;
        }
    }
    comped=CompressBuf(oret,len2);
    FileWrite(to,comped,MSize(comped));
    Free(comped);
    Free(oret);
}
SaveDiff("PARSER.DIFF",diff);
U0 ApplyDiff(U8 *dst,U8 *src,U8 *diff,I64 *sz=NULL) {
    CArcCompress *comped=FileRead(diff,NULL);
    U8 *raw=ExpandBuf(comped),*cap,*oraw,*dptr,*odptr,*osrc=src;
    if(sz) odptr=dptr=MAlloc(*sz);
    else odptr=dptr=NULL;
    oraw=raw;
    cap=MSize(raw)+raw;
    I64 dcnt,line,cur_ln=0,rsz=0;
    src=FileRead(src,NULL);
    CLine *dlines=SplitLines(src,&dcnt);
    Free(comped);
    I64 chr;
    for(;raw+4<=cap;) {
        switch(chr=raw(U32*)[0]) {
            start:
            raw+=sizeof(U32);
            line=raw(U32*)[0];
            raw+=sizeof(U32);
            for(;cur_ln<line<dcnt;cur_ln++) {
                if(dptr) {
                    StrCpy(dptr,dlines[cur_ln].text);
                    dptr+=StrLen(dlines[cur_ln].text);
                } else rsz+=StrLen(dlines[cur_ln].text);
            }
            case DIFF_INSERT:
            if(dptr) {
                StrCpy(dptr,raw);
                dptr+=StrLen(raw);
            } else rsz+=StrLen(raw);
            raw+=StrLen(raw)+1;
            break;
            case DIFF_DELETE:
            raw+=StrLen(raw)+1;
            cur_ln++;
            break;
            end:
            break;
            default:
            throw('Merge');
        }
    }
    for(;cur_ln<dcnt;cur_ln++) {
        if(dptr) {
            StrCpy(dptr,dlines[cur_ln].text);
            dptr+=StrLen(dlines[cur_ln].text);
        } else rsz+=StrLen(dlines[cur_ln].text);
    }
    Free(oraw);
    while(--dcnt>=0)
        Free(dlines[dcnt]->text);
    Free(dlines);
    Free(src);
    if(odptr) {
        if(dst) FileWrite(dst,odptr,*sz);
        Free(odptr);
    }
    if(!sz) {
        ApplyDiff(dst,osrc,diff,&rsz);
    }
}
ApplyDiff("PARSER2.HC","../Documents/3Days2/PARSER.HC","PARSER.DIFF");

