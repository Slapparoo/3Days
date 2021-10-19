//ERROR TESTS
//Redefinition
#define I64 I64i
U0 Foo(I64 _1,I64 _2=3) {
  I64 a=10;
  I64 a=11;
}
//Lacking dft arg
Foo();
//Missing name
Foo(potato);

//Print weirness
class Struct {
  I64i a,b,c;
} S;
"%d\n",S;

//F64 bitwise mismatch
U0 Foo(I64 _1,I64 _2=3) {
  I64i a=10;
  F64 b=2.3;
  a&b;
}

//Incompat Transfer
S=1;

//Strange operators
!S;
~S;
S+S;
*S;

//Strange func call
S("Taods");

// Empty varg
U0 Vargs(...) {
}
Vargs(,,,);


//Strane If Conditon
if(S) {}

//Strange For Condition
for(;S;) {}

//Stange Do Conditon
do {}while(S);

//STrange While Conditon
while(S);

//Case overlap
switch(1) {
  case 1 ... 3:
  case 2 ... 4:
}

//STrange Switch conditon
switch(S) {
  default:
}

//Global label
GLOBAL:
//Global jump
goto GLOBAL;

//LAbel repear
U0 Repeat() {
  A:
  A:
}


//Union bigger than base type
I64 union Big {
  I64 v[8];
};

#define x x
x
