/* chained include + macro-body rescan: TWELVE expands to (SEVEN+FIVE),
 * whose tokens re-expand to (7+5). */
#include "T:/CC/TEST/pp1.h"
#define FIVE 5
#define TWELVE (SEVEN+FIVE)
