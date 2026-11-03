/* M16 conditional operator: value selection, right-associativity,
 * unevaluated arms (side effects only on the taken path), pointer and
 * struct-pointer arms keeping their type (and tags) through ->,
 * ternaries inside parens/comparisons/sizeof's silent parse. Each
 * tot=tot+ line is one numbered check; main returns that count plus
 * the always-one closer. Expect 14. */
struct pt { int x; struct pt *next; };
int sideg;
int bump()
{
  sideg=sideg+1;
  return sideg;
}
int main()
{
  int tot;
  int a;
  char *p;
  char *q;
  struct pt p1;
  struct pt p2;
  tot=0;
  a=1;
  tot=tot+((1?7:9)==7);                          /*  1 true arm */
  tot=tot+((0?7:9)==9);                          /*  2 false arm */
  tot=tot+((0?1:(1?2:3))==2);                    /*  3 right-assoc */
  tot=tot+((1?(0?4:5):6)==5);                    /*  4 nested in middle arm */
  sideg=0;
  a=bump()?100:200;                              /*  5 condition side effect runs once */
  tot=tot+((a==100)&&(sideg==1));
  sideg=0;
  a=0?bump():200;                                /*  6 untaken arm never evaluated */
  tot=tot+((a==200)&&(sideg==0));
  p="abc";
  q="xyz";
  p=(0?p:q);
  tot=tot+(*p=='x');                             /*  7 pointer arms */
  p1.x=5;
  p2.x=6;
  p1.next=&p2;
  tot=tot+(((a?&p1:&p2)->x==5));                 /*  8 struct-tag arms keep -> */
  tot=tot+(((a?p1.next:p2.next)==&p2));          /*  9 arm through member pointer */
  tot=tot+(sizeof(a?1:2)==8);                    /* 10 silent parse, int arms */
  tot=tot+(sizeof(a?p:q)==8);                    /* 11 silent parse, pointer arms */
  a=0;
  a=(a?1:(a?2:(a?3:4)));                         /* 12 chained fallbacks */
  tot=tot+(a==4);
  tot=tot+(((a==4?1:0)==1));                     /* 13 ternary inside comparison */
  return tot+1;
}
