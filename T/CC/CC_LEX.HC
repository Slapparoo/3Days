/* HolyC gotcha: `&&`/`||` do not follow C precedence — always wrap
 * every operand of && / || in parentheses. */

U0 CCError(CCCtrl *cc,U8 *msg)
{
  "CC ERROR: %s [tk=%d kw=%d num=%d str='%s']\n",
    msg,cc->tk,cc->tk_kw,cc->tk_num,cc->tk_str;
  cc->errs++;
}

U0 CCSkipSpace(CCCtrl *cc)
{
  U8 *p=cc->pc;
  //No `for (;;)` and no `continue` in this HolyC build.
  while (TRUE)
  {
    while (((*p==' ')||(*p=='\t'))||((*p=='\n')||(*p=='\r')))
      p++;
    if ((p[0]=='/')&&(p[1]=='/'))
    {
      while ((*p)&&(p[0]!='\n'))
        p++;
    }
    else if ((p[0]=='/')&&(p[1]=='*'))
    {
      p+=2;
      while ((*p)&&(!((p[0]=='*')&&(p[1]=='/'))))
        p++;
      if (*p)
        p+=2;
    }
    else
      break;
  }
  cc->pc=p;
}

Bool CIsAlpha(I64 ch)
{
  return ((ch>='a')&&(ch<='z'))||((ch>='A')&&(ch<='Z'))||(ch=='_');
}

Bool CIsDigit(I64 ch)
{
  return (ch>='0')&&(ch<='9');
}

/* M32: hex digits as one predicate, rather than the three-range chain the
 * escape table and the #if character-constant reader each repeated. */
Bool CIsHexCh(I64 ch)
{
  return ((ch>='0')&&((ch<='9')||((ch>='a')&&(ch<='f'))))||((ch>='A')&&(ch<='F'));
}

/* M32: the escape TABLE, in one place. `ch` is the byte that follows the
 * backslash and `p` points just after `ch` so \x and \ooo can read their
 * digits; *pv is the byte the escape denotes and *pp the position after it.
 * Returns FALSE for an escape this table does not know, and then the caller
 * keeps the byte as it always has: the QUOTE escapes are deliberately NOT here
 * because they differ by context (a string body escapes `", a char body
 * escapes '), and each site has its own message for an unknown escape. Before
 * this existed the table was written out twice in this file and the two copies
 * had already drifted (one handles \' and the other "); the #if evaluator in
 * CC_PREP.HC is the third user and it has to agree with the literals below it. */
Bool CCEscape(CCCtrl *cc,U8 *p,I64 ch,I64 *pv,U8 **pp)
{
  I64 ei,d2;

  if (ch=='n')
    ch='\n';
  else if (ch=='t')
    ch='\t';
  else if (ch=='r')
    ch='\r';
  else if (ch=='a')
    ch=7;
  else if (ch=='b')
    ch=8;
  else if (ch=='f')
    ch=12;
  else if (ch=='v')
    ch=11;
  else if (ch=='e')
    ch=27;
  else if (ch=='\\')
    ch='\\';
  else if (ch=='x')
  {
    //\xHH: one or two hex digits (C takes more; two is the byte-complete
    //span and what every fixture exercises). With no digit, as before: the
    //diagnostic fires and the byte becomes 0, which ends the literal early
    //exactly the way a NUL would.
    ei=0;
    ch=0;
    while (ei!=2)
    {
      d2=*p;
      if (CIsDigit(d2))
        d2=d2-'0';
      else if ((d2>='a')&&(d2<='f'))
        d2=d2-'a'+10;
      else if ((d2>='A')&&(d2<='F'))
        d2=d2-'A'+10;
      else
        break;
      ch=ch*16+d2;
      p++;
      ei++;
    }
    if (!ei)
      CCError(cc,"\\x needs a hex digit");
  }
  else if ((ch>='0')&&(ch<='7'))
  {
    //\nnn octal, up to three digits total (the first is already spent)
    ch=ch-'0';
    ei=1;
    while (ei!=3)
    {
      d2=*p;
      if ((d2<'0')||(d2>'7'))
        break;
      ch=ch*8+(d2-'0');
      p++;
      ei++;
    }
  }
  else
  {
    *pv=ch;
    *pp=p;
    return FALSE;
  }
  *pv=ch;
  *pp=p;
  return TRUE;
}

