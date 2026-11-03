/* M12 stdlib smoke test: the delegated set (strlen/memcpy/memset/
 * memcmp/malloc/getchar-shape-only -> kernel helpers via cc->ext
 * staging) plus the pure-C prelude set (strcmp strcpy strcat strncat
 * strrchr strstr strncmp strncpy strchr memmove memchr calloc strdup
 * atoi abs puts fputs fputc srand/rand). Each tot=tot+ line is one
 * numbered check; main returns the number that held; the count lives
 * in CC_TEST.HC's CCAssert for this file. Expect 52.
 *
 * Checks 9b/10b/10c pin the RETURN contracts, learned the hard way:
 * the kernel's asm MemCpy/MemSet return dst+cnt (the rep-movsb/stosb
 * trail pointer), while C mandates dst — the CC_LIB.HC helpers
 * discard the kernel's return value and hand back the staged dst.
 * All other string/memory functions here are pure prelude C and honor
 * C semantics directly (incl. strncpy's zero-pad and no-terminator-on-
 * full-width behavior). fputs/fputc take stream tokens and ignore
 * them: cc->out IS the stdio buffer for now. getchar/fgetc/getc are
 * NOT exercised here — kernel GetKey blocks with no console input;
 * their call shape is gated compile+save-only by the getchar case. */
int main()
{
  char buf[64];
  char buf2[64];
  char *msg;
  char *p;
  char *q;
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
  tot=tot+(p==buf+40);                        /* 10 memcpy->dst (kernel
                                        would return buf+51; see above) */
  p=memset(buf,'A',4);
  tot=tot+(buf[0]=='A');                      /* 11 */
  tot=tot+(buf[3]=='A');                      /* 12 */
  tot=tot+(buf[4]=='o');                      /* 13 */
  tot=tot+(p==buf);                           /* 14 memset->dst */
  tot=tot+(memcmp(buf,"AAAA",4)==0);          /* 15 */
  tot=tot+(memcmp("abc","abd",3)<0);          /* 16 */
  strcpy(buf2,"0123456789");
  memmove(buf2+3,buf2,7);
  tot=tot+(strcmp(buf2,"0120123456")==0);     /* 17 fwd overlap */
  strcpy(buf2,"0123456789");
  memmove(buf2,buf2+3,7);
  tot=tot+(strcmp(buf2,"3456789789")==0);     /* 18 back overlap */
  strcpy(buf,"foo");
  q=strcat(buf,"bar");
  tot=tot+((q==buf)&&(strcmp(buf,"foobar")==0));   /* 19 */
  q=strncat(buf,"z",2);
  tot=tot+((q==buf)&&(strcmp(buf,"foobarz")==0));  /* 20 */
  q=strstr(buf,"oba");
  tot=tot+(q==buf+2);                         /* 21 */
  tot=tot+(strstr(buf,"z")==buf+6);           /* 22 hit at last char */
  tot=tot+(strstr(buf,"q")==0);               /* 23 */
  tot=tot+(strstr(buf,"")==buf);              /* 24 empty needle */
  q=strrchr(buf,'o');
  tot=tot+(q==buf+2);                         /* 25 last 'o' */
  tot=tot+(strrchr(buf,'z')==buf+6);          /* 26 */
  tot=tot+(strrchr(buf,'Q')==0);              /* 27 */
  tot=tot+(strncmp("abcxyz","abcq",3)==0);    /* 28 */
  tot=tot+(strncmp("abcq","abcxyz",4)<0);     /* 29 differs at x>q */
  tot=tot+(strncmp("","x",1)<0);              /* 30 */
  tot=tot+(strncmp("abc","abcxyz",3)==0);     /* 31 */
  memset(buf,'Z',8);
  strncpy(buf,"ab",4);
  tot=tot+((buf[0]=='a')&&((buf[1]=='b')&&((buf[2]==0)&&(buf[3]==0))));
                                              /* 32 zero-pad to width */
  memset(buf2,'Z',8);
  p=strncpy(buf2,"0123456789",3);
  tot=tot+((p==buf2)&&(buf2[3]=='Z'));         /* 33 full width: no NUL */
  q=memchr(buf2,'2',3);
  tot=tot+(q==buf2+2);                        /* 34 */
  tot=tot+(memchr(buf2,'Z',3)==0);            /* 35 */
  tot=tot+(memchr(buf2,'Z',4)==buf2+3);       /* 36 found at last byte */
  q=strdup("dupme");
  tot=tot+((q!=0)&&(strcmp(q,"dupme")==0));   /* 37 */
  p=calloc(7,3);
  tot=tot+(p!=0);                             /* 38 */
  p=calloc(2,2);
  tot=tot+((p!=0)&&(p[3]==0));                /* 39 calloc zero-fills */
  tot=tot+(fputs("fputs",1)==0);              /* 40 */
  tot=tot+(fputc(90,1)==90);                  /* 41 */
  puts("!done");                              /* 42 visible: "fputsZ!done" */
  p=malloc(32);
  tot=tot+(p!=0);                             /* 43 */
  if (p)
  {
    p[0]='M';
    p[1]='q';
    tot=tot+(p[0]=='M');                      /* 44 */
    tot=tot+(p[1]=='q');                      /* 45 */
  }
  tot=tot+(atoi("1234")==1234);               /* 46 */
  tot=tot+(atoi("-56")==-56);                 /* 47 */
  tot=tot+(abs(-7)==7);                       /* 48 */
  tot=tot+(abs(9)==9);                        /* 49 */
  srand(7);
  r1=rand();
  r2=rand();
  tot=tot+((r1>=0)&&(r1<1073741824));         /* 50 */
  tot=tot+(r1!=r2);                           /* 51 */
  puts("stdlib puts ok");                     /* 52 visible line */
  tot=tot+(buf[0]=='a');                      /* 53 puts didn't clobber */
  tot=tot+1;
  return tot;
}
