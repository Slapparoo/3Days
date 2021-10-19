#include "FS.HC"
class Big {
  I64i a,b,c;
};
Big first64() {
  Big r={1,2,3};
  return r;
}
Big aonly=first64;
"AONLY:%d\n",aonly;
class Small {
  U8i a,b,c,d,e,f,g;
};
Small first7b() {
   Small r={1,2,3,4,5,6,7};
   return r;
}
Small x=first7b;
"SMALL:%d,%d,%d,%d,%d,%d,%d\n",x.a,x.b,x.c,x.d,x.e,x.f,x.g;
U0 PrnMul(...) {
  "ARGC:%d\n",argc;
  I64i i=0;
  while(i!=argc)
  "ARGV(%d)%d,",i,argv[i++];
  "\n";
}
PrnMul(aonly,aonly,aonly);
#define NULL (0(U0*))
"123:%d\n",StrLen("123");
#define FALSE 0
#define TRUE 1
#define U8 U8i
#define I8 I8i
U16i union U16
{
  I8i	i8[2];
  U8i	u8[2];
};

I16i union I16
{
  I8i	i8[2];
  U8i	u8[2];
};

U32i union U32
{
  I8i	i8[4];
  U8i	u8[4];
  I16	i16[2];
  U16	u16[2];
};

I32i union I32
{
  I8i	i8[4];
  U8i	u8[4];
  I16	i16[2];
  U16	u16[2];
};

U64i union U64
{
  I8i	i8[8];
  U8i	u8[8];
  I16	i16[4];
  U16	u16[4];
  I32	i32[2];
  U32	u32[2];
};

I64i union I64
{
  I8i	i8[8];
  U8i	u8[8];
  I16	i16[4];
  U16	u16[4];
  I32	i32[2];
  U32	u32[2];
};
I64 StaticT() {
  static I64 test;
  "STATIC++:%d\n",test++;
}
StaticT;
StaticT;
StaticT;
try {
  throw('TEST');
} catch {
  Fs->catch_except=1;
  "CUAGHT:%c\n",Fs->except_ch;
};
try {
  try {
    throw('TEST');
  } catch {
    "UNCUAGHT:%c\n",Fs->except_ch;
  };
} catch {
  "CUAGHT2:%c\n",Fs->except_ch;
  Fs->catch_except=1;
}
U0 PtrTest() {
  U8 *string="Potato";
  U8 **ptr=&string;
  "%s\n",*ptr;
}
PtrTest;
U0 GotoTest() {
  goto A;
  "gt FAIL\n";
  return;
  B:
  "gt SUCCESS \n";
  return;
  A:
  goto B;
}
GotoTest();
I64 Array0[]={1,2,3};
"%p,&%p,==%d\n",Array0,&Array0,Array0==&Array0;
U0 Equal(U0 *a,U0 *b) {
  "EQ:%d\n",a==b;
}
Equal(Array0,&Array0);
_import I64 union Int;
_import Def I64 Forward;
Int Asn() {
  Forward=10;
}
Int Def=0;
Asn();
"FORWARD:%d\n",Forward;
I64 Fib(I64 seq) {
  if(seq<2)
    return seq;
  return Fib(seq-1)+Fib(seq-2);
}
"FIB(32):%d\n",Fib(32);
I64 FibD(I64 seq) {
  //Debugger;
  if(seq<2)
    return seq;
  return FibD(seq-1)+FibD(seq-2);
}
"F(6),%d\n",FibD(6);
class ABC {
  I64i a,b,c;
};
U0 PrnTest() {
  ABC x={1,2,3};
  ABC y=x;
}
PrnTest;
F64 ToF64(I64 i) {
  return i;
}
I64 ToI64(F64 f) {
  return f;
}
"CONV:%d,%n\n",ToI64(2.3),ToF64(10);
U0 FooABC(I64 a=1,I64 b=2,I64 c=3) {
  "A:%d,B:%d,C:%d\n",a,b,c;
}
FooABC(-1,-2,-3);
FooABC(,,);
FooABC(100,,);
FooABC(,200,);
FooABC(,,300);
U64 i64=0;
i64.u8[1]=1;
"%d\n",i64;

I64i Array[3];
Array[0]=1;
Array[1]=2;
Array[2]=3;
"%d,%d,%d\n",Array[0],Array[1],Array[2];
I64i Array2[][]={{1},{1,2},{1,2,3}};
"%d,%d,%d\n",Array2[0][0],Array2[1][1],Array2[2][2];
class ABC {
	I64i a,b,c;
};
union DEF:ABC {
	I64i d,e,f;
};
DEF test;
test.a=10;
"%d\n",test.d;
I64i Add(I64i a,I64i b) {
	return a+b;
}
"%d\n",Add(1.1,3);
I64i main() {
	"HELLO WORLD:%d\n",10;
	I64i x=10;
	switch(x) {
		case 9:
		10;
		break;
		start:
		10;
		case :
		break;
		end:
		break;
	}
}
"HELLO %s\n","WORLD";
I64i x;x=10/2;
x+=10;
if(!x) {
	"TRUE\n";
} else {
	"FALSE\n";
}
while(x>=0) "WHILE:%d\n",x--;
for(x=10;x>=0;x--) "FOR:%d\n",x;
do {
	if(x==-1) {
		"-1\n";
	} else if(x==1) {
		"1\n";
	} else if(x==2) {
		"2\n";
	} else if(x==3) {
		"3\n";
	} else if(x==4) {
		"4\n";
	}
} while(++x<=4);
{
  I64i i=6;
	while(--i>=0)
  switch(i) {
		case 5: "FIVE\n"; break;
		case 2: "TWO\n"; break;
		case 1: "ONE\n"; break;
		case 0: "TOADS\n"; break;
		default:"f\n";
	}
}
{
I64i i;
for (i=0;i<10;i++)
	switch (i) {
		case 0: "Zero ";  break;
		case 2: "Two ";   break;
		case 4: "Four ";  break;
		start:
			"[";
			case 1: "One";  break;
			case 3: "Three";break;
			case 5: "Five"; break;
		end:
			"] ";
			break;
	}
'\n';
}
class abc {
	class {
		I64i a,b,c;
	};
};
abc var;
I64i *ptr=&var.b;
ptr[1]=10;
"ABC:%d\n",(&var)->c;
(&var)->c=123;
"ABC:%d\n",(&var)->c;
#include "BITS.HC"
#include "COMPRESS.HC"
#include "CHAR.HC"
#include "MATH.HC"
U0 WhatClass(U0 *c,U8 *lc=lastclass) {
  "ABC IS OF CLASS %s\n",lc;
}
WhatClass(&var);
