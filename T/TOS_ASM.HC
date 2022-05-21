#define REGT_NONE	0
#define REGT_R8		1
#define REGT_R16	2
#define REGT_R32	3
#define REGT_R64	4
#define REGT_SEG	5
#define REGT_FSTK	6
#define REGT_MM		7
#define REGT_XMM	8


#define OC_OP_SIZE_PREFIX	0x66
#define OC_ADDR_SIZE_PREFIX	0x67
#define OC_LOCK_PREFIX		0xF0
#define OC_NOP			0x90
#define OC_BPT			0xCC
#define OC_CALL			0xE8
#define OC_JMP_REL8		0xEB
#define OC_NOP2			(OC_NOP<<8+OC_OP_SIZE_PREFIX)

#define REG_RAX		0
#define REG_RCX		1
#define REG_RDX		2
#define REG_RBX		3
#define REG_RSP		4
#define REG_RBP		5
#define REG_RSI		6
#define REG_RDI		7
#define REG_R8		8
#define REG_REGS_NUM	16

#define REG_RIP		16	//Used by compiler, not really it's num
//Be careful: RBPu8, RSPu8, RSIu8, RDIu8 are 20-24
#define REG_NONE	32	//noreg flag sets it to this
#define REG_ALLOC	33	//reg flag sets it to this
#define REG_UNDEF	I8_MIN

#define IEF_OP_SIZE16		0x001
#define IEF_OP_SIZE32		0x002
#define IEF_PLUS_OPCODE		0x004
#define IEF_DONT_SWITCH_MODES	0x008
#define IEF_DFT			0x010
#define IEF_NOT_IN_64_BIT	0x020
#define IEF_48_REX		0x040
#define IEF_REX_ONLY_R8_R15	0x080
#define IEF_REX_XOR_LIKE	0x100
#define IEF_STI_LIKE		0x200
#define IEF_ENDING_ZERO		0x400

//Slash value
#define SV_R_REG	8
#define SV_I_REG	9
#define SV_STI_LIKE	10 //\dLK,"uasm_slash_val",A="FF:::/Compiler/AsmInit.HC,uasm_slash_val"\d only.
#define SV_NONE		11

//Opcode Modifier
#define OM_NO 0
#define OM_CB 1
#define OM_CW 2
#define OM_CD 3
#define OM_CP 4
#define OM_IB 5
#define OM_IW 6
#define OM_ID 7

#define ARGT_NONE	0
#define ARGT_REL8	1
#define ARGT_REL16	2
#define ARGT_REL32	3

#define ARGT_IMM8	4
#define ARGT_IMM16	5
#define ARGT_IMM32	6
#define ARGT_IMM64	7

#define ARGT_UIMM8	8
#define ARGT_UIMM16	9
#define ARGT_UIMM32	10
#define ARGT_UIMM64	11

#define ARGT_R8		12
#define ARGT_R16	13
#define ARGT_R32	14
#define ARGT_R64	15

#define ARGT_RM8	16
#define ARGT_RM16	17
#define ARGT_RM32	18
#define ARGT_RM64	19

#define ARGT_M8		20
#define ARGT_M16	21
#define ARGT_M32	22
#define ARGT_M64	23

#define ARGT_M1632	24 // Not implemented
#define ARGT_M16N32	25 // Not implemented
#define ARGT_M16N16	26 // Not implemented
#define ARGT_M32N32	27 // Not implemented

#define ARGT_MOFFS8	28
#define ARGT_MOFFS16	29
#define ARGT_MOFFS32	30
#define ARGT_MOFFS64	31

#define ARGT_AL		32
#define ARGT_AX		33
#define ARGT_EAX	34
#define ARGT_RAX	35

#define ARGT_CL		36
#define ARGT_DX		37
#define ARGT_SREG	39

#define ARGT_SS		40
#define ARGT_DS		41
#define ARGT_ES		42
#define ARGT_FS		43

#define ARGT_GS		44
#define ARGT_CS		45
#define ARGT_ST0	46
#define ARGT_STI	47

#define ARGT_MM		48 // Not implemented
#define ARGT_MM32	49 // Not implemented
#define ARGT_MM64	50 // Not implemented
#define ARGT_XMM	51 // Not implemented

#define ARGT_XMM32	52 // Not implemented
#define ARGT_XMM64	53 // Not implemented
#define ARGT_XMM128	54 // Not implemented
#define ARGT_XMM0	55 // Not implemented
#define OCF_ALIAS	1
CUAsmGlbls uasm;
I64 InstEntriesCompare(CInst *tmpins1,CInst *tmpins2)
{
    I64 i1,i2,j=0,res=0,oc_cnt1=tmpins1->opcode_cnt,oc_cnt2=tmpins2->opcode_cnt;
    if (tmpins1->flags&IEF_STI_LIKE)
        oc_cnt1--;
    if (tmpins2->flags&IEF_STI_LIKE)
        oc_cnt2--;
    while (TRUE) {
        if (j<oc_cnt1 && j<oc_cnt2) {
            if (res=tmpins1->opcode[j]-tmpins2->opcode[j])
                return res;
            j++;
        } else {
            if (res=oc_cnt1-oc_cnt2)
                return res;

            if (tmpins1->flags&IEF_STI_LIKE && tmpins2->flags&IEF_STI_LIKE)
                return tmpins1->opcode[j]-tmpins2->opcode[j];

            if (res=tmpins1->flags&IEF_STI_LIKE - tmpins2->flags&IEF_STI_LIKE)
                return res;

            if (res=tmpins1->slash_val-tmpins2->slash_val)
                return res;

            if (res=tmpins1->flags&IEF_OP_SIZE32 - tmpins2->flags&IEF_OP_SIZE32)
                return res;

            i1=Bt(&uasm.ins64_arg_mask,tmpins1->arg1) ||
               Bt(&uasm.ins64_arg_mask,tmpins1->arg2);
            i2=Bt(&uasm.ins64_arg_mask,tmpins2->arg1) ||
               Bt(&uasm.ins64_arg_mask,tmpins2->arg2);
            if (res=i1-i2)
                return res;

            if (res=tmpins1->flags&IEF_48_REX - tmpins2->flags&IEF_48_REX)
                return res;

            i1=tmpins1->arg2==ARGT_IMM64 || tmpins1->arg2==ARGT_UIMM64;
            i2=tmpins2->arg2==ARGT_IMM64 || tmpins2->arg2==ARGT_UIMM64;
            return i1-i2;
        }
    }
}

