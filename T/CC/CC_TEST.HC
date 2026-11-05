#include "T:/CC/CC.HC"

I64 fails=0;
U8 *tlt;
I64 tlgot=0;
U8 *gt;
I64 gg=0;

U0 CCAssert(I64 want,U8 *fn,U8 *name)
{
  I64 got=CCRunFile(fn);
  if (got==want)
    "PASS %s = %d\n",name,want;
  else
  {
    "FAIL %s got %d want %d\n",name,got,want;
    fails++;
  }
}

/* M6c: generated putchar streams through the HolyC CCFlush helper
 * ('\n' flush + near-full guard). Both programs end on a newline, so
 * the buffer must be EMPTY (count 0) when main returns; the TAIL
 * markers' presence in the streamed output is asserted by the shell
 * around the suite run. */
U0 CCM12()
{
  I64 got,n,i;

  got=CCRunFile("T:/CC/TEST/stream.c");
  n=0;
  for (i=7;i>=0;i--)
    n=n*256+cc.out[i];
  if ((got==7)&&(n==0))
    "PASS stream = rc7 resid0\n";
  else
  {
    "FAIL stream got %d resid %d\n",got,n;
    fails++;
  }
  got=CCRunFile("T:/CC/TEST/streambig.c");
  n=0;
  for (i=7;i>=0;i--)
    n=n*256+cc.out[i];
  if ((got==8)&&(n==0))
    "PASS streambig = rc8 resid0\n";
  else
  {
    "FAIL streambig got %d resid %d\n",got,n;
    fails++;
  }
}

/* The raw arg-taking CCFind thunk exercised in its verified-clean
 * shape: the enclosing HolyC function returns immediately after the
 * fp call, so the JIT's deferred arg pops land on the epilogue, where
 * the frame absorbs them. For arbitrary later statements use the
 * CCFindCallN helpers instead. */
I64 CCFindStub3()
{
  I64 (*f3)(I64,I64,I64);
  f3=CCFind("add3");
  if (!f3)
    return -3;
  return f3(2,5,7);
}

/* M7: named entry lookup (CCFind thunk) + argc/argv bridge.
 * M11: arg-taking lookups. Values via fp(f3(2,5,7)) are always right,
 * but the HolyC JIT's deferred caller-side pops for multi-arg indirect
 * fp calls desync its stack tracking and can derail LATER statements
 * (verified: pure-CC flows fault/hang after the enclosing function).
 * So the suite calls arg-taking C through CCFindCallN (cc->ext staging,
 * zero-arg stub); the raw arg-copy thunk is pinned in the one wrapper
 * shape that ran end-to-end green (findstub). */
U0 CCM7()
{
  U8 *t,*addr;
  U8 *av[3];
  I64 got;
  I64 (*fp)();
  t=CCSrc("T:/CC/TEST/find.c");
  got=-1;
  if (t)
    if (CCCompile(&cc,t))
    {
      addr=CCFind("addup");
      if (addr)
      {
        fp=addr;
        got=fp();
      }
    }
  if (got==23)
    "PASS find = 23\n";
  else
  {
    "FAIL find got %d want 23\n",got;
    fails++;
  }
  //Arg-taking lookups via cc->ext staging (the reliable path).
  //The raw arg-taking fp thunk is exercised only for its return value
  //in isolated probes: the HolyC JIT's stack tracking around
  //multi-arg IC_CALL_INDIRECT is shape-dependent and derails *later*
  //statements (see CC.HC cc_find_argpop comment; PLAN.md M11).
  t=CCSrc("T:/CC/TEST/findargs.c");
  got=-1;
  if (t)
    if (CCCompile(&cc,t))
    {
      if (CCFindCall3("add3",2,5,7)==257)
        if (CCFindCall1("sum",10)==55)
          got=55;
    }
  if (got==55)
    "PASS findargs = 257+55\n";
  else
  {
    "FAIL findargs got %d\n",got;
    fails++;
  }
  //Raw arg-copy thunk (CCFind fp signature) in the wrapper shape that
  //the argpop=0 build verified end-to-end green: a dedicated HolyC
  //function whose last statement returns the call value, with CCM7
  //and the whole harness continuing afterwards.
  got=CCFindStub3();
  if (got==257)
    "PASS findstub = 257\n";
  else
  {
    "FAIL findstub got %d want 257\n",got;
    fails++;
  }
  av[0]="prog";
  av[1]="two";
  av[2]=NULL;
  got=CCRunArgv("T:/CC/TEST/argv.c",2,av);
  if (got==2)
    "PASS argv = 2\n";
  else
  {
    "FAIL argv got %d want 2\n",got;
    fails++;
  }
}

