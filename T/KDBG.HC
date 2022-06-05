U8 *Caller(I64 num=1)
{//Returns the addr of the fun which called this one,
//or the caller of the caller, etc.
  U8 **rbp=GetRBP,**ptr;
  while (num--) {
    if (rbp>=*rbp)
      return NULL;
    rbp=*rbp;
    //TODO RESTORE
    //if (!ChkOnStk(rbp,Fs))
    //  return NULL;
  }
  ptr=rbp+1;
  return *ptr;
}
U8 *__CallerBP(U8 **rbp,I64 num=1)
{//Returns the addr of the fun which called this one,
//or the caller of the caller, etc.
  U8 **ptr;
  while (num--) {
    if (rbp>=*rbp)
      return NULL;
    rbp=*rbp;
    //TODO RESTORE
    //if (!ChkOnStk(rbp,Fs))
    //  return NULL;
  }
  ptr=rbp+1;
  return *ptr;
}
U0 FaultCB(U8 **rbp,U8 *fptr) {
    I64 bt=1;
    U8 *ln,*fn;
    CHashFun *f=FunSegFind(fptr,NULL);
    TOSPrint("ENTERED FAULT\n");
    
    ln=SrcEdLink(fptr);
    if(!ln) ln=StrNew("???");
    if(f)
        TOSPrint("[%d] %s,%s\n",0,f->str,ln);
    else
        TOSPrint("[%d] %s\n",0,ln);
    Free(ln);
    
    for(bt=0;bt<=5;bt++) {
        ln=SrcEdLink(__CallerBP(rbp,bt));
        if(!ln) ln=StrNew("???");
        f=FunSegFind(__CallerBP(rbp,bt),NULL);
        if(f)
            TOSPrint("[%d] %s,%s\n",bt,f->str,ln);
        else
            TOSPrint("[%d] %s\n",bt,ln);
        Free(ln);
    }
    Exit;
}
