/*  Kernel ASM symbols.
\dLK,"KernelB",A="FF:::/Kernel/Kernel.PRJ,KernelB"\d Kernel.PRJ
\dLK,"KernelB",A="FF:::/Compiler/Compiler.PRJ,KernelB"\d Compiler.PRJ
\dLK,"KernelB",A="FF:::/StartOS.HC,KernelB"\d StartOS.HC
*/
public _extern _BEQU Bool BEqu(
	U8 *bit_field,I64 bit,Bool val);//Set bit equ to val.
public _extern _BIT_FIELD_EXT_U32 U32 BFieldExtU32(
	U8 *bit_field,I64 bit,I64 size); //Extract U32 from bit field.
public _extern _BIT_FIELD_OR_U32 U0 BFieldOrU32(
	U8 *bit_field,I64 bit,U32 pattern); //Or U32 into bit field.
public _extern _LBEQU Bool LBEqu(
	U8 *bit_field,I64 bit,Bool val); //Locked Set bit equ to val.

public _extern _CALL I64 Call(U8 *machine_code); //Call addr with no args.
public _extern _CALLEXTSTR I64 CallExtStr(
	/*argpop*/ U8 *name,...); //Search sym table for fun and pass it args.
//Can have any fixed number of arguments.
public _extern _CALL_IND I64 CallInd(
	/*argpop*/I64 (*fp_addr)(...),...); //Call with fixed number of args.

public _extern _ARG F64 Arg(F64 x,F64 y); //Polar coordinate angle.
public _extern _CEIL F64 Ceil(F64 d); //Ceiling of F64.
public _extern _CLAMP_I64 I64 ClampI64(
	I64 num,I64 lo,I64 hi); //Clamp to I64 [] range.
public _extern _CLAMP_U64 U64 ClampU64(
	U64 num,U64 lo,U64 hi); //Clamp to U64 [] range.
public _extern _EXP F64 Exp(F64 d); //Exponential function.
_extern _FCLEX U0 Fclex();
_extern _FLDCW U0 Fldcw(U16 w);
public _extern _FLOOR F64 Floor(F64 d); //Floor of F64.
_extern _FSTCW U16 Fstcw();
_extern _FSTSW U16 Fstsw();
public _extern _LN  F64 Ln(F64 d); //Logarithm.
public _extern _LOG10 F64 Log10(F64 d); //Log base 10.
public _extern _LOG2 F64 Log2(F64 d); //Log base 2.
public _extern _POW F64 Pow(F64 base,F64 power); //F64 base to a power.
public _extern _POW10 F64 Pow10(F64 d); //Ten to the dth power.
public _extern _ROUND F64 Round(F64 d); //Round F64 to whole number.
public _extern _SIGN F64 Sign(F64 d); //Sign of F64: -1,0,1
public _extern _TRUNC F64 Trunc(F64 d); //Truncate F64.

public _extern _D3_ADD CD3 *D3Add(CD3 *sum,CD3 *d1,CD3 *d2); //sum=d1+d2
public _extern _D3_ADD_EQU CD3 *D3AddEqu(CD3 *sum,CD3 *d); //sum+=d
public _extern _D3_COPY CD3 *D3Copy(CD3 *dst,CD3 *src); //dst=src
public _extern _D3_CROSS CD3 *D3Cross(CD3 *prod,CD3 *d1,CD3 *d2); //prod=d1xd2
public _extern _D3_DIST F64 D3Dist(CD3 *d1,CD3 *d2); //Distance
public _extern _D3_DIST_SQR F64 D3DistSqr(CD3 *d1,CD3 *d2); //Distance Squared
public _extern _D3_DIV CD3 *D3Div(CD3 *quot,CD3 *d,F64 s); //quot=d/s
public _extern _D3_DIV_EQU CD3 *D3DivEqu(CD3 *quot,F64 s); //quot/=s
public _extern _D3_DOT F64 D3Dot(CD3 *d1,CD3 *d2); //d1 dot d2
public _extern _D3_EQU CD3 *D3Equ(
	CD3 *dst,F64 x=0,F64 y=0,F64 z=0); //dst=(x,y,z)
public _extern _D3_MUL CD3 *D3Mul(CD3 *prod,F64 s,CD3 *d); //prod=s*d
public _extern _D3_MUL_EQU CD3 *D3MulEqu(CD3 *prod,F64 s); //prod*=s
public _extern _D3_NORM F64 D3Norm(CD3 *d); //Norm
public _extern _D3_NORM_SQR F64 D3NormSqr(CD3 *d); //Norm Squared
public _extern _D3_SUB CD3 *D3Sub(CD3 *diff,CD3 *d1,CD3 *d2); //diff=d1-d2
public _extern _D3_SUB_EQU CD3 *D3SubEqu(CD3 *diff,CD3 *d); //diff-=d
public _extern _D3_UNIT CD3 *D3Unit(CD3 *d); //To unit vect
public _extern _D3_ZERO CD3 *D3Zero(CD3 *dst); //To zero


