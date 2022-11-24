#include "3d.h"
#include <windows.h>
#include <winuser.h>
#include <wingdi.h>
#include <shellapi.h>
#include <windowsx.h>
#include <winbase.h>
#include <shellscalingapi.h>
#include <math.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wincodec.h>
extern "C" {
	using namespace D2D1;
HANDLE *mutex;
LPSTR* WINAPI CommandLineToArgvA_wine(LPSTR lpCmdline, int* numargs);
typedef enum {
	_3D_REND_WIN32_SOFT_SCALE,
	_3D_REND_WIN32_GDI_SCALE,	
} _3DaysRenderer;
typedef struct CDrawWindow {
    int64_t sz_x,sz_y;
    int64_t scaling_enabled;
    int64_t scroll_x,scroll_y,changed_reso;
    HWND win;
    HDC dc;
    double gl_top,gl_left,gl_bottom,gl_right;
    char *texture_address;
    double dpi;
    ID2D1Factory *factory;
    _3DaysRenderer renderer_type; 
} CDrawWindow;
static void StartInputScanner();
static CDrawWindow *dw=NULL;
static char *clip_text=NULL;
CDrawWindow *NewDrawWindow() {
	if(!dw) {
		dw=TD_MALLOC(sizeof(*dw));
	}
	return dw;
}
int32_t buf[SCREEN_WIDTH*SCREEN_HEIGHT];
int64_t __3DaysSwapRGB() {
	return 0;
}
void __3DaysEnableScaling(int64_t s) {
	dw->scaling_enabled=s;
}
void DrawWindowDel() {
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
    static int64_t x,y;
    static int state;
    static int z;
    RECT rect;
    if(ms_cb) {
		switch(msg) {
			case WM_MOUSEWHEEL:
			if(GET_WHEEL_DELTA_WPARAM(w)>1)
				z-=1;
			else
				z+=1;
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
             if(dw->renderer_type==_3D_REND_WIN32_GDI_SCALE) {
				//Window corantes go from top to bottom
				int64_t y2=dw->sz_y-y;
				int64_t x2=x;
				//We need to scale out coordnates to the "screen rectangle"
				//OpenGL cordnates go from (-1.0,-1.0) to (1.0,1.0)
				//Our silly sauce rectangle doesn't take up the whole screen,
				//We use (dw->gl_left,dw->gl_bottom),(dw->gl_right,dw->gl_top)
				double xd=x2/(double)dw->sz_x*2. -1.; //Make it go from -1 to 1
				double yd=y2/(double)dw->sz_y*2. -1.; //Ditto
				if(xd<dw->gl_left)
					xd=dw->gl_left;
				if(xd>dw->gl_right)
					xd=dw->gl_right;
				if(yd<dw->gl_top)
					yd=dw->gl_top;
				if(yd>dw->gl_bottom)
					yd=dw->gl_bottom;
				yd=(yd-dw->gl_bottom)/(dw->gl_top-dw->gl_bottom);
				xd=(xd-dw->gl_left)/(dw->gl_right-dw->gl_left);
				y2=SCREEN_HEIGHT*yd;
				x2=SCREEN_WIDTH*xd;
				FFI_CALL_TOS_4(ms_cb,x2,y2,z,state);
			} else
				FFI_CALL_TOS_4(ms_cb,x,y,z,state);
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
    int argc,pf;
	char **argv;
    WNDCLASS wc = {0};
    mutex=CreateMutexA(NULL,0,NULL);
    wc.lpfnWndProc=WndProc;
    wc.lpszClassName="Holy Drawer";
	wc.hInstance=hInstance;
	wc.style=CS_HREDRAW|CS_VREDRAW;
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
	//float dpi=GetDpiForWindow(hwnd);
	float dpi=96;
	SetWindowPos(
		hwnd,
		NULL,
		NULL,
		NULL,
		ceil(SCREEN_WIDTH*dpi/96.), //Windows makes the window smaller for some reason
		ceil(SCREEN_HEIGHT*dpi/96.),
		SWP_NOMOVE
	);
	NewDrawWindow()->win=hwnd;
	dw->scaling_enabled=1;
	dw->renderer_type=_3D_REND_WIN32_GDI_SCALE;
	dw->dpi=dpi;
	dw->texture_address=(void*)buf;
	if(0) {
		software:
		dw->renderer_type=_3D_REND_WIN32_SOFT_SCALE;
	} else {
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,&dw->factory);
	}
	argv=CommandLineToArgvA_wine(lpCmdLine,&argc);
	_main(argc,argv);
	SetCursor(NULL);
	ShowWindow(hwnd, nCmdShow);
	while (GetMessage(&msg, NULL, 0, 0))  {
		DispatchMessage(&msg);
	}
	return 0;
}
void DrawWindowUpdate(struct CDrawWindow *ul,int8_t *colors,int64_t internal_width,int64_t h) {
	WaitForSingleObject(mutex,INFINITE);
	if(dw->renderer_type==_3D_REND_WIN32_GDI_SCALE)
		RedrawWindow(dw->win , NULL , NULL , RDW_INVALIDATE);
	else if(dw->renderer_type==_3D_REND_WIN32_SOFT_SCALE) {
		int64_t x,y,b,b2,mul=4;
		HBITMAP bmp;
		RECT rct;
		HDC dc=GetDC(dw->win);
		GetClientRect(dw->win,&rct);
		BITMAPINFO  binfo;
		memset(&binfo,0,sizeof(binfo));
		binfo.bmiHeader.biSize=sizeof(binfo);
		binfo.bmiHeader.biWidth=dw->sz_x;
		//See https://learn.microsoft.com/en-us/windows/win32/wmdm/-bitmapinfoheader
		//We want a top down DIB
		binfo.bmiHeader.biHeight=-dw->sz_y;
		binfo.bmiHeader.biPlanes=1;
		binfo.bmiHeader.biBitCount=32;
		binfo.bmiHeader.biCompression=BI_RGB;
		SetDIBitsToDevice(dc,0,0,dw->sz_x,dw->sz_y,0,0,0,dw->sz_y,dw->texture_address,&binfo,DIB_RGB_COLORS);
		ReleaseMutex(mutex);
		ReleaseDC(dw->win,dc);
		if(dw->changed_reso&&rct.right-rct.left!=dw->sz_x&&rct.bottom-rct.top!=dw->sz_y) {
			if(map_get(&TOSLoader,"SetScaleResolution")) {
				FFI_CALL_TOS_2(map_get(&TOSLoader,"SetScaleResolution")->data[0].val,rct.right-rct.left,rct.bottom-rct.top);
				dw->changed_reso=0;
			}
		}
	}
	ReleaseMutex(mutex);
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
	int64_t cx,cy;
	RECT *lr,rct;
	if(_shutdown) goto kill;
	if(!dw) goto dft;
	switch(msg) {
		case WM_SIZE:
		case WM_SIZING:
			if(dw->renderer_type==_3D_REND_WIN32_SOFT_SCALE)
				dw->changed_reso=1;
			break;
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
			if(dw->renderer_type==_3D_REND_WIN32_GDI_SCALE) {
				WaitForSingleObject(mutex,INFINITE);
				GetClientRect(hwnd,&rct);
				dw->sz_x=rct.right-rct.left;
				dw->sz_y=rct.bottom-rct.top;
				float sx,sy,ox,oy,rx,ry;
				if(dw->scaling_enabled) {
					sx=dw->sz_y/480.*SCREEN_WIDTH.,sy=dw->sz_y;
					if(dw->sz_x<sx) {
						sy=dw->sz_x/SCREEN_WIDTH.*480.;
						sx=dw->sz_x;
					}
					rx=sx;
					ry=sy;
					ox=(dw->sz_x-sx);
					oy=(dw->sz_y-sy);
					sx=1.-ox/dw->sz_x;
					sy=1.-oy/dw->sz_y;			
				} else {
					sx=SCREEN_WIDTH./dw->sz_x,sy=480./dw->sz_y;
					rx=SCREEN_WIDTH;ry=480;
				}
				dw->gl_left=-sx;
				dw->gl_right=sx;
				dw->gl_top=-sy;
				dw->gl_bottom=sy;
				int64_t x,y,b,b2,mul=4;
				ID2D1HwndRenderTarget *target=NULL;
				ID2D1Bitmap *bmp;
				D2D1_SIZE_U size=SizeU(dw->sz_x,dw->sz_y);
				dw->factory->CreateHwndRenderTarget(
					RenderTargetProperties(),
					HwndRenderTargetProperties(hwnd,size),
					&target
				);
				target->BeginDraw();
				D2D1_SIZE_U holyres=SizeU(SCREEN_WIDTH,480);
				D2D1_BITMAP_PROPERTIES prop;
				prop.pixelFormat=PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_IGNORE);
				prop.dpiX=96;
				prop.dpiY=96;
				target->CreateBitmap(holyres,prop,&bmp);
				bmp->CopyFromMemory(NULL,dw->texture_address,SCREEN_WIDTH*4);
				D2D1_RECT_F dst_rct=RectF((dw->sz_x-rx)/2,(dw->sz_y-ry)/2,(dw->sz_x-rx)/2+rx,(dw->sz_y-ry)/2+ry);
				target->DrawBitmap(bmp,&dst_rct,1.0,D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,NULL);
				target->EndDraw();
				bmp->Release();
				target->Release();
				ReleaseMutex(mutex);
			
			}
			return 0;
		case WM_DESTROY:
kill:
			PostQuitMessage(0);
			return 0;
	}
dft:
	return DefWindowProc(hwnd,msg,wParam,lParam);
}
void *_3DaysSetResolution(int64_t w,int64_t h) {
	WaitForSingleObject(mutex,INFINITE);
	if(dw->renderer_type==_3D_REND_WIN32_GDI_SCALE) {
		//Do nothing
		dw->texture_address=(void*)buf;
	} else if(dw->renderer_type==_3D_REND_WIN32_SOFT_SCALE) {
		dw->sz_x=w,dw->sz_y=h;
		if(dw->texture_address)
			free(dw->texture_address);
		dw->texture_address=calloc(w*h,4);
	}
	ReleaseMutex(mutex);
	return dw->texture_address;
}
static HANDLE revents[64],finevents[64];
static HANDLE scalers[64];
static void LaunchScaler(void *c) {
	for(;;) {
		WaitForSingleObject(revents[(int)c],INFINITE);
		void *fp=map_get(&TOSLoader,"ScalerMP")->data[0].val;
		FFI_CALL_TOS_1(fp,c);
		SetEvent(finevents[(int)c]);
	}
	return 0;
}
void _3DaysScaleScrn(){
	static int64_t mp_cnt;
	int64_t i;
	//See T/GR/Scale
	if(!mp_cnt) {
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		mp_cnt=info.dwNumberOfProcessors;
		for(i=0;i!=mp_cnt;i++) {
			revents[i]=CreateEvent(NULL,FALSE,FALSE,NULL);
			finevents[i]=CreateEvent(NULL,TRUE,TRUE,NULL);
		}
		for(i=0;i!=mp_cnt;i++)
			scalers[i]=CreateThread(NULL,0,&LaunchScaler,i,0,NULL);	
	}
	for(i=0;i!=mp_cnt;i++) {
		ResetEvent(finevents[i]);
		SetEvent(revents[i]);
	}
	WaitForMultipleObjects(mp_cnt,finevents,TRUE,INFINITE);
}
};
