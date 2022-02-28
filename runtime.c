#include "3d.h"
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
extern int64_t HCSetJmp(void *ptr);
extern void HCLongJmp(void *ptr);
#ifndef MACOS
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_clipboard.h>
#include <SDL2/SDL_events.h>
#else
#include <SDL.h>
#include <SDL_video.h>
#include <SDL_render.h>
#include <SDL_rect.h>
#include <SDL_clipboard.h>
#include <SDL_events.h>
#endif
#include <stddef.h>
#include <stdalign.h>
#ifdef TARGET_WIN32
#include <windows.h>
#include <fileapi.h>
#include <shlwapi.h>
#include <memoryapi.h>
#else
#include <sys/mman.h>
#endif
typedef struct CType CType;
static void *jit_INIT() {
    return jit_init();
}
#ifdef TARGET_WIN32
#include <winnt.h>
#include <dbghelp.h>
#endif
static void *jit_BREAKPOINT(void *jit,void *bp,void *routine,void *ctrl) {
  return jit_breakpoint(jit,bp,routine,ctrl);
}
static void jit_GENERATE_CODE(void *jit) {
	jit_generate_code(jit,NULL);
}
static int64_t jit_BINSIZE(void *jit) {
    return jit_bin_size(jit);
}
static void jit_DUMP_OPS(void *jit,int l) {
	jit_dump_ops(jit,l);
}
static int64_t jit_R(int64_t r) {
    return R(r);
}
static int64_t jit_FR(int64_t r) {
    return FR(r);
}
static int64_t jit_R_FP() {
    return R_FP;
}
static void *jit_ANDR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_andr(jit,a,b,c);
}
static void *jit_ANDI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_andi(jit,a,b,c);
}
static void *jit_GET_LABEL(void *j) {
    return jit_get_label(j);
}
static void *jit_PROLOG(void *jit,void *fp) {
    return jit_prolog(jit,fp);
}
static void *jit_MOVR(void *jit,int64_t a,int64_t b) {
    return jit_movr(jit,a,b);
}
static void *jit_MOVI(void *jit,int64_t a,int64_t b) {
    return jit_movi(jit,a,b);
}
static void *jit_JMPR(void *jit,int64_t a) {
    return jit_jmpr(jit,a);
}
static void *jit_JMPI(void *jit,int64_t a) {
    return jit_jmpi(jit,a);
}
static void *jit_PATCH(void *jit,void *a) {
    return jit_patch(jit,a);
}
static void *jit_PREPARE(void *jit) {
    return jit_prepare(jit);
}
static void *jit_PUTARGI(void *jit,int64_t a) {
    return jit_putargi(jit,a);
}
static void *jit_PUTARGR(void *jit,int64_t a) {
    return jit_putargr(jit,a);
}
static void *jit_CALL(void *jit,int64_t a) {
    return jit_call(jit,a);
}
static void *jit_CALLR(void *jit,int64_t a) {
    return jit_callr(jit,a);
}
static void *jit_DECLARE_ARG(void *jit,int64_t a,int64_t b) {
    return jit_declare_arg(jit,a,b);
}
static void *jit_RETR(void *jit,int64_t a) {
    return jit_retr(jit,a);
}
static void *jit_RETI(void *jit,int64_t a) {
    return jit_reti(jit,a);
}
static void *jit_RETVAL(void *jit,int64_t a) {
    return jit_retval(jit,a);
}
static void *jit_GETARG(void *jit,int64_t a,int64_t b) {
    return jit_getarg(jit,a,b);
}
static void *jit_ADDR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_addr(jit,a,b,c);
}
static void *jit_ADDI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_addi(jit,a,b,c);
}
static void *jit_SUBR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_subr(jit,a,b,c);
}
static void *jit_SUBI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_subi(jit,a,b,c);
}
static void *jit_NEGR(void *jit,int64_t a,int64_t b) {
    return jit_negr(jit,a,b);
}
static void *jit_MULR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_mulr(jit,a,b,c);
}
static void *jit_MULI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_muli(jit,a,b,c);
}
static void *jit_MULR_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_mulr_u(jit,a,b,c);
}
static void *jit_MULI_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_muli_u(jit,a,b,c);
}
static void *jit_DIVR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_divr(jit,a,b,c);
}
static void *jit_DIVI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_divi(jit,a,b,c);
}
static void *jit_DIVR_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_divr_u(jit,a,b,c);
}
static void *jit_DIVI_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_divi_u(jit,a,b,c);
}
static void *jit_MODR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_modr(jit,a,b,c);
}
static void *jit_MODI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_modi(jit,a,b,c);
}
static void *jit_MODR_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_modr_u(jit,a,b,c);
}
static void *jit_MODI_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_modi_u(jit,a,b,c);
}
static void *jit_ORR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_orr(jit,a,b,c);
}
static void *jit_ORI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_ori(jit,a,b,c);
}
static void *jit_XORR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_xorr(jit,a,b,c);
}
static void *jit_XORI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_xori(jit,a,b,c);
}
static void *jit_LSHR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_lshr(jit,a,b,c);
}
static void *jit_LSHI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_lshi(jit,a,b,c);
}
static void *jit_RSHR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_rshr(jit,a,b,c);
}
static void *jit_RSHI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_rshi(jit,a,b,c);
}
static void *jit_RSHR_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_rshr_u(jit,a,b,c);
}
static void *jit_RSHI_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_rshi_u(jit,a,b,c);
}
static void *jit_NOTR(void*jit,int64_t a,int64_t b) {
    return jit_notr(jit,a,b);
}
static void *jit_BLTR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_bltr(jit,a,b,c);
}
static void *jit_BLTI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_blti(jit,a,b,c);
}
static void *jit_BLTR_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_bltr_u(jit,a,b,c);
}
static void *jit_BLTI_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_blti_u(jit,a,b,c);
}

static void *jit_BLER(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_bler(jit,a,b,c);
}
static void *jit_BLEI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_blei(jit,a,b,c);
}
static void *jit_BLER_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_bler_u(jit,a,b,c);
}
static void *jit_BLEI_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_blei_u(jit,a,b,c);
}

static void *jit_BGTR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_bgtr(jit,a,b,c);
}
static void *jit_BGTI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_bgti(jit,a,b,c);
}
static void *jit_BGTR_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_bgtr_u(jit,a,b,c);
}
static void *jit_BGTI_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_bgti_u(jit,a,b,c);
}

static void *jit_BGER(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_bger(jit,a,b,c);
}
static void *jit_BGEI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_bgei(jit,a,b,c);
}
static void *jit_BGER_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_bger_u(jit,a,b,c);
}
static void *jit_BGEI_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_bgei_u(jit,a,b,c);
}

static void*jit_BEQR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_beqr(jit,a,b,c);
}
static void*jit_BEQI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_beqi(jit,a,b,c);
}

static void*jit_BNER(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_bner(jit,a,b,c);
}
static void*jit_BNEI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_bnei(jit,a,b,c);
}

static void *jit_LTR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_ltr(jit,a,b,c);
}
static void *jit_LTI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_lti(jit,a,b,c);
}
static void *jit_LTR_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_ltr_u(jit,a,b,c);
}
static void *jit_LTI_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_lti_u(jit,a,b,c);
}

static void *jit_GTR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_gtr(jit,a,b,c);
}
static void *jit_GTI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_gti(jit,a,b,c);
}
static void *jit_GTR_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_gtr_u(jit,a,b,c);
}
static void *jit_GTI_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_gti_u(jit,a,b,c);
}

static void *jit_GER(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_ger(jit,a,b,c);
}
static void *jit_GEI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_gei(jit,a,b,c);
}
static void *jit_GER_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_ger_u(jit,a,b,c);
}
static void *jit_GEI_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_gei_u(jit,a,b,c);
}

static void *jit_LER(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_ler(jit,a,b,c);
}
static void *jit_LEI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_lei(jit,a,b,c);
}
static void *jit_LER_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_ler_u(jit,a,b,c);
}
static void *jit_LEI_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_lei_u(jit,a,b,c);
}

static void *jit_EQR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_eqr(jit,a,b,c);
}
static void *jit_EQI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_eqi(jit,a,b,c);
}

static void *jit_NER(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_ner(jit,a,b,c);
}
static void *jit_NEI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_nei(jit,a,b,c);
}

static void* jit_LDR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_ldr(jit,a,b,c);
}
static void* jit_LDI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_ldi(jit,a,b,c);
}
static void* jit_LDXR(void *jit,int64_t a,int64_t b,int64_t c,int64_t d) {
    return jit_ldxr(jit,a,b,c,d);
}
static void* jit_LDXI(void *jit,int64_t a,int64_t b,int64_t c,int64_t d) {
    return jit_ldxi(jit,a,b,c,d);
}

