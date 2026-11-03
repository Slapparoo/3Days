/* M14 control-flow completion: switch/case/default (grouped labels,
 * fallthrough, break-belongs-to-switch, continue-belongs-to-enclosing
 * loop, default anywhere, char/negative/int32-overflow case constants,
 * no-match fall-out, empty switch) and do-while (basic, break,
 * continue-to-condition, braced and bare bodies). The count lives in
 * CC_TEST.HC. Expect 20. */
int klass(int v)
{
  switch (v)
  {
    case 1:
      return 11;
    case 2:
    case 3:
      return 23;
    default:
      return 90;
  }
}
int fallv(int v)
{
  int a;
  a=0;
  switch (v)
  {
    case 1:
      a=1;
    case 2:
      a=a+2;
      break;
    case 3:
      a=30;
  }
  return a;
}
int chklass(int c)
{
  switch (c)
  {
    case 'a':
      return 1;
    case 'b':
      return 2;
  }
  return 0;
}
int negv(int v)
{
  switch (v)
  {
    case -5:
      return 1;
  }
  return 0;
}
int bigc(int v)
{
  switch (v)
  {
    case 5000000000:
      return 1;
  }
  return 0;
}
int deffirst(int v)
{
  switch (v)
  {
    default:
      return 7;
    case 2:
      return 2;
  }
}
int loopmix()
{
  int i;
  int s;
  s=0;
  i=0;
  while (i<4)
  {
    i=i+1;
    switch (i)
    {
      case 1:
        break;
      case 2:
        continue;
      case 3:
        s=s+100;
    }
    s=s+1;
  }
  return s;
}
int main()
{
  int tot;
  int i;
  int d;
  tot=0;
  tot=tot+(klass(1)==11);                         /*  1 */
  tot=tot+(klass(2)==23);                        /*  2 grouped labels */
  tot=tot+(klass(3)==23);                        /*  3 */
  tot=tot+(klass(99)==90);                       /*  4 default */
  tot=tot+(fallv(1)==3);                         /*  5 fallthrough into case 2 */
  tot=tot+(fallv(2)==2);                         /*  6 direct case 2 + break */
  tot=tot+(fallv(3)==30);                        /*  7 */
  tot=tot+(chklass('a')==1);                     /*  8 char cases */
  tot=tot+(chklass('c')==0);                     /*  9 no match no default */
  tot=tot+(negv(-5)==1);                         /* 10 negative case */
  tot=tot+(bigc(5000000000)==1);                 /* 11 case > int32 */
  tot=tot+(bigc(3)==0);                          /* 12 */
  tot=tot+(deffirst(2)==2);                      /* 13 default before cases */
  tot=tot+(deffirst(5)==7);                      /* 14 */
  tot=tot+(loopmix()==103);                      /* 15 break/continue/fall-out */
  i=0;
  do
  {
    i=i+1;
  } while (i<3);
  tot=tot+(i==3);                                /* 16 do-while basic */
  i=0;
  do
  {
    i=i+1;
    if (i==2)
      break;
  } while (1);
  tot=tot+(i==2);                                /* 17 break exits do */
  i=0;
  d=0;
  do
  {
    i=i+1;
    if (i<3)
      continue;
    d=d+1;
  } while (i<4);
  tot=tot+((i==4)&&(d==2));                      /* 18 continue -> condition */
  i=0;
  do
    i=i+1;
  while (i<2);
  tot=tot+(i==2);                                /* 19 bare do body */
  switch (0)
  {
  }
  tot=tot+1;                                     /* 20 empty switch runs */
  return tot;
}