/* M10: object dump + reload. objdemo exercises every relocation base
 * (save/stack in the entry stub, ctrl argc injection, out via printf,
 * locals globals+string pool); strbasic reloads a struct module.
 * The object lands at a fixed scratch path: this boot image has no
 * FileDelete, so each run overwrites it. */
U0 CCM10()
{
  U8 *obj="T:/CC/TEST.OBJ";
  U8 *t,*av[3];
  I64 got;
  got=-1;
  t=CCSrc("T:/CC/TEST/objdemo.c");
  if (t)
    if (CCCompile(&cc,t))
      if (CCSave(obj)==0)
        got=CCRunObjFile(obj);
  if (got==306)
    "PASS objsave = 306\n";
  else
  {
    "FAIL objsave got %d want 306\n",got;
    fails++;
  }
  //Reloading the SAME object with a different argc: proves the &cc->argc
  //injection site (CC_B_CTRL) was rebased, not carried over stale.
  av[0]="prog";
  av[1]="two";
  av[2]=NULL;
  got=CCRunObj(obj,2,av);
  if (got==307)
    "PASS objargc = 307\n";
  else
  {
    "FAIL objargc got %d want 307\n",got;
    fails++;
  }
  got=-1;
  t=CCSrc("T:/CC/TEST/strbasic.c");
  if (t)
    if (CCCompile(&cc,t))
      if (CCSave(obj)==0)
        got=CCRunObjFile(obj);
  if (got==138)
    "PASS objstruct = 138\n";
  else
  {
    "FAIL objstruct got %d want 138\n",got;
    fails++;
  }
  /* M12: the delegated stdlib through an object. The saved module's
   * call sites name cc->lib SLOTS (CC_B_LIB), and the scratch quads
   * (CC_B_CTRL); both must rebase to the loading session's ctrl. The
   * CALL [RAX] indirection means the code never baked another session's
   * helper addresses — reload picks up this process's CCSetLib table. */
  got=-1;
  t=CCSrc("T:/CC/TEST/stdlib.c");
  if (t)
    if (CCCompile(&cc,t))
      if (CCSave(obj)==0)
        got=CCRunObjFile(obj);
  if (got==52)
    "PASS objstdlib = 52\n";
  else
  {
    "FAIL objstdlib got %d want 52\n",got;
    fails++;
  }
  /* M37: %f through an object.  A lowered float directive bakes TWO
   * relocatable addresses — &cc->fmtbuf through CC_B_CTRL and the directive
   * text through CC_B_LOCALS, the same kind a `%s` literal already used — and
   * the helper then REWRITES that text in place whenever a width or a
   * precision is over its cap.  A reloaded module binds this session's ctrl
   * and gets its own copy of the string data, so the run must answer 63, the
   * number the CCAssert of the same file reports for a direct compile. */
  got=-1;
  t=CCSrc("T:/CC/TEST/ffmt.c");
  if (t)
    if (CCCompile(&cc,t))
      if (CCSave(obj)==0)
        got=CCRunObjFile(obj);
  if (got==63)
    "PASS objfloat = 63\n";
  else
  {
    "FAIL objfloat got %d want 63\n",got;
    fails++;
  }
  /* M38: the math builtins through an object.  A math call reaches the kernel
   * through cc->lib[slot]: the code names the SLOT (CC_B_LIB) and loads through
   * the table base, so a reloaded module runs THIS session's helpers -- the same
   * rebind objstdlib exists to prove, but now for eighteen entries that are
   * `_intern` opcodes, asm `_extern` routines and plain HolyC functions at once.
   * The printf sink adds the two rebases objfloat covers for `%f`: &cc->fmtbuf
   * via CC_B_CTRL and the directive text via CC_B_LOCALS.  Both runs must answer
   * 34, the number the CCAsserts of the same files report for a direct compile. */
  got=-1;
  t=CCSrc("T:/CC/TEST/mth.c");
  if (t)
    if (CCCompile(&cc,t))
      if (CCSave(obj)==0)
        got=CCRunObjFile(obj);
  if (got==34)
    "PASS objmath = 34\n";
  else
  {
    "FAIL objmath got %d want 34\n",got;
    fails++;
  }
  got=-1;
  t=CCSrc("T:/CC/TEST/mthp.c");
  if (t)
    if (CCCompile(&cc,t))
      if (CCSave(obj)==0)
        got=CCRunObjFile(obj);
  if (got==34)
    "PASS objmathprintf = 34\n";
  else
  {
    "FAIL objmathprintf got %d want 34\n",got;
    fails++;
  }
}

