/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

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

#line 90 "HolyC.tab.c"

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

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_HC_HOLYC_TAB_H_INCLUDED
# define YY_HC_HOLYC_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef HC_DEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define HC_DEBUG 1
#  else
#   define HC_DEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define HC_DEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined HC_DEBUG */
#if HC_DEBUG
extern int HC_debug;
#endif

/* Token type.  */
#ifndef HC_TOKENTYPE
# define HC_TOKENTYPE
  enum HC_tokentype
  {
    HC_CHAR = 258,
    HC_INT = 259,
    HC_FLOAT = 260,
    HC_DOT = 261,
    HC_ARROW = 262,
    HC_SIZEOF = 263,
    HC_ADDR_OF = 264,
    HC_LNOT = 265,
    HC_BNOT = 266,
    HC_POW = 267,
    HC_SHL = 268,
    HC_SHR = 269,
    HC_MUL = 270,
    HC_DIV = 271,
    HC_MOD = 272,
    HC_BAND = 273,
    HC_BOR = 274,
    HC_BXOR = 275,
    HC_ADD = 276,
    HC_SUB = 277,
    HC_LT = 278,
    HC_GT = 279,
    HC_LE = 280,
    HC_GE = 281,
    HC_NE = 282,
    HC_EQ = 283,
    HC_LAND = 284,
    HC_LXOR = 285,
    HC_LOR = 286,
    HC_ASSIGN = 287,
    HC_EQ_SHL = 288,
    HC_EQ_SHR = 289,
    HC_EQ_MUL = 290,
    HC_EQ_DIV = 291,
    HC_EQ_MOD = 292,
    HC_EQ_BAND = 293,
    HC_EQ_BXOR = 294,
    HC_EQ_BOR = 295,
    HC_EQ_ADD = 296,
    HC_EQ_SUB = 297,
    HC_COMMA = 298,
    HC_TRY = 299,
    HC_CATCH = 300,
    HC_LASTCLASS = 301,
    HC_U0 = 302,
    HC_LEFT_PAREN = 303,
    HC_RIGHT_PAREN = 304,
    HC_INC = 305,
    HC_DEC = 306,
    HC_NAME = 307,
    HC_LEFT_SQAURE = 308,
    HC_RIGHT_SQAURE = 309,
    HC_SEMI = 310,
    HC_IF = 311,
    HC_ELSE = 312,
    HC_DO = 313,
    HC_WHILE = 314,
    HC_FOR = 315,
    HC_LEFT_CURLY = 316,
    HC_RIGHT_CURLY = 317,
    HC_CASE = 318,
    HC_COLON = 319,
    HC_DOT_DOT_DOT = 320,
    HC_EXTERN2 = 321,
    HC_LOCK = 322,
    HC_EXTERN = 323,
    HC_IMPORT = 324,
    HC_IMPORT2 = 325,
    HC_ASM_IMPORT = 326,
    HC_STATIC = 327,
    HC_PUBLIC = 328,
    HC_CLASS = 329,
    HC_UNION = 330,
    HC_INTERN = 331,
    HC_START = 332,
    HC_END = 333,
    HC_DEFAULT = 334,
    HC_BREAK = 335,
    HC_RET = 336,
    HC_GOTO = 337,
    HC_SWITCH = 338,
    HC_EVAL_NOCOMMA = 339,
    HC_EXE = 340,
    HC_EVAL = 341,
    HC_NL = 342,
    HC_DBG = 343,
    HC_STRING = 344,
    HC_TYPENAME = 345,
    HC_OPCODE = 346,
    HC_REGISTER = 347,
    HC_DOUBLE_COLON = 348,
    HC_DOUBLE_AT = 349,
    HC_DU8 = 350,
    HC_DU16 = 351,
    HC_DU32 = 352,
    HC_DU64 = 353,
    HC_ALIGN = 354,
    HC_BINFILE = 355,
    HC_ASM = 356
  };
#endif

/* Value type.  */
#if ! defined HC_STYPE && ! defined HC_STYPE_IS_DECLARED
typedef int HC_STYPE;
# define HC_STYPE_IS_TRIVIAL 1
# define HC_STYPE_IS_DECLARED 1
#endif


extern HC_STYPE HC_lval;

int HC_parse (void);

#endif /* !YY_HC_HOLYC_TAB_H_INCLUDED  */



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
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
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

#if ! defined yyoverflow || YYERROR_VERBOSE

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
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


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
#define YYFINAL  94
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1676

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  102
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  94
/* YYNRULES -- Number of rules.  */
#define YYNRULES  347
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  620

#define YYUNDEFTOK  2
#define YYMAXUTOK   356


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

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
      95,    96,    97,    98,    99,   100,   101
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
     524,   529,   539,   540,   541,   545,   552,   553,   554,   555,
     556,   561,   567,   576,   579,   586,   587,   591,   597,   601,
     605,   609,   610,   615,   620,   625,   630,   635,   639,   640,
     641,   647,   653,   668,   684,   692,   696,   702,   709,   717,
     718,   721,   725,   729,   733,   745,   758,   772,   780,   789,
     795,   802,   806,   811,   818,   819,   821,   826,   827,   833,
     837,   838,   839,   840,   841,   842,   843,   844,   848,   861,
     875,   888,   902,   918,   932,   946,   961,   974,   988,  1004,
    1022,  1023,  1025,  1026,  1029,  1035,  1043,  1049,  1054,  1061,
    1072,  1073,  1080,  1091,  1097,  1104,  1105,  1109,  1113,  1117,
    1123,  1124,  1125,  1126,  1133,  1140,  1141,  1149,  1154,  1158,
    1162,  1163,  1164,  1168,  1172,  1173,  1177,  1181,  1188,  1189,
    1190,  1194,  1198,  1202,  1206,  1210,  1211,  1217,  1223,  1224,
    1225,  1226,  1231,  1236,  1237,  1251,  1255,  1260,  1265,  1271,
    1280,  1288,  1297,  1298,  1299,  1300,  1301,  1302,  1303,  1306,
    1309,  1310,  1314,  1315,  1319,  1323,  1327,  1331,  1335,  1339,
    1343,  1347,  1348,  1352,  1356,  1362,  1368,  1372,  1377,  1381,
    1385,  1389,  1393,  1397,  1403,  1404,  1408,  1412,  1417,  1418,
    1422,  1426,  1431,  1432,  1437,  1438,  1443,  1444,  1449,  1450,
    1454,  1459,  1460,  1464,  1468,  1472,  1477,  1478,  1482,  1487,
    1488,  1493,  1494,  1499,  1500,  1505,  1506,  1510,  1514,  1518,
    1522,  1526,  1530,  1534,  1538,  1542,  1546,  1550
};
#endif

#if HC_DEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "CHAR", "INT", "FLOAT", "DOT", "ARROW",
  "SIZEOF", "ADDR_OF", "LNOT", "BNOT", "POW", "SHL", "SHR", "MUL", "DIV",
  "MOD", "BAND", "BOR", "BXOR", "ADD", "SUB", "LT", "GT", "LE", "GE", "NE",
  "EQ", "LAND", "LXOR", "LOR", "ASSIGN", "EQ_SHL", "EQ_SHR", "EQ_MUL",
  "EQ_DIV", "EQ_MOD", "EQ_BAND", "EQ_BXOR", "EQ_BOR", "EQ_ADD", "EQ_SUB",
  "COMMA", "TRY", "CATCH", "LASTCLASS", "U0", "LEFT_PAREN", "RIGHT_PAREN",
  "INC", "DEC", "NAME", "LEFT_SQAURE", "RIGHT_SQAURE", "SEMI", "IF",
  "ELSE", "DO", "WHILE", "FOR", "LEFT_CURLY", "RIGHT_CURLY", "CASE",
  "COLON", "DOT_DOT_DOT", "EXTERN2", "LOCK", "EXTERN", "IMPORT", "IMPORT2",
  "ASM_IMPORT", "STATIC", "PUBLIC", "CLASS", "UNION", "INTERN", "START",
  "END", "DEFAULT", "BREAK", "RET", "GOTO", "SWITCH", "EVAL_NOCOMMA",
  "EXE", "EVAL", "NL", "DBG", "STRING", "TYPENAME", "OPCODE", "REGISTER",
  "DOUBLE_COLON", "DOUBLE_AT", "DU8", "DU16", "DU32", "DU64", "ALIGN",
  "BINFILE", "ASM", "$accept", "sib_ib", "sib_d", "_sib", "sib",
  "opc_operand", "opc_operands", "opcode", "asm_blk_stmt", "data_exprs",
  "name_list", "asm_blk", "asm_blk_stmts", "expr0", "_callargs",
  "callargs", "expr1", "expr2", "sizeof_type", "expr3", "expr4", "expr4_5",
  "expr5", "expr6", "expr7", "expr8", "expr9", "expr10", "expr11",
  "expr12", "expr13", "expr14", "expr", "expr_comma", "primtype0",
  "_arrlit", "arrlit", "metadata", "cdecltail", "_cdecltail", "cdecl",
  "cbody", "_cheader", "_uheader", "cheader", "uheader", "_class",
  "_union", "tryblock", "namewptrs", "vardecltail", "__ptrcount",
  "vardecltail_asn", "single_decl", "vardecltails", "multi_decl",
  "funcargs", "linkage", "func", "ocstmt", "expr_opt", "loop_header",
  "ifcl", "ostmt", "cstmt", "dostmt", "subswit", "swit_body_stmt",
  "swit_body", "swit", "simple_stmt", "opcodes", "stmts", "scope",
  "global_stmts", "global_stmt", "_expr0", "str", "_expr1", "_expr2",
  "_expr3", "_expr4", "_expr4_5", "_expr5", "_expr6", "_expr7", "_expr8",
  "_expr9", "_expr10", "_expr11", "_expr12", "_expr13", "_expr14", "_expr", YY_NULLPTR
};
#endif

# ifdef YYPRINT
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
     355,   356
};
# endif

