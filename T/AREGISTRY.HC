Bool OneTimePopUp(U8 *_flags,I64 flag_num,U8 *msg)
{//See \dLK,"::/Apps/X-Caliber/X-Caliber.HC"\d.
  Bool res=FALSE;
  CDoc *doc=DocNew;
  CDocEntry *doc_e;
  if (!Bt(_flags,flag_num)) {
    if (msg) DocPrint(doc,"%s",msg);
    doc_e=DocPrint(doc,"\n\dCB,\"Do not show this msg again.\",LE=1\d");
    DocPrint(doc,"\dCM+CX,0,4\d\dBT,\"OKAY\",LE=1\d\n");
    if (PopUpMenu(doc)==1 && doc_e->de_flags&DOCEF_CHECKED_COLLAPSED) {
      LBts(_flags,flag_num);
      res=TRUE;
    }
    DocDel(doc);
  }
  return res;
}
