U0 DirEntryDel(CDirEntry *tmpde)
{   //Free node returned from \dLK,"FilesFind",A="MN:FilesFind"\d().  Doesn't Free user_data.
//Does not change the directory on disk.
    if (tmpde) {
        Free(tmpde->full_name);
        Free(tmpde);
    }
}

U0 DirEntryDel2(CDirEntry *tmpde)
{   //Free node returned from \dLK,"FilesFind",A="MN:FilesFind"\d().  Frees user_data
//Does not change the directory on disk.
    if (tmpde) {
        Free(tmpde->full_name);
        Free(tmpde->user_data);
        Free(tmpde);
    }
}

U0 DirTreeDel(CDirEntry *tmpde)
{   //Free tree returned from \dLK,"FilesFind",A="MN:FilesFind"\d().  Doesn't Free user_data.
//Does not change the directory on disk.
    CDirEntry *tmpde2;
    while (tmpde) {
        tmpde2=tmpde->next;
        if (tmpde->sub)
            DirTreeDel(tmpde->sub);
        DirEntryDel(tmpde);
        tmpde=tmpde2;
    }
}

U0 DirTreeDel2(CDirEntry *tmpde)
{   //Free tree returned from \dLK,"FilesFind",A="MN:FilesFind"\d().  Frees user_data
//Does not change the directory on disk.
    CDirEntry *tmpde2;
    while (tmpde) {
        tmpde2=tmpde->next;
        if (tmpde->sub)
            DirTreeDel2(tmpde->sub);
        DirEntryDel2(tmpde);
        tmpde=tmpde2;
    }
}

I64 DirEntryCompareName(CDirEntry *e1,CDirEntry *e2)
{
    U8 buf1[CDIR_FILENAME_LEN],buf2[CDIR_FILENAME_LEN],
    buf3[CDIR_FILENAME_LEN],buf4[CDIR_FILENAME_LEN];
    I64 d1=0,d2=0;
    if (IsDir(e1->full_name))
        d1=1;
    if (IsDir(e2->full_name))
        d2=1;
    if (d1!=d2)
        return d2-d1;
    else {
        StrCpy(buf1,e1->name);
        StrCpy(buf2,e2->name);
        FileExtRem(buf1,buf3);
        FileExtRem(buf2,buf4);
        if (d1=StrCmp(buf3,buf4))
            return d1;
        return StrCmp(buf1,buf2);
    }
}

I64 DirEntryCompareClus(CDirEntry *e1,CDirEntry *e2)
{
    return e1->clus-e2->clus;
}

#define SK_NAME		0
#define SK_CLUS	1

U0 DirFilesSort(CDirEntry **_tmpde,I64 key)
{
    I64 i,cnt;
    CDirEntry *tmpde=*_tmpde,*tmpde1,**sort_buf;
    if (tmpde) {
        cnt=LinkedLstCnt(tmpde);
        if (cnt>1) {
            sort_buf=MAlloc(cnt*sizeof(U8 *));
            i=0;
            tmpde1=tmpde;
            while (tmpde1) {
                sort_buf[i++]=tmpde1;
                tmpde1=tmpde1->next;
            }
            switch [key] {
            case SK_NAME:
                QSortI64(sort_buf,cnt,&DirEntryCompareName);
                break;
            case SK_CLUS:
                QSortI64(sort_buf,cnt,&DirEntryCompareClus);
                break;
            }
            tmpde=sort_buf[0];
            *_tmpde=tmpde;
            for (i=0; i<cnt-1; i++) {
                tmpde1=sort_buf[i];
                tmpde1->next=sort_buf[i+1];
            }
            tmpde1=sort_buf[i];
            tmpde1->next=NULL;
            Free(sort_buf);

            tmpde1=tmpde;
            while (tmpde1) {
                if (tmpde1->sub)
                    DirFilesSort(&tmpde1->sub,key);
                tmpde1=tmpde1->next;
            }
        } else if (tmpde->sub)
            DirFilesSort(&tmpde->sub,key);
    }
}

CDirEntry *DirFilesFlatten(CDirEntry *tmpde,CDirEntry **_res,I64 fuf_flags)
{   //Returns last node
    CDirEntry *tmpde1;
    Bool del;
    if (tmpde)
        while (TRUE) {
            tmpde1=tmpde->next;
            if (!IsDir(tmpde->full_name)||!(fuf_flags&FUF_JUST_FILES)) {
                _res=*_res=tmpde;
                del=FALSE;
            } else
                del=TRUE;
            if (tmpde->sub) {
                _res=DirFilesFlatten(tmpde->sub,_res,fuf_flags);
                tmpde->sub=NULL;
            }
            if (del)
                DirEntryDel(tmpde);
            if (tmpde1)
                tmpde=tmpde1;
            else
                break;
        }
    *_res=NULL;
    return _res;
}
