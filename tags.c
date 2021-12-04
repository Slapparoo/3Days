#include "3d.h"
void DumpTagsToFile(char *fn) {
  FILE *f=fopen(fn, "w");
  if(!f) return ;
  const char *key;
  map_iter_t miter;
  #ifndef BOOTSTRAPED
  miter=map_iter(&Lexer.macros);
  while(key=map_next(&Lexer.macros, &miter)) {
    CMacro *macro=map_get(&Lexer.macros,key);
    fprintf(f,"%s\t%s\t%ld;\tkind:m\n",macro->name,macro->fn,macro->line);
  }
  #endif
  miter=map_iter(&Compiler.types);
  while(key=map_next(&Compiler.types, &miter)) {
    CType **type=map_get(&Compiler.types,key);
    CMember member;
    int iter;
    if(type[0]->type==TYPE_CLASS) {
      fprintf(f,"%s\t%s\t%ld;\tkind:c\n",key,type[0]->cls.fn,type[0]->cls.ln);
      vec_foreach(&type[0]->cls.members, member, iter) {
        fprintf(f,"%s\t%s\t%ld;\tstruct:%s\n",member.name,member.fn,member.line,key);
      }
    } else if(type[0]->type==TYPE_UNION) {
      fprintf(f,"%s\t%s\t%ld;\tkind:u\n",key,type[0]->un.fn,type[0]->un.ln);
      vec_foreach(&type[0]->un.members, member, iter) {
        fprintf(f,"%s\t%s\t%ld;\tunion:%s\n",member.name,member.fn,member.line,key);
      }
    }
  }
  miter=map_iter(&Compiler.globals);
  while(key=map_next(&Compiler.globals, &miter)) {
    CVariable **var=map_get(&Compiler.globals,key);
    if(var[0]->isInternal) continue;
    if(var[0]->type->type==TYPE_FUNC) {
        fprintf(f,"%s\t%s\t%ld;\tkind:f\n",key,var[0]->fn,var[0]->line);
    } else {
        fprintf(f,"%s\t%s\t%ld;\tkind:v\n",key,var[0]->fn,var[0]->line);
    }
  }
  fclose(f);
}
