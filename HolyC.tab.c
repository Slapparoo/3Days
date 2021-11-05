/* A Bison parser, made by GNU Bison 3.7.6.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30706

/* Bison version string.  */
#define YYBISON_VERSION "3.7.6"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Substitute the type names.  */
#define YYSTYPE         HC_STYPE
/* Substitute the variable and function names.  */
#define yyparse         HC_parse
#define yylex           HC_lex
#define yyerror         HC_error
#define yydebug         HC_debug
#define yynerrs         HC_nerrs
#define yylval          HC_lval
#define yychar          HC_char

/* First part of user prologue.  */
#line 1 "HolyC.y"

#include "3d.h"
#include <stdio.h>
int yylex();
void yyerror (char const *s);
#define SOT(a,tok) SetPosFromOther(a,tok)
#define SLE(a) SetPosFromLex(a)
static CFuncInfo *CurFuncInfo;
static void *CurFramePtr;
static void (*RunPtr)(CFuncInfo *info,AST *exp,void *framePtr);

#line 91 "HolyC.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "HolyC.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_CHAR = 3,                       /* CHAR  */
  YYSYMBOL_INT = 4,                        /* INT  */
  YYSYMBOL_FLOAT = 5,                      /* FLOAT  */
  YYSYMBOL_DOT = 6,                        /* DOT  */
  YYSYMBOL_ARROW = 7,                      /* ARROW  */
  YYSYMBOL_SIZEOF = 8,                     /* SIZEOF  */
  YYSYMBOL_ADDR_OF = 9,                    /* ADDR_OF  */
  YYSYMBOL_LNOT = 10,                      /* LNOT  */
  YYSYMBOL_BNOT = 11,                      /* BNOT  */
  YYSYMBOL_POW = 12,                       /* POW  */
  YYSYMBOL_SHL = 13,                       /* SHL  */
  YYSYMBOL_SHR = 14,                       /* SHR  */
  YYSYMBOL_MUL = 15,                       /* MUL  */
  YYSYMBOL_DIV = 16,                       /* DIV  */
  YYSYMBOL_MOD = 17,                       /* MOD  */
  YYSYMBOL_BAND = 18,                      /* BAND  */
  YYSYMBOL_BOR = 19,                       /* BOR  */
  YYSYMBOL_BXOR = 20,                      /* BXOR  */
  YYSYMBOL_ADD = 21,                       /* ADD  */
  YYSYMBOL_SUB = 22,                       /* SUB  */
  YYSYMBOL_LT = 23,                        /* LT  */
  YYSYMBOL_GT = 24,                        /* GT  */
  YYSYMBOL_LE = 25,                        /* LE  */
  YYSYMBOL_GE = 26,                        /* GE  */
  YYSYMBOL_NE = 27,                        /* NE  */
  YYSYMBOL_EQ = 28,                        /* EQ  */
  YYSYMBOL_LAND = 29,                      /* LAND  */
  YYSYMBOL_LXOR = 30,                      /* LXOR  */
  YYSYMBOL_LOR = 31,                       /* LOR  */
  YYSYMBOL_ASSIGN = 32,                    /* ASSIGN  */
  YYSYMBOL_EQ_SHL = 33,                    /* EQ_SHL  */
  YYSYMBOL_EQ_SHR = 34,                    /* EQ_SHR  */
  YYSYMBOL_EQ_MUL = 35,                    /* EQ_MUL  */
  YYSYMBOL_EQ_DIV = 36,                    /* EQ_DIV  */
  YYSYMBOL_EQ_MOD = 37,                    /* EQ_MOD  */
  YYSYMBOL_EQ_BAND = 38,                   /* EQ_BAND  */
  YYSYMBOL_EQ_BXOR = 39,                   /* EQ_BXOR  */
  YYSYMBOL_EQ_BOR = 40,                    /* EQ_BOR  */
  YYSYMBOL_EQ_ADD = 41,                    /* EQ_ADD  */
  YYSYMBOL_EQ_SUB = 42,                    /* EQ_SUB  */
  YYSYMBOL_COMMA = 43,                     /* COMMA  */
  YYSYMBOL_TRY = 44,                       /* TRY  */
  YYSYMBOL_CATCH = 45,                     /* CATCH  */
  YYSYMBOL_LASTCLASS = 46,                 /* LASTCLASS  */
  YYSYMBOL_U0 = 47,                        /* U0  */
  YYSYMBOL_LEFT_PAREN = 48,                /* LEFT_PAREN  */
  YYSYMBOL_RIGHT_PAREN = 49,               /* RIGHT_PAREN  */
  YYSYMBOL_INC = 50,                       /* INC  */
  YYSYMBOL_DEC = 51,                       /* DEC  */
  YYSYMBOL_NAME = 52,                      /* NAME  */
  YYSYMBOL_LEFT_SQAURE = 53,               /* LEFT_SQAURE  */
  YYSYMBOL_RIGHT_SQAURE = 54,              /* RIGHT_SQAURE  */
  YYSYMBOL_SEMI = 55,                      /* SEMI  */
  YYSYMBOL_IF = 56,                        /* IF  */
  YYSYMBOL_ELSE = 57,                      /* ELSE  */
  YYSYMBOL_DO = 58,                        /* DO  */
  YYSYMBOL_WHILE = 59,                     /* WHILE  */
  YYSYMBOL_FOR = 60,                       /* FOR  */
  YYSYMBOL_LEFT_CURLY = 61,                /* LEFT_CURLY  */
  YYSYMBOL_RIGHT_CURLY = 62,               /* RIGHT_CURLY  */
  YYSYMBOL_CASE = 63,                      /* CASE  */
  YYSYMBOL_COLON = 64,                     /* COLON  */
  YYSYMBOL_DOT_DOT_DOT = 65,               /* DOT_DOT_DOT  */
  YYSYMBOL_EXTERN2 = 66,                   /* EXTERN2  */
  YYSYMBOL_LOCK = 67,                      /* LOCK  */
  YYSYMBOL_EXTERN = 68,                    /* EXTERN  */
  YYSYMBOL_IMPORT = 69,                    /* IMPORT  */
  YYSYMBOL_IMPORT2 = 70,                   /* IMPORT2  */
  YYSYMBOL_ASM_IMPORT = 71,                /* ASM_IMPORT  */
  YYSYMBOL_STATIC = 72,                    /* STATIC  */
  YYSYMBOL_PUBLIC = 73,                    /* PUBLIC  */
  YYSYMBOL_CLASS = 74,                     /* CLASS  */
  YYSYMBOL_UNION = 75,                     /* UNION  */
  YYSYMBOL_INTERN = 76,                    /* INTERN  */
  YYSYMBOL_START = 77,                     /* START  */
  YYSYMBOL_END = 78,                       /* END  */
  YYSYMBOL_DEFAULT = 79,                   /* DEFAULT  */
  YYSYMBOL_BREAK = 80,                     /* BREAK  */
  YYSYMBOL_RET = 81,                       /* RET  */
  YYSYMBOL_GOTO = 82,                      /* GOTO  */
  YYSYMBOL_SWITCH = 83,                    /* SWITCH  */
  YYSYMBOL_EXE = 84,                       /* EXE  */
  YYSYMBOL_EVAL = 85,                      /* EVAL  */
  YYSYMBOL_NL = 86,                        /* NL  */
  YYSYMBOL_DBG = 87,                       /* DBG  */
  YYSYMBOL_STRING = 88,                    /* STRING  */
  YYSYMBOL_TYPENAME = 89,                  /* TYPENAME  */
  YYSYMBOL_OPCODE = 90,                    /* OPCODE  */
  YYSYMBOL_REGISTER = 91,                  /* REGISTER  */
  YYSYMBOL_DOUBLE_COLON = 92,              /* DOUBLE_COLON  */
  YYSYMBOL_DOUBLE_AT = 93,                 /* DOUBLE_AT  */
  YYSYMBOL_DU8 = 94,                       /* DU8  */
  YYSYMBOL_DU16 = 95,                      /* DU16  */
  YYSYMBOL_DU32 = 96,                      /* DU32  */
  YYSYMBOL_DU64 = 97,                      /* DU64  */
  YYSYMBOL_ALIGN = 98,                     /* ALIGN  */
  YYSYMBOL_BINFILE = 99,                   /* BINFILE  */
  YYSYMBOL_ASM = 100,                      /* ASM  */
  YYSYMBOL_YYACCEPT = 101,                 /* $accept  */
  YYSYMBOL_sib_ib = 102,                   /* sib_ib  */
  YYSYMBOL_sib_d = 103,                    /* sib_d  */
  YYSYMBOL__sib = 104,                     /* _sib  */
  YYSYMBOL_sib = 105,                      /* sib  */
  YYSYMBOL_opc_operand = 106,              /* opc_operand  */
  YYSYMBOL_opc_operands = 107,             /* opc_operands  */
  YYSYMBOL_opcode = 108,                   /* opcode  */
  YYSYMBOL_asm_blk_stmt = 109,             /* asm_blk_stmt  */
  YYSYMBOL_data_exprs = 110,               /* data_exprs  */
  YYSYMBOL_name_list = 111,                /* name_list  */
  YYSYMBOL_asm_blk = 112,                  /* asm_blk  */
  YYSYMBOL_asm_blk_stmts = 113,            /* asm_blk_stmts  */
  YYSYMBOL_expr0 = 114,                    /* expr0  */
  YYSYMBOL__callargs = 115,                /* _callargs  */
  YYSYMBOL_callargs = 116,                 /* callargs  */
  YYSYMBOL_expr1 = 117,                    /* expr1  */
  YYSYMBOL_expr2 = 118,                    /* expr2  */
  YYSYMBOL_sizeof_type = 119,              /* sizeof_type  */
  YYSYMBOL_expr3 = 120,                    /* expr3  */
  YYSYMBOL_expr4 = 121,                    /* expr4  */
  YYSYMBOL_expr4_5 = 122,                  /* expr4_5  */
  YYSYMBOL_expr5 = 123,                    /* expr5  */
  YYSYMBOL_expr6 = 124,                    /* expr6  */
  YYSYMBOL_expr7 = 125,                    /* expr7  */
  YYSYMBOL_expr8 = 126,                    /* expr8  */
  YYSYMBOL_expr9 = 127,                    /* expr9  */
  YYSYMBOL_expr10 = 128,                   /* expr10  */
  YYSYMBOL_expr11 = 129,                   /* expr11  */
  YYSYMBOL_expr12 = 130,                   /* expr12  */
  YYSYMBOL_expr13 = 131,                   /* expr13  */
  YYSYMBOL_expr14 = 132,                   /* expr14  */
  YYSYMBOL_expr = 133,                     /* expr  */
  YYSYMBOL_expr_comma = 134,               /* expr_comma  */
  YYSYMBOL_primtype0 = 135,                /* primtype0  */
  YYSYMBOL__arrlit = 136,                  /* _arrlit  */
  YYSYMBOL_arrlit = 137,                   /* arrlit  */
  YYSYMBOL_metadata = 138,                 /* metadata  */
  YYSYMBOL_cdecltail = 139,                /* cdecltail  */
  YYSYMBOL__cdecltail = 140,               /* _cdecltail  */
  YYSYMBOL_cdecl = 141,                    /* cdecl  */
  YYSYMBOL_cbody = 142,                    /* cbody  */
  YYSYMBOL__cheader = 143,                 /* _cheader  */
  YYSYMBOL__uheader = 144,                 /* _uheader  */
  YYSYMBOL_cheader = 145,                  /* cheader  */
  YYSYMBOL_uheader = 146,                  /* uheader  */
  YYSYMBOL_class = 147,                    /* class  */
  YYSYMBOL_union = 148,                    /* union  */
  YYSYMBOL__class = 149,                   /* _class  */
  YYSYMBOL__union = 150,                   /* _union  */
  YYSYMBOL_tryblock = 151,                 /* tryblock  */
  YYSYMBOL_namewptrs = 152,                /* namewptrs  */
  YYSYMBOL_vardecltail = 153,              /* vardecltail  */
  YYSYMBOL___ptrcount = 154,               /* __ptrcount  */
  YYSYMBOL_vardecltail_asn = 155,          /* vardecltail_asn  */
  YYSYMBOL_single_decl = 156,              /* single_decl  */
  YYSYMBOL_vardecltails = 157,             /* vardecltails  */
  YYSYMBOL_multi_decl = 158,               /* multi_decl  */
  YYSYMBOL_funcargs = 159,                 /* funcargs  */
  YYSYMBOL_linkage = 160,                  /* linkage  */
  YYSYMBOL_func = 161,                     /* func  */
  YYSYMBOL_ocstmt = 162,                   /* ocstmt  */
  YYSYMBOL_expr_opt = 163,                 /* expr_opt  */
  YYSYMBOL_loop_header = 164,              /* loop_header  */
  YYSYMBOL_ifcl = 165,                     /* ifcl  */
  YYSYMBOL_ostmt = 166,                    /* ostmt  */
  YYSYMBOL_cstmt = 167,                    /* cstmt  */
  YYSYMBOL_dostmt = 168,                   /* dostmt  */
  YYSYMBOL_subswit = 169,                  /* subswit  */
  YYSYMBOL_swit_body_stmt = 170,           /* swit_body_stmt  */
  YYSYMBOL_swit_body = 171,                /* swit_body  */
  YYSYMBOL_swit = 172,                     /* swit  */
  YYSYMBOL_simple_stmt = 173,              /* simple_stmt  */
  YYSYMBOL_opcodes = 174,                  /* opcodes  */
  YYSYMBOL_stmts = 175,                    /* stmts  */
  YYSYMBOL_scope = 176,                    /* scope  */
  YYSYMBOL_global_stmts = 177,             /* global_stmts  */
  YYSYMBOL_global_stmt = 178,              /* global_stmt  */
  YYSYMBOL__expr0 = 179,                   /* _expr0  */
  YYSYMBOL__expr1 = 180,                   /* _expr1  */
  YYSYMBOL__expr2 = 181,                   /* _expr2  */
  YYSYMBOL__expr3 = 182,                   /* _expr3  */
  YYSYMBOL__expr4 = 183,                   /* _expr4  */
  YYSYMBOL__expr4_5 = 184,                 /* _expr4_5  */
  YYSYMBOL__expr5 = 185,                   /* _expr5  */
  YYSYMBOL__expr6 = 186,                   /* _expr6  */
  YYSYMBOL__expr7 = 187,                   /* _expr7  */
  YYSYMBOL__expr8 = 188,                   /* _expr8  */
  YYSYMBOL__expr9 = 189,                   /* _expr9  */
  YYSYMBOL__expr10 = 190,                  /* _expr10  */
  YYSYMBOL__expr11 = 191,                  /* _expr11  */
  YYSYMBOL__expr12 = 192,                  /* _expr12  */
  YYSYMBOL__expr13 = 193,                  /* _expr13  */
  YYSYMBOL__expr14 = 194,                  /* _expr14  */
  YYSYMBOL__expr = 195                     /* _expr  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined HC_STYPE_IS_TRIVIAL && HC_STYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  93
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1671

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  101
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  95
/* YYNRULES -- Number of rules.  */
#define YYNRULES  346
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  620

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   355


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100
};

