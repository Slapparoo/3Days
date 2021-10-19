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
  YYSYMBOL_OTHER = 44,                     /* OTHER  */
  YYSYMBOL_TRY = 45,                       /* TRY  */
  YYSYMBOL_CATCH = 46,                     /* CATCH  */
  YYSYMBOL_LASTCLASS = 47,                 /* LASTCLASS  */
  YYSYMBOL_U0 = 48,                        /* U0  */
  YYSYMBOL_LEFT_PAREN = 49,                /* LEFT_PAREN  */
  YYSYMBOL_RIGHT_PAREN = 50,               /* RIGHT_PAREN  */
  YYSYMBOL_INC = 51,                       /* INC  */
  YYSYMBOL_DEC = 52,                       /* DEC  */
  YYSYMBOL_NAME = 53,                      /* NAME  */
  YYSYMBOL_LEFT_SQAURE = 54,               /* LEFT_SQAURE  */
  YYSYMBOL_RIGHT_SQAURE = 55,              /* RIGHT_SQAURE  */
  YYSYMBOL_SEMI = 56,                      /* SEMI  */
  YYSYMBOL_IF = 57,                        /* IF  */
  YYSYMBOL_ELSE = 58,                      /* ELSE  */
  YYSYMBOL_DO = 59,                        /* DO  */
  YYSYMBOL_WHILE = 60,                     /* WHILE  */
  YYSYMBOL_FOR = 61,                       /* FOR  */
  YYSYMBOL_LEFT_CURLY = 62,                /* LEFT_CURLY  */
  YYSYMBOL_RIGHT_CURLY = 63,               /* RIGHT_CURLY  */
  YYSYMBOL_CASE = 64,                      /* CASE  */
  YYSYMBOL_COLON = 65,                     /* COLON  */
  YYSYMBOL_DOT_DOT_DOT = 66,               /* DOT_DOT_DOT  */
  YYSYMBOL_EXTERN2 = 67,                   /* EXTERN2  */
  YYSYMBOL_LOCK = 68,                      /* LOCK  */
  YYSYMBOL_EXTERN = 69,                    /* EXTERN  */
  YYSYMBOL_IMPORT = 70,                    /* IMPORT  */
  YYSYMBOL_IMPORT2 = 71,                   /* IMPORT2  */
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
  YYSYMBOL_BOOL = 83,                      /* BOOL  */
  YYSYMBOL_U8 = 84,                        /* U8  */
  YYSYMBOL_I8 = 85,                        /* I8  */
  YYSYMBOL_U16 = 86,                       /* U16  */
  YYSYMBOL_I16 = 87,                       /* I16  */
  YYSYMBOL_U32 = 88,                       /* U32  */
  YYSYMBOL_I32 = 89,                       /* I32  */
  YYSYMBOL_U64 = 90,                       /* U64  */
  YYSYMBOL_I64 = 91,                       /* I64  */
  YYSYMBOL_F64 = 92,                       /* F64  */
  YYSYMBOL_SWITCH = 93,                    /* SWITCH  */
  YYSYMBOL_EXE = 94,                       /* EXE  */
  YYSYMBOL_EVAL = 95,                      /* EVAL  */
  YYSYMBOL_NL = 96,                        /* NL  */
  YYSYMBOL_DBG = 97,                       /* DBG  */
  YYSYMBOL_STRING = 98,                    /* STRING  */
  YYSYMBOL_TYPENAME = 99,                  /* TYPENAME  */
  YYSYMBOL_YYACCEPT = 100,                 /* $accept  */
  YYSYMBOL_expr0 = 101,                    /* expr0  */
  YYSYMBOL__callargs = 102,                /* _callargs  */
  YYSYMBOL_callargs = 103,                 /* callargs  */
  YYSYMBOL_expr1 = 104,                    /* expr1  */
  YYSYMBOL_expr2 = 105,                    /* expr2  */
  YYSYMBOL_sizeof_type = 106,              /* sizeof_type  */
  YYSYMBOL_expr3 = 107,                    /* expr3  */
  YYSYMBOL_expr4 = 108,                    /* expr4  */
  YYSYMBOL_expr4_5 = 109,                  /* expr4_5  */
  YYSYMBOL_expr5 = 110,                    /* expr5  */
  YYSYMBOL_expr6 = 111,                    /* expr6  */
  YYSYMBOL_expr7 = 112,                    /* expr7  */
  YYSYMBOL_expr8 = 113,                    /* expr8  */
  YYSYMBOL_expr9 = 114,                    /* expr9  */
  YYSYMBOL_expr10 = 115,                   /* expr10  */
  YYSYMBOL_expr11 = 116,                   /* expr11  */
  YYSYMBOL_expr12 = 117,                   /* expr12  */
  YYSYMBOL_expr13 = 118,                   /* expr13  */
  YYSYMBOL_expr14 = 119,                   /* expr14  */
  YYSYMBOL_expr = 120,                     /* expr  */
  YYSYMBOL_expr_comma = 121,               /* expr_comma  */
  YYSYMBOL_primtype0 = 122,                /* primtype0  */
  YYSYMBOL__arrlit = 123,                  /* _arrlit  */
  YYSYMBOL_arrlit = 124,                   /* arrlit  */
  YYSYMBOL_metadata = 125,                 /* metadata  */
  YYSYMBOL_cdecltail = 126,                /* cdecltail  */
  YYSYMBOL__cdecltail = 127,               /* _cdecltail  */
  YYSYMBOL_cdecl = 128,                    /* cdecl  */
  YYSYMBOL_cbody = 129,                    /* cbody  */
  YYSYMBOL__cheader = 130,                 /* _cheader  */
  YYSYMBOL__uheader = 131,                 /* _uheader  */
  YYSYMBOL_cheader = 132,                  /* cheader  */
  YYSYMBOL_uheader = 133,                  /* uheader  */
  YYSYMBOL_class = 134,                    /* class  */
  YYSYMBOL_union = 135,                    /* union  */
  YYSYMBOL__class = 136,                   /* _class  */
  YYSYMBOL__union = 137,                   /* _union  */
  YYSYMBOL_tryblock = 138,                 /* tryblock  */
  YYSYMBOL_namewptrs = 139,                /* namewptrs  */
  YYSYMBOL_vardecltail = 140,              /* vardecltail  */
  YYSYMBOL___ptrcount = 141,               /* __ptrcount  */
  YYSYMBOL_vardecltail_asn = 142,          /* vardecltail_asn  */
  YYSYMBOL_single_decl = 143,              /* single_decl  */
  YYSYMBOL_vardecltails = 144,             /* vardecltails  */
  YYSYMBOL_multi_decl = 145,               /* multi_decl  */
  YYSYMBOL_funcargs = 146,                 /* funcargs  */
  YYSYMBOL_linkage = 147,                  /* linkage  */
  YYSYMBOL_func = 148,                     /* func  */
  YYSYMBOL_ocstmt = 149,                   /* ocstmt  */
  YYSYMBOL_expr_opt = 150,                 /* expr_opt  */
  YYSYMBOL_loop_header = 151,              /* loop_header  */
  YYSYMBOL_ifcl = 152,                     /* ifcl  */
  YYSYMBOL_ostmt = 153,                    /* ostmt  */
  YYSYMBOL_cstmt = 154,                    /* cstmt  */
  YYSYMBOL_dostmt = 155,                   /* dostmt  */
  YYSYMBOL_subswit = 156,                  /* subswit  */
  YYSYMBOL_swit_body_stmt = 157,           /* swit_body_stmt  */
  YYSYMBOL_swit_body = 158,                /* swit_body  */
  YYSYMBOL_swit = 159,                     /* swit  */
  YYSYMBOL_simple_stmt = 160,              /* simple_stmt  */
  YYSYMBOL_stmts = 161,                    /* stmts  */
  YYSYMBOL_scope = 162,                    /* scope  */
  YYSYMBOL_global_stmts = 163,             /* global_stmts  */
  YYSYMBOL_global_stmt = 164               /* global_stmt  */
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
#define YYFINAL  97
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1401

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  100
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  65
/* YYNRULES -- Number of rules.  */
#define YYNRULES  232
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  410

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   354


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
      95,    96,    97,    98,    99
};

#if HC_DEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    58,    59,    60,    61,    62,    63,    66,    69,    70,
      71,    74,    75,    76,    81,    82,    86,    90,    94,    98,
     102,   106,   110,   115,   120,   128,   129,   134,   135,   139,
     143,   149,   155,   159,   164,   168,   172,   176,   180,   184,
     190,   191,   195,   199,   204,   205,   209,   213,   218,   219,
     224,   225,   230,   231,   236,   237,   241,   246,   247,   251,
     255,   259,   264,   265,   269,   274,   275,   280,   281,   286,
     287,   292,   293,   297,   301,   305,   309,   313,   317,   321,
     325,   329,   333,   338,   340,   344,   349,   350,   351,   352,
     353,   354,   355,   356,   357,   358,   359,   360,   370,   371,
     372,   379,   380,   381,   382,   383,   388,   394,   403,   406,
     413,   414,   418,   424,   428,   432,   435,   438,   439,   444,
     449,   454,   459,   464,   468,   469,   470,   476,   482,   486,
     491,   496,   511,   527,   535,   539,   545,   552,   560,   561,
     564,   568,   572,   576,   588,   601,   615,   623,   632,   638,
     645,   649,   654,   661,   662,   664,   669,   670,   676,   680,
     681,   682,   683,   684,   685,   686,   687,   691,   704,   718,
     731,   745,   761,   775,   789,   804,   817,   831,   847,   865,
     866,   868,   869,   872,   878,   886,   892,   897,   904,   915,
     916,   923,   934,   940,   947,   948,   952,   956,   960,   966,
     967,   968,   969,   976,   983,   984,   988,   992,   993,   997,
    1001,  1008,  1009,  1010,  1014,  1018,  1022,  1026,  1030,  1031,
    1032,  1038,  1044,  1045,  1046,  1047,  1052,  1057,  1058,  1063,
    1067,  1072,  1078
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
  "EQ_ADD", "EQ_SUB", "COMMA", "OTHER", "TRY", "CATCH", "LASTCLASS", "U0",
  "LEFT_PAREN", "RIGHT_PAREN", "INC", "DEC", "NAME", "LEFT_SQAURE",
  "RIGHT_SQAURE", "SEMI", "IF", "ELSE", "DO", "WHILE", "FOR", "LEFT_CURLY",
  "RIGHT_CURLY", "CASE", "COLON", "DOT_DOT_DOT", "EXTERN2", "LOCK",
  "EXTERN", "IMPORT", "IMPORT2", "STATIC", "PUBLIC", "CLASS", "UNION",
  "INTERN", "START", "END", "DEFAULT", "BREAK", "RET", "GOTO", "BOOL",
  "U8", "I8", "U16", "I16", "U32", "I32", "U64", "I64", "F64", "SWITCH",
  "EXE", "EVAL", "NL", "DBG", "STRING", "TYPENAME", "$accept", "expr0",
  "_callargs", "callargs", "expr1", "expr2", "sizeof_type", "expr3",
  "expr4", "expr4_5", "expr5", "expr6", "expr7", "expr8", "expr9",
  "expr10", "expr11", "expr12", "expr13", "expr14", "expr", "expr_comma",
  "primtype0", "_arrlit", "arrlit", "metadata", "cdecltail", "_cdecltail",
  "cdecl", "cbody", "_cheader", "_uheader", "cheader", "uheader", "class",
  "union", "_class", "_union", "tryblock", "namewptrs", "vardecltail",
  "__ptrcount", "vardecltail_asn", "single_decl", "vardecltails",
  "multi_decl", "funcargs", "linkage", "func", "ocstmt", "expr_opt",
  "loop_header", "ifcl", "ostmt", "cstmt", "dostmt", "subswit",
  "swit_body_stmt", "swit_body", "swit", "simple_stmt", "stmts", "scope",
  "global_stmts", "global_stmt", YY_NULLPTR
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
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354
};
#endif

