#ifndef HCRT_H
#define HCRT_H
#define NULL (0(U0*))
#define FALSE 0
#define TRUE 1
#define U8 U8i
#define I8 I8i
class CCmpCtrl;
U16i union U16
{
  I8i	i8[2];
  U8i	u8[2];
};
I16i union I16
{
  I8i	i8[2];
  U8i	u8[2];
};

U32i union U32
{
  I8i	i8[4];
  U8i	u8[4];
  I16	i16[2];
  U16	u16[2];
};

I32i union I32
{
  I8i	i8[4];
  U8i	u8[4];
  I16	i16[2];
  U16	u16[2];
};

U64i union U64
{
  I8i	i8[8];
  U8i	u8[8];
  I16	i16[4];
  U16	u16[4];
  I32	i32[2];
  U32	u32[2];
};

I64i union I64
{
  I8i	i8[8];
  U8i	u8[8];
  I16	i16[4];
  U16	u16[4];
  I32	i32[2];
  U32	u32[2];
};

class CFs {
  U64i except_ch;
  U64i catch_except;
  I64i rand_seed;
  CCmpCtrl *last_cc;
};

//MATH
#define I8_MIN (-0x80)
#define I8_MAX 0x7F
#define U8_MIN 0
#define U8_MAX 0xFF
#define I16_MIN (-0x8000)
#define I16_MAX 0x7FFF
#define U16_MIN 0
#define U16_MAX 0xFFFF
#define I32_MIN (-0x80000000)
#define I32_MAX 0x7FFFFFFF
#define U32_MIN 0
#define U32_MAX 0xFFFFFFFF
#define I64_MIN (-0x8000000000000000)
#define I64_MAX 0x7FFFFFFFFFFFFFFF
#define U64_MIN 0
#define U64_MAX 0xFFFFFFFFFFFFFFFF
#define U64_F64_MAX (0x43F0000000000000(F64))
#define F64_MAX (0x7FEFFFFFFFFFFFFF(F64))
#define F64_MIN (0xFFEFFFFFFFFFFFFF(F64))
#define inf	(0x7FF0000000000000(F64))
#define pi	(0x400921FB54442D18(F64))
extern I64 ToI64(F64 i);
extern F64 ToF64(I64 i);
extern F64 Clamp(F64 d,F64 lo,F64 hi);
extern I64 MinI64(I64 n1,I64 n2);
extern F64 Min(F64 n1,F64 n2);
extern F64 Max(F64 n1,F64 n2);
extern I64 MaxI64(I64 n1,I64 n2);
extern U64 FloorU64(U64 num,U64 to);
extern U64 CeilU64(U64 num,U64 to);
extern I64 RoundI64(I64 num,I64 to);
extern I64 FloorI64(I64 num,I64 to);
extern I64 CeilI64(I64 num,I64 to);
extern I16 RandI16();
extern U16 RandU16();
extern I32 RandI32();
extern U32 RandU32();
extern I64 RandI64();
extern U64 RandU64();
extern F64 Rand();
extern I64 RandRange(I64 min=0, I64 max);
extern I64 RandRangeSeed(I64 min=0, I64 max, I64 seed);
extern U64 ClampU64(U64 a,U64 min=0,U64 max);
extern I64 ClampI64(I64 a,I64 min=0,I64 max);
//BITS.HC
extern I64 BCnt(I64 value);
extern Bool Bt(U8 *data,I64 bit);
extern U0 BEqu(U8 *data,I64 bit,Bool v);
extern Bool Btc(U8 *data,I64 bit);
extern Bool Bts(U8 *data,I64 bit);
extern Bool Btr(U8 *data,I64 bit);
extern Bool BFieldOrU32(U8 *f,I64 bit,U32 patt);
extern U32 BFieldExtU32(U8 *f,I64 bit,I64 size);
//COMPRESS.HC
class CArcCompress;
extern U8 *ExpandBuf(CArcCompress *arc);
extern CArcCompress *CompressBuf(U8 *src,I64 size);
extern U8 *StrNew(U8 *in);
extern U8 ToUpper(U8 chr);
extern  U32
  char_bmp_alpha[16],
  char_bmp_alpha_numeric[16],
  char_bmp_alpha_numeric_no_at[16],
  char_bmp_word[16],
  char_bmp_filename[16],
  char_bmp_dec_numeric[16],
  char_bmp_hex_numeric[16],
  char_bmp_white_space[16],
  char_bmp_non_eol_white_space[16],
  char_bmp_zero_cr_nl_cursor[16],
  char_bmp_zero_tab_cr_nl_cursor[16],
  char_bmp_zero_tab_cr_nl_cursor_dollar[16],
  char_bmp_macro[16],
  char_bmp_printable[16],
  char_bmp_displayable[16],
  char_bmp_safe_dollar[16],
  char_bmp_non_eol[16];
