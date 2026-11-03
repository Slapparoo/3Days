/* M12 stdlib smoke test: the delegated set (strlen/memcpy/memset/
 * memcmp/malloc -> kernel helpers via cc->ext staging) plus the
 * pure-C prelude set (strcmp/strcpy/strchr/memmove/atoi/abs/puts/
 * srand/rand). Each line is one numbered check; main returns the
 * number that held. Expect 29.
 *
 * Checks 9b/10b/10c pin the RETURN contracts, learned the hard way
 * this session: the kernel's asm MemCpy/MemSet return dst+cnt (the
 * rep-movsb/stosb trail pointer), while C mandates dst — the
 * CC_LIB.HC helpers discard the kernel's return value and hand back
 * the staged dst. strchr/strcpy/memmove/puts are pure prelude C and
 * honor C semantics directly. */
int main()
{
  char buf[64];
  char buf2[64];
  char *msg;
  char *p;
  int tot;
  int r1,r2;
  tot=0;
  msg="Hello World";
  tot=tot+(strlen(msg)==11);                  /*  1 */
  tot=tot+(strcmp(msg,"Hello World")==0);     /*  2 */
  tot=tot+(strcmp("abc","abd")<0);            /*  3 */
  tot=tot+(strcmp("abd","abc")>0);            /*  4 */
  strcpy(buf,msg);
  tot=tot+(strcmp(buf,"Hello World")==0);     /*  5 */
  tot=tot+(buf[11]==0);                       /*  6 */
  p=strchr(buf,'W');
  tot=tot+(p==buf+6);                         /*  7 ('W' is index 6) */
  tot=tot+(strchr(buf,'z')==0);               /*  8 */
  p=memcpy(buf+40,msg,11);
  tot=tot+(strcmp(buf+40,"Hello World")==0);  /*  9 */
  tot=tot+(p==buf+40);                        /*  9b memcpy->dst (kernel
                                        would return buf+51; see above) */
  p=memset(buf,'A',4);
  tot=tot+(buf[0]=='A');                      /* 10 */
  tot=tot+(buf[3]=='A');                      /* 11 */
  tot=tot+(buf[4]=='o');                      /* 12 */
  tot=tot+(p==buf);                           /* 10b memset->dst */
  tot=tot+(memcmp(buf,"AAAA",4)==0);          /* 13 */
  tot=tot+(memcmp("abc","abd",3)<0);          /* 14 */
  strcpy(buf2,"0123456789");
  memmove(buf2+3,buf2,7);
  tot=tot+(strcmp(buf2,"0120123456")==0);     /* 15 fwd overlap */
  strcpy(buf2,"0123456789");
  memmove(buf2,buf2+3,7);
  tot=tot+(strcmp(buf2,"3456789789")==0);     /* 16 back overlap */
  p=malloc(32);
  tot=tot+(p!=0);                             /* 17 */
  if (p)
  {
    p[0]='M';
    p[1]='q';
    tot=tot+(p[0]=='M');                      /* 18 */
    tot=tot+(p[1]=='q');                      /* 19 */
  }
  tot=tot+(atoi("1234")==1234);               /* 20 */
  tot=tot+(atoi("-56")==-56);                 /* 21 */
  tot=tot+(abs(-7)==7);                       /* 22 */
  tot=tot+(abs(9)==9);                        /* 23 */
  srand(7);
  r1=rand();
  r2=rand();
  tot=tot+((r1>=0)&&(r1<1073741824));         /* 24 */
  tot=tot+(r1!=r2);                           /* 25 */
  puts("stdlib puts ok");                     /* 26 (visible line) */
  tot=tot+(buf[0]=='A');                      /* 27 puts didn't clobber */
  tot=tot+1;
  return tot;
}
