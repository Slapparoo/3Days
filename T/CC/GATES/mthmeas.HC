/* M38 oracle re-measurement (in-tree HolyC driver; run it, do not transcribe
 * it).  mthrows.txt pins what the kernel printed for the 34 math cells on
 * 2026-11-05; this file calls those same 34 kernel entries and renders each
 * result with StrPrintJoin -- the very engine T:/CC/CC_LIB.HC hands a `%f` to
 * -- so the pinned table can be RE-MEASURED and diffed against itself after a
 * kernel rebuild (README.txt gate 7) instead of being trusted forever:
 *
 *   ./3d_loader -t T -c T:/CC/GATES/mthmeas.HC </dev/null > meas.log
 *   grep '^K ' meas.log            | awk '{print $2, $NF}' > got.txt
 *   awk '/^K /{print $2, $NF}' T:/CC/GATES/mthrows.txt > want.txt
 *   diff want.txt got.txt      # expect: no output, 34 rows each side
 *
 * A row prints as `K <nn> [<rendering>]`, which is mthrows.txt's own first,
 * second and last field, so the two awk calls above line the two sides up
 * without knowing the kernel expression that sits between them on the pinned
 * line.  Nothing else prints here: no banner and no summary, so the log's K
 * rows ARE the measurement, and a missing or short row is visible as a diff
 * rather than as a number to argue about.
 *
 * A bare `%f` is precision 0 in this kernel -- measured, 1.5 prints `2` -- so
 * the directive is held as TEXT in f1 and spelled `%.17f`, the same text lite
 * composes for these cells.  The value reaches the engine the way cci_fmt
 * sends it: through q, whose address is argv.  The cell number is a LITERAL in
 * each format string, so a row does not depend on how this kernel zero-pads a
 * %d, and the calls are written out one per cell rather than table-driven: a
 * table would let a row silently point at the wrong entry.
 */
I64 q;
U8 out[256];
U8 f1[16];

StrCpy(f1,"%.17f");
q=(Sqrt(2.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 01 [%s]\n",out;
q=(Sqrt(2.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 02 [%s]\n",out;
q=(Sqrt(3.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 03 [%s]\n",out;
q=(Sqrt(4.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 04 [%s]\n",out;
q=(Sqrt(4.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 05 [%s]\n",out;
q=(Sqrt(0.25))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 06 [%s]\n",out;
q=(Abs(-2.5))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 07 [%s]\n",out;
q=(Exp(1.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 08 [%s]\n",out;
q=(Exp(0.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 09 [%s]\n",out;
q=(Ln(1.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 10 [%s]\n",out;
q=(Ln(2.7))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 11 [%s]\n",out;
q=(Log10(10.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 12 [%s]\n",out;
q=(Log10(1000.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 13 [%s]\n",out;
q=(Floor(2.7))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 14 [%s]\n",out;
q=(Floor(-2.7))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 15 [%s]\n",out;
q=(Ceil(2.1))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 16 [%s]\n",out;
q=(Ceil(-2.1))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 17 [%s]\n",out;
q=(Trunc(2.7))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 18 [%s]\n",out;
q=(Pow(2.0,0.5))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 19 [%s]\n",out;
q=(Pow(2.0,3.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 20 [%s]\n",out;
q=(Pow(4.0,0.5))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 21 [%s]\n",out;
q=(Pow(2.0,10.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 22 [%s]\n",out;
q=(Pow(-2.0,0.5))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 23 [%s]\n",out;
q=(Round(2.5))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 24 [%s]\n",out;
q=(Round(3.5))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 25 [%s]\n",out;
q=(Sin(0.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 26 [%s]\n",out;
q=(Sin(1.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 27 [%s]\n",out;
q=(Cos(1.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 28 [%s]\n",out;
q=(Tan(1.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 29 [%s]\n",out;
q=(ATan(1.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 30 [%s]\n",out;
q=(ASin(0.5))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 31 [%s]\n",out;
q=(ACos(0.5))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 32 [%s]\n",out;
q=(Sinh(1.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 33 [%s]\n",out;
q=(Cosh(1.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"K 34 [%s]\n",out;
Exit;
