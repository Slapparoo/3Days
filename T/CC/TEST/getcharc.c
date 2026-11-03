/* M12.1 getchar/fgetc/getc call SHAPE. Kernel GetKey blocks with no
 * console input, so headless runs gate this file compile+save only;
 * interactive consoles exercise it for real. */
int main()
{
  int c;
  c=getchar();
  c=c+fgetc(0);
  c=c+getc(1);
  if (c>0)
    return 2;
  return 1;
}