#if HC_DEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    59,    61,    62,    66,    70,    74,    77,    78,    82,
      86,    90,    91,    94,    95,    96,    97,   102,   108,   116,
     117,   121,   122,   131,   136,   141,   146,   158,   159,   163,
     167,   171,   175,   179,   180,   181,   185,   189,   193,   197,
     201,   202,   205,   211,   219,   228,   229,   230,   231,   232,
     235,   238,   239,   240,   243,   244,   245,   250,   254,   258,
     262,   266,   270,   274,   278,   282,   286,   290,   295,   300,
     308,   309,   314,   315,   319,   323,   329,   335,   339,   344,
     348,   352,   356,   360,   364,   370,   371,   375,   379,   384,
     385,   389,   393,   398,   399,   404,   405,   410,   411,   416,
     417,   421,   426,   427,   431,   435,   439,   444,   445,   449,
     454,   455,   460,   461,   466,   467,   472,   473,   477,   481,
     485,   489,   493,   497,   501,   505,   509,   513,   518,   520,
     524,   529,   539,   540,   541,   548,   549,   550,   551,   552,
     557,   563,   572,   575,   582,   583,   587,   593,   597,   601,
     604,   607,   608,   613,   618,   623,   628,   633,   637,   638,
     639,   645,   651,   655,   660,   665,   680,   696,   704,   708,
     714,   721,   729,   730,   733,   737,   741,   745,   757,   770,
     784,   792,   801,   807,   814,   818,   823,   830,   831,   833,
     838,   839,   845,   849,   850,   851,   852,   853,   854,   855,
     856,   860,   873,   887,   900,   914,   930,   944,   958,   973,
     986,  1000,  1016,  1034,  1035,  1037,  1038,  1041,  1047,  1055,
    1061,  1066,  1073,  1084,  1085,  1092,  1103,  1109,  1116,  1117,
    1121,  1125,  1129,  1135,  1136,  1137,  1138,  1145,  1152,  1153,
    1161,  1166,  1167,  1168,  1172,  1176,  1177,  1181,  1185,  1192,
    1193,  1194,  1198,  1202,  1206,  1210,  1214,  1215,  1216,  1222,
    1228,  1229,  1230,  1231,  1236,  1241,  1242,  1247,  1251,  1256,
    1262,  1271,  1279,  1288,  1289,  1290,  1291,  1292,  1295,  1298,
    1299,  1303,  1304,  1308,  1312,  1316,  1320,  1324,  1328,  1332,
    1336,  1337,  1341,  1345,  1351,  1357,  1361,  1366,  1370,  1374,
    1378,  1382,  1386,  1392,  1393,  1397,  1401,  1406,  1407,  1411,
    1415,  1420,  1421,  1426,  1427,  1432,  1433,  1438,  1439,  1443,
    1448,  1449,  1453,  1457,  1461,  1466,  1467,  1471,  1476,  1477,
    1482,  1483,  1488,  1489,  1494,  1495,  1499,  1503,  1507,  1511,
    1515,  1519,  1523,  1527,  1531,  1535,  1539
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if HC_DEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "CHAR", "INT", "FLOAT",
  "DOT", "ARROW", "SIZEOF", "ADDR_OF", "LNOT", "BNOT", "POW", "SHL", "SHR",
  "MUL", "DIV", "MOD", "BAND", "BOR", "BXOR", "ADD", "SUB", "LT", "GT",
  "LE", "GE", "NE", "EQ", "LAND", "LXOR", "LOR", "ASSIGN", "EQ_SHL",
  "EQ_SHR", "EQ_MUL", "EQ_DIV", "EQ_MOD", "EQ_BAND", "EQ_BXOR", "EQ_BOR",
  "EQ_ADD", "EQ_SUB", "COMMA", "TRY", "CATCH", "LASTCLASS", "U0",
  "LEFT_PAREN", "RIGHT_PAREN", "INC", "DEC", "NAME", "LEFT_SQAURE",
  "RIGHT_SQAURE", "SEMI", "IF", "ELSE", "DO", "WHILE", "FOR", "LEFT_CURLY",
  "RIGHT_CURLY", "CASE", "COLON", "DOT_DOT_DOT", "EXTERN2", "LOCK",
  "EXTERN", "IMPORT", "IMPORT2", "ASM_IMPORT", "STATIC", "PUBLIC", "CLASS",
  "UNION", "INTERN", "START", "END", "DEFAULT", "BREAK", "RET", "GOTO",
  "SWITCH", "EXE", "EVAL", "NL", "DBG", "STRING", "TYPENAME", "OPCODE",
  "REGISTER", "DOUBLE_COLON", "DOUBLE_AT", "DU8", "DU16", "DU32", "DU64",
  "ALIGN", "BINFILE", "ASM", "$accept", "sib_ib", "sib_d", "_sib", "sib",
  "opc_operand", "opc_operands", "opcode", "asm_blk_stmt", "data_exprs",
  "name_list", "asm_blk", "asm_blk_stmts", "expr0", "_callargs",
  "callargs", "expr1", "expr2", "sizeof_type", "expr3", "expr4", "expr4_5",
  "expr5", "expr6", "expr7", "expr8", "expr9", "expr10", "expr11",
  "expr12", "expr13", "expr14", "expr", "expr_comma", "primtype0",
  "_arrlit", "arrlit", "metadata", "cdecltail", "_cdecltail", "cdecl",
  "cbody", "_cheader", "_uheader", "cheader", "uheader", "class", "union",
  "_class", "_union", "tryblock", "namewptrs", "vardecltail", "__ptrcount",
  "vardecltail_asn", "single_decl", "vardecltails", "multi_decl",
  "funcargs", "linkage", "func", "ocstmt", "expr_opt", "loop_header",
  "ifcl", "ostmt", "cstmt", "dostmt", "subswit", "swit_body_stmt",
  "swit_body", "swit", "simple_stmt", "opcodes", "stmts", "scope",
  "global_stmts", "global_stmt", "_expr0", "_expr1", "_expr2", "_expr3",
  "_expr4", "_expr4_5", "_expr5", "_expr6", "_expr7", "_expr8", "_expr9",
  "_expr10", "_expr11", "_expr12", "_expr13", "_expr14", "_expr", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355
};
#endif

#define YYPACT_NINF (-525)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     329,   -24,  1528,  1528,  1043,    49,   952,  -525,  -525,  -525,
    -525,    90,  1528,  1528,  1528,  1528,  1528,  1528,  -525,  1528,
    1528,  1528,  -525,  -525,   212,    26,  -525,  -525,    53,  -525,
     440,   441,    43,    92,    86,   331,   358,   360,    91,   100,
     116,   369,  -525,     4,   115,   -24,    69,  -525,   129,  1043,
     158,   160,   -19,  -525,  -525,   -14,  -525,  -525,   -12,     0,
     125,  1250,   174,   167,   212,   141,   182,   180,  -525,   123,
      12,   198,   224,   211,   229,  -525,  -525,   246,   251,  -525,
     270,    51,  -525,  -525,  1043,  1043,  -525,  -525,   275,  -525,
    -525,   225,  -525,  -525,  -525,  -525,   276,  1134,  -525,  -525,
     325,   268,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,
       2,  -525,  -525,   311,   318,  -525,  -525,   323,   326,  1134,
    -525,  -525,  1528,  1528,  1528,  1528,  1528,  1528,  1528,  1528,
    1528,  1528,  1528,  1528,  1528,  1528,  1528,  1528,  1528,  1528,
    1528,  1528,  1528,  1528,  1528,  1528,  1528,  1528,  1528,  1528,
    1528,  1528,  1528,  1528,  1528,  -525,   347,  -525,  1528,   335,
    1528,  1528,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,
    -525,  -525,   173,   357,  1528,  -525,  -525,  -525,   138,  1578,
    1578,  1578,  1578,  1578,  1578,  -525,  1528,  1578,  1578,  -525,
      18,  -525,   456,  -525,    31,  -525,  -525,   365,   405,  -525,
    -525,    73,  -525,   471,   495,   447,   448,   476,   451,   373,
     391,   470,   473,   439,   399,   452,   105,  -525,  -525,    40,
    -525,   446,   465,    76,   493,  -525,   475,   459,   472,  -525,
    -525,  -525,  -525,  -525,     5,     6,    12,   508,  -525,  -525,
    -525,   509,  -525,   515,  -525,  -525,  -525,    19,    85,  -525,
    -525,  -525,  -525,  -525,  -525,  -525,  -525,    94,  -525,    16,
     118,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,
    -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,
    -525,   369,   369,   369,   369,   369,   369,   369,   369,   369,
     369,   369,  -525,   -24,   132,   524,   139,   115,   518,  -525,
    -525,   162,  1134,  -525,   325,   469,  -525,  -525,  -525,  -525,
    -525,  -525,   218,  -525,  -525,  -525,  -525,   458,   521,   525,
     527,   512,  -525,   452,  -525,  -525,  1306,   141,   529,   531,
    1134,  -525,  -525,  1578,  1578,  1578,  1578,  1578,  1578,  1578,
    1578,  1578,  1578,  1578,  1578,  1578,  1578,  1578,  1578,  1578,
    1578,  1578,  1578,  1578,  1578,  1578,  1578,  1578,  1578,  1578,
    1578,  1578,  1578,  1578,  -525,   -36,  -525,  -525,   532,    32,
    1528,  1528,  1528,  1528,   581,  -525,  -525,   152,    47,  -525,
    -525,   166,  1326,  1402,   571,    48,  -525,  -525,   135,   163,
     533,   534,   539,  -525,  1043,  -525,  -525,  -525,  1528,  -525,
      24,  -525,  -525,  -525,  -525,  1528,  -525,  1528,   543,   104,
     259,  -525,  1422,  1528,  -525,  -525,  -525,  1306,  -525,   365,
    -525,  -525,   260,    -6,  -525,  -525,  -525,  -525,  -525,  -525,
    -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,
    -525,  -525,  -525,  -525,   399,   399,   399,   399,   399,   399,
     399,   399,   399,   399,   399,  -525,  -525,  -525,   208,   526,
     530,   221,  -525,   223,   227,   228,   564,  -525,  -525,   556,
    -525,   573,    12,  -525,   264,  1154,  -525,  -525,  -525,   131,
     102,  -525,  -525,  -525,    12,   568,  -525,  -525,  -525,  -525,
    -525,   179,  -525,  -525,  -525,   124,  -525,   267,   569,   588,
    -525,  -525,   610,   611,  -525,  -525,  -525,  -525,  -525,   575,
    -525,  -525,  -525,  1528,  -525,  -525,  -525,  -525,   624,  -525,
     -13,    76,  -525,   200,   256,   567,  -525,    79,  -525,  -525,
     103,   582,   587,  -525,   435,  -525,   584,   280,   586,  -525,
    -525,  1528,  -525,  1478,   577,   578,  -525,  -525,  -525,   679,
     546,   641,  -525,  -525,  -525,  -525,  -525,   603,  -525,  -525,
    -525,  -525,  1230,  -525,   601,   606,   616,    48,  1528,  -525,
     278,   230,   286,  -525,   617,  -525,   -11,   861,  -525,  -525,
    -525,  -525,  -525,   299,  -525,  -525,  -525,   623,   625,   147,
    -525,  -525,  -525,  -525,   626,  -525,  -525,  -525,  -525,  1528,
     770,  -525,  -525,   631,   147,   312,   313,   595,   621,   147,
     324,   147,  -525,  -525,  -525,  -525,  -525,   328,  -525,  -525
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     265,     0,     0,     0,   267,     0,     0,   268,    47,    46,
      45,     0,     0,     0,     0,     0,     0,     0,    49,     0,
       0,     0,    50,    48,     0,     0,    55,    57,    72,    85,
      89,    93,    95,    97,    99,   102,   107,   110,   112,   114,
     116,   128,   130,     0,   270,     0,    50,   252,     0,     0,
       0,     0,     0,   193,   194,     0,   196,   195,   152,   153,
       0,     0,     0,     0,   131,    21,     0,     0,   242,     0,
     173,   158,   159,     0,     0,   256,   257,   132,   133,   238,
       0,     0,   245,   266,     0,     0,   213,   214,     0,   249,
     223,   241,   250,     1,   264,   260,     0,     0,    73,   131,
      74,    70,   132,   133,    83,    84,    78,    77,    81,    82,
       0,    80,    79,     0,     0,    44,    43,     0,     0,    51,
      64,    63,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   269,     0,   255,     0,     0,
       0,   216,   197,   199,   198,   200,   154,   156,   155,   157,
     251,   254,     0,     0,     0,   275,   274,   273,     0,     0,
       0,     0,     0,     0,     0,   277,     0,     0,     0,   278,
       0,   276,     0,    14,     0,    11,    13,    19,    22,   279,
     281,   290,   303,   307,   311,   313,   315,   317,   320,   325,
     328,   330,   332,   334,   346,    15,     0,   239,   248,   174,
     168,   134,   172,   188,     0,   187,   189,     0,     0,   151,
     151,   162,   164,   243,   152,   153,   173,     0,   222,   225,
     220,   214,   247,     0,   262,   263,   261,     0,     0,    71,
     134,    56,    58,    59,    60,    61,    54,     0,    52,     0,
       0,    86,    87,    88,    90,    91,    92,    94,    96,    98,
     100,   101,   103,   104,   105,   106,   109,   108,   111,   113,
     115,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   129,     0,     0,     0,     0,   215,     0,   253,
     246,     0,     0,   291,   292,     0,   301,   302,   296,   295,
     299,   300,     0,   298,   297,     6,     2,     7,     0,     0,
       0,     0,    16,     0,   272,   271,     0,     0,     0,     0,
      51,   287,   286,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    12,     0,    37,    39,     0,     0,
       0,     0,     0,     0,     0,    33,    40,     0,   175,   169,
     163,   192,     0,     0,     0,   173,   160,   161,     0,     0,
     154,   155,   172,   244,     0,   240,    76,    75,    51,    65,
       0,    66,    62,   167,   219,     0,   217,   216,     0,     0,
       0,   280,     0,     0,    10,   282,   283,     0,    18,    20,
     284,   285,     0,     0,   304,   305,   306,   308,   309,   310,
     312,   314,   316,   318,   319,   321,   322,   323,   324,   327,
     326,   329,   331,   333,   335,   336,   337,   338,   339,   340,
     341,   342,   343,   344,   345,    24,    23,    34,     0,     0,
       0,     0,    27,     0,     0,     0,     0,    42,    41,   176,
     170,     0,   173,   190,     0,     0,   182,   183,   181,     0,
       0,   172,   186,   165,   173,     0,   150,   149,   166,   259,
     258,   192,   221,   224,    53,     0,    67,     0,     0,     0,
     294,   293,    46,     3,     8,     9,    17,   288,   289,     0,
      36,    26,    25,     0,    29,    30,    31,    32,     0,   171,
       0,   184,   185,     0,     0,     0,   137,     0,   138,   180,
       0,     0,   147,   144,   145,   148,     0,     0,     0,    68,
     226,   216,   237,     0,     0,     0,   228,   233,   235,     0,
       0,     0,    35,    28,    38,   203,   207,     0,   191,   201,
     209,   141,     0,   139,     0,     0,     0,   173,     0,   146,
       0,     0,     0,    69,     0,   231,     0,     0,   229,   236,
     234,     4,     5,     0,   140,   135,   136,     0,     0,   192,
     143,   142,   206,   210,     0,   204,   212,   218,   230,     0,
       0,   202,   208,     0,   192,     0,     0,     0,     0,   192,
       0,     0,   177,   205,   211,   232,   227,     0,   178,   179
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -525,  -525,  -525,  -196,  -178,   353,  -525,   -81,   297,   144,
    -525,  -525,  -525,  -525,   288,   361,  -525,  -525,   -82,   430,
     397,   409,   563,   565,   562,   410,   287,   417,   558,   555,
     557,   381,  -113,    10,    -9,  -525,  -451,  -525,  -525,   136,
    -525,   474,  -525,  -525,  -525,  -525,   169,   171,     1,    -3,
    -525,   -53,  -452,  -525,   233,  -497,   317,   627,  -473,  -525,
    -525,    17,  -391,  -525,  -525,   -80,   -76,  -525,  -525,  -524,
     130,  -525,  -525,  -525,  -525,    -1,  -525,  -525,  -525,  -525,
    -525,   310,   205,   216,   367,   370,   368,   222,    81,   219,
     362,   363,   359,   258,   -54
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,   317,   318,   195,   196,   197,   198,    67,   376,   461,
     458,    68,   377,    26,   256,   257,    27,    28,   100,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    69,    70,   527,   477,   569,   532,   533,
     485,   388,    71,    72,    73,    74,    75,    76,   102,   103,
      79,   481,   223,   224,   225,   473,   226,    80,   474,    81,
      82,   546,   298,    84,    85,    86,    87,    88,   547,   548,
     549,    89,    90,    91,    96,    92,     4,     5,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   323
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
       7,    78,   101,    78,   238,    77,   258,    77,   239,   241,
     243,   215,    43,    44,   322,   247,   498,   222,   537,   364,
     521,    83,   315,    95,   528,   580,   558,   219,   455,   110,
     115,   400,   534,   162,   249,   324,   459,     6,   164,   495,
     166,   292,   555,   508,   156,   154,    78,   154,     6,    93,
      77,   251,   168,   598,   599,   378,   456,   390,   391,   117,
     118,   129,   469,   219,   220,   401,   159,   221,   396,    59,
     163,   172,   236,   496,   558,   165,   580,   167,   116,   328,
     329,    78,    78,   325,   460,    77,    77,    59,   101,   169,
     155,    59,   379,    78,   167,   169,   304,    77,   250,   470,
     220,   119,   240,   120,   121,   131,   122,   248,   382,   316,
     259,   586,   130,   246,   558,   534,   605,   530,   564,   249,
     140,   330,   562,   331,   332,   234,   235,   364,   154,   383,
     141,   610,   260,   157,   397,   375,   617,   398,    97,   538,
      99,   563,    98,   399,   175,   176,   177,   142,   418,   178,
     574,   179,   180,   500,   531,   565,   181,   365,   154,   182,
     366,   154,   183,   184,    58,    59,   154,   367,   294,   101,
     296,   297,   402,   539,   154,   154,   368,   158,   218,    99,
     170,   404,   154,   392,   301,   529,   302,   185,   406,   186,
     303,   187,   188,   189,   190,    65,   312,   483,   369,   370,
     371,   372,   373,   374,   365,   154,   160,   366,   161,    58,
      59,   408,    58,    59,   467,   174,   154,   258,   113,   114,
     409,    58,    59,   368,    99,   488,   173,    99,   299,   191,
     192,   471,   193,   250,   194,   217,    99,    58,    59,   506,
      58,    59,    65,   216,   536,   369,   370,   371,   372,   373,
     374,   509,    99,    58,    59,    99,   250,   462,   462,   462,
     462,   154,   227,   510,   513,   557,   513,   411,    99,   476,
     513,   513,   229,   215,    58,    59,   514,   244,   515,     8,
       9,    10,   516,   517,    11,   258,    12,    13,   228,    99,
     230,    14,   403,   101,    15,   594,   375,    16,    17,   504,
     505,   231,   154,   398,    58,    59,   232,   523,   501,   507,
     154,   559,   410,   524,   492,    65,   540,     6,   493,    99,
      45,   423,    18,   571,    19,   233,    20,    21,    46,   572,
     242,    47,    48,   592,    49,    50,    51,     6,   245,     6,
     249,   595,    52,    59,    53,    54,    55,     6,    56,    57,
      58,    59,   132,   133,   601,   611,    60,    61,    62,    63,
       6,   612,   526,   252,    23,    64,    65,   611,   613,    25,
     253,   611,   472,   618,     6,   254,    66,   619,   255,   484,
     484,   134,   135,   136,   137,    78,    78,   138,   139,    77,
      77,    78,   293,   479,   295,    77,   344,   345,   346,   347,
     553,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   300,     1,     2,   497,     3,   297,   348,   349,
     250,   272,   273,   274,   275,   435,   436,   437,   438,   326,
     576,   353,   354,   355,   356,   357,   358,   359,   360,   361,
     362,   363,   104,   105,   106,   107,   108,   109,   327,   585,
     111,   112,   123,   124,   125,   591,   126,   127,   128,   175,
     176,   177,   319,   320,   178,   339,   179,   180,   340,   250,
     352,   181,   342,   343,   182,   319,   320,   183,   184,   412,
     413,   221,   472,   333,   334,   335,   607,   568,   383,   306,
     307,   308,   309,   310,   311,   341,    78,   313,   314,   350,
      77,   380,   185,   351,   186,   190,   187,   188,   189,   190,
     336,   337,   338,   381,   472,   463,   464,   465,   385,   556,
     261,   262,   263,   560,   281,   282,   283,   284,   285,   286,
     287,   288,   289,   290,   291,   264,   265,   266,   424,   425,
     426,   384,   270,   271,   191,   305,    78,   321,   386,   194,
      77,   297,   427,   428,   429,   276,   277,   486,   486,   487,
     487,   387,   472,   393,   433,   434,   394,   439,   440,   593,
     395,   596,   405,   407,    78,   414,   417,   415,    77,   416,
     472,   420,   602,   421,   457,   466,   480,   491,   489,   490,
     511,     8,     9,    10,   512,   472,    11,    78,    12,    13,
     472,    77,   472,    14,   499,   614,    15,   518,   519,    16,
      17,   444,   445,   446,   447,   448,   449,   450,   451,   452,
     453,   454,   520,   535,   541,   550,   551,   552,   554,   561,
     567,   566,    45,   570,    18,   573,    19,   581,    20,    21,
      46,   577,   578,    47,    48,   582,    49,    50,    51,     6,
     542,   543,   583,   587,    52,   588,    53,    54,    55,   615,
      56,    57,    58,    59,   589,   544,   597,   545,    60,    61,
      62,    63,   603,   604,   468,   606,    23,    64,    65,   609,
     419,    25,     8,     9,    10,   616,   494,    11,    66,    12,
      13,   422,   267,   269,    14,   268,   279,    15,   278,   280,
      16,    17,   482,   590,   389,   522,   430,   600,   237,   432,
     431,   443,   441,     0,   442,     0,     0,     0,     0,     0,
       0,     0,     0,    45,     0,    18,     0,    19,     0,    20,
      21,    46,     0,     0,    47,    48,     0,    49,    50,    51,
       6,   579,   543,     0,     0,    52,     0,    53,    54,    55,
       0,    56,    57,    58,    59,     0,   544,     0,   545,    60,
      61,    62,    63,     0,     0,     0,     0,    23,    64,    65,
       0,     0,    25,     8,     9,    10,     0,     0,    11,    66,
      12,    13,     0,     0,     0,    14,     0,     0,    15,     0,
       0,    16,    17,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    45,     0,    18,     0,    19,     0,
      20,    21,    46,     0,     0,    47,    48,     0,    49,    50,
      51,     6,     0,   543,     0,     0,    52,     0,    53,    54,
      55,     0,    56,    57,    58,    59,     0,   544,   608,   545,
      60,    61,    62,    63,     0,     0,     0,     0,    23,    64,
      65,     0,     0,    25,     8,     9,    10,     0,     0,    11,
      66,    12,    13,     0,     0,     0,    14,     0,     0,    15,
       0,     0,    16,    17,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    45,     0,    18,     0,    19,
       0,    20,    21,    46,     0,     0,    47,    48,     0,    49,
      50,    51,     6,     0,   543,     0,     0,    52,     0,    53,
      54,    55,     0,    56,    57,    58,    59,     0,   544,     0,
     545,    60,    61,    62,    63,     0,     0,     0,     0,    23,
      64,    65,     0,     0,    25,     8,     9,    10,     0,     0,
      11,    66,    12,    13,     0,     0,     0,    14,     0,     0,
      15,     0,     0,    16,    17,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    45,     0,    18,     0,
      19,     0,    20,    21,    46,     0,     0,    47,    48,     0,
      49,    50,    51,     6,    94,     0,     0,     0,    52,     0,
      53,    54,    55,     0,    56,    57,    58,    59,     0,     0,
       0,     0,    60,    61,    62,    63,     0,     0,     0,     0,
      23,    64,    65,     0,     0,    25,     8,     9,    10,     0,
       0,    11,    66,    12,    13,     0,     0,     0,    14,     0,
       0,    15,     0,     0,    16,    17,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    45,     0,    18,
       0,    19,     0,    20,    21,    46,     0,     0,    47,    48,
       0,    49,    50,    51,     6,     0,     0,     0,     0,    52,
       0,    53,    54,    55,     0,    56,    57,    58,    59,     0,
       0,     0,     0,    60,    61,    62,    63,     0,     0,     0,
       0,    23,    64,    65,     0,     0,    25,     8,     9,    10,
       0,     0,    11,    66,    12,    13,     0,     0,     0,    14,
       0,     0,    15,     0,     0,    16,    17,     8,     9,    10,
       0,     0,    11,     0,    12,    13,     0,     0,     0,    14,
       0,     0,    15,     0,     0,    16,    17,     0,     0,     0,
      18,     0,    19,     0,    20,    21,    22,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   525,     0,     0,
      18,     0,    19,     0,    20,    21,    22,     0,    58,    59,
       0,     0,     0,     0,     0,   475,     0,     0,     0,     0,
       0,     0,    23,    64,     0,     0,     0,    25,     0,     0,
       0,     0,     0,     8,     9,    10,     0,     0,    11,     0,
      12,    13,    23,    24,     0,    14,     0,    25,    15,     0,
       0,    16,    17,     8,     9,    10,     0,     0,    11,     0,
      12,    13,     0,     0,     0,    14,     0,     0,    15,     0,
       0,    16,    17,     0,     0,     0,    18,     0,    19,     0,
      20,    21,    22,     0,     0,     0,     0,     0,     0,     0,
       0,   475,   584,     0,     0,     0,    18,     0,    19,     0,
      20,    21,    22,     0,     0,   171,     0,     0,     0,   175,
     176,   177,     0,     0,   178,     0,   179,   180,    23,    24,
       0,   181,     0,    25,   182,     0,     0,   183,   184,     8,
       9,    10,     0,     0,    11,     0,    12,    13,    23,    24,
       0,    14,     0,    25,    15,     0,     0,    16,    17,     0,
       0,     0,   185,     0,   186,     0,   187,   188,   189,   190,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    18,     0,    19,     0,    20,    21,    22,     0,
       0,     0,     0,     0,     0,     0,     0,   475,     0,     0,
       0,     0,     0,     0,   191,   305,     0,     0,     0,   194,
       0,     0,     0,     0,     0,     8,     9,    10,     0,     0,
      11,     0,    12,    13,    23,    24,     0,    14,     0,    25,
      15,     0,     0,    16,    17,     8,   502,    10,     0,     0,
      11,     0,    12,    13,     0,     0,     0,    14,     0,     0,
      15,     0,     0,    16,    17,     0,     0,     0,    18,     0,
      19,     0,    20,    21,    22,     0,   478,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    18,     0,
      19,     0,    20,    21,    22,     0,     0,     0,     0,     0,
       0,     8,     9,    10,     0,     0,    11,     0,    12,    13,
      23,    24,     0,    14,     0,    25,    15,     0,     0,    16,
      17,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      23,    24,     0,   503,     0,    25,     0,     0,     0,     0,
       0,     0,     0,     0,    18,     0,    19,     0,    20,    21,
      22,     8,     9,    10,     0,     0,    11,     0,    12,    13,
       0,     0,   575,    14,     0,     0,    15,     0,     0,    16,
      17,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    23,    24,     0,     0,
       0,    25,     0,     0,    18,     0,    19,     0,    20,    21,
      22,   175,   176,   177,     0,     0,   178,     0,   179,   180,
       0,     0,     0,   181,     0,     0,   182,     0,     0,   183,
     184,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    23,    24,     0,     0,
       0,    25,     0,     0,   185,     0,   186,     0,   187,   188,
     189,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   191,   305,     0,     0,
       0,   194
};

