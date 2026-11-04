/* M33: the header reached by a PLAIN `#include "contaux.h"` from the main
 * file, because a quoted name is looked for in the directory of the file that
 * contains the directive before it is looked for in the header home.
 *
 * It is an INCLUDER too, and that is the point of the name below: `leafh.h` is
 * reached relative to THIS file's directory, so the `leafb.h` it pulls in is
 * the one beside IT, in sub/, and not the `leafb.h` lying beside the main
 * file — which defines the same macro to a different value precisely so the
 * answer says which rule ran. */
#define CAUX 7
#include "sub/leafh.h"
