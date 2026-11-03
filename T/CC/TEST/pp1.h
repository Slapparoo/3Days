/* #ifndef/#define/#endif guard: including this file twice must NOT
 * redefine helper1 (the second pass sees PP1_H defined and skips).
 * SEVEN travels to every includer. */
#ifndef PP1_H
#define PP1_H
#define SEVEN 7
int helper1()
{
  return SEVEN;
}
#endif