#define YYPACT_NINF (-311)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     186,   -29,  1127,  1127,   953,    56,   861,  -311,  -311,  -311,
    -311,  1107,  1127,  1127,  1127,  1127,  1127,  1127,  -311,  1127,
    1127,  1127,  -311,  -311,  -311,  -311,    94,  -311,   305,   308,
     114,    95,   132,    28,   251,   109,   124,   133,   134,   230,
    -311,   -17,   145,   -29,  -311,   160,  -311,   180,   953,   202,
     209,   -36,  -311,  -311,   -32,  -311,  -311,   -21,   -15,   142,
     284,   232,  -311,  -311,  -311,  -311,  -311,  -311,  -311,  -311,
    -311,  -311,   237,  -311,     3,     0,   226,   228,   234,   241,
    -311,  -311,   248,   252,  -311,   265,  1274,  -311,  -311,   953,
     953,  -311,  -311,   273,  -311,  -311,  -311,  -311,  -311,  -311,
     401,   120,  -311,   292,   255,  -311,  -311,  -311,  -311,  -311,
    -311,  -311,  -311,     5,  -311,  -311,   279,   281,   120,  -311,
    -311,  1127,  1127,  1127,  1127,  1127,  1127,  1127,  1127,  1127,
    1127,  1127,  1127,  1127,  1127,  1127,  1127,  1127,  1127,  1127,
    1127,  1127,  1127,  1127,  1127,  1127,  1127,  1127,  1127,  1127,
    1127,  1127,  1127,  1127,  -311,   306,  -311,  1127,   278,  1127,
    1127,  -311,  -311,  -311,  -311,  -311,  -311,  -311,  -311,  -311,
    -311,    73,   299,  1127,  -311,    20,  -311,   303,   311,    80,
     313,  -311,   321,   267,   268,  -311,  -311,  -311,  -311,  -311,
      -6,    -2,     0,   312,  -311,  -311,  -311,   315,  -311,  -311,
    -311,  -311,    49,    68,  -311,  -311,  -311,  -311,  -311,  -311,
      71,  -311,    10,    97,  -311,  -311,  -311,  -311,  -311,  -311,
    -311,  -311,  -311,  -311,  -311,  -311,  -311,  -311,  -311,  -311,
    -311,  -311,  -311,  -311,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,  -311,   -29,   104,   322,   107,
     145,   318,  -311,  -311,   112,    29,  -311,  -311,   352,  1045,
    1065,   360,    30,  -311,  -311,  1152,  1171,   320,   323,   328,
    -311,   953,  -311,  -311,  1127,  -311,    83,  -311,  -311,  -311,
    -311,  1127,  -311,  1127,   316,   327,  -311,   333,     0,  -311,
     116,    19,  -311,  -311,  -311,   101,    54,  -311,  -311,  -311,
       0,   330,  -311,  -311,  -311,  -311,  -311,  1199,  -311,  -311,
    -311,    89,  -311,   127,   331,   493,  -311,    -4,    80,  -311,
    1227,    57,   325,  -311,   -24,  -311,  -311,    74,   343,   351,
    -311,   122,  -311,   347,   141,   348,  -311,  -311,  1127,  -311,
    1118,   334,   342,  -311,  -311,  -311,   585,  -311,  -311,   358,
    -311,  -311,  -311,  -311,   175,  -311,   357,   363,   365,    30,
    1127,  -311,   161,  1255,   185,  -311,   367,  -311,   135,   769,
    -311,  -311,  -311,   190,  -311,  -311,  -311,   370,   366,  1302,
    -311,  -311,  -311,  -311,   371,  -311,  -311,  -311,  -311,  1127,
     677,  -311,  -311,   375,  1302,   149,   197,   364,   368,  1302,
     170,  1302,  -311,  -311,  -311,  -311,  -311,   171,  -311,  -311
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
     227,     0,     0,     0,   229,     0,     0,   230,     4,     3,
       2,     0,     0,     0,     0,     0,     0,     0,     6,     0,
       0,     0,     7,     5,    12,    14,    27,    40,    44,    48,
      50,    52,    54,    57,    62,    65,    67,    69,    71,    83,
      85,     0,   232,     0,    87,     7,   214,     0,     0,     0,
       0,     0,   159,   160,     0,   162,   161,   118,   119,     0,
       0,     0,    86,    88,    89,    90,    91,    92,    93,    94,
      95,    96,     0,    97,     0,   139,   124,   125,     0,     0,
     218,   219,    98,    99,   204,     0,     0,   207,   228,     0,
       0,   179,   180,     0,   211,   189,   212,     1,   226,   222,
       0,     0,    28,    29,    25,    98,    99,    38,    39,    33,
      32,    36,    37,     0,    35,    34,     0,     0,     8,    19,
      18,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   231,     0,   217,     0,     0,     0,
     182,   163,   165,   164,   166,   120,   122,   121,   123,   213,
     216,     0,     0,     0,   210,   140,   134,   100,   138,   154,
       0,   153,   155,     0,     0,   117,   117,   128,   130,   205,
     118,   119,   139,     0,   188,   191,   186,   180,   209,   224,
     225,   223,     0,     0,    26,   100,    13,    15,    16,    11,
       0,     9,     0,     0,    41,    42,    43,    45,    46,    47,
      49,    51,    53,    55,    56,    58,    59,    60,    61,    64,
      63,    66,    68,    70,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    84,     0,     0,     0,     0,
     181,     0,   215,   208,     0,   141,   135,   129,   158,     0,
       0,     0,   139,   126,   127,     0,     0,   120,   121,   138,
     206,     0,    31,    30,     8,    20,     0,    21,    17,   133,
     185,     0,   183,   182,     0,   142,   136,     0,   139,   156,
       0,     0,   148,   149,   147,     0,     0,   138,   152,   131,
     139,     0,   116,   115,   132,   221,   220,   158,   187,   190,
      10,     0,    22,     0,     0,     0,   137,     0,   150,   151,
       0,     0,     0,   103,     0,   104,   146,     0,     0,   113,
     110,   111,   114,     0,     0,     0,    23,   192,   182,   203,
       0,     0,     0,   194,   199,   201,     0,   169,   173,     0,
     157,   167,   175,   107,     0,   105,     0,     0,     0,   139,
       0,   112,     0,     0,     0,    24,     0,   197,     0,     0,
     195,   202,   200,     0,   106,   101,   102,     0,     0,   158,
     109,   108,   172,   176,     0,   170,   178,   184,   196,     0,
       0,   168,   174,     0,   158,     0,     0,     0,     0,   158,
       0,     0,   143,   171,   177,   198,   193,     0,   144,   145
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -311,  -311,   151,  -311,  -311,  -311,   329,   219,   204,   231,
     300,   302,   304,    84,   179,   105,   293,   307,   314,   199,
    -110,     1,    -9,  -311,  -280,  -311,  -311,    86,  -311,   270,
    -311,  -311,  -311,  -311,   -11,    35,     2,   -10,  -311,   -66,
    -272,  -311,   177,  -306,   205,   373,  -289,  -311,  -311,     6,
    -252,  -311,  -311,   -82,   -84,  -311,  -311,  -310,   100,  -311,
    -311,  -311,    -1,  -311,  -311
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    24,   209,   210,    25,    26,   103,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    74,    75,   324,   293,   361,   329,   330,   301,   265,
      76,    77,    78,    79,    80,    81,    82,    83,    84,   297,
     179,   180,   181,   289,   182,    85,   290,    86,    87,   343,
     251,    89,    90,    91,    92,    93,   344,   345,   346,    94,
      95,   100,    96,     4,     5
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
       7,   106,   104,    41,    42,   195,   197,   194,   211,   178,
      88,   325,    99,   105,   350,   175,   318,   161,   334,   354,
     113,   163,     8,     9,    10,   276,   153,    11,   331,    12,
      13,   314,   165,     6,    14,   255,   372,    15,   167,   355,
      16,    17,   155,   245,   285,   175,   153,   267,   153,   131,
     132,   268,   347,   176,   158,   206,    97,   350,     6,   174,
     277,   171,   322,   162,   204,   177,    18,   164,    19,   327,
      20,    21,    22,   256,   376,    58,   106,   192,   166,   154,
     372,   291,   286,   176,   168,    58,   366,   331,   105,   356,
     395,   106,   104,   166,   205,   350,   196,   168,   311,   272,
     116,   117,   203,   105,   335,   400,   201,   328,   106,   212,
     407,   153,   259,   351,   274,   129,   153,    23,   273,     6,
     105,   275,   213,     8,     9,    10,   269,   357,    11,   252,
      12,    13,   128,   312,   260,    14,   137,   138,    15,   336,
     153,    16,    17,   118,   153,   119,   120,   153,   121,   292,
     153,   130,   278,   139,   280,   153,   326,   282,   247,   320,
     249,   250,   284,   140,   211,   141,   321,    18,    44,    19,
     153,    20,    21,    22,   254,   360,   260,   337,     8,     9,
      10,   323,   205,    11,   363,    12,    13,   309,   153,   308,
      14,   364,   401,    15,    57,    58,    16,    17,   169,   402,
     388,   389,   205,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,   401,   401,   223,   224,   382,    23,    73,
     408,   409,    18,     6,    19,   156,    20,    21,    22,   157,
     368,   107,   108,   109,   110,   111,   112,   291,   374,   114,
     115,   385,   229,   230,   375,   279,   391,     6,   106,   288,
     381,   159,     6,   403,   302,   302,   300,   300,   160,     6,
     105,   295,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   152,    23,   133,   134,   135,   136,   205,   397,
       1,     2,   313,     3,   250,   172,   173,     8,     9,    10,
     177,   183,    11,   184,    12,    13,   185,   106,   288,    14,
     303,   303,    15,   186,   187,    16,    17,   204,   188,   105,
     106,   288,   225,   226,   227,   228,   348,   122,   123,   124,
     352,   189,   105,   125,   126,   127,   214,   215,   216,   198,
      58,    18,   207,    19,   208,    20,    21,    22,   248,   250,
     170,   234,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   244,   246,   106,   288,   253,   217,   218,   219,   257,
     258,   383,   261,   386,   262,   105,   263,   264,   270,   106,
     288,   281,   392,   271,   283,   296,   305,   307,   315,   306,
     316,   105,    23,   317,   106,   288,   332,   338,   353,   106,
     288,   106,   288,   358,   359,   404,   105,   362,   365,   369,
      44,   105,   199,   105,     8,     9,    10,   370,   373,    11,
     377,    12,    13,   378,   379,   394,    14,   387,   287,    15,
     393,   396,    16,    17,   399,   310,    57,    58,   220,   405,
     202,   221,   231,   406,   222,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,   380,    43,   232,    18,    44,
      19,    73,    20,    21,    45,   233,   266,    46,    47,   193,
      48,    49,    50,     6,   200,   319,     0,   298,    51,   390,
      52,    53,    54,    55,    56,    57,    58,     0,     0,     0,
       0,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,     0,     8,     9,    10,    23,
      73,    11,     0,    12,    13,     0,     0,     0,    14,     0,
       0,    15,     0,     0,    16,    17,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    43,     0,
      18,    44,    19,     0,    20,    21,    45,     0,     0,    46,
      47,     0,    48,    49,    50,     6,   339,   340,     0,     0,
      51,     0,    52,    53,    54,    55,    56,    57,    58,     0,
     341,     0,   342,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,     0,     8,     9,
      10,    23,    73,    11,     0,    12,    13,     0,     0,     0,
      14,     0,     0,    15,     0,     0,    16,    17,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      43,     0,    18,    44,    19,     0,    20,    21,    45,     0,
       0,    46,    47,     0,    48,    49,    50,     6,   371,   340,
       0,     0,    51,     0,    52,    53,    54,    55,    56,    57,
      58,     0,   341,     0,   342,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,     0,
       8,     9,    10,    23,    73,    11,     0,    12,    13,     0,
       0,     0,    14,     0,     0,    15,     0,     0,    16,    17,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    43,     0,    18,    44,    19,     0,    20,    21,
      45,     0,     0,    46,    47,     0,    48,    49,    50,     6,
       0,   340,     0,     0,    51,     0,    52,    53,    54,    55,
      56,    57,    58,     0,   341,   398,   342,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,     0,     8,     9,    10,    23,    73,    11,     0,    12,
      13,     0,     0,     0,    14,     0,     0,    15,     0,     0,
      16,    17,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,     0,    18,    44,    19,     0,
      20,    21,    45,     0,     0,    46,    47,     0,    48,    49,
      50,     6,     0,   340,     0,     0,    51,     0,    52,    53,
      54,    55,    56,    57,    58,     0,   341,     0,   342,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,     0,     8,     9,    10,    23,    73,    11,
       0,    12,    13,     0,     0,     0,    14,     0,     0,    15,
       0,     0,    16,    17,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    43,     0,    18,    44,
      19,     0,    20,    21,    45,     0,     0,    46,    47,     0,
      48,    49,    50,     6,    98,     0,     0,     0,    51,     0,
      52,    53,    54,    55,    56,    57,    58,     0,     0,     0,
       0,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,     0,     8,     9,    10,    23,
      73,    11,     0,    12,    13,     0,     0,     0,    14,     0,
       0,    15,     0,     0,    16,    17,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    43,     0,
      18,    44,    19,     0,    20,    21,    45,     0,     0,    46,
      47,     0,    48,    49,    50,     6,     0,     0,     0,     0,
      51,     0,    52,    53,    54,    55,    56,    57,    58,     0,
       0,     0,     0,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,     0,     8,     9,
      10,    23,    73,    11,     0,    12,    13,     0,     0,     0,
      14,     0,     0,    15,     0,     0,    16,    17,     8,     9,
      10,     0,     0,    11,     0,    12,    13,     0,     0,     0,
      14,     0,     0,    15,     0,     0,    16,    17,     0,     0,
       0,     0,    18,     0,    19,     0,    20,    21,    22,     0,
       0,     0,     0,     0,     0,     0,     0,   291,     0,     0,
       0,     0,    18,     0,    19,     0,    20,    21,    22,     0,
     294,     8,     9,    10,     0,     0,    11,     0,    12,    13,
       8,     9,    10,    14,     0,    11,    15,    12,    13,    16,
      17,     0,    14,    23,     0,    15,     0,     0,    16,    17,
       0,     0,     0,     0,     0,    44,   101,     0,     0,     0,
     102,     0,     0,    23,     0,    18,     0,    19,     0,    20,
      21,    22,     0,     0,    18,     0,    19,     0,    20,    21,
      22,    57,    58,   367,     0,     0,     0,     0,     0,     0,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      44,     0,     0,     0,     0,     0,    73,     0,     0,     0,
       0,     0,     0,     0,     0,   299,    23,     0,     0,    44,
       0,     0,     0,     0,     0,    23,    57,    58,     0,     0,
       0,     0,     0,     0,   304,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    57,    58,    44,     0,     0,
       0,    73,     0,     0,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,     0,   333,     0,     0,     0,     0,
      73,     0,     0,    57,    58,    44,     0,     0,     0,     0,
       0,     0,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,     0,   349,     0,     0,     0,     0,    73,     0,
       0,    57,    58,    44,     0,     0,     0,     0,     0,     0,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
       0,   384,    44,     0,     0,     0,    73,     0,     0,    57,
      58,     0,     0,     0,     0,     0,     0,     0,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,   190,   191,
      44,     0,     0,     0,    73,     0,     0,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,     0,     0,     0,
       0,     0,     0,    73,     0,     0,    57,    58,     0,     0,
       0,     0,     0,     0,     0,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,     0,     0,     0,     0,     0,
       0,    73
};

