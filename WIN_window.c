#include "3d.h"
#include <windows.h>
#include <winuser.h>
#include <wingdi.h>
#include <shellapi.h>
#include <windowsx.h>
#include <winbase.h>
#include <shellscalingapi.h>
HANDLE *mutex;
LPSTR* WINAPI CommandLineToArgvA_wine(LPSTR lpCmdline, int* numargs);
typedef struct CDrawWindow {
    int64_t sz_x,sz_y;
    int64_t scroll_x,scroll_y;
    HWND win;
} CDrawWindow;
static void CenterWindow(HWND win,int64_t *x,int64_t *y) {
	RECT rct;
	GetClientRect(win,&rct);
	int64_t _x,_y;
	_x=(rct.right-640)/2,_y=(rct.bottom-480)/2;
	if(_y<0) _y=0;
	if(_x<0) _x=0;
	if(x) *x=_x;
	if(y) *y=_y;
}
static char gr_pallete_BGR48[2*4*16]; //2 bytes,4 channels,16 colors
static int32_t gr_palette_std[]={
0x000000,0x0000AA,0x000AA00,0x00AAAA,
0xAA0000,0xAA00AA,0xAA5500,0xAAAAAA,
0x555555,0x5555FF,0x55FF55,0x55ffff,
0xFF5555,0xFF55FF,0xFFFF55,0xFFFFFF};
static int32_t palette[256];
static void StartInputScanner();
static CDrawWindow *dw=NULL;
static char *clip_text=NULL;
CDrawWindow *NewDrawWindow() {
	if(!dw) {
		dw=TD_MALLOC(sizeof(*dw));
		int64_t i;
		for(i=0;i!=16;i++) {
			palette[i]=gr_palette_std[i];
		}
	}
	return dw;
}
int32_t buf[640*480];
void DrawWindowDel(CDrawWindow *win) {
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
static int32_t __ScanKey(int64_t *ch,int64_t *sc,UINT msg,WPARAM w,LPARAM *_e) {
    LPARAM e=*_e;
    #define PERSIST_KEY(flag) \ 
    if(msg==WM_KEYUP||msg==WM_SYSKEYUP) { \
		persist_mod&=~(flag); \
		mod&=~(flag); \
    } else \
		mod|=(persist_mod|=(flag));
    int64_t mod=persist_mod,cond,dummy;
    if(!ch) ch=&dummy;
    if(!sc) sc=&dummy;    
    cond=1;
    if(cond) {
		if(msg==WM_KEYDOWN||msg==WM_SYSKEYDOWN) {
            ent:
            *ch=*sc=0;
            if(persist_mod&SCF_SHIFT)
                mod|=SCF_SHIFT;
            else
                mod|=SCF_NO_SHIFT;
            if(persist_mod&SCF_ALT)
                mod|=SCF_ALT;
            if(persist_mod&SCF_CTRL)
                mod|=SCF_CTRL;
            /*if(e.key.keysym.mod&(KMOD_CAPS))
                mod|=SCF_CAPS;
            if(e.key.keysym.mod&(KMOD_NUM))
                mod|=SCF_NUM;*/
            /*if(e.key.keysym.mod&KMOD_LGUI)
                mod|=SCF_MS_L_DOWN;
            if(e.key.keysym.mod&KMOD_RGUI)
                mod|=SCF_MS_R_DOWN;*/
            *sc=w;
            switch(*sc) {
            case VK_SPACE:
            return *sc=K2SC(' ')|mod;
            case VK_OEM_7:
            return *sc=K2SC('\'')|mod;
            case VK_OEM_COMMA:
            return *sc=K2SC(',')|mod;
            case VK_OEM_MINUS:
            return *sc=K2SC('-')|mod;
            case VK_OEM_PERIOD:
            return *sc=K2SC('.')|mod;
            case VK_OEM_3:
            return *sc=K2SC('`')|mod;
            case VK_OEM_2:
            return *sc=K2SC('/')|mod;
            case 0x30 ... 0x3A:
            return *sc=K2SC('0'+*sc-0x30)|mod; 
            case VK_OEM_1:
            return *sc=K2SC(';')|mod; 
            case VK_OEM_PLUS:
            return *sc=K2SC('=')|mod; 
            case VK_OEM_4:
            return *sc=K2SC('[')|mod; 
            case VK_OEM_6:
            return *sc=K2SC(']')|mod; 
            case VK_OEM_5:
            return *sc=K2SC('\\')|mod; 
            case 0x41 ... 0x5A:
            return *sc=K2SC(*sc-0x41+'a')|mod; 
            case VK_ESCAPE:
            *sc=mod|SC_ESC;
            return 1;
            case VK_BACK:
            *sc=mod|SC_BACKSPACE;
            return 1;
            case VK_TAB:
            *sc=mod|SC_TAB;
            return 1;
            case VK_RETURN:
            *sc=mod|SC_ENTER;
            return 1;
            case VK_SHIFT:
            PERSIST_KEY(SCF_SHIFT);
            *sc=mod|SC_SHIFT;
            return 1;
			case VK_MENU:
            PERSIST_KEY(SCF_ALT);
            *sc=mod|SC_ALT;
            return 1;
            case VK_CONTROL:
            PERSIST_KEY(SCF_CTRL);
            *sc=mod|SC_CTRL;
            return 1;
            case VK_CAPITAL:
            PERSIST_KEY(SCF_CAPS);
            *sc=mod|SC_CAPS;
            return 1;
            /*
            case SDL_SCANCODE_NUMLOCKCLEAR:
            *sc=mod|SC_NUM;
            return 1;
            */
            case VK_SCROLL:
            *sc=mod|SC_SCROLL;
            return 1;
            case VK_DOWN:
            *sc=mod|SC_CURSOR_DOWN;
            return 1;
            case VK_UP:
            *sc=mod|SC_CURSOR_UP;
            return 1;
            case VK_RIGHT:
            *sc=mod|SC_CURSOR_RIGHT;
            return 1;
            case VK_LEFT:
            *sc=mod|SC_CURSOR_LEFT;
            return 1;
            case VK_NEXT:
            *sc=mod|SC_PAGE_DOWN;
            return 1;
            case VK_PRIOR:
            *sc=mod|SC_PAGE_UP;
            return 1;
            case VK_HOME:
            *sc=mod|SC_HOME;
            return 1;
            case VK_END:
            *sc=mod|SC_END;
            return 1;
            case VK_INSERT:
            *sc=mod|SC_INS;
            return 1;
            case VK_DELETE:
            *sc=mod|SC_DELETE;
            return 1;
            case VK_LWIN:
            case VK_RWIN:
            *sc=mod|SC_GUI;
            return 1;
            case VK_PAUSE:
            *sc=mod|SC_PAUSE;
            return 1;
            case VK_F1...VK_F12:
            *sc=mod|(SC_F1+*sc-VK_F1);
            return 1;
            }
        } else if(msg==WM_KEYUP||msg==WM_SYSKEYUP) {
            mod|=SCF_KEY_UP;
            goto ent;
        }
    }
    return -1;
}
static void (*kb_cb)();
static void *kb_cb_data;
static int KBCallback(void *data,UINT msg,WPARAM w,LPARAM *e) {
    int64_t c,s;
    if(kb_cb&&(-1!=__ScanKey(&c,&s,msg,w,e)))
        FFI_CALL_TOS_2(kb_cb,c,s);
    return 0;
}
void SetKBCallback(void *fptr,void *data) {
    kb_cb=fptr;
    kb_cb_data=data;
}
//x,y,z,(l<<1)|r
static void(*ms_cb)();
static int MSCallback(HWND hwnd,void *d,UINT msg,WPARAM w,LPARAM e) {
    static int64_t x,y,cx,cy;
    CenterWindow(hwnd,&cx,&cy);
    static int state;
    static int z;
    RECT rect;
    if(ms_cb) {
		switch(msg) {
			case WM_MOUSEWHEEL:
			z+=GET_WHEEL_DELTA_WPARAM(w);
			goto ent;
            case WM_LBUTTONDOWN:
            x=GET_X_LPARAM(e),y=GET_Y_LPARAM(e);
            state|=2;
            goto ent;
            case WM_RBUTTONDOWN:
            x=GET_X_LPARAM(e),y=GET_Y_LPARAM(e);
            state|=1;
            goto ent;
            case WM_RBUTTONUP:
            x=GET_X_LPARAM(e),y=GET_Y_LPARAM(e);
            state&=~1;
            goto ent;
            case WM_LBUTTONUP:
            x=GET_X_LPARAM(e),y=GET_Y_LPARAM(e);
            state&=~2;
            goto ent;
            case WM_MOUSEMOVE:
            x=GET_X_LPARAM(e),y=GET_Y_LPARAM(e);
            ent:;
            int x2=x-cx,y2=y-cy;
            if(x2>=640) x2=640;
            if(y2>=480) y2=480;
            if(y2<0) y2=0;
            if(x2<0) x2=0;
            FFI_CALL_TOS_4(ms_cb,x2,y2,z,state);
        }
    }
    return 0;
}

void SetMSCallback(void *fptr) {
    ms_cb=fptr;
}
LRESULT CALLBACK WndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
static HANDLE normal_c;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,PSTR lpCmdLine, int nCmdShow) {
    MSG  msg;
    HWND hwnd;
    int argc;
	char **argv;
    float dpi;
    WNDCLASS wc = {0};
    mutex=CreateMutexA(NULL,0,NULL);
    wc.lpfnWndProc=WndProc;
    wc.lpszClassName="Holy Drawer";
	wc.hInstance=hInstance;
	RegisterClassA(&wc);
	hwnd=CreateWindowExA(
		NULL,
		wc.lpszClassName,
		"Holy Drawer",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,0,
		NULL,NULL,
		hInstance,
		NULL
	);
	assert(hwnd);
	dpi=GetDpiForWindow(hwnd);
	SetWindowPos(
		hwnd,
		NULL,
		NULL,
		NULL,
		10+ceil(650.*dpi/96.), //Windows makes the window smaller for some reason
		10+ceil(510.*dpi/96.),
		SWP_NOMOVE
	);
	NewDrawWindow()->win=hwnd;
	argv=CommandLineToArgvA_wine(lpCmdLine,&argc);
	_main(argc,argv);
	SetCursor(NULL);
	ShowWindow(hwnd, nCmdShow);
	while (GetMessage(&msg, NULL, 0, 0))  {
		DispatchMessage(&msg);
	}
	return 0;
}
void DrawWindowUpdate(struct CDrawWindow *win,int8_t *colors,int64_t internal_width,int64_t h) {
	int64_t x,y,b,b2,mul=4;
	WaitForSingleObject(mutex,INFINITE);
	b2=479*internal_width;
	for(y=0;y!=480;y++) {
		//Windows stores lines bottom to top
		for(x=0;x!=640;x++) {
			buf[b2++]=palette[*colors++];
			//img->data[b*4+3]=0;
		}
		//Move 2 lines backwards(the current line and the pointer to the "next" line) see above note
		b2-=internal_width<<1;
	}
	ReleaseMutex(mutex);
	RedrawWindow(
		win->win,
		NULL,
		NULL,
		RDW_INVALIDATE|
		RDW_UPDATENOW|
		RDW_NOCHILDREN
	);
}
char *ClipboardText() {
	char *ret;
	OpenClipboard(0);
	HGLOBAL glob=GetClipboardData(CF_TEXT); 
	void *locked=GlobalLock(glob);
	if(locked)
		ret=strdup(locked);
	else
		ret=strdup("");
	CloseClipboard();
	GlobalUnlock(glob);
	return ret;
}
void SetClipboard(char *text) {
	//https://stackoverflow.com/questions/1264137/how-to-copy-string-to-clipboard-in-c
	int64_t len=strlen(text)+1;
	HGLOBAL hMem =  GlobalAlloc(GMEM_MOVEABLE, len);
	memcpy(GlobalLock(hMem), text, len);
	GlobalUnlock(hMem);
	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hMem);
	CloseClipboard();
}
LRESULT CALLBACK WndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {
	PAINTSTRUCT ps;
	HBITMAP bmp;
	HBRUSH brush,obrush;
	HDC dc,dc2;
	RECT rct;
	int64_t cx,cy;
	switch(msg) {
		case WM_KILLFOCUS:
			if(persist_mod&SCF_ALT)
				FFI_CALL_TOS_2(kb_cb,' ',SC_ALT|SCF_KEY_UP);
			if(persist_mod&SCF_CTRL)
				FFI_CALL_TOS_2(kb_cb,' ',SC_CTRL|SCF_KEY_UP);
			persist_mod=0;
			break;
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
			if(kb_cb)
				KBCallback(NULL,msg,wParam,&lParam);
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MOUSEWHEEL:
		case WM_MOUSEMOVE:
			SetCursor(NULL);
			if(kb_cb)
				MSCallback(hwnd,NULL,msg,wParam,lParam);
			break;
		case WM_PAINT:
			WaitForSingleObject(mutex,INFINITE);
			GetClientRect(hwnd,&rct);
			CenterWindow(hwnd,&cx,&cy);
			dc=BeginPaint(hwnd,&ps);
			dc2=CreateCompatibleDC(dc2);
			bmp=CreateCompatibleBitmap(dc,rct.right,rct.bottom);
			obrush=SelectObject(dc2,GetStockObject(BLACK_BRUSH));
			SelectObject(dc2,bmp);
			BITMAPINFO  binfo;
			memset(&binfo,0,sizeof(binfo));
			binfo.bmiHeader.biSize=sizeof(binfo);
			binfo.bmiHeader.biWidth=640;
			binfo.bmiHeader.biHeight=480;
			binfo.bmiHeader.biPlanes=1;
			binfo.bmiHeader.biBitCount=32;
			binfo.bmiHeader.biCompression=BI_RGB;
			Rectangle(dc2,0,0,rct.right,rct.bottom);
			SetDIBitsToDevice(dc2,cx,cy,640,480,0,0,0,480,buf,&binfo,DIB_RGB_COLORS);
			BitBlt(dc,0,0,rct.right,rct.bottom,dc2,0,0,SRCCOPY);
			SelectObject(dc2,obrush);
			EndPaint(hwnd,&ps);
			ReleaseMutex(mutex);
			DeleteDC(dc2);
			DeleteObject(bmp);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
}
static int32_t BGR48(uint16_t *c) {
	int64_t r=c[2]*(255./0xffFF),g=c[1]*(255./0xffFF),b=c[0]*(255./0xffFF); //0xffFF is the highest 16bit value 
	if(r>0xff) r=0xff;
	if(g>0xff) g=0xff;
	if(b>0xff) b=0xff;
	return ((r&0xff)<<16)|((g&0xff)<<8)|(b&0xff);
}
void GrPalleteSet(int c,int64_t _color) { //B,G,R,PAD
	  int16_t *color=&_color;
	  memcpy(gr_pallete_BGR48+2*4*c,color,2*4); //2 bytes,4 channels
	  palette[c]=gr_palette_std[c]=BGR48(color);
}
char *GrPalleteGet(int64_t c) {
	int64_t r=0;
	memcpy(&r,&gr_pallete_BGR48[2*4*c],2*4);
	return r;
}