#define YYPACT_NINF (-527)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     258,  1562,   -33,  1562,  1562,  1156,    50,  -527,  -527,  -527,
     105,  1562,  1562,  1562,  1562,  1562,  1562,  -527,  1562,  1562,
    1562,  -527,  -527,    46,    27,  -527,  -527,   121,  -527,   538,
     541,    70,   120,   130,   102,   404,   187,   122,   137,   159,
     431,  -527,   103,  1064,  -527,  -527,   175,   175,   -33,   178,
    -527,   199,  1156,   219,   257,     3,  -527,  -527,     8,  -527,
    -527,     9,    98,   253,  1345,   246,   271,    46,   299,   261,
     284,  -527,    37,    15,   291,   303,   -20,   207,  -527,  -527,
    -527,   342,   166,  -527,  -527,  1156,  1156,  -527,  -527,   361,
    -527,  -527,   313,  -527,  -527,  1248,  -527,  -527,   416,   164,
     358,   439,  -527,  -527,  -527,  -527,  -527,  -527,   221,  -527,
    -527,   452,   478,  -527,  -527,   483,   485,  1248,  -527,  -527,
    1562,  1562,  1562,  1562,  1562,  1562,  1562,  1562,  1562,  1562,
    1562,  1562,  1562,  1562,  1562,  1562,  1562,  1562,  1562,  1562,
    1562,  1562,  1562,  1562,  1562,  1562,  1562,  1562,  1562,  1562,
    1562,  1562,  -527,  -527,  -527,   604,  1562,   500,  -527,  1562,
     455,  1562,  1562,  -527,  -527,  -527,  -527,  -527,  -527,  -527,
    -527,  -527,  -527,   148,   512,  1562,  -527,  -527,  -527,   110,
    1582,  1582,  1582,  1582,  1582,  1582,  -527,  1562,  1582,  1582,
    -527,    20,   202,  -527,    35,  -527,  -527,   505,   528,  -527,
     103,  -527,   113,  -527,   547,   548,   555,   559,   556,   349,
     499,   355,   565,   566,   567,   551,   553,   131,  -527,  -527,
      29,  -527,  -527,  -527,  -527,   562,    -6,   563,  -527,   558,
     523,   530,  -527,  -527,  -527,  -527,  -527,   142,   146,    51,
     568,  -527,  -527,  -527,   564,  -527,   569,    19,   222,  -527,
    -527,  -527,  -527,  -527,  -527,  -527,   223,  -527,    81,   -18,
    -527,  -527,  -527,  -527,  -527,  -527,  -527,  -527,  -527,  -527,
    -527,  -527,  -527,  -527,  -527,  -527,  -527,  -527,  -527,  -527,
     431,   431,   431,   431,   431,   431,   431,   431,   431,   431,
     431,  -527,  -527,  -527,  -527,   -33,   292,   593,   305,   175,
     594,  -527,  -527,   323,  1248,  -527,   416,   379,  -527,  -527,
    -527,  -527,  -527,  -527,   325,  -527,  -527,  -527,  -527,   378,
     597,   601,   605,   607,  -527,   553,  -527,  -527,    54,   299,
     606,   609,  1248,  -527,  -527,  1582,  1582,  1582,  1582,  1582,
    1582,  1582,  1582,  1582,  1582,  1582,  1582,  1582,  1582,  1582,
    1582,  1582,  1582,  1582,  1582,  1582,  1582,  1582,  1582,  1582,
    1582,  1582,  1582,  1582,  1582,  1582,  -527,   140,  -527,  -527,
     623,    39,  1562,  1562,  1562,  1562,   676,  -527,  -527,   235,
      80,  -527,   186,  1365,  1442,   666,    95,  -527,  -527,   169,
     183,   627,   628,   640,  -527,  1156,  -527,  -527,  -527,  1562,
    -527,    86,  -527,  -527,  -527,  -527,  1562,  -527,  1562,   629,
     167,   326,  -527,  1462,  1562,  -527,  -527,  -527,    54,  -527,
     505,  -527,  -527,   341,    67,  -527,  -527,  -527,  -527,  -527,
    -527,  -527,  -527,  -527,  -527,  -527,  -527,  -527,  -527,  -527,
    -527,  -527,  -527,  -527,  -527,   551,   551,   551,   551,   551,
     551,   551,   551,   551,   551,   551,  -527,  -527,  -527,   156,
     625,   632,   191,  -527,   194,   220,   245,   648,  -527,  -527,
     645,  -527,   643,    51,  -527,   352,   488,  -527,  -527,  -527,
       2,   107,  -527,  -527,  -527,    62,   647,  -527,  -527,  -527,
     209,  -527,  -527,  -527,   172,  -527,   353,   653,   696,  -527,
    -527,   688,   694,  -527,  -527,  -527,  -527,  -527,   658,  -527,
    -527,  -527,  1562,  -527,  -527,  -527,  -527,   708,  -527,   360,
      -6,  -527,   250,   421,   651,  -527,   -14,  -527,  -527,   116,
     667,  -527,   672,  -527,   422,  -527,   681,   434,   682,  -527,
    -527,  1562,  -527,  1512,   668,   669,  -527,  -527,  -527,   788,
     642,   731,  -527,  -527,  -527,  -527,  -527,   687,  -527,  -527,
    -527,  -527,  1257,  -527,   685,   689,   691,    95,  1562,  -527,
     423,   263,   424,  -527,   692,  -527,   425,   972,  -527,  -527,
    -527,  -527,  -527,   433,  -527,  -527,  -527,   700,   695,   237,
    -527,  -527,  -527,  -527,   701,  -527,  -527,  -527,  -527,  1562,
     880,  -527,  -527,   697,   237,   458,   447,   699,   703,   237,
     462,   237,  -527,  -527,  -527,  -527,  -527,   469,  -527,  -527
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     263,     0,     0,     0,     0,   265,     0,    47,    46,    45,
       0,     0,     0,     0,     0,     0,     0,    49,     0,     0,
       0,    50,   272,     0,     0,    55,    57,    72,    85,    89,
      93,    95,    97,    99,   102,   107,   110,   112,   114,   116,
     128,   268,    48,     0,   266,   130,   267,   269,     0,    50,
     251,     0,     0,     0,     0,     0,   190,   191,     0,   193,
     192,   152,   153,     0,     0,     0,     0,   131,    21,     0,
       0,   241,     0,   170,   158,   159,     0,     0,   132,   133,
     235,     0,     0,   244,   264,     0,     0,   210,   211,     0,
     248,   220,   240,   249,     1,     0,    73,   131,    74,    70,
       0,     0,    83,    84,    78,    77,    81,    82,     0,    80,
      79,     0,     0,    44,    43,     0,     0,    51,    64,    63,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   273,   262,   258,     0,     0,     0,   254,     0,
       0,     0,   213,   194,   196,   195,   197,   154,   156,   155,
     157,   250,   253,     0,     0,     0,   276,   275,   274,     0,
       0,     0,     0,     0,     0,     0,   278,     0,     0,     0,
     279,     0,     0,    14,     0,    11,    13,    19,    22,   280,
     277,   282,   291,   304,   308,   312,   314,   316,   318,   321,
     326,   329,   331,   333,   335,   347,    15,     0,   236,   247,
     171,   165,   255,   135,   134,   169,   185,     0,   184,   186,
       0,     0,   238,   151,   239,   151,   242,   152,   153,   170,
       0,   219,   222,   217,   211,   246,     0,     0,     0,    71,
      56,    58,    59,    60,    61,    54,     0,    52,     0,     0,
      86,    87,    88,    90,    91,    92,    94,    96,    98,   100,
     101,   103,   104,   105,   106,   109,   108,   111,   113,   115,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   260,   261,   259,   129,     0,     0,     0,     0,   212,
       0,   252,   245,     0,     0,   292,   293,     0,   302,   303,
     297,   296,   300,   301,     0,   299,   298,     6,     2,     7,
       0,     0,     0,     0,    16,     0,   271,   270,     0,     0,
       0,     0,    51,   288,   287,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    12,     0,    37,    39,
       0,     0,     0,     0,     0,     0,     0,    33,    40,     0,
     172,   166,   189,     0,     0,     0,   170,   160,   161,     0,
       0,   154,   155,   169,   243,     0,   237,    76,    75,    51,
      65,     0,    66,    62,   164,   216,     0,   214,   213,     0,
       0,     0,   281,     0,     0,    10,   283,   284,     0,    18,
      20,   285,   286,     0,     0,   305,   306,   307,   309,   310,
     311,   313,   315,   317,   319,   320,   322,   323,   324,   325,
     328,   327,   330,   332,   334,   336,   337,   338,   339,   340,
     341,   342,   343,   344,   345,   346,    24,    23,    34,     0,
       0,     0,     0,    27,     0,     0,     0,     0,    42,    41,
     173,   167,     0,   170,   187,     0,     0,   179,   180,   178,
       0,     0,   169,   183,   162,   170,     0,   163,   257,   256,
     189,   218,   221,    53,     0,    67,     0,     0,     0,   295,
     294,    46,     3,     8,     9,    17,   289,   290,     0,    36,
      26,    25,     0,    29,    30,    31,    32,     0,   168,     0,
     181,   182,     0,     0,     0,   138,     0,   139,   177,     0,
       0,   150,   148,   145,   146,   149,     0,     0,     0,    68,
     223,   213,   234,     0,     0,     0,   225,   230,   232,     0,
       0,     0,    35,    28,    38,   200,   204,     0,   188,   198,
     206,   142,     0,   140,     0,     0,     0,   170,     0,   147,
       0,     0,     0,    69,     0,   228,     0,     0,   226,   233,
     231,     4,     5,     0,   141,   136,   137,     0,     0,   189,
     144,   143,   203,   207,     0,   201,   209,   215,   227,     0,
       0,   199,   205,     0,   189,     0,     0,     0,     0,   189,
       0,     0,   174,   202,   208,   229,   224,     0,   175,   176
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -527,  -527,  -527,  -196,  -183,   432,  -527,   -78,   374,   243,
    -527,  -527,  -527,  -527,   373,   428,  -527,  -527,   -85,   345,
     517,   518,   654,   646,   655,   356,   394,   383,   644,   641,
     649,   486,     4,    14,    -9,  -527,  -449,  -527,  -527,   216,
    -527,   560,  -527,  -527,    -3,    -1,   -62,   -61,  -527,   -58,
    -452,  -527,   315,  -500,   408,   718,  -471,  -527,  -527,    11,
    -395,  -527,  -527,   -82,   -79,  -527,  -527,  -526,   224,  -527,
    -527,  -527,  -527,    -2,  -527,  -527,  -527,    97,  -527,  -527,
     225,   310,   329,   461,   463,   464,   188,   195,   203,   456,
     451,   457,   364,   -60
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,   319,   320,   195,   196,   197,   198,    70,   378,   462,
     459,    71,   379,    25,   255,   256,    26,    27,    98,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    45,    72,    73,   526,   478,   569,   532,   533,
     486,   389,    74,    75,   100,   101,    78,    79,    80,   482,
     226,   227,   228,   474,   229,    81,   475,    82,    83,   546,
     300,    85,    86,    87,    88,    89,   547,   548,   549,    90,
      91,    92,   155,    93,     5,     6,   199,    42,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   325
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      44,    99,    76,   241,    77,    41,   242,   244,   216,   324,
     247,   223,   224,   497,   246,   225,    84,    46,    47,   537,
     366,   520,   558,   580,   317,   156,   383,   527,    43,   562,
     220,   113,   108,   534,   249,   232,   403,   223,   224,   326,
      76,   233,    77,   460,   380,   156,   157,   384,   563,    76,
      94,    77,   111,   112,   154,   163,   528,   176,   177,   178,
     165,   167,   179,   160,   180,   181,   220,   221,   397,   182,
     222,   558,   183,   239,   580,   184,   185,   220,   173,   114,
     156,   381,    76,    76,    77,    77,    99,   327,   127,    61,
      62,   461,   219,   164,   306,   470,   401,   243,   166,   168,
     186,   494,   187,   221,   188,   189,   190,   191,   258,   248,
     220,   558,   318,   586,   221,   534,   507,   531,   605,   330,
     331,   257,   529,   130,   131,    61,    62,   115,   116,   366,
     402,   564,   471,   610,   259,   495,    61,    62,   617,   377,
     128,    61,    62,    22,   307,   419,   574,   221,   194,   129,
     169,   138,    76,    95,    77,    61,    62,    96,   304,   530,
     294,   332,   305,   333,   334,   200,   293,   139,   565,   117,
      99,   118,   119,   296,   120,   298,   299,   223,   224,    61,
      62,   393,   249,   367,    61,    62,   368,   538,   170,   303,
     140,   156,   152,   369,   391,    97,   223,   224,   392,   508,
      97,   314,   370,   301,   456,   176,   177,   178,   321,   322,
     179,   509,   180,   181,   136,   137,   499,   182,   156,   410,
     183,   539,    68,   184,   185,   371,   372,   373,   374,   375,
     376,   484,   168,   457,   512,   505,   170,   512,    61,    62,
     237,   238,   158,    61,    62,   487,   513,   159,   186,   514,
     187,   472,   188,   189,   190,   191,    97,    61,    62,    97,
      61,    62,   234,   512,   156,   156,   399,   161,   235,   216,
     250,   398,   400,    97,   536,   515,    97,   200,   200,   200,
     200,   200,   200,    61,    62,   200,   200,   367,   512,   200,
     368,    22,   307,   404,   323,    99,   194,   468,   174,    97,
     516,   377,   176,   177,   178,   162,   370,   179,   171,   180,
     181,    61,    62,   491,   182,   557,   492,   183,   411,   175,
     184,   185,   217,   424,    61,    62,    68,    97,   594,   371,
     372,   373,   374,   375,   376,   156,   257,    61,    62,   218,
      97,   405,     1,     2,     3,   186,     4,   187,   156,   188,
     189,   190,   191,    97,   407,   230,   102,   103,   104,   105,
     106,   107,   223,   224,   109,   110,   156,   231,   156,   156,
     344,   345,   409,   473,   412,   500,   463,   463,   463,   463,
     485,   485,   350,   351,   399,   321,   322,   477,    22,   192,
     506,   193,    76,   194,    77,   522,   156,   236,   480,   413,
     414,   523,   540,   257,    68,   308,   309,   310,   311,   312,
     313,   223,   224,   315,   316,   555,   245,   503,   504,   233,
     496,    43,   299,   223,   224,   200,   200,   132,   133,   134,
     135,   249,   200,   200,   200,   200,   200,   200,   200,   200,
     200,   200,   200,   200,   200,   200,   200,   200,   200,   200,
     200,   200,   200,   200,   200,   200,   200,   200,   200,   200,
     200,   200,   200,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   568,   384,   559,   571,   592,   595,
     525,   473,    43,   572,    43,    43,   269,   270,   601,   598,
     599,     7,     8,     9,    43,    76,    10,    77,    11,    12,
     235,   611,   613,    13,   251,   611,    14,   612,    43,    15,
      16,   618,   611,   473,   297,   200,   553,   556,   619,   275,
     276,   560,   346,   347,   348,   349,   271,   272,   273,   274,
     252,   524,   434,   435,    17,   253,    18,   254,    19,    20,
      21,   436,   437,   438,   439,   295,    76,   576,    77,   476,
     121,   122,   123,   440,   441,   299,   124,   125,   126,   335,
     336,   337,   473,   338,   339,   340,   585,   302,   593,   328,
     596,   329,   591,   341,    76,   343,    77,    22,    23,   342,
     473,   602,    24,   355,   356,   357,   358,   359,   360,   361,
     362,   363,   364,   365,   352,   473,   353,    76,   354,    77,
     473,   386,   473,   607,   614,   291,   191,     7,     8,     9,
     382,   385,    10,   387,    11,    12,   464,   465,   466,    13,
     388,   395,    14,   394,   396,    15,    16,   280,   281,   282,
     283,   284,   285,   286,   287,   288,   289,   290,   260,   261,
     262,   406,   263,   264,   265,   425,   426,   427,    48,   408,
      17,   415,    18,   416,    19,    20,    49,   417,   421,    50,
      51,   422,    52,    53,    54,    43,   292,   428,   429,   430,
      55,   418,    56,    57,    58,   458,    59,    60,    61,    62,
     467,   481,   488,   489,    63,    64,    65,    66,   490,   510,
     498,   517,   519,    22,    67,    68,   511,   518,    24,     7,
       8,     9,   535,   550,    10,    69,    11,    12,   541,   551,
     552,    13,   554,   561,    14,   567,   566,    15,    16,   445,
     446,   447,   448,   449,   450,   451,   452,   453,   454,   455,
     570,   573,   577,   578,   581,   582,   583,   587,   588,   589,
      48,   597,    17,   604,    18,   609,    19,    20,    49,   603,
     606,    50,    51,   469,    52,    53,    54,    43,   542,   543,
     423,   420,    55,   615,    56,    57,    58,   616,    59,    60,
      61,    62,   493,   544,   267,   545,    63,    64,    65,    66,
     278,   266,   277,   590,   268,    22,    67,    68,   521,   279,
      24,     7,     8,     9,   483,   390,    10,    69,    11,    12,
     240,   600,   431,    13,   443,   432,    14,   433,   442,    15,
      16,   444,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    48,     0,    17,     0,    18,     0,    19,    20,
      49,     0,     0,    50,    51,     0,    52,    53,    54,    43,
     579,   543,     0,     0,    55,     0,    56,    57,    58,     0,
      59,    60,    61,    62,     0,   544,     0,   545,    63,    64,
      65,    66,     0,     0,     0,     0,     0,    22,    67,    68,
       0,     0,    24,     7,     8,     9,     0,     0,    10,    69,
      11,    12,     0,     0,     0,    13,     0,     0,    14,     0,
       0,    15,    16,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    48,     0,    17,     0,    18,     0,
      19,    20,    49,     0,     0,    50,    51,     0,    52,    53,
      54,    43,     0,   543,     0,     0,    55,     0,    56,    57,
      58,     0,    59,    60,    61,    62,     0,   544,   608,   545,
      63,    64,    65,    66,     0,     0,     0,     0,     0,    22,
      67,    68,     0,     0,    24,     7,     8,     9,     0,     0,
      10,    69,    11,    12,     0,     0,     0,    13,     0,     0,
      14,     0,     0,    15,    16,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    48,     0,    17,     0,
      18,     0,    19,    20,    49,     0,     0,    50,    51,     0,
      52,    53,    54,    43,     0,   543,     0,     0,    55,     0,
      56,    57,    58,     0,    59,    60,    61,    62,     0,   544,
       0,   545,    63,    64,    65,    66,     0,     0,     0,     0,
       0,    22,    67,    68,     0,     0,    24,     7,     8,     9,
       0,     0,    10,    69,    11,    12,     0,     0,     0,    13,
       0,     0,    14,     0,     0,    15,    16,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    48,     0,
      17,     0,    18,     0,    19,    20,    49,     0,     0,    50,
      51,     0,    52,    53,    54,    43,   153,     0,     0,     0,
      55,     0,    56,    57,    58,     0,    59,    60,    61,    62,
       0,     0,     0,     0,    63,    64,    65,    66,     0,     0,
       0,     0,     0,    22,    67,    68,     0,     0,    24,     7,
       8,     9,     0,     0,    10,    69,    11,    12,     0,     0,
       0,    13,     0,     0,    14,     0,     0,    15,    16,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      48,     0,    17,     0,    18,     0,    19,    20,    49,     0,
       0,    50,    51,     0,    52,    53,    54,    43,     0,     0,
       0,     0,    55,     0,    56,    57,    58,     0,    59,    60,
      61,    62,     0,     0,     0,     0,    63,    64,    65,    66,
       0,     0,     0,     0,     0,    22,    67,    68,     0,     0,
      24,     7,     8,     9,     0,     0,    10,    69,    11,    12,
       7,     8,     9,    13,     0,    10,    14,    11,    12,    15,
      16,     0,    13,     0,     0,    14,     0,     0,    15,    16,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    17,     0,    18,     0,    19,    20,
      21,     0,     0,    17,     0,    18,     0,    19,    20,    21,
       0,     0,     0,     0,     0,     0,     0,     0,   476,   584,
       0,     0,    61,    62,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    22,    67,     0,
       0,     0,    24,     0,     0,     0,    22,    23,     7,     8,
       9,    24,     0,    10,     0,    11,    12,     0,     0,     0,
      13,     0,     0,    14,     0,     0,    15,    16,     7,     8,
       9,     0,     0,    10,     0,    11,    12,     0,     0,     0,
      13,     0,     0,    14,     0,     0,    15,    16,     0,     0,
       0,    17,     0,    18,     0,    19,    20,    21,     0,     0,
     172,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    17,     0,    18,     0,    19,    20,    21,     0,     0,
       0,     0,     0,     0,     0,     0,   476,     0,     0,     0,
       0,     0,     0,     0,    22,    23,     0,     0,     0,    24,
       0,     0,     0,     0,     0,     7,     8,     9,     0,     0,
      10,     0,    11,    12,    22,    23,     0,    13,     0,    24,
      14,     0,     0,    15,    16,     7,   501,     9,     0,     0,
      10,     0,    11,    12,     0,     0,     0,    13,     0,     0,
      14,     0,     0,    15,    16,     0,     0,     0,    17,     0,
      18,     0,    19,    20,    21,     0,   479,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    17,     0,
      18,     0,    19,    20,    21,     7,     8,     9,     0,     0,
      10,     0,    11,    12,     0,     0,     0,    13,     0,     0,
      14,    22,    23,    15,    16,     0,    24,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    22,    23,     0,   502,     0,    24,     0,    17,     0,
      18,     0,    19,    20,    21,     7,     8,     9,     0,     0,
      10,     0,    11,    12,     0,     0,   575,    13,     0,     0,
      14,     0,     0,    15,    16,   176,   177,   178,     0,     0,
     179,     0,   180,   181,     0,     0,     0,   182,     0,     0,
     183,    22,    23,   184,   185,     0,    24,     0,    17,     0,
      18,     0,    19,    20,    21,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   186,     0,
     187,     0,   188,   189,   190,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    22,    23,     0,     0,     0,    24,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    22,   307,     0,     0,     0,   194
};