static const yytype_int16 yycheck[] =
{
       1,     4,    11,     6,    84,     4,   119,     6,    84,    85,
      91,    65,     2,     3,   192,    97,   407,    70,   491,   215,
     472,     4,     4,     6,   475,   549,   523,    15,    64,    19,
       4,    15,   484,    52,    15,     4,     4,    61,    52,    15,
      52,   154,    55,    49,    45,    43,    49,    43,    61,     0,
      49,    49,    52,    64,    65,    15,    92,    52,    52,     6,
       7,    18,    15,    15,    52,    49,    49,    70,    49,    75,
      89,    61,    81,    49,   571,    89,   600,    89,    52,     6,
       7,    84,    85,    52,    52,    84,    85,    75,    97,    89,
      86,    75,    52,    96,    89,    89,   178,    96,   101,    52,
      52,    48,    85,    50,    51,    19,    53,    97,    32,    91,
     119,   562,    20,    96,   611,   567,   589,    15,    15,    15,
      29,    48,    43,    50,    51,    74,    75,   323,    43,    53,
      30,   604,   122,    64,    49,   216,   609,    43,    48,    15,
      89,    62,    52,    49,     3,     4,     5,    31,   326,     8,
     541,    10,    11,    49,    52,    52,    15,    52,    43,    18,
      55,    43,    21,    22,    74,    75,    43,    62,   158,   178,
     160,   161,    54,    49,    43,    43,    71,    48,    55,    89,
      55,    49,    43,   236,   174,    54,    48,    46,    49,    48,
      52,    50,    51,    52,    53,    90,   186,    62,    93,    94,
      95,    96,    97,    98,    52,    43,    48,    55,    48,    74,
      75,    49,    74,    75,    62,    48,    43,   330,     6,     7,
     302,    74,    75,    71,    89,    62,    52,    89,    55,    88,
      89,    65,    91,   236,    93,    55,    89,    74,    75,   417,
      74,    75,    90,    61,    65,    93,    94,    95,    96,    97,
      98,    43,    89,    74,    75,    89,   259,   370,   371,   372,
     373,    43,    64,    55,    43,    65,    43,    49,    89,   382,
      43,    43,    61,   327,    74,    75,    55,     1,    55,     3,
       4,     5,    55,    55,     8,   398,    10,    11,    64,    89,
      61,    15,   293,   302,    18,    65,   377,    21,    22,   412,
     413,    55,    43,    43,    74,    75,    55,    43,    49,    49,
      43,    55,   302,    49,   394,    90,    49,    61,   394,    89,
      44,   330,    46,    43,    48,    55,    50,    51,    52,    49,
      55,    55,    56,    55,    58,    59,    60,    61,    62,    61,
      15,    55,    66,    75,    68,    69,    70,    61,    72,    73,
      74,    75,    21,    22,    55,    43,    80,    81,    82,    83,
      61,    49,   475,    52,    88,    89,    90,    43,    55,    93,
      52,    43,   381,    49,    61,    52,   100,    49,    52,   388,
     389,    23,    24,    25,    26,   388,   389,    27,    28,   388,
     389,   394,    45,   383,    59,   394,    23,    24,    25,    26,
     513,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    55,    84,    85,   405,    87,   407,    27,    28,
     423,   134,   135,   136,   137,   344,   345,   346,   347,    64,
     543,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    12,    13,    14,    15,    16,    17,    43,   562,
      20,    21,    12,    13,    14,   568,    15,    16,    17,     3,
       4,     5,     6,     7,     8,    18,    10,    11,    20,   472,
      31,    15,    21,    22,    18,     6,     7,    21,    22,    21,
      22,   484,   491,    12,    13,    14,   599,    52,    53,   179,
     180,   181,   182,   183,   184,    19,   499,   187,   188,    29,
     499,    55,    46,    30,    48,    53,    50,    51,    52,    53,
      15,    16,    17,    48,   523,   371,   372,   373,    43,   520,
     123,   124,   125,   524,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   126,   127,   128,   333,   334,
     335,    48,   132,   133,    88,    89,   549,    91,    89,    93,
     549,   541,   336,   337,   338,   138,   139,   388,   389,   388,
     389,    89,   571,    55,   342,   343,    57,   348,   349,   570,
      55,   572,    48,    55,   577,    54,    64,    52,   577,    52,
     589,    52,   583,    52,    52,     4,    15,    48,    55,    55,
      64,     3,     4,     5,    64,   604,     8,   600,    10,    11,
     609,   600,   611,    15,    61,   606,    18,    43,    52,    21,
      22,   353,   354,   355,   356,   357,   358,   359,   360,   361,
     362,   363,    49,    55,    55,    15,    15,    52,     4,    62,
      43,    49,    44,    49,    46,    49,    48,    91,    50,    51,
      52,    64,    64,    55,    56,     4,    58,    59,    60,    61,
      62,    63,    49,    52,    66,    49,    68,    69,    70,    64,
      72,    73,    74,    75,    48,    77,    49,    79,    80,    81,
      82,    83,    49,    48,   377,    49,    88,    89,    90,    48,
     327,    93,     3,     4,     5,    64,   398,     8,   100,    10,
      11,   330,   129,   131,    15,   130,   141,    18,   140,   142,
      21,    22,   385,   567,   230,   472,   339,   577,    81,   341,
     340,   352,   350,    -1,   351,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    44,    -1,    46,    -1,    48,    -1,    50,
      51,    52,    -1,    -1,    55,    56,    -1,    58,    59,    60,
      61,    62,    63,    -1,    -1,    66,    -1,    68,    69,    70,
      -1,    72,    73,    74,    75,    -1,    77,    -1,    79,    80,
      81,    82,    83,    -1,    -1,    -1,    -1,    88,    89,    90,
      -1,    -1,    93,     3,     4,     5,    -1,    -1,     8,   100,
      10,    11,    -1,    -1,    -1,    15,    -1,    -1,    18,    -1,
      -1,    21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    44,    -1,    46,    -1,    48,    -1,
      50,    51,    52,    -1,    -1,    55,    56,    -1,    58,    59,
      60,    61,    -1,    63,    -1,    -1,    66,    -1,    68,    69,
      70,    -1,    72,    73,    74,    75,    -1,    77,    78,    79,
      80,    81,    82,    83,    -1,    -1,    -1,    -1,    88,    89,
      90,    -1,    -1,    93,     3,     4,     5,    -1,    -1,     8,
     100,    10,    11,    -1,    -1,    -1,    15,    -1,    -1,    18,
      -1,    -1,    21,    22,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    44,    -1,    46,    -1,    48,
      -1,    50,    51,    52,    -1,    -1,    55,    56,    -1,    58,
      59,    60,    61,    -1,    63,    -1,    -1,    66,    -1,    68,
      69,    70,    -1,    72,    73,    74,    75,    -1,    77,    -1,
      79,    80,    81,    82,    83,    -1,    -1,    -1,    -1,    88,
      89,    90,    -1,    -1,    93,     3,     4,     5,    -1,    -1,
       8,   100,    10,    11,    -1,    -1,    -1,    15,    -1,    -1,
      18,    -1,    -1,    21,    22,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    44,    -1,    46,    -1,
      48,    -1,    50,    51,    52,    -1,    -1,    55,    56,    -1,
      58,    59,    60,    61,    62,    -1,    -1,    -1,    66,    -1,
      68,    69,    70,    -1,    72,    73,    74,    75,    -1,    -1,
      -1,    -1,    80,    81,    82,    83,    -1,    -1,    -1,    -1,
      88,    89,    90,    -1,    -1,    93,     3,     4,     5,    -1,
      -1,     8,   100,    10,    11,    -1,    -1,    -1,    15,    -1,
      -1,    18,    -1,    -1,    21,    22,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    -1,    46,
      -1,    48,    -1,    50,    51,    52,    -1,    -1,    55,    56,
      -1,    58,    59,    60,    61,    -1,    -1,    -1,    -1,    66,
      -1,    68,    69,    70,    -1,    72,    73,    74,    75,    -1,
      -1,    -1,    -1,    80,    81,    82,    83,    -1,    -1,    -1,
      -1,    88,    89,    90,    -1,    -1,    93,     3,     4,     5,
      -1,    -1,     8,   100,    10,    11,    -1,    -1,    -1,    15,
      -1,    -1,    18,    -1,    -1,    21,    22,     3,     4,     5,
      -1,    -1,     8,    -1,    10,    11,    -1,    -1,    -1,    15,
      -1,    -1,    18,    -1,    -1,    21,    22,    -1,    -1,    -1,
      46,    -1,    48,    -1,    50,    51,    52,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    -1,    -1,
      46,    -1,    48,    -1,    50,    51,    52,    -1,    74,    75,
      -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,    -1,    -1,
      -1,    -1,    88,    89,    -1,    -1,    -1,    93,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,    -1,    -1,     8,    -1,
      10,    11,    88,    89,    -1,    15,    -1,    93,    18,    -1,
      -1,    21,    22,     3,     4,     5,    -1,    -1,     8,    -1,
      10,    11,    -1,    -1,    -1,    15,    -1,    -1,    18,    -1,
      -1,    21,    22,    -1,    -1,    -1,    46,    -1,    48,    -1,
      50,    51,    52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    61,    62,    -1,    -1,    -1,    46,    -1,    48,    -1,
      50,    51,    52,    -1,    -1,    55,    -1,    -1,    -1,     3,
       4,     5,    -1,    -1,     8,    -1,    10,    11,    88,    89,
      -1,    15,    -1,    93,    18,    -1,    -1,    21,    22,     3,
       4,     5,    -1,    -1,     8,    -1,    10,    11,    88,    89,
      -1,    15,    -1,    93,    18,    -1,    -1,    21,    22,    -1,
      -1,    -1,    46,    -1,    48,    -1,    50,    51,    52,    53,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    -1,    48,    -1,    50,    51,    52,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,
      -1,    -1,    -1,    -1,    88,    89,    -1,    -1,    -1,    93,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,    -1,
       8,    -1,    10,    11,    88,    89,    -1,    15,    -1,    93,
      18,    -1,    -1,    21,    22,     3,     4,     5,    -1,    -1,
       8,    -1,    10,    11,    -1,    -1,    -1,    15,    -1,    -1,
      18,    -1,    -1,    21,    22,    -1,    -1,    -1,    46,    -1,
      48,    -1,    50,    51,    52,    -1,    54,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,
      48,    -1,    50,    51,    52,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,    -1,     8,    -1,    10,    11,
      88,    89,    -1,    15,    -1,    93,    18,    -1,    -1,    21,
      22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      88,    89,    -1,    91,    -1,    93,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    -1,    48,    -1,    50,    51,
      52,     3,     4,     5,    -1,    -1,     8,    -1,    10,    11,
      -1,    -1,    64,    15,    -1,    -1,    18,    -1,    -1,    21,
      22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    88,    89,    -1,    -1,
      -1,    93,    -1,    -1,    46,    -1,    48,    -1,    50,    51,
      52,     3,     4,     5,    -1,    -1,     8,    -1,    10,    11,
      -1,    -1,    -1,    15,    -1,    -1,    18,    -1,    -1,    21,
      22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    88,    89,    -1,    -1,
      -1,    93,    -1,    -1,    46,    -1,    48,    -1,    50,    51,
      52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    88,    89,    -1,    -1,
      -1,    93
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    84,    85,    87,   177,   178,    61,   176,     3,     4,
       5,     8,    10,    11,    15,    18,    21,    22,    46,    48,
      50,    51,    52,    88,    89,    93,   114,   117,   118,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   134,    44,    52,    55,    56,    58,
      59,    60,    66,    68,    69,    70,    72,    73,    74,    75,
      80,    81,    82,    83,    89,    90,   100,   108,   112,   134,
     135,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     158,   160,   161,   162,   164,   165,   166,   167,   168,   172,
     173,   174,   176,     0,    62,   162,   175,    48,    52,    89,
     119,   135,   149,   150,   120,   120,   120,   120,   120,   120,
     134,   120,   120,     6,     7,     4,    52,     6,     7,    48,
      50,    51,    53,    12,    13,    14,    15,    16,    17,    18,
      20,    19,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    86,   176,    64,    48,   162,
      48,    48,    52,    89,    52,    89,    52,    89,    52,    89,
      55,    55,   134,    52,    48,     3,     4,     5,     8,    10,
      11,    15,    18,    21,    22,    46,    48,    50,    51,    52,
      53,    88,    89,    91,    93,   104,   105,   106,   107,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,   195,    61,    55,    55,    15,
      52,   150,   152,   153,   154,   155,   157,    64,    64,    61,
      61,    55,    55,    55,    74,    75,   135,   158,   166,   167,
     162,   167,    55,   108,     1,    62,   162,   119,   134,    15,
     150,    49,    52,    52,    52,    52,   115,   116,   133,   135,
     134,   121,   121,   121,   122,   122,   122,   123,   124,   125,
     126,   126,   127,   127,   127,   127,   128,   128,   129,   130,
     131,   132,   132,   132,   132,   132,   132,   132,   132,   132,
     132,   132,   133,    45,   134,    59,   134,   134,   163,    55,
      55,   134,    48,    52,   119,    89,   182,   182,   182,   182,
     182,   182,   134,   182,   182,     4,    91,   102,   103,     6,
       7,    91,   105,   195,     4,    52,    64,    43,     6,     7,
      48,    50,    51,    12,    13,    14,    15,    16,    17,    18,
      20,    19,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,   104,    52,    55,    62,    71,    93,
      94,    95,    96,    97,    98,   108,   109,   113,    15,    52,
      55,    48,    32,    53,    48,    43,    89,    89,   142,   142,
      52,    52,   152,    55,    57,    55,    49,    49,    43,    49,
      15,    49,    54,   176,    49,    48,    49,    55,    49,   119,
     134,    49,    21,    22,    54,    52,    52,    64,   105,   106,
      52,    52,   116,   135,   183,   183,   183,   184,   184,   184,
     185,   186,   187,   188,   188,   189,   189,   189,   189,   190,
     190,   191,   192,   193,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,    64,    92,    52,   111,     4,
      52,   110,   133,   110,   110,   110,     4,    62,   109,    15,
      52,    65,   135,   156,   159,    61,   133,   137,    54,   134,
      15,   152,   157,    62,   135,   141,   147,   148,    62,    55,
      55,    48,   166,   167,   115,    15,    49,   134,   163,    61,
      49,    49,     4,    91,   133,   133,   105,    49,    49,    43,
      55,    64,    64,    43,    55,    55,    55,    55,    43,    52,
      49,   153,   155,    43,    49,    43,   133,   136,   137,    54,
      15,    52,   139,   140,   153,    55,    65,   159,    15,    49,
      49,    55,    62,    63,    77,    79,   162,   169,   170,   171,
      15,    15,    52,   133,     4,    55,   176,    65,   156,    55,
     176,    62,    43,    62,    15,    52,    49,    43,    52,   138,
      49,    43,    49,    49,   163,    64,   133,    64,    64,    62,
     170,    91,     4,    49,    62,   133,   137,    52,    49,    48,
     140,   133,    55,   176,    65,    55,   176,    49,    64,    65,
     171,    55,   176,    49,    48,   159,    49,   133,    78,    48,
     159,    43,    49,    55,   176,    64,    64,   159,    49,    49
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   101,   102,   102,   102,   102,   102,   103,   103,   103,
     104,   105,   105,   106,   106,   106,   106,   106,   106,   107,
     107,   107,   108,   109,   109,   109,   109,   110,   110,   109,
     109,   109,   109,   109,   111,   111,   109,   109,   109,   112,
     113,   113,   112,   114,   114,   114,   114,   114,   114,   114,
     114,   115,   115,   116,   116,   117,   117,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     119,   119,   120,   120,   120,   120,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   121,   121,   121,   121,   122,
     122,   122,   122,   123,   123,   124,   124,   125,   125,   126,
     126,   126,   127,   127,   127,   127,   127,   128,   128,   128,
     129,   129,   130,   130,   131,   131,   132,   132,   132,   132,
     132,   132,   132,   132,   132,   132,   132,   132,   133,   134,
     134,   135,   135,   135,   135,   136,   136,   136,   136,   137,
     137,   137,   138,   139,   139,   140,   140,   141,   142,   142,
     142,   142,   143,   144,   143,   144,   143,   144,   145,   146,
     145,   146,   147,   148,   148,   149,   150,   151,   152,   152,
     152,   152,   153,   154,   154,   154,   154,   153,   153,   153,
     153,   153,   155,   155,   156,   156,   157,   157,   157,   158,
     159,   159,   159,   160,   160,   160,   160,   160,   160,   160,
     160,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   161,   162,   162,   163,   163,   164,   164,   165,
     166,   166,   166,   167,   167,   167,   168,   169,   170,   170,
     170,   170,   170,   170,   171,   171,   172,   172,   173,   174,
     174,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     175,   175,   175,   176,   176,   177,   177,   178,   178,   178,
     178,   179,   179,   179,   179,   179,   179,   179,   179,   180,
     180,   181,   181,   181,   181,   181,   181,   181,   181,   181,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   183,   183,   183,   183,   184,   184,   184,
     184,   185,   185,   186,   186,   187,   187,   188,   188,   188,
     189,   189,   189,   189,   189,   190,   190,   190,   191,   191,
     192,   192,   193,   193,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   195
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     3,     5,     5,     1,     1,     3,     3,
       3,     1,     2,     1,     1,     1,     2,     4,     3,     1,
       3,     0,     2,     2,     2,     3,     3,     1,     3,     3,
       3,     3,     3,     1,     1,     3,     3,     1,     4,     3,
       1,     2,     4,     2,     2,     1,     1,     1,     1,     1,
       1,     0,     1,     3,     1,     1,     3,     1,     3,     3,
       3,     3,     4,     2,     2,     4,     4,     5,     6,     7,
       1,     2,     1,     2,     2,     4,     4,     2,     2,     2,
       2,     2,     2,     2,     2,     1,     3,     3,     3,     1,
       3,     3,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     3,     1,     3,     3,     3,     3,     1,     3,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     1,     3,
       1,     1,     1,     1,     2,     3,     3,     1,     1,     3,
       4,     3,     2,     3,     1,     1,     2,     2,     3,     2,
       2,     0,     1,     1,     2,     2,     2,     2,     1,     1,
       3,     3,     2,     3,     2,     4,     4,     4,     1,     2,
       3,     4,     1,     0,     1,     2,     3,     8,     9,    10,
       4,     3,     3,     3,     2,     2,     3,     1,     1,     2,
       1,     3,     0,     1,     1,     1,     1,     2,     2,     2,
       2,     6,     8,     6,     7,     9,     7,     6,     8,     6,
       7,     9,     7,     1,     1,     1,     0,     4,     8,     4,
       2,     4,     2,     1,     4,     2,     6,     5,     1,     2,
       3,     2,     5,     1,     2,     1,     7,     6,     1,     2,
       3,     1,     1,     2,     3,     1,     3,     2,     2,     1,
       1,     2,     1,     3,     2,     2,     1,     1,     4,     4,
       1,     2,     2,     3,     2,     0,     2,     1,     2,     3,
       2,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       3,     1,     3,     3,     3,     3,     2,     2,     4,     4,
       1,     2,     2,     4,     4,     2,     2,     2,     2,     2,
       2,     2,     2,     1,     3,     3,     3,     1,     3,     3,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     3,
       1,     3,     3,     3,     3,     1,     3,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = HC_HC_EMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == HC_HC_EMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use HC_HC_error or HC_HC_UNDEF. */
#define YYERRCODE HC_HC_UNDEF


/* Enable debugging if requested.  */
#if HC_DEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !HC_DEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !HC_DEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = HC_HC_EMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == HC_HC_EMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= HC_YYEOF)
    {
      yychar = HC_YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == HC_HC_error)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = HC_HC_UNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = HC_HC_EMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* sib_ib: REGISTER  */
#line 61 "HolyC.y"
                    {yyval=AppendToSIB(NULL,NULL,yyvsp[0],NULL);}
#line 1950 "HolyC.tab.c"
    break;

  case 3: /* sib_ib: sib_ib ADD REGISTER  */
#line 62 "HolyC.y"
                                         {
  yyval=AppendToSIB(yyvsp[-2],NULL,yyvsp[0],NULL);
  ReleaseAST(yyvsp[-1]);
}
#line 1959 "HolyC.tab.c"
    break;

  case 4: /* sib_ib: sib_ib ADD INT MUL REGISTER  */
#line 66 "HolyC.y"
                                                        {
  yyval=AppendToSIB(yyvsp[-4],yyvsp[-2],yyvsp[0],NULL);
  ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-1]);
}
#line 1968 "HolyC.tab.c"
    break;

  case 5: /* sib_ib: sib_ib ADD REGISTER MUL INT  */
