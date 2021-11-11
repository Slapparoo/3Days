#include "KEYS2.HC"
#include "ROPE.HC"
#include "FONT.HC"
#define EDIT_UNUSED 0
#define EDIT_INS_STR 3
#define EDIT_DEL_STR 4
class CUndoInfo {
    I64 type;
    I64 at;
    union {
        U8 inserted_char;
        U8 removed_char;
        U8* inserted_str;
        U8* removed_str;
    };
};
#define UNDO_INFO_LENGTH 512
#define TAG_MACRO 1
#define TAG_UNION 2
#define TAG_CLASS 3
#define TAG_VAR 4
#define TAG_FUNC 5
CHashTable *TagFiles=HashTableNew(1<<7);
class CTrie {
    CTrie *ents[26+10+1+1]; //a...z/0-9/_/.
    U8 *fn; //From TagFiles
    U8 *fullname;
    U8 * name;
    I64 ln;
    U8 kind;
};
I64 TrieChrIdx(U8 chr) {
    I64 idx;
    switch(chr) {
    case '0'...'9':
        idx=26+chr-'0';
        break;
    case 'a'...'z':
        idx=chr-'a';
        break;
    case 'A'...'Z':
        idx=chr-'A';
       break;
    case '_':
        idx=26+10;
        break;
    case '.':
        idx=26+10+1;
        break;
    default:
      idx=-1;
    }
    return idx;
}
CTrie *TrieIns(CTrie *t,U8 *name,CTrie * child) {
    if(!*name) {
      t->name=StrNew(child->name);
      t->fullname=child->fullname;
      t->fn=child->fn;
      t->ln=child->ln;
      t->kind=child->kind;
      Free(child);
      return child;
    }
    I64 idx=TrieChrIdx(*name);
    CTrie *b=t->ents[idx];
    if(!b)
        t->ents[idx]=MAlloc(sizeof(CTrie));
    TrieIns(t->ents[idx],name+1,child);
    return t;
}
U0 FreeTrie(CTrie *t) {
  if(!t) return;
  I64 cnt=sizeof(t->ents)/sizeof(*t->ents);
  while(--cnt>=0) {
    FreeTrie(t->ents[cnt]);
  }
  Free(t->name);
  Free(t->fullname);
  Free(t);
}
CTrie *TagNew(U8 *fullname,U8 *name,U8 *fn,I64 ln,U8 kind) {
    CTrie *t=MAlloc(sizeof(CTrie));
    t->name=StrNew(name);
    t->fullname=StrNew(fullname);
    t->ln=ln;
    t->kind=kind;
loop:
    CHash *ff=HashFind(fn,TagFiles);
    if(ff) {
        t->fn=ff->str;
    } else {
        CHash *new=MAlloc(sizeof(CHash));
        new->str=StrNew(fn);
        HashAdd(new,TagFiles);
        goto loop;
    }
    return t;
}
CTrie *ReadTags(U8 *file) {
    CTrie *ret=MAlloc(sizeof(CTrie));
    I64 len;
    U8 *text=FileRead(file,&len);
    U8 *en=text+len,*fullname;
    U8 buf1[256],buf2[256],buf3[256];
    U8 *_buf1=buf1,*_buf2=buf2,*_buf3=buf3;
    I64 ln;
loop:
    if(text!=en) {
        text=StrScan(text,"%s\t%s\t",&_buf1,&_buf2);
        ln=Str2I64(text,,&text);
        fullname=StrNew(buf1);
        U8 type=TAG_VAR;
        if(*text==';') {
            text++;
attrloop:
            if(*text=='\t') {
                text++;
                if(0==StrNCmp("kind:",text,len=StrLen("kind:"))) {
                    text+=len;
                    switch(*text) {
                    case 'c':
                        type=TAG_CLASS;
                        break;
                    case 'm':
                        type=TAG_MACRO;
                        break;
                    case 'f':
                        type=TAG_FUNC;
                        break;
                    case 'u':
                        type=TAG_UNION;
                        break;
                    case 'v':
                        type=TAG_VAR;
                        break;
                    default:
                        throw('InvType');
                    }
                    text++;
                    goto attrloop;
                }
                if(0==StrNCmp("struct:",text,len=StrLen("struct:"))) {
                    text+=len;
                    U8 *tdelim=StrFirstOcc(text,"\x0d\n\t");
                    U8 *ofclass=StrNCpy(MAlloc(tdelim-text+1),text,tdelim-text);
                    ofclass[tdelim-text]=0;
                    Free(fullname);
                    fullname=MStrPrint("%s.%s",ofclass,buf1);
                    text=tdelim;
                    goto attrloop;
                }
                if(0==StrNCmp("union:",text,len=StrLen("union:"))) {
                    text+=len;
                    tdelim=StrFirstOcc(text,"\x0d\n\t");
                    ofclass=StrNCpy(MAlloc(tdelim-text+1),text,tdelim-text);
                    ofclass[tdelim-text]=0;
                    Free(fullname);
                    fullname=MStrPrint("%s.%s",ofclass,buf1);
                    text=tdelim;
                    goto attrloop;
                }
            } else if(*text==0x0d) {
              //Cariage return
              text++;
              if(*text++=='\n')
                  goto ins;
            }
            else if(*text++=='\n')
                goto ins;
            else {
              "%s\n",text;
              throw('Tag');
            }
        } else if(*text++!='\n') {
            "%s\n",text;
            throw('Tag');
        }
ins:
        TrieIns(ret,fullname,TagNew(fullname,buf1,buf2,ln,type));
        Free(fullname);
        goto loop;
    }
en:
    Free(text);
    return ret;
}
#define DIAG_WARN 0
#define DIAG_ERR 1
class CDiag {
    CDiag *prev,*next;
    I64 type,ln,col;
    //From TagFiles
    U8 *fn,*msg;
};
CDiag *ParseDiags(U8 *file) {
    I64 cnt,len;
    U8 *text=FileRead(file,&cnt);
    U8 *ptr=text;
    CDiag *prev=NULL;
    while(*ptr) {
        CDiag *new=MAlloc(sizeof(CDiag));
        new->prev=prev;
        if(prev) prev->next=new;
        new->fn=MAlloc(1024);
        try {
            //fn:ln:col [WARNING|ERROR] [msg]
            //Filename can contain ':' so ensure a number follows the ':'
            U8 *fnstart=ptr;
            loop:
            U8 *colon=StrFirstOcc(ptr,":");
            if(!colon) goto fail;
            if(Bt(&char_bmp_dec_numeric,colon[1])) {
                StrNCpy(new->fn,fnstart,colon-fnstart);
                ptr=colon+1;
            } else {
                ptr=colon+1;
                goto loop;
            }
            ptr=StrScan(ptr,"%d:%d: ",&new->ln,&new->col);
            if(0==StrNCmp(ptr,"WARNING",len=StrLen("WARNING"))) {
                ptr+=len;
                new->type=DIAG_WARN;
            } else if(0==StrNCmp(ptr,"ERROR",len=StrLen("ERROR"))) {
                ptr+=len;
                new->type=DIAG_ERR;
            }
            U8 *nl=StrFirstOcc(ptr,"\n");
            new->msg=StrNCpy(MAlloc(nl-ptr+1),ptr,nl-ptr);
            prev=new;
            ptr=nl+1;
        } catch {
            Fs->catch_except=TRUE;
            goto fail;
        }
    }
    CDiag *first=prev;
    while(prev) {first=prev;prev=prev->prev;}
    Free(text);
    return first;
    fail:
    Free(text);
    return NULL;
}
U0 FreeDiags(CDiag *errs) {
  if(!errs) return;
  FreeDiags(errs->next);
  Free(errs->fn);
  Free(errs->msg);
  Free(errs);
}
U0 __FuzzyMatch(CTrie *trie,U8 *name,I64 *cnt,CTrie **dumpto) {
    if(!trie) return;
    if(!*name) {
        if(trie->name) {
            if(dumpto)
                dumpto[*cnt]=trie;
            ++*cnt;
        }
    }
    I64 idx=-1;
    if(*name)
        idx=TrieChrIdx(*name);
    I64 cnt2=sizeof(trie->ents)/sizeof(*trie->ents);
    while(--cnt2>=0) {
        if(cnt2!=idx)
            __FuzzyMatch(trie->ents[cnt2],name,cnt,dumpto);
        else
            __FuzzyMatch(trie->ents[cnt2],name+1,cnt,dumpto);
    }
}
I64 MatchCmp(U0 *a,U0 *b) {
    CTrie **A=a,**B=b;
    return StrLen(A[0]->fullname)-StrLen(B[0]->fullname);
}
CTrie **FuzzyMatch(CTrie *trie,U8 *name,I64 *cnt=NULL) {
    I64 cnt2=0;
    __FuzzyMatch(trie,name,&cnt2,NULL);
    CTrie **ents=MAlloc(sizeof(CTrie*)*(cnt2+1));
    cnt2=0;
    __FuzzyMatch(trie,name,&cnt2,ents);
    if(cnt) *cnt=cnt2;
    ents[cnt2]=NULL;
    QSort(ents(U8*),cnt2,sizeof(CTrie*),&MatchCmp);
    return ents;
}
CHashTable *keywords=HashTableNew(1<<5);
U0 AddKeyword(U8i *text) {
    CHash *h=MAlloc(sizeof(CHash));
    h->str=StrNew(text);
    h->type=1;
    HashAdd(h,keywords);
}
U8 *kws[]= {
    "union",
    "catch",
    "class",
    "try",
    "if",
    "else",
    "for",
    "while",
    "extern",
    "_extern",
    "return",
    "sizeof",
    "intern",
    "do",
    "goto",
    "break",
    "switch",
    "start",
    "end",
    "case",
    "default",
    "public",
    "import",
    "_import",
    "lastclass",
    "static",
    "U0",
    "U8i",
    "I8i",
    "U16i",
    "I16i",
    "U32i",
    "I32i",
    "U64i",
    "I64i",
    "F64",
    "Bool",
    "#include",
    "#exe",
    "#define",
    "#assert",
    "#if",
    "#else",
    "#endif",
    "#ifdef",
    "#ifndef",
};
I64 cnt=sizeof(kws)/sizeof(*kws);;
while(--cnt>=0) {
    AddKeyword(kws[cnt]);
}
#define ED_RET_OK 1
#define ED_RET_QUIT 2

