/* M13 preprocessor-lite: runs on the user source text BEFORE the CC
 * prelude is prepended (the prelude is prep-free by construction).
 *
 *   #include "path" / #include <path>   plain names resolve against
 *          T:/CC/INC/ (the adapted-musl header home); anything else
 *          that looks like a path (leads with '/' or has a drive like
 *          'T:') is opened as given. Nesting is capped (CC_INC_MAX).
 *   #define NAME [body]        object-like: the body is stored verbatim and
 *          re-expanded when NAME is used.
 *   #define NAME(a,b) [body]   M31: function-like (the '(' must follow the
 *          name with no space, as C requires; `#define F (x)` is an
 *          object-like macro whose body is "(x)"). Arguments are collected
 *          from the LINE (this prep is line-shaped: a list that runs off the
 *          end of its line is an error), expanded once in the CALLER's
 *          context, then substituted; `#param` stringizes and `param ## x`
 *          pastes, and both operands are substituted UNEXPANDED. Arguments
 *          are NOT re-expanded after substitution, and a macro already
 *          disabled on the current expansion path stays a name (blue paint),
 *          which is what makes a self-referential body finite.
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

/* M31: the state of ONE function-like invocation. Both argument forms live as
 * offsets into the prep scratch: rawa/rawn is the argument text AS WRITTEN (a
 * copy, so it stays put) and is used when a parameter is an operand of `#` or
 * `##`, where C substitutes the argument unexpanded; expa/expn is the same
 * argument AFTER expansion. One address space, because the region a body is
 * scanned in is NOT the region the invocation was found in — reading raw text
 * against the body's buffer is what made `GC(1,2,3)` emit scratch garbage
 * where C puts `2,3`.
 * pnext/pend say a `##` has just been seen: the left operand was cut out of
 * the destination and is held here, so an empty right operand deletes it (that
 * is how `, ## __VA_ARGS__` drops the comma) and the whitespace `##` eats is
 * gone with the cut. floor is where this body began writing, so `##` can never
 * reach back into the text that surrounded the invocation. */
class CMacExp
{
  I64	mi;			//macro being expanded
  I64	rawa[CC_MAX_ARGS];
  I64	rawn[CC_MAX_ARGS];
  I64	expa[CC_MAX_ARGS];
  I64	expn[CC_MAX_ARGS];
  I64	narg;			//arguments collected
  U8	*vraw;			//M31: the variadic tail AS WRITTEN (from the
  I64	vrawn;			//first extra to the end of the last), because
				//`#__VA_ARGS__` stringizes the whole sequence
				//including the spacing its commas were written
				//with — probed: SV(1, 2) becomes "1, 2", which
				//per-argument joining cannot produce.
  I64	pnext;			//the next operand is a ## right side
  U8	pend[CC_TOK_MAX];	//its left side, already cut from the output
  I64	pendlen;
  I64	floor;			//destination position at the start of the body
};

/* M31: one macro expansion in progress. mask disables every macro whose
 * expansion is still open on this path (blue paint: the name stays put), so an
 * argument is expanded with the mask of its INVOCATION and a body is scanned
 * with that mask plus the bit of the macro being expanded — the pairing that
 * makes F1(F1(5)) give ((5)) while `#define SELF SELF+1` expands exactly once.
 * ex is the invocation whose body this region is (NULL for ordinary text and
 * for an object-like body, which has no parameters to substitute), and dst
 * selects the destination: 0 = the output text, 1 = the argument scratch (a
 * macro invoked while an argument is being expanded writes its body into the
 * scratch, because that body IS part of the argument text). */
class CMacScan
{
  U8	*p;		//region being scanned
  I64	n;		//its length
  I64	i;		//read position, carried here so the helpers advance it
  I64	mask;		//disabled macro indices (CC_MAX_MACROS fits in an I64)
  I64	dst;
  CMacExp *ex;
  I64	isarg;			//1 = this region is an ARGUMENT text, which is
				//the difference C cares about for `__VA_ARGS__`:
				//one written in an argument list is a token, one
				//in prose is not — and prose is exactly what the
				//continuation lines of a multi-line block comment
				//are to this scanner (M13's line-shaped model)
  I64	incond;		//1 = this region is the controlling expression
				//of an #if/#elif, where `defined` is an
				//operator rather than a name (it travels
				//into nested scans, because a macro body
				//may hold `defined(x)`)
  I64	subonly;		//1 = substitute parameters but expand NOTHING
				//else: names, `#` and `##` stay as written. M31
				//needs it because C decides an inner invocation's
				//argument list from the SUBSTITUTED stream, not
				//from the body text: `#define VP(...) Q(__VA_ARGS__)`
				//with `#define Q(a,b)` is a two-argument call, and
				//`#define PS(a,b) S(P2(a,b))` stringizes `P2(1,2)`
				//— parameters replaced, macros untouched.
};

class CPrep
{
  U8	*buf;		//output text: growable MAlloced, closed with NUL
  I64	pos;
  I64	cap;
  /* M31: argument scratch. Each invocation pushes the expanded text of its
   * arguments and, when the destination is the scratch itself, compacts them
   * away afterwards by sliding its body down over them — the scratch is only
   * ever read through recorded (offset,length) pairs, so it must never be left
   * with holes inside a live argument text. */
  U8	*sc;
  I64	scpos;
  I64	sccap;
  /* Conditional frames: bit0 = this branch emits; bit1 = the #if/#else
   * chain has already taken a branch; bit2 = the enclosing region
   * emitted when the directive ran (so an inner #else cannot resurrect
   * an outer-skipped region); bit3 = an #else has already run at this
   * level, which is what makes a later #elif a structural error (M32)
   * rather than just another branch nobody reaches. */
  I64	frm[16];
  I64	sp;
  I64	depth;		//#include recursion counter
  /* M33: the directory of the file whose text is being scanned at each
   * #include depth, always with a trailing '/', or "" when the name says
   * nothing about a directory (a bare name, or a caller that named no file at
   * all).  A quoted `#include "x.h"` looks here first; `<x.h>` never does,
   * which is what gcc does with angle brackets too. */
  U8	dir[CC_INC_MAX+2][CC_TOK_MAX];
};

/* Same-unit forward declarations use `extern` (see the CC.HH note:
 * plain prototypes mislink mutual recursion in this HolyC build).
 * CCPrepText <-> CCPrepDirective recurse via #include. */
extern I64 CCPrepText(CCCtrl *cc,CPrep *pp,U8 *in);
extern U0 CCPrepScan(CCCtrl *cc,CPrep *pp,CMacScan *ms);
extern U0 CCPrepIdent(CCCtrl *cc,CPrep *pp,CMacScan *ms,U8 *w,I64 wl);
extern I64 CCPrepVaEmit(CCCtrl *cc,CPrep *pp,CMacScan *ms,I64 raw);
extern I64 CCPrepWord(U8 *p,I64 n,I64 i,U8 *w);
extern I64 CCPrepLitLen(U8 *p,I64 n,I64 i);
extern I64 CCPrepComLen(U8 *p,I64 n,I64 i);
extern I64 CCPrepArgs(CCCtrl *cc,U8 *p,I64 n,I64 *pa,I64 *pn,I64 *pc);
extern U0 CCPrepDefined(CCCtrl *cc,CPrep *pp,CMacScan *ms,I64 wl);
extern U0 CCPrepTrim(U8 *p,I64 *pa,I64 *pn);
extern U8 *CCSrc(U8 *fn);	//defined in CC.HC (file read)
extern U0 CCPrepDir(CPrep *pp,I64 k,U8 *fn);

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

/* Append n bytes to the destination DST picks: 0 = the output text, 1 = the
 * argument scratch. Growth MOVES the buffer, so nothing may hold a pointer
 * into either one across a put — spans are carried as offsets and re-derived
 * (see CCPrepSpan), and the old buffer is intentionally leaked because
 * poopalloc pages are shared with the rest of the compiler (CCSrc comment in
 * CC.HC). */
U0 CCPrepPut(CCCtrl *cc,CPrep *pp,U8 *s,I64 n,I64 dst)
{
  U8 *nb;
  I64 nc;
  if (!n)
    return;
  if (dst)
  {
    if ((pp->scpos+n+1)>(pp->sccap))
    {
      //The scratch NEVER moves: every scan that is open holds a pointer into
      //it, so growth would leave the outer ones reading a freed buffer. A
      //diagnostic is the honest answer.
      CCPrepErr(cc,"macro argument text too large");
      return;
    }
    MemCpy(pp->sc+pp->scpos,s,n);
    pp->scpos+=n;
  }
  else
  {
    if ((pp->pos+n+1)>(pp->cap))
    {
      nc=pp->cap*2;
      while ((pp->pos+n+1)>nc)
        nc*=2;
      nb=MAlloc(nc);
      MemCpy(nb,pp->buf,pp->pos);
      pp->buf=nb;
      pp->cap=nc;
    }
    MemCpy(pp->buf+pp->pos,s,n);
    pp->pos+=n;
  }
}

/* Re-derive a span pointer from an offset: KIND 0 = the region a scan is
 * reading (fixed), 1 = the scratch (moves on growth). Every multi-step use of
 * an argument text goes through this, because a put between the offset and the
 * read can relocate the scratch. */
U8 *CCPrepSpan(CPrep *pp,U8 *region,I64 kind,I64 off)
{
  if (kind)
    return pp->sc+off;
  return region+off;
}

I64 CCPrepMacro(CCCtrl *cc,U8 *nm)
{
  I64 i;
  for (i=0;i!=cc->mn;i++)
    if (!StrCmp(cc->mname[i],nm))
      return i;
  return -1;
}

/* M31 helpers, all of them token-aware: the preprocessor must treat a literal
 * or a comment as ONE object, because a comma, a paren or a macro name inside
 * one is data. */

/* True when a[0..n) is exactly the NUL-terminated name w. */
Bool CCPrepWordEq(U8 *a,I64 n,U8 *w)
{
  I64 i;
  i=0;
  while (i!=n)
  {
    if (i>=CC_TOK_MAX)
      return FALSE;		//w is clamped, so a longer span cannot match
    if (a[i]!=w[i])
      return FALSE;
    if (!w[i])
      return FALSE;		//w ended early
    i++;
  }
  return !w[i];
}

/* Ordinal of the parameter named w in macro mi, or -1. Names are stored
 * newline-separated in mprm[mi]; the `...` of a variadic list is not stored as
 * a name, so nothing here can ever match `__VA_ARGS__`. */
I64 CCPrepParam(CCCtrl *cc,I64 mi,U8 *w)
{
  U8 *p;
  I64 i,k,ord;
  p=cc->mprm[mi];
  ord=0;
  i=0;
  k=0;
  while (p[i])
  {
    if (p[i]=='\n')
    {
      if (CCPrepWordEq(p+k,i-k,w))
        return ord;
      ord++;
      k=i+1;
    }
    i++;
  }
  if (CCPrepWordEq(p+k,i-k,w))
    return ord;
  return -1;
}

/* Read the identifier at p[i] into w (clamped to the buffer) and return the
 * bytes it consumed, which is what advances every scan: a name longer than
 * CC_TOK_MAX still spends its bytes, it simply matches nothing. Returns 0 when
 * p[i] does not start one. */
I64 CCPrepWord(U8 *p,I64 n,I64 i,U8 *w)
{
  I64 c,start;
  c=0;
  start=i;
  while ((i!=n)&&((p[i]=='_')||((CIsAlpha(p[i]))||(CIsDigit(p[i])))))
  {
    if (c<(CC_TOK_MAX-1))
    {
      w[c]=p[i];
      c++;
    }
    i++;
  }
  w[c]=0;
  return i-start;
}

