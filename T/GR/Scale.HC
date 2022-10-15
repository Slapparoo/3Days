U32 pal[16]={
(0x000000),(0x0000AA),(0x000AA00),(0x00AAAA),
(0xAA0000),(0xAA00AA),(0xAA5500),(0xAAAAAA),
(0x555555),(0x5555FF),(0x55FF55),(0x55ffff),
(0xFF5555),(0xFF55FF),(0xFFFF55),(0xFFFFFF)
};
//We store the lower 4 bits in byte 1,upper 4 bits at byte 2 
U32 *pal_mixed;
U0 AvTabGen() {
  if(!pal_mixed) pal_mixed=CAlloc(0x1000*4,adam_task);
  I64 a,b;
  U32 c,c2;
  for(a=0;a!=0x100;a++) {
     b=(a&0xf0)>>4;
     c=pal[a&0xf],c2=pal[a>>4];
     //https://www.compuphase.com/graphic/scale3.htm
     pal_mixed[(a&0xf)|b<<8]=((c^c2)&0xfffefefe)>>1+(c&c2);
  }
}
AvTabGen;
U0 GrPaletteColorSet(I64 i,CBGR48 v){
  U32 c;
  c.u8[0]=v.b/ToF64(0xffff)*0xff;
  c.u8[1]=v.g/ToF64(0xffff)*0xff;
  c.u8[2]=v.r/ToF64(0xffff)*0xff;
  pal[i]=c;
  AvTabGen;
}
U0 GrPaletteColorGet(I64 i) {
  CBGR48 r;
  U32 c=pal[i];
  r.b=c.u8[0]*(ToF64(0xffff)/0xff);
  r.g=c.u8[1]*(ToF64(0xffff)/0xff);
  r.r=c.u8[2]*(ToF64(0xffff)/0xff);
  return r;
}
U32 *grx_offs=CAlloc(641*4,adam_task);
U32 *gry_offs=CAlloc(481*4,adam_task);
U0 UpdateGROffs(I64 x,I64 y) {
   static I64 x2,y2;
   F64 idx;
   I64 cnt;
   F64 yspacing=ToF64(y)/480;
   F64 xspacing=ToF64(x)/640;
   if(x!=x2||y2!=y) {
      x2=x,y2=y;
      for(cnt=0;cnt<641;cnt++)
         grx_offs[cnt]=Round(xspacing*cnt);
      for(cnt=0;cnt<481;cnt++)
         gry_offs[cnt]=Round(yspacing*cnt);
   }
}
U0 Scale(CDC *data,I64 sx,I64 sy,CDC *result,I64 xoff,I64 yoff) {
  I64 y_next_px;
asm {
  IMPORT grx_offs;
  IMPORT gry_offs;
  IMPORT pal_mixed;
  IMPORTpal;
  PUSH_REGS
  PUSH RAX
  FSTCW U16[RSP]
  MOV RAX,U64 [RSP]
  //Set round-to-nearest mode
  OR RAX,0xc00
  PUSH RAX
  FLDCW U16[RSP]
  POP RAX
  /*
   R8 is av_Tab_gen
   R9 is result
   R10 is data
   R11 is x
   R12 is y
   R13 is data.body+off
   R15 is result body pointer(translated to [xoff,yoff])
   RSI is data pointer
   RDI is result pointer
  */

  PUSH SF_ARG3[RBP]
  PUSH SF_ARG2[RBP]
  CALL &UpdateGROffs
  
  MOV R13,SF_ARG2[RBP]
  MOV R14,SF_ARG3[RBP]
  MOV R9,SF_ARG4[RBP]
  MOV R10,SF_ARG1[RBP]
  
  MOV R15,U64 SF_ARG6[RBP]
  MOV ECX,U32 CDC.width_internal[R9]
  IMUL2 R15,RCX
  ADD R15,U64 SF_ARG5[RBP]
  LEA R15,U64 [R15*4]
  ADD R15,U64 CDC.body[R9]
  MOV RDI,R15
//y=0
  MOV R12,0
  yloop:
//data_ptr=data->body+data->width_internal*(y/yper_px1)
  MOV ECX,U32 CDC.width_internal[R10]
  IMUL2 RCX,R12
  MOV RSI,U64 CDC.body[R10]
  ADD RSI,RCX
  MOV R13,RSI
  
//dst_ptr=result->data+result->width_internal*y
  MOV RDI,U64 [gry_offs]
  MOV EBX,U32 4[RDI+R12*4]
  MOV I64 &y_next_px[RBP],RBX
  MOV EDI,U32 [RDI+R12*4]
  
  MOV ECX,U32 CDC.width_internal[R9]
  IMUL2 RDI,RCX
  LEA RDI,U64 [RDI*4]
  ADD RDI,R15
  
  //x=0
  MOV R11,0
xloop:
  MOVZX EAX,U8[RSI+R11]
  AND AL,0xf;
  MOV EAX,U32 pal[RAX*4]
  MOV R14,U64 [grx_offs]
  MOV EBX,U32 4[R14+R11*4] //next pixel
  MOV R14u32,U32 [R14+R11*4]
  MOV U32 [RDI+R14*4],EAX
  CMP RBX,R14
  JE next
  //Dont go past next pixel
  CMP R11,639
  JE next
  MOV RCX,RBX
  SUB RCX,R14
  //Avg between 2 color bytes
  MOVZX EAX,U16 [RSI+R11]
  //Only want first 4 bits of each byte
  AND EAX,0xf0f 
  MOV RBX,U64 [pal_mixed]
  MOV EAX,U32 [RBX+RAX*4]
  PUSH RDI
  LEA RDI,U64 4[RDI+R14*4]
  REP_STOSD
  POP RDI
next:
  INC R11
  CMP R11,640
  JNE U32 xloop
  
  MOV RDI,U64 [gry_offs]
  MOV EDI,U32 [RDI+R12*4]
  MOV RCX,I64 &y_next_px[RBP] 
next2:
  INC R12
  CMP R12,480
  JNE U32 yloop
//
  MOV R14,U64 CDC.body[R10]
  MOV R12,0
yloop2:
  MOV RDI,U64 [gry_offs]
  MOV EBX,U32 4[RDI+R12*4]
  MOV EDI,U32 [RDI+R12*4]
  DEC RBX
  CMP RBX,RDI
  JNG next3
//we will repeat until the next line
  MOV RCX,RBX
  SUB RCX,RDI
//get scanliune start
  MOV R10u32,U32 CDC.width_internal[R9]
  IMUL2 RDI,R10
  LEA RDI,U64 [RDI*4]
  ADD RDI,R15
  LEA RSI,U64 [RDI+R10*4] 
row_loop: 
  //push repeat_cnt
  PUSH RCX
  PUSH U64 SF_ARG2[RBP] //sx
  //Point to next line
  PUSH RSI
  PUSH R14
  ADD U64[RSP],640
  PUSH R14
  PUSH R8
  CALL AVG_2_LINES
//Pop the repeat count
  POP RCX
  LOOP row_loop
next3:
  ADD R14,640
  INC R12
  CMP R12,480
  JNE U32 yloop2
  FLDCW U16[RSP]
  POP RAX
  POP_REGS
  LEAVE
  RET1 6*8
//Avg2Lines(U8 *avg_tab,u8 *line1,U8 *line2,u32 *to,U64 sx)
AVG_2_LINES:
    PUSH RBP
    MOV RBP,RSP
    PUSH R8
    PUSH R9
    PUSH R10
    PUSH RDI
    PUSH R12
    PUSH R13
    PUSH R14
    MOV R9,U64 SF_ARG2[RBP]
    MOV R10,U64 SF_ARG3[RBP]
    MOV RDI,U64 SF_ARG4[RBP]
    MOV R8,0
    MOV R14,U64 [grx_offs]
    MOV RBX,U64 [pal_mixed]
@@loop:
    MOV R12u32,U32 4[R14+R8*4] //NExt scaled x pixel
    MOV R13u32,U32 [R14+R8*4]
    //Form 16bit color color code to avergage
    MOV CL,U8[R9+R8]
    MOV AL,U8[R10+R8] //REX promotes AH to SPL
    MOV AH,CL //See above note
    MOVZX EAX,AX
    //Only want first 4 bits of each byte
    AND EAX,0xf0f 
    MOV EAX,U32 [RBX+EAX*4]
    //
    MOV RCX,R12
    SUB RCX,R13
    REP_STOSD
    //
    INC R8
    CMP R8,640
    JNE @@loop
    POP R14
    POP R13
    POP R12
    POP RDI
    POP R10
    POP R9
    POP R8
    LEAVE 
    RET1 5*8
  };
}

