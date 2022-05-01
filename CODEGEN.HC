#include "VECTOR.HC"
CReg *IRegCodeToReg(I64 c) {
    switch(c) {
        case 0: return Str2Reg("RAX");
        case 1: return Str2Reg("RCX");
        case 2: return Str2Reg("RDX");
        case 3: return Str2Reg("RBX");
        case 4: return Str2Reg("RSP");
        case 5: return Str2Reg("RBP");
        case 6: return Str2Reg("RSI");
        case 7: return Str2Reg("RDI");
        case 8: return Str2Reg("R8");
        case 9: return Str2Reg("R9");
        case 10: return Str2Reg("R10");
        case 11: return Str2Reg("R11");
        case 12: return Str2Reg("R12");
        case 13: return Str2Reg("R13");
        case 14: return Str2Reg("R14");
        case 15: return Str2Reg("R15");
    }
}
//#define JIT_DBG 1
I64 union CRPN_Code {
    class {
    U8 is_signed;
    U8 is_flt;
    #define OP_ADD 1
    #define OP_SUB 2
    
    #define OP_MUL 3
    #define OP_DIV 4
    #define OP_MOD 5
    
    #define OP_SHL 6
    #define OP_SHR 7
    
    #define OP_AND 9
    #define OP_OR 10
    #define OP_XOR 11
    
    #define OP_POS 12
    #define OP_NEG 13
        
    #define OP_GT 14
    #define OP_LT 15
    #define OP_GE 16
    #define OP_LE 17

    #define OP_LXOR 18
    #define OP_LOR 19
    #define OP_LAND 20
    
    #define OP_POST_INC 21 //For interget version of operation,operands[1] is an I64 which is the amount to increment/decrement by
    #define OP_POST_DEC 22 //Ditto
    #define OP_PRE_INC 23 //Ditto
    #define OP_PRE_DEC 24 //Ditto
    
    #define OP_LITERAL 25 //See is_flt
    #define OP_DEREF 26 //operands[1] is an I64 which is offset,operands[2] is size 
    
    #define OP_IF 27  //if cond el,else
    #define OP_JMP 28 //Jumps to CRPN at operands[0]
    #define OP_BODY 30 //See body_length
    
    #define OP_VARIABLE 31 //See my_avalue
    
    #define OP_ASSIGN 32
    
    #define OP_CALL 33 //Return stuffed in my_avalue,last item is function to be called,arguments are put in reverse order
    #define OP_GETARG 34
    #define OP_RETURN 35
    
    #define OP_NOT 36
    #define OP_EQ 37
    #define OP_NE 38
    
    #define OP_REF_DATA 39
    #define OP_DU8 40
    #define OP_DU16 41
    #define OP_DU32 42
    #define OP_DU64 43
    #define OP_ALIGN 44 //operands[0] is an I64 which is align,and simularly operands[1] is the fill
    #define OP_ALLOCAI 45 //operands[0] is an I64 which is the size
    #define OP_RELOCATION 46 //operands[0] is a U0** whihc will be filled in later
    #define OP_ASSIGN_ADD 47
    #define OP_ASSIGN_SUB 48
    #define OP_ASSIGN_MUL 49
    #define OP_ASSIGN_DIV 50
    #define OP_ASSIGN_MOD 51
    #define OP_ASSIGN_SHL 52
    #define OP_ASSIGN_SHR 53
    #define OP_ASSIGN_AND 54
    #define OP_ASSIGN_XOR 55
    #define OP_ASSIGN_OR 56
    
    #define OP_COMMA 57
    //This is a weird one,it pushes my_avalue to the stack,this is usefull for changing OP_LITERAL/OP_VARIABLE to a push for function calls
    #define OP_INPLACE_PUSH 58
    #define OP_DUMP_PTR 59
    
    #define OP_TYPECAST_BIT4BIT 60
    
    #define OP_BREAKPOINT 61
    
    //Does not include bounds check,starts at 0,body_length tells how many items
    //See RPN.jmp_table
    #define OP_JMP_TABLE 62
    U8 code;
    U16 body_length;
    U16 vargs_base;
    };
};
CRPN_Code RPN_CodeNew(U8 code,U8 is_flt=FALSE,U8 is_signed=TRUE) {
    CRPN_Code ret;
    ret.code=code;
    ret.is_signed=is_signed;
    ret.is_flt=is_flt;
    return ret;
}
class CAsmValue;
class CAsmValue {
    #define AV_UNOCC 0 
    #define AV_IREG 1
    #define AV_FREG 2
    #define AV_GLOBAL_PTR 3
    #define AV_IREG_SHADOW 4
    #define AV_FREG_SHADOW 5
    #define AV_INT 6
    #define AV_FLT 7
    #define AV_VIREG 8
    #define AV_VFREG 9
    #define AV_IINDIR 10
    #define AV_FINDIR 11
    #define AV_IFRAME 12
    #define AV_FFRAME 13
    #define AV_IVINDIR 14
    #define AV_FVINDIR 15
    #define AV_PUSH 16 //This pushes to the stack,used for func calls
    I64 type;
    I64 vreg;
    union {
        CReg *reg;
        U8 *global_name;
        I64 int;
        F64 flt;
    };   
    I64 size,fpu_depth;
    union {
        I64 offset,f_offset;
    };
};
Bool AsmValueEqual(CAsmValue *a,CAsmValue *b) {
    if(a->type!=b->type) return FALSE;
    switch(a->type) {
        case AV_IREG:
        return a->reg==b->reg;
        case AV_IVINDIR:
        case AV_FVINDIR:
        return a->vreg==b->vreg&&a->offset==b->offset;
        case AV_IREG_SHADOW:
        case AV_FREG_SHADOW:
        case AV_VIREG:
        return a->vreg==b->vreg;
        case AV_INT:
        return a->int==b->int;
        case AV_FLT:
        return a->flt==b->flt;
        case AV_IINDIR:
        case AV_FINDIR:
        return a->reg==b->reg&&a->f_offset==b->f_offset;
        break;
        case AV_IFRAME:
        case AV_FFRAME:
        return a->f_offset==b->f_offset;
    }
    return TRUE;
}
class CRPN {
    CRPN_Code type;
    CRPN *prev,*next,*consumer;
    CAsmValue my_avalue;
    CRPN *__end;
    CAsmValue *operands[4];
    CVector *successors;
    I64 *live_in,*live_out;
    I64 *flive_in,*flive_out;
    Bool visited;
    //Has from [ireg,physical_reg_code]
    CVector *in_r_map,*r_map,*unspills,*spills,*__live_preds;
    CRPN **jmp_table;
    //Used for register adjustment
    //Has form [CRPN *dst,I64 patch_addr]
    CVector *jmp_table_trampolines;
    //These represent the stack depth of the temp regs at the point of the CRPN 
    I64 live_tistack_depth;
    I64 live_tfstack_depth;
    I64 bin_off,patch_addr;
    #define RPN_F_MERGED_ASSIGN (1)
    //Default order is spill then unspill
    #define RPN_F_UNSPILL_THEN_SPILL (1<<1)
    //Dont emit the operation in CompilerOps
    #define RPN_F_IMPLICIT_OPERATION (1<<2)
    #define RPN_F_DONT_TOUCH (1<<3)
    #define RPN_F_BIG_MOVE (1<<4)
    #define RPN_F_LIVE_SHADOW (1<<5)
    #define RPN_F_LIVE_CHANGED (1<<6)
    //This marks a label as jumped to from assembler block,so its incoming registers may be tainted,so unspill all incoming
    #define RPN_F_ASM_TAINTED (1<<7)
    #define RPN_F_SPILL_ALL (1<<8)
    #define RPN_F_NO_TYPECAST (1<<9)
    I64 flags;
};
U0 RPNFree(CRPN *r) {
    if(r->prev)
        r->prev->next=r->next;
    if(r->next)
        r->next->prev=r->prev;
    if(r->successors)
        VectorFree(r->successors);
    VectorFree(r->spills);
    Free(r->jmp_table);
    if(r->jmp_table_trampolines)
        VectorFree(r->jmp_table_trampolines);
    VectorFree(r->r_map);
    VectorFree(r->unspills);
    Free(r->flive_out);
    Free(r->flive_in);
    Free(r->live_out);
    Free(r->live_in);
    Free(r);
}
U8 *UniqLabelName() {
    static I64 i;
    return MStrPrint("__%x",++i);
}
class CMatrix;
class JIT {
    CRPN *cur_rpn_start;
    I64 cur_tfreg;
    I64 tireg_cnt,tfreg_cnt;
    I64 vireg_cnt,vfreg_cnt;
    I64 ireg_stk_depth,ireg_stk_start;
    CMatrix *igraph,*fgraph;
    U8 *buffer;
    U8 *ip,*ret_label;
    I64 buf_size,buf_cap,allocai_size;
    //Shadows are for spilled items
    I64 frame_offset,ireg_shadows,freg_shadows;
    I64 fpu_stk_size,fpu_stk_offset;
    CVector *tainted_caller_saved,*fpu_stk;
    //We can skip out on liveness analysis on items before the RPNPushValue/RPNPopValue compute registers
    //This is becuase items on the stack intefere with eachother if they are on the stack and hence dont need to be analyzised beyond the stack method
    I64 live_icap;
};
CRPN *GetRPNEnterLabel(CRPN *item);
U0 Push(JIT *jit,CRPN *op,U8 *op,CAsmValue *v);
U0 __FMov(JIT *jit,CRPN *at,CAsmValue *dst,Bool pop=FALSE);
CRPN *FirstRPN(CRPN *items) {
    for(;items&&items->prev;items=items->prev);
    return items;
}
U0 JIT_FREE(JIT *jit) {
    CRPN *rpn,*next;
    Free(jit->buffer);
    Free(jit->ret_label);
    VectorFree(jit->fpu_stk);
    VectorFree(jit->tainted_caller_saved);
    for(rpn=FirstRPN(jit->cur_rpn_start);rpn;rpn=next) {
        next=rpn->next;
        RPNFree(rpn);
    }
}
Bool IsCallerSaved(CReg *reg) {
    U8 *saved[]={
        "RBP",
        "RSI",
        "RDI",
        "R10",
        "R11",
        "R12",
        "R13",
        "R14",
        "R15",
    };
    I64 idx;
    for(idx=0;idx!=sizeof(saved)/sizeof(*saved);idx++)
        if(Str2Reg(saved[idx])==reg)
            return TRUE;
    return FALSE;
}
/*
 * Frame layout
 * RETURN PTR
 * RBP
 * frame
 * ...
 * ireg_shadows
 * ...
 * freg_shadows
 * ...
 */
U0 JIT_ExpandBufIfNeeded(JIT *jit) {
    U8 *nbuf;
    jit->buf_cap+=0xff,jit->buf_cap*=2;
    nbuf=MAlloc(jit->buf_cap);
    MemCpy(nbuf,jit->buffer,MSize(nbuf));
    Free(jit->buffer),jit->buffer=nbuf;
}
JIT *jit_Init() {
    JIT *ret=MAlloc(sizeof(JIT));
    ret->tainted_caller_saved=VectorNew;
    ret->ret_label=UniqLabelName;
    ret->fpu_stk=VectorNew;
    return ret;
}
CRPN *__jit_rpnNew(JIT *jit) {
    CRPN *rpn=MAlloc(sizeof(CRPN));
    rpn->successors=VectorNew;
    rpn->r_map=VectorNew,rpn->in_r_map=VectorNew;
    rpn->spills=VectorNew;
    rpn->unspills=VectorNew;
    if(!jit->cur_rpn_start) jit->cur_rpn_start=rpn;
    else {
        jit->cur_rpn_start->next=rpn;
        rpn->prev=jit->cur_rpn_start;
        jit->cur_rpn_start=rpn;
    }
    return rpn;
}
CRPN *jit_rpnNew(JIT *jit,I64 type) {
    CRPN *rpn=__jit_rpnNew(jit);
    return rpn->type=RPN_CodeNew(type),rpn;
}
CRPN *jit_rpnNewU(JIT *jit,I64 type) {
    CRPN *rpn=__jit_rpnNew(jit);
    rpn->type=RPN_CodeNew(type,,FALSE);
    return rpn;
}
CRPN *jit_rpnInt(JIT *jit,I64 int) {
    CRPN *rpn=__jit_rpnNew(jit);
    rpn->type=RPN_CodeNew(OP_LITERAL);
    rpn->my_avalue.int=int;
    rpn->my_avalue.type=AV_INT;
    return rpn;
}
CRPN *jit_rpnFlt(JIT *jit,F64 flt) {
    CRPN *rpn=__jit_rpnNew(jit);
    rpn->type=RPN_CodeNew(OP_LITERAL,TRUE);
    rpn->my_avalue.flt=flt;
    rpn->my_avalue.type=AV_FLT;
    return rpn;
}
CRPN *jit_rpnNewF(JIT *jit,I64 type) {
    CRPN *rpn=__jit_rpnNew(jit);
    rpn->type=RPN_CodeNew(type,TRUE);
    return rpn;
}
CRPN *jit_rpnBodyStart(JIT *jit) {
    CRPN *rpn=__jit_rpnNew(jit);
    rpn->type=RPN_CodeNew(OP_BODY);
    return rpn;
}
CRPN *jit_rpnIf(JIT *jit) {
    CRPN *rpn=__jit_rpnNew(jit);
    rpn->type=RPN_CodeNew(OP_IF);
    return rpn;
}
CRPN *jit_rpnNewVreg(JIT *jit,I64 num) {
    CRPN *rpn=__jit_rpnNew(jit);
    rpn->type=RPN_CodeNew(OP_VARIABLE);
    rpn->my_avalue.type=AV_VIREG;
    rpn->my_avalue.vreg=num;
    if(jit->vireg_cnt<num)
        jit->vireg_cnt=num;
    return rpn;
}
CRPN *jit_rpnNewVFreg(JIT *jit,I64 num) {
    CRPN *rpn=__jit_rpnNew(jit);
    rpn->type=RPN_CodeNew(OP_VARIABLE,TRUE);
    rpn->my_avalue.type=AV_VFREG;
    rpn->my_avalue.vreg=num;
    if(jit->vfreg_cnt<num)
        jit->vfreg_cnt=num;
    return rpn;
}
class CMatrix {
    U8 **items;
    I64 cnt;
};
CMatrix *NewMatrixNxN(I64 n) {
    CMatrix *ret=MAlloc(sizeof(CMatrix));
    I64 iter;
    ret->items=MAlloc(sizeof(U8*)*n);
    for(iter=0;iter!=n;iter++)
        ret->items[iter]=MAlloc(n);
    ret->cnt=n;
    return ret;
}
U0 Interfere(CMatrix *mat,I64 a,I64 b) {
    mat->items[a][b]=TRUE;
    mat->items[b][a]=TRUE;
}
Bool IsRPNUnop(CRPN *node) {
    switch(node->type.code) {
    case OP_POST_INC:
    case OP_POST_DEC:
    case OP_PRE_INC:
    case OP_PRE_DEC:
    case OP_GETARG:
    case OP_NEG:
    case OP_POS:
    case OP_NOT:
    case OP_DEREF:
    case OP_TYPECAST_BIT4BIT:
    return TRUE;
    }
    return FALSE;
    
}
Bool IsRPNBinop(CRPN *node) {
    switch(node->type.code) {
    case OP_ADD:
    case OP_SUB:
    case OP_MUL:
    case OP_DIV:
    case OP_MOD:
    
    case OP_SHL:
    case OP_SHR:
    case OP_POW:

    case OP_AND:
    case OP_OR:
    case OP_XOR:        
    case OP_GT:
    case OP_LT:
    case OP_GE:
    case OP_LE:
    
    case OP_NE:
    case OP_EQ:

    case OP_LXOR:
    case OP_LOR:
    case OP_LAND:
    case OP_ASSIGN:
    case OP_ASSIGN_ADD:
    case OP_ASSIGN_SUB:
    case OP_ASSIGN_MUL:
    case OP_ASSIGN_DIV:
    case OP_ASSIGN_MOD:
    case OP_ASSIGN_SHL:
    case OP_ASSIGN_SHR:
    case OP_ASSIGN_AND:
    case OP_ASSIGN_XOR:
    case OP_ASSIGN_OR:
    case OP_COMMA:
    return TRUE;
    }
    return FALSE;
}
CRPN *ComputeRPNSuccessor(CRPN *node) {
    if(!node) return NULL;
    if(node->__end) return node->__end;
    CRPN *onode=node;
    I64 idx;
    if(node->type.code==OP_JMP_TABLE) {
        idx=node->type.body_length;
        return ComputeRPNSuccessor(node->next);
    } else if(node.type.code==OP_JMP) {
        return node->next;
    } else if(node->type.code==OP_CALL) {
        idx=node->type.body_length+1; //+1 for func call
        node=node->next;
        while(--idx>=0) node=ComputeRPNSuccessor(node);
        return node;
    } else if(IsRPNBinop(node)) {
        node=ComputeRPNSuccessor(node->next);
        return ComputeRPNSuccessor(node);
    } else if(IsRPNUnop(node)||onode->type.code==OP_RETURN) {
        return ComputeRPNSuccessor(node->next);
    } else if(node->type.code==OP_LITERAL||node->type.code==OP_VARIABLE)
        return node->next;
    else if(node->type.code==OP_IF) {
        //Condition
        node=ComputeRPNSuccessor(node->next);
        //body
        node=ComputeRPNSuccessor(node);
        //else
        return ComputeRPNSuccessor(node);  
    } else if(node->type.code==OP_BODY) {
        idx=node->type.body_length;
        node=node->next;
        while(--idx>=0) node=ComputeRPNSuccessor(node);
        return node;
    } else 
        return onode->next;
}
Bool UsesIntArg(CRPN *rpn) {
    if(rpn->type.code==OP_CALL||rpn->type.code==OP_RELOCATION) return TRUE;
    if(IsRPNBinop(rpn)) {
        switch(ComputeRPNSuccessor(rpn->next)->my_avalue.type) {
            case AV_IVINDIR:
            case AV_VIREG:
            return TRUE;
        }
        check_nxt:
        switch(rpn->next->my_avalue.type) {
            case AV_IVINDIR:
            case AV_VIREG:
            return TRUE;
        }
    } else if(IsRPNUnop(rpn)) {
        goto check_nxt;
    } else if(rpn->type.code==OP_IF||rpn->type.code==OP_RETURN||rpn->type.code==OP_JMP_TABLE)
        goto check_nxt;
    return FALSE;
}
U0 RPNPushValue(JIT *jit,CRPN *onode) {
    I64 iter;
    if(onode->type.code==OP_INPLACE_PUSH)
        onode->live_tistack_depth=jit->ireg_stk_depth;
    if(IsRPNBinop(onode)||IsRPNUnop(onode)||onode->type.code==OP_CALL||onode->type.code==OP_RELOCATION||onode->type.code==OP_REF_DATA) {
        if(onode->type.is_flt)
            jit->tfreg_cnt=MaxI64(jit->tfreg_cnt,++jit->cur_tfreg);
        else goto i;
        if(UsesIntArg(onode)) {
            i:        
            onode->live_tistack_depth=++jit->ireg_stk_depth;
        }
        //Dont assign register to big/wierd sized deref
        if(onode->type.code==OP_DEREF&&!onode->type.is_flt) {
            switch(onode->operands[2](I64)) {
                case 1:
                case 2:
                case 4:
                case 8:
                break;
                default:
                return;
            }
        }
        if(onode->flags&RPN_F_DONT_TOUCH) return;
        if(onode->my_avalue.type==AV_PUSH) return;
        //Trust me,you want to "pretend" that the virtual register stack was changed even if we dont change the physical register
        if(onode->type.is_flt) {
            onode->my_avalue.type=AV_VFREG;
            onode->my_avalue.vreg=jit->tfreg_cnt-1;
        } else {
            jit->tireg_cnt=MaxI64(jit->tireg_cnt,jit->ireg_stk_start+jit->ireg_stk_depth);
            onode->my_avalue.type=AV_VIREG;
            onode->my_avalue.vreg=jit->ireg_stk_start+jit->ireg_stk_depth-1;
        }
    }
}
U0 RPNPopValue(JIT *jit,CRPN *onode) {
    if(IsRPNBinop(onode)||IsRPNUnop(onode)||onode->type.code==OP_CALL||onode->type.code==OP_RELOCATION||onode->type.code==OP_REF_DATA) {
        if(onode->type.is_flt) --jit->cur_tfreg;
        else {
            i:
            --jit->ireg_stk_depth;
            return;
        }
        if(UsesIntArg(onode)) goto i;
    }
}
Bool IsRPNAssign(CRPN *r) {
    switch(r->type.code) {
        case OP_ASSIGN_ADD:
        case OP_ASSIGN_SUB:
        case OP_ASSIGN_MUL:
        case OP_ASSIGN_DIV:
        case OP_ASSIGN_MOD:
        case OP_ASSIGN_SHL:
        case OP_ASSIGN_SHR:
        case OP_ASSIGN_AND:
        case OP_ASSIGN_XOR:
        case OP_ASSIGN_OR:
        case OP_ASSIGN:
        case OP_POST_INC:
        case OP_POST_DEC:
        case OP_PRE_INC:
        case OP_PRE_DEC:
        return TRUE;
    }
    return FALSE;
}
CRPN *RPN_AssignOperandValues(JIT *jit,CRPN *node,CAsmValue **write_to=NULL,CRPN *consumer=NULL) {
    if(!node) return NULL;
    CRPN *onode=node,*prev,*buffer;
    I64 iter;
    onode->consumer=consumer;
    if(write_to&&!*write_to) *write_to=&node->my_avalue;
    else if(write_to&&*write_to) goto en;
    if(IsRPNBinop(onode)) {
        node=RPN_AssignOperandValues(jit,node->next,&onode->operands[0],onode);
        RPNPushValue(jit,onode->next);
        node=RPN_AssignOperandValues(jit,node,&onode->operands[1],onode);
        RPNPushValue(jit,ComputeRPNSuccessor(onode->next));
        RPNPopValue(jit,onode->next);
        RPNPopValue(jit,ComputeRPNSuccessor(onode->next));
    } else if(onode->type.code==OP_LITERAL) {
        //Do nothing
    } else if(IsRPNUnop(onode)||onode->type.code==OP_RETURN||onode->type.code==OP_JMP_TABLE) {
        node=RPN_AssignOperandValues(jit,node->next,&onode->operands[0],onode);
        RPNPushValue(jit,onode->next);
        RPNPopValue(jit,onode->next);
        //    __
        //   |  |
        //\/(*__*)\/
        // \( \/ )/  DONT MERGE ON FUNCTION PUSHES,we dont assign dude,we push
        // (______)
        // /\    /\
        if(onode->consumer&&onode->consumer->type.code!=OP_CALL)
        if(IsRPNAssign(onode->prev)||onode->consumer->type.is_signed==onode->type.is_signed)
        if(onode->type.code==OP_DEREF&&!(RPN_F_DONT_TOUCH&onode->flags)) {
            if(IsRPNAssign(onode->prev))
                onode->prev->type.is_signed=onode->type.is_signed;
            //    ___
            //   (   )
            //  / * *|    If we have compound OP_DEREFs be sure to store rightmost reref into a register to we can merge that register into a AV_IINDIR/AV_FINDIR
            // <--   |>
            //  \    |>
            //   \ __|
            //    |  |
            //  __|  |___
            // crowdude  
            dloop:
            if(onode->prev->type.code!=OP_DEREF) {
                if(onode->next->my_avalue.type==AV_VIREG) {
                    if(onode->type.is_flt)
                        onode->my_avalue.type=AV_FVINDIR;
                    else
                        onode->my_avalue.type=AV_IVINDIR;
                    //We "merged" the source into the destination,
                    onode->flags|=RPN_F_IMPLICIT_OPERATION|RPN_F_MERGED_ASSIGN|RPN_F_DONT_TOUCH;
                    onode->my_avalue.offset=onode->operands[1](I64);
                    onode->my_avalue.size=onode->operands[2](I64);
                    onode->my_avalue.vreg=onode->next->my_avalue.vreg;
                    goto en;
                } else if(onode->next->my_avalue.type==AV_IREG) {    
                    if(onode->type.is_flt)
                        onode->my_avalue.type=AV_FINDIR;
                    else
                        onode->my_avalue.type=AV_IINDIR;
                    //We "merged" the source into the destination,
                    onode->flags|=RPN_F_IMPLICIT_OPERATION|RPN_F_MERGED_ASSIGN|RPN_F_DONT_TOUCH;
                    onode->my_avalue.offset=onode->operands[1](I64);
                    onode->my_avalue.size=onode->operands[2](I64);
                    onode->my_avalue.reg=onode->next->my_avalue.reg;
                    goto en;
                }
            }
        }
    } else if(node->type.code==OP_IF) {
        node=RPN_AssignOperandValues(jit,node->next,&onode->operands[0],onode);
        RPNPushValue(jit,onode->next);
        RPNPopValue(jit,onode->next);
        prev=node;
        node=RPN_AssignOperandValues(jit,node);
        RPNPushValue(jit,prev);
        RPNPopValue(jit,prev);
        prev=node;
        node=RPN_AssignOperandValues(jit,node);
        RPNPushValue(jit,prev);
        RPNPopValue(jit,prev);
    } else if(node->type.code==OP_BODY) {
        iter=node->type.body_length;
        node=node->next;
        while(--iter>=0) {
            prev=node;
            node=RPN_AssignOperandValues(jit,node);
            RPNPushValue(jit,prev);
            RPNPopValue(jit,prev);
        }
    } else if(onode->type.code==OP_CALL) {
        iter=onode->type.body_length+1;
        node=onode->next;
        while(--iter>=0) {
            prev=node;
            node=RPN_AssignOperandValues(jit,node,,onode);
            RPNPushValue(jit,prev);
            RPNPopValue(jit,prev);
        }
    }
    en:
    return ComputeRPNSuccessor(onode);
}
U0 IsRPNData(CRPN *rpn) {
    switch(rpn->type.code) {
        case OP_DU8...OP_DU64:
        return TRUE;
    }
    return FALSE;
}
//Heres the deal,
//from contains the items from the previous operation on input,
//and on exit it contains the outgoing items. This simplifies things alot 
U0 FlattenRPNTo(CRPN *rpn,CVector *to,CVector *from=NULL) {
    I64 iter,iter2;    
    CRPN *cur,*next1,*next2,*old;
    CVector *old_vec,*ul;
    if(!from) {
        old_vec=VectorNew;
        FlattenRPNTo(rpn,to,old_vec);
        VectorFree(old_vec);
        return;
    }
    if(rpn->type.code==OP_JMP_TABLE) {
        FlattenRPNTo(rpn->next,to,from);
        VectorPush(to,rpn);
        if(from) {
            iter2=from->size;
            while(--iter2>=0) {
                VectorAdd(VectorPop(from)(CRPN*)->successors,rpn(I64));
            }
        }
        for(iter2=0;iter2!=rpn->type.body_length;iter2++) {
            VectorAdd(rpn->successors,GetRPNEnterLabel(rpn->jmp_table[iter2])(I64));
        }
        VectorPush(from,rpn);
        return;
    } else if(rpn->type.code==OP_INPLACE_PUSH||rpn->type.code==OP_RELOCATION||rpn->type.code==OP_REF_DATA) {
        if(from) {
            iter2=from->size;
            while(--iter2>=0) {
                VectorAdd(VectorPop(from)(CRPN*)->successors,rpn(I64));
            }
        }
        VectorPush(to,rpn);
        VectorPush(from,rpn);
    } else if(IsRPNData(rpn)) {
        if(from) {
            iter2=from->size;
            while(--iter2>=0) {
                VectorAdd(VectorPop(from)(CRPN*)->successors,rpn(I64));
            }
        }
        VectorPush(to,rpn);
        VectorPush(from,rpn);
    }else if(rpn->type.code==OP_REF_DATA) {
        if(from) {
            iter2=from->size;
            while(--iter2>=0) {
                VectorAdd(VectorPop(from)(CRPN*)->successors,rpn(I64));
            }
        }
        VectorPush(to,rpn);
        VectorPush(from,rpn);
    } else if(rpn->type.code==OP_JMP) {
        if(from) {
            iter2=from->size;
            while(--iter2>=0) {
                VectorAdd(VectorPop(from)(CRPN*)->successors,rpn(I64));
            }
        }
        next1=rpn->operands[0](CRPN*);
        VectorAdd(rpn->successors,next1);
        VectorPush(to,rpn);
        VectorPush(from,rpn);
    } else if(rpn->type.code==OP_CALL) {
        for(cur=rpn->next,iter=0;iter!=rpn->type.body_length+1;iter++) {
            FlattenRPNTo(cur,to,from);
            
            cur=ComputeRPNSuccessor(cur);
        }
        if(from) {
            iter2=from->size;
            while(--iter2>=0) {
                VectorAdd(VectorPop(from)(CRPN*)->successors,rpn);
            }
        }
        VectorPush(to,rpn);
        VectorPush(from,rpn);
    } else if(IsRPNBinop(rpn)) {
        FlattenRPNTo(rpn->next,to,from);
        FlattenRPNTo(ComputeRPNSuccessor(rpn->next),to,from);
        if(from) {
            iter2=from->size;
            while(--iter2>=0) {
                VectorAdd(VectorPop(from)(CRPN*)->successors,rpn);
            }
        }
        VectorPush(to,rpn);
        VectorPush(from,rpn);
    } else if(IsRPNUnop(rpn)) {
        FlattenRPNTo(rpn->next,to,from);
        if(from) {
            iter2=from->size;
            while(--iter2>=0) {
                VectorAdd(VectorPop(from)(CRPN*)->successors,rpn);
            }
        }
        VectorPush(from,rpn);
        VectorPush(to,rpn);
    } else if(rpn.type.code==OP_RETURN) {
        FlattenRPNTo(rpn->next,to,from);
        VectorPush(to,rpn);
        if(from) {
            iter2=from->size;
            while(--iter2>=0) {
                VectorAdd(VectorPop(from)(CRPN*)->successors,rpn);
            }
        }
        VectorPush(from,rpn);
    } else if(rpn->type.code==OP_BODY) {
        VectorPush(to,rpn);
        if(from) {
            iter2=from->size;
            while(--iter2>=0) {
                VectorAdd(VectorPop(from)(CRPN*)->successors,rpn);
            }
        }
        VectorPush(from,rpn);
        cur=rpn->next;
        old=rpn;
        for(iter=0;iter!=rpn->type.body_length;iter++) {
            FlattenRPNTo(cur,to,from);
            if(cur->type.code==OP_RETURN||cur->type.code==OP_JMP)
                from->size=0;
            cur=ComputeRPNSuccessor(cur);
        }
    } else if(rpn->type.code==OP_IF) {
        //Condtion
        FlattenRPNTo(rpn->next,to,from);
        if(from) {
            iter2=from->size;
            while(--iter2>=0) {
                VectorAdd(VectorPop(from)(CRPN*)->successors,rpn);
            }
        }
        VectorPush(to,rpn);
        VectorPush(from,rpn);
        //body and else
        FlattenRPNTo(next1=ComputeRPNSuccessor(rpn->next),to,from);
        //Keep the successors on the stack from later
        old_vec=VectorNew;
        if(from) {
            iter2=from->size;
            while(--iter2>=0) {
                VectorAdd(old_vec,VectorPop(from));
            }
        }
        VectorPush(from,rpn);
        FlattenRPNTo(next2=ComputeRPNSuccessor(next1),to,from);
        iter2=old_vec->size;
        while(--iter2>=0) {
            VectorAdd(from,VectorPop(old_vec));
        }
        VectorFree(old_vec);
    } else
        VectorPush(to,rpn);
}
U0 jit_rpnBodyEnd(JIT *jit,CRPN *strt) {
    CRPN *rpn=strt;
    I64 len=0;
    for(strt=strt->next;strt;len++) {
        if(!strt) break;
        strt=ComputeRPNSuccessor(strt);
    }
    rpn->type.body_length=len;
}
U0 PrintLiveness(I64 *live) {
    I64 max=MSize(live)*8;
    while(--max>=0) {
        if(Bt(live,max))
            "%d,",max;
    }
} 
U0 DumpRPN(CRPN *rpn,I64 flags=0,I64 indent=0) {
    if(!rpn) return;
    I64 iter;
    CRPN *cur;
    if(indent) "%*c",4*indent,' ';
    "[%p] ",rpn;
    switch(rpn->type.code) {
    start:
    if(rpn->flags&RPN_F_IMPLICIT_OPERATION)
        "IMPLCT:";
    switch(rpn->my_avalue.type) {
        case AV_VIREG:
        "V(%d),",rpn->my_avalue.vreg;
        break;
        case AV_IVINDIR:
        "IV(%d),",rpn->my_avalue.vreg;
        break;
        case AV_INT:
        "INT(%p),",rpn->my_avalue.int;
        break;
        case AV_FREG:
        "FR";
    }
    case OP_COMMA:
    ",";
    break;
    case OP_NE:
    "!=";
    break;
    case OP_EQ:
    "==";
    break;
    case OP_TYPECAST_BIT4BIT:
    "TC";
    break;
    case OP_RELOCATION:
    "RELOC";
    break;
    case OP_INPLACE_PUSH:
    "PUSH";
    break;
    case OP_JMP:
    "JMP";
    break;
    case OP_RETURN:
    "RETURN";
    break;
    case OP_GETARG:
    "GETARG";
    break;
    
    case OP_NOT:
    "~";
    break;
    
    case OP_ADD:
    "+";
    break;
    case OP_SUB:
    "-";
    break;
    
    case OP_MUL:
    "*";
    break;
    case OP_DIV:
    "/";
    break;
    case OP_MOD:
    "%%";
    break;
    case OP_SHL:
    "<<";
    break;
    case OP_SHR:
    ">>";
    break;
    case OP_POW:
    "`";
    break;
    case OP_AND:
    "&";
    break;
    case OP_OR:
    "|";
    break;
    case OP_XOR:
    "^";
    break;
    case OP_ASSIGN:
    "=";
    break;
    case OP_POS:
    "+(unary)";
    break;
    case OP_NEG:
    "-(unary)";
    break;
    case OP_GT:
    ">";
    break;
    case OP_LT:
    "<";
    break;
    case OP_GE:
    ">=";
    break;
    case OP_LE:
    "<=";
    break;
    case OP_LXOR:
    "^^";
    break;
    case OP_LOR:
    "||";
    break;
    case OP_LAND:
    "&&";
    break;
    case OP_POST_INC:
    "++";
    break;
    case OP_POST_DEC:
    "--";
    break;
    case OP_PRE_INC:
    "++";
    break;
    case OP_PRE_DEC:
    "--";
    break;
    case OP_VARIABLE:
    "VAR(%d)",rpn->my_avalue.vreg;
    break;
    case OP_LITERAL:
    if(rpn->type.is_flt)
        "%n",rpn->my_avalue.flt;
    else
        "%d",rpn->my_avalue.int;
    break;
    case OP_DEREF:
    "DEREF(%d,%d)",rpn->operands[1],rpn->operands[2];
    break;
    case OP_IF:
    "IF";
    
    break;
    case OP_BODY:
    "BODY";
    break;
    case OP_CALL:
    "CALL";
    break;
    case OP_ASSIGN_ADD:
    "+=";
    break;
    case OP_ASSIGN_SUB:
    "-=";
    break;
    case OP_ASSIGN_MUL:
    "*=";
    break;
    case OP_ASSIGN_DIV:
    "/=";
    break;
    case OP_ASSIGN_MOD:
    "%=";
    break;
    case OP_ASSIGN_SHL:
    "<<=";
    break;
    case OP_ASSIGN_SHR:
    ">>=";
    break;
    case OP_ASSIGN_AND:
    "&=";
    break;
    case OP_ASSIGN_XOR:
    "^=";
    break;
    case OP_ASSIGN_OR:
    "|=";
    break;
    case OP_JMP_TABLE:
    "JTABLE";
    end:
    " <";
    if(rpn->successors)    
    for(iter=0;iter!=rpn->successors->size;iter++) {
        "%p,",rpn->successors->data[iter];
    }
    ">";
    "(In:";
    if(rpn->live_in)
       PrintLiveness(rpn->live_in); 
    " Out:f";
    if(rpn->live_out)
       PrintLiveness(rpn->live_out);
    ")";
    "(Fin:";
    if(rpn->flive_in)
       PrintLiveness(rpn->flive_in); 
    " Fout:";
    if(rpn->flive_out)
       PrintLiveness(rpn->flive_out); 
    ");"
    "IRMAP[";
    if(rpn->in_r_map)
    for(iter=0;iter!=rpn->in_r_map->size;iter+=2)
        "%d,%s,",rpn->r_map->data[iter],IRegCodeToReg(rpn->in_r_map->data[iter+1])->str;
    "]";
    "RMAP[";
    for(iter=0;iter!=rpn->r_map->size;iter+=2)
        "%d,%s,",rpn->r_map->data[iter],IRegCodeToReg(rpn->r_map->data[iter+1])->str;
    "]\n";
    if(rpn->type.code==OP_JMP_TABLE) {
        DumpRPN(rpn->next,flags,indent);
    } else if(IsRPNBinop(rpn)) {
        DumpRPN(rpn->next,flags,indent);
        DumpRPN(ComputeRPNSuccessor(rpn->next),flags,indent);
    } else if(IsRPNUnop(rpn)||rpn->type.code==OP_RETURN) {
        DumpRPN(rpn->next,flags,indent);
    } else if(rpn->type.code==OP_IF) {
        DumpRPN(rpn->next,flags,indent);
        DumpRPN(cur=ComputeRPNSuccessor(rpn->next),flags,indent+1);
        "ELSE\n";
        DumpRPN(ComputeRPNSuccessor(cur),flags,indent+1);
    } else if(rpn->type.code==OP_BODY) {
        cur=rpn->next;
        for(iter=0;iter!=rpn->type.body_length;iter++) {
            DumpRPN(cur,flags,indent+1);
            cur=ComputeRPNSuccessor(cur);
        }
    } else if(rpn->type.code==OP_CALL) {
        cur=rpn->next;
        for(iter=0;iter!=rpn->type.body_length;iter++) {
            DumpRPN(cur,flags,indent+1);
            cur=ComputeRPNSuccessor(cur);
        }
        "FUNC:";
        DumpRPN(cur,flags,indent);
    } else if(rpn->type.code==OP_JMP) {
    }
    }
}
U0 InAdd(I64 cap,I64 *set,CAsmValue *value,Bool fp) {
    switch(value.type) {
        case AV_FVINDIR:
        case AV_IVINDIR:
        case AV_VIREG:
        if(value->vreg<0) {
            "%d\n",value->vreg;
            throw('Toad');
        }
        if(value->vreg<=cap)
            if(!fp) Bts(set,value->vreg);
        break;
        case AV_VFREG:
        if(value->vreg<=cap)
            if(fp) Bts(set,value->vreg);
        break;
    }    
}
CRPN *GetCallFunction(CRPN *call) {
    I64 iter=call->type.body_length;
    call=call->next;
    while(--iter>=0)
        call=ComputeRPNSuccessor(call);
    return call;
}
U0 AddLiveIncoming(I64 cap,I64 *set,CRPN *node,Bool fp) {
    CAsmValue *v;
    I64 idx=0,iter;
    CRPN *cur;
    if(node->type.code==OP_INPLACE_PUSH) {
        InAdd(cap,set,&node->my_avalue,fp);
    } else if(IsRPNBinop(node)) {
        InAdd(cap,set,node->operands[1],fp);
        InAdd(cap,set,node->operands[0],fp);
    } else if(IsRPNUnop(node)) {
        InAdd(cap,set,node->operands[0],fp);
    } else if(node->type.code==OP_CALL) {
        //Function
        // Arguments are pushed to the stack so dont look for them for liveness
        InAdd(cap,set,&GetCallFunction(node)->my_avalue,fp);
    } else if(node->type.code==OP_RETURN||node->type.code==OP_JMP_TABLE) {
        InAdd(cap,set,node->operands[0],fp);
    } else if(node->type.code==OP_IF) {
        InAdd(cap,set,&node->next->my_avalue,fp);
    }
}
//                         __________________________________
//                        / We Dont remove                  /
//       __________      / indirect values from the        /
//      /  O    O  \   _/ set as we dont "write" to them  /
//     {   \____/   } <__________________________________/
//      \__________/
//   ____///    \\\____
//   ___///      \\\___
//  ___///        \\\___
//
// Octopus
U0 RemFromSet(I64 cap,I64 *set,CRPN *value,Bool fp) {
    switch(value->my_avalue.type) {
        //See the octopus
        case AV_VIREG:
        if(value->my_avalue->vreg<=cap)
            if(!fp) Btr(set,value->my_avalue->vreg);
        break;
        case AV_VFREG:
        if(value->my_avalue->vreg<=cap)
            if(fp) Btr(set,value->my_avalue->vreg);
        break;
    }
}
Bool AnalyzeOp(JIT *jit,CRPN *op,I64 max_regs,Bool fp) {
    I64 iter,iter2,qwords,cap=jit->live_icap;
    CRPN *prepare,*cur;
    Bool changed=FALSE;
    //Optimize by checking only for items with shadow set
    if(op->flags&RPN_F_LIVE_SHADOW) {
        goto possible_change;
    }
    op->flags&=~RPN_F_LIVE_SHADOW;
    if(IsRPNUnop(op)||op->type.code==OP_CALL||op->type.code==OP_IF||op->type.code==OP_RETURN||op->type.code==OP_JMP_TABLE)
        if(op->next->flags&RPN_F_LIVE_CHANGED)
            goto possible_change;
    if(IsRPNBinop(op)) {
        if(op->next->flags&RPN_F_LIVE_CHANGED)
            goto possible_change;
        if(ComputeRPNSuccessor(op->next)->flags&RPN_F_LIVE_CHANGED)
            goto possible_change;
    }
    if(op->type.code==OP_CALL) {
        if(GetCallFunction(op)->flags&RPN_F_LIVE_CHANGED)
            goto possible_change;
    }
    for(iter=0;iter!=op->successors->size;iter++) {
        cur=op->successors->data[iter];
        if(cur->flags&RPN_F_LIVE_CHANGED)
            goto possible_change;
    }
    return FALSE;
    possible_change:
    I64 *live_in=MAlloc(qwords=max_regs/8*8+8);
    qwords/=8;
    I64 *live_out=MAlloc(max_regs/8*8+8);
    if(!fp)
        MemCpy(live_in,op->live_out,MSize(live_in));
    else
        MemCpy(live_in,op->flive_out,MSize(live_in));
    if(op->type.code==OP_RETURN||op->type.code==OP_JMP_TABLE) {
        AddLiveIncoming(cap,live_in,op,fp);
    } else if(op->type.code==OP_INPLACE_PUSH) {
        AddLiveIncoming(cap,live_in,op,fp);
    } else if(op->type.code==OP_CALL) {
        RemFromSet(cap,live_in,op,fp);
        AddLiveIncoming(cap,live_in,op,fp);
    } else if(IsRPNBinop(op)&&!IsRPNAssign(op)) {
        RemFromSet(cap,live_in,op,fp);
        AddLiveIncoming(cap,live_in,op,fp);
    } else if(IsRPNBinop(op)&&IsRPNAssign(op)) {
        //2nd is gen
        RemFromSet(cap,live_in,op,fp);
        //kill
        RemFromSet(cap,live_in,op->next,fp);
        AddLiveIncoming(cap,live_in,op,fp);
    } else if(IsRPNUnop(op)) {
        RemFromSet(cap,live_in,op,fp);
        AddLiveIncoming(cap,live_in,op,fp);
    } else if(op->type.code==OP_IF) {
        AddLiveIncoming(cap,live_in,op,fp);
    }
    skip:
    //"C:%d\n",op->type.code;
    for(iter=0;iter!=op->successors->size;iter++) {
        iter2=qwords;
        while(--iter2>=0)  {
            if(!fp)
                live_out[iter2]|=op->successors->data[iter](CRPN*)->live_in[iter2];
            else
                live_out[iter2]|=op->successors->data[iter](CRPN*)->flive_in[iter2];
        }
    }
    iter2=qwords;
    while(--iter2>=0) {
        if(!fp) {
            changed|=op->live_in[iter2]!=live_in[iter2];
            changed|=op->live_out[iter2]!=live_out[iter2];
        } else {
            changed|=op->flive_in[iter2]!=live_in[iter2];
            changed|=op->flive_out[iter2]!=live_out[iter2];
        }
        if(changed) {
            break;
        }
    }
    if(!fp) {
        Free(op->live_in),op->live_in=live_in;
        Free(op->live_out),op->live_out=live_out;
    } else {
        Free(op->flive_in),op->flive_in=live_in;
        Free(op->flive_out),op->flive_out=live_out;
    }
    if(changed) op->flags|=RPN_F_LIVE_SHADOW|RPN_F_LIVE_CHANGED;
    else op->flags&=~(RPN_F_LIVE_SHADOW|RPN_F_LIVE_CHANGED);
    return changed;
}
U0 ComputeRPNLiveness(JIT *jit,CRPN *first,Bool fp=FALSE) {
    CVector *flat=VectorNew;
    I64 iter,max_regs,iter2;
    CRPN *rpn;
    Bool changed;
    FlattenRPNTo(first,flat);  //This assigns the live_[ti]fstack_depth feild
    for(iter=flat->size-1;iter>=0;iter--) {
        rpn=flat->data[iter];
        if(!fp)
            max_regs=MaxI64(jit->vireg_cnt+1+rpn->live_tistack_depth,max_regs);
        else
            max_regs=MaxI64(jit->vfreg_cnt+1+rpn->live_tfstack_depth,max_regs);
    }
    for(iter=flat->size-1;iter>=0;iter--) {
        rpn=flat->data[iter](CRPN*);
        if(!fp) {
            flat->data[iter](CRPN*)->live_out=MAlloc(max_regs/8*8 + 8);
            flat->data[iter](CRPN*)->live_in=MAlloc(max_regs/8*8 + 8);
        } else {
            flat->data[iter](CRPN*)->flive_out=MAlloc(max_regs/8*8 + 8);
            flat->data[iter](CRPN*)->flive_in=MAlloc(max_regs/8*8 + 8);
        }
        rpn->flags|=RPN_F_LIVE_SHADOW;
    }
    loop:
    changed=FALSE;
    for(iter=flat->size-1;iter>=0;iter--) {
        changed|=AnalyzeOp(jit,flat->data[iter],max_regs,fp);
    }
    if(changed) goto loop;
    for(iter=0;iter!=flat->size;iter++) {
        rpn=flat->data[iter];
        for(iter2=0;iter2!=rpn->live_tistack_depth;iter2++)
            BEqu(rpn->live_out,iter2+jit->ireg_stk_start,TRUE);
        if(IsRPNBinop(rpn)) {
            for(iter2=0;iter2<rpn->live_tistack_depth+1;iter2++)
                BEqu(rpn->live_in,iter2+jit->ireg_stk_start,TRUE);
        } else {
            for(iter2=0;iter2<rpn->live_tistack_depth;iter2++)
                BEqu(rpn->live_in,iter2+jit->ireg_stk_start,TRUE);
        }
    }
    VectorFree(flat);
}
U0 JIT_AppendAsmText(JIT *jit,U8 *buf,I64 len) {
    U8 *str=buf,*old_buf;
    if(jit->buf_cap<=jit->buf_size+len+1) {
        jit->buf_cap+=len;
        jit->buf_cap*=2;
        old_buf=jit->buffer;
        jit->buffer=MAlloc(jit->buf_cap+1);
        MemCpy(jit->buffer,old_buf,jit->buf_size);
        Free(old_buf);
    }
    MemCpy(jit->buffer+jit->buf_size,str,len);
    jit->buf_size+=len;
}