/* Length of the string/char literal at p[i], quotes and escapes included. An
 * unterminated one runs to the end of the region and the real lexer reports it.
 * Literals are ATOMIC to expansion: a macro name, a comma or a paren inside one
 * is data. (M13 note, kept because it bit once: the copy loop that consumed a
 * literal must BREAK at the closing quote, not zero the remaining count — the
 * bytes after a literal are ordinary code, and zeroing them silently ate the
 * semicolon of every `char *s="x";`.) */
I64 CCPrepLitLen(U8 *p,I64 n,I64 i)
{
  I64 c,closer;
  closer=p[i];
  c=i+1;
  while (c<n)
  {
    if (p[c]=='\\')
      c+=2;
    else if (p[c]==closer)
      return c+1-i;
    else
      c++;
  }
  return n-i;
}

/* Length of the comment at p[i]. A `//` comment stops at the newline (the
 * newline itself is not part of it, so the caller keeps the line); an
 * unterminated block comment takes the rest of the region, as the raw copy
 * that this replaced did. */
I64 CCPrepComLen(U8 *p,I64 n,I64 i)
{
  I64 c;
  if (p[i+1]=='/')
  {
    c=i;
    while ((c<n)&&(p[c]!='\n'))
      c++;
    return c-i;
  }
  c=i+2;
  while ((c+1)<n)
  {
    if ((p[c]=='*')&&(p[c+1]=='/'))
      return c+2-i;
    c++;
  }
  return n-i;
}

/* Drop the whitespace wrapping an argument span. Leading and trailing
 * whitespace is not part of an argument (probed: `#define S(x) #x` stringizes
 * `S( a  b )` as "a b"), and an argument that is ONLY whitespace is empty. */
U0 CCPrepTrim(U8 *p,I64 *pa,I64 *pn)
{
  I64 a,n;
  a=*pa;
  n=*pn;
  while (n)
  {
    if (!((p[a]==' ')||((p[a]=='\t')||((p[a]=='\n')||(p[a]==13)))))
      break;
    a++;
    n--;
  }
  while (n)
  {
    if (!((p[a+n-1]==' ')||((p[a+n-1]=='\t')||((p[a+n-1]=='\n')||(p[a+n-1]==13)))))
      break;
    n--;
  }
  *pa=a;
  *pn=n;
}

/* Any non-whitespace byte in a[0..n)? Used once, to tell `F()` (zero
 * arguments) from `F( )` ... which C also calls zero arguments. */
Bool CCPrepHasText(U8 *a,I64 n)
{
  I64 i;
  i=0;
  while (i!=n)
  {
    if (!((a[i]==' ')||((a[i]=='\t')||((a[i]=='\n')||(a[i]==13)))))
      return TRUE;
    i++;
  }
  return FALSE;
}

/* M31: collect a function-like invocation's argument list. p points at the
 * byte AFTER '(' and n is what remains of the LINE: this preprocessor is
 * line-shaped, so a list that runs off its line is an error (the same kind of
 * edge as an unterminated #if), not a two-line construct. Identifiers,
 * literals and comments are stepped over whole, so a comma or paren inside one
 * never splits the list — probed against gcc: `Q("a)b")` passes ONE argument,
 * and `P2(1 /*c*/, 2)` passes TWO because the comment closed before the comma.
 * Spans are offsets from p, trimmed. An empty list yields ZERO arguments; the
 * caller turns that into one empty argument when the macro has parameters,
 * which is what C means by `Q()` producing `[]`. *pc receives the bytes
 * consumed, counting the ')'. Returns the count, or -1 on a diagnostic. */
I64 CCPrepArgs(CCCtrl *cc,U8 *p,I64 n,I64 *pa,I64 *pn,I64 *pc)
{
  U8 w[CC_TOK_MAX];
  I64 i,depth,argc,start,ln;

  i=0;
  depth=1;
  argc=0;
  start=0;
  while (i!=n)
  {
    if ((p[i]=='_')||(CIsAlpha(p[i])))
      i+=CCPrepWord(p,n,i,w);
    else if ((p[i]=='"')||(p[i]==39))
      i+=CCPrepLitLen(p,n,i);
    else if ((p[i]=='/')&&((p[i+1]=='*')||(p[i+1]=='/')))
      i+=CCPrepComLen(p,n,i);
    else if (p[i]=='(')
    {
      depth++;
      i++;
    }
    else if (p[i]==')')
    {
      depth--;
      i++;
      if (!depth)
        break;			//the list closed here
    }
    else if ((p[i]==',')&&(depth==1))
    {
      if (argc>=CC_MAX_ARGS)
      {
        CCPrepErr(cc,"too many arguments in a macro invocation");
        return -1;
      }
      pa[argc]=start;
      pn[argc]=i-start;
      CCPrepTrim(p,pa+argc,pn+argc);
      argc++;
      i++;
      start=i;
    }
    else
      i++;
  }
  if (depth)
  {
    //ran off the line without ever meeting the ')' that opened this list
    CCPrepErr(cc,"unterminated argument list in a macro invocation");
    return -1;
  }
  *pc=i;
  ln=i-1-start;			//the text before the ')' is an argument too
  if ((argc)||(CCPrepHasText(p+start,ln)))
  {
    if (argc>=CC_MAX_ARGS)
    {
      CCPrepErr(cc,"too many arguments in a macro invocation");
      return -1;
    }
    pa[argc]=start;
    pn[argc]=ln;
    CCPrepTrim(p,pa+argc,pn+argc);
    argc++;
  }
  return argc;
}

/* Copy the first token of an operand span into w and report the bytes it takes
 * in *pt (0 = the span is empty, which a `##` reads as "no right operand").
 * An identifier or a literal is ONE token; anything else is a single byte. */
I64 CCPrepHead(U8 *p,I64 n,U8 *w,I64 *pt)
{
  I64 c,ln;
  if (!n)
  {
    w[0]=0;
    *pt=0;
    return 0;
  }
  if ((p[0]=='_')||(CIsAlpha(p[0])))
  {
    ln=CCPrepWord(p,n,0,w);
    *pt=ln;
    return ln;
  }
  if ((p[0]=='"')||(p[0]==39))
  {
    ln=CCPrepLitLen(p,n,0);
    c=0;
    while ((c!=ln)&&(c<(CC_TOK_MAX-1)))
    {
      w[c]=p[c];
      c++;
    }
    w[c]=0;
    *pt=ln;
    return ln;
  }
  w[0]=p[0];
  w[1]=0;
  *pt=1;
  return 1;
}

/* M31: cut the LAST token off a destination and report what it was: 1 = an
 * identifier (w holds it, *plen its length), 0 = a single punctuation byte
 * (`, ## __VA_ARGS__` is the case that needs that), -1 = nothing to cut,
 * -2 = a token too long to fuse. Whitespace in front of `##` goes with the cut,
 * which is exactly what makes `a ## b` and `a##b` agree. The walk never passes
 * FLOOR, where this macro's body began writing, so a `##` cannot fuse with the
 * text that surrounded the invocation. -1 is the shape C calls
 * "'##' cannot appear at either end of a macro expansion". */
I64 CCPrepTail(CPrep *pp,I64 dst,I64 floor,U8 *w,I64 *plen)
{
  U8 *b;
  I64 pos,i,c,ln;
  if (dst)
    b=pp->sc;
  else
    b=pp->buf;
  if (dst)
    pos=pp->scpos;
  else
    pos=pp->pos;
  i=pos;
  while ((i>floor)&&((b[i-1]==' ')||((b[i-1]=='\t')||((b[i-1]=='\n')||(b[i-1]==13)))))
    i--;
  c=i;
  while ((c>floor)&&((b[c-1]=='_')||((CIsAlpha(b[c-1]))||(CIsDigit(b[c-1])))))
    c--;
  if (c!=i)
  {
    ln=i-c;
    if (ln>(CC_TOK_MAX-1))
    {
      *plen=ln;
      w[0]=0;
      return -2;
    }
    MemCpy(w,b+c,ln);
    w[ln]=0;
    *plen=ln;
    if (dst)
      pp->scpos=c;
    else
      pp->pos=c;
    return 1;
  }
  if (i>floor)
  {
    w[0]=b[i-1];
    w[1]=0;
    *plen=1;
    if (dst)
      pp->scpos=i-1;
    else
      pp->pos=i-1;
    return 0;
  }
  w[0]=0;
  *plen=0;
  return -1;
}

/* M31: C's stringization of raw argument text. Quoted, with `"` and `\`
 * escaped, and every run of whitespace collapsed to ONE space — all three
 * probed against gcc: `S(  a   b  )` becomes "a b", `S("a\"b\\c")` becomes
 * "\"a\\\"b\\\\c\"", and `S('x')` becomes "'x'". The argument is NOT
 * macro-expanded, which is the other half of the rule `##` follows. */
U0 CCPrepStrify(CCCtrl *cc,CPrep *pp,U8 *s,I64 n,I64 dst,I64 *pws)
{
  I64 i,j;
  i=0;
  while (i!=n)
  {
    if ((s[i]==' ')||((s[i]=='\t')||((s[i]=='\n')||(s[i]==13))))
    {
      *pws=1;
      i++;
    }
    else if ((s[i]=='/')&&((i+1)!=n)&&(s[i+1]=='*'))
    {
      //A comment inside a stringized argument became whitespace before the
      //substitution ever happened, so it contributes a space and not its own
      //characters (probed: S(a  /**/  b) becomes "a b"). A `//` comment cannot
      //appear here: it would have run to the end of the line and left the
      //argument list unterminated.
      j=i+2;
      while ((j+1)!=n)
      {
        if ((s[j]=='*')&&(s[j+1]=='/'))
          break;
        j++;
      }
      if ((j+1)==n)
        i=n;			//unterminated: the rest of the argument
      else
        i=j+2;
      *pws=1;
    }
    else
    {
      if (*pws)
      {
        CCPrepPut(cc,pp," ",1,dst);
        *pws=0;
      }
      if ((s[i]=='"')||(s[i]=='\\'))
        CCPrepPut(cc,pp,"\\",1,dst);
      CCPrepPut(cc,pp,s+i,1,dst);
      i++;
    }
  }
}

/* M31: split argument TEXT on its top-level commas and return the pieces as
 * (offset,length) pairs relative to p. This is how an inner invocation counts
 * its arguments: the text arriving here has already had the enclosing macro's
 * parameters substituted into it, so `1,2` is two arguments even though it came
 * from one written token (`__VA_ARGS__`). Parens, literals and comments are
 * stepped over whole, exactly as in CCPrepArgs. Returns the count, or -1 if
 * there are too many pieces. */
I64 CCPrepSplit(CCCtrl *cc,U8 *p,I64 n,I64 *pa,I64 *pn)
{
  U8 w[CC_TOK_MAX];
  I64 i,depth,cnt,start,ln;

  i=0;
  depth=0;
  cnt=0;
  start=0;
  while (i!=n)
  {
    if ((p[i]=='_')||(CIsAlpha(p[i])))
      i+=CCPrepWord(p,n,i,w);
    else if ((p[i]=='"')||(p[i]==39))
      i+=CCPrepLitLen(p,n,i);
    else if ((p[i]=='/')&&((p[i+1]=='*')||(p[i+1]=='/')))
      i+=CCPrepComLen(p,n,i);
    else if (p[i]=='(')
    {
      depth++;
      i++;
    }
    else if (p[i]==')')
    {
      if (depth)
        depth--;
      i++;
    }
    else if ((p[i]==',')&&(!depth))
    {
      if (cnt>=CC_MAX_ARGS)
      {
        CCPrepErr(cc,"too many arguments in a macro invocation");
        return -1;
      }
      pa[cnt]=start;
      pn[cnt]=i-start;
      CCPrepTrim(p,pa+cnt,pn+cnt);
      cnt++;
      i++;
      start=i;
    }
    else
      i++;
  }
  ln=n-start;
  if ((cnt)||(CCPrepHasText(p+start,ln)))
  {
    if (cnt>=CC_MAX_ARGS)
    {
      CCPrepErr(cc,"too many arguments in a macro invocation");
      return -1;
    }
    pa[cnt]=start;
    pn[cnt]=ln;
    CCPrepTrim(p,pa+cnt,pn+cnt);
    cnt++;
  }
  return cnt;
}