CInst *InstEntryFind(U8 *rip,I64 opsize,I64 seg_size)
{   //Binary Search
    I64 i,j,n,m,k,arg1,arg2,o1,o2,oc_cnt;
    CInst *tmpins,**table;
    i=0;
    if (seg_size==64) {
        table=uasm.table_64;
        j=uasm.table_64_entries-1;
    } else {
        table=uasm.table_16_32;
        j=uasm.table_16_32_entries-1;
    }
    while (TRUE) {
        k=(i+j)>>1;  //binary search
        tmpins=table[k];
//DumpUAsmIns(tmpins);
        m=0;
        n=0;
        while (TRUE) { //ief_compare_start
            arg1=tmpins->arg1;
            arg2=tmpins->arg2;
            oc_cnt=tmpins->opcode_cnt;
            if (tmpins->flags&IEF_STI_LIKE)
                oc_cnt--;
            if (n<oc_cnt) {
                o1=rip[n];
                if (n==tmpins->opcode_cnt-1 && tmpins->flags & IEF_PLUS_OPCODE)
                    o1&=-8;
                o2=tmpins->opcode[n++];
                if (m=o1-o2)
                    goto ief_compare_done;
            } else
                switch (tmpins->uasm_slash_val) {
                case 0...7:
                    if (!(m=rip[n]>>3&7-tmpins->slash_val)) {
                        if ((Bt(&uasm.mem_arg_mask,arg1) ||
                                Bt(&uasm.mem_arg_mask,arg2)) &&
                                rip[n]&0xC0==0xC0) {
                            m=1;
                            goto ief_compare_done;
                        }
                        if (opsize==16) {
                            if (tmpins->flags & IEF_OP_SIZE32) {
                                m=-1;
                                goto ief_compare_done;
                            }
                        } else {
                            if (tmpins->flags & IEF_OP_SIZE16) {
                                m=1;
                                goto ief_compare_done;
                            }
                        }
                        if (opsize==64||arg1==ARGT_M64||arg2==ARGT_M64) {
                            if (!Bt(&uasm.ins64_arg_mask,arg1)&&
                                    !Bt(&uasm.ins64_arg_mask,arg2)&&
                                    !(tmpins->flags&IEF_48_REX))
                                m=1;
                        } else {
                            if (Bt(&uasm.ins64_arg_mask,arg1)||
                                    Bt(&uasm.ins64_arg_mask,arg2) ||
                                    tmpins->flags&IEF_48_REX)
                                m=-1;
                        }
                    } else if ((Bt(&uasm.mem_arg_mask,arg1)||
                                Bt(&uasm.mem_arg_mask,arg2)) &&
                               rip[n]&0xC0==0xC0)
                        m=1;
                    goto ief_compare_done;
                case SV_I_REG:
                    m=rip[n]>>3-tmpins->opcode[tmpins->opcode_cnt-1]>>3;
                    goto ief_compare_done;
                case SV_STI_LIKE:
                    if (!(m=rip[n]>>3-tmpins->opcode[tmpins->opcode_cnt-1]>>3))
                        m=rip[n]-tmpins->opcode[tmpins->opcode_cnt-1];
                    goto ief_compare_done;
                case SV_R_REG:
                case SV_NONE:
                    m=0;
                    if (opsize==16) {
                        if (tmpins->flags & IEF_OP_SIZE32) {
                            m=-1;
                            goto ief_compare_done;
                        }
                    } else {
                        if (tmpins->flags & IEF_OP_SIZE16) {
                            m=1;
                            goto ief_compare_done;
                        }
                    }
                    if (opsize==64 || arg1==ARGT_M64 || arg2==ARGT_M64) {
                        if (!Bt(&uasm.ins64_arg_mask,arg1) &&
                                !Bt(&uasm.ins64_arg_mask,arg2) &&
                                !(tmpins->flags&IEF_48_REX)&& !(arg2==ARGT_NONE &&
                                                                (ARGT_UIMM8<=arg1<=ARGT_UIMM64 ||
                                                                        ARGT_IMM8<=arg1<=ARGT_IMM64)))
                            m=1;
                        else if (tmpins->arg2==ARGT_IMM64 || tmpins->arg2==ARGT_UIMM64) {
                            if (arg2!=ARGT_IMM64&&arg2!=ARGT_UIMM64)
                                m=1;
                        } else if (arg2==ARGT_IMM64||arg2==ARGT_UIMM64)
                            m=-1;
                    } else {
                        if (Bt(&uasm.ins64_arg_mask,arg1) ||
                                Bt(&uasm.ins64_arg_mask,arg2) ||
                                tmpins->flags&IEF_48_REX)
                            m=-1;
                    }
                    goto ief_compare_done;
                }
        }
ief_compare_done:
        if (m>0) {
            if (k==i) {
                k=j;
                break;
            } else
                i=k;
        } else if (m<0) {
            if (k-i<=1) {
                k=i;
                break;
            } else
                j=k;
        } else
            break;
    }
    return table[k];
}
U0 UAsmHashLoad()
{
    CHashOpcode *tmph;
    CInst *tmpins;
    I64 i,j1,j2,k;

    uasm.ins64_arg_mask=0x0880888880+1<<ARGT_ST0+1<<ARGT_STI;
    uasm.signed_arg_mask=1<<ARGT_REL8+1<<ARGT_REL16+1<<ARGT_REL32+
                         1<<ARGT_IMM8+1<<ARGT_IMM16+1<<ARGT_IMM32+1<<ARGT_IMM64;
    uasm.mem_arg_mask=1<<ARGT_M8+1<<ARGT_M16+1<<ARGT_M32+1<<ARGT_M64;

    uasm.table_16_32_entries=uasm.table_64_entries=0;
    for (i=0; i<=cmp.asm_hash->mask; i++) {
        tmph=cmp.asm_hash->body[i];
        while (tmph) {
            if (tmph->type==HTT_OPCODE && !(tmph->oc_flags&OCF_ALIAS)) {
                tmpins=&tmph->ins;
                for (k=0; k<tmph->inst_entry_cnt; k++) {
                    uasm.table_16_32_entries++;
                    if (!(tmpins->flags&IEF_NOT_IN_64_BIT))
                        uasm.table_64_entries++;
                    tmpins++;
                }
            }
            tmph=tmph->next;
        }
    }

    j1=j2=0;
    uasm.table_16_32=MAlloc(uasm.table_16_32_entries*sizeof(U8 *));
    uasm.table_64   =MAlloc(uasm.table_64_entries   *sizeof(U8 *));
    for (i=0; i<=cmp.asm_hash->mask; i++) {
        tmph=cmp.asm_hash->body[i];
        while (tmph) {
            if (tmph->type==HTT_OPCODE && !(tmph->oc_flags&OCF_ALIAS)) {
                tmpins=&tmph->ins;
                for (k=0; k<tmph->inst_entry_cnt; k++) {
                    uasm.table_16_32[j1++]=tmpins;
                    if (!(tmpins->flags&IEF_NOT_IN_64_BIT))
                        uasm.table_64[j2++]=tmpins;
                    tmpins++;
                }
            }
            tmph=tmph->next;
        }
    }
    QSortI64(uasm.table_16_32,uasm.table_16_32_entries,&InstEntriesCompare);
    QSortI64(uasm.table_64,uasm.table_64_entries,&InstEntriesCompare);
}
U0 Ui(U8 *buf,U8 **_rip,I64 seg_size=64,I64 *_jmp_dst=NULL,Bool just_ins=FALSE)
{   //Unassembles one inst
    I64 i,disp,imm,opsize,opadd,
        arg1,arg2,reloced_arg1,reloced_arg2,
        arg1_size=0,arg2_size=0,reloced_arg1_size,reloced_arg2_size,
        ModrM=-1,SIB=-1,scale,r1,r2,
        Mod=-1,RM1=-1,RM2=-1,REX=-1,REX_r=0,REX_x=0,REX_b=0;
    Bool cont;
    CInst *tmpins,*tmpins2;
    CHashOpcode *tmpo;
    U8 *rip=*_rip,*ptr,*reloced_arg1_st,*reloced_arg2_st,
        *bin_data_area1,*bin_data_area2,
        line1[512],line2[512],buf2[512],arg1_st[512],
        arg2_st[512],seg_overrides[32];

    if (_jmp_dst) *_jmp_dst=-1;
    if (seg_size==16) {
        opsize=16;
        opadd=16;
    } else if (seg_size==32) {
        opsize=32;
        opadd=32;
    } else {
        opsize=32;
        opadd=64;
    }
    *arg1_st=0;
    *arg2_st=0;
    line1[0]=0,line2[0]=0;
    //CatPrint(line1,"%p\n",rip);
    StrPrint(line1+StrLen(line1),"%24tp ",rip);
    bin_data_area1=line1+StrLen(line1);
    for (i=0; i<6; i++)
        CatPrint(line1,"%02X",rip[i]);
    CatPrint(line1," ");

    StrPrint(line2,"%24tp ",rip+6);
    bin_data_area2=line2+StrLen(line2);
    for (i=6; i<12; i++)
        CatPrint(line2,"%02X",rip[i]);

    *seg_overrides=0;
    cont=TRUE;
    while (TRUE) {
        switch (*rip) {
        case 0x2E:
            if (StrLen(seg_overrides)<24)
                CatPrint(seg_overrides,"CS:");
            break;
        case 0x36:
            if (StrLen(seg_overrides)<24)
                CatPrint(seg_overrides,"SS:");
            break;
        case 0x3E:
            if (StrLen(seg_overrides)<24)
                CatPrint(seg_overrides,"DS:");
            break;
        case 0x26:
            if (StrLen(seg_overrides)<24)
                CatPrint(seg_overrides,"ES:");
            break;
        case 0x64:
            if (StrLen(seg_overrides)<24)
                CatPrint(seg_overrides,"FS:");
            break;
        case 0x65:
            if (StrLen(seg_overrides)<24)
                CatPrint(seg_overrides,"GS:");
            break;
        case OC_OP_SIZE_PREFIX:
            if (opsize==16)
                opsize=32;
            else
                opsize=16;
            break;
        case OC_ADDR_SIZE_PREFIX:
            if (opadd==16)
                opadd=32;
            else
                opadd=16;
            break;
        case 0x40...0x4F:
            if (seg_size==64) {
                REX=*rip;
                if (REX>=0x48)
                    opsize=64;
                REX_b=Bt(&REX,0)<<3;
                REX_x=Bt(&REX,1)<<3;
                REX_r=Bt(&REX,2)<<3;
                break;
            } //Fall thru if !64
        default:
            cont=FALSE;
        }
        if (cont)
            rip++;
        else
            break;
    }
    tmpins=InstEntryFind(rip,opsize,seg_size);
    if (opsize==32 && seg_size==64) {
        tmpins2=InstEntryFind(rip,64,seg_size);
        if (tmpins2!=tmpins && tmpins2->flags&IEF_REX_ONLY_R8_R15 ||
                tmpins2->flags&IEF_REX_XOR_LIKE&& rip[1]>>3&7==rip[1]&7)
            tmpins=tmpins2;
    }

    rip+=tmpins->opcode_cnt;
    tmpo=tmpins(U8 *)-tmpins->ins_entry_num*sizeof(CInst)
         -CHashOpcode.ins;
    if (just_ins)
        *line1=0;
    CatPrint(line1,tmpo->str);

    arg1=tmpins->arg1;
    arg2=tmpins->arg2;

    if (arg1_size=tmpins->size1) {
        if (Bt(&uasm.signed_arg_mask,arg1))
            CatPrint(arg1_st,"I%d ",arg1_size);
        else
            CatPrint(arg1_st,"U%d ",arg1_size);
    }

    if (arg2_size=tmpins->size2) {
        if (Bt(&uasm.signed_arg_mask,arg2))
            CatPrint(arg2_st,"I%d ",arg2_size);
        else
            CatPrint(arg2_st,"U%d ",arg2_size);
    }

    if (tmpins->flags & IEF_PLUS_OPCODE) {
        rip--;
        RM1=*rip++ - tmpins->opcode[tmpins->opcode_cnt-1]+REX_b;
        ptr=NULL;
        if (ARGT_R8<=arg1<=ARGT_R64) {
            if (arg1_size==8) {
                if (REX!=-1)
                    ptr="ST_U8_REX_REGS";
                else
                    ptr="ST_U8_REGS";
            } else if (arg1_size==16)
                ptr="ST_U16_REGS";
            else if (arg1_size==32)
                ptr="ST_U32_REGS";
            else if (arg1_size==64)
                ptr="ST_U64_REGS";
            if (ptr)
                CatPrint(arg1_st,"%s",DefineSub(RM1,ptr));
        } else {
            if (arg2_size==8) {
                if (REX!=-1)
                    ptr="ST_U8_REX_REGS";
                else
                    ptr="ST_U8_REGS";
            } else if (arg2_size==16)
                ptr="ST_U16_REGS";
            else if (arg2_size==32)
                ptr="ST_U32_REGS";
            else if (arg2_size==64)
                ptr="ST_U64_REGS";
            if (ptr)
                CatPrint(arg2_st,"%s",DefineSub(RM1,ptr));
        }
    }

    if (ARGT_RM8<=arg1<=ARGT_RM64 || ARGT_M8<=arg1<=ARGT_M64 ||
            ARGT_RM8<=arg2<=ARGT_RM64 || ARGT_M8<=arg2<=ARGT_M64) {
        if (ARGT_RM8<=arg2<=ARGT_RM64 || ARGT_M8<=arg2<=ARGT_M64) {
            reloced_arg1=arg2;
            reloced_arg2=arg1;
            reloced_arg1_size=arg2_size;
            reloced_arg2_size=arg1_size;
            reloced_arg1_st=arg2_st;
            reloced_arg2_st=arg1_st;
        } else {
            reloced_arg1=arg1;
            reloced_arg2=arg2;
            reloced_arg1_size=arg1_size;
            reloced_arg2_size=arg2_size;
            reloced_arg1_st=arg1_st;
            reloced_arg2_st=arg2_st;
        }

        CatPrint(reloced_arg1_st,seg_overrides);
        ModrM=*rip++;
        Mod=ModrM>>6 & 3;
        RM1=ModrM & 7+REX_b;
        RM2=ModrM>>3 & 7+REX_r;
        if (Mod<3 && RM1&7==4)
            SIB=*rip++;
        if (Mod==1) {
            disp=*rip(U8 *);
            rip++;
            CatPrint(reloced_arg1_st,"%02X",disp);
        } else if (Mod==2) {
            disp=*rip(U32 *);
            rip+=2;
            CatPrint(reloced_arg1_st,"%08X",disp);
        }
        if (tmpins->slash_val<8)
            RM2=-1;
        else {
            ptr=NULL;
            if (reloced_arg2==ARGT_SREG) {
                if (RM2<=5)
                    ptr="ST_SEG_REGS";
            } else if (!(ARGT_IMM8<=reloced_arg2<=ARGT_IMM64) &&
                       !(ARGT_UIMM8<=reloced_arg2<=ARGT_UIMM64)) {
                if (reloced_arg2_size==8) {
                    if (REX!=-1)
                        ptr="ST_U8_REX_REGS";
                    else
                        ptr="ST_U8_REGS";
                } else if (reloced_arg2_size==16)
                    ptr="ST_U16_REGS";
                else if (reloced_arg2_size==32)
                    ptr="ST_U32_REGS";
                else if (reloced_arg2_size==64)
                    ptr="ST_U64_REGS";
            }
            if (ptr)
                CatPrint(reloced_arg2_st,"%s",DefineSub(RM2,ptr));
        }
        if (RM1&7==5 && !Mod) {
            disp=*rip(I32 *);
            rip++;
            if (seg_size==64) {
                disp+=rip;
                if (reloced_arg2==ARGT_IMM8 || reloced_arg2==ARGT_UIMM8)
                    disp++;
                else if (reloced_arg2==ARGT_IMM16 || reloced_arg2==ARGT_UIMM16)
                    disp+=2;
                else if (reloced_arg2==ARGT_IMM32 || reloced_arg2==ARGT_UIMM32)
                    disp+=4;
                else if (reloced_arg2==ARGT_IMM64 || reloced_arg2==ARGT_UIMM64)
                    disp+=8;
            }
            CatPrint(reloced_arg1_st,"[%X]",disp);
            RM1=-1;
        } else {
            if (Mod<3) {
                if (RM1&7==4) {
                    RM1=-1;
                    r1=SIB & 7+REX_b;
                    r2=SIB>>3 & 7+REX_x;
                    scale=SIB>>6 &3;
                    if (scale==3)
                        scale=8;
                    else if (scale==2)
                        scale=4;
                    else if (scale==1)
                        scale=2;
                    else
                        scale=1;
                    if (seg_size==64)
                        ptr="ST_U64_REGS";
                    else
                        ptr="ST_U32_REGS";
                    if (r1==REG_RBP && !Mod) {
                        disp=*rip(U32 *);
                        rip+=4;
                        CatPrint(reloced_arg1_st,"%08X[%s*%d]",disp,DefineSub(r2,ptr),scale);
                    } else if (r2==4)
                        CatPrint(reloced_arg1_st,"[%s]",DefineSub(r1,ptr));
                    else
                        CatPrint(reloced_arg1_st,"[%s+%s*%d]",DefineSub(r1,ptr),DefineSub(r2,ptr),scale);
                } else {
                    if (opadd==16)
                        ptr="ST_U16_REGS";
                    else if (opadd==32)
                        ptr="ST_U32_REGS";
                    else
                        ptr="ST_U64_REGS";
                    CatPrint(reloced_arg1_st,"[%s]",DefineSub(RM1,ptr));
                }
            } else {
                ptr=NULL;
                if (reloced_arg1_size==8) {
                    if (REX!=-1)
                        ptr="ST_U8_REX_REGS";
                    else
                        ptr="ST_U8_REGS";
                } else if (reloced_arg1_size==16)
                    ptr="ST_U16_REGS";
                else if (reloced_arg1_size==32)
                    ptr="ST_U32_REGS";
                else if (reloced_arg1_size==64)
                    ptr="ST_U64_REGS";
                if (ptr)
                    CatPrint(reloced_arg1_st,DefineSub(RM1,ptr));
            }
        }
    }

    switch (arg1) {
    case ARGT_IMM8:
    case ARGT_UIMM8:
        imm=*rip(U8*);
        rip++;
        CatPrint(arg1_st,"%02X",imm);
        if (tmpins->opcode[0]==0xCD && (ptr=DefineSub(imm,"ST_INT_NAMES")))
            CatPrint(arg1_st," %s",ptr);
        break;
    case ARGT_IMM16:
    case ARGT_UIMM16:
        CatPrint(arg1_st,"%04X",*rip(U16 *));
        rip+=2;
        break;
    case ARGT_IMM32:
    case ARGT_UIMM32:
        CatPrint(arg1_st,"%08X",*rip(U32 *));
        rip+=4;
        break;
    case ARGT_IMM64:
    case ARGT_UIMM64:
        CatPrint(arg1_st,"%016X",*rip(I64 *));
        rip+=8;
        break;
start:
    case ARGT_REL8:
        disp=*rip(I8 *);
        rip++;

        break;
    case ARGT_REL16:
        disp=*rip(I16 *);
        rip+=2;
        break;
    case ARGT_REL32:
        disp=*rip(I32 *);
        rip+=4;
        break;
end:
        disp+=rip;
        CatPrint(arg1_st,"%p ",disp);
        if (_jmp_dst) *_jmp_dst=disp;
        break;
    case ARGT_MOFFS8...ARGT_MOFFS64:
        CatPrint(arg1_st,seg_overrides);
        if (arg1_size==8) {
            disp=*rip(U8 *);
            rip++;
        }      else if (opadd==16) {
            disp=*rip(U16 *);
            rip+=2;
        }      else {
            disp=*rip(U32 *);
            rip+=4;
        }
        CatPrint(arg1_st,"[%X]",disp);
        break;
    case ARGT_AL ... ARGT_DX:
    case ARGT_SS ... ARGT_ST0:
        CatPrint(arg1_st,"%s",DefineSub(arg1-ARGT_AL,
                                        "AL\0AX\0EAX\0RAX\0CL\0DX\0 \0 \0SS\0DS\0ES\0FS\0GS\0CS\0ST0\0"));
        break;
    case ARGT_STI:
        rip--;
        CatPrint(arg1_st,"%s",DefineSub(ptr*rip++ - tmpins->opcode[tmpins->opcode_cnt-1],
                                        "ST_FSTK_REGS"));
        break;
    }

    switch (arg2) {
    case ARGT_IMM8:
    case ARGT_UIMM8:
        CatPrint(arg2_st,"%02X",*rip(U8 *));
        rip++;
        break;
    case ARGT_IMM16:
    case ARGT_UIMM16:
        CatPrint(arg2_st,"%04X",*rip(U16 *));
        rip+=2;
        break;
    case ARGT_IMM32:
    case ARGT_UIMM32:
        CatPrint(arg2_st,"%08X",*rip(U32 *));
        rip+=4;
        break;
    case ARGT_IMM64:
    case ARGT_UIMM64:
        CatPrint(arg2_st,"%016X",*rip(I64 *));
        rip+=8;
        break;
    case ARGT_MOFFS8...ARGT_MOFFS64:
        CatPrint(arg2_st,seg_overrides);
        if (arg2_size==8) {
            disp=*rip(U8 *);
            rip++;
        } else if (opadd==16) {
            disp=*rip(U16 *);
            rip+=2;
        } else {
            disp=*rip(U32 *);
            rip+=4;
        }
        CatPrint(arg2_st,"[%X]",disp);
        break;
    case ARGT_AL ... ARGT_DX:
    case ARGT_SS ... ARGT_ST0:
        CatPrint(arg2_st,"%s",arg2-ARGT_AL,
                 "AL\0AX\0EAX\0RAX\0CL\0DX\0 \0 \0SS\0DS\0ES\0FS\0GS\0CS\0ST0\0");
        break;
    case ARGT_STI:
        rip--;
        CatPrint(arg2_st,"%s",*rip++ -tmpins->opcode[tmpins->opcode_cnt-1],
                 "ST_FSTK_REGS");
        break;
    }
    if (tmpins->flags&IEF_ENDING_ZERO)
        rip++;

    if (*arg1_st)
        CatPrint(line1,"\t%s",arg1_st);
    if (*arg2_st)
        CatPrint(line1,",%s",arg2_st);
    CatPrint(line1,"\n");
    CatPrint(line2,"\n");
    if (!just_ins) {
        for (i=rip-(*_rip)(I64); i<6; i++) {
            bin_data_area1[i<<1]=' ';
            bin_data_area1[i<<1+1]=' ';
        }
        for (i=rip-(*_rip)(I64); i<12; i++) {
            bin_data_area2[(i-6)<<1]=' ';
            bin_data_area2[(i-6)<<1+1]=' ';
        }
    }
    StrCpy(buf,line1);
    if (!just_ins && rip-(*_rip)(I64)>6)
        CatPrint(buf,line2);
    *_rip=rip;
}

