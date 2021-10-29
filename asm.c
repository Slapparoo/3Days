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
void* EvalLabelExpr(AST* a) {
  COldFuncState old = CreateCompilerState();
  map_CVariable_t oldlocals=Compiler.locals;
  map_init(&Compiler.locals);
  {
    //Prepare old local variables 
    const char* key;
    map_iter_t iter = map_iter(&oldlocals);
    while (key = map_next(&oldlocals, &iter)) {
      map_set(&Compiler.locals, key, *map_get(&oldlocals, key));
    }
  }
  CType* u8p = CreatePtrType(CreatePrimType(TYPE_U8));
  Compiler.returnType = u8p;
  //Prepare local labels
  const char* key;
  map_iter_t iter = map_iter(&Compiler.labelPtrs);

  while (key = map_next(&Compiler.labelPtrs, &iter)) {
    CVariable* v = TD_MALLOC(sizeof(CVariable));
    v->type = u8p;
    v->name = strdup(key);
    v->isGlobal = 1;
    v->linkage.type = LINK_NORMAL;
    v->linkage.globalPtr = map_get(&Compiler.labelPtrs, key);
    map_set(&Compiler.locals, key, v);
  }

  Compiler.JIT = jit_init();
  void* (*fptr)() = NULL;
  jit_prolog(Compiler.JIT, &fptr);
  int olda = Assembler.active,oldf=Compiler.addrofFrameoffsetMode;
  //Compile will recognize assembler imports if active
  Assembler.active = 1;
  Compiler.addrofFrameoffsetMode=1;
  __CompileAST(CreateReturn(a));
  Compiler.addrofFrameoffsetMode=oldf;
  Assembler.active = olda;
  jit_generate_code(Compiler.JIT,  NULL);
  iter = map_iter(&Compiler.labelPtrs);

  while (key = map_next(&Compiler.labelPtrs, &iter)) {
    ReleaseVariable(*map_get(&Compiler.locals, key));
  }

  map_deinit(&Compiler.locals);
  GC_Disable();
  void* ptr = fptr();
  GC_Enable();
  jit_free(Compiler.JIT);
  RestoreCompilerState(old);
  return ptr;
}
void* AST2X64Mode(AST* a, int64_t* lab_offset) {
  if (lab_offset) *lab_offset = 0;

  if (a->type == AST_ASM_REG) {
    CVariable* toreg = *map_get(&Compiler.globals, "X64ModeReg");
    return ((void* (*)(void*))toreg->func->funcptr)(a->asmReg);
  } else if (a->type == AST_ASM_ADDR) {
    CVariable* toreg = *map_get(&Compiler.globals, "X64ModeMem");
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

    if (a->asmAddr.disp) disp = INT32_MAX; //will be computed later

    Compiler.addrofFrameoffsetMode = 0;

    void *segment=NULL;
    if(a->asmAddr.segment) {
      segment=a->asmAddr.segment->asmReg;
    }
    //Segment scale,index,base,offset,width
    return ((void* (*)(void*,int64_t, void*, void*, int64_t, int64_t))toreg->func->funcptr)(segment,scale, index, base, disp, a->asmAddr.width);
  } else if (a->type == AST_INT) {
    CVariable* toimm = *map_get(&Compiler.globals, "X64ModeImm");
    return ((void* (*)(int64_t))toimm->func->funcptr)(a->integer);
  } else if (a->type == AST_FLOAT) {
    CVariable* toimm = *map_get(&Compiler.globals, "X64ModeImm");
    union {
      int64_t i;
      double d;
    } un;
    un.d = a->floating;
    return ((void* (*)(int64_t))toimm->func->funcptr)(un.i);
  } else {
    CVariable* toimm = *map_get(&Compiler.globals, "X64ModeImm");

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
  CVariable** enc = map_get(&Compiler.globals, "GetRegister");

  if (!enc) return NULL;

  return ((void* (*)(char*))enc[0]->func->funcptr)(name);
}
int IsOpcode(char* name) {
  CVariable** enc = map_get(&Compiler.globals, "IsOpcode");

  if (!enc) return 0;

  return ((int(*)(char*))enc[0]->func->funcptr)(name);
}
void AssembleOpcode(AST* at, char* name, vec_AST_t operands) {
  int8_t isjmp = 0;
  CVariable* f = *map_get(&Compiler.globals, "X64ModeFree");
  Compiler.addrofFrameoffsetMode = 1;
  //Check if conditonal op
  CVariable* enc = *map_get(&Compiler.globals, "EncodeOpcode");
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
    struct ExceptFrame* exf = EnterTry();
    int64_t sibo, immo;

    if (!HCSetJmp(exf)) {
      end_ptr = enc_ptr(buffer, name, &isjmp, &sibo, &immo, operands.length, args);
      PopTryFrame();
    } else {
      CVariable* err = *map_get(&Compiler.globals, "AsmError");
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
      vec_foreach(&operands, a, iter)

	if (a->type == AST_ASM_ADDR) {
	  pat->exp = a->asmAddr.disp;
	  if(!pat->exp) pat->exp=CreateI64(0);
	}

      jit_dump_ptr(Compiler.JIT, &pat->ptr);
      jit_data_dword(Compiler.JIT, 0);
      pat->width = 4;
      vec_push(&Compiler.asmPatches, pat);
    }

    if (immo != -1) {
      CAsmPatch* pat = TD_MALLOC(sizeof(CAsmPatch));
      pat->isRel = 0;
      vec_foreach(&operands, a, iter) {
        //Immeadite
        if (a->type != AST_ASM_ADDR && a->type != AST_ASM_REG )
          pat->exp = a;
      }
      pat->width = 4;
      jit_dump_ptr(Compiler.JIT, &pat->ptr);
      jit_data_dword(Compiler.JIT, 0);
      vec_push(&Compiler.asmPatches, pat);
    }

    return;
jmp:
    ;
    CAsmPatch* pat = TD_MALLOC(sizeof(CAsmPatch));
    pat->isRel = 1;
    jit_dump_ptr(Compiler.JIT, &pat->ptr);
    jit_data_dword(Compiler.JIT, 0);
    pat->exp = operands.data[0];
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
  Compiler.globalLabelCount++;
  return r;
}
AST* CreateLocalLabel(AST* name) {
  AST* r = TD_MALLOC(sizeof(AST));
  r->type = AST_EXPORT_LABEL;
  char fmted[1024];
  char buffer[256];

  if (name->type == AST_INT) {
    sprintf(buffer, "%ld", name->integer);
  } else {
    strcpy(buffer, name->name);
  }

  sprintf(fmted, LOCAL_LAB_FMT, Compiler.globalLabelCount, buffer);
  r->labelNode = name;
  return r;
}
AST* CreateDU8(AST* comma) {
  AST* r = TD_MALLOC(sizeof(AST));
  r->type = AST_ASM_DU8;
  r->duData = CommaToVec(comma);
  return r;
}
AST* CreateDU16(AST* comma) {
  AST* r = TD_MALLOC(sizeof(AST));
  r->type = AST_ASM_DU16;
  r->duData = CommaToVec(comma);
  return r;
}
AST* CreateDU32(AST* comma) {
  AST* r = TD_MALLOC(sizeof(AST));
  r->type = AST_ASM_DU32;
  r->duData = CommaToVec(comma);
  return r;
}
AST* CreateDU64(AST* comma) {
  AST* r = TD_MALLOC(sizeof(AST));
  r->type = AST_ASM_DU64;
  r->duData = CommaToVec(comma);
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
void ApplyPatches() {
  CAsmPatch* pat;
  int iter;
  vec_foreach(&Compiler.asmPatches, pat, iter) {
    int64_t i = (int64_t)EvalLabelExpr(pat->exp);

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