/* TRUE when the next token in this macro body is `##`. That decides whether
 * the parameter about to be substituted is an OPERAND of it, and therefore
 * substituted unexpanded (probed: with `#define ONE 1` and
 * `#define CAT(a,b) a##b`, CAT(ONE,x) yields ONEx, not 1x). */
Bool CCPrepPeekPaste(U8 *p,I64 n,I64 i)
{
  while ((i!=n)&&((p[i]==' ')||((p[i]=='\t')||((p[i]=='\n')||(p[i]==13)))))
    i++;
  if (((i+1)!=n)&&(p[i]=='#')&&(p[i+1]=='#'))
    return TRUE;
  return FALSE;
}

/* Emit the variadic extras of the current invocation into the destination: the
 * expanded arguments joined with single commas, or (RAW) their text as written.
 * Returns the bytes emitted — the answer a `##` needs to know whether its
 * operand vanished, and the reason the join is emitted in place instead of being
 * materialized first: when the left operand of a paste is a comma there is
 * nothing to fuse, so in-place emission is precisely what C produces. */
I64 CCPrepVaEmit(CCCtrl *cc,CPrep *pp,CMacScan *ms,I64 raw)
{
  CMacExp *ex;
  I64 k,first,cnt;
  ex=ms->ex;
  first=1;
  cnt=0;
  k=cc->mnarg[ex->mi];		//the named parameters are not part of this
  while (k!=ex->narg)
  {
    if (!first)
    {
      CCPrepPut(cc,pp,",",1,ms->dst);
      cnt++;
    }
    first=0;
    if (raw)
    {
      if (ex->rawn[k])
      {
        CCPrepPut(cc,pp,pp->sc+ex->rawa[k],ex->rawn[k],ms->dst);
        cnt+=ex->rawn[k];
      }
    }
    else if (ex->expn[k])
    {
      CCPrepPut(cc,pp,pp->sc+ex->expa[k],ex->expn[k],ms->dst);
      cnt+=ex->expn[k];
    }
    k++;
  }
  return cnt;
}


/* M31: finish a `##`. The right operand's text (possibly empty) arrives here,
 * the left side is already cut out of the destination and held in the
 * invocation. C expands NEITHER operand but does expand the token they FORM,
 * and the two halves of that are pinned separately: with `#define code 5`,
 * CAT(co,de) gives 5, while with `#define Z 3`, `a##Z` gives 1Z — the fused
 * text is looked up, and `1Z` simply is not a name. An EMPTY operand deletes
 * the left side, which is the `, ## __VA_ARGS__` idiom. Whatever follows the
 * operand's first token is emitted verbatim: it is argument text adjacent to a
 * `##`, so C leaves it unexpanded too. */
U0 CCPrepFuse(CCCtrl *cc,CPrep *pp,CMacScan *ms,U8 *s,I64 ln)
{
  CMacScan bs;
  U8 tok[CC_TOK_MAX],fused[CC_TOK_MAX];
  I64 tb,rem,fl,fmi;

  ms->ex->pnext=0;
  if (!ln)
    return;				//empty operand: the left side stays deleted
  CCPrepHead(s,ln,tok,&tb);
  fl=ms->ex->pendlen;
  if ((fl+tb)>(CC_TOK_MAX-1))
  {
    CCPrepErr(cc,"macro paste token too long to fuse");
    return;
  }
  MemCpy(fused,ms->ex->pend,fl);
  MemCpy(fused+fl,tok,tb);
  fused[fl+tb]=0;
  //C requires the pasted result to be ONE valid preprocessing token, and gcc
  //makes that a diagnostic ("pasting \"1\" and \"+\" does not give a valid
  //preprocessing token" for `a##+##b`). NOT checked here: a check is only
  //worth having if it can fire, and CCPrepTail hands back either a whole
  //identifier or a single byte, so every fused text is name+name (always a
  //token), punct+punct (`->`, `<=`: also tokens) or name+punct — and name+
  //punct cannot be rejected either, because `1`##`.` is the pp-number `1.`.
  //The rule needs the token classification the real lexer does, so the
  //permissive answer is documented rather than half-checked.
  rem=ln-tb;
  fmi=-1;
  if ((fl&&((fused[0]=='_')||(CIsAlpha(fused[0]))))&&((tb&&((tok[0]=='_')||(CIsAlpha(tok[0]))))||!tb))
    fmi=CCPrepMacro(cc,fused);		//only an identifier-shaped result can name one
  if ((fmi>=0)&&(!(ms->mask&(1<<fmi)))&&(cc->mnarg[fmi]<0))
  {
    bs.p=cc->mbody[fmi];
    bs.n=StrLen(cc->mbody[fmi]);
    bs.i=0;
    bs.mask=ms->mask|(1<<fmi);
    bs.dst=ms->dst;
    bs.ex=NULL;
    bs.isarg=0;			//the region is a macro body
    bs.incond=ms->incond;		//and a body is a RESCAN of the line
    bs.subonly=ms->subonly;
    CCPrepScan(cc,pp,&bs);
    if (cc->errs)
      return;
  }
  else
    CCPrepPut(cc,pp,fused,fl+tb,ms->dst);
  if (rem)
    CCPrepPut(cc,pp,s+tb,rem,ms->dst);	//the operand's own remaining text
}

/* M31: emit one identifier and spend the region bytes it takes — more than the
 * name itself when a function-like macro's argument list was eaten. Rules, each
 * one pinned against `gcc -E`:
 *   - an argument is expanded ONCE, in the context of its INVOCATION rather
 *     than of the macro being expanded, and is then substituted verbatim:
 *     F1(F1(5)) gives ((5)), and with `#define A A+1` plus `#define F(x) x`,
 *     F(A) gives A+1 and NOT A+1+1.
 *   - a name whose macro is already open on this path stays a name (blue
 *     paint), which is what makes a self-referential body terminate:
 *     `#define SELF SELF+1` expands once, and AA/BB mutual recursion stops at
 *     AA+1 +1 the way gcc's output does.
 *   - a function-like name is an invocation only when '(' follows it, and
 *     whitespace between them is allowed (SP (7) expands) while a bare name is
 *     just an identifier (a variable may be called ADD2 in the same file).
 *   - a parameter's own name shadows a macro of the same name, because
 *     substitution happens first (with `#define X 9` and `#define FX(X) [(X)]`,
 *     FX(1) gives [(1)]). */
