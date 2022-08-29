#include "3d.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/keysymdef.h>
typedef struct CDrawWindow {
    Window window;
    Display *disp;
    GC gc;
    Atom clip3days;
    Cursor empty_cursor;
    int64_t sz_x,sz_y;
} CDrawWindow;
static int32_t gr_palette_std[]={
0x000000,0x0000AA,0x000AA00,0x00AAAA,
0xAA0000,0xAA00AA,0xAA5500,0xAAAAAA,
0x555555,0x5555FF,0x55FF55,0x55ffff,
0xFF5555,0xFF55FF,0xFFFF55,0xFFFFFF};
static uint32_t palette[256];
static void StartInputScanner();
static CDrawWindow *dw=NULL;
static char *clip_text=NULL;
static void
utf8_prop(Display *dpy, XEvent ev);
CDrawWindow *NewDrawWindow() {
	if(!dw) {
		XInitThreads();
		dw=TD_MALLOC(sizeof(*dw));
		dw->disp=XOpenDisplay(NULL);
		long screen=DefaultScreen(dw->disp);
		Colormap cmap;
		XColor c;
		Pixmap cpm;
		Cursor cursor;
		int64_t i,black,white;
		for(i=0;i!=16;i++) {
			palette[i]=gr_palette_std[i];
		}
		for(i=16;i!=256;i++) {
			palette[i]=0;
		}
		black=BlackPixel(dw->disp,screen);
		dw->window=XCreateSimpleWindow(
			dw->disp,
			RootWindow(dw->disp,screen),
			0,0,640,480,0,1,black
		);
		XSetStandardProperties(dw->disp,dw->window,"3Days",NULL,None,NULL,0,NULL);
		XSelectInput(dw->disp,dw->window,
			KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask|PointerMotionMask|ButtonMotionMask|Button3MotionMask|Button4MotionMask|Button5MotionMask|FocusChangeMask|StructureNotifyMask);
		dw->gc=XCreateGC(dw->disp,dw->window,0,0);
		//Make a empty cursor
		char data[]={0};
		XColor colors[]={0};
		cpm=XCreateBitmapFromData(dw->disp,dw->window,data,1,1);
		dw->empty_cursor=XCreatePixmapCursor(dw->disp,cpm,cpm,colors,colors,0,0);
		XFreePixmap(dw->disp,cpm);
		XDefineCursor(dw->disp,dw->window,dw->empty_cursor);
		//
		dw->sz_x=640,dw->sz_y=480;
		white=WhitePixel(dw->disp,screen);
		XSetBackground(dw->disp,dw->gc,white);
		XSetForeground(dw->disp,dw->gc,black);
		XMapWindow(dw->disp,dw->window);
		XEvent e;
		while(XNextEvent(dw->disp,&e))
			if(e.type==Expose)
				break;
	}
	return dw;
}
static void CenterImage(CDrawWindow *win,int64_t *x_off,int64_t *y_off) {
	if(win->sz_x>640) {
		*x_off=(win->sz_x-640)/2;
	} else 
		*x_off=0;
	if(win->sz_y>480) {
		*y_off=(win->sz_y-480)/2;
	} else 
		*y_off=0;
}
char buf[640*480*4];
void DrawWindowUpdate(CDrawWindow *win,int8_t *_colors,int64_t internal_width,int64_t h) {
	uint8_t *colors=_colors;
	int64_t x,y,cx,cy,b,black,white,wx,wy,b2;
	XLockDisplay(dw->disp);
	long screen=DefaultScreen(dw->disp);
	Visual *vis=XDefaultVisual(dw->disp,screen);
	int dplanes=DisplayPlanes(dw->disp,screen);
	wx=win->sz_x>480?win->sz_x:480;
	wy=win->sz_y>640?win->sz_y:640;
	static int64_t old_wx,old_wy;
	static uint32_t *old_scrn;
	uint32_t *scrn;
	if(old_wx!=wx||old_wy!=wy) {
		if(old_scrn) free(old_scrn);
		old_scrn=calloc(4,wx*wy);
		old_wx=wx;
		old_wy=wy;
	}
	scrn=old_scrn;
	XImage *img=XCreateImage(dw->disp,vis,dplanes,ZPixmap,0,scrn,wx,wy,8,0);
	CenterImage(dw,&cx,&cy);
	for(y=cy;y!=cy+h;y++)
		for(x=cx;x!=cx+internal_width;x++) {
			b2=x+y*wx;
			((uint32_t*)img->data)[b2]=palette[*colors++];
		}
	XPutImage(dw->disp,dw->window,dw->gc,img,0,0,0,0,wx,wy);
	XFlush(dw->disp);
	XFree(img);
	XUnlockDisplay(dw->disp);
}
void DrawWindowDel(CDrawWindow *win) {
	XLockDisplay(dw->disp);
	XFreeGC(dw->disp,win->gc);
	XDestroyWindow(dw->disp,win->window);
	XCloseDisplay(dw->disp);
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
static int64_t persist_mod=0;
static int32_t __ScanKey(int64_t *ch,int64_t *sc,XEvent *_e) {
    XEvent e=*_e;
    #define PERSIST_KEY(flag) \ 
    if(e.type==KeyRelease) { \
		persist_mod&=~(flag); \
		mod&=~(flag); \
    } else \
		mod|=(persist_mod|=(flag));
    int64_t mod=persist_mod,cond,dummy;
    static int64_t prev_key_press=-1;
    if(!ch) ch=&dummy;
    if(!sc) sc=&dummy;    
    cond=1;
    if(cond) {
		if(e.type==KeyPress) {
			if(prev_key_press==XLookupKeysym(&e,0))
				return -1;
			prev_key_press=XLookupKeysym(&e,0);
            ent:
            *ch=*sc=0;
            if(e.xkey.state&ShiftMask)
                mod|=SCF_SHIFT;
            else
                mod|=SCF_NO_SHIFT;
            if(e.xkey.state&(ControlMask))
                mod|=SCF_CTRL;
            /*if(e.key.keysym.mod&(KMOD_CAPS))
                mod|=SCF_CAPS;
            if(e.key.keysym.mod&(KMOD_NUM))
                mod|=SCF_NUM;*/
            /*if(e.key.keysym.mod&KMOD_LGUI)
                mod|=SCF_MS_L_DOWN;
            if(e.key.keysym.mod&KMOD_RGUI)
                mod|=SCF_MS_R_DOWN;*/
            *sc=XLookupKeysym(&e,0);
            switch(*sc) {
            case XK_space:
            return *sc=K2SC(' ')|mod;
            case XK_apostrophe:
            return *sc=K2SC('\'')|mod;
            case XK_comma:
            return *sc=K2SC(',')|mod;
            case XK_minus:
            return *sc=K2SC('-')|mod;
            case XK_period:
            return *sc=K2SC('.')|mod;
            case XK_grave:
            return *sc=K2SC('`')|mod;
            case XK_slash:
            return *sc=K2SC('/')|mod;
            case XK_0:
            return *sc=K2SC('0')|mod; 
            case XK_1:
            return *sc=K2SC('1')|mod; 
            case XK_2:
            return *sc=K2SC('2')|mod; 
            case XK_3:
            return *sc=K2SC('3')|mod; 
            case XK_4:
            return *sc=K2SC('4')|mod; 
            case XK_5:
            return *sc=K2SC('5')|mod; 
            case XK_6:
            return *sc=K2SC('6')|mod; 
            case XK_7:
            return *sc=K2SC('7')|mod; 
            case XK_8:
            return *sc=K2SC('8')|mod; 
            case XK_9:
            return *sc=K2SC('9')|mod; 
            case XK_semicolon:
            return *sc=K2SC(';')|mod; 
            case XK_equal:
            return *sc=K2SC('=')|mod; 
            case XK_bracketleft:
            return *sc=K2SC('[')|mod; 
            case XK_bracketright:
            return *sc=K2SC(']')|mod; 
            case XK_backslash:
            return *sc=K2SC('\\')|mod; 
            case XK_q:
            return *sc=K2SC('q')|mod; 
            case XK_w:
            return *sc=K2SC('w')|mod;
            case XK_e:
            return *sc=K2SC('e')|mod; 
            case XK_r:
            return *sc=K2SC('r')|mod;
            case XK_t:
            return *sc=K2SC('t')|mod;
            case XK_y:
            return *sc=K2SC('y')|mod;
            case XK_u:
            return *sc=K2SC('u')|mod;
            case XK_i:
            return *sc=K2SC('i')|mod;
            case XK_o:
            return *sc=K2SC('o')|mod;
            case XK_p:
            return *sc=K2SC('p')|mod;
            case XK_a:
            return *sc=K2SC('a')|mod;
            case XK_s:
            return *sc=K2SC('s')|mod;
            case XK_d:
            return *sc=K2SC('d')|mod;
            case XK_f:
            return *sc=K2SC('f')|mod;
            case XK_g:
            return *sc=K2SC('g')|mod;
            case XK_h:
            return *sc=K2SC('h')|mod;
            case XK_j:
            return *sc=K2SC('j')|mod;
            case XK_k:
            return *sc=K2SC('k')|mod;
            case XK_l:
            return *sc=K2SC('l')|mod;
            case XK_z:
            return *sc=K2SC('z')|mod;
            case XK_x:
            return *sc=K2SC('x')|mod;
            case XK_c:
            return *sc=K2SC('c')|mod;
            case XK_v:
            return *sc=K2SC('v')|mod;
            case XK_b:
            return *sc=K2SC('b')|mod;
            case XK_n:
            return *sc=K2SC('n')|mod;
            case XK_m:
            return *sc=K2SC('m')|mod;
            case XK_Escape:
            *sc=mod|SC_ESC;
            return 1;
            case XK_BackSpace:
            *sc=mod|SC_BACKSPACE;
            return 1;
            case XK_Tab:
            *sc=mod|SC_TAB;
            return 1;
            case XK_Return:
            *sc=mod|SC_ENTER;
            return 1;
            case XK_Shift_L:
            case XK_Shift_R:
            PERSIST_KEY(SCF_SHIFT);
            *sc=mod|SC_SHIFT;
            return 1;
            case XK_Alt_L:
            case XK_Alt_R:
            PERSIST_KEY(SCF_ALT);
            *sc=mod|SC_ALT;
            return 1;
            case XK_Control_L:
            case XK_Control_R:
            PERSIST_KEY(SCF_CTRL);
            *sc=mod|SC_CTRL;
            return 1;
            case XK_Caps_Lock:
            PERSIST_KEY(SCF_CAPS);
            *sc=mod|SC_CAPS;
            return 1;
            /*
            case SDL_SCANCODE_NUMLOCKCLEAR:
            *sc=mod|SC_NUM;
            return 1;
            */
            case XK_Scroll_Lock:
            *sc=mod|SC_SCROLL;
            return 1;
            case XK_Down:
            *sc=mod|SC_CURSOR_DOWN;
            return 1;
            case XK_Up:
            *sc=mod|SC_CURSOR_UP;
            return 1;
            case XK_Right:
            *sc=mod|SC_CURSOR_RIGHT;
            return 1;
            case XK_Left:
            *sc=mod|SC_CURSOR_LEFT;
            return 1;
            case XK_Page_Down:
            *sc=mod|SC_PAGE_DOWN;
            return 1;
            case XK_Page_Up:
            *sc=mod|SC_PAGE_UP;
            return 1;
            case XK_Home:
            *sc=mod|SC_HOME;
            return 1;
            case XK_End:
            *sc=mod|SC_END;
            return 1;
            case XK_Insert:
            *sc=mod|SC_INS;
            return 1;
            case XK_Delete:
            *sc=mod|SC_DELETE;
            return 1;
            case XK_Super_L:
            case XK_Super_R:
            *sc=mod|SC_GUI;
            return 1;
            case XK_Pause:
            *sc=mod|SC_PAUSE;
            return 1;
            case XK_F1...XK_F12:
            *sc=mod|(SC_F1+*sc-XK_F1);
            return 1;
            }
        } else if(e.type==KeyRelease) {
			prev_key_press=0;
            mod|=SCF_KEY_UP;
            goto ent;
        }
    }
    return -1;
}
static void (*kb_cb)();
static void *kb_cb_data;
static int KBCallback(void *d,XEvent *e) {
    int64_t c,s;
    if(kb_cb&&(-1!=__ScanKey(&c,&s,e)))
        FFI_CALL_TOS_2(kb_cb,c,s);
    return 0;
}
void SetKBCallback(void *fptr,void *data) {
    kb_cb=fptr;
    kb_cb_data=data;
}
//x,y,z,(l<<1)|r
static void(*ms_cb)();
static int MSCallback(void *d,XEvent *e) {
    static int64_t x,y,cx,cy;
    static int state;
    static int z;
    if(ms_cb)
        switch(e->type) {
            case ButtonPress:
            x=e->xbutton.x,y=e->xbutton.y;
            if(e->xbutton.button==1)
                state|=2;
            else if(e->xbutton.button==4)
				z--;
			else if(e->xbutton.button==5)
				z++;
			else if(e->xbutton.button==3)
                state|=1;
            goto ent;
            case ButtonRelease:
            x=e->xbutton.x,y=e->xbutton.y;
            if(e->xbutton.button==1)
                state&=~2;
            else if(e->xbutton.button==3)
                state&=~1;
            goto ent;
            case MotionNotify:
            x=e->xmotion.x,y=e->xmotion.y;
            ent:
            CenterImage(dw,&cx,&cy);
            cx=x-cx;
            cy=y-cy;
            if(cx<0) cx=0;
            if(cy<0) cy=0;
            if(cx>640) cx=640;
            if(cy>480) cy=480;
            FFI_CALL_TOS_4(ms_cb,cx,cy,z,state);
        }
    return 0;
}

void SetMSCallback(void *fptr) {
    ms_cb=fptr;
}
static void utf8_send(char *text,XEvent ev) {
	Display *dpy=dw->disp;
    XSelectionEvent ssev;
	XLockDisplay(dpy);
	Atom utf8 = XInternAtom(dpy, "UTF8_STRING", False);
	Atom targets=XInternAtom(dw->disp, "TARGETS", False);
    Atom atom=XInternAtom(dw->disp, "ATOM", False);
    XSelectionRequestEvent *sev;
	loop:;
	sev=&ev.xselectionrequest;
	if(sev->target==targets) {
		XChangeProperty(
			dpy,
			sev->requestor,
			sev->property,
			atom,
			32, //expects 8/16/32 bits
			PropModeReplace,
			&utf8,
			1
		);
		goto set;
	} else if(sev->target!=utf8||sev->property==None) {
		//Oh no
		ssev.type=SelectionNotify;
		ssev.requestor=sev->requestor;
		ssev.selection=sev->selection;
		ssev.target=sev->target;
		ssev.property=None;
		ssev.time=CurrentTime;
		XSendEvent(dpy, sev->requestor, True, NoEventMask, (XEvent *)&ssev);
		goto end;
	}
	if(!text)
		text="";
    XChangeProperty(dpy, sev->requestor, sev->property, utf8, 8, PropModeReplace,
                    (unsigned char *)text, strlen(text));
     set:
    ssev.type = SelectionNotify;
    ssev.requestor = sev->requestor;
    ssev.selection = sev->selection;
    ssev.target = sev->target;
    ssev.property = sev->property;
    ssev.time = sev->time;
    XSendEvent(dpy, sev->requestor, True, NoEventMask, (XEvent *)&ssev);
    end:
    XUnlockDisplay(dpy);
}
void SetClipboard(char *text) {
	TD_FREE(clip_text);
	XLockDisplay(dw->disp);
	Atom sel = XInternAtom(dw->disp, "CLIPBOARD", False);
    XSetSelectionOwner(dw->disp, sel, dw->window, CurrentTime);
	XUnlockDisplay(dw->disp);
	clip_text=strdup(text);
}
static void __InputLoop(void *ul,int64_t clip_only) {
    XEvent e;
    XSelectionEvent *sev;
    for(;!*(int64_t*)ul;) {
		XNextEvent(dw->disp,&e);
		if(e.type==DestroyNotify){
			DrawWindowDel(dw);
			break;
		} else if(e.type==ConfigureNotify&&dw) {
			dw->sz_x=e.xconfigure.width;
			dw->sz_y=e.xconfigure.height;
		} else if(e.type==SelectionRequest) {
			utf8_send(clip_text,e);
		} else if(e.type==SelectionNotify)
			utf8_prop(dw->disp,e);
		else if(e.type==FocusOut) {
			if((persist_mod|SCF_ALT)&&kb_cb)
				FFI_CALL_TOS_2(kb_cb,0,SC_ALT|SCF_ALT|SCF_KEY_UP|SCF_NO_SHIFT);
			if((persist_mod|SCF_CTRL)&&kb_cb)
				FFI_CALL_TOS_2(kb_cb,0,SC_CTRL|SCF_CTRL|SCF_KEY_UP|SCF_NO_SHIFT);
			if((persist_mod|SCF_SHIFT)&&kb_cb)
				FFI_CALL_TOS_2(kb_cb,0,SC_SHIFT|SCF_SHIFT|SCF_KEY_UP);
			if((persist_mod|SCF_CAPS)&&kb_cb)
				FFI_CALL_TOS_2(kb_cb,0,SC_CAPS|SCF_NO_SHIFT|SCF_KEY_UP);
			persist_mod&=~(SCF_CAPS|SCF_SHIFT|SCF_CTRL|SCF_ALT);
			continue;
		}
		if(kb_cb)
			KBCallback(kb_cb_data,&e);
		if(ms_cb)
			MSCallback(NULL,&e);
	}
}
char *ClipboardText() {
	XLockDisplay(dw->disp);
	char *ret;
	Atom sel = XInternAtom(dw->disp, "CLIPBOARD", False);
	Window owner = XGetSelectionOwner(dw->disp, sel);
	XEvent ev;
	if(owner==dw->window) {
		ret=strdup(clip_text);
		XUnlockDisplay(dw->disp);
		return ret;
	}
	Atom utf8=XInternAtom(dw->disp, "UTF8_STRING", False);
	Atom target=XInternAtom(dw->disp, "3DaysCLIP", False);
	XConvertSelection(dw->disp, sel, utf8, target, dw->window,
						  CurrentTime);
	TD_FREE(clip_text);
	clip_text=NULL;
	for(;;) {
		XNextEvent(dw->disp,&ev);
		if(ev.type==SelectionNotify) {
			utf8_prop(dw->disp,ev);
			break;
		}
	}
	XUnlockDisplay(dw->disp);
	if(!clip_text)
		clip_text=strdup("");
	return strdup(clip_text);
}
void InputLoop(void *ul) {
	__InputLoop(ul,0);
}
//https://www.google.com/search?q=xlib+clipboard&client=firefox-b-1-e&sxsrf=ALiCzsb9A6kPI8rhFAVztJKwMeVH6WiTJQ%3A1655908750653&ei=jimzYuS0J_6fptQP6pStoAs&ved=0ahUKEwjks_PmpMH4AhX-j4kEHWpKC7QQ4dUDCA0&uact=5&oq=xlib+clipboard&gs_lcp=Cgdnd3Mtd2l6EAMyBggAEB4QFjIGCAAQHhAWMgUIABCGAzIFCAAQhgMyBQgAEIYDMgUIABCGAzoHCAAQRxCwAzoHCAAQsAMQQzoKCAAQ5AIQsAMYAToSCC4QxwEQowIQyAMQsAMQQxgCOgwILhDIAxCwAxBDGAI6BAgjECc6BAgAEEM6CwgAEIAEELEDEIMBOgUIABCABDoICAAQgAQQsQM6CwguEIAEEMcBENEDOgoIABCABBCHAhAUSgQIQRgASgQIRhgBULkCWKMXYIkaaAFwAXgAgAGYA4gB1B2SAQcyLTQuNy4xmAEAoAEByAERwAEB2gEGCAEQARgJ2gEGCAIQARgI&sclient=gws-wiz
static void
utf8_prop(Display *dpy, XEvent ev)
{
	Window w=dw->window;
	XLockDisplay(dpy);
	Atom sel = XInternAtom(dw->disp, "CLIPBOARD", False);
	Atom utf8 = XInternAtom(dw->disp, "UTF8_STRING", False);
	Atom target=XInternAtom(dw->disp, "3DaysCLIP", False);
	Atom da, incr, type;
    int di;
    unsigned long size, dul;
	unsigned char *prop_ret = NULL;
	XGetWindowProperty(dpy, w, target, 0, 0, False, AnyPropertyType,
                       &type, &di, &dul, &size, &prop_ret);
    XFree(prop_ret);
    XGetWindowProperty(dpy, w, target, 0, size, False, AnyPropertyType,
                       &da, &di, &dul, &dul, &prop_ret);
    TD_FREE(clip_text);
    if(!size)
		clip_text=strdup("");
	else
		clip_text=strdup(prop_ret);
    XFree(prop_ret);
    XDeleteProperty(dpy, w, target);
    XUnlockDisplay(dpy);
}
