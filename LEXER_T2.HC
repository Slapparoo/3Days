class Incompat1 {
  I64 a;
} A;
class Incompat2 {
  I64 b;
} B;
A=B;
U0 Excess(I64 a,I64 b) {
}
Excess(1,2,3);

#define A FOO
goto A;
