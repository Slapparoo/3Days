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
#define YYFINAL  93
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1709

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  102
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  93
/* YYNRULES -- Number of rules.  */
#define YYNRULES  345
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  619

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
    1280,  1288,  1297,  1298,  1299,  1300,  1301,  1304,  1307,  1308,
    1312,  1313,  1317,  1321,  1325,  1329,  1333,  1337,  1341,  1345,
    1346,  1350,  1354,  1360,  1366,  1370,  1375,  1379,  1383,  1387,
    1391,  1395,  1401,  1402,  1406,  1410,  1415,  1416,  1420,  1424,
    1429,  1430,  1435,  1436,  1441,  1442,  1447,  1448,  1452,  1457,
    1458,  1462,  1466,  1470,  1475,  1476,  1480,  1485,  1486,  1491,
    1492,  1497,  1498,  1503,  1504,  1508,  1512,  1516,  1520,  1524,
    1528,  1532,  1536,  1540,  1544,  1548
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
  "global_stmts", "global_stmt", "_expr0", "_expr1", "_expr2", "_expr3",
  "_expr4", "_expr4_5", "_expr5", "_expr6", "_expr7", "_expr8", "_expr9",
  "_expr10", "_expr11", "_expr12", "_expr13", "_expr14", "_expr", YY_NULLPTR
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
     416,  1595,   -27,  1595,  1595,  1155,    38,  -525,  -525,  -525,
      96,  1595,  1595,  1595,  1595,  1595,  1595,  -525,  1595,  1595,
    1595,  -525,  -525,    49,    21,  -525,  -525,    87,  -525,   270,
     494,    29,    46,   144,   163,   100,   251,   120,   124,   166,
     315,  -525,  1063,  -525,  -525,     1,   151,   -27,   109,  -525,
     171,  1155,   182,   193,     2,  -525,  -525,     5,  -525,  -525,
       7,    13,   156,  1344,   192,   198,    49,   252,   187,   206,
    -525,     8,    12,   204,   231,   132,   161,  -525,  -525,  -525,
     255,   168,  -525,  -525,  1155,  1155,  -525,  -525,   257,  -525,
    -525,   216,  -525,  -525,   424,  -525,  -525,   308,   240,   279,
     302,  -525,  -525,  -525,  -525,  -525,  -525,     3,  -525,  -525,
     319,   333,  -525,  -525,   344,   347,   424,  -525,  -525,  1595,
    1595,  1595,  1595,  1595,  1595,  1595,  1595,  1595,  1595,  1595,
    1595,  1595,  1595,  1595,  1595,  1595,  1595,  1595,  1595,  1595,
    1595,  1595,  1595,  1595,  1595,  1595,  1595,  1595,  1595,  1595,
    1595,  -525,  -525,   603,  1595,  -525,   358,  -525,  1595,   356,
    1595,  1595,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,
    -525,  -525,    67,   371,  1595,  -525,  -525,  -525,   108,  1615,
    1615,  1615,  1615,  1615,  1615,  -525,  1595,  1615,  1615,  -525,
      19,  -525,   199,  -525,    24,  -525,  -525,   369,   426,  -525,
    -525,   111,  -525,   517,   518,   453,   475,   454,   316,   482,
     332,   468,   473,   486,   415,   466,   137,  -525,  -525,    28,
    -525,  -525,  -525,  -525,   484,    17,   488,  -525,   521,   470,
     476,  -525,  -525,  -525,  -525,  -525,    37,    52,    27,   513,
    -525,  -525,  -525,   515,  -525,   519,    90,   210,  -525,  -525,
    -525,  -525,  -525,  -525,  -525,   223,  -525,    25,    76,  -525,
    -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,
    -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,   315,
     315,   315,   315,   315,   315,   315,   315,   315,   315,   315,
    -525,  -525,  -525,  -525,   -27,   228,   522,   232,   151,   523,
    -525,  -525,   276,   424,  -525,   308,   434,  -525,  -525,  -525,
    -525,  -525,  -525,   296,  -525,  -525,  -525,  -525,   456,   527,
     530,   531,   520,  -525,   466,  -525,  -525,  1364,   252,   535,
     536,   424,  -525,  -525,  1615,  1615,  1615,  1615,  1615,  1615,
    1615,  1615,  1615,  1615,  1615,  1615,  1615,  1615,  1615,  1615,
    1615,  1615,  1615,  1615,  1615,  1615,  1615,  1615,  1615,  1615,
    1615,  1615,  1615,  1615,  1615,  -525,   -35,  -525,  -525,   537,
      26,  1595,  1595,  1595,  1595,   586,  -525,  -525,   235,    91,
    -525,   211,  1421,  1441,   576,    99,  -525,  -525,   150,   164,
     538,   540,   544,  -525,  1155,  -525,  -525,  -525,  1595,  -525,
     116,  -525,  -525,  -525,  -525,  1595,  -525,  1595,   539,   130,
     338,  -525,  1498,  1595,  -525,  -525,  -525,  1364,  -525,   369,
    -525,  -525,   339,   -14,  -525,  -525,  -525,  -525,  -525,  -525,
    -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,
    -525,  -525,  -525,  -525,   415,   415,   415,   415,   415,   415,
     415,   415,   415,   415,   415,  -525,  -525,  -525,   112,   533,
     541,   125,  -525,   135,   158,   172,   566,  -525,  -525,   558,
    -525,   563,    27,  -525,   340,  1247,  -525,  -525,  -525,   175,
     101,  -525,  -525,  -525,    16,   560,  -525,  -525,  -525,   234,
    -525,  -525,  -525,   142,  -525,   341,   561,   695,  -525,  -525,
     602,   604,  -525,  -525,  -525,  -525,  -525,   568,  -525,  -525,
    -525,  1595,  -525,  -525,  -525,  -525,   618,  -525,   303,    17,
    -525,   246,   337,   564,  -525,    70,  -525,  -525,   114,   574,
    -525,   584,  -525,   440,  -525,   579,   351,   580,  -525,  -525,
    1595,  -525,  1518,   567,   569,  -525,  -525,  -525,   787,   542,
     626,  -525,  -525,  -525,  -525,  -525,   583,  -525,  -525,  -525,
    -525,  1267,  -525,   585,   587,   590,    99,  1595,  -525,   359,
     253,   363,  -525,   591,  -525,   479,   971,  -525,  -525,  -525,
    -525,  -525,   370,  -525,  -525,  -525,   592,   594,   190,  -525,
    -525,  -525,  -525,   595,  -525,  -525,  -525,  -525,  1595,   879,
    -525,  -525,   597,   190,   352,   375,   571,   575,   190,   394,
     190,  -525,  -525,  -525,  -525,  -525,   395,  -525,  -525
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     263,     0,     0,     0,     0,   265,     0,    47,    46,    45,
       0,     0,     0,     0,     0,     0,     0,    49,     0,     0,
       0,    50,    48,     0,     0,    55,    57,    72,    85,    89,
      93,    95,    97,    99,   102,   107,   110,   112,   114,   116,
     128,   268,     0,   266,   130,     0,   269,     0,    50,   251,
       0,     0,     0,     0,     0,   190,   191,     0,   193,   192,
     152,   153,     0,     0,     0,     0,   131,    21,     0,     0,
     241,     0,   170,   158,   159,     0,     0,   132,   133,   235,
       0,     0,   244,   264,     0,     0,   210,   211,     0,   248,
     220,   240,   249,     1,     0,    73,   131,    74,    70,     0,
       0,    83,    84,    78,    77,    81,    82,     0,    80,    79,
       0,     0,    44,    43,     0,     0,    51,    64,    63,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   262,   258,     0,     0,   267,     0,   254,     0,     0,
       0,   213,   194,   196,   195,   197,   154,   156,   155,   157,
     250,   253,     0,     0,     0,   274,   273,   272,     0,     0,
       0,     0,     0,     0,     0,   276,     0,     0,     0,   277,
       0,   275,     0,    14,     0,    11,    13,    19,    22,   278,
     280,   289,   302,   306,   310,   312,   314,   316,   319,   324,
     327,   329,   331,   333,   345,    15,     0,   236,   247,   171,
     165,   255,   135,   134,   169,   185,     0,   184,   186,     0,
       0,   238,   151,   239,   151,   242,   152,   153,   170,     0,
     219,   222,   217,   211,   246,     0,     0,     0,    71,    56,
      58,    59,    60,    61,    54,     0,    52,     0,     0,    86,
      87,    88,    90,    91,    92,    94,    96,    98,   100,   101,
     103,   104,   105,   106,   109,   108,   111,   113,   115,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     260,   261,   259,   129,     0,     0,     0,     0,   212,     0,
     252,   245,     0,     0,   290,   291,     0,   300,   301,   295,
     294,   298,   299,     0,   297,   296,     6,     2,     7,     0,
       0,     0,     0,    16,     0,   271,   270,     0,     0,     0,
       0,    51,   286,   285,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    12,     0,    37,    39,     0,
       0,     0,     0,     0,     0,     0,    33,    40,     0,   172,
     166,   189,     0,     0,     0,   170,   160,   161,     0,     0,
     154,   155,   169,   243,     0,   237,    76,    75,    51,    65,
       0,    66,    62,   164,   216,     0,   214,   213,     0,     0,
       0,   279,     0,     0,    10,   281,   282,     0,    18,    20,
     283,   284,     0,     0,   303,   304,   305,   307,   308,   309,
     311,   313,   315,   317,   318,   320,   321,   322,   323,   326,
     325,   328,   330,   332,   334,   335,   336,   337,   338,   339,
     340,   341,   342,   343,   344,    24,    23,    34,     0,     0,
       0,     0,    27,     0,     0,     0,     0,    42,    41,   173,
     167,     0,   170,   187,     0,     0,   179,   180,   178,     0,
       0,   169,   183,   162,   170,     0,   163,   257,   256,   189,
     218,   221,    53,     0,    67,     0,     0,     0,   293,   292,
      46,     3,     8,     9,    17,   287,   288,     0,    36,    26,
      25,     0,    29,    30,    31,    32,     0,   168,     0,   181,
     182,     0,     0,     0,   138,     0,   139,   177,     0,     0,
     150,   148,   145,   146,   149,     0,     0,     0,    68,   223,
     213,   234,     0,     0,     0,   225,   230,   232,     0,     0,
       0,    35,    28,    38,   200,   204,     0,   188,   198,   206,
     142,     0,   140,     0,     0,     0,   170,     0,   147,     0,
       0,     0,    69,     0,   228,     0,     0,   226,   233,   231,
       4,     5,     0,   141,   136,   137,     0,     0,   189,   144,
     143,   203,   207,     0,   201,   209,   215,   227,     0,     0,
     199,   205,     0,   189,     0,     0,     0,     0,   189,     0,
       0,   174,   202,   208,   229,   224,     0,   175,   176
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -525,  -525,  -525,  -196,  -180,   320,  -525,   -82,   265,   165,
    -525,  -525,  -525,  -525,   248,   321,  -525,  -525,   -80,   354,
     420,   425,   524,   529,   532,   429,   390,   427,   543,   528,
     548,   318,     4,    14,    -9,  -525,  -449,  -525,  -525,   102,
    -525,   423,  -525,  -525,    -3,    -1,   -62,   -61,  -525,   -57,
    -451,  -525,   195,  -501,   285,   593,  -467,  -525,  -525,    11,
    -394,  -525,  -525,   -81,   -78,  -525,  -525,  -524,   103,  -525,
    -525,  -525,  -525,    -2,  -525,  -525,  -525,  -525,  -525,   225,
     217,   218,   342,   348,   346,   233,   180,   236,   330,   343,
     349,   127,   -59
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,   318,   319,   195,   196,   197,   198,    69,   377,   461,
     458,    70,   378,    25,   254,   255,    26,    27,    97,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    44,    71,    72,   525,   477,   568,   531,   532,
     485,   388,    73,    74,    99,   100,    77,    78,    79,   481,
     225,   226,   227,   473,   228,    80,   474,    81,    82,   545,
     299,    84,    85,    86,    87,    88,   546,   547,   548,    89,
      90,    91,   153,    92,     5,     6,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   324
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      43,    98,    75,   240,    76,    41,   241,   243,   215,   245,
     222,   223,   323,   496,   246,   224,    83,    45,    46,   365,
     557,   519,   536,   316,   579,   112,   526,   219,   325,   455,
     459,   219,   107,   533,    42,   506,   222,   223,    93,    75,
     400,    76,   219,   379,   154,   156,   154,   126,    75,   382,
      76,   154,   249,   152,   162,   110,   111,   164,   456,   166,
      60,    61,   159,   218,   220,   168,   127,   221,   220,   557,
     383,   530,   238,   113,   401,   579,   326,   172,   460,   220,
     380,    75,    75,    76,    76,    98,    60,    61,   155,   390,
      60,    61,   163,   114,   115,   165,   242,   167,   305,    60,
      61,    60,    61,   169,   391,   248,   469,   257,   247,   557,
     154,   317,   585,   561,   219,   533,   528,   329,   330,   154,
     256,   604,   300,   131,   132,   133,   134,   167,   365,   563,
     402,   493,   562,   258,   376,   116,   609,   117,   118,   396,
     119,   616,   169,   470,    94,   248,   573,   418,    95,   137,
      75,   220,    76,   529,   138,   507,   303,   537,   293,   331,
     304,   332,   333,   128,   292,   494,   564,   508,   511,    98,
      60,    61,   295,   157,   297,   298,   222,   223,   511,   498,
     512,   392,    60,    61,   129,   130,    96,   231,   302,   366,
     513,   538,   367,   232,   154,   222,   223,   139,    96,   368,
     313,   511,   175,   176,   177,   320,   321,   178,   369,   179,
     180,   170,   483,   514,   181,   511,   233,   182,   154,   158,
     183,   184,   234,   409,    60,    61,   486,   515,    67,   527,
     160,   370,   371,   372,   373,   374,   375,   504,    60,    61,
      96,   161,   236,   237,   173,   185,   174,   186,   216,   187,
     188,   189,   190,   154,    96,   175,   176,   177,    96,   397,
     178,   217,   179,   180,    60,    61,   398,   181,   229,   215,
     182,   154,   399,   183,   184,   154,   471,   404,   135,   136,
      96,   406,   120,   121,   122,    60,    61,   366,   191,   306,
     367,   322,   403,   194,    98,   230,   376,   467,   185,   535,
     186,    96,   187,   188,   189,   190,   369,    67,    60,    61,
     235,   556,   244,   490,    60,    61,   491,   410,   593,   154,
      60,    61,   423,   248,    96,   408,    67,    60,    61,   370,
     371,   372,   373,   374,   375,   256,    96,   343,   344,   154,
     232,   191,   192,    96,   193,   411,   194,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   554,   349,
     350,   222,   223,   234,    42,   101,   102,   103,   104,   105,
     106,   250,   472,   108,   109,   462,   462,   462,   462,   484,
     484,   154,   398,   521,   154,   251,   476,   499,   505,   522,
     539,    75,   558,    76,   570,   610,   252,   479,    42,   253,
     571,   611,   256,   294,   307,   308,   309,   310,   311,   312,
     222,   223,   314,   315,   591,   296,   502,   503,   594,   495,
      42,   298,   222,   223,    42,   600,   301,     7,     8,     9,
     612,    42,    10,   327,    11,    12,    42,   610,   610,    13,
     320,   321,    14,   617,   618,    15,    16,   354,   355,   356,
     357,   358,   359,   360,   361,   362,   363,   364,   279,   280,
     281,   282,   283,   284,   285,   286,   287,   288,   289,   328,
      17,   340,    18,   342,    19,    20,    21,   412,   413,   524,
     472,   444,   445,   446,   447,   448,   449,   450,   451,   452,
     453,   454,   567,   383,    75,   341,    76,   351,    60,    61,
       1,     2,     3,   352,     4,   345,   346,   347,   348,   123,
     124,   125,   472,    22,    66,   552,   555,   353,    24,   190,
     559,   270,   271,   272,   273,   435,   436,   437,   438,   334,
     335,   336,   381,   337,   338,   339,   384,   463,   464,   465,
     259,   260,   261,   597,   598,    75,   575,    76,   262,   263,
     264,   424,   425,   426,   298,   427,   428,   429,   268,   269,
     386,   472,   274,   275,   385,   584,   387,   592,   393,   595,
     405,   590,   394,    75,   395,    76,   433,   434,   407,   472,
     601,   414,   415,   416,   417,   439,   440,   420,   421,   457,
     466,   480,   489,   487,   472,   488,    75,   509,    76,   472,
     497,   472,   606,   613,   290,   510,     7,     8,     9,   516,
     517,    10,   518,    11,    12,   534,   540,   549,    13,   550,
     551,    14,   553,   565,    15,    16,   560,   566,   569,   572,
     581,   576,   582,   577,   580,   614,   587,   586,   588,   615,
     596,   602,   603,   468,   605,   608,   492,    47,   419,    17,
     265,    18,   422,    19,    20,    48,   266,   389,    49,    50,
     267,    51,    52,    53,    42,   291,   277,   520,   589,    54,
     482,    55,    56,    57,   239,    58,    59,    60,    61,   599,
     276,   441,   430,    62,    63,    64,    65,   278,   432,   431,
       0,     0,    22,    66,    67,   442,     0,    24,     7,     8,
       9,     0,   443,    10,    68,    11,    12,     0,     0,     0,
      13,     0,     0,    14,     0,     0,    15,    16,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    47,
       0,    17,     0,    18,     0,    19,    20,    48,     0,     0,
      49,    50,     0,    51,    52,    53,    42,   541,   542,     0,
       0,    54,     0,    55,    56,    57,     0,    58,    59,    60,
      61,     0,   543,     0,   544,    62,    63,    64,    65,     0,
       0,     0,     0,     0,    22,    66,    67,     0,     0,    24,
       7,     8,     9,     0,     0,    10,    68,    11,    12,     0,
       0,     0,    13,     0,     0,    14,     0,     0,    15,    16,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    47,     0,    17,     0,    18,     0,    19,    20,    48,
       0,     0,    49,    50,     0,    51,    52,    53,    42,   578,
     542,     0,     0,    54,     0,    55,    56,    57,     0,    58,
      59,    60,    61,     0,   543,     0,   544,    62,    63,    64,
      65,     0,     0,     0,     0,     0,    22,    66,    67,     0,
       0,    24,     7,     8,     9,     0,     0,    10,    68,    11,
      12,     0,     0,     0,    13,     0,     0,    14,     0,     0,
      15,    16,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    47,     0,    17,     0,    18,     0,    19,
      20,    48,     0,     0,    49,    50,     0,    51,    52,    53,
      42,     0,   542,     0,     0,    54,     0,    55,    56,    57,
       0,    58,    59,    60,    61,     0,   543,   607,   544,    62,
      63,    64,    65,     0,     0,     0,     0,     0,    22,    66,
      67,     0,     0,    24,     7,     8,     9,     0,     0,    10,
      68,    11,    12,     0,     0,     0,    13,     0,     0,    14,
       0,     0,    15,    16,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    47,     0,    17,     0,    18,
       0,    19,    20,    48,     0,     0,    49,    50,     0,    51,
      52,    53,    42,     0,   542,     0,     0,    54,     0,    55,
      56,    57,     0,    58,    59,    60,    61,     0,   543,     0,
     544,    62,    63,    64,    65,     0,     0,     0,     0,     0,
      22,    66,    67,     0,     0,    24,     7,     8,     9,     0,
       0,    10,    68,    11,    12,     0,     0,     0,    13,     0,
       0,    14,     0,     0,    15,    16,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    47,     0,    17,
       0,    18,     0,    19,    20,    48,     0,     0,    49,    50,
       0,    51,    52,    53,    42,   151,     0,     0,     0,    54,
       0,    55,    56,    57,     0,    58,    59,    60,    61,     0,
       0,     0,     0,    62,    63,    64,    65,     0,     0,     0,
       0,     0,    22,    66,    67,     0,     0,    24,     7,     8,
       9,     0,     0,    10,    68,    11,    12,     0,     0,     0,
      13,     0,     0,    14,     0,     0,    15,    16,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    47,
       0,    17,     0,    18,     0,    19,    20,    48,     0,     0,
      49,    50,     0,    51,    52,    53,    42,     0,     0,     0,
       0,    54,     0,    55,    56,    57,     0,    58,    59,    60,
      61,     0,     0,     0,     0,    62,    63,    64,    65,     0,
       0,     0,     0,     0,    22,    66,    67,     0,     0,    24,
       7,     8,     9,     0,     0,    10,    68,    11,    12,     0,
       0,     0,    13,     0,     0,    14,     0,     0,    15,    16,
       7,     8,     9,     0,     0,    10,     0,    11,    12,     0,
       0,     0,    13,     0,     0,    14,     0,     0,    15,    16,
     523,     0,     0,    17,     0,    18,     0,    19,    20,    21,
       0,     0,     0,     0,     0,     0,     0,     0,   475,     0,
       0,     0,     0,    17,     0,    18,     0,    19,    20,    21,
       0,     0,     0,     0,     0,     0,     0,     0,   475,   583,
       0,     0,     0,     0,     0,     0,    22,    23,     0,     0,
       0,    24,     0,     0,     0,     0,     0,     7,     8,     9,
       0,     0,    10,     0,    11,    12,    22,    23,     0,    13,
       0,    24,    14,     0,     0,    15,    16,   175,   176,   177,
       0,     0,   178,     0,   179,   180,     0,     0,     0,   181,
       0,     0,   182,     0,     0,   183,   184,     0,     0,     0,
      17,     0,    18,     0,    19,    20,    21,     0,     0,   171,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     185,     0,   186,     0,   187,   188,   189,   190,     0,     0,
       0,     0,     0,     0,     7,     8,     9,     0,     0,    10,
       0,    11,    12,    22,    23,     0,    13,     0,    24,    14,
       0,     0,    15,    16,     7,     8,     9,     0,     0,    10,
       0,    11,    12,   191,   306,     0,    13,     0,   194,    14,
       0,     0,    15,    16,     0,     0,     0,    17,     0,    18,
       0,    19,    20,    21,     0,     0,     0,     0,     0,     0,
       0,     0,   475,     0,     0,     0,     0,    17,     0,    18,
       0,    19,    20,    21,     0,   478,     0,     0,     0,     0,
       0,     7,   500,     9,     0,     0,    10,     0,    11,    12,
      22,    23,     0,    13,     0,    24,    14,     0,     0,    15,
      16,     7,     8,     9,     0,     0,    10,     0,    11,    12,
      22,    23,     0,    13,     0,    24,    14,     0,     0,    15,
      16,     0,     0,     0,    17,     0,    18,     0,    19,    20,
      21,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    17,     0,    18,     0,    19,    20,
      21,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   574,     0,     0,     0,     0,    22,    23,     0,
     501,     0,    24,     0,     0,     0,     0,     0,     7,     8,
       9,     0,     0,    10,     0,    11,    12,    22,    23,     0,
      13,     0,    24,    14,     0,     0,    15,    16,   175,   176,
     177,     0,     0,   178,     0,   179,   180,     0,     0,     0,
     181,     0,     0,   182,     0,     0,   183,   184,     0,     0,
       0,    17,     0,    18,     0,    19,    20,    21,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   185,     0,   186,     0,   187,   188,   189,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    22,    23,     0,     0,     0,    24,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   191,   306,     0,     0,     0,   194
};