static void* jit_LDR_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_ldr_u(jit,a,b,c);
}
static void* jit_LDI_U(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_ldi_u(jit,a,b,c);
}
static void* jit_LDXR_U(void *jit,int64_t a,int64_t b,int64_t c,int64_t d) {
    return jit_ldxr_u(jit,a,b,c,d);
}
static void* jit_LDXI_U(void *jit,int64_t a,int64_t b,int64_t c,int64_t d) {
    return jit_ldxi_u(jit,a,b,c,d);
}

static void* jit_STR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_str(jit,a,b,c);
}
static void* jit_STI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_sti(jit,a,b,c);
}

static void* jit_STXR(void *jit,int64_t a,int64_t b,int64_t c,int64_t d) {
    return jit_stxr(jit,a,b,c,d);
}
static void* jit_STXI(void *jit,int64_t a,int64_t b,int64_t c,int64_t d) {
    return jit_stxi(jit,a,b,c,d);
}

static void* jit_FMOVR(void *jit,int64_t a,int64_t b) {
    return jit_fmovr(jit,a,b);
}
static void* jit_FMOVI(void *jit,int64_t a,double b) {
    return jit_fmovi(jit,a,b);
}

static void* jit_FADDR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_faddr(jit,a,b,c);
}
static void* jit_FADDI(void *jit,int64_t a,int64_t b,double c) {
    return jit_faddi(jit,a,b,c);
}

static void* jit_FSUBR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_fsubr(jit,a,b,c);
}
static void* jit_FSUBI(void *jit,int64_t a,int64_t b,double c) {
    return jit_fsubi(jit,a,b,c);
}

static void* jit_FMULR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_fmulr(jit,a,b,c);
}
static void* jit_FMULI(void *jit,int64_t a,int64_t b,double c) {
    return jit_fmuli(jit,a,b,c);
}

static void* jit_FDIVR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_fdivr(jit,a,b,c);
}
static void* jit_FDIVI(void *jit,int64_t a,int64_t b,double c) {
    return jit_fdivi(jit,a,b,c);
}

static void* jit_FNEGR(void *jit,int64_t a,int64_t b) {
    return jit_fnegr(jit,a,b);
}

static void* jit_EXTR(void *jit,int64_t a,int64_t b) {
    return jit_extr(jit,a,b);
}
static void* jit_TRUNCR(void *jit,int64_t a,int64_t b) {
    return jit_truncr(jit,a,b);
}
static void* jit_FLOORR(void *jit,int64_t a,int64_t b) {
    return jit_floorr(jit,a,b);
}
static void* jit_CEILR(void *jit,int64_t a,int64_t b) {
    return jit_ceilr(jit,a,b);
}
static void* jit_ROUNDR(void *jit,int64_t a,int64_t b) {
    return jit_roundr(jit,a,b);
}

static void* jit_FBLTR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_fbltr(jit,a,b,c);
}
static void* jit_FBLTI(void *jit,int64_t a,int64_t b,double c) {
    return jit_fblti(jit,a,b,c);
}
static void* jit_FBGTR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_fbgtr(jit,a,b,c);
}
static void* jit_FBGTI(void *jit,int64_t a,int64_t b,double c) {
    return jit_fbgti(jit,a,b,c);
}

static void* jit_FBLER(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_fbler(jit,a,b,c);
}
static void* jit_FBLEI(void *jit,int64_t a,int64_t b,double c) {
    return jit_fblei(jit,a,b,c);
}
static void* jit_FBGER(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_fbger(jit,a,b,c);
}
static void* jit_FBGEI(void *jit,int64_t a,int64_t b,double c) {
    return jit_fbgei(jit,a,b,c);
}

static void* jit_FBEQR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_fbeqr(jit,a,b,c);
}
static void* jit_FBEQI(void *jit,int64_t a,int64_t b,double c) {
    return jit_fbeqi(jit,a,b,c);
}

static void* jit_FBNER(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_fbner(jit,a,b,c);
}
static void* jit_FBNEI(void *jit,int64_t a,int64_t b,double c) {
    return jit_fbnei(jit,a,b,c);
}

static void* jit_FSTR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_fstr(jit,a,b,c);
}
static void* jit_FSTI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_fsti(jit,a,b,c);
}
static void* jit_FSTXR(void *jit,int64_t a,int64_t b,int64_t c,int64_t d) {
    return jit_fstxr(jit,a,b,c,d);
}
static void* jit_FSTXI(void *jit,int64_t a,int64_t b,int64_t c,int64_t d) {
    return jit_fstxi(jit,a,b,c,d);
}

static void* jit_FLDR(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_fldr(jit,a,b,c);
}
static void* jit_FLDI(void *jit,int64_t a,int64_t b,int64_t c) {
    return jit_fldi(jit,a,b,c);
}

static void* jit_FLDXR(void *jit,int64_t a,int64_t b,int64_t c,int64_t d) {
    return jit_fldxr(jit,a,b,c,d);
}
static void* jit_FLDXI(void *jit,int64_t a,int64_t b,int64_t c,int64_t d) {
    return jit_fldxi(jit,a,b,c,d);
}

static void* jit_FPUTARGR(void *jit,int64_t a,int64_t b) {
    return jit_fputargr(jit,a,b);
}
static void* jit_FPUTARGI(void *jit,double a,int64_t b) {
    return jit_fputargi(jit,a,b);
}

static void* jit_FRETR(void *jit,int64_t a,int64_t b) {
    return jit_fretr(jit,a,b);
}
static void* jit_FRETI(void *jit,double a,int64_t b) {
    return jit_freti(jit,a,b);
}
static void* jit_FRETVAL(void *jit,int64_t a,int64_t b) {
    return jit_fretval(jit,a,b);
}

static void *jit_REF_CODE(void *jit,int64_t a,int64_t b) {
    return jit_ref_code(jit,a,b);
}
static void *jit_REF_DATA(void *jit,int64_t a,int64_t b) {
    return jit_ref_data(jit,a,b);
}
static void *jit_TAINT_LABEL(void *jit,int64_t a) {
    return jit_taint_label(jit,a);
}
static void *jit_END_ASM_BLK(void *jit) {
    return jit_end_asm_blk(jit);
}
static void *jit_CODE_ALIGN(void *jit,int64_t a) {
    return jit_code_align(jit,a);
}
static void *jit_CODE_ALIGN_FILL(void *jit,int64_t a,int64_t b) {
    return jit_align_fill(jit,a,b);
}
static void *jit_DATA_STR(void *jit,void *a) {
    jit_data_str(jit,a);
    return NULL;
}
static void *jit_DATA_BYTE(void *jit,int64_t a) {
    return jit_data_byte(jit,a);
}
static void *jit_DATA_WORD(void *jit,int64_t a) {
    jit_data_word(jit,a);
    return NULL;
}
static void *jit_DATA_DWORD(void *jit,int64_t a) {
    jit_data_dword(jit,a);
    return NULL;
}
static void *jit_DATA_QWORD(void *jit,int64_t a) {
    jit_data_qword(jit,a);
    return NULL;
}

static void *jit_DATA_REF_CODE(void *jit,int64_t a) {
    return jit_data_ref_code(jit,a);
}
static void *jit_DATA_REF_DATA(void *jit,int64_t a) {
    return jit_data_ref_data(jit,a);
}
static void *jit_DUMP_PTR(void *jit,void *a) {
    return jit_dump_ptr(jit,a);
}
static void *jit_RELOCATION(void *jit,int64_t a,void *b) {
    return jit_relocation(jit,a,b);
}
static int64_t jit_ALLOCAI(void *jit,int64_t a) {
    return jit_allocai(jit,a);
}
TOS_Fs Fs;
void *GetFs() {
    return &Fs;
}
ExceptFrame *curframe;
ExceptFrame *EnterTry() {
    ExceptFrame *new=TD_MALLOC(sizeof(ExceptFrame));
    new->parent=curframe;
    curframe=new;
    return new;
}
void PopTryFrame() {
    ExceptFrame *c=curframe;
    ExceptFrame *par=curframe->parent;
    TD_FREE(curframe);
    curframe=par;
}
static void GCollect() {
    int old=PoopSetGCEnable(1);
    PoopCollect();
    PoopSetGCEnable(old);
}
void throw(uint64_t val) {
    if(!curframe)
      exit(0);
    Fs.except_ch=val;
    Fs.catch_except=0;
    ExceptFrame old=*curframe;
    PopTryFrame();
    if(!curframe) {
      fail:;
      /*
      #ifdef BOOTSTRAPED
      char code[9];
      *(uint64_t*)code=Fs.except_ch;
      code[8]=0;
      fprintf(stderr,"Uncaught exception('%s'):\n",code);
      Backtrace(-1,1);
      #endif
      */
    }
    HCLongJmp(old.pad);
}
/**
 * If Fs.catch_except is not set,it will throw again
 */
