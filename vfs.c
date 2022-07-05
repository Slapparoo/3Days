#include "3d.h"
static  int RootPathLen();
//I dislike this file. Windows uses '\\' as a delimeter ,but TempleOS uses '/'
//So key an eye out for delim/TOS_delim
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#ifndef TARGET_WIN32
#include <pthread.h>
#else
#include <windows.h>
#include <synchapi.h>
#include <processthreadsapi.h>
#endif
#define VFS_T_FILE 1 
#define VFS_T_DIR 2 
#define VFS_T_DRIVE 3 
static __thread map_str_t drive_dirs;
#ifndef TARGET_WIN32
	pthread_mutex_t mp_mtx=PTHREAD_MUTEX_INITIALIZER;
#else
	HANDLE mp_mtx=NULL;
#endif
static map_str_t mount_points;
#define cur_dir (*(char**)((char*)GetFs()+112))
void VFsGlobalInit() {
	#ifdef TARGET_WIN32
	mp_mtx=CreateMutex(NULL,0,NULL);
	#endif
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
	return strdup(cur_dir);
}
#ifdef TARGET_WIN32
static int __FExists(char *path) {
	char buf[strlen(path)+128];
	strcpy(buf,path);
	if(buf[strlen(path)-1]==delim)
		buf[strlen(path)-1]=0;
	path=buf;
	if(strchr(buf,'*')||strchr(buf,'?')) {
		//Not in the mood to match wildcards,we do that in HolyC
		return 0;
	}
    return PathFileExistsA(buf);
}
static int __FIsDir(char *path) {
	int r=0;
	char buf[strlen(path)+128];
	strcpy(buf,path);
	if(buf[strlen(path)-1]==delim)
		buf[strlen(path)-1]=0;
	path=buf;
    if(strchr(buf,'*')||strchr(buf,'?')) {
		//Not in the mood to match wildcards,we do that in HolyC
		return 0;
	}
	return PathIsDirectoryA(buf);
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
	char buffer[2]={*cur_dir,0},**base;
	#ifndef TARGET_WIN32
	pthread_mutex_lock(&mp_mtx);
	#else
	WaitForSingleObject(mp_mtx,INFINITE);
	#endif
	assert(base=map_get(&mount_points,buffer));
	#ifndef TARGET_WIN32
	pthread_mutex_unlock(&mp_mtx);
	#else
	ReleaseMutex(mp_mtx);
	#endif
	int64_t r=strlen(*base);
	if(base[0][r-1]==delim)
		r--;
	return r;
}
static char *GetHomeDirForDrive(char drv) {
	char drv_buf[2]={drv,0};
	#ifndef TARGET_WIN32
	pthread_mutex_lock(&mp_mtx);
	#else
	WaitForSingleObject(mp_mtx,INFINITE);
	#endif
	char *root=*map_get(&mount_points,drv_buf);
        #ifndef TARGET_WIN32
	pthread_mutex_unlock(&mp_mtx);
	#else
	ReleaseMutex(mp_mtx);
	#endif
    if(root) {
		if(*root=='/') { //TODO check for windows
			return HostHomeDir();
		}
	}
	return strdup("/Home");
}
int VFsCd(char *to,int flags) {
	if(!to)  {
		to=VFsDirCur();
		int r=VFsCd(to,flags);
		TD_FREE(to);
		return r; 
	}
	int make=flags&VFS_CDF_MAKE;
    int allow_fail=flags&VFS_CDF_FILENAME_ABS;
    //int only_dirs=flags&VFS_ONLY_DIRS;
    int failed=0;
    int folder_depth=0;
    char *d,*top,*top2,*root,drv=*cur_dir,*start;
    int root_len;
    vec_char_t path;
    vec_init(&path);
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
    char drv_buf[2]={drv,0};
    #ifndef TARGET_WIN32
	pthread_mutex_lock(&mp_mtx);
	#else
	WaitForSingleObject(mp_mtx,INFINITE);
	#endif
    root_len=strlen(root=*map_get(&mount_points,drv_buf));
    #ifndef TARGET_WIN32
	pthread_mutex_unlock(&mp_mtx);
	#else
	ReleaseMutex(mp_mtx);
	#endif
    vec_pusharr(&path,root,strlen(root));
    if(to[0]==TOS_delim) {
		//Is an absolute path
		vec_push(&path,0);
    } else if(to[0]=='~') {
		to++;
		vec_pop(&path);
		root=GetHomeDirForDrive(drv);
		vec_pusharr(&path,root,strlen(root)+1);
		PoopFree(root);
	} else {
        vec_pusharr(&path,cur_dir+2,strlen(cur_dir+2)+1); // +2 for 'drv-letter' and ':'
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
    start=strrchr(path.data,delim);
    if(start) start++;
    else start=path.data;
    if(!strcmp(start,".")) {
        vec_pop(&path);
        vec_pop(&path);
        vec_push(&path,0);
        goto next;
    } else if(!strcmp(start,"..")) {
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
		TD_FREE(cur_dir);
		char buffer[4048];
		if(path.data[root_len]!='/')
			sprintf(buffer,"%s:/%s",drv_buf,path.data+root_len);
		else
			sprintf(buffer,"%s:%s",drv_buf,path.data+root_len);
		cur_dir=PoopAllocSetTask(strdup(buffer),GetFs());
	} else
		vec_deinit(&path);
    return !failed;
}
int64_t VFsDel(char *p) {
	p=__VFsFileNameAbs(p);
	if(!__FExists(p))
		return TD_FREE(p),0;
	#ifdef TARGET_WIN32
	if(__FIsDir(p))
		RemoveDirectory(p);
	else
		DeleteFileA(p);
	#else
	remove(p);
	#endif
	return TD_FREE(p),!__FExists(p);
}
//Returns Host OS location of file
char *__VFsFileNameAbs(char *name) {
    int failed=0,root_len;
    char *file,*old_dir=VFsDirCur(),drv=*cur_dir,*root;
    if(strrchr(name,':')) {
        switch(toupper(*name)) {
            case 'A'...'Z':
            drv=toupper(*name);
            break;
            case ':':
            //Use dft drive?
            break;
        }
        name=strrchr(name,':')+1;
    }
    char dbuf[2]={drv,0};
    vec_char_t path,head;
    vec_init(&path);
    vec_init(&head);
    if(name[0]!=TOS_delim) {
		vec_pusharr(&path,old_dir,strlen(old_dir));
        vec_push(&path,TOS_delim);
        vec_pusharr(&path,name,strlen(name));
        vec_push(&path,0);
    } else {
        vec_push(&path,drv);
		vec_push(&path,':');
        vec_pusharr(&path,name,strlen(name)+1);
    }
    VFsInplaceHostDelims(path.data);
    vec_pusharr(&head,path.data,path.length);
    VFsInplaceConvertDelims(head.data);
    file=strdup(1+strrchr(head.data,TOS_delim));
    strrchr(head.data,TOS_delim)[1]=0;
    failed=!VFsCd(head.data,VFS_CDF_FILENAME_ABS);
    vec_deinit(&path);
    vec_init(&path);
    #ifndef TARGET_WIN32
	pthread_mutex_lock(&mp_mtx);
	#else
	WaitForSingleObject(mp_mtx,INFINITE);
	#endif
    vec_pusharr(&path,*map_get(&mount_points,dbuf),strlen(*map_get(&mount_points,dbuf)));
    #ifndef TARGET_WIN32
	pthread_mutex_unlock(&mp_mtx);
	#else
	ReleaseMutex(mp_mtx);
	#endif
    vec_pusharr(&path,cur_dir+2,strlen(cur_dir+2));
    vec_pusharr(&path,file,strlen(file));
    vec_push(&path,0);
    VFsCd(old_dir,0);
    end:
    vec_deinit(&head);
    PoopFree(file);
    if(!failed)
        return path.data;
    vec_deinit(&path);
    return NULL;
}
#ifndef TARGET_WIN32
int64_t VFsUnixTime(char *name) {
	char *fn=__VFsFileNameAbs(name);
	struct stat s;
	stat(fn,&s);
	int64_t r=mktime(localtime(&s.st_ctime));
	TD_FREE(fn);
	return r;
}
int64_t VFsFSize(char *name) {
	char *fn=__VFsFileNameAbs(name);
	struct stat s;
	stat(fn,&s);
	TD_FREE(fn);
	return s.st_size;
}
#else
int64_t VFsUnixTime(char *name) {
	char *fn=__VFsFileNameAbs(name);
	int64_t t64;
	FILETIME t;
	GetFileTime(fn,NULL,NULL,&t);
	t64=t.dwLowDateTime|(t.dwHighDateTime<<32);
	PoopFree(fn);
	return t64;
}
int64_t VFsFSize(char *name) {
	char *fn=__VFsFileNameAbs(name);
	int64_t s64;
	int32_t h32;
	s64=GetFileSize(fn,&h32);
	s64|=h32<<32;
	PoopFree(fn);
	return fn;
}
#endif
char *VFsFileNameAbs(char *name) {
	if(!name)
		return VFsDirCur();
	char *tmp=__VFsFileNameAbs(name),*r;
    if(!tmp) return NULL;
    char buf[strlen(tmp)+1+3];
    int64_t offset=3;
    if(tmp) {
        if(tmp[0]==TOS_delim)
            offset=2;
        strcpy(buf+offset,tmp+RootPathLen());
        buf[0]=*cur_dir;
        buf[1]=':';
        if(offset==3)
            buf[2]=TOS_delim;
        return strdup(VFsInplaceConvertDelims(buf));
    }
    return NULL;
}
char VFsChDrv(char to) {	
    char old=*cur_dir,buf[2]={toupper(*cur_dir),0},drv;
    switch(toupper(to)) {
        case 'A'...'Z':
        drv=toupper(to);
        goto end;
        break;
        end:
        if(map_get(&drive_dirs,buf))
			PoopFree(*map_get(&drive_dirs,buf));
		map_set(&drive_dirs,buf,strdup(cur_dir));
		buf[0]=drv;
		PoopFree(cur_dir);
		if(map_get(&drive_dirs,buf)) {
			cur_dir=strdup(*map_get(&drive_dirs,buf));
		} else
			cur_dir=strdup("/");
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
    TD_FREE(name);
    return !!name;
}
int64_t VFsFileRead(char *name,int64_t *len) {
    if(len) *len=0;
    FILE *f;
    int64_t s,e;
    void *data=NULL;
    name=__VFsFileNameAbs(name);
    if(!name) return NULL;
    if(__FExists(name))
    if(!__FIsDir(name)) {
        f=fopen(name,"rb");
        if(!f) goto end;
        s=ftell(f);
        fseek(f,0,SEEK_END);
        e=ftell(f);
        fseek(f,0,SEEK_SET);
        fread(data=TD_MALLOC(e-s+1),1,e-s,f);
        fclose(f);
        if(len) *len=e-s;
    }
    end:
    TD_FREE(name);
    return data;
}
void VFsThrdInit() {
	#ifndef TARGET_WIN32
	pthread_mutex_lock(&mp_mtx);
	#else
	WaitForSingleObject(mp_mtx,INFINITE);
	#endif
	map_init(&drive_dirs);
	cur_dir=strdup("T:/");
	#ifndef TARGET_WIN32
    pthread_mutex_unlock(&mp_mtx);
	#else
	ReleaseMutex(mp_mtx);
	#endif
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
			TD_FREE(s);
		}
	}
}
void CreateTemplateBootDrv(char *to,char *template,int overwrite) {
	if(!overwrite)
		if(__FExists(to)) {
			VFsMountDrive('T',to);
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
    VFsMountDrive('T',to);
}
int VFsFileExists(char *path) {
	path=__VFsFileNameAbs(path);
	int e=__FExists(path);
	PoopFree(path);
	return e;
}
int VFsMountDrive(char let,char *path) {
	char buf[2]={let,0};
	#ifndef TARGET_WIN32
	pthread_mutex_lock(&mp_mtx);
	#else
	WaitForSingleObject(mp_mtx,INFINITE);
	#endif
	map_set(&mount_points,buf,strdup(path));
	#ifndef TARGET_WIN32
	pthread_mutex_unlock(&mp_mtx);
	#else
	ReleaseMutex(mp_mtx);
	#endif
}
