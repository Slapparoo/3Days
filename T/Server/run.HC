#define WIKI_EDIT"/EDIT"
#define WIKI_ROOT "/Wiki/"
#define WIKI_NAME "TempleOS Tiki"
#define WIKI_HOME "/"
#define WIKI_INDEX "/INDEX"
#define WIKI_SEARCH "/FIND"
#define WIKI_PREVIEW "/Wiki/Preview"
#define WIKI_UPLOAD "/UPLOAD"
#define WIKI_BACKUP "/WikiBackups/"
#define WIKI_CHANGES "/CHANGES"
#define WIKI_CHANGES_FOR_FILE "/FCHANGE"
#define WIKI_RESTORE "/RESTORE" //Takes FILE and REVISION 
if(!FileFind(WIKI_ROOT))
  DirMk(WIKI_ROOT);
if(!FileFind(WIKI_PREVIEW))
  DirMk(WIKI_PREVIEW);
if(!FileFind(WIKI_BACKUP))
  DirMk(WIKI_BACKUP);
#include "utils.HC"
#include "http.HC"
#include "header_footer.HC"
#include "find.HC"
#include "filter.HC"
#include "diff.HC"
#include "filter.HC"
#include "backup.HC"
#include "edit.HC"
#include "index.HC"
#include "gen_html.HC";