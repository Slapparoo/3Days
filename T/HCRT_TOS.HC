#define FPTRS 1
#include "KERNELA.HH"
#include "KGLBLS.HC"
U8 *FileRead(U8 *fn,I64 *len=NULL,CTask *t=NULL) {
	I64 len2;
    //CHANGE,TempleOS grabs loaded files from RAM,some functions use
    //blkdev.tmp_filename as a "defualt" file,so im going to assume its loaded
    if(fn==blkdev.tmp_filename) {
        return StrNew("");
    }
	U8 *fn2=FileNameAbs(fn),*data=NULL,*exp;
	if(!fn2) {
        PrintErr("File \"%s\" not found!",fn);
        return NULL;
    }
	if(!t) t=Fs;
    if(!FileFind(fn2)) {
		"File %s not found!!!\n",fn2;
		throw('FILE');
	}
	data=__FileRead(fn,&len2,t);
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
I64 FileWrite(U8 *fn,U8 *buf,I64 len) {
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
#ifdef INIT_EVERYTHING
TaskInit(Fs,0);
#endif
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
#include "GR/MakeGr.HC"
#include "MENU.HC"
#include "WIN.HC"
#include "DOLDOC/MakeDoc.HC"

fp_puts2=&KDDocPutS;
U0 PutKey(I64 ch,I64 s)	 {
    DocPutKey(DocPut,ch,s);
}

#include "CTRLSA.HC"
#include "CTRLSBTTN.HC"
#include "CTRLSSLIDER.HC"
#include "WINMGR.HC"
#include "WALLPAPER.HC"
#include "AREGISTRY.HC"
#include "AutoComplete/MakeAC.HC"
#include "God/MakeGod.HC"
#endif