static const yytype_int16 yycheck[] =
{
       2,    10,     5,    84,     5,     1,    84,    85,    67,    91,
      72,    72,   192,   407,    94,    72,     5,     3,     4,   215,
     521,   472,   489,     4,   548,     4,   475,    15,     4,    64,
       4,    15,    18,   484,    61,    49,    98,    98,     0,    42,
      15,    42,    15,    15,    43,    47,    43,    18,    51,    32,
      51,    43,    49,    42,    52,     6,     7,    52,    93,    52,
      74,    75,    51,    55,    52,    52,    20,    55,    52,   570,
      53,    55,    81,    52,    49,   599,    52,    63,    52,    52,
      52,    84,    85,    84,    85,    94,    74,    75,    87,    52,
      74,    75,    90,     6,     7,    90,    85,    90,   178,    74,
      75,    74,    75,    90,    52,    15,    15,   116,    94,   610,
      43,    92,   561,    43,    15,   566,    15,     6,     7,    43,
     116,   588,    55,    23,    24,    25,    26,    90,   324,    15,
      54,    15,    62,   119,   216,    48,   603,    50,    51,    49,
      53,   608,    90,    52,    48,    15,   540,   327,    52,    29,
     153,    52,   153,    52,    30,    43,    48,    15,   154,    48,
      52,    50,    51,    19,   153,    49,    52,    55,    43,   178,
      74,    75,   158,    64,   160,   161,   238,   238,    43,    49,
      55,   238,    74,    75,    21,    22,    90,    55,   174,    52,
      55,    49,    55,    61,    43,   257,   257,    31,    90,    62,
     186,    43,     3,     4,     5,     6,     7,     8,    71,    10,
      11,    55,    62,    55,    15,    43,    55,    18,    43,    48,
      21,    22,    61,   303,    74,    75,    62,    55,    91,    54,
      48,    94,    95,    96,    97,    98,    99,   417,    74,    75,
      90,    48,    74,    75,    52,    46,    48,    48,    61,    50,
      51,    52,    53,    43,    90,     3,     4,     5,    90,    49,
       8,    55,    10,    11,    74,    75,    43,    15,    64,   328,
      18,    43,    49,    21,    22,    43,    65,    49,    27,    28,
      90,    49,    12,    13,    14,    74,    75,    52,    89,    90,
      55,    92,   294,    94,   303,    64,   378,    62,    46,    65,
      48,    90,    50,    51,    52,    53,    71,    91,    74,    75,
      55,    65,    55,   394,    74,    75,   394,   303,    65,    43,
      74,    75,   331,    15,    90,    49,    91,    74,    75,    94,
      95,    96,    97,    98,    99,   331,    90,    21,    22,    43,
      61,    89,    90,    90,    92,    49,    94,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    55,    27,
      28,   423,   423,    61,    61,    11,    12,    13,    14,    15,
      16,    52,   381,    19,    20,   371,   372,   373,   374,   388,
     389,    43,    43,    43,    43,    52,   382,    49,    49,    49,
      49,   394,    55,   394,    43,    43,    52,   383,    61,    52,
      49,    49,   398,    45,   179,   180,   181,   182,   183,   184,
     472,   472,   187,   188,    55,    59,   412,   413,    55,   405,
      61,   407,   484,   484,    61,    55,    55,     3,     4,     5,
      55,    61,     8,    64,    10,    11,    61,    43,    43,    15,
       6,     7,    18,    49,    49,    21,    22,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,    43,
      46,    18,    48,    19,    50,    51,    52,    21,    22,   475,
     489,   354,   355,   356,   357,   358,   359,   360,   361,   362,
     363,   364,    52,    53,   497,    20,   497,    29,    74,    75,
      84,    85,    86,    30,    88,    23,    24,    25,    26,    15,
      16,    17,   521,    89,    90,   511,   518,    31,    94,    53,
     522,   131,   132,   133,   134,   345,   346,   347,   348,    12,
      13,    14,    48,    15,    16,    17,    48,   372,   373,   374,
     120,   121,   122,    64,    65,   548,   542,   548,   123,   124,
     125,   334,   335,   336,   540,   337,   338,   339,   129,   130,
      90,   570,   135,   136,    43,   561,    90,   569,    55,   571,
      48,   567,    57,   576,    55,   576,   343,   344,    55,   588,
     582,    54,    52,    52,    64,   349,   350,    52,    52,    52,
       4,    15,    48,    55,   603,    55,   599,    64,   599,   608,
      61,   610,   598,   605,     1,    64,     3,     4,     5,    43,
      52,     8,    49,    10,    11,    55,    55,    15,    15,    15,
      52,    18,     4,    49,    21,    22,    62,    43,    49,    49,
       4,    64,    49,    64,    92,    64,    49,    52,    48,    64,
      49,    49,    48,   378,    49,    48,   398,    44,   328,    46,
     126,    48,   331,    50,    51,    52,   127,   234,    55,    56,
     128,    58,    59,    60,    61,    62,   138,   472,   566,    66,
     385,    68,    69,    70,    81,    72,    73,    74,    75,   576,
     137,   351,   340,    80,    81,    82,    83,   139,   342,   341,
      -1,    -1,    89,    90,    91,   352,    -1,    94,     3,     4,
       5,    -1,   353,     8,   101,    10,    11,    -1,    -1,    -1,
      15,    -1,    -1,    18,    -1,    -1,    21,    22,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,
      -1,    46,    -1,    48,    -1,    50,    51,    52,    -1,    -1,
      55,    56,    -1,    58,    59,    60,    61,    62,    63,    -1,
      -1,    66,    -1,    68,    69,    70,    -1,    72,    73,    74,
      75,    -1,    77,    -1,    79,    80,    81,    82,    83,    -1,
      -1,    -1,    -1,    -1,    89,    90,    91,    -1,    -1,    94,
       3,     4,     5,    -1,    -1,     8,   101,    10,    11,    -1,
      -1,    -1,    15,    -1,    -1,    18,    -1,    -1,    21,    22,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    44,    -1,    46,    -1,    48,    -1,    50,    51,    52,
      -1,    -1,    55,    56,    -1,    58,    59,    60,    61,    62,
      63,    -1,    -1,    66,    -1,    68,    69,    70,    -1,    72,
      73,    74,    75,    -1,    77,    -1,    79,    80,    81,    82,
      83,    -1,    -1,    -1,    -1,    -1,    89,    90,    91,    -1,
      -1,    94,     3,     4,     5,    -1,    -1,     8,   101,    10,
      11,    -1,    -1,    -1,    15,    -1,    -1,    18,    -1,    -1,
      21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    44,    -1,    46,    -1,    48,    -1,    50,
      51,    52,    -1,    -1,    55,    56,    -1,    58,    59,    60,
      61,    -1,    63,    -1,    -1,    66,    -1,    68,    69,    70,
      -1,    72,    73,    74,    75,    -1,    77,    78,    79,    80,
      81,    82,    83,    -1,    -1,    -1,    -1,    -1,    89,    90,
      91,    -1,    -1,    94,     3,     4,     5,    -1,    -1,     8,
     101,    10,    11,    -1,    -1,    -1,    15,    -1,    -1,    18,
      -1,    -1,    21,    22,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    44,    -1,    46,    -1,    48,
      -1,    50,    51,    52,    -1,    -1,    55,    56,    -1,    58,
      59,    60,    61,    -1,    63,    -1,    -1,    66,    -1,    68,
      69,    70,    -1,    72,    73,    74,    75,    -1,    77,    -1,
      79,    80,    81,    82,    83,    -1,    -1,    -1,    -1,    -1,
      89,    90,    91,    -1,    -1,    94,     3,     4,     5,    -1,
      -1,     8,   101,    10,    11,    -1,    -1,    -1,    15,    -1,
      -1,    18,    -1,    -1,    21,    22,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    -1,    46,
      -1,    48,    -1,    50,    51,    52,    -1,    -1,    55,    56,
      -1,    58,    59,    60,    61,    62,    -1,    -1,    -1,    66,
      -1,    68,    69,    70,    -1,    72,    73,    74,    75,    -1,
      -1,    -1,    -1,    80,    81,    82,    83,    -1,    -1,    -1,
      -1,    -1,    89,    90,    91,    -1,    -1,    94,     3,     4,
       5,    -1,    -1,     8,   101,    10,    11,    -1,    -1,    -1,
      15,    -1,    -1,    18,    -1,    -1,    21,    22,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,
      -1,    46,    -1,    48,    -1,    50,    51,    52,    -1,    -1,
      55,    56,    -1,    58,    59,    60,    61,    -1,    -1,    -1,
      -1,    66,    -1,    68,    69,    70,    -1,    72,    73,    74,
      75,    -1,    -1,    -1,    -1,    80,    81,    82,    83,    -1,
      -1,    -1,    -1,    -1,    89,    90,    91,    -1,    -1,    94,
       3,     4,     5,    -1,    -1,     8,   101,    10,    11,    -1,
      -1,    -1,    15,    -1,    -1,    18,    -1,    -1,    21,    22,
       3,     4,     5,    -1,    -1,     8,    -1,    10,    11,    -1,
      -1,    -1,    15,    -1,    -1,    18,    -1,    -1,    21,    22,
      43,    -1,    -1,    46,    -1,    48,    -1,    50,    51,    52,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,
      -1,    -1,    -1,    46,    -1,    48,    -1,    50,    51,    52,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    62,
      -1,    -1,    -1,    -1,    -1,    -1,    89,    90,    -1,    -1,
      -1,    94,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
      -1,    -1,     8,    -1,    10,    11,    89,    90,    -1,    15,
      -1,    94,    18,    -1,    -1,    21,    22,     3,     4,     5,
      -1,    -1,     8,    -1,    10,    11,    -1,    -1,    -1,    15,
      -1,    -1,    18,    -1,    -1,    21,    22,    -1,    -1,    -1,
      46,    -1,    48,    -1,    50,    51,    52,    -1,    -1,    55,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      46,    -1,    48,    -1,    50,    51,    52,    53,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,    -1,     8,
      -1,    10,    11,    89,    90,    -1,    15,    -1,    94,    18,
      -1,    -1,    21,    22,     3,     4,     5,    -1,    -1,     8,
      -1,    10,    11,    89,    90,    -1,    15,    -1,    94,    18,
      -1,    -1,    21,    22,    -1,    -1,    -1,    46,    -1,    48,
      -1,    50,    51,    52,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    61,    -1,    -1,    -1,    -1,    46,    -1,    48,
      -1,    50,    51,    52,    -1,    54,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,    -1,     8,    -1,    10,    11,
      89,    90,    -1,    15,    -1,    94,    18,    -1,    -1,    21,
      22,     3,     4,     5,    -1,    -1,     8,    -1,    10,    11,
      89,    90,    -1,    15,    -1,    94,    18,    -1,    -1,    21,
      22,    -1,    -1,    -1,    46,    -1,    48,    -1,    50,    51,
      52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    -1,    48,    -1,    50,    51,
      52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    64,    -1,    -1,    -1,    -1,    89,    90,    -1,
      92,    -1,    94,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,    -1,    -1,     8,    -1,    10,    11,    89,    90,    -1,
      15,    -1,    94,    18,    -1,    -1,    21,    22,     3,     4,
       5,    -1,    -1,     8,    -1,    10,    11,    -1,    -1,    -1,
      15,    -1,    -1,    18,    -1,    -1,    21,    22,    -1,    -1,
      -1,    46,    -1,    48,    -1,    50,    51,    52,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    46,    -1,    48,    -1,    50,    51,    52,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    90,    -1,    -1,    -1,    94,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    90,    -1,    -1,    -1,    94
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    84,    85,    86,    88,   176,   177,     3,     4,     5,
       8,    10,    11,    15,    18,    21,    22,    46,    48,    50,
      51,    52,    89,    90,    94,   115,   118,   119,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,    61,   175,   134,   135,   135,    44,    52,    55,
      56,    58,    59,    60,    66,    68,    69,    70,    72,    73,
      74,    75,    80,    81,    82,    83,    90,    91,   101,   109,
     113,   135,   136,   144,   145,   146,   147,   148,   149,   150,
     157,   159,   160,   161,   163,   164,   165,   166,   167,   171,
     172,   173,   175,     0,    48,    52,    90,   120,   136,   146,
     147,   121,   121,   121,   121,   121,   121,   135,   121,   121,
       6,     7,     4,    52,     6,     7,    48,    50,    51,    53,
      12,    13,    14,    15,    16,    17,    18,    20,    19,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    62,   161,   174,    43,    87,   175,    64,    48,   161,
      48,    48,    52,    90,    52,    90,    52,    90,    52,    90,
      55,    55,   135,    52,    48,     3,     4,     5,     8,    10,
      11,    15,    18,    21,    22,    46,    48,    50,    51,    52,
      53,    89,    90,    92,    94,   105,   106,   107,   108,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   191,   192,   193,   194,    61,    55,    55,    15,
      52,    55,   148,   149,   151,   152,   153,   154,   156,    64,
      64,    55,    61,    55,    61,    55,    74,    75,   136,   157,
     165,   166,   161,   166,    55,   109,   120,   135,    15,    49,
      52,    52,    52,    52,   116,   117,   134,   136,   135,   122,
     122,   122,   123,   123,   123,   124,   125,   126,   127,   127,
     128,   128,   128,   128,   129,   129,   130,   131,   132,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
       1,    62,   161,   134,    45,   135,    59,   135,   135,   162,
      55,    55,   135,    48,    52,   120,    90,   181,   181,   181,
     181,   181,   181,   135,   181,   181,     4,    92,   103,   104,
       6,     7,    92,   106,   194,     4,    52,    64,    43,     6,
       7,    48,    50,    51,    12,    13,    14,    15,    16,    17,
      18,    20,    19,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,   105,    52,    55,    62,    71,
      94,    95,    96,    97,    98,    99,   109,   110,   114,    15,
      52,    48,    32,    53,    48,    43,    90,    90,   143,   143,
      52,    52,   151,    55,    57,    55,    49,    49,    43,    49,
      15,    49,    54,   175,    49,    48,    49,    55,    49,   120,
     135,    49,    21,    22,    54,    52,    52,    64,   106,   107,
      52,    52,   117,   136,   182,   182,   182,   183,   183,   183,
     184,   185,   186,   187,   187,   188,   188,   188,   188,   189,
     189,   190,   191,   192,   193,   193,   193,   193,   193,   193,
     193,   193,   193,   193,   193,    64,    93,    52,   112,     4,
      52,   111,   134,   111,   111,   111,     4,    62,   110,    15,
      52,    65,   136,   155,   158,    61,   134,   138,    54,   135,
      15,   151,   156,    62,   136,   142,    62,    55,    55,    48,
     165,   166,   116,    15,    49,   135,   162,    61,    49,    49,
       4,    92,   134,   134,   106,    49,    49,    43,    55,    64,
      64,    43,    55,    55,    55,    55,    43,    52,    49,   152,
     154,    43,    49,    43,   134,   137,   138,    54,    15,    52,
      55,   140,   141,   152,    55,    65,   158,    15,    49,    49,
      55,    62,    63,    77,    79,   161,   168,   169,   170,    15,
      15,    52,   134,     4,    55,   175,    65,   155,    55,   175,
      62,    43,    62,    15,    52,    49,    43,    52,   139,    49,
      43,    49,    49,   162,    64,   134,    64,    64,    62,   169,
      92,     4,    49,    62,   134,   138,    52,    49,    48,   141,
     134,    55,   175,    65,    55,   175,    49,    64,    65,   170,
      55,   175,    49,    48,   158,    49,   134,    78,    48,   158,
      43,    49,    55,   175,    64,    64,   158,    49,    49
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
     178,   178,   178,   178,   178,   178,   178,   178,   179,   179,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   181,   181,
     181,   181,   182,   182,   182,   182,   183,   183,   183,   183,
     184,   184,   185,   185,   186,   186,   187,   187,   187,   188,
     188,   188,   188,   188,   189,   189,   189,   190,   190,   191,
     191,   192,   192,   193,   193,   193,   193,   193,   193,   193,
     193,   193,   193,   193,   193,   194
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
       2,     3,     2,     0,     2,     1,     2,     3,     2,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     3,     3,     3,     3,     2,     2,     4,     4,     1,
       2,     2,     4,     4,     2,     2,     2,     2,     2,     2,
       2,     2,     1,     3,     3,     3,     1,     3,     3,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     3,     1,
       3,     3,     3,     3,     1,     3,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     1
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
#line 2099 "HolyC.tab.c"
    break;

  case 3:
#line 62 "HolyC.y"
                                         {
  yyval=AppendToSIB(yyvsp[-2],NULL,yyvsp[0],NULL);
  ReleaseAST(yyvsp[-1]);
}
#line 2108 "HolyC.tab.c"
    break;

  case 4:
#line 66 "HolyC.y"
                                                        {
  yyval=AppendToSIB(yyvsp[-4],yyvsp[-2],yyvsp[0],NULL);
  ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-1]);
}
#line 2117 "HolyC.tab.c"
    break;

  case 5:
