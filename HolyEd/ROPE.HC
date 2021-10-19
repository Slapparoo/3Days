#define ROPE_LEN 48

I64 x=10;

class CRope {
  CRope *left,*right,*parent;
  I64 text_length;
  U8 isLeaf;
  U8 buffer[ROPE_LEN+1];
};
CRope *RopeCondense(CRope *r);
//a can be NULL for an empty leaf node
CRope *RopeAppendText(CRope *a,U8 *text);
I64 x=10;
I64 RopeLength(CRope *rope) {
  if(!rope) return 0;
  if(rope->isLeaf) return rope->text_length;
  return RopeLength(rope->left)+RopeLength(rope->right);
}
CRope *GetNodeAtI(CRope *root,I64 i,I64 *ri=NULL) {
  if(root->isLeaf) {
    if(ri) *ri=i;
    return root;
  }
  I64 w;
  if((w=RopeLength(root->left))<=i&&root->right) {
    if(i-w>=RopeLength(root->right)) throw('rope');
    return GetNodeAtI(root->right,i-w,ri);
  }
  return GetNodeAtI(root->left,i,ri);
}
CRope **GetParentList(CRope *node,I64 *cnt) {
  CRope *orig=node;
  I64 cnt2=0;
  while(node) cnt2++,node=node->parent;
  CRope **ret=MAlloc(cnt2+1);
  cnt2=0;
  node=orig;
  while(node) {
    ret[cnt2++]=node;
    node=node->parent;
  }
  ret[cnt2]=NULL;
  if(cnt) *cnt=cnt2;
  return ret;
}
U0 DisconnectFromPar(CRope *n) {
  if(!n->parent) return;
  if(n->parent->left==n) n->parent->left=NULL;
  else if(n->parent->right==n) n->parent->right=NULL;
  n->parent=NULL;
}
CRope *RopeCat(CRope *a,CRope *b) {
  CRope *n=MAlloc(sizeof(CRope));
  n->left=a,n->right=b,n->parent=NULL,n->isLeaf=FALSE;
  return n;
}
//https://stackoverflow.com/questions/21958560/how-to-split-a-rope-tree
U0 RopeSplit(CRope *root,I64 at,CRope **left,CRope **right) {
  if(root->isLeaf) {
    *left=root;
    *right=RopeAppendText(NULL,&root->buffer[at]);
    left[0]->buffer[at]=0;
    left[0]->text_length=at;
    return;
  }
  I64 w;
  CRope *left2,*right2;
  if(at<=(w=RopeLength(root->left))) {
    RopeSplit(root->left,at,left,&right2);
    root->right=RopeCat(right2,root->right);
    root->right->parent=root;
    *right=root->right;
  } else if(at>w) {
    RopeSplit(root->right,at-w,&left2,right);
    root->left=RopeCat(root->left,left2);
    root->left->parent=root;
    *left=root->left;
  }
}
//a can be NULL for an empty leaf node
CRope *RopeAppendText(CRope *a,U8 *text) {
  U8 buffer[ROPE_LEN+1];
  buffer[ROPE_LEN]=0;
  CRope *r=MAlloc(sizeof(CRope));
  I64 len;
  r->isLeaf=TRUE,StrNCpy(r->buffer,text,len=MinI64(ROPE_LEN,StrLen(text)));
  buffer[len]=0;
  r->text_length=len;
  if(text[len]!=0)
    r=RopeCat(r,RopeAppendText(NULL,text+len));
  if(!a) return r;
  return RopeCat(a,r);
}
U8 RopeChar(CRope *r,I64 i) {
  I64 ri;
  CRope *n=GetNodeAtI(r,i,&ri);
  return n->buffer[ri];
}
CRope *RopeInsText(CRope *r,U8 *text,I64 at) {
  CRope *left,*right;
  RopeSplit(r,at,&left,&right);
  return RopeCat(RopeAppendText(left,text),right);
}
U0 RopeFree(CRope *r) {
  if(r->isLeaf) goto f;
  if(r->left) RopeFree(r->left);
  if(r->right) RopeFree(r->right);
  f:
  Free(r);
}
CRope *RopeCondense(CRope *r) {
  if(r->isLeaf) return r;
  U8 buffer[ROPE_LEN+1];
  I64 lw=RopeLength(r->left),rw=RopeLength(r->right);
  if(lw+rw<=ROPE_LEN) {
    I64 i=0;
    for(;i!=lw;i++)
      buffer[i]=RopeChar(r->left,i);
    for(i=0;i!=rw;i++)
      buffer[i+lw]=RopeChar(r->right,i);
    buffer[lw+rw]=0;
    CRope *new=RopeAppendText(NULL,buffer);
    CRope *oldpar=r->parent;
    RopeFree(r);
    if(oldpar) {
      if(oldpar->left==r)
        oldpar->left=new;
      else
        oldpar->right=new;
    }
    return new;
  }
  r->left=RopeCondense(r->left);
  r->right=RopeCondense(r->right);
  return r;
}
CRope *RopeDelText(CRope *r,I64 s,I64 e) {
  CRope *l1,*r1;
  RopeSplit(r,s,&l1,&r1);
  CRope *l2,*r2;
  RopeSplit(r1,e-s,&l2,&r2);
  RopeFree(l2);
  return RopeCat(l1,r2);
}
I64 __Rope2Str(CRope *r,U8 *writeto,I64 off) {
  if(r->isLeaf) {
    StrNCpy(writeto+off,r->buffer,r->text_length);
    return off+r->text_length;
  }
  off=__Rope2Str(r->left,writeto,off);
  off=__Rope2Str(r->right,writeto,off);
  return off;
}
U8 *Rope2Str(CRope *r) {
  U8 *ret=MAlloc(RopeLength(r)+1);
  __Rope2Str(r,ret,0);
  return ret;
}
U8 *str="123456789";
CRope *r=NULL;
I64 x=0;
for(;x!=StrLen(str);x++) {
  U8 chr[2];
  chr[1]=0;
  chr[0]=str[x];
  r=RopeAppendText(r,chr);
}
r=RopeCondense(r);
for(;x!=StrLen(str);x++)
  if(str[x]!=RopeChar(r,x))
    "FAIL AT %d:(%c,%c)\n",x,str[x],RopeChar(r,x);
CRope *li,*ri;
RopeSplit(r,6,&li,&ri);
for(x=0;x!=6;x++)
  if(str[x]!=RopeChar(li,x))
    "FAIL AT %d:(%c,%c)\n",x,str[x],RopeChar(li,x);
x=0;
for(;x!=3;x++)
  if(str[x+6]!=RopeChar(ri,x))
    "FAIL AT %d:(%c,%c)\n",x,str[x+6],RopeChar(ri,x);
r=RopeAppendText(NULL,str);
r=RopeInsText(r,"abc",4);
str="1234abc56789";
for(;x!=StrLen(str);x++)
  if(str[x]!=RopeChar(r,x))
    "FAIL AT %d:(%c,%c)\n",x,str[x],RopeChar(r,x);
r=RopeDelText(r,4,4+3);
str="123456789";
for(x=0;x!=StrLen(str);x++)
  if(str[x]!=RopeChar(r,x))
    "FAIL AT %d:(%c,%c)\n",x,str[x],RopeChar(r,x);