static const yytype_int16 yycheck[] =
{
       1,    11,    11,     2,     3,    89,    90,    89,   118,    75,
       4,   291,     6,    11,   320,    15,   288,    53,   307,    43,
      19,    53,     3,     4,     5,    15,    43,     8,   300,    10,
      11,   283,    53,    62,    15,    15,   346,    18,    53,    63,
      21,    22,    43,   153,    15,    15,    43,    53,    43,    21,
      22,    53,    56,    53,    48,    50,     0,   363,    62,    56,
      50,    60,    43,    99,    15,    75,    47,    99,    49,    15,
      51,    52,    53,    53,   354,    75,    86,    86,    99,    96,
     390,    62,    53,    53,    99,    75,   338,   359,    86,    15,
     379,   101,   101,    99,   104,   401,    90,    99,    15,    50,
       6,     7,   101,   101,    15,   394,   100,    53,   118,   118,
     399,    43,    32,    56,    43,    20,    43,    98,    50,    62,
     118,    50,   121,     3,     4,     5,   192,    53,     8,    56,
      10,    11,    18,    50,    54,    15,    27,    28,    18,    50,
      43,    21,    22,    49,    43,    51,    52,    43,    54,   259,
      43,    19,    55,    29,    50,    43,    55,    50,   157,    43,
     159,   160,    50,    30,   274,    31,    50,    47,    48,    49,
      43,    51,    52,    53,   173,    53,    54,    50,     3,     4,
       5,   291,   192,     8,    43,    10,    11,   271,    43,   271,
      15,    50,    43,    18,    74,    75,    21,    22,    56,    50,
      65,    66,   212,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    43,    43,   131,   132,    56,    98,    99,
      50,    50,    47,    62,    49,    65,    51,    52,    53,    49,
     340,    12,    13,    14,    15,    16,    17,    62,    63,    20,
      21,    56,   137,   138,   354,   246,    56,    62,   258,   258,
     360,    49,    62,    56,   265,   266,   265,   266,    49,    62,
     258,   260,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    98,    23,    24,    25,    26,   288,   389,
      94,    95,   281,    97,   283,    53,    49,     3,     4,     5,
     300,    65,     8,    65,    10,    11,    62,   307,   307,    15,
     265,   266,    18,    62,    56,    21,    22,    15,    56,   307,
     320,   320,   133,   134,   135,   136,   317,    12,    13,    14,
     321,    56,   320,    15,    16,    17,   122,   123,   124,    56,
      75,    47,    53,    49,    53,    51,    52,    53,    60,   338,
      56,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,    46,   363,   363,    56,   125,   126,   127,    56,
      49,   362,    49,   364,    43,   363,    99,    99,    56,   379,
     379,    49,   373,    58,    56,    15,    56,    49,    62,    56,
      53,   379,    98,    50,   394,   394,    56,    56,    63,   399,
     399,   401,   401,    50,    43,   396,   394,    50,    50,    65,
      48,   399,     1,   401,     3,     4,     5,    65,    50,     8,
      53,    10,    11,    50,    49,    49,    15,    50,    66,    18,
      50,    50,    21,    22,    49,   274,    74,    75,   128,    65,
     101,   129,   139,    65,   130,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,   359,    45,   140,    47,    48,
      49,    99,    51,    52,    53,   141,   186,    56,    57,    86,
      59,    60,    61,    62,    63,   288,    -1,   262,    67,   369,
      69,    70,    71,    72,    73,    74,    75,    -1,    -1,    -1,
      -1,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    -1,     3,     4,     5,    98,
      99,     8,    -1,    10,    11,    -1,    -1,    -1,    15,    -1,
      -1,    18,    -1,    -1,    21,    22,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      47,    48,    49,    -1,    51,    52,    53,    -1,    -1,    56,
      57,    -1,    59,    60,    61,    62,    63,    64,    -1,    -1,
      67,    -1,    69,    70,    71,    72,    73,    74,    75,    -1,
      77,    -1,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    -1,     3,     4,
       5,    98,    99,     8,    -1,    10,    11,    -1,    -1,    -1,
      15,    -1,    -1,    18,    -1,    -1,    21,    22,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    -1,    47,    48,    49,    -1,    51,    52,    53,    -1,
      -1,    56,    57,    -1,    59,    60,    61,    62,    63,    64,
      -1,    -1,    67,    -1,    69,    70,    71,    72,    73,    74,
      75,    -1,    77,    -1,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    -1,
       3,     4,     5,    98,    99,     8,    -1,    10,    11,    -1,
      -1,    -1,    15,    -1,    -1,    18,    -1,    -1,    21,    22,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    47,    48,    49,    -1,    51,    52,
      53,    -1,    -1,    56,    57,    -1,    59,    60,    61,    62,
      -1,    64,    -1,    -1,    67,    -1,    69,    70,    71,    72,
      73,    74,    75,    -1,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    -1,     3,     4,     5,    98,    99,     8,    -1,    10,
      11,    -1,    -1,    -1,    15,    -1,    -1,    18,    -1,    -1,
      21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    47,    48,    49,    -1,
      51,    52,    53,    -1,    -1,    56,    57,    -1,    59,    60,
      61,    62,    -1,    64,    -1,    -1,    67,    -1,    69,    70,
      71,    72,    73,    74,    75,    -1,    77,    -1,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    -1,     3,     4,     5,    98,    99,     8,
      -1,    10,    11,    -1,    -1,    -1,    15,    -1,    -1,    18,
      -1,    -1,    21,    22,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    47,    48,
      49,    -1,    51,    52,    53,    -1,    -1,    56,    57,    -1,
      59,    60,    61,    62,    63,    -1,    -1,    -1,    67,    -1,
      69,    70,    71,    72,    73,    74,    75,    -1,    -1,    -1,
      -1,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    -1,     3,     4,     5,    98,
      99,     8,    -1,    10,    11,    -1,    -1,    -1,    15,    -1,
      -1,    18,    -1,    -1,    21,    22,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,
      47,    48,    49,    -1,    51,    52,    53,    -1,    -1,    56,
      57,    -1,    59,    60,    61,    62,    -1,    -1,    -1,    -1,
      67,    -1,    69,    70,    71,    72,    73,    74,    75,    -1,
      -1,    -1,    -1,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    -1,     3,     4,
       5,    98,    99,     8,    -1,    10,    11,    -1,    -1,    -1,
      15,    -1,    -1,    18,    -1,    -1,    21,    22,     3,     4,
       5,    -1,    -1,     8,    -1,    10,    11,    -1,    -1,    -1,
      15,    -1,    -1,    18,    -1,    -1,    21,    22,    -1,    -1,
      -1,    -1,    47,    -1,    49,    -1,    51,    52,    53,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    62,    -1,    -1,
      -1,    -1,    47,    -1,    49,    -1,    51,    52,    53,    -1,
      55,     3,     4,     5,    -1,    -1,     8,    -1,    10,    11,
       3,     4,     5,    15,    -1,     8,    18,    10,    11,    21,
      22,    -1,    15,    98,    -1,    18,    -1,    -1,    21,    22,
      -1,    -1,    -1,    -1,    -1,    48,    49,    -1,    -1,    -1,
      53,    -1,    -1,    98,    -1,    47,    -1,    49,    -1,    51,
      52,    53,    -1,    -1,    47,    -1,    49,    -1,    51,    52,
      53,    74,    75,    65,    -1,    -1,    -1,    -1,    -1,    -1,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      48,    -1,    -1,    -1,    -1,    -1,    99,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    63,    98,    -1,    -1,    48,
      -1,    -1,    -1,    -1,    -1,    98,    74,    75,    -1,    -1,
      -1,    -1,    -1,    -1,    63,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    74,    75,    48,    -1,    -1,
      -1,    99,    -1,    -1,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    -1,    66,    -1,    -1,    -1,    -1,
      99,    -1,    -1,    74,    75,    48,    -1,    -1,    -1,    -1,
      -1,    -1,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    -1,    66,    -1,    -1,    -1,    -1,    99,    -1,
      -1,    74,    75,    48,    -1,    -1,    -1,    -1,    -1,    -1,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      -1,    66,    48,    -1,    -1,    -1,    99,    -1,    -1,    74,
      75,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    74,    75,
      48,    -1,    -1,    -1,    99,    -1,    -1,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    -1,    -1,    -1,
      -1,    -1,    -1,    99,    -1,    -1,    74,    75,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    -1,    -1,    -1,    -1,    -1,
      -1,    99
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    94,    95,    97,   163,   164,    62,   162,     3,     4,
       5,     8,    10,    11,    15,    18,    21,    22,    47,    49,
      51,    52,    53,    98,   101,   104,   105,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   121,    45,    48,    53,    56,    57,    59,    60,
      61,    67,    69,    70,    71,    72,    73,    74,    75,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    99,   121,   122,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   145,   147,   148,   149,   151,
     152,   153,   154,   155,   159,   160,   162,     0,    63,   149,
     161,    49,    53,   106,   122,   136,   137,   107,   107,   107,
     107,   107,   107,   121,   107,   107,     6,     7,    49,    51,
      52,    54,    12,    13,    14,    15,    16,    17,    18,    20,
      19,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    96,   162,    65,    49,   149,    49,
      49,    53,    99,    53,    99,    53,    99,    53,    99,    56,
      56,   121,    53,    49,    56,    15,    53,   137,   139,   140,
     141,   142,   144,    65,    65,    62,    62,    56,    56,    56,
      74,    75,   122,   145,   153,   154,   149,   154,    56,     1,
      63,   149,   106,   121,    15,   137,    50,    53,    53,   102,
     103,   120,   122,   121,   108,   108,   108,   109,   109,   109,
     110,   111,   112,   113,   113,   114,   114,   114,   114,   115,
     115,   116,   117,   118,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   120,    46,   121,    60,   121,
     121,   150,    56,    56,   121,    15,    53,    56,    49,    32,
      54,    49,    43,    99,    99,   129,   129,    53,    53,   139,
      56,    58,    50,    50,    43,    50,    15,    50,    55,   162,
      50,    49,    50,    56,    50,    15,    53,    66,   122,   143,
     146,    62,   120,   124,    55,   121,    15,   139,   144,    63,
     122,   128,   134,   135,    63,    56,    56,    49,   153,   154,
     102,    15,    50,   121,   150,    62,    53,    50,   140,   142,
      43,    50,    43,   120,   123,   124,    55,    15,    53,   126,
     127,   140,    56,    66,   146,    15,    50,    50,    56,    63,
      64,    77,    79,   149,   156,   157,   158,    56,   162,    66,
     143,    56,   162,    63,    43,    63,    15,    53,    50,    43,
      53,   125,    50,    43,    50,    50,   150,    65,   120,    65,
      65,    63,   157,    50,    63,   120,   124,    53,    50,    49,
     127,   120,    56,   162,    66,    56,   162,    50,    65,    66,
     158,    56,   162,    50,    49,   146,    50,   120,    78,    49,
     146,    43,    50,    56,   162,    65,    65,   146,    50,    50
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   100,   101,   101,   101,   101,   101,   101,   102,   102,
     103,   103,   104,   104,   105,   105,   105,   105,   105,   105,
     105,   105,   105,   105,   105,   106,   106,   107,   107,   107,
     107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     108,   108,   108,   108,   109,   109,   109,   109,   110,   110,
     111,   111,   112,   112,   113,   113,   113,   114,   114,   114,
     114,   114,   115,   115,   115,   116,   116,   117,   117,   118,
     118,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   120,   121,   121,   122,   122,   122,   122,
     122,   122,   122,   122,   122,   122,   122,   122,   122,   122,
     122,   123,   123,   123,   123,   124,   124,   124,   125,   126,
     126,   127,   127,   128,   129,   129,   129,   129,   130,   131,
     130,   131,   130,   131,   132,   133,   132,   133,   134,   135,
     135,   136,   137,   138,   139,   139,   139,   139,   140,   141,
     141,   141,   141,   140,   140,   140,   140,   140,   142,   142,
     143,   143,   144,   144,   144,   145,   146,   146,   146,   147,
     147,   147,   147,   147,   147,   147,   147,   148,   148,   148,
     148,   148,   148,   148,   148,   148,   148,   148,   148,   149,
     149,   150,   150,   151,   151,   152,   153,   153,   153,   154,
     154,   154,   155,   156,   157,   157,   157,   157,   157,   157,
     158,   158,   159,   159,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   161,   161,   161,   162,   162,   163,   163,   164,
     164,   164,   164
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     0,     1,
       3,     1,     1,     3,     1,     3,     3,     4,     2,     2,
       4,     4,     5,     6,     7,     1,     2,     1,     2,     2,
       4,     4,     2,     2,     2,     2,     2,     2,     2,     2,
       1,     3,     3,     3,     1,     3,     3,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     3,     1,     3,     3,
       3,     3,     1,     3,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     3,     3,     1,     1,     3,     4,     3,     2,     3,
       1,     1,     2,     2,     3,     2,     2,     0,     1,     1,
       2,     2,     2,     2,     1,     1,     3,     3,     2,     3,
       2,     4,     4,     4,     1,     2,     3,     4,     1,     0,
       1,     2,     3,     8,     9,    10,     4,     3,     3,     3,
       2,     2,     3,     1,     1,     2,     1,     3,     0,     1,
       1,     1,     1,     2,     2,     2,     2,     6,     8,     6,
       7,     9,     7,     6,     8,     6,     7,     9,     7,     1,
       1,     1,     0,     4,     8,     4,     2,     4,     2,     1,
       4,     2,     6,     5,     1,     2,     3,     2,     5,     1,
       2,     1,     7,     6,     1,     2,     3,     1,     3,     2,
       2,     1,     1,     2,     1,     3,     2,     2,     1,     1,
       4,     4,     1,     2,     2,     3,     2,     0,     2,     1,
       2,     3,     2
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
  case 2: /* expr0: FLOAT  */
#line 59 "HolyC.y"
                {yyval=SLE(yyvsp[0]);}
#line 1757 "HolyC.tab.c"
    break;

  case 3: /* expr0: INT  */
#line 60 "HolyC.y"
              {yyval=SLE(yyvsp[0]);}
#line 1763 "HolyC.tab.c"
    break;

  case 4: /* expr0: CHAR  */
#line 61 "HolyC.y"
               {yyval=SLE(yyvsp[0]);}
#line 1769 "HolyC.tab.c"
    break;

  case 5: /* expr0: STRING  */
#line 62 "HolyC.y"
                 {yyval=SLE(yyvsp[0]);}
#line 1775 "HolyC.tab.c"
    break;

  case 6: /* expr0: LASTCLASS  */
#line 63 "HolyC.y"
                    {
  yyval=SLE(yyvsp[0]);
}
#line 1783 "HolyC.tab.c"
    break;

  case 7: /* expr0: NAME  */
#line 66 "HolyC.y"
               {
  yyval=SLE(yyvsp[0]);
}
#line 1791 "HolyC.tab.c"
    break;

  case 8: /* _callargs: %empty  */
#line 69 "HolyC.y"
              {yyval=NULL;}
#line 1797 "HolyC.tab.c"
    break;

  case 9: /* _callargs: expr  */
#line 70 "HolyC.y"
                      {yyval=yyvsp[0];}
#line 1803 "HolyC.tab.c"
    break;

  case 10: /* callargs: callargs COMMA _callargs  */
#line 71 "HolyC.y"
                                               {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA),yyvsp[-1]);
}
#line 1811 "HolyC.tab.c"
    break;

  case 11: /* callargs: _callargs  */