#line 70 "HolyC.y"
                                                         {
  yyval=AppendToSIB(yyvsp[-4],yyvsp[0],yyvsp[-2],NULL);
  ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-1]);
}
#line 1977 "HolyC.tab.c"
    break;

  case 6: /* sib_ib: INT  */
#line 74 "HolyC.y"
                  {
  yyval=AppendToSIB(NULL,NULL,NULL,yyvsp[0]);
 }
#line 1985 "HolyC.tab.c"
    break;

  case 8: /* sib_d: sib_ib ADD expr  */
#line 78 "HolyC.y"
                                   {
  yyval=AppendToSIB(yyvsp[-2],NULL,NULL,yyvsp[0]);
  ReleaseAST(yyvsp[-1]);
}
#line 1994 "HolyC.tab.c"
    break;

  case 9: /* sib_d: sib_ib SUB expr  */
#line 82 "HolyC.y"
                                    {
  yyval=AppendToSIB(yyvsp[-2],NULL,NULL,yyvsp[0]);
  ReleaseAST(yyvsp[-1]);
}
#line 2003 "HolyC.tab.c"
    break;

  case 10: /* _sib: LEFT_SQAURE sib_d RIGHT_SQAURE  */
#line 86 "HolyC.y"
                                                      {
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
  yyval=yyvsp[-1];
}
#line 2012 "HolyC.tab.c"
    break;

  case 12: /* sib: _expr _sib  */
#line 91 "HolyC.y"
                            {
  yyval=AppendToSIB(yyvsp[0],NULL,NULL,yyvsp[-1]);
 }
#line 2020 "HolyC.tab.c"
    break;

  case 16: /* opc_operand: TYPENAME sib  */
#line 97 "HolyC.y"
                                 {
  yyval=yyvsp[0];
  CType *t=*map_get(&Compiler.types,yyvsp[-1]->name);
  yyval->asmAddr.width=TypeSize(t);
}
#line 2030 "HolyC.tab.c"
    break;

  case 17: /* opc_operand: TYPENAME REGISTER COLON sib  */
#line 102 "HolyC.y"
                                                     {
  CType *t=*map_get(&Compiler.types,yyvsp[-3]->name);
  yyvsp[0]->asmAddr.width=TypeSize(t);
  yyvsp[0]->asmAddr.segment=yyvsp[-2];
  yyval=yyvsp[0];
}
#line 2041 "HolyC.tab.c"
    break;

  case 18: /* opc_operand: opc_operand COLON sib  */
#line 108 "HolyC.y"
                                            {
  if(yyvsp[-2]->type!=AST_ASM_REG) RaiseError(yyvsp[-1],"Expected register.");
  else {
    yyvsp[0]->asmAddr.segment=yyvsp[-2];
  }
  yyval=yyvsp[0];
  ReleaseAST(yyvsp[-1]);
}
#line 2054 "HolyC.tab.c"
    break;

  case 19: /* opc_operands: opc_operand  */
#line 116 "HolyC.y"
                                {yyval=yyvsp[0];}
#line 2060 "HolyC.tab.c"
    break;

  case 20: /* opc_operands: opc_operands COMMA opc_operand  */
#line 117 "HolyC.y"
                                                            {
  yyval=CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA);
  ReleaseAST(yyvsp[-1]);
}
#line 2069 "HolyC.tab.c"
    break;

  case 21: /* opc_operands: %empty  */
#line 121 "HolyC.y"
                 {yyval=NULL;}
#line 2075 "HolyC.tab.c"
    break;

  case 22: /* opcode: OPCODE opc_operands  */
#line 122 "HolyC.y"
                                     {
  AST *r=TD_MALLOC(sizeof(AST));
  r->refCnt=1;
  r->type=AST_ASM_OPCODE;
  r->asmOpcode.name=yyvsp[-1];
  r->asmOpcode.operands=CommaToVec(yyvsp[0]);
  r->labelContext=Compiler.labelContext;
  yyval=SOT(r,yyvsp[-1]);
}
#line 2089 "HolyC.tab.c"
    break;

  case 23: /* asm_blk_stmt: NAME DOUBLE_COLON  */
#line 131 "HolyC.y"
                                         {
  yyval=SOT(CreateExportedLabel(yyvsp[-1]),yyvsp[-1]);
  yyval->inAsmBlk=1;
  ReleaseAST(yyvsp[0]);
}
#line 2099 "HolyC.tab.c"
    break;

  case 24: /* asm_blk_stmt: NAME COLON  */
#line 136 "HolyC.y"
                                  {
  yyval=SOT(CreateLabel(yyvsp[-1]),yyvsp[-1]);
  yyval->inAsmBlk=1;
  ReleaseAST(yyvsp[0]);
 }
#line 2109 "HolyC.tab.c"
    break;

  case 25: /* asm_blk_stmt: DOUBLE_AT NAME COLON  */
#line 141 "HolyC.y"
                                                {
  yyval=SOT(CreateLocalLabel(yyvsp[-1]),yyvsp[-2]);
  yyval->inAsmBlk=1;
  ReleaseAST(yyvsp[0]),ReleaseAST(yyvsp[-2]);
}
#line 2119 "HolyC.tab.c"
    break;

  case 26: /* asm_blk_stmt: DOUBLE_AT INT COLON  */
#line 146 "HolyC.y"
                                               {
  //Make a name token out of $n
  char buffer[256];
  sprintf(buffer,"%li",yyvsp[-1]->integer);
  AST *n=TD_MALLOC(sizeof(AST));
  n->type=AST_NAME;
  n->name=strdup(buffer);
  yyval=SOT(CreateLocalLabel(SOT(n,yyvsp[-1])),yyvsp[-2]);
  yyval->inAsmBlk=1;
  ReleaseAST(yyvsp[0]),ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[-1]);
}
#line 2135 "HolyC.tab.c"
    break;

  case 28: /* data_exprs: data_exprs COMMA expr  */
#line 159 "HolyC.y"
                                                {
  yyval=CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA);
  ReleaseAST(yyvsp[-1]);
}
#line 2144 "HolyC.tab.c"
    break;

  case 29: /* asm_blk_stmt: DU8 data_exprs SEMI  */
