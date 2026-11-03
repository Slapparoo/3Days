/* M6c near-full guard: 1200 x 80 bytes with NO per-iteration newlines
 * cross the 64K buffer via the bounded-flush path (old code AND-wrapped
 * and silently lost everything past 64K — the streamed output must
 * contain every iteration's bytes). Ends with TAIL88 on its own line,
 * so the buffer must be empty (count 0) when main returns. */
int main()
{
  int i;
  for (i=0;i!=1200;i++)
    printf("0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");
  printf("\nTAIL88\n");
  return 8;
}
