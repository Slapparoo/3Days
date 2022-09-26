CQue valid_blks;
I64 mem_blks_lock;
U0 InitHeaps() {
  mem_blks_lock=0;
  QueInit(&valid_blks);
}
U0 SysBadFree(I64 *ptr)
{
   "AT:%p\n",ptr;
   throw('BadFree');
}

U0 SysBadMAlloc(I64 *ptr)
{
  throw('BadMallo');
}
#ifdef BC_ENABLE
class CBlkCacheEnt {
  U8 *s,*e;
  CMemBlk *mem_blk;
  I64 spin_lock;
};
#define BLK_CACHE_MASK 0xfff
class CBlkCache {
  CBlkCacheEnt ents[BLK_CACHE_MASK+1][16];
  I64 indexes[BLK_CACHE_MASK+1];
  I64 spin_lock;
} blk_cache;
I64 HashPtr(U64 ptr) {
  ptr>>=MEM_PAG_BITS;
  return ptr*((ptr>>8)&0xff);
}
CMemBlk *GetMemBlkFromCache(CBlkCache *cache,U8 *ptr) {
  I64 bucket=HashPtr(ptr)&BLK_CACHE_MASK,idx;
  CBlkCacheEnt *ent;
  for(idx=0;idx!=16;idx++) {
    ent=&cache->ents[bucket][idx];
    while(LBts(&ent->spin_lock,0))
      PAUSE;
    if(ent->s<=ptr<ent->e) {
      LBtr(&ent->spin_lock,0);  
      return ent->mem_blk;
    }
    LBtr(&ent->spin_lock,0);  
  }
  return NULL;
}
U0 MemBlkCacheAdd(CBlkCache *cache,CMemBlk *blk) {
  I64 p=blk->pags,bucket,i;
  U8 *ref=blk;
  CBlkCacheEnt *ent;
  while(p--) {
    bucket=HashPtr(blk(U8*)+p<<MEM_PAG_BITS)&BLK_CACHE_MASK;
    while(LBts(&cache->spin_lock,0))
      PAUSE;
    i=cache->indexes[bucket]++;
    cache->indexes[bucket]&=0xf;
    ent=&cache->ents[bucket][i];
    ent->s=blk;
    ent->e=blk(U8*)+blk->pags<<MEM_PAG_BITS;
    ent->mem_blk=blk;
    LBtr(&cache->spin_lock,0);
  }
}
U0 MemBlkCacheRemove(CBlkCache *cache,CMemBlk *blk) {
  I64 p=blk->pags,bucket,i;
  U8 *ref=blk;
  CBlkCacheEnt *ent;
  while(p--) {
    bucket=HashPtr(blk(U8*)+p<<MEM_PAG_BITS)&BLK_CACHE_MASK;
    while(LBts(&cache->spin_lock,0))
      PAUSE;
    for(i=0;i!=16;i++) {
      ent=&cache->ents[bucket][i];
      if(ent->mem_blk==blk) {
        while(LBts(&ent->spin_lock,0))
	  PAUSE;
        ent->s=NULL;
        ent->e=NULL;
        ent->mem_blk=NULL;
        LBtr(&ent->spin_lock,0);
      }
    }
    LBtr(&cache->spin_lock,0);
  }
}
Bool IsValidPtr(U8 *ptr,I64 sz=1) {
  CMemBlk *blk;
  I64 addr,k;
  U8 *src;
  if(blk=GetMemBlkFromCache(&blk_cache,ptr)) {
  } else {
    while(LBts(&mem_blks_lock,0))
      PAUSE;
    blk=valid_blks.next;
    while(blk!=&valid_blks) {
      blk=blk(U8*)-CMemBlk.next2;
      if(blk(U8*)<=ptr<blk(U8*)+blk->pags<<MEM_PAG_BITS) {
        MemBlkCacheAdd(&blk_cache,blk);
        LBtr(&mem_blks_lock,0);
        goto en;
      }
      blk=blk(U8*)+CMemBlk.next2;
      blk=blk->next;
    }
    LBtr(&mem_blks_lock,0);
//TODO
//Could be allocated outside of holyC heap(from 3d_loader?)
    return FALSE; 
f:
    for(k=0;TRUE;k++) {
      //IsValidPtr+1
      //_SYS_WHINE_OOB+2
      if(!Caller(k+2)) break;
      src=SrcEdLink(Caller(k+2));
      if(src)
        TOSPrint("[%d] %s\n",k+1,src);
      else
        TOSPrint("[%d] ???\n",k+1);
      Free(src);
    }
    return FALSE;
  }
en:
  //https://static.googleusercontent.com/media/research.google.com/en//pubs/archive/37752.pdf
  //Here the deal,the allocator provides rounded to 8-byte sizes,with 8-byte "useless" room
  //So we only need to check for an 8-byte  practically
  addr=(ptr-blk(U8*))>>3;
  /*
  k=blk->shadow[addr];
  if(k&&(addr&7)+sz>k)
    return TRUE;
  */
  if(!blk->shadow[addr])
    goto f;
  return TRUE;
}
#else
Bool IsValidPtr(U8 *ptr,I64 sz=1) {
  "Use '-b' on 3d_loader to enable bounds-checker.\n";
  throw('UseBC');
};
#endif
U8 *MemPagAlloc(I64 pags,CBlkPool *bp=NULL)
{/*Alloc pags from BlkPool. Don't link to task.
(Linking to a task means they will be freed when the task dies.)
It might give you more than you asked for.

Return: NULL if out of memory.
*/
  CMemBlk *res=NULL,*m;
  I64 i;
  Bool old;
  PUSHFD
  old=!Bts(&(Fs->task_flags),TASKf_BREAK_LOCKED); 
  res=NewVirtualChunk(pags<<MEM_PAG_BITS,!bp);
  ins:
  QueInit(res);
  QueInit(&res->next2);
  while(LBts(&mem_blks_lock,0))
    PAUSE;
  QueIns(&res->next2,valid_blks.last);
  LBtr(&mem_blks_lock,0);
  res->pags=pags;
  res->mb_signature=MBS_UNUSED_SIGNATURE_VAL;
#ifdef BC_ENABLE
  MemBlkCacheAdd(&blk_cache,res);
  res->shadow=NewVirtualChunk(pags<<MEM_PAG_BITS/8,FALSE);
  MemSet(res->shadow,FALSE,pags<<MEM_PAG_BITS/8);
  MemSet(res->shadow,8,sizeof CMemBlk/8);
#endif

at_done:
at_done2:
  if(old)
	BreakUnlock;
  POPFD
  return res;
}