U0 CCPrepIdent(CCCtrl *cc,CPrep *pp,CMacScan *ms,U8 *w,I64 wl)
{
  CMacScan bs;
  CMacExp nx;
  I64 ea[CC_MAX_ARGS];
  I64 en[CC_MAX_ARGS];
  CMacExp *ex;
  I64 sa2[CC_MAX_ARGS];
  I64 en2[CC_MAX_ARGS];
  I64 suba[CC_MAX_ARGS];
  I64 subl[CC_MAX_ARGS];
  U8 *p;
  I64 n,dst,par,va,mi,gap,argc,k,mark,bodymark,fl,used,cnt,nprm,sv,sf,nact,j2,vempty,kk;

  p=ms->p;
  n=ms->n;
  dst=ms->dst;
  ex=ms->ex;
  par=-1;
  va=0;
  if (ex)
  {
    par=CCPrepParam(cc,ex->mi,w);
    if ((par<0)&&(cc->mvar[ex->mi])&&(!StrCmp(w,"__VA_ARGS__")))
      va=1;				//not a parameter: this invocation's extras
  }
  if ((par<0)&&(!va)&&(!StrCmp(w,"__VA_ARGS__"))&&((ex)||(ms->isarg)))
  {
    //`__VA_ARGS__` can only name the extras of the invocation whose body is
    //being scanned. With no invocation, or a non-variadic one, there are no
    //extras, and C makes that an error: a top-level `V(__VA_ARGS__)` is
    //rejected even though V's own definition is perfectly legal, so this is the
    //half of the rule that CCPrepCheckBody cannot see. It is limited to text
    //that arrived FROM THE MACHINERY — a body, or an argument list — on
    //purpose: this prep scans one line at a time, so the continuation lines of
    //a multi-line block comment are ordinary text here, and a manual that
    //mentions `__VA_ARGS__` in a sentence must not fail to compile. The real
    //lexer sees that prose as a comment either way, so being permissive there
    //costs an unreported misuse, while being strict costs a file.
    CCPrepErr(cc,"__VA_ARGS__ is only meaningful inside a variadic macro expansion");
    ms->i=n;
    return;
  }
  if ((ex)&&(ex->pnext))
  {
    //---- this identifier is the RIGHT operand of a `##` ----
    if (va)
    {
      //Emission order decides this one: the left side has to come first, so the
      //emptiness question is answered by the raw text of the extras, and the
      //answer is measured over ALL of them (a single zero-length extra is still
      //an argument). C deletes the token before `##` only when it follows a
      //`__VA_ARGS__` that matched nothing AND that token is a comma; every other
      //left operand survives (probed: `x##__VA_ARGS__` with no extras keeps x,
      //while `(h , ## __VA_ARGS__)` keeps the `(h )`).
      fl=0;
      k=cc->mnarg[ex->mi];
      vempty=1;
      kk=k;		//the extras start at the named-parameter count
      while (kk!=ex->narg)
      {
        if (ex->rawn[kk])
          vempty=0;
        kk++;
      }
      if ((vempty)&&((ex->pendlen==1)&&(ex->pend[0]==',')))
        fl=1;
      if (!fl)
        CCPrepPut(cc,pp,ex->pend,ex->pendlen,dst);
      //CCPrepFuse clears the paste flag on the paths that reach it; this one
      //does not go through it, so it clears it here. Leaving it set made the
      //NEXT body token look like a second operand, which is how `GC(1)`
      //produced `(1 ,)`: the paste re-fired and pasted the held comma onto the
      //closing paren.
      ex->pnext=0;
      CCPrepVaEmit(cc,pp,ms,1);
      ms->i+=wl;
      return;
    }
    if (par>=0)
    {
      //C substitutes an operand of `##` unexpanded, so the raw text is used
      fl=ex->rawa[par];
      used=ex->rawn[par];
      if (used)
        CCPrepFuse(cc,pp,ms,pp->sc+fl,used);
      else
      {
        //An empty right operand yields the LEFT one (probed: P(1,) gives 1), so
        //the side that was cut to be fused has to be put back. Deleting a token
        //is specific to `## __VA_ARGS__`, which is the path above.
        if (ex->pendlen)
          CCPrepPut(cc,pp,ex->pend,ex->pendlen,dst);
        ex->pnext=0;
      }
      ms->i+=wl;
      return;
    }
    //a plain name: the operand is this very token, unexpanded, and a function-
    //like macro behind it is NOT invoked because the name is being pasted
    CCPrepFuse(cc,pp,ms,p+ms->i,wl);
    ms->i+=wl;
    return;
  }
  if (par>=0)
  {
    if (CCPrepPeekPaste(p,n,ms->i+wl))
    {
      if (ex->rawn[par])
        CCPrepPut(cc,pp,pp->sc+ex->rawa[par],ex->rawn[par],dst);
    }
    else if (ex->expn[par])
      CCPrepPut(cc,pp,pp->sc+ex->expa[par],ex->expn[par],dst);
    ms->i+=wl;
    return;
  }
  if (va)
  {
    CCPrepVaEmit(cc,pp,ms,0);
    ms->i+=wl;
    return;
  }
  if ((ms->incond)&&(!StrCmp(w,"defined")))
  {
    //Reached only for a `defined` that is not a parameter of the enclosing
    //invocation: a parameter named `defined` is ordinary substitution, and the
    //paste and `__VA_ARGS__` paths above already returned.
    CCPrepDefined(cc,pp,ms,wl);
    return;
  }
  mi=CCPrepMacro(cc,w);
  if ((mi<0)||(ms->subonly))
  {
    //in the substitution pass a macro name is just text: the invocation it would
    //open belongs to the stream this text is being placed INTO
    mi=-1;
  }
  if (mi<0)
  {
    CCPrepPut(cc,pp,p+ms->i,wl,dst);	//not a macro name
    ms->i+=wl;
    return;
  }
  if (ms->mask&(1<<mi))
  {
    //blue paint: this macro's own expansion is still open further up the path
    CCPrepPut(cc,pp,p+ms->i,wl,dst);
    ms->i+=wl;
    return;
  }
  nprm=cc->mnarg[mi];
  if (nprm<0)
  {
    //object-like: the body is scanned with THIS macro now disabled
    bs.p=cc->mbody[mi];
    bs.n=StrLen(cc->mbody[mi]);
    bs.i=0;
    bs.mask=ms->mask|(1<<mi);
    bs.dst=dst;
    bs.ex=NULL;
    bs.isarg=0;			//the region is a macro body
    bs.incond=ms->incond;		//and a body is a RESCAN of the line
    bs.subonly=ms->subonly;
    CCPrepScan(cc,pp,&bs);
    ms->i+=wl;
    return;
  }
  //---- function-like: an invocation only when '(' follows the name ----
  gap=ms->i+wl;
  while ((gap!=n)&&((p[gap]==' ')||(p[gap]=='\t')))
    gap++;
  if ((gap==n)||(p[gap]!='('))
  {
    CCPrepPut(cc,pp,p+ms->i,wl,dst);	//a function-like name on its own is a name
    ms->i+=wl;
    return;
  }
  argc=CCPrepArgs(cc,p+gap+1,n-(gap+1),ea,en,&used);
  if (argc<0)
  {
    ms->i=n;				//the diagnostic already fired
    return;
  }
  nx.mi=mi;
  nx.pnext=0;
  nx.pendlen=0;
  nx.pend[0]=0;
  nx.vraw=NULL;
  nx.vrawn=0;
  if ((cc->mvar[mi])&&(argc>nprm))
  {
    //`#__VA_ARGS__` stringizes the variadic tail AS WRITTEN, commas and the
    //spacing around them included (probed: SV(1, 2) becomes "1, 2"). Only the
    //source region holds that text: the per-argument spans, trimmed and copied
    //one at a time, lose the separator between arguments, which is what turned
    //"1, 2" into "12". Copy it here, BEFORE mark, because the compaction that
    //ends this invocation slides the body down over everything from mark onward
    //and would erase it.
    nx.vraw=pp->sc+pp->scpos;
    nx.vrawn=(ea[argc-1]+en[argc-1])-ea[nprm];
    CCPrepPut(cc,pp,p+gap+1+ea[nprm],nx.vrawn,1);
    if (cc->errs)
    {
      ms->i=n;
      return;
    }
  }
  mark=pp->scpos;
  //Pass 1 — substitute what the ENCLOSING invocation (if any) can substitute,
  //and expand nothing. C decides an inner macro's argument list from the
  //SUBSTITUTED token stream, not from the body text as written, and a `#` or
  //`##` operand must see the argument as the enclosing body leaves it. In
  //ordinary text (ms->ex NULL) this pass is a copy.
  k=0;
  while (k!=argc)
  {
    suba[k]=pp->scpos;
    bs.p=p+gap+1+ea[k];
    bs.n=en[k];
    bs.i=0;
    bs.mask=ms->mask;
    bs.dst=1;
    bs.subonly=1;
    bs.isarg=1;			//the region IS an argument text
    bs.incond=0;			//an argument is expanded as its OWN line:
				//`defined` is an operator only in the stream
				//this text is placed into (probed: with
				//`#define A 1` and `#define K(x) x`,
				//`#if K(defined(A))` expands the argument to
				//`defined(1)` and diagnoses THAT, not 1)
    bs.ex=ms->ex;
    sv=0;
    sf=0;
    if (bs.ex)
    {
      //The paste state belongs to the body: a `##` written inside an argument
      //is ordinary tokens until that argument lands in a body. Give the span
      //its own floor so a cut inside it cannot reach the body's output.
      sv=bs.ex->pnext;
      sf=bs.ex->floor;
      bs.ex->pnext=0;
      bs.ex->floor=pp->scpos;
    }
    CCPrepScan(cc,pp,&bs);
    if (bs.ex)
    {
      bs.ex->pnext=sv;
      bs.ex->floor=sf;
    }
    subl[k]=pp->scpos-suba[k];
    k++;
    if (cc->errs)
    {
      ms->i=n;
      return;
    }
  }
  //Pass 2 — split each substituted text on its top-level commas: THAT is the
  //argument list the inner macro sees. `Q(__VA_ARGS__)` against a two-parameter
  //Q is a two-argument call, and `#define T(x) S(x)` hands S the text `1+1`.
  nact=0;
  k=0;
  while (k!=argc)
  {
    cnt=CCPrepSplit(cc,pp->sc+suba[k],subl[k],sa2,en2);
    if (cnt<0)
    {
      ms->i=n;
      return;
    }
    if (!cnt)
    {
      //An argument written empty (`T(5,)`) stays ONE empty argument even
      //though its text splits into nothing: CCPrepSplit reports "no pieces",
      //which is not the same question.
      sa2[0]=0;
      en2[0]=0;
      cnt=1;
    }
    j2=0;
    while (j2!=cnt)
    {
      if (nact>=CC_MAX_ARGS)
      {
        CCPrepErr(cc,"too many arguments in a macro invocation");
        ms->i=n;
        return;
      }
      nx.rawa[nact]=suba[k]+sa2[j2];
      nx.rawn[nact]=en2[j2];
      nact++;
      j2++;
    }
    k++;
  }
  argc=nact;
  nx.narg=argc;
  if (!argc&&(nprm>0))
  {
    nx.rawa[0]=pp->scpos;		//C: `Q()` with parameters IS one empty
    nx.rawn[0]=0;			//argument (probed: Q() gives [])
    nx.narg=1;
    argc=1;
  }
  if (argc<nprm)
  {
    "CC ERROR: prep: macro '%s' requires %d arguments, but only %d given\n",
        cc->mname[mi],nprm,argc;
    cc->errs++;
    ms->i=n;
    return;
  }
  if ((argc>nprm)&&(!cc->mvar[mi]))
  {
    "CC ERROR: prep: macro '%s' passed %d arguments, but takes just %d\n",
        cc->mname[mi],argc,nprm;
    cc->errs++;
    ms->i=n;
    return;
  }
  //Pass 3 — expand each argument exactly once, in the INVOCATION's context
  //(`F1(F1(5))` gives ((5)) because the inner call is not disabled yet).
  k=0;
  while (k!=argc)
  {
    nx.expa[k]=pp->scpos;
    bs.p=pp->sc+nx.rawa[k];
    bs.n=nx.rawn[k];
    bs.i=0;
    bs.mask=ms->mask;			//the caller's mask, not this macro's
    bs.dst=1;
    bs.subonly=0;
    bs.isarg=1;			//the region IS an argument text
    bs.incond=0;			//see the pass-1 note: argument text is
				//expanded outside the #if's operator scope
    bs.ex=ms->ex;			//its parameters are already in the
    sv=0;				//text; this only carries the context a
    sf=0;				//nested call or a `__VA_ARGS__` needs
    if (bs.ex)
    {
      sv=bs.ex->pnext;
      sf=bs.ex->floor;
      bs.ex->pnext=0;
      bs.ex->floor=pp->scpos;
    }
    CCPrepScan(cc,pp,&bs);
    if (bs.ex)
    {
      bs.ex->pnext=sv;
      bs.ex->floor=sf;
    }
    nx.expn[k]=pp->scpos-nx.expa[k];
    k++;
    if (cc->errs)
    {
      ms->i=n;
      return;
    }
  }
  if (dst)
    fl=pp->scpos;
  else
    fl=pp->pos;
  nx.floor=fl;
  bodymark=fl;
  bs.p=cc->mbody[mi];
  bs.n=StrLen(cc->mbody[mi]);
  bs.i=0;
  bs.mask=ms->mask|(1<<mi);		//and this macro is now disabled on the path
  bs.dst=dst;
  bs.ex=&nx;
  bs.isarg=0;			//the region is a macro body
  bs.incond=ms->incond;		//and a body is a RESCAN of the line, so
				//`defined` written in one is an operator
  bs.subonly=0;
  CCPrepScan(cc,pp,&bs);
  cnt=0;
  if (dst)
  {
    //The argument texts sit under the body as scratch. When the body is going
    //into the scratch (this invocation happened while an argument was being
    //expanded) they have to GO: the enclosing argument text is read as one
    //contiguous span, so the body slides down over them.
    cnt=pp->scpos-bodymark;
    MemCpy(pp->sc+mark,pp->sc+bodymark,cnt);
    pp->scpos=mark+cnt;
  }
  else
    pp->scpos=mark;			//the body reached the output already
  ms->i=gap+1+used;
}

/* Emit one region of text (a line of source, or a macro body) into the
 * destination ms picks, expanding identifiers along the way. Literals and
 * comments are copied as single objects so nothing inside them is substituted,
 * and every byte of the region is accounted for: an identifier spends either
 * its own bytes or the invocation it opened. */