public _extern _LXCHG_I64 I64 LXchgI64(I64 *dst,I64 d); //Locked eXchange I64s.
public _extern _LXCHG_U16 U16 LXchgU16(U16 *dst,U16 d); //Locked eXchange U16s.
public _extern _LXCHG_U32 U32 LXchgU32(U32 *dst,U32 d); //Locked eXchange U32s.
public _extern _LXCHG_U8  U8 LXchgU8(U8 *dst,U8 d); //Locked eXchange U8s.
public _extern _SET_MSR U0 SetMSR(
	I64 model_specific_reg,I64 val); //Model Specific Reg See \dLK,"MSRs",A="MN:IA32_EFER"\d.
public _extern _SYS_HLT U0 SysHlt(); //Loops doing HLT inst.
public _extern _XCHG_I64 I64 XchgI64(I64 *dst,I64 d); //eXchange I64s.
public _extern _XCHG_U16 U16 XchgU16(U16 *dst,U16 d); //eXchange U16s.
public _extern _XCHG_U32 U32 XchgU32(U32 *dst,U32 d); //eXchange U32s.
public _extern _XCHG_U8  U8 XchgU8(U8 *dst,U8 d); //eXchange U8s.
public _extern _ARG F64 Arg(F64 x,F64 y); //Polar coordinate angle.
public _extern _CEIL F64 Ceil(F64 d); //Ceiling of F64.
public _extern _CLAMP_I64 I64 ClampI64(
	I64 num,I64 lo,I64 hi); //Clamp to I64 [] range.
public _extern _CLAMP_U64 U64 ClampU64(
	U64 num,U64 lo,U64 hi); //Clamp to U64 [] range.
public _extern _EXP F64 Exp(F64 d); //Exponential function.
_extern _FCLEX U0 Fclex();
_extern _FLDCW U0 Fldcw(U16 w);
public _extern _FLOOR F64 Floor(F64 d); //Floor of F64.
_extern _FSTCW U16 Fstcw();
_extern _FSTSW U16 Fstsw();
public _extern _LN  F64 Ln(F64 d); //Logarithm.
public _extern _LOG10 F64 Log10(F64 d); //Log base 10.
public _extern _LOG2 F64 Log2(F64 d); //Log base 2.
public _extern _POW F64 Pow(F64 base,F64 power); //F64 base to a power.
public _extern _POW10 F64 Pow10(F64 d); //Ten to the dth power.
public _extern _ROUND F64 Round(F64 d); //Round F64 to whole number.
public _extern _SIGN F64 Sign(F64 d); //Sign of F64: -1,0,1
public _extern _TRUNC F64 Trunc(F64 d); //Truncate F64.
public _extern _MEMCMP I64 MemCmp(
	U8 *ptr1,U8 *ptr2,I64 cnt); //Compare chunk of memory.
public _extern _MEMCPY U8 *MemCpy(
	U8 *dst,U8 *src,I64 cnt); //Copy chunk of memory. Only goes fwd.
public _extern _MEMSET U8 *MemSet(
	U8 *dst,I64 val,I64 cnt); //Set chunk of U8s to value.
public _extern _MEMSET_I64 I64 *MemSetI64(
	I64 *dst,I64 val,I64 I64cnt); //Set chunk of I64s to value.
public _extern _MEMSET_U16 U16 *MemSetU16(
	U16 *dst,I64 val,I64 U16cnt); //Set chunk of U16s to value.
public _extern _MEMSET_U32 U32 *MemSetU32(
	U32 *dst,I64 val,I64 U32cnt); //Set chunk of U32s to value.
public _extern _MEMSET U8 *MemSetU8(
	U8 *dst,I64 val,I64 U8cnt); //Set chunk of U8s to value.
public _extern _HASH_REM_DEL Bool HashRemDel(CHash *tmph,CHashTable *table,
	I64 instance=1);//instance must match tmph's

#ifdef BOOT_3DAYS
_extern _SYS_WHINE_OOB U0 SYS_WHINE_OOB;
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

_extern _STRCMP I64 StrCmp(
	U8 *st1,U8 *st2); //Compare two strings.
