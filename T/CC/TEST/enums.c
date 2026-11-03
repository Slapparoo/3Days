/* M20 enum + compile-time constant folding: enumerator tables (auto
 * increment, folded initializers, chained references), constants as
 * case labels/subscripts/array sizes/global initializers/comparison
 * operands, enum tags in declarations and sizeof, and adjacent string
 * literal concatenation. Expect 36. */
enum Color { RED, GREEN, BLUE=GREEN*2, SKY=BLUE+1 };
enum { NO, YES=1, MAYBE=YES+1 };
enum Days { MON, TUES, WEDNESDAY, THU, FRI, SAT, SUN };
enum Kind { PLAIN, FANCY=4 };
int folded=PLAIN+FANCY*10;
enum Kind named;
int counts[BLUE+1];
char label[MON+2];
int main()
{
  enum Color pick;
  enum Kind local;
  int *cp;
  int tot;
  char *s;
  tot=0;
  tot=tot+(RED==0);                            /*  1 auto-increment base */
  tot=tot+(GREEN==1);                          /*  2 */
  tot=tot+(BLUE==2);                           /*  3 initializer folds */
  tot=tot+(SKY==3);                            /*  4 chains from prior */
  tot=tot+((NO==0)&&(YES==1)&&(MAYBE==2));     /*  5 anonymous enum */
  tot=tot+(SUN==6);                            /*  6 seven enumerators */
  pick=SKY;
  tot=tot+(pick==3);                           /*  7 enum var = int model */
  tot=tot+(sizeof(pick)==8);                   /*  8 */
  tot=tot+(sizeof(RED)==8);                    /*  9 constants are quads */
  tot=tot+(sizeof(enum Color)==8);             /* 10 tag form */
  switch (pick)
  {
    case RED:
      tot=tot+0;
      break;
    case GREEN:
      tot=tot+0;
      break;
    default:
      tot=tot+1;                               /* 11 no constant matched */
      break;
  }
  switch (GREEN)
  {
    case GREEN:
      tot=tot+1;                               /* 12 enum value as case label */
      break;
    default:
      tot=tot+0;
      break;
  }
  tot=tot+(GREEN<BLUE);                        /* 13 constants in comparisons */
  tot=tot+((GREEN|SKY)==3);                    /* 14 */
  tot=tot+(counts[GREEN]==0);                  /* 15 array sized by enum, zeroed */
  cp=counts;
  cp[BLUE]=9;
  tot=tot+(counts[2]==9);                      /* 16 writable through a pointer */
  label[MON]='x';
  tot=tot+(label[0]=='x');                     /* 17 enum-indexed subscript */
  tot=tot+(folded==40);                        /* 18 global init folded (4*10) */
  named=FANCY;
  tot=tot+(named==4);                          /* 19 tag-typed global */
  local=PLAIN;
  tot=tot+((local+named)==4);                  /* 20 tag-typed local */
  s="ab" "cd";
  tot=tot+(strlen(s)==4);                      /* 21 adjacent literals concatenate */
  tot=tot+(s[2]=='c');                         /* 22 concatenated content */
  s="x" "y" "z";
  tot=tot+((s[0]=='x')&&(s[2]=='z')&&(strlen(s)==3));  /* 23 three-way */
  tot=tot+((1<<3)==8);                         /* 24 shift folds live */
  tot=tot+((- -7)==7);                         /* 25 unary chains */
  tot=tot+(~0==-1);                            /* 26 complement */
  tot=tot+((1&&0)==0);                         /* 27 logical and */
  tot=tot+((0||5)==1);                         /* 28 logical or */
  tot=tot+((5>3)==1);                          /* 29 relational */
  tot=tot+((2|4|8)==14);                       /* 30 or-chain */
  tot=tot+((7&3)==3);                          /* 31 and-chain */
  tot=tot+((9^5)==12);                         /* 32 xor */
  tot=tot+('a'=='\x61');                       /* 33 hex escape equivalence */
  tot=tot+((FANCY/2)==2);                      /* 34 constant division */
  tot=tot+((MON==SAT)==0);                     /* 35 equality fold */
  tot=tot+((BLUE-GREEN)==1);                   /* 36 subtraction fold */
  return tot+1;
}
