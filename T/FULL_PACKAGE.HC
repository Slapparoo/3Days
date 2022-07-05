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
static U0 __BootstrapForeachSymbol__(U0 *cptr) {
    __BootstrapForeachSymbol(cptr);
}
U0 LoadImps() {
    ExePrint2(
        "#define FPTRS\n#include \"KERNELA.HH\";"
    );
    __BootstrapForeachSymbol(&BootstrapImportSymbol);
    //Load asm externs second
    ExePrint2("#define BOOT_3DAYS\n#include \"KERNELB.HH\";\n");
}
static U0 SetGsNum(I64) {
	Gs->num=__CoreNum;
}
U0 StartUpTasks()
{
  CTask *user1,*user2;
  user1=User;
  user2=User;
  WinToTop(user1);
  WinTileVert;
  WallPaperInit;
  DeathWait(Spawn(&ACInit,"/KERNEL*.HH",,mp_cnt-1));
  AutoComplete(ON);
}
U0 SethTask(I64) {
	QueInit(&(Gs->next_dying));
	Gs->num=__CoreNum;
	Gs->seth_task=Fs;
	TOSPrint("INITED:%d\n",Gs->num);
	mp_cores[Gs->num]=Gs;
	while(TRUE) {
		TaskKillDying;
		Yield;
	}
}
U0 Adam(U8 *) {
	I64 idx;
	LoadImps;
	Gs->num=__CoreNum;//It should be 0
	for(idx=Gs->num+1;idx<mp_cnt;idx++) {
		__SpawnCore(idx,&SethTask);
	}
    if(FileFind("HCRT.DBG.Z"))
		DbgInfoRead("HCRT.DBG.Z");
	boundscheck_enable=1;
	if(!__IsCmdLine) {
		sys_winmgr_task=Spawn(&WinMgrTask);
	} else {
		DeathWait(User);
		Exit3Days;
	}
	Refresh; //Wait for window manager.
	StartUpTasks;
	//We are "adam" task
	SrvTaskCont;
}
Gs->seth_task=Spawn(&SethTask,NULL,"Seth");
//Our entering thread doesnt have a cur_thrd,so spawn one
adam_task=Spawn(&Adam,NULL,"Adam",,);
//Dont Exit
DeathWait(adam_task);
