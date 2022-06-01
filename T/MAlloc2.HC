//This includes stuff for bounds checking
U8 *MAlloc(I64 sz,CTask *t=NULL) {
	U8 *ret;
	ret=__MAlloc(sz,t);
	return ret;
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
	return ret;
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
U0 OOBBacktrace(U8 *ptr,I64 len=5) {
	//In poopalloc.c callers are stored before ptr
	TOSPrint("Out of bounds at:\n");
	U8 **p=ptr,*link;
	I64 i=0;
	for(i=0;i!=len;i++) {
		link=SrcEdLink(Caller(i+2)); //+1 for self,+1 for WhineOOB
		if(link) {
			TOSPrint("%s\n",link);
			Free(link);
		} else 
			TOSPrint("???\n");
	}
	if(ptr!=INVALID_PTR&&ptr) {
		TOSPrint("%p was allocated at :\n",ptr);
		while(--len>=0) {
			link=SrcEdLink(*--p);
			if(link) {
				TOSPrint("%s\n",link);
				Free(link);
			} else 
				TOSPrint("???\n");
		}
	}
}
U8 *InBounds(U8 *ptr,I64 sz) {
	return __InBounds(ptr,sz);
}
asm {
	_WHINE_OOB::
	PUSH RBP
	MOV RBP,RSP
	PUSH_REGS
	PUSH U64 SF_ARG2[RBP]
	PUSH U64 SF_ARG1[RBP]
	CALL &InBounds
	CMP RAX,0
	JE @@exit
	PUSH 5
	PUSH RAX
	CALL &OOBBacktrace
	@@exit:
	POP_REGS
	LEAVE
	RET1 0x10
}
_extern _WHINE_OOB U0 WhineOOB(U8 *ptr,I64 sz);
