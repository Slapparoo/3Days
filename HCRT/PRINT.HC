U8 *StrPrint(U8 *dst,U8 *fmt,...);
#define STR_LEN 144
#define PRTF_PAD_ZERO 0x001
#define PRTF_LEFT_JUSTIFY 0x002
#define PRTF_TRUNCATE 0x004
#define PRTF_COMMA 0x008
#define PRTF_DOLLAR 0x010
#define PRTF_SLASH 0x020
#define PRTF_QUESTION 0x040
#define PRTF_AUX_FMT_NUM 0x080
#define PRTF_DECIMAL  0x100
#define PRTF_NEG  0x200
#define PRTF_NEG_E  0x400
#define PRTF_NEG_AUX_FMT_NUM 0x800
U0 SPutChar(U8 **_dst,U8 ch,U8 **_buf)
{
  I64 i;
  U8 *dst=*_dst,*buf;
  if (_buf)
  {
    buf=*_buf;
    i=dst-buf;
    if (i>=MSize(buf))
    {
      buf=MAlloc(i<<1+1);
      MemCpy(buf,*_buf,i);
      Free(*_buf);
      dst=buf+i;
      *_buf=buf;
    }
  }
  *dst++=ch;
  *_dst=dst;
}

U0 SPutChars(U8 **_dst,U8 *src,U8 **_buf, I64 len)
{
  I64 i;
  if (len<=0)
    return;
  U8 *dst=*_dst,*buf;
  if (_buf)
  {
    buf=*_buf;
    i=dst-buf;
    if (i+len>=MSize(buf))
    {
      buf=MAlloc((i+len)<<1+1);
      MemCpy(buf,*_buf,i);
      Free(*_buf);
      dst=buf+i;
      *_buf=buf;
    }
  }
  MemCpy(dst,src,len);
  *_dst=dst+len;
}

U0 SetChars(U8 **_dst,I64 ch,U8 **_buf, I64 len)
{
  I64 i;
  if (len<=0)
    return;
  U8 *dst=*_dst,*buf;
  if (_buf)
  {
    buf=*_buf;
    i=dst-buf;
    if (i+len>=MSize(buf))
    {
      buf=MAlloc((i+len)<<1+1);
      MemCpy(buf,*_buf,i);
      Free(*_buf);
      dst=buf+i;
      *_buf=buf;
    }
  }
  MemSet(dst,ch,len);
  *_dst=dst+len;
}

