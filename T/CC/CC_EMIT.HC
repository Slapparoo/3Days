/* Machine code emission into the executable code buffer.
 * All emits bounds-checked; overflow is reported as an error. */

/* CC compiles its own tiny C runtime, prepended to every module, so
 * printf call sites can emit CALLs into symbols that already exist.
 * Nothing here may rely on Free(). putchar appends to cc->out and
 * delegates every console write to the zero-arg HolyC CCFlush helper
 * (M6c); JIT'd code never touches the kernel directly. */
U8 *CC_PRELUDE=
"int CCOutByte(int cc_c)\n"
"{\n"
"  putchar(cc_c);\n"
"  return 0;\n"
"}\n"
"int CCStr(char *cc_s)\n"
"{\n"
"  while (*cc_s)\n"
"  {\n"
"    putchar(*cc_s);\n"
"    cc_s=cc_s+1;\n"
"  }\n"
"  return 0;\n"
"}\n"
"/* The sign is chosen by the caller and the walker NEVER negates its input:\n"
" * `cc_v=-cc_v` is a no-op for the most negative quad, so the old walker was\n"
" * handed -2^63, failed its own `cc_v>=10` test and printed ONE digit --\n"
" * 48+(-8) is `(`, so `%d` of that value printed `-(` instead of twenty\n"
" * characters (BUG-0001).  Negating the NON-negative side instead makes every\n"
" * value reach the walker negative, the one domain where -2^63 has a digit\n"
" * walk.  `/` truncates toward zero (IDIV), so `cc_v-cc_v/10*10` lies in\n"
" * [-9,0] for a negative cc_v and its digit needs no sign fix.  cc_dig below\n"
" * is the same rule with a buffer for a sink; sprintf's return value is why\n"
" * the two cannot be one function. */\n"
"int CCItoaNeg(int cc_v)\t//cc_v<=0: print its magnitude, MSB-first\n"
"{\n"
"  if (cc_v<=-10)\n"
"    CCItoaNeg(cc_v/10);\n"
"  putchar(48-(cc_v-cc_v/10*10));\n"
"  return 0;\n"
"}\n"
"int CCItoa(int cc_v)\n"
"{\n"
"  if (cc_v<0)\n"
"    putchar(45);\n"
"  else\n"
"    cc_v=-cc_v;\t//0 and positives become negative here\n"
"  CCItoaNeg(cc_v);\n"
"  return 0;\n"
"}\n"
"/* M12 pure-C stdlib half: the kernel has no equivalents for these, so
 * CC compiles them itself. Byte semantics match CC's char* model; see
 * the fixture stdlib.c for the pinned behavior. free() is deliberately
 * absent (poopalloc chunk hazard, see CC.HC CCSrc comment). */\n"