I64 RMapGetPhysicalReg(CVector *map,I64 virt) {
    I64 idx=0;
    for(;idx<map->size;idx+=2)
        if(map->data[idx]==virt)
            return map->data[idx+1];
    return -1;
}
I64 RMapGetVirtualReg(CVector *map,I64 physical) {
    I64 idx=0;
    for(;idx<map->size;idx+=2)
        if(map->data[idx+1]==physical)
            return map->data[idx];
    return -1;
}
Bool RMapHasVirt(CVector *map,I64 virt) {
    return -1!=RMapGetPhysicalReg(map,virt);
}
CReg *IRegCodeToReg16(I64 c) {
    switch(c) {
        case 0: return Str2Reg("AX");
        case 1: return Str2Reg("CX");
        case 2: return Str2Reg("DX");
        case 3: return Str2Reg("BX");
        case 4: return Str2Reg("SP");
        case 5: return Str2Reg("BP");
        case 6: return Str2Reg("SI");
        case 7: return Str2Reg("DI");
        case 8: return Str2Reg("R8u16");
        case 9: return Str2Reg("R9u16");
        case 10: return Str2Reg("R10u16");
        case 11: return Str2Reg("R11u16");
        case 12: return Str2Reg("R12u16");
        case 13: return Str2Reg("R13u16");
        case 14: return Str2Reg("R14u16");
        case 15: return Str2Reg("R15u16");
    }
}
CReg *IRegCodeToReg32(I64 c) {
    switch(c) {
        case 0: return Str2Reg("EAX");
        case 1: return Str2Reg("ECX");
        case 2: return Str2Reg("EDX");
        case 3: return Str2Reg("EBX");
        case 4: return Str2Reg("ESP");
        case 5: return Str2Reg("EBP");
        case 6: return Str2Reg("ESI");
        case 7: return Str2Reg("EDI");
        case 8: return Str2Reg("R8u32");
        case 9: return Str2Reg("R9u32");
        case 10: return Str2Reg("R10u32");
        case 11: return Str2Reg("R11u32");
        case 12: return Str2Reg("R12u32");
        case 13: return Str2Reg("R13u32");
        case 14: return Str2Reg("R14u32");
        case 15: return Str2Reg("R15u32");
    }
}
#define BASEREG_CODE 5
CReg *FRegCodeToReg(I64 c) {
    switch(c) {
        case 0: return Str2Reg("ST0");
        case 1: return Str2Reg("ST1");
        case 2: return Str2Reg("ST2");
        case 3: return Str2Reg("ST3");
        case 4: return Str2Reg("ST4");
        case 5: return Str2Reg("ST5");
        case 6: return Str2Reg("ST6");
        case 7: return Str2Reg("ST7");
    }
}
U0 IPushR(JIT *jit,I64 push) {
    U8 buffer[32],*end_ptr;
    X64Mode *reg=X64ModeReg(IRegCodeToReg(push));
    end_ptr=EncodeOpcode(buffer,"PUSH",NULL,NULL,NULL,NULL,reg);
    JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
    X64ModeFree(reg);
}
U0 IPopR(JIT *jit,I64 pop) {
    U8 buffer[32],*end_ptr;
    X64Mode *reg=X64ModeReg(IRegCodeToReg(pop));
    end_ptr=EncodeOpcode(buffer,"POP",NULL,NULL,NULL,NULL,reg);
    JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
    X64ModeFree(reg);
}
U0 IMovRR(JIT *jit,I64 d,I64 s) {
    U8 buffer[32],*end_ptr;
    X64Mode *reg1=X64ModeReg(IRegCodeToReg(d)),*reg2=X64ModeReg(IRegCodeToReg(s));
    end_ptr=EncodeOpcode(buffer,"MOV",NULL,NULL,NULL,NULL,reg1,reg2);
    JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
    X64ModeFree(reg1),X64ModeFree(reg2);
}
U0 RemoveFromRMap(CVector *map,I64 v) {
    I64 *data,size,idx;
    if(-1==RMapGetPhysicalReg(map,v)) return;
    if(map->size>2) {
        data=map->data;
        size=map->size;
        for(idx=0;idx!=size;idx+=2) {
            if(data[idx]==v) {
                SwapI64(&data[size-2],&data[idx]);
                SwapI64(&data[size-1],&data[idx+1]);
                VectorPop(map),VectorPop(map);
                break;
            }
        }
    } else VectorPop(map),VectorPop(map);
}
U0 SpillIReg(JIT *jit,CRPN *at,I64 virt,Bool dry=FALSE) {
    U8 buffer[32],*end_ptr;
    X64Mode *frame,*reg;
    I64 phy=RMapGetPhysicalReg(at->r_map,virt);
    I64 size=at->r_map->size,idx,*data=at->r_map->data;
    if(phy!=-1) {
        if(!dry) {
            frame=X64ModeMem(NULL,0,NULL,Str2Reg("RBP"),jit->ireg_shadows+8*virt,8);
            reg=X64ModeReg(IRegCodeToReg(phy));
            end_ptr=EncodeOpcode(buffer,"MOV",NULL,NULL,NULL,NULL,frame,reg);
            JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
            X64ModeFree(reg),X64ModeFree(frame);
        } else {
            RemoveFromRMap(at->r_map,virt);
            if(Bt(at->live_in,virt)) {
                VectorPush(at->spills,virt);
                VectorPush(at->spills,phy);
            }
        }
    } else
        return;
}
CReg *__JIT_GetFreeIReg(JIT *jit,CRPN *at) {
    U8 *cans[]={
        "RBX",
        "RCX",
        "RDX",
        "R8",
        "R9",
        "R10",
        "R11",
        "R12",
        "R13",
        "R14",
        "R15",
        //Save these for last as they are callee saved,we want to reduce stack spilling on the called functions end
        "RSI",
        "RDI",
    };
    I64 iter,v;
    CReg *r;
    for(iter=0;iter!=sizeof(cans)/sizeof(*cans);iter++) {
        if(
            -1!=(v=RMapGetVirtualReg(at->r_map,(r=Str2Reg(cans[iter]))->code))
            ||-1!=(v=RMapGetVirtualReg(at->in_r_map,r->code))
        )
            ;
        else {
            if(IsCallerSaved(Str2Reg(cans[iter])))
                VectorAdd(jit->tainted_caller_saved,r->code);
            return r;
        }
    }
    return NULL;
}
Bool PhyRegisterAtUseOnOperand(CRPN *at,CAsmValue *value,I64 phy) {
    if(value->type==AV_IREG)
        if(value->reg->code==phy)
            return TRUE;
    if(value->type==AV_VIREG)
        return phy==RMapGetPhysicalReg(at->r_map,value->vreg);
    return FALSE;
}
U0 UnspillIReg(JIT *jit,CRPN *at,I64 virt,Bool dry=FALSE) {
    CRPN *spill;
    I64 phy,idx,code,v,idx2,fspill_try=0,no_rdx=FALSE,no_rcx=FALSE,big_move=FALSE;
    phy=RMapGetPhysicalReg(at->r_map,virt);
    //See the Snailnelious 
    switch(at->type.code) {
        case OP_MUL:
        case OP_MOD:
        case OP_DIV:
        case OP_ASSIGN_MUL:
        case OP_ASSIGN_MOD:
        case OP_ASSIGN_DIV:
        idx=RMapGetVirtualReg(at->r_map,Str2Reg("RDX")->code);
        if(idx!=-1)
            SpillIReg(jit,at,idx,dry);
        no_rdx=TRUE;
        if(Str2Reg("RDX")->code==phy) phy=-1;
        break;
        case OP_SHL:
        case OP_ASSIGN_SHL:
        case OP_SHR:
        case OP_ASSIGN_SHR:
        idx=RMapGetVirtualReg(at->r_map,Str2Reg("RCX")->code);
        if(idx!=-1)
            SpillIReg(jit,at,idx,dry);
        no_rcx=TRUE;
        if(Str2Reg("RCX")->code==phy) phy=-1;
        break;
        case OP_ASSIGN:
        switch(at->operands[0]->type) {
            case AV_IINDIR:
            case AV_IVINDIR:
            switch(at->operands[0]->size) {
                case 1:
                case 2:
                case 4:
                case 8:
                break;
                default:
                at->flags|=RPN_F_BIG_MOVE;
                big_move=TRUE;
            }
        }
    }
    if(phy!=-1) 
        return;
    U8 *cans[]={
        "RBX",
        "RCX",
        "RDX",
        "R8",
        "R9",
        "R10",
        "R11",
        "R12",
        "R13",
        "R14",
        "R15",
        "RSI",
        "RDI",
    };
    if(no_rdx)
        for(idx=0;idx!=sizeof(cans)/sizeof(*cans);idx++)
            if(!StrCmp(cans[idx],"RDX")) {
                cans[idx]=NULL;
                break;
            }
    if(no_rcx) 
        for(idx=0;idx!=sizeof(cans)/sizeof(*cans);idx++)
            if(!StrCmp(cans[idx],"RCX")) {
                cans[idx]=NULL;
                break;
            }
    if(big_move)
        for(idx=0;idx!=sizeof(cans)/sizeof(*cans);idx++) {
            if(!StrCmp(cans[idx],"RDI")||!StrCmp(cans[idx],"RSI"))
                cans[idx]=NULL;
        }
    for(idx=sizeof(cans)/sizeof(*cans)-1;idx>=0;idx--) {
        if(!cans[idx]) goto can_next;
        code=Str2Reg(cans[idx])->code;
        if(PhyRegisterAtUseOnOperand(at,&at->my_avalue,code)) {
                nospill:
                cans[idx]=NULL;
                goto can_next;
            }
        if(IsRPNUnop(at)&&PhyRegisterAtUseOnOperand(at,at->operands[0],code))
            goto nospill;
        if(IsRPNBinop(at)) {
            if(PhyRegisterAtUseOnOperand(at,at->operands[0],code))
                goto nospill;
            if(PhyRegisterAtUseOnOperand(at,at->operands[1],code))
                goto nospill;
        }
        if(at->type.code==OP_CALL)
            if(PhyRegisterAtUseOnOperand(at,&GetCallFunction(at)->my_avalue,code))
                goto nospill;
        can_next:
    }
    loop:;
    for(idx=0;idx!=sizeof(cans)/sizeof(*cans);idx++) {
        if(!cans[idx]) goto next;
        if(-1==(v=RMapGetVirtualReg(at->r_map,code=Str2Reg(cans[idx])->code))) {
            unspill:
            if(Bt(at->live_in,virt)) {
                VectorPush(at->unspills,virt);
                VectorPush(at->unspills,code);
            }
            if(IsCallerSaved(Str2Reg(cans[idx])))
                VectorAdd(jit->tainted_caller_saved,Str2Reg(cans[idx])->code);
            if(dry) {
                RemoveFromRMap(at->r_map,virt);
                VectorPush(at->r_map,virt);
                VectorPush(at->r_map,code);
            }
            return;
        } else if(!Bt(at->live_in,v)&&!Bt(at->live_out,v)) {
            RemoveFromRMap(at->r_map,v);
            goto unspill;
        }
        next:
    }
    v=-1;
    while(v==-1) {
        //v may be -1 if hit other unspilled cnaidate
        if(cans[fspill_try])
            v=RMapGetVirtualReg(at->r_map,Str2Reg(cans[fspill_try])->code);
        fspill_try++;
    }
    SpillIReg(jit,at,v,dry);
    goto loop;
}
U0 __CVT2Reg(JIT *jit,CRPN *rpn,CAsmValue *v,Bool dry) {
    //Check AV_IREG/AV_IINDIR/AV_FINDIR for liveness of .vreg,if .vreg doesn not have the mapping as .reg,turn it back into a virtual register then reconvert to unspill into a new(or maybe same) register
    //The reason for this is if we add to function calls,we invalidate the register mapping for that call maybe for argument a.
    //So when it comes time to add,we have 2 register mappings for each of the operands(as the result register is part of each operand's mapping)
    //This is why we do this.
    // + //RCX+RCX? Nope ;)
    // RCX=CALL(1,2,3) //RCX is not preserved so we map its free for the next operand
    // RCX=CALL(4,5,6) //Oh no
    //    
    CAsmValue virt=*v;
    switch(virt.type) {
        case AV_IREG:
        case AV_IINDIR:
        case AV_FINDIR: 
        if(virt->reg->code!=BASEREG_CODE&&RMapGetPhysicalReg(rpn->r_map,virt.vreg)!=virt.reg->code) {
            SpillIReg(jit,rpn,virt.vreg,dry);
            v=&virt;
        }
    }
    if(v.type==AV_VIREG) {
        UnspillIReg(jit,rpn,v.vreg,dry);
    } else if(v.type==AV_IVINDIR) {
        UnspillIReg(jit,rpn,v.vreg,dry);
    } else if(v.type==AV_FVINDIR) {
        UnspillIReg(jit,rpn,v.vreg,dry);
    }
}
U0 BigMove(JIT *jit,CRPN *rpn,CAsmValue *dst,CAsmValue *src);
U0 UnspillRPNRegs(JIT *jit,CRPN *rpn,Bool dry=TRUE) {
    I64 idx2,idx;
    CRPN *prev;
    CReg *other_r;
    //BigMove is used for moving big & wierd sized items,BigMove consumes RSI/RDI
    if(rpn->type.code==OP_ASSIGN&&!rpn->type.is_flt) {
        switch(rpn->operands[0]->type) {
            case AV_IFRAME:
            case AV_IINDIR:
            case AV_IVINDIR:
            switch(rpn->operands[0]->size) {
                case 1:
                case 2:
                case 4:
                case 8:
                break;
                default:
                rpn->flags|=RPN_F_BIG_MOVE;
                //Spilling doesnt invalidate the registers contents 
                if(-1!=(idx=RMapGetVirtualReg(rpn->r_map,Str2Reg("RDI")->code))) {
                    if(Bt(rpn->live_in,idx))
                        SpillIReg(jit,rpn,idx,TRUE);
                }
                if(-1!=(idx=RMapGetVirtualReg(rpn->r_map,Str2Reg("RSI")->code))) 
                    if(Bt(rpn->live_in,idx))
                        SpillIReg(jit,rpn,idx,TRUE);
            }
        }
    }
    if(rpn->type.code==OP_RELOCATION||rpn->type.code==OP_REF_DATA) {
        __CVT2Reg(jit,rpn,&rpn->my_avalue,dry);
    } else if(IsRPNBinop(rpn)||IsRPNUnop(rpn)||IsRPNAssign(rpn)||rpn->type.code==OP_IF||rpn->type.code==OP_RETURN||rpn->type.code==OP_INPLACE_PUSH) {
        __CVT2Reg(jit,rpn,&rpn->my_avalue,dry);
        if(IsRPNBinop(rpn))
            __CVT2Reg(jit,rpn,rpn->operands[0],dry),__CVT2Reg(jit,rpn,rpn->operands[1],dry);
        if(IsRPNUnop(rpn)||rpn->type.code==OP_RETURN||rpn->type.code==OP_JMP_TABLE)
            __CVT2Reg(jit,rpn,rpn->operands[0],dry);
        if(rpn->type.code==OP_IF)
            __CVT2Reg(jit,rpn,rpn->operands[0],dry);
    }
    if(rpn->type.code==OP_JMP_TABLE) {
        //SPill all as we prepare to jump to a mystery location
        __CVT2Reg(jit,rpn,&rpn->next->my_avalue,dry);
        for(idx=0;idx!=16;idx++)
            if(-1!=(idx2=RMapGetVirtualReg(rpn->r_map,idx)))
                SpillIReg(jit,rpn,idx2,TRUE);
    } else if(rpn->type.code==OP_CALL) {
        prev=rpn->next;
        //Spill Alive non-saved regs
        for(idx=0;idx!=16;idx++)
            if(-1!=(idx2=RMapGetVirtualReg(rpn->r_map,idx)))
                if(!IsCallerSaved(IRegCodeToReg(idx)))
                    SpillIReg(jit,rpn,idx2,TRUE);
        if(dry) jit->fpu_stk_size=0;
        __CVT2Reg(jit,rpn,&GetCallFunction(rpn)->my_avalue,dry);
        __CVT2Reg(jit,rpn,&rpn->my_avalue,dry);
    }
    if(rpn->my_avalue.type==AV_FREG&&rpn->type.code!=OP_VARIABLE) {
        if(dry) jit->fpu_stk_size=MinI64(jit->fpu_stk_size+1,6);
    }
}
U0 IMovRI(JIT *jit,I64 r,I64 imm) {
    U8 buffer[32],*end_ptr;
    X64Mode *ximm=X64ModeImm(imm),*reg=X64ModeReg(IRegCodeToReg(r));
    end_ptr=EncodeOpcode(buffer,"MOV",NULL,NULL,NULL,NULL,reg,ximm);
    JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
    X64ModeFree(reg),X64ModeFree(ximm);
}
U0 IMovRInd(JIT *jit,I64 r,I64 in,I64 off=0,I64 sz=8,Bool unsigned) {
    U8 buffer[32],*end_ptr;
    X64Mode *ind,*reg;
    switch(sz) {
        case 1:
        case 2:
        ind=X64ModeMem(NULL,0,NULL,IRegCodeToReg(in),off,sz);
        reg=X64ModeReg(IRegCodeToReg(r));
        if(unsigned)
            end_ptr=EncodeOpcode(buffer,"MOVZX",NULL,NULL,NULL,NULL,reg,ind);
        else
            end_ptr=EncodeOpcode(buffer,"MOVSX",NULL,NULL,NULL,NULL,reg,ind);
        JIT_AppendAsmText(jit,buffer,end_ptr-buffer);    
        break;
        case 4:
        case 8:
        ind=X64ModeMem(NULL,0,NULL,IRegCodeToReg(in),off,sz);
        reg=X64ModeReg(IRegCodeToReg(r));
        if(sz==8)
            end_ptr=EncodeOpcode(buffer,"MOV",NULL,NULL,NULL,NULL,reg,ind);
        else if(sz==4&&!unsigned)
            end_ptr=EncodeOpcode(buffer,"MOVSXD",NULL,NULL,NULL,NULL,reg,ind);
        else {
            X64ModeFree(reg);
            reg=X64ModeReg(IRegCodeToReg32(r));
            end_ptr=EncodeOpcode(buffer,"MOV",NULL,NULL,NULL,NULL,reg,ind);
        }
        JIT_AppendAsmText(jit,buffer,end_ptr-buffer);    
        break;
    }
    X64ModeFree(reg),X64ModeFree(ind);
}
U0 IMovIndR(JIT *jit,I64 in,I64 off=0,I64 r,I64 sz=8) {
    U8 buffer[32],*end_ptr;
    X64Mode *ind,*reg;
    switch(sz) {
        start:
        ind=X64ModeMem(NULL,0,NULL,IRegCodeToReg(in),off,sz);
        case 1:
        IMovRR(jit,0,r);
        reg=X64ModeReg(Str2Reg("AL"));
        break;
        case 2:
        reg=X64ModeReg(IRegCodeToReg16(r));
        break;
        case 4:
        reg=X64ModeReg(IRegCodeToReg32(r));
        break;
        case 8:
        reg=X64ModeReg(IRegCodeToReg(r));
        end:
        end_ptr=EncodeOpcode(buffer,"MOV",NULL,NULL,NULL,NULL,ind,reg);
        JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
        break;
    }
    X64ModeFree(reg),X64ModeFree(ind); 
}

