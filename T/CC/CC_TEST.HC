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
CCAssert(12,"T:/CC/TEST/sprintf.c","sprintf");
CCAssert(134,"T:/CC/TEST/abortc.c","abort");
/* M13: preprocessor-lite (guards/chained includes/macros/#if arms)
 * and the adapted-musl headers from T:/CC/INC/ against this compiler. */
CCAssert(12,"T:/CC/TEST/preproc.c","preproc");
CCAssert(16,"T:/CC/TEST/hdr.c","hdr");
/* M14: switch/case/default + do-while (control-flow completion). */
CCAssert(20,"T:/CC/TEST/switch.c","switch");
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
"CC TESTS: %d failed\n",fails;
Exit;