U0 OutStr(U8 *ptr,U8 **_buf,U8 **_dst,I64 len,I64 flags)
{
  I64 i;
  if (!ptr)
    i=0;
  else
    i=StrLen(ptr);
  if (flags&PRTF_TRUNCATE && i>len)
    i=len;
  if (flags&PRTF_LEFT_JUSTIFY)
  {
    SPutChars(_dst,ptr,_buf,i);
    SetChars(_dst,' ',_buf,len-i);
  }
  else
  {
    SetChars(_dst,' ',_buf,len-i);
    SPutChars(_dst,ptr,_buf,i);
  }
}
U8 *MPrintQ(U8 *ptr,I64 flags)
{
  U8 **_buf,*buf,**_dst,*dst,buf2[8],*ptr2;
  I64 ch;
  buf=MAlloc(STR_LEN);
  _buf=&buf;
  dst=buf;
  _dst=&dst;
  if (ptr)
    while (ch=*ptr++)
    {
      switch (ch)
      {
      case '$$':
        if (flags&PRTF_DOLLAR)
        {
          SPutChar(_dst,'\\',_buf);
          SPutChar(_dst,'d',_buf);
        }
        else
        {
          SPutChar(_dst,ch,_buf);
          SPutChar(_dst,ch,_buf);
        }
        break;
      case '%':
        SPutChar(_dst,ch,_buf);
        if (flags&PRTF_SLASH)
          SPutChar(_dst,ch,_buf);
        break;
      case '\n':
        SPutChar(_dst,'\\',_buf);
        SPutChar(_dst,'n',_buf);
        break;
      case '\r':
        SPutChar(_dst,'\\',_buf);
        SPutChar(_dst,'r',_buf);
        break;
      case '\t':
        SPutChar(_dst,'\\',_buf);
        SPutChar(_dst,'t',_buf);
        break;
      case '"':
      case '\\':
        SPutChar(_dst,'\\',_buf);
        SPutChar(_dst,ch,_buf);
        break;
      default:
        StrPrint(buf2,"\\x%02X",ch);
        ptr2=buf2;
        while (*ptr2)
            SPutChar(_dst,*ptr2++,_buf);
      }
    }
  SPutChar(_dst,0,_buf);
  return buf;
}
U8 *MPrintq(U8 *ptr,I64 flags)
{
  U8 **_buf,*buf,**_dst,*dst;
  I64 i,j,ch,ch1;
  buf=MAlloc(STR_LEN);
  _buf=&buf;
  dst=buf;
  _dst=&dst;
  if (ptr)
    while (ch=*ptr++)
    {
      ch1=*ptr;
      switch (ch)
      {
      case '\\':
        switch (ch1)
        {
start:
        case '0':
          SPutChar(_dst,0,_buf);
          break;
        case '\'':
          SPutChar(_dst,'\'',_buf);
          break;
        case '`':
          SPutChar(_dst,'`',_buf);
          break;
        case '"':
          SPutChar(_dst,'"',_buf);
          break;
        case '\\':
          SPutChar(_dst,'\\',_buf);
          break;
        case 'd':
          SPutChar(_dst,'$$',_buf);
          break;
        case 'n':
          SPutChar(_dst,'\n',_buf);
          break;
        case 'r':
          SPutChar(_dst,'\r',_buf);
          break;
        case 't':
          SPutChar(_dst,'\t',_buf);
          break;
end:
          ptr++;
          break;
        case 'x':
        case 'X':
          i=0;
          ptr++;
          for (j=0; j<2; j++)
          {
            ch1=ToUpper(*ptr++);
            if (Bt(char_bmp_hex_numeric,ch1))
            {
              if (ch1<='9')
                i=i<<4+ch1-'0';
              else
                i=i<<4+ch1-'A'+10;
            }
            else
            {
              ptr--;
              break;
            }
          }
          SPutChar(_dst,i,_buf);
          break;
        default:
          SPutChar(_dst,ch,_buf);
        }
        break;
      case '$$':
        SPutChar(_dst,ch,_buf);
        if (ch1=='$$')
          ptr++;
        break;
      case '%':
        SPutChar(_dst,ch,_buf);
        if (flags&PRTF_SLASH && ch1=='%')
          ptr++;
        break;
      default:
        SPutChar(_dst,ch,_buf);
      }
    }
  SPutChar(_dst,0,_buf);
  return buf;
}
#define TMP_BUF_LEN 256
#define SLOP  8
U8 *sys_pos_pows_lets=" KMGTPEZY",
   *sys_neg_pows_lets=" munpfazy";
