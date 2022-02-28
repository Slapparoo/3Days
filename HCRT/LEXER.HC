#include "../LEXER.HH"
class CLexFile
{
    CLexFile *next;
    U8 	*buf,	*buf_ptr;
    I64 	line_num,flags;
    U8 	*full_name,	*line_start;
    U8 *dir;
    I32 	depth;
    U8 	last_U16;
};
class CFileLines:CHash {
    I64 *line_starts;
};
#define CCf_USE_LAST_U16 0
#define CCF_USE_LAST_U16 (1<<CCf_USE_LAST_U16)
#define CCF_LAST_WAS_DOT (1<<1)
#define CCF_IN_QUOTES (1<<2)
#define CCF_NO_DEFINES (1<<3)
#define CCF_IN_IF (1<<4)
#define CCF_KEEP_NEW_LINES (1<<5)
#define CCF_REPL_MODE (1<<6)
//This is triggered when we are existing a #exe block
#define CCF_USE_EXE_TEXT (1<<7)
class CExeBlk {
  CExeBlk *last,*next;
  U8 *text;
};
class CCmpCtrl {
    CLexFile *lex_include_stk;
    U8 *cur_buf_ptr;
    I64 flags,last_line_num,code,token,cur_i64,dollar_cnt;
    CLexFile *lex_prs_stk;
    U8 *cur_str;
    I64 cur_str_len;
    F64 cur_f64;
    CHash *hash_entry;
    CHashTable *file_lines;
    //Used for StreamPrint()
    CExeBlk *exe_stream;
    U16 last_U16;
};
CLexFile *LexFilePush(CCmpCtrl *cc) {
    CLexFile *res=MAlloc(sizeof(CLexFile));
    if(res->next=cc->lex_include_stk) {
        res->depth=res->next->depth+1;
    } else {
        res->depth=-1;
    }
    return cc->lex_include_stk=res;
}
CLexFile *LexFilePop(CCmpCtrl *cc) {
    CLexFile *tmpf;
    if(tmpf=cc->lex_include_stk) {
        if(cc->lex_include_stk=tmpf->next) {
            Free(tmpf->buf);
        }
    }
    Free(tmpf->dir);
    Free(tmpf->full_name);
    Free(tmpf);
    if(cc->lex_include_stk) {
        if(cc->lex_include_stk->dir)
            Cd(cc->lex_include_stk->dir);
    }
    return cc->lex_include_stk;
}
I64 LexGetChar(CCmpCtrl *cc) {
    if(cc->exe_stream&&(cc->flags&CCF_USE_EXE_TEXT)) {
        cc->flags&=~CCF_USE_EXE_TEXT;
        LexIncludeStr(cc,"@TMP",cc->exe_stream->text,FALSE);
        cc->exe_stream=cc->exe_stream->last;
        if(cc->exe_stream) Free(cc->exe_stream->next);
    }
    U8 *ptr,*src,*prompt,*cd;
    CLexFile *tmpf;
    if(!Btr(&cc->flags,CCf_USE_LAST_U16)) {
lgc_start1:
        if (!(src=cc->cur_buf_ptr++)) {
            cc->cur_buf_ptr=NULL;
            goto lgc_here;
        }
        switch(cc->last_U16=*src++) {
        case 0:
lgc_here:
            tmpf=cc->lex_include_stk;
            if (tmpf->next) {
                tmpf=LexFilePop(cc);
                cc->cur_buf_ptr=tmpf->buf_ptr;
                cc->flags&=~CCF_USE_LAST_U16;
                if(!(cc->last_U16=tmpf->last_U16))
                    goto lgc_start1;
            }
            break;
        case '\n':
            tmpf=cc->lex_include_stk;
            tmpf->line_num++;
            tmpf->line_start=src;
            break;
        }
lgc_done:
    }
    if(!cc->last_U16) {
        if(cc->flags&CCF_REPL_MODE) {
            prompt=MStrPrint("(%s):>>> ",cd=DirCur);
            src=GetStr(prompt);
            Free(prompt),Free(cd);
            LexIncludeStr(cc,"(nofile)",MStrPrint("%s\n",src),FALSE);
            Free(src);
            goto lgc_start1;
        }
    }
    return cc->last_U16;
}
U0 LexSkipEol(CCmpCtrl *cc)
{//$LK,"LexGetChar",A="MN:LexGetChar"$ to NULL until end-of-line.
  I64 ch;
  do ch=LexGetChar(cc);
  while (Bt(char_bmp_non_eol,ch));
}
U0 LexBackupLastChar(CCmpCtrl *cc)
{
    CLexFile *tmpf=cc->lex_include_stk;
    tmpf->buf_ptr=cc->cur_buf_ptr;
    if (cc->flags & CCF_USE_LAST_U16) {
        tmpf->last_U16=cc->last_U16;
        cc->flags&=~CCF_USE_LAST_U16;
    } else
        tmpf->last_U16=0;
}
U0 LexIncludeStr(CCmpCtrl *cc,U8 *abs_filename,U8 *src,Bool actual_file)
{
    CFileLines *nlns;
    I64 lcnt,idx;
    U8 *st=src;
    LexBackupLastChar(cc);
    CLexFile *tmpf=LexFilePush(cc);
    if (actual_file) {
        tmpf->full_name=StrNew(abs_filename);
    }else
        tmpf->full_name=StrNew("@TMP");
    tmpf->dir=DirCur;
    tmpf->line_num=1;
    tmpf->buf=tmpf->buf_ptr=tmpf->line_start=cc->cur_buf_ptr=src;
    if(actual_file) {
        nlns=HashFind(abs_filename,cc->file_lines);
        if(!nlns) {
            nlns=MAlloc(sizeof(CFileLines));
            nlns->str=StrNew(abs_filename);
            HashAdd(nlns,cc->file_lines);
        } else
            Free(nlns->line_starts);
        nlns->line_starts=MAlloc(sizeof(I64)+sizeof(I64)*(lcnt=StrOcc(src,'\n')));
        nlns->line_starts[0]=0;
        for(idx=0;idx!=lcnt;idx++) {
            src=StrFirstOcc(src,"\n")+1;
            nlns->line_starts[idx+1]=src-st;
        }
    }
}
U0 *MAllocIdent(U0 *ptr) {
    I64 size=MSize(ptr);
    return MemNCpy(MAlloc(size),ptr,size);
}
U0 LexPush(CCmpCtrl *cc)
{   //Create token-stream save point.
    CLexFile *tmpf;
    LexBackupLastChar(cc);
    if (cc->lex_include_stk->last_U16)
        cc->flags|=CCF_USE_LAST_U16;
    tmpf=MAllocIdent(cc->lex_include_stk);
    tmpf->next=cc->lex_prs_stk;
    cc->lex_prs_stk=tmpf;
}
U0 LexPopRestore(CCmpCtrl *cc)
{   //Restore token-stream saved-point.
//Bad things can happen if you cross an #include file boundary.
    CLexFile *tmpf=cc->lex_prs_stk;
    cc->cur_buf_ptr=tmpf->buf_ptr;
    if (cc->last_U16=tmpf->last_U16)
        cc->flags|=CCF_USE_LAST_U16;
    else
        cc->flags&=~CCF_USE_LAST_U16;
    //Ignores CLexFileNext *next
    MemCpy(cc->lex_include_stk(U8 *)+sizeof(U8 *),tmpf(U8 *)+sizeof(U8 *),
           sizeof(CLexFile)-sizeof(U8 *));
    cc->lex_prs_stk=tmpf->next;
    Free(tmpf);
}
CHashTable *lexer_macros=HashTableNew(0x100);
class CHashSrcSym:CHash {
  U8 *src_link;
};
class CHashDefineStr:CHashSrcSym {
    U8 *data;
    I64 cnt;
};
#define HTT_DEFINE_STR (1<<0)
#define HTT_KEYWORD (1<<1)
#define LFSF_DEFINE (1<<0)
#define STR_LEN 1024
U0 HashSrcFileSet(CCmpCtrl *cc,CHashSrcSym *h,I64 line_num_offset=0)
{//Set $LK,"CHashSrcSym",A="MN:CHashSrcSym"$ link and help_index by cur cc pos.
  CLexFile *tmpf=cc->lex_include_stk;
  loop:
  if(!tmpf) throw('Lexer');
  if(tmpf->flags&LFSF_DEFINE) {
    tmpf=tmpf->next;
    goto loop;
  }
  I64 line_num=tmpf->line_num+line_num_offset;
  if (line_num<1) line_num=1;
  Free(h->src_link);
  h->src_link=MStrPrint("%s,%d",tmpf->full_name,line_num);
}
U0 LexWarn(CCmpCtrl *cc,U8 *fmt,...) {
    CLexFile *tmpf=cc->lex_include_stk;
    loop:
    if(!tmpf) throw('Lexer');
    if(tmpf->flags&LFSF_DEFINE) {
      tmpf=tmpf->next;
      goto loop;
    }
    U8 *buf=StrPrintJoin(NULL,fmt,argc,argv);;
    "%s,%d:: %s\n",tmpf->full_name,tmpf->line_num,buf;
    Free(buf);
}
U0 LexExcept(CCmpCtrl *cc,U8 *fmt,...) {
  CLexFile *tmpf=cc->lex_include_stk;
  loop:
  if(!tmpf) throw('Lexer');
  if(tmpf->flags&LFSF_DEFINE) {
    tmpf=tmpf->next;
    goto loop;
  }
  U8 *buf=StrPrintJoin(NULL,fmt,argc,argv);;
  "%s,%d:: %s\n",tmpf->full_name,tmpf->line_num,buf;
  Free(buf);
  throw('Lexer');
}
I64 LexInStr(CCmpCtrl *cc,U8 *buf,I64 size,Bool *done)
{
    I64 i=0,j,k,ch;
    *done=TRUE;
    while (i<size-1) {
        ch=LexGetChar(cc);
        if (!ch || ch=='"') {
            buf[i++]=0;
            return i;
        } else if (ch=='\\') {
            switch (ch=LexGetChar(cc)) {
            case '0':
                buf[i++]=0;
                break;
            case '\'':
                buf[i++]='\'';
                break;
            case '\\':
                buf[i++]='\\';
                break;
            case '"':
                buf[i++]='"';
                break;
            case 'd':
                buf[i++]='$$';
                break;
            case 'n':
                buf[i++]='\n';
                break;
            case 'r':
                buf[i++]='\r';
                break;
            case 't':
                buf[i++]='\t';
                break;
            case 'x':
            case 'X':
                j=0;
                for (k=0; k<2; k++) {
                    ch=ToUpper(LexGetChar(cc));
                    if (Bt(char_bmp_hex_numeric,ch)) {
                        if (ch<='9')
                            j=j<<4+ch-'0';
                        else
                            j=j<<4+ch-'A'+10;
                    } else {
                        cc->flags|=CCF_USE_LAST_U16;
                        break;
                    }
                }
                buf[i++]=j;
                break;
            default:
                cc->flags|=CCF_USE_LAST_U16;
                buf[i++]='\\';
            }
        } else if (ch=='$$') {
            buf[i++]='$$';
            if (cc->dollar_cnt)
                cc->dollar_cnt--;
            else if (LexGetChar(cc)!='$$') {
                cc->dollar_cnt=1;
                cc->flags|=CCF_USE_LAST_U16;
            }
        } else
            buf[i++]=ch;
    }
    *done=FALSE;
    return i;
}
I64 LexExpression(CCmpCtrl *cc) {
    U8 buf[STR_LEN];
    I64 i=0,ch;
    while(ch=LexGetChar(cc)) {
        if(ch=='\n') break;
        buf[i++]=ch;
        if(i==STR_LEN) LexExcept(cc,"Too large expression.");
    }
    buf[i]=0;
    return JIT_Eval(buf,NULL);
}
CCmpCtrl *CmpCtrlNew(U8 *buf=NULL,I64 flags=0,U8 *filename=NULL)
{   //MAlloc and Init CCmpCtrl.
//Frees buf in $LK,"CmpCtrlDel",A="MN:CmpCtrlDel"$ unless $LK,"CCF_DONT_FREE_BUF",A="MN:CCF_DONT_FREE_BUF"$ flag is set.
    //FileName is for error reporting.  If files are #included,
    //new names are used.  See $LK,"Psalmody CmpCtrlNew",A="FF:::/Apps/Psalmody/PsalmodyFile.HC,CmpCtrlNew"$.
    CCmpCtrl *cc=MAlloc(sizeof(CCmpCtrl));
    CLexFile *tmpf=LexFilePush(cc);
    if (filename)
        tmpf->full_name=FileNameAbs(filename);
    else
        tmpf->full_name=StrNew("@TMP");
    tmpf->buf=tmpf->buf_ptr=tmpf->line_start=cc->cur_buf_ptr=buf;
    tmpf->line_num=1;
    cc->file_lines=HashTableNew(0x10);
    return cc;
}
class CHashKeyword:CHash {
  I64 user_data0;
};
CHashTable *keywords=HashTableNew(0x10);
class CCmpGlbls {
    U32 *dual_U16_tokens1;
    U32 *dual_U16_tokens2;
    U32 *dual_U16_tokens3;
} cmp;
U0 CInit() {
    class {
      U8 *name;
      I64 code;
    } kws[]={
      {"union",KW_UNION},
      {"catch",KW_CATCH},
      {"class",KW_CLASS},
      {"try",KW_TRY},
      {"if",KW_IF},
      {"else",KW_ELSE},
      {"for",KW_FOR},
      {"while",KW_WHILE},
      {"extern",KW_EXTERN},
      {"_extern",KW__EXTERN},
      {"return",KW_RETURN},
      {"sizeof",KW_SIZEOF},
      {"do",KW_DO},
      {"goto",KW_GOTO},
      {"break",KW_BREAK},
      {"switch",KW_SWITCH},
      {"start",KW_START},
      {"end",KW_END},
      {"case",KW_CASE},
      {"default",KW_DEFAULT},
      {"public",KW_PUBLIC},
      {"import",KW_IMPORT},
      {"_import",KW__IMPORT},
      {"lastclass",KW_LASTCLASS},
      {"static",KW_STATIC},
      {"DU8",AKW_DU8},
      {"DU16",AKW_DU16},
      {"DU32",AKW_DU32},
      {"DU64",AKW_DU64},
      {"ALIGN",AKW_ALIGN},
      {"BINFILE",AKW_BINFILE},
      {"asm",KW_ASM},
      {"IMPORT",KW_IMPORT},
    };
    I64 cnt=sizeof(kws)/sizeof(*kws);
    while(--cnt>=0) {
      CHashKeyword *kw=MAlloc(sizeof(CHashKeyword));
      kw->type=HTT_KEYWORD;
      kw->str=StrNew(kws[cnt].name);
      kw->user_data0=kws[cnt].code;
      HashAdd(kw,keywords);
    }

    U32 *d;
    cmp.dual_U16_tokens1=d=MAlloc(sizeof(U32)*TK_TKS_NUM);
    d['!']=TK_NOT_EQU<<16+'=';
    d['&']=TK_AND_AND<<16+'&';
    d['*']=TK_MUL_EQU<<16+'=';
    d['+']=TK_PLUS_PLUS<<16+'+';
    d['-']=TK_DEREFERENCE<<16+'>';
    d['/']='*';
    d[':']=TK_DBL_COLON<<16+':';
    d['<']=TK_LESS_EQU<<16+'=';
    d['=']=TK_EQU_EQU<<16+'=';
    d['>']=TK_GREATER_EQU<<16+'=';
    d['^']=TK_XOR_EQU<<16+'=';
    d['|']=TK_OR_OR<<16+'|';
    d['%']=TK_MOD_EQU<<16+'=';
    cmp.dual_U16_tokens2=d=MAlloc(sizeof(U32)*TK_TKS_NUM);
    d['&']=TK_AND_EQU<<16+'=';
    d['+']=TK_ADD_EQU<<16+'=';
    d['-']=TK_MINUS_MINUS<<16+'-';
    d['/']='/';
    d['<']=TK_SHL<<16+'<';
    d['>']=TK_SHR<<16+'>';
    d['^']=TK_XOR_XOR<<16+'^';
    d['|']=TK_OR_EQU<<16+'=';

    cmp.dual_U16_tokens3=d=MAlloc(sizeof(U32)*TK_TKS_NUM);
    d['-']=TK_SUB_EQU<<16+'=';
    d['/']=TK_DIV_EQU<<16+'=';
}
CInit;
Bool ParseAndRun(CCmpCtrl *cc);
U0 __LexExe(CCmpCtrl *cc) {
    Lex(cc);
    if(cc->token!='{') LexExcept(cc,"Expected '{'.");
    Lex(cc);
    for(;cc->token!='}';) {
        if(!ParseAndRun(cc))
            LexExcept(cc,"Unable to parse stmt.");;
    }
    Lex(cc);
}
I64 Lex(CCmpCtrl *cc) {
    I64 i,j,k,l,ch;
    CHash *tmph;
    CExeBlk *tmpe;
    U8 *fbuf,*buf1,*buf2,*buf3,buf[STR_LEN];
    cc->last_line_num=cc->lex_include_stk->line_num;
    Bool neg_e,str_done,in_str;
    while(TRUE) {
lex_cont:
        switch(ch=LexGetChar(cc)) {
        case 0:
            return cc->token=TK_EOF;
        case '@':
        case 'a'...'z':
        case 'A'...'Z':
        case '_':
        case 128 ... 255:
lex_ident:
            i=0;
            buf[i++]=ch;
            while(TRUE) {
                if(i>=STR_LEN) {
                    LexExcept(cc,"Ident exceedes STR_LEN!\n");
                } else if(!(ch=LexGetChar(cc)))
                    break;
                else if (Bt(char_bmp_alpha_numeric,ch))
                    buf[i++]=ch;
                else {
                    cc->flags|=CCF_USE_LAST_U16;
                    break;
                }

            }
            buf[i++]=0;
            tmph=HashFind(buf,lexer_macros);
            if(tmph) j=tmph->type;
            else j=0;
            if(j&HTT_DEFINE_STR&!(cc->flags&CCF_NO_DEFINES)) {
                LexIncludeStr(cc,tmph->str,StrNew(tmph(CHashDefineStr*).data),FALSE);
                 cc->lex_include_stk->flags|=LFSF_DEFINE;
            } else {
                Free(cc->cur_str);
                cc->hash_entry=tmph;
                cc->cur_str=StrNew(buf);
                cc->token=TK_IDENT;
                goto lex_end;
            }
            break;
        case '0' ... '9':
            i=ch-'0';
            ch=ToUpper(LexGetChar(cc));
            if(ch=='X') {
                while (TRUE) {
                    ch=ToUpper(LexGetChar(cc));
                    if (Bt(char_bmp_hex_numeric,ch)) {
                        if (ch<='9')
                            i=i<<4+ch-'0';
                        else
                            i=i<<4+ch-'A'+10;
                    } else {
                        cc->cur_i64=i;
                        cc->flags|=CCF_USE_LAST_U16;
                        cc->token=TK_I64;
                        goto lex_end;
                    }
                }
            } else if(ch=='B') {
                while (TRUE) {
                    ch=LexGetChar(cc);
                    if (ch=='0')
                        i=i<<1;
                    else if (ch=='1')
                        i=i<<1+1;
                    else {
                        cc->cur_i64=i;
                        cc->flags|=CCF_USE_LAST_U16;
                        cc->token=TK_I64;
                        goto lex_end;
                    }
                }
            }
            while (TRUE) {
                if (Bt(char_bmp_dec_numeric,ch))
                    i=i*10+ch-'0';
                else {
                    if (ch=='.' || ch=='e' || ch=='E') break;
lex_is_int:
                    cc->cur_i64=i;
                    cc->flags|=CCF_USE_LAST_U16;
                    cc->token=TK_I64;
                    goto lex_end;
                }
                ch=LexGetChar(cc);
            }
            if (ch=='.') {
                ch=LexGetChar(cc);
                if (ch=='.') {
                    cc->flags|=CCF_LAST_WAS_DOT;
                    goto lex_is_int;
                }
            }
lex_float_start:
            k=0;
            while (TRUE) {
                if (Bt(char_bmp_dec_numeric,ch)) {
                    i=i*10+ch-'0';
                    k++;
                } else {
                    if (ch=='e' || ch=='E')
                        break;
                    cc->cur_f64=i*10.`-k;
                    cc->flags|=CCF_USE_LAST_U16;
                    cc->token=TK_F64;
                    goto lex_end;
                }
                ch=LexGetChar(cc);
            }
            ch=LexGetChar(cc);
            neg_e=FALSE;
            if (ch=='-') {
                neg_e=TRUE;
                ch=LexGetChar(cc);
            }
            j=0;
            while (TRUE) {
                if (Bt(char_bmp_dec_numeric,ch))
                    j=j*10+ch-'0';
                else {
                    if (neg_e)
                        cc->cur_f64=i*10.`(-j-k);
                    else
                        cc->cur_f64=i*10.`(j-k);
                    cc->flags|=CCF_USE_LAST_U16;
                    cc->token=TK_F64;
                    goto lex_end;
                }
                ch=LexGetChar(cc);
            }
            break;
        case '"':
            cc->flags|=CCF_IN_QUOTES;
            buf2=NULL;
            i=0;
            do {
                j=LexInStr(cc,buf,STR_LEN,&str_done);
                buf3=MAlloc(i+j);
                if (buf2) {
                    MemCpy(buf3,buf2,i);
                    Free(buf2);
                    buf2=buf3;
                    MemCpy(buf2+i,buf,j);
                } else {
                    buf2=buf3;
                    MemCpy(buf2,buf,j);
                }
                i+=j;
            } while (!str_done);
            Free(cc->cur_str);
            cc->cur_str=MAlloc(i);
            MemCpy(cc->cur_str,buf2,i);
            Free(buf2);
            cc->cur_str_len=i;
            cc->flags&=~CCF_IN_QUOTES;
            cc->token=TK_STR;
            goto lex_end;
        case '\'':
            k=0;
            for (j=0; j<=8; j++) {
                if (!(ch=LexGetChar(cc)) || ch=='\'')
                    break;
                if (ch=='\\') {
                    switch (ch=LexGetChar(cc)) {
                    case '0':
                        k.u8[j]=0;
                        break;
                    case '\'':
                        k.u8[j]='\'';
                        break;
                    case '"':
                        k.u8[j]='"';
                        break;
                    case '\\':
                        k.u8[j]='\\';
                        break;
                    case 'd':
                        k.u8[j]='$$';
                        break;
                    case 'n':
                        k.u8[j]='\n';
                        break;
                    case 'r':
                        k.u8[j]='\r';
                        break;
                    case 't':
                        k.u8[j]='\t';
                        break;
                    case 'x':
                    case 'X':
                        i=0;
                        for (l=0; l<2; l++) {
                            ch=ToUpper(LexGetChar(cc));
                            if (Bt(char_bmp_hex_numeric,ch)) {
                                if (ch<='9')
                                    i=i<<4+ch-'0';
                                else
                                    i=i<<4+ch-'A'+10;
                            } else {
                                cc->flags|=CCF_USE_LAST_U16;
                                break;
                            }
                        }
                        k.u8[j]=i;
                        break;
                    default:
                        k.u8[j]='\\';
                        cc->flags|=CCF_USE_LAST_U16;
                    }
                } else if (ch=='$$') {
                    ch=LexGetChar(cc);
                    k.u8[j]='$$';
                    if (ch!='$$')
                        cc->flags|=CCF_USE_LAST_U16;
                } else
                    k.u8[j]=ch;
            }
            if (ch!='\'' ) {
                LexExcept(cc,"Char const limited to 8 chars at ");
            }
            cc->cur_i64=k;
            cc->token=TK_CHAR_CONST;
            goto lex_end;
        case '!':
        case '$'...'&':
        case '('...'-':
        case '/':
        case ':'...'?':
        case '[':
        case ']'...'^':
        case '{'...'~':
        case '`':
            if (!(i=cmp.dual_U16_tokens1[ch])) {
                if (ch=='$$') {
                    ch=LexGetChar(cc);
                    if (ch=='$$') {
                        cc->token='$$';
                        goto lex_end;
                    } else if (ch) {
                        do ch=LexGetChar(cc);
                        while (ch && ch!='$$');
                        if (!ch) {
                            cc->token=TK_EOF;
                            goto lex_end;
                        } else
                            goto lex_cont;
                    } else {
                        cc->flags|=CCF_USE_LAST_U16;
                        cc->token='$$';
                        goto lex_end;
                    }
                } else {
                    cc->token=ch;
                    goto lex_end;
                }
            } else {
                j=LexGetChar(cc);
                if (i.u16[0]==j) {
                    i>>=16;
                    if (!i) {// "/*"
                        j=1;
                        do {
                            if (!(ch=LexGetChar(cc)))
                                return cc->token=TK_EOF;
lex_check_comment:
                            if (ch=='*') {
                                if (!(ch=LexGetChar(cc)))
                                    return cc->token=TK_EOF;
                                if (ch=='/')
                                    j--;
                                else
                                    goto lex_check_comment;
                            } else if (ch=='/') {
                                if (!(ch=LexGetChar(cc)))
                                    return cc->token=TK_EOF;
                                if (ch=='*')
                                    j++;
                                else
                                    goto lex_check_comment;
                            }
                        } while (j);
                        goto lex_cont;
                    } else {
                        cc->token=i;
                        goto lex_end;
                    }
                }
                if (i=cmp.dual_U16_tokens2[ch]) {
                    if (i.u16[0]==j) {
                        i>>=16;
                        if (!i) {// "//"
                            LexSkipEol(cc);
                            if (cc->flags&CCF_KEEP_NEW_LINES) {
                                cc->token='\n';
                                goto lex_end;
                            } else
                                goto lex_cont;
                        } else {
                            if (i==TK_SHL || i==TK_SHR) {
                                j=LexGetChar(cc);
                                if (j=='=') {
                                    if (i==TK_SHL)
                                        i=TK_SHL_EQU;
                                    else
                                        i=TK_SHR_EQU;
                                } else
                                    cc->flags|=CCF_USE_LAST_U16;
                            }
                            cc->token=i;
                            goto lex_end;
                        }
                    }
                    if (i=cmp.dual_U16_tokens3[ch]) {
                        if (i.u16[0]==j) {
                            cc->token=i.u16[1];
                            goto lex_end;
                        }
                    }
                }
                cc->flags|=CCF_USE_LAST_U16;
                cc->token=ch;
                goto lex_end;
            }
        case '.':
        /*if (cc->flags&CCF_KEEP_DOT) {
      	  cc->token=ch;
      	  goto lex_end;
      	}*/
      	if (cc->flags&CCF_LAST_WAS_DOT) {
      	  cc->flags&=~CCF_LAST_WAS_DOT;
      	  goto lex_dot_dot;
      	}
      	ch=LexGetChar(cc);
      	if ('0'<=ch<='9') {
      	  i=0;
      	  goto lex_float_start;
      	} else if (ch=='.') {
      lex_dot_dot:
      	  cc->token=TK_DOT_DOT;
      	  if (LexGetChar(cc)=='.')
      	    cc->token=TK_ELLIPSIS;
      	  else
      	    cc->flags|=CCF_USE_LAST_U16;
      	  goto lex_end;
      	}
      	cc->flags|=CCF_USE_LAST_U16;
      	cc->token='.';
      	goto lex_end;
        case '#':
            if(Lex(cc)!=TK_IDENT) {
                LexExcept(cc,"Stray '#'!");
            }
            if(!StrCmp(cc->cur_str,"define")) {
                cc->flags|=CCF_NO_DEFINES;
                if(Lex(cc)!=TK_IDENT)
                    LexExcept(cc,"Expected name!");
                tmph=MAlloc(sizeof(CHashDefineStr));
                tmph->str=cc->cur_str;
                cc->cur_str=NULL;
                tmph->type=HTT_DEFINE_STR;
                HashSrcFileSet(cc,tmph);
                do ch=LexGetChar(cc); //skip space between define name and start
                while (Bt(char_bmp_non_eol_white_space,ch));
                cc->flags&=~CCF_NO_DEFINES;
                i=j=0;
                buf2=NULL;
                if (ch) {
                    in_str=FALSE;
                    do {
                        if (ch=='\\') {
                            if (ch=LexGetChar(cc)) {
                                if (ch!='\r' && ch!='\n') {
                                    buf[j++]='\\';
                                    buf[j++]=ch;
                                } else if (ch=='\r' || ch!='\n')
                                    cc->flags|=CCF_USE_LAST_U16;
                            } else {
                                buf[j++]='\\';
                                break;
                            }
                        } else if (ch!='\n') {
                            if (ch=='\"')
                                in_str=!in_str;
                            buf[j++]=ch;
                        } else
                            break;
                        while (ch=LexGetChar(cc)) {
                            if (ch=='/') {
                                ch=LexGetChar(cc);
                                if (ch=='/' && !in_str) {
                                    do ch=LexGetChar(cc);
                                    while (Bt(char_bmp_non_eol,ch));
                                    break;
                                } else {
                                    buf[j++]='/';
                                    cc->flags|=CCF_USE_LAST_U16;
                                }
                            } else if (ch=='\\') {
                                if (ch=LexGetChar(cc)) {
                                    if (ch=='\"') {
                                        buf[j++]='\\';
                                        buf[j++]=ch;
                                    } else {
                                        cc->flags|=CCF_USE_LAST_U16;
                                        ch='\\';
                                        break;
                                    }
                                }
                            } else if (Bt(char_bmp_non_eol,ch)) {
                                if (ch=='\"')
                                    in_str=!in_str;
                                buf[j++]=ch;
                            } else
                                break;
                            if (j>=STR_LEN-4) {//Spot for ['\'][ch],[ch],[0]
                                buf[j++]=0;
                                buf3=MAlloc(i+j);
                                if (buf2) {
                                    MemCpy(buf3,buf2,i);
                                    Free(buf2);
                                    buf2=buf3;
                                    MemCpy(buf2+i,buf,j);
                                } else {
                                    buf2=buf3;
                                    MemCpy(buf2,buf,j);
                                }
                                i+=j-1;
                                j=0;
                            }
                        }
                    } while (ch=='\\');
                }
                buf[j++]=0;
                buf3=MAlloc(i+j);
                if (buf2) {
                    MemCpy(buf3,buf2,i);
                    Free(buf2);
                    buf2=buf3;
                    MemCpy(buf2+i,buf,j);
                } else {
                    buf2=buf3;
                    MemCpy(buf2,buf,j);
                }
                tmph(CHashDefineStr *)->data=buf2;
                tmph(CHashDefineStr *)->cnt=-1;
                HashAdd(tmph,lexer_macros);
            } else if(!StrCmp(cc->cur_str,"include")) {
              if (Lex(cc)!=TK_STR)
	               goto lex_end;
              buf2=FileNameAbs(cc->cur_str);
              if(!buf2) {
                LexWarn(cc,"Unable to include file \"%q\".",cc->cur_str);
                goto lex_cont;
              }
              /*
              if(FileExtDot(buf2)) {
                  if(!StrCmp(FileExtDot(buf2),".BIN")) {
                    LoadBinFile(buf2);
                    Free(buf2);
                    goto lex_cont;
                  }
              }*/
              buf3=buf2;
              if(IsWindows) {
                if(StrOcc(buf2,'\\')) {
                  buf3=StrLastOcc(buf2,"\\");
                  *buf3++=0;
                  Cd(buf2);
                }
              } else {
                if(StrOcc(buf2,'/')) {
                  buf3=StrLastOcc(buf2,"/");
                  *buf3++=0;
                  Cd(buf2);
                }
              }
              LexIncludeStr(cc,buf2=FileNameAbs(buf3),FileRead(buf3,NULL),TRUE);
              Free(buf2);
              break;
            } else if(!StrCmp(cc->cur_str,"else")) {
                if (cc->flags & CCF_IN_IF) {
                    cc->token=TK_ELSE;
                    goto lex_end;
                }
lex_else:
                j=1;
                do {
                    if (ch=LexGetChar(cc)) {
                        if (ch=='#') {
                            if(Lex(cc)!=TK_IDENT) {
                                LexExcept(cc,"Expected identifier.");
                            }
                            if (
                                !StrCmp(cc->cur_str,"if")||
                                !StrCmp(cc->cur_str,"ifdef")||
                                !StrCmp(cc->cur_str,"ifaot")||
                                !StrCmp(cc->cur_str,"ifjit")||
                                !StrCmp(cc->cur_str,"ifndef")
                            )
                                j++;
                            else if (!StrCmp(cc->cur_str,"endif"))
                                j--;
                        }
                    } else {
                        cc->token=TK_EOF;
                        goto lex_end;
                    }
                } while (j);
            } else if(!StrCmp(cc->cur_str,"if")) {
                if (cc->flags & CCF_IN_IF) {
                    cc->token=TK_IF;
                    goto lex_end;
                }
lex_if:
                cc->flags|=CCF_IN_IF;
                if (LexExpression(cc)) {
                    cc->flags&=~CCF_IN_IF;
                    return Lex(cc);
                } else {
                    cc->flags&=~CCF_IN_IF;
                    if (cc->token!=TK_ENDIF && cc->token!=TK_ELSE) {
                        if (cc->token==TK_IF || cc->token==TK_IFDEF ||
                                cc->token==TK_IFNDEF || cc->token==TK_IFAOT ||
                                cc->token==TK_IFJIT)
                            j=2;
                        else
                            j=1;
                        do {
                            if (ch=LexGetChar(cc)) {
                                if (ch=='#') {
                                    if (!Lex(cc))
                                        goto lex_end;
                                    if (!StrCmp(cc->cur_str,"if")||
                                            !StrCmp(cc->cur_str,"ifdef")||
                                            !StrCmp(cc->cur_str,"ifaot")||
                                            !StrCmp(cc->cur_str,"ifjit")||
                                            !StrCmp(cc->cur_str,"ifndef"))
                                        j++;
                                    else if (!StrCmp(cc->cur_str,"endif"))
                                        j--;
                                    else if (!StrCmp(cc->cur_str,"else") && j==1)
                                        break;
                                }
                            } else {
                                cc->token=TK_EOF;
                                goto lex_end;
                            }
                        } while (j);
                    }
                }
                break;
            } else if(!StrCmp(cc->cur_str,"ifdef")) {
                if (cc->flags & CCF_IN_IF) {
                    cc->token=TK_IFDEF;
                    goto lex_end;
                }
lex_ifdef:
                cc->flags|=CCF_NO_DEFINES;
                if (!Lex(cc)) {
                    cc->flags&=~CCF_NO_DEFINES;
                    goto lex_end;
                }
                cc->flags&=~CCF_NO_DEFINES;
                if (cc->token!=TK_IDENT)
                    goto lex_end;
                if (cc->hash_entry)
	                 goto lex_cont;

                j=1;
                do {
                    if (ch=LexGetChar(cc)) {
                        if (ch=='#') {
                            if (!Lex(cc))
                                goto lex_end;
                            if (!StrCmp(cc->cur_str,"if")||
                                    !StrCmp(cc->cur_str,"ifdef")||
                                    !StrCmp(cc->cur_str,"ifaot")||
                                    !StrCmp(cc->cur_str,"ifjit")||
                                    !StrCmp(cc->cur_str,"ifndef"))
                                j++;
                            else if (!StrCmp(cc->cur_str,"endif"))
                                j--;
                            else if (!StrCmp(cc->cur_str,"else")&& j==1)
                                break;
                        }
                    } else {
                        cc->token=TK_EOF;
                        goto lex_end;
                    }
                } while (j);
                break;
            } else if(!StrCmp(cc->cur_str,"ifndef")) {
                if (cc->flags & CCF_IN_IF) {
                    cc->token=TK_IFNDEF;
                    goto lex_end;
                }
lex_ifndef:
                cc->flags|=CCF_NO_DEFINES;
                if (!Lex(cc)) {
                    cc->flags&=~CCF_NO_DEFINES;
                    goto lex_end;
                }
                cc->flags&=~CCF_NO_DEFINES;
                if (cc->token!=TK_IDENT)
                    goto lex_end;
	              if (!cc->hash_entry)
	                 goto lex_cont;
                j=1;
                do {
                    if (ch=LexGetChar(cc)) {
                        if (ch=='#') {
                            if (!Lex(cc))
                                goto lex_end;
                            if (!StrCmp(cc->cur_str,"if")||
                                    !StrCmp(cc->cur_str,"ifdef")||
                                    !StrCmp(cc->cur_str,"ifaot")||
                                    !StrCmp(cc->cur_str,"ifjit")||
                                    !StrCmp(cc->cur_str,"ifndef"))
                                j++;
                            else if (!StrCmp(cc->cur_str,"endif"))
                                j--;
                            else if (!StrCmp(cc->cur_str,"else") && j==1)
                                break;
                        }
                    } else {
                        cc->token=TK_EOF;
                        goto lex_end;
                    }
                } while (j);
                break;
            }  else if(!StrCmp(cc->cur_str,"endif")) {
                if (cc->flags & CCF_IN_IF) {
                    cc->token=TK_ENDIF;
                    goto lex_end;
                }
            } else if(!StrCmp(cc->cur_str,"assert")) {
                if (!Lex(cc))
                    goto lex_end;
                if (!LexExpression(cc))
                    LexExcept(cc,"Assert Failed ");
                goto lex_end;
            } else if(!StrCmp(cc->cur_str,"exe")) {
                tmpe=cc->exe_stream;
                (cc->exe_stream=(MAlloc(sizeof(CExeBlk))))->text=StrNew("");
                if(tmpe) tmpe->next=cc->exe_stream;
                cc->exe_stream->last=tmpe;
                __LexExe(cc);
                cc->flags|=CCF_USE_EXE_TEXT;
            } else {
                LexExcept(cc,"Invalid preproccessor directive \"%s\".",cc->cur_str);
            }
        }
    }
lex_end:
    LexGetChar(cc); //Do this so WAS_NEW_LINE is right
    cc->flags|=CCF_USE_LAST_U16;
    return cc->token;
}
U0 DbgPrintToken(CCmpCtrl *cc,I64 t) {
  switch(t) {
    case TK_EOF: "<EOF>\n"; break;
    case TK_IDENT: "<TK_IDENT:%s>\n",cc->cur_str; break;
    case TK_STR: "<STR:\"%q\">\n",cc->cur_str; break;
    case TK_CHAR_CONST: "<CC:%c>\n",cc->cur_i64; break;
    case TK_I64: "<I64:%d>\n",cc->cur_i64; break;
    case TK_F64: "<F64:%n>\n",cc->cur_f64; break;
    case TK_PLUS_PLUS: "++\n"; break;
    case TK_MINUS_MINUS: "--\n"; break;
    case TK_DEREFERENCE: "->\n"; break;
    case TK_DBL_COLON: "::\n"; break;
    case TK_SHL: "<<\n"; break;
    case TK_SHR: ">>\n"; break;
    case TK_EQU_EQU: "==\n"; break;
    case TK_NOT_EQU: "!=\n";break;
    case TK_GREATER_EQU: ">=\n"; break;
    case TK_AND_AND: "&&\n"; break;
    case TK_OR_OR: "||\n"; break;
    case TK_XOR_EQU: "^=\n"; break;
    case TK_SHL_EQU: "<<=\n"; break;
    case TK_SHR_EQU: ">>=\n"; break;
    case TK_MUL_EQU: "*=\n"; break;
    case TK_DIV_EQU: "/=\n"; break;
    case TK_AND_EQU: "&=\n"; break;
    case TK_SUB_EQU: "-=\n"; break;
    case TK_IF: "if\n"; break;
    case TK_ELSE: "else\n"; break;
    case TK_MOD_EQU: "%=\n"; break;
    case TK_DOT_DOT: "..\n"; break;
    case TK_ELLIPSIS: "...\n"; break;
    break;
    default:
      "%c\n",t;
  }
}
Bool LexSetReplMode(CCmpCtrl *ctrl,Bool set) {
    Bool ret=!!(ctrl->flags&CCF_REPL_MODE);
    if(set)
        ctrl->flags|=CCF_REPL_MODE;
    else
        ctrl->flags&=~CCF_REPL_MODE;
    return ret;
}
U8 *LexMatchFile(CCmpCtrl *cc,U8 *name) {
    I64 cnt=cc->file_lines->mask+1;
    U8 *delim,*bptr,*bptr2;
    //delim="\\";
    delim="/";
    while(--cnt>=0) {
        CFileLines *tmph=cc->file_lines->body[cnt];
        for(;tmph;tmph=tmph->next) {
            bptr2=tmph->str;
            while(bptr2=StrFirstOcc(bptr2,delim)) {
                if(!StrCmp(++bptr2,name)) return tmph->str;
            }
            if(!StrCmp(tmph->str,name)) return tmph->str;
        }
    }
    return NULL;
}
I64 *LexFileLines(CCmpCtrl *cc,U8 *name,I64 *cnt) {
    CFileLines *lns=HashFind(name,cc->file_lines);
    if(cnt) *cnt=0;
    if(!lns) return NULL;
    if(cnt) *cnt=MSize(lns->line_starts)/sizeof(I64);
    return lns->line_starts;
}
U0 StreamPrint(U8 *fmt,...) {
    CCmpCtrl *last_cc=Fs->last_cc;
    U8 *buf=StrPrintJoin(NULL,fmt,argc,argv),*st;
    if(last_cc) {
        if(!last_cc->exe_stream) {
           //TODO
        } else {
          st=MStrPrint("%s%s",last_cc->exe_stream->text,buf);
          Free(last_cc->exe_stream->text);
          last_cc->exe_stream->text=st;
        }
    }
    Free(buf);
}
