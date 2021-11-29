U8 *Vars[]={
    "fF64",
    "rF64",
    "gF64",
    "*iF64",
    "fI64",
    "rI64",
    "gI64",
    "*iI64",
};
I64 cnt=sizeof(Vars)/sizeof(*Vars);
U0 GenTest() {
    "F64 gF64;\n";
    "I64 gI64;\n";
    "U0 AssignFuzz() {\n";
    "\tF64 rF64;\n";
    "\tF64 fF64,fF64_2;\n";
    "\tF64 *iF64=&fF64_2;\n";
    "\tI64 rI64;\n";
    "\tI64 fI64,fI642;\n";
    "\tI64 *iI64=&fI64_2;\n";
    "\t\"%%p,%%p\",&fI64,&fF64;\n";
    I64 a,b;
    for(a=0;a!=cnt;a++) {
        for(b=0;b!=cnt;b++) {
            "\t%s=0;%s=10;\n",Vars[a],Vars[b];
            "\t%s=%s;\n",Vars[a],Vars[b];
            "\t\"%s=%s:%%d\\n\",%s==10;\n",Vars[a],Vars[b],Vars[a];
        }
    }
    "}";
    "AssignFuzz;\n";
}
GenTest;


