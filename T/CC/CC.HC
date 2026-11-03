/* CC driver: umbrella include + read/compile/execute.
 * Usage from a command-line script (run from repo root):
 *   #include "T:/CC/CC.HC"
 *   "rc=%d\n",CCRunFile("T:/CC/TEST/ret42.c");
 *   Exit;
 */
#include "T:/CC/CC.HH"
#include "T:/CC/CC_LEX.HC"
#include "T:/CC/CC_EMIT.HC"
#include "T:/CC/CC_SYM.HC"
#include "T:/CC/CC_PARSE.HC"

CCCtrl cc;

#include "T:/CC/CC_LIB.HC"	//M12 helpers reference cc directly: splice after it

U0 CCInit(CCCtrl *c)
{
  MemSet(c,0,sizeof(CCCtrl));
  //sty/psty carry tag ids; the zeroed image would alias tag 0, so the
  //"not a struct" sentinel has to be -1 from the start.
  c->sty=-1;
  c->code=MAlloc(CC_CODE_SIZE);
  c->ep=c->code;
  c->stack=MAlloc(CC_STACK_SIZE);
  c->save=MAlloc(0x40);
  c->locals=MAlloc(CC_LOCAL_SIZE);
  //C globals start zeroed.
  MemSet(c->locals,0,CC_LOCAL_SIZE);
  c->out=MAlloc(CC_OUT_SIZE);
  //out[0..7] = LE byte count, starts at 0.
  MemSet(c->out,0,CC_OUT_SIZE);
}

/* Read a T:-drive file into a NUL terminated buffer.
 * The FileRead() block is deliberately NOT freed: this port's Free() can
 * drop the mmap chunk shared with neighbouring live allocations (it munmaps
 * at chunk granularity, see poopalloc.c FreeVirtualChunk), which poisoned
 * long-lived arenas. CC leaks its small buffers by design. */
U8 *CCSrc(U8 *fn)
{
  I64 len;
  U8 *data=FileRead(fn,&len),*s;
  if (!data)
    return NULL;
  s=MAlloc(len+1);
  MemCpy(s,data,len);
  s[len]=0;
  return s;
}

/* M6c streaming: the zero-arg console flush helper. Generated C code
 * CALLs this (absolute address via CCQuadAbs base CC_B_HFLUSH, zero-arg
 * activation — the only call form the HolyC JIT handles reliably in
 * every context) after writing '\n' and when the buffer is nearly
 * full. ALL console output is delegated to HolyC's own "%s" print,
 * i.e. the kernel Print path — exactly what CCOutPrint does for the
 * harness; the JIT'd program itself never touches the OS. Zeroes the
 * buffer count so writes continue at out[8]; any bytes after the last
 * newline are still pending for CCOutPrint as usual. */
U0 CCFlush()
{
  I64 i,n;
  n=0;
  for (i=7;i>=0;i--)
    n=n*256+cc.out[i];
  if ((n<0)||(n>CC_OUT_SIZE-9))
    n=0;
  if (!n)
    return;
  cc.out[8+n]=0;
  "%s",cc.out+8;
  for (i=0;i!=8;i++)
    cc.out[i]=0;
}

/* Compile source text. TRUE on success. */
Bool CCCompile(CCCtrl *c,U8 *text)
{
  I64 entry,plen,tlen;
  U8 *full;
  CCInit(c);
  c->hflush=&CCFlush;	//M6c: generated putchar streams through this
  CCSetLib(c);		//M12: stdlib delegation helper table
  //The CC runtime prelude comes first so its symbols exist and get
  //defined before user code (printf lowering) can reference them.
  plen=StrLen(CC_PRELUDE);
  tlen=StrLen(text);
  full=MAlloc(plen+tlen+1);
  MemCpy(full,CC_PRELUDE,plen);
  MemCpy(full+plen,text,tlen);
  full[plen+tlen]=0;
  c->src=full;
  c->pc=full;
  CCLex(c);
  entry=CCParse(c);
  if (entry<0)
    return FALSE;
  return TRUE;
}

