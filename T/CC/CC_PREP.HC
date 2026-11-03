/* M13 preprocessor-lite: runs on the user source text BEFORE the CC
 * prelude is prepended (the prelude is prep-free by construction).
 *
 *   #include "path" / #include <path>   plain names resolve against
 *          T:/CC/INC/ (the adapted-musl header home); anything else
 *          that looks like a path (leads with '/' or has a drive like
 *          'T:') is opened as given. Nesting is capped (CC_INC_MAX).
 *   #define NAME [body]   OBJECT-LIKE only. The body is stored verbatim
 *          and re-expanded when NAME is used (bounded rescans break
 *          self-referential bodies). Function-like #define F(a) is NOT
 *          implemented: F expands as a bare name and the leftover call
 *          text fails to parse loudly, by design.
 *   #undef / #ifdef / #ifndef / #else / #endif / #if 0 / #if 1 / #pragma
 *   Unknown directives are errors, not silently eaten.
 *
 * Skipped regions pass NO text through but keep one newline per
 * dropped line, so parse-side source shape stays sane. Expansion works
 * on a hand-rolled scanner — never the CCLex state — because it must
 * treat string/char literals as opaque (a macro name inside a literal
 * is NOT substituted) and pass comments through as raw bytes for the
 * real lexer. Deliberately line-shaped: a '#' at the start of a line
 * INSIDE a multi-line block comment would be read as a directive
 * (accepted edge: our headers and tests keep conditionals and block
 * comments disjoint, like most real code). */

class CPrep
{
  U8	*buf;		//output text: growable MAlloced, closed with NUL
  I64	pos;
  I64	cap;
  /* Conditional frames: bit0 = this branch emits; bit1 = the #if/#else
   * chain has already taken a branch; bit2 = the enclosing region
   * emitted when the directive ran (so an inner #else cannot resurrect
   * an outer-skipped region). */
  I64	frm[16];
  I64	sp;
  I64	depth;		//#include recursion counter
};

/* Same-unit forward declarations use `extern` (see the CC.HH note:
 * plain prototypes mislink mutual recursion in this HolyC build).
 * CCPrepText <-> CCPrepDirective recurse via #include. */
extern I64 CCPrepText(CCCtrl *cc,CPrep *pp,U8 *in);
extern U0 CCPrepScan(CCCtrl *cc,CPrep *pp,U8 *p,I64 n,I64 xdepth);
extern U8 *CCSrc(U8 *fn);	//defined in CC.HC (file read)

U0 CCPrepErr(CCCtrl *cc,U8 *msg)
{
  "CC ERROR: prep: %s\n",msg;
  cc->errs++;
}

I64 CCPrepAccept(CPrep *pp)
{
  if (!pp->sp)
    return TRUE;
  return pp->frm[pp->sp-1]&1;
}

U0 CCPrepPut(CPrep *pp,U8 *s,I64 n)
{
  U8 *nb;
  I64 nc;
  if (!n)
    return;
  if ((pp->pos+n+1)>(pp->cap))
  {
    nc=pp->cap*2;
    while ((pp->pos+n+1)>nc)
      nc*=2;
    nb=MAlloc(nc);
    MemCpy(nb,pp->buf,pp->pos);
    //Old buffer intentionally leaked: poopalloc pages are shared with
    //the rest of the compiler (see the CCSrc comment in CC.HC).
    pp->buf=nb;
    pp->cap=nc;
  }
  MemCpy(pp->buf+pp->pos,s,n);
  pp->pos+=n;
}

I64 CCPrepMacro(CCCtrl *cc,U8 *nm)
{
  I64 i;
  for (i=0;i!=cc->mn;i++)
    if (!StrCmp(cc->mname[i],nm))
      return i;
  return -1;
}

/* Emit one region of ordinary text (no leading '#'): identifiers that
 * name macros expand to their bodies (rescanned, depth-bounded);
 * literals, comments and all other bytes copy verbatim. */