U0 CCPrepScan(CCCtrl *cc,CPrep *pp,CMacScan *ms)
{
  U8 tok[CC_TOK_MAX];
  I64 wl,j,ln,par,ws;
  U8 *p;
  I64 n,dst;

  p=ms->p;
  n=ms->n;
  dst=ms->dst;
  while ((ms->i!=n)&&(!cc->errs))
  {
    if ((p[ms->i]=='_')||(CIsAlpha(p[ms->i])))
    {
      wl=CCPrepWord(p,n,ms->i,tok);
      CCPrepIdent(cc,pp,ms,tok,wl);
    }
    else if ((ms->ex)&&(p[ms->i]=='#')&&(!ms->subonly)&&(!ms->ex->pnext))
    {
      if (((ms->i+1)!=n)&&(p[ms->i+1]=='#'))
      {
        //`##`: cut the left operand out of the destination and hold it. That
        //is what deletes it when the right operand turns out to be empty, and
        //it drops the whitespace a paste swallows.
        ws=CCPrepTail(pp,dst,ms->ex->floor,tok,&ln);
        if (ws==-2)
        {
          CCPrepErr(cc,"macro paste token too long to fuse");
          return;
        }
        if (ws==-1)
        {
          //Nothing was there to cut: substitution left this operand empty. A
          //paste with one operand yields the OTHER one (probed against gcc:
          //`#define P(a,b) a##b` gives 2 for P(,2) and 1 for P(1,)), so the run
          //carries an empty left side forward instead of diagnosing. A body that
          //starts with `##` never gets here — CCPrepCheckBody rejects it when the
          //macro is defined.
          ln=0;
        }
        StrCpy(ms->ex->pend,tok);
        ms->ex->pendlen=ln;
        ms->ex->pnext=1;
        ms->i+=2;
      }
      else
      {
        //# param: stringize. The space between them is allowed, and the
        //argument is used as written — never macro-expanded.
        j=ms->i+1;
        while ((j!=n)&&((p[j]==' ')||((p[j]=='\t')||((p[j]=='\n')||(p[j]==13)))))
          j++;
        wl=CCPrepWord(p,n,j,tok);
        if (!wl)
        {
          CCPrepErr(cc,"#' in a macro body must be followed by a parameter");
          return;
        }
        par=CCPrepParam(cc,ms->ex->mi,tok);
        ws=0;
        CCPrepPut(cc,pp,"\"",1,dst);
        if (par>=0)
        {
          if (ms->ex->rawn[par])
            CCPrepStrify(cc,pp,pp->sc+ms->ex->rawa[par],ms->ex->rawn[par],dst,&ws);
        }
        else if ((cc->mvar[ms->ex->mi])&&(!StrCmp(tok,"__VA_ARGS__")))
        {
          if (ms->ex->vrawn)
            CCPrepStrify(cc,pp,ms->ex->vraw,ms->ex->vrawn,ms->dst,&ws);
        }
        else
        {
          CCPrepErr(cc,"#' in a macro body must be followed by a parameter");
          return;
        }
        if (ws)
          CCPrepPut(cc,pp," ",1,dst);
        CCPrepPut(cc,pp,"\"",1,dst);
        ms->i=j+wl;
      }
    }
    else if ((ms->ex)&&(ms->ex->pnext)&&((p[ms->i]==' ')||((p[ms->i]=='\t')||((p[ms->i]=='\n')||(p[ms->i]==13)))))
    {
      //whitespace between `##` and its operand is not part of the paste
      ms->i++;
    }
    else if ((p[ms->i]=='"')||(p[ms->i]==39))
    {
      //A literal is opaque: neither its contents nor a name inside it are
      //parameters, and it is copied whole. The copy must stop at the closing
      //quote rather than at the end of the count — the bytes after a literal
      //are ordinary code (M13: zeroing the count there silently ate the
      //semicolon of every `char *s="x";`).
      ln=CCPrepLitLen(p,n,ms->i);
      if ((ms->ex)&&(ms->ex->pnext))
        CCPrepFuse(cc,pp,ms,p+ms->i,ln);	//a literal pastes by concatenation
      else
        CCPrepPut(cc,pp,p+ms->i,ln,dst);
      ms->i+=ln;
    }
    else if ((p[ms->i]=='/')&&(((ms->i+1)!=n)&&((p[ms->i+1]=='*')||(p[ms->i+1]=='/'))))
    {
      //Comments pass through as raw bytes for the real lexer, and to a `##`
      //they are whitespace: the operand after one is still the next token.
      ln=CCPrepComLen(p,n,ms->i);
      CCPrepPut(cc,pp,p+ms->i,ln,dst);
      ms->i+=ln;
    }
    else
    {
      if ((ms->ex)&&(ms->ex->pnext))
        CCPrepFuse(cc,pp,ms,p+ms->i,1);	//one byte is a whole operand
      else
        CCPrepPut(cc,pp,p+ms->i,1,dst);
      ms->i++;
    }
  }
  if ((ms->ex)&&(ms->ex->pnext)&&(!cc->errs))
    CCPrepErr(cc,"## at the end of a macro body");	//an operand was owed
}
/* Skip whitespace AND comments in a scan region. `defined` may be separated
 * from its operand or its parenthesis by either: translation phase 3 replaced
 * every comment with a space before the directive was even recognised. Returns
 * the position of the next significant byte, or n. */
I64 CCPrepSkipWS(U8 *p,I64 n,I64 i)
{
  while (i!=n)
  {
    if ((p[i]==' ')||((p[i]=='\t')||((p[i]=='\n')||(p[i]==13))))
      i++;
    else if ((p[i]=='/')&&((i+1)!=n)&&((p[i+1]=='*')||(p[i+1]=='/')))
      i+=CCPrepComLen(p,n,i);
    else
      return i;
  }
  return n;
}

/* M32: handle `defined` in a controlling expression. The operand is read as a
 * NAME and is never macro-expanded — that is the whole content of the operator
 * (probed: with `#define HAVE 1`, `#if defined(HAVE)` is 1 and NOT `defined(1)`)
 * — except that a PARAMETER in that position stands for its argument's EXPANDED
 * text, because by the time a body is rescanned the substitution has happened
 * (probed: `#define ISDEF(x) defined(x)` with `#define HAVE2 1` reports
 * "operator defined requires an identifier", while the same call with an
 * undefined argument is quietly 0). Both the parenthesised and the bare form
 * count (`#if defined X` is 0 for an undefined X, probed). The answer is
 * emitted as a literal 1 or 0 so the evaluator below never has to know the
 * operator existed. Returns after advancing ms->i, or after reporting. */
U0 CCPrepDefined(CCCtrl *cc,CPrep *pp,CMacScan *ms,I64 wl)
{
  U8 w2[CC_TOK_MAX];
  U8 *p,*s;
  CMacExp *ex;
  I64 n,dst,i,par,open,ln,j,len,ok;

  p=ms->p;
  n=ms->n;
  dst=ms->dst;
  ex=ms->ex;
  open=0;
  i=CCPrepSkipWS(p,n,ms->i+wl);
  if ((i!=n)&&(p[i]=='('))
  {
    open=1;
    i=CCPrepSkipWS(p,n,i+1);
  }
  ln=CCPrepWord(p,n,i,w2);
  i+=ln;
  par=-1;
  if (ex)
    par=CCPrepParam(cc,ex->mi,w2);
  ok=0;
  if (par>=0)
  {
    //the operand is a parameter of the invocation whose body this is
    s=pp->sc+ex->expa[par];
    len=ex->expn[par];
    j=CCPrepSkipWS(s,len,0);
    ln=CCPrepWord(s,len,j,w2);
    if (((ln)&&(((w2[0]=='_')||(CIsAlpha(w2[0])))))&&
        ((j+ln)==CCPrepSkipWS(s,len,j+ln)))
      ok=1;			//ONE identifier, and nothing else
  }
  else if ((ln)&&(((w2[0]=='_')||(CIsAlpha(w2[0])))))
    ok=1;
  if (!ok)
  {
    CCPrepErr(cc,"the operand of defined must be an identifier");
    ms->i=n;
    return;
  }
  if (open)
  {
    j=CCPrepSkipWS(p,n,i);
    if (((j==n)||(p[j]!=')')))
    {
      CCPrepErr(cc,"missing ')' after defined");
      ms->i=n;
      return;
    }
    i=j+1;
  }
  if (CCPrepMacro(cc,w2)>=0)
    CCPrepPut(cc,pp,"1",1,dst);
  else
    CCPrepPut(cc,pp,"0",1,dst);
  ms->i=i;
}

/* M32: the controlling expression of `#if` and `#elif`. What arrives here has
 * already been macro-expanded, with `defined` replaced by 1 or 0 during that
 * scan, so what is left is numbers, character constants, operators,
 * parentheses and the identifiers that C evaluates as ZERO (6.10.1/3: after
 * expansion, a name that is still a name has the value 0). A name is
 * deliberately NOT the start of a call here: `#if NOPE(1)` with NOPE undefined
 * is a syntax error in C, and reading it as a call would invent a rule.
 * `live` is how short-circuiting is expressed — `&&`, `||` and `?:` parse both
 * operands but only one is live, and division by zero is a diagnostic only for
 * a live one (probed: `#if 1 || 1/0` and `#if 0 && 1/0` are not division
 * errors, and faulting on them would stop a compile that C completes). */
class CPrepExpr
{
  U8	*p;
  I64	n;
  I64	i;		//read position
  I64	bad;		//a diagnostic has fired: stop evaluating
  I64	live;		//0 = this operand is parsed, not evaluated
};

I64 CCPrepExBad(CCCtrl *cc,CPrepExpr *pe,U8 *msg)
{
  if (!pe->bad)
  {
    CCPrepErr(cc,msg);
    pe->bad=1;
  }
  return 0;
}

/* Whitespace, and comments as whitespace: translation phase 3 replaced a
 * comment with a space long before the directive was recognised, so
 * `#if 1 /* x */ == 1` takes. */
U0 CCPrepExSpace(CPrepExpr *pe)
{
  while (pe->i!=pe->n)
  {
    if ((pe->p[pe->i]==' ')||((pe->p[pe->i]=='\t')||((pe->p[pe->i]=='\n')||(pe->p[pe->i]==13))))
      pe->i++;
    else if ((pe->p[pe->i]=='/')&&((pe->i+1)!=pe->n)&&((pe->p[pe->i+1]=='*')||(pe->p[pe->i+1]=='/')))
      pe->i+=CCPrepComLen(pe->p,pe->n,pe->i);
    else
      return;
  }
}

/* One integer literal. Rules are THIS compiler's, not gcc's, on purpose: hex
 * and DECIMAL, where a leading zero means decimal because `int x = 017;` is 17
 * to this front end (no octal) — a literal meaning 15 in a condition and 17 in
 * the code under it would be worse than the divergence. `0b` is accepted for
 * parity with gcc even though the C front end rejects it, and a trailing
 * u/U/l/L is consumed and ignored, because accepting a suffix can only turn a
 * rejection into an answer. */
I64 CCPrepExNumber(CPrepExpr *pe,I64 *pv)
{
  U8 *p;
  I64 i,v,ch,rad;

  p=pe->p;
  i=pe->i;
  v=0;
  rad=10;
  if ((p[i]=='0')&&((i+1)<pe->n)&&(((p[i+1]=='x')||(p[i+1]=='X'))||((p[i+1]=='b')||(p[i+1]=='B'))))
  {
    if ((p[i+1]=='b')||(p[i+1]=='B'))
      rad=2;
    else
      rad=16;
    i+=2;
  }
  while (i!=pe->n)
  {
    ch=p[i];
    if (CIsDigit(ch))
      ch=ch-'0';
    else if ((ch>='a')&&(ch<='f'))
      ch=ch-'a'+10;
    else if ((ch>='A')&&(ch<='F'))
      ch=ch-'A'+10;
    else
      break;
    if (ch>=rad)
      break;		//`0b2` and a hex-only letter stop and stay for the
			//caller to complain about
    v=v*rad+ch;
    i++;
  }
  while ((i!=pe->n)&&(((p[i]=='u')||((p[i]=='U')||((p[i]=='l')||(p[i]=='L'))))))
    i++;
  pe->i=i;
  *pv=v;
  return TRUE;
}

/* A character constant, decoded with the SAME table the literals in the code
 * below it use (CCEscape in CC_LEX.HC) — a condition and the code it guards may
 * not read one differently. A multi-character constant packs left to right
 * (`'ab'` is 0x6162), which is what gcc produces as well. */
I64 CCPrepExCharLit(CCCtrl *cc,CPrepExpr *pe,I64 *pv)
{
  U8 *p;
  U8 *np;
  I64 i,v,ch;

  p=pe->p;
  i=pe->i+1;		//past the opening quote
  v=0;
  while ((i!=pe->n)&&(p[i]!=39))
  {
    ch=p[i];
    i++;
    if (ch=='\\')
    {
      if (i==pe->n)
        return CCPrepExBad(cc,pe,"unterminated character constant in a #if expression");
      ch=p[i];
      i++;
      if ((ch=='x')&&((i==pe->n)||(!CIsHexCh(p[i]))))
      {
        //CCEscape would report this through the LEXER's message, which carries
        //a token context this expression has none of; asking first keeps the
        //diagnostic in the preprocessor's own words.
        return CCPrepExBad(cc,pe,"\\x needs a hex digit in a #if character constant");
      }
      np=p+i;
      CCEscape(cc,np,ch,&ch,&np);	//FALSE keeps the byte: an escaped
      i=np-pe->p;			//quote, or an unknown escape
    }
    v=v*256+ch;		//multi-character constants pack left to right
  }
  if (i==pe->n)
    return CCPrepExBad(cc,pe,"unterminated character constant in a #if expression");
  pe->i=i+1;		//past the closing quote
  *pv=v;
  return TRUE;
}

extern I64 CCPrepExQ(CCCtrl *cc,CPrepExpr *pe);	//see the extern note: a
//plain prototype here makes a second CCPrepExQ

I64 CCPrepExPrim(CCCtrl *cc,CPrepExpr *pe)
{
  U8 w[CC_TOK_MAX];
  I64 v,ln;

  CCPrepExSpace(pe);
  if (pe->i==pe->n)
    return CCPrepExBad(cc,pe,"a #if expression ended too early");
  if (pe->p[pe->i]=='(')
  {
    pe->i++;
    v=CCPrepExQ(cc,pe);
    CCPrepExSpace(pe);
    if ((pe->i==pe->n)||(pe->p[pe->i]!=')'))
      return CCPrepExBad(cc,pe,"missing ')' in a #if expression");
    pe->i++;
    return v;
  }
  if (CIsDigit(pe->p[pe->i]))
  {
    CCPrepExNumber(pe,&v);
    return v;
  }
  if (pe->p[pe->i]==39)
  {
    if (!CCPrepExCharLit(cc,pe,&v))
      return 0;
    return v;
  }
  if ((pe->p[pe->i]=='_')||(CIsAlpha(pe->p[pe->i])))
  {
    //C 6.10.1/3: what survived expansion is ZERO. Not a call, and that is why
    //`#if NOPE(1)` becomes a syntax error instead of a reference to a function
    ln=CCPrepWord(pe->p,pe->n,pe->i,w);
    pe->i+=ln;
    return 0;
  }
  if (pe->p[pe->i]=='"')
    return CCPrepExBad(cc,pe,"a string literal is not valid in a #if expression");
  return CCPrepExBad(cc,pe,"unexpected token in a #if expression");
}

