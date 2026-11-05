/* WHY A LITE CALL CAN ONLY EVER OFFER ONE ARGUMENT SLOT.  Read-and-not-run:
 * this file is deliberately CODE-FREE, because the fact it records is a PARSE
 * limit and running it would only produce the error.
 *
 * Measured on this tree (3Days, `-c`, stdbuf -oL), with q holding 1.5's bit
 * pattern and w = 12:
 *
 *   StrPrintJoin(b, "%f",  1, &q)   ->  parses, prints `2`
 *   StrPrintJoin(b, "%d",  1, &q)   ->  parses, prints the QUAD as decimal
 *   StrPrintJoin(b, "%10d", 2, &w, &q)  ->  ERROR: Missing ')' at ","
 *   StrPrintJoin(b, "%10d", 3, &w, &q, &q) ->  the same parse error
 *   StrPrintJoin(b, "%10d", 2, 7, q)    ->  the same parse error with PLAIN
 *                                            values, so it is not about `&`
 *
 * A HolyC call site of a varargs FFI function accepts at most ONE argument
 * beyond the named ones.  Two is a compile error, whatever the arguments are.
 * That is a property of THIS dialect's compiler, not of the engine: the engine's
 * signature is `StrPrintJoin(dst, fmt, I64 argc, I64 *argv)`, an explicit count
 * plus an array, and prelude C can and does call it with a count of 1 because C
 * takes the address of a local quad.  HolyC syntax cannot express a 2-slot call
 * at all, so it is NOT a route to a `*` field width: the width and the value
 * would need two slots, and the second one does not parse.
 *
 * The practical consequence for the delegation: every directive lite hands over
 * is measured, or refused, at argc=1 with the value in the single slot.  A
 * directive whose conversion consumes that slot gets the value; a directive that
 * would consume a SECOND slot is unreachable here rather than dangerous, and
 * `%*` is dropped from the composed text for that reason plus the engine's own
 * `cur_arg>=argc` throw at T:/STRPRINT.HC:252 and :263.
 */
Exit;
