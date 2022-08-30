public Bool ChkOnStk(U8 *ptr,CTask *task=NULL) {
  if(!task) task=Fs;
  return task->stk<=ptr<task->stk+MSize(task->stk);
}
U8 *Caller(I64 num=1)
{//Returns the addr of the fun which called this one,
//or the caller of the caller, etc.
  U8 **rbp=GetRBP,**ptr;
  while (num--) {
    if (rbp>=*rbp)
      return NULL;
    rbp=*rbp;
    if (!ChkOnStk(rbp,Fs))
      return NULL;
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
    if (!ChkOnStk(rbp,Fs))
      return NULL;
  }
  ptr=rbp+1;
  return *ptr;
}
U8 *TaskCaller(CTask *task=NULL,I64 num=0,Bool saved_context=FALSE)
{//Fetches addr of Nth caller on task's stk.
  U8 **ptr,**rbp,**rsp;
  if (!task) task=Fs;
  if (!saved_context && task==Fs)
    return Caller(num+1);
  if (!TaskValidate(task))
    return NULL;
  rbp=task->rbp;
  rsp=task->rsp;
  if (num) {
    while (rbp) {
      if(!ChkOnStk(rbp,task))
        break;
      ptr=rbp+1;
      //Here's the deal,I assume wine muddy's the caller stack to make it more epic,in FFI_CALL_TOS_0_BP(FFI_WIN64.yasm)
      //I zero the caller ptr
      if(!ptr) break;
      if (! --num)
	return *ptr;
      if (rbp>=*rbp)
	break;
      rbp=*rbp;
    }
    return NULL;
  } else {
    if (task->rip==_RET)
      return *rsp;
    else
      return task->rip;
  }
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
