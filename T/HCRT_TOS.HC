#define FPTRS 1
#include "KERNELA.HH"
Bool dol_doc_inited=FALSE;
Bool IsRaw() {
	return __IsCmdLine;
}
CSysFixedArea sys_fixed_area,*SYS_FIXED_AREA;
SYS_FIXED_AREA=&sys_fixed_area;
U0 InitFX() {
  I64 noreg ptr;
  ptr=SYS_FIXED_AREA;
  MOV RAX,U64 &ptr[RBP];
  FXSAVE U64[RAX]
}
InitFX;
extern U0 InitHeaps();
InitHeaps;
CHeapCtrl ahc,ahd;
HeapCtrlInit(&ahc,Fs,0);
HeapCtrlInit(&ahd,Fs,1);
SetFs(MAlloc(sizeof(CTask),&ahc));
Fs->data_heap=&ahd;
Fs->code_heap=&ahc;
Fs->cur_dir=StrNew("T:/",&ahc);
U32 SYS_CTRL_ALT_FLAGS=0;
CCPU *mp_cores[64];
CCPU *Gs() {
	return GetGs;
}
mp_cores[0]=Gs;
QueInit(&(Gs->next_dying));
CTask *Fs() {
	return GetFs;
}
TaskInit(Fs,0);
#include "KGLBLS.HC"
#include "DOLDOC/DocExt.HC"
#include "GR/Gr.HH"
public extern I64 ExeCmdLine(CCmpCtrl *cc);
CTask *adam_task=Fs;
#include "KMATH.HC"
#include "STR.HC"
#include "HASH.HC"
#include "CHASH.HC"
#include "KUTILS.HC"
#include "KERNELB.HH"
#include "KDate.HC"
#include "STRB.HC"
#include "FUNSEG.HC"
#include "STRPRINT.HC"
#include "KDATATYPES.HC"
#include "COMPRESS.HC"
#include "QSORT.HC"
#include "KDBG.HC"
#include "KEXCEPT.HC"
#include "DISPLAY.HC"
#include "MAlloc2.HC"
#include "BlkDev2/MakeBlkDev.HC"
#ifdef COMPONET_COMPILER
#include "CMP_PRJ.HC"
CInit;
#endif
#include "AMATH.HC"
#include "FONT.HC"
#include "FONT2.HC"
#include "KMISC.HC"
#include "KBMESSAGE.HC"
#include "MOUSE.HC"
#include "JOB.HC"
#include "KTASK.HC"
#include "KDefine.HC"
#include "KLOAD.HC"
#include "KEND.HC"
#include "EDLITE.HC"
#ifdef COMPONET_GRAPHICS
#include "TASK_SETTINGS.HC"
#include "ASND.HC"
#include "AMathODE.HC.Z";
#include "GR/MakeGr.HC"
#include "MENU.HC"
#include "WIN.HC"
#include "DOLDOC/MakeDoc.HC"
//See KeyDev.HC in TempleOS
Bool PutS(U8 *st) {
	if(!__IsCmdLine) {
		I64 ch;
		Bool cont;
		U8 *ptr;
		CKeyDevEntry *tmpk=keydev.put_key_head.next;
		if(!st) return;
		KDDocPutS(st);
		while (cont && tmpk!=&keydev.put_key_head) {
		  if (tmpk->put_s) {
		if ((*tmpk->put_s)(st))
		  break;
		  } else {
		ptr=st;
		while (ch=*ptr++)
		  if ((*tmpk->put_key)(ch,0))
			cont=FALSE;
		  }
		  tmpk=tmpk->next;
		}
	} else {
		TOSPrint("%s",st);
	}
}
fp_puts2=&PutS;
U0 PutKey(I64 ch,I64 s)	 {
    DocPutKey(DocPut,ch,s);
}

#include "CTRLSA.HC"
#include "CTRLSBTTN.HC"
#include "CTRLSSLIDER.HC"
#include "WINMGR.HC"
#include "AutoComplete/MakeAC.HC"
#include "God/MakeGod.HC"
#include "Find.HC"
U8 *StrNew(U8 *s,CTask *t=0) {
	U8 *r;
	StrCpy(r=MAlloc(StrLen(s)+2,t),s);
	return r;
} 
U8 *MAllocIdent(U8 *s,CTask *t=0) {
	U8 *r;
	MemCpy(r=MAlloc(MSize(s),t),s,MSize(s));
	return r;
} 
#include "ARegistry.HC"
#include "AHash.HC"
#include "ADskA.HC"
#include "ADskB.HC"
#include "ADBG.HC"
#include "Sched.HC"
#include "Diff.HC"
#include "KeyDev.HC"
#include "FILEMGR.HC"
#include "Training.HC.Z"
#include "InFile.HC"
#include"Community-Heiresis/HolyCipher/CIPHER.HC";
#include "Mount.HC"
/*I64 CheckSum(U8 *data,I64 len) {
  I64 sum=0;
  while(--len>=0) sum+=data[len];
  return sum;
}
Bool IsEncryptedRight(U8 *file) {
  I64 len,ret;
  U8 *data=__FileRead(file,&len),*tmp,*buf2=CAlloc(len);
  tmp=GetCipherPasswd;
  Decrypt(data,buf2,len-8,tmp,StrLen(tmp)); //See above blinking note
  ret=data[len-8](U64)==CheckSum(buf2,len-8);
  Free(data),Free(tmp);
  return ret;
}
*/
#endif
