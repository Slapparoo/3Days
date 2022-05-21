U0 HashDel(CHashSrcSym *tmph)
{   //Free a std TempleOS system hash entry.
    if (!tmph) return;
    if (!(tmph->type&HTT_DICT_WORD))
        Free(tmph->str);
    if (tmph->type & HTG_SRC_SYM) {
        Free(tmph->src_link);
        Free(tmph->idx);
        Free(tmph->import_name);
        LinkedLstDel(tmph->ie_lst);
        if (tmph->type & (HTT_FUN | HTT_EXPORT_SYS_SYM))
            Free(tmph->dbg_info);
        if (tmph->type & (HTT_FUN | HTT_CLASS))
//Assumes code not on heap, so doesn't Free.
            //\dLK,"ClassMemberLstDel",A="MN:ClassMemberLstDel"\d() is an import to the Kernel module
            ;//ClassMemberLstDel(tmph);
        else if (tmph->type&HTT_DEFINE_STR)
            Free(tmph(CHashDefineStr *)->data);
        else if (tmph->type & HTT_GLBL_VAR) {
            if (!(tmph(CHashGlblVar *)->flags&GVF_ALIAS))
                Free(tmph(CHashGlblVar *)->data_addr);
            LinkedLstDel(tmph(CHashGlblVar *)->dim.next);
            if (tmph(CHashGlblVar *)->fun_ptr)
                HashDel(tmph(CHashGlblVar *)->fun_ptr
                        -tmph(CHashGlblVar *)->fun_ptr->ptr_stars_cnt);
        }
    } else if (tmph->type & HTT_FILE)
        Free(tmph(CHashGeneric *)->user_data0);
    Free(tmph);
}
   asm {
//************************************
        SYS_HASH_STR::
// IN:	RSI=Addr of string
// OUT:	RAX
        XOR	RAX,RAX
        TEST	RSI,RSI
        JZ	@@15

        PUSH	RSI
        PUSH	RBX
        XOR	RBX,RBX
        JMP	@@10

        @@05:	SHL1	RBX
        ADC	RBX,RAX
        @@10:	LODSB
        TEST	AL,AL
        JNZ	@@05

        MOV	RAX,RBX
        SHR	RBX,16
        ADC	RAX,RBX
        POP	RBX
        POP	RSI

        @@15:	RET
//************************************
        SYS_HASH_SINGLE_TABLE_FIND1::
// IN:	RAX=HASHED STRING VAL
//	RSI=STR
//	RBX=TYPE MASK
//	RDI=TABLE
//	RCX=INSTANCE, NOT ZERO
// OUT: RAX=ENTRY OR ZERO NOT FOUND
//	RDX=POINTER TO POINTER TO ENTRY
//	RCX IF NOT FOUND ENOUGH, DECREMENTED BY NUM MATCHES
//	ZERO FLAG SET NOT FOUND
        MOV	RCX,1
        SYS_HASH_SINGLE_TABLE_FIND::
        TEST	RCX,RCX
        JNZ	@@05
        XOR	RAX,RAX
        RET
        @@05:	AND	RAX,U64 CHashTable.mask[RDI]
        MOV	RDX,U64 CHashTable.body[RDI]
        LEA	RDX,U64 [RDX+RAX*8]
@@10:	MOV	RAX,U64 [RDX]
        TEST	RAX,RAX
        JNZ	@@15
        RET

@@15:	TEST	U32 CHash.type[RAX],EBX
        JZ	@@30
        PUSH	RAX
        PUSH	RDI
        PUSH	RSI
        MOV	RDI,U64 CHash.str[RAX]
@@20:	LODSB
        CMP	U8 [RDI],AL
        JNE	@@25
        INC	RDI
        TEST	AL,AL
        JNZ	@@20
        POP	RSI
        POP	RDI
        POP	RAX
        DEC RCX
        CMP RCX,0
        JNZ	@@30
        INC	U32 CHash.use_cnt[RAX]
        TEST	RAX,RAX
        RET

@@25:	POP	RSI
        POP	RDI
        POP	RAX

@@30:	LEA	RDX,U64 CHash.next[RAX]
        JMP	@@10
//************************************
        SYS_HASH_FIND1::
// IN:	RSI=STR
//	RBX=TYPE MASK
//	RDI=TABLE
//	RCX=INSTANCE NUM
// OUT: RAX=ENTRY OR ZERO NOT FOUND
//	ZERO FLAG SET NOT FOUND
        MOV	RCX,1
        SYS_HASH_FIND::
        PUSH	RDI
        CALL	SYS_HASH_STR

@@05:	PUSH	RAX
        CALL	SYS_HASH_SINGLE_TABLE_FIND
        JNZ	@@15
        POP	RAX
@@10:	MOV	RDI,U64 CHashTable.next[RDI]
        TEST	RDI,RDI
        JNZ	@@05
        POP	RDI
        XOR	RAX,RAX
        RET

@@15:	ADD	RSP,8
        POP	RDI
        TEST	RAX,RAX
        RET
//************************************
        SYS_HASH_BUCKET_FIND::
// IN:	RSI=STR
//	RDI=TABLE
// OUT: RAX=BUCKET
        PUSH	RDX
        CALL	SYS_HASH_STR
        AND	RAX,U64 CHashTable.mask[RDI]
        MOV	RDX,U64 CHashTable.body[RDI]
        LEA	RAX,U64 [RDX+RAX*8]
        POP	RDX
        RET
        _HASH_STR::
        PUSH	RBP
        MOV	RBP,RSP
        PUSH	RSI
        MOV	RSI,U64 SF_ARG1[RBP]
        CALL	SYS_HASH_STR
        POP	RSI
        POP	RBP
        RET1	8
        _HASH_FIND::
        PUSH	RBP
        MOV	RBP,RSP
        PUSH	RSI
        PUSH	RDI
        MOV	RSI,U64 SF_ARG1[RBP]
        MOV	RDI,U64 SF_ARG2[RBP]
        MOV	RBX,U64 SF_ARG3[RBP]
        MOV	RCX,U64 SF_ARG4[RBP]
        CALL	SYS_HASH_FIND
        POP	RDI
        POP	RSI
        POP	RBP
        RET1	32
        _HASH_SINGLE_TABLE_FIND::
        PUSH	RBP
        MOV	RBP,RSP
        PUSH	RSI
        PUSH	RDI
        MOV	RSI,U64 SF_ARG1[RBP]
        MOV	RDI,U64 SF_ARG2[RBP]
        MOV	RBX,U64 SF_ARG3[RBP]
        MOV	RCX,U64 SF_ARG4[RBP]
        CALL	SYS_HASH_STR
        CALL	SYS_HASH_SINGLE_TABLE_FIND
        POP	RDI
        POP	RSI
        POP	RBP
        RET1	32
        _HASH_BUCKET_FIND::
        PUSH	RBP
        MOV	RBP,RSP
        PUSH	RSI
        PUSH	RDI
        MOV	RSI,U64 SF_ARG1[RBP]
        MOV	RDI,U64 SF_ARG2[RBP]
        CALL	SYS_HASH_BUCKET_FIND
        POP	RDI
        POP	RSI
        POP	RBP
        RET1	16
        _HASH_ADD::
        PUSH	RBP
        MOV	RBP,RSP
        PUSH	RSI
        PUSH	RDI
        MOV	RCX,U64 SF_ARG1[RBP]
        MOV	RSI,U64 CHash.str[RCX]
        MOV	RDI,U64 SF_ARG2[RBP]
        CALL	SYS_HASH_BUCKET_FIND
        MOV	RCX,U64 SF_ARG1[RBP]
        PUSHFD
        MOV	RBX,U64 [RAX]
        MOV	U64 CHash.next[RCX],RBX
        MOV	U64 [RAX],RCX

        POPFD
        POP	RDI
        POP	RSI
        POP	RBP
        RET1	16
        _HASH_ADD_AFTER::
        PUSH	RBP
        MOV	RBP,RSP
        PUSH	RDI
        MOV	RCX,U64 SF_ARG1[RBP]
        MOV	RDI,U64 SF_ARG3[RBP]
        PUSHFD
        MOV	RAX,SF_ARG2[RBP]
        MOV	RBX,U64 [RAX]
        MOV	U64 CHash.next[RCX],RBX
        MOV	U64 [RAX],RCX

        POPFD
        POP	RDI
        POP	RBP
        RET1	24
        _HASH_REM_DEL::
        PUSH	RBP
        MOV	RBP,RSP
        PUSH	RSI
        PUSH	RDI
        MOV	RCX,U64 SF_ARG1[RBP]
        TEST	RCX,RCX
        JZ	@@10
        MOV	RSI,U64 CHash.str[RCX]
        XOR	RBX,RBX
        MOV	EBX,U32 CHash.type[RCX]
        AND	EBX,~HTG_FLAGS_MASK&0xFFFFFFFF
        MOV	RDI,U64 SF_ARG2[RBP]
        MOV	RCX,U64 SF_ARG3[RBP]
        CALL	SYS_HASH_STR

        PUSHFD
        CALL	SYS_HASH_SINGLE_TABLE_FIND
        JZ	@@05
        CMP	RAX,U64 SF_ARG1[RBP]
        JNE	@@05

        MOV	RBX,U64 CHash.next[RAX]
        MOV	U64 [RDX],RBX

        POPFD

        PUSH_C_REGS
        PUSH	RAX
        CALL	&HashDel;
        POP_C_REGS

        POP	RDI
        POP	RSI
        MOV	RAX,1
        POP	RBP
        RET1	24

@@05:	POPFD
@@10:	POP	RDI
        POP	RSI
        XOR	RAX,RAX
        POP	RBP
        RET1	24
    }