"int strcmp(char *cc_a,char *cc_b)\n"
"{\n"
"  while (*cc_a)\n"
"  {\n"
"    if (*cc_a!=*cc_b)\n"
"      return *cc_a-*cc_b;\n"
"    cc_a=cc_a+1;\n"
"    cc_b=cc_b+1;\n"
"  }\n"
"  if (*cc_b)\n"
"    return -1;\n"
"  return 0;\n"
"}\n"
"char *strcpy(char *cc_d,char *cc_s)\n"
"{\n"
"  char *cc_r;\n"
"  cc_r=cc_d;\n"
"  while (*cc_s)\n"
"  {\n"
"    *cc_d=*cc_s;\n"
"    cc_d=cc_d+1;\n"
"    cc_s=cc_s+1;\n"
"  }\n"
"  *cc_d=0;\n"
"  return cc_r;\n"
"}\n"
"char *strchr(char *cc_s,int cc_c)\n"
"{\n"
"  while (*cc_s)\n"
"  {\n"
"    if (*cc_s==cc_c)\n"
"      return cc_s;\n"
"    cc_s=cc_s+1;\n"
"  }\n"
"  if (cc_c==0)\n"
"    return cc_s;\n"
"  return 0;\n"
"}\n"
"char *memmove(char *cc_d,char *cc_s,int cc_n)\n"
"{\n"
"  char *cc_r;\n"
"  cc_r=cc_d;\n"
"  if (cc_d<cc_s)\n"
"  {\n"
"    while (cc_n)\n"
"    {\n"
"      *cc_d=*cc_s;\n"
"      cc_d=cc_d+1;\n"
"      cc_s=cc_s+1;\n"
"      cc_n=cc_n-1;\n"
"    }\n"
"    return cc_r;\n"
"  }\n"
"  cc_d=cc_d+cc_n-1;\n"
"  cc_s=cc_s+cc_n-1;\n"
"  while (cc_n)\n"
"  {\n"
"    *cc_d=*cc_s;\n"
"    cc_d=cc_d-1;\n"
"    cc_s=cc_s-1;\n"
"    cc_n=cc_n-1;\n"
"  }\n"
"  return cc_r;\n"
"}\n"
"int cc_rseed;\n"
"int srand(int cc_s)\n"
"{\n"
"  cc_rseed=cc_s;\n"
"  return 0;\n"
"}\n"
"int rand()\n"
"{\n"
"  cc_rseed=cc_rseed*1103515245+12345;\n"
"  if (cc_rseed<0)\n"
"    cc_rseed=-cc_rseed;\n"
"  cc_rseed=cc_rseed-cc_rseed/1073741824*1073741824;\n"
"  return cc_rseed;\n"
"}\n"
"int atoi(char *cc_s)\n"
"{\n"
"  int cc_neg,cc_v;\n"
"  cc_neg=0;\n"
"  cc_v=0;\n"
"  if (*cc_s==45)\n"
"  {\n"
"    cc_neg=1;\n"
"    cc_s=cc_s+1;\n"
"  }\n"
"  while (*cc_s)\n"
"  {\n"
"    if ((*cc_s<48)||(*cc_s>57))\n"
"      break;\n"
"    cc_v=cc_v*10+*cc_s-48;\n"
"    cc_s=cc_s+1;\n"
"  }\n"
"  if (cc_neg)\n"
"    return -cc_v;\n"
"  return cc_v;\n"
"}\n"
"int abs(int cc_x)\n"
"{\n"
"  if (cc_x<0)\n"
"    return -cc_x;\n"
"  return cc_x;\n"
"}\n"
"int puts(char *cc_s)\n"
"{\n"
"  while (*cc_s)\n"
"  {\n"
"    putchar(*cc_s);\n"
"    cc_s=cc_s+1;\n"
"  }\n"
"  putchar(10);\n"
"  return 0;\n"
"}\n"
"/* M12.1 completion batch: the remaining string/file surface that is\n"
" * pure C (kernel has no equivalent or the semantics are C-specific),\n"
" * the stdio stream-token wrappers that ignore the FILE* argument (the\n"
" * integer stream slots of a buffered stdio still to come), and the\n"
" * sprintf engine. cc_sprintf takes 6 args — the parser pads user calls\n"
" * (like printf it is name-dispatched); the engine interprets the format\n"
" * AT RUNTIME, so a sprintf format may be a variable. Conversions:\n"
" * %d %s %c %x %%; at most 4 conversion args; no width/precision yet.\n"
" * Writes land in the caller's buffer — the cc->out stream is untouched.\n"
" * fgetc/getc call the getchar BUILTIN (kernel GetKey: blocks until a\n"
" * console key event; returns the event, ASCII in the low 8 bits). */\n"
"char *strcat(char *cc_d,char *cc_s)\n"
"{\n"
"  char *cc_r;\n"
"  cc_r=cc_d;\n"
"  while (*cc_d)\n"
"  {\n"
"    cc_d=cc_d+1;\n"
"  }\n"
"  while (*cc_s)\n"
"  {\n"
"    *cc_d=*cc_s;\n"
"    cc_d=cc_d+1;\n"
"    cc_s=cc_s+1;\n"
"  }\n"
"  *cc_d=0;\n"
"  return cc_r;\n"
"}\n"
"char *strncat(char *cc_d,char *cc_s,int cc_n)\n"
"{\n"
"  char *cc_r;\n"
"  cc_r=cc_d;\n"
"  while (*cc_d)\n"
"  {\n"
"    cc_d=cc_d+1;\n"
"  }\n"
"  while (cc_n)\n"
"  {\n"
"    if (!*cc_s)\n"
"    {\n"
"      cc_n=0;\n"
"    }\n"
"    else\n"
"    {\n"
"      *cc_d=*cc_s;\n"
"      cc_d=cc_d+1;\n"
"      cc_s=cc_s+1;\n"
"      cc_n=cc_n-1;\n"
"    }\n"
"  }\n"
"  *cc_d=0;\n"
"  return cc_r;\n"
"}\n"
"char *strrchr(char *cc_s,int cc_c)\n"
"{\n"
"  char *cc_e;\n"
"  cc_c=cc_c&255;\n"
"  cc_e=cc_s;\n"
"  while (*cc_e)\n"
"  {\n"
"    cc_e=cc_e+1;\n"
"  }\n"
"  if (!cc_c)\n"
"  {\n"
"    return cc_e;\t//the terminator is the last NUL\n"
"  }\n"
"  while (cc_e!=cc_s)\n"
"  {\n"
"    cc_e=cc_e-1;\n"
"    if (*cc_e==cc_c)\n"
"    {\n"
"      return cc_e;\n"
"    }\n"
"  }\n"
"  return 0;\n"
"}\n"
"char *strstr(char *cc_h,char *cc_n)\n"
"{\n"
"  char *cc_a,*cc_b,*cc_s;\n"
"  int cc_m;\n"
"  cc_s=cc_h;\n"
"  while (*cc_s)\n"
"  {\n"
"    cc_a=cc_s;\n"
"    cc_b=cc_n;\n"
"    cc_m=1;\n"
"    //A mismatch must NOT be signalled with a cc_b=0 sentinel: the\n"
"    //loop test then dereferences ADDRESS ZERO, which is unmapped on\n"
"    //the host (this exact bug faulted the first M12.1 runs). Instead\n"
"    //clear the flag and keep walking cc_b to the needle's own NUL.\n"
"    while (*cc_b)\n"
"    {\n"
"      if (*cc_a!=*cc_b)\n"
"      {\n"
"        cc_m=0;\n"
"        cc_b=cc_b+1;\n"
"      }\n"
"      else\n"
"      {\n"
"        if (cc_m)\n"
"        {\n"
"          cc_a=cc_a+1;\n"
"        }\n"
"        cc_b=cc_b+1;\n"
"      }\n"
"    }\n"
"    if (cc_m)\t//needle exhausted with no mismatch -> match at cc_s\n"
"    {\n"
"      return cc_s;\n"
"    }\n"
"    cc_s=cc_s+1;\n"
"  }\n"
"  if (!*cc_n)\t//empty needle matches at the start\n"
"  {\n"
"    return cc_h;\n"
"  }\n"
"  return 0;\n"
"}\n"
"int strncmp(char *cc_a,char *cc_b,int cc_n)\n"
"{\n"
"  while (cc_n)\n"
"  {\n"
"    if (*cc_a!=*cc_b)\n"
"    {\n"
"      return *cc_a-*cc_b;\n"
"    }\n"
"    if (!*cc_a)\n"
"    {\n"
"      return 0;\n"
"    }\n"
"    cc_a=cc_a+1;\n"
"    cc_b=cc_b+1;\n"
"    cc_n=cc_n-1;\n"
"  }\n"
"  return 0;\n"
"}\n"
"char *strncpy(char *cc_d,char *cc_s,int cc_n)\n"
"{\n"
"  char *cc_r;\n"
"  cc_r=cc_d;\n"
"  while (cc_n)\t//C semantics: zero-pad, no terminator when full\n"
"  {\n"
"    if (*cc_s)\n"
"    {\n"
"      *cc_d=*cc_s;\n"
"      cc_s=cc_s+1;\n"
"    }\n"
"    else\n"
"    {\n"
"      *cc_d=0;\n"
"    }\n"
"    cc_d=cc_d+1;\n"
"    cc_n=cc_n-1;\n"
"  }\n"
"  return cc_r;\n"
"}\n"
"char *memchr(char *cc_p,int cc_c,int cc_n)\n"
"{\n"
"  cc_c=cc_c&255;\n"
"  while (cc_n)\n"
"  {\n"
"    if (*cc_p==cc_c)\n"
"    {\n"
"      return cc_p;\n"
"    }\n"
"    cc_p=cc_p+1;\n"
"    cc_n=cc_n-1;\n"
"  }\n"
"  return 0;\n"
"}\n"
"char *strdup(char *cc_s)\n"
"{\n"
"  char *cc_d;\n"
"  int cc_i;\n"
"  cc_d=malloc(strlen(cc_s)+1);\t//builtins dispatch from prelude too\n"
"  if (cc_d)\n"
"  {\n"
"    cc_i=0;\n"
"    while (cc_s[cc_i])\n"
"    {\n"
"      cc_d[cc_i]=cc_s[cc_i];\n"
"      cc_i=cc_i+1;\n"
"    }\n"
"    cc_d[cc_i]=0;\n"
"  }\n"
"  return cc_d;\n"
"}\n"
"char *calloc(int cc_n,int cc_m)\n"
"{\n"
"  char *cc_p;\n"
"  int cc_t,cc_i;\n"
"  cc_t=cc_n*cc_m;\n"
"  cc_p=malloc(cc_t);\n"
"  if (!cc_p)\n"
"  {\n"
"    return 0;\n"
"  }\n"
"  cc_i=0;\n"
"  while (cc_i!=cc_t)\t//MAlloc does not zero-fill; C calloc must\n"
"  {\n"
"    cc_p[cc_i]=0;\n"
"    cc_i=cc_i+1;\n"
"  }\n"
"  return cc_p;\n"
"}\n"
"int fputc(int cc_c,int cc_st)\n"
"{\n"
"  putchar(cc_c);\t//stream tokens (0/1/2...) are accepted and ignored:\n"
"  return cc_c;\t//cc->out IS the stdio buffer; see PLAN limitations\n"
"}\n"
"int putc(int cc_c,int cc_st)\n"
"{\n"
"  return fputc(cc_c,cc_st);\n"
"}\n"
"int fputs(char *cc_s,int cc_st)\n"
"{\n"
"  while (*cc_s)\n"
"  {\n"
"    putchar(*cc_s);\n"
"    cc_s=cc_s+1;\n"
"  }\n"
"  return 0;\n"
"}\n"
"int fgetc(int cc_st)\n"
"{\n"
"  return getchar();\n"
"}\n"
"int getc(int cc_st)\n"
"{\n"
"  return getchar();\n"
"}\n"
"/* cc_dig takes cc_v <= 0 and writes its magnitude, MSB-first, returning the\n"
" * digit count -- the caller has already written the sign, exactly as for\n"
" * CCItoaNeg (see the note there for why the walker must not negate). */\n"
"int cc_dig(char *cc_d,int cc_v)\n"
"{\n"
"  int cc_n;\n"
"  if (cc_v<=-10)\n"
"  {\n"
"    cc_n=cc_dig(cc_d,cc_v/10);\n"
"  }\n"
"  else\n"
"  {\n"
"    cc_n=0;\n"
"  }\n"
"  cc_d[cc_n]=48-(cc_v-cc_v/10*10);\n"
"  return cc_n+1;\n"
"}\n"
"/* C's %x: the QUAD'S UNSIGNED READING, MSB-first, no leading zeros.  Nibbles\n"
" * come from a shift and a mask, so no division is needed and it does not\n"
" * matter that lite's >> is arithmetic -- the mask discards whatever filled\n"
" * the top.  The old version divided by 16 AFTER NEGATING, so it printed the\n"
" * MAGNITUDE: `1` for -1 (C and the kernel both print sixteen f's) and a\n"
" * single `0` for -2^63, which has no positive magnitude to print. */\n"
"int cc_hexd(char *cc_d,int cc_v)\n"
"{\n"
"  int cc_n,cc_t,cc_i,cc_f;\n"
"  cc_n=0;\n"
"  cc_f=0;\t\t//seen a non-zero nibble: everything after it prints\n"
"  cc_i=60;\t\t//top nibble of a quad\n"
"  while (cc_i>=0)\n"
"  {\n"
"    cc_t=(cc_v>>cc_i)&15;\n"
"    if (cc_t)\n"
"      cc_f=1;\n"
"    if (cc_f)\n"
"    {\n"
"      if (cc_t<10)\n"
"        cc_d[cc_n]=48+cc_t;\n"
"      else\n"
"        cc_d[cc_n]=87+cc_t;\n"
"      cc_n=cc_n+1;\n"
"    }\n"
"    cc_i=cc_i-4;\n"
"  }\n"
"  if (!cc_f)\n"
"  {\n"
"    cc_d[0]=48;\t//zero prints one digit, like C\n"
"    cc_n=1;\n"
"  }\n"
"  return cc_n;\n"
"}\n"
"int cc_sprintf(char *cc_d,char *cc_f,int cc_a1,int cc_a2,int cc_a3,int cc_a4)\n"
"{\n"
"  char *cc_b,*cc_s;\n"
"  int cc_k,cc_v,cc_n,cc_w;\n"
"  cc_b=cc_d;\n"
"  cc_k=0;\n"
"  cc_w=0;\t\t//written-char count; NEVER trust pointer subtraction\n"
"  if (!cc_f)\t//forgive sprintf(dst) — a NULL format yields \"\"\n"
"  {\n"
"    return 0;\n"
"  }\n"
"  while (*cc_f)\n"
"  {\n"
"    if (*cc_f!=37)\t//37 is '%' — numeric literals only in the prelude\n"
"    {\n"
"      *cc_b=*cc_f;\n"
"      cc_b=cc_b+1;\n"
"      cc_f=cc_f+1;\n"
"      cc_w=cc_w+1;\n"
"    }\n"
"    else\n"
"    {\n"
"      cc_f=cc_f+1;\n"
"      if (*cc_f==37)\n"
"      {\n"
"        *cc_b=37;\n"
"        cc_b=cc_b+1;\n"
"        cc_f=cc_f+1;\n"
"        cc_w=cc_w+1;\n"
"      }\n"
"      else\n"
"      {\n"
"        if (cc_k==0)\n"
"        {\n"
"          cc_v=cc_a1;\n"
"        }\n"
"        if (cc_k==1)\n"
"        {\n"
"          cc_v=cc_a2;\n"
"        }\n"
"        if (cc_k==2)\n"
"        {\n"
"          cc_v=cc_a3;\n"
"        }\n"
"        if (cc_k>=3)\n"
"        {\n"
"          cc_v=cc_a4;\t//C's UB on too few args: extra %s get 0 -> skipped\n"
"        }\n"
"        cc_k=cc_k+1;\n"
"        if (*cc_f==100)\t//d\n"
"        {\n"
"          cc_f=cc_f+1;\n"
"          if (cc_v<0)\n"
"          {\n"
"            *cc_b=45;\n"
"            cc_b=cc_b+1;\n"
"            cc_w=cc_w+1;\n"
"          }\n"
"          else\n"
"          {\n"
"            cc_v=-cc_v;\t//into cc_dig's domain\n"
"          }\n"
"          cc_n=cc_dig(cc_b,cc_v);\n"
"          cc_b=cc_b+cc_n;\n"
"          cc_w=cc_w+cc_n;\n"
"        }\n"
"        else if (*cc_f==120)\t//x: the QUAD'S UNSIGNED READING, which is\n"
"        {\n"
"          cc_f=cc_f+1;\t//C's %x for a 64-bit int.  The negate that used\n"
"          cc_n=cc_hexd(cc_b,cc_v);\t//to sit here printed the MAGNITUDE.\n"
"          cc_b=cc_b+cc_n;\n"
"          cc_w=cc_w+cc_n;\n"
"        }\n"
"        else if (*cc_f==99)\t//c\n"
"        {\n"
"          cc_f=cc_f+1;\n"
"          *cc_b=cc_v;\t//byte store truncates: C's (unsigned char) cast\n"
"          cc_b=cc_b+1;\n"
"          cc_w=cc_w+1;\n"
"        }\n"
"        else if (*cc_f==115)\t//s\n"
"        {\n"
"          cc_f=cc_f+1;\n"
"          cc_s=cc_v;\t//a pointer arrives as an int slot value\n"
"          while (*cc_s)\n"
"          {\n"
"            *cc_b=*cc_s;\n"
"            cc_b=cc_b+1;\n"
"            cc_s=cc_s+1;\n"
"            cc_w=cc_w+1;\n"
"          }\n"
"        }\n"
"      }\n"
"    }\n"
"  }\n"
"  *cc_b=0;\n"
"  return cc_w;\n"
"}\n";
U0 CCByte(CCCtrl *cc,I64 b)