/* M35: is there an EXPONENT at p?  Shared by the caller's "might be a float"
 * peek and by the scanner below, so the two cannot drift apart (PLAN's mirror
 * hazard).  C wants digits after e/E, so `1e` is NOT an exponent: it stays the
 * integer 1 followed by the identifier e. */
I64 CCExpAt(U8 *p)
{
  if ((*p!='e')&&(*p!='E'))
    return FALSE;
  p++;
  if ((*p=='+')||(*p=='-'))
    p++;
  return CIsDigit(*p);
}

/* The kernel's decimal conversion sums the digits it is handed into a SIGNED
 * 64-BIT integer and never checks it.  Measured (m35end.py):
 * `12345678901234567890` came back -6.1e18 and `0.12345678901234567890` came back
 * -0.061, each with the end pointer reporting that the whole text was read.
 * 1.2e19 walked through the sign bit, and the conversion never noticed.  This is
 * the test the host omits: TRUE when the `n` digits in sd[] (most significant
 * first, then `zeros` more of them) cannot be summed that way without passing
 * 0x7FFFFFFFFFFFFFFF.  922337203685477580 is that limit divided by ten, with 7
 * left over.  Called with zeros==0 it asks about the digits as they stand; with
 * the zeros an exponent would add, it asks about the plain spelling of the same
 * value. */
Bool CCDigOvf(I64 *sd,I64 n,I64 zeros)
{
  I64 i,acc,d;

  acc=0;
  for (i=0;i!=n;i++)
  {
    d=sd[i];
    if ((acc>922337203685477580)||((acc==922337203685477580)&&(d>7)))
      return TRUE;
    acc=acc*10+d;
  }
  for (i=0;i!=zeros;i++)
  {
    if (acc>922337203685477580)
      return TRUE;
    acc=acc*10;
  }
  return FALSE;
}

