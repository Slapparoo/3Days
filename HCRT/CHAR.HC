U8 *StrNew(U8 *in) {
  return StrCpy(MAlloc(StrLen(in)+1),in);
}
U8 ToUpper(U8 chr) {
  switch(chr) {
    case 'a'...'z': return chr-'a'+'A';
  }
  return chr;
}
U32
  char_bmp_alpha[16]=
  {0x0000000,0x00000000,0x87FFFFFF,0x07FFFFFE,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0,0,0,0,0,0,0,0},

  char_bmp_alpha_numeric[16]=
  {0x0000000,0x03FF0000,0x87FFFFFF,0x07FFFFFE,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0,0,0,0,0,0,0,0},

  char_bmp_alpha_numeric_no_at[16]=
  {0x0000000,0x03FF0000,0x87FFFFFE,0x07FFFFFE,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0,0,0,0,0,0,0,0},

  char_bmp_word[16]=
  {0x0000000,0x03FF0080,0x87FFFFFE,0x07FFFFFE,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0,0,0,0,0,0,0,0},

  char_bmp_filename[16]=
  {0x0000000,0x03FF73FB,0xEFFFFFFF,0x6FFFFFFF,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0,0,0,0,0,0,0,0},

  char_bmp_dec_numeric[16]=
  {0x0000000,0x03FF0000,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

  char_bmp_hex_numeric[16]=
  {0x0000000,0x03FF0000,0x7E,0x7E,0,0,0,0,0,0,0,0,0,0,0,0},

  char_bmp_white_space[16]=
  {0x80002600,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

  char_bmp_non_eol_white_space[16]=
  {0x80000200,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

  char_bmp_zero_cr_nl_cursor[16]=
  {0x00002421,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

  char_bmp_zero_tab_cr_nl_cursor[16]=
  {0x00002621,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

  char_bmp_zero_tab_cr_nl_cursor_dollar[16]=
  {0x00002621,0x10,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

  char_bmp_macro[16]=
  {0x80002600,0xFFFFFFDF,0xFFFFFFFF,0x7FFFFFFF,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0,0,0,0,0,0,0,0},

  char_bmp_printable[16]=
  {0x80002600,0xFFFFFFFF,0xFFFFFFFF,0x7FFFFFFF,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0,0,0,0,0,0,0,0},

  char_bmp_displayable[16]=
  {0x80000000,0xFFFFFFFF,0xFFFFFFFF,0x7FFFFFFF,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0,0,0,0,0,0,0,0},

  char_bmp_safe_dollar[16]=
  {0x80000000,0xFFFFFFEF,0xFFFFFFFF,0x7FFFFFFF,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0,0,0,0,0,0,0,0},//same but no dollar sign

  char_bmp_non_eol[16]=
  {0xFFFFDBFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF};

I64 Str2I64(U8 *st,I64 radix=10,U8 **_end_ptr=NULL)
{//String to I64. Similar to strtoul().
//Allows radix change with "0x20" "0b1010" "0d123" "0o18".
  //Be careful of Str2I64("0b101",16)-->0xB101.
  Bool neg=FALSE;
  I64 ch,res=0;
  if (!st || !(2<=radix<=36)) {
    if (_end_ptr) *_end_ptr=st;
    return 0;
  }
  while (Bt(char_bmp_white_space,*st))
    st++;
  while (TRUE)
    switch (*st) {
      case '-':
	st++;
	neg=!neg;
	break;
      case '+':
	st++;
	break;
      case '0':
	st++;
	ch=ToUpper(*st);
	if (ch>='B' && (radix<=10 || ch>'A'+radix-11))
	  switch (ch) {
	    case 'B': radix=2;	st++; break;
	    case 'D': radix=10; st++; break;
	    case 'X': radix=16; st++; break;
	  }
      default:
	goto ai_cont;
    }
ai_cont:
  while (ch=ToUpper(*st++)) {
    if (radix>10) {
      if ('0'<=ch<='9')
	res=res*radix+ch-'0';
      else if ('A'<=ch<='A'+radix-11)
	res=res*radix+ch-'A'+10;
      else
	break;
    } else if ('0'<=ch<='0'+radix-1)
      res=res*radix+ch-'0';
    else
      break;
  }
  if (_end_ptr) *_end_ptr=st-1;
  if (neg)
    return -res;
  else
    return res;
}
F64 Str2F64(U8 *src,U8 **_end_ptr=NULL)
{/*String to F64.
Does not allow more than 18-digits
before or after the decimal point
because the numbers before and after
the decimal point are stored
in 64-bits.Use exponentiated forms
to avoid this.
*/
  I64 i,j,k,ch;
  F64 d;
  Bool neg=FALSE,neg_e=FALSE;

  ch=*src++;
  while (Bt(char_bmp_white_space,ch))
    ch=*src++;
  if (ch=='-') {
    neg=TRUE;
    ch=*src++;
  }
  if (!StrNCmp(src-1,"inf",3)) {
    d=0x7FF0000000000000(F64);
    src+=3;
    goto a2f_end;
  }
  i=0;
  while (TRUE) {
    if (Bt(char_bmp_dec_numeric,ch))
      i=i*10+ch-'0';
    else {
      if (ch=='.' || ch=='e' || ch=='E')
	break;
      d=i;
      goto a2f_end;
    }
    ch=*src++;
  }
  if (ch=='.')
    ch=*src++;
  k=0;
  while (TRUE) {
    if (Bt(char_bmp_dec_numeric,ch)) {
      i=i*10+ch-'0';
      k++;
    } else {
      if (ch=='e' || ch=='E')
	break;
      d=i*10.`(-k);
      goto a2f_end;
    }
    ch=*src++;
  }
  ch=*src++;
  if (ch=='-') {
    neg_e=TRUE;
    ch=*src++;
  }
  j=0;
  while (TRUE) {
    if (Bt(char_bmp_dec_numeric,ch))
      j=j*10+ch-'0';
    else {
      if (neg_e)
	d=i*10.`(-j-k);
      else
	d=i*10.`(j-k);
      goto a2f_end;
    }
    ch=*src++;
  }
a2f_end:
  if (_end_ptr) *_end_ptr=src-1;
  if (neg)
    return -d;
  else
    return d;
}
U8 *StrScan(U8 *src,U8 *fmt,...)
{/*Opposite of sprintf().Pass ptrs to data to be scanned-in.
For "%s", pass ptr to ptr (be careful because addr
of array is the same as array--create ptr to array
and take addr.
*/
  U8 *buf,*ptr,**pptr;
  Bool left_justify=FALSE;
  I64 ch,cur_arg=0,i,len,*i_ptr,dec_len;
  F64 *d_ptr;
  if (!fmt)
    throw('Scan');
  while (ch = *fmt++) {
    if (ch=='%') {
      if (*fmt=='%') {
	src++;
	fmt++;
      } else {
	if (*fmt=='-') {
	  left_justify=TRUE;
	  fmt++;
	} else
	  left_justify=FALSE;
	len=0;
	while ('0'<=*fmt<='9')
	  len=len*10+ (*fmt++ -'0');
	if (*fmt=='*') {
	  fmt++;
	  if (cur_arg>=argc)
	    throw('Scan');
	  len=argv[cur_arg++];
	}
	ch=*fmt++;
	if (ch && !len) {
	  ptr=src;
	  while (*ptr && *ptr!=*fmt)
	    ptr++;
	  len=ptr-src;
	} else {
	  if (ch=='.') {
	    dec_len=0;
	    while ('0'<=*fmt<='9')
	      dec_len=dec_len*10+ (*fmt++-'0');
	    if (*fmt=='*') {
	      fmt++;
	      if (cur_arg>=argc)
		throw('Scan');
	      dec_len=argv[cur_arg++];
	    }
	    ch=*fmt++;
	  }
	}
	buf=MAlloc(len+1);
	for (i=0;i<len;i++)
	  buf[i]=*src++;
	buf[i]=0;
	switch (ch) {
	  case 's':
	    if (cur_arg>=argc)
	      throw('Scan');
	    pptr=argv[cur_arg++];
	    StrCpy(*pptr,buf);
	    break;
	  case 'c':
	    if (cur_arg>=argc)
	      throw('Scan');
	    ptr=argv[cur_arg++];
	    *ptr=*buf;
	    break;
	  case 'C':
	    if (cur_arg>=argc)
	      throw('Scan');
	    ptr=argv[cur_arg++];
	    *ptr=ToUpper(*buf);
	    break;
      /*
	  case 'z':
	    if (cur_arg+1>=argc)
	      throw('Scan');
	    i_ptr=argv[cur_arg++];
	    *i_ptr=LstMatch(buf,argv[cur_arg++]);
	    break;
	  case 'Z':
	    if (cur_arg+1>=argc)
	      throw('Scan');
	    i_ptr=argv[cur_arg++];
	    *i_ptr=DefineMatch(buf,argv[cur_arg++]);
	    break;
      */
	  case 'd':
	    if (cur_arg>=argc)
	      throw('Scan');
	    i_ptr=argv[cur_arg++];
	    *i_ptr=Str2I64(buf);
	    break;
	  case 'X':
	    if (cur_arg>=argc)
	      throw('Scan');
	    i_ptr=argv[cur_arg++];
	    *i_ptr=Str2I64(buf,16);
	    break;
	  case 'b':
	    if (cur_arg>=argc)
	      throw('Scan');
	    i_ptr=argv[cur_arg++];
	    *i_ptr=Str2I64(buf,2);
	    break;
	  case 'e':
	  case 'f':
	  case 'g':
	  case 'n':
	    if (cur_arg>=argc)
	      throw('Scan');
	    d_ptr=argv[cur_arg++];
	    *d_ptr=Str2F64(buf);
	    break;
    /*
	  case 'D':
	    if (cur_arg>=argc)
	      throw('Scan');
	    i_ptr=argv[cur_arg++];
	    *i_ptr=Str2Date(buf);
	    break;
    */
	}
	Free(buf);
      }
    } else
      src++;
  }
  return src;
}
U8 *GetStr(U8 *msg=NULL,U8 *dft=NULL,I64 flags=0)
{//Returns a $LK,"MAlloc",A="MN:MAlloc"$()ed prompted string.  See $LK,"Flags",A="MN:GSF_SHIFT_ESC_EXIT"$.
  U8 *st;
  if (!msg)
    msg="";
  st=__GetStr(msg);
  if (!*st) {
    Free(st);
    if (dft)
      return StrNew(dft);
    else
      return StrNew("");
  }
  return st;
}

Bool YorN()
{//Wait for user to answer Y or N.
  I64 ch;
  while (TRUE) {
    U8 *yes=GetStr("(y or n)? ");
    ch=ToUpper(*yes);
    Free(yes);
    if (ch=='Y') {
      return TRUE;
    } else if (ch=='N') {
      return FALSE;
    }
  }
}
I64 PressAKey()
{//Print "Press a key" and wait for non-zero $LK,"ASCII",A="MN:CH_CTRLA"$ key.
  Free(GetStr);
}
public I64 GetI64(U8 *msg=NULL,I64 dft=0,I64 lo=I64_MIN,I64 hi=I64_MAX)
{//Prompt user for I64 expression.
  Bool okay;
  U8 *st;
  I64 res;
  while (TRUE) {
    if (msg)
    "%s\n",msg;
    st=GetStr;
    if (!*st) {
      Free(st);
      return dft;
    }
    try {
      res=Str2I64(st);
      okay=TRUE;
    } catch {
      Fs->catch_except=TRUE;
      okay=FALSE;
    }
    Free(st);
    if (okay && lo<=res<=hi)
      return res;
  }
}
public F64 GetF64(U8 *msg=NULL,F64 dft=0,F64 lo=F64_MIN,F64 hi=F64_MAX)
{//Prompt user for F64 expression.
  Bool okay;
  U8 *st;
  F64 res;
  while (TRUE) {
    if (msg)
      "%s\n",msg;
    st=GetStr;
    if (!*st) {
      Free(st);
      return dft;
    }
    try {
      res=Str2F64(st);
      okay=TRUE;
    } catch {
      Fs->catch_except=TRUE;
      okay=FALSE;
    }
    Free(st);
    if (okay && lo<=res<=hi)
      return res;
  }
}
U8 *StrIMatch(U8 *src,U8 *find) {
    U8 *orig=find;
    while(*src) {
        if(!*find) return src-StrLen(orig);
        if(ToUpper(*src)==ToUpper(*find)) find++;
        else find=orig;
        src++;
    }
    return NULL;
}
I64 StrOcc(U8 *src, I64 ch)
{//Count occurrences of a char.
  I64 i=0;
  if (!src) return 0;
  while (*src)
    if (*src++==ch)
      i++;
  return i;
}
U8 *StrFirstOcc(U8 *src,U8 *marker)
{//Point to 1st occurrence of marker set in str.
  I64 ch;
  while ((ch=*src++) && !StrOcc(marker,ch));
  if (ch)
    return src-1;
  else
    return NULL;
}
U8 *StrLastOcc(U8 *str,U8 *marker) {
  I64 ch,cnt;
  if(*str==0) return NULL;
  cnt=StrLen(str);
  while((ch=str[--cnt])&&!StrOcc(marker,ch)) {
     if(cnt==0) break;
  }
  if(ch) return &str[cnt];
  return NULL;
}