U0 CCPrepScan(CCCtrl *cc,CPrep *pp,U8 *p,I64 n,I64 xdepth)
{
  U8 tok[CC_TOK_MAX];
  I64 i,q,mi,closer;
  while (n>0)
  {
    if ((*p=='_')||(CIsAlpha(*p)))
    {
      i=0;
      while ((n>0)&&((*p=='_')||((CIsAlpha(*p))||(CIsDigit(*p)))))
      {
        if (i<(CC_TOK_MAX-1))
          tok[i++]=*p;
        p++;
        n--;
      }
      tok[i]=0;
      mi=CCPrepMacro(cc,tok);
      if ((mi>=0)&&(xdepth<4))
        CCPrepScan(cc,pp,cc->mbody[mi],StrLen(cc->mbody[mi]),xdepth+1);
      else
        CCPrepPut(pp,tok,i);
    }
    else if ((*p=='"')||(*p==39))	//39 is ' : literals are opaque
    {				//to expansion (see header note)
      closer=*p;
      CCPrepPut(pp,p,1);
      p++;
      n--;
      while (n>0)
      {
        if (*p=='\\')
        {
          if (n>1)
          {
            CCPrepPut(pp,p,2);
            p+=2;
            n-=2;
          }
          else
            break;	//dangling backslash at line end
        }
        else
        {
          CCPrepPut(pp,p,1);
          q=*p;
          p++;
          n--;
          //break (NOT n=0): n must keep tracking the rest of the
          //region — the bytes after a closing quote are ordinary code
          //and n=0 here silently ate them (every `char *s="x";` lost
          //its semicolon).
          if (q==closer)
            break;
        }
      }
    }
    else if ((*p=='/')&&(p[1]=='*'))
    {
      //Block comments copy raw: the real lexer re-tokenizes them.
      CCPrepPut(pp,p,2);
      p+=2;
      n-=2;
      while (n>1)
      {
        if ((*p=='*')&&(p[1]=='/'))
        {
          CCPrepPut(pp,p,2);
          p+=2;
          n-=2;
          break;
        }
        CCPrepPut(pp,p,1);
        p++;
        n--;
      }
      while (n>0)
      {
        CCPrepPut(pp,p,1);
        p++;
        n--;
      }
    }
    else if ((*p=='/')&&(p[1]=='/'))
    {
      while ((n>0)&&(*p!='\n'))
      {
        CCPrepPut(pp,p,1);
        p++;
        n--;
      }
    }
    else
    {
      CCPrepPut(pp,p,1);
      p++;
      n--;
    }
  }
}

/* Record one object-like macro: NAME [body-to-EOL]. Redefinition
 * silently replaces (C would warn; we keep it simple). */
I64 CCPrepDefine(CCCtrl *cc,U8 *q,U8 *e)
{
  U8 nm[CC_TOK_MAX];
  I64 i=0,b;
  while ((q<e)&&((*q==' ')||(q=='\t')))		//skip the gap after 'define'
    q++;
  while ((q<e)&&((*q=='_')||((CIsAlpha(*q))||(CIsDigit(*q)))))
  {
    if (i<(CC_TOK_MAX-1))
      nm[i++]=*q;
    q++;
  }
  nm[i]=0;
  if (!i)
  {
    CCPrepErr(cc,"#define needs a name");
    return FALSE;
  }
  while ((q<e)&&((*q==' ')||(q=='\t')))
    q++;
  b=CCPrepMacro(cc,nm);
  if (b<0)
  {
    if (cc->mn>=CC_MAX_MACROS)
    {
      CCPrepErr(cc,"too many #define macros");
      return FALSE;
    }
    b=cc->mn;
    cc->mn=b+1;
    StrCpy(cc->mname[b],nm);
    cc->mbody[b][0]=0;
  }
  i=0;
  while ((q<e)&&(i<(CC_MACRO_BODY-1)))
  {
    cc->mbody[b][i++]=*q;
    q++;
  }
  cc->mbody[b][i]=0;
  return TRUE;
}