{
  if (cc->noemit)		//M15: sizeof-type analysis emits nothing
    return;
  if (cc->ep>=cc->code+CC_CODE_SIZE)
  {
    CCError(cc,"code buffer overflow");
    return;
  }
  *cc->ep++=b;
}

U0 CCHalf(CCCtrl *cc,I64 h)
{
  CCByte(cc,h);
  CCByte(cc,h>>8);
}

U0 CCWord(CCCtrl *cc,I64 w)
{
  CCByte(cc,w);
  CCByte(cc,w>>8);
  CCByte(cc,w>>16);
  CCByte(cc,w>>24);
}

U0 CCQuad(CCCtrl *cc,I64 q)
{
  CCWord(cc,q);
  CCWord(cc,q>>32);
}

/* movabs rax,imm64 */
U0 CCMovImmRax(CCCtrl *cc,I64 val)
{
  CCByte(cc,0x48);
  CCByte(cc,0xB8);
  CCQuad(cc,val);
}

/* Current address of an external base. Reloading rebases these from
 * the freshly MAlloced arenas (CCSave/CCLoad, M10). */
I64 CCBase(CCCtrl *cc,I64 kind)
{
  if (kind==CC_B_LOCALS)
    return cc->locals;
  if (kind==CC_B_STACK)
    return cc->stack;
  if (kind==CC_B_SAVE)
    return cc->save;
  if (kind==CC_B_OUT)
    return cc->out;
  if (kind==CC_B_HFLUSH)
    return cc->hflush;
  if (kind==CC_B_LIB)
    return cc->lib;		//delta = 8*slot into the helper table
  if (kind==CC_B_CODE)
    return cc->code;		//M18: function-address values
  return cc;			//CC_B_CTRL: the cc block itself
}

