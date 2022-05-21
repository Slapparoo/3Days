#include "TEMPLATES.HC"
U8 sys_var_init_flag=1;
U8 sys_var_init_val=0;
CHashDefineStr *DefineLstLoad(U8 *dname,U8 *lst)
{   //Create DEFINE list. Not efficient, but handy.
    I64 cnt=0;
    U8 *ptr,**idx;
    CHashDefineStr *tmph=MAlloc(sizeof(CHashDefineStr));
    tmph->type=HTT_DEFINE_STR;
    tmph->str=StrNew(dname);
    ptr=lst;
    while (*ptr) {
        if (*ptr!='@')
            cnt++;
        while (*ptr++);
    }
    tmph->data=MAlloc(ptr+1-lst);
    MemCpy(tmph->data,lst,ptr+1-lst);
    tmph->cnt=cnt;

    idx=tmph->sub_idx=MAlloc(cnt*sizeof(U8 *));
    ptr=lst;
    while (*ptr) {
        if (*ptr!='@')
            *idx++=ptr;
        while (*ptr++);
    }

    HashAdd(tmph,Fs->hash_table);
    return tmph;
}
U8 *DefineSub(I64 sub,U8 *dname)
{   //Return DEFINE list entry indexed by number.
    CHashDefineStr *tmph;
    if (tmph=HashFind(dname,Fs->hash_table,-1)) {
        if(tmph->type==HTT_DEFINE_STR) {
            if (0<=sub<tmph->cnt)
                return tmph->sub_idx[sub];
            else
                return NULL;
        }
    } else if (dname)
        return "UNDEF";
    else
        return NULL;
}
//See HCRT/LEXER.HC
I64 LstMatch(U8 *needle, U8 *haystack_lst,I64 flags=0)
{   //-2 if Ambiguous
// -1 if not found
    // Not efficient. Use hash tables for efficiency.
    I64 n,sub=0,res=-1;
    U8 *ptr;
    Bool exact_match=FALSE;
    if (!haystack_lst) return -1;
    n=StrLen(needle);
    while (*haystack_lst) {
        if (*haystack_lst=='@') {	//Check for '@' alias haystack_lst entry
            sub--;
            haystack_lst++;
        }
        ptr=needle;
        if (flags & LMF_IGNORE_CASE)
            while (*ptr && ToUpper(*ptr)==ToUpper(*haystack_lst)) {
                ptr++;
                haystack_lst++;
            }
        else
            while (*ptr && *ptr==*haystack_lst) {
                ptr++;
                haystack_lst++;
            }
        if (!*ptr) {		//Did we reach end of needle?
            if (!*haystack_lst)	//Did we reach end of haystack_lst?
                return sub;		//Found Exact match
            else {
                if (res!=-1) {
                    if (!exact_match)
                        res=-2;		//Ambiguous unless later exact match.
                } else {
                    if (!(flags & LMF_EXACT))
                        res=sub;
                }
            }
        }
        while (*haystack_lst)	//Advance to end of cur entry.
            haystack_lst++;
        haystack_lst++;		//Skip trailing zero
        sub++;
    }
    return res;
}
U8 *DefineMatch(U8 *f,U8 *dname,I64 flags=0)
{   //Return DEFINE list entry indexed by number.
    CHashDefineStr *tmph;
    if (tmph=HashFind(dname,Fs->hash_table,-1)) {
        if(tmph->type==HTT_DEFINE_STR) {
            return LstMatch(f,tmph->data,flags);
        }
    }
    return NULL;
}
U8 *ExtDft(U8 *filename,U8 *extension)
{   //Give extension if has none.
    U8 *res=MAlloc(StrLen(filename)+1+StrLen(extension)+1);
    StrCpy(res,filename);
    if (!FileExtDot(filename))
        CatPrint(res,".%s",extension);
    return res;
}
#include "KLOAD.HC"
#include "CEXT.HH"
#include "CINIT.HC"
#include "CEXCEPT.HC"
#include "LEXLIB.HC"
#include "LEX.HC"
#include "TOS_ASM.HC"
#include "PRSLIB.HC"
#include "ASMLIB.HC"
#include "ASMRESOLVE.HC"
#include "ASM.HC"
#include "OPTLIB.HC"
#include "PRSEXP.HC"
#include "PRSVAR.HC"
#include "CMISC.HC"
#include "CMAIN.HC"
#include "PRSSTMT.HC"
#include "OPTPASS012.HC"
#include "OPTPASS3.HC"
#include "OPTPASS4.HC"
#include "OPTPASS5.HC"
#include "OPTPASS6.HC"
#include "BACKLIB.HC"
#include "BACKFA.HC"
#include "BACKFB.HC"
#include "BACKA.HC"
#include "BACKB.HC"
#include "BACKC.HC"
#include "OPTPASS789A.HC"
