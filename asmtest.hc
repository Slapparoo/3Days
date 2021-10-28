U0 Hello() {
	"HELLO WORLD\n";
}
/*
U0 Pile() {
  asm {
    PUSH 0x7f;
    PUSH 0x7fff;
    PUSH 0x7ffff;
    //PUSH R12u16; //32bit only
    PUSH R12u32;  //Weird
    PUSH R12u64;
    //PUSH I16 [R12]; //32bit only
    PUSH I32 [R12]; //Weird
    PUSH I64 [R12];
    //
    MOV AL, I8 [0x1122334455];
    MOV AX, I16 [0x1122334455];
    MOV EAX, I32 [0x1122334455];
    MOV I8 [0x1122334455],AL;
    MOV I16 [0x1122334455],AX;
    MOV I32 [0x1122334455], EAX;
    MOV R9u8, I8 [RBX+4*RCX];
    MOV R9u16, I16 [RBX+4*RCX];
    MOV R9u32, I32 [RBX+4*RCX];
    MOV R9u64, I64 [RAX+4*RDX];
    MOV I8 [RBX+4*RCX], R9u8;
    MOV I16 [RBX+4*RCX], R9u16;
    MOV I32 [RBX+4*RCX], R9u32;
    MOV I64 [RAX+4*RDX], R9u64;
    MOV R9u16, CS;
    MOV CS, R10u16;
    MOV R12u8,0xff;
    MOV R12u16,0x7fff;
    MOV R12u32,0x11223344;
    MOV R12,0x1122334455;
    MOV I8 [R12], 0xff;
    MOV I16 [R12], 0x7fff;
    MOV I32 [R12], 0x11223344;
    MOV I64 [R12], 0x11223344;
    ADC AL, 0xff;
    ADC AX, 0x7fff;
    ADC EAX, 0x11223344;
    ADC I8 [R13],0x7f;
    ADC I16 [R13],0x7fff;
    ADC I32 [R13],0x7ffff;
    ADC I64 [R13],0x7ffff;
    ADC I8 [R13],R13u8;
    ADC I16 [R13],R13u16;
    ADC I32 [R13],R13u32;
    ADC I64 [R13],R13;
    ADC R13u8, I8 [R13];
    ADC R13u16, I16 [R13];
    ADC R13u32, I32 [R13];
    ADC R13, I64 [R13];
    NOP;
    NOP2;
    CMOVC RAX,I64 [RAX];
    MOV R12,10
  loop:
    IMPORT Hello;
    MOV RAX, Hello;
    CALL RAX;
    DEC R12;
    CMP R12,0;
    JNE loop;
  };
}
*/
U0 Dummy() {
asm {
  C_HELLO::
    IMPORT Hello;
  PUSH R12;
  MOV R12,10
 Loop:
  MOV RAX,Hello;
  CALL RAX;
  MOV RAX,Loop-10
  DEC R12;
  CMP R12,0;
  JNE Loop;
 en:
  POP R12;
  RET;
 indir:
  DU64 Loop-10;
  
};
}
U0 Foo() {
	asm {
		IMPORT Hello;
		MOV RAX, Hello;
		CALL RAX;
		
	};
	return;
}
_import C_HELLO U0 Hi();
Hi;