I64 CCPrepExUnary(CCCtrl *cc,CPrepExpr *pe)
{
  I64 v;

  CCPrepExSpace(pe);
  if ((pe->i!=pe->n)&&(((pe->p[pe->i]=='-')||((pe->p[pe->i]=='+')||((pe->p[pe->i]=='~')||(pe->p[pe->i]=='!'))))))
  {
    v=pe->p[pe->i];
    pe->i++;
    if (v=='-')
      return -CCPrepExUnary(cc,pe);
    if (v=='+')
      return CCPrepExUnary(cc,pe);
    if (v=='~')
      return ~CCPrepExUnary(cc,pe);
    return !CCPrepExUnary(cc,pe);		//! is the last unary form
  }
  return CCPrepExPrim(cc,pe);
}

/* `live` only guards the division diagnostics: an operand reached through a
 * short-circuited branch is parsed, and C does not fault it. */
I64 CCPrepExMul(CCCtrl *cc,CPrepExpr *pe)
{
  I64 v,r,op;

  v=CCPrepExUnary(cc,pe);
  while (!pe->bad)
  {
    CCPrepExSpace(pe);
    if ((pe->i==pe->n)||((pe->p[pe->i]!='*')&&((pe->p[pe->i]!='/')&&(pe->p[pe->i]!='%'))))
      return v;
    op=pe->p[pe->i];
    pe->i++;
    r=CCPrepExUnary(cc,pe);
    if (pe->bad)
      return v;
    if (op=='*')
      v=v*r;
    else if (!r)
    {
      if (pe->live)
        return CCPrepExBad(cc,pe,"division by zero in a #if expression");
      v=0;		//a dead operand still must not fault
    }
    else if (op=='/')
      v=v/r;
    else
      v=v%r;
  }
  return v;
}

I64 CCPrepExAdd(CCCtrl *cc,CPrepExpr *pe)
{
  I64 v,r,op;

  v=CCPrepExMul(cc,pe);
  while (!pe->bad)
  {
    CCPrepExSpace(pe);
    if ((pe->i==pe->n)||((pe->p[pe->i]!='+')&&(pe->p[pe->i]!='-')))
      return v;
    op=pe->p[pe->i];
    pe->i++;
    r=CCPrepExMul(cc,pe);
    if (pe->bad)
      return v;
    if (op=='+')
      v=v+r;
    else
      v=v-r;
  }
  return v;
}

I64 CCPrepExShift(CCCtrl *cc,CPrepExpr *pe)
{
  I64 v,r,left;

  v=CCPrepExAdd(cc,pe);
  while (!pe->bad)
  {
    CCPrepExSpace(pe);
    if ((pe->i+1)>=pe->n)
      return v;
    if (((pe->p[pe->i]=='<')&&(pe->p[pe->i+1]=='<'))||((pe->p[pe->i]=='>')&&(pe->p[pe->i+1]=='>')))
    {
      left=(pe->p[pe->i]=='<');
      pe->i+=2;
      r=CCPrepExAdd(cc,pe);
      if (pe->bad)
        return v;
      if (left)
        v=v<<r;
      else
        v=v>>r;
    }
    else
      return v;
  }
  return v;
}

I64 CCPrepExRel(CCCtrl *cc,CPrepExpr *pe)
{
  I64 v,r,op;

  v=CCPrepExShift(cc,pe);
  while (!pe->bad)
  {
    CCPrepExSpace(pe);
    if (pe->i==pe->n)
      return v;
    //Only `<`, `>` and their `=` forms live at this level. `==` and `!=` are
    //the CALLER's (CCPrepExEq), so ExRel must leave them: taking `==` here and
    //then falling through to the `>` case is how `#if SEVEN == 7` asked whether
    //7 was greater than 7. A lone `=` is left too, and becomes a diagnostic one
    //level up for want of an operator.
    op=pe->p[pe->i];
    if ((op!='<')&&(op!='>'))
      return v;
    if (((pe->i+1)!=pe->n)&&(pe->p[pe->i+1]=='='))
    {
      pe->i+=2;
      if (op=='<')
        op=1;		//<=
      else
        op=2;		//>=
    }
    else
    {
      pe->i++;
      if (op=='<')
        op=4;		//<
      else
        op=5;		//>
    }
    r=CCPrepExShift(cc,pe);
    if (pe->bad)
      return v;
    if (op==1)
      v=(v<=r);
    else if (op==2)
      v=(v>=r);
    else if (op==4)
      v=(v<r);
    else
      v=(v>r);		//op==5
  }
  return v;
}

I64 CCPrepExEq(CCCtrl *cc,CPrepExpr *pe)
{
  I64 v,r,eq;

  v=CCPrepExRel(cc,pe);
  while (!pe->bad)
  {
    CCPrepExSpace(pe);
    if ((pe->i+1)>=pe->n)
      return v;
    if (((pe->p[pe->i]=='=')&&(pe->p[pe->i+1]=='='))||((pe->p[pe->i]=='!')&&(pe->p[pe->i+1]=='=')))
    {
      eq=(pe->p[pe->i]=='=');
      pe->i+=2;
      r=CCPrepExRel(cc,pe);
      if (pe->bad)
        return v;
      if ((v==r)!=eq)
        v=0;
      else
        v=1;
    }
    else
      return v;
  }
  return v;
}

I64 CCPrepExBAnd(CCCtrl *cc,CPrepExpr *pe)
{
  I64 v,r;

  v=CCPrepExEq(cc,pe);
  while (!pe->bad)
  {
    CCPrepExSpace(pe);
    //a single `&` only: `&&` belongs to the level above
    if (((pe->i+1)>=pe->n)||((pe->p[pe->i]!='&')||(pe->p[pe->i+1]=='&')))
      return v;
    pe->i++;
    r=CCPrepExEq(cc,pe);
    if (pe->bad)
      return v;
    v=v&r;
  }
  return v;
}

I64 CCPrepExBXor(CCCtrl *cc,CPrepExpr *pe)
{
  I64 v,r;

  v=CCPrepExBAnd(cc,pe);
  while (!pe->bad)
  {
    CCPrepExSpace(pe);
    if ((pe->i==pe->n)||(pe->p[pe->i]!='^'))
      return v;
    pe->i++;
    r=CCPrepExBAnd(cc,pe);
    if (pe->bad)
      return v;
    v=v^r;
  }
  return v;
}

I64 CCPrepExBOr(CCCtrl *cc,CPrepExpr *pe)
{
  I64 v,r;

  v=CCPrepExBXor(cc,pe);
  while (!pe->bad)
  {
    CCPrepExSpace(pe);
    //a single `|` only, for the same reason as `&` above
    if (((pe->i+1)>=pe->n)||((pe->p[pe->i]!='|')||(pe->p[pe->i+1]=='|')))
      return v;
    pe->i++;
    r=CCPrepExBXor(cc,pe);
    if (pe->bad)
      return v;
    v=v|r;
  }
  return v;
}

I64 CCPrepExAnd(CCCtrl *cc,CPrepExpr *pe)
{
  I64 v,r,sv;

  v=CCPrepExBOr(cc,pe);
  while (!pe->bad)
  {
    CCPrepExSpace(pe);
    if (((pe->i+1)>=pe->n)||((pe->p[pe->i]!='&')||(pe->p[pe->i+1]!='&')))
      return v;
    pe->i+=2;
    sv=pe->live;
    if (!v)
      pe->live=0;		//short-circuited: parsed, not evaluated
    r=CCPrepExBOr(cc,pe);
    pe->live=sv;
    if (pe->bad)
      return v;
    if (v&&r)
      v=1;
    else
      v=0;		//C says a logical operator yields exactly 0 or 1
  }
  return v;
}

I64 CCPrepExOr(CCCtrl *cc,CPrepExpr *pe)
{
  I64 v,r,sv;

  v=CCPrepExAnd(cc,pe);
  while (!pe->bad)
  {
    CCPrepExSpace(pe);
    if (((pe->i+1)>=pe->n)||((pe->p[pe->i]!='|')||(pe->p[pe->i+1]!='|')))
      return v;
    pe->i+=2;
    sv=pe->live;
    if (v)
      pe->live=0;		//short-circuited on the left
    r=CCPrepExAnd(cc,pe);
    pe->live=sv;
    if (pe->bad)
      return v;
    if (v||r)
      v=1;
    else
      v=0;
  }
  return v;
}

/* `?:` is the outermost level, and its arms are conditionals too. C evaluates
 * only the chosen arm, so the other is parsed with live=0 (probed against gcc:
 * the untaken side of a `?:`, `&&` or `||` does not get to report a division by
 * zero). Both arms are still PARSED here, because the ':' has to be found. */
I64 CCPrepExQ(CCCtrl *cc,CPrepExpr *pe)
{
  I64 v,t,f,sv;

  v=CCPrepExOr(cc,pe);
  CCPrepExSpace(pe);
  if (((pe->bad)||(pe->i==pe->n))||(pe->p[pe->i]!='?'))
    return v;
  pe->i++;
  sv=pe->live;
  pe->live=sv&&v;
  t=CCPrepExQ(cc,pe);
  pe->live=sv;
  CCPrepExSpace(pe);
  if (((pe->bad)||(pe->i==pe->n))||(pe->p[pe->i]!=':'))
    return CCPrepExBad(cc,pe,"missing ':' in a #if expression");
  pe->i++;
  pe->live=sv&&(!v);
  f=CCPrepExQ(cc,pe);
  pe->live=sv;
  if (v)
    return t;
  return f;
}

/* Expand a controlling expression and evaluate it. The expanded text goes into
 * the scratch and is dropped afterwards: nothing else can be looking at it,
 * because every invocation the expansion opened compacts its own region back
 * below this one's start, and the scratch never moves. */
