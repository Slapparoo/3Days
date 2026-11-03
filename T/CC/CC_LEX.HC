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

U0 CCLex(CCCtrl *cc)
{
  U8 *p;
  I64 i,ch;
  CCSkipSpace(cc);
  p=cc->pc;
  cc->tk_kw=KW_NONE;
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
    return;
  }
  if (*p=='"')
  {
    //String literal: bytes land in tk_str (NUL terminated). Escape
    //support: \n \t \r \\ \" — embedded NULs unsupported. Silent
    //truncation would be a nasty bug, so over-length is an error.
    i=0;
    p++;
    while ((*p)&&(*p!='"'))
    {
      ch=*p++;
      if (ch=='\\')
      {
        ch=*p;
        if (ch=='n')
          ch='\n';
        else if (ch=='t')
          ch='\t';
        else if (ch=='r')
          ch='\r';
        p++;
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
  if (*p==39)		//' char literal -> TK_NUM (escapes: \n \t \r \0 \\ \')
  {
    //39 spares HolyC a nested-quote escape in this source.
    p++;
    ch=*p++;
    if (ch=='\\')
    {
      ch=*p++;
      if (ch=='n')
        ch='\n';
      else if (ch=='t')
        ch='\t';
      else if (ch=='r')
        ch='\r';
      else if (ch=='0')
        ch=0;
      else if (ch=='\\')
        ch='\\';
      else if (ch==39)
        ch=39;
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
    cc->tk_num=0;
    if ((*p=='0')&&((p[1]=='x')||(p[1]=='X')))
    {
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
    cc->pc=p;
    cc->tk=TK_NUM;
    return;
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