#line 74 "HolyC.y"
                          {yyval=yyvsp[0];}
#line 1817 "HolyC.tab.c"
    break;

  case 13: /* expr1: LEFT_PAREN expr_comma RIGHT_PAREN  */
#line 76 "HolyC.y"
                                                         {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 1826 "HolyC.tab.c"
    break;

  case 15: /* expr2: expr2 DOT NAME  */
#line 82 "HolyC.y"
                                  {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 1835 "HolyC.tab.c"
    break;

  case 16: /* expr2: expr2 ARROW NAME  */
#line 86 "HolyC.y"
                                    {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 1844 "HolyC.tab.c"
    break;

  case 17: /* expr2: expr2 LEFT_SQAURE expr_comma RIGHT_SQAURE  */
#line 90 "HolyC.y"
                                                                    {
  yyval=SOT(CreateArrayAccess(yyvsp[-3],yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 1853 "HolyC.tab.c"
    break;

  case 18: /* expr2: expr2 DEC  */
#line 94 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[-1],AST_POST_DEC),yyvsp[0]);
  ReleaseAST(yyvsp[0]);
}
#line 1862 "HolyC.tab.c"
    break;

  case 19: /* expr2: expr2 INC  */
#line 98 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[-1],AST_POST_INC),yyvsp[0]);
  ReleaseAST(yyvsp[0]);
}
#line 1871 "HolyC.tab.c"
    break;

  case 20: /* expr2: expr2 LEFT_PAREN callargs RIGHT_PAREN  */
#line 102 "HolyC.y"
                                                                {
  yyval=SOT(CreateFuncCall(yyvsp[-3],yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 1880 "HolyC.tab.c"
    break;

  case 21: /* expr2: expr2 LEFT_PAREN primtype0 RIGHT_PAREN  */
#line 106 "HolyC.y"
                                                                 {
  yyval=CreateExplicitTypecast(yyvsp[-3],yyvsp[-1]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 1889 "HolyC.tab.c"
    break;

  case 22: /* expr2: expr2 LEFT_PAREN primtype0 MUL RIGHT_PAREN  */
#line 110 "HolyC.y"
                                                                          {
  yyvsp[-2]->type2=CreateMultiLvlPtr(yyvsp[-2]->type2,1);
  yyval=CreateExplicitTypecast(yyvsp[-4],yyvsp[-2]);
  ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 1899 "HolyC.tab.c"
    break;

  case 23: /* expr2: expr2 LEFT_PAREN primtype0 MUL MUL RIGHT_PAREN  */
#line 115 "HolyC.y"
                                                                                   {
  yyvsp[-3]->type2=CreateMultiLvlPtr(yyvsp[-3]->type2,2);
  yyval=CreateExplicitTypecast(yyvsp[-5],yyvsp[-3]);
  ReleaseAST(yyvsp[-4]),ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 1909 "HolyC.tab.c"
    break;

  case 24: /* expr2: expr2 LEFT_PAREN primtype0 MUL MUL MUL RIGHT_PAREN  */
#line 120 "HolyC.y"
                                                                                            {
  yyvsp[-4]->type2=CreateMultiLvlPtr(yyvsp[-4]->type2,3);
  yyval=CreateExplicitTypecast(yyvsp[-6],yyvsp[-4]);
  ReleaseAST(yyvsp[-5]),ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 1919 "HolyC.tab.c"
    break;

  case 25: /* sizeof_type: primtype0  */
#line 128 "HolyC.y"
                             {yyval=yyvsp[0];}
#line 1925 "HolyC.tab.c"
    break;

  case 26: /* sizeof_type: sizeof_type MUL  */
#line 129 "HolyC.y"
                                        {
  CType *ptr =CreateMultiLvlPtr(yyvsp[-1]->type2,1);
  yyval=CreateTypeNode(ptr);
  ReleaseAST(yyvsp[0]);
}
#line 1935 "HolyC.tab.c"
    break;

  case 28: /* expr3: SIZEOF NAME  */
#line 135 "HolyC.y"
                            {
  yyval=SOT(CreateSizeof(yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 1944 "HolyC.tab.c"
    break;

  case 29: /* expr3: SIZEOF sizeof_type  */
#line 139 "HolyC.y"
                                   {
  yyval=SOT(CreateSizeof(yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 1953 "HolyC.tab.c"
    break;

  case 30: /* expr3: SIZEOF LEFT_PAREN expr_comma RIGHT_PAREN  */
#line 143 "HolyC.y"
                                                                    {
  yyval=SOT(CreateSizeof(yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 1964 "HolyC.tab.c"
    break;

  case 31: /* expr3: SIZEOF LEFT_PAREN sizeof_type RIGHT_PAREN  */
#line 149 "HolyC.y"
                                                                     {
  yyval=SOT(CreateSizeof(yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 1975 "HolyC.tab.c"
    break;

  case 32: /* expr3: BAND expr3  */
#line 155 "HolyC.y"
                           {
  yyval=SOT(CreateUnop(yyvsp[0],AST_ADDROF),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 1984 "HolyC.tab.c"
    break;

  case 33: /* expr3: MUL expr3  */
#line 159 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_DERREF),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 1993 "HolyC.tab.c"
    break;

  case 34: /* expr3: DEC expr3  */
#line 164 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_PRE_DEC),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2002 "HolyC.tab.c"
    break;

  case 35: /* expr3: INC expr3  */
#line 168 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_PRE_INC),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2011 "HolyC.tab.c"
    break;

  case 36: /* expr3: ADD expr3  */
#line 172 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_POS),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2020 "HolyC.tab.c"
    break;

  case 37: /* expr3: SUB expr3  */
#line 176 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_NEG),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2029 "HolyC.tab.c"
    break;

  case 38: /* expr3: LNOT expr3  */
#line 180 "HolyC.y"
                           {
  yyval=SOT(CreateUnop(yyvsp[0],AST_LNOT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2038 "HolyC.tab.c"
    break;

  case 39: /* expr3: BNOT expr3  */
#line 184 "HolyC.y"
                           {
  yyval=SOT(CreateUnop(yyvsp[0],AST_BNOT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2047 "HolyC.tab.c"
    break;

  case 41: /* expr4: expr4 POW expr4  */
#line 191 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_POW),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2056 "HolyC.tab.c"
    break;

  case 42: /* expr4: expr4 SHL expr4  */
#line 195 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SHL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2065 "HolyC.tab.c"
    break;

  case 43: /* expr4: expr4 SHR expr4  */
#line 199 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SHR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2074 "HolyC.tab.c"
    break;

  case 45: /* expr4_5: expr4_5 MUL expr4_5  */
#line 205 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_MUL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2083 "HolyC.tab.c"
    break;

  case 46: /* expr4_5: expr4_5 DIV expr4_5  */
#line 209 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_DIV),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2092 "HolyC.tab.c"
    break;

  case 47: /* expr4_5: expr4_5 MOD expr4_5  */
#line 213 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_MOD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2101 "HolyC.tab.c"
    break;

  case 49: /* expr5: expr5 BAND expr5  */
#line 219 "HolyC.y"
                                    {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2110 "HolyC.tab.c"
    break;

  case 51: /* expr6: expr6 BXOR expr6  */
#line 225 "HolyC.y"
                                    {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2119 "HolyC.tab.c"
    break;

  case 53: /* expr7: expr7 BOR expr7  */
#line 231 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2128 "HolyC.tab.c"
    break;

  case 55: /* expr8: expr8 ADD expr8  */
#line 237 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ADD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2137 "HolyC.tab.c"
    break;

  case 56: /* expr8: expr8 SUB expr8  */
#line 241 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SUB),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2146 "HolyC.tab.c"
    break;

  case 58: /* expr9: expr9 LT expr9  */
#line 247 "HolyC.y"
                                  {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_LT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2155 "HolyC.tab.c"
    break;

  case 59: /* expr9: expr9 GT expr9  */
#line 251 "HolyC.y"
                                  {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_GT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2164 "HolyC.tab.c"
    break;

  case 60: /* expr9: expr9 LE expr9  */
#line 255 "HolyC.y"
                                  {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_LE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2173 "HolyC.tab.c"
    break;

  case 61: /* expr9: expr9 GE expr9  */
#line 259 "HolyC.y"
                                  {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_GE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2182 "HolyC.tab.c"
    break;

  case 63: /* expr10: expr10 EQ expr10  */
#line 265 "HolyC.y"
                                     {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_EQ),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2191 "HolyC.tab.c"
    break;

  case 64: /* expr10: expr10 NE expr10  */
#line 269 "HolyC.y"
                                     {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_NE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2200 "HolyC.tab.c"
    break;

  case 66: /* expr11: expr11 LAND expr11  */
#line 275 "HolyC.y"
                                       {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2209 "HolyC.tab.c"
    break;

  case 68: /* expr12: expr12 LXOR expr12  */
#line 281 "HolyC.y"
                                       {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2218 "HolyC.tab.c"
    break;

  case 70: /* expr13: expr13 LOR expr13  */
#line 287 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2227 "HolyC.tab.c"
    break;

  case 72: /* expr14: expr14 ASSIGN expr14  */
#line 293 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2236 "HolyC.tab.c"
    break;

  case 73: /* expr14: expr14 EQ_SHL expr14  */
#line 297 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SHL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2245 "HolyC.tab.c"
    break;

  case 74: /* expr14: expr14 EQ_SHR expr14  */
#line 301 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SHR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2254 "HolyC.tab.c"
    break;

  case 75: /* expr14: expr14 EQ_MUL expr14  */
#line 305 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_MUL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2263 "HolyC.tab.c"
    break;

  case 76: /* expr14: expr14 EQ_DIV expr14  */
#line 309 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_DIV),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2272 "HolyC.tab.c"
    break;

  case 77: /* expr14: expr14 EQ_MOD expr14  */
#line 313 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_MOD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2281 "HolyC.tab.c"
    break;

  case 78: /* expr14: expr14 EQ_BAND expr14  */
#line 317 "HolyC.y"
                                          {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2290 "HolyC.tab.c"
    break;

  case 79: /* expr14: expr14 EQ_BXOR expr14  */
#line 321 "HolyC.y"
                                          {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2299 "HolyC.tab.c"
    break;

  case 80: /* expr14: expr14 EQ_BOR expr14  */
#line 325 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2308 "HolyC.tab.c"
    break;

  case 81: /* expr14: expr14 EQ_ADD expr14  */
#line 329 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_ADD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2317 "HolyC.tab.c"
    break;

  case 82: /* expr14: expr14 EQ_SUB expr14  */
#line 333 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SUB),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2326 "HolyC.tab.c"
    break;

  case 84: /* expr_comma: expr_comma COMMA expr  */
#line 340 "HolyC.y"
                                              {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2335 "HolyC.tab.c"
    break;

  case 86: /* primtype0: BOOL  */
#line 349 "HolyC.y"
                     {CType *prim =CreatePrimType(TYPE_BOOL);yyval=SLE(CreateTypeNode(prim));ReleaseAST(yyvsp[0]);}
#line 2341 "HolyC.tab.c"
    break;

  case 87: /* primtype0: U0  */
#line 350 "HolyC.y"
                   {CType *prim =CreatePrimType(TYPE_U0);yyval=SLE(CreateTypeNode(prim));ReleaseAST(yyvsp[0]);}
#line 2347 "HolyC.tab.c"
    break;

  case 88: /* primtype0: U8  */
#line 351 "HolyC.y"
                   {CType *prim =CreatePrimType(TYPE_U8);yyval=SLE(CreateTypeNode(prim));ReleaseAST(yyvsp[0]);}
#line 2353 "HolyC.tab.c"
    break;

  case 89: /* primtype0: I8  */
#line 352 "HolyC.y"
                   {CType *prim =CreatePrimType(TYPE_I8);yyval=SLE(CreateTypeNode(prim));ReleaseAST(yyvsp[0]);}
#line 2359 "HolyC.tab.c"
    break;

  case 90: /* primtype0: U16  */
#line 353 "HolyC.y"
                    {CType *prim =CreatePrimType(TYPE_U16);yyval=SLE(CreateTypeNode(prim));ReleaseAST(yyvsp[0]);}
#line 2365 "HolyC.tab.c"
    break;

  case 91: /* primtype0: I16  */
#line 354 "HolyC.y"
                    {CType *prim =CreatePrimType(TYPE_I16);yyval=SLE(CreateTypeNode(prim));ReleaseAST(yyvsp[0]);}
#line 2371 "HolyC.tab.c"
    break;

  case 92: /* primtype0: U32  */
#line 355 "HolyC.y"
                    {CType *prim =CreatePrimType(TYPE_U32);yyval=SLE(CreateTypeNode(prim));ReleaseAST(yyvsp[0]);}
#line 2377 "HolyC.tab.c"
    break;

  case 93: /* primtype0: I32  */
#line 356 "HolyC.y"
                    {CType *prim =CreatePrimType(TYPE_I32);yyval=SLE(CreateTypeNode(prim));ReleaseAST(yyvsp[0]);}
#line 2383 "HolyC.tab.c"
    break;

  case 94: /* primtype0: U64  */
#line 357 "HolyC.y"
                    {CType *prim =CreatePrimType(TYPE_U64);yyval=SLE(CreateTypeNode(prim));ReleaseAST(yyvsp[0]);}
#line 2389 "HolyC.tab.c"
    break;

  case 95: /* primtype0: I64  */
#line 358 "HolyC.y"
                    {CType *prim =CreatePrimType(TYPE_I64);yyval=SLE(CreateTypeNode(prim));ReleaseAST(yyvsp[0]);}
#line 2395 "HolyC.tab.c"
    break;

  case 96: /* primtype0: F64  */
#line 359 "HolyC.y"
                    {CType *prim =CreatePrimType(TYPE_F64);yyval=SLE(CreateTypeNode(prim));ReleaseAST(yyvsp[0]);}
#line 2401 "HolyC.tab.c"
    break;

  case 97: /* primtype0: TYPENAME  */
#line 360 "HolyC.y"
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
#line 2416 "HolyC.tab.c"
    break;

  case 98: /* primtype0: _class  */
#line 370 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 2422 "HolyC.tab.c"
    break;

  case 99: /* primtype0: _union  */
#line 371 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 2428 "HolyC.tab.c"
    break;

  case 100: /* primtype0: primtype0 _union  */
#line 372 "HolyC.y"
                                      {
  yyval=yyvsp[0];
  AssignUnionBasetype(yyvsp[0],yyvsp[-1]);
}
#line 2437 "HolyC.tab.c"
    break;

  case 101: /* _arrlit: _arrlit COMMA expr  */
#line 379 "HolyC.y"
                                          {yyval=AppendToArrLiteral(yyvsp[-2],yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 2443 "HolyC.tab.c"
    break;

  case 102: /* _arrlit: _arrlit COMMA arrlit  */
#line 380 "HolyC.y"
                                            {yyval=AppendToArrLiteral(yyvsp[-2],yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 2449 "HolyC.tab.c"
    break;

  case 103: /* _arrlit: expr  */
#line 381 "HolyC.y"
              {yyval=AppendToArrLiteral(NULL,yyvsp[0]);}
#line 2455 "HolyC.tab.c"
    break;

  case 104: /* _arrlit: arrlit  */
#line 382 "HolyC.y"
                {yyval=AppendToArrLiteral(NULL,yyvsp[0]);}
#line 2461 "HolyC.tab.c"
    break;

  case 105: /* arrlit: LEFT_CURLY _arrlit RIGHT_CURLY  */
#line 383 "HolyC.y"
                                                       {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 2471 "HolyC.tab.c"
    break;

  case 106: /* arrlit: LEFT_CURLY _arrlit COMMA RIGHT_CURLY  */
#line 388 "HolyC.y"
                                                                 {
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 2482 "HolyC.tab.c"
    break;

  case 107: /* arrlit: LEFT_CURLY COMMA RIGHT_CURLY  */
#line 394 "HolyC.y"
                                                      {
  yyval=NULL;
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 2493 "HolyC.tab.c"
    break;

  case 108: /* metadata: NAME expr  */
#line 403 "HolyC.y"
                             {
  yyval=AppendToMetaData(NULL,yyvsp[-1],yyvsp[0]);
}
#line 2501 "HolyC.tab.c"
    break;

  case 109: /* cdecltail: cdecltail COMMA _cdecltail  */
#line 406 "HolyC.y"
                                                    {
  assert(yyvsp[0]->type==__AST_DECL_TAILS);
  CDeclTail b2=vec_last(&yyvsp[0]->declTail);
  AppendToDecls(yyvsp[-2],b2.basetype,b2.finalType,b2.name,b2.dft,b2.metaData);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 2513 "HolyC.tab.c"
    break;

  case 110: /* cdecltail: _cdecltail  */
#line 413 "HolyC.y"
                            {yyval=yyvsp[0];}
#line 2519 "HolyC.tab.c"
    break;

  case 111: /* _cdecltail: vardecltail  */
#line 414 "HolyC.y"
                              {
  AST *t=yyvsp[0];
  yyval=yyvsp[0];
}
#line 2528 "HolyC.tab.c"
    break;

  case 112: /* _cdecltail: vardecltail metadata  */
#line 418 "HolyC.y"
                                          {
  AST *meta=yyvsp[-1]->declTail.data[0].metaData;
  meta=AppendToMetaData(meta,yyvsp[0]->metaData.data[0].name,yyvsp[0]->metaData.data[0].value);
  yyvsp[-1]->declTail.data[0].metaData=meta;
  yyval=yyvsp[-1];
}
#line 2539 "HolyC.tab.c"
    break;

  case 113: /* cdecl: primtype0 cdecltail  */
#line 424 "HolyC.y"
                                    {
  DeclsFillInBasetype(yyvsp[-1]->type2,yyvsp[0]);
  yyval=yyvsp[0];
}
#line 2548 "HolyC.tab.c"
    break;

  case 114: /* cbody: cbody cdecl SEMI  */
#line 428 "HolyC.y"
                                      {
 yyval=AppendToStmts(yyvsp[-2],yyvsp[-1]);
 ReleaseAST(yyvsp[0]);
}
#line 2557 "HolyC.tab.c"
    break;

  case 115: /* cbody: cbody union  */
#line 432 "HolyC.y"
                            {
 yyval=AppendToStmts(yyvsp[-1],yyvsp[0]);
}
#line 2565 "HolyC.tab.c"
    break;

  case 116: /* cbody: cbody class  */
#line 435 "HolyC.y"
                            {
 yyval=AppendToStmts(yyvsp[-1],yyvsp[0]);
}
#line 2573 "HolyC.tab.c"
    break;

  case 117: /* cbody: %empty  */
#line 438 "HolyC.y"
       {yyval=NULL;}
#line 2579 "HolyC.tab.c"
    break;

  case 118: /* _cheader: CLASS  */
#line 439 "HolyC.y"
                        {
  CType *t=CreateClassForwardDecl(NULL,NULL);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[0]);
}
#line 2589 "HolyC.tab.c"
    break;

  case 119: /* _uheader: UNION  */
#line 444 "HolyC.y"
                        {
  CType *t=CreateUnionForwardDecl(NULL,NULL);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[0]);
}
#line 2599 "HolyC.tab.c"
    break;

  case 120: /* _cheader: CLASS NAME  */
#line 449 "HolyC.y"
                                {
  CType *t=CreateClassForwardDecl(NULL,yyvsp[0]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-1]);
}
#line 2609 "HolyC.tab.c"
    break;

  case 121: /* _uheader: UNION NAME  */
#line 454 "HolyC.y"
                                {
  CType *t=CreateUnionForwardDecl(NULL,yyvsp[0]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-1]);
}
#line 2619 "HolyC.tab.c"
    break;

  case 122: /* _cheader: CLASS TYPENAME  */
#line 459 "HolyC.y"
                                    {
  CType *t=CreateClassForwardDecl(NULL,yyvsp[0]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-1]);
}
#line 2629 "HolyC.tab.c"
    break;

  case 123: /* _uheader: UNION TYPENAME  */
#line 464 "HolyC.y"
                               {
  CType *t=CreateUnionForwardDecl(NULL,yyvsp[0]);
  yyval=CreateTypeNode(t);
}
#line 2638 "HolyC.tab.c"
    break;

  case 124: /* cheader: _cheader  */
#line 468 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 2644 "HolyC.tab.c"
    break;

  case 125: /* uheader: _uheader  */
#line 469 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 2650 "HolyC.tab.c"
    break;

  case 126: /* cheader: _cheader COLON TYPENAME  */
#line 470 "HolyC.y"
                                                 {
  CType *t=*map_get(&Compiler.types,yyvsp[0]->name);
  InheritFromType(yyvsp[-2]->type2,t);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 2661 "HolyC.tab.c"
    break;

  case 127: /* uheader: _uheader COLON TYPENAME  */
#line 476 "HolyC.y"
                                                 {
  CType *t=*map_get(&Compiler.types,yyvsp[0]->name);
  InheritFromType(yyvsp[-2]->type2,t);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 2672 "HolyC.tab.c"
    break;

  case 128: /* class: _class SEMI  */
#line 482 "HolyC.y"
                              {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[0]);
}
#line 2681 "HolyC.tab.c"
    break;

  case 129: /* union: primtype0 _union SEMI  */
#line 486 "HolyC.y"
                                            {
  yyval=yyvsp[-1];
  AssignUnionBasetype(yyvsp[-1],yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 2691 "HolyC.tab.c"
    break;

  case 130: /* union: _union SEMI  */
#line 491 "HolyC.y"
                              {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[0]);
}
#line 2700 "HolyC.tab.c"
    break;

  case 131: /* _class: cheader LEFT_CURLY cbody RIGHT_CURLY  */
#line 496 "HolyC.y"
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
#line 2720 "HolyC.tab.c"
    break;

  case 132: /* _union: uheader LEFT_CURLY cbody RIGHT_CURLY  */
#line 511 "HolyC.y"
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
#line 2737 "HolyC.tab.c"
    break;

  case 133: /* tryblock: TRY scope CATCH scope  */
#line 527 "HolyC.y"
                                                   {
  yyval=CreateTry(yyvsp[-2],yyvsp[0]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-1]);
}
#line 2747 "HolyC.tab.c"
    break;

  case 134: /* namewptrs: NAME  */
#line 535 "HolyC.y"
                   {
  CType *t =CreatePrimType(TYPE_I64);
  yyval=SOT(AppendToDecls(NULL,t,t,yyvsp[0],NULL,NULL),yyvsp[0]);
}
#line 2756 "HolyC.tab.c"
    break;

  case 135: /* namewptrs: MUL NAME  */
#line 539 "HolyC.y"
                            {
  CType *t =CreatePrimType(TYPE_I64);
  CType *ptr =CreateMultiLvlPtr(t,1);
  yyval=SOT(AppendToDecls(NULL,t,ptr,yyvsp[0],NULL,NULL),yyvsp[0]);
  ReleaseAST(yyvsp[-1]);
}
#line 2767 "HolyC.tab.c"
    break;

  case 136: /* namewptrs: MUL MUL NAME  */
#line 545 "HolyC.y"
                                     {
  CType *t =CreatePrimType(TYPE_I64);
  CType *ptr =CreateMultiLvlPtr(t,2);
  yyval=SOT(AppendToDecls(NULL,t,ptr,yyvsp[0],NULL,NULL),yyvsp[0]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
}
#line 2779 "HolyC.tab.c"
    break;

  case 137: /* namewptrs: MUL MUL MUL NAME  */
#line 552 "HolyC.y"
                                              {
  CType *t =CreatePrimType(TYPE_I64);
  CType *ptr =CreateMultiLvlPtr(t,3);
  yyval=SOT(AppendToDecls(NULL,t,ptr,yyvsp[0],NULL,NULL),yyvsp[0]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
}
#line 2792 "HolyC.tab.c"
    break;

  case 139: /* __ptrcount: %empty  */
#line 561 "HolyC.y"
            {
    yyval=CreateI64(0);
}
#line 2800 "HolyC.tab.c"
    break;

  case 140: /* __ptrcount: MUL  */
#line 564 "HolyC.y"
                     {
  yyval=CreateI64(1);
  ReleaseAST(yyvsp[0]);
}
#line 2809 "HolyC.tab.c"
    break;

  case 141: /* __ptrcount: MUL MUL  */
#line 568 "HolyC.y"
                              {
  yyval=CreateI64(2);
  ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 2818 "HolyC.tab.c"
    break;

  case 142: /* __ptrcount: MUL MUL MUL  */
#line 572 "HolyC.y"
                                       {
  yyval=CreateI64(3);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 2827 "HolyC.tab.c"
    break;

  case 143: /* vardecltail: __ptrcount LEFT_PAREN MUL NAME RIGHT_PAREN LEFT_PAREN funcargs RIGHT_PAREN  */
#line 576 "HolyC.y"
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
#line 2844 "HolyC.tab.c"
    break;

  case 144: /* vardecltail: __ptrcount LEFT_PAREN MUL MUL NAME RIGHT_PAREN LEFT_PAREN funcargs RIGHT_PAREN  */
#line 588 "HolyC.y"
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
#line 2862 "HolyC.tab.c"
    break;

  case 145: /* vardecltail: __ptrcount LEFT_PAREN MUL MUL MUL NAME RIGHT_PAREN LEFT_PAREN funcargs RIGHT_PAREN  */
#line 601 "HolyC.y"
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
#line 2881 "HolyC.tab.c"
    break;

  case 146: /* vardecltail: vardecltail LEFT_SQAURE expr_comma RIGHT_SQAURE  */
#line 615 "HolyC.y"
                                                                                     {
  assert(yyvsp[-3]->type==__AST_DECL_TAILS);
  CType *bt=vec_last(&yyvsp[-3]->declTail).finalType;
  vec_last(&yyvsp[-3]->declTail).finalType=CreateArrayType(bt,yyvsp[-1]);
  yyval=yyvsp[-3];
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 2894 "HolyC.tab.c"
    break;

  case 147: /* vardecltail: vardecltail LEFT_SQAURE RIGHT_SQAURE  */
#line 623 "HolyC.y"
                                                                     {
  assert(yyvsp[-2]->type==__AST_DECL_TAILS);
  CType *bt=vec_last(&yyvsp[-2]->declTail).finalType;
  vec_last(&yyvsp[-2]->declTail).finalType=CreateArrayType(bt,NULL);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 2907 "HolyC.tab.c"
    break;

  case 148: /* vardecltail_asn: vardecltail ASSIGN expr  */
#line 632 "HolyC.y"
                                                       {
  assert(yyvsp[-2]->type==__AST_DECL_TAILS);
  vec_last(&yyvsp[-2]->declTail).dft=yyvsp[0];
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 2918 "HolyC.tab.c"
    break;

  case 149: /* vardecltail_asn: vardecltail ASSIGN arrlit  */
#line 638 "HolyC.y"
                                                         {
  assert(yyvsp[-2]->type==__AST_DECL_TAILS);
  vec_last(&yyvsp[-2]->declTail).dft=yyvsp[0];
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 2929 "HolyC.tab.c"
    break;

  case 150: /* single_decl: primtype0 vardecltail  */
#line 645 "HolyC.y"
                                                {
  DeclsFillInBasetype(yyvsp[-1]->type2,yyvsp[0]);
  yyval=yyvsp[0];
}
#line 2938 "HolyC.tab.c"
    break;

  case 151: /* single_decl: primtype0 vardecltail_asn  */
#line 649 "HolyC.y"
                                                    {
  DeclsFillInBasetype(yyvsp[-1]->type2,yyvsp[0]);
  yyval=yyvsp[0];
}
#line 2947 "HolyC.tab.c"
    break;

  case 152: /* vardecltails: vardecltails COMMA vardecltails  */
#line 654 "HolyC.y"
                                                            {
  assert(yyvsp[0]->type==__AST_DECL_TAILS);
  CDeclTail b2=vec_last(&yyvsp[0]->declTail);
  AppendToDecls(yyvsp[-2],b2.basetype,b2.finalType,b2.name,b2.dft,NULL);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 2959 "HolyC.tab.c"
    break;

  case 155: /* multi_decl: primtype0 vardecltails  */
#line 664 "HolyC.y"
                                                 {
  DeclsFillInBasetype(yyvsp[-1]->type2,yyvsp[0]);
  yyval=yyvsp[0];
}
#line 2968 "HolyC.tab.c"
    break;

  case 157: /* funcargs: funcargs COMMA single_decl  */
#line 670 "HolyC.y"
                                                   {
  assert(yyvsp[0]->type==__AST_DECL_TAILS);
  CDeclTail b2=vec_last(&yyvsp[0]->declTail);
  yyval=AppendToDecls(yyvsp[-2],b2.basetype,b2.finalType,b2.name,b2.dft,NULL);
  ReleaseAST(yyvsp[-1]);
}
#line 2979 "HolyC.tab.c"
    break;

  case 158: /* funcargs: %empty  */
#line 676 "HolyC.y"
          {yyval=NULL;}
#line 2985 "HolyC.tab.c"
    break;

  case 159: /* linkage: EXTERN  */
#line 680 "HolyC.y"
                     {yyval=CreateExternLinkage(NULL);ReleaseAST(yyvsp[0]);}
#line 2991 "HolyC.tab.c"
    break;

  case 160: /* linkage: IMPORT  */
#line 681 "HolyC.y"
                     {yyval=CreateImportLinkage(NULL);ReleaseAST(yyvsp[0]);}
#line 2997 "HolyC.tab.c"
    break;

  case 161: /* linkage: PUBLIC  */
#line 682 "HolyC.y"
                     {yyval=NULL;}
#line 3003 "HolyC.tab.c"
    break;

  case 162: /* linkage: STATIC  */
#line 683 "HolyC.y"
                     {yyval=CreateStaticLinkage();ReleaseAST(yyvsp[0]);}
#line 3009 "HolyC.tab.c"
    break;

  case 163: /* linkage: EXTERN2 NAME  */
#line 684 "HolyC.y"
                              {yyval=CreateExternLinkage(yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3015 "HolyC.tab.c"
    break;

  case 164: /* linkage: IMPORT2 NAME  */
#line 685 "HolyC.y"
                              {yyval=CreateImportLinkage(yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3021 "HolyC.tab.c"
    break;

  case 165: /* linkage: EXTERN2 TYPENAME  */
#line 686 "HolyC.y"
                                  {yyval=CreateExternLinkage(yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3027 "HolyC.tab.c"
    break;

  case 166: /* linkage: IMPORT2 TYPENAME  */
#line 687 "HolyC.y"
                                  {yyval=CreateImportLinkage(yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3033 "HolyC.tab.c"
    break;

  case 167: /* func: primtype0 namewptrs LEFT_PAREN funcargs RIGHT_PAREN SEMI  */
#line 691 "HolyC.y"
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
#line 3051 "HolyC.tab.c"
    break;

  case 168: /* func: primtype0 namewptrs LEFT_PAREN funcargs COMMA DOT_DOT_DOT RIGHT_PAREN SEMI  */
#line 704 "HolyC.y"
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
#line 3070 "HolyC.tab.c"
    break;

  case 169: /* func: primtype0 namewptrs LEFT_PAREN DOT_DOT_DOT RIGHT_PAREN SEMI  */
#line 718 "HolyC.y"
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
#line 3088 "HolyC.tab.c"
    break;

  case 170: /* func: linkage primtype0 namewptrs LEFT_PAREN funcargs RIGHT_PAREN SEMI  */
#line 731 "HolyC.y"
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
#line 3107 "HolyC.tab.c"
    break;

  case 171: /* func: linkage primtype0 namewptrs LEFT_PAREN funcargs COMMA DOT_DOT_DOT RIGHT_PAREN SEMI  */
#line 745 "HolyC.y"
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
#line 3128 "HolyC.tab.c"
    break;

  case 172: /* func: linkage primtype0 namewptrs LEFT_PAREN DOT_DOT_DOT RIGHT_PAREN SEMI  */
#line 761 "HolyC.y"
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
#line 3147 "HolyC.tab.c"
    break;

  case 173: /* func: primtype0 namewptrs LEFT_PAREN DOT_DOT_DOT RIGHT_PAREN scope  */
#line 775 "HolyC.y"
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
#line 3166 "HolyC.tab.c"
    break;

  case 174: /* func: primtype0 namewptrs LEFT_PAREN funcargs COMMA DOT_DOT_DOT RIGHT_PAREN scope  */
#line 789 "HolyC.y"
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
#line 3186 "HolyC.tab.c"
    break;

  case 175: /* func: primtype0 namewptrs LEFT_PAREN funcargs RIGHT_PAREN scope  */
#line 804 "HolyC.y"
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
#line 3204 "HolyC.tab.c"
    break;

  case 176: /* func: linkage primtype0 namewptrs LEFT_PAREN DOT_DOT_DOT RIGHT_PAREN scope  */
#line 817 "HolyC.y"
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
#line 3223 "HolyC.tab.c"
    break;

  case 177: /* func: linkage primtype0 namewptrs LEFT_PAREN funcargs COMMA DOT_DOT_DOT RIGHT_PAREN scope  */
#line 831 "HolyC.y"
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
#line 3244 "HolyC.tab.c"
    break;

  case 178: /* func: linkage primtype0 namewptrs LEFT_PAREN funcargs RIGHT_PAREN scope  */
#line 847 "HolyC.y"
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
#line 3262 "HolyC.tab.c"
    break;

  case 179: /* ocstmt: ostmt  */
#line 865 "HolyC.y"
                    {yyval=yyvsp[0];}
#line 3268 "HolyC.tab.c"
    break;

  case 180: /* ocstmt: cstmt  */
#line 866 "HolyC.y"
                    {yyval=yyvsp[0];}
#line 3274 "HolyC.tab.c"
    break;

  case 182: /* expr_opt: %empty  */
#line 869 "HolyC.y"
          {yyval=NULL;}
#line 3280 "HolyC.tab.c"
    break;

  case 183: /* loop_header: WHILE LEFT_PAREN expr_comma RIGHT_PAREN  */
#line 872 "HolyC.y"
                                                                           {
  yyval=SOT(CreateWhile(yyvsp[-1],NULL),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3290 "HolyC.tab.c"
    break;

  case 184: /* loop_header: FOR LEFT_PAREN expr_opt SEMI expr_opt SEMI expr_opt RIGHT_PAREN  */
#line 878 "HolyC.y"
                                                                                                                         {
  yyval=SOT(CreateFor(yyvsp[-5],yyvsp[-3],yyvsp[-1],NULL),yyvsp[-7]);
  ReleaseAST(yyvsp[-6]);
  ReleaseAST(yyvsp[-4]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3302 "HolyC.tab.c"
    break;

  case 185: /* ifcl: IF LEFT_PAREN expr_comma RIGHT_PAREN  */
#line 886 "HolyC.y"
                                                              {
  yyval=SOT(CreateIf(yyvsp[-1],NULL,NULL),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3312 "HolyC.tab.c"
    break;

  case 186: /* ostmt: ifcl ocstmt  */
#line 892 "HolyC.y"
                            {
  assert(yyvsp[-1]->type==AST_IF);
  yyval=yyvsp[-1];
  yyvsp[-1]->ifStmt.body=yyvsp[0];
}
#line 3322 "HolyC.tab.c"
    break;

  case 187: /* ostmt: ifcl cstmt ELSE ostmt  */
#line 897 "HolyC.y"
                                              {
  assert(yyvsp[-3]->type==AST_IF);
  yyval=yyvsp[-3];
  yyvsp[-3]->ifStmt.body=yyvsp[-2];
  yyvsp[-3]->ifStmt.elseBody=yyvsp[0];
  ReleaseAST(yyvsp[-1]);
}
#line 3334 "HolyC.tab.c"
    break;

  case 188: /* ostmt: loop_header ostmt  */
#line 904 "HolyC.y"
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
#line 3349 "HolyC.tab.c"
    break;

  case 190: /* cstmt: ifcl cstmt ELSE cstmt  */
#line 916 "HolyC.y"
                                              {
  assert(yyvsp[-3]->type==AST_IF);
  yyval=yyvsp[-3];
  yyvsp[-3]->ifStmt.body=yyvsp[-2];
  yyvsp[-3]->ifStmt.elseBody=yyvsp[0];
  ReleaseAST(yyvsp[-1]);
}
#line 3361 "HolyC.tab.c"
    break;

  case 191: /* cstmt: loop_header cstmt  */
#line 923 "HolyC.y"
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
#line 3376 "HolyC.tab.c"
    break;

  case 192: /* dostmt: DO ocstmt WHILE LEFT_PAREN expr_comma RIGHT_PAREN  */
#line 934 "HolyC.y"
                                                                                     {
  yyval=SOT(CreateDo(yyvsp[-1],yyvsp[-4]),yyvsp[-5]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3387 "HolyC.tab.c"
    break;

  case 193: /* subswit: START COLON swit_body END COLON  */
#line 940 "HolyC.y"
                                                                 {
  yyval=SOT(CreateSubswitch(yyvsp[-2]),yyvsp[-4]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3398 "HolyC.tab.c"
    break;

  case 194: /* swit_body_stmt: ocstmt  */
#line 947 "HolyC.y"
                       {yyval=yyvsp[0];}
#line 3404 "HolyC.tab.c"
    break;

  case 195: /* swit_body_stmt: DEFAULT COLON  */
#line 948 "HolyC.y"
                                         {
  yyval=SOT(CreateDefault(),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3413 "HolyC.tab.c"
    break;

  case 196: /* swit_body_stmt: CASE expr COLON  */
#line 952 "HolyC.y"
                                              {
  yyval=SOT(CreateCase(yyvsp[-1],NULL),yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3422 "HolyC.tab.c"
    break;

  case 197: /* swit_body_stmt: CASE COLON  */
#line 956 "HolyC.y"
                                      {
  yyval=SOT(CreateCase(NULL,NULL),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3431 "HolyC.tab.c"
    break;

  case 198: /* swit_body_stmt: CASE expr DOT_DOT_DOT expr COLON  */
#line 960 "HolyC.y"
                                                                       {
  yyval=SOT(CreateCase(yyvsp[-3],yyvsp[-1]),yyvsp[-4]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);

}
#line 3442 "HolyC.tab.c"
    break;

  case 199: /* swit_body_stmt: subswit  */
#line 966 "HolyC.y"
                              {yyval=yyvsp[0];}
#line 3448 "HolyC.tab.c"
    break;

  case 200: /* swit_body: swit_body swit_body_stmt  */
#line 967 "HolyC.y"
                                             {yyval=AppendToStmts(yyvsp[-1],yyvsp[0]);}
#line 3454 "HolyC.tab.c"
    break;

  case 201: /* swit_body: swit_body_stmt  */
#line 968 "HolyC.y"
                                {yyval=yyvsp[0];}
#line 3460 "HolyC.tab.c"
    break;

  case 202: /* swit: SWITCH LEFT_PAREN expr_comma RIGHT_PAREN LEFT_CURLY swit_body RIGHT_CURLY  */
#line 969 "HolyC.y"
                                                                                                                      {
  yyval=SOT(CreateSwitch(yyvsp[-4],yyvsp[-1]),yyvsp[-6]);
  ReleaseAST(yyvsp[-5]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3472 "HolyC.tab.c"
    break;

  case 203: /* swit: SWITCH LEFT_PAREN expr_comma RIGHT_PAREN LEFT_CURLY RIGHT_CURLY  */
#line 976 "HolyC.y"
                                                                                                       {
  yyval=SOT(CreateSwitch(yyvsp[-3],NULL),yyvsp[-5]);
  ReleaseAST(yyvsp[-4]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3484 "HolyC.tab.c"
    break;

  case 205: /* simple_stmt: multi_decl SEMI  */
#line 984 "HolyC.y"
                                       {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[0]);
}
#line 3493 "HolyC.tab.c"
    break;

  case 206: /* simple_stmt: linkage multi_decl SEMI  */
#line 988 "HolyC.y"
                                                  {
  yyval=ApplyLinkageToDecls(yyvsp[-2],yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3502 "HolyC.tab.c"
    break;

  case 208: /* simple_stmt: GOTO NAME SEMI  */
#line 993 "HolyC.y"
                                         {
  yyval=SOT(CreateGoto(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3511 "HolyC.tab.c"
    break;

  case 209: /* simple_stmt: dostmt SEMI  */
#line 997 "HolyC.y"
                                   {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[0]);
}
#line 3520 "HolyC.tab.c"
    break;

  case 210: /* simple_stmt: expr_comma SEMI  */
#line 1001 "HolyC.y"
                                       {
  yyval=yyvsp[-1];
  if(yyvsp[-1]->type==AST_STRING) {
    yyval=SOT(CreatePrint(yyvsp[-1]->string),yyvsp[-1]);
  }
  ReleaseAST(yyvsp[0]);
}
#line 3532 "HolyC.tab.c"
    break;

  case 211: /* simple_stmt: swit  */
#line 1008 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 3538 "HolyC.tab.c"
    break;

  case 212: /* simple_stmt: scope  */
#line 1009 "HolyC.y"
                         {yyval=yyvsp[0];}
#line 3544 "HolyC.tab.c"
    break;

  case 213: /* simple_stmt: BREAK SEMI  */
#line 1010 "HolyC.y"
                                  {
  yyval=SOT(CreateBreak(),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3553 "HolyC.tab.c"
    break;

  case 214: /* simple_stmt: SEMI  */
#line 1014 "HolyC.y"
                         {
  yyval=CreateNop();
  ReleaseAST(yyvsp[0]);
}
#line 3562 "HolyC.tab.c"
    break;

  case 215: /* simple_stmt: RET expr_comma SEMI  */
#line 1018 "HolyC.y"
                                           {
  yyval=SOT(CreateReturn(yyvsp[-1]),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3571 "HolyC.tab.c"
    break;

  case 216: /* simple_stmt: RET SEMI  */
#line 1022 "HolyC.y"
                                 {
  yyval=SOT(CreateReturn(NULL),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3580 "HolyC.tab.c"
    break;

  case 217: /* simple_stmt: NAME COLON  */
#line 1026 "HolyC.y"
                                  {
  yyval=SOT(CreateLabel(yyvsp[-1]),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3589 "HolyC.tab.c"
    break;

  case 218: /* simple_stmt: class  */
#line 1030 "HolyC.y"
                         {yyval=yyvsp[0];}
#line 3595 "HolyC.tab.c"
    break;

  case 219: /* simple_stmt: union  */
#line 1031 "HolyC.y"
                         {yyval=yyvsp[0];}
#line 3601 "HolyC.tab.c"
    break;

  case 220: /* simple_stmt: linkage UNION NAME SEMI  */
#line 1032 "HolyC.y"
                                                        {
  CType *t=CreateUnionForwardDecl(yyvsp[-3],yyvsp[-1]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3612 "HolyC.tab.c"
    break;

  case 221: /* simple_stmt: linkage CLASS NAME SEMI  */
#line 1038 "HolyC.y"
                                                        {
  CType *t=CreateClassForwardDecl(yyvsp[-3],yyvsp[-1]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3623 "HolyC.tab.c"
    break;

  case 222: /* stmts: ocstmt  */
#line 1044 "HolyC.y"
                    {yyval=yyvsp[0];}
#line 3629 "HolyC.tab.c"
    break;

  case 223: /* stmts: stmts ocstmt  */
#line 1045 "HolyC.y"
                             {yyval=AppendToStmts(yyvsp[-1],yyvsp[0]);}
#line 3635 "HolyC.tab.c"
    break;

  case 224: /* stmts: stmts error  */
#line 1046 "HolyC.y"
                   {YYABORT;}
#line 3641 "HolyC.tab.c"
    break;

  case 225: /* scope: LEFT_CURLY stmts RIGHT_CURLY  */
#line 1047 "HolyC.y"
                                                    {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3651 "HolyC.tab.c"
    break;

  case 226: /* scope: LEFT_CURLY RIGHT_CURLY  */
#line 1052 "HolyC.y"
                                           {
  yyval=CreateNop();
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3661 "HolyC.tab.c"
    break;

  case 227: /* global_stmts: %empty  */
#line 1057 "HolyC.y"
                {yyval=NULL;}
#line 3667 "HolyC.tab.c"
    break;

  case 228: /* global_stmts: global_stmts ocstmt  */
#line 1058 "HolyC.y"
                                        {
  RunStatement(yyvsp[0]);
  ReleaseAST(yyvsp[0]);
  yyval=NULL;
}
#line 3677 "HolyC.tab.c"
    break;

  case 229: /* global_stmt: global_stmts  */
#line 1063 "HolyC.y"
                                {
  yyval=NULL;
}
#line 3685 "HolyC.tab.c"
    break;

  case 230: /* global_stmt: EXE scope  */
#line 1067 "HolyC.y"
                             {
  RunStatement(yyvsp[0]);
  YYACCEPT;
  yyval=NULL;
}
#line 3695 "HolyC.tab.c"
    break;

  case 231: /* global_stmt: EVAL expr_comma NL  */
#line 1072 "HolyC.y"
                                      {
  RunPtr(CurFuncInfo,yyvsp[-1],CurFramePtr);
  YYACCEPT;
  yyval=NULL;
}
#line 3705 "HolyC.tab.c"
    break;

  case 232: /* global_stmt: DBG expr_comma  */
#line 1078 "HolyC.y"
                                  {
  RunPtr(CurFuncInfo,yyvsp[0],CurFramePtr);
  YYACCEPT;
  yyval=NULL;
}
#line 3715 "HolyC.tab.c"
    break;


#line 3719 "HolyC.tab.c"

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

#line 1083 "HolyC.y"

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
  CType *rtype =AssignTypeToNode(node);
  COldFuncState old=CreateCompilerState();
  vec_CVariable_t args;
  vec_init(&args);
  Compiler.returnType=rtype;
  AST *retn =CreateReturn(node);
  CFunction *f=CompileAST(NULL,retn,args);
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
