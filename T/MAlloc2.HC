I64 boundscheck_enable=0;
//This includes stuff for bounds checking
U8 *MAlloc(I64 sz,CTask *t=NULL) {
	U8 *ret;
	ret=__MAlloc(sz,t);
	SetPtrCallers(
		ret,
		5,
		Caller(1),
		Caller(2),
		Caller(3),
		Caller(4),
		Caller(5)
	);
	return ret;
}
U8 *MAlloc32(I64 sz,CTask *t=NULL) {
	U8 *ret;
	ret=__MAlloc32(sz,t);
	SetPtrCallers(
		ret,
		5,
		Caller(1),
		Caller(2),
		Caller(3),
		Caller(4),
		Caller(5)
	);
	return ret;
}
U0 Free(U8 *ptr) {
	if(!ptr) return;
	SetPtrCallers(
		ptr,
		5,
		Caller(1),
		Caller(2),
		Caller(3),
		Caller(4),
		Caller(5)
	);
	__Free(ptr);
}
U8 *CAlloc(I64 sz,CTask *t=NULL) {
	return MAlloc(sz,t);
}
U0 OOBBacktrace(U8 *ptr,I64 len=5,U8 *got) {
	//In poopalloc.c callers are stored before ptr
	I64 old=boundscheck_enable;
	boundscheck_enable=0;
	TOSPrint("Out of bounds at:\n");
	U8 **p=ptr,*link,*par,*fstr;
	CHashFun *f;
	I64 i=0;
	for(i=0;i!=len;i++) {
		link=SrcEdLink(par=Caller(i+2)); //+1 for self,+1 for WhineOOB
		f=FunSegFind(par,NULL);
		if(f)
			fstr=f->str;
		else
			fstr="???";
		if(link) {
			TOSPrint("%s(%s)\n",link,fstr);
			Free(link);
		} else 
			TOSPrint("???\n");
	}
	if(ptr!=INVALID_PTR&&ptr) {
		if(MSize(ptr)!=-1)
			TOSPrint("%p(size %d) was allocated at (%d bytes OOB):\n",ptr,MSize(ptr),got-(ptr+MSize(ptr)));
		else
			TOSPrint("%p was free'd at:\n");
		p-=len;
		while(--len>=0) {
			link=SrcEdLink(par=*p++);
			f=FunSegFind(par,NULL);
			if(f)
				fstr=f->str;
			else
				fstr="???";
			if(link) {
				TOSPrint("%s(%s)\n",link,fstr);
				Free(link);
			} else 
				TOSPrint("???\n");
		}
	}
	boundscheck_enable=old;
}
U8 *InBounds(U8 *ptr,I64 sz) {
	return __InBounds(ptr,sz);
}
asm {
	SYS_WHINE_OOB::
	PUSH RBP
	MOV RBP,RSP
	CMP U64 [&boundscheck_enable],0
	JE @@ret
	PUSH RAX
	PUSH RBX
	PUSH RCX
	PUSH RDX
	PUSH RSI
	PUSH RDI
	PUSH R8
	PUSH R9
	PUSH R10
	PUSH R11
	PUSH R12
	PUSH R13
	PUSH R14
	PUSH R15
	PUSH U64 SF_ARG2[RBP]
	PUSH U64 SF_ARG1[RBP]
	CALL &InBounds
	CMP RAX,0
	JE @@exit
	PUSH U64 SF_ARG1[RBP]
	PUSH 5
	PUSH RAX
	CALL &OOBBacktrace
	@@exit:
	POP R15
	POP R14
	POP R13
	POP R12
	POP R11
	POP R10
	POP R9
	POP R8
	POP RDI
	POP RSI
	POP RDX
	POP RCX
	POP RBX
	POP RAX
	@@ret:
	POP RBP
	RET1 0x10
};
