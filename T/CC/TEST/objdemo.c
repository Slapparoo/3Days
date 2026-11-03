int counter=3;
char *msg="obj";
int add(int a,int b)
{
  return a+b;
}
int main(int argc,char *argv[])
{
  int i;
  int t=0;
  for (i=0;i<argc;i++)
    t=t+argc;	//argc-sensitive: a reload with a new argc must show it
  printf("%s %d %d\n",msg,add(counter,4),t);
  return counter*100+add(2,3)+argc;
}
