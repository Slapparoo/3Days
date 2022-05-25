#include "3d.h"
typedef struct CDrawWindow {
    SDL_Window *window;
    SDL_Surface *surf;
    int64_t sz_x,sz_y;
    int64_t scroll_x,scroll_y;
} CDrawWindow;
static int32_t gr_palette_std[]={
0x000000,0x0000AA,0x000AA00,0x00AAAA,
0xAA0000,0xAA00AA,0xAA5500,0xAAAAAA,
0x555555,0x5555FF,0x55FF55,0x55ffff,
0xFF5555,0xFF55FF,0xFFFF55,0xFFFFFF};

CDrawWindow *NewDrawWindow() {
    CDrawWindow *ret=PoopMAlloc(sizeof(CDrawWindow));
    ret->window=SDL_CreateWindow("HolyC Drawer",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,640,480,SDL_WINDOW_RESIZABLE);
    ret->surf=SDL_CreateRGBSurfaceWithFormat(0,640,480,32,SDL_PIXELFORMAT_BGR24);
    SDL_SetWindowMaximumSize(ret->window,640,480);
    //Let 3Days draw the mouse from HolyC
    SDL_ShowCursor(SDL_DISABLE);
    return ret;
}
void DrawWindowUpdate(CDrawWindow *win,int8_t *colors,int64_t internal_width,int64_t h) {
    SDL_Surface *s=win->surf;
    int64_t x,y,c,i;
    SDL_LockSurface(s);
    for(y=0;y!=h;y++) {
        for(x=0;x!=internal_width;x++) {
            i=x+y*internal_width;
            if(16<=colors[i]) {
                //Transparent?
                c=0;
            } else 
                c=colors[i];
            ((char*)s->pixels)[i*3]=gr_palette_std[c]&0xff;
            ((char*)s->pixels)[i*3+1]=(gr_palette_std[c]>>8)&0xff;
            ((char*)s->pixels)[i*3+2]=(gr_palette_std[c]>>16)&0xff;
        }
    }
    SDL_UnlockSurface(s);
    SDL_BlitSurface(s,NULL,SDL_GetWindowSurface(win->window),NULL);
    SDL_UpdateWindowSurface(win->window);
}
void DrawWindowDel(CDrawWindow *win) {
    SDL_FreeSurface(win->surf);
    SDL_DestroyWindow(win->window);
    PoopFree(win);
}
#define CH_CTRLA	0x01
#define CH_CTRLB	0x02
#define CH_CTRLC	0x03
#define CH_CTRLD	0x04
#define CH_CTRLE	0x05
#define CH_CTRLF	0x06
#define CH_CTRLG	0x07
#define CH_CTRLH	0x08
#define CH_CTRLI	0x09
#define CH_CTRLJ	0x0A
#define CH_CTRLK	0x0B
#define CH_CTRLL	0x0C
#define CH_CTRLM	0x0D
#define CH_CTRLN	0x0E
#define CH_CTRLO	0x0F
#define CH_CTRLP	0x10
#define CH_CTRLQ	0x11
#define CH_CTRLR	0x12
#define CH_CTRLS	0x13
#define CH_CTRLT	0x14
#define CH_CTRLU	0x15
#define CH_CTRLV	0x16
#define CH_CTRLW	0x17
#define CH_CTRLX	0x18
#define CH_CTRLY	0x19
#define CH_CTRLZ	0x1A
#define CH_CURSOR	0x05
#define CH_BACKSPACE	0x08
#define CH_ESC		0x1B
#define CH_SHIFT_ESC	0x1C
#define CH_SHIFT_SPACE	0x1F
#define CH_SPACE	0x20


//Scan code flags
#define SCf_E0_PREFIX	7
#define SCf_KEY_UP	8
#define SCf_SHIFT	9
#define SCf_CTRL	10
#define SCf_ALT 	11
#define SCf_CAPS	12
#define SCf_NUM 	13
#define SCf_SCROLL	14
#define SCf_NEW_KEY	15
#define SCf_MS_L_DOWN	16
#define SCf_MS_R_DOWN	17
#define SCf_DELETE	18
#define SCf_INS		19
#define SCf_NO_SHIFT	30
#define SCf_KEY_DESC	31
#define SCF_E0_PREFIX	(1<<SCf_E0_PREFIX)
#define SCF_KEY_UP	(1<<SCf_KEY_UP)
#define SCF_SHIFT	(1<<SCf_SHIFT)
#define SCF_CTRL	(1<<SCf_CTRL)
#define SCF_ALT		(1<<SCf_ALT)
#define SCF_CAPS	(1<<SCf_CAPS)
#define SCF_NUM		(1<<SCf_NUM)
#define SCF_SCROLL	(1<<SCf_SCROLL)
#define SCF_NEW_KEY	(1<<SCf_NEW_KEY)
#define SCF_MS_L_DOWN	(1<<SCf_MS_L_DOWN)
#define SCF_MS_R_DOWN	(1<<SCf_MS_R_DOWN)
#define SCF_DELETE	(1<<SCf_DELETE)
#define SCF_INS 	(1<<SCf_INS)
#define SCF_NO_SHIFT	(1<<SCf_NO_SHIFT)
#define SCF_KEY_DESC	(1<<SCf_KEY_DESC)