U8 *StrPrintJoin(U8 *dst,U8 *fmt,I64 argc,I64 *argv)
{
  /*$LK,"Print(\"\") Fmt Strings",A="FI:::/Doc/Print.DD"$
  In float formatting, do not exceed 18-digits
  before or after the decimal point
  because the numbers before and after
  the decimal point are stored
  in 64-bits.Use exponentiated forms
  to avoid this.
  */
  I64 i,j,l,ch,k,k0,n,n0,len,dec_len,flags,old_flags,
      aux_fmt_num,comma_cnt,comma_fmt_cnt,cur_arg=0;
  I64 m;
  F64 d,d1;
  U8 *ptr,**_buf,*buf,**_dst,tmp_buf[TMP_BUF_LEN],tmp_buf2[TMP_BUF_LEN*2];
  if (!fmt)
    throw('StrPrint');
  if (dst)
  {
    _buf=NULL;
    buf=dst;
  }
  else
  {
    buf=MAlloc(STR_LEN);
    _buf=&buf;
    dst=buf;
  }
  _dst=&dst;
  while (ch = *fmt++)
  {
    if (ch=='%')
    {
      flags=0;
      if (*fmt=='-')
      {
        flags|=PRTF_LEFT_JUSTIFY;
        fmt++;
      }
      if (*fmt=='0')
      {
        flags|=PRTF_PAD_ZERO;
        fmt++;
      }
      len=0;
      while ('0'<=*fmt<='9')
        len=len*10+ *fmt++ -'0';
      if (*fmt=='*')
      {
        fmt++;
        if (cur_arg>=argc)
          throw('StrPrint');
        len=argv[cur_arg++];
      }
      dec_len=0;
      if (*fmt=='.')
      {
        fmt++;
        while ('0'<=*fmt<='9')
          dec_len=dec_len*10+ *fmt++ -'0';
        if (*fmt=='*')
        {
          fmt++;
          if (cur_arg>=argc)
            throw('StrPrint');
          dec_len=argv[cur_arg++];
        }
        flags|=PRTF_DECIMAL;
      }
      aux_fmt_num=0;
      while (TRUE)
      {
        switch (*fmt)
        {
start:
        case '$$':
          flags|=PRTF_DOLLAR;
          break;
        case '/':
          flags|=PRTF_SLASH;
          break;
        case ',':
          flags|=PRTF_COMMA;
          break;
        case 't':
          flags|=PRTF_TRUNCATE;
          break;
        case 'l': //harmless
          break;
end:
          fmt++;
          break;
        case 'h':
          fmt++;
          flags|=PRTF_AUX_FMT_NUM;
          if (*fmt=='?')
          {
            fmt++;
            flags|=PRTF_QUESTION;
          }
          else
          {
            if (*fmt=='*')
            {
              fmt++;
              if (cur_arg>=argc)
                throw('StrPrint');
              aux_fmt_num=argv[cur_arg++];
            }
            else
            {
              if (*fmt=='-')
              {
                fmt++;
                flags|=PRTF_NEG_AUX_FMT_NUM;
              }
              while ('0'<=*fmt<='9')
                aux_fmt_num=aux_fmt_num*10+ *fmt++ -'0';
              if (flags&PRTF_NEG_AUX_FMT_NUM)
                aux_fmt_num=-aux_fmt_num;
            }
          }
          break;
        default:
          goto sp_arg;
        }
      }
sp_arg:
      k=0;
      switch (*fmt++)
      {
start:
      case 'Q':
        if (cur_arg>=argc)
          throw('StrPrint');
        ptr=MPrintQ(argv[cur_arg++],flags);
        break;
      case 'q':
        if (cur_arg>=argc)
          throw('StrPrint');
        ptr=MPrintq(argv[cur_arg++],flags);
        break;
        /*
      case 'D':
        if (cur_arg>=argc)
          throw('StrPrint');
        ptr=MPrintDate(argv[cur_arg++]);
        break;
      case 'T':
        if (cur_arg>=argc)
          throw('StrPrint');
        ptr=MPrintTime(argv[cur_arg++]);
        break;
        */
end:
        OutStr(ptr,_buf,_dst,len,flags);
        Free(ptr);
        break;
start:
      case 's':
        if (cur_arg>=argc)
          throw('StrPrint');
        ptr=argv[cur_arg++];
        break;
        /* //TODO
      case 'S':
        if (cur_arg>=argc)
          throw('StrPrint');
        ptr=Define(argv[cur_arg++]);
        break;
      case 'z':
        if (cur_arg+1>=argc)
          throw('StrPrint');
        ptr=LstSub(argv[cur_arg],argv[cur_arg+1]);
        cur_arg=cur_arg+2;
        break;
      case 'Z':
        if (cur_arg+1>=argc)
          throw('StrPrint');
        ptr=DefineSub(argv[cur_arg],argv[cur_arg+1]);
        cur_arg=cur_arg+2;
        break;
        */
end:
        OutStr(ptr,_buf,_dst,len,flags);
        break;
start:
      case 'c':
        if (cur_arg>=argc)
          throw('StrPrint');
        tmp_buf[0](I64)=argv[cur_arg++];
        tmp_buf[8]=0;
        break;
      case 'C':
        if (cur_arg>=argc)
          throw('StrPrint');
        tmp_buf[0](I64)=argv[cur_arg++];
        tmp_buf[8]=0;
        ptr=tmp_buf;
        while (*ptr)
        {
          *ptr=ToUpper(*ptr);
          ptr++;
        }
        break;
end:
        if (!(flags&PRTF_AUX_FMT_NUM))
          aux_fmt_num=1;
        while (aux_fmt_num-->0)
          OutStr(tmp_buf,_buf,_dst,len,flags);
        break;
      case 'p':
        goto sp_hex;
        if (cur_arg>=argc)
          throw('StrPrint');
        //StrPrintFunSeg(tmp_buf,argv[cur_arg++],len,flags);
        OutStr(tmp_buf,_buf,_dst,len,flags);
        break;
      case 'd':
        if (cur_arg>=argc)
          throw('StrPrint');
        m=argv[cur_arg++];
        if (m(I64)<0)
        {
          flags|=PRTF_NEG;
          m=-m;
        }
sp_out_dec:
        if (flags&PRTF_AUX_FMT_NUM)
        {
          if (!len) len=12;
          d=m;
          goto sp_out_eng;
        }
        if (flags&PRTF_COMMA)
        {
          comma_fmt_cnt=comma_cnt=3;
          do
          {
            tmp_buf[k++]=(m)(I64)%10+'0';
            m/=10;
            if (!m) break;
            if (!--comma_cnt)
            {
              tmp_buf[k++]=',';
              comma_cnt=3;
            }
          }
          while (k<TMP_BUF_LEN-SLOP);
sp_out_comma_num:
          if (flags&PRTF_NEG)
            i=1;
          else
            i=0;
          if (len<0)
            len=0;
          if (flags&PRTF_TRUNCATE && k+i>len)
            k=len-i;
          if (k<0)
            k=0;
          if (flags&PRTF_PAD_ZERO)
          {
            if (flags&PRTF_NEG)
              SPutChar(_dst,'-',_buf);
            comma_cnt=(len-k-i+comma_fmt_cnt-comma_cnt+1)
                      %(comma_fmt_cnt+1)+1;
            for (; i<len-k; i++)
            {
              if (!--comma_cnt)
              {
                SPutChar(_dst,',',_buf);
                comma_cnt=comma_fmt_cnt;
                if (++i>=len-k)
                  break;
              }
              SPutChar(_dst,'0',_buf);
            }
          }
          else
          {
            for (; i<len-k; i++)
              SPutChar(_dst,' ',_buf);
            if (flags&PRTF_NEG)
              SPutChar(_dst,'-',_buf);
          }
        }
        else
        {
          do
          {
            tmp_buf[k++]=(m)(I64)%10+'0';
            m/=10;
            if (!m) break;
          }
          while (k<TMP_BUF_LEN-SLOP);
sp_out_num:
          if (flags&PRTF_NEG)
            i=1;
          else
            i=0;
          if (len<0)
            len=0;
          if (flags&PRTF_TRUNCATE && k+i>len)
            k=len-i;
          if (k<0)
            k=0;
          if (flags&PRTF_PAD_ZERO)
          {
            if (flags&PRTF_NEG)
              SPutChar(_dst,'-',_buf);
            for (; i<len-k; i++)
              SPutChar(_dst,'0',_buf);
          }
          else
          {
            for (; i<len-k; i++)
              SPutChar(_dst,' ',_buf);
            if (flags&PRTF_NEG)
              SPutChar(_dst,'-',_buf);
          }
        }
        for (i=k-1; i>=0; i--)
          SPutChar(_dst,tmp_buf[i],_buf);
        break;
      case 'u':
        if (cur_arg>=argc)
          throw('StrPrint');
        m=argv[cur_arg++];
        goto sp_out_dec;
      case 'f':
        if (cur_arg>=argc)
          throw('StrPrint');
        d=argv[cur_arg++](F64);
        if (d<0)
        {
          flags|=PRTF_NEG;
          d=-d;
        }
        if (d==inf)
        {
sp_out_inf:
          if (flags&PRTF_NEG)
            i=1;
          else
            i=0;
          k=1;
          if (len<0)
            len=0;
          if (flags&PRTF_TRUNCATE && k+i>len)
            k=len-i;
          if (k<0)
            k=0;
          for (; i<len-k; i++)
            SPutChar(_dst,' ',_buf);
          if (flags&PRTF_NEG)
            SPutChar(_dst,'-',_buf);
          for (i=0; i<k; i++)
            SPutChar(_dst,'inf',_buf);
          break;
        }
sp_out_f:
        if (dec_len<0)
          dec_len=0;
        n=Log10(d);
        if (i=dec_len)
        {
          if (flags&PRTF_COMMA)
            i=i-i/4;
          if (n+i>17)
          {
            n+=i-17;
            d*=10.`(i-n);
          }
          else
          {
            n=0;
            d*=10.`i;
          }
          i=dec_len;
        }
        else if (n>17)
        {
          n-=17;
          d*=10.`-n;
        }
        else
          n=0;
        m=Round(d);
        if (flags&PRTF_COMMA)
        {
          comma_cnt=i&3;
          while (i-- && k<TMP_BUF_LEN-SLOP)
          {
            if (i>2 && !comma_cnt--)
            {
              tmp_buf[k++]=',';
              comma_cnt=2;
              if (!--i) break;
            }
            if (n)
            {
              n--;
              tmp_buf[k++]='0';
            }
            else {
              tmp_buf[k++]=(m)(I64)%10+'0';
                m/=10;
            }
            if (!i) break;
          }
        }
        else
        {
          while (i-- && k<TMP_BUF_LEN-SLOP)
          {
            if (n)
            {
              n--;
              tmp_buf[k++]='0';
            }
            else {
              tmp_buf[k++]=(m)(I64)%10+'0';
                m/=10;
            }
          }
        }
        if (dec_len)
          tmp_buf[k++]='.';
        if (flags&PRTF_COMMA)
        {
          comma_cnt=3;
          do
          {
            if (n)
            {
              n--;
              tmp_buf[k++]='0';
            }
            else {
              tmp_buf[k++]=(m)(I64)%10+'0';
                m/=10;
            }
            if (!m) break;
            if (!--comma_cnt)
            {
              tmp_buf[k++]=',';
              comma_cnt=3;
            }
          }
          while (k<TMP_BUF_LEN-SLOP);
        }
        else
        {
          do
          {
            if (n)
            {
              n--;
              tmp_buf[k++]='0';
            }
            else {
              tmp_buf[k++]=(m)(I64)%10+'0';
                m/=10;
            }
            if (!m) break;
          }
          while (k<TMP_BUF_LEN-SLOP);
        }
        goto sp_out_num;
      case 'e':
        if (!len) len=12;
        flags|=PRTF_TRUNCATE;
        if (cur_arg>=argc)
          throw('StrPrint');
        d=argv[cur_arg++](F64);
        if (d<0)
        {
          flags|=PRTF_NEG;
          d=-d;
        }
        if (d==inf) goto sp_out_inf;
        if (d)
          n=Floor(Log10(d));
        else
          n=0;
sp_out_e:
        d/=10.`n;
        k0=k;
        for (l=0; l<2; l++)
        {
          n0=n;
          if (n<0)
          {
            n=-n;
            flags|=PRTF_NEG_E;
          }
          else
            flags&=~PRTF_NEG_E;
          i=3;
          do {
            tmp_buf[k++]=(n)(I64)%10+'0';
            n/=10;
          } while (n && i--);
          if (flags&PRTF_NEG_E)
            tmp_buf[k++]='-';
          tmp_buf[k++]='e';
          dec_len=len-k-2;
          if (flags&PRTF_NEG)
            dec_len--;
          if (d)
          {
            d1=d+10.`-dec_len/2;
            if (d1<1.0)
            {
              d*=10;
              n=n0-1;
              k=k0;
            }
            else if (d1>=10)
            {
              d/=10;
              n=n0+1;
              k=k0;
            }
            else
              break;
          }
          else
            break;
        }
        goto sp_out_f;
      case 'g':
        if (!len) len=12;
        flags|=PRTF_TRUNCATE;
        if (cur_arg>=argc)
          throw('StrPrint');
        d=argv[cur_arg++](F64);
        if (d<0)
        {
          flags|=PRTF_NEG;
          d=-d;
        }
        if (d==inf) goto sp_out_inf;
        if (d)
          n=Floor(Log10(d));
        else
          n=0;
        if (n>=len-1-dec_len || n<-(dec_len-1))
          goto sp_out_e;
        else
          goto sp_out_f;
      case 'n':
        if (!len) len=12;
        flags|=PRTF_TRUNCATE;
        if (cur_arg>=argc)
          throw('StrPrint');
        d=argv[cur_arg++](F64);
        if (d<0)
        {
          flags|=PRTF_NEG;
          d=-d;
        }
sp_out_eng: //Engineering notation
        if (d==inf) goto sp_out_inf;
        if (d)
          n=FloorI64(Floor(Log10(d)),3);
        else
          n=0;
        d/=10.`n;
        if (n<0)
        {
          n=-n;
          flags|=PRTF_NEG_E;
        }
        if (flags&PRTF_AUX_FMT_NUM && -24<=n<=24)
        {
          if (flags&PRTF_QUESTION)
          {
            if (flags&PRTF_NEG_E)
              i=-n/3;
            else
              i=n/3;
            j=0;
          }
          else
          {
            if (flags&PRTF_NEG_E)
              j=-n-aux_fmt_num;
            else
              j=n-aux_fmt_num;
            d*=10.`j;
            i=aux_fmt_num/3;
          }
          if (i<0)
            tmp_buf[k++]=sys_neg_pows_lets[-i];
          else if (i>0)
            tmp_buf[k++]=sys_pos_pows_lets[i];
          else if (len!=0)
            tmp_buf[k++]=' ';
          if (!(flags&PRTF_DECIMAL))
          {
            dec_len=len-k-2;
            if (flags&PRTF_NEG)
              dec_len--;
            if (j>0)
            {
              if (flags&PRTF_COMMA)
                dec_len-=4*j/3;
              else
                dec_len-=j;
            }
            d1=d+10.`(-dec_len+1)/2;
            if (d1>=10)
            {
              dec_len--;
              if (d1>=100)
                dec_len--;
            }
          }
        }
        else
        {
          i=3;
          do {
            tmp_buf[k++]=(n)(I64)%10+'0';
            n/=10;
          } while (n && i--);
          if (flags&PRTF_NEG_E)
            tmp_buf[k++]='-';
          tmp_buf[k++]='e';
          if (!dec_len)
          {
            dec_len=len-k-2;
            if (flags&PRTF_NEG)
              dec_len--;
            d1=d+10.`(-dec_len+1)/2;
            if (d1>=10)
            {
              dec_len--;
              if (d1>=100)
                dec_len--;
            }
          }
        }
        if (flags&PRTF_COMMA)
        {
          if (len && dec_len>0 && !(dec_len&3))
            tmp_buf[k++]=',';
          dec_len-=dec_len/4;
        }
        goto sp_out_f;
      case 'X':
        sp_hex:
        if (cur_arg>=argc)
          throw('StrPrint');
        m=argv[cur_arg++];
        if (flags&PRTF_COMMA)
        {
          comma_fmt_cnt=comma_cnt=4;
          do
          {
            tmp_buf[k]= m&15 +'0';
            if (tmp_buf[k]>'9') tmp_buf[k]+='A'-0x3A;
            k++;
            m>>=4;
            if (!m) break;
            if (!--comma_cnt)
            {
              tmp_buf[k++]=',';
              comma_cnt=4;
            }
          }
          while (k<TMP_BUF_LEN-SLOP);
          goto sp_out_comma_num;
        }
        else
        {
          do
          {
            tmp_buf[k]= m&15 +'0';
            if (tmp_buf[k]>'9') tmp_buf[k]+='A'-0x3A;
            k++;
            m>>=4;
          }
          while (m && k<TMP_BUF_LEN-SLOP);
          goto sp_out_num;
        }
      case 'x':
        if (cur_arg>=argc)
          throw('StrPrint');
        m=argv[cur_arg++];
        if (flags&PRTF_COMMA)
        {
          comma_fmt_cnt=comma_cnt=4;
          do
          {
            tmp_buf[k]= m&15 +'0';
            if (tmp_buf[k]>'9') tmp_buf[k]+='a'-0x3A;
            k++;
            m>>=4;
            if (!m) break;
            if (!--comma_cnt)
            {
              tmp_buf[k++]=',';
              comma_cnt=4;
            }
          }
          while (k<TMP_BUF_LEN-SLOP);
          goto sp_out_comma_num;
        }
        else
        {
          do
          {
            tmp_buf[k]= m&15 +'0';
            if (tmp_buf[k]>'9') tmp_buf[k]+='a'-0x3A;
            k++;
            m>>=4;
          }
          while (m && k<TMP_BUF_LEN-SLOP);
          goto sp_out_num;
        }
      case 'b':
      case 'B':
        if (cur_arg>=argc)
          throw('StrPrint');
        m=argv[cur_arg++];
        if (flags&PRTF_COMMA)
        {
          comma_fmt_cnt=comma_cnt=4;
          do
          {
            tmp_buf[k++]= m&1 +'0';
            m>>=1;
            if (!m) break;
            if (!--comma_cnt)
            {
              tmp_buf[k++]=',';
              comma_cnt=4;
            }
          }
          while (k<TMP_BUF_LEN-SLOP);
          goto sp_out_comma_num;
        }
        else
        {
          do
          {
            tmp_buf[k++]= m&1 +'0';
            m>>=1;
          }
          while (m && k<TMP_BUF_LEN-SLOP);
          goto sp_out_num;
        }
      case '%':
        SPutChar(_dst,'%',_buf);
        break;
      }
    }
    else
    {
      SPutChar(_dst,ch,_buf);
    }
  }
  SPutChar(_dst,0,_buf);
  return buf;
}

U8 *StrPrint(U8 *dst,U8 *fmt,...)
{//See $LK,"StrPrintJoin",A="MN:StrPrintJoin"$().
  return StrPrintJoin(dst,fmt,argc,argv);
}

U8 *CatPrint(U8 *_dst,U8 *fmt,...)
{//StrCat().See $LK,"StrPrintJoin",A="MN:StrPrintJoin"$().
  U8 *dst=_dst;
  while (*dst)
    dst++;
  StrPrintJoin(dst,fmt,argc,argv);
  return _dst;
}

U0 Print(U8 *fmt,...)
{//$LK,"Print(\"\") Fmt Strings",A="FI:::/Doc/Print.DD"$.See $LK,"StrPrintJoin",A="MN:StrPrintJoin"$().
//Don't use this.  $LK,"See Print() shortcut.",A="FF:::/Doc/HolyC.DD,DemoHolyC"$
  U8 *buf=StrPrintJoin(NULL,fmt,argc,argv);
  "%s",buf;
  Free(buf);
}

U8 *MStrPrint(U8 *fmt,...)
{//MAlloc StrPrint.See $LK,"StrPrintJoin",A="MN:StrPrintJoin"$().
  U8 *res,*buf=StrPrintJoin(NULL,fmt,argc,argv);
  res=StrNew(buf);
  Free(buf);
  return res;
}
