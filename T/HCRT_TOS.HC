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
#include "MAlloc2.HC"
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
#include "ARegistry.HC"
#include "AHash.HC"
#include "ADskB.HC"
#include "ADBG.HC"
#include "Sched.HC"
#include "Diff.HC"
#include "KeyDev.HC"
#include "FILEMGR.HC"
#include "Training.HC.Z"
#include "InFile.HC"
#include"Community-Heiresis/HolyCipher/CIPHER.HC";
//
// $BK,1$Enc files will contain a extra 8 bytes that contain a checksum$BK,0$
//
U8 *FileRead(U8 *fn,I64 *len=NULL,CTask *t=NULL) {
  I64 len2;
  U8 *tmp,*buf2;
//CHANGE,TempleOS grabs loaded files from RAM,some functions use
  //blkdev.tmp_filename as a "defualt" file,so im going to assume its loaded
  if(fn==blkdev.tmp_filename) {
    return StrNew("");
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
  data=__FileRead(fn2,&len2,t);
  if(IsDotZ(fn2)) {
    exp=ExpandBuf(data,t);
    len2=data(CArcCompress*)->expanded_size;
    if(len)
      *len=data(CArcCompress*)->expanded_size;
    Free(data);
    data=exp;
  } else {
    if(len)
      *len=len2;
  }
  tmp=FileExtDot(fn2);
  if(!StrNCmp(tmp,".ENC",4)) {
    tmp=GetCipherPasswd;
    if(!tmp) throw('NoPasswd');
    buf2=MAlloc(len2-8+1);//+1 for NULL byte
    buf2[len2-8]=0;
    Decrypt(data,buf2,len2-8,tmp,StrLen(tmp)); //See above blinking note
    if(data[len2-8](U64)!=HashStr(buf2))
      throw('BadEnc');
    len2-=8;
    if(len) *len=len2;
    Free(data);
    data=buf2;
    Free(tmp);
  }
  Free(fn2);
  return data;
}
I64i FileWrite(U8i *fn,U8i *buf,I64i len) {
  I64 len2,r;
  Bool fr=FALSE;;
  U8 *fn2=FileNameAbs(fn),*data=NULL,*tmp;
  CArcCompress *cmp;
  if(!fn2) return 0;
  tmp=FileExtDot(fn2);
  if(!StrNCmp(tmp,".ENC",4)) {
    data=CAlloc(len+8);
    MemCpy(data,buf,len);
    tmp=GetCipherPasswd;
    if(!tmp) throw('NoPasswd');
    Encrypt(data,buf,len,tmp,StrLen(tmp));
    data[len](U64)=HashStr(buf);
    len+=8;
    Free(tmp);
    fr=TRUE;
    buf=data;
  }
  if(IsDotZ(fn2)) {
    if(!buf) buf="";
    cmp=CompressBuf(buf,len,Fs);
    r=__FileWrite(fn2,cmp,cmp->compressed_size);
    Free(cmp);
  } else
    r=__FileWrite(fn2,buf,len);
  Free(fn2);
  if(fr) Free(buf);
  return r;
}

#endif