/* M35: a C floating literal.  lite implements NO float parser: no decimal->binary
 * algorithm, no rounding decision, no binary exponent.  The text is handed to the
 * kernel's own conversion (Str2F64, KERNELA.HH) and the token value is the 64-bit
 * pattern that F64 arrives in — which is what a TY_DOUBLE slot holds, so nothing
 * downstream converts it either.  What this function does instead is DECIMAL
 * bookkeeping: digit placement, the overflow test above, and a check of the
 * answer.  Three measurements decided its shape (m35end.py, m35cliff.py,
 * m35shift.py; tables in PLAN.md M35):
 *
 * 1. THE HOST SUMS DIGITS IN AN UNCHECKED I64, AND SAYS NOTHING WHEN IT WRAPS.
 *    Only significant digits count (leading zeros reach the accumulator on
 *    neither side: `000000000000000000001` is 1 in both), and 20 of them are
 *    always past the limit.  A literal whose written digits overflow is not
 *    handed over at all: digits past what fits are dropped and the exponent
 *    absorbs them, which was measured (m35trunc.py, 409 long literals) never to
 *    change which double the literal rounds to.  What it replaces is exact and
 *    worth naming: the host's answer for an overflowing digit string is the digit
 *    sum reduced modulo 2^64 and read as a signed integer, so
 *    `12345678901234567890` comes back -6.1010651724749834e18 where C says
 *    1.2345678901234567e19 — low by exactly 2^64, sign included.
 *
 * 2. AN INTEGER-VALUED DECIMAL IS EXACT, SO A POSITIVE EXPONENT IS FOLDED IN.
 *    A positive exponent is applied by multiplication and drifts: `1e2` comes
 *    back 99.99999999999997 and `1e10` 9999999999.9999561, where C99 6.4.4/6
 *    requires exactly 100 and exactly 10000000000 — `1e2 == 100` is false here
 *    and true in C, which is a wrong answer about an ordinary literal.  The same
 *    rational written without an exponent goes down the sum-then-convert path,
 *    exact twice over: the sum holds the integer (rule 1 is what makes that
 *    certain) and an I64->F64 conversion is correctly rounded (CC_LIB.HC), which
 *    is the value C asks for.  Measured: every shifted form whose digits fit came
 *    back EXACT — `1e1`->`10`, `1e10`->`10000000000`, `1.5e3`->`1500`,
 *    `100e-2`->`1` — while the exponent spellings did not.  A NEGATIVE exponent
 *    gains nothing, and that is measured rather than assumed: m35neg.py spells 19
 *    such values both ways and EVERY pair returned the identical quad (`1e-5` and
 *    `0.00001` are both 12 ULP high, `1e-7` and `0.0000001` both 4), so there is
 *    no better spelling to choose and no shift is attempted.  A plain spelling
 *    that would not fit (1e19, 1e100, 1e308) cannot be built either, so those
 *    keep the spelling the program wrote and come with the drift recorded below.
 *
 * 3. STR2F64 REPORTS WHERE IT STOPPED, AND THE REPORT IS CHECKED.
 *    On a spelling it does not know it stops early and answers the part it read:
 *    `1e+2` stops after `1e` and answers 1.0 where C says 100 — a wrong VALUE
 *    with no diagnostic.  A delegation that cannot tell whether the host
 *    understood its input is a guess, so the stop point is compared with the text
 *    handed over.  The one spelling difference that carries no information — an
 *    exponent's `+`, since C's exponent is `e [sign] digits` and a `+` exponent
 *    IS the unmarked one — is dropped when the text is built, so the ordinary
 *    case is repaired rather than merely caught.
 *
 * What is left over is the host's own rounding, recorded rather than chased:
 * chasing it means a correctly-rounded decimal->binary converter, the second
 * implementation the delegation rule in PLAN.md exists to avoid.  Measured against
 * gcc in ULPs of the value C requires (the spacing of doubles at that value, so
 * the errors come out whole): `0.123456789012345` +11, `0.1234567890123456789`
 * +28, `3.141592653589793` +9, `1e-5` +12, `1e-7` +4, `0.15` +1, `1.5e18` -14,
 * `1e19` -16, `1e100` -117, `1.7976931348623157e308` -225 — while `1.5`, `2.5`,
 * `.5`, `2.`, `0.1`,
 * `100`, `1000.0`, `123456.789` and `123456789012345678` come back EXACT.  The
 * host's OWN literals drift the same way (its `0.3` is one ULP high, which is why
 * `0.1+0.2==0.3` answers 1 here and 0 in C), so a program comparing a decimal
 * LITERAL for exact equality with a computed value can differ from gcc.  Overflow
 * and underflow agree with C: `1e400` is infinity and `1e-400` is zero (m35huge),
 * and an exponent clamped here is already past both.  `0x1p3`-style hex floats are
 * not supported: the caller keeps hex digits as an integer because Str2F64 answers
 * 0 for them, and the parse then reports the syntax error a compiler without hex
 * floats reports.  The `f`/`F`/`l`/`L` suffixes are consumed and ignored: `float`
 * and `long double` are TY_DOUBLE.
 * p points at the first digit, or at the '.' of a `.5` spelling; the caller has
 * already established that a digit or this dot starts a number token. */
