#include "3d.h"
void *AST2X64Mode(AST *a) {
    if(a->type==AST_ASM_REG) {
        CVariable *toreg=*map_get(&Compiler.globals,"X64ModeReg");
        return ((void*(*)(void*))toreg->func->funcptr)(a->asmReg);
    } else if(a->type==AST_ASM_ADDR) {
        CVariable *toreg=*map_get(&Compiler.globals,"X64ModeMem");
        int64_t scale=0;
        Compiler.addrofFrameoffsetMode=1;
        if(a->asmAddr.scale) scale=EvaluateInt(a->asmAddr.scale);
        Compiler.addrofFrameoffsetMode=0;
        void *index=NULL;
        if(a->asmAddr.index) index=a->asmAddr.index->asmReg;
        void *base=NULL;
        if(a->asmAddr.base) base=a->asmAddr.base->asmReg;
        int64_t disp=0;
        Compiler.addrofFrameoffsetMode=1;
        if(a->asmAddr.disp) disp=EvaluateInt(a->asmAddr.disp);
        Compiler.addrofFrameoffsetMode=0;
        return ((void*(*)(int64_t,void*,void*,int64_t,int64_t))toreg->func->funcptr)(scale,index,base,disp,a->asmAddr.width);
    } else if(a->type==AST_INT) {
        CVariable *toimm=*map_get(&Compiler.globals,"X64ModeImm");
        return ((void*(*)(int64_t))toimm->func->funcptr)(a->integer);
    } else if(a->type==AST_FLOAT) {
        CVariable *toimm=*map_get(&Compiler.globals,"X64ModeImm");
        union {
          int64_t i;
          double d;
        } un;
        un.d=a->floating;
        return ((void*(*)(int64_t))toimm->func->funcptr)(un.i);
      } else {
        CVariable *toimm=*map_get(&Compiler.globals,"X64ModeImm");
        CType *t=AssignTypeToNode(a);
        union {
            int64_t i;
            double d;
        } un;
        Compiler.addrofFrameoffsetMode=1;
        if(IsIntegerType(t)) {
            un.i=EvaluateInt(a);
        } else if(IsF64(t)) {
            un.d=EvaluateF64(a);
        }
        Compiler.addrofFrameoffsetMode=0;
        return ((void*(*)(int64_t))toimm->func->funcptr)(un.i);
    }
    return NULL;
}
int IsRegister(char *name) {
  CVariable **enc=map_get(&Compiler.globals,"IsRegister");
  if(!enc) return 0;
  return ((int(*)(char*))enc[0]->func->funcptr)(name);
}
int IsOpcode(char *name) {
  CVariable **enc=map_get(&Compiler.globals,"IsOpcode");
  if(!enc) return 0;
  return ((int(*)(char*))enc[0]->func->funcptr)(name);
}
void AssembleOpcode(char *name,vec_AST_t operands) {
    Compiler.addrofFrameoffsetMode=1;
    //Check if conditonal op
    CVariable *ij=*map_get(&Compiler.globals,"IsJmpOpcode");
    int isjmp=0;
    if(((int8_t(*)(char*))ij->func->funcptr)(name)) {
        isjmp=1;
    }
    CVariable *enc=*map_get(&Compiler.globals,"EncodeOpcode");
    int8_t* (*enc_ptr)(int8_t *ip,char *,int64_t ,void **)=(void*)enc->func->funcptr;
    char buffer[32];
    if(!isjmp) {
        void *args[operands.length];
        int iter;AST *a;
        vec_foreach(&operands,a,iter) {
            args[iter]=AST2X64Mode(a);
        }
        char *endptr=enc_ptr(buffer,name,operands.length,args);
        CVariable *f=*map_get(&Compiler.globals,"X64ModeFree");
        iter=operands.length;
        while(--iter>=0)
            ((void(*)(void*))f->func->funcptr)(args[iter]);
        jit_data_bytes(Compiler.JIT,endptr-buffer,buffer);
    } else {
        char *end_ptr=enc_ptr(buffer,name,0,NULL);
        long opsize=end_ptr-buffer;
        AST *operand=operands.data[0];
        if(operand->type==AST_NAME) {
            refsloop1:;
            vec_CLabelRef_t *refs=map_get(&Compiler.labelRefs,operand->name);
            if(!refs) {
                vec_CLabelRef_t refs2;
                vec_init(&refs2);
                map_set(&Compiler.labelRefs,operand->name,refs2);
                goto refsloop1;
            }
            CLabelRef ref;
            ref.at=operand;
            ref.op=jit_data_code_offset(Compiler.JIT,(jit_value)NULL,-opsize-4);
            vec_push(refs,ref);
        } else {
            AST* name=NULL,*orig=operand,*namepar;
            loop:
            //Offsets can be added or subtracted to label(or be a label without an offset)
            if(operand->type==AST_ADD||operand->type==AST_SUB) {
                    if(operand->binop.a->type==AST_NAME) {
                    } else {
                        namepar=operand;
                        operand=operand->binop.a;
                        goto loop;
                    }
            } else {
                RaiseError(operand,"Label expected as leftmost side of expression.");
            }
            //Replace name with a dummy 0 value,
            namepar->binop.a=CreateI64(0);
            Compiler.addrofFrameoffsetMode=1;
            int64_t offset=EvaluateInt(operand);
            Compiler.addrofFrameoffsetMode=0;
            jit_data_code_offset(Compiler.JIT,(jit_value)NULL,-opsize-offset-4);
            ReleaseAST(namepar->binop.a);
            namepar->binop.a=name;
        }
    }
    ;
}
AST *AppendToSIB(AST *sib,AST *scale,AST *reg,AST *offset) {
  if(!sib) {
    sib=TD_MALLOC(sizeof(AST));
    sib->refCnt=1;
    sib->type=AST_ASM_ADDR;
  }
  if(scale) {
    s:
    if(sib->asmAddr.scale) RaiseError(scale,"SIB can have one index");
    sib->asmAddr.scale=scale;
    sib->asmAddr.index=reg;
  }
  if(!scale&&reg) {
    if(sib->asmAddr.base) {
      scale=CreateI64(1);
      goto s;
    }
    sib->asmAddr.base=reg;
  }
  if(offset) {
    if(sib->asmAddr.disp) {
      Compiler.addrofFrameoffsetMode=1;
      sib->asmAddr.disp+=EvaluateInt(offset);
      Compiler.addrofFrameoffsetMode=0;
      ReleaseAST(offset);
    } else {
      Compiler.addrofFrameoffsetMode=1;
      sib->asmAddr.disp=CreateI64(EvaluateInt(offset));
      Compiler.addrofFrameoffsetMode=0;
    }
  }
  return sib;
}
