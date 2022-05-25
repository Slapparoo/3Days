#include "3d.h"
static  int RootPathLen();
//I dislike this file. Windows uses '\\' as a delimeter ,but TempleOS uses '/'
//So key an eye out for delim/TOS_delim
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#define VFS_T_FILE 1 
#define VFS_T_DIR 2 
#define VFS_T_DRIVE 3 
char* cur_dir;
char cur_drv;
static map_str_t mount_points;
void VFsGlobalInit() {
	map_init(&mount_points);
}
#define TOS_delim '/'
#ifdef TARGET_WIN32
#include <windows.h>
#include <fileapi.h>
#define delim '\\'
#else
#define delim '/'
#include <sys/types.h>
#include <sys/stat.h>
#endif

static char *VFsInplaceConvertDelims(char *p) {
	if(TOS_delim!=delim)
		for(;strchr(p,delim);)
			*strchr(p,delim)=TOS_delim;
	return p;
}
static char *VFsInplaceHostDelims(char *p) {
	if(TOS_delim!=delim)
		for(;strchr(p,TOS_delim);)
			*strchr(p,TOS_delim)=delim;
	return p;
}
char * VFsDirCur() {
    vec_char_t ret;
    vec_init(&ret);
    vec_push(&ret,cur_drv);
    vec_push(&ret,':');
     //+3 Remove ./(Drive char)
    vec_pusharr(&ret,cur_dir+RootPathLen(),strlen(cur_dir+RootPathLen()));
    vec_push(&ret,0);
    return VFsInplaceConvertDelims(ret.data);
}
#ifdef TARGET_WIN32
static int __FExists(char *path) {
	char buf[strlen(path)+1];
	strcpy(buf,path);
	if(buf[strlen(path)-1]==delim)
		buf[strlen(path)-1]=0;
	path=buf;
	if(strchr(buf,'*')||strchr(buf,'?')) {
		//Not in the mood to match wildcards,we do that in HolyC
		return 0;
	}
    WIN32_FIND_DATA data;
    HANDLE h=FindFirstFile(path,&data);
    if(h!=INVALID_HANDLE_VALUE) {
        FindClose(h);
        return 1;
    }
    return 0;
}
static int __FIsDir(char *path) {
	int r=0;
	char buf[strlen(path)+1];
	strcpy(buf,path);
	if(buf[strlen(path)-1]==delim)
		buf[strlen(path)-1]=0;
	path=buf;
    if(strchr(buf,'*')||strchr(buf,'?')) {
		//Not in the mood to match wildcards,we do that in HolyC
		return 0;
	}
	WIN32_FIND_DATA data;
    HANDLE h=FindFirstFile(path,&data);
    if(h!=INVALID_HANDLE_VALUE) {
        r=!!(data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY);
        FindClose(h);
    }
    return r;
}
#else
static int __FExists(char *path) {
    return access(path,F_OK)==0;
}
static int __FIsDir(char *path) {
    struct stat s;
    stat(path,&s);
    return (s.st_mode&S_IFMT)==S_IFDIR;
}
#endif
static  int RootPathLen() {
	char buffer[2]={cur_drv,0},**base;
	assert(base=map_get(&mount_points,buffer));
	int64_t r=strlen(*base);
	if(base[0][r-1]==delim)
		r--;
	return r;
}
int VFsCd(char *to,int flags) {
	int make=flags&VFS_CDF_MAKE;
    int allow_fail=flags&VFS_CDF_FILENAME_ABS;
    //int only_dirs=flags&VFS_ONLY_DIRS;
    int failed=0;
    int folder_depth=0;
    char *d,*top,*top2,*root,drv=cur_drv;
    char drv_buf[2]={cur_drv,0};
    root=*map_get(&mount_points,drv_buf);
    vec_char_t path;
    vec_init(&path);
    vec_pusharr(&path,root,strlen(root)+1);
    if(strrchr(to,':')) {
        switch(toupper(*to)) {
            case 'A'...'Z':
            drv=toupper(*to);
            break;
            case ':':
            //Use dft drive?
            break;
        }
        to=strrchr(to,':')+1;
    }
    if(to[0]==TOS_delim) {
		//Is an absolute path
		
    } else {
        vec_deinit(&path);
        vec_pusharr(&path,cur_dir,strlen(cur_dir)+1);
    }
    VFsInplaceHostDelims(path.data);
    if(!__FExists(path.data)&&!make) {
		if(!allow_fail)
			failed=1;
    } else if(!__FExists(path.data)&&make) {
        #ifndef TARGET_WIN32
        mkdir(path.data,0700);
        #else
        mkdir(path.data);
        #endif
    }
    loop:
    vec_pop(&path);
    d=strchr(to,TOS_delim);
    if(!d)
        d=to+strlen(to);
    vec_pusharr(&path,to,d-to);
    vec_push(&path,0);
    if(!strcmp(strrchr(path.data,delim)+1,".")) {
        vec_pop(&path);
        vec_pop(&path);
        vec_push(&path,0);
        goto next;
    } else if(!strcmp(strrchr(path.data,delim)+1,"..")) {
        //Pop a directory item
        top=strrchr(path.data,delim);
        assert(top);
        if(top-1>=path.data){
            //Swap out the delim byte so it doesnt appear while we look the the second-to-last delim 
            *(top2=top)=0;
            top=strrchr(path.data,delim);
            *top2=delim;
        }
        if(RootPathLen()>=top-path.data) {
            path.length=RootPathLen(); //+1 for delim
            vec_push(&path,0);
            goto next;
        } else if(top) {
            path.length=top-path.data+1;
            vec_push(&path,0);
        }
    } else 
        folder_depth++; 
    vec_pop(&path);
    if(path.data[path.length-1]!=delim)
        vec_push(&path,delim);
    vec_push(&path,0);
    if(!__FExists(path.data)&&!make) {
		if(!allow_fail)
			failed=1;
    } else if(!__FExists(path.data)&&make) {
        #ifndef TARGET_WIN32
        mkdir(path.data,0700);
        #else
        mkdir(path.data);
        #endif
    }
    if(*d) {
        next:
        to=++d;
        goto loop;
    }
    vec_pop(&path);
    if(path.data[path.length-1]!=delim)
        vec_push(&path,delim);
    vec_push(&path,0);
    if(!failed) {
		PoopFree(cur_dir);
		cur_dir=path.data;
	} else
		vec_deinit(&path);
    return !failed;
}
int64_t VFsDel(char *p) {
	p=__VFsFileNameAbs(p);
	if(!__FExists(p))
		return PoopFree(p),0;
	#ifdef TARGET_WIN32
	if(__FIsDir(p))
		RemoveDirectory(p);
	else
		DeleteFileA(p);
	#else
	remove(p);
	#endif
	return PoopFree(p),!__FExists(p);
}
//Returns Host OS location of file
char *__VFsFileNameAbs(char *name) {
    int failed=0;
    char *file,*old_dir=VFsDirCur();
    if(strrchr(name,':')) {
        switch(toupper(*name)) {
            case 'A'...'Z':
            cur_drv=toupper(*name);
            break;
            case ':':
            //Use dft drive?
            break;
        }
        name=strrchr(name,':')+1;
    }
    vec_char_t path,head;
    vec_init(&path);
    vec_init(&head);
    if(name[0]!=TOS_delim) {
        vec_pusharr(&path,old_dir,strlen(old_dir));
        vec_push(&path,TOS_delim);
        vec_pusharr(&path,name,strlen(name));
        vec_push(&path,0);
    } else {
        vec_pusharr(&path,name,strlen(name)+1);
    }
    VFsInplaceHostDelims(path.data);
    vec_pusharr(&head,path.data,path.length);
    VFsInplaceConvertDelims(head.data);
    file=strrchr(head.data,TOS_delim)+1;
    file[-1]=0;
    failed=!VFsCd(head.data,VFS_CDF_FILENAME_ABS);
    vec_deinit(&path);
    vec_init(&path);
    vec_pusharr(&path,cur_dir,strlen(cur_dir));
    vec_pusharr(&path,file,strlen(file));
    vec_push(&path,0);
    VFsCd(old_dir,0);
    end:
    vec_deinit(&head);
    if(!failed)
        return path.data;
    vec_deinit(&path);
    VFsInplaceHostDelims(path.data);
    return NULL;
}
char *VFsFileNameAbs(char *name) {
    char *tmp=__VFsFileNameAbs(name),*r;
    if(!tmp) return NULL;
    char buf[strlen(tmp)+1+3];
    int64_t offset=3;
    if(tmp) {
        if(tmp[0]==TOS_delim)
            offset=2;
        strcpy(buf+offset,tmp+RootPathLen());
        buf[0]=cur_drv;
        buf[1]=':';
        if(offset==3)
            buf[2]=TOS_delim;
        return strdup(VFsInplaceConvertDelims(buf));
    }
    return NULL;
}
char VFsChDrv(char to) {
    char old=cur_drv;
    switch(toupper(cur_drv)) {
        case 'A'...'Z':
        cur_drv=toupper(cur_drv);
        break;
    }
    return old;
}
int64_t VFsFileWrite(char *name,char *data,int64_t len) {
    FILE *f;
    name=__VFsFileNameAbs(name);
    if(name) {
        f=fopen(name,"wb");
        fwrite(data,1,len,f);
        fclose(f);
    }
    PoopFree(name);
    return !!name;
}
int64_t VFsFileRead(char *name,int64_t *len) {
    if(len) *len=0;
    FILE *f;
    int64_t s,e;
    void *data=NULL;
    name=__VFsFileNameAbs(name);
    if(!name) return NULL;
    if(!__FIsDir(name)) {
        f=fopen(name,"rb");
        if(!f) goto end;
        s=ftell(f);
        fseek(f,0,SEEK_END);
        e=ftell(f);
        fseek(f,0,SEEK_SET);
        fread(data=PoopMAlloc(e-s+1),1,e-s,f);
        fclose(f);
        if(len) *len=e-s;
    }
    end:
    PoopFree(name);
    return data;
}
void VFsThrdInit() {
    cur_drv='T';
    char buf[]={cur_drv,0};
    cur_dir=strdup(map_get(&mount_points,buf)[0]);
}
#ifdef TARGET_WIN32
#include <shlobj.h> 
char *HostHomeDir() {
	char home[MAX_PATH];
	if(S_OK==SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, home))
		return strdup(home);
	return NULL;
}
#else
#include <pwd.h>
//https://stackoverflow.com/questions/2910377/get-home-directory-in-linux
char *HostHomeDir() {
	char buf[0x2000],*ret;
	struct passwd pwd;
	struct passwd *result;
	getpwuid_r(getuid(),&pwd,buf,0x2000,&result);
	if(result) {
			return strdup(result->pw_dir);
	}
	return NULL;
}
#endif
//Creates a virtual drive by a template
static void CopyDir(char *dst,char *src) {
	if(!__FExists(dst)) {
		#ifdef TARGET_WIN32
		mkdir(dst);
		#else
		mkdir(dst,0700);
		#endif
	}
	char buf[1024],sbuf[1024],*s;
	int64_t root,sz,sroot;
	strcpy(buf,dst);
	buf[root=strlen(buf)]=delim;
	buf[++root]=0;
	
	strcpy(sbuf,src);
	sbuf[sroot=strlen(sbuf)]=delim;
	sbuf[++sroot]=0;
	
	DIR *d=opendir(src);
	struct dirent *ent;
	while(ent=readdir(d)) {
		if(!strcmp(ent->d_name,".")||!strcmp(ent->d_name,".."))
			continue;
		buf[root]=0;
		sbuf[sroot]=0;
		strcat(buf,ent->d_name);
		strcat(sbuf,ent->d_name);
		if(__FIsDir(sbuf)) {
			CopyDir(buf,sbuf);
		} else {
			s=FileRead(sbuf,&sz);
			FileWrite(buf,s,sz);
			PoopFree(s);
		}
	}
}
void MountDrive(char d,char *path) {
	int64_t len=strlen(path);
	char buf[2]={toupper(d),0},buf2[1024];
	strcpy(buf2,path);
	if(buf2[len-1]!=delim)
		buf2[len]=delim,buf2[len+1]=0;
	map_set(&mount_points,buf,strdup(buf2));
}
void CreateTemplateBootDrv(char *to,char *template,int overwrite) {
	if(!overwrite)
		if(__FExists(to)) {
			MountDrive('T',to);
			return;
		}
	char buffer[1024],drvl[16],buffer2[1024];
	#ifdef TARGET_WIN32
    GetModuleFileNameA(NULL,buffer,sizeof(buffer));
    _splitpath(buffer,drvl,buffer2,NULL,NULL);
    strcat(buffer2,template);
    strcpy(buffer,drvl);
    strcat(buffer,buffer2);
    if(buffer2[strlen(buffer2)-1]==delim)
		buffer2[strlen(buffer2)-1]=0;
    assert(__FExists(buffer2));
    strcat(buffer,"\\");
    #else
    strcpy(buffer2,template);
    assert(__FExists(buffer2));
    #endif
    CopyDir(to,buffer2);
    MountDrive('T',to);
}