#define ED_LNF_COMMENTED_OUT 1
#define ED_LNF_COMMENTED_START (1<<1)
#define ED_LNF_COMMENTED_END (1<<2)
#define ED_LNF_ERR (1<<3)
#define ED_LNF_WARN (1<<4)
class CLine {
    CLine *prev,*next;
    I64 line,length,flags;
    CDiag *diag;
    //Used with ED_LNF_COMMENTED_START
    CLine *commentStart;
    CRope *text;
    //Will be invalided when CEditor.line_cache_size is lesser than line number
    I64 cached_offset;
};
I64 IndentLevel(CLine *ln) {
    U8 *txt=Rope2Str(ln->text);
    I64 cnt=0;
    while(txt[cnt]==' ') cnt++;
    cnt/=4;
    Free(txt);
    return cnt;
}
//Color pairs
#define CP_BLANK 0
#define CP_KW 1
#define CP_WORD 2
#define CP_TOK 3
#define CP_NUM 4
#define CP_STR 5
#define CP_WHITE 6
#define CP_LINUM 7
#define CP_COMMENT 8
#define CP_AC_SEL 9
#define CP_AC_UNSEL 10
#define CP_MENU_SEL 11
#define CP_MENU_UNSEL 12
#define CP_FUZZY_SEL_A 13
#define CP_FUZZY_UNSEL_A 14
#define CP_FUZZY_SEL_B 15
#define CP_FUZZY_UNSEL_B 16
#define CP_LINUM_ERR 17
#define CP_LINUM_WARN 18
#define CP_CURSOR 19
//TODO
#define SOL_BLACK   0x000000
#define SOL_WHITE   0xfdf6e3
#define SOL_YELLOW  0xb58900
#define SOL_ORANGE  0xcb4b16
#define SOL_RED     0xdc322f
#define SOL_MAGENTA 0xd33682
#define SOL_BLUE    0x286bd2
#define SOL_CYAN    0x2aa198
#define SOL_GREEN   0x859900
U32 union U_RGBA8888 {
    class {
    U8 b;
    U8 g;
    U8 r;
    U8 a;
    };
};
class CSDL_ColorPair {
    U_RGBA8888 fg;
    U_RGBA8888 bg;
};
CSDL_ColorPair SDL_ColorPairs[64];
SDL_Window *global_window=NULL;
SDL_Surface *global_font=NULL;
#define FONT_X 8
#define FONT_Y 8
U0 SDL_InitScr() {
    global_window=SDL_CreateWindow("HolyEd",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,640,480,SDL_WINDOW_RESIZABLE);
    global_font=SDL_CreateRGBSurface(0,FONT_X*0x100,FONT_Y,32,0,0,0,0);
    SDL_FillRect(global_font, NULL, 0x00000000);
    I64 cnt=0x100;
    while(--cnt>=0) {
        I64 top=8;
        while(--top>=0) {
            I64 bit=0;
            U8 byte=sys_font_std[cnt].u8[top];
            for(;bit!=8;bit++) {
                if(Bt(&byte,bit)) {
                    SDL_Rect pixel={cnt*8+bit,top,1,1};
                    SDL_FillRect(global_font,&pixel,0xffffffff);
                }
            }
        }
    }
    SDL_SetColorKey(global_font,1,0x00000000);
    SDL_StartTextInput;
}
class CSDL_ScrnChr {
    U8 chr;
    I8 cp; //negative for inverse
};
class CSDL_TextWin {
    SDL_Surface *text;
    U8 cur_cp; //From SDL_ColorPairs
    I64 x,y,begx,begy,curx,cury;
    //The charactors on the screen.
    CSDL_ScrnChr *char_cache;
    Bool cur_enabled;
    Bool inverse;
};
#define FONT_X 8
#define FONT_Y 8
#define FONT_X2 16
#define FONT_Y2 16
CSDL_TextWin *SDL_NewTextWin(I64 w,I64 h) {
    CSDL_TextWin *ret=MAlloc(sizeof(CSDL_TextWin));
    ret->text=SDL_CreateRGBSurface(0,w*FONT_X2,h*FONT_Y2,32,0,0,0,0);
    ret->cur_cp=CP_WHITE;
    ret->char_cache=MAlloc(sizeof(CSDL_ScrnChr)*w*h);
    return ret;
}
U0 SDL_DestroyTextWin(CSDL_TextWin *win) {
    Free(win->char_cache);
    SDL_FreeSurface(win->text);
    Free(win);
}
I64 SDL_WinX(CSDL_TextWin *w) {
    return SDL_GetSurfaceWidth(w->text)/FONT_X2;
}
I64 SDL_WinY(CSDL_TextWin *w) {
    return SDL_GetSurfaceHeight(w->text)/FONT_Y2;
}
//Pass CSDL_TextWin's
U0 SDL_DrawWin(...) {
    I64 idx=0;
    I32 winx,winy;
    SDL_GetWindowSize(global_window,&winx,&winy);
    winx/=FONT_X2,winy/=FONT_Y2;
    SDL_Surface *winsurf=SDL_GetWindowSurface(global_window);
    SDL_FillRect(winsurf,NULL,SOL_WHITE);
    for(;idx!=argc;idx++) {
        CSDL_TextWin *w=argv[idx];
        I32 rx=SDL_GetSurfaceWidth(w->text),ry=SDL_GetSurfaceHeight(w->text);
        SDL_Rect sbox={w->begx*FONT_X2,w->begy*FONT_Y2,rx/FONT_X2*FONT_X2,ry/FONT_Y2*FONT_Y2};
        SDL_BlitSurface(w->text,NULL,winsurf,&sbox);
        if(rx/FONT_X2!=winx||ry/FONT_Y2!=winy) {
            "RESETING\n";
            SDL_FreeSurface(w->text);
            w->text=SDL_CreateRGBSurface(0,winx*FONT_X2,winy*FONT_Y2,32,0,0,0,0);
            Free(w->char_cache);
            w->char_cache=MAlloc(sizeof(CSDL_ScrnChr)*winx*winy);
        }
        if(w->cur_enabled) {
            SDL_Rect crect={w->curx*FONT_X2,w->cury*FONT_Y2+FONT_Y2-2,FONT_X2,4};
            SDL_FillRect(winsurf,&crect,0xff000000);
        }
    }
    SDL_UpdateWindowSurface(global_window);
}
U0 SDL_AddCh(CSDL_TextWin *win,U8 ch,Bool force_in_view=FALSE) {
    I64 w=SDL_WinX(win),h=SDL_WinY(win);
    if(win->x>=w) goto next;
    if(win->y>=h) goto next;
    if(force_in_view&&ch!='\n') {
        if(win->x+1>=w) return;
    }
    if(ch=='\n') goto nxtln;
    I64 mult=1;
    if(win->inverse) mult=-1;
    if((win->char_cache[win->y*w+win->x].chr==ch)&&(win->char_cache[win->y*w+win->x].cp==mult*win->cur_cp)) goto next;
    SDL_Rect fsrc={ch*FONT_X,0,FONT_X,FONT_Y};
    SDL_Rect dstbox={win->x*FONT_X2,win->y*FONT_Y2,FONT_X2,FONT_Y2};
    U_RGBA8888 bg=SDL_ColorPairs[win->cur_cp].bg;
    U_RGBA8888 fg=SDL_ColorPairs[win->cur_cp].fg;
    if(win->inverse) {
        U_RGBA8888 tmp=bg;
        bg=fg;
        fg=tmp;
    }
    SDL_SetSurfaceColorMod(global_font,fg.r,fg.g,fg.b);
    SDL_FillRect(win->text,&dstbox,bg);
    SDL_BlitScaled(global_font,&fsrc,win->text,&dstbox);
    win->char_cache[win->y*w+win->x].chr=ch;
    win->char_cache[win->y*w+win->x].cp=win->cur_cp*mult;
    next:
    if(++win->x>=w) {
        nxtln:
        win->x=0,win->y++;
    }
    if(win->y<h) {
    } else {
        win->y=h-1;
    }
}
U0 SDL_Print(CSDL_TextWin *win,U8 *str) {
    I64 len=StrLen(str),idx=0;
    for(;idx!=len;idx++) {
        SDL_AddCh(win,str[idx],TRUE);
    }
}
U0 SDL_ClrToEOL(CSDL_TextWin *w) {
    I64 width=SDL_WinX(w)-w->x;
    U_RGBA8888 white=SOL_WHITE;
    SDL_Rect r={w->x*FONT_X2,w->y*FONT_Y2,width*FONT_X2,FONT_Y2};
    SDL_LockSurface(w->text);
    SDL_FillRect(w->text,&r,SOL_WHITE);
    SDL_UnlockSurface(w->text);
    I64 idx=w->y*SDL_WinX(w)+w->x;
    w->cur_cp=CP_WHITE;
    while(--width>=0)
        SDL_AddCh(w,' ');
}
class CEditor {
    CSDL_TextWin *window;
    CUndoInfo undo_info[UNDO_INFO_LENGTH];
    I64 undo_info_cur;
    I64 undo_info_end;
    I64 undo_info_start;
    Bool *dirty_screen_lines;
    I64 vp_start_line;
    I64 x_scroll;
    I64 curx,cury;
    //The expected x position,,we can enter a line that isnt as long as our expected curx
    I64 _curx;
    I64 h,w;
    CLine *lines;
    I64 line_count;
    //
    CLine **line_cache;
    I64 line_cache_size;
    //
    CTrie *tags;
    CDiag *diags;
    U8 *fn;
    I64 margin_top,margin_bottom;
    I64 sel_start,sel_end;
};
U8 *EdGetSelText(CEditor *ed);
U0 EdDelSel(CEditor *ed);
U0 MarkLineColors(CEditor *ed);
U0 WriteWithinWin(CSDL_TextWin *win,U8 *text,I64 x_scroll=0);
I64 OpenFile(U8 *name,I64 ln=0);
I64 ShowDiagsWindow(CEditor *parent) {
    CDiag *first=parent->diags;
    I64 y_scroll=0,x_scroll=0,curln=0,ret=ED_RET_OK;
    CDiag *iter=first;
    I64 cnt=0;
    while(iter) cnt++,iter=iter->next;
    iter=first;
    CDiag **lines=MAlloc(cnt*sizeof(CDiag*));
    cnt=0;
    while(iter) {lines[cnt++]=iter;iter=iter->next;}
    parent->window->cur_cp=CP_WHITE;
    CSDL_TextWin *win=parent->window;
    draw:
    I64 h=getmaxy(stdscr);
    I64 cap=MinI64(y_scroll+h,cnt);
    I64 cnt2=0;
    dloop:
    if(cnt2+y_scroll>=cnt) {
    clear:
      while(cnt2<SDL_WinY(win)) {
        win->y=cnt2++;
        win->x=0;
        SDL_ClrToEOL(win);
      }
      goto inp;
    }
    if(cnt2+y_scroll==curln) {
      win->cur_cp=CP_FUZZY_SEL_B;
    } else {
      win->cur_cp=CP_FUZZY_UNSEL_A;
    }
    win->y=cnt2,win->x=0;
    I64 off=-x_scroll;
    CDiag *cur=lines[cnt2++ +y_scroll];
    U8 *lncol=MStrPrint("%s:%d:%d: ",cur->fn,cur->ln,cur->col);
    WriteWithinWin(win,lncol,off);
    off+=StrLen(lncol);
    Free(lncol);

    if(cur->type==DIAG_ERR) {
        WriteWithinWin(win,"ERROR",off);
        //TODO COLOR
        off+=StrLen("ERROR");
    } else {
        WriteWithinWin(win,"WARNING",off);
        //TODO COLOR
        off+=StrLen("WARNING");
    }
    if(StrFirstOcc(cur.msg,"\r\n")) *StrFirstOcc(cur.msg,"\n\r")=0;
    WriteWithinWin(win,cur->msg,off);
    SDL_ClrToEOL(win);
    if(cnt2+y_scroll<cap) goto dloop;
    goto clear;
    inp:
    SDL_DrawWin(win);
    I64 key=GetKey();
    switch(key) {
        case ED_KEY_ESCAPE: goto exit;
        case ERR: goto inp;
        case ED_KEY_PAGEUP:
        curln=MaxI64(0,curln-SDL_WinY(win));
        break;
        case ED_KEY_PAGEDOWN:
        curln=MinI64(cnt,curln+SDL_WinY(win));
        break;
        case ED_KEY_RIGHT: ++x_scroll; break;
        case ED_KEY_LEFT: x_scroll=MaxI64(0,--x_scroll); break;
        case ED_KEY_UP: curln=MinI64(MaxI64(0,--curln),cnt); break;
        case ED_KEY_DOWN: curln++; break;
        case '\n':
        if(curln<cnt) {
            ret=OpenFile(lines[curln]->fn,lines[curln]->ln);
            goto exit;
        } else {
            goto exit;
        }
        break;
        case ED_KEY_REDRAW:
        case ED_KEY_RESIZE:
        SDL_DrawWin(win);
        break;
    }
    if(curln<y_scroll) y_scroll=curln;
    else if(curln>=SDL_WinY(win)+y_scroll) y_scroll=curln-SDL_WinY(win)+1;
    goto draw;
    exit:
    Free(lines);
    return ret;
}