CCAssert(42,"T:/CC/TEST/ret42.c","ret42");
CCAssert(13,"T:/CC/TEST/prec.c","prec");
CCAssert(10,"T:/CC/TEST/unary.c","unary");
CCAssert(14,"T:/CC/TEST/div.c","div");
CCAssert(2,"T:/CC/TEST/mod.c","mod");
CCAssert(21,"T:/CC/TEST/loop1.c","loop1");
CCAssert(2,"T:/CC/TEST/ifelse.c","ifelse");
CCAssert(7,"T:/CC/TEST/ifconst.c","ifconst");
CCAssert(7,"T:/CC/TEST/logic.c","logic");
CCAssert(3,"T:/CC/TEST/nest.c","nest");
CCAssert(4,"T:/CC/TEST/cmp.c","cmp");
CCAssert(720,"T:/CC/TEST/fact.c","fact");
CCAssert(55,"T:/CC/TEST/fib.c","fib");
CCAssert(1,"T:/CC/TEST/mutual.c","mutual");
CCAssert(14,"T:/CC/TEST/nargs.c","nargs");
CCAssert(3,"T:/CC/TEST/glob.c","glob");
CCAssert(42,"T:/CC/TEST/globinit.c","globinit");
CCAssert(50,"T:/CC/TEST/param.c","param");
CCAssert(0,"T:/CC/TEST/hello.c","hello");
CCAssert(7,"T:/CC/TEST/exit.c","exit");
CCAssert(45,"T:/CC/TEST/forloop.c","forloop");
CCAssert(20,"T:/CC/TEST/fordec.c","fordec");
CCAssert(12,"T:/CC/TEST/forcont.c","forcont");
CCAssert(7,"T:/CC/TEST/addr.c","addr");
CCAssert(6,"T:/CC/TEST/deref.c","deref");
CCAssert(49,"T:/CC/TEST/arr.c","arr");
CCAssert(9,"T:/CC/TEST/parr.c","parr");
CCAssert(0,"T:/CC/TEST/str.c","str");
CCAssert(8,"T:/CC/TEST/strlen.c","strlen");
CCAssert(7,"T:/CC/TEST/parg.c","parg");
CCAssert(21,"T:/CC/TEST/comma.c","comma");
CCAssert(66,"T:/CC/TEST/char.c","char");
CCAssert(131,"T:/CC/TEST/globstr.c","globstr");
CCAssert(6,"T:/CC/TEST/pinc.c","pinc");
CCAssert(0,"T:/CC/TEST/printf.c","printf");
CCAssert(7,"T:/CC/TEST/pfmt2.c","pfmt2");
CCAssert(15,"T:/CC/TEST/mdecl.c","mdecl");
CCAssert(127,"T:/CC/TEST/gdecl.c","gdecl");
CCAssert(1055,"T:/CC/TEST/bitops.c","bitops");
CCAssert(101,"T:/CC/TEST/cmpd.c","cmpd");
CCAssert(31,"T:/CC/TEST/charl.c","charl");
CCAssert(12,"T:/CC/TEST/assexpr.c","assexpr");
CCAssert(85,"T:/CC/TEST/chpost.c","chpost");
CCAssert(138,"T:/CC/TEST/strbasic.c","strbasic");
CCAssert(47,"T:/CC/TEST/strchar.c","strchar");
CCAssert(41,"T:/CC/TEST/strlink.c","strlink");
CCM7();
CCM10();
CCM12();
CCAssert(52,"T:/CC/TEST/stdlib.c","stdlib");
CCAssert(15,"T:/CC/TEST/sprintf.c","sprintf");
CCAssert(134,"T:/CC/TEST/abortc.c","abort");
/* M13: preprocessor-lite (guards/chained includes/macros/#if arms)
 * and the adapted-musl headers from T:/CC/INC/ against this compiler. */
CCAssert(12,"T:/CC/TEST/preproc.c","preproc");
CCAssert(16,"T:/CC/TEST/hdr.c","hdr");
/* M14: switch/case/default + do-while (control-flow completion). */
CCAssert(20,"T:/CC/TEST/switch.c","switch");
/* M15: sizeof (C-truth sizes, arrays via symbol, silent expr path)
 * and casts (char truncation, pointer retype, struct-tag retention). */
CCAssert(26,"T:/CC/TEST/sizeof.c","sizeof");
/* M16: goto/labels (forward + backward + cross-structure exits) and
 * the conditional operator (unevaluated arms, pointer/tag unification). */
CCAssert(10,"T:/CC/TEST/goto.c","goto");
CCAssert(14,"T:/CC/TEST/tern.c","tern");
/* M17: two-dimensional arrays — row scaling, array-row params (int/
 * char/struct), byte-stride char[] params, 2-D sizeof, paren decay. */
CCAssert(22,"T:/CC/TEST/array2.c","array2");
/* M18: function pointers — decls in every position, forward addresses
 * patched at definition, indirect/member/typedef calls, NULL sentinels. */
