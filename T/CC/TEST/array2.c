/* M17 multi-dimensional arrays: 2-D int/char locals and globals,
 * row-scaling parameters (`int a[][3]`, `char t[][8]`, `struct g[][2]`),
 * byte-stride char[] parameters, rows decaying to element pointers,
 * 2-D sizeof totals, * a parenthesized decay that keeps its row scales intact. Expect 22. */
int m2[2][3];
char tab[2][8];
struct pt { int x; struct pt *next; };
int sumrow(int a[][3],int rows)
{
  int i;
  int s;
  s=0;
  for (i=0;i<rows;i++)
    s=s+(a[i][0]+a[i][1]+a[i][2]);
  return s;
}
int bytenum(char t[][8],int r)
{
  return atoi(t[r]);
}
int copy1(char s[])
{
  s[1]='Z';
  return strlen(s);
}
int gridsum(struct pt g[][2],int rows)
{
  int i;
  int j;
  int t;
  t=0;
  for (i=0;i<rows;i++)
    for (j=0;j<2;j++)
      t=t+g[i][j].x;
  return t;
}
int main()
{
  int i;
  int j;
  int tot;
  int x;
  int loc[2][3];
  char ct[3][4];
  char *ap[2];
  char *pp;
  char buf[8];
  struct pt grid[2][2];
  tot=0;
  i=0;
  while (i<2)
  {
    j=0;
    while (j<3)
    {
      m2[i][j]=i*10+j;
      loc[i][j]=i*10+j;
      j=j+1;
    }
    i=i+1;
  }
  tot=tot+(m2[1][2]==12);                       /*  1 global 2-D */
  tot=tot+((loc[1][2]==12)&&(loc[0][0]==0));    /*  2 local 2-D */
  tot=tot+(m2[0][1]+m2[1][0]==11);              /*  3 rows do not alias */
  tot=tot+(sumrow(m2,2)==36);                   /*  4 int[][3] rows: 3+33 */
  strcpy(tab[0],"123");
  strcpy(tab[1],"45");
  tot=tot+((tab[0][0]=='1')&&(tab[1][1]=='5')); /*  5 char rows, byte scale */
  tot=tot+(bytenum(tab,1)==45);                 /*  6 char[][8] param */
  ct[0][0]='a';
  ct[2][3]='z';
  tot=tot+((ct[0][0]=='a')&&(ct[2][3]=='z'));   /*  7 char[3][4] local */
  tot=tot+(sizeof(m2)==48);                     /*  8 2-D totals */
  tot=tot+(sizeof(ct)==12);
  i=m2[1];
  tot=tot+(*i==10);                             /* 10 row decays to int* */
  tot=tot+(strlen(tab[0])==3);                  /* 11 row as a string */
  ap[0]="hi";
  ap[1]="yo";
  tot=tot+(strcmp(ap[0],"hi")==0);              /* 12 pointer-array element */
  pp=ap[1];
  tot=tot+(pp[1]=='o');                         /* 13 then byte-indexed */
  tot=tot+(*(char *)(ap[0])=='h');              /* 14 byte deref via cast */
  strcpy(buf,"hi");
  tot=tot+((copy1(buf)==2)&&(buf[1]=='Z'));     /* 15 char[] param byte stride */
  grid[0][0].x=1;
  grid[0][1].x=2;
  grid[1][0].x=10;
  grid[1][1].x=20;
  tot=tot+(gridsum(grid,2)==33);                /* 16 struct[][2] param rows */
  tot=tot+((grid[1][0].x==10)&&(grid[0][1].next==0));
                                               /* 17 direct 2-D members */
  x=(m2)[0][2];
  tot=tot+(x==2);                               /* 18 paren-group keeps row decay */
  tot=tot+(sizeof(m2[1][1])==8);                /* 19 scalar element via expr */
  memset(tab[0],'x',8);
  tot=tot+(strlen(tab[0])==10);                 /* 20 row adjacency C-true */
  tot=tot+(sizeof(tab)==16);                    /* 21 char 2-D byte total */
  return tot+1;
}
