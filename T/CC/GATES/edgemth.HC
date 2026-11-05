/* M38 edge probe driver: runs T:/CC/GATES/edgemth.c, then answers the same 15
 * expressions by calling the kernel entries DIRECTLY in this session and printing
 * them with StrPrintJoin -- the engine lite hands a `%f` to.  Each oracle prints
 * immediately after its own call, so a kernel entry that FAULTS on an input ends
 * the log at that row and the cell is identifiable.  Nothing here asserts what a
 * NaN or an infinity should look like; it asserts that lite and the kernel answer
 * alike.  Expect 15 Es rows, 15 O rows and rc=0.
 */
#include "T:/CC/CC.HC"
I64 q;
U8 out[256];
U8 f1[16];

StrCpy(f1,"%.17f");
Cd("T:/");
"S rc=%d\n", CCRunFile("T:/CC/GATES/edgemth.c");

q=(Sqrt(-1.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"O01|%s|\n", out;
q=(Sqrt(0.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"O02|%s|\n", out;
q=(Sqrt(-0.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"O03|%s|\n", out;
q=(Ln(0.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"O04|%s|\n", out;
q=(Ln(-1.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"O05|%s|\n", out;
q=(Exp(1000.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"O06|%s|\n", out;
q=(Exp(-1000.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"O07|%s|\n", out;
q=(Pow(0.0,0.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"O08|%s|\n", out;
q=(Pow(0.0,-1.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"O09|%s|\n", out;
q=(Pow(2.0,1000.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"O10|%s|\n", out;
q=(Trunc(-0.5))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"O11|%s|\n", out;
q=(Floor(-0.5))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"O12|%s|\n", out;
q=(Round(-0.5))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"O13|%s|\n", out;
q=(Abs(-0.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"O14|%s|\n", out;
q=(Log10(0.0))(I64);
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"O15|%s|\n", out;
Exit;
