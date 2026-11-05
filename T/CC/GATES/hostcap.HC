/* M37 the delegation probe: what the KERNEL's format engine prints for the
 * directive spellings lite could hand it, from ONE argument slot.  Measured so
 * lite can hand over a directive instead of reimplementing float printing.  The
 * value enters as the ADDRESS of an I64 holding the bit pattern (T:/STRPRINT.HC
 * `case 'f':` reads `argv[cur_arg++](F64)` at :525-527), so no decimal parsing
 * is involved: these are the bits the engine formats.
 *
 * Rows are  K <value-row> <directive-row> <StrLen(text)> [<text>]  one line
 * each.  The two row numbers index bits[] and fmts[] below; the length comes
 * from the engine's own output, so a ceiling claim never depends on how a host
 * tool counts.  Line-buffer the run (stdbuf -oL): stdout is block-buffered and
 * a faulting process loses its tail.  The run ends with DONE; a run that stops
 * early printed fewer than 18x15 = 270 rows, and that is the failure to look
 * for here.
 *
 * WHY THESE 15 AND NOT MORE, all measured with the same one-slot call:
 *   `%e` and `%g` DO work at argc=1 -- both spell 1.5 as `1.50000000e0`, 12
 *     characters, note `e0` where C writes `e+00` -- and `%E` and `%G` print
 *     NOTHING (length 0).  They are not delegated because their rendering is a
 *     different conversion, not because they are unreachable.
 *   `%s` DIES: it dereferences the slot as a U8*, and the slot holds a bit
 *     pattern, so the 1.5 pattern as a pointer faults inside OutStr
 *     (T:/STRPRINT.HC:27) and the run drops into the debugger.  That is what a
 *     28-directive version of this probe looked like when it stopped after 48
 *     of 504 rows.  Not a throw: try/catch catches a HolyC `throw` but not this.
 *   `%*` is unreachable from HolyC at all: a varargs call site here accepts at
 *     most ONE argument beyond the named ones, and two is a parse error
 *     whatever the arguments -- see hostcapvararg.HC.  The engine reads a `*`
 *     from the NEXT argv slot (T:/STRPRINT.HC:250-254 for a width, :261-265 for
 *     a precision) and throws `StrPrint` when there is none, so a `*` is dropped
 *     from the text lite composes.
 *   `%d %x %c` DO format the slot (as an integer), but they are C's integer
 *     conversions, which lite lowers itself; `%w` is TempleOS's own quad spelling.
 *   `%lf` prints the literal text `lf` -- the engine has no `l` modifier.
 *
 * The buffer is 512 bytes and StrPrintJoin is given NO length, so this probe can
 * only see renderings shorter than that.  rows.txt is the durable record of the
 * narrower 38x9 grid that the fixture and the sweep are built from.
 */
I64 bits[18];
U8 *fmts[15];
U8 buf[512];
U8 *p;
I64 i, k, q;
bits[0] = 4609434218613702656;		//1.5
bits[1] = -4610560118520545280;		//-2.5
bits[2] = 4756540486875873280;		//1e10
bits[3] = 4876203697187506176;		//1e18
bits[4] = 4891288408196988160;		//1e19
bits[5] = 9094988921128908188;		//1e300
bits[6] = 4621819117588971520;		//10.0
bits[7] = 0;		//+0.0
bits[8] = -4382002437431492608;		//-2^52
bits[9] = 4607182418800017408;		//1.0
bits[10] = 9218868437227405312;		//+inf
bits[11] = -4503599627370496;		//-inf
bits[12] = 9221120237041090560;		//nan(default QNaN)
bits[13] = -2251799813685248;		//-nan
bits[14] = 9218868437227405311;		//DBL_MAX
bits[15] = 4591870180066957722;		//0.1
bits[16] = 4599676419421066581;		//1/3
bits[17] = 4786511204640095437;		//999999999999.9
fmts[0] = "%f";
fmts[1] = "%.0f";
fmts[2] = "%.1f";
fmts[3] = "%.6f";
fmts[4] = "%.15f";
fmts[5] = "%.20f";
fmts[6] = "%10.3f";
fmts[7] = "%010.3f";
fmts[8] = "%-10.3f";
fmts[9] = "%+f";
fmts[10] = "% f";
fmts[11] = "%#f";
fmts[12] = "%10f";
fmts[13] = "%-8f";
fmts[14] = "%100.50f";
for (i = 0; i != 18; i++)
{
  q = bits[i];
  for (k = 0; k != 15; k++)
  {
    MemSet(buf, 0, 512);
    p = StrPrintJoin(buf, fmts[k], 1, &q);
    "K %d %d %d [%s]\n", i, k, StrLen(buf), buf;
  }
}
"DONE %d values %d directives\n", 18, 15;
Exit;