_extern _HASH_ADD_AFTER U0 HashAddAfter(CHash *tmph,
        CHash *pred,CHashTable *table); //Add hash entry after entry.
_extern _HASH_STR I64 HashStr(U8 *st); //Hash a string.
_extern _HASH_FIND CHash *HashFind(U8 *needle_str,CHashTable *haystack_table,
                                   I64 mask,I64 instance=1);//Find string in hash table.
_extern _HASH_SINGLE_TABLE_FIND CHash *HashSingleTableFind(U8 *needle_str,
        CHashTable *haystack_table,
        I64 mask,I64 instance=1);//Find string in single hash table.
_extern _HASH_BUCKET_FIND CHash **HashBucketFind(U8 *needle_str,
        CHashTable *haystack_table); //Find hash bucket.
_extern _HASH_ADD U0 HashAdd(CHash *tmph,
                             CHashTable *table); //Add entry to hash table.
_extern _HASH_REM_DEL Bool HashRemDel(CHash *tmph,CHashTable *table,
                                      I64 instance=1);//Remove hash entry and del. Instance must match.

U0 HashSrcFileSet(CCmpCtrl *cc,CHashSrcSym *h,I64 line_num_offset=0)
{   //Set \dLK,"CHashSrcSym",A="MN:CHashSrcSym"\d link and help_index by cur cc pos.
    CLexFile *tmpf=cc->lex_include_stk;
    I64 line_num=tmpf->line_num+line_num_offset;
    if (line_num<1) line_num=1;
    Free(h->src_link);
    h->src_link=MStrPrint("FL:%s,%d",tmpf->full_name,line_num);
    if (Bt(&cc->opts,OPTf_KEEP_PRIVATE))
        h->type|=HTF_PRIVATE;
    Free(h->idx);
    if (cc->cur_help_idx && *cc->cur_help_idx)
        h->idx=StrNew(cc->cur_help_idx);
    else
        h->idx=NULL;
}
I64 HashTypeNum(CHash *tmph)
{   //Return bit num of hash type, limited to just types.
    if (tmph)
        return Bsf(tmph->type&HTG_TYPE_MASK);
    else
        return -1;
}

