/* M12.1 sprintf engine: runtime-interpreted format, writes land in the
 * caller's buffer, the cc->out stream is untouched (the trailing puts
 * proves both: the formatted text arrives exactly once). %d %s %c %x %%
 * are pinned; the return value is the written length.  Checks 12-14 are
 * M36's: the signed-quad edges, and %x reading the QUAD and not the
 * magnitude.  Expect 15 — the value gcc -std=gnu99 gives this same file
 * with its numeric directives widened to the quad reading (%lld/%llx),
 * which is the only spelling where C's varargs carry the same values. */
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
  tot=tot+(sprintf(b,"%d",-2147483648)==11);         /*  9 the 32-bit edge, */
  f="%d!";
  n=sprintf(b,f,7);                                  /* runtime format: */
  tot=tot+((n==2)&&(strcmp(b,"7!")==0));             /* 10 the point of   */
  tot=tot+(sprintf(b,"s=[]")==4);                    /* 11 no-conversion */
  /* 12, 13 and 14 are M36's: the signed-quad edges, and %x's reading */
  n=sprintf(b,"%d",-9223372036854775807-1);         /* 12 the edge that */
  tot=tot+((n==20)&&(strcmp(b,"-9223372036854775808")==0));  /* printed "-(" */
  n=sprintf(b,"%d %d",-9223372036854775806,9223372036854775807);  /* 13 and the */
  tot=tot+((n==40)&&(strcmp(b,"-9223372036854775806 9223372036854775807")==0));
  n=sprintf(b,"%x %x %x",-1,-9223372036854775807-1,4294967296);  /* 14 %x is the */
  tot=tot+((n==43)&&(strcmp(b,"ffffffffffffffff 8000000000000000 100000000")==0));
  puts(b);                                           /* 15 visible text  */
  tot=tot+1;
  return tot;
}
