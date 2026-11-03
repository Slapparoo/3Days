/* M6c streaming: every line is flushed at its '\n' through the HolyC
 * helper, and the program ENDS on a newline, so the buffer must be
 * completely empty (count 0) when main returns — the harness asserts
 * that. The TAIL42 marker must appear in the streamed output. */
int main()
{
  int i;
  for (i=0;i!=200;i++)
    printf("stream line %d aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n",i);
  printf("TAIL42\n");
  return 7;
}
