U0 FuzzGen() {
    "U0 Fuzz() {\n";
    U8 *asn_ops[]={
        "=",
        "+=",
        "-=",
        "*=",
        "/=",
        "%=",
        "<<=",
        ">>=",
        "&=",
        "^=",
        "|=",
    }, *binops[]={
        "+",
        "-",
        "*",
        "&",
        "/",
        "<<",
        ">>",    
        ">",    
        "<",
        ">=",
        "<=",
        "==",
        "!=",
        "^",
        "|",
        "&&",
        "||",
        "^^",
        ","            
    };
    I64 cnt1,cnt2,cnt3;
    cnt1=sizeof(asn_ops)/sizeof(*asn_ops);
    "I64i v1,v3=2;\n";
    "F64 v2,v4=2.;\n";    
    while(--cnt1>=0) {
        for(cnt3=0;cnt3!=2;cnt3++)    
        for(cnt2=0;cnt2!=2;cnt2++) {
            if(cnt2)
                "v1=1;\nv1";
            else
                "v2=1.;\nv2";
            "%s",asn_ops[cnt1];
            if(cnt3)
                "v3;\n";
            else
                "2;\n";
            if(cnt2)
                "\"1%s2 is %%d\\n\",v1;\n",asn_ops[cnt1];
            else
                "\"1%s2 is %%n\\n\",v2;\n",asn_ops[cnt1];
            
        }
    }
    "v1=1,v2=1.;\n";
    "\"%%d,v1++\\n\",v1++;\n";
    "\"%%n,v2++\\n\",v2++;\n";
    "\"%%d,v1\\n\",v1;\n";
    "\"%%n,v2\\n\",v2;\n";
    "v1=1,v2=1.;\n";
    "\"%%d,++v1\\n\",++v1;\n";
    "\"%%n,++v2\\n\",++v2;\n";
    "\"%%d,v1\\n\",v1;\n";
    "\"%%n,v2\\n\",v2;\n";
    
    
    "v1=1,v2=1.;\n";
    "\"%%d,--v1\\n\",--v1;\n";
    "\"%%n,--v2\\n\",--v2;\n";
    "\"%%d,v1\\n\",v1;\n";
    "\"%%n,v2\\n\",v2;\n";
    "\"%%d,v1--\\n\",v1--;\n";
    "\"%%n,v2--\\n\",v2--;\n";
 
    cnt1=sizeof(binops)/sizeof(*binops);   
    while(--cnt1>=0) {
        for(cnt3=0;cnt3!=2;cnt3++)
        for(cnt2=0;cnt2!=2;cnt2++) {
            if(cnt2)
                "v1=1;\nv1=v1 ";
            else
                "v2=1.\n;v2=v2 ";
            "%s",binops[cnt1];
            if(cnt3)
                "v3;\n";
            else
                "2;\n";
            if(cnt2)
                "\"1%s2 is %%d\\n\",v1;\n",binops[cnt1];
            else
                "\"1. %s 2. is %%n\\n\",v2;\n",binops[cnt1];
            
            }
    }
    //Const folding.
    cnt1=sizeof(binops)/sizeof(*binops);   
    while(--cnt1>=0) {
        for(cnt2=0;cnt2!=2;cnt2++) {
            if(cnt2)
                "\"1 %s 2 is %%d\\n\",1%s2;\n",binops[cnt1],binops[cnt1];
            else
                "\"1. %s 2. is %%n\\n\",1. %s 2.;\n",binops[cnt1],binops[cnt1];
        }
    }
    "}\n";
    "Fuzz;";
}
FuzzGen;
    
