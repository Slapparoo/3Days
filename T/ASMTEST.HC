"SKIPPY\n";
asm {
	Hai::
	MOV RAX,"TAODS"
	PUSH RAX
	MOV RCX,&Print
	CALL RCX
	POP RAX
	RET
};
"TAOD\n";
_import Hai U0 Hello();
Hello;