I64 UndoBufSize(CEditor* ed) {
    if (ed->undo_info_end > ed->undo_info_start)
    return ed->undo_info_end - ed->undo_info_start;
    else
    return (UNDO_INFO_LENGTH - ed->undo_info_start) + ed->undo_info_end;
}
U0 InsUndoInfo(CUndoInfo* info, CEditor* ed) {
    ed->undo_info_end = (ed->undo_info_cur + ed->undo_info_start) % UNDO_INFO_LENGTH;

    if (ed->undo_info_cur != UNDO_INFO_LENGTH) {
        ed->undo_info_cur++;
    } else if (ed->undo_info_end == ed->undo_info_start) {
        ed->undo_info_start = (ed->undo_info_start + 1) % UNDO_INFO_LENGTH;
    }

    //Destroy old value
    switch (ed->undo_info[ed->undo_info_end]->type) {
        case EDIT_DEL_STR:
        Free(ed->undo_info[ed->undo_info_end]->removed_str);
        break;

        case EDIT_INS_STR:
        Free(ed->undo_info[ed->undo_info_end]->inserted_str);
        break;
    }

    ed->undo_info[ed->undo_info_end] = *info;
    ed->undo_info_end = (ed->undo_info_cur + ed->undo_info_start) % UNDO_INFO_LENGTH;
}
CUndoInfo* GetUndoInfo(CEditor* ed) {
    if ((ed->undo_info_cur + ed->undo_info_start) % UNDO_INFO_LENGTH == ed->undo_info_end)
    if (ed->undo_info_cur)
    return NULL;

    I64 i = (ed->undo_info_cur + ed->undo_info_start) % UNDO_INFO_LENGTH;
    I64 s = ed->undo_info_start;
    I64 e = ed->undo_info_end;

    if (s == e) goto en;

    if (s < e) {
        if (!(s <= i < e)) return NULL;
    } else if (e <= i < s) return NULL;

    en:

    if (ed->undo_info[i]->type == EDIT_UNUSED)
    return NULL;

    return &ed->undo_info[i];
}
class CBuffer:CHash {
  CEditor *ed;
};
CHashTable *buffers=HashTableNew(1<<5);
U0 FocusCursor(CEditor *ed,I64 lmargin=0) {
    //https://stackoverflow.com/questions/1811955/ncurses-terminal-size
    I64 w=SDL_WinX(ed->window)-lmargin;
    if(ed->curx<ed->x_scroll) {
        ed->x_scroll=ed->curx;
    } else if(ed->x_scroll+w<=ed->curx) {
        ed->x_scroll=ed->curx-w+1;
    }
    if(ed->cury<ed->vp_start_line) {
        ed->vp_start_line=ed->cury;
    } else if(ed->vp_start_line+SDL_WinY(ed->window)-ed->margin_top<=ed->cury) {
        ed->vp_start_line=ed->cury-SDL_WinY(ed->window)+1+ed->margin_top;
    }
}
I64 GetLinum(CEditor *ed,CLine *ln) {
    CLine *s=ed->lines;
    I64 num=0;
    while(s) {
        if(s==ln)
            return num;
        num++;
        s=s->next;
    }
    return -1;
}
I64 GetLineCount(CEditor *ed) {
  I64 ret=0;
  CLine *ln=ed->lines;
  while(ln) {
    ln=ln->next,ret++;
  }
  return ret;
}
CLine *GetLine(CEditor *ed,I64 ln) {
    if(ed->line_cache_size>ln)
      return ed->line_cache[ln];
    CLine *s=ed->lines;
    if(s->cached_offset!=0) throw('');
    I64 num=0,off=0;
    if(MSize(ed->line_cache)/sizeof(CLine*)<=ln) {
      I64 pow2=1;
      while((1<<pow2)<=ln) pow2++;
      CLine **new=MAlloc((1<<pow2)*sizeof(CLine*));
      MemNCpy(new,ed->line_cache,sizeof(CLine*)*ed->line_cache_size);
        if(ed->line_cache_size){
         s=ed->line_cache[ed->line_cache_size-1];
         off=s->cached_offset;
         num=ed->line_cache_size-1;
        }
        Free(ed->line_cache);
        ed->line_cache=new;
    }
    while(s) {
      ed->line_cache[num]=s;
      s->cached_offset=off;
        if(num==ln) {
          ed->line_cache_size=MaxI64(ln,ed->line_cache_size);
          return s;
        }
        off+=RopeLength(s->text);
        s=s->next;
        num++;
    }
    ed->line_cache_size=num;
    return NULL;
}
I64 GetLineOffset(CEditor *ed,I64 ln) {
    CLine *ln2=GetLine(ed,ln);
    if(ln2) return ln2->cached_offset;
    return -1;
}

