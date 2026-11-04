/* M33: a DECOY with the same name as sub/leafb.h, lying in the main file
 * directory.  If a header ever resolved its own relative includes against the
 * directory of the file at the top of the include chain instead of its own,
 * LEAF would be 60 here and cont.c would fail. */
#define LEAF 60
