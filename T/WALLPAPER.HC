U0 WallPaper(CTask *_task)
{
  _task->text_attr=BLUE<<4+WHITE;
}
U0 DrawTermBttn(CDC *dc,CCtrl *c)
{
  U8 *str="Term";
  if (Blink) {
    dc->color=YELLOW;
    GrRect(dc,c->left,c->top,100,100);
    dc->color=BLACK;
    GrPrint(dc,c->left+100/2-FONT_HEIGHT/2,c->top+100/2-(StrLen(str)*FONT_WIDTH)/2,str);
  }
}

U0 LeftClickTermBttn(CCtrl *,I64,I64,Bool down)
{
  if (down)
    User;
}

CCtrl *TermBttnNew()
{
  I64 min_x=0,max_x=100,min_y=0,max_y=100;
  CCtrl *c=CAlloc(sizeof(CCtrl));

  c->win_task=sys_winmgr_task;
  c->flags=CTRLF_SHOW;
  c->type=CTRLT_GENERIC;
  c->draw_it=&DrawTermBttn;
  c->left_click=&LeftClickTermBttn;

//min must be zero
  c->left=sys_winmgr_task->pix_width-(max_x-min_x+1);
  c->right=c->left+(max_x-min_x+1)-1;
  c->top=sys_winmgr_task->pix_height-(max_y-min_y+1);
  c->bottom=c->top+(max_y-min_y+1)-1;

  QueIns(c,sys_winmgr_task->last_ctrl);
  TaskDerivedValsUpdate(sys_winmgr_task);

  return c;
}

U0 WallPaperInit()
{
  TermBttnNew;
  gr.fp_wall_paper=&WallPaper;
}