U0 DirtyLinesBelow(CEditor *ed,I64 ln) {
    ed->line_cache_size=ln;
    ln-=ed->vp_start_line;
}
U0 WriteWithinWin(CSDL_TextWin *win,U8 *text,I64 x_scroll=0) {
    I64 l=StrLen(text)+x_scroll;
    I64 s=x_scroll;
    while(s!=l) {
        if(SDL_WinX(win)>=s>=0) {
           U8 chr=text[s-x_scroll];
           if(chr=='\t')
                SDL_AddCh(win,' ',TRUE);
           else if(chr!='\r'){
                SDL_AddCh(win,chr,TRUE);
           }
        }
        s++;
    }
}
U0 WriteWithinView(CEditor *ed,U8 *text,I64 off) {
    I64 l=StrLen(text);
    I64 s=off;
    I64 e=off+l;
    while(s!=e) {
        if(ed->x_scroll+SDL_WinX(ed->window)>=s>=ed->x_scroll) {
          if(text[s-off]=='\t')
            SDL_AddCh(ed->window,' ',TRUE);
          else if(text[s-off]!='\r')
            SDL_AddCh(ed->window,text[s-off],TRUE);
        }
        s++;
    }
}
Bool IsEscaped(U8 *sstart,U8 *chr) {
    if(chr-1>=sstart)
        if(chr[-1]=='\\')
            return !IsEscaped(sstart,chr-1);
    return FALSE;
}
CLine *SearchForMLCommmentStart(CLine *s) {
    while(s) {
        if(s->flags&ED_LNF_COMMENTED_START)
            if(!(s->flags&ED_LNF_COMMENTED_OUT))
                return s;
        s=s->prev;
    }
    return NULL;
}
CLine *SearchForMLCommmentEnd(CLine *s) {
    while(s) {
        if(s->flags&ED_LNF_COMMENTED_END)
            return s;
        s=s->next;
    }
    return NULL;
}
U0 __HighlightWrite(CEditor *ed,U8 *text,I64 off,I64 sel_start=I64_MAX,I64 sel_end=I64_MAX) {
    I64 len=StrLen(text),idx;
    for(idx=0;idx!=len;idx++) {
        if(off+idx>=ed->x_scroll) {
            if(sel_start<=off+idx<sel_end) {
                ed->window->inverse=TRUE;
            } else
                ed->window->inverse=FALSE;
            SDL_AddCh(ed->window,text[idx],TRUE);
        }
    }
    ed->window->inverse=FALSE;
}
U0 Highlight(CEditor *ed,CLine *ln,Bool dryRun=FALSE,Bool recur=TRUE) {
    I64 ln_off=GetLineOffset(ed,GetLinum(ed,ln));
    I64 sel_start=ed->sel_start;
    I64 sel_end=ed->sel_end;
    if(sel_start==sel_end) goto ignoresel;
    sel_start-=ln_off;
    sel_end-=ln_off;
    //Compute relative to line start
    if(sel_start<0) sel_start=0;
    if(sel_end<0) sel_end=0;
    if(sel_end>RopeLength(ln->text))
        sel_end=RopeLength(ln->text);
    if(sel_start>sel_end) SwapI64(&sel_start,&sel_end);
    ignoresel:
    U8 *text=Rope2Str(ln->text);
    U8 *nl=StrFirstOcc(text,"\n");
    if(nl) *nl=0;
    U8 *buf=MAlloc(StrLen(text)+1);
    I64 idx=0,ns=0;
    if(ln->prev) {
        if(!(ln->prev->flags&ED_LNF_COMMENTED_OUT))
            ln->flags&=~ED_LNF_COMMENTED_OUT;
        if(!(ln->flags&ED_LNF_COMMENTED_END))
            if((ln->prev->flags&ED_LNF_COMMENTED_OUT)||(ln->prev->flags&ED_LNF_COMMENTED_START))
                ln->flags|=ED_LNF_COMMENTED_OUT;
    }
    I64 isCommentedOut1=(ln->flags&ED_LNF_COMMENTED_OUT)||(ln->flags&ED_LNF_COMMENTED_START);
    I64 isMlEnd1=ln->flags&ED_LNF_COMMENTED_END,isMlStart1=ln->flags&ED_LNF_COMMENTED_START;
    ln->flags&=~ED_LNF_COMMENTED_START;
    if((ln->flags&ED_LNF_COMMENTED_END)||(ln->flags&ED_LNF_COMMENTED_OUT)) {
        while(text[idx]) {
            if(0==StrNCmp(text+idx,"*/",2)) {
                idx+=2;
                ed->window->cur_cp=CP_COMMENT;
                StrNCpy(buf,text,idx);
                buf[idx]=0;
                if(!dryRun) __HighlightWrite(ed,buf,0,sel_start,sel_end);
                ln->flags|=ED_LNF_COMMENTED_END;
                ln->flags&=~ED_LNF_COMMENTED_OUT;
                goto s;
            }
            idx++;
        }
        ed->window->cur_cp=CP_COMMENT;
        if(!dryRun) __HighlightWrite(ed,text,0,sel_start,sel_end);
        ln->flags&=~ED_LNF_COMMENTED_END;
        ln->flags|=ED_LNF_COMMENTED_OUT;
    }
s:
    U8 *orig=text;
loop:
    ns=idx;
    //Try lexing name
    if(!Bt(char_bmp_dec_numeric,text[idx]))
        while(Bt(char_bmp_alpha_numeric,text[idx])||text[idx]=='#') idx++;
    if(ns!=idx) {
        StrNCpy(buf,&text[ns],idx-ns);
        buf[idx-ns]=0;
        CHash *kw;
        if(kw=HashFind(buf,keywords)) {
            ed->window->cur_cp=CP_KW;
        } else {
            ed->window->cur_cp=CP_WORD;
        }
        if(!dryRun) __HighlightWrite(ed,buf,ns,sel_start,sel_end);
        goto loop;
    }

    if(text[idx]=='/') {
        if(text[idx+1]=='/') {
            StrCpy(buf,text+idx);
            ed->window->cur_cp=CP_COMMENT;
            if(!dryRun) __HighlightWrite(ed,text+ns,ns,sel_start,sel_end);
            idx=StrLen(text);
            goto loop;
        } else if(text[idx+1]=='*') {
            idx++;
            while(text[idx]) {
                if(0==StrNCmp(text+idx,"*/",2)) {
                    idx+=2;
                    StrNCpy(buf,text+ns,idx-ns);
                    buf[idx-ns]=0;
                    ed->window->cur_cp=CP_COMMENT;
                    if(!dryRun) __HighlightWrite(ed,buf,ns,sel_start,sel_end);
                    goto loop;
                }
                idx++;
            }
            ed->window->cur_cp=CP_COMMENT;
            if(!dryRun) __HighlightWrite(ed,text+ns,ns,sel_start,sel_end);
            ln->flags|=ED_LNF_COMMENTED_START;
            //Dry run hgihlight until we find a line end
            CLine *nxtln=ln->next;
            if(!recur) for(; nxtln;) {
                Highlight(ed, nxtln, TRUE); //Will set ED_LNF_COMMENTED_OUT;
                if(nxtln->flags&ED_LNF_COMMENTED_END) break;
                nxtln=nxtln->next;
            }
        }
    }
    //Try lexing a string
    if(StrOcc("'\"",text[idx])) {
        U8 term=text[idx++];
        U8 sseek[2]= {term,0};
sloop:
        U8 *sfind=StrFirstOcc(text+idx,sseek);
        if(!sfind) {
            ed->window->cur_cp=CP_STR;
            if(!dryRun) __HighlightWrite(ed,text+ns,ns,sel_start,sel_end);
            idx=StrLen(text);
        } else if(IsEscaped(text,sfind)) {
            idx=sfind-orig+1;
            goto sloop;
        } else {
            idx=sfind-orig+1;
            ed->window->cur_cp=CP_STR;
            StrNCpy(buf,text+ns,idx-ns);
            buf[idx-ns]=0;
            if(!dryRun) __HighlightWrite(ed,buf,ns,sel_start,sel_end);
            goto loop;
        }
    }

    //Lex numbers
    ns=idx;
    while (Bt(char_bmp_white_space,text[idx])) idx++;
    if(ns!=idx)  {
        StrNCpy(buf,text+ns,idx-ns);
        buf[idx-ns]=0;
        ed->window->cur_cp=CP_WHITE;
        if(!dryRun) __HighlightWrite(ed,buf,ns,sel_start,sel_end);
        goto loop;
    }
    U8 *idx2=text+idx;
    Str2I64(text+idx,,&idx2);
    if(idx2!=text+idx) {
        idx=idx2-text;
        StrNCpy(buf,text+ns,idx-ns);
        buf[idx-ns]=0;
        ed->window->cur_cp=CP_NUM;
        if(!dryRun) __HighlightWrite(ed,buf,ns,sel_start,sel_end);
        goto loop;
    }
    Str2F64(idx2,&idx2);
    if(text+idx!=idx2) {
        idx=idx2-text;
        StrNCpy(buf,text+ns,idx-ns);
        buf[idx-ns]=0;
        ed->window->cur_cp=CP_NUM;
        if(!dryRun) __HighlightWrite(ed,buf,ns,sel_start,sel_end);
        goto loop;
    }
    if(!text[idx]) goto en;
    ed->window->cur_cp=CP_TOK;
    buf[1]=0;
    buf[0]=text[idx];
    if(!dryRun) __HighlightWrite(ed,buf,ns,sel_start,sel_end);
    idx++;
    goto loop;
en:
    I64 isMlEnd2=ln->flags&ED_LNF_COMMENTED_END,isMlStart2=ln->flags&ED_LNF_COMMENTED_START;
    I64 isCommentedOut2=(ln->flags&ED_LNF_COMMENTED_OUT)||(ln->flags&ED_LNF_COMMENTED_START);
    if(recur&&(isMlEnd1!=isMlEnd2||isMlStart1!=isMlStart2)) {
        nxtln=ln->next;
        for(;nxtln;nxtln=nxtln->next) {
            Bool comm_end=nxtln->flags&ED_LNF_COMMENTED_END;
            Highlight(ed, nxtln, TRUE);
            if(isMlEnd2) { //Added '*/',quit at next '*/'
                if(comm_end) break;
            } else { //Removed '*/',quit at next '*/'
                if(comm_end) break;
            }
        }
    }
    if(recur&&isCommentedOut2!=isCommentedOut1) {
        nxtln=ln->next;
        for(;nxtln;nxtln=nxtln->next) {
            Highlight(ed,nxtln,TRUE);
            if((nxtln->flags&ED_LNF_COMMENTED_OUT)!=isCommentedOut2) break;
        }
    }
    ed->window->cur_cp=CP_WHITE;
    SDL_ClrToEOL(ed->window);
    Free(buf);
}
U0 __EditorInsText(CEditor *ed,U8 *text,I64 at) {
    U8 *buf;
    I64 ln_off=0;
    CLine *ln=ed->lines,*prev=NULL;
    I64 linum=0;
    while(ln) {
        if(ln_off<=at<ln_off+RopeLength(ln->text)) break;
        ln_off+=RopeLength(ln->text);
        prev=ln;
        ln=ln->next,linum++;
    }
    DirtyLinesBelow(ed,GetLinum(ed,ln));

    if(ln_off<=at&&!ln) {
        if(prev) {
            ln=prev;
            ln_off-=RopeLength(ln->text);
        } else {
            CLine *nl=MAlloc(sizeof(CLine));
            ed->line_count++;
            nl->prev=prev;
            if(prev) prev->next=nl;
            ln=nl;
            ln->text=RopeAppendText(NULL,"");
            if(!ed->lines) ed->lines=nl;
        }
    }
    ln->text=RopeInsText(ln->text, text, at-ln_off);
    ln->text=RopeCondense(ln->text);
    Highlight(ed, ln, TRUE);
    CLine *start_ln=ln;
loop:
    U8 *text2=Rope2Str(ln->text);
    U8 *nl3=StrFirstOcc(text2,"\n");
    if(nl3)
        if(nl3[1]) {
            CRope *right;
            RopeSplit(ln->text,  nl3-text2+1, &ln->text, &right);
            //Insert a blank line
            nl=MAlloc(sizeof(CLine));
            ed->line_count++;
            nl->prev=ln;
            if(ln) nl->next=ln->next;
            if(nl->prev) nl->prev->next=nl;
            if(nl->next) nl->next->prev=nl;
            nl->text=right;
            ln=nl;
            Free(text2);
            goto loop;
        }

    if(!StrLen(text2)) ln->text=RopeAppendText(ln->text, "\n");
    else if(text2[StrLen(text2)-1]!='\n') ln->text=RopeAppendText(ln->text, "\n");
    DirtyLinesBelow(ed, GetLinum(ed,start_ln));
    Free(text2);
}
U0 RenumberLines(CEditor *ed,CLine *l=NULL) {
    I64 num=0;
    if(!l) l=ed->lines;
    else num=l->line;
    while(l) {
        l->line=num++;
        l=l->next;
    }
}
U0 EditorInsText(CEditor *ed,U8 *text,I64 at,Bool undo=FALSE) {
    EdDelSel(ed); //Clear selection if needed
    __EditorInsText(ed, text, at);
    if(undo) {
        CUndoInfo *prev;
        if(!(prev=GetUndoInfo(ed))) {
            ins1:
            CUndoInfo info;
            info.type=EDIT_INS_STR;
            info.inserted_str=StrNew(text);
            info.at=at;
            InsUndoInfo(&info,ed);
            goto ret;
        } else if(prev->type==EDIT_INS_STR) {
            if(prev->at+StrLen(prev->inserted_str)==at) {
                U8 *new=MStrPrint("%s%s",prev->inserted_str,text);
                Free(prev->inserted_str);
                prev->inserted_str=new;
            }
        }
        goto ins1;
    }
    ret:
    RenumberLines(ed);
}
U0 SDL_InitPair(I64 idx,U_RGBA8888 fg,U_RGBA8888 bg) {
    SDL_ColorPairs[idx].fg=fg,SDL_ColorPairs[idx].bg=bg;
}
CEditor *EditorNew(Bool gen_rt_tags=FALSE) {
    CEditor *r=MAlloc(sizeof(CEditor));
    SDL_InitPair(CP_KW,SOL_MAGENTA,SOL_WHITE);
    SDL_InitPair(CP_WORD,SOL_BLUE,SOL_WHITE);
    SDL_InitPair(CP_TOK,SOL_GREEN,SOL_WHITE);
    SDL_InitPair(CP_NUM,SOL_YELLOW,SOL_WHITE);
    SDL_InitPair(CP_STR,SOL_RED,SOL_WHITE);
    SDL_InitPair(CP_WHITE,SOL_BLACK,SOL_WHITE);
    SDL_InitPair(CP_LINUM,SOL_WHITE,SOL_BLUE);
    SDL_InitPair(CP_COMMENT,SOL_CYAN,SOL_WHITE);
    SDL_InitPair(CP_AC_SEL,SOL_CYAN,SOL_RED);
    SDL_InitPair(CP_AC_UNSEL,SOL_YELLOW,SOL_BLUE);
    SDL_InitPair(CP_MENU_SEL,SOL_YELLOW,SOL_RED);
    SDL_InitPair(CP_MENU_UNSEL,SOL_WHITE,SOL_BLUE);
    SDL_InitPair(CP_FUZZY_SEL_A,SOL_BLACK,SOL_YELLOW);
    SDL_InitPair(CP_FUZZY_UNSEL_A,SOL_BLACK,SOL_WHITE);
    SDL_InitPair(CP_FUZZY_SEL_B,SOL_RED,SOL_YELLOW);
    SDL_InitPair(CP_FUZZY_UNSEL_B,SOL_RED,SOL_WHITE);
    SDL_InitPair(CP_LINUM_ERR,SOL_WHITE,SOL_RED);
    SDL_InitPair(CP_LINUM_WARN,SOL_WHITE,SOL_YELLOW);
    I32 x,y;
    SDL_GetWindowSize(global_window,&x,&y);
    r->window=SDL_NewTextWin(x/FONT_X2,y/FONT_Y2);
    r->vp_start_line=0;
    r->line_cache=MAlloc(sizeof(CLine*));
    r->line_cache_size=0;
    EditorInsText(r,"",0);
    if(gen_rt_tags) {
        CreateTagsAndErrorsFiles("TAGS","ERRS",r->fn);
        r->tags=ReadTags("TAGS");
        r->diags=ParseDiags("ERRS");
        MarkLineColors(r);
    }
    return r;
}
U0 FreeLine(CLine *ln) {
    //Update multiline comments
    if(ln->flags&ED_LNF_COMMENTED_START) {
        CLine *s=SearchForMLCommmentStart(ln->prev);
        CLine* e=SearchForMLCommmentEnd(ln);
adjust:
        if(s) {
            if(e) {
                e->commentStart=s;
            } else {
                do {
                    s->flags|=ED_LNF_COMMENTED_OUT;
                    s=s->next;
                } while(s);
            }
        } else if(e) {
            e->commentStart=NULL;
            do {
                e->flags&=~ED_LNF_COMMENTED_OUT;
                e=e->prev;
            } while(e);
        }
    } else if(ln->flags&ED_LNF_COMMENTED_END) {
        s=SearchForMLCommmentStart(ln);
        e=SearchForMLCommmentEnd(ln->next);
        goto adjust;
    }
    if(ln->prev) ln->prev->next=ln->next;
    if(ln->next) ln->next->prev=ln->prev;
    if(ln->text) RopeFree(ln->text);
    Free(ln);
}
U0 __EditorDelText(CEditor *ed,I64 soff,I64 eoff) {
    I64 ln_off=0;
    CLine *ln=ed->lines,*prev=NULL;
    Bool merge=TRUE;
    CLine *end_ln;
    while(ln) {
        if(ln_off<=soff<ln_off+RopeLength(ln->text)) {
            CLine *start_ln=ln;
            DirtyLinesBelow(ed,GetLinum(ed, start_ln));
            I64 en=MinI64(RopeLength(ln->text),eoff-ln_off);
            ln->text=RopeDelText(ln->text,soff-ln_off,en);
            eoff-=en-(soff-ln_off);
            ln->text=RopeCondense(ln->text);
            Highlight(ed, ln, TRUE);
            ln_off+=RopeLength(ln->text);
            if(ln_off+RopeLength(ln->text)>=eoff) {
                DirtyLinesBelow(ed,GetLinum(ed, ln));
                //End of deletion is on same line
                //Merge with next line if doesnt contain an endline
                U8 *txt=Rope2Str(ln->text);
                if(!StrFirstOcc(txt,"\n")) {
                    Free(txt);
                    end_ln=ln->next;
                    goto merge;
                }
                Free(txt);
                return;
            }
            break;
        }
        ln_off+=RopeLength(ln->text);
        prev=ln;
        ln=ln->next;
    }
    if(!ln) return;
    while(ln) {
        if(ln_off<=eoff<ln_off+RopeLength(ln->text)) break;
        ln_off+=RopeLength(ln->text);
        prev=ln;
        ln=ln->next;
    }
    end_ln=ln;
merge:
    //start_ln!=end_ln;
    if(start_ln!=end_ln)
        ln=start_ln->next;
    DirtyLinesBelow(ed,GetLinum(ed, start_ln));
    while(ln!=end_ln) {
        CLine *nxt=ln->next;
        if(ln==ed->lines) ed->lines=nxt;
        ed->line_count--;
        eoff-=RopeLength(ln->text);
        FreeLine(ln);
        ln=nxt;
    }
    if(merge&&end_ln) {
        U8 *t=Rope2Str(end_ln->text);
        start_ln->text=RopeAppendText(start_ln->text,t+eoff-GetLineOffset(ed,GetLinum(ed,end_ln)));
        start_ln->text=RopeCondense(start_ln->text);
        Highlight(ed, start_ln, TRUE);
        Free(t);
        FreeLine(end_ln);
        ed->line_count--;
    }
    //All lines must end in newline.
    if(RopeLength(start_ln->text)==0) {
        addnl:
        start_ln->text=RopeAppendText(start_ln->text,"\n");
    } else if(RopeChar(start_ln->text,RopeLength(start_ln->text)-1)!='\n'){
        goto addnl;
    }
}
U8 *EdTextSlice(CEditor *ed,I64 soff,I64 eoff) {
    U8 *ret=MAlloc(eoff-soff+1);
    U8 *retptr=ret;
    I64 off=0;
    CLine *ln=ed->lines;
    sloop:
    if(off<=soff<off+RopeLength(ln->text)) {
        U8 *text=Rope2Str(ln->text);
        if(off<=eoff<off+RopeLength(ln->text)) {
            StrNCpy(ret,text+soff-off,eoff-soff);
            Free(text);
            return ret;
        } else {
            StrCpy(ret,text+soff-off);
            retptr+=StrLen(ret);
            off+=RopeLength(ln->text);
            ln=ln->next;
        }
    } else {
        off+=RopeLength(ln->text);
        ln=ln->next;
        goto sloop;
    }
    eloop:
    if(!ln) goto en;
    text=Rope2Str(ln->text);
    if(off<=eoff<off+RopeLength(ln->text)) {
        StrNCpy(retptr,text,eoff-off);
        Free(text);
        return ret;
    } else {
        off+=RopeLength(ln->text);
        StrCpy(retptr,text);
        retptr+=StrLen(retptr);
        ln=ln->next;
        Free(text);
        goto eloop;
    }
    en:
    return ret;
}
U0 EditorDelText(CEditor *ed,I64 soff,I64 eoff,Bool undo=FALSE) {
    if(undo)
        U8 *slice=EdTextSlice(ed,soff,eoff);
     __EditorDelText(ed, soff, eoff);
    if(undo) {
        CUndoInfo *prev=GetUndoInfo(ed);
        if(!prev) {
            ins1:
            CUndoInfo del;
            del.type=EDIT_DEL_STR;
            del.removed_str=StrNew(slice);
            del.at=soff;
            InsUndoInfo(&del,ed);
            goto ret;
        } else if(prev->type==EDIT_DEL_STR&&prev->at==soff) {
            U8 *new=MStrPrint("%s%s",slice,prev->removed_str);
            Free(prev->removed_str);
            prev->removed_str=new;
            goto ret;
        }
        goto ins1;
    }
    ret:
    if(undo) Free(slice);
    RenumberLines(ed);
}
U0 EdJumpToChar(CEditor *ed,I64 chr) {
  CLine *ln=ed->lines;
  I64 off=0;
  loop:
  if(!ln) {
    ed->curx=ed->cury=0;
    return;
  }
  I64 len;
  if(off<=chr<off+(len=RopeLength(ln->text))) {
    ed->cury=GetLinum(ed, ln);
    ed->curx=ed->_curx=chr-off;
    return;
  }
  off+=len;
  ln=ln->next;
  goto loop;
}
U0 Undo(CEditor *ed) {
  if(ed->undo_info_cur-1<0) return;
  ed->undo_info_cur--;
  CUndoInfo *info=GetUndoInfo(ed);
  switch(info->type) {
    case EDIT_INS_STR:
    EditorDelText(ed,info->at,info->at+StrLen(info->inserted_str));
    EdJumpToChar(ed,info->at);
    break;
    case EDIT_DEL_STR:
    EditorInsText(ed,info->removed_str,info->at);
    EdJumpToChar(ed,info->at+StrLen(info->removed_str));
    break;
  }
}
U0 Redo(CEditor *ed) {
  if(ed->undo_info_cur>=UndoBufSize(ed)) return;
  CUndoInfo *info=GetUndoInfo(ed);
  switch(info->type) {
    case EDIT_DEL_STR:
    EditorDelText(ed,info->at,info->at+StrLen(info->removed_str));
    EdJumpToChar(ed,info->at);
    break;
    case EDIT_INS_STR:
    EditorInsText(ed,info->inserted_str,info->at);
    EdJumpToChar(ed,info->at+StrLen(info->inserted_str));
    break;
  }
  ed->undo_info_cur++;
}
U0 DrawLinumWithPad(CEditor *ed,I64 num,I64 barwidth) {
    U8 *lnumtxt=MStrPrint("%d: ",num);
    I64 lnumlen=StrLen(lnumtxt);
    SDL_Print(ed->window,lnumtxt);
    Free(lnumtxt);
    while(barwidth>lnumlen++) SDL_AddCh(ed->window,' ');
}
#define ED_DRAW_LINUMS 1
#define ED_DRAW_SEARCH 2
#define ED_DRAW_DIALOG 4
#define ED_DRAW_MENUBAR 8
#define ED_DRAW_DFT (ED_DRAW_LINUMS|ED_DRAW_MENUBAR)
#define ED_DRAW_NO_RENDER 16
U0 AddMenuBar(CEditor *ed,I64 active=-1);
I64 DrawEditor(CEditor *ed,I64 flags=0,U8 *dialogtxt=NULL,I64 mb_active=-1) {
    curs_set(0);
    CLine *diagln=NULL;
    //If we are at a line with a diagnostic,be sure to make a bottom margin
    if(flags&(ED_DRAW_DIALOG|ED_DRAW_SEARCH)) {
    } else {
        diagln=GetLine(ed,ed->cury);
        if(diagln) {
           if(!(diagln->flags&(ED_LNF_ERR|ED_LNF_WARN))) diagln=NULL;
        }
    }
    if(flags&ED_DRAW_MENUBAR) {
        if(AddMenuBar(ed,mb_active)==ED_RET_QUIT) return ED_RET_QUIT;
        ed->margin_top=1;
    }
    if(flags&ED_DRAW_LINUMS) {
        U8 *lnumtxt=MStrPrint("%d: ",ed->line_count);
        I64 lnumlen=StrLen(lnumtxt);
        Free(lnumtxt);
    } else if(flags&ED_DRAW_SEARCH) {
        ed->window->cur_cp=CP_LINUM;
        lnumtxt=MStrPrint("Search: ");
        lnumlen=StrLen(lnumtxt);
        ed->window->y=ed->window->x=0;
        SDL_Print(ed->window,lnumtxt);
        Free(lnumtxt);
    } else if(flags&ED_DRAW_DIALOG) {
        ed->window->cur_cp=CP_LINUM;
        lnumtxt=MStrPrint("%s: ",dialogtxt);
        lnumlen=StrLen(lnumtxt);
        ed->window->y=ed->window->x=0;
        SDL_Print(ed->window,lnumtxt);
        Free(lnumtxt);
    } else
        lnumlen=0;
    FocusCursor(ed,lnumlen);
    //if(ed->x_scroll) Debugger;
    ed->w-=lnumlen;
    CLine *ln=ed->lines;
    while(ln) {
        if(ln->line==ed->vp_start_line) break;
        ln=ln->next;
    }
    I64 cnt=SDL_WinY(ed->window),line=ed->vp_start_line+1,screen_line=ed->margin_top;
    cnt-=(ed->margin_top+ed->margin_bottom);
    while(--cnt>=0&&ln) {
        if(flags&ED_DRAW_LINUMS) {
            ed->window->y=screen_line,ed->window->x=0;
            I64 cp=CP_LINUM;
            if(ln->flags&ED_LNF_ERR) {
                cp=CP_LINUM_ERR;
            } else if(ln->flags&ED_LNF_WARN) {
                cp=CP_LINUM_WARN;
            }
            ed->window->cur_cp=cp;
            DrawLinumWithPad(ed,line++,lnumlen);
        }
        if(1) {
            ed->window->y=screen_line,ed->window->x=lnumlen;
            Highlight(ed,ln,,TRUE);
        }
        ++screen_line;
        ln=ln->next;
    }
    U8 *pad=MStrPrint("%*c",lnumlen,' ');
    while(cnt>=0) {
        ed->window->cur_cp=CP_LINUM;
        ed->window->y=screen_line,ed->window->x=0;
        SDL_Print(ed->window,pad);
        ed->window->cur_cp=CP_WHITE;
        ed->window->y=screen_line++,ed->window->x=lnumlen;
        SDL_ClrToEOL(ed->window);
        cnt--;
    }
    if(diagln) {
        ed->window->y=SDL_WinY(ed->window)-1,ed->window->x=0;
        if(diagln->flags&ED_LNF_ERR) ed->window->cur_cp=CP_LINUM_ERR;
        else if(diagln->flags&ED_LNF_WARN) ed->window->cur_cp=CP_LINUM_WARN;
        else throw('InvDiag');
        SDL_Print(ed->window,diagln->diag->msg);
        SDL_ClrToEOL(ed->window);
    }
    ed->window->cur_enabled=TRUE;
    ed->window->curx=lnumlen+ed->curx-ed->x_scroll;
    ed->window->cury=ed->cury-ed->vp_start_line+ed->margin_top;
    if(flags&ED_DRAW_NO_RENDER);
    else SDL_DrawWin(ed->window);
    return ED_RET_OK;
}
U0 EdUp(CEditor *ed) {
    if(ed->cury>0) {
        --ed->cury;
        if(ed->cury<ed->vp_start_line)
            ed->vp_start_line=ed->cury;
        if(ed->_curx>RopeLength(GetLine(ed, ed->cury)->text))
            ed->curx=RopeLength(GetLine(ed, ed->cury)->text);
        else
            ed->curx=ed->_curx;
    } else {
        ed->_curx=ed->curx=0;
    }
}
U0 EdDown(CEditor *ed) {
    CLine *dnext=GetLine(ed,ed->cury);
    if(dnext->next) {
        ++ed->cury;
        if(ed->_curx>RopeLength(GetLine(ed, ed->cury)->text))
            ed->curx=RopeLength(GetLine(ed, ed->cury)->text);
        else
            ed->curx=ed->_curx;
    } else {
        ed->_curx=ed->curx=RopeLength(GetLine(ed, ed->cury)->text);
    }
}
#define ED_MOVE_DONT_SKIP_TAB 1
U0 EdRight(CEditor *ed,I64 flags=0) {
    CLine *rline=GetLine(ed,ed->cury);
    U8 *text=Rope2Str(rline->text);
    I64 x=ed->curx,cnt=0;
    I64 ox=x;
    x++;
    if(!flags&ED_MOVE_DONT_SKIP_TAB) {
        while((--x>=0)&&text[x]==' ') cnt++;
        if(x==-1&&cnt) {
          cnt++;
          cnt/=4;
          I64 cap=4*(cnt+1);
          while(text[ed->curx]==' ') {
            if(ed->curx>=cap) break;
            ed->curx++;
          }
          if(ox==ed->curx) goto next;
          ed->_curx=ed->curx;
          Free(text);
          return;
        }
    }
    next:
    Free(text);
    if(ed->curx+1>=RopeLength(rline->text)) {
        rline=rline->next;
        if(rline) {
            ed->_curx=ed->curx=0;
            ed->cury++;
        } else {
            ed->_curx=ed->curx=RopeLength(GetLine(ed,ed->cury)->text);
        }
    } else ed->_curx=++ed->curx;
}
U0 EdLeft(CEditor *ed,I64 flags=0) {
    CLine *lline=GetLine(ed,ed->cury);
    U8 *text=Rope2Str(lline->text);
    I64 x=ed->curx,cnt=1;
    if(!flags&ED_MOVE_DONT_SKIP_TAB) {
        if(x) {
          while(--x&&text[x]==' ') cnt++;
          if(!x&&cnt) {
            cnt/=4;
            if(cnt)
              ed->_curx=ed->curx=4*(cnt-1);
            else
              ed->_curx=ed->curx=0;
            Free(text);
            return;
          }
        }
    }
    Free(text);
    if(ed->curx==0) {
        lline=lline->prev;
        if(lline) {
            ed->_curx=ed->curx=RopeLength(lline->text);
            ed->cury--;
        } else {
            ed->_curx=ed->curx=0;
        }
    } else ed->_curx=--ed->curx;
}
U0 EdWordLeft(CEditor *ed) {
    CLine *lline=GetLine(ed,ed->cury);
    U8 *text=Rope2Str(lline->text);
    I64 idx=ed->curx;
    Bool hit_word=FALSE;
    if(idx!=0) idx--;
    while(idx>0) {
        if(Bt(char_bmp_alpha_numeric,text[idx])) {
            hit_word=TRUE;
        } else if(hit_word)
            break;
        idx--;
    }
    if(idx==0) {
        lline=lline->prev;
        if(!lline)
            ed->_curx=ed->curx=0;
        else
            ed->_curx=ed->curx=RopeLength(lline->text)-1,ed->cury--; //-1 ignores newline
    } else
        ed->_curx=ed->curx=idx;
    Free(text);
}
U0 EdWordRight(CEditor *ed) {
    CLine *lline=GetLine(ed,ed->cury);
    U8 *text=Rope2Str(lline->text);
    I64 idx=ed->curx;
    Bool hit_word=FALSE;
    idx++;
    I64 len=RopeLength(lline->text);
    while(idx<len) {
        if(!text[idx]) break;
        if(Bt(char_bmp_alpha_numeric,text[idx])) {
            hit_word=TRUE;
        } else if(hit_word)
            break;
        idx++;
    }
    if(idx==len) {
        lline=lline->next;
        if(lline) {
            ed->cury++;
            ed->_curx=ed->curx=0;
        }
    } else
        ed->_curx=ed->curx=idx;
    Free(text);
}
U0 EdBackspace(CEditor *ed) {
    CLine *bline=GetLine(ed,ed->cury);
    I64 eoff=GetLineOffset(ed,ed->cury);
    I64 ox=ed->curx,oy=ed->cury;
    EdLeft(ed);
    if(ox==0) {
        //Kill Newline on previous line
        if(bline->prev) {
            I64 no=RopeLength(bline->prev->text)-1; //ignore newline
            EditorDelText(ed, eoff-1, eoff,TRUE);
            ed->_curx=ed->curx=no;
            return;
        } else {
            //Do nothing
        }
    } else {
        U8 *text=Rope2Str(bline->text);
        I64 cnt=0,orig=ox;
        while(--ox&&text[ox]==' ') cnt++;
        if(!ox&&cnt) {
            cnt/=4;
            EditorDelText(ed,eoff,eoff+orig,TRUE);
            Free(text);
            ed->curx=ed->_curx=0;
            //text=MStrPrint("%*c",cnt,'    ');
            //EditorInsText(ed,text,eoff);
            //ed->curx=ed->_curx=StrLen(text);
        } else {
            EditorDelText(ed, eoff+orig-1, eoff+orig,TRUE);
        }
        Free(text);
    }
}
U0 EdPageDown(CEditor *ed) {
    //Check if there is enough room for a page down
    CLine *lline=GetLine(ed,ed->cury);
    I64 cnt=SDL_WinY(ed->window),cnt2=0;
    while(lline&&--cnt>=0) {
        lline=lline->next;
        cnt2++;
    }
    if(!lline) cnt2--;
    ed->vp_start_line+=cnt2;
    ed->cury+=cnt2;
    lline=GetLine(ed,ed->cury);
    if(RopeLength(lline->text)>=ed->_curx) ed->curx=RopeLength(lline->text);
}
U0 EdPageUp(CEditor *ed) {
    ed->vp_start_line-=SDL_WinY(ed->window);
    ed->vp_start_line=MaxI64(ed->vp_start_line,0);
    CLine *lline=GetLine(ed,ed->vp_start_line);
    ed->cury=ed->vp_start_line;
    if(RopeLength(lline->text)>=ed->_curx) ed->curx=RopeLength(lline->text);
}
U0 FreeEditor(CEditor *ed);
I64 EdSearch(CEditor *ed) {
    static U8 prevs[1024];
    CEditor *sed=EditorNew();
    goto resize;
loop:
    I64 key=GetKey;
    switch(key) {
    case ERR:
        goto loop;
    case ED_KEY_RESIZE:
resize:
        SDL_DrawWin(ed->window);
        sed->window->begy=SDL_WinY(ed->window)-1;
        goto mvsrncur;
    case ED_KEY_CTRL_UP:
    case ED_KEY_UP: //TODO
        goto mvsrncur;
    case ED_KEY_CTRL_DOWN:
    case ED_KEY_DOWN: //TODO
        goto mvsrncur;
    case ED_KEY_RIGHT:
        EdRight(sed);
        goto mvsrncur;
    case ED_KEY_LEFT:
        EdLeft(sed);
        goto mvsrncur;
    case ED_KEY_REDRAW: goto mvsrncur;
    case ED_KEY_CTRL_LEFT:
left:
        EdWordLeft(sed);
        goto mvsrncur;
    case ED_KEY_CTRL_RIGHT:
        EdWordRight(sed);
        goto mvsrncur;
    case ED_KEY_BACKSPACE:
        //Check for a tab
        EdBackspace(sed);
        goto mvsrncur;
    case ED_KEY_INSERT: //TODO
        goto mvsrncur;
    case ED_KEY_DELETE: //TODO
        goto mvsrncur;
    default:
        if(key=='\n') {
            U8 *needle=Rope2Str(sed->lines->text); //Only one line
            if(StrFirstOcc(needle,"\n")) *StrFirstOcc(needle,"\n")=0;
            if(StrLen(needle))
                StrCpy(prevs,needle);
            else {
                Free(needle);
                needle=StrNew(prevs);
            }
            I64 origln=ed->cury;
            I64 curln=origln;
            CLine *ln=GetLine(ed, ed->cury);
            I64 lnoff=ed->curx;
            Bool wrapped_around=FALSE;
loop2:
            while(ln) {
                U8 *txt=Rope2Str(ln->text);
                U8 *find=StrIMatch(txt+lnoff,needle);
                Free(txt);
                if(find) {
                    ed->cury=curln;
                    ed->_curx=ed->curx=find-txt+StrLen(needle);
                    Free(needle);
                    goto en;
                }
                if(wrapped_around&&curln==origln) goto fail;
                lnoff=0;
                ln=ln->next;
                curln++;
            }
            ln=ed->lines;
            curln=0;
            wrapped_around=TRUE;
            goto loop2;
fail:
            Free(needle);
            goto en;
        }
        I64 soff=GetLineOffset(sed, sed->cury)+sed->curx++;
        U8 buffer[2]= {key,0};
        EditorInsText(sed,buffer,soff,TRUE);
    }
mvsrncur:
    DrawEditor(ed,ED_DRAW_DFT|ED_DRAW_NO_RENDER);
    sed->window->y=SDL_WinY(ed->window)-1;
    DrawEditor(sed,ED_DRAW_SEARCH|ED_DRAW_NO_RENDER);
    SDL_DrawWin(ed.window,sed.window);
    goto loop;
en:
    DrawEditor(ed,ED_DRAW_DFT);
    FreeEditor(sed);
}
I64 Edit(CEditor *ed);
//interactive tells us to enter the autcomplete
Bool DrawAutocomplete(CEditor *parent,Bool interactive=FALSE) {
    Bool ret=FALSE;
    I64 wx1=0;
    I64 wy1=0;
    I64 sw=SDL_WinX(parent->window);
    I64 sh=SDL_WinY(parent->window);
    CLine *ln=GetLine(parent, parent->cury);
    U8 *text=Rope2Str(ln->text);
    I64 idx=parent->curx;
    if(idx==0) goto fin;
    --idx;
loop0:
    if(idx==0) goto begin;
    switch(text[idx]) {
    case 'a'...'z':
    case 'A'...'Z':
    case '0'...'9':
    case '_':
        idx--;
        goto loop0;
    default:
        idx++;
    }
begin:
    if(idx==parent->curx) goto fin;
    I64 screenx=wx1+parent->curx-parent->x_scroll;
    I64 screeny=wy1+parent->cury-parent->vp_start_line+parent->margin_top;
    I64 len=parent->curx-idx;
    U8 *sub=StrNCpy(MAlloc(len+1),text+idx,len);
    if(StrLen(sub)) ret=TRUE;
    I64 cnt;
    I64 active=-1;
loop:
    CTrie **ents=FuzzyMatch(parent->tags, sub, &cnt);
    if(!cnt) goto free;
    I64 maxlen=StrLen(ents[cnt-1]->fullname); //Ents are sorted by length
    if(screenx+1<sw&&screeny+1<sh) {
        CSDL_TextWin *ac=SDL_NewTextWin(MinI64(maxlen,sw-screenx-wx1),MinI64(cnt,sh-screeny-wy1));
        ac->begx=screenx+1,ac->begy=screeny+1;
        I64 iter=0;
        for(; iter<cnt; iter++) {
            len=StrLen(ents[iter]->fullname);
            I64 chr=0;
            if(iter==active)
                ac->cur_cp=CP_AC_SEL;
            else
                ac->cur_cp=CP_AC_UNSEL;
            ac->y=iter,ac->x=0;
            I64 cap=SDL_WinX(ac);
            WriteWithinWin(ac,ents[iter]->fullname);
            while(len++<cap) {
              SDL_AddCh(ac,' ');
            }
        }
        DrawEditor(parent, ED_DRAW_DFT|ED_DRAW_NO_RENDER);
        SDL_DrawWin(parent->window, ac);
        SDL_DestroyTextWin(ac);
    }
    if(interactive) {
winput:
        I64 key=GetKey();
        switch(key) {
        case ERR:
            goto winput;
        case ED_KEY_UP:
            active=MaxI64(0,--active);
            goto loop;
        case ED_KEY_DOWN:
            active=MinI64(cnt-1,++active);
            goto loop;
        case '\n':
            if(active==-1) active=0;
            I64 off=GetLineOffset(parent,parent->cury);
            EditorDelText(parent, off+idx,off+parent->curx,TRUE);
            EditorInsText(parent, ents[active]->name, off+idx,TRUE);
            parent->_curx=parent->curx=(idx+StrLen(ents[active]->name));
            goto free;
        default:
            goto free;
        }
    }
free:
    Free(ents),Free(sub);
fin:
    Free(text);
    SDL_DrawWin(parent->window);
    return ret;
}
class CMenuEnt {
    U8 *name;
    U8 key;
    I64(*callback)(CEditor *ed);
};
U8 *Editor2Str(CEditor *ed,I64 *cnt) {
    CLine *lns=ed->lines;
    I64 len=0;
    while(lns) {
        len+=RopeLength(lns->text);
        lns=lns->next;
    }
    U8 *ret=MAlloc(len+1);
    lns=ed->lines,len=0;
    while(lns) {
        U8 *tmp=Rope2Str(lns->text);
        StrCpy(ret+len,tmp);
        Free(tmp);
        len+=RopeLength(lns->text);
        lns=lns->next;
    }
    if(cnt) *cnt=len;
    return ret;
}
U8 *FileDialog(CEditor *parent);
I64 GotoSymbol(CEditor *ed);
I64 SaveDocAs(CEditor *ed);
/**
 * This will color the lines based on diagnostitc
 */
