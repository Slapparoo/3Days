/* M12.1 sprintf engine: runtime-interpreted format, writes land in the
 * caller's buffer, the cc->out stream is untouched (the trailing puts
 * proves both: the formatted text arrives exactly once). %d %s %c %x %%
 * are pinned; the return value is the written length. Expect 12. */
int main()
{
  char b[64];
  char *f;
  int n;
  int tot;
  tot=0;
  n=sprintf(b,"x=%d|s=%s|c=%c|%%|h=%x\n",42,"ab",65,255);
  tot=tot+(strcmp(b,"x=42|s=ab|c=A|%|h=ff\n")==0);   /*  1 */
  tot=tot+(n==21);                                   /*  2 length */
  tot=tot+(b[21]==0);                                /*  3 terminator */
  n=sprintf(b,"n=%d",-9);
  tot=tot+((n==4)&&(strcmp(b,"n=-9")==0));           /*  4 */
  tot=tot+(sprintf(b,"plain")==5);                   /*  5 no conversions */
  tot=tot+(strcmp(b,"plain")==0);                    /*  6 */
  n=sprintf(b,"%x %d",16,0);
  tot=tot+((n==4)&&(strcmp(b,"10 0")==0));           /*  7 */
  n=sprintf(b,"%c%c",'a','b');
  tot=tot+((n==2)&&((b[0]=='a')&&(b[1]=='b')));      /*  8 */
  tot=tot+(sprintf(b,"%d",-2147483648)==11);         /*  9 widest %d */
  f="%d!";
  n=sprintf(b,f,7);                                  /* runtime format: */
  tot=tot+((n==2)&&(strcmp(b,"7!")==0));             /* 10 the point of   */
  tot=tot+(sprintf(b,"s=[]")==4);                    /* 11 no-conversion */
  puts(b);                                           /* 12 visible "7!"  */
  tot=tot+1;
  return tot;
}