/* M24: write a POINTER value into the arena at byte offset off and
 * record the relocation that lets a saved object re-derive it. The
 * arena moves on every load exactly like the code buffer, so a compile
 * time absolute stored here is otherwise stale; delta is measured from
 * the same CCBase() the code fixups use. */
U0 CCArenaPtr(CCCtrl *cc,I64 off,I64 kind,I64 addr)
{
  I64 v;
  U8 *p;

  v=addr;
  p=cc->locals+off;
  p[0]=v;
  p[1]=v>>8;
  p[2]=v>>16;
  p[3]=v>>24;
  p[4]=v>>32;
  p[5]=v>>40;
  p[6]=v>>48;
  p[7]=v>>56;
  if (cc->nafx>=CC_FIXUPS)
    CCError(cc,"too many arena relocations");
  else
  {
    cc->afx_pos[cc->nafx]=off;
    cc->afx_kind[cc->nafx]=kind;
    cc->afx_delta[cc->nafx]=addr-CCBase(cc,kind);
    cc->nafx++;
  }
}

/* Emit the 8-byte absolute `addr` and record the site so a dumped
 * object can rebase it: the delta from that base is computed HERE as
 * plain I64 arithmetic, because mixing `&member - ptr` in one caller
 * expression misparses in this HolyC build (probe: `&x.b-p` yields 1).
 * Caller passes exactly the operand it would pass to CCQuad, after
 * emitting the MOVABS opcode bytes (0x48 + 0xB8+reg). */