U0 MemPagFree(CMemBlk *m,CBlkPool *bp=NULL)
{//Return non-task pags to BlkPool.
  I64 i,pags;
  Bool old;
  if (m) {
    PUSHFD
    old=!Bts(&(Fs->task_flags),TASKf_BREAK_LOCKED); 
    pags=m->pags;
    m->mb_signature=MBS_UNUSED_SIGNATURE_VAL;
#ifdef BC_ENABLE
    MemBlkCacheRemove(&blk_cache,m);
    FreeVirtualChunk(m->shadow,pags<<MEM_PAG_BITS/8); 
#endif
    FreeVirtualChunk(m,pags<<MEM_PAG_BITS);
    if(old)
	  BreakUnlock;
    POPFD
  }
}

CMemBlk *MemPagTaskAlloc(I64 pags,CHeapCtrl *hc)
{/*hc must be locked.  Don't preempt this routine.
Currently, this is only called from $LK,"MAlloc",A="MN:MAlloc"$().
Return: NULL if out of memory.
*/
  CMemBlk *res;
  I64 threshold,cnt,size;
  CMemUnused *uum,**_uum,**_ptr;
  if (res=MemPagAlloc(pags,hc->bp)) {
    QueIns(res,hc->last_mem_blk);
    res->mb_signature=MBS_USED_SIGNATURE_VAL;
    hc->alloced_u8s+=res->pags<<MEM_PAG_BITS;

    //Tidy-up free lst (Move into heap hash)
    //because if free lst gets long, delay causes crash.
    threshold=MEM_HEAP_HASH_SIZE>>4;
#assert MEM_HEAP_HASH_SIZE>>4>=sizeof(U8 *)
    do {
      cnt=0;
      _uum=&hc->malloc_free_lst;
      while (uum=*_uum) {
#assert !offset(CMemUnused.next)
	size=uum->size;
	if (size<threshold) {
	  *_uum=uum->next;
	  _ptr=(&hc->heap_hash)(U8 *)+size;
	  uum->next=*_ptr;
	  *_ptr=uum;
	} else {
	  cnt++;
	  _uum=uum;
	}
      }
      threshold<<=1;
    } while (cnt>8 && threshold<=MEM_HEAP_HASH_SIZE);
  }
  return res;
}

U0 MemPagTaskFree(CMemBlk *m,CHeapCtrl *hc)
{//hc must be locked
  Bool old;
  if (m) {
    PUSHFD
    old=!Bts(&(Fs->task_flags),TASKf_BREAK_LOCKED); 
    if (m->mb_signature!=MBS_USED_SIGNATURE_VAL)
      SysBadFree(m);
    else {
      QueRem(m);
      while(LBts(&mem_blks_lock,0))
        PAUSE;
      QueRem(&m->next2);
      LBtr(&mem_blks_lock,0);
      hc->alloced_u8s-=m->pags<<MEM_PAG_BITS;
      MemPagFree(m,NULL);
    }
    if(old)
	  BreakUnlock;
    POPFD
  }
}

asm {
	_SYS_WHINE_OOB::
	PUSH	RBP
	MOV	RBP,RSP
 	PUSH_C_REGS
	PUSH	U64 SF_ARG2[RBP]
	PUSH	U64 SF_ARG1[RBP]
	CALL	&IsValidPtr
	POP_C_REGS
	LEAVE
	RET1	16
};
Bool IsInValidBlk(U8 *ptr) {
  Bool ret=FALSE;
  CMemBlk *blks=valid_blks.next;
  while(LBts(&mem_blks_lock,0))
    PAUSE;
  while(blks!=&valid_blks) {
    blks=blks(U8*)-CMemBlk.next2;  
    if(blks(U8*)<=ptr<blks(U8*)+blks->pags<<MEM_PAG_BITS) {
      ret=TRUE;
      break;
    }
    blks=blks(U8*)+CMemBlk.next2;
    blks=blks->next;
  }
  LBtr(&mem_blks_lock,0);
  return ret;
}