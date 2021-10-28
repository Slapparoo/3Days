#pragma once
#include <stdint.h>
#include <stdlib.h>
#include "jitlib.h"
#include "jitlib-core.h"
typedef struct CMatrix {
  uint8_t **items;
  int itemCount;
  int type:1;
} CMatrix;
static CMatrix CreateMatrix(int items) {
  CMatrix mat;
  mat.itemCount=items;
  items+=16; //For extra registes
  mat.items=JIT_MALLOC(sizeof(uint8_t*)*items);
  int iter=items;
  while(--iter>=0) {
    mat.items[iter]=JIT_MALLOC(items);
    memset(mat.items[iter],0,items);
  }
  return mat;
}
static void Interfere(CMatrix *mat,int64_t a,int64_t b) {
  if(a>=mat->itemCount) abort();
  if(b>=mat->itemCount) abort();
  mat->items[a][b]=1;
  mat->items[b][a]=1;
}
typedef struct {
  uint8_t *found;
  unsigned type:1;
  int len;
} RFThunk;
static int ColoringIsBlacklisted(jit_value v) {
  return
    v==R_FP||
    v==R_OUT||
    v==R_IMM||
    v==FR_IMM;
}
static void SearchForRegs(int64_t key,void * found) {
  RFThunk *thunk=found;
  if(JIT_REG(key).type!=thunk->type) return;
  if(ColoringIsBlacklisted(key))
    return;
  int id=JIT_REG(key).id;
  if(id>=thunk->len+16) abort();
  thunk->found[id]=1;
}
static void GlobRegisters(CMatrix *mat,jit_set *live,int type) {
  uint8_t found[mat->itemCount+16];
  memset(found,0,mat->itemCount+16);
  RFThunk thunk={found,type,mat->itemCount};
  int var,iter;
  vec_foreach(&live->vec, var, iter) SearchForRegs(var, &thunk);
  int a,b;
  for(a=0;a!=mat->itemCount;a++) {
    for(b=0;b!=mat->itemCount;b++) {
      if(a==b) continue;
      if(mat->items[a][b]) continue;
      if(found[a]&&found[b]){
        Interfere(mat,a,b);
      }
    }
  }
}
static void CreateInterferenceGraphs(struct jit *jit,CMatrix *_igraph,CMatrix *_fgraph) {
  struct jit_func_info *info=jit_current_func_info(jit);
  CMatrix igraph=CreateMatrix(info->gp_reg_count);
  CMatrix fgraph=CreateMatrix(info->fp_reg_count);
  struct jit_op *ops=jit_op_first(jit->ops);
  for(;ops;ops=ops->next) {
    GlobRegisters(&igraph,ops->live_in,JIT_RTYPE_INT);
    GlobRegisters(&fgraph,ops->live_in,JIT_RTYPE_FLOAT);
    GlobRegisters(&igraph,ops->live_out,JIT_RTYPE_INT);
    GlobRegisters(&fgraph,ops->live_out,JIT_RTYPE_FLOAT);
    jit_set *ko;
    jit_set_addall(ko=jit_set_clone(ops->live_kill),ops->live_out);
    GlobRegisters(&igraph,ko,JIT_RTYPE_INT);
    GlobRegisters(&fgraph,ko,JIT_RTYPE_FLOAT);
    jit_set_free(ko);
  }
  *_igraph=igraph;
  *_fgraph=fgraph;
}
//https://www.geeksforgeeks.org/graph-coloring-set-2-greedy-algorithm/
static void ColorGraph(CMatrix *mat,int *colors) {
  int iter=mat->itemCount;
  if(!iter) return;
  while(--iter>=0) {
    colors[iter]=-1;
  }
  colors[0]=0;
  loop:;
  iter=mat->itemCount;
  int changed=0;
  uint8_t freeColors[mat->itemCount];
  memset(freeColors,1,mat->itemCount);
  for(iter=1;iter<mat->itemCount;iter++) {
    int iter2=0;
    for(;iter2!=mat->itemCount;iter2++) {
      if(!mat->items[iter][iter2]) continue;
      if(colors[iter2]!=-1) {
        freeColors[colors[iter2]]=0;
      }
    }
    //Pick first free
    for(iter2=0;iter2!=mat->itemCount;iter2++){
      if(freeColors[iter2]) {
        break;
      }
    }
    colors[iter]=iter2;
    memset(freeColors,1,mat->itemCount);
  }
  if(changed) goto loop;
  /*for(iter=0;iter!=mat->itemCount;iter++) {
    printf("%i has color %i\n",iter,colors[iter]);
  }*/
  //Validate
  iter=mat->itemCount;
  while(--iter>=0) {
    int iter2=0;
    for(;iter2!=mat->itemCount;iter2++) {
      if(!mat->items[iter][iter2]) continue;
      assert(colors[iter]!=colors[iter2]);
    }
  }
}
static void RemapRegisterByColor(struct jit *jit,int *colors,int type) {
  struct jit_op *op=jit_op_first(jit->ops);
  for(;op;op=op->next) {
    int t=3;
    while(--t>=0) {
      int spec=op->spec;
      if(t)
        spec>>=(2*t); //0b11;
      spec&=3;
      switch(spec) {
        case TREG:
        case REG:
        if(JIT_REG(op->arg[t]).type!=type) break;
        if(ColoringIsBlacklisted(op->arg[t])) break;
        int color=colors[JIT_REG(op->arg[t]).id];
        jit_reg reg;
        memcpy(&reg,&op->arg[t],sizeof(reg));
        if(color!=JIT_REG(op->arg[t]).id)
        reg.id=color;
        memcpy(&op->arg[t],&reg,sizeof(reg));
        break;
      }
    }
  }
}
static void FreeMatrix(CMatrix *mat) {
  int iter=mat->itemCount+16; //We made more eailier
  while(--iter>=0)
    JIT_FREE(mat->items[iter]);
  JIT_FREE(mat->items);
}
#include "../../../3d.h"
static void ReduceRegisterCount(struct jit *jit,struct CFunction *f) {
  jit_flw_analysis(jit);
  //jit_dump_ops(jit, JIT_DEBUG_OPS);
  CMatrix igraph,fgraph;
	CreateInterferenceGraphs(jit,&igraph,&fgraph);
	int icolors[igraph.itemCount+16];
	ColorGraph(&igraph, icolors);
  RemapRegisterByColor(jit,icolors,JIT_RTYPE_INT);
  FreeMatrix(&igraph);
  int fcolors[fgraph.itemCount+16];
	ColorGraph(&fgraph, fcolors);
  RemapRegisterByColor(jit,fcolors,JIT_RTYPE_FLOAT);
  FreeMatrix(&fgraph);

  struct jit_func_info *info=jit_current_func_info(jit);
  CVariable *ilocals[info->gp_reg_count+16];
  CVariable *flocals[info->fp_reg_count+16];
  map_iter_t liter=map_iter(&f->locals);
  const char *key;
  if(f) {
    int iter;
    while(key=map_next(&f->locals, &liter)) {
      CVariable *v=*map_get(&f->locals,key);
      if(!v->isReg) continue;
      if(IsF64(v->type))
	flocals[v->reg]=v;
      else
	ilocals[v->reg]=v;
    }
  }

  struct jit_op *op=jit_op_first(jit->ops);
  int64_t index=0;
  static int64_t msgIndex=0;
  for(;op;op=op->next,index++) {
    int iter;
    int *val;
    vec_foreach_ptr(&op->live_in->vec,val,iter) {
      if(ColoringIsBlacklisted(*val)) continue;
      if(JIT_REG(*val).type==JIT_RTYPE_INT) {
        jit_reg reg=JIT_REG(*val);
        reg.id=icolors[JIT_REG(*val).id];
        memcpy(val,&reg,sizeof(*val));
      }
      if(JIT_REG(*val).type==JIT_RTYPE_FLOAT)  {
        jit_reg reg=JIT_REG(*val);
        reg.id=fcolors[JIT_REG(*val).id];
        memcpy(val,&reg,sizeof(*val));
      }
    }
    vec_foreach_ptr(&op->live_out->vec,val,iter) {
      if(ColoringIsBlacklisted(*val)) continue;
      if(JIT_REG(*val).type==JIT_RTYPE_INT) {
        jit_reg reg=JIT_REG(*val);
        reg.id=icolors[JIT_REG(*val).id];
        memcpy(val,&reg,sizeof(*val));
      }
      if(JIT_REG(*val).type==JIT_RTYPE_FLOAT)  {
        jit_reg reg=JIT_REG(*val);
        reg.id=fcolors[JIT_REG(*val).id];
        memcpy(val,&reg,sizeof(*val));
      }
    }
  }
  //jit_dump_ops(jit, JIT_DEBUG_OPS);
}