extern I64 Str2I64(U8 *st,I64 radix=10,U8 **_end_ptr=NULL);
extern F64 Str2F64(U8 *src,U8 **_end_ptr=NULL);
extern U8 *StrScan(U8 *src,U8 *fmt,...);
extern U8 *GetStr(U8 *msg=NULL,U8 *dft=NULL,I64 flags=0);
extern Bool YorN();
extern I64 PressAKey();
extern I64 GetI64(U8 *msg=NULL,I64 dft=0,I64 lo=I64_MIN,I64 hi=I64_MAX);
extern F64 GetF64(U8 *msg=NULL,F64 dft=0,F64 lo=F64_MIN,F64 hi=F64_MAX);
extern U8 *StrIMatch(U8 *src,U8 *find);
extern I64 StrOcc(U8 *src, I64 ch);
extern U8 *StrFirstOcc(U8 *src,U8 *marker);
extern U8 *StrLastOcc(U8 *str,U8 *marker);
extern U8 *StrFirstRem(U8 *src,U8 *marker,U8 *dst=NULL);
extern U8 *StrLastRem(U8 *src,U8 *marker,U8 *dst=NULL);
//FIFO.HC
class CFifoU8 {
  U8 *buf;
  I64 mask,in_ptr,out_ptr;
};
class CFifoI64 {
  I64 *buf;
  I64 mask,in_ptr,out_ptr;
};
extern CFifoU8 *FifoU8New(I64 size);
extern U0 FifoU8Del(CFifoU8 *f);
extern Bool FifoU8Ins(CFifoU8 *f,U8 b);
extern Bool FifoU8Rem(CFifoU8 *f,U8 *_b);
extern Bool FifoU8Peek(CFifoU8 *f,U8 *_b);
extern U0 FifoU8Flush(CFifoU8 *f);
extern I64 FifoU8Cnt(CFifoU8 *f);
extern CFifoI64 *FifoI64New(I64 size);
extern U0 FifoI64Del(CFifoI64 *f);
extern Bool FifoI64Ins(CFifoI64 *f,I64 q);
extern Bool FifoI64Rem(CFifoI64 *f,I64 *_q);
extern Bool FifoI64Peek(CFifoI64 *f,I64 *_q);
extern U0 FifoI64Flush(CFifoI64 *f);
extern I64 FifoI64Cnt(CFifoI64 *f);
U0 LoadBinModule(U8 *fn,Bool verbose=FALSE);
#define CDIR_FILENAME_LEN 1024
//FILE.HC
class CDirEntry
{
  CDirEntry *next,*parent,*prev;
 U16 attr;	//Unused in Cubit
 U8 name[CDIR_FILENAME_LEN];
 I64 size;	//In bytes
 I64 datetime; //In Unix time(not used yet)
};
extern U8 *DirFile(U8 *dirname,U8 *name=NULL,U8 *_extension=NULL);
extern U8 *FileExtDot(U8 *src);
extern Bool FileFind(U8 *str,CDirEntry *_en=NULL,I64 flags=0);
#define FUF_RECURSE (1<<0)
#define FUF_SINGLE (1<<1)
#define FUF_JUST_DIRS (1<<3)
#define FUF_JUST_DIRS (1<<4)
#define FUF_JUST_FILES (1<<5)
#define FUF_JUST_TXT (1<<6)
#define FUF_JUST_DD (1<<7)
#define FUF_JUST_SRC (1<<9)
extern CDirEntry *FilesFind(U8 *mask,I64 flags=0);
extern U0 DirEntryFree(CDirEntry *ents);
extern U0 Dir(U8 *path=".");
extern U0 Zip(U8 *f,I64 flags=0);
extern U0 Unzip(U8 *f,I64 flags=0);
extern U0 Move(U8 *mask,U8 *to);
//HASH.HC
class CHash {
  CHash* next;
  U8* str;
  U32 type, use_cnt;
};
class CHashTable {
  I64 mask;
  CHash** body;
};
extern CHashTable* HashTableNew(I64 size, U0* pool = NULL);
extern I64 HashStr(U8* str);
extern U0 HashAdd(CHash* tmp, CHashTable* t);
extern CHash* HashFind(U8* needle, CHashTable* table);
U0 HashTableDel(CHashTable* table, U0(*free)(CHash* h) = NULL);
//PRINT.HC
extern U8 *StrPrintJoin(U8 *dst,U8 *fmt,I64 argc,I64 *argv);
extern U8 *StrPrint(U8 *dst,U8 *fmt,...);
extern U8 *CatPrint(U8 *_dst,U8 *fmt,...);
extern U0 Print(U8 *fmt,...);
extern U8 *MStrPrint(U8 *fmt,...);
//QSORT.HC
extern U0 SwapI64(I64 *a,I64 *b);
extern U0 QSortI64(I64 *base,I64 num, I64 (*fp_compare)(I64 e1,I64 e2));
extern U0 QSort2a(U8 **base,I64 num,I64 (*fp_compare)(U8 **_e1,U8 **_e2));
extern U0 QSort2b(U8 *base,I64 num, I64 width,
	I64 (*fp_compare)(U8 *e1,U8 *e2),U8 *tmp);