I64 HashVal(CHash *tmph)
{   //Returns most likely desired value.
    switch (HashTypeNum(tmph)) {
    case HTt_EXPORT_SYS_SYM:
        return tmph(CHashExport *)->val;
    case HTt_IMPORT_SYS_SYM:
        return tmph(CHashImport *)->module_base;
    case HTt_DEFINE_STR:
    case HTt_CLASS:
    case HTt_INTERNAL_TYPE:
    case HTt_WORD:
    case HTt_DICT_WORD:
    case HTt_OPCODE:
    case HTt_HELP_FILE:
        return tmph;
    case HTt_GLBL_VAR:
        if (tmph(CHashGlblVar *)->flags&GVF_EXTERN)
            return &tmph(CHashGlblVar *)->data_addr;
        else
            return tmph(CHashGlblVar *)->data_addr;
    case HTt_FUN:
        if (Bt(&tmph(CHashFun *)->flags,Cf_EXTERN))
            return tmph;
        else
            return tmph(CHashFun *)->exe_addr;
    case HTt_REG:
        return tmph(CHashReg *)->reg_num|tmph(CHashReg *)->reg_type<<8;
    case HTt_KEYWORD:
    case HTt_ASM_KEYWORD:
    case HTt_MODULE:
    case HTt_FILE:
    case HTt_FRAME_PTR:
        return tmph(CHashGeneric *)->user_data0;

    case -1:		//nobound switch
    case HTt_TYPES_NUM: //nobound switch
    default:
        return 0;
    }
}