U0 RMapSync(JIT *jit,CRPN *dst,CRPN *src) {
    I64 idx,pr,desired,idx2,qwords=MSize(dst->live_in)/8,bit;
    CVector *to_load=VectorNew,*to_unspill=VectorNew;
    for(idx=0;idx!=src->r_map->size;idx+=2) {
        if(-1==RMapGetPhysicalReg(dst->in_r_map,src->r_map->data[idx])) {
            desired=src->r_map->data[idx+1];
            IMovIndR(jit,BASEREG_CODE,jit->ireg_shadows+8*src->r_map->data[idx],desired,8);
        }
    }
    //Start with those loaded in dst's rmap
    for(idx=0;idx!=dst->in_r_map->size;idx+=2) {
        desired=dst->in_r_map->data[idx+1];
        if(-1!=(pr=RMapGetPhysicalReg(src->r_map,dst->in_r_map->data[idx]))) {
            if(pr==desired) goto ipass;
            //If conflicts with existing in src,spill to stack then reaload later
            if(-1!=RMapGetVirtualReg(src->r_map,desired)) {
                VectorPush(to_load,dst->in_r_map->data[idx]);
                IPushR(jit,pr);
                goto ipass;
            } else {
                IMovRR(jit,desired,pr);
            }
        } else {
            for(idx2=0;idx2!=dst->unspills->size;idx2+=2)
                if(dst->unspills->data[idx2]==dst->in_r_map->data[idx])
                    goto ipass;
            //Unspill desired reg
            VectorPush(to_unspill,dst->in_r_map->data[idx]);
        }
        ipass:
    }
    //Load values expected to be spilled
    for(idx=0;idx!=dst->spills->size;idx+=2)
        if(-1==RMapGetPhysicalReg(dst->in_r_map,dst->spills->data[idx])) {
            VectorAdd(to_unspill,dst->spills->data[idx]);
        }
    for(;to_load->size;) {
        idx=VectorPop(to_load);
        IPopR(jit,RMapGetPhysicalReg(dst->in_r_map,idx));
    }
    for(;to_unspill->size;) {
        desired=RMapGetPhysicalReg(dst->in_r_map,idx=VectorPop(to_unspill));
        IMovRInd(jit,desired,BASEREG_CODE,jit->ireg_shadows+8*idx,8,FALSE);
    } 
    VectorFree(to_load),VectorFree(to_unspill);
}
U0 FISTP(JIT *jit,I64 in,I64 off=0) {
    U8 buffer[32],*end_ptr;
    X64Mode *ind=X64ModeMem(NULL,0,NULL,IRegCodeToReg(in),off,8);
    end_ptr=EncodeOpcode(buffer,"FISTP",NULL,NULL,NULL,NULL,ind);
    JIT_AppendAsmText(jit,buffer,end_ptr-buffer);    
    X64ModeFree(ind);
}
U0 __X87Push(JIT *jit,CRPN *at,CAsmValue *src);
U0 IMov(JIT *jit,CRPN *at,CAsmValue *dst,CAsmValue *src,Bool unsigned) {
    if(AsmValueEqual(dst,src)) return;
    I64 p,reg,old_buf_sz;
    CAsmValue dummy;
    #ifdef JIT_DBG
    old_buf_sz=jit->buf_size;
    #endif
    switch(dst->type) {
        case AV_PUSH:
        switch(src->type) {
            case AV_FREG_SHADOW:
            case AV_FVINDIR:
            case AV_FINDIR:
            __X87Push(jit,at,src);
            case AV_FREG:
            //SUB RSP,8;
            JIT_AppendAsmText(jit,"\x48\x83\xEC\x08",4);
            FISTP(jit,Str2Reg("RSP")->code,0);
            return;
        }
        Push(jit,at,"PUSH",src);
        break;
        case AV_VIREG:
        reg=RMapGetPhysicalReg(at->r_map,dst->vreg);
        if(reg==-1) throw('CodeGen');
        goto r;
        case AV_IREG:
        reg=dst->reg->code;
        r:
        switch(src->type) {
            case AV_IREG:
            IMovRR(jit,reg,src->reg->code);
            break;
            case AV_VIREG:
            if(RMapGetPhysicalReg(at->r_map,src->vreg)==-1) throw('CodeGen');
            IMovRR(jit,reg,RMapGetPhysicalReg(at->r_map,src->vreg));
            break;
            case AV_INT:
            IMovRI(jit,reg,src->int);
            break;
            case AV_FLT:
            IMovRI(jit,reg,src->flt);
            break;
            case AV_IREG_SHADOW:
            IMovRInd(jit,reg,BASEREG_CODE,jit->ireg_shadows+8*src->vreg,,unsigned);
            break;
            case AV_IFRAME:
            IMovRInd(jit,reg,BASEREG_CODE,src->f_offset,,unsigned);
            break;
            case AV_FREG:
            //SUB RSP,8,FISTP [RSP]
            JIT_AppendAsmText(jit,"\x48\x83\xEC\x08\xDF\x3C\x24",4+3);
            IPopR(jit,reg);
            break;
            case AV_FREG_SHADOW:
            case AV_FINDIR:
            case AV_FFRAME:
            case AV_FVINDIR:
            __X87Push(jit,at,src);
            //SUB RSP,8,FISTP [RSP]
            JIT_AppendAsmText(jit,"\x48\x83\xEC\x08\xDF\x3C\x24",4+3);
            IPopR(jit,reg);
            break;
            case AV_IVINDIR:
            IMovRInd(jit,reg,RMapGetPhysicalReg(at->r_map,src->vreg),src->offset,src->size,unsigned);
            break;
            case AV_IINDIR:
            IMovRInd(jit,reg,src->reg->code,src->offset,src->size,unsigned);
            break;
        }
        break;
        case AV_IFRAME:
        switch(src->type) {
            case AV_IREG:
            IMovIndR(jit,BASEREG_CODE,dst->f_offset,src->reg->code);
            break;
            case AV_VIREG:
            IMovIndR(jit,BASEREG_CODE,dst->f_offset,RMapGetPhysicalReg(at->r_map,src->vreg));
            break;
            case AV_INT:
            IMovRI(jit,Str2Reg("RAX")->code,src->int);
            IMovIndR(jit,BASEREG_CODE,dst->f_offset,Str2Reg("RAX")->code);
            break;
            case AV_FLT:
            IMovRI(jit,Str2Reg("RAX")->code,src->flt);
            IMovIndR(jit,BASEREG_CODE,dst->f_offset,Str2Reg("RAX")->code);
            break;
            case AV_IREG_SHADOW:
            IMovRInd(jit,Str2Reg("RAX")->code,BASEREG_CODE,jit->ireg_shadows+8*src->vreg,,unsigned);
            IMovIndR(jit,BASEREG_CODE,dst->f_offset,Str2Reg("RAX")->code);
            break;
            case AV_FREG:
            FISTP(jit,BASEREG_CODE,dst->f_offset);
            break;
            case AV_FINDIR:
            case AV_FFRAME:
            case AV_FREG_SHADOW:
            __X87Push(jit,at,src);
            FISTP(jit,BASEREG_CODE,dst->f_offset);
            break;
            case AV_IFRAME:
            IMovRInd(jit,Str2Reg("RAX")->code,BASEREG_CODE,src->f_offset,src->size,unsigned);
            IMovIndR(jit,BASEREG_CODE,dst->f_offset,Str2Reg("RAX")->code);
            break;
            case AV_IVINDIR:
            IMovRInd(jit,Str2Reg("RAX")->code,RMapGetPhysicalReg(at->r_map,src->vreg),src->offset,src->size,unsigned);
            IMovIndR(jit,BASEREG_CODE,dst->f_offset,Str2Reg("RAX")->code);
            break;
            
            case AV_IINDIR:
            IMovRInd(jit,Str2Reg("RAX")->code,src->reg->code,src->offset,src->size,unsigned);
            IMovIndR(jit,BASEREG_CODE,dst->f_offset,Str2Reg("RAX")->code);
            break;
        }
        break;
        case AV_IREG_SHADOW:
        switch(src->type) {
            case AV_IREG:
            IMovIndR(jit,BASEREG_CODE,jit->ireg_shadows+8*dst->vreg,src->reg->code,8);
            break;
            case AV_VIREG:
            IMovIndR(jit,BASEREG_CODE,jit->ireg_shadows+8*dst->vreg,RMapGetPhysicalReg(at->r_map,src->vreg),8);
            break;
            case AV_INT:
            IMovRI(jit,Str2Reg("RAX")->code,src->int);
            IMovIndR(jit,BASEREG_CODE,jit->ireg_shadows+8*dst->vreg,Str2Reg("RAX")->code,8);
            break;
            case AV_FLT:
            IMovRI(jit,Str2Reg("RAX")->code,src->flt);
            IMovIndR(jit,BASEREG_CODE,jit->ireg_shadows+8*dst->vreg,Str2Reg("RAX")->code,8);
            break;
            case AV_IREG_SHADOW:
            IMovRInd(jit,Str2Reg("RAX")->code,BASEREG_CODE,jit->ireg_shadows+8*src->vreg,,unsigned);
            IMovIndR(jit,BASEREG_CODE,jit->ireg_shadows+8*dst->vreg,Str2Reg("RAX")->code,8);
            break;
            case AV_FREG:
            FISTP(jit,BASEREG_CODE,jit->ireg_shadows+8*dst->vreg);
            break;
            case AV_FREG_SHADOW:
            case AV_FINDIR:
            case AV_FVINDIR:
            case AV_FFRAME:
            __X87Push(jit,at,src);
            FISTP(jit,BASEREG_CODE,jit->ireg_shadows+8*dst->vreg);
            break;
            case AV_IVINDIR:
            IMovRInd(jit,Str2Reg("RAX")->code,RMapGetPhysicalReg(at->r_map,src->vreg),src->offset,src->size,unsigned);
            IMovIndR(jit,BASEREG_CODE,jit->ireg_shadows+8*dst->vreg,Str2Reg("RAX")->code,8);
            break;
            case AV_IINDIR:
            IMovRInd(jit,Str2Reg("RAX")->code,src->reg->code,src->offset,src->size,unsigned);
            IMovIndR(jit,BASEREG_CODE,jit->ireg_shadows+8*dst->vreg,Str2Reg("RAX")->code,8);
            break;
        }
        break;
        case AV_IINDIR:
        reg=dst->reg->code;
        ii:
        switch(src->type) {
            case AV_IREG:
            IMovIndR(jit,reg,dst->offset,src->reg->code,dst->size);
            break;
            case AV_VIREG:
            IMovIndR(jit,reg,dst->offset,RMapGetPhysicalReg(at->r_map,src->vreg),dst->size);
            break;
            case AV_INT:
            IMovRI(jit,Str2Reg("RAX")->code,src->int);
            IMovIndR(jit,reg,dst->offset,Str2Reg("RAX")->code,dst->size);
            break;
            case AV_FLT:
            IMovRI(jit,Str2Reg("RAX")->code,src->flt);
            IMovIndR(jit,reg,dst->offset,Str2Reg("RAX")->code,dst->size);
            break;
            case AV_IREG_SHADOW:
            IMovRInd(jit,Str2Reg("RAX")->code,BASEREG_CODE,jit->ireg_shadows+8*src->vreg,,unsigned);
            IMovIndR(jit,reg,dst->offset,Str2Reg("RAX")->code,8);
            break;
            case AV_IFRAME:
            IMovRInd(jit,Str2Reg("RAX")->code,BASEREG_CODE,src->f_offset,src->size,unsigned);
            IMovIndR(jit,reg,dst->offset,Str2Reg("RAX")->code,dst->size);
            break;
            case AV_FREG:
            FISTP(jit,reg,dst->offset);
            break;
            case AV_FREG_SHADOW:
            case AV_FINDIR:
            case AV_FFRAME:
            case AV_FVINDIR:
            __X87Push(jit,at,src);
            FISTP(jit,reg,dst->offset);
            break;
            case AV_IVINDIR:
            IMovRInd(jit,Str2Reg("RAX")->code,RMapGetPhysicalReg(at->r_map,src->vreg),src->offset,src->size,unsigned);
            IMovIndR(jit,reg,dst->offset,Str2Reg("RAX")->code,dst->size);
            break;
            case AV_IINDIR:
            IMovRInd(jit,Str2Reg("RAX")->code,src->reg->code,src->offset,src->size,unsigned);
            IMovIndR(jit,reg,dst->offset,Str2Reg("RAX")->code,dst->size);
            break;
        }
        break;
        case AV_IVINDIR:
        reg=RMapGetPhysicalReg(at->r_map,dst->vreg);
        goto ii;
        case AV_UNOCC:
        if(src->type==AV_FREG)
            //FSTP ST0;
            JIT_AppendAsmText(jit,"\xDD\xD8",2);
    }
    #ifdef JIT_DBG
    if(old_buf_sz==jit->buf_size) {
        "NOTHING EMITED AT %d\n",at->type.code;
    }
    #endif
}
U0 IArithUnop(JIT *jit,CRPN *at,U8 *op,CAsmValue *dst,CAsmValue *src,Bool unsigned=FALSE) {
    I64 reg,base,p,offset,old_buf_sz=jit->buf_size;
    U8 buffer[32],*end_ptr;
    X64Mode *mode;
    CAsmValue dummy;
    loop:
    switch(dst->type) {
        case AV_PUSH:
        Push(jit,at,"PUSH",src);
        dummy.reg=Str2Reg("RSP");
        dummy.offset=0;
        dummy.size=8;
        dummy.type=AV_IINDIR;  
        dst=&dummy;
        goto loop;
        case AV_IREG:
        reg=dst->reg->code;
        r:
        IMov(jit,at,dst,src,unsigned);
        end_ptr=EncodeOpcode(buffer,op,NULL,NULL,NULL,NULL,mode=X64ModeReg(IRegCodeToReg(reg)));
        JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
        break;
        case AV_VIREG:
        reg=RMapGetPhysicalReg(at->r_map,dst->vreg);
        goto r;
        case AV_IFRAME:
        base=BASEREG_CODE;
        offset=dst->f_offset;
        ind:
        IMov(jit,at,dst,src,unsigned);
        end_ptr=EncodeOpcode(buffer,op,NULL,NULL,NULL,NULL,mode=X64ModeMem(NULL,0,NULL,IRegCodeToReg(base),offset,dst->size));
        JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
        break;
        case AV_IREG_SHADOW:
        base=BASEREG_CODE;
        offset=jit->ireg_shadows+8*dst->vreg;
        goto ind;
        break;
        case AV_IINDIR:
        base=dst->reg->code;
        offset=dst->offset;
        goto ind;
        case AV_IVINDIR:
        offset=dst->offset;
        base=RMapGetPhysicalReg(at->r_map,dst->vreg);
        goto ind;
    }
    X64ModeFree(mode);
    #ifdef JIT_DBG
    if(old_buf_sz==jit->buf_size) {
        "NOTHING EMITED AT %d\n",at->type.code;
    }
    #endif
}
U0 IIncOp(JIT *jit,CRPN *at,U8 *op,CAsmValue *a,Bool unsigned) {
    I64 reg,base,p,offset,p2,imm,old_buf_sz=jit->buf_size;
    CAsmValue dummy;
    U8 buffer[32],*end_ptr;
    X64Mode *mode;
    switch(a->type) {
        case AV_PUSH:
        Push(jit,at,"PUSH",a);
        base=Str2Reg("RSP")->code;
        offset=0;
        goto i;
        case AV_IREG:
        reg=a->reg->code;
        r:
        end_ptr=EncodeOpcode(buffer,op,NULL,NULL,NULL,NULL,mode=X64ModeReg(IRegCodeToReg(reg)));
        JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
        break;
        case AV_VIREG:
        reg=RMapGetPhysicalReg(at->r_map,a->vreg);
        goto r;
        case AV_IFRAME:
        base=BASEREG_CODE;
        offset=a->f_offset;
        i:
        end_ptr=EncodeOpcode(buffer,op,NULL,NULL,NULL,NULL,mode=X64ModeMem(NULL,0,NULL,IRegCodeToReg(base),offset,a->size));
        JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
        break;
        case AV_IVINDIR:
        base=RMapGetPhysicalReg(at->r_map,a->vreg);
        offset=a->offset;
        goto i;
        case AV_IINDIR:
        base=a->reg->code;
        offset=a->offset;
        goto i;
        case AV_IREG_SHADOW:
        base=BASEREG_CODE;
        offset=jit->ireg_shadows+8*a->vreg;
        goto i;
    }
    X64ModeFree(mode);
    #ifdef JIT_DBG
    if(old_buf_sz==jit->buf_size) {
        "NOTHING EMITED AT %d\n",at->type.code;
    }
    #endif
}
U0 Push(JIT *jit,CRPN *at,U8 *op,CAsmValue *a) {
    I64 imm,reg,p,base,offset,old_buf_sz=jit->buf_size;
    CAsmValue dummy,dummy2;
    U8 buffer[32],*end_ptr;
    X64Mode *mode;
    switch(a->type) {
        case AV_INT:
        imm=a->int;
        pimm:
        if(I32_MIN<=imm<=I32_MAX) {
            end_ptr=EncodeOpcode(buffer,"REX",NULL,NULL,NULL,NULL);
            JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
            end_ptr=EncodeOpcode(buffer,"PUSH",NULL,NULL,NULL,NULL,mode=X64ModeImm(imm));
            JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
            X64ModeFree(mode);
        } else {
            dummy2.type=AV_INT;
            dummy2.int=imm;
            dummy.type=AV_IREG;
            dummy.reg=Str2Reg("RAX");
            IMov(jit,at,&dummy,&dummy2,FALSE);
            Push(jit,at,op,&dummy);
        }
        break;
        case AV_FLT:
        imm=a->flt(I64);
        goto pimm;
        case AV_IREG:
        reg=a->reg->code;
        r:
        IPushR(jit,reg);
        break;
        case AV_VIREG:
        reg=RMapGetPhysicalReg(at->r_map,a->vreg);
        goto r;
        break;
        case AV_FREG:
        fr:
        //SUB RSP,8; FSTP F64 [RSP]
        JIT_AppendAsmText(jit,"\x48\x83\xEC\x08\xDD\x1C\x24",4+3);
        break;
        case AV_IFRAME:
        base=BASEREG_CODE;
        offset=a->f_offset;
        i:
        if(a->size==8) {
            mode=X64ModeMem(NULL,0,NULL,IRegCodeToReg(base),offset,a->size);
            end_ptr=EncodeOpcode(buffer,"PUSH",NULL,NULL,NULL,NULL,mode);
            JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
            X64ModeFree(mode);
        } else {
            dummy.type=AV_IREG;
            dummy.reg=Str2Reg("RAX");
            IMov(jit,at,&dummy,a,!at->type.is_signed);
            Push(jit,at,op,&dummy);
        }
        break;
        case AV_IREG_SHADOW:
        base=BASEREG_CODE;
        offset=jit->ireg_shadows+8*a->vreg;
        goto i;
        case AV_FREG_SHADOW:
        base=BASEREG_CODE;
        offset=jit->freg_shadows+8*a->vreg;
        goto i;
        case AV_FINDIR:
        case AV_IINDIR:
        //FINDIR's are always 64-bit
        base=a->reg->code;
        offset=a->offset;
        goto i;
        case AV_FVINDIR:
        case AV_IVINDIR:
        base=RMapGetPhysicalReg(at->r_map,a->vreg);
        offset=a->offset;
        goto i;
        case AV_FFRAME:
        base=BASEREG_CODE;
        offset=a->f_offset;
        goto i;
    }
    #ifdef JIT_DBG
    if(old_buf_sz==jit->buf_size) {
        "NOTHING EMITED AT %d\n",at->type.code;
    }
    #endif

}
U0 Pop(JIT *jit,CRPN *at,U8 *op,CAsmValue *a) {
    I64 imm,reg,p,base,offset,old_buf_sz=jit->buf_size;
    CAsmValue dummy,dummy2;
    U8 buffer[32],*end_ptr;
    X64Mode *mode;
    switch(a->type) {
        case AV_IREG:
        reg=a->reg->code;
        r:
        IPopR(jit,reg);
        break;
        case AV_VIREG:
        IPopR(jit,RMapGetPhysicalReg(at->r_map,a->vreg));
        break;
        case AV_FREG:
        fr:
        //FLD F64 [RSP];
        //ADD RSP,8
        JIT_AppendAsmText(jit,"\xDD\x04\x24\x48\x83\xC4\x08",3+4);
        break;
        case AV_FFRAME:
        case AV_IFRAME:
        base=BASEREG_CODE;
        offset=a->f_offset;
        i:
        if(a->size==8) {
            mode=X64ModeMem(NULL,0,NULL,IRegCodeToReg(base),offset,a->size);
            end_ptr=EncodeOpcode(buffer,"POP",NULL,NULL,NULL,NULL,mode);
            JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
            X64ModeFree(mode);
        } else {
            dummy.type=AV_IREG;
            dummy.reg=Str2Reg("RAX");
            Pop(jit,at,op,&dummy);
            IMov(jit,at,a,&dummy,!at->type.is_signed); //TODO Mov,not IMov
        }
        break;
        case AV_IREG_SHADOW:
        base=BASEREG_CODE;
        offset=jit->ireg_shadows+8*a->vreg;
        goto i;
        case AV_FREG_SHADOW:
        base=BASEREG_CODE;
        offset=jit->freg_shadows+8*a->vreg;
        goto i;
        case AV_FINDIR:
        case AV_IINDIR:
        //FINDIR's are always 64-bit
        base=a->reg->code;
        offset=a->offset;
        goto i;
        case AV_IVINDIR:
        base=RMapGetPhysicalReg(at->r_map,a->vreg);
        offset=a->offset;
        goto i;
        case AV_UNOCC:
        //ADD RSP,8
        JIT_AppendAsmText(jit,"\x48\x83\xC4\x08",4);
        break;
    }
    #ifdef JIT_DBG
    if(old_buf_sz==jit->buf_size) {
        "NOTHING EMITED AT %d\n",at->type.code;
    }
    #endif
}
U0 __SetOnCondOp(JIT *jit,CRPN *at,U8 *op,CAsmValue *dst) {
    I64 reg,base,offset,p,old_buf_sz=jit->buf_size;
    U8 *br,*br2;
    CAsmValue dummy,dummy2;
    U8 buffer[32],*end_ptr;
    X64Mode *mode,*mode2,*mode3;
    switch(dst->type) {
        case AV_PUSH:
        dummy.type=AV_INT;
        dummy.int=0;
        Push(jit,at,"PUSH",&dummy);
        base=Str2Reg("RSP")->code;
        offset=0;
        dummy.size=8;
        dummy.type=AV_IINDIR;
        dummy.offset=0;
        dummy.reg=Str2Reg("RSP");
        dst=&dummy;
        goto d_i;
        case AV_IREG:
        reg=dst->reg->code;
        d_r:
        mode=X64ModeReg(Str2Reg("AL"));
        end_ptr=EncodeOpcode(buffer,op,NULL,NULL,NULL,NULL,mode);
        JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
        mode2=X64ModeReg(IRegCodeToReg(reg));
        end_ptr=EncodeOpcode(buffer,"MOVZX",NULL,NULL,NULL,NULL,mode2,mode);
        JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
        X64ModeFree(mode),X64ModeFree(mode2);        
        break;
        case AV_VIREG:
        reg=RMapGetPhysicalReg(at->r_map,dst->vreg);
        goto d_r;
        case AV_IFRAME:
        base=BASEREG_CODE;
        offset=dst->f_offset;
        d_i:
        mode=X64ModeReg(Str2Reg("AL"));
        end_ptr=EncodeOpcode(buffer,op,NULL,NULL,NULL,NULL,mode);
        JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
        X64ModeFree(mode);
        //
        mode2=X64ModeMem(NULL,0,NULL,IRegCodeToReg(base),offset,dst->size);
        switch(dst->size) {
            case 1:
            mode=X64ModeReg(Str2Reg("AL"));
            end_ptr=EncodeOpcode(buffer,"MOV",NULL,NULL,NULL,NULL,mode2,mode);
            JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
            break;
            start:
            //MOVZX RAX,AL
            JIT_AppendAsmText(jit,"\x48\x0F\xB6\xC0",4);
            case 2:
            mode=X64ModeReg(Str2Reg("AX"));
            break;
            case 4:
            mode=X64ModeReg(Str2Reg("EAX"));
            break;
            case 8:
            mode=X64ModeReg(Str2Reg("RAX"));
            end:
            end_ptr=EncodeOpcode(buffer,"MOV",NULL,NULL,NULL,NULL,mode2,mode);
            JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
            break;
        }
        X64ModeFree(mode),X64ModeFree(mode2);
        break;
        case AV_IINDIR:
        base=dst->reg->code;
        offset=dst->offset;
        goto d_i;
        case AV_IVINDIR:
        base=RMapGetPhysicalReg(at->r_map,dst->vreg);
        offset=dst->offset;
        goto d_i;
        
        case AV_IREG_SHADOW:
        base=BASEREG_CODE;
        offset=jit->ireg_shadows+dst->vreg*8;
        goto d_i;
        case AV_FREG:
        case AV_FREG_SHADOW:
        case AV_FFRAME:
        case AV_FINDIR:
        mode=X64ModeReg(Str2Reg("AL"));
        end_ptr=EncodeOpcode(buffer,op,NULL,NULL,NULL,NULL,mode);
        JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
        X64ModeFree(mode);
        /*
        TEST al,al
        JNZ t;
        FLDZ;
        JMP e;
        t:
        FLD1;
        e:
        */
        JIT_AppendAsmText(jit,"\x84\xC0\x75\x04\xD9\xEE\xEB\x02\xD9\xE8",8+2);
        if(dst->type!=AV_FREG)
            __FMov(jit,at,dst,TRUE);
    }
    #ifdef JIT_DBG
    if(old_buf_sz==jit->buf_size) {
        "NOTHING EMITED AT %d\n",at->type.code;
    }
    #endif
}
//Leave dst NULL for raw cmp,use SETcc
U0 ICmpOp(JIT *jit,CRPN *at,U8 *op,CAsmValue *dst,CAsmValue *a,CAsmValue *b,Bool unsigned=FALSE) {
    I64 reg,base,p,offset,p2,imm,old_buf_sz=jit->buf_size,stk_adj=0;
    U8 *br,*br2;
    CAsmValue dummy,ddummy;
    CReg *spare;
    U8 buffer[32],*end_ptr;
    X64Mode *mode,*mode2;
    loop:
    switch(a->type) {
        case AV_IREG:
        reg=a->reg->code;
        r:
        switch(b->type) {
            case AV_INT:
            imm=b->int;
            try_imm:
            if(I32_MIN<=imm<=I32_MAX) {
                mode2=X64ModeReg(IRegCodeToReg(reg));
                mode=X64ModeImm(imm);
                end_ptr=EncodeOpcode(buffer,"CMP",NULL,NULL,NULL,NULL,mode2,mode);
                JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
                X64ModeFree(mode),X64ModeFree(mode2);
            } else {
                dummy.type=AV_IREG;
                dummy.reg=Str2Reg("RAX");
                IMov(jit,at,&dummy,b,unsigned);
                ICmpOp(jit,at,op,dst,a,&dummy,unsigned);
                return;
            }
            break;
            case AV_FLT:
            imm=ToI64(b->flt);
            goto try_imm;
            case AV_IREG:
            mode2=X64ModeReg(IRegCodeToReg(reg));
            mode=X64ModeReg(b->reg);
            end_ptr=EncodeOpcode(buffer,"CMP",NULL,NULL,NULL,NULL,mode2,mode);
            JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
            X64ModeFree(mode),X64ModeFree(mode2);
            break;
            case AV_VIREG:
            mode2=X64ModeReg(IRegCodeToReg(reg));
            mode=X64ModeReg(IRegCodeToReg(RMapGetPhysicalReg(at->r_map,b->vreg)));
            end_ptr=EncodeOpcode(buffer,"CMP",NULL,NULL,NULL,NULL,mode2,mode);
            JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
            X64ModeFree(mode),X64ModeFree(mode2);
            break;
        
            case AV_IFRAME:
            base=BASEREG_CODE;
            offset=b->f_offset;
            f1:
            switch(b->size) {
                case 1:
                mode2=X64ModeReg(Str2Reg("AL"));
                IMovRR(jit,0,reg);
                break;
                case 2:
                mode2=X64ModeReg(IRegCodeToReg16(reg));
                break;
                case 4:
                mode2=X64ModeReg(IRegCodeToReg32(reg));
                break;
                case 8:
                mode2=X64ModeReg(IRegCodeToReg(reg));
            }
            mode=X64ModeMem(NULL,0,NULL,IRegCodeToReg(base),offset,b->size);
            end_ptr=EncodeOpcode(buffer,"CMP",NULL,NULL,NULL,NULL,mode2,mode);
            JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
            X64ModeFree(mode),X64ModeFree(mode2);
            break;
            case AV_IINDIR:
            base=b->reg->code;
            offset=b->offset;
            goto f1;
            case AV_IVINDIR:
            base=RMapGetPhysicalReg(at->r_map,b->vreg);
            offset=b->offset;
            goto f1;
        
            case AV_IREG_SHADOW:
            base=BASEREG_CODE;
            offset=jit->ireg_shadows+8*b->vreg;
            goto f1;
        }
        break;
        case AV_VIREG:
        reg=RMapGetPhysicalReg(at->r_map,a->vreg);
        goto r;
        case AV_IFRAME:
        base=BASEREG_CODE;
        offset=a->f_offset;
        i:
        switch(b->type) {
            start:
            case AV_INT:
            imm=b->int;
            break;
            case AV_FLT:
            imm=b->flt;
            end:
            if(I32_MIN<=imm<=I32_MAX) {
                mode2=X64ModeMem(NULL,0,NULL,IRegCodeToReg(base),offset,a->size);
                mode=X64ModeImm(imm);
                end_ptr=EncodeOpcode(buffer,"CMP",NULL,NULL,NULL,NULL,mode2,mode);
                JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
                X64ModeFree(mode),X64ModeFree(mode2);
                break;
            }
            m2rax:
            dummy.type=AV_IREG;
            dummy.reg=Str2Reg("RAX");
            IMov(jit,at,&dummy,b,unsigned);
            reg=Str2Reg("RAX")->code;
            r2:
            mode2=X64ModeMem(NULL,0,NULL,IRegCodeToReg(base),offset,a->size);
            switch(a->size) {
                case 1:
                mode=X64ModeReg(Str2Reg("AL"));
                IMovRR(jit,0,reg);
                break;
                case 2:
                mode=X64ModeReg(IRegCodeToReg16(reg));
                break;
                case 4:
                mode=X64ModeReg(IRegCodeToReg32(reg));
                break;
                case 8:
                mode=X64ModeReg(IRegCodeToReg(reg));
            }
            end_ptr=EncodeOpcode(buffer,"CMP",NULL,NULL,NULL,NULL,mode2,mode);
            JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
            X64ModeFree(mode),X64ModeFree(mode2);
            break;
            case AV_IREG:
            reg=b->reg->code;
            goto r2;
            case AV_VIREG:
            reg=RMapGetPhysicalReg(at->r_map,b->vreg);
            goto r2;
            case AV_IVINDIR:
            case AV_IINDIR:
            case AV_IREG_SHADOW:
            goto m2rax;
        }
        break;
        case AV_IINDIR:
        base=a->reg->code;
        offset=a->offset;
        goto i;
        case AV_IVINDIR:
        base=RMapGetPhysicalReg(at->r_map,a->vreg);
        offset=a->offset;
        goto i;
        case AV_IREG_SHADOW:
        base=BASEREG_CODE;
        offset=jit->ireg_shadows*8*a->vreg;
        goto i;
        break;
        case AV_FLT:
        case AV_INT:
        if(spare=__JIT_GetFreeIReg(jit,at)) {
            dummy.type=AV_IREG;
            dummy.reg=spare;
            IMov(jit,at,&dummy,a,unsigned);
            reg=spare->code;
            goto r;
        } else {
            Push(jit,at,"PUSH",a);
            ddummy.reg=Str2Reg("RSP");
            ddummy.offset=0;
            ddummy.size=8;
            ddummy.type=AV_IINDIR;
            a=&ddummy;
            stk_adj=8;
            goto loop;
        }
        break;
        
    }
    if(stk_adj==8) 
        IPopR(jit,Str2Reg("RAX")->code);
    #ifdef JIT_DBG
    if(old_buf_sz==jit->buf_size) {
        "NOTHING EMITED AT %d\n",at->type.code;
    }
    #endif
    if(dst)
        __SetOnCondOp(jit,at,op,dst);
}
Bool IsAsmIndirect(CAsmValue *v) {
    switch(v->type) {
        case AV_IINDIR:
        case AV_FINDIR:
        case AV_IVINDIR:
        case AV_FVINDIR:
        return TRUE;
    }
    return FALSE;
}
U0 IArithOp(JIT *jit,CRPN *at,U8 *name,CAsmValue *d,CAsmValue *a,CAsmValue *b,Bool unsigned=FALSE) {
    I64 reg,base,offset,old_buf_sz=jit->buf_size,dreg;
    CAsmValue dummy,dummy2,*orig_d=d;
    U8 buffer[32],*end_ptr;
    X64Mode *mode,*mode2;
    Bool use_treg=FALSE,pop=FALSE;
    if(IsAsmIndirect(a)||IsAsmIndirect(b))
        goto usetr;
    if(!(AsmValueEqual(d,a)||AsmValueEqual(d,b))) {
        IMov(jit,at,d,a,unsigned);
    } else if(AsmValueEqual(d,a)) {
    } else  {
        usetr:
        dummy2.type=AV_IREG;
        dummy2.reg=__JIT_GetFreeIReg(jit,at);
        if(!dummy2.reg) {
            dummy2.type=AV_IINDIR;
            dummy2.reg=Str2Reg("RSP");
            dummy2.size=8;
            dummy2.offset=0;
            Push(jit,at,"PUSH",a);
            pop=TRUE;
        } else {
            use_treg=TRUE;
            IMov(jit,at,&dummy2,a,unsigned);
        }
        d=&dummy2;
    }
    loop:
    switch(d->type) {
        case AV_PUSH:
        Push(jit,at,"PUSH",d);
        base=Str2Reg("RSP")->code;
        offset=0;
        dummy.size=8;
        dummy.type=AV_IINDIR;
        dummy.offset=0;
        dummy.reg=Str2Reg("RSP");
        d=&dummy;
        goto loop;
        case AV_IREG:
        dreg=d->reg->code;
        r:
        switch(b->type) {
            case AV_IREG:
            reg=b->reg->code;
            r2:
            mode2=X64ModeReg(IRegCodeToReg(dreg));
            mode=X64ModeReg(IRegCodeToReg(reg));
            end_ptr=EncodeOpcode(buffer,name,NULL,NULL,NULL,NULL,mode2,mode);
            JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
            X64ModeFree(mode),X64ModeFree(mode2);
            break;
            case AV_VIREG:
            reg=RMapGetPhysicalReg(at->r_map,b->vreg);
            goto r2;
            break;
            case AV_IFRAME:
            base=BASEREG_CODE;
            offset=b->f_offset;
            i2:
            reg=dreg;
            if(b->size==8) {
                mode2=X64ModeReg(IRegCodeToReg(dreg));
                mode=X64ModeMem(NULL,0,NULL,IRegCodeToReg(base),offset,8);
                end_ptr=EncodeOpcode(buffer,name,NULL,NULL,NULL,NULL,mode2,mode);
                JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
                X64ModeFree(mode),X64ModeFree(mode2);
            } else
                goto mrax; 
            break;
            case AV_IINDIR:
            base=b->reg->code;
            offset=b->offset;
            goto i2;
            case AV_IVINDIR:
            base=RMapGetPhysicalReg(at->r_map,b->vreg);
            offset=b->offset;
            goto i2;        
            case AV_IREG_SHADOW:
            base=BASEREG_CODE;
            offset=jit->ireg_shadows+8*b->vreg;
            goto i2;
            case AV_INT:
            if(I32_MIN<=b->int<=I32_MAX) {
                mode2=X64ModeReg(IRegCodeToReg(dreg));
                mode=X64ModeImm(b->int);
                end_ptr=EncodeOpcode(buffer,name,NULL,NULL,NULL,NULL,mode2,mode);
                JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
                X64ModeFree(mode),X64ModeFree(mode2);
                break;
            }
            default:
            mrax:
            dummy.type=AV_IREG;
            dummy.reg=Str2Reg("RAX");
            reg=dummy.reg->code;
            IMov(jit,at,&dummy,b,unsigned);
            b=&dummy;
            goto r2;
            break;
        }
        break;
        case AV_VIREG:
        dreg=RMapGetPhysicalReg(at->r_map,d->vreg);
        goto r;
        break;
        case AV_IFRAME:
        base=BASEREG_CODE;
        offset=d->f_offset;
        i:
        switch(b->type) {
            case AV_IREG:
            reg=b->reg->code;
            r3:
            mode2=X64ModeMem(NULL,0,NULL,IRegCodeToReg(base),offset,d->size);
            switch(d->size) {
                case 1:
                mode=X64ModeReg(Str2Reg("AL"));
                IMovRR(jit,0,reg);
                break;
                case 2:
                mode=X64ModeReg(IRegCodeToReg16(reg));
                break;
                case 4:
                mode=X64ModeReg(IRegCodeToReg32(reg));
                break;
                case 8:
                mode=X64ModeReg(IRegCodeToReg(reg));
                break;
                default:
                "%d,%d,%d,%p,%p,%p\n",d->size,d->type,b->type,d,&dummy,&dummy2;
                Debugger;
            }
            end_ptr=EncodeOpcode(buffer,name,NULL,NULL,NULL,NULL,mode2,mode);
            JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
            X64ModeFree(mode),X64ModeFree(mode2);
            break;
            
            case AV_VIREG:
            reg=RMapGetPhysicalReg(at->r_map,b->vreg);
            goto r3;
            case AV_INT:
            if(I32_MIN<=b->int<=I32_MAX) {
                mode2=X64ModeMem(NULL,0,NULL,IRegCodeToReg(base),offset,d->size);
                mode=X64ModeImm(b->int);
                switch(d->size) {
                    case 1:
                    b->int&=0xff;
                    break;
                    case 2:
                    b->int&=0xffFF;
                    break;
                    case 4:
                    b->int&=0xffFFffFF;
                    break;
                }
                end_ptr=EncodeOpcode(buffer,name,NULL,NULL,NULL,NULL,mode2,mode);
                JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
                X64ModeFree(mode),X64ModeFree(mode2);
                break;
            }
            default:
            dummy.type=AV_IREG;
            dummy.reg=Str2Reg("RAX");
            IMov(jit,at,&dummy,b,unsigned);
            b=&dummy;
            goto i;
        }
        break;
        case AV_IINDIR:
        base=d->reg->code;
        offset=d->offset;
        goto i;
        case AV_IVINDIR:
        base=RMapGetPhysicalReg(at->r_map,d->vreg);
        offset=d->offset;
        goto i;
        case AV_IREG_SHADOW:
        base=BASEREG_CODE;
        offset=jit->ireg_shadows+8*d->vreg;
        goto i;
    }
    if(use_treg)
        IMov(jit,at,orig_d,&dummy2,unsigned);
    else if(pop) {
        Pop(jit,at,"POP",orig_d);
    }
    #ifdef JIT_DBG
    if(old_buf_sz==jit->buf_size) {
        "NOTHING EMITED AT %d\n",at->type.code;
    }
    #endif

}
Bool IRegIsAliveAfter(CRPN *at,CReg *reg) {
    I64 p;
    if(-1!=(p=RMapGetVirtualReg(at->r_map,reg->code))) {
        if(Bt(at->live_out,p))
            return TRUE;
    }
    return FALSE;
}
//
// >..<      Be sure not to pass RDX to this function
//  ww______
//  \______/
//
U0 __Div(JIT *jit,CRPN *at,U8 *op,CAsmValue *a,CAsmValue *b) {
    CAsmValue dummy,cdummy;
    I64 reg,p,base,offset,old_buf_sz=jit->buf_size;
    Bool pop=FALSE,pop_rdx;
    U8 buffer[32],*end_ptr;
    X64Mode *mode,*mode2;
    cdummy.type=AV_IREG;
    if(!(cdummy.reg=__JIT_GetFreeIReg(jit,at)))
        goto push;
    else if(cdummy.reg==Str2Reg("RDX"))
        goto push;
    loop:
    switch(b->type) {
        case AV_INT:
        m2rcx:
        if(!cdummy.reg) {
            push:
            pop=TRUE;
            Push(jit,at,"PUSH",b);
            cdummy.type=AV_IINDIR;
            cdummy.reg=Str2Reg("RSP");
            cdummy.offset=0;
            cdummy.size=8;
            b=&cdummy;
            goto loop;
        }
        IMov(jit,at,&cdummy,b,FALSE);
        b=&cdummy;
        reg=b->reg->code;
        goto reg;
        case AV_FLT:
        break;
        case AV_IREG:
        reg=b->reg->code;
        reg:
        dummy.type=AV_IREG;
        dummy.reg=Str2Reg("RAX");
        IMov(jit,at,&dummy,a,FALSE);
        //CQO
        JIT_AppendAsmText(jit,"\x48\x99",2);
        mode=X64ModeReg(IRegCodeToReg(reg));
        end_ptr=EncodeOpcode(buffer,op,NULL,NULL,NULL,NULL,mode);
        JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
        X64ModeFree(mode);
        break;
        case AV_VIREG:
        reg=RMapGetPhysicalReg(at->r_map,b->vreg);
        goto reg;
        case AV_IFRAME:
        base=BASEREG_CODE;
        offset=b->f_offset;
        i:
        if(b->size==8) {
            dummy.type=AV_IREG;
            dummy.reg=Str2Reg("RAX");
            IMov(jit,at,&dummy,a,FALSE);
            //CQO
            JIT_AppendAsmText(jit,"\x48\x99",2);
            mode=X64ModeMem(NULL,0,NULL,IRegCodeToReg(base),offset,8);
            end_ptr=EncodeOpcode(buffer,op,NULL,NULL,NULL,NULL,mode);
            JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
            X64ModeFree(mode);
        } else
            goto m2rcx;
        break;
        case AV_IINDIR:
        base=b->reg->code;
        offset=b->offset;
        goto i;
        case AV_IVINDIR:
        base=RMapGetPhysicalReg(at->r_map,b->vreg);
        offset=b->offset;
        goto i;
        case AV_IREG_SHADOW:
        base=BASEREG_CODE;
        offset=jit->ireg_shadows+8*b->vreg;
        goto i;
    }
    if(pop)
        JIT_AppendAsmText(jit,"\x48\x83\xC4\x08",4);
    #ifdef JIT_DBG
    if(old_buf_sz==jit->buf_size) {
        "NOTHING EMITED AT %d\n",at->type.code;
    }
    #endif
}
U8 *StrPrintJoin2(U8 *dst,U8 *fmt,...) {
    return StrPrintJoin(dst,fmt,argc,argv);
}
U0 __X87Push(JIT *jit,CRPN *at,CAsmValue *v) {
    I64 p,old_buf_sz=jit->buf_size,base,offset;
    U8 buffer[32],*end_ptr;
    X64Mode *mode;
    switch(v->type) {
        start:
        case AV_INT:
        case AV_IREG:
        case AV_VIREG:
        case AV_IINDIR:
        case AV_IVINDIR:
        Push(jit,at,"PUSH",v);
        end:
        //FILD I64 [RSP];
        //ADD RSP,8;
        JIT_AppendAsmText(jit,"\xDF\x2C\x24\x48\x83\xC4\x08",3+4);
        break;
        case AV_FLT:
        Push(jit,at,"PUSH",v);
        //FLD I64 [RSP];
        //ADD RSP,8;
        JIT_AppendAsmText(jit,"\xDD\x04\x24\x48\x83\xC4\x08",3+4);
        break;
        
        case AV_IFRAME:
        offset=v->offset;
        base=BASEREG_CODE;
        ii:
        mode=X64ModeMem(NULL,0,NULL,IRegCodeToReg(base),offset,8);
        end_ptr=EncodeOpcode(buffer,"FILD",NULL,NULL,NULL,NULL,mode);
        JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
        X64ModeFree(mode);
        break;
        case AV_FFRAME:
        offset=v->offset;
        base=BASEREG_CODE;
        goto fi;
        break;
        case AV_FVINDIR:
        base=RMapGetPhysicalReg(at->r_map,v->vreg);
        offset=v->offset;
        goto fi;
        case AV_FINDIR:
        base=v->reg->code;
        offset=v->offset;
        fi:
        mode=X64ModeMem(NULL,0,NULL,IRegCodeToReg(base),offset,8);
        end_ptr=EncodeOpcode(buffer,"FLD",NULL,NULL,NULL,NULL,mode);
        JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
        X64ModeFree(mode);
        break;
        case AV_IREG_SHADOW:
        base=BASEREG_CODE;
        offset=jit->ireg_shadows+8*v->vreg;
        goto ii;
        break;
        case AV_FREG_SHADOW:
        offset=jit->freg_shadows+8*v->vreg;
        base=BASEREG_CODE;
        goto fi;
    }
    #ifdef JIT_DBG
    if(old_buf_sz==jit->buf_size) {
        "NOTHING EMITED AT %d\n",at->type.code;
    }
    #endif
}
U0 __FMov(JIT *jit,CRPN *at,CAsmValue *dst,Bool pop) {
    I64 reg,base,offset,reg2,old_buf_sz=jit->buf_size;
    U8 buffer[32],*end_ptr;
    X64Mode *mode;
    switch(dst->type) {
        case AV_PUSH:
        //SUB RSP,8
        JIT_AppendAsmText(jit,"\x48\x83\xEC\x08",4);
        mode=X64ModeMem(NULL,0,NULL,Str2Reg("RSP"),0,8);
        if(pop)
            end_ptr=EncodeOpcode(buffer,"FSTP",NULL,NULL,NULL,NULL,mode);
        else
            end_ptr=EncodeOpcode(buffer,"FST",NULL,NULL,NULL,NULL,mode);
        JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
        X64ModeFree(mode);
        break;
        case AV_FREG:
        return;
        start:
            //SUB RSP,8
            JIT_AppendAsmText(jit,"\x48\x83\xEC\x08",4);mode=X64ModeMem(NULL,0,NULL,Str2Reg("RSP"),0,8);
            mode=X64ModeMem(NULL,0,NULL,Str2Reg("RSP"),0,8);
            if(pop)
                end_ptr=EncodeOpcode(buffer,"FSTP",NULL,NULL,NULL,NULL,mode);
            else
                end_ptr=EncodeOpcode(buffer,"FST",NULL,NULL,NULL,NULL,mode);
            JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
            X64ModeFree(mode);
        case AV_VIREG:
            reg=RMapGetPhysicalReg(at->r_map,dst->vreg);
            break;
        case AV_IREG:
            reg=dst->reg->code;
        end:
            IPopR(jit,reg);
        return;
        case AV_FFRAME:
        base=BASEREG_CODE;
        offset=dst->f_offset;
        i:
        mode=X64ModeMem(NULL,0,NULL,IRegCodeToReg(base),offset,8);
        if(pop)
            end_ptr=EncodeOpcode(buffer,"FSTP",NULL,NULL,NULL,NULL,mode);
        else
            end_ptr=EncodeOpcode(buffer,"FST",NULL,NULL,NULL,NULL,mode);
        JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
        X64ModeFree(mode);
        break;
        case AV_FINDIR:
        base=dst->reg->code;
        offset=dst->offset;
        goto i;
        case AV_IVINDIR:
        base=RMapGetPhysicalReg(at->r_map,dst->vreg);
        offset=dst->offset;
        goto ii;
        case AV_IINDIR:
        base=dst->reg->code;
        offset=dst->offset;
        ii:
        mode=X64ModeMem(NULL,0,NULL,IRegCodeToReg(base),offset,8);
        if(pop)
            end_ptr=EncodeOpcode(buffer,"FISTP",NULL,NULL,NULL,NULL,mode);
        else
            end_ptr=EncodeOpcode(buffer,"FIST",NULL,NULL,NULL,NULL,mode);
        JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
        X64ModeFree(mode);
        break;
        case AV_IREG_SHADOW:
        base=BASEREG_CODE;
        offset=jit->ireg_shadows+8*dst->vreg;
        goto ii;
        case AV_FREG_SHADOW:
        base=BASEREG_CODE;
        offset=jit->freg_shadows+8*dst->vreg;
        goto i;
        case AV_FVINDIR:
        base=RMapGetPhysicalReg(at->r_map,dst->vreg);
        offset=dst->offset;
        goto i;
        case AV_UNOCC:
        //FSTP ST0;
        JIT_AppendAsmText(jit,"\xDD\xD8",2);
        break;
    }
    #ifdef JIT_DBG
    if(old_buf_sz==jit->buf_size) {
        "NOTHING EMITED AT %d\n",at->type.code;
    }
    #endif
}
//Use pop variant
U0 FPUOp(JIT *jit,CRPN *at,U8 *op,CAsmValue *a,CAsmValue *b) {
    I64 old_buf_sz=jit->buf_size;
    I64 pop=0;
    U8 buffer[32],*end_ptr;
    X64Mode *mode,*mode2;
    if(a->type==AV_FREG&&b->type==AV_FREG) {
    } else if(a->type==AV_FREG) {
        __X87Push(jit,at,b);
    } else if(b->type==AV_FREG) {
        __X87Push(jit,at,a);
        //FLD ST1
        JIT_AppendAsmText(jit,"\xD9\xC1",2);
        pop=1;
    } else {
        __X87Push(jit,at,a);
        __X87Push(jit,at,b);
    }
    mode2=X64ModeReg(Str2Reg("ST0"));
    mode=X64ModeReg(Str2Reg("ST1"));
    end_ptr=EncodeOpcode(buffer,op,NULL,NULL,NULL,NULL,mode,mode2);
    JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
    X64ModeFree(mode),X64ModeFree(mode2);        
    while(--pop>=0)
        JIT_AppendAsmText(jit,"\xDD\xD9",2); //FSTP ST1
    #ifdef JIT_DBG
    if(old_buf_sz==jit->buf_size) {
        "NOTHING EMITED AT %d\n",at->type.code;
    }
    #endif
}
U0 FPRemOp(JIT *jit,CRPN *at,CAsmValue *a,CAsmValue *b) {
    I64 old_buf_sz=jit->buf_size;
    I64 pop=0;
    if(a->type==AV_FREG&&b->type==AV_FREG) {
    } else if(a->type==AV_FREG) {
        __X87Push(jit,at,b);
    } else if(b->type==AV_FREG) {
        __X87Push(jit,at,a);
        //FLD ST1
        JIT_AppendAsmText(jit,"\xD9\xC1",2);
        pop=2;
    } else {
        __X87Push(jit,at,a);
        __X87Push(jit,at,b);
        pop=1;
    }
    JIT_AppendAsmText(jit,"\xD9\xF8",2); //FPREM;
    while(--pop>=0)
        JIT_AppendAsmText(jit,"\xDD\xD9",2); //FSTP ST1
    #ifdef JIT_DBG
    if(old_buf_sz==jit->buf_size) {
        "NOTHING EMITED AT %d\n",at->type.code;
    }
    #endif
}