CCAssert(27,"T:/CC/TEST/fptr.c","fptr");
/* M19: qualifier keywords, unsigned/signed char, unions (slot-aliased
 * tags, by-value one-slot members), escapes, literal indexing. */
CCAssert(40,"T:/CC/TEST/unionc.c","unionc");
/* M20: enum constants + the compile-time constant folder (case labels,
 * array sizes, global inits, folded expressions) + literal concat. */
CCAssert(37,"T:/CC/TEST/enums.c","enums");
/* M21: block scopes (shadowing + waterline erasure) and `static` block
 * objects on arena storage: counters, arrays, structs, fps, pointers,
 * loop/switch/nested-block scopes, shadowing params. */
CCAssert(49,"T:/CC/TEST/scoped.c","scoped");
/* M22: aggregate initializers — brace lists (grouped by C's element
 * rules), size deduction, string byte init, tail zero fill, globals +
 * `static` arrays, braced scalars. */
CCAssert(27,"T:/CC/TEST/arrinit.c","arrinit");
/* M23: struct|union initializers — members in declaration order, partial
 * init zero-filled, C's brace-group-to-element mapping with union arity 1,
 * struct arrays (deduced too), globals, statics, one-slot members. */
CCAssert(22,"T:/CC/TEST/structinit.c","structinit");
/* M24: pointer-valued initializers (string literals, DEFINED function
 * names, &globals) + ARENA relocations: pointer slots inside the arena are
 * re-derived on load instead of pointing at the compile-time arena. */
CCAssert(25,"T:/CC/TEST/ptrinit.c","ptrinit");
/* M25: element type through array decay — char* arrays stride/byte-load,
 * struct* arrays reach members, 1-element arrays finally decay, and 2-D
 * pointer arrays chain three levels. */
CCAssert(28,"T:/CC/TEST/decay.c","decay");
/* M26: arrays of function pointers — declared everywhere, deduced or sized,
 * passed as params, called via tab[i](x) / (*tab[i])(x) / (tab[i])(x), with
 * code-address elements that relocate on reload. */
CCAssert(31,"T:/CC/TEST/fptab.c","fptab");
/* M27: whole-object assignment — structs and unions copy slot by slot:
 * objects, array elements, dereferences, chained, frame<->arena, statics. */
CCAssert(28,"T:/CC/TEST/agcopy.c","agcopy");
/* M28: prefix ++/-- parse at all, and postfix moved into the chain so
 * (*p)++ and (v)++ work; one emitter, so both spellings scale alike. */
CCAssert(42,"T:/CC/TEST/incr.c","incr");
/* M29: pointer arithmetic — p+n and n+p scale by the element step, p-q is an
 * element count, struct arrays step tags, 2-D arrays step rows. */
CCAssert(33,"T:/CC/TEST/ptrmath.c","ptrmath");
/* M30: initializer designators, and the brace-elision cursor they share:
 * enclosures take the next SUBOBJECT (a whole element only at an element
 * boundary), designations move that same cursor, and ordering is by target. */
CCAssert(43,"T:/CC/TEST/desig.c","desig");
/* M31: function-like macros — arguments expanded once in the caller's context
 * and substituted without re-expansion, # and ## with unexpanded operands,
 * variadic tails (stringized as written, comma-deleted only after a `, ##` that
 * matched nothing), and a paste whose empty operand leaves the other side. */
CCAssert(45,"T:/CC/TEST/macros.c","macros");
/* M32: conditionals — defined() in four written forms, a real constant
 * expression in #if/#elif (C precedence, short-circuit that keeps a dead 1/0
 * silent, and the zero C gives a surviving identifier), #elif chains that stop
 * evaluating once a branch is taken, #error/#warning, and text inside #if 0
 * that must never be parsed, expanded or read. */
CCAssert(43,"T:/CC/TEST/cond.c","cond");
/* M33: line continuations and includer-relative #include — a body, a parameter
 * list, a name, a ## operand, an #if condition, a call's argument list, a
 * string, a character constant, an identifier, an initializer and a for header
 * all crossing the splice; the whitespace that survives on each side of it; a
 * `//` line and a block comment both claiming the line below them; and a quoted
 * name resolving against the directory of the header that wrote it rather than
 * the main file's directory. */
CCAssert(30,"T:/CC/TEST/cont.c","cont");
/* M34: file-scope redeclaration — `extern int g;` and a definition of g bind ONE
 * object at ONE address (a function compiled between them proves it), a
 * tentative definition is that object's zero initializer, only one declaration
 * may carry an initializer, an `extern int a[]` gets its length from a later
 * definition without the slots moving, a tag object declared `extern` already
 * knows its tag, and an extern nothing defines is an error. */
