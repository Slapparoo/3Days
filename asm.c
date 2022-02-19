#include "3d.h"
static void scan(AST* n, void* data) {
  int* i = data;

  if (n->type == AST_NAME) {
    if (map_get(&Compiler.globals, n->name)) return;

    *i = 1;
  }
}
static int ExprRefsLabel(AST* a) {
  int ret = 0;
  ScanAST(a, scan, &ret);
  return ret;
}
static void __TaintLabels(AST *n, void *data) {
  if(n->type==AST_NAME)
    if(!GetVariable(n->name))
      map_set(&Compiler.asmTaintedLabels, n->name, NULL);
}
void TaintLabelsInExpr(AST* a) {
  ScanAST(a, __TaintLabels, NULL);
}
//Used for loading from binary file.
void PatchLabelExprFunc(CAsmPatch *patch,CFunction *func) {
    const char *key;
    map_iter_t iter=map_iter(&Compiler.exportedLabels);
    //RELOCATIONS POINT TO VARIABLE(WHICH CONTAINS THE LABLE ADDRESS)
    //Think "MOV LABEL_PTR,I64 [relocation]"
    long glob_idx=0;
    while(key=map_next(&Compiler.exportedLabels, &iter)) {
        vec_voidppp_t *pats=map_get(&patch->apply->relocations,key);
        if(!pats) continue;
        long iter;
        void ***pat;
        vec_foreach(pats,pat,iter) {
            //map_get returns pointer to pointer ,DITTO(SEE ABOVE)
            **pat=map_get(&Compiler.exportedLabels,key);
            TD_FREE(*pat);
        }
        map_remove(&patch->apply->relocations,key);
    }
    iter=map_iter(&func->labelPtrs);
    while(key=map_next(&func->labelPtrs, &iter)) {
        char resolved[256];
        LabelContext ul;
        if(2==sscanf(key, LOCAL_LAB_FMT,&ul,resolved)) {
            if(ul!=patch->context) continue;
            //Add a "@@" prefix
            memmove(resolved+2,resolved,strlen(resolved)+1);
            strncpy(resolved,"@@",2);
        } else {
            strcpy(resolved,key);
        }
        vec_voidppp_t *pats=map_get(&patch->apply->relocations,resolved);
        if(!pats) continue;
        long iter;
        void ***pat;
        vec_foreach(pats,pat,iter) {
            //map_get returns pointer to pointer ,DITTO(SEE ABOVE)
            **pat=map_get(&func->labelPtrs,key);
            TD_FREE(*pat);
        }
        map_remove(&patch->apply->relocations,resolved);
    }
    FillFunctionRelocations(func);
}
CFunction *CompileLabelExprFunc(CFunction *func,AST* a,LabelContext labContext) {
      int err=0;
  COldFuncState old = EnterFunction(CreatePrimType(TYPE_I64),NULL);
  Compiler.inFunction=0;
  map_CVariable_t oldlocals=func->locals;
  map_init(&Compiler.locals);
  {
    //Prepare old local variables
    const char* key;
    map_iter_t iter = map_iter(&oldlocals);
    while (key = map_next(&oldlocals, &iter)) {
      map_set(&Compiler.locals, key, *map_get(&oldlocals, key));
    }
  }
  CType* i64 = CreatePrimType(TYPE_I64);
  Compiler.returnType = i64;
  const char *key;
  map_iter_t iter=map_iter(&Compiler.exportedLabels);
  while(key=map_next(&Compiler.exportedLabels, &iter)) {
    CVariable* v = TD_MALLOC(sizeof(CVariable));
    v->type = i64;
    v->name = strdup(key);
    v->isGlobal = 1;
    v->linkage.type = LINK_NORMAL;
    v->linkage.globalPtr = TD_MALLOC(sizeof(void*));
    *(void**)v->linkage.globalPtr=GetGlobalPtr(*map_get(&Compiler.exportedLabels, key));
    map_set(&Compiler.locals, key, v);
  }
  //Prepare local labels
  iter = map_iter(&func->labelPtrs);
  char buffer[256];
  while (key = map_next(&func->labelPtrs, &iter)) {
    long ul;
    if(!sscanf(key, LOCAL_LAB_FMT,&ul,buffer)) {
      CVariable* v = TD_MALLOC(sizeof(CVariable));
      v->type = i64;
      v->name = strdup(key);
      v->isGlobal = 1;
      v->linkage.type = LINK_NORMAL;
      v->linkage.globalPtr = map_get(&func->labelPtrs, key);
      map_set(&Compiler.locals, key, v);
    }
  }
  //Import local labels for context
  iter = map_iter(&func->labelPtrs);
  while (key = map_next(&func->labelPtrs, &iter)) {
    LabelContext ul;
    if(2==sscanf(key, LOCAL_LAB_FMT,&ul,buffer)) {
      if(ul!=labContext) continue;
      char buffer2[258];
      sprintf(buffer2, "@@%s", buffer);
      CVariable* v = TD_MALLOC(sizeof(CVariable));
      v->type = i64;
      v->name = strdup(buffer2);
      v->isGlobal = 1;
      v->linkage.type = LINK_NORMAL;
      v->linkage.globalPtr = map_get(&func->labelPtrs, key);
      map_set(&Compiler.locals, buffer2, v);
    }
  }
    int oldf=Compiler.addrofFrameoffsetMode,olda=Assembler.active;
      Compiler.addrofFrameoffsetMode=1;
      vec_CVariable_t empty;
      vec_init(&empty);
      CFunction *apply_func=CompileAST(&Compiler.locals,CreateReturn(a),empty,func->funcInfo.frameOffset,C_AST_IS_TMP_FUNCTION);
      Compiler.addrofFrameoffsetMode=oldf;
      Assembler.active = olda;
    RestoreCompilerState(old);
    return apply_func;
}
void* EvalLabelExpr(CFunction *func,AST* a,LabelContext labContext) {
    void* ptr=NULL;
    int err=0;
    CFunction *apply_func=CompileLabelExprFunc(func,a,labContext);
    if(apply_func) {
      GC_Disable();
      ptr=((void*(*)())apply_func->funcptr)();
      GC_Enable();
      if(!Compiler.AOTMode) ReleaseFunction(apply_func);
    }
 end:
  Compiler.errorFlag|=!apply_func;
  return ptr;
}
void* AST2X64Mode(AST* a, int64_t* lab_offset) {
  if (lab_offset) *lab_offset = 0;

  if (a->type == AST_ASM_REG) {
    CVariable* toreg = GetHCRTVar( "X64ModeReg");
    return ((void* (*)(void*))toreg->func->funcptr)(a->asmReg);
  } else if (a->type == AST_ASM_ADDR) {
    CVariable* toreg = GetHCRTVar("X64ModeMem");
    int64_t scale = 0;
    Compiler.addrofFrameoffsetMode = 1;

    if (a->asmAddr.scale) scale = EvaluateInt(a->asmAddr.scale, EVAL_INT_F_PRESERVE_LOCALS);

    Compiler.addrofFrameoffsetMode = 0;
    void* index = NULL;

    if (a->asmAddr.index) index = a->asmAddr.index->asmReg;

    void* base = NULL;

    if (a->asmAddr.base) base = a->asmAddr.base->asmReg;

    int64_t disp = 0;
    Compiler.addrofFrameoffsetMode = 1;

    if (a->asmAddr.disp) {
      disp = INT32_MAX; //will be computed later
      TaintLabelsInExpr(a->asmAddr.disp);
    }

    Compiler.addrofFrameoffsetMode = 0;

    void *segment=NULL;
    if(a->asmAddr.segment) {
      segment=a->asmAddr.segment->asmReg;
    }
    //Segment scale,index,base,offset,width
    return ((void* (*)(void*,int64_t, void*, void*, int64_t, int64_t))toreg->func->funcptr)(segment,scale, index, base, disp, a->asmAddr.width);
  } else if (a->type == AST_INT) {
    CVariable* toimm = GetHCRTVar("X64ModeImm");
    return ((void* (*)(int64_t))toimm->func->funcptr)(a->integer);
  } else if (a->type == AST_FLOAT) {
    CVariable* toimm = GetHCRTVar("X64ModeImm");
    union {
      int64_t i;
      double d;
    } un;
    un.d = a->floating;
    return ((void* (*)(int64_t))toimm->func->funcptr)(un.i);
  } else {
    CVariable* toimm = GetHCRTVar("X64ModeImm");

    if (ExprRefsLabel(a)) {
      return ((void* (*)(int64_t))toimm->func->funcptr)(0xffFFffFF);
    }

    CType* t = AssignTypeToNode(a);
    union {
      int64_t i;
      double d;
    } un;
    Compiler.addrofFrameoffsetMode = 1;

    if (IsIntegerType(t)) {
      un.i = EvaluateInt(a, EVAL_INT_F_PRESERVE_LOCALS);
    } else if (IsF64(t)) {
      un.d = EvaluateF64(a);
    }

    Compiler.addrofFrameoffsetMode = 0;
    return ((void* (*)(int64_t))toimm->func->funcptr)(un.i);
  }

  return NULL;
}
void* GetRegister(char* name) {
#ifndef BOOTSTRAP
  return NULL;
#endif // BOOTSTRAP
  if(Compiler.tagsFile) return NULL;
  if(!Compiler.loadedHCRT) return 0;
  CVariable* enc = GetHCRTVar("GetRegister");

  if (!enc) return NULL;
  if(!enc->func) return NULL;
  if(!enc->func->funcptr) return NULL;

  return ((void* (*)(char*))enc->func->funcptr)(name);
}
int IsOpcode(char* name) {
#ifndef BOOTSTRAP
  return NULL;
#endif // BOOTSTRAP
  if(Compiler.tagsFile) return 0;
  if(!Compiler.loadedHCRT) return 0;
  CVariable* enc = GetHCRTVar("IsOpcode");

  if (!enc) return 0;
  if(!enc->func) return 0;
  if(!enc->func->funcptr) return NULL;

  return ((int(*)(char*))enc->func->funcptr)(name);
}
void AssembleOpcode(AST* at, char* name, vec_AST_t operands) {
  if(Compiler.tagsFile) return;
  int8_t isjmp = 0;
  CVariable* f = GetHCRTVar("X64ModeFree");
  Compiler.addrofFrameoffsetMode = 1;
  //Check if conditonal op
  CVariable* enc = GetHCRTVar( "EncodeOpcode");
  //Instptr,name,is_jmp,sib_off,imm_off,argc,args
  int8_t* (*enc_ptr)(int8_t* ip, char*, int8_t*, int64_t*, int64_t*, int64_t, void**) = (void*)enc->func->funcptr;
  char buffer[32], *end_ptr;

  if (!isjmp) {
    void* args[operands.length];
    int iter;
    AST* a;
    vec_foreach(&operands, a, iter) {
      args[iter] = AST2X64Mode(a, NULL);
    }
    struct ExceptFrame* exf = EnterCTry();
    int64_t sibo, immo;

    if (!HCSetJmp(exf)) {
      end_ptr = enc_ptr(buffer, name, &isjmp, &sibo, &immo, operands.length, args);
      PopTryFrame();
    } else {
      CVariable* err = GetHCRTVar( "AsmError");
      RaiseError(at, *(char**)err->linkage.globalPtr);
    }

    int offset = 0;

    if (isjmp) {
      jit_data_bytes(Compiler.JIT, end_ptr - buffer - offset, buffer);
      goto jmp;
    }

    iter = operands.length;

    while (--iter >= 0)
      ((void(*)(void*))f->func->funcptr)(args[iter]);

    if (sibo != -1) offset += 4;

    if (immo != -1) offset += 4;

    jit_data_bytes(Compiler.JIT, end_ptr - buffer - offset, buffer);

    if (sibo != -1) {
      CAsmPatch* pat = TD_MALLOC(sizeof(CAsmPatch));
      pat->isRel = 0;
      pat->context=at->labelContext;
      vec_foreach(&operands, a, iter)

	if (a->type == AST_ASM_ADDR) {
	  pat->exp= a->asmAddr.disp;
	  if(!pat->exp) pat->exp=CreateI64(0);
	}

      jit_dump_ptr(Compiler.JIT, &pat->ptr);
      jit_data_dword(Compiler.JIT, 0);
      TaintLabelsInExpr(pat->exp);
      pat->width = 4;
      vec_push(&Compiler.asmPatches, pat);
    }

    if (immo != -1) {
      CAsmPatch* pat = TD_MALLOC(sizeof(CAsmPatch));
      pat->isRel = 0;
      pat->context=at->labelContext;
      vec_foreach(&operands, a, iter) {
        //Immeadite
        if (a->type != AST_ASM_ADDR && a->type != AST_ASM_REG )
          pat->exp = a;
      }
      pat->width = 4;
      TaintLabelsInExpr(pat->exp);
      jit_dump_ptr(Compiler.JIT, &pat->ptr);
      jit_data_dword(Compiler.JIT, 0);
      vec_push(&Compiler.asmPatches, pat);
    }

    return;
jmp:
    ;
    CAsmPatch* pat = TD_MALLOC(sizeof(CAsmPatch));
    pat->isRel = 1;
    pat->context=at->labelContext;
    jit_dump_ptr(Compiler.JIT, &pat->ptr);
    jit_data_dword(Compiler.JIT, 0);
    pat->exp = operands.data[0];
    TaintLabelsInExpr(pat->exp);
    pat->rel_offset = -offset - 4;
    pat->width = 4;
    vec_push(&Compiler.asmPatches, pat);
  }
}
AST* AppendToSIB(AST* sib, AST* scale, AST* reg, AST* offset) {
  if (!sib) {
    sib = TD_MALLOC(sizeof(AST));
    sib->refCnt = 1;
    sib->type = AST_ASM_ADDR;
  }

  if (scale) {
s:

    if (sib->asmAddr.scale) RaiseError(scale, "SIB can have one index");

    sib->asmAddr.scale = scale;
    sib->asmAddr.index = reg;
  }

  if (!scale && reg) {
    if (sib->asmAddr.base) {
      scale = CreateI64(1);
      goto s;
    }

    sib->asmAddr.base = reg;
  }

  if (offset) {
    if (sib->asmAddr.disp) {
      Compiler.addrofFrameoffsetMode = 1;
      sib->asmAddr.disp = CreateBinop(sib->asmAddr.disp, offset, AST_ADD);
      Compiler.addrofFrameoffsetMode = 0;
    } else {
      Compiler.addrofFrameoffsetMode = 1;
      sib->asmAddr.disp = offset;
      Compiler.addrofFrameoffsetMode = 0;
    }
  }

  return sib;
}
AST* CreateExportedLabel(AST* name) {
  AST* r = TD_MALLOC(sizeof(AST));
  r->type = AST_EXPORT_LABEL;
  r->labelNode = name;
  Compiler.lastLabel = r;
  r->labelContext=NextLabelContext();
  return r;
}
AST* CreateLocalLabel(AST* name) {
  AST* r = TD_MALLOC(sizeof(AST));
  r->type = AST_LOCAL_LABEL;
  r->labelNode = name;
  r->labelContext=Compiler.labelContext;
  return r;
}
AST* CreateDU8(AST* comma) {
  AST* r = TD_MALLOC(sizeof(AST));
  r->type = AST_ASM_DU8;
  r->duData = CommaToVec(comma);
  r->labelContext=Compiler.labelContext;
  return r;
}
AST* CreateDU16(AST* comma) {
  AST* r = TD_MALLOC(sizeof(AST));
  r->type = AST_ASM_DU16;
  r->duData = CommaToVec(comma);
  r->labelContext=Compiler.labelContext;
  return r;
}
AST* CreateDU32(AST* comma) {
  AST* r = TD_MALLOC(sizeof(AST));
  r->type = AST_ASM_DU32;
  r->duData = CommaToVec(comma);
  r->labelContext=Compiler.labelContext;
  return r;
}
AST* CreateDU64(AST* comma) {
  AST* r = TD_MALLOC(sizeof(AST));
  r->type = AST_ASM_DU64;
  r->duData = CommaToVec(comma);
  r->labelContext=Compiler.labelContext;
  return r;
}
AST* CreateAsmImport(AST* comma) {
  AST* r = TD_MALLOC(sizeof(AST));
  r->type = AST_ASM_IMPORT;
  r->asmImports = CommaToVec(comma);
  return r;
}
AST* CreateAsmAlign(AST* a, AST* fill) {
  AST* r = TD_MALLOC(sizeof(AST));
  r->type = AST_ASM_ALIGN;
  r->asmAlign.align = a;
  r->asmAlign.fill = fill;
  return r;
}
CAssembler Assembler;
void ApplyPatches(CFunction *func) {
  CAsmPatch* pat;
  int iter;
  vec_foreach(&func->asmPatches, pat, iter) {
    if(!pat->apply)
        pat->apply=CompileLabelExprFunc(func,pat->exp,pat->context);
    int64_t i = ((int64_t(*)())pat->apply->funcptr)();

    if (pat->isRel) {
      i += pat->rel_offset;
      i -= (int64_t)pat->ptr;
    }

    switch (pat->width) {
    case 1:
      *((int8_t*)pat->ptr) = i;
      break;

    case 2:
      *((int16_t*)pat->ptr) = i;
      break;

    case 4:
      *((int32_t*)pat->ptr) = i;
      break;

    case 8:
      *((int64_t*)pat->ptr) = i;
      break;
    }
  }
}