/* Print the C program's buffered output. This uses HolyC's own "%s"
 * print, i.e. the kernel Print path (blue screen + CLI), exactly like
 * a HolyC program's output — the JIT'd code itself never does I/O. */
U0 CCOutPrint(CCCtrl *c)
{
  I64 i,n;
  n=0;
  for (i=7;i>=0;i--)
    n=n*256+c->out[i];
  if ((n<0)||(n>CC_OUT_SIZE-9))
    n=0;
  c->out[8+n]=0;
  if (n)
    "%s",c->out+8;
}

/* Optional debug trail for CCFind thunks: marker stores into the
 * locals arena (free when a module has no globals/strings). Off by
 * default: helpers emit nothing, so thunks stay byte-identical. */
I64 cc_find_dbg=0;

/* CCFind thunks that take stack arguments: return epilogue variants.
 * The values are correct either way; the difference is how well the
 * activation agrees with the HolyC JIT's bookkeeping of an
 * IC_CALL_INDIRECT ("fp") call site, which in this boot image is only
 * reliable for zero-arg calls and for arg-taking calls where the
 * enclosing HolyC function RETURNS IMMEDIATELY after the call (the
 * frame absorbs the caller-side deferred pops — verified green in
 * probes and pinned by the findstub case). Otherwise (later statements
 * in the same function; statements after a top-level arg-taking call)
 * the JIT's deferred caller-side ADD RSP desyncs RSP tracking and
 * later code faults or hangs — reproduced across epilogue variants and
 * shapes, including with arg-taking native HolyC callees substituted,
 * so it is a HolyC-JIT property, not a codegen property of the thunk.
 * The robust paths are CCFindCallN (cc->ext staging, zero-arg
 * activations only) and the main-entry stubs. cc_find_argpop=1 swaps
 * the thunk epilogue to native-style RET imm16 8*argc (cc_find_popextra
 * adds bytes to that immediate for probing); 0 = plain RET, which is
 * what the suite verifies. Zero-arg thunks and main-entry stubs
 * (CCRunFile/CCRunArgv/CCRunObj) never take stack args and are
 * unaffected by all of this. */
I64 cc_find_argpop=0;

/* Experimental knob for the argpop epilogue: RET imm16 = 8*fargc + this.
 * For probing the HolyC JIT's caller-side accounting; 0 in practice. */
I64 cc_find_popextra=0;

U0 CCSetFindArgPop(I64 on)
{
  cc_find_argpop=on;
}

U0 CCSetFindDbg(I64 on)
{
  cc_find_dbg=on;
}

U0 CCDbgMark(I64 off,I64 val)
{
  if (!cc_find_dbg)
    return;
  CCByte(&cc,0x48);CCByte(&cc,0xB9);CCQuad(&cc,cc.locals);
  CCByte(&cc,0x48);CCByte(&cc,0xC7);CCByte(&cc,0x81);
  CCWord(&cc,off);
  CCWord(&cc,val);
}

/* Record the thunk's copy of the caller RSP at locals+0x30. Only valid
 * right after MOVABS RCX,save; MOV RSI,[RCX] — clobbers RCX (dead there). */
U0 CCDbgMarkRsi()
{
  if (!cc_find_dbg)
    return;
  CCByte(&cc,0x48);CCByte(&cc,0xB9);CCQuad(&cc,cc.locals);
  CCByte(&cc,0x48);CCByte(&cc,0x89);CCByte(&cc,0x71);CCByte(&cc,0x30);
}

U0 CCDbgMarkRsp(I64 off)
{
  if (!cc_find_dbg)
    return;
  CCByte(&cc,0x48);CCByte(&cc,0xB9);CCQuad(&cc,cc.locals);
  CCByte(&cc,0x48);CCByte(&cc,0x89);CCByte(&cc,0xA1);CCWord(&cc,off);
}

