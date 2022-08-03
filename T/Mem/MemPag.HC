CQue valid_blks;
CQue recycle32;
I64 mem_blks_lock;
U0 InitHeaps() {
	mem_blks_lock=0;
	QueInit(&valid_blks);
	QueInit(&recycle32);
}
Bool IsValidPtr(U8 *ptr) {
	return TRUE;
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
  if(!bp) {
	//Look for recylceable 32bit pages
	while(LBts(&mem_blks_lock,0))
	  PAUSE;
	for(m=recycle32.next;m!=&recycle32;m=m->next)
	  if(m->pags>=pags) {
	     res=m;
	     QueRem(res);
	     LBtr(&mem_blks_lock,0);
	     goto ins;
	  }
	LBtr(&mem_blks_lock,0);
  }
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
  Bool old;
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
      if(!(0<=m<=U32_MAX))
		MemPagFree(m,NULL);
      else {
        while(LBts(&mem_blks_lock,0))
          PAUSE;
        QueInit(m);
        QueIns(m,recycle32.last);
        LBtr(&mem_blks_lock,0);
      }
    }
    if(old)
	  BreakUnlock;
    POPFD
  }
}