U0 MarkLineColors(CEditor *ed) {
    if(!ed->fn) return;
    CLine *line=ed->lines;
    while(line) {
        line->flags&=~(ED_LNF_WARN|ED_LNF_ERR);
        line=line->next;
    }
    CDiag *diag=ed->diags;
    while(diag) {
        if(0!=StrCmp(diag->fn,ed->fn)) goto next;
        line=GetLine(ed,diag->ln-1); //Diag lines are 1 indexes
        if(line) {
            //Errors>warning
            if(line->flags&ED_LNF_ERR) goto next;
            if(diag->type==DIAG_ERR) {
                line->flags&=~ED_LNF_WARN;
                line->flags|=ED_LNF_ERR;
                line->diag=diag;
            } else {
                line->flags|=ED_LNF_WARN;
                line->diag=diag;
            }
        } else ;//???
        next:
        diag=diag->next;
    }
}
I64 SaveDoc(CEditor *ed) {
    if(!ed->fn) {
        SaveDocAs(ed);
    } else {
        CSDL_TextWin *msgs=SDL_NewTextWin(SDL_WinX(ed->window),SDL_WinY(ed->window));
        SDL_Print(msgs,"SAVING FILE\n");
        SDL_DrawWin(msgs);
        U8 *etxt=Editor2Str(ed,&cnt);
        FileWrite(ed->fn,etxt,cnt);
        Free(etxt);
        FreeTrie(ed->tags);
        FreeDiags(ed->diags);
        SDL_Print(msgs,"Generating tags and diagnostics.\n");
        SDL_DrawWin(msgs);
        CreateTagsAndErrorsFiles("TAGS","ERRS",ed->fn);
        SDL_Print(msgs,"Parsing Tags.\n");
        SDL_DrawWin(msgs);
        ed->tags=ReadTags("TAGS");
        ed->diags=ParseDiags("ERRS");
        SDL_Print(msgs,"Garbage collecting.\n");
        SDL_DrawWin(msgs);
        GC_Collect();
        MarkLineColors(ed);
        SDL_DrawWin(ed->window);
    }
    return ED_RET_OK;
}