/* After a successful CCCompile: return a HolyC-callable address for a
 * defined function: an entry thunk (private-stack switch + frame) that
 * CALLs the body and returns its value in RAX.
 *
 * HolyC's fp-call convention (probe-verified: the args land at
 * [caller_rsp+8+8*i] after the return address, registers are clobbered
 * scratch, and the CALLER pops after RET) is the same stack convention
 * CC uses internally. So for an N-arg function the thunk reloads the
 * saved caller RSP, copies the N args onto the private stack in
 * left-to-right push order (exactly what the body's [RBP+16+...] reads
 * expect), CALLs, and pops them itself. The HolyC caller then pops
 * again from its own view of RSP, which the entry epilogue restored —
 * no double-pop, because RSP is restored from cc->save, not adjusted.
 *
 * The HolyC call site MUST pass exactly fargc args (a typed fp with
 * matching arity is the safe way); too few/many corrupts nothing on
 * our side but the body reads stale stack. NULL on error. */
U8 *CCFind(U8 *name)
{
  CSym *s=CCSymFind(&cc,name);
  I64 thunk,i;
  if ((!s)||((s->kind!=SC_FUN)||(s->offset<0)))
  {
    "CC ERROR: CCFind: no defined function '%s'\n",name;
    return NULL;
  }
  if (s->fargc>CC_MAX_PARAMS)
  {
    "CC ERROR: CCFind: too many params for '%s'\n",name;
    return NULL;
  }
  if ((cc.ep-cc.code)+128*(s->fargc+4)>CC_CODE_SIZE)
  {
    "CC ERROR: CCFind: code buffer full\n";
    return NULL;
  }
  thunk=cc.ep-cc.code;
  cc.entry_inject=0;	//args come from the caller stack, not argc/argv
  CCPrologue(&cc,TRUE);	//saves caller RSP into cc->save, switches stack
  CCDbgMark(0x38,0xAAAA);
  if (s->fargc)
  {
    CCByte(&cc,0x48);CCByte(&cc,0xB9);		//MOVABS RCX,save
    CCQuadAbs(&cc,CC_B_SAVE,cc.save);
    CCByte(&cc,0x48);CCByte(&cc,0x8B);CCByte(&cc,0x31);	//MOV RSI,[RCX]
    CCDbgMarkRsi();
    for (i=0;i!=s->fargc;i++)
    {
      //MOV RAX,[RSI+8+8*i]; PUSH RAX — left-to-right, as CCCall pushes.
      CCByte(&cc,0x48);CCByte(&cc,0x8B);CCByte(&cc,0x46);
      CCByte(&cc,8+8*i);
      CCByte(&cc,0x50);
    }
  }
  CCCall(&cc,s);
  CCDbgMark(0x50,0xBBBB);	//proof the body RETurned to the thunk
  CCPopArgs(&cc,s->fargc);	//thunk is the body's caller
  CCDbgMarkRsp(0x58);
  if (!cc_find_dbg)
  {
    if (!s->fargc)
      CCFunRet(&cc,TRUE);		//zero-arg: plain RET (C8 with imm 0 would pop an extra quad)
    else if (!cc_find_argpop)
      CCFunRet(&cc,TRUE);
    else
    {
      //Match native HolyC callees (verified from JIT bytes: every
      //HolyC-compiled callee ends POP R10;POP RDI;POP RSI;POP RBP;
      //RET imm16 8*argc). The fp-call site defers the caller-side
      //ADD RSP until the enclosing block/function epilogue, so the
      //thunk must leave the arg slots intact for statements that run
      //between the call and that flush; popping them here shifts the
      //JIT's RSP-relative addressing mid-body and derails it.
      CCByte(&cc,0xC9);
      CCByte(&cc,0x48);CCByte(&cc,0xB9);
      CCQuadAbs(&cc,CC_B_SAVE,cc.save);
      CCByte(&cc,0x48);CCByte(&cc,0x8B);CCByte(&cc,0x21);	//MOV RSP,[RCX]
      CCByte(&cc,0xC8);		//RET imm16: pop RIP, then RSP+=imm16+8
      CCHalf(&cc,8*s->fargc+cc_find_popextra);
    }
  }
  else
  {
    //CCFunRet(cc,TRUE) byte-for-byte, with a marker before the RET:
    CCByte(&cc,0xC9);
    CCByte(&cc,0x48);CCByte(&cc,0xB9);
    CCQuadAbs(&cc,CC_B_SAVE,cc.save);
    CCByte(&cc,0x48);CCByte(&cc,0x8B);CCByte(&cc,0x21);
    CCDbgMark(0x68,0xCCCC);
    CCByte(&cc,0xC3);
  }
  return cc.code+thunk;
}

