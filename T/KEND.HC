U0 SysGlblsInit() {
	I64 i;
	pow10_I64=CAlloc(sizeof(F64)*(308+308+1));
	for (i=-308;i<309;i++)
		pow10_I64[i+309]=Pow(10.,i);
    text.font=sys_font_std;
    text.cols=GR_WIDTH/FONT_WIDTH;
    text.rows=GR_HEIGHT/FONT_HEIGHT;
    text.raw_scrn_image=CAlloc(GR_WIDTH*GR_HEIGHT/8);
    text.border_chars[2] (I64)=0x0908070605040302;
    text.border_chars[10](U32)=0x0D0C0B0A;
    dbg.fun_seg_cache=CAlloc(FUN_SEG_CACHE_SIZE*sizeof(CFunSegCache));
    blkdev.tmp_filename=StrNew("/Tmp.DD.Z",adam_task);
}
SysGlblsInit;
SysDefinesLoad;
class CTimerPair {
    U0 (*fptr)(U8 *);
    I64 ms;
    U8 *data;
};
static U0 TimerSpawn(CTimerPair *p) {
    while(TRUE) {
        (*p->fptr)(p->data);
        Sleep(p->ms);
    }
}
U0 __AddTimer(I64 ms,U0 (*fptr)(),U8 *data) {
    CTimerPair *p=MAlloc(sizeof(CTimerPair));
    p->ms=ms,p->fptr=fptr,p->data=data;
    Spawn(&TimerSpawn,p);
}
__AddTimer(2,&JiffieInc,2);
