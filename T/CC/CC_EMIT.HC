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
"int CCItoa(int cc_v)\n"
"{\n"
"  if (cc_v<0)\n"
"  {\n"
"    putchar(45);\n"
"    cc_v=-cc_v;\n"
"  }\n"
"  if (cc_v>=10)\n"
"    CCItoa(cc_v/10);\n"
"  putchar(48+cc_v-(cc_v/10*10));\n"
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
"}\n";
U0 CCByte(CCCtrl *cc,I64 b)

{
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
  return cc;			//CC_B_CTRL: the cc block itself
}

/* Emit the 8-byte absolute `addr` and record the site so a dumped
 * object can rebase it: the delta from that base is computed HERE as
 * plain I64 arithmetic, because mixing `&member - ptr` in one caller
 * expression misparses in this HolyC build (probe: `&x.b-p` yields 1).
 * Caller passes exactly the operand it would pass to CCQuad, after
 * emitting the MOVABS opcode bytes (0x48 + 0xB8+reg). */
U0 CCQuadAbs(CCCtrl *cc,I64 kind,I64 addr)
{
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
  CCByte(cc,0xE9);
  pos=cc->ep-cc->code;
  CCWord(cc,0);
  return pos;
}

I64 CCJz(CCCtrl *cc)
{
  I64 pos;
  CCByte(cc,0x0F);CCByte(cc,0x84);
  pos=cc->ep-cc->code;
  CCWord(cc,0);
  return pos;
}

/* Point the rel32 at pos to the current emit position. */
U0 CCPatchJmp(CCCtrl *cc,I64 pos)
{
  I64 rel=(cc->ep-cc->code)-pos-4;
  U8 *p=cc->code+pos;
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
