class CHash {
  CHash* next;
  U8* str;
  U32 type, use_cnt;
};
class CHashTable {
  I64 mask;
  CHash** body;
};
CHashTable* HashTableNew(I64 size, U0* pool = NULL) {
  CHashTable* t = MAlloc(sizeof(CHashTable));
  t->body = MAlloc(sizeof(CHash) * size);
  t->mask = size - 1;
  return t;
}
I64 HashStr(U8* str) {
  I64 r = 0;
  I64 l = StrLen(str);

  while (--l >= 0) {
    r *= 31;
    r += str[l];
  }

  return r;
}
U0 HashAdd(CHash* tmp, CHashTable* t) {
  tmp->next = NULL;
  I64 hash = HashStr(tmp->str);
  CHash* bucket = t->body[hash & t->mask];
  if (!bucket) {
    t->body[hash & t->mask] = tmp;
    return;
  }
  CHash *prev=NULL;
  for(;bucket;bucket=bucket->next) {
    if(!StrCmp(bucket->str,tmp->str)) {
      tmp->next=bucket->next;
      if(prev) prev->next=tmp;
      else t->body[hash & t->mask] = tmp;
      return;
    }
    prev=bucket;
  }
  prev->next=tmp;
  return;
}
CHash* HashFind(U8* needle, CHashTable* table) {
  I64 hash = HashStr(needle);
  CHash* bucket = table->body[hash & table->mask];

  while (bucket) {
    I64 cmp = StrCmp(needle, bucket->str);

    if (0 == cmp) return bucket;

    //if(cmp<0) break;
    bucket = bucket->next;
  }

  return NULL;
}
U0 HashTableDel(CHashTable* table, U0(*free)(CHash* h) = NULL) {
  I64 s = table->mask + 1;

  while (--s >= 0) {
    CHash* h = table->body[s];
    if(!h) goto next;
loop:
    CHash* next = h->next;

    if (free) free(h);

    Free(h->str);
    Free(h);
    h = next;
    next:
  }

  Free(table->body);
  Free(table);
}