#line 70 "HolyC.y"
                                                         {
  yyval=AppendToSIB(yyvsp[-4],yyvsp[0],yyvsp[-2],NULL);
  ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-1]);
}
#line 2126 "HolyC.tab.c"
    break;

  case 6:
#line 74 "HolyC.y"
                  {
  yyval=AppendToSIB(NULL,NULL,NULL,yyvsp[0]);
 }
#line 2134 "HolyC.tab.c"
    break;

  case 8:
#line 78 "HolyC.y"
                                   {
  yyval=AppendToSIB(yyvsp[-2],NULL,NULL,yyvsp[0]);
  ReleaseAST(yyvsp[-1]);
}
#line 2143 "HolyC.tab.c"
    break;

  case 9:
#line 82 "HolyC.y"
                                    {
  yyval=AppendToSIB(yyvsp[-2],NULL,NULL,yyvsp[0]);
  ReleaseAST(yyvsp[-1]);
}
#line 2152 "HolyC.tab.c"
    break;

  case 10:
#line 86 "HolyC.y"
                                                      {
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
  yyval=yyvsp[-1];
}
#line 2161 "HolyC.tab.c"
    break;

  case 12:
#line 91 "HolyC.y"
                            {
  yyval=AppendToSIB(yyvsp[0],NULL,NULL,yyvsp[-1]);
 }