U0 FuncCall(JIT *jit,CRPN *at,CAsmValue *func,CAsmValue *ret,I64 argc,CRPN **argv) {
    I64 idx,v,p,old_buf_sz=jit->buf_size,creg;
    CAsmValue dummy;
    U8 buffer[32],*end_ptr;
    X64Mode *mode,*mode2;
    U8 *ikeep[]={
        "RSI",
        "RDI",
        "R10",
        "R11",
        "R12",
        "R13",
        "R14",
        "R15",
    };
    //We push the call address to the stack,and use RET to pop the stack and jump to its location
    switch(func->type) {
        default:
        dummy.type=AV_IREG;
        dummy.reg=Str2Reg("RAX");
        IMov(jit,at,&dummy,func,FALSE);
        mode=X64ModeReg(Str2Reg("RAX"));
        break;
        case AV_VIREG:
        creg=RMapGetPhysicalReg(at->r_map,func->vreg);
        mode=X64ModeReg(IRegCodeToReg(creg));
        break;
        case AV_IREG:
        mode=X64ModeReg(func->reg);
        break;
        case AV_IVINDIR:
        mode=X64ModeMem(NULL,0,NULL,IRegCodeToReg(RMapGetPhysicalReg(at->r_map,func->vreg)),func->offset,8);
        break;
        case AV_IINDIR:
        mode=X64ModeMem(NULL,0,NULL,func->reg,func->offset,8);
    }
    end_ptr=EncodeOpcode(buffer,"CALL",NULL,NULL,NULL,NULL,mode);
    JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
    X64ModeFree(mode);
    if(argc>at->type.vargs_base) {
		mode=X64ModeReg(Str2Reg("RSP"));
		mode2=X64ModeImm((argc-at->type.vargs_base)*8);
		end_ptr=EncodeOpcode(buffer,"ADD",NULL,NULL,NULL,NULL,mode,mode2);
		JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
		X64ModeFree(mode),X64ModeFree(mode2);
	}
    //If we call using a non caller saved register,unspill it after we call the function as its value is discarded
    switch(func->type) {
        case AV_IVINDIR:
        case AV_VIREG:
        if(Bt(at->live_out,func->vreg)) {
            creg=RMapGetPhysicalReg(at->r_map,func->vreg);
            if(!IsCallerSaved(IRegCodeToReg(creg)))
                IMovRInd(jit,creg,BASEREG_CODE,8*func->vreg+jit->ireg_shadows,8,FALSE);
        }
    }
    switch(ret->type) {
        case AV_FREG_SHADOW:
        IMovIndR(jit,BASEREG_CODE,jit->freg_shadows+8*ret->vreg,0,8); //0 is RAX's code
        break;
        case AV_FREG:
        rfr:
        dummy.type=AV_IREG;
        dummy.reg=Str2Reg("RAX");
        Push(jit,at,"PUSH",&dummy);
        dummy.type=AV_FINDIR;
        dummy.offset=0;
        dummy.size=8;
        dummy.reg=Str2Reg("RSP");
        __X87Push(jit,at,&dummy);
        __FMov(jit,at,ret,FALSE);
        //ADD RSP,8
        JIT_AppendAsmText(jit,"\x48\x83\xC4\x08",4);
        break;
        default:;
        dummy.type=AV_IREG;
        dummy.reg=Str2Reg("RAX");
        IMov(jit,at,ret,&dummy,FALSE);
    }
    #ifdef JIT_DBG
    if(old_buf_sz==jit->buf_size) {
        "NOTHING EMITED AT %d\n",at->type.code;
    }
    #endif
}
U0 FBinaryOp(JIT *jit,CRPN *at,U8 *op,CAsmValue *a,CAsmValue *b) {
    CAsmValue dummy,dummy2;
    I64 old_buf_sz=jit->buf_size;
    U8 buffer[32],*end_ptr;
    X64Mode *mode,*mode2;
    //AssignX87Regs will mark binop args as popped if both are regs,so pop
    if(a->type==AV_FREG&&b->type==AV_FREG) {    
        //SUB RSP,16
        //FSTP F64 [RSP+8]
        //FSTP F64 [RSP]
        JIT_AppendAsmText(jit,"\x48\x83\xEC\x10\xDD\x5C\x24\x08\xDD\x1C\x24",8+3);
    } else {
        switch(a->type) {
            default:
            Push(jit,at,"PUSH",a);
            break;
            case AV_INT:
            case AV_VIREG:
            case AV_IREG:
            case AV_IFRAME:
            case AV_IINDIR:
            case AV_IVINDIR:
            case AV_IREG_SHADOW:
            __X87Push(jit,at,a);
            dummy.type=AV_FREG;
            Push(jit,at,"PUSH",&dummy);
        }
        switch(b->type) {
            default:
            Push(jit,at,"PUSH",b);
            break;
            case AV_INT:
            case AV_IREG:
            case AV_VIREG:
            case AV_IFRAME:
            case AV_IINDIR:
            case AV_IVINDIR:
            case AV_IREG_SHADOW:
            __X87Push(jit,at,b);
            dummy.type=AV_FREG;
            Push(jit,at,"PUSH",&dummy);
        }
        
    }
    //MOV RAX,I64[RSP+8]
    JIT_AppendAsmText(jit,"\x48\x8B\x44\x24\x08",5);
    mode=X64ModeMem(NULL,0,NULL,Str2Reg("RSP"),0,8);
    mode2=X64ModeReg(Str2Reg("RAX"));
    end_ptr=EncodeOpcode(buffer,op,NULL,NULL,NULL,NULL,mode,mode2);
    X64ModeFree(mode),X64ModeFree(mode2);
    JIT_AppendAsmText(jit,buffer,end_ptr-buffer);    
    //FLD I64 [RSP];
    //ADD RSP,16;
    JIT_AppendAsmText(jit,"\xDD\x04\x24\x48\x83\xC4\x10",3+4);
    #ifdef JIT_DBG
    if(old_buf_sz==jit->buf_size) {
        "NOTHING EMITED AT %d\n",at->type.code;
    }
    #endif
}
U0 ShiftOp(JIT *jit,CRPN *at,U8 *op,CAsmValue *dst,CAsmValue *src,CAsmValue *shift,Bool unsigned=FALSE) {
    CAsmValue dummy,dst2,dummy2;
    I64 reg,base,offset,p,bits,old_buf_sz=jit->buf_size;
    CReg *tmp_reg;
    Bool pop=FALSE;
    U8 buffer[32],*end_ptr;
    X64Mode *mode,*mode2;
    if(src->type==AV_FREG&&shift->type==AV_FREG) {
        if(IRegIsAliveAfter(at,Str2Reg("RDX")))
            IPushR(jit,Str2Reg("RDX")->code);
        //SUB RSP,16
        //FSTP QWORD PTR [RSP+8]
        //FISTP QWORD PTR [RSP]
        JIT_AppendAsmText(jit,"\x48\x83\xEC\x10\xDD\x5C\x24\x08\xDF\x3C\x24",8+3);
        dummy.type=AV_IINDIR;
        dummy.size=8;
        dummy.reg=Str2Reg("RDX");
        dummy.offset=8;
        dummy2.type=AV_IINDIR;
        dummy2.size=8;
        dummy2.reg=Str2Reg("RDX");
        dummy2.offset=0;
        ShiftOp(jit,at,op,&dummy2,&dummy,&dummy2);
        //FLD QWORD PTR [RSP]
        //ADD RSP,16
        JIT_AppendAsmText(jit,"\xDD\x04\x24\x48\x83\xC4\x10",3+4);
        if(IRegIsAliveAfter(at,Str2Reg("RDX")))
            IPopR(jit,Str2Reg("RDX")->code);
        return;
    }
    switch(dst->type) {     
        case AV_FREG:
        pop=TRUE;
        case AV_PUSH:
        push:
        Push(jit,at,"PUSH",src);
        dst2.type=AV_IINDIR;
        dst2.offset=0;
        dst2.reg=Str2Reg("RSP");
        dst2.size=8;
        break;
        default:
        if(AsmValueEqual(dst,shift)) {
            pop=TRUE;
            goto push;
        }
        IMov(jit,at,dst,src,unsigned);
        dst2=*dst;
    }
    if(shift->type==AV_INT||shift->type==AV_FLT) {
        if(shift->type==AV_INT) bits=shift->int;
        else bits=shift->flt;
        switch(dst2.type) {
            case AV_IREG:
            reg=dst2.reg->code;
            r0:
            mode=X64ModeReg(IRegCodeToReg(reg));
            mode2=X64ModeImm(bits);
            end_ptr=EncodeOpcode(buffer,op,NULL,NULL,NULL,NULL,mode,mode2);
            X64ModeFree(mode),X64ModeFree(mode2);
            JIT_AppendAsmText(jit,buffer,end_ptr-buffer);    
            break;
            case AV_VIREG:
            reg=RMapGetPhysicalReg(at->r_map,dst2.vreg);
            goto r0;
            case AV_IFRAME:
            case AV_FFRAME:
            base=BASEREG_CODE;
            offset=dst2.f_offset;
            i0:
            mode=X64ModeMem(NULL,0,NULL,IRegCodeToReg(base),offset,dst2.size);
            mode2=X64ModeImm(bits);
            end_ptr=EncodeOpcode(buffer,op,NULL,NULL,NULL,NULL,mode,mode2);
            X64ModeFree(mode),X64ModeFree(mode2);
            JIT_AppendAsmText(jit,buffer,end_ptr-buffer);    
            break;
            case AV_IVINDIR:
            base=RMapGetPhysicalReg(at->r_map,dst2.vreg);
            offset=dst2.offset;
            goto i0;
            break;
            case AV_IINDIR:
            case AV_FINDIR:
            base=dst2.reg->code;
            offset=dst2.offset;
            goto i0;
            case AV_IREG_SHADOW:
            base=BASEREG_CODE;
            offset=dst2.f_offset;
            goto i0;
            case AV_FREG_SHADOW:
            base=BASEREG_CODE;
            offset=dst2.f_offset;
            goto i0;
        }
    } else {
        switch(dst2.type) {
            case AV_IREG:
            reg=dst2.reg->code;
            r:
            if(reg==Str2Reg("RCX")->code) {
                IPushR(jit,Str2Reg("RCX")->code);
                pop=TRUE;
                goto i;
            }
            dummy.type=AV_IREG;
            dummy.reg=Str2Reg("RCX");
            IMov(jit,at,&dummy,shift,unsigned);
            mode=X64ModeReg(IRegCodeToReg(reg));
            mode2=X64ModeReg(Str2Reg("CL"));
            end_ptr=EncodeOpcode(buffer,op,NULL,NULL,NULL,NULL,mode,mode2);
            X64ModeFree(mode),X64ModeFree(mode2);
            JIT_AppendAsmText(jit,buffer,end_ptr-buffer);    
            break;
            case AV_VIREG:
            reg=RMapGetPhysicalReg(at->r_map,dst2.vreg);
            goto r;
            case AV_IFRAME:
            case AV_FFRAME:
            base=BASEREG_CODE;
            offset=dst2.f_offset;
            i:
            dummy.type=AV_IREG;
            dummy.reg=Str2Reg("RCX");
            IMov(jit,at,&dummy,shift,unsigned);
            mode=X64ModeMem(NULL,0,NULL,IRegCodeToReg(base),offset,dst2.size);
            mode2=X64ModeReg(Str2Reg("CL"));
            end_ptr=EncodeOpcode(buffer,op,NULL,NULL,NULL,NULL,mode,mode2);
            X64ModeFree(mode),X64ModeFree(mode2);
            JIT_AppendAsmText(jit,buffer,end_ptr-buffer);    
            break;
            case AV_IVINDIR:
            base=RMapGetPhysicalReg(at->r_map,dst2.vreg);
            offset=dst2.offset;
            goto i;
            case AV_IINDIR:
            case AV_FINDIR:
            base=dst2.reg->code;
            offset=dst2.offset;
            goto i;
            case AV_IREG_SHADOW:
            base=BASEREG_CODE;
            offset=dst2.f_offset;
            goto i;
            case AV_FREG_SHADOW:
            base=BASEREG_CODE;
            offset=dst2.f_offset;
            goto i;
        }
    }
    if(pop) {
        Pop(jit,at,"POP",dst);
    }
    #ifdef JIT_DBG
    if(old_buf_sz==jit->buf_size) {
        "NOTHING EMITED AT %d\n",at->type.code;
    }
    #endif
}
U0 Patch32(JIT *jit,I64 from,I64 to) {
    (jit->buffer+from)(U32*)[0]=to-(from+4);
}
Bool IsControlNode(CRPN *item) {
    return IsRPNUnop(item)
    ||IsRPNBinop(item)
    ||item->type.code==OP_CALL
    ||item->type.code==OP_RELOCATION
    ||item->type.code==OP_IF
    ||item->type.code==OP_JMP
    ||item->type.code==OP_RETURN
    ||item->type.code==OP_JMP_TABLE;
}
//TODO document
CRPN *GetRPNEnterLabel(CRPN *item) {
    if(!item) return item;
    I64 iter;
    CRPN *last;
    if(IsControlNode(item)) {
        if(item->type.code==OP_CALL) {
            last=item;
            item=item->next;
            for(iter=0;iter!=last->type.body_length+1;iter++) {
                if(IsControlNode(item)||item->type.code==OP_INPLACE_PUSH) {
                    return GetRPNEnterLabel(item);
                }
                item=ComputeRPNSuccessor(item);
            }
            return last;
        }
        if((IsRPNBinop(item)||IsRPNUnop(item)||item->type.code==OP_IF||item->type.code==OP_RETURN)&&(IsControlNode(item->next)||item->next->type.code==OP_BODY))
            return GetRPNEnterLabel(item->next);
        if(IsRPNBinop(item)&&(IsControlNode(last=ComputeRPNSuccessor(item->next))||last->type.code==OP_BODY))
            return GetRPNEnterLabel(last);
    }
    return item;
}
U0 JIT_DebuggerHit(U0 *,U0 *,U0 *);
I64 JTabCmp(I64 a,I64 b) {
    return a-b;
}
U0 CreateJmpTableTrampolines(JIT *jit,CRPN *j) {
    CVector *unique=VectorNew;
    U8 pad[8];
    X64Mode *mode;
    U8 buffer[32],*end_ptr;
    CRPN **sorted=MAlloc(sizeof(CRPN*)*j->type.body_length),*prev;
    I64 iter,table_start,dptr,iter2;
    MemCpy(sorted,j->jmp_table,MSize(sorted));
    QSortI64(sorted,j->type.body_length,&JTabCmp);
    prev=NULL;
    for(iter=0;iter!=j->type.body_length;iter++) {
        if(prev!=sorted[iter]) {
            prev=sorted[iter];
            //CRPN *dst
            VectorPush(unique,sorted[iter]);
            //I64 patch_addr (fill later)
            VectorPush(unique,0);
        }
    }
    Free(sorted);
    table_start=jit->buf_size;
    for(iter=0;iter!=j->type.body_length;iter++) {
        JIT_AppendAsmText(jit,pad,8);
    }
    mode=X64ModeLabel(NULL);
    for(iter=0;iter!=unique->size;iter+=2) {
        for(iter2=0;iter2!=j->type.body_length;iter2++) {
                dptr=table_start+iter2*8;
            if(j->jmp_table[iter2]==unique->data[iter](CRPN*))
                (jit->buffer+dptr)(U64*)[0]=jit->buf_size-dptr;
        }
        RMapSync(jit,GetRPNEnterLabel(unique->data[iter](CRPN*)),j);
        end_ptr=EncodeOpcode(buffer,"JMP",NULL,NULL,NULL,NULL,mode);
        JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
        JIT_AppendAsmText(jit,"\x44\x33\x22\x11",4);
        unique->data[iter+1]=jit->buf_size-4; //The jmp address
    }
    X64ModeFree(mode);
    j->jmp_table_trampolines=unique;
}
I64 CompileOps(JIT *jit,CVector *flat,I64 st=0,CRPN *stop_at=NULL) {
    CAsmValue dummy,dummy2,*dummy_p;
    Bool unsigned=FALSE;
    CRPN *cur,*el,*exit,*prev,**args,*true,*jmp_to;
    U8 *cond_op,*exit_lab,*jmp_lab,*pad;
    I64 iter,iter2,lab_t_off,lab_f_off,lab_end_off,stack_adj,idx,idx2;
    CReg *avail_reg;
    U8 buffer[32],*end_ptr;
    X64Mode *mode,*mode2;
    Bool pop;
    for(iter=st;iter!=flat->size&&flat->data[iter]!=stop_at;) {
        cur=flat->data[iter];
        cur->bin_off=jit->buf_size;
        for(idx=0;idx!=cur->spills->size;idx+=2) {
            IMovIndR(jit,BASEREG_CODE,jit->ireg_shadows+cur->spills->data[idx]*8,cur->spills->data[idx+1]);
        }
        for(idx=0;idx!=cur->unspills->size;idx+=2) {
            idx2=cur->unspills->data[idx];
            IMovRInd(jit,cur->unspills->data[idx+1],BASEREG_CODE,jit->ireg_shadows+idx2*8,,FALSE);
        }
        if(cur->flags&RPN_F_IMPLICIT_OPERATION) {
            iter++;
            goto next;
        }
        jit->fpu_stk_offset=0;
        if(!cur->type.is_flt) {
            unsigned=!cur->type.is_signed;
            switch(cur->type.code) {
                case OP_JMP_TABLE:
                if(avail_reg=__JIT_GetFreeIReg(jit,cur)) {
                    dummy.type=AV_IREG;
                    dummy.reg=Str2Reg("RAX");
                    IMov(jit,cur,&dummy,&cur->next->my_avalue,TRUE);
                    //SHL RAX,3(mul by 8)
                    JIT_AppendAsmText(jit,"\x48\xC1\xE0\x03",4);
                    mode=X64ModeReg(avail_reg);
                    mode2=X64ModeMem(NULL,0,NULL,Str2Reg("RIP"),0x11223344,8);
                    end_ptr=EncodeOpcode(buffer,"LEA",NULL,&cur->patch_addr,NULL,NULL,mode,mode2);
                    X64ModeFree(mode),X64ModeFree(mode2);
                    cur->patch_addr+=jit->buf_size-4; //-4 ignores sib size
                    JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
                    mode=X64ModeReg(Str2Reg("RAX"));
                    mode2=X64ModeReg(avail_reg);
                    end_ptr=EncodeOpcode(buffer,"ADD",NULL,NULL,NULL,NULL,mode,mode2);
                    JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
                    X64ModeFree(mode),X64ModeFree(mode2);
                } else {
                    dummy.type=AV_IREG;
                    dummy.reg=Str2Reg("RAX");
                    IMov(jit,cur,&dummy,&cur->next->my_avalue,TRUE);
                    //SHL RAX,3(mul by 8)
                    JIT_AppendAsmText(jit,"\x48\xC1\xE0\x03",4);
                    IPushR(jit,1);
                    mode=X64ModeReg(IRegCodeToReg(1));
                    mode2=X64ModeMem(NULL,0,NULL,Str2Reg("RIP"),0x11223344,8);
                    end_ptr=EncodeOpcode(buffer,"LEA",NULL,&cur->patch_addr,NULL,NULL,mode,mode2);
                    X64ModeFree(mode),X64ModeFree(mode2);
                    cur->patch_addr+=jit->buf_size-4; //-4 ignores sib size
                    JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
                    mode=X64ModeReg(Str2Reg("RAX"));
                    mode2=X64ModeReg(IRegCodeToReg(1));
                    end_ptr=EncodeOpcode(buffer,"ADD",NULL,NULL,NULL,NULL,mode,mode2);
                    X64ModeFree(mode),X64ModeFree(mode2);
                    JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
                    IPopR(jit,1);
                }
                //ADD RAX,[RAX]
                //JMP RAX
                JIT_AppendAsmText(jit,"\x48\x03\x00",3);
                JIT_AppendAsmText(jit,"\xFF\xE0",2);
                Patch32(jit,cur->patch_addr,jit->buf_size);
                cur->patch_addr=jit->buf_size;
                CreateJmpTableTrampolines(jit,cur);
                break;
                case OP_BREAKPOINT:
                idx=Str2Reg("RAX")->code;
                IMovRI(jit,idx,cur->operands[0](I64));
                IPushR(jit,idx);
                IMovRI(jit,idx,cur->operands[1](I64));
                IPushR(jit,idx);
                IMovRI(jit,idx,cur->operands[2](I64));
                IPushR(jit,idx);
                IMovRI(jit,idx,&JIT_DebuggerHit);
                //CALL RAX
                //ADD RSP,0x18
                JIT_AppendAsmText(jit,"\xFF\xD0\x48\x83\xC4\x18",2+4);
                break;
                case OP_DEREF:
                //
                //   //  |\ //
                //   ||  || || 
                //  ___________
                // (_         _)
                //   | O   O |
                //   |   .   |
                //   | _____ | 
                //   \ \___/ /
                //    \_____/
                //
                // I am frypot,dont let crowdude eat my fries 
                // Also know that OP_DEREF may be a implicit operation
                if(cur->operands[0]->type==AV_IREG) {
                    dummy.type=AV_IINDIR;
                    dummy.reg=cur->operands[0]->reg;
                    dummy.offset=cur->operands[1](I64);
                    dummy.size=cur->operands[2](I64);
                    IMov(jit,cur,&cur->my_avalue,&dummy,!cur->type.is_signed);
                } else if(cur->operands[0]->type==AV_VIREG) {
                    dummy.type=AV_IVINDIR;
                    dummy.vreg=cur->operands[0]->vreg;
                    dummy.offset=cur->operands[1](I64);
                    dummy.size=cur->operands[2](I64);
                    IMov(jit,cur,&cur->my_avalue,&dummy,!cur->type.is_signed);
                } else {
                    dummy.type=AV_IREG;
                    dummy.reg=Str2Reg("RAX");
                    IMov(jit,cur,&dummy,cur->operands[0],!cur->type.is_signed);
                    dummy.offset=0;
                    dummy.type=AV_IINDIR;
                    dummy.size=cur->operands[2](I64);
                    IMov(jit,cur,&cur->my_avalue,&dummy,!cur->type.is_signed);
                }
                break;
                case OP_TYPECAST_BIT4BIT:
                if(cur->next->type.is_flt) {
                    Push(jit,cur,"PUSH",cur->operands[0]);
                    if(cur->my_avalue.type!=AV_PUSH)
                        Pop(jit,cur,"POP",&cur->my_avalue);
                } else 
                    IMov(jit,cur,&cur->my_avalue,cur->operands[0],!cur->type.is_signed);
                break;
                case OP_VARIABLE:
                break;
                case OP_INPLACE_PUSH:
                Push(jit,cur,"PUSH",&cur->my_avalue);
                break;
                case OP_BODY: //Accounted for with flattended
                case OP_LITERAL:
                break;
                case OP_ALIGN:
                //https://en.wikipedia.org/wiki/Data_structure_alignment
                pad=MAlloc((jit->buf_size-(jit->buf_size%cur->operands[0]))%cur->operands[0]);
                MemSet(pad,cur->operands[1],MSize(pad));
                JIT_AppendAsmText(jit,pad,MSize(pad));
                Free(pad);
                break;
                case OP_REF_DATA:
                if(cur->my_avalue.type==AV_IREG) {
                    mode=X64ModeReg(cur->my_avalue.reg);
                    mode2=X64ModeMem(NULL,0,NULL,Str2Reg("RIP"),0x11223344,8);
                    end_ptr=EncodeOpcode(buffer,"LEA",NULL,&cur->patch_addr,NULL,NULL,mode,mode2);
                    cur->patch_addr+=jit->buf_size-4; //-4 ignores sib size
                    X64ModeFree(mode),X64ModeFree(mode2);
                    JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
                } else if(cur->my_avalue.type==AV_VIREG) {
                    mode=X64ModeReg(IRegCodeToReg(RMapGetPhysicalReg(cur->r_map,cur->my_avalue.vreg)));
                    mode2=X64ModeMem(NULL,0,NULL,Str2Reg("RIP"),0x11223344,8);
                    end_ptr=EncodeOpcode(buffer,"LEA",NULL,&cur->patch_addr,NULL,NULL,mode,mode2);
                    cur->patch_addr+=jit->buf_size-4; //-4 ignores sib size
                    X64ModeFree(mode),X64ModeFree(mode2);
                    JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
                } else {
                    mode=X64ModeReg(Str2Reg("RAX"));
                    mode2=X64ModeMem(NULL,0,NULL,Str2Reg("RIP"),0x11223344,8);
                    end_ptr=EncodeOpcode(buffer,"LEA",NULL,&cur->patch_addr,NULL,NULL,mode,mode2);
                    cur->patch_addr+=jit->buf_size-4; //-4 ignores sib size
                    X64ModeFree(mode),X64ModeFree(mode2);
                    JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
                    dummy.type=AV_IREG;
                    dummy.reg=Str2Reg("RAX");
                    IMov(jit,cur,&cur->my_avalue,&dummy,!cur->type.is_signed);
                }
                break;
                case OP_DU8:
                JIT_AppendAsmText(jit,&cur->operands[0],1);
                break;
                case OP_DU16:
                JIT_AppendAsmText(jit,&cur->operands[0],2);
                break;
                case OP_DU32:
                JIT_AppendAsmText(jit,&cur->operands[0],4);
                break;
                case OP_DU64:
                JIT_AppendAsmText(jit,&cur->operands[0],8);
                break;
                /*if(1) {
                    JIT_AppendAsmText(jit,"//INT: ");
                    for(iter2=0;iter2!=cur->r_map->size;iter2+=2) {
                        JIT_AppendAsmText(jit,"%d=%s,",cur->r_map->data[iter2],IRegCodeToStr(cur->r_map->data[iter2+1]));
                    }
                    JIT_AppendAsmText(jit,"\n");
                }*/
                case OP_RELOCATION:
                //TODO fill in later
                if(cur->my_avalue.type==AV_IREG)
                    mode=X64ModeReg(cur->my_avalue.reg);
                else if(cur->my_avalue.type==AV_VIREG)
                    mode=X64ModeReg(IRegCodeToReg(RMapGetPhysicalReg(cur->r_map,cur->my_avalue.vreg)));
                else
                    mode=X64ModeReg(Str2Reg("RAX"));
                if(!mode->reg) {
                    "%p,%p,%p\n",&cur->r_map,cur->r_map->cap,cur->r_map->size;
                }
                mode2=X64ModeImm(0x1122334455);
                end_ptr=EncodeOpcode(buffer,"MOV",NULL,NULL,&cur->patch_addr,NULL,mode,mode2);
                cur->patch_addr+=jit->buf_size;
                X64ModeFree(mode),X64ModeFree(mode2);
                JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
                if(cur->my_avalue.type!=AV_IREG&&cur->my_avalue.type!=AV_VIREG) {
                    dummy.type=AV_IREG;
                    dummy.reg=Str2Reg("RAX");
                    IMov(jit,cur,&cur->my_avalue,&dummy,!cur->type.is_signed);
                }
                break;
                start:
                    case OP_ASSIGN_ADD:
                    IArithOp(jit,cur,"ADD",cur->operands[0],cur->operands[0],cur->operands[1],unsigned);
                    break;
                    case OP_ASSIGN_SUB:
                    IArithOp(jit,cur,"SUB",cur->operands[0],cur->operands[0],cur->operands[1],unsigned);
                    break;
                    case OP_ASSIGN_SHL:
                    ShiftOp(jit,cur,"SHL",cur->operands[0],cur->operands[0],cur->operands[1]);
                    break;
                    case OP_ASSIGN_SHR:
                    ShiftOp(jit,cur,"SHR",cur->operands[0],cur->operands[0],cur->operands[1]);
                    break;
                    case OP_ASSIGN_AND:
                    IArithOp(jit,cur,"AND",cur->operands[0],cur->operands[0],cur->operands[1],unsigned);
                    break;
                    case OP_ASSIGN_XOR:
                    IArithOp(jit,cur,"XOR",cur->operands[0],cur->operands[0],cur->operands[1],unsigned);
                    break;
                    case OP_ASSIGN_OR:
                    IArithOp(jit,cur,"OR",cur->operands[0],cur->operands[0],cur->operands[1],unsigned);
                end:
                IMov(jit,cur,&cur->my_avalue,cur->operands[0],unsigned);
                break;
                case OP_CALL:
                FuncCall(jit,cur,&GetCallFunction(cur)->my_avalue,&cur->my_avalue,cur->type.body_length,NULL);
                break;
                case OP_RETURN:
                dummy.type=AV_IREG;
                dummy.reg=Str2Reg("RAX");
                IMov(jit,cur,&dummy,cur->operands[0],!cur->type.is_signed);
                mode=X64ModeLabel(NULL);
                end_ptr=EncodeOpcode(buffer,"JMP",NULL,NULL,NULL,NULL,mode);
                X64ModeFree(mode);
                JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
                cur->patch_addr=jit->buf_size;
                JIT_AppendAsmText(jit,"\x00\x00\x00\x00",4);
                break;
                case OP_COMMA:
                IMov(jit,cur,&cur->my_avalue,cur->operands[1],unsigned);
                if(cur->operands[0]->type==AV_FREG) {
                    //FSTP ST0
                    JIT_AppendAsmText(jit,"\xDD\xD8",2);
                }
                break;
                case OP_NE:
                ICmpOp(jit,cur,"SETNE",&cur->my_avalue,cur->operands[0],cur->operands[1]);
                break;
                case OP_EQ:
                ICmpOp(jit,cur,"SETE",&cur->my_avalue,cur->operands[0],cur->operands[1]);
                break;
                case OP_NOT:
                IArithUnop(jit,cur,"NOT",&cur->my_avalue,cur->operands[0]);
                break;
                case OP_GETARG:
                dummy.type=AV_IFRAME;
                dummy.reg=Str2Reg("RBP");
                //
                // Stack
                // 
                // ARGN
                // ...
                // ARG0
                // RET_PTR
                // OLD_RBP<--RBP
                dummy.f_offset=2*8+8*cur->operands[0]->int;
                IMov(jit,cur,&cur->my_avalue,&dummy,!cur->type.is_signed);
                break;
                case OP_IF:
                //Assign to label to the else conditon
                //Skip conditon then body
                true=GetRPNEnterLabel(ComputeRPNSuccessor(cur->next));
                el=GetRPNEnterLabel(ComputeRPNSuccessor(ComputeRPNSuccessor(cur->next)));
                exit=GetRPNEnterLabel(ComputeRPNSuccessor(cur));
                //Check for compare-style operand,use a sassy conditional-style jump
                //SEE Potato Drop
                cond_op=NULL;
                switch(cur->next->type.code) {
                    case OP_EQ:
                    cond_op="JE";
                    break;
                    case OP_NE:
                    cond_op="JNE";
                    break;
                    case OP_GT:
                    //   ____________
                    //  /  \         \ X87 items expect unsigned style compares
                    // |.__.|        |
                    // | \/ |        | - Mr. Noodles
                    //  \__/_________/
                    if(!cur->next->type.is_signed||cur->next->type->is_flt)
                        cond_op="JA";
                    else
                        cond_op="JG";
                    break;
                    case OP_LT:
                    if(!cur->next->type.is_signed||cur->next->type->is_flt)
                        cond_op="JB";
                    else
                        cond_op="JL";
                    break;
                    case OP_GE:
                    if(!cur->next->type.is_signed||cur->next->type->is_flt)
                        cond_op="JAE";
                    else
                        cond_op="JGE";
                    break;
                    case OP_LE:
                    if(!cur->next->type.is_signed||cur->next->type->is_flt)
                        cond_op="JBE";
                    else
                        cond_op="JLE";
                    break;
                    default:
                    switch(cur->operands[0]->type) {
                        start:
                            case AV_FREG:
                            break;
                            case AV_FINDIR:
                            case AV_FFRAME:
                            case AV_FREG_SHADOW:
                            __X87Push(jit,cur,cur->operands[0]);
                            break;
                        end:
                        //FLDZ;
                        //FCOMIP ST0,ST1;
                        //FSTP ST0;
                        JIT_AppendAsmText(jit,"\xD9\xEE\xDF\xF1\xDD\xD8",6);
                        break;
                        default:
                        dummy.type=AV_IREG;
                        dummy.reg=Str2Reg("RAX");
                        IMov(jit,cur,&dummy,cur->operands[0],!cur->type.is_signed);
                        //CMP RAX,0
                        JIT_AppendAsmText(jit,"\x48\x83\xF8\x00",4);
                    }
                    cond_op="JNZ";
                }
                //Jcc TRUE
                mode=X64ModeLabel(NULL);
                end_ptr=EncodeOpcode(buffer,cond_op,NULL,NULL,NULL,NULL,mode);
                //Jump opcodes leave an area after to write our jmp offset
                lab_t_off=jit->buf_size+end_ptr-buffer;
                X64ModeFree(mode);
                JIT_AppendAsmText(jit,buffer,4+end_ptr-buffer); //TempleOS uses 4 byte jumps always
                //
                //sync(false) & JMP FALSE
                //
                RMapSync(jit,el,cur);
                mode=X64ModeLabel(NULL);
                end_ptr=EncodeOpcode(buffer,"JMP",NULL,&lab_f_off,NULL,NULL,mode);
                X64ModeFree(mode);
                JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
                lab_f_off=jit->buf_size;
                JIT_AppendAsmText(jit,"\x00\x00\x00\x00",4);
                //
                //sync(true) & TRUE
                //
                Patch32(jit,lab_t_off,jit->buf_size);
                RMapSync(jit,true,cur);
                iter=CompileOps(jit,flat,iter+1,el);
                //
                //sync(exit) & Jump to end
                //
                if(exit)
                    RMapSync(jit,exit,flat->data[iter-1]);
                mode=X64ModeLabel(NULL);
                end_ptr=EncodeOpcode(buffer,"JMP",NULL,NULL,NULL,NULL,mode);
                X64ModeFree(mode);
                JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
                lab_end_off=jit->buf_size;
                JIT_AppendAsmText(jit,"\x00\x00\x00\x00",4);
                //
                //FALSE
                //
                Patch32(jit,lab_f_off,jit->buf_size);
                iter=CompileOps(jit,flat,iter,exit);
                if(flat->data[iter-1]&&exit)
                    RMapSync(jit,exit,flat->data[iter-1]);
                Patch32(jit,lab_end_off,jit->buf_size);
                goto next;
                case OP_JMP:
                mode=X64ModeLabel(NULL);
                RMapSync(jit,GetRPNEnterLabel(cur->operands[0]),cur);
                end_ptr=EncodeOpcode(buffer,"JMP",NULL,&cur->patch_addr,NULL,NULL,mode);
                X64ModeFree(mode);
                JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
                cur->patch_addr=jit->buf_size;
                JIT_AppendAsmText(jit,"\x00\x00\x00\x00",4);
                break;
                case OP_ADD:
                IArithOp(jit,cur,"ADD",&cur->my_avalue,cur->operands[0],cur->operands[1],unsigned);
                break;
                case OP_SUB:
                IArithOp(jit,cur,"SUB",&cur->my_avalue,cur->operands[0],cur->operands[1],unsigned);
                break;
                //These ones are special
                start:
                stack_adj=0;
                //  _          _    ____________________________________
                // <*>        <*>  /                                    \   
                //  \\        //   | SPILL RDX BEFOREHAND IN JitEmitCode |
                //   \\______//    |  See UnspillIReg                    |
                //   | _____ |     | ____________________________________/
                //  /  \___/  \  <==/
                // /___________\
                //
                //  Snailnelious
                div:
                case OP_DIV:
                __Div(jit,cur,"IDIV",cur->operands[0],cur->operands[1]);
                dummy2.type=AV_IREG;
                dummy2.reg=Str2Reg("RAX");
                IMov(jit,cur,&cur->my_avalue,&dummy2,unsigned);
                break;
                case OP_ASSIGN_DIV:
                __Div(jit,cur,"IDIV",cur->operands[0],cur->operands[1]);
                dummy2.type=AV_IREG;
                dummy2.reg=Str2Reg("RAX");
                IMov(jit,cur,cur->operands[0],&dummy2,unsigned);
                break;
                case OP_ASSIGN_MOD:
                __Div(jit,cur,"IDIV",cur->operands[0],cur->operands[1]);
                dummy2.type=AV_IREG;
                dummy2.reg=Str2Reg("RDX");
                IMov(jit,cur,cur->operands[0],&dummy2,unsigned);
                break;
                case OP_MOD:
                __Div(jit,cur,"IDIV",cur->operands[0],cur->operands[1]);
                dummy2.type=AV_IREG;
                dummy2.reg=Str2Reg("RDX");
                IMov(jit,cur,&cur->my_avalue,&dummy2,unsigned);
                break;
                case OP_ASSIGN_MUL:
                __Div(jit,cur,"IMUL",cur->operands[0],cur->operands[1]);
                dummy2.type=AV_IREG;
                dummy2.reg=Str2Reg("RAX");
                IMov(jit,cur,cur->operands[0],&dummy2,unsigned);
                break;
                case OP_MUL:
                __Div(jit,cur,"IMUL",cur->operands[0],cur->operands[1]);
                dummy2.type=AV_IREG;
                dummy2.reg=Str2Reg("RAX");
                end:
                IMov(jit,cur,&cur->my_avalue,&dummy2,unsigned);
                break;
                case OP_SHL:
                ShiftOp(jit,cur,"SHL",&cur->my_avalue,cur->operands[0],cur->operands[1],unsigned);
                break;
                case OP_SHR:
                if(unsigned)
                    ShiftOp(jit,cur,"SHR",&cur->my_avalue,cur->operands[0],cur->operands[1],unsigned);
                else
                    ShiftOp(jit,cur,"SAR",&cur->my_avalue,cur->operands[0],cur->operands[1],unsigned);
                break;
                case OP_AND:
                IArithOp(jit,cur,"AND",&cur->my_avalue,cur->operands[0],cur->operands[1],unsigned);
                break;
                case OP_OR:
                IArithOp(jit,cur,"OR",&cur->my_avalue,cur->operands[0],cur->operands[1],unsigned);
                break;
                case OP_XOR:
                IArithOp(jit,cur,"XOR",&cur->my_avalue,cur->operands[0],cur->operands[1],unsigned);
                break;
                case OP_POS:
                pos:
                IMov(jit,cur,&cur->my_avalue,cur->operands[0],unsigned);
                break;
                case OP_NEG:
                IArithUnop(jit,cur,"NEG",&cur->my_avalue,cur->operands[0],unsigned);
                break;
        
                case OP_GT:
                if(!unsigned)
                    ICmpOp(jit,cur,"SETG",&cur->my_avalue,cur->operands[0],cur->operands[1],unsigned);
                else
                    ICmpOp(jit,cur,"SETA",&cur->my_avalue,cur->operands[0],cur->operands[1],unsigned);
                break;
                case OP_LT:
                if(!unsigned)
                    ICmpOp(jit,cur,"SETL",&cur->my_avalue,cur->operands[0],cur->operands[1],unsigned);
                else
                    ICmpOp(jit,cur,"SETB",&cur->my_avalue,cur->operands[0],cur->operands[1],unsigned);
                break;
                case OP_GE:
                if(!unsigned)
                    ICmpOp(jit,cur,"SETGE",&cur->my_avalue,cur->operands[0],cur->operands[1],unsigned);
                else
                    ICmpOp(jit,cur,"SETAE",&cur->my_avalue,cur->operands[0],cur->operands[1],unsigned);
                break;
                case OP_LE:
                if(!unsigned)
                    ICmpOp(jit,cur,"SETLE",&cur->my_avalue,cur->operands[0],cur->operands[1],unsigned);
                else
                    ICmpOp(jit,cur,"SETBE",&cur->my_avalue,cur->operands[0],cur->operands[1],unsigned);
                break;
                start:
				Push(jit,cur,"PUSH",cur->operands[0]);
				case OP_POST_INC:
                    if(cur->operands[1](I64)==1)
                        IIncOp(jit, cur,"INC",cur->operands[0],unsigned);
                    else {
                        dummy2.type=AV_INT;
                        dummy2.int=cur->operands[1](I64);
                        IArithOp(jit,cur,"ADD",cur->operands[0],cur->operands[0],&dummy2,unsigned);
                    }
                    break;
                case OP_POST_DEC:
                    if(cur->operands[1](I64)==1)
                        IIncOp(jit,cur,"DEC",cur->operands[0],unsigned);
                    else {
                        dummy2.type=AV_INT;
                        dummy2.int=cur->operands[1](I64);
                        IArithOp(jit,cur,"SUB",cur->operands[0],cur->operands[0],&dummy2,unsigned);
                    }
                end:
                Pop(jit,cur,"POP",&cur->my_avalue);
                break;
                case OP_PRE_INC:
                if(cur->operands[1](I64)==1)
                    IIncOp(jit,cur,"INC",cur->operands[0],unsigned);
                else {
                    dummy.type=AV_INT;
                    dummy.int=cur->operands[1](I64);
                    IArithOp(jit,cur,"ADD",cur->operands[0],cur->operands[0],&dummy,unsigned);
                }
                IMov(jit,cur,&cur->my_avalue,cur->operands[0],unsigned);
                break;
                case OP_PRE_DEC:
                if(cur->operands[1](I64)==1)
                    IIncOp(jit,cur,"DEC",cur->operands[0],unsigned);
                else {
                    dummy.type=AV_INT;
                    dummy.int=cur->operands[1](I64);
                    IArithOp(jit,cur,"SUB",cur->operands[0],cur->operands[0],&dummy,unsigned);
                }
                IMov(jit,cur,&cur->my_avalue,cur->operands[0],unsigned);
                break;
                case OP_ASSIGN:
                if(cur->flags&RPN_F_BIG_MOVE) {
                    BigMove(jit,cur,cur->operands[0],cur->operands[1]);
                    if(cur->my_avalue.type!=AV_UNOCC)
                        BigMove(jit,cur,&cur->my_avalue,cur->operands[0]);
                    break;
                }
                IMov(jit,cur,cur->operands[0],cur->operands[1],unsigned);
                if(cur->my_avalue.type!=AV_UNOCC)
                    IMov(jit,cur,&cur->my_avalue,cur->operands[0],unsigned);
                case OP_DUMP_PTR:
                break;
                default:
                "UNIMPI:%d\n",cur->type.code;
                break;
            }
        } else if(cur->type.is_flt) {
            switch(cur->type.code) {
                case OP_DEREF:
                //See frypot
                //TODO non fpu pass if not directed to a x87 reg
                if(cur->operands[0]->type==AV_VIREG) {
                    dummy.type=AV_FVINDIR;
                    dummy.vreg=cur->operands[0].vreg;
                    dummy.offset=cur->operands[1](I64);
                    __X87Push(jit,cur,&dummy);
                    __FMov(jit,cur,&cur->my_avalue,TRUE);
                } else if(cur->operands[0]->type==AV_IREG) {
                    dummy.type=AV_FINDIR;
                    dummy.reg=cur->operands[0]->reg;
                    dummy.offset=cur->operands[1](I64);
                    __X87Push(jit,cur,&dummy);
                    __FMov(jit,cur,&cur->my_avalue,TRUE);
                } else {
                    dummy.type=AV_IREG;
                    dummy.reg=Str2Reg("RAX");
                    IMov(jit,cur,&dummy,cur->operands[0],!cur->type.is_signed);
                    dummy.offset=cur->operands[1](I64);
                    dummy.type=AV_FINDIR;
                    __X87Push(jit,cur,&dummy);
                    __FMov(jit,cur,&cur->my_avalue,TRUE);
                }
                break;    
                case OP_INPLACE_PUSH:
                Push(jit,cur,"PUSH",&cur->my_avalue);
                break;
                case OP_REF_DATA:
                //TODO test
                mode=X64ModeReg(Str2Reg("RAX"));
                mode2=X64ModeMem(NULL,0,NULL,Str2Reg("RIP"),0x11223344);
                end_ptr=EncodeOpcode(buffer,"LEA",NULL,&cur->patch_addr,NULL,NULL,mode,mode2);
                X64ModeFree(mode),X64ModeFree(mode2);
                cur->patch_addr+=jit->buf_size-4; //-4 ignores sib size
                JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
                IMov(jit,cur,&cur->my_avalue,&dummy,!cur->type.is_signed);
                break;
                case OP_LITERAL:
                case OP_VARIABLE:
                break;
                start:
                case OP_COMMA:
                if(cur->operands[1]->type!=AV_FREG)
                    __X87Push(jit,cur,cur->operands[1]);
                __FMov(jit,cur,&cur->my_avalue,TRUE);
                if(cur->operands[0]->type==AV_FREG)
                    //FSTP ST0
                    JIT_AppendAsmText(jit,"\xDD\xD8",2);
                break;
                case OP_ASSIGN:
                if(cur->my_avalue.type!=AV_UNOCC) {
                    // Optimization in AssignX87Regs that will pop if operand is register
                    if(cur->operands[1]->type==AV_FREG) {
                        __FMov(jit,cur,cur->operands[0],FALSE);
                    } else {
                        __X87Push(jit,cur,cur->operands[1]);
                        __FMov(jit,cur,cur->operands[0],FALSE);
                    }
                    if(cur->my_avalue.type!=AV_FREG);
                        __FMov(jit,cur,&cur->my_avalue,TRUE);
                } else {
                    // Optimization in AssignX87Regs that will pop if operand is register
                    if(cur->operands[1]->type==AV_FREG) {
                        __FMov(jit,cur,cur->operands[0],TRUE);
                    } else {
                        __X87Push(jit,cur,cur->operands[1]);
                        __FMov(jit,cur,cur->operands[0],TRUE);
                    }
                }
                break;
                case OP_RETURN:
                dummy.type=AV_IREG;
                dummy.reg=Str2Reg("RAX");
                if(cur->operands[0]->type!=AV_FREG)
                    __X87Push(jit,cur,cur->operands[0]);
                __FMov(jit,cur,&dummy,TRUE);
                mode=X64ModeLabel(NULL);
                end_ptr=EncodeOpcode(buffer,"JMP",NULL,&cur->patch_addr,NULL,NULL,mode);
                X64ModeFree(mode);
                JIT_AppendAsmText(jit,buffer,end_ptr-buffer);
                cur->patch_addr=jit->buf_size;
                JIT_AppendAsmText(jit,"\x00\x00\x00\x00",4);
                break;
                case OP_BODY:
                break;
                break;
                
                case OP_GETARG:
                dummy.type=AV_FFRAME;
                dummy.reg=Str2Reg("RBP");
                dummy.size=8;
                //
                // Stack
                // 
                // ARGN
                // ...
                // ARG0
                // RET_PTR
                // OLD_RBP<--RBP
                dummy.f_offset=2*8+8*cur->operands[0]->int;
                __X87Push(jit,cur,&dummy);
                __FMov(jit,cur,&cur->my_avalue,TRUE);
                break;
                
                case OP_ADD:
                FPUOp(jit,cur,"FADDP",cur->operands[0],cur->operands[1]);
                __FMov(jit,cur,&cur->my_avalue,TRUE);
                break;
                case OP_SUB:
                FPUOp(jit,cur,"FSUBP",cur->operands[0],cur->operands[1]);
                __FMov(jit,cur,&cur->my_avalue,TRUE);
                break;
                case OP_MUL:
                FPUOp(jit,cur,"FMULP",cur->operands[0],cur->operands[1]);
                __FMov(jit,cur,&cur->my_avalue,TRUE);
                break;
                case OP_DIV:
                FPUOp(jit,cur,"FDIVP",cur->operands[0],cur->operands[1]);
                __FMov(jit,cur,&cur->my_avalue,TRUE);
                break;
                case OP_MOD:
                //Doesnt pop
                FPRemOp(jit,cur,cur->operands[0],cur->operands[1]);
                __FMov(jit,cur,&cur->my_avalue,TRUE);
                break;
                
                case OP_SHL:
                ShiftOp(jit,cur,"SHL",&cur->my_avalue,cur->operands[0],cur->operands[1]);
                break;
                case OP_SHR:
                ShiftOp(jit,cur,"SHR",&cur->my_avalue,cur->operands[0],cur->operands[1]);
                break;
                case OP_AND:
                FBinaryOp(jit,cur,"AND",cur->operands[0],cur->operands[1]);
                __FMov(jit,cur,&cur->my_avalue,TRUE);
                break;
                case OP_OR:
                FBinaryOp(jit,cur,"OR",cur->operands[0],cur->operands[1]);
                __FMov(jit,cur,&cur->my_avalue,TRUE);
                break;
                case OP_XOR:
                FBinaryOp(jit,cur,"XOR",cur->operands[0],cur->operands[1]);
                __FMov(jit,cur,&cur->my_avalue,TRUE);
                break;
                start:
                    case OP_ASSIGN_ADD:
                    FPUOp(jit,cur,"FADDP",cur->operands[0],cur->operands[1]);
                    break;
                    case OP_ASSIGN_SUB:
                    FPUOp(jit,cur,"FSUBP",cur->operands[0],cur->operands[1]);
                    break;
                    case OP_ASSIGN_MUL:
                    FPUOp(jit,cur,"FMULP",cur->operands[0],cur->operands[1]);
                    break;
                    case OP_ASSIGN_DIV:
                    FPUOp(jit,cur,"FDIVP",cur->operands[0],cur->operands[1]);
                    break;
                    case OP_ASSIGN_MOD:
                    FPUOp(jit,cur,"FPREM;\nFSTP ST1",cur->operands[0],cur->operands[1]);
                    break;
                    case OP_ASSIGN_SHL:
                    ShiftOp(jit,cur,"SHL",&cur->operands[0],cur->operands[0],cur->operands[1]);
                    break;
                    case OP_ASSIGN_SHR:
                    ShiftOp(jit,cur,"SAR",&cur->operands[0],cur->operands[0],cur->operands[1]);
                    break;
                    case OP_ASSIGN_AND:
                    FBinaryOp(jit,cur,"ADD",cur->operands[0],cur->operands[1]);
                    break;
                    case OP_ASSIGN_XOR:
                    FBinaryOp(jit,cur,"XOR",cur->operands[0],cur->operands[1]);
                    break;
                    case OP_ASSIGN_OR:
                    FBinaryOp(jit,cur,"OR",cur->operands[0],cur->operands[1]);
                    break;
                end:
                if(cur->my_avalue.type==AV_UNOCC) {
                    if(cur->operands[0]->type!=AV_FREG)
                        __FMov(jit,cur,cur->operands[0],TRUE);
                    else
                        //FSTP ST0
                        JIT_AppendAsmText(jit,"\xDD\xD8",2);
                } else {
                    if(cur->operands[0]->type!=AV_FREG)
                        __FMov(jit,cur,cur->operands[0],FALSE);
                    if(cur->my_avalue.type!=AV_FREG);
                        __FMov(jit,cur,&cur->my_avalue,TRUE);
                }
                break;
                case OP_TYPECAST_BIT4BIT:
                if(!cur->next->type.is_flt) {
                    Push(jit,cur,"PUSH",cur->operands[0]);
                    if(cur->my_avalue.type!=AV_PUSH)
                        Pop(jit,cur,"POP",&cur->my_avalue);
                } else {
                    __X87Push(jit,cur,cur->operands[0]);
                    __FMov(jit,cur,&cur->my_avalue,TRUE);
                }
                break;
                
                case OP_POS:
                //See AssignX87Regs as it opitmizes out a push here for unops
                if(cur->operands[0]->type!=AV_FREG) 
                    __X87Push(jit,cur,cur->operands[0]);
                __FMov(jit,cur,&cur->my_avalue,TRUE);
                break;
                case OP_NOT:
                if(cur->operands[0]->type!=AV_FREG) 
                    __X87Push(jit,cur,cur->operands[0]);
                dummy.type=AV_FREG;
                Push(jit,cur,"PUSH",&dummy);
                //NOT I64 [RSP]
                JIT_AppendAsmText(jit,"\x48\xF7\x14\x24",4);
                Pop(jit,cur,"POP",&cur->my_avalue);
                break;

                case OP_NEG:
                //See above
                if(cur->operands[0]->type!=AV_FREG) 
                    __X87Push(jit,cur,cur->operands[0]);
                //FCHS
                JIT_AppendAsmText(jit,"\xD9\xE0",2);
                __FMov(jit,cur,&cur->my_avalue,TRUE);
                break;
                start:
                    if(cur->operands[0]->type==AV_FREG&&cur->operands[1]->type==AV_FREG) {
                        //FXCH ST1
                        JIT_AppendAsmText(jit,"\xD9\xC9",2);
                    } else if(cur->operands[0]->type==AV_FREG) {
                        __X87Push(jit,cur,cur->operands[1]);
                        //FXCH ST1
                        JIT_AppendAsmText(jit,"\xD9\xC9",2);
                    } else if(cur->operands[1]->type==AV_FREG) {
                        __X87Push(jit,cur,cur->operands[0]);        
                    } else {
                        __X87Push(jit,cur,cur->operands[1]);
                        __X87Push(jit,cur,cur->operands[0]);
                    }
                    //FCOMIP ST,ST1;
                    JIT_AppendAsmText(jit,"\xDF\xF1",2);
                    //FSTP ST0
                    JIT_AppendAsmText(jit,"\xDD\xD8",2);
                    case OP_GT:
                    __SetOnCondOp(jit,cur,"SETA",&cur->my_avalue);
                    break;
                    case OP_LT:
                    __SetOnCondOp(jit,cur,"SETB",&cur->my_avalue);
                    break;
                    case OP_GE:
                    __SetOnCondOp(jit,cur,"SETAE",&cur->my_avalue);
                    break;
                    case OP_LE:
                    __SetOnCondOp(jit,cur,"SETBE",&cur->my_avalue);
                    break;
                    case OP_NE:
                    __SetOnCondOp(jit,cur,"SETNE",&cur->my_avalue);
                    break;
                    case OP_EQ:
                    __SetOnCondOp(jit,cur,"SETE",&cur->my_avalue);
                    break;
                end:
                break;
                start:
                Push(jit,cur,"PUSH",cur->operands[0]);
                __X87Push(jit,cur,cur->operands[0]);
					case OP_POST_INC:
                    //FLD1
                    //FADDP ST1,ST0
                    JIT_AppendAsmText(jit,"\xD9\xE8\xDE\xC1",4);
                    __FMov(jit,cur,cur->operands[0],TRUE);
                    break;	
                    case OP_POST_DEC:
                    //FLD1
                    //FSUBP ST1,ST0
                    JIT_AppendAsmText(jit,"\xD9\xE8\xDE\xE9",4);
                    __FMov(jit,cur,cur->operands[0],TRUE);
                    break;
                end:
                Pop(jit,cur,"POP",&cur->my_avalue);
                break;
                case OP_PRE_INC:
                __X87Push(jit,cur,cur->operands[0]);
                //FLD1
                //FADDP ST1,ST0
                JIT_AppendAsmText(jit,"\xD9\xE8\xDE\xC1",4);                    
                __FMov(jit,cur,cur->operands[0],FALSE);
                __FMov(jit,cur,&cur->my_avalue,TRUE);
                break;
                case OP_PRE_DEC:
                __X87Push(jit,cur,cur->operands[0]);
                //FLD1
                //FSUBP ST1,ST0
                JIT_AppendAsmText(jit,"\xD9\xE8\xDE\xE9",4);
                __FMov(jit,cur,cur->operands[0],FALSE);
                __FMov(jit,cur,&cur->my_avalue,TRUE);
                end:
                break;
                case OP_CALL:
                FuncCall(jit,cur,&GetCallFunction(cur)->my_avalue,&cur->my_avalue,cur->type.body_length,NULL);
                break;
                default:
                "UNIMPF:%d\n",cur->type.code;
                break;
            }
        }
        iter++;
        next:;
    }
    return iter;
}
//
// SPECIAL CASE!!!
// If our previous op is a OP_IF,then discard the result for using special Jcc style jumps
//      |\    ______    /|
//      | \  /*   * \  / |
//      |  \|        |/  |
//      |  /|  ====  |\  |  
//      | / |   WW   | \ |
//      |/   \______/   \|
//
//              Potato Drop
//
U0 NullifyUselessDests(JIT *jit,CVector *flat) {
    I64 idx;
    CRPN *rpn;
    for(idx=0;idx!=flat->size;idx++) {
        rpn=flat->data[idx];
        if(!(rpn->flags&RPN_F_MERGED_ASSIGN)) {
            switch(rpn->type.code) {
                case OP_LT:
                case OP_GT:
                case OP_LE:
                case OP_GE:
                case OP_NE:
                case OP_EQ:
                if(rpn->prev&&rpn->prev->type.code==OP_IF)
                    rpn->my_avalue.type=AV_UNOCC;
            }
            if(IsRPNUnop(rpn)||IsRPNBinop(rpn)) {
                if(!rpn->consumer)
                    rpn->my_avalue.type=AV_UNOCC;
            }
        }
    }
}
U0 JIT_PropigateAssigns(JIT *jit,CRPN *rpn=NULL,CRPN *stop_at=NULL) {
    /*
    CRPN *from,*next2;
    if(!rpn)
        rpn=FirstRPN(jit->cur_rpn_start);
    loop:
    for(;rpn!=stop_at;rpn=rpn->next) {
        if(rpn->type.code==OP_CALL) {
            //Dont propigate assigns in calls as we we may inplace push their values
            for(next2=ComputeRPNSuccessor(rpn),rpn=rpn->next;rpn!=next2;) {
                if(IsRPNAssign(rpn)) {
                    //Skip past the assign and do the assign's operands
                    JIT_PropigateAssigns(jit,rpn->next,ComputeRPNSuccessor(rpn));
                } else 
                    JIT_PropigateAssigns(jit,rpn,ComputeRPNSuccessor(rpn));
                rpn=ComputeRPNSuccessor(rpn);
            }
            goto loop;
        } else if(rpn->type.code==OP_FUNC_CALL) {
        
        } else if(rpn->type.code==OP_ASSIGN) {
            if(rpn->next->type.code==OP_VARIABLE) {
                from=ComputeRPNSuccessor(rpn->next);
                if(rpn->type.is_flt!=from->type.is_flt) goto next;
                switch(from->type.code) {
                case OP_VARIABLE:
                case OP_LITERAL:
                case OP_RELOCATION:
                break;
                default:
                if(!(from->flags&(RPN_F_DONT_TOUCH|RPN_F_IMPLICIT_OPERATION))) {
                    from->flags|=RPN_F_MERGED_ASSIGN|RPN_F_DONT_TOUCH;
                    from->my_avalue=rpn->next->my_avalue;
                    next2=rpn->next->next;
                    RPNFree(rpn->next),RPNFree(rpn);
                    rpn=next2;
                    goto loop;
                }
                }
            }
        }
        next:;
    }*/
}
U0 AssignX87Regs(JIT *jit,CVector *flat) {
    I64 idx,stk_regs=0,idx2;
    CRPN *rpn,*dst;
    CAsmValue *value;
    for(idx=0;idx!=flat->size;idx++) {
        rpn=flat->data[idx];
        rpn->my_avalue.fpu_depth=jit->fpu_stk->size;
        if(rpn->type.code==OP_CALL) {
            while(jit->fpu_stk->size) {
                value=VectorPop(jit->fpu_stk);
                value->type=AV_FREG_SHADOW;
                value->size=8;
            }
            stk_regs=jit->fpu_stk_size=0;
        } else if(IsRPNUnop(rpn)||rpn->type.code==OP_RETURN||rpn->type.code==OP_IF||rpn->type.code==OP_JMP_TABLE) {
            if(rpn->operands[0]->type==AV_VFREG) {
                VectorPop(jit->fpu_stk),stk_regs--;
            }
        } else if(IsRPNAssign(rpn)&&rpn->operands[1]->type==AV_VFREG) {
            VectorPop(jit->fpu_stk),stk_regs--;
        } else if(IsRPNBinop(rpn)) {
            if(rpn->operands[0]->type==AV_VFREG)
                VectorPop(jit->fpu_stk),stk_regs--;
            if(rpn->operands[1]->type==AV_VFREG)
                VectorPop(jit->fpu_stk),stk_regs--;
        }
        //Check if result is "consumed" by successor
        if(!rpn->consumer) {
            rpn->my_avalue.type=AV_UNOCC;
            goto next;
        }
        if(rpn->my_avalue.type==AV_VFREG) {
            jit->vfreg_cnt=MaxI64((rpn->my_avalue.vreg=stk_regs)+1,jit->vfreg_cnt);
            VectorPush(jit->fpu_stk,&rpn->my_avalue);
            if(++stk_regs>=6) {
                for(idx2=0;idx2!=jit->fpu_stk->size;idx2++) {
                    value=jit->fpu_stk->data[idx2];
                    if(value->type==AV_VFREG) {
                        value->type=AV_FREG_SHADOW;
                        value->size=8;
                        stk_regs--;
                        break;
                    }
                }
            }
        }
        next:
    }
    jit->fpu_stk_offset=0;
    for(idx=0;idx!=flat->size;idx++) {
        rpn=flat->data[idx];
        if(rpn->my_avalue.type==AV_VFREG)
            rpn->my_avalue.type=AV_FREG;
    }
}
Bool IsConst(CRPN *const) {
    return const->type.code==OP_LITERAL&&(const->my_avalue.type==AV_INT||const->my_avalue.type==AV_FLT);
}
F64 ConstF64(CRPN *const) {
    if(const->type.is_flt) return const->my_avalue.flt;
    return const->my_avalue.int;
}
I64 ConstI64(CRPN *const) {
    if(const->type.is_flt) return const->my_avalue.flt;
    return const->my_avalue.int;
}
//
// Run this after the RPN is finalized,dont run if you intend on modifing the RPN
//
// 
U0 CacheRPNSuccessors(JIT *jit) {
    CRPN *r=FirstRPN(jit->cur_rpn_start);
    for(;r->next;r=r->next);
    for(;r;r=r->prev)
        r->__end=ComputeRPNSuccessor(r);
}
I64 ShiftifyAmt(I64 v) {
    if(BCnt(v)==1)
        return Bsf(v);
    return -1;
}
CRPN *FoldConstants(JIT *jit) {
    I64 idx,mul;
    CRPN *next,*rpn,*const1,*const2,*first=FirstRPN(jit->cur_rpn_start),*new;
    loop1:
    //Phase 1,Constant folding
    for(rpn=first;rpn->next;rpn=rpn->next);
    for(;rpn;rpn=rpn->prev) {
        rpn->__end=NULL;
        if(IsRPNUnop(rpn)&&IsConst(const1=rpn->next)) {
            switch(rpn->type.code) {
                start:
                rpn->type.code=OP_LITERAL;
                case OP_POS:
                if(rpn->type.is_flt) {
                    rpn->my_avalue.type=AV_FLT;
                    rpn->my_avalue.flt=ConstF64(const1);
                } else {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstI64(const1);
                }
                break;
                //case OP_NOT:
                //if(rpn->type.is_flt) {
                //    rpn->my_avalue.type=AV_FLT;
                //    rpn->my_avalue.flt=~ConstF64(const1);
                //} else {
                //    rpn->my_avalue.type=AV_INT;
                //    rpn->my_avalue.int=~ConstI64(const1);
                //}
                break;
                case OP_NEG:
                if(rpn->type.is_flt) {
                    rpn->my_avalue.type=AV_FLT;
                    rpn->my_avalue.flt=-ConstF64(const1);
                } else {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=-ConstI64(const1);
                }
                break;
                end:
                RPNFree(const1);
            }
            goto next1;
        }
        if(!IsRPNBinop(rpn)) goto next1;
        if(IsConst(const1=rpn->next)&&IsConst(const2=ComputeRPNSuccessor(rpn->next))) {
            switch(rpn->type.code) {
                start:
                rpn->type.code=OP_LITERAL;
                case OP_ADD:
                if(rpn->type.is_flt) {
                    rpn->my_avalue.type=AV_FLT;
                    rpn->my_avalue.flt=ConstF64(const1)+ConstF64(const2);
                } else {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstI64(const1)+ConstI64(const2);
                }
                break;
                case OP_SUB:
                if(rpn->type.is_flt) {
                    rpn->my_avalue.type=AV_FLT;
                    rpn->my_avalue.flt=ConstF64(const1)-ConstF64(const2);
                } else {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstI64(const1)-ConstI64(const2);
                }
                break;
                case OP_MUL:
                if(rpn->type.is_flt) {
                    rpn->my_avalue.type=AV_FLT;
                    rpn->my_avalue.flt=ConstF64(const1)*ConstF64(const2);
                } else {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstI64(const1)*ConstI64(const2);
                }
                break;
                case OP_DIV:
                if(rpn->type.is_flt) {
                    rpn->my_avalue.type=AV_FLT;
                    rpn->my_avalue.flt=ConstF64(const1)/ConstF64(const2);
                } else {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstI64(const1)/ConstI64(const2);
                }
                break;
                case OP_MOD:
                if(rpn->type.is_flt) {
                    rpn->my_avalue.type=AV_FLT;
                    rpn->my_avalue.flt=ConstF64(const1)%ConstF64(const2);
                } else {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstI64(const1)%ConstI64(const2);
                }
                break;
                
                case OP_SHL:
                if(rpn->type.is_flt) {
                    rpn->my_avalue.type=AV_FLT;
                    rpn->my_avalue.flt=ConstF64(const1)<<ConstF64(const2);
                } else {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstI64(const1)<<ConstI64(const2);
                }
                break;
                case OP_SHR:
                if(rpn->type.is_flt) {
                    rpn->my_avalue.type=AV_FLT;
                    rpn->my_avalue.flt=ConstF64(const1)>>ConstF64(const2);
                } else if(rpn->type.is_signed) {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstI64(const1)>>ConstI64(const2);
                } else {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstI64(const1)(U64)>>(ConstI64(const2)(U64));
                }
                break;
                
                case OP_AND:
                if(rpn->type.is_flt) {
                    rpn->my_avalue.type=AV_FLT;
                    rpn->my_avalue.flt=ConstF64(const1)&ConstF64(const2);
                } else {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstI64(const1)&ConstI64(const2);
                }
                break;
                case OP_OR:
                if(rpn->type.is_flt) {
                    rpn->my_avalue.type=AV_FLT;
                    rpn->my_avalue.flt=ConstF64(const1)|ConstF64(const2);
                } else {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstI64(const1)|ConstI64(const2);
                }
                break;
                case OP_XOR:
                if(rpn->type.is_flt) {
                    rpn->my_avalue.type=AV_FLT;
                    rpn->my_avalue.flt=ConstF64(const1)^ConstF64(const2);
                } else {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstI64(const1)^ConstI64(const2);
                }
                break;
                    
                    
                case OP_GT:
                if(rpn->type.is_flt) {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstF64(const1)>ConstF64(const2);
                } else {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstI64(const1)>ConstI64(const2);
                }
                break;
                case OP_LT:
                if(rpn->type.is_flt) {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstF64(const1)<ConstF64(const2);
                } else {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstI64(const1)<ConstI64(const2);
                }
                break;
                case OP_GE:
                if(rpn->type.is_flt) {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstF64(const1)>=ConstF64(const2);
                } else {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstI64(const1)>=ConstI64(const2);
                }
                break;
                case OP_LE:
                if(rpn->type.is_flt) {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstF64(const1)<=ConstF64(const2);
                } else {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstI64(const1)<=ConstI64(const2);
                }
                break;
                case OP_EQ:
                if(rpn->type.is_flt) {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstF64(const1)==ConstF64(const2);
                } else {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstI64(const1)==ConstI64(const2);
                }
                break;
                case OP_NE:
                if(rpn->type.is_flt) {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstF64(const1)!=ConstF64(const2);
                } else {
                    rpn->my_avalue.type=AV_INT;
                    rpn->my_avalue.int=ConstI64(const1)!=ConstI64(const2);
                }
                break;
                end:
                RPNFree(const2);
                RPNFree(const1);
                break;
            }
        }
        next1:
    }
    //Phase 2,communitive values
    rpn=jit->cur_rpn_start;
    loop2:
    for(rpn=first;rpn;rpn=rpn->next) {
        rpn->__end=NULL;
        if(!IsRPNBinop(rpn)) goto next2;
        const1=ComputeRPNSuccessor(rpn->next);
        if(IsConst(const1)) {
            switch(rpn->type.code) {
                case OP_ADD:
                if(rpn->next->type.code==OP_ADD) {
                    const2=ComputeRPNSuccessor(rpn->next->next);;
                    if(IsConst(const2)) {
                        if(rpn->next->type.is_flt) {
                            const2->my_avalue.flt=ConstF64(const2)+ConstF64(const1);
                        } else
                            const2->my_avalue.int=ConstI64(const2)+ConstI64(const1);
                        next=rpn->next;
                        RPNFree(const1),RPNFree(rpn);
                        rpn=next;
                        goto loop2;
                    }
                }
                break;
                case OP_SUB:
                if(rpn->next->type.code==OP_SUB) {
                    const2=ComputeRPNSuccessor(rpn->next->next);;
                    if(IsConst(const2)) {
                        if(rpn->next->type.is_flt) {
                            const2->my_avalue.flt=ConstF64(const1)+ConstF64(const2);
                        } else
                            const2->my_avalue.int=ConstI64(const1)+ConstI64(const2);
                        next=rpn->next;
                        RPNFree(const1),RPNFree(rpn);
                        rpn=next;
                        goto loop2;
                    }
                }
                break;
                case OP_MUL:
                if(rpn->next->type.code==OP_MUL) {
                    const2=ComputeRPNSuccessor(rpn->next->next);;
                    if(IsConst(const2)) {
                        if(rpn->next->type.is_flt) {
                            const2->my_avalue.flt=ConstF64(const2)*ConstF64(const1);
                        } else
                            const2->my_avalue.int=ConstI64(const2)*ConstI64(const1);
                        next=rpn->next;
                        RPNFree(const1),RPNFree(rpn);
                        rpn=next;
                        goto loop2;
                    }
                }
                break;
            }
        }
        next2:
    }
    //Phase 3 OP_MUL/OP_DIV shiftifys/remove 1's
    for(rpn=first;rpn;){
        loop3:
        if((rpn->type.code==OP_MUL)&&!rpn->type.is_flt) {
            if(IsConst(const1=rpn->next)) {
                if(ConstI64(const1)==1) {
                    next=ComputeRPNSuccessor(const1);
                    RPNFree(const1),RPNFree(rpn);
                    rpn=next;
                    goto loop3;
                } else if(-1!=(mul=ShiftifyAmt(ConstI64(const1)))) {
                    next=ComputeRPNSuccessor(ComputeRPNSuccessor(const1));
                    if(next) {
                        RPNFree(const1);
                        new=MAlloc(sizeof(CRPN));
                        new->type.code=OP_LITERAL;
                        new->my_avalue.type=AV_INT;
                        new->my_avalue.int=mul;
                        new->next=next;
                        new->prev=next->prev;
                        new->next->prev=new;
                        new->prev->next=new;
                        new->successors=VectorNew;
                        new->r_map=VectorNew,new->in_r_map=VectorNew;
                        new->spills=VectorNew;
                        new->unspills=VectorNew;
                        rpn->type.code=OP_SHL;
                    }
                }
            } else if(IsConst(const2=ComputeRPNSuccessor(rpn->next))) {
                if(ConstI64(const2)==1) {
                    next=rpn->next;
                    RPNFree(const2),RPNFree(rpn);
                    rpn=next;
                    goto loop3; 
                } else if(-1!=ShiftifyAmt(ConstI64(const2))) {
                    const2->my_avalue.int=ShiftifyAmt(ConstI64(const2));
                    const2->my_avalue.type=AV_INT;
                    rpn->type.code=OP_SHL;
                }
            }
        } else if((rpn->type.code==OP_DIV||rpn->type.code==OP_ASSIGN_DIV)&&!rpn->type.is_flt) {
            if(IsConst(const2=ComputeRPNSuccessor(rpn->next))) {
                if(ConstI64(const2)==1) {
                    next=rpn->next;
                    RPNFree(const2),RPNFree(rpn);
                    rpn=next;
                    goto loop3; 
                } else if(-1!=ShiftifyAmt(ConstI64(const2))) {
                    const2->my_avalue.int=ShiftifyAmt(ConstI64(const2));
                    const2->my_avalue.type=AV_INT;
                    if(rpn->type.code==OP_DIV)
                        rpn->type.code=OP_SHR;
                    else
                        rpn->type.code=OP_ASSIGN_SHR;
                }
            }
        }
        rpn=rpn->next;
    }
    //Phase 4 OP_DEREF offsets
    for(rpn=first;rpn;){
        loop4:
        if(rpn->type.code==OP_DEREF) {
            if(rpn->next->type.code==OP_ADD) {
                mul=1;
                deref_merge:
                if(IsConst(const1=rpn->next->next)) {
                    rpn->operands[1](I64)+=mul*ConstI64(const1);
                    RPNFree(const1),RPNFree(rpn->next);
                    goto loop4;
                } else if(IsConst(const2=ComputeRPNSuccessor(rpn->next->next))) {
                    rpn->operands[1](I64)+=mul*ConstI64(const2);
                    RPNFree(const2),RPNFree(rpn->next);
                    goto loop4;
                }
            } else if(rpn->next->type.code==OP_SUB) {
                mul=-1;
                goto deref_merge;
            }
        }
        rpn=rpn->next;
    }
    for(rpn=first;rpn->next;rpn=rpn->next);
    jit->cur_rpn_start=rpn;
}
U0 TellArgumentsToBePushed(JIT *jit) {
    I64 idx,idx2;
    CRPN *rpn,*arg;
    for(rpn=FirstRPN(jit->cur_rpn_start);rpn;rpn=rpn->next) {
        if(rpn->type.code==OP_CALL) {
            arg=rpn->next;
            for(idx2=0;idx2!=rpn->type.body_length;idx2++) {
                switch(arg->type.code) {
                    case OP_VARIABLE:
                    case OP_LITERAL:
                    arg->type.code=OP_INPLACE_PUSH;
                    break;
                    default:
                    arg->my_avalue.type=AV_PUSH;
                }
                arg=ComputeRPNSuccessor(arg);
            }
        }
    }
}
//Pass indirects to me
U0 BigMove(JIT *jit,CRPN *rpn,CAsmValue *dst,CAsmValue *src) {
	VectorAdd(jit->tainted_caller_saved,Str2Reg("RDI")->code);
    VectorAdd(jit->tainted_caller_saved,Str2Reg("RSI")->code);
    I64 len=MinI64(dst->size,src->size),r;
    CReg *rcx;
    Bool pop=FALSE;
    CAsmValue dummy,dummy2,dummy3;
    dummy.type=AV_IREG;
    //RDI/RSI were spilled before this operation,so they are not alive here so we are free to poo poo them
    switch(dst->type) {
        start:
            dummy2.type=AV_IREG;
            case AV_IVINDIR:
            case AV_FVINDIR:
            dummy2.reg=IRegCodeToReg(r=RMapGetPhysicalReg(rpn->r_map,dst->vreg));
            if(r==-1) {
                dummy2.reg=Str2Reg("RDI");
                IMovRInd(jit,dummy2.reg->code,BASEREG_CODE,jit->ireg_shadows+8*dst->vreg,8,FALSE);
            }
            break;
            case AV_IINDIR:
            case AV_FINDIR:
            dummy2.reg=dst->reg;
            break;
            case AV_IFRAME:
            case AV_FFRAME:
            dummy2.reg=Str2Reg("RSP");
        end:
        switch(src->type) {
            start:
                dummy3.type=AV_IREG;
                case AV_IVINDIR:
                case AV_FVINDIR:
                dummy3.reg=IRegCodeToReg(r=RMapGetPhysicalReg(rpn->r_map,src->vreg));
                if(r==-1) {
                    dummy3.reg=Str2Reg("RSI");
                    IMovRInd(jit,dummy3.reg->code,BASEREG_CODE,jit->ireg_shadows+8*src->vreg,8,FALSE);
                }
                break;
                case AV_IINDIR:
                case AV_FINDIR:
                dummy3.reg=src->reg;
                break;
                case AV_FFRAME:
                case AV_IFRAME:
                dummy3.reg=Str2Reg("RSP");
            end:
            dummy.type=AV_IREG;
            dummy.reg=Str2Reg("RSI");
            IMov(jit,rpn,&dummy,&dummy3,FALSE); 
            if(src->offset) {
                //ADD RSI,xxxxxxxx
                JIT_AppendAsmText(jit,"\x48\x81\xC6",3);
                JIT_AppendAsmText(jit,&src->offset,4);
            }
        }
        dummy.type=AV_IREG;
        dummy.reg=Str2Reg("RDI");
        IMov(jit,rpn,&dummy,&dummy2,FALSE);
        if(dst->offset) {
            //ADD RDI,xxxxxxxx
            JIT_AppendAsmText(jit,"\x48\x81\xC7",3);
            JIT_AppendAsmText(jit,&dst->offset,4);
        }
    }
    loop:
    switch(len) {
        case 0:
        goto en;
        case 1:
        //MOVSB
        JIT_AppendAsmText(jit,"\xA4",1);
        goto en;
        case 2:
        //MOVSW
        JIT_AppendAsmText(jit,"\x66\xA5",2);
        goto loop;
        case 3:
        //MOVSW+MOVSB
        JIT_AppendAsmText(jit,"\x66\xA5""\xA4",2+1);
        goto en;
        case 4:
        //MOVSD
        JIT_AppendAsmText(jit,"\xA5",1);
        goto en;
        case 5:
        //MOVSD+MOVSB
        JIT_AppendAsmText(jit,"\xA5""\xA4",1+1);
        goto en;
        case 6:
        //MOVSD+MOVSW
        JIT_AppendAsmText(jit,"\xA5""\x66\xA5",1+2);
        goto en;
        case 7:
        //MOVSD+MOVSW+MOVSB
        JIT_AppendAsmText(jit,"\xA5""\x66\xA5""\xA4",1+2+1);
        goto en;
        case 8:
        //Juicy 64 bit one(MOVSQ)
        JIT_AppendAsmText(jit,"\x48\xA5",2);
        goto en;
        default:
        if(IRegIsAliveAfter(rpn,rcx=Str2Reg("RCX")))
            IPushR(jit,rcx->code);
        IMovRI(jit,rcx->code,len/8);
        //REP MOVSQ
        JIT_AppendAsmText(jit,"\xF3\x48\xA5",3);
        len%=8;
        if(IRegIsAliveAfter(rpn,rcx=Str2Reg("RCX")))
            IPopR(jit,rcx->code);
        goto loop;
    }
    en:
}
U0 JitTypecastDerefs(JIT *jit) {
    CRPN *rpn,*next;
    I64 iter;
    for(rpn=FirstRPN(jit->cur_rpn_start);rpn;) {
        next=rpn->next;
        if(rpn->type.code!=OP_TYPECAST_BIT4BIT)
            goto next;
        if(rpn->next->type.code!=OP_DEREF)
            goto next;
        if(rpn->flags&RPN_F_NO_TYPECAST)
            goto next;
        if(rpn->type.is_flt)
            rpn->next->type.is_flt=TRUE,rpn->next->operands[2](I64)=8;
        else {
            rpn->next->type.is_flt=FALSE;
            rpn->next->operands[2](I64)=rpn->operands[1](I64);
        }
        RPNFree(rpn);
        next:
        rpn=next;
    }
}
U8 *JitEmitCode(JIT *jit,I64 argc) {
    CVector *flat=VectorNew,*incoming,*outgoing;
    I64 idx,idx2,foffset,bytes,body_size,prolog_size,taddr;
    U8 *tmp,*epilog,*body_code;
    U16 word;
    U32 dword;
    CRPN *rpn,*prev,*jmp_to;
    jit->live_icap=(jit->ireg_stk_start=jit->tireg_cnt=jit->vireg_cnt+1);
    jit->tfreg_cnt=jit->cur_tfreg=jit->vfreg_cnt+1;
    FoldConstants(jit);
    JitTypecastDerefs(jit);
    JIT_PropigateAssigns(jit);
    for(rpn=FirstRPN(jit->cur_rpn_start);rpn;rpn=rpn->next) {
        if(rpn->type.code==OP_JMP)
            rpn->operands[0]=GetRPNEnterLabel(rpn->operands[0]);
    }
    CacheRPNSuccessors(jit);
    TellArgumentsToBePushed(jit);
    FlattenRPNTo(FirstRPN(jit->cur_rpn_start),flat);
    RPN_AssignOperandValues(jit,FirstRPN(jit->cur_rpn_start),NULL);
    AssignX87Regs(jit,flat);
    NullifyUselessDests(jit,flat);
    ComputeRPNLiveness(jit,FirstRPN(jit->cur_rpn_start));
    //Aligned to 8
    foffset=-jit->allocai_size;
    //We started tireg_cnt at vireg_cnt
    foffset-=8*(jit->tireg_cnt);
    jit->ireg_shadows=foffset;
    //DITTO
    foffset-=8*(jit->tfreg_cnt+1);
    jit->freg_shadows=foffset;
    jit->fpu_stk_size=0;
    outgoing=VectorNew;
    for(idx=0;idx!=flat->size;idx++) {
        rpn=flat->data[idx];
        incoming=VectorNew;
        for(idx2=0;idx2!=outgoing->size;idx2+=2) {
            if(rpn->flags&RPN_F_ASM_TAINTED||rpn->flags&RPN_F_SPILL_ALL) {
                goto skip;
            }
            if(IsControlNode(rpn))
                if(!Bt(rpn->live_in,outgoing->data[idx2]))
                    goto skip;
            VectorPush(rpn->r_map,outgoing->data[idx2]);
            VectorPush(rpn->r_map,outgoing->data[idx2+1]);
            VectorPush(incoming,outgoing->data[idx2]);
            VectorPush(incoming,outgoing->data[idx2+1]);
            skip:
        }
        UnspillRPNRegs(jit,rpn,TRUE);
        if(rpn->type.code==OP_JMP_TABLE) {
            rpn->r_map->size=0;
            if(rpn->next->my_avalue.type==AV_VIREG)
                rpn->next->my_avalue.type=AV_IREG_SHADOW;
            rpn->my_avalue.type=AV_UNOCC;
        }
        outgoing->size=0;
        for(idx2=0;idx2!=rpn->r_map->size;idx2+=2) {
            VectorPush(outgoing,rpn->r_map->data[idx2]);
            VectorPush(outgoing,rpn->r_map->data[idx2+1]);
        }
        rpn->in_r_map=incoming;
    }
    VectorFree(outgoing);
    #ifdef JIT_DBG 
    DumpRPN(FirstRPN(jit->cur_rpn_start));
    #endif
    jit->fpu_stk_size=0;
    //This generates the body,we will need to generate a prolog and epilog
    CompileOps(jit,flat);
    //Patch ops
    for(idx=0;idx!=flat->size;idx++) {
        rpn=flat->data[idx];
        switch(rpn->type.code) {
            case OP_JMP_TABLE:
            for(idx2=0;idx2!=rpn->jmp_table_trampolines->size;idx2+=2) {
                jmp_to=rpn->jmp_table_trampolines->data[idx2];
                taddr=rpn->jmp_table_trampolines->data[idx2+1];
                (jit->buffer+taddr)(I32*)[0]=GetRPNEnterLabel(jmp_to)->bin_off-(taddr+4);
            }
            break;
            case OP_REF_DATA:
            Patch32(jit,rpn->patch_addr,rpn->operands[0](CRPN*)->bin_off);
            break;
            case OP_RETURN:
            //jit->buf_size points to epilog
            Patch32(jit,rpn->patch_addr,jit->buf_size);
            break;
            case OP_JMP:
            Patch32(jit,rpn->patch_addr,GetRPNEnterLabel(rpn->operands[0](CRPN*))->bin_off);
            break;
        }
    }
    //
    // Hacky solution,backup the buffer and size,write the prolog,then append the eplilog
    //
    body_size=jit->buf_size;
    body_code=jit->buffer;
    jit->buffer=NULL;
    jit->buf_cap=jit->buf_size=0;
    //PUSH rbp
    //MOV rbp,rsp
    //SUB rsp,foffset
    //
    JIT_AppendAsmText(jit,"\x55\x48\x89\xE5\x48\x81\xEC",4+3);
    foffset-=8; //TODO REMOVE
    I64 fstato=foffset;
    foffset=-foffset;
    JIT_AppendAsmText(jit,&foffset.u32[0],4);
    for(idx=0;idx!=jit->tainted_caller_saved->size;idx++)
        IPushR(jit,jit->tainted_caller_saved->data[idx]);
    foffset=-foffset;
    foffset-=jit->tainted_caller_saved->size*8;
    //FSTSW AX
    //SHR AX,11
    //AND AX,0b111
    //MOV [RBP+offset],AX
    JIT_AppendAsmText(jit,"\x9B\xDF\xE0\x66\xC1\xE8\x0B\x66\x83\xE0\x07\x66\x89\x85",0xb+3);
    JIT_AppendAsmText(jit,&fstato.u32[0],4);
    tmp=MAlloc(jit->buf_cap=body_size+jit->buf_size+128);
    MemCpy(tmp,jit->buffer,prolog_size=jit->buf_size);
    Free(jit->buffer),jit->buffer=tmp;
    //
    JIT_AppendAsmText(jit,body_code,body_size);
    Free(body_code);
    //Trigers a segfault when fpu top is mismatched
    //FSTSW AX
    //SHR AX,11
    //AND AX,7
    //CMP AX,[RBP+0x11223344]
    //JE exit
    //MOV RAX,[0]
    //exit:
    IPushR(jit,0);
    JIT_AppendAsmText(jit,"\x9B\xDF\xE0\x66\xC1\xE8\x0B\x66\x83\xE0\x07\x66\x3B\x85",0xb+3);
    JIT_AppendAsmText(jit,&fstato.u32[0],4);
    JIT_AppendAsmText(jit,"\x74\x08\x48\x8B\x04\x25\x00\x00\x00\x00",0xa);
    IPopR(jit,0);
    //LEA RSP,[RBP+foffset]
    JIT_AppendAsmText(jit,"\x48\x8D\xA5",3);
    JIT_AppendAsmText(jit,&foffset.u32[0],4);
   for(idx=jit->tainted_caller_saved->size-1;idx>=0;idx--) {
        IPopR(jit,jit->tainted_caller_saved->data[idx]);
    }
	JIT_AppendAsmText(jit,"\xC9\xC2",2); //LEAVE,RET1 8*argc
	word=8*argc;
	JIT_AppendAsmText(jit,&word,2);
    for(idx=0;idx!=flat->size;idx++) {
        rpn=flat->data[idx];
        switch(rpn->type.code) {
            case OP_RELOCATION:
            rpn->operands[0](U0**)[0]=jit->buffer+prolog_size+rpn->patch_addr;
            break;
            case OP_DUMP_PTR:
            rpn->operands[0](U0**)[0]=jit->buffer+prolog_size+rpn->bin_off;
            break;
        }
    }
    Free(epilog);
    VectorFree(flat);
    return jit->buffer;
}
class CBreakpoint {
    I64 enabled;
    CRPN *at;
    I64 user_data[0];
};
CRPN *JIT_BREAKPOINT(JIT *jit,U0 *ctrl,CBreakpoint *bp,U0 (*routine)()) {
    CRPN *ret=jit_rpnNew(jit,OP_BREAKPOINT);
    ret->operands[0]=ctrl(CAsmValue*);
    ret->operands[1]=bp(CAsmValue*);
    ret->operands[2]=routine(CAsmValue*);
    return ret;
}
CRPN *JIT_GetBreakpointNodeByPtr(JIT *jit,U0 *ptr) {
    CRPN *st=FirstRPN(jit->cur_rpn_start);
    CRPN *pbp=NULL;
    for(;st;st=st->next) {
        if(st->type.code==OP_BREAKPOINT) {
            if(pbp) {
                if(jit->buffer(U8*)+pbp->bin_off<=ptr<jit->buffer(U8*)+st->bin_off)
                    return pbp;
            }
            pbp=st;
        }
    }
    if(!pbp) return NULL;
    return pbp;
}
CBreakpoint *JIT_GetBreakpointByPtr(JIT *jit,U0 *ptr) {
    CRPN *pbp=JIT_GetBreakpointNodeByPtr(jit,ptr);
    if(!pbp) return NULL;
    return pbp->operands[1](CBreakpoint*);
}
//Make less work for the garbage collector
CRPN *JIT_RemoveNonDebuggerInfo(JIT *jit) {
    CRPN *cur=FirstRPN(jit->cur_rpn_start),*next;
    jit->cur_rpn_start=NULL;
    for(;cur;cur=next) {
        next=cur->next;
        if(cur->type.code!=OP_BREAKPOINT)
            RPNFree(cur);
        else
            jit->cur_rpn_start=cur;
    }
}
//See debuggerAsm.yasm
class CDebuggerRegs {
    I64 dummy2; //0
    I64 RBX; //8
    I64 RCX; //16
    I64 RDX; //24
    I64 RSI; //32
    I64 RDI; //40
    I64 dummy; //48
    I64 RBP; //56
    I64 R8; //64
    I64 R9; //72
    I64 R10; //80
    I64 R11; //88
    I64 R12; //96
    I64 R13; //112
    I64 R14; //120
    I64 R15; //128
};
I64 *JIT_GetVregPtr(JIT *parent,CDebuggerRegs *regs,U0 *ptr,I64 vreg) {
    CRPN *bp_n=JIT_GetBreakpointNodeByPtr(parent,ptr);
    if(!bp_n) return NULL;
    if(-1==RMapGetPhysicalReg(bp_n->r_map,vreg)) {
        return &(regs->RBP(U8*)+parent->ireg_shadows)(I64*)[vreg];
    }
    switch(RMapGetPhysicalReg(bp_n->r_map,vreg)) {
        //case 0: RAX is only used as an accumulator
        case 1: return &regs->RCX;
        case 2: return &regs->RDX;
        case 3: return &regs->RBX;
        //case 4: No way am i using the stack register as a gp
        case 5: return &regs->RBP;
        case 6: return &regs->RSI;
        case 7: return &regs->RDI;
        case 8: return &regs->R8;
        case 9: return &regs->R9;
        case 10: return &regs->R10;
        case 11: return &regs->R11;
        case 12: return &regs->R12;
        case 13: return &regs->R13;
        case 14: return &regs->R14;
        case 15: return &regs->R15;
    }
    return NULL;
} 
I64 *JIT_GetVregPtrFromParent(JIT *parent,U0 *ptr,U0 *rbp,I64 vreg) {
    CRPN *bp_n=JIT_GetBreakpointNodeByPtr(parent,ptr);
    if(!bp_n) return NULL;
    //If its stored in a vreg,its not spilled to the stack
    if(-1!=RMapGetPhysicalReg(bp_n->r_map,vreg)) return NULL;
    return &(rbp(U8*)+parent->ireg_shadows)(I64*)[vreg];
}
class jit_debugger_ctrl {
    #define JIT_DBG_STEP 1
    #define JIT_DBG_NEXT 2
    #define JIT_DBG_FIN 3
    I64 code;
    U0 *prev_stack_ptr;
    //Our silly debugger "enters" into functions,but we will need the last known RIP to compute which breakpoint we were at before entering a breakpoint.
    U0 *last_rip;
};
U0 JIT_ASM_TAINT_LABEL(CRPN *op) {
    op->flags|=RPN_F_ASM_TAINTED;
}
CRPN *JIT_MOV(JIT *jit) {
    return jit_rpnNew(jit,OP_ASSIGN);
}
CRPN *JIT_F64(JIT *jit,F64 f) {
    return jit_rpnFlt(jit,f);
}
CRPN *JIT_I64(JIT *jit,I64 i) {
    return jit_rpnInt(jit,i);
}
CRPN *JIT_JMP(JIT *jit,CRPN *to)  {
    CRPN *ret=jit_rpnNew(jit,OP_JMP);
    ret->operands[0]=to;
    return ret;
}
CRPN *JIT_LABEL(JIT *jit) {
    CRPN *ret=jit_rpnBodyStart(jit);
    jit_rpnBodyEnd(jit,ret);
    return ret;
}
CRPN *JIT_PATCH(JIT *jit,CRPN *item,CRPN *to=NULL) {
    if(!to)
        to=JIT_LABEL(jit);
    item->operands[0]=to;
    return to;
}