U0 CCQuadAbs(CCCtrl *cc,I64 kind,I64 addr)
{
  //noemit also suppresses the fixup RECORD: a recorded position during
  //frozen-pointer parsing would relocate bytes of LIVE code at link.
  if (cc->noemit)		//M15
    return;
  CCQuad(cc,addr);
  if (cc->nfix>=CC_FIXUPS)
    CCError(cc,"too many absolute-address sites");
  else
  {
    cc->fix_pos[cc->nfix]=cc->ep-cc->code-8;
    cc->fix_kind[cc->nfix]=kind;
    cc->fix_delta[cc->nfix]=addr-CCBase(cc,kind);
    cc->nfix++;
  }
}

/* Function entry stub.
 * A HolyC function-pointer call enters with RSP pointing at the return
 * address and the slots below it are NOT mapped (verified: reading
 * [rsp-8] faults), so we must not PUSH before relocating the stack.
 * Sequence: save caller RSP into cc->save, switch RSP to our private
 * stack, then the usual frame.
 *   MOVABS RCX,save; MOV [RCX],RSP; MOVABS RSP,stack_top;
 *   PUSH RBP; MOV RBP,RSP
 * If entry_inject (main with parameters): the harness passed argc/argv
 * through the cc block, so store them into the parameter slots the body
 * reads ([RBP+16+8*(n-1-i)]) — C code sees them as ordinary params. */
