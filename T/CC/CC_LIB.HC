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

/* ---------------- M35: floating point, delegated whole ----------------
 *
 * Every float operation lite performs is a HOST operation.  These helpers are
 * HolyC, compiled by the kernel's own JIT, so `+`, `/`, `<` and the
 * int<->F64 assignments below ARE TempleOS F64 semantics — lite implements no
 * float arithmetic, no rounding mode, no conversion algorithm and emits no
 * SSE encoding.  A double is one slot holding the IEEE-754 binary64 pattern,
 * and a postfix cast moves bits between the slot and an F64 without converting
 * them (`a=cc.ext[0](F64)`), while a plain assignment DOES convert (`r=a`
 * truncates toward zero, `r=cc.ext[0]` widens an integer exactly): that
 * distinction is the entire representation, and both halves were measured
 * before being leaned on (PLAN.md M35 has the tables and the reproduction).
 *
 * Measured, so the record is not a claim: on reinterpreted quads the host's
 * +,-,*,/ and unary minus come out BIT-EXACT against gcc over nan, -nan, inf,
 * -inf, +-0.0, 1e308, 5e-324, 0.1, 1+2^-52, -7.875 and pi, and its ordered
 * compares (`<`, `>`, `<=`, `>=`) are IEEE for every finite pair — including
 * two negatives, where comparing the quads instead would answer backwards —
 * and false for NaN pairs, which is what C asks for.  What is NOT C is the
 * host's `==`/`!=`, which are bitwise, so `-0.0 == 0.0` answers 0 and a NaN
 * equals itself.  CCXFEq/CCXFNe below therefore COMPOSE C's equality from the
 * ordered compares rather than using the host's `==`: measured over 16 pairs
 * (both zeros, four NaN forms, both infinities, subnormals, equal and unequal
 * finite values) the composition answers every case C's way, and it costs one
 * extra compare instead of a new mechanism.  C's TRUTHINESS is not composed —
 * it stays the bits-nonzero test the integer model uses, one recorded
 * divergence (`-0.0` is truthy here, falsy in C); see PLAN M35.
 *
 * One host mis-lowering is load-bearing for how these bodies are spelled: a
 * `!` written DIRECTLY on a parenthesized floating comparison answers 0 for
 * every pair (measured: `!(1.0>1.0)` gives 0 where C wants 1, and `!(1.0<2.0)`
 * gives 0 where C wants 1), while the same negation through an integer
 * temporary is correct.  That is why `<=`/`>=` use the host's own operators
 * rather than being composed, and why CCXFEq negates a `||` result held in an
 * I64.  Do not "simplify" either of those into `!(float compare)`.
 *
 * cc->ext[] is the ABI, so these read their operands from memory rather than
 * registers: that also means the host compiler cannot constant-fold any operand
 * here, which matters because a comparison against a literal 0.0 gets folded
 * into a sign test and mis-classifies -0.0.  Zero operands reach the helpers as
 * staged quads for the same reason. */

I64 CCXFAdd()
{
  F64 a,b,r;
  a=cc.ext[0](F64);
  b=cc.ext[1](F64);
  r=a+b;
  return r(I64);
}

I64 CCXFSub()
{
  F64 a,b,r;
  a=cc.ext[0](F64);
  b=cc.ext[1](F64);
  r=a-b;
  return r(I64);
}

I64 CCXFMul()
{
  F64 a,b,r;
  a=cc.ext[0](F64);
  b=cc.ext[1](F64);
  r=a*b;
  return r(I64);
}

I64 CCXFDiv()
{
  F64 a,b,r;
  a=cc.ext[0](F64);
  b=cc.ext[1](F64);
  r=a/b;
  return r(I64);		//C's `/` on doubles is division with truncation
}
					//toward zero, and the host truncates
					//its F64 quotient (7.0/2.0 -> 3), so
					//this is C's answer; a quotient that
					//does not fit a quad is C-undefined
					//and lands wherever the cast lands.

I64 CCXFNeg()
{
  F64 a,r;
  a=cc.ext[0](F64);
  r=-a;
  return r(I64);
}

I64 CCXFLt()
{
  F64 a,b;
  a=cc.ext[0](F64);
  b=cc.ext[1](F64);
  return a<b;
}

I64 CCXFGt()
{
  F64 a,b;
  a=cc.ext[0](F64);
  b=cc.ext[1](F64);
  return a>b;
}

/* `<=` and `>=` are the host's OWN operators, and the reason they are not
 * composed the way `==` is: measured over equal pairs, unequal pairs, two
 * negatives (-2.0 vs -4.0, where comparing the quads answers backwards),
 * -0.0 vs 0.0, both infinities, 1e308 vs -inf and both NaN forms, the host's
 * `a<=b`/`a>=b` come out exactly as C defines them — including NaN, where
 * every ordered comparison is false.  Composing them as `!(a>b)` instead is
 * what I first wrote here, and it is WRONG in this HolyC build: a `!` written
 * directly on a parenthesized FLOATING comparison answers 0 for every pair
 * (probe: !(1.0>1.0)=0 and !(1.0<2.0)=0 where C wants 1 and 0, while
 * `r=a>b; return !r;` gives the right 1 both times).  So the negation goes
 * through an integer temporary wherever it is needed, and here it is not
 * needed at all.  See PLAN M35 for the table. */
