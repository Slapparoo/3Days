/* M38 float evidence, in two parts.  Run:
 *   ./3d_loader -t T -c T:/CC/GATES/mathulp.HC
 *
 * A. R lines show what the KERNEL PRINTER does with doubles whose exact decimal
 *    value is short and known.  The first eight are 1.0, 2.0, 2.5, 3.0, 0.5,
 *    0.25, 1024 and 1000: a correct printer must render those digit for digit.
 *    They are given as BIT PATTERNS, so no decimal scanner is involved and
 *    nothing but the printer is being measured.  What it does is render them
 *    several ulps LOW -- 1.0 comes out 0.99999999999999760 and 1024 comes out
 *    1023.99999999999888000.  That is a property of the engine this compiler
 *    hands every `%f` to (M37 delegated it whole), not of sqrt or pow or log10,
 *    and it means a `%f` rendering cannot be read as a value.  These lines are
 *    why PLAN.md stopped quoting renderings as ulp differences.  pi, pi/2 and e
 *    are here for context; their exact decimals are not short.
 *
 * B. U lines score the kernel math ENTRIES instead.  Each prints
 *    `kernel answer quad - reference quad` as an integer, which inside one
 *    binade IS an ulp count: 0 means the kernel answered the correctly-rounded
 *    double.  The reference column is T:/CC/GATES/mathref.txt -- the real value
 *    to 60 digits and its nearest double, never a C library's answer.  got= and
 *    ref= are printed as plain decimals so the subtraction can be re-done by
 *    hand.  N lines cover what needs no reference arithmetic at all: how a NaN
 *    and an infinity render, whether the kernel's two routes to sqrt(2) agree,
 *    and what Round does with a tie.
 */
I64 q;
U8 out[256];
U8 f1[16];

StrCpy(f1,"%.17f");

q=0x3FF0000000000000;
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"R 1.0 [%s]\n", out;
q=0x4000000000000000;
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"R 2.0 [%s]\n", out;
q=0x4004000000000000;
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"R 2.5 [%s]\n", out;
q=0x4008000000000000;
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"R 3.0 [%s]\n", out;
q=0x3FE0000000000000;
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"R 0.5 [%s]\n", out;
q=0x3FD0000000000000;
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"R 0.25 [%s]\n", out;
q=0x4090000000000000;
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"R 1024 [%s]\n", out;
q=0x408F400000000000;
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"R 1000 [%s]\n", out;
q=0x400921FB54442D18;
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"R pi [%s]\n", out;
q=0x3FF921FB54442D18;
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"R pi/2 [%s]\n", out;
q=0x4005BF0A8B145769;
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"R e [%s]\n", out;

q=(Sqrt(2.0))(I64);
"U sqrt(2) diff=%d got=%d ref=%d\n",
  q - 0x3FF6A09E667F3BCD, q, 0x3FF6A09E667F3BCD;
q=(Pow(2.0,0.5))(I64);
"U pow(2,.5) diff=%d got=%d ref=%d\n",
  q - 0x3FF6A09E667F3BCD, q, 0x3FF6A09E667F3BCD;
q=(Sqrt(4.0))(I64);
"U sqrt(4) diff=%d got=%d ref=%d\n",
  q - 0x4000000000000000, q, 0x4000000000000000;
q=(Sqrt(0.25))(I64);
"U sqrt(.25) diff=%d got=%d ref=%d\n",
  q - 0x3FE0000000000000, q, 0x3FE0000000000000;
q=(Pow(2.0,10.0))(I64);
"U pow(2,10) diff=%d got=%d ref=%d\n",
  q - 0x4090000000000000, q, 0x4090000000000000;
q=(Pow(10.0,3.0))(I64);
"U pow(10,3) diff=%d got=%d ref=%d\n",
  q - 0x408F400000000000, q, 0x408F400000000000;
q=(Pow(-2.0,0.5))(I64);
"U pow(-2,.5) diff=%d got=%d ref=%d\n",
  q - 0x3FF6A09E667F3BCD, q, 0x3FF6A09E667F3BCD;
q=(Log10(10.0))(I64);
"U log10(10) diff=%d got=%d ref=%d\n",
  q - 0x3FF0000000000000, q, 0x3FF0000000000000;
q=(Log10(1000.0))(I64);
"U log10(1e3) diff=%d got=%d ref=%d\n",
  q - 0x4008000000000000, q, 0x4008000000000000;
q=(Log10(1000000.0))(I64);
"U log10(1e6) diff=%d got=%d ref=%d\n",
  q - 0x4018000000000000, q, 0x4018000000000000;