/* Zero-arg entry stub for 's': arguments are staged in cc->ext[] by
 * the caller, copied PUSH-left-to-right into the body's parameter
 * slots (same geometry as the argc/argv bridge, M4), then the body is
 * CALLed. The HolyC-side activation is a ZERO-ARGUMENT indirect call:
 * nothing sits on the caller stack, so it never depends on the HolyC
 * JIT's deferred caller-side pops for multi-arg IC_CALL_INDIRECT —
 * the desync that derails statements after an arg-taking fp call in
 * this boot image (PLAN.md M11). Address cached in s->extstub; CCInit
 * zeroes it. Returns the stub address, 0 on trouble. */
I64 CCExtStub(CSym *s)
{
  I64 thunk,n,i;
  if (!s)
    return 0;
  if (s->kind!=SC_FUN)
  {
    "CC ERROR: CCExtStub: not a function\n";
    return 0;
  }
  if (s->offset<0)
  {
    "CC ERROR: CCExtStub: function has no body\n";
    return 0;
  }
  n=s->fargc;
  if (n>4)
  {
    "CC ERROR: CCExtStub: more than 4 staged args\n";
    return 0;
  }
  if (s->extstub)
    return s->extstub;
  if ((cc.ep-cc.code)+128*(n+4)>CC_CODE_SIZE)
  {
    "CC ERROR: CCExtStub: code buffer full\n";
    return 0;
  }
  thunk=cc.ep-cc.code;
  cc.entry_inject=0;
  CCPrologue(&cc,TRUE);		//private-stack switch + frame
  if (n)
  {
    CCByte(&cc,0x48);CCByte(&cc,0xB9);	//MOVABS RCX,&cc.ext[0]
    CCQuadAbs(&cc,CC_B_CTRL,&cc.ext[0]);
    for (i=0;i!=n;i++)
    {
      //MOV RAX,[RCX+8*i]; PUSH RAX — left-to-right, as CCCall pushes.
      CCByte(&cc,0x48);CCByte(&cc,0x8B);CCByte(&cc,0x81);
      CCWord(&cc,8*i);
      CCByte(&cc,0x50);
    }
  }
  CCCall(&cc,s);
  CCPopArgs(&cc,n);
  CCFunRet(&cc,TRUE);
  s->extstub=cc.code+thunk;
  return s->extstub;
}

/* Reliable ways to run an arg-taking compiled C function from HolyC:
 * stage the arguments in cc->ext, invoke the zero-arg stub. Every
 * HolyC call involved is either direct or zero-arg indirect — the two
 * activation forms verified clean in every context, including
 * top-level script statements. Returns the C function's return value;
 * -2 if the name is not a compiled function. */
I64 CCFindCall1(U8 *name,I64 a0)
{
  I64 e;
  I64 (*f)();
  cc.ext[0]=a0;
  e=CCExtStub(CCSymFind(&cc,name));
  if (!e)
    return -2;
  f=e;
  return f();
}

I64 CCFindCall2(U8 *name,I64 a0,I64 a1)
{
  I64 e;
  I64 (*f)();
  cc.ext[0]=a0;
  cc.ext[1]=a1;
  e=CCExtStub(CCSymFind(&cc,name));
  if (!e)
    return -2;
  f=e;
  return f();
}