I64 CCXFLe()
{
  F64 a,b;
  a=cc.ext[0](F64);
  b=cc.ext[1](F64);
  return a<=b;
}

I64 CCXFGe()
{
  F64 a,b;
  a=cc.ext[0](F64);
  b=cc.ext[1](F64);
  return a>=b;
}

I64 CCXFEq()
{
  F64 a,b;
  I64 r;
  a=cc.ext[0](F64);
  b=cc.ext[1](F64);
  r=(a<b || b<a);
  return !r;			//C's equality: NOT "strictly one before the
}				//other".  -0.0 and 0.0 come out equal (the host's
				//bitwise `==` says they are not) and a NaN comes
				//out unequal to itself (its bitwise `==` says it
				//is).  Measured over 16 pairs: 0 disagreements
				//with C, where the host's own `==` had 3.  The
				//negation goes through an INTEGER temporary:
				//`!` written directly on a floating comparison
				//is the mis-lowering the two helpers below
				//document, and `||` is what makes the integer
				//answer available here.

I64 CCXFNe()
{
  F64 a,b;
  a=cc.ext[0](F64);
  b=cc.ext[1](F64);
  return (a<b || b<a);		//the negation of the composition above, not
}				//the host's `!=` (which is bitwise)

I64 CCXFItoD()
{
  F64 r;
  r=cc.ext[0];			//ASSIGNMENT converts an integer to F64
  return r(I64);			//(numerically, measured exact for 9 and for
}				//every integer up to 2^53; the postfix cast on
				//the way out reinterprets the F64 back to bits)

I64 CCXFDtoI()
{
  F64 a;
  I64 r;
  a=cc.ext[0](F64);		//reinterpret: the slot IS the double's bits
  r=a;				//ASSIGNMENT converts: truncates toward zero
  return r;			//(7.875 -> 7, -7.875 -> -7, both measured).
}				//Out of range C is undefined; the host yields
				//0x8000000000000000, as x86 cvttsd2si does.

/* The constant folder's route to the SAME helpers.  A compile-time float
 * expression is evaluated by calling the very function a compiled program calls
 * at run time — not an equivalent HolyC expression, the same one — so the
 * compiler and the program cannot disagree about what 2.0 * 3.0 is, and the
 * folder grows no float semantics of its own.  It stages into cc->ext exactly
 * like generated code does: safe because the folder runs while the unit is
 * being PARED, and generated code touches ext only when it RUNS. */
I64 CCFold(CCCtrl *cc,I64 slot,I64 a,I64 b)
{
  cc->ext[0]=a;
  cc->ext[1]=b;
  if (slot==CC_LIB_FADD)
    return CCXFAdd();
  if (slot==CC_LIB_FSUB)
    return CCXFSub();
  if (slot==CC_LIB_FMUL)
    return CCXFMul();
  if (slot==CC_LIB_FDIV)
    return CCXFDiv();
  if (slot==CC_LIB_FNEG)
    return CCXFNeg();
  if (slot==CC_LIB_FLT)
    return CCXFLt();
  if (slot==CC_LIB_FGT)
    return CCXFGt();
  if (slot==CC_LIB_FLE)
    return CCXFLe();
  if (slot==CC_LIB_FGE)
    return CCXFGe();
  if (slot==CC_LIB_FEQ)
    return CCXFEq();
  if (slot==CC_LIB_FNE)
    return CCXFNe();
  if (slot==CC_LIB_I2D)
    return CCXFItoD();
  if (slot==CC_LIB_D2I)
    return CCXFDtoI();
  return 0;			//unreachable: every slot has a branch above,
}				//and 0 is what a fold of an unknown op deserves.

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
  c->lib[CC_LIB_FADD]=&CCXFAdd;
  c->lib[CC_LIB_FSUB]=&CCXFSub;
  c->lib[CC_LIB_FMUL]=&CCXFMul;
  c->lib[CC_LIB_FDIV]=&CCXFDiv;
  c->lib[CC_LIB_FNEG]=&CCXFNeg;
  c->lib[CC_LIB_FLT]=&CCXFLt;
  c->lib[CC_LIB_FGT]=&CCXFGt;
  c->lib[CC_LIB_FLE]=&CCXFLe;
  c->lib[CC_LIB_FGE]=&CCXFGe;
  c->lib[CC_LIB_FEQ]=&CCXFEq;
  c->lib[CC_LIB_FNE]=&CCXFNe;
  c->lib[CC_LIB_I2D]=&CCXFItoD;
  c->lib[CC_LIB_D2I]=&CCXFDtoI;
}