q=(Exp(1.0))(I64);
"U exp(1) diff=%d got=%d ref=%d\n",
  q - 0x4005BF0A8B145769, q, 0x4005BF0A8B145769;
q=(Exp(0.0))(I64);
"U exp(0) diff=%d got=%d ref=%d\n",
  q - 0x3FF0000000000000, q, 0x3FF0000000000000;
q=(Exp(1000.0))(I64);
"U exp(1000) diff=%d got=%d ref=%d\n",
  q - 0x7FF0000000000000, q, 0x7FF0000000000000;
q=(Ln(1.0))(I64);
"U ln(1) diff=%d got=%d ref=%d\n",
  q - 0x0000000000000000, q, 0x0000000000000000;
q=(Ln(2.7))(I64);
"U ln(2.7) diff=%d got=%d ref=%d\n",
  q - 0x3FEFC8B7F138BDEA, q, 0x3FEFC8B7F138BDEA;
q=(ACos(0.5))(I64);
"U acos(.5) diff=%d got=%d ref=%d\n",
  q - 0x3FF0C152382D7366, q, 0x3FF0C152382D7366;
q=(ASin(0.5))(I64);
"U asin(.5) diff=%d got=%d ref=%d\n",
  q - 0x3FE0C152382D7366, q, 0x3FE0C152382D7366;
q=(ATan(1.0))(I64);
"U atan(1) diff=%d got=%d ref=%d\n",
  q - 0x3FE921FB54442D18, q, 0x3FE921FB54442D18;
q=(Sin(1.0))(I64);
"U sin(1) diff=%d got=%d ref=%d\n",
  q - 0x3FEAED548F090CEE, q, 0x3FEAED548F090CEE;
q=(Cos(1.0))(I64);
"U cos(1) diff=%d got=%d ref=%d\n",
  q - 0x3FE14A280FB5068C, q, 0x3FE14A280FB5068C;
q=(Tan(1.0))(I64);
"U tan(1) diff=%d got=%d ref=%d\n",
  q - 0x3FF8EB245CBEE3A6, q, 0x3FF8EB245CBEE3A6;
q=(Sinh(1.0))(I64);
"U sinh(1) diff=%d got=%d ref=%d\n",
  q - 0x3FF2CD9FC44EB982, q, 0x3FF2CD9FC44EB982;
q=(Cosh(1.0))(I64);
"U cosh(1) diff=%d got=%d ref=%d\n",
  q - 0x3FF8B07551D9F550, q, 0x3FF8B07551D9F550;
q=(Floor(2.7))(I64);
"U floor(2.7) diff=%d got=%d ref=%d\n",
  q - 0x4000000000000000, q, 0x4000000000000000;
q=(Ceil(2.1))(I64);
"U ceil(2.1) diff=%d got=%d ref=%d\n",
  q - 0x4008000000000000, q, 0x4008000000000000;
q=(Trunc(2.7))(I64);
"U trunc(2.7) diff=%d got=%d ref=%d\n",
  q - 0x4000000000000000, q, 0x4000000000000000;
q=(Trunc(-2.7))(I64);
"U trunc(-2.7) diff=%d got=%d ref=%d\n",
  q - 0xC000000000000000, q, 0xC000000000000000;
q=(Round(2.5))(I64);
"U round(2.5) diff=%d got=%d ref=%d\n",
  q - 0x4008000000000000, q, 0x4008000000000000;
q=(Round(3.5))(I64);
"U round(3.5) diff=%d got=%d ref=%d\n",
  q - 0x4010000000000000, q, 0x4010000000000000;
q=(Abs(-2.5))(I64);
"U fabs(-2.5) diff=%d got=%d ref=%d\n",
  q - 0x4004000000000000, q, 0x4004000000000000;

q=0x7FF8000000000000;
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"N positive quiet NaN renders [%s]
", out;
q=0xFFF8000000000000;
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"N negative quiet NaN renders [%s]
", out;
q=0x7FF0000000000000;
MemSet(out,0,256);
StrPrintJoin(out,f1,1,&q);
"N +inf renders [%s]
", out;
q=(Pow(2.0,0.5))(I64)-(Sqrt(2.0))(I64);
"N pow(2,0.5) minus sqrt(2) = %d ulp (two kernel routines)
", q;
q=(Round(2.5))(I64);
"N round(2.5) quad = %d  and 2.0 is %d  and 3.0 is %d
",
  q, 0x4000000000000000, 0x4008000000000000;
q=(Round(3.5))(I64);
"N round(3.5) quad = %d  and 3.0 is %d  and 4.0 is %d
",
  q, 0x4008000000000000, 0x4010000000000000;
Exit;