//TempleOS places a 1 in bit 7 for
//keys with an E0 prefix.
//See \dLK,"::/Doc/CharOverview.DD"\d and \dLK,"KbdHndlr",A="MN:KbdHndlr"\d().
#define SC_ESC		0x01
#define SC_BACKSPACE	0x0E
#define SC_TAB		0x0F
#define SC_ENTER	0x1C
#define SC_SHIFT	0x2A
#define SC_CTRL		0x1D
#define SC_ALT		0x38
#define SC_CAPS		0x3A
#define SC_NUM		0x45
#define SC_SCROLL	0x46
#define SC_CURSOR_UP	0x48
#define SC_CURSOR_DOWN	0x50
#define SC_CURSOR_LEFT	0x4B
#define SC_CURSOR_RIGHT 0x4D
#define SC_PAGE_UP	0x49
#define SC_PAGE_DOWN	0x51
#define SC_HOME		0x47
#define SC_END		0x4F
#define SC_INS		0x52
#define SC_DELETE	0x53
#define SC_F1		0x3B
#define SC_F2		0x3C
#define SC_F3		0x3D
#define SC_F4		0x3E
#define SC_F5		0x3F
#define SC_F6		0x40
#define SC_F7		0x41
#define SC_F8		0x42
#define SC_F9		0x43
#define SC_F10		0x44
#define SC_F11		0x57
#define SC_F12		0x58
#define SC_PAUSE	0x61
#define SC_GUI		0xDB
#define SC_PRTSCRN1	0xAA
#define SC_PRTSCRN2	0xB7

#define IS_CAPS(x) (x&(KMOD_RSHIFT|KMOD_LSHIFT|KMOD_CAPS))
//http://www.rohitab.com/discuss/topic/39438-keyboard-driver/
static char keys[]={
		0, CH_ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',  'q',
		'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's', 'd',
		'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b',
		'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0,	0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, '5', 0, '+', 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0
	};