/* Read an identifier out of a directive tail. REPORT=0 stays silent
 * (a skipped region tolerates junk it never acts on). */
I64 CCPrepName(CCCtrl *cc,U8 **pq,U8 *e,U8 *nm,I64 report)
{
  U8 *q=*pq;
  I64 i=0;
  while ((q<e)&&((*q==' ')||(q=='\t')))
    q++;
  if ((q>=e)||(!(CIsAlpha(*q))))
  {
    if (report)
      CCPrepErr(cc,"directive expects an identifier");
    return FALSE;
  }
  while ((q<e)&&((*q=='_')||((CIsAlpha(*q))||(CIsDigit(*q)))))
  {
    if (i<(CC_TOK_MAX-1))
      nm[i++]=*q;
    q++;
  }
  nm[i]=0;
  *pq=q;
  return TRUE;
}

/* Push one conditional frame: PARENT accepts, COND true. */
U0 CCPrepCond(CPrep *pp,I64 parent,I64 cond)
{
  I64 acc;
  acc=parent&&cond;
  pp->frm[pp->sp]=acc|(acc<<1)|(parent<<2);
  pp->sp++;
}

I64 CCPrepDirective(CCCtrl *cc,CPrep *pp,U8 *q,U8 *e)
{
  U8 kw[CC_TOK_MAX],nm[CC_TOK_MAX],path[CC_TOK_MAX],full[CC_TOK_MAX+16];
  U8 *inc,*start;
  I64 accept,par,f,mi,cond,i,close,qw;

  accept=CCPrepAccept(pp);
  q++;				//'#'
  while ((q<e)&&((*q==' ')||(q=='\t')))
    q++;
  i=0;
  while ((q<e)&&(CIsAlpha(*q)))
  {
    if (i<(CC_TOK_MAX-1))
      kw[i++]=*q;
    q++;
  }
  kw[i]=0;
  if (!StrCmp(kw,"define"))
  {
    if (accept)
      return CCPrepDefine(cc,q,e);
    return TRUE;
  }
  if (!StrCmp(kw,"undef"))
  {
    if (!accept)
      return TRUE;
    if (!CCPrepName(cc,&q,e,nm,TRUE))
      return FALSE;
    mi=CCPrepMacro(cc,nm);
    if (mi>=0)
    {
      cc->mn--;
      while (mi!=cc->mn)
      {
        StrCpy(cc->mname[mi],cc->mname[mi+1]);
        StrCpy(cc->mbody[mi],cc->mbody[mi+1]);
        mi++;
      }
    }
    return TRUE;
  }
  if (!StrCmp(kw,"include"))
  {
    if (!accept)
      return TRUE;
    while ((q<e)&&((*q==' ')||(q=='\t')))
      q++;
    close=0;
    if (q<e)
    {
      if (*q=='<')
        close='>';
      else if (*q=='"')
        close='"';
    }
    if (!close)
    {
      CCPrepErr(cc,"#include wants \"name\" or <name>");
      return FALSE;
    }
    q++;
    start=q;
    while ((q<e)&&(*q!=close))
      q++;
    if (q>=e)
    {
      CCPrepErr(cc,"unterminated #include name");
      return FALSE;
    }
    i=0;
    while ((start<q)&&(i<(CC_TOK_MAX-16)))
      path[i++]=*start++;
    path[i]=0;
    //Plain names come from the header home; explicit paths pass
    //through untouched (drive-qualified or leading '/').
    if ((path[0]=='/')||(path[1]==':'))
      ;
    else
    {
      StrCpy(full,"T:/CC/INC/");
      i=StrLen(full);
      StrCpy(full+i,path);
      StrCpy(path,full);
    }
    if (pp->depth>=CC_INC_MAX)
    {
      CCPrepErr(cc,"#include nesting too deep");
      return FALSE;
    }
    inc=CCSrc(path);
    if (!inc)
    {
      "CC ERROR: prep: cannot read %s\n",path;
      cc->errs++;
      return FALSE;
    }
    pp->depth++;
    f=CCPrepText(cc,pp,inc);
    pp->depth--;
    return f;
  }
  if (!StrCmp(kw,"ifdef"))
  {
    par=accept;
    cond=FALSE;
    if (CCPrepName(cc,&q,e,nm,par))
      cond=CCPrepMacro(cc,nm)>=0;
    CCPrepCond(pp,par,cond);
    return TRUE;
  }
  if (!StrCmp(kw,"ifndef"))
  {
    par=accept;
    cond=TRUE;
    if (CCPrepName(cc,&q,e,nm,par))
      cond=CCPrepMacro(cc,nm)<0;
    CCPrepCond(pp,par,cond);
    return TRUE;
  }
  if (!StrCmp(kw,"if"))
  {
    par=accept;
    cond=FALSE;
    qw=0;
    while ((q<e)&&((*q==' ')||(q=='\t')))
      q++;
    if ((q<e)&&(*q=='0'))
    {
      qw=TRUE;
      cond=FALSE;
    }
    else if ((q<e)&&(*q=='1'))
    {
      qw=TRUE;
      cond=TRUE;
    }
    if (!qw)
    {
      if (par)
      {
        CCPrepErr(cc,"#if supports only literal 0 or 1");
        return FALSE;
      }
      cond=FALSE;
    }
    CCPrepCond(pp,par,cond);
    return TRUE;
  }
  if (!StrCmp(kw,"else"))
  {
    if (!pp->sp)
    {
      CCPrepErr(cc,"#else without #if/#ifdef/#ifndef");
      return FALSE;
    }
    f=pp->frm[pp->sp-1];
    cond=(f&4)&&!(f&2);
    pp->frm[pp->sp-1]=cond|(f&2)|(cond<<1)|(f&4);
    return TRUE;
  }
  if (!StrCmp(kw,"endif"))
  {
    if (!pp->sp)
    {
      CCPrepErr(cc,"#endif without #if");
      return FALSE;
    }
    pp->sp--;
    return TRUE;
  }
  if (!StrCmp(kw,"pragma"))
    return TRUE;
  if (accept)
  {
    "CC ERROR: prep: unknown directive '#%s'\n",kw;
    cc->errs++;
    return FALSE;
  }
  return TRUE;
}