I64 CCFindCall3(U8 *name,I64 a0,I64 a1,I64 a2)
{
  I64 e;
  I64 (*f)();
  cc.ext[0]=a0;
  cc.ext[1]=a1;
  cc.ext[2]=a2;
  e=CCExtStub(CCSymFind(&cc,name));
  if (!e)
    return -2;
  f=e;
  return f();
}

I64 CCFindCall4(U8 *name,I64 a0,I64 a1,I64 a2,I64 a3)
{
  I64 e;
  I64 (*f)();
  cc.ext[0]=a0;
  cc.ext[1]=a1;
  cc.ext[2]=a2;
  cc.ext[3]=a3;
  e=CCExtStub(CCSymFind(&cc,name));
  if (!e)
    return -2;
  f=e;
  return f();
}

/* Default argv so `int main(int argc,char *argv[])` always sees a
 * valid argv even from CCRunFile. */
U8 *cc_dargv[2];

/* Compile a file and call main with an argc/argv bridge: the entry
 * prologue copies these fields into main's parameter slots.
 * Returns the C program's exit value, or -1 on compile failure. */
I64 CCRunArgv(U8 *fn,I64 argc,U8 **argv)
{
  U8 *text=CCSrc(fn);
  I64 (*entry)();
  I64 rc;
  if (!text)
  {
    "CC ERROR: cannot read %s\n",fn;
    return -1;
  }
  if (!CCCompile(&cc,text))
    return -1;
  //Set AFTER CCCompile: CCInit zeroes the control block.
  cc.argc=argc;
  cc.argv=argv;
  entry=cc.code+cc.entry_off;
  rc=entry();
  CCOutPrint(&cc);
  //text is intentionally leaked, not freed: see CCSrc comment.
  return rc;
}

/* Compile a file and call its entry point.
 * Returns the C program's exit value, or -1 on compile failure. */
I64 CCRunFile(U8 *fn)
{
  cc_dargv[0]="cc";
  cc_dargv[1]=NULL;
  return CCRunArgv(fn,1,cc_dargv);
}

/* ---- M10: object dump / reload -------------------------------------
 * CCSave serializes the COMPILED module to a file: machine code, the
 * initial data arena (global initializer values + the string pool are
 * written there at compile time), and the relocation table that
 * CCQuadAbs recorded for every absolute address that leaves the code
 * buffer. CCLoad rebuilds the module into a fresh CCInit'd set of
 * arenas and rebases each site. rel32 calls/jumps and RBP-relative
 * locals are self-relative, so they need nothing.
 *
 * File layout (all I64 little-endian):
 *   magic, version, entry_off, code_len, arena_len, nfix,
 *   nfix * (kind, delta, pos),  code bytes,  arena bytes
 *
 * The arena is dumped as of the save moment — the compile-time
 * snapshot is what a reload expects, so SAVE BEFORE RUNNING.
 * The symbol table is not part of the object: a loaded module runs
 * main(); CCFind needs the module recompiled. */
#define CCOBJ_MAGIC	0x43434F31	//'CCO1'
#define CCOBJ_VER	1

/* Dump the compiled module. 0 on success, -1 on error. */
I64 CCSave(U8 *fn)
{
  I64 clen,alen,i,total;
  I64 *hdr;
  U8 *blob,*p;

  if (cc.errs)
  {
    "CC ERROR: CCSave: module has compile errors\n";
    return -1;
  }
  if (cc.entry_off<0)
  {
    "CC ERROR: CCSave: module has no main\n";
    return -1;
  }
  clen=cc.ep-cc.code;
  alen=cc.goff;
  total=(6+3*cc.nfix)*8+clen+alen;
  blob=MAlloc(total);
  hdr=blob;
  hdr[0]=CCOBJ_MAGIC;
  hdr[1]=CCOBJ_VER;
  hdr[2]=cc.entry_off;
  hdr[3]=clen;
  hdr[4]=alen;
  hdr[5]=cc.nfix;
  for (i=0;i!=cc.nfix;i++)
  {
    hdr[6+3*i]=cc.fix_kind[i];
    hdr[7+3*i]=cc.fix_delta[i];
    hdr[8+3*i]=cc.fix_pos[i];
  }
  p=blob+(6+3*cc.nfix)*8;
  MemCpy(p,cc.code,clen);
  MemCpy(p+clen,cc.locals,alen);
  if (FileWrite(fn,blob,total)<=0)
  {
    "CC ERROR: CCSave: cannot write %s\n",fn;
    return -1;
  }
  return 0;
}

