//https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
#define ED_KEY_UP 1+512
#define ED_KEY_DOWN 2+512
#define ED_KEY_RIGHT 3+512
#define ED_KEY_LEFT 4+512
#define ED_KEY_HOME 5+512
#define ED_KEY_END 6+512
#define ED_KEY_CTRL_UP 7+512
#define ED_KEY_CTRL_DOWN 8+512
#define ED_KEY_CTRL_LEFT 9+512
#define ED_KEY_CTRL_RIGHT 10+512
#define ED_KEY_BACKSPACE 11+512
#define ED_KEY_ESCAPE 12+512
#define ED_KEY_INSERT 13+512
#define ED_KEY_DELETE 14+512
#define ED_KEY_PAGEUP 15+512
#define ED_KEY_PAGEDOWN 16+512
#define ED_KEY_F1 17+512
#define ED_KEY_F2 18+512
#define ED_KEY_F3 19+512
#define ED_KEY_F4 20+512
#define ED_KEY_F5 21+512
#define ED_KEY_F6 22+512
#define ED_KEY_F7 23+512
#define ED_KEY_F8 24+512
#define ED_KEY_F9 25+512
#define ED_KEY_F10 26+512
#define ED_KEY_F11 27+512
#define ED_KEY_F12 28+512
#define ED_KEY_RESIZE 29+512
#define ED_KEY_SHIFT_LEFT 30+512
#define ED_KEY_SHIFT_RIGHT 31+512
#define ED_KEY_SHIFT_UP 32+512
#define ED_KEY_SHIFT_DOWN 33+512
I64 ALT_KEY(U8 chr) {return ToUpper(chr)+256;}
#if !IsWindows
I64 GetKey(WINDOW *w) {
  wtimeout(w,100);
  raw;
  I64 chr=wgetch(w);
  if(chr==KEY_RESIZE) {
    clear;
    endwin;
    refresh;
    initscr;
    clear;
    resize_term(0,0);
    return ED_KEY_RESIZE;
  }
  if(chr==0x7f) return ED_KEY_BACKSPACE;
  if(chr!='\x1b')
    return chr;
  I64 can;
  switch(chr=wgetch(w)) {
    case 0:
    case ERR:
    return ED_KEY_ESCAPE;
    case 'a'...'z':
    return ALT_KEY(chr);
    case 'O':
    switch(chr=wgetch(w)) {
      case 'P': return ED_KEY_F1;
      case 'Q': return ED_KEY_F2;
      case 'R': return ED_KEY_F3;
      case 'S': return ED_KEY_F4;
    }
    goto undef;
    case '[':
    switch(chr=wgetch(w)) {
      case 'A': return ED_KEY_UP;
      case 'B': return ED_KEY_DOWN;
      case 'C': return ED_KEY_RIGHT;
      case 'D': return ED_KEY_LEFT;
      case 'H': return ED_KEY_HOME;
      case 'F': return ED_KEY_END;
      start:
      case '2': can=ED_KEY_INSERT; break;
      case '3': can=ED_KEY_DELETE; break;
      case '5': can=ED_KEY_PAGEUP; break;
      case '6': can=ED_KEY_PAGEDOWN; break;
      end:
      if((chr=wgetch(w))=='~') return can;
      break;
      case '1':
      chr=wgetch(w);
      if(chr==';') {
        chr=wgetch(w);
        if(chr=='5') {
          switch(chr=wgetch(w)) {
            case 'A': return ED_KEY_CTRL_UP;
            case 'B': return ED_KEY_CTRL_DOWN;
            case 'C': return ED_KEY_CTRL_RIGHT;
            case 'D': return ED_KEY_CTRL_LEFT;
          }
        }
      }
      switch(chr) {
      start:
          case '5': can=ED_KEY_F5; break;
          case '7': can=ED_KEY_F6; break;
          case '8': can=ED_KEY_F7; break;
          case '9': can=ED_KEY_F8; break;
        end:
          if((chr=wgetch(w))=='~') return can;
        }
      goto undef;
      }
      case '2':
        can=0;
        switch(chr=wgetch(w)) {
        start:
          case '0': can=ED_KEY_F9; break;
          case '1': can=ED_KEY_F10; break;
          case '3': can=ED_KEY_F11; break;
          case '4': can=ED_KEY_F12; break;
        //Tested on Terminator for linux
          case 'A': return ED_KEY_SHIFT_UP;
            case 'B': return ED_KEY_SHIFT_DOWN;
            case 'C': return ED_KEY_SHIFT_RIGHT;
            case 'D': return ED_KEY_SHIFT_LEFT;
        end:
          if((chr=wgetch(w))=='~') return can;
        }
      goto undef;
    }
  undef:
  return ERR;
}
#else
I64 GetKey(WINDOW *w) {
  wtimeout(w,100);
  //nonl;
  keypad(w, TRUE);
  nonl;
  noecho;
  raw;
  I64 chr=wgetch(w);
  if(is_termresized) {
    resize_term(0,0);
    goto rz;
  }
  switch(chr) {
    case 27: return ED_KEY_ESCAPE;
    rz: case KEY_RESIZE: return ED_KEY_RESIZE;
    case KEY_UP: return ED_KEY_UP;
    case KEY_DOWN: return ED_KEY_DOWN;
    case KEY_RIGHT: return ED_KEY_RIGHT;
    case KEY_LEFT: return ED_KEY_LEFT;
    case KEY_HOME: return ED_KEY_HOME;
    case KEY_END: return ED_KEY_END;
    case 8: case 127: case KEY_BACKSPACE: return ED_KEY_BACKSPACE;
    case KEY_NPAGE: return ED_KEY_PAGEUP;
    case KEY_PPAGE: return ED_KEY_PAGEDOWN;
    case 13: return '\n';
    case KEY_TAB: return '\t';
    case ALT_A: return ALT_KEY('A');
    case ALT_B: return ALT_KEY('B');
    case ALT_C: return ALT_KEY('C');
    case ALT_D: return ALT_KEY('D');
    case ALT_E: return ALT_KEY('E');
    case ALT_F: return ALT_KEY('F');
    case ALT_G: return ALT_KEY('G');
    case ALT_H: return ALT_KEY('H');
    case ALT_I: return ALT_KEY('I');
    case ALT_J: return ALT_KEY('J');
    case ALT_K: return ALT_KEY('K');
    case ALT_L: return ALT_KEY('L');
    case ALT_M: return ALT_KEY('M');
    case ALT_N: return ALT_KEY('N');
    case ALT_O: return ALT_KEY('O');
    case ALT_P: return ALT_KEY('P');
    case ALT_Q: return ALT_KEY('Q');
    case ALT_R: return ALT_KEY('R');
    case ALT_S: return ALT_KEY('S');
    case ALT_T: return ALT_KEY('T');
    case ALT_U: return ALT_KEY('U');
    case ALT_V: return ALT_KEY('V');
    case ALT_W: return ALT_KEY('W');
    case ALT_X: return ALT_KEY('X');
    case ALT_Y: return ALT_KEY('Y');
    case ALT_Z: return ALT_KEY('Z');
  }
  return chr;
}
#endif