extern U0 QSort(U8 *base,I64 num, I64 width, I64 (*fp_compare)(U8 *e1,U8 *e2));
//ASM.HC
class CReg:CHash {
    I64 code,type;
};
#define X64ModRMIndir 0b00
#define X64ModRMIndir8 0b01
#define X64ModRMIndir32 0b10
#define X64ModRMDirect 0b11
#define OPC_F_SR 1 // /R
#define OPC_F_ADDR (1<<1) //+R
#define OPC_F_IB (1<<3)
#define OPC_F_IW (1<<4)
#define OPC_F_ID (1<<5)
#define OPC_F_IQ (1<<6)
#define OPC_F_O0 (1<<7)
#define OPC_F_O1 (1<<8)
#define OPC_F_O2 (1<<9)
#define OPC_F_O3 (1<<10)
#define OPC_F_O4 (1<<11)
#define OPC_F_O5 (1<<12)
#define OPC_F_O6 (1<<13)
#define OPC_F_O7 (1<<14)
#define OPC_F_32bit (1<<15)
#define OPC_F_48REX (1<<16)
#define OPC_F_REX_R8_15ONLY (1<<17)
#define OPC_F_STI (1<<18)
#define OPC_F_ADDI (1<<19)
//Operand size prefix
#define OPC_F_16 (1<<20)
#define OPC_F_COND (1<<21)
class COpcodeArg {
    I64 type;
    #define OPCA_NONE 0
    #define OPCA_REG 1
    #define OPCA_R8 2
    #define OPCA_R16 3
    #define OPCA_R32 4
    #define OPCA_R64 5
    #define OPCA_RM8 6
    #define OPCA_RM16 7
    #define OPCA_RM32 8
    #define OPCA_RM64 9
    #define OPCA_IMM8 10
    #define OPCA_IMM16 11
    #define OPCA_IMM32 12
    #define OPCA_IMM64 13
    #define OPCA_MOFFS8 14
    #define OPCA_MOFFS16 15
    #define OPCA_MOFFS32 16
    #define OPCA_UIMM8 17
    #define OPCA_UIMM16 18
    #define OPCA_UIMM32 19
    #define OPCA_UIMM64 20
    #define OPCA_REL8 21
    #define OPCA_REL16 22
    #define OPCA_REL32 23
    #define OPCA_M8 24
    #define OPCA_M16 25
    #define OPCA_M32 26
    #define OPCA_M64 27
    #define OPCA_STI 28
    #define OPCA_SREG 29
    union {
        CReg *reg;
    };
};
#define HASH_TYPE_OPCODE 0
#define HASH_TYPE_OPCODE_ALIAS 1
class COpcode:CHash {
    COpcode *prev_op,*next_op;
    U8 bytes[16];
    I64 bcnt;
    I64 flags,arg_count;
    COpcodeArg args[3];
};
class COpcodeAlias:CHash {
    COpcode *opcode;
};
class X64Mode {
    I64 type;
    #define X64O_REG 0
    #define X64O_MEM 1
    #define X64O_IMM 2
    #define X64O_LABEL 3
    union {
        CReg *reg;
        class {
	    CReg *segment;
            CReg *base;
            CReg *index;
            I64 scale;
            I32 disp;
            I64 width;
        } mem;
        class {
            U0 *jit_label;
            I64 label_offset;
        };
        U64 imm;
    };
};
extern CReg *Str2Reg(U8 *str);
extern X64Mode *X64ModeLabel(U0 *lab);
extern X64Mode *X64ModeReg(CReg *reg);
extern U8 *AsmError;
extern X64Mode *X64ModeMem(CReg *segment,I64 scale,CReg *index,CReg *base,I64 disp,I64 width=8);
extern X64Mode *X64ModeImm(U64 imm);
extern U8 X64ModRM(I64 modrmMode,I64 r, I64 rm);
extern U8 X64SIB(I64 s,I64 i,I64 b);
extern Bool IsOpcode(U8 *name);
extern CReg *GetRegister(U8 *name);
extern U8 *X86ModRMSIBDisp(U8 *it,I64 size,U8 *opcode,I64 opc_len,CReg *reg,I64 scale,CReg *index,CReg *base,I64 disp,I64 *sibooff=NULL);
extern U0 X64ModeFree(X64Mode *m) ;
/**
 * Jumps are not encoded here
 */