U0 CCPrologue(CCCtrl *cc,I64 entry)
{
  I64 n=cc->nparam;
  if (entry)
  {
    CCByte(cc,0x48);CCByte(cc,0xB9);	//MOVABS RCX,save
    CCQuadAbs(cc,CC_B_SAVE,cc->save);
    CCByte(cc,0x48);CCByte(cc,0x89);CCByte(cc,0x21);	//MOV [RCX],RSP
    CCByte(cc,0x55);			//PUSH RBP — stash the CALLER's
    //frame pointer on the host stack (at [cc->save]-8): the only way a
    //return-to-harness jump made from a NESTED C function (abort/exit
    //below main) can restore RBP to a host-valid value. The HolyC caller
    //rebuilds its own frame from RBP; returning with a code-stack RBP
    //made its epilogue ret through garbage and jump to address 0.
    CCByte(cc,0x48);CCByte(cc,0xBC);	//MOVABS RSP,
    CCQuadAbs(cc,CC_B_STACK,cc->stack+CC_STACK_SIZE-0x100);
  }
  CCByte(cc,0x55);			//PUSH RBP
  CCByte(cc,0x48);CCByte(cc,0x89);CCByte(cc,0xE5);	//MOV RBP,RSP
  if ((entry)&&(cc->entry_inject))
  {
    if (cc->pty[0]==TY_INT)
    {
      CCByte(cc,0x48);CCByte(cc,0xB9);	//MOVABS RCX,&cc->argc
      CCQuadAbs(cc,CC_B_CTRL,&cc->argc);
      CCByte(cc,0x48);CCByte(cc,0x8B);CCByte(cc,0x01);	//MOV RAX,[RCX]
      CCByte(cc,0x48);CCByte(cc,0x89);CCByte(cc,0x85);	//MOV [RBP+d32],RAX
      CCWord(cc,16+8*(n-1));
    }
    if (n>=2)
    {
      if ((cc->pty[1]==TY_PTR)||(cc->pty[1]==TY_CHARP))
      {
        CCByte(cc,0x48);CCByte(cc,0xB9);	//MOVABS RCX,&cc->argv
        CCQuadAbs(cc,CC_B_CTRL,&cc->argv);
        CCByte(cc,0x48);CCByte(cc,0x8B);CCByte(cc,0x01);	//MOV RAX,[RCX]
        CCByte(cc,0x48);CCByte(cc,0x89);CCByte(cc,0x85);
        CCWord(cc,16+8*(n-2));
      }
    }
  }
}

/* Matching return. Entry also restores the caller (HolyC) RSP. */
U0 CCFunRet(CCCtrl *cc,I64 entry)
{
  CCByte(cc,0xC9);			//LEAVE
  if (entry)
  {
    CCByte(cc,0x48);CCByte(cc,0xB9);	//MOVABS RCX,
    CCQuadAbs(cc,CC_B_SAVE,cc->save);
    CCByte(cc,0x48);CCByte(cc,0x8B);CCByte(cc,0x21);	//MOV RSP,[RCX]
    CCByte(cc,0x48);CCByte(cc,0x8B);CCByte(cc,0x6C);	//MOV RBP,[RSP-8]
    CCByte(cc,0x24);CCByte(cc,0xF8);
    //The prologue stashed the caller's RBP in the host quad just BELOW
    //the saved RSP (PUSH RBP right after saving it). LOAD it, do not
    //POP: the quad at the restored RSP is the return address itself —
    //popping it as RBP made RET jump back to the entry address and the
    //program ran forever (the abort-in-helper hang). The HolyC caller
    //rebuilds its frame from RBP, so a nested-function return jump must
    //leave a HOST-VALID RBP here; a code-stack RBP made its epilogue
    //ret through garbage and jump to address 0.
  }
  CCByte(cc,0xC3);			//RET
}

/* Point rel32 at pos at code offset target_off. */
U0 CCPatchCallPos(CCCtrl *cc,I64 pos,I64 target_off)
{
  I64 rel=target_off-pos-4;
  U8 *p=cc->code+pos;

  p[0]=rel;
  p[1]=rel>>8;
  p[2]=rel>>16;
  p[3]=rel>>24;
}

/* CALL rel32. If the target is defined yet, patch immediately;
 * otherwise queue the site on the symbol for its definition. */
U0 CCCall(CCCtrl *cc,CSym *f)
{
  I64 pos;

  if (cc->noemit)		//M15: sizeof(f(x)) needs only the type;
    return;			//skipping also keeps the patch queue clean
  CCByte(cc,0xE8);
  pos=cc->ep-cc->code;
  CCWord(cc,0);
  if (f->offset>=0)
    CCPatchCallPos(cc,pos,f->offset);
  else
  {
    if (f->npatch>=CC_PATCHES)
      CCError(cc,"too many forward call sites for one function");
    else
      f->patches[f->npatch++]=pos;
    //CCParseFun resolves the queue when the definition arrives.
  }
}

/* unsigned-below (jump if CF) and not-equal (jump if ZF=0), rel32.
 * Needed by CCEmitPutchar's flush conditionals, which precede the
 * generic jump helpers below (same-unit order matters). */
I64 CCJb(CCCtrl *cc)
{
  I64 pos;
  CCByte(cc,0x0F);CCByte(cc,0x82);
  pos=cc->ep-cc->code;
  CCWord(cc,0);
  return pos;
}

I64 CCJnz(CCCtrl *cc)
{
  I64 pos;
  CCByte(cc,0x0F);CCByte(cc,0x85);
  pos=cc->ep-cc->code;
  CCWord(cc,0);
  return pos;
}

/* Load RDI with the ctrl scratch quads (reloc base CTRL, rebases with
 * the object load). Any CALL into a JIT-compiled HolyC function must be
 * preceded by this: its frame-zeroing prologue REP STOSQs 8*RCX bytes at
 * the incoming RDI (CC.HH libscr comment). Harmless otherwise visible
 * only as random data loss — RDI from C code is whatever the last
 * expression left there, often a string-pool pointer. */
U0 CCSetScratch(CCCtrl *cc)
{
  CCByte(cc,0x48);CCByte(cc,0xBF);	//MOVABS RDI,&cc->libscr[0]
  CCQuadAbs(cc,CC_B_CTRL,cc->libscr);
}

