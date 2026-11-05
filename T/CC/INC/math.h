/* M38: <math.h>.  These names are PARSER builtins - CC_PARSE.HC's name
 * dispatch calls CCLibCallM, which calls a CC_LIB.HC helper, which makes ONE
 * call to the kernel entry - so the prototypes below are INERT for a call
 * through this header: nothing is linked and no library is involved.  They are
 * here so that a pointer to a builtin, or a use in a unit that reaches the
 * dispatch by another route, has a type.
 *
 * The mapping is the whole contract, and every target is the kernel's own:
 *   sqrt->Sqrt, fabs->Abs, sin->Sin, cos->Cos, tan->Tan, atan->ATan
 *   (T:/KERNELA.HH:4160-4183, `_intern`); asin->ASin, acos->ACos, sinh->Sinh,
 *   cosh->Cosh (:4343-4346, plain extern); exp->Exp, log->Ln, log10->Log10,
 *   pow->Pow, floor->Floor, ceil->Ceil, trunc->Trunc, round->Round
 *   (T:/KERNELB.HH:23-41, asm `_extern`).
 * These C names have NO kernel entry and are therefore NOT provided: tanh,
 * atan2, hypot, log2, frexp, ldexp, modf.  Where the kernel's VALUE differs
 * from C - and it does, in ways that are listed with their measurements in
 * T:/CC/PLAN.md - this header neither hides the difference nor emulates C:
 * lite delegates, and the delegated answer is the answer.
 */
#ifndef _MATH_H
#define _MATH_H
double sqrt(double x);
double fabs(double x);
double exp(double x);
double log(double x);
double log10(double x);
double pow(double x,double y);
double floor(double x);
double ceil(double x);
double trunc(double x);
double round(double x);
double sin(double x);
double cos(double x);
double tan(double x);
double atan(double x);
double asin(double x);
double acos(double x);
double sinh(double x);
double cosh(double x);
#endif