Bool CCNumFloat(CCCtrl *cc,U8 *p)
{
  U8 *s;
  U8 *tc;
  U8 *me;			//end of the mantissa+exponent, before the suffix
  U8 *end;
  U8 tx[8];
  F64 f;
  I64 sd[CC_DIG_MAX];
  I64 nd,intdig,lz,sig,keep,started,sawdot,dotrel,ev,expsgn,expcnt,plain,wrote,n;
  I64 e2,e1,i,d,tl;

  s=p;
  nd=0;
  intdig=0;
  lz=0;
  sig=0;
  keep=0;
  started=0;
  sawdot=0;
  ev=0;
  expsgn=1;
  expcnt=0;
  while (1)
  {
    if (CIsDigit(*p))
    {
      d=*p-'0';
      nd++;
      if (!sawdot)
        intdig++;
      if (!started)
      {
        if (d)
          started=1;
        else
          lz++;			//a leading zero reaches neither accumulator
      }
      if (started)
      {
        if (sig<CC_SIG_MAX)
          sig++;		//counted past the cap: it decides the spelling
        if (keep<CC_DIG_MAX)
        {
          sd[keep]=d;
          keep++;
        }
      }
      p++;
    }
    else if ((*p=='.')&&(!sawdot))
    {
      sawdot=1;
      p++;
    }
    else
    {
      break;
    }
  }
  if (CCExpAt(p))
  {
    p++;
    expsgn=1;
    if (*p=='+')
      p++;
    else if (*p=='-')
    {
      expsgn=-1;
      p++;
    }
    while (CIsDigit(*p))
    {
      if (expcnt<6)
      {
        ev=ev*10+(*p-'0');	//clamped: past a million the value is already
        expcnt++;		//zero or infinity, which is C's answer too
      }
      p++;
    }
    if (expsgn<0)
      ev=-ev;
  }
  else if (!sawdot)
    return FALSE;			//plain integer: `12`, or `1e` with no
  					//digits after the e (CCExpAt requires a
  					//digit after the optional sign)
  if (!nd)
    return FALSE;			//at least one digit: `.5` and `5.` are
   					//C-legal, a bare `.` is not a number
  me=p;
  if ((*p=='f')||(*p=='F')||(*p=='l')||(*p=='L'))
    p++;				//folded: float == long double == double,
   					//and consumed HERE: the suffix is C's,
   					//the host's parser knows no such thing,
   					//and handing it over makes Str2F64 stop
   					//after the mantissa (it read `1.5` out of
   					//`1.5f` and reported exactly that)
  if (!keep)
  {
    //Every digit was a zero: the value is zero whatever the exponent says, and
    //"0" is the one spelling that says so with nothing left to place.
    cc->tk_str[0]='0';
    cc->tk_str[1]=0;
    n=1;
  }
  else
  {
    //Could the host sum the digits exactly as this program wrote them?  Twenty
    //significant digits are always past the limit, and nineteen can be (nineteen
    //nines are), so both the count and the sum are asked.  `wrote` is what
    //decides whether the written spelling may be handed over at all.
    wrote=1;
    if (sig>19)
      wrote=0;
    else if (CCDigOvf(sd,sig,0))
      wrote=0;
    while ((keep>1)&&(sd[keep-1]==0))
      keep--;			//a trailing zero belongs to the exponent, not to
    				//the digit string: `100e-2` is the integer 1
    while ((keep>1)&&CCDigOvf(sd,keep,0))
      keep--;			//rule 1: drop the least significant digit, which
    				//the exponent below absorbs
    dotrel=intdig-lz;		//where the point sits, counted from the first
    				//significant digit: `0.00123` gives -2
    e2=dotrel-keep+ev;		//the power of ten that goes with the kept digits
    plain=0;
    if ((e2>=0)&&((keep+e2)<=CC_DIG_MAX)&&(!CCDigOvf(sd,keep,e2)))
      plain=1;			//rule 2: an integer decimal that the host can sum
    n=0;
    if (plain)
    {
      for (i=0;i!=keep;i++)
      {
        cc->tk_str[n]='0'+sd[i];
        n++;
      }
      for (i=0;i!=e2;i++)
      {
        cc->tk_str[n]='0';
        n++;
      }
      cc->tk_str[n]=0;
    }
    else if (wrote&&(me-s)<CC_TOK_MAX)
    {
      //The literal as written: the host can sum every one of its digits, and the
      //only spelling change worth making is the exponent's `+` (rule 3).  A
      //negative exponent is left exactly as the program wrote it, because the
      //measurement says folding it in would not improve the answer.
      tc=s;
      while (tc!=me)
      {
        if ((*tc=='+')&&((tc[-1]=='e')||(tc[-1]=='E')))
          tc++;
        else
        {
          cc->tk_str[n]=*tc;
          n++;
          tc++;
        }
      }
      cc->tk_str[n]=0;
    }
    else
    {
      //Too many significant digits for the host's sum, or too many characters
      //for the token buffer: the written spelling cannot be handed over, so what
      //goes instead is the same value in the shortest form lite can build from
      //digits it has already checked fit.
      cc->tk_str[0]='0'+sd[0];
      n=1;
      if (keep>1)
      {
        cc->tk_str[n]='.';
        n++;
        for (i=1;i!=keep;i++)
        {
          cc->tk_str[n]='0'+sd[i];
          n++;
        }
      }
      cc->tk_str[n]='e';
      n++;
      e1=dotrel-1+ev;		//the exponent of sd[0].sd[1..]
      if (e1>1000000)
        e1=1000000;		//past this every double is 0 or infinity, so the
      if (e1<-1000000)		//clamp cannot change the answer (measured for
        e1=-1000000;		//1e1000000 and 1e-1000000)
      if (e1<0)
      {
        cc->tk_str[n]='-';
        n++;
        e1=-e1;
      }
      tl=0;
      while ((tl<7)&&(e1!=0))
      {
        tx[tl]='0'+(e1%10);
        e1=e1/10;
        tl++;
      }
      if (!tl)
        tx[tl++]='0';		//an exponent of zero still needs a digit
      while (tl)
      {
        tl--;
        cc->tk_str[n]=tx[tl];
        n++;
      }
      cc->tk_str[n]=0;
    }
  }
  end=0;
  f=Str2F64(cc->tk_str,&end);
  if ((!end)||(end!=(cc->tk_str+n)))
  {
    //The host did not read the whole text it was handed, so the number it
    //returned is the number of the part it DID read: `1e+2` answering 1.0 is what
    //this test exists for.  lite built both spellings it can hand over, so a
    //mismatch here is a disagreement about grammar, and reporting it beats
    //returning a value nobody asked for.
    CCError(cc,"the kernel's decimal conversion did not read this literal whole");
    cc->tk_isf=0;
    cc->tk=TK_NUM;
    cc->pc=p;
    return TRUE;
  }
  cc->tk_num=f(I64);			//the bits; no lite-side conversion
  cc->tk_isf=1;
  cc->tk=TK_NUM;
  cc->pc=p;
  return TRUE;
}

