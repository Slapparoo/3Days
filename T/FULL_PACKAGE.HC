#define COMPONET_COMPILER 1
#define COMPONET_GRAPHICS 1
#define IMPORT_BUILTINS 1
#define INIT_EVERYTHING 1
#define FPTRS 1
#define USE_DOLDOC 1
#include "HCRT_TOS.HC"
static U0 BootstrapImportSymbol(U8 *name,U8 *ptr,I64 type) {
    CHash *find=HashFind(name,Fs->hash_table,-1);
    if(find&&find->type&HTT_GLBL_VAR) {
        find(CHashGlblVar*)->data_addr=ptr;
    } else if(find&&find->type&HTT_FUN&&!(find(CHashFun*)->flags&1<<Ff_INTERNAL)) {
        find(CHashFun*)->exe_addr=ptr;
        Btr(&find(CHashFun*)->flags,Cf_EXTERN);
    } else if(name[0]=='_'||!StrNCmp(name,"SYS",3)) {
		find=MAlloc(sizeof(CHashExport));
		find->str=StrNew(name);
		find->type=HTT_EXPORT_SYS_SYM;
		find(CHashExport*)->val=ptr;
		HashAdd(find,Fs->hash_table);
    }
}
static U0 __BootstrapForeachSymbol__(U0 *cptr) {
    __BootstrapForeachSymbol(cptr);
}
asm {
//I am embeding these into the binary so end-users don't mess up thier KERNEL*.HH files
  KERNELA_BIN::
  BINFILE "/KERNELA.HH";
  DU8 0;
  KERNELB_BIN::
  BINFILE "/KERNELB.HH";
  DU8 0;

};
U0 LoadImps() {
  ExePrint2("#define FPTRS\n;;");
  ExePutS2(KERNELA_BIN,"/KERNELA.HH");
  __BootstrapForeachSymbol(&BootstrapImportSymbol);
//Load asm externs second
  ExePrint2("#define BOOT_3DAYS\n;;");
  ExePutS2(KERNELB_BIN,"/KERNELB.HH");
}
static U0 StartUpCore1(I64) {
  ACInit("/KERNEL*.HH*");
}
U0 StartUpTasks()
{
  CTask *user1,*user2;
  user1=User;
  user2=User;
  WinToTop(user1);
  WinTileVert;
  try {
    ExeFile("/WallPaper.HC.Z");
    ExePrint("WallPaperInit;;");
  } catch
    PutExcept;
  AutoComplete(ON);
}
U0 Adam(U8 *) {
	I64 idx;
	LoadImps;
	cpu_structs[Gs->num=__CoreNum]=Gs;//It should be 0
	for(idx=Gs->num+1;idx<mp_cnt;idx++) {
		__SpawnCore(idx,&CoreAPSethTask);
                while(!cpu_structs[idx])
                  PAUSE;
	}
#ifndef BC_ENABLE
    if(FileFind("HCRT.DBG.Z"))
		DbgInfoRead("HCRT.DBG.Z");
#else
    if(FileFind("HCRT_BC.DBG.Z"))
		DbgInfoRead("HCRT_BC.DBG.Z");
#endif
	if(!__IsCmdLine) {
		sys_winmgr_task=Spawn(&WinMgrTask);
		//Grab pointer
		ExePrint("#include \"AMOUSE.HC.Z\";;");
	} else {
		DeathWait(User);
		Exit3Days;
	}
        Spawn(&StartUpCore1,NULL,,mp_cnt-1);
	Refresh; //Wait for window manager.
	StartUpTasks;
	//We are "adam" task
	SrvTaskCont;
}
Gs->seth_task=Spawn(&CoreAPSethTask,NULL,"Seth");
//Our entering thread doesnt have a cur_thrd,so spawn one
adam_task=Spawn(&Adam,NULL,"Adam",,);
//Dont Exit
DeathWait(adam_task);