//Tested 1
CRPN *JIT_FUNC_CALL(JIT *jit,I64 cnt,I64 vargs_base=-1) {
    CRPN *ret=jit_rpnNew(jit,OP_CALL);
    ret->type.body_length=cnt;
    if(vargs_base!=-1)
		ret->type.vargs_base=vargs_base;
	else
		ret->type.vargs_base=cnt;
    return ret;
}
//Tested 1
CRPN *JIT_GETARGI(JIT *jit,I64 a) {
    CRPN *ret=jit_rpnNew(jit,OP_GETARG);
    jit_rpnInt(jit,a);
    return ret;
}
//Tested 1
CRPN *JIT_GETARGF(JIT *jit,I64 a) {
    CRPN *ret=jit_rpnNewF(jit,OP_GETARG);
    jit_rpnInt(jit,a);
    return ret;
}
//Tested 1
CRPN *JIT_RET(JIT *jit) {
    return jit_rpnNew(jit,OP_RETURN);
}
//Tested 1
CRPN *JIT_ADDI(JIT *jit) {
    return jit_rpnNew(jit,OP_ADD);
}
//Tested 1
CRPN *JIT_SUBI(JIT *jit) {
    return jit_rpnNew(jit,OP_SUB);
}
//Tested 1
CRPN *JIT_NEGI(JIT *jit) {
    return jit_rpnNew(jit,OP_NEG);
}
CRPN *JIT_FPOS(JIT *jit) {
    return jit_rpnNewF(jit,OP_POS);
}
CRPN *JIT_POSI(JIT *jit) {
    return jit_rpnNew(jit,OP_POS);
}
//Tested 1
CRPN *JIT_MULI(JIT *jit) {
    return jit_rpnNew(jit,OP_MUL);
}
//Tested 1
CRPN *JIT_DIVI(JIT *jit) {
    return jit_rpnNew(jit,OP_DIV);
}
//Tested 1
CRPN *JIT_MODI(JIT *jit) {
    return jit_rpnNew(jit,OP_MOD);
}
//tested 1
CRPN *JIT_ORI(JIT *jit) {
    return jit_rpnNew(jit,OP_OR);
}
//tested 1
CRPN *JIT_XORI(JIT *jit) {
    return jit_rpnNew(jit,OP_XOR);
}
//Tested 1
CRPN *JIT_ANDI(JIT *jit) {
    return jit_rpnNew(jit,OP_AND);
}
//Tested 1
CRPN *JIT_LSHI(JIT *jit) {
    return jit_rpnNew(jit,OP_SHL);
}
//Tested 1
CRPN *JIT_RSHI(JIT *jit) {
    return jit_rpnNew(jit,OP_SHR);
}
//tested
CRPN *JIT_RSHU(JIT *jit) {
    return jit_rpnNewU(jit,OP_SHR);
}
//TESTED 1
CRPN *JIT_NOTI(JIT *jit) {
    return jit_rpnNew(jit,OP_NOT);
}
//Tested 1
CRPN *JIT_LTI(JIT *jit) {
    return jit_rpnNew(jit,OP_LT);
}
//Tested 1
CRPN *JIT_LTU(JIT *jit) {
    return jit_rpnNewU(jit,OP_LT);
}
//Tested 1
CRPN *JIT_GTI(JIT *jit) {
    return jit_rpnNew(jit,OP_GT);
}
//Tested 1
CRPN *JIT_GTU(JIT *jit) {
    return jit_rpnNewU(jit,OP_GT);
}
//Tested 1
CRPN *JIT_LEI(JIT *jit) {
    return jit_rpnNew(jit,OP_LE);
}
//Tested 1
CRPN *JIT_LEU(JIT *jit) {
    return jit_rpnNewU(jit,OP_LE);
}
//Tested 1
CRPN *JIT_GEI(JIT *jit) {
    return jit_rpnNew(jit,OP_GE);
}
//Tested 1
CRPN *JIT_GEU(JIT *jit) {
    return jit_rpnNewU(jit,OP_GE);
}
//Tested 1
CRPN *JIT_NEI(JIT *jit) {
    return jit_rpnNew(jit,OP_NE);
}
//Tested 1
CRPN *JIT_EQI(JIT *jit) {
    return jit_rpnNew(jit,OP_EQ);
}
CRPN *JIT_TYPECAST(JIT *jit,I64 sz=8) {
    CRPN *ret=jit_rpnNew(jit,OP_TYPECAST_BIT4BIT);
    ret->operands[1]=sz;
    return ret;
}
CRPN *JIT_FTYPECAST(JIT *jit) {
    CRPN *ret=jit_rpnNewF(jit,OP_TYPECAST_BIT4BIT);
    ret->operands[1]=8;
    return ret;
}
CRPN *JIT_REF_DATA(JIT *jit,CRPN *to=NULL) {
    CRPN *ret=jit_rpnNew(jit,OP_REF_DATA);
    ret->operands[0]=to;
    return ret;
}
CRPN *JIT_DU8(JIT *jit,I64 v) {
    CRPN *ret=jit_rpnNew(jit,OP_DU8);
    ret->operands[0]=v;
    return ret;
}
CRPN *JIT_DU16(JIT *jit,I64 v) {
    CRPN *ret=jit_rpnNew(jit,OP_DU16);
    ret->operands[0]=v;
    return ret;
}
CRPN *JIT_DU32(JIT *jit,I64 v) {
    CRPN *ret=jit_rpnNew(jit,OP_DU32);
    ret->operands[0]=v;
    return ret;
}
CRPN *JIT_DU64(JIT *jit,U64 v) {
    CRPN *ret=jit_rpnNew(jit,OP_DU64);
    ret->operands[0]=v;
    return ret;
}
CRPN *JIT_UDEREF(JIT *jit,I32 off=0,I64 sz=8) {
    CRPN *ret=jit_rpnNewU(jit,OP_DEREF);
    ret->operands[1]=off(CAsmValue*);
    ret->operands[2]=sz(CAsmValue*);
    return ret;
}
CRPN *JIT_UDEREF_NO_TC(JIT *jit,I32 off=0,I64 sz=8) {
    CRPN *r=JIT_UDEREF(jit,off,sz);
    r->flags|=RPN_F_NO_TYPECAST;
    return r;
}
CRPN *JIT_DEREF(JIT *jit,I32 off=0,I64 sz=8) {
    CRPN *ret=jit_rpnNew(jit,OP_DEREF);
    ret->operands[1]=off(CAsmValue*);
    ret->operands[2]=sz(CAsmValue*);
    return ret;
}
CRPN *JIT_DEREF_NO_TC(JIT *jit,I32 off=0,I64 sz=8) {
    CRPN *r=JIT_DEREF(jit,off,sz);
    r->flags|=RPN_F_NO_TYPECAST;
    return r;
}
CRPN *JIT_FMOV(JIT *jit) {
    return jit_rpnNewF(jit,OP_ASSIGN);
}
CRPN *JIT_FADD(JIT *jit) {
    return jit_rpnNewF(jit,OP_ADD);
}
CRPN *JIT_FSUB(JIT *jit) {
    return jit_rpnNewF(jit,OP_SUB);
}
CRPN *JIT_FAND(JIT *jit) {
    return jit_rpnNewF(jit,OP_AND);
}
CRPN *JIT_FXOR(JIT *jit) {
    return jit_rpnNewF(jit,OP_XOR);
}
CRPN *JIT_FNEG(JIT *jit) {
    return jit_rpnNewF(jit,OP_NEG);
}
CRPN *JIT_FNOT(JIT *jit) {
    return jit_rpnNewF(jit,OP_NOT);
}
CRPN *JIT_FOR(JIT *jit) {
    return jit_rpnNewF(jit,OP_OR);
}
CRPN *JIT_FSHL(JIT *jit) {
    return jit_rpnNewF(jit,OP_SHL);
}
CRPN *JIT_FSHR(JIT *jit) {
    return jit_rpnNewF(jit,OP_SHR);
}
CRPN *JIT_FMUL(JIT *jit) {
    return jit_rpnNewF(jit,OP_MUL);
}
CRPN *JIT_FDIV(JIT *jit) {
    return jit_rpnNewF(jit,OP_DIV);
}
CRPN *JIT_FPREM(JIT *jit) {
    return jit_rpnNewF(jit,OP_MOD);
}
CRPN *JIT_FLT(JIT *jit) {
    return jit_rpnNewF(jit,OP_LT);
}
CRPN *JIT_FGT(JIT *jit) {
    return jit_rpnNewF(jit,OP_GT);
}
CRPN *JIT_FLE(JIT *jit) {
    return jit_rpnNewF(jit,OP_LE);
}
CRPN *JIT_FGE(JIT *jit) {
    return jit_rpnNewF(jit,OP_GE);
}
CRPN *JIT_FEQ(JIT *jit) {
    return jit_rpnNewF(jit,OP_EQ);
}
CRPN *JIT_FNE(JIT *jit) {
    return jit_rpnNewF(jit,OP_NE);
}
CRPN *JIT_FMOD(JIT *jit) {
    return jit_rpnNewF(jit,OP_MOD);
}
CRPN *JIT_FCALL(JIT *jit,I64 argc,I64 vargs_base=-1) {
    CRPN *ret=jit_rpnNewF(jit,OP_CALL);
    ret->type.body_length=argc;
    if(vargs_base!=-1)
		ret->type.vargs_base=vargs_base;
	else
		ret->type.vargs_base=argc;
    return ret;
}
CRPN *JIT_FDEREF(JIT *jit,I32 off=0) {
    CRPN *ret=jit_rpnNewF(jit,OP_DEREF);
    ret->operands[1]=off(CAsmValue*);
    ret->operands[2]=8;
    return ret;
}
CRPN *JIT_ALIGN_FILL(JIT *jit,I64 align,I64 fill) {
    CRPN *ret=jit_rpnNew(jit,OP_ALIGN);
    ret->operands[0]=align(CAsmValue*);
    ret->operands[1]=fill(CAsmValue*);
    return ret;
}
CRPN *JIT_R(JIT *jit,I64 f) {
    return jit_rpnNewVreg(jit,f);
}
I64 JIT_ALLOCAI(JIT *jit,I64 sz) {
    CRPN *ret=jit_rpnNew(jit,OP_ALIGN);
    ret->operands[0]=sz(CAsmValue*);
    jit->allocai_size+=sz;
    //https://en.wikipedia.org/wiki/Data_structure_alignment
    return -(jit->allocai_size=(jit->allocai_size+(8-1))&-8);
}
CRPN *JIT_FFRAME(JIT *jit,I64 off) {
    CRPN *ret=jit_rpnNewF(jit,OP_VARIABLE);
    ret->my_avalue.type=AV_FFRAME;
    ret->my_avalue.size=8;
    ret->my_avalue.offset=off;
    return ret;
}
CRPN *JIT_IFRAME(JIT *jit,I64 off,I64 sz=8) {
    CRPN *ret=jit_rpnNew(jit,OP_VARIABLE);
    ret->my_avalue.type=AV_IFRAME;
    ret->my_avalue.size=sz;
    ret->my_avalue.offset=off;
    return ret;
}
CRPN *JIT_RBP(JIT *jit) {
    CRPN *ret=jit_rpnNew(jit,OP_VARIABLE);
    ret->my_avalue.type=AV_IREG;
    ret->my_avalue.reg=Str2Reg("RBP");
    return ret;
}
CRPN *JIT_RELOCATION(JIT *jit,U0 **ptr) {
    CRPN *ret=jit_rpnNew(jit,OP_RELOCATION);
    ret->operands[0](U0**)=ptr;
    return ret;
}
CRPN *JIT_FASSIGN_ADD(JIT *jit) {
    return jit_rpnNewF(jit,OP_ASSIGN_AND);
}
CRPN *JIT_FASSIGN_SUB(JIT *jit) {
    return jit_rpnNewF(jit,OP_ASSIGN_SUB);
}
CRPN *JIT_FASSIGN_MUL(JIT *jit) {
    return jit_rpnNewF(jit,OP_ASSIGN_MUL);
}
CRPN *JIT_FASSIGN_MOD(JIT *jit) {
    return jit_rpnNewF(jit,OP_ASSIGN_MOD);
}
CRPN *JIT_FASSIGN_DIV(JIT *jit) {
    return jit_rpnNewF(jit,OP_ASSIGN_DIV);
}
CRPN *JIT_FASSIGN_OR(JIT *jit) {
    return jit_rpnNewF(jit,OP_ASSIGN_OR);
}
CRPN *JIT_FASSIGN_AND(JIT *jit) {
    return jit_rpnNewF(jit,OP_ASSIGN_AND);
}
CRPN *JIT_FASSIGN_XOR(JIT *jit) {
    return jit_rpnNewF(jit,OP_ASSIGN_XOR);
}
CRPN *JIT_FASSIGN_SHL(JIT *jit) {
    return jit_rpnNewF(jit,OP_ASSIGN_SHL);
}
CRPN *JIT_FASSIGN_SHR(JIT *jit) {
    return jit_rpnNewF(jit,OP_ASSIGN_SHR);
}
//
CRPN *JIT_ASSIGN_ADDI(JIT *jit) {
    return jit_rpnNew(jit,OP_ASSIGN_ADD);
}
CRPN *JIT_ASSIGN_SUBI(JIT *jit) {
    return jit_rpnNew(jit,OP_ASSIGN_SUB);
}
CRPN *JIT_ASSIGN_MULI(JIT *jit) {
    return jit_rpnNew(jit,OP_ASSIGN_MUL);
}
CRPN *JIT_ASSIGN_MODI(JIT *jit) {
    return jit_rpnNew(jit,OP_ASSIGN_MOD);
}
CRPN *JIT_ASSIGN_DIVI(JIT *jit) {
    return jit_rpnNew(jit,OP_ASSIGN_DIV);
}
CRPN *JIT_ASSIGN_ORI(JIT *jit) {
    return jit_rpnNew(jit,OP_ASSIGN_OR);
}
CRPN *JIT_ASSIGN_ANDI(JIT *jit) {
    return jit_rpnNew(jit,OP_ASSIGN_AND);
}
CRPN *JIT_ASSIGN_XORI(JIT *jit) {
    return jit_rpnNew(jit,OP_ASSIGN_XOR);
}
CRPN *JIT_ASSIGN_SHLI(JIT *jit) {
    return jit_rpnNew(jit,OP_ASSIGN_SHL);
}
CRPN *JIT_ASSIGN_SHRI(JIT *jit) {
    return jit_rpnNew(jit,OP_ASSIGN_SHR);
}
CRPN *JIT_COMMAI(JIT *jit) {
    return jit_rpnNew(jit,OP_COMMA);
}
CRPN *JIT_FCOMMA(JIT *jit) {
    return jit_rpnNewF(jit,OP_COMMA);
}
CRPN *JIT_FPRE_INC(JIT *jit) {
    return jit_rpnNewF(jit,OP_PRE_INC);
}
CRPN *JIT_FPRE_DEC(JIT *jit) {
    return jit_rpnNewF(jit,OP_PRE_DEC);
}
CRPN *JIT_FPOST_INC(JIT *jit) {
    return jit_rpnNewF(jit,OP_POST_INC);
}
CRPN *JIT_FPOST_DEC(JIT *jit) {
    return jit_rpnNewF(jit,OP_POST_DEC);
}
CRPN *JIT_PRE_INCI(JIT *jit,I64 amt=1) {
    CRPN *ret=jit_rpnNew(jit,OP_PRE_INC);
    ret->operands[1](I64)=amt;
    return ret;
}
CRPN *JIT_PRE_DECI(JIT *jit,I64 amt=1) {
    CRPN *ret=jit_rpnNew(jit,OP_PRE_DEC);
    ret->operands[1](I64)=amt;
    return ret;
}
CRPN *JIT_POST_INCI(JIT *jit,I64 amt=1) {
    CRPN *ret=jit_rpnNew(jit,OP_POST_INC);
    ret->operands[1](I64)=amt;
    return ret;
}
CRPN *JIT_FRET(JIT *jit) {
    return jit_rpnNewF(jit,OP_RETURN);
}
CRPN *JIT_RETI(JIT *jit) {
    return jit_rpnNew(jit,OP_RETURN);
}
CRPN *JIT_POST_DECI(JIT *jit,I64 amt=1) {
    CRPN *ret=jit_rpnNew(jit,OP_POST_DEC);
    ret->operands[1](I64)=amt;
    return ret;
}
CRPN *JIT_DUMP_PTR(JIT *jit,U0 **ptr) {
    CRPN *ret=jit_rpnNew(jit,OP_DUMP_PTR);
    ret->operands[0](U0**)=ptr;
    return ret;
}
CRPN *JIT_JMP_TABLE(JIT *jit,I64 len) {
    CRPN *ret=jit_rpnNew(jit,OP_JMP_TABLE);
    ret->type.body_length=len;
    ret->jmp_table=MAlloc(len*sizeof(CRPN*));
    return ret;
} 
U8 *HexifyStr(U8 *str,I64 len) {
    U8 *ret=MAlloc(len*2+1),*rptr;
    I64 idx=0;
    U8 *hex_digits="0123456789ABCDEF";
    for(rptr=ret;idx!=len;idx++) {
        *rptr++=hex_digits[(str[idx]>>4)&0xf];
        *rptr++=hex_digits[str[idx]&0xf];
    }
    return ret;
}
U0 JitDumpBytes(JIT *jit) {
    U8 *hex=HexifyStr(jit->buffer,jit->buf_size);
    "\n\n\n%s\n\n\n",hex;
    Free(hex);
}
#ifdef JIT_DBG
U0 FCallTests1() {
    I64 f1,f2;
    JIT *jit;
    "=== [FUNC_CALL 1] ===\n";
    jit=jit_Init;
    CRPN *body0=jit_rpnBodyStart(jit);
    JIT_FUNC_CALL(jit,3);
    JIT_R(jit,0);
    //
    JIT_MOV(jit);
    JIT_R(jit,1);
    JIT_I64(jit,1);
    //
    JIT_MOV(jit);
    JIT_R(jit,2);
    JIT_I64(jit,2);
    //
    JIT_F64(jit,3);
    jit_rpnBodyEnd(jit,body0);
    JitEmitCode(jit);
    JitDumpBytes(jit);
    JIT_FREE(jit);
    "=== [FUNC_CALL 2] ===\n";
    jit=jit_Init;
    JIT_FADD(jit);
    JIT_FMUL(jit);
    JIT_F64(jit,1.0);
    JIT_F64(jit,2.0);
    //
    JIT_FUNC_CALL(jit,1);
    JIT_R(jit,0);
    //
    JIT_FDIV(jit);
    JIT_F64(jit,4.0);
    JIT_F64(jit,5.0);
    //FADD
    JIT_F64(jit,3.0);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
    //Test for not prigating pushed arguments(whoose my_avalue would be overwriten with a AV_PUSH)
    "=== [FUNC_CALL 3] ===\n";
    jit=jit_Init;
    JIT_FUNC_CALL(jit,2);
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_I64(jit,0);
    JIT_MOV(jit);
    JIT_R(jit,1);
    JIT_I64(jit,1);
    JIT_I64(jit,0x11223344);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
FCallTests1;
U0 GetArgTests1() {
    JIT *jit;
    CRPN *body;
    I64 off;
    "=== [OP_GETARG 1] ===\n";
    jit=jit_Init;
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,16);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_GETARGI(jit,0);
    //
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off);
    JIT_GETARGF(jit,1);
    //
    JIT_FRET(jit);
    JIT_FADD(jit);
    JIT_FFRAME(jit,off);
    JIT_R(jit,0);
    //
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
GetArgTests1;
U0 ReturnTests1() {
    JIT *jit;
    CRPN *body;
    I64 off;
    "=== [OP_RET 1] ===\n";
    jit=jit_Init;
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,8);
    JIT_RET(jit);
    JIT_R(jit,0);
    //
    JIT_RET(jit);
    JIT_R(jit,1);
    //
    JIT_FRET(jit);
    JIT_FFRAME(jit,off);
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
ReturnTests1;
U0 AddTests1() {
    JIT *jit;
    CRPN *body;
    I64 off;
    jit=jit_Init;
    body=jit_rpnBodyStart(jit);
    "=== [Add tests 1] ===\n";
    off=JIT_ALLOCAI(jit,16);
    //
    JIT_MOV(jit);
    JIT_IFRAME(jit,off,4);
    JIT_ADDI(jit);
    JIT_ADDI(jit);
    JIT_ADDI(jit);
    JIT_I64(jit,101);
    JIT_R(jit,0);
    JIT_IFRAME(jit,off);
    JIT_FFRAME(jit,off+8);
    //
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off+8);
    JIT_FADD(jit);
    JIT_FADD(jit);
    JIT_FADD(jit);
    JIT_F64(jit,101);
    JIT_R(jit,0);
    JIT_IFRAME(jit,off);
    JIT_FFRAME(jit,off+8);
    //Communitive operation reduction 
    JIT_MOV(jit);
    JIT_IFRAME(jit,off+8);
    JIT_ADDI(jit);
    JIT_ADDI(jit);
    JIT_ADDI(jit);
    JIT_R(jit,0);
    JIT_I64(jit,1);
    JIT_I64(jit,2);
    JIT_I64(jit,3);
    jit_rpnBodyEnd(jit,body);
    //
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
AddTests1;
U0 SubTests1() {
    JIT *jit;
    CRPN *body;
    I64 off;
    jit=jit_Init;
    "=== [Sub tests 1] ===\n";
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,8);
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_SUBI(jit);
    JIT_SUBI(jit);
    JIT_SUBI(jit);
    JIT_R(jit,1);
    JIT_I64(jit,1);
    JIT_I64(jit,2);
    JIT_I64(jit,3);
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off);
    JIT_FSUB(jit);
    JIT_FSUB(jit);
    JIT_FSUB(jit);
    JIT_F64(jit,1);
    JIT_F64(jit,1);
    JIT_F64(jit,2);
    JIT_F64(jit,3);
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off);
    JIT_FSUB(jit);
    JIT_FFRAME(jit,off);
    JIT_FFRAME(jit,off);
    jit_rpnBodyEnd(jit,body);
    //
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
SubTests1;
U0 NegTests1() {
    JIT *jit;
    CRPN *body;
    I64 off,r;
    jit=jit_Init;
    body=jit_rpnBodyStart(jit);
    "=== [NEG tests 1] ===\n";
    off=JIT_ALLOCAI(jit,8);
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_NEGI(jit);
    JIT_I64(jit,1);
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_NEGI(jit);
    JIT_R(jit,0);
    //
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off);
    JIT_FNEG(jit);
    JIT_F64(jit,1);
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off);
    JIT_FNEG(jit);
    JIT_FFRAME(jit,off);
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
NegTests1;
U0 MulTests1() {
    JIT *jit;
    CRPN *body;
    I64 off,r;
    jit=jit_Init;
    "=== [Mul tests] ===\n";
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,16);
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_MULI(jit);
    JIT_MULI(jit);
    JIT_MULI(jit);
    JIT_R(jit,1);
    JIT_I64(jit,1);
    JIT_I64(jit,2);
    JIT_I64(jit,3);
    //
    JIT_MOV(jit);
    JIT_IFRAME(jit,off);
    JIT_MULI(jit);
    JIT_I64(jit,2);
    JIT_I64(jit,3);
    //
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off+8);
    JIT_FMUL(jit);
    JIT_FMUL(jit);
    JIT_FMUL(jit);
    JIT_F64(jit,3);
    JIT_F64(jit,4);
    JIT_FMUL(jit);
    JIT_F64(jit,1);
    JIT_F64(jit,2);
    JIT_FFRAME(jit,off+8);
    //
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
    //
    "=== [Mul tests(shift reduction)] ===\n";
    jit=jit_Init;
    body=jit_rpnBodyStart(jit);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_I64(jit,1);
    for(r=0;r!=64;r++) {
        JIT_MOV(jit);
        JIT_R(jit,0);
        JIT_MULI(jit);
        JIT_R(jit,0);
        JIT_I64(jit,1<<r);
    }
    //
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
MulTests1;
U0 DivTests1() {
    JIT *jit;
    CRPN *body;
    I64 off;
    jit=jit_Init;
    "=== [Div tests 1] ===\n";
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,16);
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_DIVI(jit);
    JIT_R(jit,1);
    JIT_R(jit,2);
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off+8);
    JIT_FDIV(jit);
    JIT_FFRAME(jit,off+8);
    JIT_FFRAME(jit,off);
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
DivTests1;
U0 ModTests1() {
    JIT *jit;
    CRPN *body;
    I64 off;
    jit=jit_Init;
    "=== [Mod tests 1] ===\n";
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,16);
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_MODI(jit);
    JIT_MODI(jit);
    JIT_R(jit,1);
    JIT_R(jit,2);
    JIT_IFRAME(jit,off);
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off+8);
    JIT_FMOD(jit);
    JIT_FFRAME(jit,off+8);
    JIT_FFRAME(jit,off);
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
ModTests1;
U0 OrTests1() {
    JIT *jit;
    CRPN *body;
    I64 off;
    jit=jit_Init;
    "=== [OR tests 1] ===\n";
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,16);
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_ORI(jit);
    JIT_R(jit,1);
    JIT_R(jit,2);
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off);
    JIT_FOR(jit);
    JIT_FFRAME(jit,off);
    JIT_FFRAME(jit,off+8);
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
OrTests1;
U0 XorTests1() {
    JIT *jit;
    CRPN *body;
    I64 off;
    jit=jit_Init;
    "=== [XOR tests 1] ===\n";
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,16);
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_XORI(jit);
    JIT_R(jit,1);
    JIT_IFRAME(jit,off);
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off);
    JIT_FXOR(jit);
    JIT_FFRAME(jit,off);
    JIT_FXOR(jit);
    JIT_F64(jit,1);
    JIT_FFRAME(jit,off+8);
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
XorTests1;
U0 AndTests1() {
    JIT *jit;
    CRPN *body;
    I64 off;
    jit=jit_Init;
    "=== [AND tests 1] ===\n";
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,16);
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_ANDI(jit);
    JIT_IFRAME(jit,off);
    JIT_IFRAME(jit,off+8);
    //
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off);
    JIT_FAND(jit);
    JIT_R(jit,0);
    JIT_IFRAME(jit,off+8);
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
AndTests1;
U0 ShlTests1() {
    JIT *jit;
    CRPN *body;
    I64 off;
    jit=jit_Init;
    "=== [SHL Tests 1] ===\n";
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,16);
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_LSHI(jit);
    JIT_IFRAME(jit,off);
    JIT_IFRAME(jit,off+8);
    //
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off);
    JIT_FSHL(jit);
    JIT_FFRAME(jit,off);
    JIT_FFRAME(jit,off+8);
    //
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off);
    JIT_FSHL(jit);
    JIT_FFRAME(jit,off);
    JIT_R(jit,0);
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
ShlTests1;
U0 ShrTests1() {
    JIT *jit;
    CRPN *body;
    I64 off;
    jit=jit_Init;
    "=== [SHR Tests 1] ===\n";
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,16);
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_RSHI(jit);
    JIT_IFRAME(jit,off);
    JIT_IFRAME(jit,off+8);
    //
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off);
    JIT_FSHR(jit);
    JIT_FFRAME(jit,off);
    JIT_FFRAME(jit,off+8);
    //
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off);
    JIT_FSHR(jit);
    JIT_FFRAME(jit,off);
    JIT_R(jit,0);
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
ShrTests1;
U0 ShrUTests1() {
    JIT *jit;
    CRPN *body;
    I64 off;
    jit=jit_Init;
    "=== [SHRU Tests 1] ===\n";
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,16);
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_RSHU(jit);
    JIT_R(jit,0);
    JIT_IFRAME(jit,off+8);
    //
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
ShrUTests1;
U0 NotTests1() {
    JIT *jit;
    CRPN *body;
    I64 off;
    jit=jit_Init;
    "=== [NOT Tests 1] ===\n";
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,16);
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_NOTI(jit);
    JIT_R(jit,0);
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off);
    JIT_FNOT(jit);
    JIT_FFRAME(jit,off+8);
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    
    JIT_FREE(jit);
}
NotTests1;
U0 CmpTests1() {
    JIT *jit;
    CRPN *body;
    I64 off;
    jit=jit_Init;
    "=== [CMP Tests 1] ===\n";
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,16);
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_LTI(jit);
    JIT_R(jit,0);
    JIT_I64(jit,1);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_GTI(jit);
    JIT_R(jit,0);
    JIT_I64(jit,2);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_GEI(jit);
    JIT_R(jit,0);
    JIT_I64(jit,3);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_LEI(jit);
    JIT_R(jit,0);
    JIT_I64(jit,4);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_LTU(jit);
    JIT_R(jit,0);
    JIT_I64(jit,1);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_GTU(jit);
    JIT_R(jit,0);
    JIT_I64(jit,2);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_GEU(jit);
    JIT_R(jit,0);
    JIT_I64(jit,3);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_LEU(jit);
    JIT_R(jit,0);
    JIT_I64(jit,4);
    //
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off);
    JIT_FLT(jit);
    JIT_FFRAME(jit,off);
    JIT_I64(jit,1);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_FGT(jit);
    JIT_FFRAME(jit,off);
    JIT_F64(jit,2);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_FGE(jit);
    JIT_FFRAME(jit,off);
    JIT_F64(jit,1.2);
    //
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off);
    JIT_FLE(jit);
    JIT_R(jit,0);
    JIT_FFRAME(jit,off);
    //
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
CmpTests1;
U0 NeTests1() {
    JIT *jit;
    CRPN *body;
    I64 off;
    jit=jit_Init;
    "=== [NE Tests 1] ===\n";
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,16);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_NEI(jit);
    JIT_R(jit,0);
    JIT_I64(jit,1);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_FNE(jit);
    JIT_R(jit,0);
    JIT_FFRAME(jit,off+8);
    //
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
NeTests1;
U0 EqTests1() {
    JIT *jit;
    CRPN *body;
    I64 off;
    jit=jit_Init;
    "=== [EQ Tests 1] ===\n";
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,16);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_EQI(jit);
    JIT_R(jit,0);
    JIT_I64(jit,1);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_FEQ(jit);
    JIT_R(jit,0);
    JIT_FFRAME(jit,off+8);
    //
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
EqTests1;
U0 IncTests1() {
    JIT *jit;
    CRPN *body;
    I64 off;
    jit=jit_Init;
    "=== [INC Tests 1] ===\n";
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,16);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_I64(jit,0);
    JIT_MOV(jit);
    JIT_R(jit,1);
    JIT_I64(jit,1);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_POST_INCI(jit);
    JIT_R(jit,1);
    //
    JIT_PRE_INCI(jit);
    JIT_R(jit,1);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_POST_INCI(jit,8);
    JIT_R(jit,1);
    //
    JIT_PRE_INCI(jit,8);
    JIT_R(jit,1);
    //
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off);
    JIT_FPOST_INC(jit);
    JIT_FFRAME(jit,off+8);
    //
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off);
    JIT_FPRE_INC(jit);
    JIT_FFRAME(jit,off+8);
    //
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
IncTests1;
U0 DecTests1() {
    JIT *jit;
    CRPN *body;
    I64 off;
    jit=jit_Init;
    "=== [DEC Tests 1] ===\n";
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,16);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_I64(jit,0);
    JIT_MOV(jit);
    JIT_R(jit,1);
    JIT_I64(jit,1);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_POST_DECI(jit);
    JIT_R(jit,1);
    //
    JIT_PRE_DECI(jit);
    JIT_R(jit,1);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_POST_DECI(jit,8);
    JIT_R(jit,1);
    //
    JIT_PRE_DECI(jit,8);
    JIT_R(jit,1);
    //
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off);
    JIT_FPOST_DEC(jit);
    JIT_FFRAME(jit,off+8);
    //
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off);
    JIT_FPRE_DEC(jit);
    JIT_FFRAME(jit,off+8);
    //
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
DecTests1;
U0 DerefTests1() {
    JIT *jit;
    CRPN *body;
    I64 off;
    jit=jit_Init;
    "=== [DEREF Tests 1] ===\n";
    //
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,16);
    //
    JIT_MOV(jit);
    JIT_R(jit,1);
    JIT_I64(jit,1);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_ADDI(jit);
    JIT_RBP(jit);
    JIT_I64(jit,off);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_DEREF(jit);
    JIT_R(jit,0);
    //
    JIT_MOV(jit);
    JIT_DEREF(jit);
    JIT_DEREF(jit);
    JIT_DEREF(jit);
    JIT_R(jit,0);
    JIT_R(jit,1);
    //Test offset merging
    JIT_MOV(jit);
    JIT_DEREF(jit);
    JIT_ADDI(jit);
    JIT_R(jit,1);
    JIT_I64(jit,123);
    JIT_I64(jit,3);
    //Ditto
    JIT_MOV(jit);
    JIT_DEREF(jit);
    JIT_ADDI(jit);
    JIT_I64(jit,123);
    JIT_R(jit,1);
    JIT_I64(jit,4);
    //
    JIT_FMOV(jit);
    JIT_FDEREF(jit);
    JIT_ADDI(jit);
    JIT_I64(jit,123);
    JIT_R(jit,1);
    JIT_F64(jit,4);
    
    //
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
DerefTests1;
U0 BigMoveTests1() {
    JIT *jit;
    CRPN *body;
    I64 off,r;
    jit=jit_Init;
    "=== [BIG MOVE 1] ===\n";
    body=jit_rpnBodyStart(jit);
    for(r=0;r!=20;r++) {
        JIT_MOV(jit);
        JIT_R(jit,r);
        JIT_I64(jit,r);
    }
    //
    JIT_MOV(jit);
    JIT_DEREF(jit,,1024);
    JIT_R(jit,0xb);
    JIT_DEREF(jit,1025,1024);
    JIT_R(jit,0xc);
    //
    JIT_MOV(jit);
    JIT_DEREF(jit,256,1024);
    JIT_R(jit,0xc);
    JIT_DEREF(jit,,256);
    JIT_R(jit,0xb);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    for(r=0;r!=19;r++)
        JIT_ADDI(jit);
    for(r=0;r!=20;r++)
        JIT_R(jit,r);
    //
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
BigMoveTests1;
U0 TypecastTests1() {
    JIT *jit;
    CRPN *body;
    I64 off,r;
    jit=jit_Init;
    "=== [TYPECAST 1] ===\n";
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,16);
    
    JIT_MOV(jit);
    JIT_R(jit,1);
    JIT_I64(jit,1);
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off+0);
    JIT_F64(jit,2.0);
    //De-facto bit for bit typecast
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off+8);
    JIT_FTYPECAST(jit);
    JIT_R(jit,1);
    //
    JIT_MOV(jit);
    JIT_IFRAME(jit,off+0);
    JIT_TYPECAST(jit);
    JIT_FFRAME(jit,off+8);
    //Test tyepcasting derefernces
    JIT_FMOV(jit);
    JIT_FDEREF(jit);
    JIT_ADDI(jit);
    JIT_RBP(jit);
    JIT_I64(jit,off+1);
    JIT_F64(jit,2.0);
    //
    JIT_FTYPECAST(jit);
    JIT_R(jit,1);
    //
    JIT_MOV(jit);
    JIT_DEREF(jit);
    JIT_ADDI(jit);
    JIT_RBP(jit);
    JIT_I64(jit,off);
    JIT_TYPECAST(jit);
    JIT_FFRAME(jit,off+8);
    //
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
TypecastTests1;
//
//Here we add lots of register variables to check for spilling
//
U0 GrindTest1() {
    JIT *jit;
    CRPN *body;
    I64 off,r;
    jit=jit_Init;
    "=== [GRIND 1] ===\n";
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,8);
    for(r=0;r!=20;r++) {
        JIT_MOV(jit);
        JIT_R(jit,r);
        JIT_I64(jit,r);
    }
    JIT_MOV(jit);
    JIT_IFRAME(jit,off);
    for(r=0;r!=19;r++)
        JIT_ADDI(jit);
    for(r=0;r!=20;r++)
        JIT_R(jit,r);
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
    //Test for not messing with rdx when __Div-ing
    "=== [GRIND 1.5] ===\n";
    jit=jit_Init;
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,8);
    for(r=0;r!=20;r++) {
        JIT_MOV(jit);
        JIT_R(jit,r);
        JIT_I64(jit,r);
    }
    JIT_MOV(jit);
    JIT_IFRAME(jit,off);
    for(r=0;r!=19;r++)
        JIT_DIVI(jit);
    for(r=0;r!=20;r++)
        JIT_R(jit,r);
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
    
}
GrindTest1;
//Grind the arguments to the function to test AV_PUSH
U0 GrindTest2() {
    JIT *jit;
    CRPN *body;
    I64 off,r;
    jit=jit_Init;
    "=== [GRIND 2] ===\n";
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,8);
    for(r=0;r!=20;r++) {
        JIT_MOV(jit);
        JIT_R(jit,r);
        JIT_I64(jit,r);
    }
    JIT_MOV(jit);
    JIT_IFRAME(jit,off);
    //Our architypes are
    //IArithUnop
    //IIncOp
    //IArithOp
    //FPUOp
    //__Div
    //ShiftOp
    //OP_INLINE_PUSH
    JIT_FUNC_CALL(jit,7);
    JIT_R(jit,0);
    //1. IArithUnop
    JIT_NEGI(jit);
    JIT_R(jit,1);
    //2. IIncOp
    JIT_PRE_INCI(jit);
    JIT_R(jit,2);
    //3. IArithOp
    JIT_ADDI(jit);
    JIT_R(jit,3);
    JIT_R(jit,4);
    //4. FPUOp
    JIT_FDIV(jit);
    JIT_F64(jit,1);
    JIT_FFRAME(jit,off);
    //5. __Div
    JIT_DIVI(jit);
    JIT_R(jit,5);
    JIT_R(jit,6);
    //6. ShiftOp
    JIT_LSHI(jit);
    JIT_R(jit,7);
    JIT_R(jit,8);
    //7. OP_INPLACE_PUSH
    JIT_R(jit,9);
    // Do a sum of registers to test for spilled registers
    JIT_MOV(jit);
    JIT_IFRAME(jit,off);
    for(r=0;r!=19;r++)
        JIT_ADDI(jit);
    for(r=0;r!=20;r++)
        JIT_R(jit,r);
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
    "=== [GRIND 2.5] ===\n";
    jit=jit_Init;
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,40);
    //Test spilling Floating point values at time of function call
    JIT_FMOV(jit);
    JIT_FFRAME(jit,off);
    JIT_FADD(jit); //FADD,FCALL
    JIT_FADD(jit); 
    JIT_FFRAME(jit,off+8);
    JIT_FFRAME(jit,off+16);
    JIT_FCALL(jit,1);
    JIT_R(jit,0);
    JIT_FADD(jit);
    JIT_FFRAME(jit,off+24);
    JIT_FFRAME(jit,off+32);
    //
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
GrindTest2;
U0 RelocTest1() {
    JIT *jit;
    CRPN *body;
    I64 off,r;
    U8 *pat;
    jit=jit_Init;
    "=== [RELOC 1] ===\n";
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,8);
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_RELOCATION(jit,&pat);
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    *pat=0xB0b15C001;
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
RelocTest1;
U0 IfTest1() {
    JIT *jit;
    CRPN *body,*t,*f;
    I64 off,r;
    jit=jit_Init;
    //Test switching to different register mappings per jmp
    "=== [IF 1] ===\n";
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,8);
    for(r=0;r!=20;r++) {
        JIT_MOV(jit);
        JIT_R(jit,r);
        JIT_I64(jit,r);
    }
    jit_rpnIf(jit);
    JIT_R(jit,0);
    //
    t=jit_rpnBodyStart(jit);
    //Reverse order
    for(r=20-1;r>=0;r--) {
        JIT_MOV(jit);
        JIT_R(jit,r);
        JIT_I64(jit,r);
    }
    jit_rpnBodyEnd(jit,t);
    //
    f=jit_rpnBodyStart(jit);
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_I64(jit,123);
    /*
    //Reverse order
    for(r=0;r!=19;r++)
        JIT_SUBI(jit);
    for(r=20-1;r>=0;r--)
        JIT_R(jit,r);
    */
    jit_rpnBodyEnd(jit,f);
    //
    JIT_MOV(jit);
    JIT_R(jit,0);
    //Forward order
    for(r=0;r!=19;r++)
        JIT_ADDI(jit);
    for(r=0;r!=20;r++)
        JIT_R(jit,r);
    //
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
IfTest1;
U0 JmpTest1() {
    JIT *jit;
    CRPN *body,*jmp;
    I64 off,r;
    jit=jit_Init;
    //Test switching to different register mappings per jmp
    "=== [JMP 1] ===\n";
    body=jit_rpnBodyStart(jit);
    off=JIT_ALLOCAI(jit,8);
    for(r=0;r!=20;r++) {
        JIT_MOV(jit);
        JIT_R(jit,r);
        JIT_I64(jit,r);
    }
    jit_rpnIf(jit);
    JIT_R(jit,0);
    //
    jmp=JIT_JMP(jit,NULL);
    // "NULL" false case
    JIT_I64(jit,-1);
    for(r=19;r>=0;r--) {
        JIT_MOV(jit);
        JIT_R(jit,r);
        JIT_I64(jit,r);
    }
    //Test swapping register mapping
    JIT_PATCH(jit,jmp,JIT_LABEL(jit));
    JIT_MOV(jit);
    JIT_R(jit,19);
    for(r=0;r!=19;r++)
        JIT_ADDI(jit);
    for(r=0;r!=20;r++)
        JIT_R(jit,r);
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    JIT_FREE(jit);
}
JmpTest1;
U0 JTabTests1() {
    JIT *jit;
    CRPN *body,*j1,*j2,*j3,*d;
    I64 off,r;
    jit=jit_Init;
    body=jit_rpnBodyStart(jit);
    JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_I64(jit,1);
    j1=JIT_JMP_TABLE(jit,3);
    JIT_R(jit,0);
    JIT_RETI(jit);
    JIT_I64(jit,0);
    //
    j1->jmp_table[0]=JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_ADDI(jit);
    JIT_R(jit,0);
    JIT_R(jit,1);
    //
    j1->jmp_table[1]=JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_MULI(jit);
    JIT_R(jit,0);
    JIT_R(jit,2);
    //
    j1->jmp_table[2]=JIT_MOV(jit);
    JIT_R(jit,0);
    JIT_SUBI(jit);
    JIT_R(jit,0);
    JIT_R(jit,3);
    jit_rpnBodyEnd(jit,body);
    JitEmitCode(jit,0);
    JitDumpBytes(jit);
    DumpRPN(FirstRPN(jit->cur_rpn_start));
    JIT_FREE(jit);
}
JTabTests1;
#endif
/*
//Count to 10
jit=jit_Init;
CRPN *body0=jit_rpnBodyStart(jit);
I64 frame=JIT_ALLOCAI(jit,16);
JIT_FMOV(jit);
JIT_FFRAME(jit,0);
JIT_F64(jit,0.0);
JIT_FMOV(jit);
JIT_FFRAME(jit,8);
JIT_F64(jit,1.0);
CRPN *if_op=JIT_LABEL(jit);
jit_rpnIf(jit);
JIT_FLT(jit);
JIT_F64(jit,10.0);
JIT_FFRAME(jit,0);
CRPN *body=jit_rpnBodyStart(jit);
JIT_FMOV(jit);
JIT_FFRAME(jit,0);
JIT_FADD(jit);
JIT_FADD(jit);
JIT_FFRAME(jit,0);
JIT_F64(jit,101);
JIT_FADD(jit);
JIT_FFRAME(jit,8);
JIT_F64(jit,102);
JIT_JMP(jit,if_op);
jit_rpnBodyEnd(jit,body);
body=jit_rpnBodyStart(jit);
JIT_RET(jit);
JIT_I64(jit,0);
jit_rpnBodyEnd(jit,body);
jit_rpnBodyEnd(jit,body0);
"==== [Count to 10] ====\n";
"%s\n",JitEmitCode(jit,0);
*/



















































