U8 *U(U8 *rip,I64 cnt=20,I64 seg_size=64)
{   //Unassembles a num of insts.
    I64 i;
    U8 buf[1024];
    if (seg_size==16)
        "16-bit unassembly is not well supported.\n";
    for (i=0; i<cnt; i++) {
        Ui(buf,&rip,seg_size);
        "%s"
        ,buf;
    }
    return rip;
}

I64 Un(U8 *rip,I64 cnt=0x80,I64 seg_size=64)
{   //Unassembles a num of bytes
    I64 i=0;
    U8 buf[1024],*end_rip=rip(I64)+cnt;
    if (seg_size==16)
        "16-bit unassembly is not well supported.\n";
    while (rip<end_rip) {
        Ui(buf,&rip,seg_size);
        "%s"
        ,buf;
        i++;
    }
    return i;
}

U0 AsmPrsInsFlags(CCmpCtrl *cc,CInst *tmpins)
{
    I64 i;
    while (TRUE) {
        switch (cc->token) {
        case TK_IDENT:
            if ((i=LstMatch(cc->cur_str,"NO\0CB\0CW\0CD\0CP\0IB\0IW\0ID\0"))>=0) {
                tmpins->opcode_modifier=i;
                break;
            } else
                return;
        case TK_I64:
            if (cc->cur_i64==16)
                tmpins->flags|=IEF_OP_SIZE16;
            else if (cc->cur_i64==32)
                tmpins->flags|=IEF_OP_SIZE32;
            else
                return;
            break;
        case '+':
            tmpins->flags|=IEF_PLUS_OPCODE;
        case '/':
            if (Lex(cc)==TK_I64 && cc->cur_i64<8)
                tmpins->slash_val=cc->cur_i64;
            else if (cc->token==TK_IDENT) {
                if (!StrCmp(cc->cur_str,"R"))
                    tmpins->slash_val=SV_R_REG;
                else if (!StrCmp(cc->cur_str,"I"))
                    tmpins->slash_val=SV_I_REG;
                else
                    return;
            } else
                return;
            break;
        case '!':
            tmpins->flags|=IEF_DONT_SWITCH_MODES;
            break;
        case '&':
            tmpins->flags|=IEF_DFT;
            break;
        case '%':
            tmpins->flags|=IEF_NOT_IN_64_BIT;
            break;
        case '=':
            tmpins->flags|=IEF_48_REX;
            break;
        case '`':
            tmpins->flags|=IEF_REX_ONLY_R8_R15;
            break;
        case '^':
            tmpins->flags|=IEF_REX_XOR_LIKE;
            break;
        case '*':
            tmpins->flags|=IEF_STI_LIKE;
            break;
        case '\d':
            tmpins->flags|=IEF_ENDING_ZERO;
            break;
        default:
            return;
        }
        Lex(cc);
    }
}
U0 AsmHashLoad()
{   //See \dLK,"::/Compiler/OpCodes.DD"\d.
    I64 i,j,size,size_max;
    CInternalType *tmpit;
    CCmpCtrl *cc;
    CHashGeneric *tmph;
    CHashReg *tmpr;
    CHashOpcode *tmpo,*tmpo2,*tmpo_max;
    CInst *tmpins;
    CHashClass *tmpc;

    cmp.size_arg_mask[0]=0x3FF0FFFFFE;
    cmp.size_arg_mask[1]=0x1110111112;
    cmp.size_arg_mask[2]=0x2220222224;
    cmp.size_arg_mask[4]=0x0440444448;
    cmp.size_arg_mask[8]=0x0880888880;

    cmp.asm_hash=HashTableNew(1024);
    size_max=CHashOpcode.ins+sizeof(CInst)<<5;
    tmpo_max=MAlloc(size_max);

    cc=CmpCtrlNew(FileRead("OpCodes.DD"),,"OpCodes.DD");
    Lex(cc);
    while (cc->token) {
		if (cc->token!=TK_IDENT) {
            "Expecting identifier!!!";
            throw('Lex');
        }
        i=LstMatch(cc->cur_str,"NONE\0R8\0R16\0R32\0R64\0SEG\0FSTK\0"
                   "MM\0XMM\0OPCODE\0KEYWORD\0ASM_KEYWORD\0");
        if (i<=0) {
            "Unknown Stmt";
        }
        Lex(cc); //skip keyword
        if (cc->token!=TK_IDENT) {
            "Expecting identifier at ";
            throw('Lex');
        }
        switch (i) {
        case REGT_R8...REGT_XMM:
            tmpr=MAlloc(sizeof(CHashReg));
            tmpr->str=cc->cur_str;
            cc->cur_str=NULL;
            Lex(cc); //skip keyword name
            if (cc->token!=TK_I64) {
                "Expecting int!!!";
                throw('Lex');
            }
            tmpr->type=HTT_REG;
            tmpr->reg_type=i;
            tmpr->reg_num=cc->cur_i64;
            HashAdd(tmpr,cmp.asm_hash);
            Lex(cc); //Skip INT
            break;
        case: //OPCODE
            if (cc->token!=TK_IDENT) {
                "Expecting opcode!!!";
                throw('Lex');
            }
            MemSet(tmpo_max,0,size_max);
            tmpo_max->type=HTT_OPCODE;
            tmpo_max->inst_entry_cnt=0;
            tmpo_max->str=cc->cur_str;
            cc->cur_str=NULL;
            Lex(cc);	//Skip OPCODE
            while (cc->token && cc->token!=';' && cc->token!=':') {
                tmpins=&tmpo_max->ins[tmpo_max->inst_entry_cnt];
                tmpins->ins_entry_num=tmpo_max->inst_entry_cnt++;
                tmpins->slash_val=SV_NONE; //Not zero!!
                while (cc->token==TK_I64) {
                    tmpins->opcode[tmpins->opcode_cnt++]=cc->cur_i64;
                    Lex(cc);
                }
                if (cc->token==',')
                    Lex(cc);
                else if (cc->token!=';') {
                    "Expecting ','!!!!(Got '%c')\n",cc->token;
                    throw('Lex');
                }
                AsmPrsInsFlags(cc,tmpins);

                tmpins->uasm_slash_val=tmpins->slash_val;
                if (tmpins->flags&IEF_STI_LIKE && tmpins->slash_val!=SV_I_REG)
                    tmpins->uasm_slash_val=SV_STI_LIKE;

                tmpins->arg1=tmpins->arg2=tmpins->size1=tmpins->size2=0;
                if (cc->token==TK_IDENT) {
                    j=DefineMatch(cc->cur_str,"ST_ARG_TYPES");
                    tmpins->arg1=j;
                    if (Bt(&cmp.size_arg_mask[1],j))
                        tmpins->size1=8;
                    else if (Bt(&cmp.size_arg_mask[2],j))
                        tmpins->size1=16;
                    else if (Bt(&cmp.size_arg_mask[4],j))
                        tmpins->size1=32;
                    else if (Bt(&cmp.size_arg_mask[8],j))
                        tmpins->size1=64;

                    if (Lex(cc)==TK_IDENT) {
                        j=DefineMatch(cc->cur_str,"ST_ARG_TYPES");
                        Lex(cc);
                        tmpins->arg2=j;
                        if (Bt(&cmp.size_arg_mask[1],j))
                            tmpins->size2=8;
                        else if (Bt(&cmp.size_arg_mask[2],j))
                            tmpins->size2=16;
                        else if (Bt(&cmp.size_arg_mask[4],j))
                            tmpins->size2=32;
                        else if (Bt(&cmp.size_arg_mask[8],j))
                            tmpins->size2=64;
                    }
                }
            }
            size=CHashOpcode.ins+sizeof(CInst)*tmpo_max->inst_entry_cnt;
            tmpo=MAlloc(size);
            MemCpy(tmpo,tmpo_max,size);
            tmpo->use_cnt=0;
            if (HashFind(tmpo->str,cmp.asm_hash,-1)) {
                "Duplicate OPCODE entry %s\n",tmpo->str;
                throw('Lex');
            }
            HashAdd(tmpo,cmp.asm_hash);
            //Parse aliases.
            if (cc->token==':') {
                while (Lex(cc)==TK_IDENT) {
                    tmpo2=MAllocIdent(tmpo);
                    tmpo2->str=cc->cur_str;
                    cc->cur_str=0;
                    tmpo2->oc_flags|=OCF_ALIAS;
                    if (HashFind(tmpo2->str,cmp.asm_hash,-1)) {
                        "Duplicate OPCODE entry %s\n",tmpo->str;
                        throw('Lex');
                    }
                    HashAdd(tmpo2,cmp.asm_hash);
                }
            }
            break;
        case: //KEYWORD
        case: //ASM_KEYWORD
            tmph=MAlloc(sizeof(CHashGeneric));
            tmph->str=cc->cur_str;
            cc->cur_str=NULL;
            Lex(cc); //skip keyword name
            if (cc->token!=TK_I64) {
                "Expecting int!!!";
                throw('Lex');
            }
            tmph->user_data0=cc->cur_i64;
            if (i==10)
                tmph->type=HTT_KEYWORD;
            else
                tmph->type=HTT_ASM_KEYWORD;
            HashAdd(tmph,cmp.asm_hash);
            Lex(cc); //Skip INT
            break;
        }
        if (cc->token!=';') {
            "Missing ';'\n";
            throw('Lex');
        }
        Lex(cc); //Skip ';'
    }
    Free(tmpo_max);
    for (i=0;i<INTERNAL_TYPES_NUM;i++) {
        tmpit=&internal_types_table[i];
        tmpc=PrsClassNew;
        tmpc->type=HTT_INTERNAL_TYPE;
        tmpc->raw_type=tmpit->type;
        Bts(&tmpc->flags,Cf_INTERNAL_TYPE);
        tmpc->size=tmpit->size;
        tmpc->str=StrNew(tmpit->name);
        HashAdd(tmpc,cmp.asm_hash);
        cmp.internal_types[tmpc->raw_type]=tmpc;
      }
    //TODO TODO READ THIS
    if(!Fs->hash_table) Fs->hash_table=cmp.asm_hash;
    else Fs->hash_table->next=cmp.asm_hash;
    CmpCtrlDel(cc);
}
U0 CmpLoadDefines()
{
	DefineLstLoad("ST_RAW_TYPES",
				  " \0 \0I0\0U0\0I8\0U8\0I16\0U16\0I32\0U32\0I64\0U64\0"
				  "F32\0UF32\0F64\0UF64\0");
    DefineLstLoad("ST_NATURAL_TYPES",
                  "I8\0U8\0I16\0U16\0I32\0U32\0I64\0U64\0F64\0String\0");
    DefineLstLoad("ST_INT_SIZE_TYPES",
                  "I8\0U8\0I16\0U16\0I32\0U32\0I64\0U64\0");
    DefineLstLoad("ST_TY_TYPES","NULL\0STK\0IMM\0REG\0DISP\0SIB\0RIP\0");
    DefineLstLoad("ST_U8_REGS","AL\0CL\0DL\0BL\0AH\0CH\0DH\0BH\0"
                  "R8u8\0R9u8\0R10u8\0R11u8\0R12u8\0R13u8\0R14u8\0R15u8\0");
    DefineLstLoad("ST_U8_REX_REGS","AL\0CL\0DL\0BL\0RSPu8\0RBPu8\0RSIu8\0RDIu8\0"
                  "R8u8\0R9u8\0R10u8\0R11u8\0R12u8\0R13u8\0R14u8\0R15u8\0");
    DefineLstLoad("ST_U16_REGS","AX\0CX\0DX\0BX\0SP\0BP\0SI\0DI\0"
                  "R8u16\0R9u16\0R10u16\0R11u16\0R12u16\0R13u16\0R14u16\0R15u16\0");
    DefineLstLoad("ST_U32_REGS","EAX\0ECX\0EDX\0EBX\0ESP\0EBP\0ESI\0EDI\0"
                  "R8u32\0R9u32\0R10u32\0R11u32\0R12u32\0R13u32\0R14u32\0R15u32\0");
    DefineLstLoad("ST_U64_REGS","RAX\0RCX\0RDX\0RBX\0RSP\0RBP\0RSI\0RDI\0"
                  "R8\0R9\0R10\0R11\0R12\0R13\0R14\0R15\0");
    DefineLstLoad("ST_MM_REGS","MM0\0MM1\0MM2\0MM3\0MM4\0MM5\0MM6\0MM7\0");
    DefineLstLoad("ST_XMM_REGS",
                  "XMM0\0XMM1\0XMM2\0XMM3\0XMM4\0XMM5\0XMM6\0XMM7\0");
    DefineLstLoad("ST_ARG_TYPES","NONE\0REL8\0REL16\0REL32\0"
                  "IMM8\0IMM16\0IMM32\0IMM64\0UIMM8\0UIMM16\0UIMM32\0UIMM64\0"
                  "R8\0R16\0R32\0R64\0RM8\0RM16\0RM32\0RM64\0"
                  "M8\0M16\0M32\0M64\0M1632\0M16N32\0M16N16\0M32N32\0"
                  "MOFFS8\0MOFFS16\0MOFFS32\0MOFFS64\0"
                  "AL\0AX\0EAX\0RAX\0CL\0DX\0 \0SREG\0"
                  "SS\0DS\0ES\0FS\0GS\0CS\0ST0\0STI\0"
                  "MM\0MM32\0MM64\0XMM\0XMM32\0XMM64\0XMM128\0XMM0\0");
    DefineLstLoad("ST_SEG_REGS","ES\0CS\0SS\0DS\0FS\0GS\0");
    DefineLstLoad("ST_FSTK_REGS","ST0\0ST1\0ST2\0ST3\0ST4\0ST5\0ST6\0ST7\0");
}
