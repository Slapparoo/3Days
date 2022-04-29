#ifndef VECTOR_H
#define VECTOR_H
class CVector {
    I64 *data;
    I64 cap,size;
};
CVector *VectorNew() {
    return MAlloc(sizeof(CVector));
}
U0 VectorFree(CVector *v) {
    Free(v->data);
    Free(v);
}
U0 VectorPush(CVector *vec,I64 v) {
    I64 *nmem;
    if(vec->cap<=1+vec->size) {
        nmem=MAlloc((vec->cap=2*(vec->cap+1))*sizeof(I64));
        MemNCpy(nmem,vec->data,MSize(vec->data));
        Free(vec->data);
        vec->data=nmem;
    }
    vec->data[vec->size++]=v;
}
I64 VectorPop(CVector *vec) {
    if(vec->size==0) {
        "%s",1323;
        throw('Vector');
    }
    return vec->data[--vec->size];
}
U0 VectorAdd(CVector *vec,I64 v) {
    I64 iter=vec->size;
    while(--iter>=0)
        if(vec->data[iter]==v) return;
    VectorPush(vec,v);
}
U0 VectorAddAll(CVector *vec,CVector *vec2) {
    I64 iter=vec2->size;
    while(--iter>=0)
        VectorAdd(vec,vec2->data[iter]);    
}
U0 VectorRemove(CVector *vec,I64 v) {
    I64 idx;
    for(idx=0;idx<vec->size;) {
        if(vec->data[idx]==v)
            vec->data[idx]=VectorPop(vec);
        else
            idx++;
    }
}
#endif