static const yytype_int16 yycheck[] =
{
       2,    10,     5,    85,     5,     1,    85,    86,    68,   192,
      95,    73,    73,   408,    92,    73,     5,     3,     4,   490,
     216,   473,   522,   549,     4,    43,    32,   476,    61,    43,
      15,     4,    18,   485,    15,    55,    54,    99,    99,     4,
      43,    61,    43,     4,    15,    43,    48,    53,    62,    52,
       0,    52,     6,     7,    43,    52,    54,     3,     4,     5,
      52,    52,     8,    52,    10,    11,    15,    52,    49,    15,
      55,   571,    18,    82,   600,    21,    22,    15,    64,    52,
      43,    52,    85,    86,    85,    86,    95,    52,    18,    74,
      75,    52,    55,    90,   179,    15,    15,    86,    90,    90,
      46,    15,    48,    52,    50,    51,    52,    53,   117,    95,
      15,   611,    92,   562,    52,   567,    49,    55,   589,     6,
       7,   117,    15,    21,    22,    74,    75,     6,     7,   325,
      49,    15,    52,   604,   120,    49,    74,    75,   609,   217,
      20,    74,    75,    89,    90,   328,   541,    52,    94,    19,
      52,    29,   155,    48,   155,    74,    75,    52,    48,    52,
     156,    48,    52,    50,    51,    68,   155,    30,    52,    48,
     179,    50,    51,   159,    53,   161,   162,   239,   239,    74,
      75,   239,    15,    52,    74,    75,    55,    15,    90,   175,
      31,    43,    89,    62,    52,    90,   258,   258,    52,    43,
      90,   187,    71,    55,    64,     3,     4,     5,     6,     7,
       8,    55,    10,    11,    27,    28,    49,    15,    43,   304,
      18,    49,    91,    21,    22,    94,    95,    96,    97,    98,
      99,    62,    90,    93,    43,   418,    90,    43,    74,    75,
      74,    75,    64,    74,    75,    62,    55,    48,    46,    55,
      48,    65,    50,    51,    52,    53,    90,    74,    75,    90,
      74,    75,    55,    43,    43,    43,    43,    48,    61,   329,
      49,    49,    49,    90,    65,    55,    90,   180,   181,   182,
     183,   184,   185,    74,    75,   188,   189,    52,    43,   192,
      55,    89,    90,   295,    92,   304,    94,    62,    52,    90,
      55,   379,     3,     4,     5,    48,    71,     8,    55,    10,
      11,    74,    75,   395,    15,    65,   395,    18,   304,    48,
      21,    22,    61,   332,    74,    75,    91,    90,    65,    94,
      95,    96,    97,    98,    99,    43,   332,    74,    75,    55,
      90,    49,    84,    85,    86,    46,    88,    48,    43,    50,
      51,    52,    53,    90,    49,    64,    11,    12,    13,    14,
      15,    16,   424,   424,    19,    20,    43,    64,    43,    43,
      21,    22,    49,   382,    49,    49,   372,   373,   374,   375,
     389,   390,    27,    28,    43,     6,     7,   383,    89,    90,
      49,    92,   395,    94,   395,    43,    43,    55,   384,    21,
      22,    49,    49,   399,    91,   180,   181,   182,   183,   184,
     185,   473,   473,   188,   189,    55,    55,   413,   414,    61,
     406,    61,   408,   485,   485,   328,   329,    23,    24,    25,
      26,    15,   335,   336,   337,   338,   339,   340,   341,   342,
     343,   344,   345,   346,   347,   348,   349,   350,   351,   352,
     353,   354,   355,   356,   357,   358,   359,   360,   361,   362,
     363,   364,   365,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    52,    53,    55,    43,    55,    55,
     476,   490,    61,    49,    61,    61,   130,   131,    55,    64,
      65,     3,     4,     5,    61,   498,     8,   498,    10,    11,
      61,    43,    55,    15,    52,    43,    18,    49,    61,    21,
      22,    49,    43,   522,    59,   418,   512,   519,    49,   136,
     137,   523,    23,    24,    25,    26,   132,   133,   134,   135,
      52,    43,   344,   345,    46,    52,    48,    52,    50,    51,
      52,   346,   347,   348,   349,    45,   549,   543,   549,    61,
      12,    13,    14,   350,   351,   541,    15,    16,    17,    12,
      13,    14,   571,    15,    16,    17,   562,    55,   570,    64,
     572,    43,   568,    18,   577,    19,   577,    89,    90,    20,
     589,   583,    94,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    29,   604,    30,   600,    31,   600,
     609,    43,   611,   599,   606,     1,    53,     3,     4,     5,
      48,    48,     8,    90,    10,    11,   373,   374,   375,    15,
      90,    57,    18,    55,    55,    21,    22,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   121,   122,
     123,    48,   124,   125,   126,   335,   336,   337,    44,    55,
      46,    54,    48,    52,    50,    51,    52,    52,    52,    55,
      56,    52,    58,    59,    60,    61,    62,   338,   339,   340,
      66,    64,    68,    69,    70,    52,    72,    73,    74,    75,
       4,    15,    55,    55,    80,    81,    82,    83,    48,    64,
      61,    43,    49,    89,    90,    91,    64,    52,    94,     3,
       4,     5,    55,    15,     8,   101,    10,    11,    55,    15,
      52,    15,     4,    62,    18,    43,    49,    21,    22,   355,
     356,   357,   358,   359,   360,   361,   362,   363,   364,   365,
      49,    49,    64,    64,    92,     4,    49,    52,    49,    48,
      44,    49,    46,    48,    48,    48,    50,    51,    52,    49,
      49,    55,    56,   379,    58,    59,    60,    61,    62,    63,
     332,   329,    66,    64,    68,    69,    70,    64,    72,    73,
      74,    75,   399,    77,   128,    79,    80,    81,    82,    83,
     139,   127,   138,   567,   129,    89,    90,    91,   473,   140,
      94,     3,     4,     5,   386,   235,     8,   101,    10,    11,
      82,   577,   341,    15,   353,   342,    18,   343,   352,    21,
      22,   354,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    44,    -1,    46,    -1,    48,    -1,    50,    51,
      52,    -1,    -1,    55,    56,    -1,    58,    59,    60,    61,
      62,    63,    -1,    -1,    66,    -1,    68,    69,    70,    -1,
      72,    73,    74,    75,    -1,    77,    -1,    79,    80,    81,
      82,    83,    -1,    -1,    -1,    -1,    -1,    89,    90,    91,
      -1,    -1,    94,     3,     4,     5,    -1,    -1,     8,   101,
      10,    11,    -1,    -1,    -1,    15,    -1,    -1,    18,    -1,
      -1,    21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    44,    -1,    46,    -1,    48,    -1,
      50,    51,    52,    -1,    -1,    55,    56,    -1,    58,    59,
      60,    61,    -1,    63,    -1,    -1,    66,    -1,    68,    69,
      70,    -1,    72,    73,    74,    75,    -1,    77,    78,    79,
      80,    81,    82,    83,    -1,    -1,    -1,    -1,    -1,    89,
      90,    91,    -1,    -1,    94,     3,     4,     5,    -1,    -1,
       8,   101,    10,    11,    -1,    -1,    -1,    15,    -1,    -1,
      18,    -1,    -1,    21,    22,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    44,    -1,    46,    -1,
      48,    -1,    50,    51,    52,    -1,    -1,    55,    56,    -1,
      58,    59,    60,    61,    -1,    63,    -1,    -1,    66,    -1,
      68,    69,    70,    -1,    72,    73,    74,    75,    -1,    77,
      -1,    79,    80,    81,    82,    83,    -1,    -1,    -1,    -1,
      -1,    89,    90,    91,    -1,    -1,    94,     3,     4,     5,
      -1,    -1,     8,   101,    10,    11,    -1,    -1,    -1,    15,
      -1,    -1,    18,    -1,    -1,    21,    22,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    -1,
      46,    -1,    48,    -1,    50,    51,    52,    -1,    -1,    55,
      56,    -1,    58,    59,    60,    61,    62,    -1,    -1,    -1,
      66,    -1,    68,    69,    70,    -1,    72,    73,    74,    75,
      -1,    -1,    -1,    -1,    80,    81,    82,    83,    -1,    -1,
      -1,    -1,    -1,    89,    90,    91,    -1,    -1,    94,     3,
       4,     5,    -1,    -1,     8,   101,    10,    11,    -1,    -1,
      -1,    15,    -1,    -1,    18,    -1,    -1,    21,    22,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      44,    -1,    46,    -1,    48,    -1,    50,    51,    52,    -1,
      -1,    55,    56,    -1,    58,    59,    60,    61,    -1,    -1,
      -1,    -1,    66,    -1,    68,    69,    70,    -1,    72,    73,
      74,    75,    -1,    -1,    -1,    -1,    80,    81,    82,    83,
      -1,    -1,    -1,    -1,    -1,    89,    90,    91,    -1,    -1,
      94,     3,     4,     5,    -1,    -1,     8,   101,    10,    11,
       3,     4,     5,    15,    -1,     8,    18,    10,    11,    21,
      22,    -1,    15,    -1,    -1,    18,    -1,    -1,    21,    22,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    -1,    48,    -1,    50,    51,
      52,    -1,    -1,    46,    -1,    48,    -1,    50,    51,    52,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    62,
      -1,    -1,    74,    75,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,    90,    -1,
      -1,    -1,    94,    -1,    -1,    -1,    89,    90,     3,     4,
       5,    94,    -1,     8,    -1,    10,    11,    -1,    -1,    -1,
      15,    -1,    -1,    18,    -1,    -1,    21,    22,     3,     4,
       5,    -1,    -1,     8,    -1,    10,    11,    -1,    -1,    -1,
      15,    -1,    -1,    18,    -1,    -1,    21,    22,    -1,    -1,
      -1,    46,    -1,    48,    -1,    50,    51,    52,    -1,    -1,
      55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    46,    -1,    48,    -1,    50,    51,    52,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    90,    -1,    -1,    -1,    94,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,    -1,
       8,    -1,    10,    11,    89,    90,    -1,    15,    -1,    94,
      18,    -1,    -1,    21,    22,     3,     4,     5,    -1,    -1,
       8,    -1,    10,    11,    -1,    -1,    -1,    15,    -1,    -1,
      18,    -1,    -1,    21,    22,    -1,    -1,    -1,    46,    -1,
      48,    -1,    50,    51,    52,    -1,    54,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,
      48,    -1,    50,    51,    52,     3,     4,     5,    -1,    -1,
       8,    -1,    10,    11,    -1,    -1,    -1,    15,    -1,    -1,
      18,    89,    90,    21,    22,    -1,    94,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    89,    90,    -1,    92,    -1,    94,    -1,    46,    -1,
      48,    -1,    50,    51,    52,     3,     4,     5,    -1,    -1,
       8,    -1,    10,    11,    -1,    -1,    64,    15,    -1,    -1,
      18,    -1,    -1,    21,    22,     3,     4,     5,    -1,    -1,
       8,    -1,    10,    11,    -1,    -1,    -1,    15,    -1,    -1,
      18,    89,    90,    21,    22,    -1,    94,    -1,    46,    -1,
      48,    -1,    50,    51,    52,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,
      48,    -1,    50,    51,    52,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    89,    90,    -1,    -1,    -1,    94,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    89,    90,    -1,    -1,    -1,    94
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    84,    85,    86,    88,   176,   177,     3,     4,     5,
       8,    10,    11,    15,    18,    21,    22,    46,    48,    50,
      51,    52,    89,    90,    94,   115,   118,   119,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   179,    61,   175,   134,   135,   135,    44,    52,
      55,    56,    58,    59,    60,    66,    68,    69,    70,    72,
      73,    74,    75,    80,    81,    82,    83,    90,    91,   101,
     109,   113,   135,   136,   144,   145,   146,   147,   148,   149,
     150,   157,   159,   160,   161,   163,   164,   165,   166,   167,
     171,   172,   173,   175,     0,    48,    52,    90,   120,   136,
     146,   147,   121,   121,   121,   121,   121,   121,   135,   121,
     121,     6,     7,     4,    52,     6,     7,    48,    50,    51,
      53,    12,    13,    14,    15,    16,    17,    18,    20,    19,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    89,    62,   161,   174,    43,   175,    64,    48,
     161,    48,    48,    52,    90,    52,    90,    52,    90,    52,
      90,    55,    55,   135,    52,    48,     3,     4,     5,     8,
      10,    11,    15,    18,    21,    22,    46,    48,    50,    51,
      52,    53,    90,    92,    94,   105,   106,   107,   108,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   191,   192,   193,   194,   195,    61,    55,    55,
      15,    52,    55,   148,   149,   151,   152,   153,   154,   156,
      64,    64,    55,    61,    55,    61,    55,    74,    75,   136,
     157,   165,   166,   161,   166,    55,   109,   120,   135,    15,
      49,    52,    52,    52,    52,   116,   117,   134,   136,   135,
     122,   122,   122,   123,   123,   123,   124,   125,   126,   127,
     127,   128,   128,   128,   128,   129,   129,   130,   131,   132,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,     1,    62,   161,   134,    45,   135,    59,   135,   135,
     162,    55,    55,   135,    48,    52,   120,    90,   182,   182,
     182,   182,   182,   182,   135,   182,   182,     4,    92,   103,
     104,     6,     7,    92,   106,   195,     4,    52,    64,    43,
       6,     7,    48,    50,    51,    12,    13,    14,    15,    16,
      17,    18,    20,    19,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,   105,    52,    55,    62,
      71,    94,    95,    96,    97,    98,    99,   109,   110,   114,
      15,    52,    48,    32,    53,    48,    43,    90,    90,   143,
     143,    52,    52,   151,    55,    57,    55,    49,    49,    43,
      49,    15,    49,    54,   175,    49,    48,    49,    55,    49,
     120,   135,    49,    21,    22,    54,    52,    52,    64,   106,
     107,    52,    52,   117,   136,   183,   183,   183,   184,   184,
     184,   185,   186,   187,   188,   188,   189,   189,   189,   189,
     190,   190,   191,   192,   193,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,    64,    93,    52,   112,
       4,    52,   111,   134,   111,   111,   111,     4,    62,   110,
      15,    52,    65,   136,   155,   158,    61,   134,   138,    54,
     135,    15,   151,   156,    62,   136,   142,    62,    55,    55,
      48,   165,   166,   116,    15,    49,   135,   162,    61,    49,
      49,     4,    92,   134,   134,   106,    49,    49,    43,    55,
      64,    64,    43,    55,    55,    55,    55,    43,    52,    49,
     152,   154,    43,    49,    43,   134,   137,   138,    54,    15,
      52,    55,   140,   141,   152,    55,    65,   158,    15,    49,
      49,    55,    62,    63,    77,    79,   161,   168,   169,   170,
      15,    15,    52,   134,     4,    55,   175,    65,   155,    55,
     175,    62,    43,    62,    15,    52,    49,    43,    52,   139,
      49,    43,    49,    49,   162,    64,   134,    64,    64,    62,
     169,    92,     4,    49,    62,   134,   138,    52,    49,    48,
     141,   134,    55,   175,    65,    55,   175,    49,    64,    65,
     170,    55,   175,    49,    48,   158,    49,   134,    78,    48,
     158,    43,    49,    55,   175,    64,    64,   158,    49,    49
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   102,   103,   103,   103,   103,   103,   104,   104,   104,
     105,   106,   106,   107,   107,   107,   107,   107,   107,   108,
     108,   108,   109,   110,   110,   110,   110,   111,   111,   110,
     110,   110,   110,   110,   112,   112,   110,   110,   110,   113,
     114,   114,   113,   115,   115,   115,   115,   115,   115,   115,
     115,   116,   116,   117,   117,   118,   118,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     120,   120,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   122,   122,   122,   122,   123,
     123,   123,   123,   124,   124,   125,   125,   126,   126,   127,
     127,   127,   128,   128,   128,   128,   128,   129,   129,   129,
     130,   130,   131,   131,   132,   132,   133,   133,   133,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   134,   135,
     135,   136,   136,   136,   136,   136,   137,   137,   137,   137,
     138,   138,   138,   139,   140,   140,   141,   141,   142,   143,
     143,   143,   144,   145,   144,   145,   144,   145,   146,   147,
     146,   147,   148,   149,   150,   151,   151,   151,   151,   152,
     153,   153,   153,   153,   152,   152,   152,   152,   152,   154,
     154,   155,   155,   156,   156,   156,   157,   158,   158,   158,
     159,   159,   159,   159,   159,   159,   159,   159,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     161,   161,   162,   162,   163,   163,   164,   165,   165,   165,
     166,   166,   166,   167,   168,   169,   169,   169,   169,   169,
     169,   170,   170,   171,   171,   172,   173,   173,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   174,   174,
     174,   175,   175,   176,   176,   177,   177,   177,   177,   177,
     178,   178,   179,   179,   178,   178,   178,   178,   178,   178,
     180,   180,   181,   181,   181,   181,   181,   181,   181,   181,
     181,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   183,   183,   183,   183,   184,   184,
     184,   184,   185,   185,   186,   186,   187,   187,   188,   188,
     188,   189,   189,   189,   189,   189,   190,   190,   190,   191,
     191,   192,   192,   193,   193,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   195
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
       1,     1,     1,     1,     2,     2,     3,     3,     1,     1,
       3,     4,     3,     2,     3,     1,     1,     2,     2,     3,
       3,     0,     1,     1,     2,     2,     2,     2,     1,     1,
       3,     3,     4,     4,     4,     1,     2,     3,     4,     1,
       0,     1,     2,     3,     8,     9,    10,     4,     3,     3,
       3,     2,     2,     3,     1,     1,     2,     1,     3,     0,
       1,     1,     1,     1,     2,     2,     2,     2,     6,     8,
       6,     7,     9,     7,     6,     8,     6,     7,     9,     7,
       1,     1,     1,     0,     4,     8,     4,     2,     4,     2,
       1,     4,     2,     6,     5,     1,     2,     3,     2,     5,
       1,     2,     1,     7,     6,     1,     2,     3,     2,     2,
       1,     1,     2,     3,     1,     3,     2,     2,     1,     1,
       2,     1,     3,     2,     2,     2,     4,     4,     1,     2,
       2,     3,     2,     0,     2,     1,     2,     2,     2,     2,
       2,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     3,     1,     3,     3,     3,     3,     2,     2,     4,
       4,     1,     2,     2,     4,     4,     2,     2,     2,     2,
       2,     2,     2,     2,     1,     3,     3,     3,     1,     3,
       3,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       3,     1,     3,     3,     3,     3,     1,     3,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
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

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



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
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule)
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
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
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
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
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


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
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
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
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
# undef YYSTACK_RELOCATE
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

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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
  yychar = YYEMPTY;
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
  case 2:
#line 61 "HolyC.y"
                    {yyval=AppendToSIB(NULL,NULL,yyvsp[0],NULL);}
#line 2093 "HolyC.tab.c"
    break;

  case 3:
#line 62 "HolyC.y"
                                         {
  yyval=AppendToSIB(yyvsp[-2],NULL,yyvsp[0],NULL);
  ReleaseAST(yyvsp[-1]);
}
#line 2102 "HolyC.tab.c"
    break;

  case 4:
#line 66 "HolyC.y"
                                                        {
  yyval=AppendToSIB(yyvsp[-4],yyvsp[-2],yyvsp[0],NULL);
  ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-1]);
}
#line 2111 "HolyC.tab.c"
    break;

  case 5:
#line 70 "HolyC.y"
                                                         {
  yyval=AppendToSIB(yyvsp[-4],yyvsp[0],yyvsp[-2],NULL);
  ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-1]);
}
#line 2120 "HolyC.tab.c"
    break;

  case 6:
#line 74 "HolyC.y"
                  {
  yyval=AppendToSIB(NULL,NULL,NULL,yyvsp[0]);
 }
#line 2128 "HolyC.tab.c"
    break;

  case 8:
#line 78 "HolyC.y"
                                   {
  yyval=AppendToSIB(yyvsp[-2],NULL,NULL,yyvsp[0]);
  ReleaseAST(yyvsp[-1]);
}
#line 2137 "HolyC.tab.c"
    break;

  case 9:
#line 82 "HolyC.y"
                                    {
  yyval=AppendToSIB(yyvsp[-2],NULL,NULL,yyvsp[0]);
  ReleaseAST(yyvsp[-1]);
}
#line 2146 "HolyC.tab.c"
    break;

  case 10:
#line 86 "HolyC.y"
                                                      {
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
  yyval=yyvsp[-1];
}
#line 2155 "HolyC.tab.c"
    break;

  case 12:
#line 91 "HolyC.y"
                            {
  yyval=AppendToSIB(yyvsp[0],NULL,NULL,yyvsp[-1]);
 }
#line 2163 "HolyC.tab.c"
    break;

  case 16:
#line 97 "HolyC.y"
                                 {
  yyval=yyvsp[0];
  CType *t=*map_get(&Compiler.types,yyvsp[-1]->name);
  yyval->asmAddr.width=TypeSize(t);
}
#line 2173 "HolyC.tab.c"
    break;

  case 17:
#line 102 "HolyC.y"
                                                     {
  CType *t=*map_get(&Compiler.types,yyvsp[-3]->name);
  yyvsp[0]->asmAddr.width=TypeSize(t);
  yyvsp[0]->asmAddr.segment=yyvsp[-2];
  yyval=yyvsp[0];
}
#line 2184 "HolyC.tab.c"
    break;

  case 18:
#line 108 "HolyC.y"
                                            {
  if(yyvsp[-2]->type!=AST_ASM_REG) RaiseError(yyvsp[-1],"Expected register.");
  else {
    yyvsp[0]->asmAddr.segment=yyvsp[-2];
  }
  yyval=yyvsp[0];
  ReleaseAST(yyvsp[-1]);
}
#line 2197 "HolyC.tab.c"
    break;

  case 19:
#line 116 "HolyC.y"
                                {yyval=yyvsp[0];}
#line 2203 "HolyC.tab.c"
    break;

  case 20:
#line 117 "HolyC.y"
                                                            {
  yyval=CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA);
  ReleaseAST(yyvsp[-1]);
}
#line 2212 "HolyC.tab.c"
    break;

  case 21:
#line 121 "HolyC.y"
                 {yyval=NULL;}
#line 2218 "HolyC.tab.c"
    break;

  case 22:
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
#line 2232 "HolyC.tab.c"
    break;

  case 23:
#line 131 "HolyC.y"
                                         {
  yyval=SOT(CreateExportedLabel(yyvsp[-1]),yyvsp[-1]);
  yyval->inAsmBlk=1;
  ReleaseAST(yyvsp[0]);
}
#line 2242 "HolyC.tab.c"
    break;

  case 24:
#line 136 "HolyC.y"
                                  {
  yyval=SOT(CreateLabel(yyvsp[-1]),yyvsp[-1]);
  yyval->inAsmBlk=1;
  ReleaseAST(yyvsp[0]);
 }
#line 2252 "HolyC.tab.c"
    break;

  case 25:
#line 141 "HolyC.y"
                                                {
  yyval=SOT(CreateLocalLabel(yyvsp[-1]),yyvsp[-2]);
  yyval->inAsmBlk=1;
  ReleaseAST(yyvsp[0]),ReleaseAST(yyvsp[-2]);
}
#line 2262 "HolyC.tab.c"
    break;

  case 26:
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
#line 2278 "HolyC.tab.c"
    break;

  case 28:
#line 159 "HolyC.y"
                                                {
  yyval=CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA);
  ReleaseAST(yyvsp[-1]);
}
#line 2287 "HolyC.tab.c"
    break;

  case 29:
#line 163 "HolyC.y"
                                                {
  yyval=SOT(CreateDU8(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2296 "HolyC.tab.c"
    break;

  case 30:
#line 167 "HolyC.y"
                                                 {
  yyval=SOT(CreateDU16(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2305 "HolyC.tab.c"
    break;

  case 31:
#line 171 "HolyC.y"
                                                 {
  yyval=SOT(CreateDU32(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2314 "HolyC.tab.c"
    break;

  case 32:
#line 175 "HolyC.y"
                                                 {
  yyval=SOT(CreateDU64(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2323 "HolyC.tab.c"
    break;

  case 35:
#line 181 "HolyC.y"
                                             {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA),yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
}
#line 2332 "HolyC.tab.c"
    break;

  case 36:
#line 185 "HolyC.y"
                                                       {
  yyval=SOT(CreateAsmImport(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2341 "HolyC.tab.c"
    break;

  case 37:
#line 189 "HolyC.y"
                          {
  yyval=SOT(CreateNop(),yyvsp[0]);
  ReleaseAST(yyvsp[0]);
 }
#line 2350 "HolyC.tab.c"
    break;

  case 38:
#line 193 "HolyC.y"
                                                           {
  yyval=SOT(CreateAsmAlign(yyvsp[-2],yyvsp[0]),yyvsp[-3]);
  ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-3]);
}
#line 2359 "HolyC.tab.c"
    break;

  case 39:
#line 197 "HolyC.y"
                                                   {
  yyval=SOT(CreateNop(),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 2368 "HolyC.tab.c"
    break;

  case 41:
#line 202 "HolyC.y"
                                                   {
  yyval=AppendToStmts(yyvsp[-1],yyvsp[0]);
}
#line 2376 "HolyC.tab.c"
    break;

  case 42:
#line 205 "HolyC.y"
                                                                    {
  yyval=yyvsp[-1];
  yyval->type=AST_ASM_BLK;
  ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2386 "HolyC.tab.c"
    break;

  case 43:
#line 211 "HolyC.y"
                            {
  yyval=SOT(yyvsp[0],yyvsp[-1]);
  char buffer[256];
  sprintf(buffer,"@@%s",yyval->name);
  TD_FREE(yyval->name);
  yyval->name=strdup(buffer);
  ReleaseAST(yyvsp[-1]);
}
#line 2399 "HolyC.tab.c"
    break;

  case 44:
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
#line 2413 "HolyC.tab.c"
    break;

  case 45:
#line 228 "HolyC.y"
                {yyval=SLE(yyvsp[0]);}
#line 2419 "HolyC.tab.c"
    break;

  case 46:
#line 229 "HolyC.y"
              {yyval=SLE(yyvsp[0]);}
#line 2425 "HolyC.tab.c"
    break;

  case 47:
#line 230 "HolyC.y"
               {yyval=SLE(yyvsp[0]);}
#line 2431 "HolyC.tab.c"
    break;

  case 48:
#line 231 "HolyC.y"
              {yyval=yyvsp[0];}
#line 2437 "HolyC.tab.c"
    break;

  case 49:
#line 232 "HolyC.y"
                    {
  yyval=SLE(yyvsp[0]);
}
#line 2445 "HolyC.tab.c"
    break;

  case 50:
#line 235 "HolyC.y"
               {
  yyval=SLE(yyvsp[0]);
}
#line 2453 "HolyC.tab.c"
    break;

  case 51:
#line 238 "HolyC.y"
              {yyval=NULL;}
#line 2459 "HolyC.tab.c"
    break;

  case 52:
#line 239 "HolyC.y"
                      {yyval=yyvsp[0];}
#line 2465 "HolyC.tab.c"
    break;

  case 53:
#line 240 "HolyC.y"
                                               {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA),yyvsp[-1]);
}
#line 2473 "HolyC.tab.c"
    break;

  case 54:
#line 243 "HolyC.y"
                          {yyval=yyvsp[0];}
#line 2479 "HolyC.tab.c"
    break;

  case 56:
#line 245 "HolyC.y"
                                                         {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2488 "HolyC.tab.c"
    break;

  case 58:
#line 254 "HolyC.y"
                                     {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2497 "HolyC.tab.c"
    break;

  case 59:
#line 258 "HolyC.y"
                                       {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2506 "HolyC.tab.c"
    break;

  case 60:
#line 262 "HolyC.y"
                                  {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2515 "HolyC.tab.c"
    break;

  case 61:
#line 266 "HolyC.y"
                                    {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2524 "HolyC.tab.c"
    break;

  case 62:
#line 270 "HolyC.y"
                                                                    {
  yyval=SOT(CreateArrayAccess(yyvsp[-3],yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2533 "HolyC.tab.c"
    break;

  case 63:
#line 274 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[-1],AST_POST_DEC),yyvsp[0]);
  ReleaseAST(yyvsp[0]);
}
#line 2542 "HolyC.tab.c"
    break;

  case 64:
#line 278 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[-1],AST_POST_INC),yyvsp[0]);
  ReleaseAST(yyvsp[0]);
}
#line 2551 "HolyC.tab.c"
    break;

  case 65:
#line 282 "HolyC.y"
                                                                {
  yyval=SOT(CreateFuncCall(yyvsp[-3],yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2560 "HolyC.tab.c"
    break;

  case 66:
#line 286 "HolyC.y"
                                                                 {
  yyval=CreateExplicitTypecast(yyvsp[-3],yyvsp[-1]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2569 "HolyC.tab.c"
    break;

  case 67:
#line 290 "HolyC.y"
                                                                          {
  yyvsp[-2]->type2=CreateMultiLvlPtr(yyvsp[-2]->type2,1);
  yyval=CreateExplicitTypecast(yyvsp[-4],yyvsp[-2]);
  ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 2579 "HolyC.tab.c"
    break;

  case 68:
#line 295 "HolyC.y"
                                                                                   {
  yyvsp[-3]->type2=CreateMultiLvlPtr(yyvsp[-3]->type2,2);
  yyval=CreateExplicitTypecast(yyvsp[-5],yyvsp[-3]);
  ReleaseAST(yyvsp[-4]),ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 2589 "HolyC.tab.c"
    break;

  case 69:
#line 300 "HolyC.y"
                                                                                            {
  yyvsp[-4]->type2=CreateMultiLvlPtr(yyvsp[-4]->type2,3);
  yyval=CreateExplicitTypecast(yyvsp[-6],yyvsp[-4]);
  ReleaseAST(yyvsp[-5]),ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 2599 "HolyC.tab.c"
    break;

  case 70:
#line 308 "HolyC.y"
                             {yyval=yyvsp[0];}
#line 2605 "HolyC.tab.c"
    break;

  case 71:
#line 309 "HolyC.y"
                                        {
  CType *ptr =CreateMultiLvlPtr(yyvsp[-1]->type2,1);
  yyval=CreateTypeNode(ptr);
  ReleaseAST(yyvsp[0]);
}
#line 2615 "HolyC.tab.c"
    break;

  case 73:
#line 315 "HolyC.y"
                            {
  yyval=SOT(CreateSizeof(yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2624 "HolyC.tab.c"
    break;

  case 74:
#line 319 "HolyC.y"
                                   {
  yyval=SOT(CreateSizeof(yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2633 "HolyC.tab.c"
    break;

  case 75:
#line 323 "HolyC.y"
                                                                    {
  yyval=SOT(CreateSizeof(yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 2644 "HolyC.tab.c"
    break;

  case 76:
#line 329 "HolyC.y"
                                                                     {
  yyval=SOT(CreateSizeof(yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 2655 "HolyC.tab.c"
    break;

  case 77:
#line 335 "HolyC.y"
                           {
  yyval=SOT(CreateUnop(yyvsp[0],AST_ADDROF),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2664 "HolyC.tab.c"
    break;

  case 78:
#line 339 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_DERREF),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2673 "HolyC.tab.c"
    break;

  case 79:
#line 344 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_PRE_DEC),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2682 "HolyC.tab.c"
    break;

  case 80:
#line 348 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_PRE_INC),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2691 "HolyC.tab.c"
    break;

  case 81:
#line 352 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_POS),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2700 "HolyC.tab.c"
    break;

  case 82:
#line 356 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_NEG),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2709 "HolyC.tab.c"
    break;

  case 83:
#line 360 "HolyC.y"
                           {
  yyval=SOT(CreateUnop(yyvsp[0],AST_LNOT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2718 "HolyC.tab.c"
    break;

  case 84:
#line 364 "HolyC.y"
                           {
  yyval=SOT(CreateUnop(yyvsp[0],AST_BNOT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2727 "HolyC.tab.c"
    break;

  case 86:
#line 371 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_POW),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2736 "HolyC.tab.c"
    break;

  case 87:
#line 375 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SHL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2745 "HolyC.tab.c"
    break;

  case 88:
#line 379 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SHR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2754 "HolyC.tab.c"
    break;

  case 90:
#line 385 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_MUL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2763 "HolyC.tab.c"
    break;

  case 91:
#line 389 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_DIV),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2772 "HolyC.tab.c"
    break;

  case 92:
#line 393 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_MOD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2781 "HolyC.tab.c"
    break;

  case 94:
#line 399 "HolyC.y"
                                    {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2790 "HolyC.tab.c"
    break;

  case 96:
#line 405 "HolyC.y"
                                    {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2799 "HolyC.tab.c"
    break;

  case 98:
#line 411 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2808 "HolyC.tab.c"
    break;

  case 100:
#line 417 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ADD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2817 "HolyC.tab.c"
    break;

  case 101:
#line 421 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SUB),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2826 "HolyC.tab.c"
    break;

  case 103:
#line 427 "HolyC.y"
                                  {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_LT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2835 "HolyC.tab.c"
    break;

  case 104:
#line 431 "HolyC.y"
                                  {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_GT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2844 "HolyC.tab.c"
    break;

  case 105:
#line 435 "HolyC.y"
                                  {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_LE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2853 "HolyC.tab.c"
    break;

  case 106:
#line 439 "HolyC.y"
                                  {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_GE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2862 "HolyC.tab.c"
    break;

  case 108:
#line 445 "HolyC.y"
                                     {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_EQ),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2871 "HolyC.tab.c"
    break;

  case 109:
#line 449 "HolyC.y"
                                     {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_NE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2880 "HolyC.tab.c"
    break;

  case 111:
#line 455 "HolyC.y"
                                       {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2889 "HolyC.tab.c"
    break;

  case 113:
#line 461 "HolyC.y"
                                       {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2898 "HolyC.tab.c"
    break;

  case 115:
#line 467 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2907 "HolyC.tab.c"
    break;

  case 117:
#line 473 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2916 "HolyC.tab.c"
    break;

  case 118:
#line 477 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SHL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2925 "HolyC.tab.c"
    break;

  case 119:
#line 481 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SHR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2934 "HolyC.tab.c"
    break;

  case 120:
#line 485 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_MUL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2943 "HolyC.tab.c"
    break;

  case 121:
#line 489 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_DIV),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2952 "HolyC.tab.c"
    break;

  case 122:
#line 493 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_MOD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2961 "HolyC.tab.c"
    break;

  case 123:
#line 497 "HolyC.y"
                                          {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2970 "HolyC.tab.c"
    break;

  case 124:
#line 501 "HolyC.y"
                                          {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2979 "HolyC.tab.c"
    break;

  case 125:
#line 505 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2988 "HolyC.tab.c"
    break;

  case 126:
#line 509 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_ADD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2997 "HolyC.tab.c"
    break;

  case 127:
#line 513 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SUB),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 3006 "HolyC.tab.c"
    break;

  case 129:
#line 520 "HolyC.y"
                                              {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 3015 "HolyC.tab.c"
    break;

  case 131:
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
#line 3030 "HolyC.tab.c"
    break;

  case 132:
#line 539 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 3036 "HolyC.tab.c"
    break;

  case 133:
#line 540 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 3042 "HolyC.tab.c"
    break;

  case 134:
#line 541 "HolyC.y"
                                      {
  yyval=yyvsp[0];
  AssignUnionBasetype(yyvsp[0],yyvsp[-1]);
}
#line 3051 "HolyC.tab.c"
    break;

  case 135:
#line 545 "HolyC.y"
                                      {
  yyval=yyvsp[0];
  AssignClassBasetype(yyvsp[0],yyvsp[-1]);
}
#line 3060 "HolyC.tab.c"
    break;

  case 136:
#line 552 "HolyC.y"
                                          {yyval=AppendToArrLiteral(yyvsp[-2],yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3066 "HolyC.tab.c"
    break;

  case 137:
#line 553 "HolyC.y"
                                            {yyval=AppendToArrLiteral(yyvsp[-2],yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3072 "HolyC.tab.c"
    break;

  case 138:
#line 554 "HolyC.y"
              {yyval=AppendToArrLiteral(NULL,yyvsp[0]);}
#line 3078 "HolyC.tab.c"
    break;

  case 139:
#line 555 "HolyC.y"
                {yyval=AppendToArrLiteral(NULL,yyvsp[0]);}
#line 3084 "HolyC.tab.c"
    break;

  case 140:
#line 556 "HolyC.y"
                                                       {
  yyval=SOT(yyvsp[-1],yyvsp[-2]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3094 "HolyC.tab.c"
    break;

  case 141:
#line 561 "HolyC.y"
                                                                 {
  yyval=SOT(yyvsp[-2],yyvsp[-3]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3105 "HolyC.tab.c"
    break;

  case 142:
#line 567 "HolyC.y"
                                                      {
  yyval=NULL;
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3116 "HolyC.tab.c"
    break;

  case 143:
#line 576 "HolyC.y"
                             {
  yyval=AppendToMetaData(NULL,yyvsp[-1],yyvsp[0]);
}
#line 3124 "HolyC.tab.c"
    break;

  case 144:
#line 579 "HolyC.y"
                                                    {
  assert(yyvsp[0]->type==__AST_DECL_TAILS);
  CDeclTail b2=vec_last(&yyvsp[0]->declTail);
  AppendToDecls(yyvsp[-2],b2.basetype,b2.finalType,b2.name,b2.dft,b2.metaData);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 3136 "HolyC.tab.c"
    break;

  case 145:
#line 586 "HolyC.y"
                            {yyval=yyvsp[0];}
#line 3142 "HolyC.tab.c"
    break;

  case 146:
#line 587 "HolyC.y"
                              {
  AST *t=yyvsp[0];
  yyval=yyvsp[0];
}
#line 3151 "HolyC.tab.c"
    break;

  case 147:
#line 591 "HolyC.y"
                                          {
  AST *meta=yyvsp[-1]->declTail.data[0].metaData;
  meta=AppendToMetaData(meta,yyvsp[0]->metaData.data[0].name,yyvsp[0]->metaData.data[0].value);
  yyvsp[-1]->declTail.data[0].metaData=meta;
  yyval=yyvsp[-1];
}
#line 3162 "HolyC.tab.c"
    break;

  case 148:
#line 597 "HolyC.y"
                                    {
  DeclsFillInBasetype(yyvsp[-1]->type2,yyvsp[0]);
  yyval=yyvsp[0];
}
#line 3171 "HolyC.tab.c"
    break;

  case 149:
#line 601 "HolyC.y"
                                      {
 yyval=AppendToStmts(yyvsp[-2],yyvsp[-1]);
 ReleaseAST(yyvsp[0]);
}
#line 3180 "HolyC.tab.c"
    break;

  case 150:
#line 605 "HolyC.y"
                                         {
 yyval=AppendToStmts(yyvsp[-2],yyvsp[-1]);
 ReleaseAST(yyvsp[0]);
}
#line 3189 "HolyC.tab.c"
    break;

  case 151:
#line 609 "HolyC.y"
       {yyval=NULL;}
#line 3195 "HolyC.tab.c"
    break;

  case 152:
#line 610 "HolyC.y"
                        {
  CType *t=CreateClassForwardDecl(NULL,NULL);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[0]);
}
#line 3205 "HolyC.tab.c"
    break;

  case 153:
#line 615 "HolyC.y"
                        {
  CType *t=CreateUnionForwardDecl(NULL,NULL);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[0]);
}
#line 3215 "HolyC.tab.c"
    break;

  case 154:
#line 620 "HolyC.y"
                                {
  CType *t=CreateClassForwardDecl(NULL,yyvsp[0]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-1]);
}
#line 3225 "HolyC.tab.c"
    break;

  case 155:
#line 625 "HolyC.y"
                                {
  CType *t=CreateUnionForwardDecl(NULL,yyvsp[0]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-1]);
}
#line 3235 "HolyC.tab.c"
    break;

  case 156:
#line 630 "HolyC.y"
                                    {
  CType *t=CreateClassForwardDecl(NULL,yyvsp[0]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-1]);
}
#line 3245 "HolyC.tab.c"
    break;

  case 157:
#line 635 "HolyC.y"
                               {
  CType *t=CreateUnionForwardDecl(NULL,yyvsp[0]);
  yyval=CreateTypeNode(t);
}
#line 3254 "HolyC.tab.c"
    break;

  case 158:
#line 639 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 3260 "HolyC.tab.c"
    break;

  case 159:
#line 640 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 3266 "HolyC.tab.c"
    break;

  case 160:
#line 641 "HolyC.y"
                                                 {
  CType *t=*map_get(&Compiler.types,yyvsp[0]->name);
  InheritFromType(yyvsp[-2]->type2,t);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 3277 "HolyC.tab.c"
    break;

  case 161:
#line 647 "HolyC.y"
                                                 {
  CType *t=*map_get(&Compiler.types,yyvsp[0]->name);
  InheritFromType(yyvsp[-2]->type2,t);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 3288 "HolyC.tab.c"
    break;

  case 162:
#line 653 "HolyC.y"
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
#line 3308 "HolyC.tab.c"
    break;

  case 163:
#line 668 "HolyC.y"
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
#line 3325 "HolyC.tab.c"
    break;

  case 164:
#line 684 "HolyC.y"
                                                   {
  yyval=CreateTry(yyvsp[-2],yyvsp[0]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-1]);
}
#line 3335 "HolyC.tab.c"
    break;

  case 165:
#line 692 "HolyC.y"
                   {
  CType *t =CreatePrimType(TYPE_I64);
  yyval=SOT(AppendToDecls(NULL,t,t,yyvsp[0],NULL,NULL),yyvsp[0]);
}
#line 3344 "HolyC.tab.c"
    break;

  case 166:
#line 696 "HolyC.y"
                            {
  CType *t =CreatePrimType(TYPE_I64);
  CType *ptr =CreateMultiLvlPtr(t,1);
  yyval=SOT(AppendToDecls(NULL,t,ptr,yyvsp[0],NULL,NULL),yyvsp[0]);
  ReleaseAST(yyvsp[-1]);
}
#line 3355 "HolyC.tab.c"
    break;

  case 167:
#line 702 "HolyC.y"
                                     {
  CType *t =CreatePrimType(TYPE_I64);
  CType *ptr =CreateMultiLvlPtr(t,2);
  yyval=SOT(AppendToDecls(NULL,t,ptr,yyvsp[0],NULL,NULL),yyvsp[0]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
}
#line 3367 "HolyC.tab.c"
    break;

  case 168:
#line 709 "HolyC.y"
                                              {
  CType *t =CreatePrimType(TYPE_I64);
  CType *ptr =CreateMultiLvlPtr(t,3);
  yyval=SOT(AppendToDecls(NULL,t,ptr,yyvsp[0],NULL,NULL),yyvsp[0]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
}
#line 3380 "HolyC.tab.c"
    break;

  case 170:
#line 718 "HolyC.y"
            {
    yyval=CreateI64(0);
}
#line 3388 "HolyC.tab.c"
    break;

  case 171:
#line 721 "HolyC.y"
                     {
  yyval=CreateI64(1);
  ReleaseAST(yyvsp[0]);
}
#line 3397 "HolyC.tab.c"
    break;

  case 172:
#line 725 "HolyC.y"
                              {
  yyval=CreateI64(2);
  ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 3406 "HolyC.tab.c"
    break;

  case 173:
#line 729 "HolyC.y"
                                       {
  yyval=CreateI64(3);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 3415 "HolyC.tab.c"
    break;

  case 174:
#line 733 "HolyC.y"
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
#line 3432 "HolyC.tab.c"
    break;

  case 175:
#line 745 "HolyC.y"
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
#line 3450 "HolyC.tab.c"
    break;

  case 176:
#line 758 "HolyC.y"
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
#line 3469 "HolyC.tab.c"
    break;

  case 177:
#line 772 "HolyC.y"
                                                                                     {
  assert(yyvsp[-3]->type==__AST_DECL_TAILS);
  CType *bt=vec_last(&yyvsp[-3]->declTail).finalType;
  vec_last(&yyvsp[-3]->declTail).finalType=CreateArrayType(bt,yyvsp[-1]);
  yyval=yyvsp[-3];
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3482 "HolyC.tab.c"
    break;

  case 178:
#line 780 "HolyC.y"
                                                                     {
  assert(yyvsp[-2]->type==__AST_DECL_TAILS);
  CType *bt=vec_last(&yyvsp[-2]->declTail).finalType;
  vec_last(&yyvsp[-2]->declTail).finalType=CreateArrayType(bt,NULL);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3495 "HolyC.tab.c"
    break;

  case 179:
#line 789 "HolyC.y"
                                                       {
  assert(yyvsp[-2]->type==__AST_DECL_TAILS);
  vec_last(&yyvsp[-2]->declTail).dft=yyvsp[0];
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 3506 "HolyC.tab.c"
    break;

  case 180:
#line 795 "HolyC.y"
                                                         {
  assert(yyvsp[-2]->type==__AST_DECL_TAILS);
  vec_last(&yyvsp[-2]->declTail).dft=yyvsp[0];
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 3517 "HolyC.tab.c"
    break;

  case 181:
#line 802 "HolyC.y"
                                                {
  DeclsFillInBasetype(yyvsp[-1]->type2,yyvsp[0]);
  yyval=yyvsp[0];
}
#line 3526 "HolyC.tab.c"
    break;

  case 182:
#line 806 "HolyC.y"
                                                    {
  DeclsFillInBasetype(yyvsp[-1]->type2,yyvsp[0]);
  yyval=yyvsp[0];
}
#line 3535 "HolyC.tab.c"
    break;

  case 183:
#line 811 "HolyC.y"
                                                            {
  assert(yyvsp[0]->type==__AST_DECL_TAILS);
  CDeclTail b2=vec_last(&yyvsp[0]->declTail);
  AppendToDecls(yyvsp[-2],b2.basetype,b2.finalType,b2.name,b2.dft,NULL);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 3547 "HolyC.tab.c"
    break;

  case 186:
#line 821 "HolyC.y"
                                                 {
  DeclsFillInBasetype(yyvsp[-1]->type2,yyvsp[0]);
  yyval=yyvsp[0];
}
#line 3556 "HolyC.tab.c"
    break;

  case 188:
#line 827 "HolyC.y"
                                                   {
  assert(yyvsp[0]->type==__AST_DECL_TAILS);
  CDeclTail b2=vec_last(&yyvsp[0]->declTail);
  yyval=AppendToDecls(yyvsp[-2],b2.basetype,b2.finalType,b2.name,b2.dft,NULL);
  ReleaseAST(yyvsp[-1]);
}
#line 3567 "HolyC.tab.c"
    break;

  case 189:
#line 833 "HolyC.y"
          {yyval=NULL;}
#line 3573 "HolyC.tab.c"
    break;

  case 190:
#line 837 "HolyC.y"
                     {yyval=CreateExternLinkage(NULL);ReleaseAST(yyvsp[0]);}
#line 3579 "HolyC.tab.c"
    break;

  case 191:
#line 838 "HolyC.y"
                     {yyval=CreateImportLinkage(NULL);ReleaseAST(yyvsp[0]);}
#line 3585 "HolyC.tab.c"
    break;

  case 192:
#line 839 "HolyC.y"
                     {yyval=NULL;}
#line 3591 "HolyC.tab.c"
    break;

  case 193:
#line 840 "HolyC.y"
                     {yyval=CreateStaticLinkage();ReleaseAST(yyvsp[0]);}
#line 3597 "HolyC.tab.c"
    break;

  case 194:
#line 841 "HolyC.y"
                              {yyval=CreateExternLinkage(yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3603 "HolyC.tab.c"
    break;

  case 195:
#line 842 "HolyC.y"
                              {yyval=CreateImportLinkage(yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3609 "HolyC.tab.c"
    break;

  case 196:
#line 843 "HolyC.y"
                                  {yyval=CreateExternLinkage(yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3615 "HolyC.tab.c"
    break;

  case 197:
#line 844 "HolyC.y"
                                  {yyval=CreateImportLinkage(yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3621 "HolyC.tab.c"
    break;

  case 198:
#line 848 "HolyC.y"
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
#line 3639 "HolyC.tab.c"
    break;

  case 199:
#line 861 "HolyC.y"
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
#line 3658 "HolyC.tab.c"
    break;

  case 200:
#line 875 "HolyC.y"
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
#line 3676 "HolyC.tab.c"
    break;

  case 201:
#line 888 "HolyC.y"
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
#line 3695 "HolyC.tab.c"
    break;

  case 202:
#line 902 "HolyC.y"
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
#line 3716 "HolyC.tab.c"
    break;

  case 203:
#line 918 "HolyC.y"
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
#line 3735 "HolyC.tab.c"
    break;

  case 204:
#line 932 "HolyC.y"
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
#line 3754 "HolyC.tab.c"
    break;

  case 205:
#line 946 "HolyC.y"
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
#line 3774 "HolyC.tab.c"
    break;

  case 206:
#line 961 "HolyC.y"
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
#line 3792 "HolyC.tab.c"
    break;

  case 207:
#line 974 "HolyC.y"
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
#line 3811 "HolyC.tab.c"
    break;

  case 208:
#line 988 "HolyC.y"
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
#line 3832 "HolyC.tab.c"
    break;

  case 209:
#line 1004 "HolyC.y"
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
#line 3850 "HolyC.tab.c"
    break;

  case 210:
#line 1022 "HolyC.y"
                    {yyval=yyvsp[0];}
#line 3856 "HolyC.tab.c"
    break;

  case 211:
#line 1023 "HolyC.y"
                    {yyval=yyvsp[0];}
#line 3862 "HolyC.tab.c"
    break;

  case 213:
#line 1026 "HolyC.y"
          {yyval=NULL;}
#line 3868 "HolyC.tab.c"
    break;

  case 214:
#line 1029 "HolyC.y"
                                                                           {
  yyval=SOT(CreateWhile(yyvsp[-1],NULL),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3878 "HolyC.tab.c"
    break;

  case 215:
#line 1035 "HolyC.y"
                                                                                                                         {
  yyval=SOT(CreateFor(yyvsp[-5],yyvsp[-3],yyvsp[-1],NULL),yyvsp[-7]);
  ReleaseAST(yyvsp[-6]);
  ReleaseAST(yyvsp[-4]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3890 "HolyC.tab.c"
    break;

  case 216:
#line 1043 "HolyC.y"
                                                              {
  yyval=SOT(CreateIf(yyvsp[-1],NULL,NULL),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3900 "HolyC.tab.c"
    break;

  case 217:
#line 1049 "HolyC.y"
                            {
  assert(yyvsp[-1]->type==AST_IF);
  yyval=yyvsp[-1];
  yyvsp[-1]->ifStmt.body=yyvsp[0];
}
#line 3910 "HolyC.tab.c"
    break;

  case 218:
#line 1054 "HolyC.y"
                                              {
  assert(yyvsp[-3]->type==AST_IF);
  yyval=yyvsp[-3];
  yyvsp[-3]->ifStmt.body=yyvsp[-2];
  yyvsp[-3]->ifStmt.elseBody=yyvsp[0];
  ReleaseAST(yyvsp[-1]);
}
#line 3922 "HolyC.tab.c"
    break;

  case 219:
#line 1061 "HolyC.y"
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
#line 3937 "HolyC.tab.c"
    break;

  case 221:
#line 1073 "HolyC.y"
                                              {
  assert(yyvsp[-3]->type==AST_IF);
  yyval=yyvsp[-3];
  yyvsp[-3]->ifStmt.body=yyvsp[-2];
  yyvsp[-3]->ifStmt.elseBody=yyvsp[0];
  ReleaseAST(yyvsp[-1]);
}
#line 3949 "HolyC.tab.c"
    break;

  case 222:
#line 1080 "HolyC.y"
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
#line 3964 "HolyC.tab.c"
    break;

  case 223:
#line 1091 "HolyC.y"
                                                                                     {
  yyval=SOT(CreateDo(yyvsp[-1],yyvsp[-4]),yyvsp[-5]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3975 "HolyC.tab.c"
    break;

  case 224:
#line 1097 "HolyC.y"
                                                                 {
  yyval=SOT(CreateSubswitch(yyvsp[-2]),yyvsp[-4]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3986 "HolyC.tab.c"
    break;

  case 225:
#line 1104 "HolyC.y"
                       {yyval=yyvsp[0];}
#line 3992 "HolyC.tab.c"
    break;

  case 226:
#line 1105 "HolyC.y"
                                         {
  yyval=SOT(CreateDefault(),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4001 "HolyC.tab.c"
    break;

  case 227:
#line 1109 "HolyC.y"
                                              {
  yyval=SOT(CreateCase(yyvsp[-1],NULL),yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4010 "HolyC.tab.c"
    break;

  case 228:
#line 1113 "HolyC.y"
                                      {
  yyval=SOT(CreateCase(NULL,NULL),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4019 "HolyC.tab.c"
    break;

  case 229:
#line 1117 "HolyC.y"
                                                                       {
  yyval=SOT(CreateCase(yyvsp[-3],yyvsp[-1]),yyvsp[-4]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);

}
#line 4030 "HolyC.tab.c"
    break;

  case 230:
#line 1123 "HolyC.y"
                              {yyval=yyvsp[0];}
#line 4036 "HolyC.tab.c"
    break;

  case 231:
#line 1124 "HolyC.y"
                                             {yyval=AppendToStmts(yyvsp[-1],yyvsp[0]);}
#line 4042 "HolyC.tab.c"
    break;

  case 232:
#line 1125 "HolyC.y"
                                {yyval=yyvsp[0];}
#line 4048 "HolyC.tab.c"
    break;

  case 233:
#line 1126 "HolyC.y"
                                                                                                                      {
  yyval=SOT(CreateSwitch(yyvsp[-4],yyvsp[-1]),yyvsp[-6]);
  ReleaseAST(yyvsp[-5]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4060 "HolyC.tab.c"
    break;

  case 234:
#line 1133 "HolyC.y"
                                                                                                       {
  yyval=SOT(CreateSwitch(yyvsp[-3],NULL),yyvsp[-5]);
  ReleaseAST(yyvsp[-4]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4072 "HolyC.tab.c"
    break;

  case 236:
#line 1141 "HolyC.y"
                            {
  AST *r=TD_MALLOC(sizeof(AST));
  r->type=AST_ASM_BLK;
  vec_init(&r->stmts);
  vec_push(&r->stmts,yyvsp[-1]);
  yyval=r;
  ReleaseAST(yyvsp[0]);
}
#line 4085 "HolyC.tab.c"
    break;

  case 237:
#line 1149 "HolyC.y"
                                          {
  vec_push(&yyvsp[-2]->stmts,yyvsp[-1]);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[0]);
}
#line 4095 "HolyC.tab.c"
    break;

  case 238:
#line 1154 "HolyC.y"
                                 {
    yyval=yyvsp[-1];
    ReleaseAST(yyvsp[0]);
}
#line 4104 "HolyC.tab.c"
    break;

  case 239:
#line 1158 "HolyC.y"
                                 {
    yyval=yyvsp[-1];
    ReleaseAST(yyvsp[0]);
}
#line 4113 "HolyC.tab.c"
    break;

  case 242:
#line 1164 "HolyC.y"
                                       {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[0]);
}
#line 4122 "HolyC.tab.c"
    break;

  case 243:
#line 1168 "HolyC.y"
                                                  {
  yyval=ApplyLinkageToDecls(yyvsp[-2],yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4131 "HolyC.tab.c"
    break;

  case 245:
#line 1173 "HolyC.y"
                                         {
  yyval=SOT(CreateGoto(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4140 "HolyC.tab.c"
    break;

  case 246:
#line 1177 "HolyC.y"
                                   {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[0]);
}
#line 4149 "HolyC.tab.c"
    break;

  case 247:
#line 1181 "HolyC.y"
                                       {
  yyval=yyvsp[-1];
  if(yyvsp[-1]->type==AST_STRING) {
    yyval=SOT(CreatePrint(yyvsp[-1]->string),yyvsp[-1]);
  }
  ReleaseAST(yyvsp[0]);
}
#line 4161 "HolyC.tab.c"
    break;

  case 248:
#line 1188 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 4167 "HolyC.tab.c"
    break;

  case 249:
#line 1189 "HolyC.y"
                         {yyval=yyvsp[0];}
#line 4173 "HolyC.tab.c"
    break;

  case 250:
#line 1190 "HolyC.y"
                                  {
  yyval=SOT(CreateBreak(),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4182 "HolyC.tab.c"
    break;

  case 251:
#line 1194 "HolyC.y"
                         {
  yyval=CreateNop();
  ReleaseAST(yyvsp[0]);
}
#line 4191 "HolyC.tab.c"
    break;

  case 252:
#line 1198 "HolyC.y"
                                           {
  yyval=SOT(CreateReturn(yyvsp[-1]),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4200 "HolyC.tab.c"
    break;

  case 253:
#line 1202 "HolyC.y"
                                 {
  yyval=SOT(CreateReturn(NULL),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4209 "HolyC.tab.c"
    break;

  case 254:
#line 1206 "HolyC.y"
                                  {
  yyval=SOT(CreateLabel(yyvsp[-1]),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4218 "HolyC.tab.c"
    break;

  case 255:
#line 1210 "HolyC.y"
                                      {yyval=yyvsp[-1];ReleaseAST(yyvsp[0]);}
#line 4224 "HolyC.tab.c"
    break;

  case 256:
#line 1211 "HolyC.y"
                                                        {
  CType *t=CreateUnionForwardDecl(yyvsp[-3],yyvsp[-1]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4235 "HolyC.tab.c"
    break;

  case 257:
#line 1217 "HolyC.y"
                                                        {
  CType *t=CreateClassForwardDecl(yyvsp[-3],yyvsp[-1]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4246 "HolyC.tab.c"
    break;

  case 258:
#line 1223 "HolyC.y"
                    {yyval=yyvsp[0];}
#line 4252 "HolyC.tab.c"
    break;

  case 259:
#line 1224 "HolyC.y"
                             {yyval=AppendToStmts(yyvsp[-1],yyvsp[0]);}
#line 4258 "HolyC.tab.c"
    break;

  case 260:
#line 1225 "HolyC.y"
                   {YYABORT;}
#line 4264 "HolyC.tab.c"
    break;

  case 261:
#line 1226 "HolyC.y"
                                                    {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4274 "HolyC.tab.c"
    break;

  case 262:
#line 1231 "HolyC.y"
                                           {
  yyval=CreateNop();
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4284 "HolyC.tab.c"
    break;

  case 263:
#line 1236 "HolyC.y"
                {yyval=NULL;}
#line 4290 "HolyC.tab.c"
    break;

  case 264:
#line 1237 "HolyC.y"
                                        {
  if(!Compiler.AOTMode) {
    RunStatement(yyvsp[0]);
    ReleaseAST(yyvsp[0]);
  } else {
    COldFuncState old=CreateCompilerState();
    vec_CVariable_t empty;
    vec_init(&empty);
    ReleaseFunction(CompileAST(NULL,yyvsp[0],empty,C_AST_FRAME_OFF_DFT,C_AST_F_NO_COMPILE));
    vec_push(&Compiler.AOTGlobalStmts,yyvsp[0]);
    RestoreCompilerState(old);
  }
  yyval=NULL;
}
#line 4309 "HolyC.tab.c"
    break;

  case 265:
#line 1251 "HolyC.y"
                                {
  yyval=NULL;
}
#line 4317 "HolyC.tab.c"
    break;

  case 266:
#line 1255 "HolyC.y"
                             {
  RunStatement(yyvsp[0]);
  YYACCEPT;
  yyval=NULL;
}
#line 4327 "HolyC.tab.c"
    break;

  case 267:
#line 1260 "HolyC.y"
                                   {
  RunPtr(CurFuncInfo,yyvsp[0],CurFramePtr);
  YYACCEPT;
  yyval=NULL;
}
#line 4337 "HolyC.tab.c"
    break;

  case 268:
#line 1265 "HolyC.y"
                                     {
  RunPtr(NULL,yyvsp[0],NULL);
  YYACCEPT;
  yyval=NULL;
}
#line 4347 "HolyC.tab.c"
    break;

  case 269:
#line 1271 "HolyC.y"
                                  {
  RunPtr(CurFuncInfo,yyvsp[0],CurFramePtr);
  YYACCEPT;
  yyval=NULL;
}
#line 4357 "HolyC.tab.c"
    break;

  case 270:
#line 1280 "HolyC.y"
                             {
  yyval=SOT(yyvsp[0],yyvsp[-1]);
  char buffer[256];
  sprintf(buffer,"@@%s",yyval->name);
  TD_FREE(yyval->name);
  yyval->name=strdup(buffer);
  ReleaseAST(yyvsp[-1]);
}
#line 4370 "HolyC.tab.c"
    break;

  case 271:
#line 1288 "HolyC.y"
                               {
 //Make a name token out of $n
  char buffer[256];
  sprintf(buffer,"@@%li",yyvsp[0]->integer);
  AST *n=TD_MALLOC(sizeof(AST));
  n->type=AST_NAME,n->name=strdup(buffer);
  yyval=n;
  ReleaseAST(yyvsp[-1]);
}
#line 4384 "HolyC.tab.c"
    break;

  case 272:
#line 1297 "HolyC.y"
               {yyval=SLE(yyvsp[0]);}
#line 4390 "HolyC.tab.c"
    break;

  case 273:
#line 1298 "HolyC.y"
                         {yyval=JoinStrings(yyvsp[-1],yyvsp[0]);}
#line 4396 "HolyC.tab.c"
    break;

  case 274:
#line 1299 "HolyC.y"
                 {yyval=SLE(yyvsp[0]);}
#line 4402 "HolyC.tab.c"
    break;

  case 275:
#line 1300 "HolyC.y"
               {yyval=SLE(yyvsp[0]);}
#line 4408 "HolyC.tab.c"
    break;

  case 276:
#line 1301 "HolyC.y"
                {yyval=SLE(yyvsp[0]);}
#line 4414 "HolyC.tab.c"
    break;

  case 277:
#line 1302 "HolyC.y"
               {yyval=yyvsp[0];}
#line 4420 "HolyC.tab.c"
    break;

  case 278:
#line 1303 "HolyC.y"
                     {
  yyval=SLE(yyvsp[0]);
}
#line 4428 "HolyC.tab.c"
    break;

  case 279:
#line 1306 "HolyC.y"
                {
  yyval=SLE(yyvsp[0]);
}
#line 4436 "HolyC.tab.c"
    break;

  case 281:
#line 1310 "HolyC.y"
                                                          {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 4445 "HolyC.tab.c"
    break;

  case 283:
#line 1315 "HolyC.y"
                                     {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4454 "HolyC.tab.c"
    break;

  case 284:
#line 1319 "HolyC.y"
                                        {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4463 "HolyC.tab.c"
    break;

  case 285:
#line 1323 "HolyC.y"
                                    {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4472 "HolyC.tab.c"
    break;

  case 286:
#line 1327 "HolyC.y"
                                      {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4481 "HolyC.tab.c"
    break;

  case 287:
#line 1331 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[-1],AST_POST_DEC),yyvsp[0]);
  ReleaseAST(yyvsp[0]);
}
#line 4490 "HolyC.tab.c"
    break;

  case 288:
#line 1335 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[-1],AST_POST_INC),yyvsp[0]);
  ReleaseAST(yyvsp[0]);
}
#line 4499 "HolyC.tab.c"
    break;

  case 289:
#line 1339 "HolyC.y"
                                                                  {
  yyval=SOT(CreateFuncCall(yyvsp[-3],yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 4508 "HolyC.tab.c"
    break;

  case 290:
#line 1343 "HolyC.y"
                                                                   {
  yyval=CreateExplicitTypecast(yyvsp[-3],yyvsp[-1]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 4517 "HolyC.tab.c"
    break;

  case 292:
#line 1348 "HolyC.y"
                             {
  yyval=SOT(CreateSizeof(yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4526 "HolyC.tab.c"
    break;

  case 293:
#line 1352 "HolyC.y"
                                    {
  yyval=SOT(CreateSizeof(yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4535 "HolyC.tab.c"
    break;

  case 294:
#line 1356 "HolyC.y"
                                                                     {
  yyval=SOT(CreateSizeof(yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4546 "HolyC.tab.c"
    break;

  case 295:
#line 1362 "HolyC.y"
                                                                      {
  yyval=SOT(CreateSizeof(yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4557 "HolyC.tab.c"
    break;

  case 296:
#line 1368 "HolyC.y"
                             {
  yyval=SOT(CreateUnop(yyvsp[0],AST_ADDROF),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4566 "HolyC.tab.c"
    break;

  case 297:
#line 1372 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[0],AST_DERREF),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4575 "HolyC.tab.c"
    break;

  case 298:
#line 1377 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[0],AST_PRE_DEC),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4584 "HolyC.tab.c"
    break;

  case 299:
#line 1381 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[0],AST_PRE_INC),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4593 "HolyC.tab.c"
    break;

  case 300:
#line 1385 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[0],AST_POS),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4602 "HolyC.tab.c"
    break;

  case 301:
#line 1389 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[0],AST_NEG),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4611 "HolyC.tab.c"
    break;

  case 302:
#line 1393 "HolyC.y"
                             {
  yyval=SOT(CreateUnop(yyvsp[0],AST_LNOT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4620 "HolyC.tab.c"
    break;

  case 303:
#line 1397 "HolyC.y"
                             {
  yyval=SOT(CreateUnop(yyvsp[0],AST_BNOT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4629 "HolyC.tab.c"
    break;

  case 305:
#line 1404 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_POW),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4638 "HolyC.tab.c"
    break;

  case 306:
#line 1408 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SHL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4647 "HolyC.tab.c"
    break;

  case 307:
#line 1412 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SHR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4656 "HolyC.tab.c"
    break;

  case 309:
#line 1418 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_MUL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4665 "HolyC.tab.c"
    break;

  case 310:
#line 1422 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_DIV),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4674 "HolyC.tab.c"
    break;

  case 311:
#line 1426 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_MOD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4683 "HolyC.tab.c"
    break;

  case 313:
#line 1432 "HolyC.y"
                                       {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4692 "HolyC.tab.c"
    break;

  case 315:
#line 1438 "HolyC.y"
                                       {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4701 "HolyC.tab.c"
    break;

  case 317:
#line 1444 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4710 "HolyC.tab.c"
    break;

  case 319:
#line 1450 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ADD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4719 "HolyC.tab.c"
    break;

  case 320:
#line 1454 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SUB),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4728 "HolyC.tab.c"
    break;

  case 322:
#line 1460 "HolyC.y"
                                     {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_LT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4737 "HolyC.tab.c"
    break;

  case 323:
#line 1464 "HolyC.y"
                                     {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_GT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4746 "HolyC.tab.c"
    break;

  case 324:
#line 1468 "HolyC.y"
                                     {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_LE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4755 "HolyC.tab.c"
    break;

  case 325:
#line 1472 "HolyC.y"
                                     {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_GE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4764 "HolyC.tab.c"
    break;

  case 327:
#line 1478 "HolyC.y"
                                        {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_EQ),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4773 "HolyC.tab.c"
    break;

  case 328:
#line 1482 "HolyC.y"
                                        {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_NE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4782 "HolyC.tab.c"
    break;

  case 330:
#line 1488 "HolyC.y"
                                          {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4791 "HolyC.tab.c"
    break;

  case 332:
#line 1494 "HolyC.y"
                                          {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4800 "HolyC.tab.c"
    break;

  case 334:
#line 1500 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4809 "HolyC.tab.c"
    break;

  case 336:
#line 1506 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4818 "HolyC.tab.c"
    break;

  case 337:
#line 1510 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SHL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4827 "HolyC.tab.c"
    break;

  case 338:
#line 1514 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SHR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4836 "HolyC.tab.c"
    break;

  case 339:
#line 1518 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_MUL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4845 "HolyC.tab.c"
    break;

  case 340:
#line 1522 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_DIV),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4854 "HolyC.tab.c"
    break;

  case 341:
#line 1526 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_MOD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4863 "HolyC.tab.c"
    break;

  case 342:
#line 1530 "HolyC.y"
                                             {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4872 "HolyC.tab.c"
    break;

  case 343:
#line 1534 "HolyC.y"
                                             {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4881 "HolyC.tab.c"
    break;

  case 344:
#line 1538 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4890 "HolyC.tab.c"
    break;

  case 345:
#line 1542 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_ADD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4899 "HolyC.tab.c"
    break;

  case 346:
#line 1546 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SUB),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4908 "HolyC.tab.c"
    break;


#line 4912 "HolyC.tab.c"

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
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

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
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
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

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
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
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

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


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
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
                  yystos[+*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 1552 "HolyC.y"

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
static int64_t exprret;
static void __EvalExprNoComma(CFuncInfo *dummy1,AST *node,void *fp) {
    CType *bt=BaseType(AssignTypeToNode(node));
    exprret=0;
    if(!bt) return;
    union {
        double f;
        int64_t i;
    } ret;
    if(!HCSetJmp(EnterTry()))
    {
        if(bt->type==TYPE_F64) {
            ret.f=EvaluateF64(node);
        } else {
            ret.i=EvaluateInt(node,0);
        }
        PopTryFrame();
    }
    ReleaseType(bt);
    exprret=ret.i;
}
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
  CFunction *f=CompileAST(NULL,retn,args,C_AST_FRAME_OFF_DFT,0);
  int ret;
  if(!HCSetJmp(EnterTry())) {
    if(IsF64(rtype)) {
      ret=0!=((double(*)())f->funcptr)();
    } else if(IsIntegerType(rtype)) {
      ret=0!=((int64_t(*)())f->funcptr)();
    }
    PopTryFrame();
  }
  ReleaseFunction(f);
  vec_deinit(&args);
  RestoreCompilerState(old);
  __IsTruePassed=ret;
}
int EvalConditionForPreproc() {
  RunPtr=__IsTrue;
  __IsTruePassed=0;
  yyparse();
  return __IsTruePassed;
}
void
yyerror (char const *s)
{
  RaiseError(Lexer.lastToken,"Parsing stoped here.");
}
void DebugEvalExpr(void *frameptr,CFuncInfo *info,char *text) {
  CLexer old=ForkLexer(0);
  #ifndef BOOTSTRAPED
    mrope_append_text(Lexer.source,strdup(text));
  #else
    void(*inc)(void*cc,char *fn,char *src,int64_t act_f)=(void*)GetVariable("LexIncludeStr")->func->funcptr;
    inc(Lexer.HCLexer,"(nofile)",strdup(text),0);
    Lexer.isEvalExpr=1;
  #endif
  CurFuncInfo=info;
  CurFramePtr=frameptr;
  RunPtr=&EvalDebugExpr;
  if(!HCSetJmp(EnterTry())) {
    yyparse();
    printf("\n");
    PopTryFrame();
  }
  Lexer=old;
}
int64_t EvalExprNoComma(char *text,char **en) {
    CLexer old=ForkLexer(0);
    #ifndef BOOTSTRAPED
        mrope_append_text(Lexer.source,strdup(text));
    #else
        void(*inc)(void*cc,char *fn,char *src,int64_t act_f)=(void*)GetVariable("LexIncludeStr")->func->funcptr;
        inc(Lexer.HCLexer,"(nofile)",text,0);
        Lexer.isEvalExpr=1;
    #endif
    RunPtr=__EvalExprNoComma;
    yyparse();
    #ifndef BOOTSTRAPED
    if(en) *en=text+Lexer.cursor_pos;
    #endif
    Lexer=old;
    return exprret;
}