I64 SaveDocAs(CEditor *ed) {
    U8 *new=FileDialog(ed);
    if(new) {
      Free(ed->fn);
      ed->fn=new;
      SaveDoc(ed);
    }
    return ED_RET_OK;
}
I64 OpenDoc(CEditor *ed) {
    U8 *new=FileDialog(ed);
    if(new) {
      I64 ret=OpenFile(new);
      return ret;
    }
    return ED_RET_OK;
}
U0 DrawMenuDropdown(CEditor *ed,CMenuEnt *ent,I64 off,I64 active=-1) {
    I64 x=0;
    I64 he=SDL_WinY(ed->window);
    I64 w=SDL_WinX(ed->window);
    I64 mwidth=0;
    I64 h=0;
    while(ent[h].name) {
        mwidth=MaxI64(mwidth,StrLen(ent[h].name));
        h++;
    }
    if(!h) return;
    CSDL_TextWin *win=SDL_NewTextWin(MinI64(mwidth,w-off),MinI64(h, he-1));
    win->begy=1;
    win->begx=off;
    I64 i;
    for(i=0; i!=h; i++) {
        if(active==i) win->cur_cp=CP_AC_SEL;
        else win->cur_cp=CP_AC_UNSEL;
        win->y=i,win->x=0;
        SDL_Print(win,ent[i].name);
        I64 len=StrLen(ent[i].name);
        while(len++<mwidth) SDL_AddCh(win,' ');
    }
    DrawEditor(ed,ED_DRAW_DFT|ED_DRAW_NO_RENDER);
    SDL_DrawWin(ed->window,win);
    SDL_DestroyTextWin(win);
}
I64 BufferSelect(CEditor *ed);
U0 EdCopy(CEditor *ed) {
    U8 *clip=EdGetSelText(ed);
    SDL_SetClipboardText(clip);
    Free(clip);
    ed->sel_start=ed->sel_end=-1;
}
U0 EdPaste(CEditor *ed) {
    U8 *clip=SDL_GetClipboardText;
    EditorInsText(ed,clip,GetLineOffset(ed,ed->cury)+ed->curx,TRUE);
    Free(clip);
}
U0 EdCut(CEditor *ed) {
    U8 *clip=EdGetSelText(ed);
    SDL_SetClipboardText(clip);
    EdDelSel(ed);
    Free(clip);
}
//Draws at first line
U0 AddMenuBar(CEditor *ed,I64 active=-1) {
    I64 ret=ED_RET_OK;
    ed->margin_top=1;
    CMenuEnt File[]= {
        {"[S]ave",'s',&SaveDoc},
        {"Save [A]s",'a',&SaveDocAs},
        {"[O]pen File",'o',&OpenDoc},
        {"[B]uffer Select",'b',&BufferSelect},
        {NULL,0,NULL},
    };
    CMenuEnt Edit[]={
        {"[C]opy",'c',&EdCopy},
        {"Cu[t]",'t',&EdCut},
        {"[P]aste",'p',&EdPaste},
        {NULL,0,NULL},
    };
    CMenuEnt Code[]= {
        {"[G]oto Symbol",'g',&GotoSymbol},
        {"[C]ompile Check",'c',&ShowDiagsWindow},
        {NULL,0,NULL},
    };
    I64 x=getbegx(stdscr);
    I64 y=getbegy(stdscr);
    I64 w=getmaxx(stdscr);
    class {
        CMenuEnt *spec;
        U8 *name;
        U8 key;
    } cats[]= {
        {&File,"[F]ile",'f'},
        {&Edit,"[E]dit",'e'},
        {&Code,"[C]ode",'c'},
    };
    I64 cnt=sizeof(cats)/sizeof(*cats),cur,active_sub=0;
    goto draw;
loop:
    I64 key=GetKey();
    switch(key) {
      case ERR: goto loop;
    case ED_KEY_ESCAPE: goto en;
    case ED_KEY_UP:
        active_sub=MaxI64(0,--active_sub);
        break;
    case ED_KEY_DOWN:
        if(cats[active].spec[++active_sub].name==NULL) --active_sub;
        break;
    case ED_KEY_LEFT:
        active=MaxI64(0,--active),active_sub=0;
        break;
    case ED_KEY_RIGHT:
        active=MinI64(cnt-1,++active),active_sub=0;
        break;
    case ED_KEY_REDRAW: goto draw;
    case '\n':
        if(cats[active].spec[active_sub].callback)
            ret=(cats[active].spec[active_sub].callback[0])(ed);
        goto en;
    default:
        I64 s=0;
        if(active!=-1)
        for(; cats[active].spec[s].name; s++) {
            if(cats[active].spec[s].key==key) {
                ret=(cats[active].spec[s].callback[0])(ed);
                goto en;
            }
        }
    }
draw:
    ed->window->x=ed->window->y=0;
    I64 off=0,sel_off=-1;
    for(cur=0; cur!=cnt; cur++) {
        if(cur==active) {
            ed->window->cur_cp=CP_MENU_SEL;
            sel_off=cur;
        } else
            ed->window->cur_cp=CP_MENU_UNSEL;
        SDL_Print(ed->window,cats[cur].name);
        if(cur>=active)
          ; //Do Nothing
        else
          off+=StrLen(cats[cur].name);
    }
    SDL_ClrToEOL(ed->window);
    if(sel_off!=-1) {
      DrawEditor(ed, ED_DRAW_DFT&(~ED_DRAW_MENUBAR));
      if(DrawMenuDropdown(ed,cats[active].spec, off,active_sub)==ED_RET_QUIT) return ED_RET_QUIT;
    }
    if(active!=-1)
    goto loop;
en:
  return ret;
}
U0 FreeEditor(CEditor *ed) {
    CLine *ln=ed->lines;
    while(ln) {
        CLine *n=ln->next;
        FreeLine(ln);
        ln=n;
    }
    SDL_DestroyTextWin(ed->window);
    Free(ed->dirty_screen_lines);
    Free(ed->line_cache);
    if(ed->tags) FreeTrie(ed->tags);
    if(ed->diags) FreeDiags(ed->diags);
    Free(ed);
}
U8 *RemoveNL(U8 *t) {
  U8 *f;
  if(f=StrFirstOcc(t,"\n")) *f=0;
  return t;
}
U8 **FuzzySymbolSelect(CEditor *ed,U8 *str) {
  I64 cnt;
  CTrie **t=FuzzyMatch(ed->tags, str,&cnt);
  U8 **ret=MAlloc((cnt+1)*sizeof(U8*));
  while(--cnt>=0) {
    ret[cnt]=StrNew(t[cnt]->fullname);
  }
  Free(t);
  return ret;
}
U8 *FuzzySelectWindow(CEditor *parent,U8** (*match_cb)(CEditor *ed,U8 *str),U8 *init=NULL) {
  CEditor *ed=EditorNew();
  if(init) {
    EditorInsText(ed,init,0);
    ed->curx+=StrLen(init);
  }
  I64 active=-1,idx;
  if(!init)
    U8 **matches=(*match_cb)(parent,"");
  else
    matches=(*match_cb)(parent,init);
  goto draw;
  loop:
  I64 key=GetKey();
  switch(key) {
    case ERR: goto loop;
    case ED_KEY_ESCAPE:
    for(idx=0;matches[idx];idx++)
      Free(matches[idx]);
    Free(matches);
    return NULL;
    case ED_KEY_UP:
    active=MaxI64(0,--active);
    goto draw;
    case ED_KEY_DOWN:
    if(active==-1) {active=0; goto draw;}
    if(active>=SDL_WinY(ed->window)-1) goto loop;
    if(!matches[active]) goto loop;
    if(!matches[active+1]) goto loop;
    active++;
    goto draw;
    case ED_KEY_CTRL_LEFT:
    EdWordLeft(ed);
    goto draw;
    case ED_KEY_CTRL_RIGHT:
    EdWordRight(ed);
    goto draw;
    case ED_KEY_LEFT:
    left:
    EdLeft(ed);
    goto draw;
    case ED_KEY_BACKSPACE:
    EdBackspace(ed);
    goto update;
    case ED_KEY_REDRAW: goto draw;
    case ED_KEY_RIGHT:
    right:
    EdRight(ed);
    goto draw;
    case '\n':
    U8 *ret=NULL;
   for(idx=0;matches[idx];idx++)
      if(active!=idx) Free(matches[idx]);
    if(active==-1) {
      ret=RemoveNL(Rope2Str(ed->lines->text));
    } else {
      ret=matches[active];
    }
    Free(matches);
    parent->margin_bottom=0;
    SDL_DrawWin(parent->window);
    return ret;
    default:
    U8 buffer[]={key,0};
    EditorInsText(ed, buffer,ed->curx,TRUE);
    update:
    active=-1;
    for(idx=0;matches[idx];idx++)
      Free(matches[idx]);
    Free(matches);
    U8 *t=RemoveNL(Rope2Str(ed->lines->text));
    matches=(*match_cb)(parent,t);
    Free(t);
    if(key!=ED_KEY_BACKSPACE)
        goto right;
  }
  draw:
  FocusCursor(ed);
  if(active==-1)
    ed->window->cur_cp=CP_FUZZY_SEL_A;
  else
    ed->window->cur_cp=CP_FUZZY_UNSEL_A;
  t=RemoveNL(Rope2Str(ed->lines->text));
  ed->window->y=ed->window->x=0;
  SDL_ClrToEOL(ed->window);
  ed->window->y=ed->window->x=0;
  WriteWithinView(ed,t, 0);
  idx=1;
  for(;idx<SDL_WinY(ed->window);idx++) {
    if(!matches[idx-1]) break;
    U8 *mat=matches[idx-1];
    U8 *tc=t;
    I64 offset=0;
    ed->window->y=idx,ed->window->x=0;
    hl:
    if(active==idx-1)
      ed->window->cur_cp=CP_FUZZY_SEL_A;
    else
      ed->window->cur_cp=CP_FUZZY_UNSEL_A;
    U8 buffer2[]={*(tc++),0};
    U8 *chr=StrFirstOcc(mat,buffer2);
    if(!chr) {
      WriteWithinView(ed,mat, offset);
      mat+=StrLen(mat);
    } else {
      U8 *slice=StrNCpy(MAlloc(chr-mat+1),mat,chr-mat);
      WriteWithinView(ed,slice, offset);
      mat+=StrLen(slice);
      offset+=StrLen(slice);
      Free(slice);
      if(active==idx-1)
        ed->window->cur_cp=CP_FUZZY_SEL_B;
      else
        ed->window->cur_cp=CP_FUZZY_UNSEL_B;
      WriteWithinView(ed,buffer2, offset++);
      mat++;
    }
    if(*mat) goto hl;
    SDL_ClrToEOL(ed->window);
  }
  for(;idx<SDL_WinY(ed->window);idx++) {
    ed->window->y=idx,ed->window.x=0;
    SDL_ClrToEOL(ed->window);
  }
  ed->window->cury=0,ed->window->curx=ed->curx-ed->x_scroll;
  ed->window->cur_enabled=TRUE;
  SDL_DrawWin(ed->window);
  ed->window->cur_enabled=FALSE;
  Free(t);
  goto loop;
}
I64 OpenFile(U8 *name,I64 ln=0)  {
  loop:
  CDirEntry *find=FilesFind(name,FUF_SINGLE);
  if(find) {
    DirEntryFree(find);
    name=FileNameAbs(name);
  } else {
    name=StrNew(name);
    FileWrite(name,"",0);
    goto loop;
  }
  CBuffer *buf;
  if(buf=HashFind(name,buffers)) {
    Free(name);
    buf->ed->cury=MinI64(ln,GetLineCount(buf->ed));
    return Edit(buf->ed);
  }
  I64 cnt;
  U8 *text=FileRead(name,&cnt);
  if(!text) return ED_RET_OK;
  CEditor *new=EditorNew();
  EditorInsText(new,text,0);
  new->fn=StrNew(name);
  new->cury=MinI64(ln,GetLineCount(new));
  Free(text);
  CreateTagsAndErrorsFiles("TAGS","ERRS",new->fn);
  new->tags=ReadTags("TAGS");
  new->diags=ParseDiags("ERRS");
  MarkLineColors(new);
  buf=MAlloc(sizeof(*buf));
  buf->str=StrNew(name);
  buf->ed=new;
  HashAdd(buf,buffers);
  Free(name);
  return Edit(new);
}
U8 **FuzzyFile(CEditor *ed,U8 *pat) {
  U8 *pat2=MStrPrint("%s*",pat);
  CDirEntry *ents=FilesFind(pat2);
  Free(pat2);
  CDirEntry *orig=ents;
  I64 cnt=0;
  while(ents) {
    ents=ents->next,cnt++;
  }
  U8 **ret=MAlloc(sizeof(U8*)*(++cnt));
  ents=orig,cnt=0;
  while(ents) {
    ret[cnt++]=StrNew(ents->name);
    ents=ents->next;
  }
  if(orig)
    DirEntryFree(orig);
  return ret;
}
I64 GotoSymbol(CEditor *ed) {
  U8 *str=FuzzySelectWindow(ed,&FuzzySymbolSelect);
  if(!str) return ED_RET_OK;
  U8 *orig=str;
  CTrie *tags=ed->tags;
  while(*str&&tags) {
    try {
      tags=tags->ents[TrieChrIdx(*str++)];
    } catch {
      Fs->catch_except=1;
      return ED_RET_OK;
    }
  }
  Free(orig);
  if(tags)
    return OpenFile(tags->fn, tags->ln);
  return ED_RET_OK;
}
U8 *FileDialog(CEditor *parent) {
  U8 * fn=NULL;
  loop:
  fn=FuzzySelectWindow(parent, &FuzzyFile,fn);
  if(!fn) return fn;
  if(IsDir(fn)) {
    U8 *dir;
    #if IsWindows
    dir=MStrPrint("%s\\",fn);
    #else
    dir=MStrPrint("%s/",fn);
    #endif
    Free(fn);
    fn=dir;
    goto loop;
  }
  return fn;
}
U8 **FuzzyBufferSel(CEditor *ed,U8 *pat) {
  U8 **ret=NULL,*orig=pat;
  loop:
  I64 cnt=buffers->mask+1,cnt2=0;
  while(--cnt>=0) {
    CBuffer *bucket=buffers->body[cnt];
    for(;bucket;bucket=bucket->next) {
      U8 *str=bucket->str;
      pat=orig;
      next:
      if(!*pat) {
        if(ret)
          ret[cnt2]=StrNew(bucket->str);
        cnt2++;
        goto cont;
      }
      U8 buffer[]={*pat++,0};
      str=StrFirstOcc(str,buffer);
      if(str) {
        str++;
        goto next;
      }
      cont:
    }
  }
  if(!ret) {
    ret=MAlloc(sizeof(U8*)*(1+cnt2));
    goto loop;
  }
  return ret;
}
I64 BufferSelect(CEditor *ed) {
  U8 *sel=FuzzySelectWindow(ed,&FuzzyBufferSel);
  if(sel) {
    CBuffer *buf=HashFind(sel,buffers);
    I64 ret=Edit(buf->ed);
    Free(sel);
    return ret;
  }
  return ED_RET_OK;
}
I64 EdOffset(CEditor *ed) {
    return GetLineOffset(ed,ed->cury)+ed->curx;
}
I64 EdClearSelect(CEditor *ed) {
    ed->sel_start=ed->sel_end;
}
I64 EdSelLeft(CEditor *ed) {
    Bool in_select=ed->sel_start!=ed->sel_end;
    if(!in_select) ed->sel_start=EdOffset(ed);
    EdLeft(ed);
    ed->sel_end=EdOffset(ed);
}
I64 EdSelRight(CEditor *ed) {
    Bool in_select=ed->sel_start!=ed->sel_end;
    if(!in_select) ed->sel_start=EdOffset(ed);
    EdRight(ed);
    ed->sel_end=EdOffset(ed);
}
I64 EdSelUp(CEditor *ed) {
    Bool in_select=ed->sel_start!=ed->sel_end;
    if(!in_select) ed->sel_start=EdOffset(ed);
    EdUp(ed);
    ed->sel_end=EdOffset(ed);
}
I64 EdSelDown(CEditor *ed) {
    Bool in_select=ed->sel_start!=ed->sel_end;
    if(!in_select) ed->sel_start=EdOffset(ed);
    EdDown(ed);
    ed->sel_end=EdOffset(ed);
}
U0 EdDelSel(CEditor *ed) {
    Bool in_select=ed->sel_start!=ed->sel_end;
    if(!in_select) return;
    if(ed->sel_start>ed->sel_end) SwapI64(&ed->sel_end,&ed->sel_start);
    EditorDelText(ed,ed->sel_start,ed->sel_end,TRUE);
    EdJumpToChar(ed,ed->sel_start);
    ed->sel_start=ed->sel_end=-1;
}
U8 *EdGetSelText(CEditor *ed) {
    Bool in_select=ed->sel_start!=ed->sel_end;
    if(!in_select) return NULL;
    if(ed->sel_start>ed->sel_end) {
        SwapI64(&ed->sel_end,&ed->sel_start);
    }
    U8 *ret=MAlloc(ed->sel_end-ed->sel_start+1);
    I64 cnt=GetLineCount(ed),idx=0;
    I64 offset=0,reti=0;
    for(;idx!=cnt;idx++) {
        offset=GetLineOffset(ed,idx);
        CRope *text;
        I64 en=RopeLength(text=GetLine(ed,idx)->text);
        {
            I64 st=MaxI64(ed->sel_start-offset,0);
            en=MinI64(en,ed->sel_end-offset);
            if(st>en) goto next;
            U8 *lntxt=Rope2Str(text);
            StrNCpy(ret+reti,lntxt+st,en-st);
            Free(lntxt);
            reti+=en-st;
            if(reti>=ed->sel_end-ed->sel_start) {
                goto ret;
            }
            next:
        }
    }
    ret:
    return ret;
}
I64 Edit(CEditor *ed) {
  goto mvsrncur;
loop:
    I64 key=GetKey,cnt,cnt2;
    if(key==ERR) goto loop;
    I64 eoff,soff;
    switch(key) {
    case 'e'&0x1f:
    if(ed->diags)
      if(ED_RET_QUIT==ShowDiagsWindow(ed)) return ED_RET_QUIT;
    break;
      /**
       * [F]ile
       * [C]ode
       */
    case 'x'&0x1f:
    U8 *clip=EdGetSelText(ed);
    EdDelSel(ed);
    SDL_SetClipboardText(clip);
    Free(clip);
    goto mvsrncur;
    case 'c'&0x1f:
    clip=EdGetSelText(ed);
    SDL_SetClipboardText(clip);
    EdClearSelect(ed);
    goto mvsrncur;
    case 'v'&0x1f:
    EdDelSel(ed);
    clip=SDL_GetClipboardText();
    EditorInsText(ed,clip,GetLineOffset(ed,ed->cury)+ed->curx,TRUE);
    I64 righti=StrLen(clip);
    Free(clip);
    while(--righti>=0) EdRight(ed,ED_MOVE_DONT_SKIP_TAB);
    goto mvsrncur;
    case ED_KEY_SHIFT_UP:
    EdSelUp(ed);
    goto mvsrncur;
    case ED_KEY_SHIFT_DOWN:
    EdSelDown(ed);
    goto mvsrncur;
    case ED_KEY_SHIFT_LEFT:
    EdSelLeft(ed);
    goto mvsrncur;
    case ED_KEY_SHIFT_RIGHT:
    EdSelRight(ed);
    goto mvsrncur;
    case ED_KEY_ESCAPE:
    case ALT_KEY('f'):
    if(DrawEditor(ed,ED_DRAW_DFT,,0)==ED_RET_QUIT) return ED_RET_QUIT; //First menu item
    goto mvsrncur;
    case ALT_KEY('e'):
    if(DrawEditor(ed,ED_DRAW_DFT,,1)==ED_RET_QUIT) return ED_RET_QUIT;
    goto mvsrncur;
    case ALT_KEY('c'):
    if(DrawEditor(ed,ED_DRAW_DFT,,2)==ED_RET_QUIT) return ED_RET_QUIT; //First menu item
    goto mvsrncur;
    case ED_KEY_RESIZE:
        goto mvsrncur;
    case ED_KEY_UP:
        EdClearSelect(ed);
        EdUp(ed);
        goto mvsrncur;
    case ED_KEY_DOWN:
        EdClearSelect(ed);
        EdDown(ed);
        goto mvsrncur;
    case ED_KEY_REDRAW: goto mvsrncur;
    case ED_KEY_LEFT:
left:
        EdClearSelect(ed);
        EdLeft(ed);
        goto mvsrncur;
    case ED_KEY_RIGHT:
right:
        EdClearSelect(ed);
        EdRight(ed);
        goto mvsrncur;
    case ED_KEY_END:
        EdClearSelect(ed);
        ed->_curx=ed->curx=0;
        goto mvsrncur;
    case ED_KEY_HOME:
        EdClearSelect(ed);
        CLine *heline=GetLine(ed,ed->cury);
        ed->_curx=ed->curx=RopeLength(heline->text);
        goto mvsrncur;
    case ED_KEY_CTRL_UP:
        EdClearSelect(ed);
        goto pageup;
    case ED_KEY_CTRL_DOWN:
        EdClearSelect(ed);
        goto pagedown;
    case ED_KEY_CTRL_LEFT:
        EdClearSelect(ed);
        EdWordLeft(ed);
        goto mvsrncur;
    case ED_KEY_CTRL_RIGHT:
        EdClearSelect(ed);
        EdWordRight(ed);
        goto mvsrncur;
    case ED_KEY_BACKSPACE:
        if(ed->sel_start!=ed->sel_end)
            EdDelSel(ed);
        else
            EdBackspace(ed);
        goto mvsrncur;
    case ED_KEY_PAGEUP:
pageup:
        EdPageUp(ed);
        goto mvsrncur;
    case ED_KEY_PAGEDOWN:
pagedown:
        EdPageDown(ed);
        goto mvsrncur;
    case '\t':
        if(!DrawAutocomplete(ed,TRUE)) {
          EditorInsText(ed,"    ",GetLineOffset(ed,ed->cury)+ed->curx,TRUE);
          ed->curx+=4;
        }
        goto mvsrncur;
    case 'y'&0x1f:
    Redo(ed);
    goto mvsrncur;
    case 'z'&0x1f:
    Undo(ed);
    goto mvsrncur;
    case 'b'&0x1f:
    BufferSelect(ed);
    SDL_DrawWin(ed->window);
    goto mvsrncur;
    case 't'&0x1f:
    GotoSymbol(ed);
    SDL_DrawWin(ed->window);
    goto mvsrncur;
    case 'o'&0x1f:
    OpenDoc(ed);
    SDL_DrawWin(ed->window);
    goto mvsrncur;
    case 's'&0x1f:
    SaveDoc(ed);
    SDL_DrawWin(ed->window);
    goto mvsrncur;
    default:
        if(key==('q'&0x1f)) {return ED_RET_QUIT;}
        if(key==('f'&0x1f)) {
            EdSearch(ed);
            goto mvsrncur;
        }
        soff=GetLineOffset(ed, ed->cury)+ed->curx++;
        U8 buffer[2]= {key,0};
        EditorInsText(ed,buffer,soff,TRUE);
        if(key=='\n') {
            CLine *prev=GetLine(ed,ed->cury);
            EdRight(ed);
            U8 *margin=MStrPrint("%*c",IndentLevel(prev),'    ');
            EditorInsText(ed,margin,soff+1,TRUE);
            ed->_curx=ed->curx=4*IndentLevel(prev);
        }
        goto mvsrncur;
    }
mvsrncur:
    //Move cursor left if at newline
    CLine *lline=GetLine(ed,ed->cury);
    if(ed->curx+1>RopeLength(lline->text)) ed->curx=RopeLength(lline->text)-1;
    DrawEditor(ed,ED_DRAW_DFT);
    goto loop;
exit:
  return ED_RET_OK;
}
/*
EditorInsText(ed,"if(Potato)\nx{3.14 'abc\\'def\\\\'}\n",0);
EditorInsText(ed,"Tom\nato\n",4);
DrawEditor(ed);
FreeEditor(ed);
CEditor *ed=EditorNew();
EditorInsText(ed,"012\n345\n678\n9ab",0);
EditorDelText(ed,1,2);
EditorDelText(ed,1,15-1);
DrawEditor(ed);
*/
SDL_InitScr;
CEditor *ed=EditorNew(TRUE);
Edit(ed);
U0 FreeBuffer(U0 *buf) {
    FreeEditor(buf(CBuffer*)->ed);
}
U0 SDL_ExitScr() {
    SDL_DestroyWindow(global_window);
    SDL_FreeSurface(global_font);
    global_window=NULL,global_font=NULL;
    SDL_StopTextInput;
    HashTableDel(buffers,(&FreeBuffer)(U0*));
}
SDL_ExitScr;