#line 163 "HolyC.y"
                                                {
  yyval=SOT(CreateDU8(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2153 "HolyC.tab.c"
    break;

  case 30: /* asm_blk_stmt: DU16 data_exprs SEMI  */
#line 167 "HolyC.y"
                                                 {
  yyval=SOT(CreateDU16(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2162 "HolyC.tab.c"
    break;

  case 31: /* asm_blk_stmt: DU32 data_exprs SEMI  */
#line 171 "HolyC.y"
                                                 {
  yyval=SOT(CreateDU32(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2171 "HolyC.tab.c"
    break;

  case 32: /* asm_blk_stmt: DU64 data_exprs SEMI  */
#line 175 "HolyC.y"
                                                 {
  yyval=SOT(CreateDU64(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2180 "HolyC.tab.c"
    break;

  case 35: /* name_list: name_list COMMA NAME  */
#line 181 "HolyC.y"
                                             {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA),yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
}
#line 2189 "HolyC.tab.c"
    break;

  case 36: /* asm_blk_stmt: ASM_IMPORT name_list SEMI  */
#line 185 "HolyC.y"
                                                       {
  yyval=SOT(CreateAsmImport(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2198 "HolyC.tab.c"
    break;

  case 37: /* asm_blk_stmt: SEMI  */
#line 189 "HolyC.y"
                          {
  yyval=SOT(CreateNop(),yyvsp[0]);
  ReleaseAST(yyvsp[0]);
 }
#line 2207 "HolyC.tab.c"
    break;

  case 38: /* asm_blk_stmt: ALIGN INT COMMA INT  */
#line 193 "HolyC.y"
                                                           {
  yyval=SOT(CreateAsmAlign(yyvsp[-2],yyvsp[0]),yyvsp[-3]);
  ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-3]);
}
#line 2216 "HolyC.tab.c"
    break;

  case 39: /* asm_blk: ASM LEFT_CURLY RIGHT_CURLY  */
#line 197 "HolyC.y"
                                                   {
  yyval=SOT(CreateNop(),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 2225 "HolyC.tab.c"
    break;

  case 41: /* asm_blk_stmts: asm_blk_stmts asm_blk_stmt  */
#line 202 "HolyC.y"
                                                   {
  yyval=AppendToStmts(yyvsp[-1],yyvsp[0]);
}
#line 2233 "HolyC.tab.c"
    break;

  case 42: /* asm_blk: ASM LEFT_CURLY asm_blk_stmts RIGHT_CURLY  */
#line 205 "HolyC.y"
                                                                    {
  yyval=yyvsp[-1];
  yyval->type=AST_ASM_BLK;
  ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2243 "HolyC.tab.c"
    break;

  case 43: /* expr0: DOUBLE_AT NAME  */
#line 211 "HolyC.y"
                            {
  yyval=SOT(yyvsp[0],yyvsp[-1]);
  char buffer[256];
  sprintf(buffer,"@@%s",yyval->name);
  TD_FREE(yyval->name);
  yyval->name=strdup(buffer);
  ReleaseAST(yyvsp[-1]);
}
#line 2256 "HolyC.tab.c"
    break;

  case 44: /* expr0: DOUBLE_AT INT  */
#line 219 "HolyC.y"
                              {
//Make a name token out of $n
  char buffer[256];
  sprintf(buffer,"@@%li",yyvsp[0]->integer);
  AST *n=TD_MALLOC(sizeof(AST));
  n->type=AST_NAME,n->name=strdup(buffer);
  yyval=n;
  ReleaseAST(yyvsp[-1]);
}
#line 2270 "HolyC.tab.c"
    break;

  case 45: /* expr0: FLOAT  */
#line 228 "HolyC.y"
                {yyval=SLE(yyvsp[0]);}
#line 2276 "HolyC.tab.c"
    break;

  case 46: /* expr0: INT  */
#line 229 "HolyC.y"
              {yyval=SLE(yyvsp[0]);}
#line 2282 "HolyC.tab.c"
    break;

  case 47: /* expr0: CHAR  */
#line 230 "HolyC.y"
               {yyval=SLE(yyvsp[0]);}
#line 2288 "HolyC.tab.c"
    break;

  case 48: /* expr0: STRING  */
#line 231 "HolyC.y"
                 {yyval=SLE(yyvsp[0]);}
#line 2294 "HolyC.tab.c"
    break;

  case 49: /* expr0: LASTCLASS  */
#line 232 "HolyC.y"
                    {
  yyval=SLE(yyvsp[0]);
}
#line 2302 "HolyC.tab.c"
    break;

  case 50: /* expr0: NAME  */
#line 235 "HolyC.y"
               {
  yyval=SLE(yyvsp[0]);
}
#line 2310 "HolyC.tab.c"
    break;

  case 51: /* _callargs: %empty  */
#line 238 "HolyC.y"
              {yyval=NULL;}
#line 2316 "HolyC.tab.c"
    break;

  case 52: /* _callargs: expr  */
#line 239 "HolyC.y"
                      {yyval=yyvsp[0];}
#line 2322 "HolyC.tab.c"
    break;

  case 53: /* callargs: callargs COMMA _callargs  */
#line 240 "HolyC.y"
                                               {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA),yyvsp[-1]);
}
#line 2330 "HolyC.tab.c"
    break;

  case 54: /* callargs: _callargs  */
#line 243 "HolyC.y"
                          {yyval=yyvsp[0];}
#line 2336 "HolyC.tab.c"
    break;

  case 56: /* expr1: LEFT_PAREN expr_comma RIGHT_PAREN  */
#line 245 "HolyC.y"
                                                         {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2345 "HolyC.tab.c"
    break;

  case 58: /* expr2: TYPENAME DOT NAME  */
#line 254 "HolyC.y"
                                     {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2354 "HolyC.tab.c"
    break;

  case 59: /* expr2: TYPENAME ARROW NAME  */
#line 258 "HolyC.y"
                                       {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2363 "HolyC.tab.c"
    break;

  case 60: /* expr2: expr2 DOT NAME  */
#line 262 "HolyC.y"
                                  {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2372 "HolyC.tab.c"
    break;

  case 61: /* expr2: expr2 ARROW NAME  */
#line 266 "HolyC.y"
                                    {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2381 "HolyC.tab.c"
    break;

  case 62: /* expr2: expr2 LEFT_SQAURE expr_comma RIGHT_SQAURE  */
#line 270 "HolyC.y"
                                                                    {
  yyval=SOT(CreateArrayAccess(yyvsp[-3],yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2390 "HolyC.tab.c"
    break;

  case 63: /* expr2: expr2 DEC  */
#line 274 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[-1],AST_POST_DEC),yyvsp[0]);
  ReleaseAST(yyvsp[0]);
}
#line 2399 "HolyC.tab.c"
    break;

  case 64: /* expr2: expr2 INC  */
#line 278 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[-1],AST_POST_INC),yyvsp[0]);
  ReleaseAST(yyvsp[0]);
}
#line 2408 "HolyC.tab.c"
    break;

  case 65: /* expr2: expr2 LEFT_PAREN callargs RIGHT_PAREN  */
#line 282 "HolyC.y"
                                                                {
  yyval=SOT(CreateFuncCall(yyvsp[-3],yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2417 "HolyC.tab.c"
    break;

  case 66: /* expr2: expr2 LEFT_PAREN primtype0 RIGHT_PAREN  */
#line 286 "HolyC.y"
                                                                 {
  yyval=CreateExplicitTypecast(yyvsp[-3],yyvsp[-1]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2426 "HolyC.tab.c"
    break;

  case 67: /* expr2: expr2 LEFT_PAREN primtype0 MUL RIGHT_PAREN  */
#line 290 "HolyC.y"
                                                                          {
  yyvsp[-2]->type2=CreateMultiLvlPtr(yyvsp[-2]->type2,1);
  yyval=CreateExplicitTypecast(yyvsp[-4],yyvsp[-2]);
  ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 2436 "HolyC.tab.c"
    break;

  case 68: /* expr2: expr2 LEFT_PAREN primtype0 MUL MUL RIGHT_PAREN  */
#line 295 "HolyC.y"
                                                                                   {
  yyvsp[-3]->type2=CreateMultiLvlPtr(yyvsp[-3]->type2,2);
  yyval=CreateExplicitTypecast(yyvsp[-5],yyvsp[-3]);
  ReleaseAST(yyvsp[-4]),ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 2446 "HolyC.tab.c"
    break;

  case 69: /* expr2: expr2 LEFT_PAREN primtype0 MUL MUL MUL RIGHT_PAREN  */
#line 300 "HolyC.y"
                                                                                            {
  yyvsp[-4]->type2=CreateMultiLvlPtr(yyvsp[-4]->type2,3);
  yyval=CreateExplicitTypecast(yyvsp[-6],yyvsp[-4]);
  ReleaseAST(yyvsp[-5]),ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 2456 "HolyC.tab.c"
    break;

  case 70: /* sizeof_type: primtype0  */
#line 308 "HolyC.y"
                             {yyval=yyvsp[0];}
#line 2462 "HolyC.tab.c"
    break;

  case 71: /* sizeof_type: sizeof_type MUL  */
#line 309 "HolyC.y"
                                        {
  CType *ptr =CreateMultiLvlPtr(yyvsp[-1]->type2,1);
  yyval=CreateTypeNode(ptr);
  ReleaseAST(yyvsp[0]);
}
#line 2472 "HolyC.tab.c"
    break;

  case 73: /* expr3: SIZEOF NAME  */
#line 315 "HolyC.y"
                            {
  yyval=SOT(CreateSizeof(yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2481 "HolyC.tab.c"
    break;

  case 74: /* expr3: SIZEOF sizeof_type  */
#line 319 "HolyC.y"
                                   {
  yyval=SOT(CreateSizeof(yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2490 "HolyC.tab.c"
    break;

  case 75: /* expr3: SIZEOF LEFT_PAREN expr_comma RIGHT_PAREN  */
#line 323 "HolyC.y"
                                                                    {
  yyval=SOT(CreateSizeof(yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 2501 "HolyC.tab.c"
    break;

  case 76: /* expr3: SIZEOF LEFT_PAREN sizeof_type RIGHT_PAREN  */
#line 329 "HolyC.y"
                                                                     {
  yyval=SOT(CreateSizeof(yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 2512 "HolyC.tab.c"
    break;

  case 77: /* expr3: BAND expr3  */
#line 335 "HolyC.y"
                           {
  yyval=SOT(CreateUnop(yyvsp[0],AST_ADDROF),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2521 "HolyC.tab.c"
    break;

  case 78: /* expr3: MUL expr3  */
#line 339 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_DERREF),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2530 "HolyC.tab.c"
    break;

  case 79: /* expr3: DEC expr3  */
#line 344 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_PRE_DEC),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2539 "HolyC.tab.c"
    break;

  case 80: /* expr3: INC expr3  */
#line 348 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_PRE_INC),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2548 "HolyC.tab.c"
    break;

  case 81: /* expr3: ADD expr3  */
#line 352 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_POS),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2557 "HolyC.tab.c"
    break;

  case 82: /* expr3: SUB expr3  */
#line 356 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_NEG),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2566 "HolyC.tab.c"
    break;

  case 83: /* expr3: LNOT expr3  */
#line 360 "HolyC.y"
                           {
  yyval=SOT(CreateUnop(yyvsp[0],AST_LNOT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2575 "HolyC.tab.c"
    break;

  case 84: /* expr3: BNOT expr3  */
#line 364 "HolyC.y"
                           {
  yyval=SOT(CreateUnop(yyvsp[0],AST_BNOT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2584 "HolyC.tab.c"
    break;

  case 86: /* expr4: expr4 POW expr4  */
#line 371 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_POW),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2593 "HolyC.tab.c"
    break;

  case 87: /* expr4: expr4 SHL expr4  */
#line 375 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SHL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2602 "HolyC.tab.c"
    break;

  case 88: /* expr4: expr4 SHR expr4  */
#line 379 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SHR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2611 "HolyC.tab.c"
    break;

  case 90: /* expr4_5: expr4_5 MUL expr4_5  */
#line 385 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_MUL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2620 "HolyC.tab.c"
    break;

  case 91: /* expr4_5: expr4_5 DIV expr4_5  */
#line 389 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_DIV),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2629 "HolyC.tab.c"
    break;

  case 92: /* expr4_5: expr4_5 MOD expr4_5  */
#line 393 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_MOD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2638 "HolyC.tab.c"
    break;

  case 94: /* expr5: expr5 BAND expr5  */
#line 399 "HolyC.y"
                                    {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2647 "HolyC.tab.c"
    break;

  case 96: /* expr6: expr6 BXOR expr6  */
#line 405 "HolyC.y"
                                    {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2656 "HolyC.tab.c"
    break;

  case 98: /* expr7: expr7 BOR expr7  */
#line 411 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2665 "HolyC.tab.c"
    break;

  case 100: /* expr8: expr8 ADD expr8  */
#line 417 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ADD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2674 "HolyC.tab.c"
    break;

  case 101: /* expr8: expr8 SUB expr8  */
#line 421 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SUB),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2683 "HolyC.tab.c"
    break;

  case 103: /* expr9: expr9 LT expr9  */
#line 427 "HolyC.y"
                                  {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_LT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2692 "HolyC.tab.c"
    break;

  case 104: /* expr9: expr9 GT expr9  */
#line 431 "HolyC.y"
                                  {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_GT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2701 "HolyC.tab.c"
    break;

  case 105: /* expr9: expr9 LE expr9  */
#line 435 "HolyC.y"
                                  {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_LE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2710 "HolyC.tab.c"
    break;

  case 106: /* expr9: expr9 GE expr9  */
#line 439 "HolyC.y"
                                  {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_GE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2719 "HolyC.tab.c"
    break;

  case 108: /* expr10: expr10 EQ expr10  */
#line 445 "HolyC.y"
                                     {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_EQ),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2728 "HolyC.tab.c"
    break;

  case 109: /* expr10: expr10 NE expr10  */
#line 449 "HolyC.y"
                                     {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_NE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2737 "HolyC.tab.c"
    break;

  case 111: /* expr11: expr11 LAND expr11  */
#line 455 "HolyC.y"
                                       {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2746 "HolyC.tab.c"
    break;

  case 113: /* expr12: expr12 LXOR expr12  */
#line 461 "HolyC.y"
                                       {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2755 "HolyC.tab.c"
    break;

  case 115: /* expr13: expr13 LOR expr13  */
#line 467 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2764 "HolyC.tab.c"
    break;

  case 117: /* expr14: expr14 ASSIGN expr14  */
#line 473 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2773 "HolyC.tab.c"
    break;

  case 118: /* expr14: expr14 EQ_SHL expr14  */
#line 477 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SHL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2782 "HolyC.tab.c"
    break;

  case 119: /* expr14: expr14 EQ_SHR expr14  */
#line 481 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SHR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2791 "HolyC.tab.c"
    break;

  case 120: /* expr14: expr14 EQ_MUL expr14  */
#line 485 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_MUL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2800 "HolyC.tab.c"
    break;

  case 121: /* expr14: expr14 EQ_DIV expr14  */
#line 489 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_DIV),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2809 "HolyC.tab.c"
    break;

  case 122: /* expr14: expr14 EQ_MOD expr14  */
#line 493 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_MOD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2818 "HolyC.tab.c"
    break;

  case 123: /* expr14: expr14 EQ_BAND expr14  */
#line 497 "HolyC.y"
                                          {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2827 "HolyC.tab.c"
    break;

  case 124: /* expr14: expr14 EQ_BXOR expr14  */
#line 501 "HolyC.y"
                                          {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2836 "HolyC.tab.c"
    break;

  case 125: /* expr14: expr14 EQ_BOR expr14  */
#line 505 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2845 "HolyC.tab.c"
    break;

  case 126: /* expr14: expr14 EQ_ADD expr14  */
#line 509 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_ADD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2854 "HolyC.tab.c"
    break;

  case 127: /* expr14: expr14 EQ_SUB expr14  */
#line 513 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SUB),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2863 "HolyC.tab.c"
    break;

  case 129: /* expr_comma: expr_comma COMMA expr  */
#line 520 "HolyC.y"
                                              {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2872 "HolyC.tab.c"
    break;

  case 131: /* primtype0: TYPENAME  */
#line 529 "HolyC.y"
                          {
  CType **cls=map_get(&Compiler.types,yyvsp[0]->name);
  CType *t ;
  if(cls) t=*cls;
  else {
    RaiseError(yyvsp[0],"Invalid type \"%s\"",yyvsp[0]->name);
    t=CreatePrimType(TYPE_I64);
  }
  yyval=CreateTypeNode(t);
}
#line 2887 "HolyC.tab.c"
    break;

  case 132: /* primtype0: _class  */
#line 539 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 2893 "HolyC.tab.c"
    break;

  case 133: /* primtype0: _union  */
#line 540 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 2899 "HolyC.tab.c"
    break;

  case 134: /* primtype0: primtype0 _union  */
#line 541 "HolyC.y"
                                      {
  yyval=yyvsp[0];
  AssignUnionBasetype(yyvsp[0],yyvsp[-1]);
}
#line 2908 "HolyC.tab.c"
    break;

  case 135: /* _arrlit: _arrlit COMMA expr  */
#line 548 "HolyC.y"
                                          {yyval=AppendToArrLiteral(yyvsp[-2],yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 2914 "HolyC.tab.c"
    break;

  case 136: /* _arrlit: _arrlit COMMA arrlit  */
#line 549 "HolyC.y"
                                            {yyval=AppendToArrLiteral(yyvsp[-2],yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 2920 "HolyC.tab.c"
    break;

  case 137: /* _arrlit: expr  */
#line 550 "HolyC.y"
              {yyval=AppendToArrLiteral(NULL,yyvsp[0]);}
#line 2926 "HolyC.tab.c"
    break;

  case 138: /* _arrlit: arrlit  */
#line 551 "HolyC.y"
                {yyval=AppendToArrLiteral(NULL,yyvsp[0]);}
#line 2932 "HolyC.tab.c"
    break;

  case 139: /* arrlit: LEFT_CURLY _arrlit RIGHT_CURLY  */
#line 552 "HolyC.y"
                                                       {
  yyval=SOT(yyvsp[-1],yyvsp[-2]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 2942 "HolyC.tab.c"
    break;

  case 140: /* arrlit: LEFT_CURLY _arrlit COMMA RIGHT_CURLY  */
#line 557 "HolyC.y"
                                                                 {
  yyval=SOT(yyvsp[-2],yyvsp[-3]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 2953 "HolyC.tab.c"
    break;

  case 141: /* arrlit: LEFT_CURLY COMMA RIGHT_CURLY  */
#line 563 "HolyC.y"
                                                      {
  yyval=NULL;
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 2964 "HolyC.tab.c"
    break;

  case 142: /* metadata: NAME expr  */
#line 572 "HolyC.y"
                             {
  yyval=AppendToMetaData(NULL,yyvsp[-1],yyvsp[0]);
}
#line 2972 "HolyC.tab.c"
    break;

  case 143: /* cdecltail: cdecltail COMMA _cdecltail  */
#line 575 "HolyC.y"
                                                    {
  assert(yyvsp[0]->type==__AST_DECL_TAILS);
  CDeclTail b2=vec_last(&yyvsp[0]->declTail);
  AppendToDecls(yyvsp[-2],b2.basetype,b2.finalType,b2.name,b2.dft,b2.metaData);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 2984 "HolyC.tab.c"
    break;

  case 144: /* cdecltail: _cdecltail  */
#line 582 "HolyC.y"
                            {yyval=yyvsp[0];}
#line 2990 "HolyC.tab.c"
    break;

  case 145: /* _cdecltail: vardecltail  */
#line 583 "HolyC.y"
                              {
  AST *t=yyvsp[0];
  yyval=yyvsp[0];
}
#line 2999 "HolyC.tab.c"
    break;

  case 146: /* _cdecltail: vardecltail metadata  */
#line 587 "HolyC.y"
                                          {
  AST *meta=yyvsp[-1]->declTail.data[0].metaData;
  meta=AppendToMetaData(meta,yyvsp[0]->metaData.data[0].name,yyvsp[0]->metaData.data[0].value);
  yyvsp[-1]->declTail.data[0].metaData=meta;
  yyval=yyvsp[-1];
}
#line 3010 "HolyC.tab.c"
    break;

  case 147: /* cdecl: primtype0 cdecltail  */
#line 593 "HolyC.y"
                                    {
  DeclsFillInBasetype(yyvsp[-1]->type2,yyvsp[0]);
  yyval=yyvsp[0];
}
#line 3019 "HolyC.tab.c"
    break;

  case 148: /* cbody: cbody cdecl SEMI  */
#line 597 "HolyC.y"
                                      {
 yyval=AppendToStmts(yyvsp[-2],yyvsp[-1]);
 ReleaseAST(yyvsp[0]);
}
#line 3028 "HolyC.tab.c"
    break;

  case 149: /* cbody: cbody union  */
#line 601 "HolyC.y"
                            {
 yyval=AppendToStmts(yyvsp[-1],yyvsp[0]);
}
#line 3036 "HolyC.tab.c"
    break;

  case 150: /* cbody: cbody class  */
#line 604 "HolyC.y"
                            {
 yyval=AppendToStmts(yyvsp[-1],yyvsp[0]);
}
#line 3044 "HolyC.tab.c"
    break;

  case 151: /* cbody: %empty  */
#line 607 "HolyC.y"
       {yyval=NULL;}
#line 3050 "HolyC.tab.c"
    break;

  case 152: /* _cheader: CLASS  */
#line 608 "HolyC.y"
                        {
  CType *t=CreateClassForwardDecl(NULL,NULL);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[0]);
}
#line 3060 "HolyC.tab.c"
    break;

  case 153: /* _uheader: UNION  */
#line 613 "HolyC.y"
                        {
  CType *t=CreateUnionForwardDecl(NULL,NULL);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[0]);
}
#line 3070 "HolyC.tab.c"
    break;

  case 154: /* _cheader: CLASS NAME  */
#line 618 "HolyC.y"
                                {
  CType *t=CreateClassForwardDecl(NULL,yyvsp[0]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-1]);
}
#line 3080 "HolyC.tab.c"
    break;

  case 155: /* _uheader: UNION NAME  */
#line 623 "HolyC.y"
                                {
  CType *t=CreateUnionForwardDecl(NULL,yyvsp[0]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-1]);
}
#line 3090 "HolyC.tab.c"
    break;

  case 156: /* _cheader: CLASS TYPENAME  */
#line 628 "HolyC.y"
                                    {
  CType *t=CreateClassForwardDecl(NULL,yyvsp[0]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-1]);
}
#line 3100 "HolyC.tab.c"
    break;

  case 157: /* _uheader: UNION TYPENAME  */
#line 633 "HolyC.y"
                               {
  CType *t=CreateUnionForwardDecl(NULL,yyvsp[0]);
  yyval=CreateTypeNode(t);
}
#line 3109 "HolyC.tab.c"
    break;

  case 158: /* cheader: _cheader  */
#line 637 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 3115 "HolyC.tab.c"
    break;

  case 159: /* uheader: _uheader  */
#line 638 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 3121 "HolyC.tab.c"
    break;

  case 160: /* cheader: _cheader COLON TYPENAME  */
#line 639 "HolyC.y"
                                                 {
  CType *t=*map_get(&Compiler.types,yyvsp[0]->name);
  InheritFromType(yyvsp[-2]->type2,t);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 3132 "HolyC.tab.c"
    break;

  case 161: /* uheader: _uheader COLON TYPENAME  */
#line 645 "HolyC.y"
                                                 {
  CType *t=*map_get(&Compiler.types,yyvsp[0]->name);
  InheritFromType(yyvsp[-2]->type2,t);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 3143 "HolyC.tab.c"
    break;

  case 162: /* class: _class SEMI  */
#line 651 "HolyC.y"
                              {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[0]);
}
#line 3152 "HolyC.tab.c"
    break;

  case 163: /* union: primtype0 _union SEMI  */
#line 655 "HolyC.y"
                                            {
  yyval=yyvsp[-1];
  AssignUnionBasetype(yyvsp[-1],yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3162 "HolyC.tab.c"
    break;

  case 164: /* union: _union SEMI  */
#line 660 "HolyC.y"
                              {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[0]);
}
#line 3171 "HolyC.tab.c"
    break;

  case 165: /* _class: cheader LEFT_CURLY cbody RIGHT_CURLY  */
#line 665 "HolyC.y"
                                                                {
  if(yyvsp[-1]) {
    int iter;
    AST *decl;
    assert(yyvsp[-1]->type==AST_STMT_GROUP);
    vec_foreach(&yyvsp[-1]->stmts,decl,iter)
      AppendToTypeMembers(yyvsp[-3],decl);
  }
  yyvsp[-3]->type2=FinalizeClass(yyvsp[-3]->type2);
  yyvsp[-3]->type2->cls.isFwd=0;
  AST *t=yyvsp[-3];
  yyval=yyvsp[-3];
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3191 "HolyC.tab.c"
    break;

  case 166: /* _union: uheader LEFT_CURLY cbody RIGHT_CURLY  */
#line 680 "HolyC.y"
                                                                {
  if(yyvsp[-1]) {
    int iter;
    AST *decl;
    vec_foreach(&yyvsp[-1]->stmts,decl,iter)
      AppendToTypeMembers(yyvsp[-3],decl);
  }
  yyvsp[-3]->type2->un.isFwd=0;
  yyval=yyvsp[-3];
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3208 "HolyC.tab.c"
    break;

  case 167: /* tryblock: TRY scope CATCH scope  */
#line 696 "HolyC.y"
                                                   {
  yyval=CreateTry(yyvsp[-2],yyvsp[0]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-1]);
}
#line 3218 "HolyC.tab.c"
    break;

  case 168: /* namewptrs: NAME  */
#line 704 "HolyC.y"
                   {
  CType *t =CreatePrimType(TYPE_I64);
  yyval=SOT(AppendToDecls(NULL,t,t,yyvsp[0],NULL,NULL),yyvsp[0]);
}
#line 3227 "HolyC.tab.c"
    break;

  case 169: /* namewptrs: MUL NAME  */
#line 708 "HolyC.y"
                            {
  CType *t =CreatePrimType(TYPE_I64);
  CType *ptr =CreateMultiLvlPtr(t,1);
  yyval=SOT(AppendToDecls(NULL,t,ptr,yyvsp[0],NULL,NULL),yyvsp[0]);
  ReleaseAST(yyvsp[-1]);
}
#line 3238 "HolyC.tab.c"
    break;

  case 170: /* namewptrs: MUL MUL NAME  */
#line 714 "HolyC.y"
                                     {
  CType *t =CreatePrimType(TYPE_I64);
  CType *ptr =CreateMultiLvlPtr(t,2);
  yyval=SOT(AppendToDecls(NULL,t,ptr,yyvsp[0],NULL,NULL),yyvsp[0]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
}
#line 3250 "HolyC.tab.c"
    break;

  case 171: /* namewptrs: MUL MUL MUL NAME  */
#line 721 "HolyC.y"
                                              {
  CType *t =CreatePrimType(TYPE_I64);
  CType *ptr =CreateMultiLvlPtr(t,3);
  yyval=SOT(AppendToDecls(NULL,t,ptr,yyvsp[0],NULL,NULL),yyvsp[0]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
}
#line 3263 "HolyC.tab.c"
    break;

  case 173: /* __ptrcount: %empty  */
#line 730 "HolyC.y"
            {
    yyval=CreateI64(0);
}
#line 3271 "HolyC.tab.c"
    break;

  case 174: /* __ptrcount: MUL  */
#line 733 "HolyC.y"
                     {
  yyval=CreateI64(1);
  ReleaseAST(yyvsp[0]);
}
#line 3280 "HolyC.tab.c"
    break;

  case 175: /* __ptrcount: MUL MUL  */
#line 737 "HolyC.y"
                              {
  yyval=CreateI64(2);
  ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 3289 "HolyC.tab.c"
    break;

  case 176: /* __ptrcount: MUL MUL MUL  */
#line 741 "HolyC.y"
                                       {
  yyval=CreateI64(3);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 3298 "HolyC.tab.c"
    break;

  case 177: /* vardecltail: __ptrcount LEFT_PAREN MUL NAME RIGHT_PAREN LEFT_PAREN funcargs RIGHT_PAREN  */
#line 745 "HolyC.y"
                                                                                                                               {
  CType *bt =CreatePrimType(TYPE_I64);
  CType *btp=CreateMultiLvlPtr(bt,yyvsp[-7]->integer);
  CType *func =CreateFuncType(btp,yyvsp[-1],0);
  CType *ptr =CreatePtrType(func);
  yyval=SOT(AppendToDecls(NULL,bt,ptr,yyvsp[-4],NULL,NULL),yyvsp[-4]);
  ReleaseAST(yyvsp[-6]),ReleaseAST(yyvsp[-7]);
  ReleaseAST(yyvsp[-5]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3315 "HolyC.tab.c"
    break;

  case 178: /* vardecltail: __ptrcount LEFT_PAREN MUL MUL NAME RIGHT_PAREN LEFT_PAREN funcargs RIGHT_PAREN  */
#line 757 "HolyC.y"
                                                                                                                                        {
  CType *bt =CreatePrimType(TYPE_I64);
  CType *btp=CreateMultiLvlPtr(bt,yyvsp[-8]->integer);
  CType *func =CreateFuncType(btp,yyvsp[-1],0);
  CType *ptr =CreateMultiLvlPtr(func,2);
  yyval=SOT(AppendToDecls(NULL,bt,ptr,yyvsp[-4],NULL,NULL),yyvsp[-4]);
  ReleaseAST(yyvsp[-7]),ReleaseAST(yyvsp[-8]);
  ReleaseAST(yyvsp[-6]);
  ReleaseAST(yyvsp[-5]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3333 "HolyC.tab.c"
    break;

  case 179: /* vardecltail: __ptrcount LEFT_PAREN MUL MUL MUL NAME RIGHT_PAREN LEFT_PAREN funcargs RIGHT_PAREN  */
#line 770 "HolyC.y"
                                                                                                                                                 {
  CType *bt =CreatePrimType(TYPE_I64);
  CType *btp=CreateMultiLvlPtr(bt,yyvsp[-9]->integer);
  CType *func =CreateFuncType(btp,yyvsp[-1],0);
  CType *ptr =CreateMultiLvlPtr(func,3);
  yyval=SOT(AppendToDecls(NULL,bt,ptr,yyvsp[-4],NULL,NULL),yyvsp[-4]);
  ReleaseAST(yyvsp[-8]),ReleaseAST(yyvsp[-9]);
  ReleaseAST(yyvsp[-7]);
  ReleaseAST(yyvsp[-6]);
  ReleaseAST(yyvsp[-5]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3352 "HolyC.tab.c"
    break;

  case 180: /* vardecltail: vardecltail LEFT_SQAURE expr_comma RIGHT_SQAURE  */
#line 784 "HolyC.y"
                                                                                     {
  assert(yyvsp[-3]->type==__AST_DECL_TAILS);
  CType *bt=vec_last(&yyvsp[-3]->declTail).finalType;
  vec_last(&yyvsp[-3]->declTail).finalType=CreateArrayType(bt,yyvsp[-1]);
  yyval=yyvsp[-3];
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3365 "HolyC.tab.c"
    break;

  case 181: /* vardecltail: vardecltail LEFT_SQAURE RIGHT_SQAURE  */
#line 792 "HolyC.y"
                                                                     {
  assert(yyvsp[-2]->type==__AST_DECL_TAILS);
  CType *bt=vec_last(&yyvsp[-2]->declTail).finalType;
  vec_last(&yyvsp[-2]->declTail).finalType=CreateArrayType(bt,NULL);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3378 "HolyC.tab.c"
    break;

  case 182: /* vardecltail_asn: vardecltail ASSIGN expr  */
#line 801 "HolyC.y"
                                                       {
  assert(yyvsp[-2]->type==__AST_DECL_TAILS);
  vec_last(&yyvsp[-2]->declTail).dft=yyvsp[0];
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 3389 "HolyC.tab.c"
    break;

  case 183: /* vardecltail_asn: vardecltail ASSIGN arrlit  */
#line 807 "HolyC.y"
                                                         {
  assert(yyvsp[-2]->type==__AST_DECL_TAILS);
  vec_last(&yyvsp[-2]->declTail).dft=yyvsp[0];
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 3400 "HolyC.tab.c"
    break;

  case 184: /* single_decl: primtype0 vardecltail  */
#line 814 "HolyC.y"
                                                {
  DeclsFillInBasetype(yyvsp[-1]->type2,yyvsp[0]);
  yyval=yyvsp[0];
}
#line 3409 "HolyC.tab.c"
    break;

  case 185: /* single_decl: primtype0 vardecltail_asn  */
#line 818 "HolyC.y"
                                                    {
  DeclsFillInBasetype(yyvsp[-1]->type2,yyvsp[0]);
  yyval=yyvsp[0];
}
#line 3418 "HolyC.tab.c"
    break;

  case 186: /* vardecltails: vardecltails COMMA vardecltails  */
#line 823 "HolyC.y"
                                                            {
  assert(yyvsp[0]->type==__AST_DECL_TAILS);
  CDeclTail b2=vec_last(&yyvsp[0]->declTail);
  AppendToDecls(yyvsp[-2],b2.basetype,b2.finalType,b2.name,b2.dft,NULL);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 3430 "HolyC.tab.c"
    break;

  case 189: /* multi_decl: primtype0 vardecltails  */
#line 833 "HolyC.y"
                                                 {
  DeclsFillInBasetype(yyvsp[-1]->type2,yyvsp[0]);
  yyval=yyvsp[0];
}
#line 3439 "HolyC.tab.c"
    break;

  case 191: /* funcargs: funcargs COMMA single_decl  */
#line 839 "HolyC.y"
                                                   {
  assert(yyvsp[0]->type==__AST_DECL_TAILS);
  CDeclTail b2=vec_last(&yyvsp[0]->declTail);
  yyval=AppendToDecls(yyvsp[-2],b2.basetype,b2.finalType,b2.name,b2.dft,NULL);
  ReleaseAST(yyvsp[-1]);
}
#line 3450 "HolyC.tab.c"
    break;

  case 192: /* funcargs: %empty  */
#line 845 "HolyC.y"
          {yyval=NULL;}
#line 3456 "HolyC.tab.c"
    break;

  case 193: /* linkage: EXTERN  */
#line 849 "HolyC.y"
                     {yyval=CreateExternLinkage(NULL);ReleaseAST(yyvsp[0]);}
#line 3462 "HolyC.tab.c"
    break;

  case 194: /* linkage: IMPORT  */
#line 850 "HolyC.y"
                     {yyval=CreateImportLinkage(NULL);ReleaseAST(yyvsp[0]);}
#line 3468 "HolyC.tab.c"
    break;

  case 195: /* linkage: PUBLIC  */
#line 851 "HolyC.y"
                     {yyval=NULL;}
#line 3474 "HolyC.tab.c"
    break;

  case 196: /* linkage: STATIC  */
#line 852 "HolyC.y"
                     {yyval=CreateStaticLinkage();ReleaseAST(yyvsp[0]);}
#line 3480 "HolyC.tab.c"
    break;

  case 197: /* linkage: EXTERN2 NAME  */
#line 853 "HolyC.y"
                              {yyval=CreateExternLinkage(yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3486 "HolyC.tab.c"
    break;

  case 198: /* linkage: IMPORT2 NAME  */
#line 854 "HolyC.y"
                              {yyval=CreateImportLinkage(yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3492 "HolyC.tab.c"
    break;

  case 199: /* linkage: EXTERN2 TYPENAME  */
#line 855 "HolyC.y"
                                  {yyval=CreateExternLinkage(yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3498 "HolyC.tab.c"
    break;

  case 200: /* linkage: IMPORT2 TYPENAME  */
#line 856 "HolyC.y"
                                  {yyval=CreateImportLinkage(yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3504 "HolyC.tab.c"
    break;

  case 201: /* func: primtype0 namewptrs LEFT_PAREN funcargs RIGHT_PAREN SEMI  */
#line 860 "HolyC.y"
                                                                                              {
  DeclsFillInBasetype(yyvsp[-5]->type2,yyvsp[-4]);
  CType *rtype=yyvsp[-4]->declTail.data[0].finalType;
  AST *name=yyvsp[-4]->declTail.data[0].name;
  CreateFuncForwardDecl(NULL,rtype,name,yyvsp[-2],0);
 yyval=CreateNop();
 ReleaseAST(yyvsp[-5]);
 ReleaseAST(yyvsp[-4]);
 ReleaseAST(yyvsp[-2]);
 ReleaseAST(yyvsp[-3]);
 ReleaseAST(yyvsp[-1]);
 ReleaseAST(yyvsp[0]);
}
#line 3522 "HolyC.tab.c"
    break;

  case 202: /* func: primtype0 namewptrs LEFT_PAREN funcargs COMMA DOT_DOT_DOT RIGHT_PAREN SEMI  */
#line 873 "HolyC.y"
                                                                                                                     {
  DeclsFillInBasetype(yyvsp[-7]->type2,yyvsp[-6]);
 CType *rtype=yyvsp[-6]->declTail.data[0].finalType;
 AST *name=yyvsp[-6]->declTail.data[0].name;
 CreateFuncForwardDecl(NULL,rtype,name,yyvsp[-4],1);
 yyval=CreateNop();
 ReleaseAST(yyvsp[-7]);
 ReleaseAST(yyvsp[-6]);
 ReleaseAST(yyvsp[-4]);
 ReleaseAST(yyvsp[-3]);
 ReleaseAST(yyvsp[-2]);
 ReleaseAST(yyvsp[-1]);
 ReleaseAST(yyvsp[0]);
}
#line 3541 "HolyC.tab.c"
    break;

  case 203: /* func: primtype0 namewptrs LEFT_PAREN DOT_DOT_DOT RIGHT_PAREN SEMI  */
#line 887 "HolyC.y"
                                                                                                {
  DeclsFillInBasetype(yyvsp[-5]->type2,yyvsp[-4]);
 CType *rtype=yyvsp[-4]->declTail.data[0].finalType;
 AST *name=yyvsp[-4]->declTail.data[0].name;
  CreateFuncForwardDecl(NULL,rtype,name,NULL,1);
 yyval=CreateNop();
 ReleaseAST(yyvsp[-5]);
 ReleaseAST(yyvsp[-4]);
 ReleaseAST(yyvsp[-3]);
 ReleaseAST(yyvsp[-2]);
 ReleaseAST(yyvsp[-1]);
 ReleaseAST(yyvsp[0]);
}
#line 3559 "HolyC.tab.c"
    break;

  case 204: /* func: linkage primtype0 namewptrs LEFT_PAREN funcargs RIGHT_PAREN SEMI  */
#line 900 "HolyC.y"
                                                                                                         {
  DeclsFillInBasetype(yyvsp[-5]->type2,yyvsp[-4]);
  CType *rtype=yyvsp[-4]->declTail.data[0].finalType;
  AST *name=yyvsp[-4]->declTail.data[0].name;
  CreateFuncForwardDecl(yyvsp[-6],rtype,name,yyvsp[-2],0);
  yyval=CreateNop();
  ReleaseAST(yyvsp[-6]);
  ReleaseAST(yyvsp[-5]);
  ReleaseAST(yyvsp[-4]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3578 "HolyC.tab.c"
    break;

  case 205: /* func: linkage primtype0 namewptrs LEFT_PAREN funcargs COMMA DOT_DOT_DOT RIGHT_PAREN SEMI  */
#line 914 "HolyC.y"
                                                                                                                                     {
  DeclsFillInBasetype(yyvsp[-7]->type2,yyvsp[-6]);
  CType *rtype=yyvsp[-6]->declTail.data[0].finalType;
  AST *name=yyvsp[-6]->declTail.data[0].name;
  CreateFuncForwardDecl(yyvsp[-8],rtype,name,yyvsp[-4],1);
  yyval=CreateNop();
  ReleaseAST(yyvsp[-7]);
  ReleaseAST(yyvsp[-6]);
  ReleaseAST(yyvsp[-4]);
  ReleaseAST(yyvsp[-8]);
  ReleaseAST(yyvsp[-5]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3599 "HolyC.tab.c"
    break;

  case 206: /* func: linkage primtype0 namewptrs LEFT_PAREN DOT_DOT_DOT RIGHT_PAREN SEMI  */
#line 930 "HolyC.y"
                                                                                                           {
  DeclsFillInBasetype(yyvsp[-5]->type2,yyvsp[-4]);
  CType *rtype=yyvsp[-4]->declTail.data[0].finalType;
  AST *name=yyvsp[-4]->declTail.data[0].name;
  CreateFuncForwardDecl(yyvsp[-6],rtype,name,NULL,1);
  yyval=CreateNop();
  ReleaseAST(yyvsp[-6]);
  ReleaseAST(yyvsp[-5]);
  ReleaseAST(yyvsp[-4]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3618 "HolyC.tab.c"
    break;

  case 207: /* func: primtype0 namewptrs LEFT_PAREN DOT_DOT_DOT RIGHT_PAREN scope  */
#line 944 "HolyC.y"
                                                                                                     {
  DeclsFillInBasetype(yyvsp[-5]->type2,yyvsp[-4]);
  CType *rtype=yyvsp[-4]->declTail.data[0].finalType;
  AST *name=yyvsp[-4]->declTail.data[0].name;
  CompileFunction(NULL,rtype,name,NULL,yyvsp[0],1);
  yyval=CreateNop();

  ReleaseAST(yyvsp[-5]);
  ReleaseAST(yyvsp[-4]);
  ReleaseAST(yyvsp[0]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
}
#line 3637 "HolyC.tab.c"
    break;

  case 208: /* func: primtype0 namewptrs LEFT_PAREN funcargs COMMA DOT_DOT_DOT RIGHT_PAREN scope  */
#line 958 "HolyC.y"
                                                                                                                               {
  DeclsFillInBasetype(yyvsp[-7]->type2,yyvsp[-6]);
  CType *rtype=yyvsp[-6]->declTail.data[0].finalType;
  AST *name=yyvsp[-6]->declTail.data[0].name;
  CompileFunction(NULL,rtype,name,yyvsp[-4],yyvsp[0],1);
  yyval=CreateNop();
  ReleaseAST(yyvsp[-7]);
  ReleaseAST(yyvsp[-6]);
  ReleaseAST(yyvsp[-4]);
  ReleaseAST(yyvsp[0]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[-5]);
}
#line 3657 "HolyC.tab.c"
    break;

  case 209: /* func: primtype0 namewptrs LEFT_PAREN funcargs RIGHT_PAREN scope  */
#line 973 "HolyC.y"
                                                                                                   {
  DeclsFillInBasetype(yyvsp[-5]->type2,yyvsp[-4]);
  CType *rtype=yyvsp[-4]->declTail.data[0].finalType;
  AST *name=yyvsp[-4]->declTail.data[0].name;
  CompileFunction(NULL,rtype,name,yyvsp[-2],yyvsp[0],0);
  yyval=CreateNop();
  ReleaseAST(yyvsp[-5]);
  ReleaseAST(yyvsp[-4]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-1]);
}
#line 3675 "HolyC.tab.c"
    break;

  case 210: /* func: linkage primtype0 namewptrs LEFT_PAREN DOT_DOT_DOT RIGHT_PAREN scope  */
#line 986 "HolyC.y"
                                                                                                                {
  DeclsFillInBasetype(yyvsp[-5]->type2,yyvsp[-4]);
  CType *rtype=yyvsp[-4]->declTail.data[0].finalType;
  AST *name=yyvsp[-4]->declTail.data[0].name;
  CompileFunction(yyvsp[-6],rtype,name,NULL,yyvsp[0],1);
  yyval=CreateNop();
  ReleaseAST(yyvsp[-6]);
  ReleaseAST(yyvsp[-5]);
  ReleaseAST(yyvsp[-4]);
  ReleaseAST(yyvsp[0]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
}
#line 3694 "HolyC.tab.c"
    break;

  case 211: /* func: linkage primtype0 namewptrs LEFT_PAREN funcargs COMMA DOT_DOT_DOT RIGHT_PAREN scope  */
#line 1000 "HolyC.y"
                                                                                                                                          {
  DeclsFillInBasetype(yyvsp[-7]->type2,yyvsp[-6]);
  CType *rtype=yyvsp[-6]->declTail.data[0].finalType;
  AST *name=yyvsp[-6]->declTail.data[0].name;
  CompileFunction(yyvsp[-8],rtype,name,yyvsp[-4],yyvsp[0],1);
  ReleaseAST(yyvsp[-8]);
  ReleaseAST(yyvsp[-7]);
  ReleaseAST(yyvsp[-6]);
  ReleaseAST(yyvsp[-4]);
  ReleaseAST(yyvsp[0]);
  ReleaseAST(yyvsp[-5]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
  yyval=CreateNop();
}
#line 3715 "HolyC.tab.c"
    break;

  case 212: /* func: linkage primtype0 namewptrs LEFT_PAREN funcargs RIGHT_PAREN scope  */
#line 1016 "HolyC.y"
                                                                                                              {
  DeclsFillInBasetype(yyvsp[-5]->type2,yyvsp[-4]);
  CType *rtype=yyvsp[-4]->declTail.data[0].finalType;
  AST *name=yyvsp[-4]->declTail.data[0].name;
  CompileFunction(yyvsp[-6],rtype,name,yyvsp[-2],yyvsp[0],0);
  ReleaseAST(yyvsp[-6]);
  ReleaseAST(yyvsp[-5]);
  ReleaseAST(yyvsp[-4]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-1]);
  yyval=CreateNop(yyvsp[0]);
}
#line 3733 "HolyC.tab.c"
    break;

  case 213: /* ocstmt: ostmt  */
#line 1034 "HolyC.y"
                    {yyval=yyvsp[0];}
#line 3739 "HolyC.tab.c"
    break;

  case 214: /* ocstmt: cstmt  */
#line 1035 "HolyC.y"
                    {yyval=yyvsp[0];}
#line 3745 "HolyC.tab.c"
    break;

  case 216: /* expr_opt: %empty  */
#line 1038 "HolyC.y"
          {yyval=NULL;}
#line 3751 "HolyC.tab.c"
    break;

  case 217: /* loop_header: WHILE LEFT_PAREN expr_comma RIGHT_PAREN  */
#line 1041 "HolyC.y"
                                                                           {
  yyval=SOT(CreateWhile(yyvsp[-1],NULL),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3761 "HolyC.tab.c"
    break;

  case 218: /* loop_header: FOR LEFT_PAREN expr_opt SEMI expr_opt SEMI expr_opt RIGHT_PAREN  */
#line 1047 "HolyC.y"
                                                                                                                         {
  yyval=SOT(CreateFor(yyvsp[-5],yyvsp[-3],yyvsp[-1],NULL),yyvsp[-7]);
  ReleaseAST(yyvsp[-6]);
  ReleaseAST(yyvsp[-4]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3773 "HolyC.tab.c"
    break;

  case 219: /* ifcl: IF LEFT_PAREN expr_comma RIGHT_PAREN  */
#line 1055 "HolyC.y"
                                                              {
  yyval=SOT(CreateIf(yyvsp[-1],NULL,NULL),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3783 "HolyC.tab.c"
    break;

  case 220: /* ostmt: ifcl ocstmt  */
#line 1061 "HolyC.y"
                            {
  assert(yyvsp[-1]->type==AST_IF);
  yyval=yyvsp[-1];
  yyvsp[-1]->ifStmt.body=yyvsp[0];
}
#line 3793 "HolyC.tab.c"
    break;

  case 221: /* ostmt: ifcl cstmt ELSE ostmt  */
#line 1066 "HolyC.y"
                                              {
  assert(yyvsp[-3]->type==AST_IF);
  yyval=yyvsp[-3];
  yyvsp[-3]->ifStmt.body=yyvsp[-2];
  yyvsp[-3]->ifStmt.elseBody=yyvsp[0];
  ReleaseAST(yyvsp[-1]);
}
#line 3805 "HolyC.tab.c"
    break;

  case 222: /* ostmt: loop_header ostmt  */
#line 1073 "HolyC.y"
                                  {
  if(yyvsp[-1]->type==AST_FOR) {
    yyvsp[-1]->forStmt.body=yyvsp[0];
  } else if(yyvsp[-1]->type==AST_DO) {
    yyvsp[-1]->doStmt.body=yyvsp[0];
  } else if(yyvsp[-1]->type==AST_WHILE) {
    yyvsp[-1]->whileStmt.body=yyvsp[0];
  }
  yyval=yyvsp[-1];
}
#line 3820 "HolyC.tab.c"
    break;

  case 224: /* cstmt: ifcl cstmt ELSE cstmt  */
#line 1085 "HolyC.y"
                                              {
  assert(yyvsp[-3]->type==AST_IF);
  yyval=yyvsp[-3];
  yyvsp[-3]->ifStmt.body=yyvsp[-2];
  yyvsp[-3]->ifStmt.elseBody=yyvsp[0];
  ReleaseAST(yyvsp[-1]);
}
#line 3832 "HolyC.tab.c"
    break;

  case 225: /* cstmt: loop_header cstmt  */
#line 1092 "HolyC.y"
                                  {
  if(yyvsp[-1]->type==AST_FOR) {
    yyvsp[-1]->forStmt.body=yyvsp[0];
  } else if(yyvsp[-1]->type==AST_DO) {
    yyvsp[-1]->doStmt.body=yyvsp[0];
  } else if(yyvsp[-1]->type==AST_WHILE) {
    yyvsp[-1]->whileStmt.body=yyvsp[0];
  }
  yyval=yyvsp[-1];
}
#line 3847 "HolyC.tab.c"
    break;

  case 226: /* dostmt: DO ocstmt WHILE LEFT_PAREN expr_comma RIGHT_PAREN  */
#line 1103 "HolyC.y"
                                                                                     {
  yyval=SOT(CreateDo(yyvsp[-1],yyvsp[-4]),yyvsp[-5]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3858 "HolyC.tab.c"
    break;

  case 227: /* subswit: START COLON swit_body END COLON  */
#line 1109 "HolyC.y"
                                                                 {
  yyval=SOT(CreateSubswitch(yyvsp[-2]),yyvsp[-4]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3869 "HolyC.tab.c"
    break;

  case 228: /* swit_body_stmt: ocstmt  */
#line 1116 "HolyC.y"
                       {yyval=yyvsp[0];}
#line 3875 "HolyC.tab.c"
    break;

  case 229: /* swit_body_stmt: DEFAULT COLON  */
#line 1117 "HolyC.y"
                                         {
  yyval=SOT(CreateDefault(),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3884 "HolyC.tab.c"
    break;

  case 230: /* swit_body_stmt: CASE expr COLON  */
#line 1121 "HolyC.y"
                                              {
  yyval=SOT(CreateCase(yyvsp[-1],NULL),yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3893 "HolyC.tab.c"
    break;

  case 231: /* swit_body_stmt: CASE COLON  */
#line 1125 "HolyC.y"
                                      {
  yyval=SOT(CreateCase(NULL,NULL),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3902 "HolyC.tab.c"
    break;

  case 232: /* swit_body_stmt: CASE expr DOT_DOT_DOT expr COLON  */
#line 1129 "HolyC.y"
                                                                       {
  yyval=SOT(CreateCase(yyvsp[-3],yyvsp[-1]),yyvsp[-4]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);

}
#line 3913 "HolyC.tab.c"
    break;

  case 233: /* swit_body_stmt: subswit  */
#line 1135 "HolyC.y"
                              {yyval=yyvsp[0];}
#line 3919 "HolyC.tab.c"
    break;

  case 234: /* swit_body: swit_body swit_body_stmt  */
#line 1136 "HolyC.y"
                                             {yyval=AppendToStmts(yyvsp[-1],yyvsp[0]);}
#line 3925 "HolyC.tab.c"
    break;

  case 235: /* swit_body: swit_body_stmt  */
#line 1137 "HolyC.y"
                                {yyval=yyvsp[0];}
#line 3931 "HolyC.tab.c"
    break;

  case 236: /* swit: SWITCH LEFT_PAREN expr_comma RIGHT_PAREN LEFT_CURLY swit_body RIGHT_CURLY  */
#line 1138 "HolyC.y"
                                                                                                                      {
  yyval=SOT(CreateSwitch(yyvsp[-4],yyvsp[-1]),yyvsp[-6]);
  ReleaseAST(yyvsp[-5]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3943 "HolyC.tab.c"
    break;

  case 237: /* swit: SWITCH LEFT_PAREN expr_comma RIGHT_PAREN LEFT_CURLY RIGHT_CURLY  */
#line 1145 "HolyC.y"
                                                                                                       {
  yyval=SOT(CreateSwitch(yyvsp[-3],NULL),yyvsp[-5]);
  ReleaseAST(yyvsp[-4]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3955 "HolyC.tab.c"
    break;

  case 239: /* opcodes: opcode SEMI  */
#line 1153 "HolyC.y"
                            {
  AST *r=TD_MALLOC(sizeof(AST));
  r->type=AST_ASM_BLK;
  vec_init(&r->stmts);
  vec_push(&r->stmts,yyvsp[-1]);
  yyval=r;
  ReleaseAST(yyvsp[0]);
}
#line 3968 "HolyC.tab.c"
    break;

  case 240: /* opcodes: opcodes opcode SEMI  */
#line 1161 "HolyC.y"
                                          {
  vec_push(&yyvsp[-2]->stmts,yyvsp[-1]);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[0]);
}
#line 3978 "HolyC.tab.c"
    break;

  case 243: /* simple_stmt: multi_decl SEMI  */
#line 1168 "HolyC.y"
                                       {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[0]);
}
#line 3987 "HolyC.tab.c"
    break;

  case 244: /* simple_stmt: linkage multi_decl SEMI  */
#line 1172 "HolyC.y"
                                                  {
  yyval=ApplyLinkageToDecls(yyvsp[-2],yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3996 "HolyC.tab.c"
    break;

  case 246: /* simple_stmt: GOTO NAME SEMI  */
#line 1177 "HolyC.y"
                                         {
  yyval=SOT(CreateGoto(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4005 "HolyC.tab.c"
    break;

  case 247: /* simple_stmt: dostmt SEMI  */
#line 1181 "HolyC.y"
                                   {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[0]);
}
#line 4014 "HolyC.tab.c"
    break;

  case 248: /* simple_stmt: expr_comma SEMI  */
#line 1185 "HolyC.y"
                                       {
  yyval=yyvsp[-1];
  if(yyvsp[-1]->type==AST_STRING) {
    yyval=SOT(CreatePrint(yyvsp[-1]->string),yyvsp[-1]);
  }
  ReleaseAST(yyvsp[0]);
}
#line 4026 "HolyC.tab.c"
    break;

  case 249: /* simple_stmt: swit  */
#line 1192 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 4032 "HolyC.tab.c"
    break;

  case 250: /* simple_stmt: scope  */
#line 1193 "HolyC.y"
                         {yyval=yyvsp[0];}
#line 4038 "HolyC.tab.c"
    break;

  case 251: /* simple_stmt: BREAK SEMI  */
#line 1194 "HolyC.y"
                                  {
  yyval=SOT(CreateBreak(),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4047 "HolyC.tab.c"
    break;

  case 252: /* simple_stmt: SEMI  */
#line 1198 "HolyC.y"
                         {
  yyval=CreateNop();
  ReleaseAST(yyvsp[0]);
}
#line 4056 "HolyC.tab.c"
    break;

  case 253: /* simple_stmt: RET expr_comma SEMI  */
#line 1202 "HolyC.y"
                                           {
  yyval=SOT(CreateReturn(yyvsp[-1]),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4065 "HolyC.tab.c"
    break;

  case 254: /* simple_stmt: RET SEMI  */
#line 1206 "HolyC.y"
                                 {
  yyval=SOT(CreateReturn(NULL),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4074 "HolyC.tab.c"
    break;

  case 255: /* simple_stmt: NAME COLON  */
#line 1210 "HolyC.y"
                                  {
  yyval=SOT(CreateLabel(yyvsp[-1]),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4083 "HolyC.tab.c"
    break;

  case 256: /* simple_stmt: class  */
#line 1214 "HolyC.y"
                         {yyval=yyvsp[0];}
#line 4089 "HolyC.tab.c"
    break;

  case 257: /* simple_stmt: union  */
#line 1215 "HolyC.y"
                         {yyval=yyvsp[0];}
#line 4095 "HolyC.tab.c"
    break;

  case 258: /* simple_stmt: linkage UNION NAME SEMI  */
#line 1216 "HolyC.y"
                                                        {
  CType *t=CreateUnionForwardDecl(yyvsp[-3],yyvsp[-1]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4106 "HolyC.tab.c"
    break;

  case 259: /* simple_stmt: linkage CLASS NAME SEMI  */
#line 1222 "HolyC.y"
                                                        {
  CType *t=CreateClassForwardDecl(yyvsp[-3],yyvsp[-1]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4117 "HolyC.tab.c"
    break;

  case 260: /* stmts: ocstmt  */
#line 1228 "HolyC.y"
                    {yyval=yyvsp[0];}
#line 4123 "HolyC.tab.c"
    break;

  case 261: /* stmts: stmts ocstmt  */
#line 1229 "HolyC.y"
                             {yyval=AppendToStmts(yyvsp[-1],yyvsp[0]);}
#line 4129 "HolyC.tab.c"
    break;

  case 262: /* stmts: stmts error  */
#line 1230 "HolyC.y"
                   {YYABORT;}
#line 4135 "HolyC.tab.c"
    break;

  case 263: /* scope: LEFT_CURLY stmts RIGHT_CURLY  */
#line 1231 "HolyC.y"
                                                    {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4145 "HolyC.tab.c"
    break;

  case 264: /* scope: LEFT_CURLY RIGHT_CURLY  */
#line 1236 "HolyC.y"
                                           {
  yyval=CreateNop();
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4155 "HolyC.tab.c"
    break;

  case 265: /* global_stmts: %empty  */
#line 1241 "HolyC.y"
                {yyval=NULL;}
#line 4161 "HolyC.tab.c"
    break;

  case 266: /* global_stmts: global_stmts ocstmt  */
#line 1242 "HolyC.y"
                                        {
  RunStatement(yyvsp[0]);
  ReleaseAST(yyvsp[0]);
  yyval=NULL;
}
#line 4171 "HolyC.tab.c"
    break;

  case 267: /* global_stmt: global_stmts  */
#line 1247 "HolyC.y"
                                {
  yyval=NULL;
}
#line 4179 "HolyC.tab.c"
    break;

  case 268: /* global_stmt: EXE scope  */
#line 1251 "HolyC.y"
                             {
  RunStatement(yyvsp[0]);
  YYACCEPT;
  yyval=NULL;
}
#line 4189 "HolyC.tab.c"
    break;

  case 269: /* global_stmt: EVAL expr_comma NL  */
#line 1256 "HolyC.y"
                                      {
  RunPtr(CurFuncInfo,yyvsp[-1],CurFramePtr);
  YYACCEPT;
  yyval=NULL;
}
#line 4199 "HolyC.tab.c"
    break;

  case 270: /* global_stmt: DBG expr_comma  */
#line 1262 "HolyC.y"
                                  {
  RunPtr(CurFuncInfo,yyvsp[0],CurFramePtr);
  YYACCEPT;
  yyval=NULL;
}
#line 4209 "HolyC.tab.c"
    break;

  case 271: /* _expr0: DOUBLE_AT NAME  */
#line 1271 "HolyC.y"
                             {
  yyval=SOT(yyvsp[0],yyvsp[-1]);
  char buffer[256];
  sprintf(buffer,"@@%s",yyval->name);
  TD_FREE(yyval->name);
  yyval->name=strdup(buffer);
  ReleaseAST(yyvsp[-1]);
}
#line 4222 "HolyC.tab.c"
    break;

  case 272: /* _expr0: DOUBLE_AT INT  */
#line 1279 "HolyC.y"
                               {
 //Make a name token out of $n
  char buffer[256];
  sprintf(buffer,"@@%li",yyvsp[0]->integer);
  AST *n=TD_MALLOC(sizeof(AST));
  n->type=AST_NAME,n->name=strdup(buffer);
  yyval=n;
  ReleaseAST(yyvsp[-1]);
}
#line 4236 "HolyC.tab.c"
    break;

  case 273: /* _expr0: FLOAT  */
#line 1288 "HolyC.y"
                 {yyval=SLE(yyvsp[0]);}
#line 4242 "HolyC.tab.c"
    break;

  case 274: /* _expr0: INT  */
#line 1289 "HolyC.y"
               {yyval=SLE(yyvsp[0]);}
#line 4248 "HolyC.tab.c"
    break;

  case 275: /* _expr0: CHAR  */
#line 1290 "HolyC.y"
                {yyval=SLE(yyvsp[0]);}
#line 4254 "HolyC.tab.c"
    break;

  case 276: /* _expr0: STRING  */
#line 1291 "HolyC.y"
                  {yyval=SLE(yyvsp[0]);}
#line 4260 "HolyC.tab.c"
    break;

  case 277: /* _expr0: LASTCLASS  */
#line 1292 "HolyC.y"
                     {
  yyval=SLE(yyvsp[0]);
}
#line 4268 "HolyC.tab.c"
    break;

  case 278: /* _expr0: NAME  */
#line 1295 "HolyC.y"
                {
  yyval=SLE(yyvsp[0]);
}
#line 4276 "HolyC.tab.c"
    break;

  case 280: /* _expr1: LEFT_PAREN expr_comma RIGHT_PAREN  */
#line 1299 "HolyC.y"
                                                          {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 4285 "HolyC.tab.c"
    break;

  case 282: /* _expr2: TYPENAME DOT NAME  */
#line 1304 "HolyC.y"
                                     {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4294 "HolyC.tab.c"
    break;

  case 283: /* _expr2: TYPENAME ARROW NAME  */
#line 1308 "HolyC.y"
                                        {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4303 "HolyC.tab.c"
    break;

  case 284: /* _expr2: _expr2 DOT NAME  */
#line 1312 "HolyC.y"
                                    {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4312 "HolyC.tab.c"
    break;

  case 285: /* _expr2: _expr2 ARROW NAME  */
#line 1316 "HolyC.y"
                                      {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4321 "HolyC.tab.c"
    break;

  case 286: /* _expr2: _expr2 DEC  */
#line 1320 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[-1],AST_POST_DEC),yyvsp[0]);
  ReleaseAST(yyvsp[0]);
}
#line 4330 "HolyC.tab.c"
    break;

  case 287: /* _expr2: _expr2 INC  */
#line 1324 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[-1],AST_POST_INC),yyvsp[0]);
  ReleaseAST(yyvsp[0]);
}
#line 4339 "HolyC.tab.c"
    break;

  case 288: /* _expr2: _expr2 LEFT_PAREN callargs RIGHT_PAREN  */
#line 1328 "HolyC.y"
                                                                  {
  yyval=SOT(CreateFuncCall(yyvsp[-3],yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 4348 "HolyC.tab.c"
    break;

  case 289: /* _expr2: _expr2 LEFT_PAREN primtype0 RIGHT_PAREN  */
#line 1332 "HolyC.y"
                                                                   {
  yyval=CreateExplicitTypecast(yyvsp[-3],yyvsp[-1]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 4357 "HolyC.tab.c"
    break;

  case 291: /* _expr3: SIZEOF NAME  */
#line 1337 "HolyC.y"
                             {
  yyval=SOT(CreateSizeof(yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4366 "HolyC.tab.c"
    break;

  case 292: /* _expr3: SIZEOF sizeof_type  */
#line 1341 "HolyC.y"
                                    {
  yyval=SOT(CreateSizeof(yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4375 "HolyC.tab.c"
    break;

  case 293: /* _expr3: SIZEOF LEFT_PAREN expr_comma RIGHT_PAREN  */
#line 1345 "HolyC.y"
                                                                     {
  yyval=SOT(CreateSizeof(yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4386 "HolyC.tab.c"
    break;

  case 294: /* _expr3: SIZEOF LEFT_PAREN sizeof_type RIGHT_PAREN  */
#line 1351 "HolyC.y"
                                                                      {
  yyval=SOT(CreateSizeof(yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4397 "HolyC.tab.c"
    break;

  case 295: /* _expr3: BAND _expr3  */
#line 1357 "HolyC.y"
                             {
  yyval=SOT(CreateUnop(yyvsp[0],AST_ADDROF),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4406 "HolyC.tab.c"
    break;

  case 296: /* _expr3: MUL _expr3  */
#line 1361 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[0],AST_DERREF),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4415 "HolyC.tab.c"
    break;

  case 297: /* _expr3: DEC _expr3  */
#line 1366 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[0],AST_PRE_DEC),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4424 "HolyC.tab.c"
    break;

  case 298: /* _expr3: INC _expr3  */
#line 1370 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[0],AST_PRE_INC),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4433 "HolyC.tab.c"
    break;

  case 299: /* _expr3: ADD _expr3  */
#line 1374 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[0],AST_POS),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4442 "HolyC.tab.c"
    break;

  case 300: /* _expr3: SUB _expr3  */
#line 1378 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[0],AST_NEG),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4451 "HolyC.tab.c"
    break;

  case 301: /* _expr3: LNOT _expr3  */
#line 1382 "HolyC.y"
                             {
  yyval=SOT(CreateUnop(yyvsp[0],AST_LNOT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4460 "HolyC.tab.c"
    break;

  case 302: /* _expr3: BNOT _expr3  */
#line 1386 "HolyC.y"
                             {
  yyval=SOT(CreateUnop(yyvsp[0],AST_BNOT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4469 "HolyC.tab.c"
    break;

  case 304: /* _expr4: _expr4 POW _expr4  */
#line 1393 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_POW),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4478 "HolyC.tab.c"
    break;

  case 305: /* _expr4: _expr4 SHL _expr4  */
#line 1397 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SHL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4487 "HolyC.tab.c"
    break;

  case 306: /* _expr4: _expr4 SHR _expr4  */
#line 1401 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SHR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4496 "HolyC.tab.c"
    break;

  case 308: /* _expr4_5: _expr4_5 MUL _expr4_5  */
#line 1407 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_MUL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4505 "HolyC.tab.c"
    break;

  case 309: /* _expr4_5: _expr4_5 DIV _expr4_5  */
#line 1411 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_DIV),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4514 "HolyC.tab.c"
    break;

  case 310: /* _expr4_5: _expr4_5 MOD _expr4_5  */
#line 1415 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_MOD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4523 "HolyC.tab.c"
    break;

  case 312: /* _expr5: _expr5 BAND _expr5  */
#line 1421 "HolyC.y"
                                       {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4532 "HolyC.tab.c"
    break;

  case 314: /* _expr6: _expr6 BXOR _expr6  */
#line 1427 "HolyC.y"
                                       {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4541 "HolyC.tab.c"
    break;

  case 316: /* _expr7: _expr7 BOR _expr7  */
#line 1433 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4550 "HolyC.tab.c"
    break;

  case 318: /* _expr8: _expr8 ADD _expr8  */
#line 1439 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ADD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4559 "HolyC.tab.c"
    break;

  case 319: /* _expr8: _expr8 SUB _expr8  */
#line 1443 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SUB),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4568 "HolyC.tab.c"
    break;

  case 321: /* _expr9: _expr9 LT _expr9  */
#line 1449 "HolyC.y"
                                     {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_LT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4577 "HolyC.tab.c"
    break;

  case 322: /* _expr9: _expr9 GT _expr9  */
#line 1453 "HolyC.y"
                                     {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_GT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4586 "HolyC.tab.c"
    break;

  case 323: /* _expr9: _expr9 LE _expr9  */
#line 1457 "HolyC.y"
                                     {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_LE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4595 "HolyC.tab.c"
    break;

  case 324: /* _expr9: _expr9 GE _expr9  */
#line 1461 "HolyC.y"
                                     {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_GE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4604 "HolyC.tab.c"
    break;

  case 326: /* _expr10: _expr10 EQ _expr10  */
#line 1467 "HolyC.y"
                                        {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_EQ),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4613 "HolyC.tab.c"
    break;

  case 327: /* _expr10: _expr10 NE _expr10  */
#line 1471 "HolyC.y"
                                        {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_NE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4622 "HolyC.tab.c"
    break;

  case 329: /* _expr11: _expr11 LAND _expr11  */
#line 1477 "HolyC.y"
                                          {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4631 "HolyC.tab.c"
    break;

  case 331: /* _expr12: _expr12 LXOR _expr12  */
#line 1483 "HolyC.y"
                                          {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4640 "HolyC.tab.c"
    break;

  case 333: /* _expr13: _expr13 LOR _expr13  */
#line 1489 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4649 "HolyC.tab.c"
    break;

  case 335: /* _expr14: _expr14 ASSIGN _expr14  */
#line 1495 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4658 "HolyC.tab.c"
    break;

  case 336: /* _expr14: _expr14 EQ_SHL _expr14  */
#line 1499 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SHL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4667 "HolyC.tab.c"
    break;

  case 337: /* _expr14: _expr14 EQ_SHR _expr14  */
#line 1503 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SHR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4676 "HolyC.tab.c"
    break;

  case 338: /* _expr14: _expr14 EQ_MUL _expr14  */
#line 1507 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_MUL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4685 "HolyC.tab.c"
    break;

  case 339: /* _expr14: _expr14 EQ_DIV _expr14  */
#line 1511 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_DIV),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4694 "HolyC.tab.c"
    break;

  case 340: /* _expr14: _expr14 EQ_MOD _expr14  */
#line 1515 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_MOD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4703 "HolyC.tab.c"
    break;

  case 341: /* _expr14: _expr14 EQ_BAND _expr14  */
#line 1519 "HolyC.y"
                                             {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4712 "HolyC.tab.c"
    break;

  case 342: /* _expr14: _expr14 EQ_BXOR _expr14  */
#line 1523 "HolyC.y"
                                             {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4721 "HolyC.tab.c"
    break;

  case 343: /* _expr14: _expr14 EQ_BOR _expr14  */
#line 1527 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4730 "HolyC.tab.c"
    break;

  case 344: /* _expr14: _expr14 EQ_ADD _expr14  */
#line 1531 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_ADD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4739 "HolyC.tab.c"
    break;

  case 345: /* _expr14: _expr14 EQ_SUB _expr14  */
#line 1535 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SUB),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4748 "HolyC.tab.c"
    break;


#line 4752 "HolyC.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == HC_HC_EMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= HC_YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == HC_YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = HC_HC_EMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != HC_HC_EMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 1541 "HolyC.y"

static int code;
int yylex() {
  yylval=LexItem();
  if(!yylval) return 0;
  return ASTToToken(yylval);
}
static void LexerCB(int tok,AST *d) {
  code=tok;
  yylval=d;
  yylex();
}
void AttachParserToLexer() {
    //Lexer.cb=LexerCB;
}
static int __IsTruePassed;
static void __IsTrue(CFuncInfo *dummy1,AST *node,void *fp) {
  if(Compiler.tagsFile) {
    __IsTruePassed=1;
    return ;
  }
  CType *rtype =AssignTypeToNode(node);
  COldFuncState old=CreateCompilerState();
  vec_CVariable_t args;
  vec_init(&args);
  Compiler.returnType=rtype;
  AST *retn =CreateReturn(node);
  CFunction *f=CompileAST(NULL,retn,args,C_AST_FRAME_OFF_DFT);
  int ret;
  if(IsF64(rtype)) {
    ret=0!=((double(*)())f->funcptr)();
  } else if(IsIntegerType(rtype)) {
    ret=0!=((int64_t(*)())f->funcptr)();
  }
  ReleaseFunction(f);
  vec_deinit(&args);
  RestoreCompilerState(old);
  __IsTruePassed=ret;
}
int EvalConditionForPreproc() {
  Lexer.stopAtNewline=1;
  Lexer.isEvalMode=1;
  RunPtr=__IsTrue;
  __IsTruePassed=0;
  yyparse();
  Lexer.stopAtNewline=0;
  return __IsTruePassed;
}
void
yyerror (char const *s)
{
  RaiseError(Lexer.lastToken,"Parsing stoped here.");
  FlushLexer();
}
void DebugEvalExpr(void *frameptr,CFuncInfo *info,char *text) {
  CLexer old=ForkLexer(PARSER_HOLYC);
  Lexer.isDebugExpr=1;
  mrope_append_text(Lexer.source,strdup(text));
  CurFuncInfo=info;
  CurFramePtr=frameptr;
  RunPtr=&EvalDebugExpr;
  yyparse();
  printf("\n");
  Lexer=old;
}