static void ExitCatch() {
    if(!Fs.catch_except) {
        if(!curframe) {
          #ifdef BOOTSTRAPED
          char code[9];
          *(uint64_t*)code=Fs.except_ch;
          code[8]=0;
          fprintf(stderr,"Uncaught exception('%s'):\n",code);
          Backtrace(-1,1);
          #endif
        }
        ExceptFrame old=*curframe;
        PopTryFrame();
        HCLongJmp(old.pad);
    }
}
static int64_t BFFS(int64_t v) {
    return __builtin_ffsl(v);
}
static int64_t BCLZ(int64_t v) {
    return __builtin_clzl(v);
}
static uint64_t PowU64(uint64_t x,uint64_t n) {
    if(n==0) return 1;
    uint64_t y=1;
    while(n>1) {
        if(!(n%2)) {
            x=x*x;
            n=n/2;
        } else {
            y=x*y;
            x=x*x;
            n=(n-1)/2;
        }
    }
    return x*y;
}
static int64_t PowI64(int64_t x,int64_t n) {
    if(n<0) return 0;
    if(n==0) return 1;
    int64_t y=1;
    while(n>1) {
        if(!(n%2)) {
            x=x*x;
            n=n/2;
        } else {
            y=x*y;
            x=x*x;
            n=(n-1)/2;
        }
    }
    return x*y;
}
static char *__GetStr(char*txt) {
    return rl(txt);
}
static void *MemNCpy(void *d,void *s,long sz) {
    return memcpy(d,s,sz);
}
static int64_t __Move(char *old,char *new) {
    return 0==rename(old, new);
}
static int64_t Cd(char *dir) {
    return 0==chdir(dir);
}
static char *DirCur(char *dir) {
    char buffer[1024];
    getcwd(buffer,1024);
    return strdup(buffer);
}
static int64_t DirMk(char *dir) {
#ifndef TARGET_WIN32
    return 0==mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#else
    return 0==CreateDirectoryA(dir,NULL);
#endif
}
static char *FileNameAbs(char *fn) {

#ifndef TARGET_WIN32
    char *d=realpath(fn,NULL);
    if(!d) return NULL;
    char *ret=strdup(d);
    free(d);
    return ret;
#else
    char buf[1024];
    if(GetFileAttributesA(fn)==INVALID_FILE_ATTRIBUTES)
      return NULL;
    GetFullPathNameA(fn,1024,buf,NULL);
    return strdup(buf);
#endif
}
static int64_t IsDir(char *fn) {
    struct stat buf;
    stat(fn, &buf);
    return S_ISDIR(buf.st_mode);
}
static int64_t FileWrite(char *fn,void *data,int64_t sz) {
    FILE *f=fopen(fn,"wb");
    if(!f) return 0;
    fwrite(data, 1, sz, f);
    fclose(f);
    return 1;
}
void* FileRead(char *fn,int64_t *sz) {
    FILE *f=fopen(fn,"rb");
    if(!f) return NULL;
    fseek(f,0,SEEK_END);
    size_t len=ftell(f);
    fseek(f,0, SEEK_SET);
    len-=ftell(f);
    void *data=TD_MALLOC(len+1);
    fread(data, 1, len, f);
    fclose(f);
    if(sz) *sz=len;
    return data;
}
static char **__Dir(char *fn) {
    DIR *dir=opendir(fn);
    if(!dir) return NULL;
    struct dirent *ent;
    vec_str_t items;
    vec_init(&items);
    while(ent=readdir(dir))
        vec_push(&items,strdup(ent->d_name));
    vec_push(&items,NULL);
    return items.data;
}
static int64_t IsWindows() {
#ifdef TARGET_WIN32
    return 1;
#else
    return 0;
#endif
}
static int64_t IsMac() {
#ifdef MACOS
    return 1;
#else
    return 0;
#endif
}
#ifdef TARGET_WIN32
static void EscapePathCat(char *buffer,char *path,DWORD  buf_sz) {
#else
static void EscapePathCat(char *buffer,char *path,size_t buf_sz) {
#endif
#ifdef TARGET_WIN32
  char spaced[2048];
  strcpy(spaced,path);
  PathQuoteSpaces(spaced);
  strcpy(buffer+strlen(buffer),spaced);
  #else
  //TODO escape for unix paths
  strcpy(buffer+strlen(buffer),path);
  #endif
}
void CreateTagsAndErrorsFiles(char *tags,char *errs,char *root) {
  char buffer[2048];
  buffer[0]=0;
  #ifdef TARGET_WIN32
  #else
  EscapePathCat(buffer,CompilerPath,sizeof(buffer));
  #endif
  if(tags) {
        sprintf(buffer+strlen(buffer)," -t ");
        EscapePathCat(buffer,tags,sizeof(buffer));
  }
  if(errs) {
        sprintf(buffer+strlen(buffer)," -e ");
        EscapePathCat(buffer,errs,sizeof(buffer));
  }
  if(root) {
      sprintf(buffer+strlen(buffer)," ");
      EscapePathCat(buffer,root,sizeof(buffer));
  }
  #ifdef TARGET_WIN32
  //https://www.codeproject.com/Articles/1842/A-newbie-s-elementary-guide-to-spawning-processes
  SHELLEXECUTEINFO ShExecInfo = {0};
  ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
  ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
  ShExecInfo.hwnd = NULL;
  ShExecInfo.lpVerb = NULL;
  ShExecInfo.lpFile = CompilerPath;
  ShExecInfo.lpParameters = buffer;
  ShExecInfo.lpDirectory = NULL;
  ShExecInfo.nShow = SW_HIDE;
  ShExecInfo.hInstApp = NULL;
  ShellExecuteEx(&ShExecInfo);
  WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
  CloseHandle(ShExecInfo.hProcess);
  #else
  system(buffer);
  #endif
}
static void Test(int64_t a,int64_t b,int64_t c,int64_t d,int64_t e,int64_t f,int64_t  g) {
  printf("%lld,%lld,%lld,%lld,%lld,%lld,%lld\n",a,b,c,d,e,f,g);
}
//https://docs.microsoft.com/en-us/cpp/build/x64-calling-convention?view=msvc-160
static void TestMixed(int a, double b, int c, double d, int e, double f) {
  printf("%d,%lf,%d,%lf,%d,%lf\n",a,b,c,d,e,f);
}
static char *hc_SDL_GetWindowTitle(SDL_Window *win) {
    return strdup(SDL_GetWindowTitle(win));
}
static char *hc_SDL_GetClipboardText() {
    return strdup(SDL_GetClipboardText());
}
static int64_t GetSurfaceW(SDL_Surface *s) {
    return s->w;
}
static int64_t GetSurfaceH(SDL_Surface *s) {
    return s->h;
}
static double Bit4BitU64ToF64(uint64_t b) {
    union {double f;int64_t i;} val;
    val.i=b;
    return val.f;
}
static uint64_t Bit4BitF64ToU64(double b) {
    union {double f;int64_t i;} val;
    val.f=b;
    return val.i;
}
static double F64And(double a,double b) {
    return (*(uint64_t*)&a)&(*(uint64_t*)&b);
}
static double F64Or(double a,double b) {
    return (*(uint64_t*)&a)&(*(uint64_t*)&b);
}
static double F64Xor(double a,double b) {
    return (*(uint64_t*)&a)&(*(uint64_t*)&b);
}
static double F64Shl(double a,int64_t b) {
    return (*(uint64_t*)&a)<<b;
}
static double F64Shr(double a,int64_t b) {
    return (*(uint64_t*)&a)>>b;
}
static void ForeachFunc(void(*func)(const char *name,void *ptr,long sz)) {
  map_iter_t iter=map_iter(&Loader.symbols);
  const char *key;
  while(key=map_next(&Loader.symbols,&iter)) {
    CSymbol *var=map_get(&Loader.symbols, key);
    if(!var->is_importable)
        continue;
    func(key,var->value_ptr,var->size);
  }
}
static void jit_DATA_BYTES(struct jit *j,int64_t count,void *d) {
  jit_data_bytes(j,count,d);
}
static void RegisterFunctionPtr(char *name,void *fptr) {
    CSymbol sym;
    sym.type=SYM_FUNC;
    sym.value_ptr=fptr;
    sym.is_importable=1;
    map_set(&Loader.symbols, name, sym);
}
static double Log2(double c) {
    return log(c)/log(2);
}
#ifndef TARGET_WIN32
void UnblockSignals() {
    sigset_t set;
    sigfillset(&set);
    sigprocmask(SIG_UNBLOCK,&set,NULL);
}
#else
void UnblockSignals() {
}
#endif
char *GetBuiltinMacrosText();
void RegisterFuncPtrs() {
    RegisterFunctionPtr("RegisterRuntimeClasses",RegisterRuntimeClasses);
    RegisterFunctionPtr("UnblockSignals",UnblockSignals);
    RegisterFunctionPtr("signal",signal);
    RegisterFunctionPtr("IsDir",IsDir);
    RegisterFunctionPtr("FileRead",FileRead);
    RegisterFunctionPtr("FileWrite",FileWrite);
    RegisterFunctionPtr("jit_breakpoint",jit_BREAKPOINT);
    RegisterFunctionPtr("jit_putargi",jit_PUTARGI);
    RegisterFunctionPtr("jit_dump_ptr",jit_DUMP_PTR);
    RegisterFunctionPtr("jit_putargr",jit_PUTARGR);
    RegisterFunctionPtr("jit_get_breakpoint_by_ptr",jit_get_breakpoint_btr_ptr);
    RegisterFunctionPtr("jit_free",jit_free);
    RegisterFunctionPtr("jit_BREAKPOINT",jit_BREAKPOINT);
    RegisterFunctionPtr("jit_debugger_get_reg_ptr",jit_debugger_get_reg_ptr);
    RegisterFunctionPtr("jit_debugger_get_vreg_ptr_from_parent",jit_debugger_get_vreg_ptr_from_parent);
    RegisterFunctionPtr("jit_get_breakpoint_btr_ptr",jit_get_breakpoint_btr_ptr);
    RegisterFunctionPtr("jit_disable_optimization",jit_disable_optimization);
    RegisterFunctionPtr("jit_enable_optimization",jit_enable_optimization);
    RegisterFunctionPtr("ForeachFuncInTable",ForeachFunc);//
    RegisterFunctionPtr("jit_init",jit_INIT);
    RegisterFunctionPtr("jit_dump_ops",jit_DUMP_OPS);
    RegisterFunctionPtr("jit_generate_code",jit_GENERATE_CODE);
    RegisterFunctionPtr("jit_andr",jit_ANDR);
    RegisterFunctionPtr("jit_andi",jit_ANDI);
    RegisterFunctionPtr("R_FP",jit_R_FP);
    RegisterFunctionPtr("jit_bin_size",jit_BINSIZE);
    RegisterFunctionPtr("jit_FR",jit_FR);
    RegisterFunctionPtr("jit_R",jit_R);
    RegisterFunctionPtr("jit_allocai",jit_ALLOCAI);
    RegisterFunctionPtr("jit_get_label",jit_GET_LABEL);
    RegisterFunctionPtr("jit_prolog",jit_PROLOG);
    RegisterFunctionPtr("jit_movr",jit_MOVR);
    RegisterFunctionPtr("jit_movi",jit_MOVI);
    RegisterFunctionPtr("jit_jmpr",jit_JMPR);
    RegisterFunctionPtr("jit_jmpi",jit_JMPI);
    RegisterFunctionPtr("jit_patch",jit_PATCH);
    RegisterFunctionPtr("jit_prepare",jit_PREPARE);
    RegisterFunctionPtr("jit_call",jit_CALL);
    RegisterFunctionPtr("jit_callr",jit_CALLR);
    RegisterFunctionPtr("jit_reti",jit_RETI);
    RegisterFunctionPtr("jit_retr",jit_RETR);
    RegisterFunctionPtr("jit_retval",jit_RETVAL);
    RegisterFunctionPtr("jit_getarg",jit_GETARG);
    RegisterFunctionPtr("jit_declare_arg",jit_DECLARE_ARG);
    RegisterFunctionPtr("jit_addr",jit_ADDR);
    RegisterFunctionPtr("jit_addi",jit_ADDI);
    RegisterFunctionPtr("jit_subr",jit_SUBR);
    RegisterFunctionPtr("jit_subi",jit_SUBI);
    RegisterFunctionPtr("jit_mulr",jit_MULR);
    RegisterFunctionPtr("jit_muli",jit_MULI);
    RegisterFunctionPtr("jit_mulr_u",jit_MULR_U);
    RegisterFunctionPtr("jit_muli_u",jit_MULI_U);
    RegisterFunctionPtr("jit_divr",jit_DIVR);
    RegisterFunctionPtr("jit_divi",jit_DIVI);
    RegisterFunctionPtr("jit_divr_u",jit_DIVR_U);
    RegisterFunctionPtr("jit_divi_u",jit_DIVI_U);
    RegisterFunctionPtr("jit_modr",jit_MODR);
    RegisterFunctionPtr("jit_modi",jit_MODI);
    RegisterFunctionPtr("jit_modr_u",jit_MODR_U);
    RegisterFunctionPtr("jit_modi_u",jit_MODI_U);
    RegisterFunctionPtr("jit_orr",jit_ORR);
    RegisterFunctionPtr("jit_ori",jit_ORI);
    RegisterFunctionPtr("jit_xorr",jit_XORR);
    RegisterFunctionPtr("jit_xori",jit_XORI);
    RegisterFunctionPtr("jit_lshr",jit_LSHR);
    RegisterFunctionPtr("jit_lshi",jit_LSHI);
    RegisterFunctionPtr("jit_rshr",jit_RSHR);
    RegisterFunctionPtr("jit_rshi",jit_RSHI);
    RegisterFunctionPtr("jit_rshr_u",jit_RSHR_U);
    RegisterFunctionPtr("jit_rshi_u",jit_RSHI_U);
    RegisterFunctionPtr("jit_notr",jit_NOTR);
    RegisterFunctionPtr("jit_bltr",jit_BLTR);
    RegisterFunctionPtr("jit_blti",jit_BLTI);
    RegisterFunctionPtr("jit_bltr_u",jit_BLTR_U);
    RegisterFunctionPtr("jit_blti_u",jit_BLTI_U);
    RegisterFunctionPtr("jit_bler",jit_BLER);
    RegisterFunctionPtr("jit_blei",jit_BLEI);
    RegisterFunctionPtr("jit_bler_u",jit_BLER_U);
    RegisterFunctionPtr("jit_blei_u",jit_BLEI_U);
    RegisterFunctionPtr("jit_bger",jit_BGER);
    RegisterFunctionPtr("jit_bgei",jit_BGEI);
    RegisterFunctionPtr("jit_bger_u",jit_BGER_U);
    RegisterFunctionPtr("jit_bgei_u",jit_BGEI_U);
    RegisterFunctionPtr("jit_bgtr",jit_BGTR);
    RegisterFunctionPtr("jit_bgti",jit_BGTI);
    RegisterFunctionPtr("jit_bgtr_u",jit_BGTR_U);
    RegisterFunctionPtr("jit_bgti_u",jit_BGTI_U);
    RegisterFunctionPtr("jit_beqr",jit_BEQR);
    RegisterFunctionPtr("jit_beqi",jit_BEQI);
    RegisterFunctionPtr("jit_bner",jit_BNER);
    RegisterFunctionPtr("jit_bnei",jit_BNEI);

    RegisterFunctionPtr("jit_ltr",jit_LTR);
    RegisterFunctionPtr("jit_lti",jit_LTI);
    RegisterFunctionPtr("jit_ltr_u",jit_LTR_U);
    RegisterFunctionPtr("jit_lti_u",jit_LTI_U);
    RegisterFunctionPtr("jit_ler",jit_LER);
    RegisterFunctionPtr("jit_lei",jit_LEI);
    RegisterFunctionPtr("jit_ger_u",jit_GER_U);
    RegisterFunctionPtr("jit_gei_u",jit_GEI_U);
    RegisterFunctionPtr("jit_ger",jit_GER);
    RegisterFunctionPtr("jit_gei",jit_GEI);
    RegisterFunctionPtr("jit_ger_u",jit_GER_U);
    RegisterFunctionPtr("jit_gei_u",jit_GEI_U);
    RegisterFunctionPtr("jit_gtr",jit_GTR);
    RegisterFunctionPtr("jit_gti",jit_GTI);
    RegisterFunctionPtr("jit_gtr_u",jit_GTR_U);
    RegisterFunctionPtr("jit_gti_u",jit_GTI_U);
    RegisterFunctionPtr("jit_eqr",jit_EQR);
    RegisterFunctionPtr("jit_eqi",jit_EQI);
    RegisterFunctionPtr("jit_ner",jit_NER);
    RegisterFunctionPtr("jit_nei",jit_NEI);
    RegisterFunctionPtr("jit_ldr",jit_LDR);
    RegisterFunctionPtr("jit_ldi",jit_LDI);
    RegisterFunctionPtr("jit_ldxr",jit_LDXR);
    RegisterFunctionPtr("jit_ldxi",jit_LDXI);
    RegisterFunctionPtr("jit_ldr_u",jit_LDR_U);
    RegisterFunctionPtr("jit_ldi_u",jit_LDI_U);
    RegisterFunctionPtr("jit_ldxr_u",jit_LDXR_U);
    RegisterFunctionPtr("jit_ldxi_u",jit_LDXI_U);
    RegisterFunctionPtr("jit_str",jit_STR);
    RegisterFunctionPtr("jit_sti",jit_STI);
    RegisterFunctionPtr("jit_stxr",jit_STXR);
    RegisterFunctionPtr("jit_stxi",jit_STXI);
    RegisterFunctionPtr("jit_fmovi",jit_FMOVI);
    RegisterFunctionPtr("jit_fmovr",jit_FMOVR);
    RegisterFunctionPtr("jit_faddr",jit_FADDR);
    RegisterFunctionPtr("jit_faddi",jit_FADDI);
    RegisterFunctionPtr("jit_fsubi",jit_FSUBI);
    RegisterFunctionPtr("jit_fsubr",jit_FSUBR);
    RegisterFunctionPtr("jit_fmulr",jit_FMULR);
    RegisterFunctionPtr("jit_fmuli",jit_FMULI);
    RegisterFunctionPtr("jit_fdivi",jit_FDIVI);
    RegisterFunctionPtr("jit_fdivr",jit_FDIVR);
    RegisterFunctionPtr("jit_fnegr",jit_FNEGR);
    RegisterFunctionPtr("jit_negr",jit_NEGR);
    RegisterFunctionPtr("jit_extr",jit_EXTR);
    RegisterFunctionPtr("jit_truncr",jit_TRUNCR);
    RegisterFunctionPtr("jit_floorr",jit_FLOORR);
    RegisterFunctionPtr("jit_ceilr",jit_CEILR);
    RegisterFunctionPtr("jit_roundr",jit_ROUNDR);
    RegisterFunctionPtr("jit_fbltr",jit_FBLTR);
    RegisterFunctionPtr("jit_fbgtr",jit_FBGTR);
    RegisterFunctionPtr("jit_fbler",jit_FBLER);
    RegisterFunctionPtr("jit_fbger",jit_FBGER);
    RegisterFunctionPtr("jit_fbeqr",jit_FBEQR);
    RegisterFunctionPtr("jit_fbner",jit_FBNER);
    RegisterFunctionPtr("jit_fblti",jit_FBLTI);
    RegisterFunctionPtr("jit_fbgti",jit_FBGTI);
    RegisterFunctionPtr("jit_fblei",jit_FBLEI);
    RegisterFunctionPtr("jit_fbgei",jit_FBGEI);
    RegisterFunctionPtr("jit_fbeqi",jit_FBEQI);
    RegisterFunctionPtr("jit_fbnei",jit_FBNEI);
    RegisterFunctionPtr("jit_fstr",jit_FSTR);
    RegisterFunctionPtr("jit_fsti",jit_FSTI);
    RegisterFunctionPtr("jit_fstxr",jit_FSTXR);
    RegisterFunctionPtr("jit_fstxi",jit_FSTXI);
    RegisterFunctionPtr("jit_fldr",jit_FLDR);
    RegisterFunctionPtr("jit_fldi",jit_FLDI);
    RegisterFunctionPtr("jit_fldxr",jit_FLDXR);
    RegisterFunctionPtr("jit_fldxi",jit_FLDXI);
    RegisterFunctionPtr("jit_fputargi",jit_FPUTARGI);
    RegisterFunctionPtr("jit_fputargr",jit_FPUTARGR);
    RegisterFunctionPtr("jit_freti",jit_FRETI);
    RegisterFunctionPtr("jit_fretr",jit_FRETR);
    RegisterFunctionPtr("jit_fretval",jit_FRETVAL);
    RegisterFunctionPtr("jit_ref_code",jit_REF_CODE);
    RegisterFunctionPtr("jit_ref_data",jit_REF_DATA);
    RegisterFunctionPtr("jit_taint_label",jit_TAINT_LABEL);
    RegisterFunctionPtr("jit_end_asm_blk",jit_END_ASM_BLK);
    RegisterFunctionPtr("jit_code_align",jit_CODE_ALIGN);
    RegisterFunctionPtr("jit_code_align_fill",jit_CODE_ALIGN_FILL);
    RegisterFunctionPtr("jit_data_str",jit_DATA_STR);
    RegisterFunctionPtr("jit_data_byte",jit_DATA_BYTE);
    RegisterFunctionPtr("jit_data_word",jit_DATA_WORD);
    RegisterFunctionPtr("jit_data_dword",jit_DATA_DWORD);
    RegisterFunctionPtr("jit_data_qword",jit_DATA_QWORD);
    RegisterFunctionPtr("jit_data_ref_code",jit_DATA_REF_CODE);
    RegisterFunctionPtr("jit_data_ref_data",jit_DATA_REF_DATA);
    RegisterFunctionPtr("jit_relocation",jit_RELOCATION);
    RegisterFunctionPtr("jit_data_bytes",jit_DATA_BYTES);
    RegisterFunctionPtr("HCSetJmp",HCSetJmp);
    RegisterFunctionPtr("PopTryFrame",PopTryFrame);
    RegisterFunctionPtr("EnterTry",EnterTry);
    RegisterFunctionPtr("ExitCatch",ExitCatch);
    RegisterFunctionPtr("TOSPrint",TOSPrint);
    RegisterFunctionPtr("PowI64",PowI64);
    RegisterFunctionPtr("PowU64",PowU64);
    RegisterFunctionPtr("FMod",fmod);
    RegisterFunctionPtr("WhineOnOutOfBounds",InBounds);
    RegisterFunctionPtr("Bit4BitU64ToF64",Bit4BitU64ToF64);
    RegisterFunctionPtr("Bit4BitF64ToU64",Bit4BitF64ToU64);
    RegisterFunctionPtr("F64And",F64And);
    RegisterFunctionPtr("F64Xor",F64Xor);
    RegisterFunctionPtr("F64Shr",F64Shr);
    RegisterFunctionPtr("F64Shl",F64Shl);
    RegisterFunctionPtr("Pow",pow);
    RegisterFunctionPtr("PoopSetGCEnable",PoopSetGCEnable);
    RegisterFunctionPtr("GC_Collect",GCollect);
    RegisterFunctionPtr("CreateTagsAndErrorsFiles",CreateTagsAndErrorsFiles);
    RegisterFunctionPtr("MSize",MSize);
    RegisterFunctionPtr("Bsf",BFFS);
    RegisterFunctionPtr("Bsr",BCLZ);
    RegisterFunctionPtr("throw",throw);
    RegisterFunctionPtr("MAlloc",PoopMAlloc);
    RegisterFunctionPtr("Free",PoopFree);
    RegisterFunctionPtr("MemCpy",MemNCpy);
    RegisterFunctionPtr("MemNCpy",MemNCpy);
    RegisterFunctionPtr("StrLen",strlen);
    RegisterFunctionPtr("StrCmp",strcmp);
    RegisterFunctionPtr("StrNCmp",strncmp);
    RegisterFunctionPtr("StrCpy",strcpy);
    RegisterFunctionPtr("StrNCpy",strncpy);
    RegisterFunctionPtr("StrMatch",strstr);
    RegisterFunctionPtr("Fs",GetFs);
    RegisterFunctionPtr("__GetStr",__GetStr);
    RegisterFunctionPtr("ATan",atan);
    RegisterFunctionPtr("Abs",fabs);
    RegisterFunctionPtr("Cos",cos);
    RegisterFunctionPtr("Sin",sin);
    RegisterFunctionPtr("Sqrt",sqrt);
    RegisterFunctionPtr("Tan",tan);
    RegisterFunctionPtr("Ceil",ceil);
    RegisterFunctionPtr("Floor",floor);
    RegisterFunctionPtr("Ln",log);
    RegisterFunctionPtr("Log10",log10);
    RegisterFunctionPtr("Log2",Log2);
    RegisterFunctionPtr("Round",round);
    RegisterFunctionPtr("Trunc",trunc);
    RegisterFunctionPtr("Exit",exit);
    RegisterFunctionPtr("Cd",Cd);
    RegisterFunctionPtr("DirCur",DirCur);
    RegisterFunctionPtr("DirMk",DirMk);
    RegisterFunctionPtr("__Move",__Move);
    RegisterFunctionPtr("FileNameAbs",FileNameAbs);
    RegisterFunctionPtr("DirNameAbs",FileNameAbs);
    RegisterFunctionPtr("__Dir",__Dir);
    RegisterFunctionPtr("IsWindows",IsWindows);
    RegisterFunctionPtr("IsMac",IsMac);
    RegisterFunctionPtr("MemSet",memset);
    RegisterFunctionPtr("SDL_Init",SDL_Init);
    RegisterFunctionPtr("SDL_WasInit",SDL_WasInit);
    RegisterFunctionPtr("SDL_CreateWindow",SDL_CreateWindow);
    RegisterFunctionPtr("SDL_SetTextureBlendMode",SDL_SetTextureBlendMode);
    RegisterFunctionPtr("SDL_CreateWindowAndRenderer",SDL_CreateWindowAndRenderer);
    RegisterFunctionPtr("SDL_DestroyWindow",SDL_DestroyWindow);
    RegisterFunctionPtr("SDL_DestroyRenderer",SDL_DestroyRenderer);
    RegisterFunctionPtr("SDL_DisableScreenSaver",SDL_DisableScreenSaver);
    RegisterFunctionPtr("SDL_GetGrabbedWindow",SDL_GetGrabbedWindow);
    RegisterFunctionPtr("SDL_GetWindowPosition",SDL_GetWindowPosition);
    RegisterFunctionPtr("SDL_GetWindowMinimumSize",SDL_GetWindowMinimumSize);
    RegisterFunctionPtr("SDL_GetWindowMaximumSize",SDL_GetWindowMaximumSize);
    RegisterFunctionPtr("SDL_GetWindowSize",SDL_GetWindowSize);
    RegisterFunctionPtr("SDL_GetWindowTitle",hc_SDL_GetWindowTitle);
    RegisterFunctionPtr("SDL_HideWindow",SDL_HideWindow);
    RegisterFunctionPtr("SDL_MaximizeWindow",SDL_MaximizeWindow);
    RegisterFunctionPtr("SDL_MinimizeWindow",SDL_MinimizeWindow);
    RegisterFunctionPtr("SDL_SetWindowBordered",SDL_SetWindowBordered);
    RegisterFunctionPtr("SDL_SetWindowFullscreen",SDL_SetWindowFullscreen);
    RegisterFunctionPtr("SDL_SetWindowMaximumSize",SDL_SetWindowMaximumSize);
    RegisterFunctionPtr("SDL_SetWindowMinimumSize",SDL_SetWindowMinimumSize);
    RegisterFunctionPtr("SDL_SetWindowResizable",SDL_SetWindowResizable);
    RegisterFunctionPtr("SDL_SetWindowSize",SDL_SetWindowSize);
    RegisterFunctionPtr("SDL_SetWindowTitle",SDL_SetWindowTitle);
    RegisterFunctionPtr("SDL_ShowWindow",SDL_ShowWindow);
    RegisterFunctionPtr("SDL_CreateRenderer",SDL_CreateRenderer);
    RegisterFunctionPtr("SDL_CreateTexture",SDL_CreateTexture);
    RegisterFunctionPtr("SDL_DestroyRenderer",SDL_DestroyRenderer);
    RegisterFunctionPtr("SDL_GetRenderDrawColor",SDL_GetRenderDrawColor);
    RegisterFunctionPtr("SDL_GetRenderer",SDL_GetRenderer);
    RegisterFunctionPtr("SDL_GetRendererOutputSize",SDL_GetRendererOutputSize);
    RegisterFunctionPtr("SDL_GetRenderTarget",SDL_GetRenderTarget);
    RegisterFunctionPtr("SDL_GetTextureAlphaMod",SDL_GetTextureAlphaMod);
    RegisterFunctionPtr("SDL_GetTextureColorMod",SDL_GetTextureColorMod);
    RegisterFunctionPtr("SDL_RenderClear",SDL_RenderClear);
    RegisterFunctionPtr("SDL_RenderCopy",SDL_RenderCopy);
    RegisterFunctionPtr("SDL_RenderDrawLine",SDL_RenderDrawLine);
    RegisterFunctionPtr("SDL_RenderDrawLines",SDL_RenderDrawLines);
    RegisterFunctionPtr("SDL_RenderDrawPoint",SDL_RenderDrawPoint);
    RegisterFunctionPtr("SDL_RenderDrawPoints",SDL_RenderDrawPoints);
    RegisterFunctionPtr("SDL_RenderDrawRect",SDL_RenderDrawRect);
    RegisterFunctionPtr("SDL_RenderDrawRects",SDL_RenderDrawRects);
    RegisterFunctionPtr("SDL_RenderFillRects",SDL_RenderFillRects);
    RegisterFunctionPtr("SDL_RenderFillRects",SDL_RenderFillRects);
    RegisterFunctionPtr("SDL_RenderGetClipRect",SDL_RenderGetClipRect);
    RegisterFunctionPtr("SDL_RenderPresent",SDL_RenderPresent);
    RegisterFunctionPtr("SDL_RenderSetClipRect",SDL_RenderSetClipRect);
    RegisterFunctionPtr("SDL_SetRenderDrawColor",SDL_SetRenderDrawColor);
    RegisterFunctionPtr("SDL_SetRenderTarget",SDL_SetRenderTarget);
    RegisterFunctionPtr("SDL_SetTextureAlphaMod",SDL_SetTextureAlphaMod);
    RegisterFunctionPtr("SDL_SetTextureColorMod",SDL_SetTextureColorMod);
    RegisterFunctionPtr("SDL_UpdateTexture",SDL_UpdateTexture);
    RegisterFunctionPtr("SDL_QueryTexture",SDL_QueryTexture);
    RegisterFunctionPtr("SDL_GetClipboardText",hc_SDL_GetClipboardText);
    RegisterFunctionPtr("SDL_SetClipboardText",SDL_SetClipboardText);
    RegisterFunctionPtr("SDL_PollEvent",SDL_PollEvent);
    RegisterFunctionPtr("SDL_WaitEvent",SDL_WaitEvent);
    RegisterFunctionPtr("SDL_DestroyRenderer",SDL_DestroyRenderer);
    RegisterFunctionPtr("SDL_DestroyTexture",SDL_DestroyTexture);
    RegisterFunctionPtr("SDL_StartTextInput",SDL_StartTextInput);
    RegisterFunctionPtr("SDL_StopTextInput",SDL_StopTextInput);
    RegisterFunctionPtr("SDL_GetError",SDL_GetError);
    RegisterFunctionPtr("SDL_ClearError",SDL_ClearError);
    RegisterFunctionPtr("SDL_FlushEvent",SDL_FlushEvent);
    RegisterFunctionPtr("SDL_GetSurfaceWidth",GetSurfaceW);
    RegisterFunctionPtr("SDL_GetSurfaceHeight",GetSurfaceH);
    RegisterFunctionPtr("SDL_CreateRGBSurface",SDL_CreateRGBSurface);
    RegisterFunctionPtr("SDL_CreateRGBSurfaceFrom",SDL_CreateRGBSurfaceFrom);
    RegisterFunctionPtr("SDL_UpperBlit",SDL_UpperBlit);
    RegisterFunctionPtr("SDL_FillRect",SDL_FillRect);
    RegisterFunctionPtr("SDL_FillRects",SDL_FillRects);
    RegisterFunctionPtr("SDL_GetClipRect",SDL_GetClipRect);
    RegisterFunctionPtr("SDL_GetColorKey",SDL_GetColorKey);
    RegisterFunctionPtr("SDL_GetSurfaceAlphaMod",SDL_GetSurfaceAlphaMod);
    RegisterFunctionPtr("SDL_GetSurfaceColorMod",SDL_GetSurfaceColorMod);
    RegisterFunctionPtr("SDL_LockSurface",SDL_LockSurface);
    RegisterFunctionPtr("SDL_UnlockSurface",SDL_UnlockSurface);
    RegisterFunctionPtr("SDL_SetClipRect",SDL_SetClipRect);
    RegisterFunctionPtr("SDL_SetColorKey",SDL_SetColorKey);
    RegisterFunctionPtr("SDL_SetSurfaceAlphaMod",SDL_SetSurfaceAlphaMod);
    RegisterFunctionPtr("SDL_SetSurfaceColorMod",SDL_SetSurfaceColorMod);
    RegisterFunctionPtr("SDL_SetSurfaceRLE",SDL_SetSurfaceRLE);
    RegisterFunctionPtr("SDL_BlitSurface",SDL_BlitSurface);
    RegisterFunctionPtr("SDL_BlitScaled",SDL_BlitScaled);
    RegisterFunctionPtr("SDL_FreeSurface",SDL_FreeSurface);
    RegisterFunctionPtr("SDL_UpdateWindowSurface",SDL_UpdateWindowSurface);
    RegisterFunctionPtr("SDL_GetWindowSurface",SDL_GetWindowSurface);
    RegisterFunctionPtr("__GetBuiltinMacrosText",GetBuiltinMacrosText);
}
static void *GetType(void *fp,char *name,long ptr_level) {
    CSymbol *gt;
    if(!fp) {
        gt=map_get(&Loader.symbols,"GetType");
        if(!gt) return NULL;
        fp=gt->value_ptr;
    }
    return ((void*(*)(char *,long))fp)(name,ptr_level);
}
static void *__LoaderCreateTypeFwd(void *fp,char *name,long sz,long align) {
    CSymbol *gt;
    if(!fp) {
        gt=map_get(&Loader.symbols,"LoaderCreateTypeFwd");
        if(!gt) return NULL;
        fp=gt->value_ptr;
    }
    return ((void*(*)(char *,long,long))fp)(name,sz,align);
}
#define LoaderCreateTypeFwd(fp,t) __LoaderCreateTypeFwd(fp,#t,sizeof(t),8);
static void __LoaderAddMember(void *fp,void *dst,void *t,char *name,long off) {
    CSymbol *gt;
    if(!fp) {
        gt=map_get(&Loader.symbols,"LoaderAddMember");
        if(!gt) return NULL;
        fp=gt->value_ptr;
    }
    return ((void(*)(void*,void*,char *,long))fp)(dst,t,name,off);
}
static CType *PrimType(void *fp,int64_t sz) {
    switch(sz) {
    case 1:
        return GetType(fp,"I8i",0);
    case 2:
        return GetType(fp,"I16i",0);
    case 4:
        return GetType(fp,"I32i",0);
    case 8:
        return GetType(fp,"I64i",0);
    default: abort();
    }
}
static CType *LoaderCreateArrayType(void *fp,char *name,int64_t dim) {
    CSymbol *gt;
    if(!fp) {
        gt=map_get(&Loader.symbols,"LoaderCreateArrayType");
        if(!gt) return NULL;
        fp=gt->value_ptr;
    }
    return ((void*(*)(char *,long))fp)(name,dim);
}
#define LoaderAddMember(fp,d,t,ct,mem) __LoaderAddMember(fp,d,t,#mem,offsetof(ct,mem));
#define LoaderAddPrimMember(fp,gt,d,ct,mem) __LoaderAddMember(fp,d,PrimType(gt,sizeof(((ct*)NULL)->mem)),#mem,offsetof(ct,mem));
static void CreateMacroInt(vec_char_t *to,char *name,int64_t value) {
    char buffer[1024];
    sprintf(buffer,"#define %s %ld\n",name,value);
    vec_pusharr(to,buffer,strlen(buffer));
}
vec_char_t CreateMacros() {
    vec_char_t macros;
    vec_init(&macros);
    CreateMacroInt(&macros,"JIT_SIGNED_NUM", JIT_SIGNED_NUM);
    CreateMacroInt(&macros,"JIT_UNSIGNED_NUM", JIT_UNSIGNED_NUM);
    CreateMacroInt(&macros,"JIT_FLOAT_NUM", JIT_FLOAT_NUM);
    CreateMacroInt(&macros,"JIT_PTR", JIT_PTR);
    CreateMacroInt(&macros,"JIT_OPT_ALL",JIT_OPT_ALL);
    CreateMacroInt(&macros,"JIT_OPT_JOIN_ADDMUL",JIT_OPT_JOIN_ADDMUL);
    CreateMacroInt(&macros,"JIT_OPT_OMIT_UNUSED_ASSIGNEMENTS",JIT_OPT_OMIT_UNUSED_ASSIGNEMENTS);
    CreateMacroInt(&macros,"JIT_OPT_OMIT_FRAME_PTR",JIT_OPT_OMIT_FRAME_PTR);
    //SIGNALS
    #ifndef TARGET_WIN32
    CreateMacroInt(&macros,"SIGILL",SIGILL);
    CreateMacroInt(&macros,"SIGABRT",SIGABRT);
    CreateMacroInt(&macros,"SIGBUS",SIGBUS);
    CreateMacroInt(&macros,"SIGFPE",SIGFPE);
    CreateMacroInt(&macros,"SIGKILL",SIGKILL);
    CreateMacroInt(&macros,"SIGSEGV",SIGSEGV);
    CreateMacroInt(&macros,"SIGTERM",SIGTERM);
    CreateMacroInt(&macros,"SIGSTOP",SIGSTOP);
    #endif
    CreateMacroInt(&macros,"SDL_INIT_TIMER",SDL_INIT_TIMER);
    CreateMacroInt(&macros,"SDL_INIT_AUDIO",SDL_INIT_AUDIO);
    CreateMacroInt(&macros,"SDL_INIT_VIDEO",SDL_INIT_VIDEO);
    CreateMacroInt(&macros,"SDL_INIT_EVENTS",SDL_INIT_EVENTS);
    CreateMacroInt(&macros,"SDL_INIT_EVERYTHING",SDL_INIT_EVERYTHING);
    CreateMacroInt(&macros,"SDL_BLENDMODE_NONE",SDL_BLENDMODE_NONE);
    CreateMacroInt(&macros,"SDL_BLENDMODE_BLEND",SDL_BLENDMODE_BLEND);
    CreateMacroInt(&macros,"SDL_BLENDMODE_ADD",SDL_BLENDMODE_ADD);
    CreateMacroInt(&macros,"SDL_BLENDMODE_MOD",SDL_BLENDMODE_MOD);
    CreateMacroInt(&macros,"SDL_WINDOWPOS_UNDEFINED",SDL_WINDOWPOS_UNDEFINED);
    CreateMacroInt(&macros,"SDL_WINDOWPOS_CENTERED",SDL_WINDOWPOS_CENTERED);
    CreateMacroInt(&macros,"SDL_WINDOW_FULLSCREEN",SDL_WINDOW_FULLSCREEN);
    CreateMacroInt(&macros,"SDL_WINDOW_FULLSCREEN_DESKTOP",SDL_WINDOW_FULLSCREEN_DESKTOP);
    CreateMacroInt(&macros,"SDL_WINDOW_RESIZABLE",SDL_WINDOW_RESIZABLE);
    CreateMacroInt(&macros,"SDL_WINDOW_MINIMIZED",SDL_WINDOW_MINIMIZED);
    CreateMacroInt(&macros,"SDL_WINDOW_MAXIMIZED",SDL_WINDOW_MAXIMIZED);
    CreateMacroInt(&macros,"SDL_RENDERER_SOFTWARE",SDL_RENDERER_SOFTWARE);
    CreateMacroInt(&macros,"SDL_RENDERER_ACCELERATED",SDL_RENDERER_ACCELERATED);
    CreateMacroInt(&macros,"SDL_RENDERER_PRESENTVSYNC",SDL_RENDERER_PRESENTVSYNC);
    CreateMacroInt(&macros,"SDL_RENDERER_TARGETTEXTURE",SDL_RENDERER_TARGETTEXTURE);
    CreateMacroInt(&macros,"SDL_TEXTUREACCESS_TARGET",SDL_TEXTUREACCESS_TARGET);
    CreateMacroInt(&macros,"SDL_TEXTUREACCESS_STREAMING",SDL_TEXTUREACCESS_STREAMING);
    CreateMacroInt(&macros,"SDL_TEXTUREACCESS_STATIC",SDL_TEXTUREACCESS_STATIC);
    CreateMacroInt(&macros,"SDL_PIXELFORMAT_RGBA8888",SDL_PIXELFORMAT_RGBA8888);
    CreateMacroInt(&macros,"SDL_WINDOWEVENT",SDL_WINDOWEVENT);
    CreateMacroInt(&macros,"SDL_WINDOWEVENT_RESIZED",SDL_WINDOWEVENT_RESIZED);
    CreateMacroInt(&macros,"SDL_KEYUP",SDL_KEYUP);
    CreateMacroInt(&macros,"SDL_KEYDOWN",SDL_KEYDOWN);
    CreateMacroInt(&macros,"KMOD_LSHIFT",KMOD_LSHIFT);
    CreateMacroInt(&macros,"KMOD_RSHIFT",KMOD_RSHIFT);
    CreateMacroInt(&macros,"KMOD_SHIFT",KMOD_SHIFT);
    CreateMacroInt(&macros,"KMOD_LCTRL",KMOD_LCTRL);
    CreateMacroInt(&macros,"KMOD_RCTRL",KMOD_RCTRL);
    CreateMacroInt(&macros,"KMOD_CTRL",KMOD_CTRL);
    CreateMacroInt(&macros,"KMOD_LALT",KMOD_LALT);
    CreateMacroInt(&macros,"KMOD_RALT",KMOD_RALT);
    CreateMacroInt(&macros,"KMOD_ALT",KMOD_ALT);
    CreateMacroInt(&macros,"KMOD_LGUI",KMOD_LGUI);
    CreateMacroInt(&macros,"KMOD_RGUI",KMOD_RGUI);
    CreateMacroInt(&macros,"KMOD_GUI",KMOD_GUI);
    CreateMacroInt(&macros,"KMOD_CAPS",KMOD_CAPS);
    CreateMacroInt(&macros,"SDL_TEXTEDITING",SDL_TEXTEDITING);
    CreateMacroInt(&macros,"SDL_TEXTINPUT",SDL_TEXTINPUT);
    CreateMacroInt(&macros,"SDL_MOUSEMOTION",SDL_MOUSEMOTION);
    CreateMacroInt(&macros,"SDL_BUTTON_LMASK",SDL_BUTTON_LMASK);
    CreateMacroInt(&macros,"SDL_BUTTON_RMASK",SDL_BUTTON_RMASK);
    CreateMacroInt(&macros,"SDL_BUTTON_MMASK",SDL_BUTTON_MMASK);
    CreateMacroInt(&macros,"SDL_MOUSEBUTTONUP",SDL_MOUSEBUTTONUP);
    CreateMacroInt(&macros,"SDL_MOUSEBUTTONDOWN",SDL_MOUSEBUTTONDOWN);
    CreateMacroInt(&macros,"SDL_PRESSED",SDL_PRESSED);
    CreateMacroInt(&macros,"SDL_RELEASED",SDL_RELEASED);
    CreateMacroInt(&macros,"SDL_QUIT",SDL_QUIT);
    return macros;
}
char *GetBuiltinMacrosText() {
    vec_char_t macros=CreateMacros();
    vec_push(&macros,0);
    return macros.data;
}
void RegisterRuntimeClasses(void *gt,void *ctf,void *add_mem,void *c_arr_t) {
    //SDL2 bindings
    __LoaderCreateTypeFwd(ctf,"SDL_Window",0,1);
    __LoaderCreateTypeFwd(ctf,"SDL_Renderer",0,1);
    __LoaderCreateTypeFwd(ctf,"SDL_Texture",0,1);
    LoaderCreateTypeFwd(ctf,SDL_Surface);
    CType *sdlr_t=LoaderCreateTypeFwd(ctf,SDL_Rect);
    LoaderAddPrimMember(add_mem,gt,sdlr_t,SDL_Rect,x);
    LoaderAddPrimMember(add_mem,gt,sdlr_t,SDL_Rect,y);
    LoaderAddPrimMember(add_mem,gt,sdlr_t,SDL_Rect,w);
    LoaderAddPrimMember(add_mem,gt,sdlr_t,SDL_Rect,h);
    CType *sdlp_t=LoaderCreateTypeFwd(ctf,SDL_Point);
    LoaderAddPrimMember(add_mem,gt,sdlp_t,SDL_Point,x);
    LoaderAddPrimMember(add_mem,gt,sdlp_t,SDL_Point,y);

    CType *sdlevent_t=LoaderCreateTypeFwd(ctf,SDL_Event);
    {
        LoaderAddPrimMember(add_mem,gt,sdlevent_t,SDL_Event,type);
        {
            CType *sdlevent_wind_t=LoaderCreateTypeFwd(ctf,SDL_WindowEvent);
            LoaderAddPrimMember(add_mem,gt,sdlevent_wind_t,SDL_WindowEvent,type);
            LoaderAddPrimMember(add_mem,gt,sdlevent_wind_t,SDL_WindowEvent,timestamp);
            LoaderAddPrimMember(add_mem,gt,sdlevent_wind_t,SDL_WindowEvent,event);
            LoaderAddPrimMember(add_mem,gt,sdlevent_wind_t,SDL_WindowEvent,data1);
            LoaderAddPrimMember(add_mem,gt,sdlevent_wind_t,SDL_WindowEvent,data2);
            LoaderAddMember(add_mem,sdlevent_t,sdlevent_wind_t,SDL_Event,window);
        }
        {
            CType *sdlevent_key_t=LoaderCreateTypeFwd(ctf,SDL_KeyboardEvent);
            LoaderAddPrimMember(add_mem,gt,sdlevent_key_t,SDL_KeyboardEvent,type);
            LoaderAddPrimMember(add_mem,gt,sdlevent_key_t,SDL_KeyboardEvent,timestamp);
            LoaderAddPrimMember(add_mem,gt,sdlevent_key_t,SDL_KeyboardEvent,state);
            LoaderAddPrimMember(add_mem,gt,sdlevent_key_t,SDL_KeyboardEvent,repeat);
            //
            CType *sdlkeysym_t=LoaderCreateTypeFwd(ctf,SDL_Keysym);
            LoaderAddPrimMember(add_mem,gt,sdlkeysym_t,SDL_Keysym,scancode);
            LoaderAddPrimMember(add_mem,gt,sdlkeysym_t,SDL_Keysym,sym);
            LoaderAddPrimMember(add_mem,gt,sdlkeysym_t,SDL_Keysym,mod);
            //
            LoaderAddMember(add_mem,sdlevent_key_t,sdlkeysym_t,SDL_KeyboardEvent,keysym);
            LoaderAddMember(add_mem,sdlevent_t,sdlevent_key_t,SDL_Event,key);
        }
        {
            CType *sdlevent_edit_t=LoaderCreateTypeFwd(ctf,SDL_TextEditingEvent);
            LoaderAddPrimMember(add_mem,gt,sdlevent_edit_t,SDL_TextEditingEvent,type);
            LoaderAddPrimMember(add_mem,gt,sdlevent_edit_t,SDL_TextEditingEvent,timestamp);
            LoaderAddMember(add_mem,sdlevent_edit_t,LoaderCreateArrayType(c_arr_t,"U8i",32),SDL_TextEditingEvent,text);
            LoaderAddPrimMember(add_mem,gt,sdlevent_edit_t,SDL_TextEditingEvent,start);
            LoaderAddPrimMember(add_mem,gt,sdlevent_edit_t,SDL_TextEditingEvent,length);
            LoaderAddMember(add_mem,sdlevent_t,sdlevent_edit_t,SDL_Event,edit);
        }
        {
            CType *sdlevent_text_t=LoaderCreateTypeFwd(ctf,SDL_TextInputEvent);
            LoaderAddPrimMember(add_mem,gt,sdlevent_text_t,SDL_TextInputEvent,type);
            LoaderAddPrimMember(add_mem,gt,sdlevent_text_t,SDL_TextInputEvent,timestamp);
            LoaderAddMember(add_mem,sdlevent_text_t,LoaderCreateArrayType(c_arr_t,"U8i",32),SDL_TextInputEvent,text);
            LoaderAddMember(add_mem,sdlevent_t,sdlevent_text_t,SDL_Event,text);
        }
        {
            CType *sdlevent_mousemot_t=LoaderCreateTypeFwd(ctf,SDL_MouseMotionEvent);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mousemot_t,SDL_MouseMotionEvent,type);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mousemot_t,SDL_MouseMotionEvent,timestamp);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mousemot_t,SDL_MouseMotionEvent,which);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mousemot_t,SDL_MouseMotionEvent,state);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mousemot_t,SDL_MouseMotionEvent,x);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mousemot_t,SDL_MouseMotionEvent,y);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mousemot_t,SDL_MouseMotionEvent,xrel);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mousemot_t,SDL_MouseMotionEvent,yrel);
            LoaderAddMember(add_mem,sdlevent_t,sdlevent_mousemot_t,SDL_Event,motion);
            LoaderAddMember(add_mem,sdlevent_t,sdlevent_mousemot_t,SDL_Event,wheel);
        }
        {
            CType *sdlevent_mouse_t=LoaderCreateTypeFwd(ctf,SDL_MouseButtonEvent);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mouse_t,SDL_MouseButtonEvent,type);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mouse_t,SDL_MouseButtonEvent,timestamp);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mouse_t,SDL_MouseButtonEvent,which);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mouse_t,SDL_MouseButtonEvent,button);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mouse_t,SDL_MouseButtonEvent,state);
            LoaderAddPrimMember(add_mem,gt,sdlevent_mouse_t,SDL_MouseButtonEvent,clicks);;
            LoaderAddMember(add_mem,sdlevent_t,sdlevent_mouse_t,SDL_Event,button);
        }
        {
            CType *sdlevent_quit_t=LoaderCreateTypeFwd(ctf,SDL_QuitEvent);
            LoaderAddPrimMember(add_mem,gt,sdlevent_quit_t,SDL_QuitEvent,type);
            LoaderAddPrimMember(add_mem,gt,sdlevent_quit_t,SDL_QuitEvent,timestamp);
            LoaderAddMember(add_mem,sdlevent_t,sdlevent_quit_t,SDL_Event,quit);
        }
    }
}
void RegisterMacrosAndREPL(char *includes,int flags,char *body_code) {
    vec_char_t macros=CreateMacros();
    const char *rtc="#define RUNTIME_C\n";
    vec_pusharr(&macros,rtc,strlen(rtc));
    if(includes)
      vec_pusharr(&macros,includes,strlen(includes));
    vec_push(&macros,0);
    CSymbol *repl=map_get(&Loader.symbols,"REPL");
    assert(repl);
    for(;;) {
        if(!HCSetJmp(EnterTry())) {
            ((void(*)(char*,long,char *))repl->value_ptr)(macros.data,flags,body_code);
            break;
        } else {
            fprintf(stderr,"Caught exception,re-entering.\n");
        }
        if(macros.data)
            vec_deinit(&macros);
        macros.data=NULL;
    }
}
void RegisterMacrosAndCompile(char *includes,char *to_file,char *embed_header) {
    vec_char_t macros=CreateMacros();
    const char *rtc="#define RUNTIME_C\n";
    vec_pusharr(&macros,rtc,strlen(rtc));
    if(includes)
      vec_pusharr(&macros,includes,strlen(includes));
    vec_push(&macros,0);
    CSymbol *comp=map_get(&Loader.symbols,"CompileBinModule");
    ((void(*)(char*,char*,char *))comp->value_ptr)(macros.data,to_file,embed_header);
}
