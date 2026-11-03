/* M16 goto/labels: backward gotos (loop idiom), forward gotos patched
 * by the later definition, exits from while/for/switch nests, stacked
 * labels sharing one position, label scope per function. Each tot=tot+
 * line is one numbered check; main returns that count plus the
 * always-one closer. Expect 10. */
int jumps(int v)
{
  int a;
  a=0;
  switch (v)
  {
    case 1:
      goto done;
    case 2:
      goto mid;
  }
  a=a+1;
mid:
  a=a+10;
done:
  return a;
}
int loopn()
{
  int i;
  int s;
  i=0;
  s=0;
loop:
  i=i+1;
  s=s+i;
  if (i<3)
    goto loop;
  return s;
}
int outof()
{
  int i;
  i=0;
  while (1)
  {
    i=i+1;
    if (i==2)
      goto out;
  }
out:
  return i*10;
}
int nest()
{
  int i;
  int j;
  int c;
  c=0;
  for (i=0;i<3;i++)
  {
    for (j=0;j<3;j++)
    {
      if ((i==1)&&(j==1))
        goto skip;
      c=c+1;
    }
  }
skip:
  return c;
}
int stack2()
{
  int a;
  int b;
  a=0;
  b=0;
  goto two;
one:
  b=b+1;
two:
  a=a+10;
  if (b==0)
    goto one;
  return a+b;
}
int mixout()
{
  int i;
  int s;
  s=0;
  for (i=0;i<5;i++)
  {
    switch (i)
    {
      case 2:
        goto bail;
      default:
        s=s+1;
    }
  }
bail:
  return s;
}
int main()
{
  int tot;
  tot=0;
  tot=tot+(jumps(1)==0);                         /*  1 forward out of switch */
  tot=tot+(jumps(2)==10);                        /*  2 forward into mid */
  tot=tot+(jumps(7)==11);                        /*  3 no match falls to mid */
  tot=tot+(loopn()==6);                          /*  4 backward goto loop */
  tot=tot+(outof()==20);                         /*  5 goto out of while(1) */
  tot=tot+(nest()==4);                           /*  6 goto out of nested fors */
  tot=tot+(stack2()==21);                        /*  7 stacked labels, re-entry */
  tot=tot+(mixout()==2);                         /*  8 goto out of switch-in-for */
  tot=tot+((jumps(2)+loopn())==16);              /*  9 across functions */
  return tot+1;
}