/* Load an object: the global cc becomes the module (fresh arenas, code
 * at cc.code, entry at cc.code+cc.entry_off). TRUE on success.
 * The FileRead block is leaked, never Free()d: see CCSrc comment. */
Bool CCLoad(U8 *fn)
{
  I64 len,nfix,clen,alen,i,total,val,kind;
  I64 *hdr;
  U8 *data,*p,*q;

  data=FileRead(fn,&len);
  if (!data)
  {
    "CC ERROR: CCLoad: cannot read %s\n",fn;
    return FALSE;
  }
  if (len<48)
  {
    "CC ERROR: CCLoad: %s too short for a CC object\n",fn;
    return FALSE;
  }
  hdr=data;
  if ((hdr[0]!=CCOBJ_MAGIC)||(hdr[1]!=CCOBJ_VER))
  {
    "CC ERROR: CCLoad: %s is not a CC object\n",fn;
    return FALSE;
  }
  clen=hdr[3];
  alen=hdr[4];
  nfix=hdr[5];
  if (((nfix<0)||(nfix>CC_FIXUPS))||((clen<1)||(clen>CC_CODE_SIZE))||
      ((alen<0)||(alen>CC_LOCAL_SIZE))||(len!=(6+3*nfix)*8+clen+alen))
  {
    "CC ERROR: CCLoad: %s has bad sizes\n",fn;
    return FALSE;
  }
  for (i=0;i!=nfix;i++)
  {
    kind=hdr[6+3*i];
    if (((kind<0)||(kind>CC_B_LIB))||((hdr[8+3*i]<0)||(hdr[8+3*i]+8>clen)))
    {
      "CC ERROR: CCLoad: %s has a corrupt relocation\n",fn;
      return FALSE;
    }
  }
  CCInit(&cc);		//fresh arenas; all relocations target these
  cc.hflush=&CCFlush;	//M6c: rebind the flush helper to this session
  CCSetLib(&cc);	//M12: rebind the stdlib helper table likewise
  cc.entry_off=hdr[2];
  p=data+(6+3*nfix)*8;
  MemCpy(cc.code,p,clen);
  MemCpy(cc.locals,p+clen,alen);
  cc.ep=cc.code+clen;
  for (i=0;i!=nfix;i++)
  {
    val=CCBase(&cc,hdr[6+3*i])+hdr[7+3*i];
    q=cc.code+hdr[8+3*i];
    q[0]=val;
    q[1]=val>>8;
    q[2]=val>>16;
    q[3]=val>>24;
    q[4]=val>>32;
    q[5]=val>>40;
    q[6]=val>>48;
    q[7]=val>>56;
  }
  return TRUE;
}

/* Load and run: returns the C program's exit value, -1 on load error. */
I64 CCRunObj(U8 *fn,I64 argc,U8 **argv)
{
  I64 (*entry)();
  I64 rc;
  if (!CCLoad(fn))
    return -1;
  cc.argc=argc;
  cc.argv=argv;
  entry=cc.code+cc.entry_off;
  rc=entry();
  CCOutPrint(&cc);
  return rc;
}

I64 CCRunObjFile(U8 *fn)
{
  cc_dargv[0]="cc";
  cc_dargv[1]=NULL;
  return CCRunObj(fn,1,cc_dargv);
}