_extern _STRICMP I64 StrICmp(
	U8 *st1,U8 *st2); //Compare two strings, ignoring case.
_extern _STRNCMP I64 StrNCmp(
	U8 *st1,U8 *st2,I64 n); //Compare N bytes in two strings.
_extern _STRNICMP I64 StrNICmp(
	U8 *st1,U8 *st2,I64 n); //Compare N bytes in two strings, ignoring case.
_extern _STRMATCH U8 *StrMatch(
	U8 *needle,U8 *haystack_str); //Scan for string in string.
_extern _STRIMATCH U8 *StrIMatch(
	U8 *needle,U8 *haystack_str);//Scan for string in string, ignoring case.
_extern _STRCPY U0 StrCpy(
	U8 *dst,U8 *src); //Copy string.
_extern _FREE U0 Free(U8 *addr); //Free $LK,"MAlloc",A="MN:MAlloc"$()ed memory chunk.
_extern _MSIZE I64 MSize(U8 *src); //Size of heap object.
_extern _MSIZE2 I64 MSize2(U8 *src); //Internal size of heap object.
_extern _MHEAP_CTRL CHeapCtrl *MHeapCtrl(U8 *src); //$LK,"CHeapCtrl",A="MN:CHeapCtrl"$ of object.
//_extern _MALLOC U8 *MAlloc(I64 size,CTask *mem_task=NULL); //Alloc memory chunk.
_extern _BREAK U0 Break();

public _extern SYS_PROGRESS1 I64 progress1; //Current progress 1.
public _extern SYS_PROGRESS1_DESC
	U8 progress1_desc[PROGRESS_DESC_LEN]; //Progress 1 desc.
public _extern SYS_PROGRESS1_MAX I64 progress1_max; //Progress 1 max.
public _extern SYS_PROGRESS1_T0 F64 progress1_t0; //Progress 1 start time.
public _extern SYS_PROGRESS1_TF F64 progress1_tf; //Progress 1 end time.
public _extern SYS_PROGRESS2 I64 progress2; //Current progress 2.
public _extern SYS_PROGRESS2_DESC
	U8  progress2_desc[PROGRESS_DESC_LEN]; //Progress 2 desc.
public _extern SYS_PROGRESS2_MAX I64 progress2_max; //Progress 2 max.
public _extern SYS_PROGRESS2_T0 F64 progress2_t0; //Progress 2 start time.
public _extern SYS_PROGRESS2_TF F64 progress2_tf; //Progress 2 end time.
public _extern SYS_PROGRESS3 I64 progress3; //Current progress 3.
public _extern SYS_PROGRESS3_DESC
	U8  progress3_desc[PROGRESS_DESC_LEN]; //Progress 3 desc.
public _extern SYS_PROGRESS3_MAX I64 progress3_max; //Progress 3 max.
public _extern SYS_PROGRESS3_T0 F64 progress3_t0; //Progress 3 start time.
public _extern SYS_PROGRESS3_TF F64 progress3_tf; //Progress 3 end time.
public _extern SYS_PROGRESS4 I64 progress4; //Current progress 4.
public _extern SYS_PROGRESS4_DESC
	U8  progress4_desc[PROGRESS_DESC_LEN]; //Progress 4 desc.
public _extern SYS_PROGRESS4_MAX I64 progress4_max; //Progress 4 max.
public _extern SYS_PROGRESS4_T0 F64 progress4_t0; //Progress 4 start time.
public _extern SYS_PROGRESS4_TF F64 progress4_tf; //Progress 4 end time.
public _extern SYS_PROGRESSES
	CProgress sys_progresses[PROGRESS_BARS_NUM]; //All progress bars.
public _extern _TEXT_CHAR Bool TextChar(CTask *task,Bool allow_border=FALSE,
I64 x,I64 y,I64 d); //Plot char. 30fps in Fs->draw_it() callback.
public _extern _TEXT_LEN_STR Bool TextLenStr(CTask *task,I64 x,I64 y,I64 len,
I64 attr,U8 *s); //Plot str with len.  30fps in Fs->draw_it() callback.
public _extern _TEXT_LEN_ATTR_STR Bool TextLenAttrStr(CTask *task,I64 x,I64 y,
I64 len,U32 *_attr); //Plot attr str with len.	30fps in Fs->draw_it() callback.
public _extern _TEXT_LEN_ATTR Bool TextLenAttr(CTask *task,I64 x,I64 y,I64 len,
I64 attr); //Plot attrs with len.  30fps in Fs->draw_it() callback.
_extern _MALLOC U8 *MAlloc(I64 size,CTask *mem_task=NULL); //Alloc memory chunk.
#endif