extern U8 *EncodeOpcode(U8 *ip,U8 *name,Bool *is_jmp,I64 *sibooff ,I64 *immoff,...);
//LEXER.HC
CCmpCtrl *CmpCtrlNew(U8 *buf=NULL,I64 flags=0,U8 *filename=NULL);
I64 Lex(CCmpCtrl *cc);
U0 *LexItem(CCmpCtrl *ctrl);
U0 LexIncludeStr(CCmpCtrl *cc,U8 *abs_filename,U8 *src,Bool actual_file);
Bool LexSetReplMode(CCmpCtrl *ctrl,Bool set); //Returns old value
U8 *LexMatchFile(CCmpCtrl *cc,U8 *name);
I64 *LexFileLines(CCmpCtrl *cc,U8 *name,I64 *cnt);
U0 __StreamPrint(U8 *fmt,...);
// ../HolyEd/EDITOR.HC
U0 Ed(U8 *fn);
//runtime.c
extern U0 *signal(I64 sig,U0 *handler); //runtime.c
extern U0 UnblockSignals(); //runtime.c
Bool IsMac();
Bool IsWindows();
U0 UnblockSignals();
Bool IsDir(U8 *dir);
U8 *FileRead(U8 *fn,I64* cnt=NULL);
U0 FileWrite(U8 *fn,U8 *d,I64* cnt=NULL);
U0 CreateTagsAndErrorsFiles(U8 *t,U8 *e, U8 *root);
#ifdef RUNTIME_C
I64 SDL_Init(I64 flags=SDL_INIT_EVERYTHING);
Bool SDL_WasInit();
SDL_Window *SDL_CreateWindow(U8 *t,I64 x= SDL_WINDOWPOS_UNDEFINED,I64 y= SDL_WINDOWPOS_UNDEFINED,I64 w=640,I64 h=480,I64 flags=SDL_WINDOW_RESIZABLE);
I64 SDL_SetTextureBlendMode(SDL_Texture *t,I64);
I64 SDL_CreateWindowAndRenderer(I64 w=640,I64 h=480,I64 f=SDL_WINDOW_RESIZABLE,SDL_Window **,SDL_Renderer**);
I64 SDL_DestroyWindow(SDL_Window *);
I64 SDL_DestroyRenderer(SDL_Renderer*);
U0 SDL_DisableScreenSaver();
SDL_Window *SDL_GetGrabbedWindow();
U0 SDL_GetWindowPosition(SDL_Window *,I32 *,I32 *);
U0 SDL_GetWindowMinimumSize(SDL_Window *,I32 *,I32 *);
U0 SDL_GetWindowMinimumSize(SDL_Window *,I32 *,I32 *);
U0 SDL_GetWindowMaximumSize(SDL_Window *,I32 *,I32 *);
U0 SDL_GetWindowSize(SDL_Window *,I32 *,I32 *);
U8 *SDL_GetWindowTitle(SDL_Window *);
U0 SDL_HideWindow(SDL_Window *);
U0 SDL_MaximizeWindow(SDL_Window *);
U0 SDL_MinimizeWindow(SDL_Window *);
U0 SDL_SetWindowBordered(SDL_Window *,Bool);
I64 SDL_SetWindowFullscreen(SDL_Window *,I64 flags=0);
U0 SDL_SetWindowMaximumSize(SDL_Window *,I64,I64);
U0 SDL_SetWindowMinimumSize(SDL_Window *,I64,I64);
U0 SDL_SetWindowSize(SDL_Window *,I64,I64);
U0 SDL_SetWindowResizable(SDL_Window *,Bool);
U0 SDL_SetWindowTitle(SDL_Window *wind,U8 *);
U0 SDL_ShowWindow(SDL_Window *wind);
SDL_Renderer *SDL_CreateRenderer(SDL_Window*,I64 ,I64 flags=SDL_RENDERER_ACCELERATED);
SDL_Texture *SDL_CreateTexture(SDL_Renderer*,U64 fmt,I64 access,I64 w,I64 h);
I64 SDL_DestroyRenderer(SDL_Renderer*);
SDL_Renderer *SDL_GetRenderer(SDL_Window*);
I64 SDL_GetRenderDrawColor(SDL_Renderer*,U8 *,U8 *,U8 *,U8 *);
I64 SDL_GetRendererOutputSize(SDL_Renderer*,I64 *,I64 *);
SDL_Texture *SDL_GetRenderTarget(SDL_Renderer *);
I64 SDL_GetTextureAlphaMod(SDL_Texture*,U8 *);
I64 SDL_GetTextureColorMod(SDL_Texture*,U8 *);
I64 SDL_RenderClear(SDL_Renderer*);
SDL_Surface *SDL_GetWindowSurface(SDL_Window*);
I64 SDL_RenderCopy(SDL_Renderer*,SDL_Texture*,SDL_Rect *s,SDL_Rect *d);
I64 SDL_RenderDrawLine(SDL_Renderer *,I64,I64,I64,I64);
I64 SDL_RenderDrawPoint(SDL_Renderer*,I64,I64);
I64 SDL_RenderDrawPoints(SDL_Renderer *,SDL_Point *,I64);
I64 SDL_RenderDrawRect(SDL_Renderer*,SDL_Rect*);
I64 SDL_RenderDrawRects(SDL_Renderer*,SDL_Rect*,I64);
I64 SDL_RenderFillRect(SDL_Renderer*,SDL_Rect*);
I64 SDL_RenderFillRects(SDL_Renderer*,SDL_Rect*,I64);
U0 SDL_RenderGetClipRect(SDL_Renderer*,SDL_Rect *);
U0 SDL_RenderPresent(SDL_Renderer*);
I64 SDL_RenderSetClipRect(SDL_Renderer*,SDL_Rect *);
I64 SDL_SetTextureAlphaMod(SDL_Texture*,U8);
I64 SDL_SetTextureColorMod(SDL_Texture*,U8,U8,U8);
I64 SDL_UpdateTexture(SDL_Texture*,SDL_Rect*,U0 *,I64);
I64 SDL_QueryTexture(SDL_Texture*,U32 *,I32 *,U0 *,I32*,I32*);
U8 *SDL_GetClipboardText();
I64 SDL_SetClipboardText(U8*);
I64 SDL_PollEvent(SDL_Event *);
I64 SDL_WaitEvent(SDL_Event *);
U0 SDL_DestroyTexture(SDL_Texture*);
U0 SDL_StartTextInput();
U0 SDL_StopTextInput();
U8 *SDL_GetError();
U0 SDL_ClearError();
U0 SDL_FlushEvent(U32);
I64 SDL_GetSurfaceWidth(SDL_Surface*);
I64 SDL_GetSurfaceHeight(SDL_Surface*);
SDL_Surface *SDL_CreateRGBSurface(U32 flags=0,I64 w,I64 h,I64 d,I64 rm,I64 gm,I64 bm,I64 am);
SDL_Surface *SDL_CreateRGBSurfaceFrom(U0 *,I64 w,I64 h,I64 d,I64 p,U32,U32,U32,U32);
I64 SDL_UpperBlit(SDL_Surface*,SDL_Rect *,SDL_Surface *,SDL_Rect *);
I64 SDL_FillRect(SDL_Surface*,SDL_Rect*,U32);
I64 SDL_FillRects(SDL_Surface*,SDL_Rect*,I64,U32);
U0 SDL_GetClipRect(SDL_Surface*,SDL_Rect*);
I64 SDL_GetSurfaceAlphaMod(SDL_Surface*,U8 *);
I64 SDL_GetSurfaceColorMod(SDL_Surface*,U8 *,U8 *,U8 *);
I64 SDL_LockSurface(SDL_Surface*);
U0 SDL_UnlockSurface(SDL_Surface*);
Bool SDL_SetClipRect(SDL_Surface *,SDL_Rect*);
I64 SDL_SetColorKey(SDL_Surface *,I64 f,U32);
I64 SDL_SetSurfaceAlphaMod(SDL_Surface*,U8 );
I64 SDL_SetSurfaceColorMod(SDL_Surface*,U8,U8,U8);
I64 SDL_SetSurfaceRLE(SDL_Surface*,I64);
I64 SDL_BlitSurface(SDL_Surface*,SDL_Rect*,SDL_Surface*,SDL_Rect*);
I64 SDL_BlitScaled(SDL_Surface*,SDL_Rect*,SDL_Surface*,SDL_Rect*);
U0 SDL_FreeSurface(SDL_Surface*);
I64 SDL_UpdateWindowSurface(SDL_Window*);
I64 SDL_GetColorKey(SDL_Surface*,U32*);
#endif
U0 Del(U8 *);
U8 *__GetBuiltinMacrosText();
CFs *Fs();
extern U0 HCSetJmp();
extern U0 PopTryFrame();
extern U0 EnterTry();
extern U0 ExitCatch();
extern I64 PowI64(I64 a,I64 b);
extern Bool WhineOnOutOfBounds(U0 *a,I64 b);
extern F64 Pow(F64 a,F64 b);
extern U0 GC_Collect();
extern I64 MSize(U0 *a);
extern I64 Bsf(I64 a);
extern I64 Bsr(I64 a);
extern U0 throw(U64 a);
extern U0 *MAlloc(I64 a);
extern U0 Free(U0*a);
extern U0 *MemCpy(U0*a,U0*b,I64 c) ;
extern U0 *MemNCpy(U0*a,U0*b,I64 c);
extern I64 StrLen(U8*a);
extern I64 StrCmp(U8*a,U8 *b);
extern I64 StrNCmp(U8*a,U8 *b,I64 c);
extern I64 StrMatch(U8*a,U8 *b);
extern I64 StrCpy(U8*a,U8 *b);
extern I64 StrNCpy(U8*a,U8 *b,I64 c);
extern CFs *Fs();
extern U8 *__GetStr(U8 *a);
extern F64 ATan(F64 a);
extern F64 Abs(F64 a);
extern F64 Sin(F64 a);
extern F64 Cos(F64 a);
extern F64 Sqrt(F64 a);
extern F64 Tan(F64 a);
extern F64 Ceil(F64 a);
extern F64 Floor(F64 a);
extern F64 Log(F64 a);
extern F64 Log10(F64 a);
extern F64 Log2(F64 a);
extern F64 Round(F64 a);
extern F64 Trunc(F64 a);
extern U0 Exit(I64 code=0);
extern I64 Cd(U8 *to=".");
extern U8 *DirCur();
extern I64 DirMk(U8 *a);
extern U8 *__Move(U8 *a,U8 *b);
extern U8 *FileNameAbs(U8 *a);
extern U8 *__IsDir(U8 *a);
extern U8 *FileWrite(U8 *a,U0 *b,I64 c);
extern U8 *FileRead(U8 *a,I64 *len=NULL);
extern U8 **__Dir(U8 *a);
extern U0 __LexExe();
extern U0 *MemSet(U0 * a,U8 b,I64 c);
extern U0 Print(U8 * a,...);
extern CFs *Fs();
extern Bool IsWindows();
extern Bool IsDir(U8 *dir);
extern U0 TOSPrint(U8 * a,...);
extern I64 JIT_Eval(U8 * a,U8*  a);
extern U0 *HC_CreateToken(I64  a);
extern U0 *HC_CreateStr(U8 * a);
extern U0 *HC_CreateIdent(U8 * a);
extern U0 *HC_CreateI64(I64 a);
extern U0 *HC_CreateF64(F64 f);
extern U0 *HC_SetPosFromLexer(U0 *a,U8 *b,I64 c);
extern U0 Debugger(U0 *regs=NULL,U0 *bp=NULL,U0 *caller=NULL);
extern U0 ForeachFuncInTable(U0(*fptr)());
extern U0 VirtAlloc(I64 a);
extern U0 VirtFree(U0* p,I64 s);
U0 WriteTagsAndErrsToFile(U8 *src,U8 *tags=NULL,U8 *errs=NULL);
I64 PoopSetGCEnable(I64 en);
I64 GetUnique();
U0 Debugger();
//For internal use 
U0 RegisterRuntimeClasses(U0*gt,U0*fwd,U0*am,U0 *cat);
#endif