static int64_t K2SC(char ch) {
    int64_t i=0;
    for(;i!=sizeof(keys)/sizeof(*keys);i++) {
        if(keys[i]==ch) return i;
    }
}
static int32_t __ScanKey(int64_t *ch,int64_t *sc,SDL_Event *_e) {
    SDL_Event e=*_e;
    int64_t mod=0,cond,dummy;
    if(!ch) ch=&dummy;
    if(!sc) sc=&dummy;    
    cond=1;
    if(cond) {
        if(e.type==SDL_KEYDOWN) {
            ent:
            *ch=*sc=0;
            if(e.key.keysym.mod&(KMOD_LSHIFT|KMOD_RSHIFT))
                mod|=SCF_SHIFT;
            else
                mod|=SCF_NO_SHIFT;
            if(e.key.keysym.mod&(KMOD_LCTRL|KMOD_RCTRL))
                mod|=SCF_CTRL;
            if(e.key.keysym.mod&(KMOD_LALT|KMOD_RALT))
                mod|=SCF_ALT;
            if(e.key.keysym.mod&(KMOD_CAPS))
                mod|=SCF_CAPS;
            if(e.key.keysym.mod&(KMOD_NUM))
                mod|=SCF_NUM;
            if(e.key.keysym.mod&KMOD_LGUI)
                mod|=SCF_MS_L_DOWN;
            if(e.key.keysym.mod&KMOD_RGUI)
                mod|=SCF_MS_R_DOWN;
            *sc=e.key.keysym.scancode;
            switch(e.key.keysym.scancode) {
            case SDL_SCANCODE_SPACE:
            return *sc=K2SC(' ')|mod;
            case SDL_SCANCODE_APOSTROPHE:
            return *sc=K2SC('\'')|mod;
            case SDL_SCANCODE_COMMA:
            return *sc=K2SC(',')|mod;
            case SDL_SCANCODE_MINUS:
            return *sc=K2SC('-')|mod;
            case SDL_SCANCODE_PERIOD:
            return *sc=K2SC('.')|mod;
            case SDL_SCANCODE_GRAVE:
            return *sc=K2SC('`')|mod;
            case SDL_SCANCODE_SLASH:
            return *sc=K2SC('/')|mod;
            case SDL_SCANCODE_0:
            return *sc=K2SC('0')|mod; 
            case SDL_SCANCODE_1:
            return *sc=K2SC('1')|mod; 
            case SDL_SCANCODE_2:
            return *sc=K2SC('2')|mod; 
            case SDL_SCANCODE_3:
            return *sc=K2SC('3')|mod; 
            case SDL_SCANCODE_4:
            return *sc=K2SC('4')|mod; 
            case SDL_SCANCODE_5:
            return *sc=K2SC('5')|mod; 
            case SDL_SCANCODE_6:
            return *sc=K2SC('6')|mod; 
            case SDL_SCANCODE_7:
            return *sc=K2SC('7')|mod; 
            case SDL_SCANCODE_8:
            return *sc=K2SC('8')|mod; 
            case SDL_SCANCODE_9:
            return *sc=K2SC('9')|mod; 
            case SDL_SCANCODE_SEMICOLON:
            return *sc=K2SC(';')|mod; 
            case SDL_SCANCODE_EQUALS:
            return *sc=K2SC('=')|mod; 
            case SDL_SCANCODE_LEFTBRACKET:
            return *sc=K2SC('[')|mod; 
            case SDL_SCANCODE_RIGHTBRACKET:
            return *sc=K2SC(']')|mod; 
            case SDL_SCANCODE_BACKSLASH:
            return *sc=K2SC('\\')|mod; 
            case SDL_SCANCODE_Q:
            return *sc=K2SC('q')|mod; 
            case SDL_SCANCODE_W:
            return *sc=K2SC('w')|mod;
            case SDL_SCANCODE_E:
            return *sc=K2SC('e')|mod; 
            case SDL_SCANCODE_R:
            return *sc=K2SC('r')|mod;
            case SDL_SCANCODE_T:
            return *sc=K2SC('t')|mod;
            case SDL_SCANCODE_Y:
            return *sc=K2SC('y')|mod;
            case SDL_SCANCODE_U:
            return *sc=K2SC('u')|mod;
            case SDL_SCANCODE_I:
            return *sc=K2SC('i')|mod;
            case SDL_SCANCODE_O:
            return *sc=K2SC('o')|mod;
            case SDL_SCANCODE_P:
            return *sc=K2SC('p')|mod;
            case SDL_SCANCODE_A:
            return *sc=K2SC('a')|mod;
            case SDL_SCANCODE_S:
            return *sc=K2SC('s')|mod;
            case SDL_SCANCODE_D:
            return *sc=K2SC('d')|mod;
            case SDL_SCANCODE_F:
            return *sc=K2SC('f')|mod;
            case SDL_SCANCODE_G:
            return *sc=K2SC('g')|mod;
            case SDL_SCANCODE_H:
            return *sc=K2SC('h')|mod;
            case SDL_SCANCODE_J:
            return *sc=K2SC('j')|mod;
            case SDL_SCANCODE_K:
            return *sc=K2SC('k')|mod;
            case SDL_SCANCODE_L:
            return *sc=K2SC('l')|mod;
            case SDL_SCANCODE_Z:
            return *sc=K2SC('z')|mod;
            case SDL_SCANCODE_X:
            return *sc=K2SC('x')|mod;
            case SDL_SCANCODE_C:
            return *sc=K2SC('c')|mod;
            case SDL_SCANCODE_V:
            return *sc=K2SC('v')|mod;
            case SDL_SCANCODE_B:
            return *sc=K2SC('b')|mod;
            case SDL_SCANCODE_N:
            return *sc=K2SC('n')|mod;
            case SDL_SCANCODE_M:
            return *sc=K2SC('m')|mod;
            case SDL_SCANCODE_ESCAPE:
            *sc=mod|SC_ESC;
            return 1;
            case SDL_SCANCODE_BACKSPACE:
            *sc=mod|SC_BACKSPACE;
            return 1;
            case SDL_SCANCODE_TAB:
            *sc=mod|SC_TAB;
            return 1;
            case SDL_SCANCODE_RETURN:
            *sc=mod|SC_ENTER;
            return 1;
            case SDL_SCANCODE_LSHIFT:
            case SDL_SCANCODE_RSHIFT:
            *sc=mod|SC_SHIFT;
            return 1;
            case SDL_SCANCODE_LALT:
            *sc=mod|SC_ALT;
            return 1;
            case SDL_SCANCODE_RALT:
            *sc=mod|SC_ALT;
            return 1;
            case SDL_SCANCODE_LCTRL:
            *sc=mod|SC_CTRL;
            return 1;
            case SDL_SCANCODE_RCTRL:
            *sc=mod|SC_CTRL;
            return 1;
            case SDL_SCANCODE_CAPSLOCK:
            *sc=mod|SC_CAPS;
            return 1;
            case SDL_SCANCODE_NUMLOCKCLEAR:
            *sc=mod|SC_NUM;
            return 1;
            case SDL_SCANCODE_SCROLLLOCK:
            *sc=mod|SC_SCROLL;
            return 1;
            case SDL_SCANCODE_DOWN:
            *sc=mod|SC_CURSOR_DOWN;
            return 1;
            case SDL_SCANCODE_UP:
            *sc=mod|SC_CURSOR_UP;
            return 1;
            case SDL_SCANCODE_RIGHT:
            *sc=mod|SC_CURSOR_RIGHT;
            return 1;
            case SDL_SCANCODE_LEFT:
            *sc=mod|SC_CURSOR_LEFT;
            return 1;
            case SDL_SCANCODE_PAGEDOWN:
            *sc=mod|SC_PAGE_DOWN;
            return 1;
            case SDL_SCANCODE_PAGEUP:
            *sc=mod|SC_PAGE_UP;
            return 1;
            case SDL_SCANCODE_HOME:
            *sc=mod|SC_HOME;
            return 1;
            case SDL_SCANCODE_END:
            *sc=mod|SC_END;
            return 1;
            case SDL_SCANCODE_INSERT:
            *sc=mod|SC_INS;
            return 1;
            case SDL_SCANCODE_DELETE:
            *sc=mod|SC_DELETE;
            return 1;
            case SC_GUI:
            *sc=mod|SDL_SCANCODE_APPLICATION;
            return 1;
            case SDL_SCANCODE_PRINTSCREEN:
            *sc=mod|SC_PRTSCRN1;
            return 1;
            case SDL_SCANCODE_PAUSE:
            *sc=mod|SC_PAUSE;
            return 1;
            case SDL_SCANCODE_F1...SDL_SCANCODE_F12:
            *sc=mod|(SC_F1+e.key.keysym.scancode-SDL_SCANCODE_F1);
            return 1;
            }
        } else if(e.type==SDL_KEYUP) {
            mod|=SCF_KEY_UP;
            goto ent;
        }
    }
    return -1;
}
static int32_t TimerCb(int32_t interval,void *data) {
    FFI_CALL_TOS_1(data,interval);
    return interval;
}
void __AddTimer(int64_t interval,void (*tos_fptr)()) {
    SDL_AddTimer(interval,&TimerCb,tos_fptr);
}
static void (*kb_cb)();
static void *kb_cb_data;
static int SDLCALL KBCallback(void *d,SDL_Event *e) {
    int64_t c,s;
    if(kb_cb&&(-1!=__ScanKey(&c,&s,e)))
        FFI_CALL_TOS_2(kb_cb,c,s);
    return 0;
}
void SetKBCallback(void *fptr,void *data) {
    kb_cb=fptr;
    kb_cb_data=data;
    static init;
    if(!init)
        init=1,SDL_AddEventWatch(KBCallback,data);
}
//x,y,z,(l<<1)|r
static void(*ms_cb)();
static int SDLCALL MSCallback(void *d,SDL_Event *e) {
    int64_t x,y;
    static int state;
    static int z;
    if(ms_cb)
        switch(e->type) {
            case SDL_MOUSEBUTTONDOWN:
            x=e->button.x,y=e->button.y;
            if(e->button.button==SDL_BUTTON_LEFT)
                state|=2;
            else
                state|=1;
            goto ent;
            case SDL_MOUSEBUTTONUP:
            x=e->button.x,y=e->button.y;
            if(e->button.button==SDL_BUTTON_LEFT)
                state&=~2;
            else
                state&=~1;
            goto ent;
            case SDL_MOUSEWHEEL:
            z+=e->wheel.y;
            goto ent;
            case SDL_MOUSEMOTION:
            x=e->motion.x,y=e->motion.y;
            ent:
            FFI_CALL_TOS_4(ms_cb,x,y,z,state);
        }
    return 0;
}

void SetMSCallback(void *fptr) {
    ms_cb=fptr;
    static init;
    if(!init)
        init=1,SDL_AddEventWatch(MSCallback,NULL);
}
void ScanSDLEvent() {
    SDL_Event e;
    if(SDL_PollEvent(&e)) {
        if(e.type==SDL_QUIT) {
            SDL_Quit();
            exit(0);
        }
    }
}