U0 CCLex(CCCtrl *cc)
{
  U8 *p;
  U8 *s;
  I64 i,ch,ish;
  CCSkipSpace(cc);
  p=cc->pc;
  cc->tk_kw=KW_NONE;
  cc->tk_isf=0;		//M35: floatness is the lexer's answer for THIS token,
  			//cleared with tk_kw so a number token never inherits
  			//the previous literal's bits-as-float status.
  if (!*p)
  {
    cc->tk=TK_EOF;
    return;
  }
  if (CIsAlpha(*p))
  {
    i=0;
    while ((CIsAlpha(*p))||(CIsDigit(*p)))
    {
      if (i<CC_TOK_MAX-1)
        cc->tk_str[i++]=*p;
      p++;
    }
    cc->tk_str[i]=0;
    cc->pc=p;
    cc->tk=TK_IDENT;
    if (!StrCmp(cc->tk_str,"int"))
      cc->tk_kw=KW_INT;
    else if (!StrCmp(cc->tk_str,"return"))
      cc->tk_kw=KW_RETURN;
    else if (!StrCmp(cc->tk_str,"if"))
      cc->tk_kw=KW_IF;
    else if (!StrCmp(cc->tk_str,"else"))
      cc->tk_kw=KW_ELSE;
    else if (!StrCmp(cc->tk_str,"while"))
      cc->tk_kw=KW_WHILE;
    else if (!StrCmp(cc->tk_str,"break"))
      cc->tk_kw=KW_BREAK;
    else if (!StrCmp(cc->tk_str,"char"))
      cc->tk_kw=KW_CHAR;
    else if (!StrCmp(cc->tk_str,"for"))
      cc->tk_kw=KW_FOR;
    else if (!StrCmp(cc->tk_str,"continue"))
      cc->tk_kw=KW_CONTINUE;
    else if (!StrCmp(cc->tk_str,"struct"))
      cc->tk_kw=KW_STRUCT;
    else if (!StrCmp(cc->tk_str,"const"))
      cc->tk_kw=KW_CONST;
    else if (!StrCmp(cc->tk_str,"void"))
      cc->tk_kw=KW_VOID;
    else if (!StrCmp(cc->tk_str,"unsigned"))
      cc->tk_kw=KW_UNSIGNED;
    else if (!StrCmp(cc->tk_str,"long"))
      cc->tk_kw=KW_LONG;
    else if (!StrCmp(cc->tk_str,"signed"))
      cc->tk_kw=KW_SIGNED;
    else if (!StrCmp(cc->tk_str,"typedef"))
      cc->tk_kw=KW_TYPEDEF;
    else if (!StrCmp(cc->tk_str,"case"))
      cc->tk_kw=KW_CASE;
    else if (!StrCmp(cc->tk_str,"default"))
      cc->tk_kw=KW_DEFAULT;
    else if (!StrCmp(cc->tk_str,"switch"))
      cc->tk_kw=KW_SWITCH;
    else if (!StrCmp(cc->tk_str,"do"))
      cc->tk_kw=KW_DO;
    else if (!StrCmp(cc->tk_str,"sizeof"))
      cc->tk_kw=KW_SIZEOF;
    else if (!StrCmp(cc->tk_str,"short"))
      cc->tk_kw=KW_SHORT;
    else if (!StrCmp(cc->tk_str,"goto"))
      cc->tk_kw=KW_GOTO;
    else if (!StrCmp(cc->tk_str,"union"))
      cc->tk_kw=KW_UNION;
    else if (!StrCmp(cc->tk_str,"extern"))
      cc->tk_kw=KW_EXTERN;
    else if (!StrCmp(cc->tk_str,"static"))
      cc->tk_kw=KW_STATIC;
    else if (!StrCmp(cc->tk_str,"register"))
      cc->tk_kw=KW_REGISTER;
    else if (!StrCmp(cc->tk_str,"auto"))
      cc->tk_kw=KW_AUTO;
    else if (!StrCmp(cc->tk_str,"volatile"))
      cc->tk_kw=KW_VOLATILE;
    else if (!StrCmp(cc->tk_str,"inline"))
      cc->tk_kw=KW_INLINE;
    else if (!StrCmp(cc->tk_str,"enum"))
      cc->tk_kw=KW_ENUM;
    else if (!StrCmp(cc->tk_str,"double"))
      cc->tk_kw=KW_DOUBLE;	//M35: the F64 model (TY_DOUBLE)
    else if (!StrCmp(cc->tk_str,"float"))
      cc->tk_kw=KW_FLOAT;	//folded to TY_DOUBLE: the host has no
      				//single precision to fold it INTO
    return;
  }
  if (*p=='"')
  {
    //String literal: bytes land in tk_str (NUL terminated). Escapes:
    //\n \t \r \\ \" \a \b \f \v \e, \xHH (M19) and \0..\7 octal
    //(M19, up to 3 digits). Embedded NULs unsupported: a \0 ends the
    //literal early exactly like C strings behave downstream anyway.
    //Silent truncation would be a nasty bug, so over-length is an error.
    i=0;
    p++;
    while ((*p)&&(*p!='"'))
    {
      ch=*p++;
      if (ch=='\\')
      {
        ch=*p++;
        if (ch=='"')
          ch='"';		//the quote this literal is closed by
        else
          CCEscape(cc,p,ch,&ch,&p);	//FALSE keeps the byte: an unknown
      }
      if (i>=CC_TOK_MAX-1)
      {
        cc->pc=p;
        cc->tk=TK_STR;
        CCError(cc,"string literal too long");
        return;
      }
      cc->tk_str[i++]=ch;
    }
    cc->tk_str[i]=0;
    if (*p=='"')
      p++;
    else
      CCError(cc,"unterminated string");
    cc->pc=p;
    cc->tk=TK_STR;
    return;
  }
  if (*p==39)		//' char literal -> TK_NUM (full M19 escape set:
			//\n \t \r \\ \' \a \b \f \v \e \xHH \octal)
  {
    //39 spares HolyC a nested-quote escape in this source.
    p++;
    ch=*p++;
    if (ch=='\\')
    {
      ch=*p++;
      if (ch==39)
        ch=39;		//the quote this literal is closed by
      else
        CCEscape(cc,p,ch,&ch,&p);	//FALSE keeps the byte
    }
    if (*p!=39)
    {
      cc->pc=p;
      cc->tk=TK_NUM;
      CCError(cc,"unterminated char literal");
      return;
    }
    p++;
    cc->pc=p;
    cc->tk_num=ch;
    cc->tk=TK_NUM;
    return;
  }
  if (CIsDigit(*p))
  {
    s=p;
    ish=0;
    cc->tk_num=0;
    if ((*p=='0')&&((p[1]=='x')||(p[1]=='X')))
    {
      ish=1;
      p+=2;
      while (TRUE)
      {
        ch=*p;
        if (CIsDigit(ch))
          ch-='0';
        else if ((ch>='a')&&(ch<='f'))
          ch=ch-'a'+10;
        else if ((ch>='A')&&(ch<='F'))
          ch=ch-'A'+10;
        else
          break;
        cc->tk_num=cc->tk_num*16+ch;
        p++;
      }
    }
    else
      while (CIsDigit(*p))
        cc->tk_num=cc->tk_num*10+(*p++-'0');
    //M35: a dot or an exponent makes this a float spelling.  The peek is
    //deliberately more permissive than the scan itself (`1e` peeks positive and
    //CCNumFloat then declines it, which is right: that is the integer 1 plus the
    //identifier e), and HEX is excluded because the kernel's Str2F64 answers 0
    //for `0x1p3` — silently taking 0 for a hex float would be a wrong value,
    //where leaving the digits as an integer gives a syntax error instead.
    if (!ish)
    {
      if (((*p=='.')||CCExpAt(p))&&CCNumFloat(cc,s))
        return;
    }
    cc->pc=p;
    cc->tk=TK_NUM;
    return;
  }
  if ((*p=='.')&&CIsDigit(p[1]))		//`.5`: C-legal, and unambiguous —
  {					//a '.' in a member access is always
    if (CCNumFloat(cc,p))			//followed by an identifier, so a
      return;				//digit after it means a number here
  }
  //3-char ops must be tested before their 2-char prefixes.
  if ((p[0]=='<')&&(p[1]=='<')&&(p[2]=='='))
  {
    cc->tk_num=OP_SHLEQ;
    cc->pc=p+3;
    cc->tk=TK_PUNCT;
    return;
  }
  if ((p[0]=='>')&&(p[1]=='>')&&(p[2]=='='))
  {
    cc->tk_num=OP_SHREQ;
    cc->pc=p+3;
    cc->tk=TK_PUNCT;
    return;
  }
  cc->tk_num=*p;
  if ((p[0]=='=')&&(p[1]=='='))
    cc->tk_num=OP_EQ;
  else if ((p[0]=='!')&&(p[1]=='='))
    cc->tk_num=OP_NE;
  else if ((p[0]=='<')&&(p[1]=='='))
    cc->tk_num=OP_LE;
  else if ((p[0]=='>')&&(p[1]=='='))
    cc->tk_num=OP_GE;
  else if ((p[0]=='&')&&(p[1]=='&'))
    cc->tk_num=OP_AND;
  else if ((p[0]=='|')&&(p[1]=='|'))
    cc->tk_num=OP_OR;
  else if ((p[0]=='+')&&(p[1]=='+'))
    cc->tk_num=OP_INC;
  else if ((p[0]=='-')&&(p[1]=='-'))
    cc->tk_num=OP_DEC;
  else if ((p[0]=='-')&&(p[1]=='>'))
    cc->tk_num=OP_ARROW;
  else if ((p[0]=='<')&&(p[1]=='<'))
    cc->tk_num=OP_SHL;
  else if ((p[0]=='>')&&(p[1]=='>'))
    cc->tk_num=OP_SHR;
  else if ((p[0]=='+')&&(p[1]=='='))
    cc->tk_num=OP_PLUSEQ;
  else if ((p[0]=='-')&&(p[1]=='='))
    cc->tk_num=OP_MINUSEQ;
  else if ((p[0]=='*')&&(p[1]=='='))
    cc->tk_num=OP_STAREQ;
  else if ((p[0]=='/')&&(p[1]=='='))
    cc->tk_num=OP_SLASHEQ;
  else if ((p[0]=='%')&&(p[1]=='='))
    cc->tk_num=OP_PCTEQ;
  else if ((p[0]=='&')&&(p[1]=='='))
    cc->tk_num=OP_AMPEQ;
  else if ((p[0]=='|')&&(p[1]=='='))
    cc->tk_num=OP_PIPEEQ;
  else if ((p[0]=='^')&&(p[1]=='='))
    cc->tk_num=OP_CARETEQ;
  if (cc->tk_num!=*p)
  {
    cc->pc=p+2;
    cc->tk=TK_PUNCT;
    return;
  }
  cc->pc=p+1;
  cc->tk=TK_PUNCT;
}