#line 2169 "HolyC.tab.c"
    break;

  case 16:
#line 97 "HolyC.y"
                                 {
  yyval=yyvsp[0];
  CType *t=*map_get(&Compiler.types,yyvsp[-1]->name);
  yyval->asmAddr.width=TypeSize(t);
}
#line 2179 "HolyC.tab.c"
    break;

  case 17:
#line 102 "HolyC.y"
                                                     {
  CType *t=*map_get(&Compiler.types,yyvsp[-3]->name);
  yyvsp[0]->asmAddr.width=TypeSize(t);
  yyvsp[0]->asmAddr.segment=yyvsp[-2];
  yyval=yyvsp[0];
}
#line 2190 "HolyC.tab.c"
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
#line 2203 "HolyC.tab.c"
    break;

  case 19:
#line 116 "HolyC.y"
                                {yyval=yyvsp[0];}
#line 2209 "HolyC.tab.c"
    break;

  case 20:
#line 117 "HolyC.y"
                                                            {
  yyval=CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA);
  ReleaseAST(yyvsp[-1]);
}
#line 2218 "HolyC.tab.c"
    break;

  case 21:
#line 121 "HolyC.y"
                 {yyval=NULL;}
#line 2224 "HolyC.tab.c"
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
#line 2238 "HolyC.tab.c"
    break;

  case 23:
#line 131 "HolyC.y"
                                         {
  yyval=SOT(CreateExportedLabel(yyvsp[-1]),yyvsp[-1]);
  yyval->inAsmBlk=1;
  ReleaseAST(yyvsp[0]);
}
#line 2248 "HolyC.tab.c"
    break;

  case 24:
#line 136 "HolyC.y"
                                  {
  yyval=SOT(CreateLabel(yyvsp[-1]),yyvsp[-1]);
  yyval->inAsmBlk=1;
  ReleaseAST(yyvsp[0]);
 }
#line 2258 "HolyC.tab.c"
    break;

  case 25:
#line 141 "HolyC.y"
                                                {
  yyval=SOT(CreateLocalLabel(yyvsp[-1]),yyvsp[-2]);
  yyval->inAsmBlk=1;
  ReleaseAST(yyvsp[0]),ReleaseAST(yyvsp[-2]);
}
#line 2268 "HolyC.tab.c"
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
#line 2284 "HolyC.tab.c"
    break;

  case 28:
#line 159 "HolyC.y"
                                                {
  yyval=CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA);
  ReleaseAST(yyvsp[-1]);
}
#line 2293 "HolyC.tab.c"
    break;

  case 29:
#line 163 "HolyC.y"
                                                {
  yyval=SOT(CreateDU8(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2302 "HolyC.tab.c"
    break;

  case 30:
#line 167 "HolyC.y"
                                                 {
  yyval=SOT(CreateDU16(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2311 "HolyC.tab.c"
    break;

  case 31:
#line 171 "HolyC.y"
                                                 {
  yyval=SOT(CreateDU32(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2320 "HolyC.tab.c"
    break;

  case 32:
#line 175 "HolyC.y"
                                                 {
  yyval=SOT(CreateDU64(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2329 "HolyC.tab.c"
    break;

  case 35:
#line 181 "HolyC.y"
                                             {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA),yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
}
#line 2338 "HolyC.tab.c"
    break;

  case 36:
#line 185 "HolyC.y"
                                                       {
  yyval=SOT(CreateAsmImport(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2347 "HolyC.tab.c"
    break;

  case 37:
#line 189 "HolyC.y"
                          {
  yyval=SOT(CreateNop(),yyvsp[0]);
  ReleaseAST(yyvsp[0]);
 }
#line 2356 "HolyC.tab.c"
    break;

  case 38:
#line 193 "HolyC.y"
                                                           {
  yyval=SOT(CreateAsmAlign(yyvsp[-2],yyvsp[0]),yyvsp[-3]);
  ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-3]);
}
#line 2365 "HolyC.tab.c"
    break;

  case 39:
#line 197 "HolyC.y"
                                                   {
  yyval=SOT(CreateNop(),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 2374 "HolyC.tab.c"
    break;

  case 41:
#line 202 "HolyC.y"
                                                   {
  yyval=AppendToStmts(yyvsp[-1],yyvsp[0]);
}
#line 2382 "HolyC.tab.c"
    break;

  case 42:
#line 205 "HolyC.y"
                                                                    {
  yyval=yyvsp[-1];
  yyval->type=AST_ASM_BLK;
  ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2392 "HolyC.tab.c"
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
#line 2405 "HolyC.tab.c"
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
#line 2419 "HolyC.tab.c"
    break;

  case 45:
#line 228 "HolyC.y"
                {yyval=SLE(yyvsp[0]);}
#line 2425 "HolyC.tab.c"
    break;

  case 46:
#line 229 "HolyC.y"
              {yyval=SLE(yyvsp[0]);}
#line 2431 "HolyC.tab.c"
    break;

  case 47:
#line 230 "HolyC.y"
               {yyval=SLE(yyvsp[0]);}
#line 2437 "HolyC.tab.c"
    break;

  case 48:
#line 231 "HolyC.y"
                 {yyval=SLE(yyvsp[0]);}
#line 2443 "HolyC.tab.c"
    break;

  case 49:
#line 232 "HolyC.y"
                    {
  yyval=SLE(yyvsp[0]);
}
#line 2451 "HolyC.tab.c"
    break;

  case 50:
#line 235 "HolyC.y"
               {
  yyval=SLE(yyvsp[0]);
}
#line 2459 "HolyC.tab.c"
    break;

  case 51:
#line 238 "HolyC.y"
              {yyval=NULL;}
#line 2465 "HolyC.tab.c"
    break;

  case 52:
#line 239 "HolyC.y"
                      {yyval=yyvsp[0];}
#line 2471 "HolyC.tab.c"
    break;

  case 53:
#line 240 "HolyC.y"
                                               {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA),yyvsp[-1]);
}
#line 2479 "HolyC.tab.c"
    break;

  case 54:
#line 243 "HolyC.y"
                          {yyval=yyvsp[0];}
#line 2485 "HolyC.tab.c"
    break;

  case 56:
#line 245 "HolyC.y"
                                                         {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2494 "HolyC.tab.c"
    break;

  case 58:
#line 254 "HolyC.y"
                                     {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2503 "HolyC.tab.c"
    break;

  case 59:
#line 258 "HolyC.y"
                                       {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2512 "HolyC.tab.c"
    break;

  case 60:
#line 262 "HolyC.y"
                                  {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2521 "HolyC.tab.c"
    break;

  case 61:
#line 266 "HolyC.y"
                                    {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2530 "HolyC.tab.c"
    break;

  case 62:
#line 270 "HolyC.y"
                                                                    {
  yyval=SOT(CreateArrayAccess(yyvsp[-3],yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2539 "HolyC.tab.c"
    break;

  case 63:
#line 274 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[-1],AST_POST_DEC),yyvsp[0]);
  ReleaseAST(yyvsp[0]);
}
#line 2548 "HolyC.tab.c"
    break;

  case 64:
#line 278 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[-1],AST_POST_INC),yyvsp[0]);
  ReleaseAST(yyvsp[0]);
}
#line 2557 "HolyC.tab.c"
    break;

  case 65:
#line 282 "HolyC.y"
                                                                {
  yyval=SOT(CreateFuncCall(yyvsp[-3],yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2566 "HolyC.tab.c"
    break;

  case 66:
#line 286 "HolyC.y"
                                                                 {
  yyval=CreateExplicitTypecast(yyvsp[-3],yyvsp[-1]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2575 "HolyC.tab.c"
    break;

  case 67:
#line 290 "HolyC.y"
                                                                          {
  yyvsp[-2]->type2=CreateMultiLvlPtr(yyvsp[-2]->type2,1);
  yyval=CreateExplicitTypecast(yyvsp[-4],yyvsp[-2]);
  ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 2585 "HolyC.tab.c"
    break;

  case 68:
#line 295 "HolyC.y"
                                                                                   {
  yyvsp[-3]->type2=CreateMultiLvlPtr(yyvsp[-3]->type2,2);
  yyval=CreateExplicitTypecast(yyvsp[-5],yyvsp[-3]);
  ReleaseAST(yyvsp[-4]),ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 2595 "HolyC.tab.c"
    break;

  case 69:
#line 300 "HolyC.y"
                                                                                            {
  yyvsp[-4]->type2=CreateMultiLvlPtr(yyvsp[-4]->type2,3);
  yyval=CreateExplicitTypecast(yyvsp[-6],yyvsp[-4]);
  ReleaseAST(yyvsp[-5]),ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 2605 "HolyC.tab.c"
    break;

  case 70:
#line 308 "HolyC.y"
                             {yyval=yyvsp[0];}
#line 2611 "HolyC.tab.c"
    break;

  case 71:
#line 309 "HolyC.y"
                                        {
  CType *ptr =CreateMultiLvlPtr(yyvsp[-1]->type2,1);
  yyval=CreateTypeNode(ptr);
  ReleaseAST(yyvsp[0]);
}
#line 2621 "HolyC.tab.c"
    break;

  case 73:
#line 315 "HolyC.y"
                            {
  yyval=SOT(CreateSizeof(yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2630 "HolyC.tab.c"
    break;

  case 74:
#line 319 "HolyC.y"
                                   {
  yyval=SOT(CreateSizeof(yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2639 "HolyC.tab.c"
    break;

  case 75:
#line 323 "HolyC.y"
                                                                    {
  yyval=SOT(CreateSizeof(yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 2650 "HolyC.tab.c"
    break;

  case 76:
#line 329 "HolyC.y"
                                                                     {
  yyval=SOT(CreateSizeof(yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 2661 "HolyC.tab.c"
    break;

  case 77:
#line 335 "HolyC.y"
                           {
  yyval=SOT(CreateUnop(yyvsp[0],AST_ADDROF),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2670 "HolyC.tab.c"
    break;

  case 78:
#line 339 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_DERREF),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2679 "HolyC.tab.c"
    break;

  case 79:
#line 344 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_PRE_DEC),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2688 "HolyC.tab.c"
    break;

  case 80:
#line 348 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_PRE_INC),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2697 "HolyC.tab.c"
    break;

  case 81:
#line 352 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_POS),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2706 "HolyC.tab.c"
    break;

  case 82:
#line 356 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_NEG),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2715 "HolyC.tab.c"
    break;

  case 83:
#line 360 "HolyC.y"
                           {
  yyval=SOT(CreateUnop(yyvsp[0],AST_LNOT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2724 "HolyC.tab.c"
    break;

  case 84:
#line 364 "HolyC.y"
                           {
  yyval=SOT(CreateUnop(yyvsp[0],AST_BNOT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2733 "HolyC.tab.c"
    break;

  case 86:
#line 371 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_POW),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2742 "HolyC.tab.c"
    break;

  case 87:
#line 375 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SHL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2751 "HolyC.tab.c"
    break;

  case 88:
#line 379 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SHR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2760 "HolyC.tab.c"
    break;

  case 90:
#line 385 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_MUL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2769 "HolyC.tab.c"
    break;

  case 91:
#line 389 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_DIV),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2778 "HolyC.tab.c"
    break;

  case 92:
#line 393 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_MOD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2787 "HolyC.tab.c"
    break;

  case 94:
#line 399 "HolyC.y"
                                    {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2796 "HolyC.tab.c"
    break;

  case 96:
#line 405 "HolyC.y"
                                    {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2805 "HolyC.tab.c"
    break;

  case 98:
#line 411 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2814 "HolyC.tab.c"
    break;

  case 100:
#line 417 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ADD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2823 "HolyC.tab.c"
    break;

  case 101:
#line 421 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SUB),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2832 "HolyC.tab.c"
    break;

  case 103:
#line 427 "HolyC.y"
                                  {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_LT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2841 "HolyC.tab.c"
    break;

  case 104:
#line 431 "HolyC.y"
                                  {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_GT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2850 "HolyC.tab.c"
    break;

  case 105:
#line 435 "HolyC.y"
                                  {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_LE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2859 "HolyC.tab.c"
    break;

  case 106:
#line 439 "HolyC.y"
                                  {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_GE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2868 "HolyC.tab.c"
    break;

  case 108:
#line 445 "HolyC.y"
                                     {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_EQ),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2877 "HolyC.tab.c"
    break;

  case 109:
#line 449 "HolyC.y"
                                     {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_NE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2886 "HolyC.tab.c"
    break;

  case 111:
#line 455 "HolyC.y"
                                       {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2895 "HolyC.tab.c"
    break;

  case 113:
#line 461 "HolyC.y"
                                       {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2904 "HolyC.tab.c"
    break;

  case 115:
#line 467 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2913 "HolyC.tab.c"
    break;

  case 117:
#line 473 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2922 "HolyC.tab.c"
    break;

  case 118:
#line 477 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SHL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2931 "HolyC.tab.c"
    break;

  case 119:
#line 481 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SHR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2940 "HolyC.tab.c"
    break;

  case 120:
#line 485 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_MUL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2949 "HolyC.tab.c"
    break;

  case 121:
#line 489 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_DIV),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2958 "HolyC.tab.c"
    break;

  case 122:
#line 493 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_MOD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2967 "HolyC.tab.c"
    break;

  case 123:
#line 497 "HolyC.y"
                                          {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2976 "HolyC.tab.c"
    break;

  case 124:
#line 501 "HolyC.y"
                                          {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2985 "HolyC.tab.c"
    break;

  case 125:
#line 505 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2994 "HolyC.tab.c"
    break;

  case 126:
#line 509 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_ADD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 3003 "HolyC.tab.c"
    break;

  case 127:
#line 513 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SUB),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 3012 "HolyC.tab.c"
    break;

  case 129:
#line 520 "HolyC.y"
                                              {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 3021 "HolyC.tab.c"
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
#line 3036 "HolyC.tab.c"
    break;

  case 132:
#line 539 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 3042 "HolyC.tab.c"
    break;

  case 133:
#line 540 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 3048 "HolyC.tab.c"
    break;

  case 134:
#line 541 "HolyC.y"
                                      {
  yyval=yyvsp[0];
  AssignUnionBasetype(yyvsp[0],yyvsp[-1]);
}
#line 3057 "HolyC.tab.c"
    break;

  case 135:
#line 545 "HolyC.y"
                                      {
  yyval=yyvsp[0];
  AssignClassBasetype(yyvsp[0],yyvsp[-1]);
}
#line 3066 "HolyC.tab.c"
    break;

  case 136:
#line 552 "HolyC.y"
                                          {yyval=AppendToArrLiteral(yyvsp[-2],yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3072 "HolyC.tab.c"
    break;

  case 137:
#line 553 "HolyC.y"
                                            {yyval=AppendToArrLiteral(yyvsp[-2],yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3078 "HolyC.tab.c"
    break;

  case 138:
#line 554 "HolyC.y"
              {yyval=AppendToArrLiteral(NULL,yyvsp[0]);}
#line 3084 "HolyC.tab.c"
    break;

  case 139:
#line 555 "HolyC.y"
                {yyval=AppendToArrLiteral(NULL,yyvsp[0]);}
#line 3090 "HolyC.tab.c"
    break;

  case 140:
#line 556 "HolyC.y"
                                                       {
  yyval=SOT(yyvsp[-1],yyvsp[-2]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3100 "HolyC.tab.c"
    break;

  case 141:
#line 561 "HolyC.y"
                                                                 {
  yyval=SOT(yyvsp[-2],yyvsp[-3]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3111 "HolyC.tab.c"
    break;

  case 142:
#line 567 "HolyC.y"
                                                      {
  yyval=NULL;
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3122 "HolyC.tab.c"
    break;

  case 143:
#line 576 "HolyC.y"
                             {
  yyval=AppendToMetaData(NULL,yyvsp[-1],yyvsp[0]);
}
#line 3130 "HolyC.tab.c"
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
#line 3142 "HolyC.tab.c"
    break;

  case 145:
#line 586 "HolyC.y"
                            {yyval=yyvsp[0];}
#line 3148 "HolyC.tab.c"
    break;

  case 146:
#line 587 "HolyC.y"
                              {
  AST *t=yyvsp[0];
  yyval=yyvsp[0];
}
#line 3157 "HolyC.tab.c"
    break;

  case 147:
#line 591 "HolyC.y"
                                          {
  AST *meta=yyvsp[-1]->declTail.data[0].metaData;
  meta=AppendToMetaData(meta,yyvsp[0]->metaData.data[0].name,yyvsp[0]->metaData.data[0].value);
  yyvsp[-1]->declTail.data[0].metaData=meta;
  yyval=yyvsp[-1];
}
#line 3168 "HolyC.tab.c"
    break;

  case 148:
#line 597 "HolyC.y"
                                    {
  DeclsFillInBasetype(yyvsp[-1]->type2,yyvsp[0]);
  yyval=yyvsp[0];
}
#line 3177 "HolyC.tab.c"
    break;

  case 149:
#line 601 "HolyC.y"
                                      {
 yyval=AppendToStmts(yyvsp[-2],yyvsp[-1]);
 ReleaseAST(yyvsp[0]);
}
#line 3186 "HolyC.tab.c"
    break;

  case 150:
#line 605 "HolyC.y"
                                         {
 yyval=AppendToStmts(yyvsp[-2],yyvsp[-1]);
 ReleaseAST(yyvsp[0]);
}
#line 3195 "HolyC.tab.c"
    break;

  case 151:
#line 609 "HolyC.y"
       {yyval=NULL;}
#line 3201 "HolyC.tab.c"
    break;

  case 152:
#line 610 "HolyC.y"
                        {
  CType *t=CreateClassForwardDecl(NULL,NULL);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[0]);
}
#line 3211 "HolyC.tab.c"
    break;

  case 153:
#line 615 "HolyC.y"
                        {
  CType *t=CreateUnionForwardDecl(NULL,NULL);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[0]);
}
#line 3221 "HolyC.tab.c"
    break;

  case 154:
#line 620 "HolyC.y"
                                {
  CType *t=CreateClassForwardDecl(NULL,yyvsp[0]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-1]);
}
#line 3231 "HolyC.tab.c"
    break;

  case 155:
#line 625 "HolyC.y"
                                {
  CType *t=CreateUnionForwardDecl(NULL,yyvsp[0]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-1]);
}
#line 3241 "HolyC.tab.c"
    break;

  case 156:
#line 630 "HolyC.y"
                                    {
  CType *t=CreateClassForwardDecl(NULL,yyvsp[0]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-1]);
}
#line 3251 "HolyC.tab.c"
    break;

  case 157:
#line 635 "HolyC.y"
                               {
  CType *t=CreateUnionForwardDecl(NULL,yyvsp[0]);
  yyval=CreateTypeNode(t);
}
#line 3260 "HolyC.tab.c"
    break;

  case 158:
#line 639 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 3266 "HolyC.tab.c"
    break;

  case 159:
#line 640 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 3272 "HolyC.tab.c"
    break;

  case 160:
#line 641 "HolyC.y"
                                                 {
  CType *t=*map_get(&Compiler.types,yyvsp[0]->name);
  InheritFromType(yyvsp[-2]->type2,t);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 3283 "HolyC.tab.c"
    break;

  case 161:
#line 647 "HolyC.y"
                                                 {
  CType *t=*map_get(&Compiler.types,yyvsp[0]->name);
  InheritFromType(yyvsp[-2]->type2,t);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 3294 "HolyC.tab.c"
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
#line 3314 "HolyC.tab.c"
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
#line 3331 "HolyC.tab.c"
    break;

  case 164:
#line 684 "HolyC.y"
                                                   {
  yyval=CreateTry(yyvsp[-2],yyvsp[0]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-1]);
}
#line 3341 "HolyC.tab.c"
    break;

  case 165:
#line 692 "HolyC.y"
                   {
  CType *t =CreatePrimType(TYPE_I64);
  yyval=SOT(AppendToDecls(NULL,t,t,yyvsp[0],NULL,NULL),yyvsp[0]);
}
#line 3350 "HolyC.tab.c"
    break;

  case 166:
#line 696 "HolyC.y"
                            {
  CType *t =CreatePrimType(TYPE_I64);
  CType *ptr =CreateMultiLvlPtr(t,1);
  yyval=SOT(AppendToDecls(NULL,t,ptr,yyvsp[0],NULL,NULL),yyvsp[0]);
  ReleaseAST(yyvsp[-1]);
}
#line 3361 "HolyC.tab.c"
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
#line 3373 "HolyC.tab.c"
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
#line 3386 "HolyC.tab.c"
    break;

  case 170:
#line 718 "HolyC.y"
            {
    yyval=CreateI64(0);
}
#line 3394 "HolyC.tab.c"
    break;

  case 171:
#line 721 "HolyC.y"
                     {
  yyval=CreateI64(1);
  ReleaseAST(yyvsp[0]);
}
#line 3403 "HolyC.tab.c"
    break;

  case 172:
#line 725 "HolyC.y"
                              {
  yyval=CreateI64(2);
  ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 3412 "HolyC.tab.c"
    break;

  case 173:
#line 729 "HolyC.y"
                                       {
  yyval=CreateI64(3);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 3421 "HolyC.tab.c"
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
#line 3438 "HolyC.tab.c"
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
#line 3456 "HolyC.tab.c"
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
#line 3475 "HolyC.tab.c"
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
#line 3488 "HolyC.tab.c"
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
#line 3501 "HolyC.tab.c"
    break;

  case 179:
#line 789 "HolyC.y"
                                                       {
  assert(yyvsp[-2]->type==__AST_DECL_TAILS);
  vec_last(&yyvsp[-2]->declTail).dft=yyvsp[0];
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 3512 "HolyC.tab.c"
    break;

  case 180:
#line 795 "HolyC.y"
                                                         {
  assert(yyvsp[-2]->type==__AST_DECL_TAILS);
  vec_last(&yyvsp[-2]->declTail).dft=yyvsp[0];
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 3523 "HolyC.tab.c"
    break;

  case 181:
#line 802 "HolyC.y"
                                                {
  DeclsFillInBasetype(yyvsp[-1]->type2,yyvsp[0]);
  yyval=yyvsp[0];
}
#line 3532 "HolyC.tab.c"
    break;

  case 182:
#line 806 "HolyC.y"
                                                    {
  DeclsFillInBasetype(yyvsp[-1]->type2,yyvsp[0]);
  yyval=yyvsp[0];
}
#line 3541 "HolyC.tab.c"
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
#line 3553 "HolyC.tab.c"
    break;

  case 186:
#line 821 "HolyC.y"
                                                 {
  DeclsFillInBasetype(yyvsp[-1]->type2,yyvsp[0]);
  yyval=yyvsp[0];
}
#line 3562 "HolyC.tab.c"
    break;

  case 188:
#line 827 "HolyC.y"
                                                   {
  assert(yyvsp[0]->type==__AST_DECL_TAILS);
  CDeclTail b2=vec_last(&yyvsp[0]->declTail);
  yyval=AppendToDecls(yyvsp[-2],b2.basetype,b2.finalType,b2.name,b2.dft,NULL);
  ReleaseAST(yyvsp[-1]);
}
#line 3573 "HolyC.tab.c"
    break;

  case 189:
#line 833 "HolyC.y"
          {yyval=NULL;}
#line 3579 "HolyC.tab.c"
    break;

  case 190:
#line 837 "HolyC.y"
                     {yyval=CreateExternLinkage(NULL);ReleaseAST(yyvsp[0]);}
#line 3585 "HolyC.tab.c"
    break;

  case 191:
#line 838 "HolyC.y"
                     {yyval=CreateImportLinkage(NULL);ReleaseAST(yyvsp[0]);}
#line 3591 "HolyC.tab.c"
    break;

  case 192:
#line 839 "HolyC.y"
                     {yyval=NULL;}
#line 3597 "HolyC.tab.c"
    break;

  case 193:
#line 840 "HolyC.y"
                     {yyval=CreateStaticLinkage();ReleaseAST(yyvsp[0]);}
#line 3603 "HolyC.tab.c"
    break;

  case 194:
#line 841 "HolyC.y"
                              {yyval=CreateExternLinkage(yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3609 "HolyC.tab.c"
    break;

  case 195:
#line 842 "HolyC.y"
                              {yyval=CreateImportLinkage(yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3615 "HolyC.tab.c"
    break;

  case 196:
#line 843 "HolyC.y"
                                  {yyval=CreateExternLinkage(yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3621 "HolyC.tab.c"
    break;

  case 197:
#line 844 "HolyC.y"
                                  {yyval=CreateImportLinkage(yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3627 "HolyC.tab.c"
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
#line 3645 "HolyC.tab.c"
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
#line 3664 "HolyC.tab.c"
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
#line 3682 "HolyC.tab.c"
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
#line 3701 "HolyC.tab.c"
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
#line 3722 "HolyC.tab.c"
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
#line 3741 "HolyC.tab.c"
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
#line 3760 "HolyC.tab.c"
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
#line 3780 "HolyC.tab.c"
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
#line 3798 "HolyC.tab.c"
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
#line 3817 "HolyC.tab.c"
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
#line 3838 "HolyC.tab.c"
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
#line 3856 "HolyC.tab.c"
    break;

  case 210:
#line 1022 "HolyC.y"
                    {yyval=yyvsp[0];}
#line 3862 "HolyC.tab.c"
    break;

  case 211:
#line 1023 "HolyC.y"
                    {yyval=yyvsp[0];}
#line 3868 "HolyC.tab.c"
    break;

  case 213:
#line 1026 "HolyC.y"
          {yyval=NULL;}
#line 3874 "HolyC.tab.c"
    break;

  case 214:
#line 1029 "HolyC.y"
                                                                           {
  yyval=SOT(CreateWhile(yyvsp[-1],NULL),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3884 "HolyC.tab.c"
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
#line 3896 "HolyC.tab.c"
    break;

  case 216:
#line 1043 "HolyC.y"
                                                              {
  yyval=SOT(CreateIf(yyvsp[-1],NULL,NULL),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3906 "HolyC.tab.c"
    break;

  case 217:
#line 1049 "HolyC.y"
                            {
  assert(yyvsp[-1]->type==AST_IF);
  yyval=yyvsp[-1];
  yyvsp[-1]->ifStmt.body=yyvsp[0];
}
#line 3916 "HolyC.tab.c"
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
#line 3928 "HolyC.tab.c"
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
#line 3943 "HolyC.tab.c"
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
#line 3955 "HolyC.tab.c"
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
#line 3970 "HolyC.tab.c"
    break;

  case 223:
#line 1091 "HolyC.y"
                                                                                     {
  yyval=SOT(CreateDo(yyvsp[-1],yyvsp[-4]),yyvsp[-5]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3981 "HolyC.tab.c"
    break;

  case 224:
#line 1097 "HolyC.y"
                                                                 {
  yyval=SOT(CreateSubswitch(yyvsp[-2]),yyvsp[-4]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3992 "HolyC.tab.c"
    break;

  case 225:
#line 1104 "HolyC.y"
                       {yyval=yyvsp[0];}
#line 3998 "HolyC.tab.c"
    break;

  case 226:
#line 1105 "HolyC.y"
                                         {
  yyval=SOT(CreateDefault(),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4007 "HolyC.tab.c"
    break;

  case 227:
#line 1109 "HolyC.y"
                                              {
  yyval=SOT(CreateCase(yyvsp[-1],NULL),yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4016 "HolyC.tab.c"
    break;

  case 228:
#line 1113 "HolyC.y"
                                      {
  yyval=SOT(CreateCase(NULL,NULL),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4025 "HolyC.tab.c"
    break;

  case 229:
#line 1117 "HolyC.y"
                                                                       {
  yyval=SOT(CreateCase(yyvsp[-3],yyvsp[-1]),yyvsp[-4]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);

}
#line 4036 "HolyC.tab.c"
    break;

  case 230:
#line 1123 "HolyC.y"
                              {yyval=yyvsp[0];}
#line 4042 "HolyC.tab.c"
    break;

  case 231:
#line 1124 "HolyC.y"
                                             {yyval=AppendToStmts(yyvsp[-1],yyvsp[0]);}
#line 4048 "HolyC.tab.c"
    break;

  case 232:
#line 1125 "HolyC.y"
                                {yyval=yyvsp[0];}
#line 4054 "HolyC.tab.c"
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
#line 4066 "HolyC.tab.c"
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
#line 4078 "HolyC.tab.c"
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
#line 4091 "HolyC.tab.c"
    break;

  case 237:
#line 1149 "HolyC.y"
                                          {
  vec_push(&yyvsp[-2]->stmts,yyvsp[-1]);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[0]);
}
#line 4101 "HolyC.tab.c"
    break;

  case 238:
#line 1154 "HolyC.y"
                                 {
    yyval=yyvsp[-1];
    ReleaseAST(yyvsp[0]);
}
#line 4110 "HolyC.tab.c"
    break;

  case 239:
#line 1158 "HolyC.y"
                                 {
    yyval=yyvsp[-1];
    ReleaseAST(yyvsp[0]);
}
#line 4119 "HolyC.tab.c"
    break;

  case 242:
#line 1164 "HolyC.y"
                                       {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[0]);
}
#line 4128 "HolyC.tab.c"
    break;

  case 243:
#line 1168 "HolyC.y"
                                                  {
  yyval=ApplyLinkageToDecls(yyvsp[-2],yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4137 "HolyC.tab.c"
    break;

  case 245:
#line 1173 "HolyC.y"
                                         {
  yyval=SOT(CreateGoto(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4146 "HolyC.tab.c"
    break;

  case 246:
#line 1177 "HolyC.y"
                                   {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[0]);
}
#line 4155 "HolyC.tab.c"
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
#line 4167 "HolyC.tab.c"
    break;

  case 248:
#line 1188 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 4173 "HolyC.tab.c"
    break;

  case 249:
#line 1189 "HolyC.y"
                         {yyval=yyvsp[0];}
#line 4179 "HolyC.tab.c"
    break;

  case 250:
#line 1190 "HolyC.y"
                                  {
  yyval=SOT(CreateBreak(),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4188 "HolyC.tab.c"
    break;

  case 251:
#line 1194 "HolyC.y"
                         {
  yyval=CreateNop();
  ReleaseAST(yyvsp[0]);
}
#line 4197 "HolyC.tab.c"
    break;

  case 252:
#line 1198 "HolyC.y"
                                           {
  yyval=SOT(CreateReturn(yyvsp[-1]),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4206 "HolyC.tab.c"
    break;

  case 253:
#line 1202 "HolyC.y"
                                 {
  yyval=SOT(CreateReturn(NULL),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4215 "HolyC.tab.c"
    break;

  case 254:
#line 1206 "HolyC.y"
                                  {
  yyval=SOT(CreateLabel(yyvsp[-1]),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4224 "HolyC.tab.c"
    break;

  case 255:
#line 1210 "HolyC.y"
                                      {yyval=yyvsp[-1];ReleaseAST(yyvsp[0]);}
#line 4230 "HolyC.tab.c"
    break;

  case 256:
#line 1211 "HolyC.y"
                                                        {
  CType *t=CreateUnionForwardDecl(yyvsp[-3],yyvsp[-1]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4241 "HolyC.tab.c"
    break;

  case 257:
#line 1217 "HolyC.y"
                                                        {
  CType *t=CreateClassForwardDecl(yyvsp[-3],yyvsp[-1]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4252 "HolyC.tab.c"
    break;

  case 258:
#line 1223 "HolyC.y"
                    {yyval=yyvsp[0];}
#line 4258 "HolyC.tab.c"
    break;

  case 259:
#line 1224 "HolyC.y"
                             {yyval=AppendToStmts(yyvsp[-1],yyvsp[0]);}
#line 4264 "HolyC.tab.c"
    break;

  case 260:
#line 1225 "HolyC.y"
                   {YYABORT;}
#line 4270 "HolyC.tab.c"
    break;

  case 261:
#line 1226 "HolyC.y"
                                                    {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4280 "HolyC.tab.c"
    break;

  case 262:
#line 1231 "HolyC.y"
                                           {
  yyval=CreateNop();
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4290 "HolyC.tab.c"
    break;

  case 263:
#line 1236 "HolyC.y"
                {yyval=NULL;}
#line 4296 "HolyC.tab.c"
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
#line 4315 "HolyC.tab.c"
    break;

  case 265:
#line 1251 "HolyC.y"
                                {
  yyval=NULL;
}
#line 4323 "HolyC.tab.c"
    break;

  case 266:
#line 1255 "HolyC.y"
                             {
  RunStatement(yyvsp[0]);
  YYACCEPT;
  yyval=NULL;
}
#line 4333 "HolyC.tab.c"
    break;

  case 267:
#line 1260 "HolyC.y"
                                      {
  RunPtr(CurFuncInfo,yyvsp[-1],CurFramePtr);
  YYACCEPT;
  yyval=NULL;
}
#line 4343 "HolyC.tab.c"
    break;

  case 268:
#line 1265 "HolyC.y"
                                     {
  RunPtr(NULL,yyvsp[0],NULL);
  YYACCEPT;
  yyval=NULL;
}
#line 4353 "HolyC.tab.c"
    break;

  case 269:
#line 1271 "HolyC.y"
                                  {
  RunPtr(CurFuncInfo,yyvsp[0],CurFramePtr);
  YYACCEPT;
  yyval=NULL;
}
#line 4363 "HolyC.tab.c"
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
#line 4376 "HolyC.tab.c"
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
#line 4390 "HolyC.tab.c"
    break;

  case 272:
#line 1297 "HolyC.y"
                 {yyval=SLE(yyvsp[0]);}
#line 4396 "HolyC.tab.c"
    break;

  case 273:
#line 1298 "HolyC.y"
               {yyval=SLE(yyvsp[0]);}
#line 4402 "HolyC.tab.c"
    break;

  case 274:
#line 1299 "HolyC.y"
                {yyval=SLE(yyvsp[0]);}
#line 4408 "HolyC.tab.c"
    break;

  case 275:
#line 1300 "HolyC.y"
                  {yyval=SLE(yyvsp[0]);}
#line 4414 "HolyC.tab.c"
    break;

  case 276:
#line 1301 "HolyC.y"
                     {
  yyval=SLE(yyvsp[0]);
}
#line 4422 "HolyC.tab.c"
    break;

  case 277:
#line 1304 "HolyC.y"
                {
  yyval=SLE(yyvsp[0]);
}
#line 4430 "HolyC.tab.c"
    break;

  case 279:
#line 1308 "HolyC.y"
                                                          {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 4439 "HolyC.tab.c"
    break;

  case 281:
#line 1313 "HolyC.y"
                                     {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4448 "HolyC.tab.c"
    break;

  case 282:
#line 1317 "HolyC.y"
                                        {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4457 "HolyC.tab.c"
    break;

  case 283:
#line 1321 "HolyC.y"
                                    {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4466 "HolyC.tab.c"
    break;

  case 284:
#line 1325 "HolyC.y"
                                      {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4475 "HolyC.tab.c"
    break;

  case 285:
#line 1329 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[-1],AST_POST_DEC),yyvsp[0]);
  ReleaseAST(yyvsp[0]);
}
#line 4484 "HolyC.tab.c"
    break;

  case 286:
#line 1333 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[-1],AST_POST_INC),yyvsp[0]);
  ReleaseAST(yyvsp[0]);
}
#line 4493 "HolyC.tab.c"
    break;

  case 287:
#line 1337 "HolyC.y"
                                                                  {
  yyval=SOT(CreateFuncCall(yyvsp[-3],yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 4502 "HolyC.tab.c"
    break;

  case 288:
#line 1341 "HolyC.y"
                                                                   {
  yyval=CreateExplicitTypecast(yyvsp[-3],yyvsp[-1]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 4511 "HolyC.tab.c"
    break;

  case 290:
#line 1346 "HolyC.y"
                             {
  yyval=SOT(CreateSizeof(yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4520 "HolyC.tab.c"
    break;

  case 291:
#line 1350 "HolyC.y"
                                    {
  yyval=SOT(CreateSizeof(yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4529 "HolyC.tab.c"
    break;

  case 292:
#line 1354 "HolyC.y"
                                                                     {
  yyval=SOT(CreateSizeof(yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4540 "HolyC.tab.c"
    break;

  case 293:
#line 1360 "HolyC.y"
                                                                      {
  yyval=SOT(CreateSizeof(yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4551 "HolyC.tab.c"
    break;

  case 294:
#line 1366 "HolyC.y"
                             {
  yyval=SOT(CreateUnop(yyvsp[0],AST_ADDROF),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4560 "HolyC.tab.c"
    break;

  case 295:
#line 1370 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[0],AST_DERREF),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4569 "HolyC.tab.c"
    break;

  case 296:
#line 1375 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[0],AST_PRE_DEC),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4578 "HolyC.tab.c"
    break;

  case 297:
#line 1379 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[0],AST_PRE_INC),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4587 "HolyC.tab.c"
    break;

  case 298:
#line 1383 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[0],AST_POS),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4596 "HolyC.tab.c"
    break;

  case 299:
#line 1387 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[0],AST_NEG),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4605 "HolyC.tab.c"
    break;

  case 300:
#line 1391 "HolyC.y"
                             {
  yyval=SOT(CreateUnop(yyvsp[0],AST_LNOT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4614 "HolyC.tab.c"
    break;

  case 301:
#line 1395 "HolyC.y"
                             {
  yyval=SOT(CreateUnop(yyvsp[0],AST_BNOT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4623 "HolyC.tab.c"
    break;

  case 303:
#line 1402 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_POW),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4632 "HolyC.tab.c"
    break;

  case 304:
#line 1406 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SHL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4641 "HolyC.tab.c"
    break;

  case 305:
#line 1410 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SHR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4650 "HolyC.tab.c"
    break;

  case 307:
#line 1416 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_MUL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4659 "HolyC.tab.c"
    break;

  case 308:
#line 1420 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_DIV),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4668 "HolyC.tab.c"
    break;

  case 309:
#line 1424 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_MOD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4677 "HolyC.tab.c"
    break;

  case 311:
#line 1430 "HolyC.y"
                                       {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4686 "HolyC.tab.c"
    break;

  case 313:
#line 1436 "HolyC.y"
                                       {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4695 "HolyC.tab.c"
    break;

  case 315:
#line 1442 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4704 "HolyC.tab.c"
    break;

  case 317:
#line 1448 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ADD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4713 "HolyC.tab.c"
    break;

  case 318:
#line 1452 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SUB),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4722 "HolyC.tab.c"
    break;

  case 320:
#line 1458 "HolyC.y"
                                     {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_LT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4731 "HolyC.tab.c"
    break;

  case 321:
#line 1462 "HolyC.y"
                                     {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_GT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4740 "HolyC.tab.c"
    break;

  case 322:
#line 1466 "HolyC.y"
                                     {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_LE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4749 "HolyC.tab.c"
    break;

  case 323:
#line 1470 "HolyC.y"
                                     {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_GE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4758 "HolyC.tab.c"
    break;

  case 325:
#line 1476 "HolyC.y"
                                        {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_EQ),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4767 "HolyC.tab.c"
    break;

  case 326:
#line 1480 "HolyC.y"
                                        {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_NE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4776 "HolyC.tab.c"
    break;

  case 328:
#line 1486 "HolyC.y"
                                          {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4785 "HolyC.tab.c"
    break;

  case 330:
#line 1492 "HolyC.y"
                                          {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4794 "HolyC.tab.c"
    break;

  case 332:
#line 1498 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4803 "HolyC.tab.c"
    break;

  case 334:
#line 1504 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4812 "HolyC.tab.c"
    break;

  case 335:
#line 1508 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SHL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4821 "HolyC.tab.c"
    break;

  case 336:
#line 1512 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SHR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4830 "HolyC.tab.c"
    break;

  case 337:
#line 1516 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_MUL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4839 "HolyC.tab.c"
    break;

  case 338:
#line 1520 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_DIV),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4848 "HolyC.tab.c"
    break;

  case 339:
#line 1524 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_MOD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4857 "HolyC.tab.c"
    break;

  case 340:
#line 1528 "HolyC.y"
                                             {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4866 "HolyC.tab.c"
    break;

  case 341:
#line 1532 "HolyC.y"
                                             {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4875 "HolyC.tab.c"
    break;

  case 342:
#line 1536 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4884 "HolyC.tab.c"
    break;

  case 343:
#line 1540 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_ADD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4893 "HolyC.tab.c"
    break;

  case 344:
#line 1544 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SUB),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4902 "HolyC.tab.c"
    break;


#line 4906 "HolyC.tab.c"

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
#line 1550 "HolyC.y"

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
    ExceptBuf osp;
    memcpy(&osp,&SigPad,sizeof(SigPad));
    if(!HCSetJmp(&SigPad)) {
        if(bt->type==TYPE_F64) {
            ret.f=EvaluateF64(node);
        } else {
            ret.i=EvaluateInt(node,0);
        }
    } else {
        ARM_SIGNALS;
    }
    memcpy(&SigPad,&osp,sizeof(SigPad));
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
int64_t EvalExprNoComma(char *text,char **en) {
    CLexer old=ForkLexer(PARSER_HOLYC);
    mrope_append_text(Lexer.source,text);
    Lexer.isEvalNoCommaMode=1;
    RunPtr=__EvalExprNoComma;
    yyparse();
    if(en) *en=text+Lexer.cursor_pos;
    Lexer=old;
    return exprret;
}