CCAssert(25,"T:/CC/TEST/decls.c","decls");
/* M35: C doubles in the F64 model — initializers converting to the object's
 * type at all five boundaries (global, array element, member, static local,
 * automatic local), the delegated arithmetic and its truncating divide, casts
 * crossing the two models, comparison including the negative pair a quad
 * compare would flip, ++/-- stepping 1.0 rather than the quad 1, argument and
 * return conversions through a prototype, recursion through a double prototype,
 * and the sizes the slot model answers C's way.  Its values are observable only
 * through (int) truncation: that fixture predates M37's %f, which is the check
 * below.  Checks 50-53 are deliberately NOT wrapped in (int), because that is
 * what hid the mixed-type comparison. */
CCAssert(53,"T:/CC/TEST/float.c","float");
/* M37: %f in BOTH sinks, against the KERNEL's renderings.  The helper does not
 * print a double: it hands the directive, and the value's bits, to
 * T:/STRPRINT.HC's `case 'f':` and reports what the kernel wrote — including
 * the widths and flags it honors (its own), the `i` and `-i` it spells inf
 * with, and the 248-character ceiling its scratch buffer puts on a digit walk.
 * What lite adds is the C part: `.6` written into the text when the source
 * stated no precision (the engine's own default is 0, measured), `*` dropped,
 * and the width and precision capped to what the 1024-byte destination holds.
 * The values are significand times exact power of two, so the fixture, the
 * sweep and CCProbeFloat below all format one and the same double.  No test
 * value is a decimal float literal: the host's Str2F64 scales by that same
 * drifting Pow(10.,i) table (measured: 1e15 12 ULP low, 0.25 1 high), and a
 * formatter checked against drifted inputs proves nothing.  The ffmtP line the
 * fixture printf's is the compile-time path, pinned against the same cells of
 * CCProbeFloat. */
CCAssert(63,"T:/CC/TEST/ffmt.c","ffmt");
/* M38: the math builtins.  The expectation is the KERNEL's own rendering of these
 * same expressions -- T:/CC/GATES/mthrows.txt, measured by calling Sqrt/Pow/ACos
 * and the rest directly and printing them with StrPrintJoin, the engine this
 * compiler hands a `%f` to -- never a C library's text.  Each fixture compares
 * what it rendered against that text and returns the number of cells that
 * matched, so 34 means all of them.  The cells carry a double literal, an INTEGER
 * literal, a double variable, an int variable, and pow with an integer in each
 * position, because that is where C's int->double conversion at a call is
 * observable: an integer quad that reaches Sqrt unconverted is the denormal
 * 9.88e-324, and sqrt(2) then answers 0.00000000000000000, which no square root
 * can. */
CCAssert(34,"T:/CC/TEST/mth.c","math");
CCAssert(34,"T:/CC/TEST/mthp.c","mathprintf");
/* M12.1: getchar/fgetc/getc call SHAPE — compile+save gate only. The
 * kernel GetKey blocks with no console input headless, but the object
 * write proves all three call forms lowered and linked. */
gt=CCSrc("T:/CC/TEST/getcharc.c");
if (gt)
  if (CCCompile(&cc,gt))
    if (CCSave("T:/CC/TEST.OBJ")==0)
      gg=1;
if (gg==1)
  "PASS getchar-shape\n";
else
{
  "FAIL getchar-shape got %d\n",gg;
  fails++;
}
/* CCFindCallN from a TOP-LEVEL statement: compile findargs.c, then
 * call the staged 3-arg entry. Direct activations are top-level-safe
 * (this is the shape CCRunFile/CCRunArgv have always used). */
tlt=CCSrc("T:/CC/TEST/findargs.c");
if (CCCompile(&cc,tlt))
  tlgot=CCFindCall3("add3",11,22,33);
else
  tlgot=-4;
if (tlgot==1353)
  "PASS tlcall = 1353\n";
else
{
  "FAIL tlcall got %d want 1353\n",tlgot;
  fails++;
}
/* M37: the KERNEL side of the ffmt table.
 * bits, same renderings, no lite anywhere in the middle.
 * StrPrintJoin is called exactly as cci_fmt calls it: argv is the
 * address of one quad holding the value's bits, and the directive
 * is the one lite's two sinks COMPOSE: `.6` written in when the
 * source stated none, `*` dropped, and the width and the precision
 * capped to what the destination holds.  This says what the kernel
 * answers; ffmt.c says lite answers the same.  The pair pins the
 * delegation and the composition.  A rendering too wide for one
 * source line is pinned by its length, its first character and its
 * last eight, which is all a padding rule fixes; the digits are
 * pinned by the short cells of the same directive nearby. */