CHashTable *HashTableNew(I64 size,CTask *t=NULL)
{   //New hash table, power-of-two in size.
    CHashTable *table;
    table=MAlloc(sizeof(CHashTable));
    table->body=MAlloc(size<<3);
    table->mask=size-1;
    return table;
}

U0 HashTableDel(CHashTable *table)
{   //Free std system hash table, calling \dLK,"HashDel",A="MN:HashDel"\d() on entries.
    I64 i;
    CHashSrcSym *tmph,*tmph1;
    if (!table) return;
    for (i=0; i<=table->mask; i++) {
        tmph=table->body[i];
        while (tmph) {
            tmph1=tmph->next;
            HashDel(tmph);
            tmph=tmph1;
        }
    }
    Free(table->body);
    Free(table);
}

I64 HashTablePurge(CHashTable *table)
{   //Eliminate ExportSysSyms that have been usurped.
    I64 i,res=0;
    CHashSrcSym *tmph,*tmph1,*tmph2;
    if (!table) return 0;
    asm {
        PUSHFD
    }
    for (i=0; i<=table->mask; i++) {
        tmph=table->body[i];
        while (tmph) {
            tmph1=tmph->next; //We delete only older ones
            if (tmph->type&(HTT_FUN|HTT_GLBL_VAR)) {
                tmph2=tmph->next; //Older always later in chain
                while (tmph2) {
                    if ((tmph2->type&HTT_EXPORT_SYS_SYM ||
                            tmph2->type&HTG_TYPE_MASK==HTT_INVALID) &&
                            !StrCmp(tmph2->str,tmph->str)) {
                        if (tmph2->type&HTG_TYPE_MASK==HTT_INVALID)
                            tmph2->type=HTT_KEYWORD;//Won't delete HTT_INVALID
                        HashRemDel(tmph2,table);
                        res++;
                        break;
                    }
                    tmph2=tmph2->next;
                }
            }
            tmph=tmph1;
        }
    }
    asm {POPFD;};
    return res;
}

CHashGeneric *HashGenericAdd(U8 *name,I64 type,
                             I64 u0=0,I64 u1=0,I64 u2=0,CTask *task=NULL)
{   //Add any type to task hash_table, 3 user_data values.
    if (!task) task=Fs;
    CHashGeneric *res=MAlloc(sizeof(CHashGeneric));
    res->type=type;
    res->user_data0=u0;
    res->user_data1=u1;
    res->user_data2=u2;
    res->str=StrNew(name);
    HashAdd(res,Fs->hash_table);
    return res;
}

