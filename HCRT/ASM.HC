U8 *SkipComment(U8 *text) {
    loop:
    U8 *orig=text;
    if(0==StrNCmp(text,"/*",2)) {
        text=StrMatch(text+2,"*/")+2;
    } else if(0==StrNCmp(text,"//",2)) {
        text=StrFirstOcc(text,"\n")+1;
    }
    while(Bt(&char_bmp_white_space,*text)) text++;
    if(orig!=text) goto loop;
    return text;
}
#define SREG 0
#define GP_8 1
#define GP_16 2
#define GP_32 3
#define GP_64 4
#define X87 5
#define MM 6
#define XMM 7
class CReg:CHash {
    I64 code,type;
};
CHashTable *regs=HashTableNew(64);
CHashTable *opcodes=HashTableNew(1<<9);
#define OPC_F_SR 1 // /R
#define OPC_F_ADDR (1<<1) //+R
#define OPC_F_IB (1<<3)
#define OPC_F_IW (1<<4)
#define OPC_F_ID (1<<5)
#define OPC_F_IQ (1<<6)
#define OPC_F_O0 (1<<7)
#define OPC_F_O1 (1<<8)
#define OPC_F_O2 (1<<9)
#define OPC_F_O3 (1<<10)
#define OPC_F_O4 (1<<11)
#define OPC_F_O5 (1<<12)
#define OPC_F_O6 (1<<13)
#define OPC_F_O7 (1<<14)
#define OPC_F_32bit (1<<15)
#define OPC_F_48REX (1<<16)
#define OPC_F_REX_R8_15ONLY (1<<17)
#define OPC_F_STI (1<<18)
#define OPC_F_ADDI (1<<19)
//Operand size prefix
#define OPC_F_16 (1<<20)
#define OPC_F_COND (1<<21)
class COpcodeArg {
    I64 type;
    #define OPCA_NONE 0
    #define OPCA_REG 1
    #define OPCA_R8 2
    #define OPCA_R16 3
    #define OPCA_R32 4
    #define OPCA_R64 5
    #define OPCA_RM8 6
    #define OPCA_RM16 7
    #define OPCA_RM32 8
    #define OPCA_RM64 9
    #define OPCA_IMM8 10
    #define OPCA_IMM16 11
    #define OPCA_IMM32 12
    #define OPCA_IMM64 13
    #define OPCA_MOFFS8 14
    #define OPCA_MOFFS16 15
    #define OPCA_MOFFS32 16
    #define OPCA_UIMM8 17
    #define OPCA_UIMM16 18
    #define OPCA_UIMM32 19
    #define OPCA_UIMM64 20
    #define OPCA_REL8 21
    #define OPCA_REL16 22
    #define OPCA_REL32 23
    #define OPCA_M8 24
    #define OPCA_M16 25
    #define OPCA_M32 26
    #define OPCA_M64 27
    #define OPCA_STI 28
    #define OPCA_SREG 29
    union {
        CReg *reg;
    };
};
#define HASH_TYPE_OPCODE 0
#define HASH_TYPE_OPCODE_ALIAS 1
class COpcode:CHash {
    COpcode *prev_op,*next_op;
    U8 bytes[16];
    I64 bcnt;
    I64 flags,arg_count;
    COpcodeArg args[3];
};
class COpcodeAlias:CHash {
    COpcode *opcode;
};
U0 ParseOpcodes(U8 *text) {
    CReg *newreg;
    I64 len,rtype;
    CHashTable *targettab;
    loop:
    text=SkipComment(text);
    if(!*text) goto en;
    else if(0==StrNCmp(text,"FSTK",4)) {
        rtype=X87;len=4;
        goto addreg;
    } else if(0==StrNCmp(text,"SEG",3)) {
        len=3,rtype=SREG;
        goto addreg;
    } else if(0==StrNCmp(text,"R8",len=StrLen("R8"))) {
        rtype=GP_8;
        addreg:
        newreg=MAlloc(sizeof(CReg));
        U8 *rn=MAlloc(8);
        I64 rc;
        text=StrScan(text+len," %s %d;",&rn,&rc);
        newreg->type=rtype,newreg->str=rn,newreg->code=rc;
        HashAdd(newreg,regs);
    } else if(!StrNCmp(text,"R16",len=StrLen("R16"))) {
        rtype=GP_16;
        goto addreg;
    } else if(!StrNCmp(text,"R32",len=StrLen("R32"))) {
        rtype=GP_32;
        goto addreg;
    } else if(!StrNCmp(text,"R64",len=StrLen("R64"))) {
        rtype=GP_64;
        goto addreg;
    } else if(!StrNCmp(text,"MM",len=StrLen("MM"))) {
        rtype=MM;
        goto addreg;
    } else if(!StrNCmp(text,"XMM",len=StrLen("XMM"))) {
        rtype=XMM;
        goto addreg;
    } else if(!StrNCmp(text,"OPCODE",len=StrLen("OPCODE"))) {
        text=SkipComment(text+len);
        U8 *name=MAlloc(16);
        I64 off=0;
        while(Bt(&char_bmp_alpha_numeric,*text)) name[off++]=*text++;
        text=SkipComment(text);
        COpcode *prev=NULL;
        oloop:
        if(*text==';') {
            text++;
            goto addop;
        }
        COpcode *opc=MAlloc(sizeof(COpcode));
        opc->type=HASH_TYPE_OPCODE;
        opc->str=StrNew(name);
        for(off=0;;off++) {
            if(off>10) Debugger;
            text=SkipComment(text);
            if(*text==';'||*text==':') {
                opc->bcnt=off;
                goto term;
            }
            if(*text==',') {
                opc->bcnt=off;
                text++;
                break;
            }
            U8 *orig="";
            opc->bytes[off]=Str2I64(text,16,&text);
        }

        I64 argument=0;
        attrloop:
        term:
        text=SkipComment(text);
        Bool has_aliases=FALSE;
        if(*text==';') {
            text++;
            goto addop;
        } else if(*text==':') {
            has_aliases=TRUE;
            text++;
            goto addop;
        }
        if(0==StrNCmp("0x",text,2)) goto oen;
        off=0;
        while(Bt(&char_bmp_alpha_numeric,*text)) text++,off++;
        if(off) {
            U8 *name2=MAlloc(off+1);
            StrNCpy(name2,text-off,off);
            CReg *areg=HashFind(name2,regs);
            if(areg&&StrCmp(name2,"R8")) { //Ignore R8 as it could mean an 8-bit register
                opc->args[argument].type=OPCA_REG;
                opc->args[argument].reg=areg;
                argument++,Free(name2);
                goto attrloop;
            } else if(!StrCmp("SREG",name2)) {
                opc.args[argument++].type=OPCA_SREG,Free(name2);
                goto attrloop;
            } else if(!StrCmp("R8",name2)) {
                opc.args[argument].type=OPCA_R8;
                argument++,Free(name2);
                goto attrloop;
            } else if(!StrCmp("R16",name2)) {
                opc.args[argument].type=OPCA_R16;
                argument++,Free(name2);
                goto attrloop;
            } else if(!StrCmp("R32",name2)) {
                opc.args[argument++].type=OPCA_R32,Free(name2);
                goto attrloop;
            } else if(!StrCmp("R64",name2)) {
                opc.args[argument++].type=OPCA_R64,Free(name2);
                goto attrloop;
            } else if(!StrCmp("IMM8",name2)) {
                opc.args[argument++].type=OPCA_IMM8,Free(name2);
                opc->flags|=OPC_F_IB;
                goto attrloop;
            } else if(!StrCmp("IMM16",name2)) {
                opc.args[argument++].type=OPCA_IMM16,Free(name2);
                opc->flags|=OPC_F_IW;
                goto attrloop;
            } else if(!StrCmp("IMM32",name2)) {
                opc.args[argument++].type=OPCA_IMM32,Free(name2);
                opc->flags|=OPC_F_ID;
                goto attrloop;
            } else if(!StrCmp("IMM64",name2)) {
                opc.args[argument++].type=OPCA_IMM64,Free(name2);
                opc->flags|=OPC_F_IQ;
                goto attrloop;
            } else if(!StrCmp("RM8",name2)) {
                opc.args[argument++].type=OPCA_RM8,Free(name2);
                goto attrloop;
            } else if(!StrCmp("RM16",name2)) {
                opc.args[argument++].type=OPCA_RM16,Free(name2);
                goto attrloop;
            } else if(!StrCmp("RM32",name2)) {
                opc.args[argument++].type=OPCA_RM32,Free(name2);
                goto attrloop;
            } else if(!StrCmp("RM64",name2)) {
                opc.args[argument++].type=OPCA_RM64,Free(name2);
                goto attrloop;
            } else if(!StrCmp("IB",name2)) {
                opc.flags|=OPC_F_IB,Free(name2);
                goto attrloop;
            } else if(!StrCmp("IW",name2)) {
                opc.flags|=OPC_F_IW,Free(name2);
                goto attrloop;
            } else if(!StrCmp("ID",name2)) {
                opc.flags|=OPC_F_ID,Free(name2);
                goto attrloop;
            } else if(!StrCmp("IQ",name2)) {
                opc.flags|=OPC_F_IQ,Free(name2);
                goto attrloop;
            } else if(!StrCmp("MOFFS8",name2)) {
                opc.args[argument++].type=OPCA_MOFFS8,Free(name2);
                goto attrloop;
            } else if(!StrCmp("MOFFS16",name2)) {
                opc.args[argument++].type=OPCA_MOFFS16,Free(name2);
                goto attrloop;
            } else if(!StrCmp("MOFFS32",name2)) {
                opc.args[argument++].type=OPCA_MOFFS32,Free(name2);
                goto attrloop;
            } else if(!StrCmp("16",name2)) {
                opc->flags|=OPC_F_16;
                Free(name2);
                goto attrloop;
            } else if(!StrCmp("32",name2)) {
                //
                Free(name2);
                goto attrloop;
            } else if(!StrCmp("UIMM8",name2)) {
                opc.args[argument++].type=OPCA_UIMM8,Free(name2);
                opc->flags|=OPC_F_IB;
                goto attrloop;
            } else if(!StrCmp("UIMM16",name2)) {
                opc.args[argument++].type=OPCA_UIMM16,Free(name2);
                opc->flags|=OPC_F_IW;
                goto attrloop;
            } else if(!StrCmp("UIMM32",name2)) {
                opc.args[argument++].type=OPCA_UIMM32,Free(name2);
                opc->flags|=OPC_F_ID;
                goto attrloop;
            } else if(!StrCmp("UIMM64",name2)) {
                opc.args[argument++].type=OPCA_UIMM64,Free(name2);
                opc->flags|=OPC_F_IQ;
                goto attrloop;
            } else if(!StrCmp("REL8",name2)) {
                opc.args[argument++].type=OPCA_REL8,Free(name2);
                goto attrloop;
            } else if(!StrCmp("REL16",name2)) {
                opc.args[argument++].type=OPCA_REL16,Free(name2);
                goto attrloop;
            } else if(!StrCmp("REL32",name2)) {
                opc.args[argument++].type=OPCA_REL32,Free(name2);
                goto attrloop;
            } else if(!StrCmp("CB",name2)) {
                opc.flags|=OPC_F_COND,Free(name2);
                goto attrloop;
            } else if(!StrCmp("CW",name2)) {
                opc.flags|=OPC_F_COND,Free(name2);
                goto attrloop;
            } else if(!StrCmp("CD",name2)) {
                opc.flags|=OPC_F_COND,Free(name2);
                goto attrloop;
            } else if(!StrCmp("M8",name2)) {
                opc.args[argument++].type=OPCA_M8,Free(name2);
                goto attrloop;
            } else if(!StrCmp("M16",name2)) {
                opc.args[argument++].type=OPCA_M16,Free(name2);
                goto attrloop;
            } else if(!StrCmp("M32",name2)) {
                opc.args[argument++].type=OPCA_M32,Free(name2);
                goto attrloop;
            } else if(!StrCmp("M64",name2)) {
                opc.args[argument++].type=OPCA_M64,Free(name2);
                goto attrloop;
            } else if(!StrCmp("STI",name2)) {
                opc.args[argument++].type=OPCA_STI,Free(name2);
                goto attrloop;
            } else {
                Debugger;
                "%s\n",text;
                throw('Asm');
            }
        } else if(*text=='!') {
            //??? Dont switch modes
            text++;
            goto attrloop;
        } else if(*text=='&') {
            ///??? DFT
            text++;
            goto attrloop;
        } else if(*text=='%') {
            text++,opc.flags|=OPC_F_32bit;
            goto attrloop;
        } else if(*text=='=') {
            text++,opc.flags|=OPC_F_48REX;
            goto attrloop;
        } else if(*text=='`') {
            text++,opc.flags|=OPC_F_REX_R8_15ONLY;
            goto attrloop;
        } else if(*text=='^') {
            text++; //REX_XOR_LILKE ???
            goto attrloop;
        } else if(*text=='*') {
            text++,opc->flags|=OPC_F_STI;
            goto attrloop;
        } else if(*text=='$') {
            //ENDING 0???
            text++;
            goto attrloop;
        } else if(!StrNCmp(text,"+R",len=2)) {
            opc->flags|=OPC_F_ADDR,text+=len;
            goto attrloop;
        } else if(!StrNCmp(text,"/R",len=2)) {
            opc->flags|=OPC_F_SR,text+=len;
            goto attrloop;
        } else if(*text=='/') {
            I64 oct=Str2I64(text+1,8,&text);
            switch(oct) {
                case 0: opc->flags|=OPC_F_O0; break;
                case 1: opc->flags|=OPC_F_O1; break;
                case 2: opc->flags|=OPC_F_O2; break;
                case 3: opc->flags|=OPC_F_O3; break;
                case 4: opc->flags|=OPC_F_O4; break;
                case 5: opc->flags|=OPC_F_O5; break;
                case 6: opc->flags|=OPC_F_O6; break;
                case 7: opc->flags|=OPC_F_O7; break;
            }
            goto attrloop;
        } if(0==StrNCmp(text,"+I",2)) {
            opc->flags|=OPC_F_ADDI;
            text+=2;
            goto attrloop;
        } else {
            Debugger;
            throw('asm');
        }
        oen:
        if(prev) {
            opc->prev_op=prev;
            prev->next_op=opc;
        }
        prev=opc;
        goto oloop;
        addop:
        if(prev) {
            opc->prev_op=prev;
            prev->next_op=opc;
        }
        while(opc) {
            prev=opc;opc=opc->prev_op;
        }
        HashAdd(prev,opcodes);
        if(has_aliases) {
            aloop:
            text=SkipComment(text);
            if(*text==';') {
                text++;
                goto loop;
            }
            off=0;
            while(Bt(char_bmp_alpha_numeric,*text)) text++,off++;
            COpcodeAlias *alias=MAlloc(sizeof(COpcodeAlias));
            alias->str=StrNCpy(MAlloc(off+1),text-off,off);
            alias->type=HASH_TYPE_OPCODE_ALIAS;
            alias->opcode=prev;
            HashAdd(alias,opcodes);
            goto aloop;
        }
    } else {
        Debugger;
        "%s\n",text;
        throw('Asm');
    }
    goto loop;
    en:
}
I64 cnt;
U8 *ops=FileRead("../Opcodes.DD",&cnt);
"PARSING OPCODES\n";
ParseOpcodes(ops);
"DONE\n";
Free(ops);