/* Zero-arg CALL to the HolyC flush helper, preserving RAX: putchar
 * must return its argument, and RAX is the only live register across
 * this call (every C live value is RBP-anchored, and the helper call
 * is a zero-arg activation — the form verified safe everywhere). The
 * helper streams cc->out to the console through the kernel and zeroes
 * its count; callers reload what they need from memory. */
U0 CCFlushCall(CCCtrl *cc)
{
  CCByte(cc,0x50);			//PUSH RAX
  CCSetScratch(cc);			//CCFlush stosqs 128B at RDI: hand it
  CCByte(cc,0x48);CCByte(cc,0xB9);	//MOVABS RCX,hflush
  CCQuadAbs(cc,CC_B_HFLUSH,cc->hflush);
  CCByte(cc,0xFF);CCByte(cc,0xD1);	//CALL RCX
  CCByte(cc,0x58);			//POP RAX
}

/* putchar(c): append AL to cc->out (bytes from offset 8; the 8-byte LE
 * counter lives at cc->out[0..7]), then stream: flush when the char is
 * '\n' (line-buffered, C semantics) or when the buffer is within 64
 * bytes of full (index bounded; the old AND-wrap silently dropped
 * bytes past 64K). All OS contact is delegated to the zero-arg HolyC
 * CCFlush helper — generated code never touches the kernel itself.
 * RAX (the char) survives — C putchar returns its argument. */
U0 CCEmitPutchar(CCCtrl *cc)
{
  I64 pos;

  //Only REX.W forms on legacy-encodable regs: an extended reg field
  //needs REX.R and an SIB index needs REX.X, and REX.W alone makes
  //field 000 mean RAX, not R8 (that bug silently zeroed the buffer).
  CCByte(cc,0x48);CCByte(cc,0xB9);		//MOVABS RCX,out+8
  CCQuadAbs(cc,CC_B_OUT,cc->out+8);
  CCByte(cc,0x48);CCByte(cc,0xBE);		//MOVABS RSI,out (count slot)
  CCQuadAbs(cc,CC_B_OUT,cc->out);
  CCByte(cc,0x48);CCByte(cc,0x8B);CCByte(cc,0x16);	//MOV RDX,[RSI]
  CCByte(cc,0x48);CCByte(cc,0x81);CCByte(cc,0xFA);	//CMP RDX,imm32
  CCWord(cc,CC_OUT_SIZE-64);
  pos=CCJb(cc);				//below the cap -> store
  CCFlushCall(cc);
  //After a full-buffer flush the count is zero and the helper has
  //clobbered our scratch: reload all three pointers fresh.
  CCByte(cc,0x48);CCByte(cc,0xB9);		//MOVABS RCX,out+8
  CCQuadAbs(cc,CC_B_OUT,cc->out+8);
  CCByte(cc,0x48);CCByte(cc,0xBE);		//MOVABS RSI,out
  CCQuadAbs(cc,CC_B_OUT,cc->out);
  CCByte(cc,0x48);CCByte(cc,0x8B);CCByte(cc,0x16);	//MOV RDX,[RSI]
  CCPatchJmp(cc,pos);
  CCByte(cc,0x88);CCByte(cc,0x04);CCByte(cc,0x11);	//MOV [RCX+RDX],AL (no REX)
  CCByte(cc,0x48);CCByte(cc,0xFF);CCByte(cc,0xC2);	//INC RDX
  CCByte(cc,0x48);CCByte(cc,0x89);CCByte(cc,0x16);	//MOV [RSI],RDX
  CCByte(cc,0x48);CCByte(cc,0x3D);CCWord(cc,10);	//CMP RAX,10 ('\n'?)
  pos=CCJnz(cc);
  CCFlushCall(cc);
  CCPatchJmp(cc,pos);
}

/* M12: stage the value in RAX into cc->ext[i] (reloc base CTRL). */
U0 CCPushExt(CCCtrl *cc,I64 i)
{
  CCByte(cc,0x48);CCByte(cc,0xB9);	//MOVABS RCX,&cc->ext[0]
  CCQuadAbs(cc,CC_B_CTRL,&cc->ext[0]);
  CCByte(cc,0x48);CCByte(cc,0x89);CCByte(cc,0x81);	//MOV [RCX+d32],RAX
  CCWord(cc,8*i);
}

/* M12: zero-arg call to the stdlib helper in cc->lib[slot]. Reloc base
 * LIB is the TABLE (not the pointer value, unlike hflush's own base), so
 * the patched quad names the SLOT and the code must LOAD through it:
 * MOVABS RAX,&lib[slot]; CALL [RAX]. That indirection is a gift: a
 * reloaded object picks up whatever helpers the loading session's
 * CCSetLib installed, with zero object-format involvement.
 * The helper is zero-arg HolyC: it reads cc->ext[], calls the kernel,
 * and returns the value in RAX. Nothing else needs saving: RAX is the
 * result by definition, and every other live generated-code value is
 * RBP-anchored (M11 analysis; the M6c flush call proved the same
 * zero-arg shape end to end). */