I64 CCPrepExprLine(CCCtrl *cc,CPrep *pp,U8 *q,U8 *e)
{
  CPrepExpr pe;
  CMacScan bs;
  I64 off,v;

  off=pp->scpos;
  bs.p=q;
  bs.n=e-q;
  bs.i=0;
  bs.mask=0;
  bs.dst=1;
  bs.ex=NULL;
  bs.isarg=0;
  bs.incond=1;
  bs.subonly=0;
  CCPrepScan(cc,pp,&bs);
  if (cc->errs)
  {
    pp->scpos=off;
    return FALSE;
  }
  pe.p=pp->sc+off;
  pe.n=pp->scpos-off;
  pe.i=0;
  pe.bad=0;
  pe.live=1;
  v=CCPrepExQ(cc,&pe);
  CCPrepExSpace(&pe);
  if ((pe.i!=pe.n)&&(!pe.bad))
    CCPrepExBad(cc,&pe,"trailing tokens in a #if expression");
  pp->scpos=off;		//done with the condition text
  if ((pe.bad)||(cc->errs))
    return FALSE;
  return v!=0;
}

/* M31: validate a macro's replacement text at DEFINITION time, which is when C
 * diagnoses these three things. For a FUNCTION-like body: a `#` operand must
 * name one of THIS macro's parameters (or `__VA_ARGS__` when the list ended
 * with `...`), and `##` may not sit at either end of the body. For either kind:
 * `__VA_ARGS__` is only meaningful inside a variadic macro's own expansion, so
 * an object-like macro whose body is `__VA_ARGS__` is rejected even when it is
 * invoked from inside a variadic one (probed: gcc errors there, too). gcc makes
 * all of these definition-time errors, so checking them here is what makes an
 * unused malformed macro as loud as a used one — the alternative is a diagnostic
 * that only appears if the macro happens to be expanded. Literals and comments
 * are stepped over whole, because a `#` inside a string literal is part of the
 * literal and not an operator (probed: `#define S(x) ["#x"]` is clean).
 * Returns FALSE after reporting. */
I64 CCPrepCheckBody(CCCtrl *cc,I64 mi)
{
  U8 *p;
  U8 tok[CC_TOK_MAX];
  I64 n,i,j,ln,fnl,tokseen;

  p=cc->mbody[mi];
  n=StrLen(p);
  fnl=(cc->mnarg[mi]>=0);
  i=0;
  tokseen=0;
  while (i!=n)
  {
    if ((p[i]=='_')||(CIsAlpha(p[i])))
    {
      ln=CCPrepWord(p,n,i,tok);
      if ((!cc->mvar[mi])&&(!StrCmp(tok,"__VA_ARGS__")))
      {
        CCPrepErr(cc,"__VA_ARGS__ is only meaningful inside a variadic macro body");
        return FALSE;
      }
      tokseen=1;
      i+=ln;
    }
    else if ((p[i]=='\"')||(p[i]==39))
    {
      tokseen=1;
      i+=CCPrepLitLen(p,n,i);
    }
    else if ((p[i]=='/')&&((i+1)!=n)&&((p[i+1]=='*')||(p[i+1]=='/')))
      i+=CCPrepComLen(p,n,i);
    else if ((p[i]==' ')||((p[i]=='\t')||((p[i]=='\n')||(p[i]==13))))
      i++;
    else if ((fnl)&&(p[i]=='#'))
    {
      if (((i+1)!=n)&&(p[i+1]=='#'))
      {
        //`##` needs an operand on BOTH sides.
        if (!tokseen)
        {
          CCPrepErr(cc,"## at the start of a macro body");
          return FALSE;
        }
        j=i+2;
        while ((j!=n)&&((p[j]==' ')||((p[j]=='\t')||((p[j]=='\n')||(p[j]==13)))))
          j++;
        if (j==n)
        {
          CCPrepErr(cc,"## at the end of a macro body");
          return FALSE;
        }
        i=j;
      }
      else
      {
        j=i+1;
        while ((j!=n)&&((p[j]==' ')||((p[j]=='\t')||((p[j]=='\n')||(p[j]==13)))))
          j++;
        ln=CCPrepWord(p,n,j,tok);
        if ((!ln)||((CCPrepParam(cc,mi,tok)<0)&&((!cc->mvar[mi])||(StrCmp(tok,"__VA_ARGS__")))))
        {
          CCPrepErr(cc,"#' in a macro body must be followed by a parameter");
          return FALSE;
        }
        tokseen=1;
        i=j+ln;
      }
    }
    else
    {
      //ordinary punctuation: it can be a paste operand, so a `##` after it is
      //not an empty left side
      tokseen=1;
      i++;
    }
  }
  return TRUE;
}

/* M31: parse a function-like macro's parameter list. q points at the '('; names
 * are written into prm newline-separated, which is the storage CCPrepParam reads
 * back. The `...` of a variadic list is deliberately NOT recorded as a name:
 * nothing in the parameter table may match `__VA_ARGS__`, because that names the
 * EXTRA arguments rather than a parameter. Returns FALSE after reporting. */
I64 CCPrepParams(CCCtrl *cc,I64 mi,U8 *q,U8 *e,U8 *prm,I64 *pcount,I64 *pva,U8 **pq)
{
  U8 nm[CC_TOK_MAX];
  I64 k,i,cnt,va,expect;

  q++;				//the '('
  k=0;				//write offset into prm
  cnt=0;
  va=0;
  expect=0;			//a comma was consumed, so a name is owed
  while (q<e)
  {
    while ((q<e)&&((*q==' ')||(q=='\t')))
      q++;
    if (q>=e)
    {
      CCPrepErr(cc,"unterminated parameter list in #define");
      return FALSE;
    }
    if (*q==')')
    {
      if (expect)
      {
        //`#define F(a,)`: a comma before the close promises a name that never
        //arrives. Accepting it silently changes the arity, which is worse than
        //the diagnostic (gcc: "expected parameter name, found \")").
        CCPrepErr(cc,"expected a parameter name in #define");
        return FALSE;
      }
      q++;
      break;
    }
    if (cnt>=CC_MAX_PARAMS)
    {
      CCPrepErr(cc,"too many parameters in #define");
      return FALSE;
    }
    if (((q+2)<e)&&((q[0]=='.')&&((q[1]=='.')&&(q[2]=='.'))))
    {
      //`...` must be last: only a ')' may follow it
      va=1;
      q+=3;
      while ((q<e)&&((*q==' ')||(q=='\t')))
        q++;
      if ((q>=e)||(q[0]!=')'))
      {
        CCPrepErr(cc,"only a ')' may follow ... in a macro parameter list");
        return FALSE;
      }
      q++;
      break;
    }
    if (!(((*q=='_')||(CIsAlpha(*q)))))
    {
      CCPrepErr(cc,"bad parameter name in #define");
      return FALSE;
    }
    i=0;
    while ((q<e)&&(((((*q=='_')||(CIsAlpha(*q)))||(CIsDigit(*q))))&&(i<(CC_TOK_MAX-1))))
    {
      nm[i]=*q;
      i++;
      q++;
    }
    nm[i]=0;
    if ((k+i+1)>=(CC_MACRO_BODY-1))
    {
      CCPrepErr(cc,"parameter names too long in #define");
      return FALSE;
    }
    if (CCPrepParam(cc,mi,nm)>=0)
    {
      //prm already holds the names accepted so far, newline-separated and
      //NUL-terminated at k, so this is the same lookup CCPrepParam performs at
      //expansion time — and it has to agree, because a repeated name would make
      //the second one unreachable (the first ordinal always wins). C diagnoses
      //it too (gcc: "duplicate macro parameter").
      CCPrepErr(cc,"duplicate parameter name in #define");
      return FALSE;
    }
    MemCpy(prm+k,nm,i);
    prm[k+i]='\n';
    k+=i+1;
    prm[k]=0;
    cnt++;
    while ((q<e)&&((*q==' ')||(q=='\t')))
      q++;
    if ((q<e)&&(q[0]==','))
    {
      q++;			//the loop re-skips whitespace and reads the next name
      expect=1;
    }
    else if ((q<e)&&(q[0]==')'))
    {
      q++;
      break;
    }
    else
    {
      CCPrepErr(cc,"expected ',' or ')' in a macro parameter list");
      return FALSE;
    }
  }
  prm[k]=0;
  *pcount=cnt;
  *pva=va;
  *pq=q;
  return TRUE;
}

/* Record one macro: NAME [body-to-EOL], or NAME(a,b) [body-to-EOL] when a '('
 * follows the name with NO space. That gap is the whole difference between the
 * two forms and it is not cosmetic: `#define F (x)` is an object-like macro
 * whose body happens to start with a paren, exactly as C says. Redefinition
 * silently replaces (C would warn; we keep it simple). */
