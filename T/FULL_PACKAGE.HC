#define COMPONET_COMPILER 1
#define COMPONET_GRAPHICS 1
#define IMPORT_BUILTINS 1
#define INIT_EVERYTHING 1
#define FPTRS 1
#define USE_DOLDOC 1
#include "HCRT_TOS.HC"
static U0 BootstrapImportSymbol(U8 *name,U8 *ptr,I64 type) {
    CHash *find=HashFind(name,Fs->hash_table,type);
    if(find&&find->type&HTT_GLBL_VAR) {
        find(CHashGlblVar*)->data_addr=ptr;
    } else if(find&&find->type&HTT_FUN) {
        find(CHashFun*)->exe_addr=ptr;
        Btr(&find(CHashFun*)->flags,Cf_EXTERN);
    } else if(name[0]=='_') {
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
U0 LoadImps() {
    ExePrint2(
        "#define FPTRS\n#include \"KERNELA.HH\";"
    );
    __BootstrapForeachSymbol(&BootstrapImportSymbol);
    //Load asm externs second
    ExePrint2("#define BOOT_3DAYS\n#include \"KERNELB.HH\";\n");
}
U0 REPL() {
    LoadImps;
    __AwaitThread(Spawn(&WinMgrTask)->_3days_spawn_ptr);
}
__AwaitThread(Spawn(&REPL,NULL,"Main",,Fs)->_3days_spawn_ptr);