I64 CCPrepText(CCCtrl *cc,CPrep *pp,U8 *in)
{
  U8 *q,*e;
  I64 accept;
  while (*in)
  {
    q=in;
    while ((*q==' ')||(q=='\t'))
      q++;
    e=q;
    while ((*e)&&(*e!='\n'))
      e++;
    accept=CCPrepAccept(pp);
    if (*q=='#')
    {
      if (!CCPrepDirective(cc,pp,q,e))
        return FALSE;
    }
    else if (accept)
      //Scan from `in`, not `q`: q exists only to find the '#' —
      //scanning from it would silently strip each line's indentation.
      CCPrepScan(cc,pp,in,e-in,0);
    //Preserve the line, even for dropped text: the parser sees the
    //same line structure the source had.
    CCPrepPut(pp,"\n",1);
    if (*e)
      in=e+1;
    else
      in=e;
  }
  return TRUE;
}

U8 *CCPrep(CCCtrl *cc,U8 *text)
{
  CPrep pp;
  I64 cap;
  cap=StrLen(text)*4+0x4000;
  pp.buf=MAlloc(cap);
  pp.cap=cap;
  pp.pos=0;
  pp.sp=0;
  pp.depth=0;
  cc->mn=0;
  if (!CCPrepText(cc,&pp,text))
    return NULL;
  pp.buf[pp.pos]=0;
  return pp.buf;
}