// See ext/x64.h
Bool __X64RegIsIntExt(I64 code) {
    return code>0b111;
}
#define X64ModRMIndir 0b00
#define X64ModRMIndir8 0b01
#define X64ModRMIndir32 0b10
#define X64ModRMDirect 0b11
I64 X64ImmSize(U64 imm) {
    if(imm<=0xff) {
        return 1;
    } else if(imm<=0xffff) {
        return 2;
    } else if(imm<=0xffFFffFF) {
        return 4;
    }
    return 8;
}
U8 X64Rex(U8 w,U8 r,U8 x,U8 b) {
    U8 wrxb=(w<<3)|(r<<2)|(x<<1)|(b);
    if(wrxb) return (0b0100<<4)|wrxb;
    return 0;
}
class X64Mode {
    I64 type;
    #define X64O_REG 0
    #define X64O_MEM 1
    #define X64O_IMM 2
    #define X64O_LABEL 3
    union {
        CReg *reg;
        class {
            CReg *base;
            CReg *index;
            I64 scale;
            I32 disp;
            I64 width;
        } mem;
        class {
            U0 *jit_label;
            I64 label_offset;
        };
        U64 imm;
    };
};
CReg *Str2Reg(U8 *str) {
    return HashFind(str,regs);
}
X64Mode *X64ModeLabel(U0 *lab) {
    X64Mode *ret=MAlloc(sizeof(X64Mode));
    ret->type=X64O_LABEL;
    ret->jit_label=lab;
    return ret;
}
X64Mode *X64ModeReg(CReg *reg) {
    X64Mode *ret=MAlloc(sizeof(X64Mode));
    ret->type=X64O_REG;
    ret->reg=reg;
    return ret;
}
U8 *AsmError=NULL;
X64Mode *X64ModeMem(I64 scale,CReg *index,CReg *base,I64 disp,I64 width=8) {
    X64Mode *ret=MAlloc(sizeof(X64Mode));
    ret->type=X64O_MEM;
    ret->mem.base=base;
    ret->mem.index=index;
    ret->mem.scale=scale;
    ret->mem.disp=disp;
    ret->mem.width=width;
    return ret;
}
X64Mode *X64ModeImm(U64 imm) {
    X64Mode *ret=MAlloc(sizeof(X64Mode));
    ret->type=X64O_IMM;
    ret->imm=imm;
    return ret;
}
U8 X64ModRM(I64 modrmMode,I64 r, I64 rm) {
    return  modrmMode<<6|((r&7)<<3)|(rm&7);
}
U8 X64SIB(I64 s,I64 i,I64 b) {
    switch(s) {
        case 0: break; //RSP ???
        case 1: s=0; break;
        case 2: s=1; break;
        case 4: s=2; break;
        case 8: s=3; break;
	  AsmError="Invalid SIB scale";
        default: throw('asm');
    }
    return (s<<6)|((i&7)<<3)|(b&7);
}
Bool IsOpcode(U8 *name) {
  return !!HashFind(name,opcodes);
}
CReg *GetRegister(U8 *name) {
  return HashFind(name,regs);
}
U8 *X86ModRMSIBDisp(U8 *it,I64 size,U8 *opcode,I64 opc_len,CReg *reg,I64 scale,CReg *index,CReg *base,I64 disp,I64 *sibooff=NULL) {
  U8 *orig_it=it;
  if(sibooff) *sibooff=-1;
  I64 mdrmcode=-1;
    if(reg)
        I64 mdrmreg=reg->code;
    else
        mdrmreg=0;
    if(base)
        I64 mdrmrm=base->code;
    else
        mdrmrm=0;
    Bool SIB=FALSE;
    I64 sibi=-1;
    I64 sibb=-1;
    I64 sibs=0,byte;
    U8 rex=0;
    U8 disps=0;
    if(!disp) {
        disps=0;
        mdrmcode=X64ModRMIndir;
    } else if(I8_MIN<=disp<=I8_MAX) {
        disps=1;
        mdrmcode=X64ModRMIndir8;
    } else {
        disps=4;
        mdrmcode=X64ModRMIndir32;
    }
    if(base==HashFind("RIP",regs)) {
      if(scale||index) {
	AsmError="RIP addressing must not have a scale.";
	throw('asm');
      }
        mdrmcode=X64ModRMIndir;
        mdrmrm=HashFind("RBP",regs)(CReg*)->code;
        disps=4;
    } else if(!index) {
        if(scale) throw('asm');
        sibi=HashFind("RSP",regs)(CReg*)->code;
        if(!base) {
            SIB=TRUE;
            mdrmrm=HashFind("RSP",regs)(CReg*)->code;
            sibb=HashFind("RBP",regs)(CReg*)->code;
            mdrmcode=X64ModRMIndir;
            disps=4;
        } else if(base==HashFind("RBP",regs)||base==HashFind("R13",regs)) {
            if(mdrmcode==X64ModRMIndir) {
                if(disp) throw('asm');
                disps=1;
                mdrmcode=X64ModRMIndir8;
            }
        } else if(base==HashFind("RSP",regs)||base==HashFind("R12",regs)) {
            SIB=TRUE;
            mdrmrm=HashFind("RSP",regs)(CReg*).code;
            sibb=base->code;
        } else {
            //Base only,no index
            I64 rrc=0;
            if(reg) rrc=reg->code;
            rex=X64Rex(
                size==8,
                rrc>>3,
                0,
                __X64RegIsIntExt(base->code)
            );
            if(rex) *it++=rex;
            for(byte=0;byte!=opc_len;byte++) *it++=opcode[byte];
            *it++=X64ModRM(mdrmcode,rrc,base->code);
            goto insdisp;
            if(SIB) throw('asm');
            if(!mdrmrm) throw('asm');
        }
    } else if(index==HashFind("RSP",regs)) {
        if(scale!=1) throw('asm');
        SIB=TRUE;
        mdrmrm=HashFind("RSP",regs)(CReg*).code;
        sibb=HashFind("RSP",regs)(CReg*).code;
        if(!base) {
            sibi=HashFind("RSP",regs)(CReg*).code;
        } else {
            sibi=base->code;
        }
    } else {
        SIB=TRUE;
        mdrmrm=HashFind("RSP",regs)(CReg*).code;
        sibi=index->code;
        sibs=scale;
        if(base) sibb=base->code; else sibb=0;
        if(!base) {
            sibb=HashFind("RBP",regs)(CReg*).code;
            mdrmcode=X64ModRMIndir;
            disps=4;
        } else if(base==HashFind("RBP",regs)||base==HashFind("R13",regs)) {
            if(mdrmcode==X64ModRMIndir) {
                disps=1;
                mdrmcode=X64ModRMIndir8;
            }
        }
    }
    I64 tmp=__X64RegIsIntExt(sibb);
    if(!SIB) tmp=__X64RegIsIntExt(mdrmrm);
    I64 treg=0;
    if(reg) treg=reg->code;
    rex=X64Rex(
        size==8,
        __X64RegIsIntExt(treg),
        SIB&&__X64RegIsIntExt(sibi),
        tmp
    );
    if(rex) *it++=rex;
    byte=0;
    for(;byte!=opc_len;byte++) *it++=opcode[byte];
    *it++=X64ModRM(mdrmcode,mdrmreg,mdrmrm);
    if(SIB) *it++=X64SIB(sibs,sibi,sibb);
    insdisp:
    if(sibooff&&disps==4) *sibooff=it-orig_it;
    switch(disps) {
        case 0: break;
        case 1:
        *it++=disp;
        break;
        case 2:
        it(U16*)[0]=disp;
        it+=2;
        break;
        case 4:
        it(U32*)[0]=disp;
        it+=4;
        break;
	AsmError="Displacement is too big.";
        default: throw('asm');
    }
    return it;
}
U8 buffer[16];
U0 SibTests() {
    U8 *RegArr[]={
        "RAX",
        "RCX",
        "RDX",
        "RBX",
        "RSP",
        "RBP",
        "RSI",
        "RDI",
        "R8",
        "R9",
        "R10",
        "R11",
        "R12",
        "R13",
        "R14",
        "R15"
    };
    I64 base=0,len=sizeof(RegArr)/sizeof(*RegArr);
    U8 buffer[16];
    //Seems to work
    for(;base!=len;base++) {
        I64 index=0;
        for(;index!=len;index++) {
            if(!StrCmp("RSP",RegArr[index])) goto next;
            U8 *bend=X86ModRMSIBDisp(buffer,4,"AA",2,
                HashFind("RAX",regs),
                4,
                HashFind(RegArr[index],regs),
                HashFind(RegArr[base],regs),
                0x7f
            );
            "[4*%s+%s]\n",RegArr[index],RegArr[base];
            U8 *bstart=buffer;
            for(;bstart!=bend;bstart++)
            "%p\n",*bstart;
            next:
        }
    }
    //Seems to work
    for(base=0;base!=len;base++) {
        bend=X86ModRMSIBDisp(buffer,0,"AA",2,
            HashFind("RAX",regs),
            0,
            NULL,
            HashFind(RegArr[base],regs),
            0x7f
        );
        "[%s+0xff]\n",RegArr[base];
        for(bstart=buffer;bstart!=bend;bstart++)
        "%p\n",*bstart;
    }
    //Seems to work.
    for(index=0;index!=len;index++) {
        if(!StrCmp(RegArr[index],"RSP")) goto next2;
        bend=X86ModRMSIBDisp(buffer,4,"AA",2,
            HashFind("RAX",regs),
            4,
            HashFind(RegArr[index],regs),
            NULL,
            0x7f
        );
        "[4*%s+0x7f]",RegArr[index];
        for(bstart=buffer;bstart!=bend;bstart++)
        "%p\n",*bstart;
        next2:
    }
}
U0 X64ModeFree(X64Mode *m) {
    Free(m);
}
/**
 * Jumps are not encoded here
 */
