#define RS_ATTR_READ_ONLY      0x01      
#define RS_ATTR_HIDDEN         0x02      
#define RS_ATTR_SYSTEM         0x04      
#define RS_ATTR_VOL_ID         0x08      
#define RS_ATTR_DIR            0x10      
#define RS_ATTR_ARCHIVE        0x20      
#define RS_ATTR_DELETED        0x100     
#define RS_ATTR_RESIDENT       0x200     
#define RS_ATTR_COMPRESSED     0x400     
#define RS_ATTR_CONTIGUOUS     0x800     
#define RS_ATTR_FIXED          0x1000
#define RS_BLK_SIZE            512
#define RS_DRV_OFFSET	       0xB000
#define RS_ROOT_CLUS           0x5A
class CDirEntryPhys {
  U16	attr;
  U8	name[38];
  I64	clus,size;
  CDate datetime;
};
U0 WriteDirent(U8 *body,U8 *cbody,U8 *to) {
	loop:;
	CDirEntryPhys *dirent=cbody;
	I64 len;
	if(!StrLen(dirent->name)) return;
	if(!StrCmp(".",dirent->name)||!StrCmp("..",dirent->name)) goto next;
	U8 *full_name=MStrPrint("%s/%s",to,dirent->name),*buf;
	if(dirent->attr&RS_ATTR_DIR) {
		DirMk(full_name);
		if(body+RS_BLK_SIZE*dirent->clus!=cbody)
			WriteDirent(body,body+RS_BLK_SIZE*dirent->clus,full_name);
	} else {
		if(IsDotZ(full_name)) {
			buf=ExpandBuf(body+RS_BLK_SIZE*dirent->clus,Fs);
			FileWrite(full_name,buf,(body+RS_BLK_SIZE*dirent->clus)(CArcCompress*)->expanded_size);
			Free(buf);
		} else
			FileWrite(full_name,body+RS_BLK_SIZE*dirent->clus,dirent->size);
	}
	Free(full_name);
	next:
	cbody+=sizeof(CDirEntryPhys);
	goto loop;
} 
public U0 ReadISOFile(U8 *isoc,U8 *to=".") {
	I64 len,offset;
	CDirEntryPhys *cur;
	U8 *body=FileRead(isoc,&len);
	offset=RS_DRV_OFFSET;
	offset+=(1+body[offset+0x20])*RS_BLK_SIZE;
	WriteDirent(body,offset+body,to);
	Free(body);
}
//Example usage.
//ReadISOFile("TOS_Distro.ISO","Distro");