U0 HashSrcFileSet(CCmpCtrl *cc,CHashSrcSym *h,I64 line_num_offset=0)
{   //Set \dLK,"CHashSrcSym",A="MN:CHashSrcSym"\d link and help_index by cur cc pos.
    CLexFile *tmpf=cc->lex_include_stk;
    I64 line_num=tmpf->line_num+line_num_offset;
    if (line_num<1) line_num=1;
    Free(h->src_link);
    h->src_link=MStrPrint("FL:%s,%d",tmpf->full_name,line_num);
    if (Bt(&cc->opts,OPTf_KEEP_PRIVATE))
        h->type|=HTF_PRIVATE;
    Free(h->idx);
    if (cc->cur_help_idx && *cc->cur_help_idx)
        h->idx=StrNew(cc->cur_help_idx);
    else
        h->idx=NULL;
}

CHashGeneric *HashPublic(U8 *st,I64 mask,Bool val=TRUE)
{   //Mark a hash entry as public and \dLK,"HashSrcFileSet",A="MN:HashSrcFileSet"\d().
    CHashGeneric *res;
    if (res=HashFind(st,Fs->hash_table,mask)) {
        if (val)
            res->type|=HTF_PUBLIC;
        else
            res->type&=~HTF_PUBLIC;
        if (res->type&HTG_SRC_SYM)
            HashSrcFileSet(Fs->last_cc,res);
        return res;
    } else
        return NULL;
}

I64 HashLstAdd(U8 *lst,I64 type,CHashTable *table)
{   //Add a list to a hash table.
    I64 i=0;
    CHashGeneric *tmph;
    if (lst) {
        while (*lst) {
            if (*lst=='@')
                lst++;
            else
                i++;
            tmph=MAlloc(sizeof(CHashGeneric));
            tmph->user_data0=i-1;
            tmph->str=StrNew(lst);
            tmph->type=type;
            HashAdd(tmph,table);
            while (*lst++);
        }
    }
    return i;
}

I64 HashDefineLstAdd(U8 *dname,I64 type,CHashTable *table)
{   //Add define list to a hash table. See \dLK,"::/Adam/DolDoc/DocInit.HC",A="FF:::/Adam/DolDoc/DocInit.HC,HashDefineLstAdd"\d.
    CHashDefineStr *tmph;
    if (tmph=HashFind(dname,Fs->hash_table,HTT_DEFINE_STR))
        return HashLstAdd(tmph->data,type,table);
    else
        return 0;
}

I64 FramePtr(U8 *name,CTask *task=NULL)
{   //Find entry in task->hash_table, Return user_data.
    CHashGeneric *tmph;
    if(!task) task=Fs;
    if (tmph=HashFind(name,task->hash_table,HTT_FRAME_PTR))
        return tmph->user_data0;
    else
        return 0;
}

CHashGeneric *FramePtrAdd(U8 *name,I64 val=0,CTask *task=NULL)
{   //Add named value to task->hash_table.
    return HashGenericAdd(name,HTT_FRAME_PTR,val,0,0);
}

I64 FramePtrSet(U8 *name,I64 val,CTask *t=NULL)
{   //Find hash entry in task->hash_table. Change user_data0.
    CHashGeneric *tmph;
    I64 ret;
    if (tmph=HashFind(name,Fs->hash_table,HTT_FRAME_PTR)) {
        ret=val,SwapI64(&tmph->user_data0,&ret);
        return ret;
    } else
        return 0;
}

I64 FramePtrDel(U8 *name,CTask *t=NULL)
{   //Remove entry and delete.
    CHashGeneric *tmph;
    I64 res=0;
    if (tmph=HashFind(name,Fs->hash_table,HTT_FRAME_PTR)) {
        res=tmph->user_data0;
        HashRemDel(tmph,Fs->hash_table);
    }
    return res;
}