U8 *EncodeOpcode(U8 *ip,U8 *name,Bool *is_jmp,I64 *sibooff ,I64 *immoff,...) {
  if(sibooff) *sibooff=-1;
  if(immoff) *immoff=-1;
  U8 *origip=ip;
    if(is_jmp) *is_jmp=FALSE;
    U8 buffer[32];
    Bool ret_after_encode=FALSE;
    X64Mode **modes=argv(U0*);
    COpcode *opc=HashFind(name,opcodes);
    if(opc->type==HASH_TYPE_OPCODE_ALIAS)
        opc=opc(COpcodeAlias*)->opcode;
    if(!opc) throw('InvOpc');
    I64 opc_cnt=0;
    while(opc)
        opc_cnt++,opc=opc->next_op;
    opc=HashFind(name,opcodes);
    if(opc->type==HASH_TYPE_OPCODE_ALIAS)
        opc=opc(COpcodeAlias*)->opcode;
    I64 *sizes=MAlloc(sizeof(I64)*opc_cnt);
    I64 item=0;
    loop:
    X64Mode *mem=NULL;
    I64 passed=1;
    I64 arg=0;
    for(;arg!=3;arg++) {
        switch(opc->args[arg].type) {
            case OPCA_NONE: if(arg<argc) goto fail;
            goto pass;
            case OPCA_REG:
                if(modes[arg]->type==X64O_REG)
                    if(modes[arg]->reg==opc->args[arg].reg)
                        goto pass;
                goto fail;
            case OPCA_R8:
            r8:
            if(modes[arg]->type==X64O_REG)
                if(modes[arg]->reg->type==GP_8)
                    goto pass;
            goto fail;
            case OPCA_R16:
            r16:
            if(modes[arg]->type==X64O_REG)
                if(modes[arg]->reg->type==GP_16)
                    goto pass;
            goto fail;
            case OPCA_R32:
            r32:
            if(modes[arg]->type==X64O_REG)
                if(modes[arg]->reg->type==GP_32)
                    goto pass;
            goto fail;
            case OPCA_R64:
            r64:
            if(modes[arg]->type==X64O_REG)
                if(modes[arg]->reg->type==GP_64)
                    goto pass;
            goto fail;
            case OPCA_RM8:
            mem=modes[arg];
            if(modes[arg]->type==X64O_MEM)
                if(modes[arg]->mem.width==1)
                    goto pass;
            goto r8;
            case OPCA_RM16:
            mem=modes[arg];
            if(modes[arg]->type==X64O_MEM)
                if(modes[arg]->mem.width==2)
                    goto pass;
            goto r16;
            case OPCA_RM32:
            mem=modes[arg];
            if(modes[arg]->type==X64O_MEM)
                if(modes[arg]->mem.width==4)
                    goto pass;
            goto r32;
            case OPCA_RM64:
            mem=modes[arg];
            if(modes[arg]->type==X64O_MEM)
                if(modes[arg]->mem.width==8)
                    goto pass;
            goto r64;
            case OPCA_IMM8:
            if(modes[arg]->type==X64O_IMM)
                if(X64ImmSize(modes[arg]->imm)<=1)
                    goto pass;
            goto fail;
            case OPCA_IMM16:
            if(modes[arg]->type==X64O_IMM)
                if(X64ImmSize(modes[arg]->imm)<=2)
                    goto pass;
            goto fail;
            case OPCA_IMM32:
            if(modes[arg]->type==X64O_IMM)
                if(X64ImmSize(modes[arg]->imm)<=4)
                    goto pass;
            goto fail;
            case OPCA_IMM64:
            if(modes[arg]->type=X64O_IMM)
                if(X64ImmSize(modes[arg]->imm)<=8)
                    goto pass;
            goto fail;
            case OPCA_UIMM8:
            if(modes[arg]->type==X64O_IMM)
                if(modes[arg]->imm<=U8_MAX)
                    goto pass;
            goto fail;
            case OPCA_UIMM16:
            if(modes[arg]->type==X64O_IMM)
                if(modes[arg]->imm<=U16_MAX)
                    goto pass;
            goto fail;
            case OPCA_UIMM32:
            if(modes[arg]->type==X64O_IMM)
                if(modes[arg]->imm<=U32_MAX)
                    goto pass;
            goto fail;
            case OPCA_UIMM64:
            if(modes[arg]->type==X64O_IMM)
                if(modes[arg]->imm<=U64_MAX)
                    goto pass;
            goto fail;
            case OPCA_SREG:
            if(modes[arg]->type==X64O_REG)
                if(modes[arg]->reg->type==SREG)
                    goto pass;
            goto fail;
            case OPCA_MOFFS8:
            case OPCA_MOFFS16:
            case OPCA_MOFFS32:
            if(modes[arg]->type==X64O_MEM) {
                if(modes[arg]->mem.index==NULL&&modes[arg]->mem.base=NULL)
                    goto pass;
            }
            goto fail;
            case OPCA_REL16:
            case OPCA_REL8:
            //32bit jumps are the best
            //Here is a picture of a foofie,(it is yellow btw).
            //   /\      /\
            //   ||______||
            //   |        |
            //   |  0  0  |
            //   |   &&   |
            //   |  \__/  |
            //   | ______ |
            //   ||      ||
            //   \/      \/
            //
            //
            goto fail;
            case OPCA_REL32:
	    mem=modes[arg];
	    if(mem->type==X64O_LABEL)
		if(opc->flags&OPC_F_COND)
			goto pass;
	    if(mem->type==X64O_IMM)
	      	if(opc->flags&OPC_F_COND)
			goto pass;
            goto fail;
            case OPCA_M8:
            mem=modes[arg];
            if(modes[arg]->type==X64O_MEM)
                if(modes[arg]->mem.width==1)
                    goto pass;
            goto fail;
            case OPCA_M16:
            mem=modes[arg];
            if(modes[arg]->type==X64O_MEM)
                if(modes[arg]->mem.width==2)
                    goto pass;
            goto fail;
            case OPCA_M32:
            mem=modes[arg];
            if(modes[arg]->type==X64O_MEM)
                if(modes[arg]->mem.width==4)
                    goto pass;
            goto fail;
            case OPCA_M64:
            mem=modes[arg];
            if(modes[arg]->type==X64O_MEM)
                if(modes[arg]->mem.width==8)
                    goto pass;
            goto fail;
            case OPCA_STI:
            if(modes[arg]->type==X64O_REG)
                if(modes[arg]->reg.type==X87)
                    goto pass;
            goto fail;
            default:
            throw('???');
        }
        fail:;
        sizes[item]=-1;
        passed=0;
        goto nextop;
        pass:
    }
    if(!passed) goto nextop;
    U8 *endptr=buffer;
    encode:
    I64 op_sz=0;
    //Check for REL32
    if(is_jmp)
    for(arg=0;arg!=argc;arg++) {
      if(opc->args[arg].type==OPCA_REL32) *is_jmp=TRUE;
    }
    mem=NULL;
    for(arg=0;arg!=argc;arg++) {
        switch(opc->args[arg].type) {
            start:
            case OPCA_RM8:
            case OPCA_M8:
            op_sz=1;
            break;
	    case OPCA_RM16:
            case OPCA_M16:
            op_sz=2;
            break;
            case OPCA_RM32:
            case OPCA_M32:
            op_sz=4;
            break;
            case OPCA_RM64:
            case OPCA_M64:
	      rexw=TRUE;
            op_sz=8;
            end:
            mem=modes[arg];
        }
    }
    CReg *mdrmR=NULL;
    I64 add=0;
    I64 immsz=0;
    U64 imm=0x0;
    Bool rexb=FALSE,rex48=FALSE,rexw=FALSE;
    //Detect MOFFS
    for(arg=0;arg!=argc;arg++) {
        switch(opc->args[arg].type) {
            start:
            case OPCA_MOFFS8:
            case OPCA_MOFFS16:
            case OPCA_MOFFS32:
            break;
            end:
            immsz=8;
            imm=modes[arg]->mem.disp;
        }
    }
    if(opc->flags&OPC_F_SR) {
        for(arg=0;arg!=argc;arg++) {
            switch(opc->args[arg].type) {
                start:
                case OPCA_R8:op_sz=1;break;
                case OPCA_R16:op_sz=2;break;
                case OPCA_R32:op_sz=4;break;
	    case OPCA_R64:op_sz=8;rexw=TRUE; break;
                end:
                mdrmR=modes[arg].reg;
                break;
                default:
            }
        }
    }
    if(opc->flags&OPC_F_ADDR||opc->flags&OPC_F_ADDI) {
        for(arg=0;arg!=argc;arg++) {
            switch(opc->args[arg].type) {
                start:
                case OPCA_R8:op_sz=1;break;
                case OPCA_R16:op_sz=2;break;
                case OPCA_R32:op_sz=4;break;
	    case OPCA_R64:op_sz=8; rexw=TRUE;break;
                end:
                add=modes[arg]->reg->code&7;
                if(modes[arg]->reg->code&~7)
                    rexb=TRUE;
                break;
            }
        }
    }
    if(opc->flags&OPC_F_IB) {
        for(arg=0;arg!=argc;arg++) {
            switch(opc->args[arg].type) {
                case OPCA_IMM8:
                case OPCA_UIMM8:
                immsz=1;
                imm=modes[arg]->imm;
            }
        }
    }
    if(opc->flags&OPC_F_IW) {
        for(arg=0;arg!=argc;arg++) {
            switch(opc->args[arg].type) {
                case OPCA_IMM8:
                case OPCA_UIMM8:
                case OPCA_IMM16:
                case OPCA_UIMM16:
                immsz=2,imm=modes[arg]->imm;
            }
        }
    }
    if(opc->flags&OPC_F_ID) {
        for(arg=0;arg!=argc;arg++) {
            switch(opc->args[arg].type) {
                case OPCA_IMM8:
                case OPCA_UIMM8:
                case OPCA_IMM16:
                case OPCA_UIMM16:
                case OPCA_IMM32:
                case OPCA_UIMM32:
                immsz=4,imm=modes[arg]->imm;
            }
        }
    }
    if(opc->flags&OPC_F_IQ) {
        for(arg=0;arg!=argc;arg++) {
            switch(opc->args[arg].type) {
                case OPCA_IMM8:
                case OPCA_UIMM8:
                case OPCA_IMM16:
                case OPCA_UIMM16:
                case OPCA_IMM32:
                case OPCA_UIMM32:
                case OPCA_IMM64:
                case OPCA_UIMM64:
                immsz=8,imm=modes[arg]->imm;
            }
        }
    }
    if(opc->flags&OPC_F_O0) {
        mdrmR=HashFind("EAX",regs);
    } else if(opc->flags&OPC_F_O1) {
        mdrmR=HashFind("ECX",regs);
    } else if(opc->flags&OPC_F_O2) {
        mdrmR=HashFind("EDX",regs);
    } else if(opc->flags&OPC_F_O3) {
        mdrmR=HashFind("EBX",regs);
    } else if(opc->flags&OPC_F_O4) {
        mdrmR=HashFind("ESP",regs);
    } else if(opc->flags&OPC_F_O5) {
        mdrmR=HashFind("EBP",regs);
    } else if(opc->flags&OPC_F_O6) {
        mdrmR=HashFind("ESI",regs);
    } else if(opc->flags&OPC_F_O7) {
        mdrmR=HashFind("EDI",regs);
    }
    if(opc->flags&OPC_F_32bit) {
      AsmError="Instruction is 32bit only.";
      throw('asm');
    }
    if(opc->flags&OPC_F_48REX) {
        rex48=TRUE;
    }
    /*
    if(opc->flags&OPC_F_R8_15ONLY) {
        if(mdrmR==HashFind("R8",regs)||mdrmR==HashFind("R15",regs))
        ;
        else {
            sizes[item]=-1;
            goto nextop;
        }
    }
    */
    U8 opcadd[16];
    MemNCpy(opcadd,opc->bytes,opc->bcnt);
    opcadd[opc->bcnt-1]+=add;
    //Operand size
    if(opc->flags&OPC_F_16) *endptr++=0x66;
    //Address size
    //TODO
    if(rex48) {
        *endptr++=0x48;
    } else if(rexb||rexw&&!(mem||mdrmR)) {
      *endptr++=X64Rex(rexw,0,0,rexb);
    }
    if(mem||mdrmR) {
      if(mem->type==X64O_REG) {
	rexb=0!=mem->reg->code&0b1000;
	rexw=mem->reg->type==GP_64;
	Bool rexr=FALSE;
	if(mdrmR) rexr=0!=mdrmR->code&0b1000; 
	if(X64Rex(rexw,rexr,0,rexb)) *endptr++=X64Rex(rexw,rexr,0,rexb);
	I64 rmasked=0;
	if(mdrmR) rmasked=mdrmR->code&7;
	MemNCpy(endptr,opcadd,opc->bcnt);
	endptr+=opc->bcnt;
	*endptr++=X64ModRM(0b11, rmasked,mem->reg->code&7);
      } else {
        I64 sibs=0;
        if(mem) sibs=mem->mem.scale;
        CReg *sibi=NULL,*sibb=NULL;
        if(mem) sibi=mem->mem.index;
        if(mem) sibb=mem->mem.base;
        I64 sibo=0;
        if(mem) sibo=mem->mem.disp;
	U8 *origendptr=endptr;
	I64 _sibooff;
        endptr=X86ModRMSIBDisp(
			       endptr,
			       op_sz,
			       opcadd,
			       opc->bcnt,
			       mdrmR,
			       sibs,
			       sibi,
			       sibb,
			       sibo,
			       &_sibooff
        );
	if(sibooff&&_sibooff!=-1) *sibooff=origendptr-origip;
      }
    } else {
        MemNCpy(endptr,opcadd,opc->bcnt);
        endptr+=opc->bcnt;
    }
    if(immsz) {
        if(ret_after_encode) if(immoff&&immsz==4) *immoff=endptr-origip;
        switch(immsz) {
            case 1:
            *endptr++=imm;
            break;
            case 2:
            endptr(U16*)[0]=imm;
            endptr+=2;
            break;
            case 4:
            endptr(U32*)[0]=imm;
            endptr+=4;
            break;
            case 8:
            endptr(U64*)[0]=imm;
            endptr+=8;
            break;
        }
    }
    if(ret_after_encode) {
	return endptr;
    }
    sizes[item]=endptr-buffer;
    nextop:
    opc=opc->next_op;
    if(opc) {
        item++;
        goto loop;
    }
    item++;
    opc=HashFind(name,opcodes);
    if(opc->type==HASH_TYPE_OPCODE_ALIAS)
        opc=opc(COpcodeAlias*)->opcode;
    COpcode *finalop=NULL;
    I64 iter=0,min=-1;
    for(;iter!=item;iter++,opc=opc->next_op) {
        if(sizes[iter]!=-1) {
            if(min==-1) goto accept;
            if(sizes[iter]<sizes[min]) {
                accept:
                min=iter;
                finalop=opc;
            }
        }
    }
    if(min==-1) {
      AsmError="Invalid opcode usage.";
      throw('asm');
    }
    Free(sizes);
    if(ip) {
        opc=finalop;
        endptr=ip;
        ret_after_encode=TRUE;
        goto encode;
    }
    return NULL;
}
/*
"MOV RAX,0xf00ba4\n";
X64Mode *rax=X64ModeReg(HashFind("RAX",regs));
X64Mode *imm=X64ModeImm(0xf00ba4);
EncodeOpcode(buffer,"MOV",rax,imm);
//PUSH
"PUSH CS\n";
X64Mode *cs=X64ModeReg(HashFind("CS",regs));
EncodeOpcode(buffer,"PUSH",cs);
"PUSH IMM8\n";
X64Mode *imm8=X64ModeImm(I8_MAX);
EncodeOpcode(buffer,"PUSH",imm8);
"PUSH IMM16\n";
X64Mode *imm16=X64ModeImm(I16_MAX);
EncodeOpcode(buffer,"PUSH",imm16);
"PUSH IMM32\n";
X64Mode *imm32=X64ModeImm(I32_MAX);
EncodeOpcode(buffer,"PUSH",imm32);
"PUSH R16\n";
X64Mode *r16=X64ModeReg(HashFind("R9u16",regs));
//EncodeOpcode(buffer,"PUSH",r16); 32bit
"PUSH R32\n";
X64Mode *r32=X64ModeReg(HashFind("R9u32",regs));
EncodeOpcode(buffer,"PUSH",r32);
"PUSH R64\n";
X64Mode *r64=X64ModeReg(HashFind("R9",regs));
EncodeOpcode(buffer,"PUSH",r64);
"PUSH RM16\n";
X64Mode *rm16=X64ModeMem(0,NULL,HashFind("R9",regs),0x7f,2);
//EncodeOpcode(buffer,"PUSH",rm16); 32bit
"PUSH RM32\n";
X64Mode *rm32=X64ModeMem(0,NULL,HashFind("R9",regs),0x7f,4);
EncodeOpcode(buffer,"PUSH",rm32);
"PUSH RM64\n";
X64Mode *rm64=X64ModeMem(0,NULL,HashFind("R9",regs),0x7f,8);
EncodeOpcode(buffer,"PUSH",rm64);

//ADD
"ADD AL,UIMM8\n";
X64Mode *al=X64ModeReg(HashFind("AL",regs));
EncodeOpcode(buffer,"ADD",al,imm8);
"ADD RM8,UIMM8\n";
X64Mode *rm8=X64ModeMem(0,NULL,HashFind("R10",regs),0x7f,1);
EncodeOpcode(buffer,"ADD",rm8,imm8);
X64Mode *r8=X64ModeReg(HashFind("R9u8",regs));
"ADD R8 RM8\n";
EncodeOpcode(buffer,"ADD",r8,rm8);
"ADD RM8n R8\n";
EncodeOpcode(buffer,"ADD",rm8,r8);
"MOVSX R64,RM8\n";
EncodeOpcode(buffer,"MOVSX",r64,rm8);
*/