U0 CCEmitLibCall(CCCtrl *cc,I64 slot)
{
  CCSetScratch(cc);			//callee stosqs at RDI - hand it ours
  CCByte(cc,0x48);CCByte(cc,0xB8);	//MOVABS RAX,&lib[slot]
  CCQuadAbs(cc,CC_B_LIB,cc->lib+slot);	//POINTER math: +1 is one quad.
  CCByte(cc,0xFF);CCByte(cc,0x10);	//CALL QWORD PTR [RAX]
}

/* M35: convert the value in RAX through a single-operand helper (CC_LIB_I2D or
 * CC_LIB_D2I).  The helper READS cc->ext[0] and answers in RAX: it writes no
 * ext slot, which is what makes it safe to run one conversion while another
 * operand is already staged in ext[1] (see CCFloatBin). */
U0 CCConvRax(CCCtrl *cc,I64 slot)
{
  CCPushExt(cc,0);
  CCEmitLibCall(cc,slot);
}

/* M35: a delegated float binary operator.  Invariant shared with CCBinEmit: the
 * left value is on the machine stack, the right value is in RAX.  `lconv`/
 * `rconv` are the conversion each operand still needs to reach the operation's
 * operand type (-1 = none): C's usual arithmetic conversions turn `i + d` into
 * "convert i, then add doubles", and they are applied while the value they
 * describe is still in hand — the right one while it is in RAX, the left one
 * immediately after it is popped back.  Then both quads sit in ext[] and the
 * HolyC helper for this operation computes the result into RAX.
 *
 * Order matters and is load-bearing: stage the RIGHT operand first (ext[1]),
 * because converting the left one goes through ext[0] as scratch, so the right
 * operand must already be somewhere else. */
U0 CCFloatBin(CCCtrl *cc,I64 slot,I64 lconv,I64 rconv)
{
  if (rconv>=0)
    CCConvRax(cc,rconv);
  CCPushExt(cc,1);			//right operand, converted
  CCByte(cc,0x58);			//POP RAX (left operand) — 0x58, not
  					//0x59: 0x59 is POP RCX, which is what
  					//I first wrote here, and it left the
  					//left operand in a register the next
  					//CCPushExt was about to overwrite.
  if (lconv>=0)
    CCConvRax(cc,lconv);
  CCPushExt(cc,0);			//left operand, converted
  CCEmitLibCall(cc,slot);
  cc->is_addr=0;
  cc->aty=0;
  cc->sty=-1;
  cc->celem=0;		//M29 discipline: an arithmetic result states its
  cc->celebsty=-1;	//own shape, so a literal parsed next cannot
}			//inherit this operand's element marker

/* M35: the one-operand form (unary minus). */
U0 CCFloatNeg(CCCtrl *cc,I64 conv)
{
  if (conv>=0)
    CCConvRax(cc,conv);
  CCPushExt(cc,0);
  CCEmitLibCall(cc,CC_LIB_FNEG);
  cc->is_addr=0;
  cc->aty=0;
  cc->sty=-1;
  cc->celem=0;
  cc->celebsty=-1;
}

/* ADD RSP,8n (caller pops args; n<=15 fits the imm8 form). */
U0 CCPopArgs(CCCtrl *cc,I64 n)
{
  if (!n)
  {
    //Nothing to pop.
  }
  else if (n<=15)
  {
    CCByte(cc,0x48);CCByte(cc,0x83);CCByte(cc,0xC4);
    CCByte(cc,8*n);
  }
  else
  {
    CCError(cc,"too many args to pop");
  }
}

/* test RAX,RAX */
U0 CCTestRax(CCCtrl *cc)
{
  CCByte(cc,0x48);CCByte(cc,0x85);CCByte(cc,0xC0);
}

/* setcc AL; movzx EAX,AL  — materialize a condition as 0/1 in RAX. */
U0 CCSetccRax(CCCtrl *cc,I64 op)
{
  CCByte(cc,0x0F);CCByte(cc,op);CCByte(cc,0xC0);
  CCByte(cc,0x0F);CCByte(cc,0xB6);CCByte(cc,0xC0);
}

/* Forward jumps with rel32 operands; caller patches later.
 * These return the operand position (for CCPatchJmp). */
I64 CCJmp(CCCtrl *cc)
{
  I64 pos;
  if (cc->noemit)		//M15: no patch slot in silent mode
    return -1;
  CCByte(cc,0xE9);
  pos=cc->ep-cc->code;
  CCWord(cc,0);
  return pos;
}

I64 CCJz(CCCtrl *cc)
{
  I64 pos;
  if (cc->noemit)		//M15: no patch slot in silent mode
    return -1;
  CCByte(cc,0x0F);CCByte(cc,0x84);
  pos=cc->ep-cc->code;
  CCWord(cc,0);
  return pos;
}

/* Point the rel32 at pos to the current emit position. */
U0 CCPatchJmp(CCCtrl *cc,I64 pos)
{
  I64 rel;
  U8 *p;
  if (pos<0)			//M15: silent-mode placeholder
    return;
  rel=(cc->ep-cc->code)-pos-4;
  p=cc->code+pos;
  p[0]=rel;
  p[1]=rel>>8;
  p[2]=rel>>16;
  p[3]=rel>>24;
}

U0 CCPatchImm32(CCCtrl *cc,I64 pos,I64 val)
{
  U8 *p=cc->code+pos;
  p[0]=val;
  p[1]=val>>8;
  p[2]=val>>16;
  p[3]=val>>24;
}
