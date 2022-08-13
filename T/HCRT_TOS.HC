#define FPTRS 1
#include "KERNELA.HH"
Bool dol_doc_inited=FALSE;
Bool IsRaw() {
	return __IsCmdLine;
}
extern U0 InitHeaps();
InitHeaps;
CHeapCtrl ahc;
HeapCtrlInit(&ahc,Fs,0);
SetFs(MAlloc(sizeof(CTask),&ahc));
Fs->data_heap=&ahc;
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
U8 *FileRead(U8 *fn,I64 *len=NULL,CTask *t=NULL) {
  I64 len2;
//CHANGE,TempleOS grabs loaded files from RAM,some functions use
  //blkdev.tmp_filename as a "defualt" file,so im going to assume its loaded
  if(fn==blkdev.tmp_filename) {
    return StrNew("");
  }
  U8 *fn2=FileNameAbs(fn),*data=NULL,*exp;
  if(!__FExists(fn2)) {
    exp=MStrPrint("%s.Z",fn2);
    if(!__FExists(exp)) {
      Free(fn2),Free(exp);
      PrintErr("File \"%s\" not found!",fn);
      throw('FILE');
    }
    Free(fn2);
    fn2=exp;
  }
  if(!t) t=Fs;
  Yield;
  data=__FileRead(fn2,&len2,t);
  if(IsDotZ(fn2)) {
    exp=ExpandBuf(data,t);
    if(len)
      *len=data(CArcCompress*)->expanded_size;
    Free(data);
    data=exp;
  } else {
    if(len)
      *len=len2;
  }
  Free(fn2);
  return data;
}
I64i FileWrite(U8i *fn,U8i *buf,I64i len) {
  I64 len2,r;
  U8 *fn2=FileNameAbs(fn),*data=NULL;
  CArcCompress *cmp;
  if(!fn2) return 0;
  if(IsDotZ(fn2)) {
    if(!buf) buf="";
    cmp=CompressBuf(buf,len,Fs);
    r=__FileWrite(fn2,cmp,cmp->compressed_size);
    Free(cmp);
  } else
    r=__FileWrite(fn2,buf,len);
  Free(fn2);
  return r;
}
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
#include "DSKSTR.HC"
#include "DSKDIRA.HC"   
#include "DSKFIND.HC"
#include "KDBG.HC"
#include "KEXCEPT.HC"
#include "DISPLAY.HC"
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
#include "BlkDev/DskCopy.HC"
#include "BlkDev/DskA.HC"
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

//#include "PSLAMODY/Load.HC"
#include "ARegistry.HC"
#include "AHash.HC"
#include "ADskB.HC"
#include "ADBG.HC"
#include "Sched.HC"
#include "MAlloc2.HC"
#include "Diff.HC"
#include "KeyDev.HC"
#include "FILEMGR.HC";
#include "Training.HC.Z";
#endif