I64 CCProbeFloat()
{
  U8 out[1100];
  I64 q,nf,L;

  q=0;
  nf=0;
  q=0;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.6f",1,&q);
  if (StrCmp(out,"0.000000"))
  {
    "FAIL float cell 0 [%s] want [0.000000]\n",out;
    nf=nf+1;
  }
  q=0;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.3f",1,&q);
  if (StrCmp(out,"0.000"))
  {
    "FAIL float cell 1 [%s] want [0.000]\n",out;
    nf=nf+1;
  }
  q=0;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.15f",1,&q);
  if (StrCmp(out,"0.000000000000000"))
  {
    "FAIL float cell 2 [%s] want [0.000000000000000]\n",out;
    nf=nf+1;
  }
  q=0;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%10.3f",1,&q);
  if (StrCmp(out,"     0.000"))
  {
    "FAIL float cell 3 [%s] want [     0.000]\n",out;
    nf=nf+1;
  }
  q=-9223372036854775808;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.6f",1,&q);
  if (StrCmp(out,"-0.000000"))
  {
    "FAIL float cell 4 [%s] want [-0.000000]\n",out;
    nf=nf+1;
  }
  q=-9223372036854775808;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.3f",1,&q);
  if (StrCmp(out,"-0.000"))
  {
    "FAIL float cell 5 [%s] want [-0.000]\n",out;
    nf=nf+1;
  }
  q=-9223372036854775808;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.15f",1,&q);
  if (StrCmp(out,"-0.000000000000000"))
  {
    "FAIL float cell 6 [%s] want [-0.000000000000000]\n",out;
    nf=nf+1;
  }
  q=-9223372036854775808;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%10.3f",1,&q);
  if (StrCmp(out,"    -0.000"))
  {
    "FAIL float cell 7 [%s] want [    -0.000]\n",out;
    nf=nf+1;
  }
  q=4607182418800017408;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.6f",1,&q);
  if (StrCmp(out,"1.000000"))
  {
    "FAIL float cell 8 [%s] want [1.000000]\n",out;
    nf=nf+1;
  }
  q=4607182418800017408;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.3f",1,&q);
  if (StrCmp(out,"1.000"))
  {
    "FAIL float cell 9 [%s] want [1.000]\n",out;
    nf=nf+1;
  }
  q=4607182418800017408;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.15f",1,&q);
  if (StrCmp(out,"0.999999999999998"))
  {
    "FAIL float cell 10 [%s] want [0.999999999999998]\n",out;
    nf=nf+1;
  }
  q=4607182418800017408;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%10.3f",1,&q);
  if (StrCmp(out,"     1.000"))
  {
    "FAIL float cell 11 [%s] want [     1.000]\n",out;
    nf=nf+1;
  }
  q=4591870180066957722;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.6f",1,&q);
  if (StrCmp(out,"0.100000"))
  {
    "FAIL float cell 12 [%s] want [0.100000]\n",out;
    nf=nf+1;
  }
  q=4591870180066957722;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.3f",1,&q);
  if (StrCmp(out,"0.100"))
  {
    "FAIL float cell 13 [%s] want [0.100]\n",out;
    nf=nf+1;
  }
  q=4591870180066957722;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.15f",1,&q);
  if (StrCmp(out,"0.100000000000000"))
  {
    "FAIL float cell 14 [%s] want [0.100000000000000]\n",out;
    nf=nf+1;
  }
  q=4591870180066957722;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%10.3f",1,&q);
  if (StrCmp(out,"     0.100"))
  {
    "FAIL float cell 15 [%s] want [     0.100]\n",out;
    nf=nf+1;
  }
  q=4621819117588971520;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.6f",1,&q);
  if (StrCmp(out,"10.000000"))
  {
    "FAIL float cell 16 [%s] want [10.000000]\n",out;
    nf=nf+1;
  }
  q=4621819117588971520;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.3f",1,&q);
  if (StrCmp(out,"10.000"))
  {
    "FAIL float cell 17 [%s] want [10.000]\n",out;
    nf=nf+1;
  }
  q=4621819117588971520;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.15f",1,&q);
  if (StrCmp(out,"9.999999999999984"))
  {
    "FAIL float cell 18 [%s] want [9.999999999999984]\n",out;
    nf=nf+1;
  }
  q=4621819117588971520;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%10.3f",1,&q);
  if (StrCmp(out,"    10.000"))
  {
    "FAIL float cell 19 [%s] want [    10.000]\n",out;
    nf=nf+1;
  }
  q=4756540486875873280;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.6f",1,&q);
  if (StrCmp(out,"9999999999.999998"))
  {
    "FAIL float cell 20 [%s] want [9999999999.999998]\n",out;
    nf=nf+1;
  }
  q=4756540486875873280;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.3f",1,&q);
  if (StrCmp(out,"10000000000.000"))
  {
    "FAIL float cell 21 [%s] want [10000000000.000]\n",out;
    nf=nf+1;
  }
  q=4756540486875873280;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.15f",1,&q);
  if (StrCmp(out,"9999999999.999995200000000"))
  {
    "FAIL float cell 22 [%s] want [9999999999.999995200000000]\n",out;
    nf=nf+1;
  }
  q=4756540486875873280;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%10.3f",1,&q);
  if (StrCmp(out,"10000000000.000"))
  {
    "FAIL float cell 23 [%s] want [10000000000.000]\n",out;
    nf=nf+1;
  }
  q=4698053240436593525;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.6f",1,&q);
  if (StrCmp(out,"1234567.891000"))
  {
    "FAIL float cell 24 [%s] want [1234567.891000]\n",out;
    nf=nf+1;
  }
  q=4698053240436593525;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.3f",1,&q);
  if (StrCmp(out,"1234567.891"))
  {
    "FAIL float cell 25 [%s] want [1234567.891]\n",out;
    nf=nf+1;
  }
  q=4698053240436593525;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.15f",1,&q);
  if (StrCmp(out,"1234567.890999994240000"))
  {
    "FAIL float cell 26 [%s] want [1234567.890999994240000]\n",out;
    nf=nf+1;
  }
  q=4698053240436593525;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%10.3f",1,&q);
  if (StrCmp(out,"1234567.891"))
  {
    "FAIL float cell 27 [%s] want [1234567.891]\n",out;
    nf=nf+1;
  }
  q=9094988921128908188;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.6f",1,&q);
  L=248;		/* the measured length of this rendering */
  if ((StrLen(out)!=L)||(out[0]!='0')||
      StrCmp(out + L - 8,"0.000000"))
  {
    "FAIL float cell 28 len %d [%s]\n",StrLen(out);
    nf=nf+1;
  }
  q=9094988921128908188;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.3f",1,&q);
  L=248;		/* the measured length of this rendering */
  if ((StrLen(out)!=L)||(out[0]!='0')||
      StrCmp(out + L - 8,"0000.000"))
  {
    "FAIL float cell 29 len %d [%s]\n",StrLen(out);
    nf=nf+1;
  }
  q=9094988921128908188;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.15f",1,&q);
  L=248;		/* the measured length of this rendering */
  if ((StrLen(out)!=L)||(out[0]!='0')||
      StrCmp(out + L - 8,"00000000"))
  {
    "FAIL float cell 30 len %d [%s]\n",StrLen(out);
    nf=nf+1;
  }
  q=9094988921128908188;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%10.3f",1,&q);
  L=248;		/* the measured length of this rendering */
  if ((StrLen(out)!=L)||(out[0]!='0')||
      StrCmp(out + L - 8,"0000.000"))
  {
    "FAIL float cell 31 len %d [%s]\n",StrLen(out);
    nf=nf+1;
  }
  q=9218868437227405312;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.6f",1,&q);
  if (StrCmp(out,"i"))
  {
    "FAIL float cell 32 [%s] want [i]\n",out;
    nf=nf+1;
  }
  q=9218868437227405312;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.3f",1,&q);
  if (StrCmp(out,"i"))
  {
    "FAIL float cell 33 [%s] want [i]\n",out;
    nf=nf+1;
  }
  q=9218868437227405312;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.15f",1,&q);
  if (StrCmp(out,"i"))
  {
    "FAIL float cell 34 [%s] want [i]\n",out;
    nf=nf+1;
  }
  q=9218868437227405312;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%10.3f",1,&q);
  if (StrCmp(out,"         i"))
  {
    "FAIL float cell 35 [%s] want [         i]\n",out;
    nf=nf+1;
  }
  q=-4503599627370496;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.6f",1,&q);
  if (StrCmp(out,"-i"))
  {
    "FAIL float cell 36 [%s] want [-i]\n",out;
    nf=nf+1;
  }
  q=-4503599627370496;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.3f",1,&q);
  if (StrCmp(out,"-i"))
  {
    "FAIL float cell 37 [%s] want [-i]\n",out;
    nf=nf+1;
  }
  q=-4503599627370496;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.15f",1,&q);
  if (StrCmp(out,"-i"))
  {
    "FAIL float cell 38 [%s] want [-i]\n",out;
    nf=nf+1;
  }
  q=-4503599627370496;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%10.3f",1,&q);
  if (StrCmp(out,"        -i"))
  {
    "FAIL float cell 39 [%s] want [        -i]\n",out;
    nf=nf+1;
  }
  q=9221120237041090560;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.6f",1,&q);
  if (StrCmp(out,"9223372036854.775808"))
  {
    "FAIL float cell 40 [%s] want [9223372036854.775808]\n",out;
    nf=nf+1;
  }
  q=9221120237041090560;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.3f",1,&q);
  if (StrCmp(out,"9223372036854775.808"))
  {
    "FAIL float cell 41 [%s] want [9223372036854775.808]\n",out;
    nf=nf+1;
  }
  q=9221120237041090560;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.15f",1,&q);
  if (StrCmp(out,"9223.372036854775808"))
  {
    "FAIL float cell 42 [%s] want [9223.372036854775808]\n",out;
    nf=nf+1;
  }
  q=9221120237041090560;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%10.3f",1,&q);
  if (StrCmp(out,"9223372036854775.808"))
  {
    "FAIL float cell 43 [%s] want [9223372036854775.808]\n",out;
    nf=nf+1;
  }
  q=4609434218613702656;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.6f",1,&q);
  if (StrCmp(out,"1.500000"))
  {
    "FAIL float cell 44 [%s] want [1.500000]\n",out;
    nf=nf+1;
  }
  q=4609434218613702656;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.3f",1,&q);
  if (StrCmp(out,"1.500"))
  {
    "FAIL float cell 45 [%s] want [1.500]\n",out;
    nf=nf+1;
  }
  q=4609434218613702656;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.15f",1,&q);
  if (StrCmp(out,"1.499999999999998"))
  {
    "FAIL float cell 46 [%s] want [1.499999999999998]\n",out;
    nf=nf+1;
  }
  q=4609434218613702656;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%10.3f",1,&q);
  if (StrCmp(out,"     1.500"))
  {
    "FAIL float cell 47 [%s] want [     1.500]\n",out;
    nf=nf+1;
  }
  q=4609434218613702656;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.0f",1,&q);
  if (StrCmp(out,"2"))
  {
    "FAIL float cell 48 [%s] want [2]\n",out;
    nf=nf+1;
  }
  q=4609434218613702656;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.1f",1,&q);
  if (StrCmp(out,"1.5"))
  {
    "FAIL float cell 49 [%s] want [1.5]\n",out;
    nf=nf+1;
  }
  q=4609434218613702656;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.16f",1,&q);
  if (StrCmp(out,"1.4999999999999970"))
  {
    "FAIL float cell 50 [%s] want [1.4999999999999970]\n",out;
    nf=nf+1;
  }
  q=4609434218613702656;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.20f",1,&q);
  if (StrCmp(out,"1.49999999999999648000"))
  {
    "FAIL float cell 51 [%s] want [1.49999999999999648000]\n",out;
    nf=nf+1;
  }
  q=4609434218613702656;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%010.3f",1,&q);
  if (StrCmp(out,"000001.500"))
  {
    "FAIL float cell 52 [%s] want [000001.500]\n",out;
    nf=nf+1;
  }
  q=4609434218613702656;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%-10.3f",1,&q);
  if (StrCmp(out,"     1.500"))
  {
    "FAIL float cell 53 [%s] want [     1.500]\n",out;
    nf=nf+1;
  }
  q=4609434218613702656;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%+.6f",1,&q);
  if (StrCmp(out,".6f"))
  {
    "FAIL float cell 54 [%s] want [.6f]\n",out;
    nf=nf+1;
  }
  q=4609434218613702656;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%.6f",1,&q);
  if (StrCmp(out,"1.500000"))
  {
    "FAIL float cell 55 [%s] want [1.500000]\n",out;
    nf=nf+1;
  }
  q=4609434218613702656;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%100.50f",1,&q);
  L=100;		/* the measured length of this rendering */
  if ((StrLen(out)!=L)||(out[0]!=' ')||
      StrCmp(out + L - 8,"00000000"))
  {
    "FAIL float cell 56 len %d [%s]\n",StrLen(out);
    nf=nf+1;
  }
  q=4609434218613702656;
  MemSet(out,0,1100);
  StrPrintJoin(out,"%01000.3f",1,&q);
  L=1000;		/* the measured length of this rendering */
  if ((StrLen(out)!=L)||(out[0]!='0')||
      StrCmp(out + L - 8,"0001.500"))
  {
    "FAIL float cell 57 len %d [%s]\n",StrLen(out);
    nf=nf+1;
  }
  return nf;
}


/* M37: the kernel side of the ffmt table, checked in HolyC in this process
 * with no C file and no lite formatter in the way.  This is the statement
 * that the renderings ffmt.c pins are the KERNEL's: 58 cells, the same list,
 * composed directives, compared against StrPrintJoin directly. */
I64 ccpf=CCProbeFloat();
if (ccpf==0)
  "PASS float kernel = 58 cells\n";
else
{
  "FAIL float kernel: %d of 58 cells\n",ccpf;
  fails=fails+ccpf;
}
"CC TESTS: %d failed\n",fails;
Exit;
