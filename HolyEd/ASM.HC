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
#define OPC_F_R8_15ONLY (1<<17)
#define OPC_F_STI (1<<18)
#define OPC_F_ADDI (1<<19)
class COpcodeArg {
    I64 type;
    #define OPCA_SREG 0
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
    union {
        CReg *reg;
    };
};
#define HASH_TYPE_OPCODE 0
#define HASH_TYPE_OPCODE_ALIAS 1
class COpcode:CHash {
    COpcode *prev_op,*next_op;
    U8 bytes[16];
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
                goto term;
            }
            if(*text==',') {
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
            if(areg) {
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
                goto attrloop;
            } else if(!StrCmp("IMM16",name2)) {
                opc.args[argument++].type=OPCA_IMM16,Free(name2);
                goto attrloop;
            } else if(!StrCmp("IMM32",name2)) {
                opc.args[argument++].type=OPCA_IMM32,Free(name);
                goto attrloop;
            } else if(!StrCmp("IMM64",name2)) {
                opc.args[argument++].type=OPCA_IMM64,Free(name2);
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
                opc.flags=OPC_F_IB,Free(name2);
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
                //
                Free(name2);
                goto attrloop;
            } else if(!StrCmp("32",name2)) {
                //
                Free(name2);
                goto attrloop;
            } else if(!StrCmp("UIMM8",name2)) {
                opc.args[argument++].type=OPCA_UIMM8,Free(name2);
                goto attrloop;
            } else if(!StrCmp("UIMM16",name2)) {
                opc.args[argument++].type=OPCA_UIMM16,Free(name2);
                goto attrloop;
            } else if(!StrCmp("UIMM32",name2)) {
                opc.args[argument++].type=OPCA_UIMM32,Free(name2);
                goto attrloop;
            } else if(!StrCmp("UIMM64",name2)) {
                opc.args[argument++].type=OPCA_UIMM64,Free(name2);
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
                goto attrloop;
            } else if(!StrCmp("CW",name2)) {
                goto attrloop;
            } else if(!StrCmp("CD",name2)) {
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
            text++,opc.flags|=OPC_F_R8_15ONLY;
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
        goto oloop;
        oen:
        if(prev) {
            opc->prev_op=prev;
            prev->next_op=opc;
        }
        goto oloop;
        addop:
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
            alias->opcode=opc;
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
Debugger;
I64 cnt;
U8 *ops=FileRead("../Opcodes.DD",&cnt);
ParseOpcodes(ops);
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
    I64 kind;
    #define X64O_REG 0
    #define X64O_MEM 1
    #define X640_IMM 2
    union {
        CReg *reg;
        class {
            CReg *base;
            CReg *index;
            I64 scale;
            I32 disp;
        } mem;
        U64 imm;
    };
};
U8 X64ModRM(I64 modrmMode,I64 r, I64 rm) {
    return  modrmMode<<6|((r&7)<<3)|(r&7);
}
U8 X64SIB(I64 s,I64 i,I64 b) {
    return (s<<6)|((i&7)<<3)|(b&7);
}
U8 *X86ModrRMSIBDisp(U8 *it,I64 size,I64 opcode,CReg *reg,I64 scale,CReg *index,CReg *base,U64 disp) {
    I64 mdrmcode=-1;
    I64 mdrmreg=reg->code;
    I64 mdrmrm=base->code;
    
    Bool SIB=FALSE;
    I64 sibi=-1;
    I64 sibb=-1;
    I64 sibs=0;
    U8 rex=0;
    U8 disps=0;
    if(!disp) {
        disps=0;
        mdrmcode=X64ModRMIndir;
    } else if(disp<0x100) {
        disps=1;
        mdrmcode=X64ModRMIndir8;
    } else {
        disps=4;
        mdrmcode=X64ModRMIndir32;
    }
    if(base==HashFind("RIP",regs)) {
        if(scale||index) throw('asm');
        mdrmcode=X64ModRMIndir;
        mdrmrm=HashFind("RBP",regs)(CReg*)->code;
        disps=4;
    } else if(!index) {
        if(!scale) throw('asm');
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
            if(SIB) throw('asm');
            if(mdrmrm) throw('asm');
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
        sibb=base->code;
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
    rex=X64Rex(
        size==8,
        __X64RegIsIntExt(reg->code),
        SIB&&__X64RegIsIntExt(sibi),
        tmp
    );
    if(rex) *it++=rex;
    *it++=X64ModRM(mdrmcode,mdrmreg,mdrmrm);
    if(SIB) *it++=X64SIB(sibs,sibi,sibb);
    switch(disps) {
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
        default: throw('asm');
    }
    return it;
}