I64 CCPrepDefine(CCCtrl *cc,U8 *q,U8 *e)
{
  U8 nm[CC_TOK_MAX];
  I64 i=0,b;
  while ((q<e)&&((*q==' ')||(q=='\t')))		//skip the gap after 'define'
    q++;
  if ((q<e)&&(CIsDigit(*q)))
  {
    //C: "macro names must be identifiers". A leading digit is the shape an
    //alnum-only read would otherwise accept in silence (`#define 1x 3`).
    CCPrepErr(cc,"macro name must be an identifier");
    return FALSE;
  }
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
  if (!StrCmp(nm,"defined"))
  {
    //C reserves the name (probed: gcc errors at the #define, before any #if
    //looks at it). Rejecting it here is what keeps `#if defined == 1` from
    //turning into an expression about whatever `defined` happened to expand to.
    CCPrepErr(cc,"\"defined\" cannot be used as a macro name");
    return FALSE;
  }
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
  //A redefinition replaces the previous form wholesale, so a macro that was
  //object-like and becomes function-like must clear its parameter record too.
  cc->mnarg[b]=-1;
  cc->mvar[b]=0;
  cc->mprm[b][0]=0;
  if ((q<e)&&(q[0]=='('))
  {
    if (!CCPrepParams(cc,b,q,e,cc->mprm[b],&i,&cc->mvar[b],&q))
      return FALSE;
    cc->mnarg[b]=i;
  }
  while ((q<e)&&((*q==' ')||(q=='\t')))
    q++;
  i=0;
  while ((q<e)&&(i<(CC_MACRO_BODY-1)))
  {
    cc->mbody[b][i++]=*q;
    q++;
  }
  cc->mbody[b][i]=0;
  if (!CCPrepCheckBody(cc,b))
    return FALSE;
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
  U8 kw[CC_TOK_MAX],nm[CC_TOK_MAX];
  /* A name read from the directive is at most CC_TOK_MAX-16 characters, but the
   * path that gets OPENED can be longer than that: it is the includer's
   * directory (up to CC_TOK_MAX-1 characters, since that is what CCPrepDir can
   * store) joined with the name.  Both the join buffer and `path` are sized for
   * the pair, because the winner of the search is copied back into `path` and a
   * 128-byte destination for a 240-byte source would have written off the end of
   * this function's stack frame (M33). */
  U8 path[2*CC_TOK_MAX],full[2*CC_TOK_MAX];
  U8 msg[CC_TOK_MAX];
  U8 *inc,*start;
  I64 accept,par,f,mi,cond,i,close,hit;

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
        //M31: every parallel field shifts together, or a macro keeps another
        //one's parameter record and substitutes the wrong argument.
        StrCpy(cc->mprm[mi],cc->mprm[mi+1]);
        cc->mnarg[mi]=cc->mnarg[mi+1];
        cc->mvar[mi]=cc->mvar[mi+1];
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
    //M33: an explicit path (a leading '/', or a drive letter at [1]) opens as
    //written.  Otherwise the QUOTED form looks in the directory of the file
    //that contains the directive first and only then in the header home, while
    //the angle-bracket form goes straight to the header home — the split gcc
    //makes between `"` and `<`.  FileFind is the probe because, unlike
    //FileRead, it says nothing when the answer is no: without it, every quoted
    //include of a header that lives in the header home would print a kernel
    //"File not found" line for the attempt that legitimately failed first.
    hit=0;
    if (!((path[0]=='/')||(path[1]==':')))
    {
      if (((close=='"')&&(pp->dir[pp->depth][0])))
      {
        StrCpy(full,pp->dir[pp->depth]);
        i=StrLen(full);
        StrCpy(full+i,path);
        if (FileFind(full))
        {
          StrCpy(path,full);
          hit=1;
        }
      }
      if (!hit)
      {
        //Plain names come from the header home.
        StrCpy(full,"T:/CC/INC/");
        i=StrLen(full);
        StrCpy(full+i,path);
        StrCpy(path,full);
      }
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
    //The file just opened is the includer for anything IT includes (M33).
    CCPrepDir(pp,pp->depth+1,path);
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
    if (par)
    {
      //M32: a real expression, not just a literal 0 or 1. Reported here and
      //not by the evaluator because C says so ("#if with no expression"), and
      //because an empty tail would otherwise look like a truncated expression.
      if (!CCPrepHasText(q,e-q))
      {
        CCPrepErr(cc,"#if with no expression");
        return FALSE;
      }
      cond=CCPrepExprLine(cc,pp,q,e);
      if (cc->errs)
        return FALSE;
    }
    CCPrepCond(pp,par,cond);
    return TRUE;
  }
  if (!StrCmp(kw,"elif"))
  {
    if (!pp->sp)
    {
      CCPrepErr(cc,"#elif without #if/#ifdef/#ifndef");
      return FALSE;
    }
    f=pp->frm[pp->sp-1];
    par=(f&4);
    if ((par)&&((f&8)))
    {
      //C: an #elif may not follow an #else. Probed: reported even when the
      //whole chain is untaken, so this is gated on the ENCLOSING region, not
      //on whether anybody will read this branch.
      CCPrepErr(cc,"#elif after #else");
      return FALSE;
    }
    cond=FALSE;
    if ((par)&&(!(f&2)))
    {
      //A chain that already took a branch does not evaluate the later
      //conditions at all (probed: `#if 1` / `#elif 1/0` is not a division
      //error, and `#elif ))` is not a syntax error) — same reason a skipped
      //region tolerates junk.
      if (!CCPrepHasText(q,e-q))
      {
        CCPrepErr(cc,"#elif with no expression");
        return FALSE;
      }
      cond=CCPrepExprLine(cc,pp,q,e);
      if (cc->errs)
        return FALSE;
    }
    pp->frm[pp->sp-1]=cond|((f&2)|(cond<<1))|(f&4)|(f&8);
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
    pp->frm[pp->sp-1]=cond|(f&2)|(cond<<1)|(f&4)|8;	//bit3: an #else has run
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
  if ((!StrCmp(kw,"error"))||(accept&&(!StrCmp(kw,"warning"))))
  {
    //M32: the message is the REST OF THE LINE, as written. C does not macro-
    //expand it (probed: `#error MSG` with `#define MSG boom` prints MSG), and
    //a #error is a hard failure while #warning is only a note — so #warning is
    //recognised solely to report it: silently accepting it would swallow
    //something the author wrote to be seen.
    while ((q<e)&&((*q==' ')||(q=='\t')))
      q++;
    i=0;
    while ((q<e)&&(i<(CC_TOK_MAX-1)))
      msg[i++]=*q++;
    msg[i]=0;
    if (!accept)
      return TRUE;
    if (!StrCmp(kw,"error"))
    {
      "CC ERROR: prep: #error %s\n",msg;
      cc->errs++;
      return FALSE;
    }
    "WARN: prep: #warning %s\n",msg;
    return TRUE;
  }
  if (accept)
  {
    "CC ERROR: prep: unknown directive '#%s'\n",kw;
    cc->errs++;
    return FALSE;
  }
  return TRUE;
}

/* M33: the directory a file lives in, with its trailing '/', written into
 * pp->dir[k].  "" when the name has no directory part: that is the case where
 * only the header home can be searched, and it is also what a driver that hands
 * prep bare text gets, so the feature costs nothing where it cannot apply. */
U0 CCPrepDir(CPrep *pp,I64 k,U8 *fn)
{
  U8 *d;
  I64 i,last;

  d=pp->dir[k];
  i=0;
  last=-1;
  while ((fn[i])&&(i<(CC_TOK_MAX-1)))
  {
    if (fn[i]=='/')
      last=i;
    i++;
  }
  if (last<0)
  {
    d[0]=0;
    return;
  }
  i=0;
  while (i<=last)
  {
    d[i]=fn[i];
    i++;
  }
  d[i]=0;
}

/* M33: is the physical line [p,p+n) continued onto the next one?  Returns the
 * offset of the splicing backslash, or -1.  C splices a backslash immediately
 * followed by a new-line; a backslash with spaces or tabs between it and the
 * new-line is not a splice in C99, but gcc splices it anyway (with a warning)
 * and deletes only the backslash, keeping the whitespace — and that is the
 * shape real code uses, so lite follows gcc's reading and stays silent.  The
 * backslash must have a new-line after it: a file that ENDS with a backslash
 * and no new-line keeps it as an ordinary byte (probed against gcc). */
I64 CCPrepCont(U8 *p,I64 n)
{
  I64 i;

  i=n;
  while ((i!=0)&&(((p[i-1]==' ')||(p[i-1]=='\t'))))
    i--;
  if (((i==0)||(p[i-1]!='\\')))
    return -1;			//i==0: the line was nothing but whitespace,
				//and `i` did NOT move only when the line ended
				//in whitespace-or-nothing, so it is not itself a
				//sign of anything — a line that ends directly
				//in the backslash leaves `i` at `n`, and that is
				//the common case (my first version treated it as
				//the odd one and never spliced anything).
  return i-1;
}

/* M33: assemble the logical line that starts at `in` — one physical line, plus
 * every physical line a trailing backslash splices onto it — and return its
 * text: `in` unchanged when nothing was spliced, otherwise a copy in the line
 * buffer (*plnb, allocated on first use and never freed, like the rest of the
 * unit's small buffers).  *plen is the LENGTH OF THAT TEXT and must be used with
 * the pointer it is returned with — the two only live in the same buffer when
 * something was spliced, and a pointer into the original text paired with a
 * pointer into the copy would let a directive scan read across two unrelated
 * allocations.  *pterm receives the terminator byte ('\n' or NUL) of the LAST
 * physical line consumed, which is where the caller resumes, and *plines the
 * number of physical lines it took, because the caller emits one output newline
 * per physical line so the line structure the C parser sees still matches the
 * source.  Joining
 * happens here, before anything recognises a directive, a comment or a literal,
 * because that is the order the translation phases have: it is why `// foo \`
 * swallows the following line, why a `#define` body can continue, and why
 * `a1\` + newline + `2[3];` declares `a12`. */
U8 *CCPrepLogical(U8 *in,U8 **pterm,I64 *plen,I64 *plines,U8 **plnb,I64 *pcap)
{
  U8 *q,*e,*out;
  I64 lines,off,bs;

  e=in;
  while ((*e)&&(*e!='\n'))
    e++;
  if (((*e!='\n')||(CCPrepCont(in,e-in)<0)))
  {
    *pterm=e;
    *plen=e-in;
    *plines=1;
    return in;				//the ordinary case: no copy at all
  }
  if (!(*plnb))
  {
    //The joined text is always SHORTER than what is still ahead of us, so one
    //allocation for the rest of this file is enough and never overruns.
    *pcap=StrLen(in)+16;
    *plnb=MAlloc(*pcap);
  }
  out=*plnb;
  off=0;
  lines=0;
  q=in;
  while (TRUE)
  {
    e=q;
    while ((*e)&&(*e!='\n'))
      e++;
    lines++;
    bs=0;
    if (*e=='\n')
      bs=CCPrepCont(q,e-q);		//-1 when this line ends the logical line
    if (bs<0)
    {
      MemCpy(out+off,q,e-q);		//the whole last physical line
      off+=e-q;
      break;
    }
    MemCpy(out+off,q,bs);		//everything except the backslash
    off+=bs;
    if (!(*(e+1)))
      break;				//a trailing backslash with no line after
    q=e+1;
  }
  out[off]=0;
  *pterm=e;
  *plen=off;
  *plines=lines;
  return out;
}

I64 CCPrepText(CCCtrl *cc,CPrep *pp,U8 *in)
{
  CMacScan ms;
  U8 *q,*e,*txt,*lnb,*term;
  I64 accept,sp0,lines,len,k,lncap;

  //Every file must close the conditionals it opened, whether it is the main
  //translation unit or an #include: the frame stack is shared, so an open #if
  //left behind by an included file would silently swallow the rest of the
  //INCLUDER, not just the header.
  sp0=pp->sp;
  lnb=NULL;				//the joined-line buffer, allocated by
  lncap=0;				//CCPrepLogical on the first splice
  while (*in)
  {
    //M33: one logical line per iteration.  Ordinarily that IS the physical
    //line and `txt` is `in` unchanged; only a line that ends with a backslash
    //costs a copy.
    txt=CCPrepLogical(in,&term,&len,&lines,&lnb,&lncap);
    e=txt+len;				//the end of THIS text, in THIS buffer
    q=txt;
    while ((*q==' ')||(q=='\t'))
      q++;
    accept=CCPrepAccept(pp);
    if (*q=='#')
    {
      if (!CCPrepDirective(cc,pp,q,e))
        return FALSE;
    }
    else if (accept)
    {
      //Scan from `txt`, not `q`: q exists only to find the '#' —
      //scanning from it would silently strip each line's indentation.
      //A source line is scanned with nothing disabled (ms.mask 0), no
      //invocation in progress (ms.ex NULL) and the output as destination.
      ms.p=txt;
      ms.n=e-txt;
      ms.i=0;
      ms.mask=0;
      ms.dst=0;
      ms.ex=NULL;
      ms.isarg=0;			//a source line is neither
      ms.incond=0;			//and only a directive's text is #if text
      ms.subonly=0;
      CCPrepScan(cc,pp,&ms);
    }
    //Preserve the line, even for dropped text: the parser sees the
    //same line structure the source had.  A logical line that ate N physical
    //lines emits N newlines, so splicing does not shift the lines after it
    //(M33) — the joined text lands where the first line was and the rest of
    //the lines it consumed become blank ones, which is what the C parser
    //makes of them anyway.
    k=0;
    while (k!=lines)
    {
      CCPrepPut(cc,pp,"\n",1,0);
      k++;
    }
    if (*term)
      in=term+1;
    else
      in=term;
  }
  if (pp->sp!=sp0)
  {
    CCPrepErr(cc,"unterminated #if at end of file");
    return FALSE;
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
  //M33: the directory of the file being compiled, so a quoted include in the
  //main file finds a header lying beside it.  `cc->fn` is set by whoever read
  //the text; when nobody did, this is "" and only the header home is searched,
  //which is exactly the old behavior.
  CCPrepDir(&pp,0,cc->fn);
  //M31: the argument scratch starts the same size as the output. It holds, at
  //any moment, only the expanded arguments of the invocations currently open,
  //so it stays small in ordinary code even when a header defines dozens of
  //macros. The macro table itself is source-level and resets here, exactly like
  //the typedef and enum tables: a saved object carries none of it.
  pp.sc=MAlloc(CC_PREP_SCRA);
  pp.sccap=CC_PREP_SCRA;
  pp.scpos=0;
  cc->mn=0;
  if (!CCPrepText(cc,&pp,text))
    return NULL;
  pp.buf[pp.pos]=0;
  return pp.buf;
}
