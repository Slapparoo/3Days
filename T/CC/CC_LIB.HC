/* M12 stdlib delegation: zero-arg HolyC helpers that read staged
 * arguments from cc->ext[] and call the KERNEL's own routines,
 * returning the value in RAX. Generated code reaches them with
 * MOVABS RAX,&cc->lib[slot]; CALL [RAX] (CCEmitLibCall) — a zero-arg
 * activation, the only cross-boundary call form this JIT executes
 * cleanly in every context (PLAN.md M11/M6c), preceded by MOVABS RDI
 * with the ctrl scratch quads because a JIT-compiled callee's frame
 * prologue REP STOSQs at the incoming RDI (PLAN.md "RDI landmine").
 * Spliced into CC.HC after `CCCtrl cc;` (they reference cc directly).
 *
 * Dialect notes (documented in PLAN.md, deliberate):
 * - char* strings are byte sequences in CC's model (literals packed,
 *   arrays byte-addressed), so kernel byte routines apply directly.
 * - malloc() delegates to MAlloc; free() is INTENTIONALLY absent —
 *   this port's Free() munmaps whole chunks and would pull shared
 *   pages out from under live arenas (see the CCSrc comment in CC.HC).
 * - Results are staged back in RAX; the parser types them (byte-ish
 *   pointers as TY_CHARP). */

I64 CCXStrLen()
{
  if (!cc.ext[0])
    return 0;
  return StrLen(cc.ext[0]);
}

I64 CCXMemCpy()
{
  U8 *d;
  if (!cc.ext[0])
    return 0;
  d=cc.ext[0];
  MemCpy(d,cc.ext[1],cc.ext[2]);	//kernel takes C arg order but RETURNs
  return d;				//dst+cnt (probe-verified v16=1: the
}					//rep movsb pointer); C wants dst.

I64 CCXMemSet()
{
  U8 *d;
  if (!cc.ext[0])
    return 0;
  d=cc.ext[0];
  MemSet(d,cc.ext[1],cc.ext[2]);	//kernel: (dst,val,cnt) = C memset,
  return d;				//but returns dst+cnt (v17=1).
}

I64 CCXMemCmp()
{
  U8 *a,*b;
  if (!cc.ext[0])
    return 0;
  a=cc.ext[0];
  b=cc.ext[1];
  return MemCmp(a,b,cc.ext[2]);		//kernel: same arg order as C
}

I64 CCXMalloc()
{
  if (!cc.ext[0])
    return 0;		//malloc(0) yields NULL (also a cold-slot guard:
  return MAlloc(cc.ext[0]);		//ext is zeroed at CCInit, and a NULL
}					//mem_task defaults to this task.

I64 CCXGetChar()
{
  return GetKey();	//kernel console read — BLOCKS until a key event,
}					//so headless -c cannot exercise it;
					//the value is a HolyC keyboard event
					//(ASCII in bits 0..7, SCF_* flags up
					//top: documented deviation from C).

/* Slot order is the ABI shared with CC.HH's CC_LIB_* defines,
 * CCEmitLibCall deltas, and the CCExprPrimary name dispatch. */
U0 CCSetLib(CCCtrl *c)
{
  c->lib[CC_LIB_STRLEN]=&CCXStrLen;
  c->lib[CC_LIB_MEMCPY]=&CCXMemCpy;
  c->lib[CC_LIB_MEMSET]=&CCXMemSet;
  c->lib[CC_LIB_MEMCMP]=&CCXMemCmp;
  c->lib[CC_LIB_MALLOC]=&CCXMalloc;
  c->lib[CC_LIB_GETCHAR]=&CCXGetChar;
}
