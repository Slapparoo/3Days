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
#define YYFINAL  95
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1693

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  102
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  95
/* YYNRULES -- Number of rules.  */
#define YYNRULES  349
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  626

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
     524,   529,   539,   540,   541,   548,   549,   550,   551,   552,
     557,   563,   572,   575,   582,   583,   587,   593,   597,   601,
     604,   607,   608,   613,   618,   623,   628,   633,   637,   638,
     639,   645,   651,   655,   659,   663,   668,   673,   688,   704,
     712,   716,   722,   729,   737,   738,   741,   745,   749,   753,
     765,   778,   792,   800,   809,   815,   822,   826,   831,   838,
     839,   841,   846,   847,   853,   857,   858,   859,   860,   861,
     862,   863,   864,   868,   881,   895,   908,   922,   938,   952,
     966,   981,   994,  1008,  1024,  1042,  1043,  1045,  1046,  1049,
    1055,  1063,  1069,  1074,  1081,  1092,  1093,  1100,  1111,  1117,
    1124,  1125,  1129,  1133,  1137,  1143,  1144,  1145,  1146,  1153,
    1160,  1161,  1169,  1174,  1175,  1176,  1180,  1184,  1185,  1189,
    1193,  1200,  1201,  1202,  1206,  1210,  1214,  1218,  1222,  1223,
    1224,  1230,  1236,  1237,  1238,  1239,  1244,  1249,  1250,  1255,
    1259,  1264,  1269,  1275,  1284,  1292,  1301,  1302,  1303,  1304,
    1305,  1308,  1311,  1312,  1316,  1317,  1321,  1325,  1329,  1333,
    1337,  1341,  1345,  1349,  1350,  1354,  1358,  1364,  1370,  1374,
    1379,  1383,  1387,  1391,  1395,  1399,  1405,  1406,  1410,  1414,
    1419,  1420,  1424,  1428,  1433,  1434,  1439,  1440,  1445,  1446,
    1451,  1452,  1456,  1461,  1462,  1466,  1470,  1474,  1479,  1480,
    1484,  1489,  1490,  1495,  1496,  1501,  1502,  1507,  1508,  1512,
    1516,  1520,  1524,  1528,  1532,  1536,  1540,  1544,  1548,  1552
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

#define YYPACT_NINF (-532)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     206,  1577,   -30,  1577,  1577,  1073,    38,  -532,  -532,  -532,
      78,  1577,  1577,  1577,  1577,  1577,  1577,  -532,  1577,  1577,
    1577,  -532,  -532,   264,    21,  -532,  -532,    55,  -532,   324,
     510,    52,    98,   106,    33,   419,   252,   132,   121,   138,
     316,  -532,   981,  -532,  -532,   -20,   150,   -30,   157,  -532,
     155,  1073,   200,   245,   -26,  -532,  -532,   -25,  -532,  -532,
     -13,   -12,   170,  1331,   130,   256,   264,  1165,   221,   254,
    -532,   131,    14,   262,   270,   135,   169,  -532,  -532,   317,
     343,  -532,   352,    68,  -532,  -532,  1073,  1073,  -532,  -532,
     371,  -532,  -532,   227,  -532,  -532,  1174,  -532,  -532,   406,
     378,   385,   395,  -532,  -532,  -532,  -532,  -532,  -532,  -532,
    -532,   208,  -532,  -532,   432,   439,  -532,  -532,   446,   467,
    1174,  -532,  -532,  1577,  1577,  1577,  1577,  1577,  1577,  1577,
    1577,  1577,  1577,  1577,  1577,  1577,  1577,  1577,  1577,  1577,
    1577,  1577,  1577,  1577,  1577,  1577,  1577,  1577,  1577,  1577,
    1577,  1577,  1577,  1577,  1577,  -532,  -532,   194,  1577,  -532,
     487,  -532,  1577,   478,  1577,  1577,  -532,  -532,  -532,  -532,
    -532,  -532,  -532,  -532,  -532,  -532,   133,   505,  1577,  -532,
    -532,  -532,    80,  1599,  1599,  1599,  1599,  1599,  1599,  -532,
    1577,  1599,  1599,  -532,    18,  -532,   444,  -532,    28,  -532,
    -532,   498,   522,  -532,  -532,    40,  -532,   517,   524,   551,
     550,   552,   301,   477,   286,   544,   545,   546,   327,   526,
     234,  -532,  -532,    48,  -532,   525,   533,    66,   535,  -532,
     541,   500,   501,  -532,  -532,  -532,  -532,  -532,  -532,  -532,
      -8,    31,    14,   538,  -532,  -532,  -532,   531,  -532,   539,
      82,   216,  -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,
     229,  -532,    27,   185,  -532,  -532,  -532,  -532,  -532,  -532,
    -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,
    -532,  -532,  -532,  -532,   316,   316,   316,   316,   316,   316,
     316,   316,   316,   316,   316,  -532,  -532,  -532,  -532,   -30,
     238,   547,   258,   150,   542,  -532,  -532,   259,  1174,  -532,
     406,   368,  -532,  -532,  -532,  -532,  -532,  -532,   263,  -532,
    -532,  -532,  -532,   366,   548,   555,   559,   532,  -532,   526,
    -532,  -532,  1353,  1165,   562,   567,  1174,  -532,  -532,  1599,
    1599,  1599,  1599,  1599,  1599,  1599,  1599,  1599,  1599,  1599,
    1599,  1599,  1599,  1599,  1599,  1599,  1599,  1599,  1599,  1599,
    1599,  1599,  1599,  1599,  1599,  1599,  1599,  1599,  1599,  1599,
    -532,    43,  -532,  -532,   568,    32,  1577,  1577,  1577,  1577,
     594,  -532,  -532,   248,    89,  -532,  -532,   -22,  1408,  1430,
     584,   105,  -532,  -532,   117,   139,   549,   570,   574,  -532,
    1073,  -532,  -532,  -532,  1577,  -532,   118,  -532,  -532,  -532,
    -532,  1577,  -532,  1577,   565,   122,   268,  -532,  1485,  1577,
    -532,  -532,  -532,  1353,  -532,   498,  -532,  -532,   272,   -16,
    -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,
    -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,
     327,   327,   327,   327,   327,   327,   327,   327,   327,   327,
     327,  -532,  -532,  -532,   134,   536,   563,   176,  -532,   177,
     179,   180,   560,  -532,  -532,   577,  -532,   581,    14,  -532,
     292,  1224,  -532,  -532,  -532,   204,   107,  -532,  -532,  -532,
      14,   578,  -532,  -532,  -532,  -532,  -532,    75,  -532,  -532,
    -532,   123,  -532,   328,   582,   613,  -532,  -532,   617,   623,
    -532,  -532,  -532,  -532,  -532,   587,  -532,  -532,  -532,  1577,
    -532,  -532,  -532,  -532,   636,  -532,   182,    66,  -532,   110,
     315,   579,  -532,    -6,  -532,  -532,   108,   593,   600,  -532,
     408,  -532,   595,   330,   596,  -532,  -532,  1577,  -532,  1507,
     583,   585,  -532,  -532,  -532,   705,   554,   644,  -532,  -532,
    -532,  -532,  -532,   601,  -532,  -532,  -532,  -532,  1276,  -532,
     599,   603,   605,   105,  1577,  -532,   329,   143,   334,  -532,
     606,  -532,   409,   889,  -532,  -532,  -532,  -532,  -532,   336,
    -532,  -532,  -532,   607,   610,   136,  -532,  -532,  -532,  -532,
     611,  -532,  -532,  -532,  -532,  1577,   797,  -532,  -532,   614,
     136,   353,   402,   590,   602,   136,   415,   136,  -532,  -532,
    -532,  -532,  -532,   450,  -532,  -532
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     267,     0,     0,     0,     0,   269,     0,    47,    46,    45,
       0,     0,     0,     0,     0,     0,     0,    49,     0,     0,
       0,    50,    48,     0,     0,    55,    57,    72,    85,    89,
      93,    95,    97,    99,   102,   107,   110,   112,   114,   116,
     128,   272,     0,   270,   130,     0,   273,     0,    50,   254,
       0,     0,     0,     0,     0,   195,   196,     0,   198,   197,
     152,   153,     0,     0,     0,     0,   131,    21,     0,     0,
     244,     0,   175,   158,   159,     0,     0,   258,   259,   132,
     133,   240,     0,     0,   247,   268,     0,     0,   215,   216,
       0,   251,   225,   243,   252,     1,     0,    73,   131,    74,
      70,     0,     0,   132,   133,    83,    84,    78,    77,    81,
      82,     0,    80,    79,     0,     0,    44,    43,     0,     0,
      51,    64,    63,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   266,   262,     0,     0,   271,
       0,   257,     0,     0,     0,   218,   199,   201,   200,   202,
     154,   156,   155,   157,   253,   256,     0,     0,     0,   278,
     277,   276,     0,     0,     0,     0,     0,     0,     0,   280,
       0,     0,     0,   281,     0,   279,     0,    14,     0,    11,
      13,    19,    22,   282,   284,   293,   306,   310,   314,   316,
     318,   320,   323,   328,   331,   333,   335,   337,   349,    15,
       0,   241,   250,   176,   170,   134,   174,   190,     0,   189,
     191,     0,     0,   163,   151,   164,   151,   162,   166,   245,
     152,   153,   175,     0,   224,   227,   222,   216,   249,     0,
       0,     0,    71,   134,    56,    58,    59,    60,    61,    54,
       0,    52,     0,     0,    86,    87,    88,    90,    91,    92,
      94,    96,    98,   100,   101,   103,   104,   105,   106,   109,
     108,   111,   113,   115,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   264,   265,   263,   129,     0,
       0,     0,     0,   217,     0,   255,   248,     0,     0,   294,
     295,     0,   304,   305,   299,   298,   302,   303,     0,   301,
     300,     6,     2,     7,     0,     0,     0,     0,    16,     0,
     275,   274,     0,     0,     0,     0,    51,   290,   289,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      12,     0,    37,    39,     0,     0,     0,     0,     0,     0,
       0,    33,    40,     0,   177,   171,   165,   194,     0,     0,
       0,   175,   160,   161,     0,     0,   154,   155,   174,   246,
       0,   242,    76,    75,    51,    65,     0,    66,    62,   169,
     221,     0,   219,   218,     0,     0,     0,   283,     0,     0,
      10,   285,   286,     0,    18,    20,   287,   288,     0,     0,
     307,   308,   309,   311,   312,   313,   315,   317,   319,   321,
     322,   324,   325,   326,   327,   330,   329,   332,   334,   336,
     338,   339,   340,   341,   342,   343,   344,   345,   346,   347,
     348,    24,    23,    34,     0,     0,     0,     0,    27,     0,
       0,     0,     0,    42,    41,   178,   172,     0,   175,   192,
       0,     0,   184,   185,   183,     0,     0,   174,   188,   167,
     175,     0,   150,   149,   168,   261,   260,   194,   223,   226,
      53,     0,    67,     0,     0,     0,   297,   296,    46,     3,
       8,     9,    17,   291,   292,     0,    36,    26,    25,     0,
      29,    30,    31,    32,     0,   173,     0,   186,   187,     0,
       0,     0,   137,     0,   138,   182,     0,     0,   147,   144,
     145,   148,     0,     0,     0,    68,   228,   218,   239,     0,
       0,     0,   230,   235,   237,     0,     0,     0,    35,    28,
      38,   205,   209,     0,   193,   203,   211,   141,     0,   139,
       0,     0,     0,   175,     0,   146,     0,     0,     0,    69,
       0,   233,     0,     0,   231,   238,   236,     4,     5,     0,
     140,   135,   136,     0,     0,   194,   143,   142,   208,   212,
       0,   206,   214,   220,   232,     0,     0,   204,   210,     0,
     194,     0,     0,     0,     0,   194,     0,     0,   179,   207,
     213,   234,   229,     0,   180,   181
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -532,  -532,  -532,  -202,  -187,   337,  -532,   -85,   284,  -231,
    -532,  -532,  -532,  -532,   273,   342,  -532,  -532,   -81,   456,
     418,   420,   561,   553,   557,   376,   370,   373,   556,   558,
     537,   266,     4,    16,    -9,  -532,  -453,  -532,  -532,   125,
    -532,   463,  -532,  -532,     6,     9,   127,   151,    30,    -1,
    -532,   -59,  -460,  -532,   223,  -508,   314,   628,  -481,  -532,
    -532,     7,  -403,  -532,  -532,   -80,   -84,  -532,  -532,  -531,
     129,  -532,  -532,  -532,  -532,    -2,  -532,  -532,  -532,  -532,
    -532,   295,   211,   213,   361,   372,   374,   210,   165,   212,
     363,   360,   364,    72,   -60
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,   323,   324,   199,   200,   201,   202,    69,   382,   467,
     464,    70,   383,    25,   259,   260,    26,    27,    99,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    44,    71,    72,   533,   483,   575,   538,   539,
     491,   394,    73,    74,   101,   102,    77,    78,   103,   104,
      81,   487,   227,   228,   229,   479,   230,    82,   480,    83,
      84,   552,   304,    86,    87,    88,    89,    90,   553,   554,
     555,    91,    92,    93,   157,    94,     5,     6,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   329
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      43,   100,   245,   247,    80,    41,   244,   219,   249,   328,
     504,    75,    85,   226,    76,   250,   543,   370,   527,    45,
      46,   564,   321,   158,   586,   116,   166,   168,   534,   223,
     540,    42,   330,   514,   111,    79,   465,   568,    95,   170,
     172,    80,   406,   477,   396,   160,   334,   335,    75,   156,
      80,    76,    60,    61,   133,   134,   569,    75,   163,    61,
      76,   118,   119,   384,   167,   169,   224,   159,    98,   564,
     130,   225,    79,   117,   242,   586,   407,   171,   173,   176,
     331,    79,   171,   397,   466,    80,    80,   100,   336,    61,
     337,   338,    75,    75,   246,    76,    76,   252,   388,   253,
     385,   310,    61,   120,   475,   121,   122,   461,   123,   564,
     322,   262,   251,   540,   611,   592,    79,    79,   131,   389,
     223,   173,   536,   570,   261,   132,    96,   370,   308,   616,
      97,   402,   309,   501,   623,   381,   462,   252,   544,   263,
     542,   476,   240,   241,   580,   424,   469,   470,   471,    60,
      61,   142,    60,    61,    60,    61,    80,   224,    98,   537,
     571,   141,   298,    75,   297,    98,    76,   502,    98,   143,
      98,   506,   545,   100,   158,   563,   158,   515,   300,   489,
     302,   303,   177,   398,    60,    61,   222,    79,   305,   516,
     233,    60,    61,   158,   307,   295,   234,     7,     8,     9,
      98,   494,    10,   162,    11,    12,   318,    98,   600,    13,
      60,    61,    14,    60,    61,    15,    16,    60,    61,   519,
     519,   161,   519,   519,   235,   174,    98,   415,   158,    98,
     236,   520,   521,    98,   522,   523,   512,   561,    47,   408,
      17,   253,    18,    42,    19,    20,    48,   158,   164,    49,
      50,   158,    51,    52,    53,    42,   296,   254,   535,   158,
      54,   253,    55,    56,    57,   403,    58,    59,    60,    61,
     114,   115,   404,   219,    62,    63,    64,    65,   405,   139,
     140,   158,   220,    22,    66,    67,   371,   410,    24,   372,
       1,     2,     3,   165,     4,    68,   373,   409,   381,   100,
     371,   158,   158,   372,   178,   374,   158,   412,   414,   221,
     473,   158,   417,   354,   355,   404,   499,   507,    67,   374,
     498,   513,   348,   349,   416,    67,   231,   429,   375,   376,
     377,   378,   379,   380,   232,   529,   124,   125,   126,    67,
     261,   530,   375,   376,   377,   378,   379,   380,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   359,
     360,   361,   362,   363,   364,   365,   366,   367,   368,   369,
     565,   158,   237,   577,   325,   326,    42,   546,   478,   578,
     468,   468,   468,   468,   598,   490,   490,   418,   419,   601,
      42,   607,   482,    80,    80,    42,   617,    42,   238,    80,
      75,    75,   618,    76,    76,   485,    75,   239,   261,    76,
     284,   285,   286,   287,   288,   289,   290,   291,   292,   293,
     294,   252,   510,   511,    79,    79,   248,   503,   253,   303,
      79,   450,   451,   452,   453,   454,   455,   456,   457,   458,
     459,   460,   135,   136,   137,   138,   234,   179,   180,   181,
     325,   326,   182,    61,   183,   184,   236,   619,   617,   185,
     574,   389,   186,    42,   624,   187,   188,   105,   106,   107,
     108,   109,   110,   604,   605,   112,   113,   253,   312,   313,
     314,   315,   316,   317,   255,   532,   319,   320,   478,   225,
     189,   256,   190,   617,   191,   192,   193,   194,   257,   625,
     350,   351,   352,   353,    80,   275,   276,   277,   278,   273,
     274,    75,   279,   280,    76,   441,   442,   443,   444,   258,
     478,   492,   492,   559,   562,   127,   128,   129,   566,   339,
     340,   341,   299,   195,   311,    79,   327,   301,   198,   342,
     343,   344,   264,   265,   266,   493,   493,   267,   268,   269,
     430,   431,   432,   582,    80,   433,   434,   435,   439,   440,
     306,    75,   332,   303,    76,   333,   445,   446,   478,   345,
     346,   347,   591,   356,   599,   357,   602,   358,   597,   194,
     386,   387,    80,   390,   391,    79,   478,   608,   400,    75,
     392,   393,    76,   399,   401,   411,   423,   413,   472,   486,
     517,   478,   420,   524,   495,    80,   478,   421,   478,   613,
     620,   422,    75,    79,   426,    76,     7,     8,     9,   427,
     463,    10,   497,    11,    12,   496,   505,   518,    13,   525,
     526,    14,   556,   541,    15,    16,    79,   547,   557,   558,
     560,   567,   572,   573,   576,   579,   587,   583,   588,   584,
     589,   593,   594,   595,   621,   603,   609,    47,   610,    17,
     612,    18,   615,    19,    20,    48,   622,   474,    49,    50,
     425,    51,    52,    53,    42,   548,   549,   500,   428,    54,
     283,    55,    56,    57,   271,    58,    59,    60,    61,   272,
     550,   270,   551,    62,    63,    64,    65,   281,   596,   395,
     282,   528,    22,    66,    67,   488,   436,    24,     7,     8,
       9,   243,   606,    10,    68,    11,    12,   448,   437,   447,
      13,   438,   449,    14,     0,     0,    15,    16,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    47,
       0,    17,     0,    18,     0,    19,    20,    48,     0,     0,
      49,    50,     0,    51,    52,    53,    42,   585,   549,     0,
       0,    54,     0,    55,    56,    57,     0,    58,    59,    60,
      61,     0,   550,     0,   551,    62,    63,    64,    65,     0,
       0,     0,     0,     0,    22,    66,    67,     0,     0,    24,
       7,     8,     9,     0,     0,    10,    68,    11,    12,     0,
       0,     0,    13,     0,     0,    14,     0,     0,    15,    16,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    47,     0,    17,     0,    18,     0,    19,    20,    48,
       0,     0,    49,    50,     0,    51,    52,    53,    42,     0,
     549,     0,     0,    54,     0,    55,    56,    57,     0,    58,
      59,    60,    61,     0,   550,   614,   551,    62,    63,    64,
      65,     0,     0,     0,     0,     0,    22,    66,    67,     0,
       0,    24,     7,     8,     9,     0,     0,    10,    68,    11,
      12,     0,     0,     0,    13,     0,     0,    14,     0,     0,
      15,    16,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    47,     0,    17,     0,    18,     0,    19,
      20,    48,     0,     0,    49,    50,     0,    51,    52,    53,
      42,     0,   549,     0,     0,    54,     0,    55,    56,    57,
       0,    58,    59,    60,    61,     0,   550,     0,   551,    62,
      63,    64,    65,     0,     0,     0,     0,     0,    22,    66,
      67,     0,     0,    24,     7,     8,     9,     0,     0,    10,
      68,    11,    12,     0,     0,     0,    13,     0,     0,    14,
       0,     0,    15,    16,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    47,     0,    17,     0,    18,
       0,    19,    20,    48,     0,     0,    49,    50,     0,    51,
      52,    53,    42,   155,     0,     0,     0,    54,     0,    55,
      56,    57,     0,    58,    59,    60,    61,     0,     0,     0,
       0,    62,    63,    64,    65,     0,     0,     0,     0,     0,
      22,    66,    67,     0,     0,    24,     7,     8,     9,     0,
       0,    10,    68,    11,    12,     0,     0,     0,    13,     0,
       0,    14,     0,     0,    15,    16,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    47,     0,    17,
       0,    18,     0,    19,    20,    48,     0,     0,    49,    50,
       0,    51,    52,    53,    42,     0,     0,     0,     0,    54,
       0,    55,    56,    57,     0,    58,    59,    60,    61,     0,
       0,     0,     0,    62,    63,    64,    65,     0,     0,     0,
       0,     0,    22,    66,    67,     0,     0,    24,   179,   180,
     181,     0,     0,   182,    68,   183,   184,     7,     8,     9,
     185,     0,    10,   186,    11,    12,   187,   188,     0,    13,
       0,     0,    14,     0,     0,    15,    16,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   189,     0,   190,     0,   191,   192,   193,   194,     0,
      17,     0,    18,     0,    19,    20,    21,     7,     8,     9,
       0,     0,    10,     0,    11,    12,     0,     0,     0,    13,
       0,     0,    14,     0,     0,    15,    16,     0,    60,    61,
       0,     0,     0,     0,   195,   196,     0,   197,     0,   198,
       0,     0,     0,    22,    66,     0,     0,   531,    24,     0,
      17,     0,    18,     0,    19,    20,    21,     0,     0,     7,
       8,     9,     0,     0,    10,   481,    11,    12,     0,     0,
       0,    13,     0,     0,    14,     0,     0,    15,    16,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    22,    23,     0,     0,     0,    24,     0,
       0,     0,    17,     0,    18,     0,    19,    20,    21,     0,
       0,     0,     0,     0,     7,     8,     9,   481,   590,    10,
       0,    11,    12,     0,     0,     0,    13,     0,     0,    14,
       0,     0,    15,    16,     0,     0,   179,   180,   181,     0,
       0,   182,     0,   183,   184,    22,    23,     0,   185,     0,
      24,   186,     0,     0,   187,   188,     0,    17,     0,    18,
       0,    19,    20,    21,     0,     0,   175,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   189,
       0,   190,     0,   191,   192,   193,   194,     0,     0,     0,
       0,     7,     8,     9,     0,     0,    10,     0,    11,    12,
      22,    23,     0,    13,     0,    24,    14,     0,     0,    15,
      16,     0,     0,     7,     8,     9,     0,     0,    10,     0,
      11,    12,   195,   311,     0,    13,     0,   198,    14,     0,
       0,    15,    16,     0,    17,     0,    18,     0,    19,    20,
      21,     0,     0,     0,     0,     0,     0,     0,     0,   481,
       0,     0,     0,     0,     0,     0,    17,     0,    18,     0,
      19,    20,    21,     0,   484,     0,     0,     0,     7,   508,
       9,     0,     0,    10,     0,    11,    12,    22,    23,     0,
      13,     0,    24,    14,     0,     0,    15,    16,     0,     0,
       7,     8,     9,     0,     0,    10,     0,    11,    12,    22,
      23,     0,    13,     0,    24,    14,     0,     0,    15,    16,
       0,    17,     0,    18,     0,    19,    20,    21,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    17,     0,    18,     0,    19,    20,    21,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   581,     0,     0,    22,    23,     0,   509,     0,    24,
       7,     8,     9,     0,     0,    10,     0,    11,    12,     0,
       0,     0,    13,     0,     0,    14,    22,    23,    15,    16,
       0,    24,   179,   180,   181,     0,     0,   182,     0,   183,
     184,     0,     0,     0,   185,     0,     0,   186,     0,     0,
     187,   188,     0,    17,     0,    18,     0,    19,    20,    21,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   189,     0,   190,     0,   191,
     192,   193,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    22,    23,     0,     0,
       0,    24,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   195,   311,
       0,     0,     0,   198
};

static const yytype_int16 yycheck[] =
{
       2,    10,    86,    87,     5,     1,    86,    67,    93,   196,
     413,     5,     5,    72,     5,    96,   497,   219,   478,     3,
       4,   529,     4,    43,   555,     4,    52,    52,   481,    15,
     490,    61,     4,    49,    18,     5,     4,    43,     0,    52,
      52,    42,    15,    65,    52,    47,     6,     7,    42,    42,
      51,    42,    74,    75,    21,    22,    62,    51,    51,    75,
      51,     6,     7,    15,    90,    90,    52,    87,    90,   577,
      18,    72,    42,    52,    83,   606,    49,    90,    90,    63,
      52,    51,    90,    52,    52,    86,    87,    96,    48,    75,
      50,    51,    86,    87,    87,    86,    87,    15,    32,   100,
      52,   182,    75,    48,    15,    50,    51,    64,    53,   617,
      92,   120,    96,   573,   595,   568,    86,    87,    20,    53,
      15,    90,    15,    15,   120,    19,    48,   329,    48,   610,
      52,    49,    52,    15,   615,   220,    93,    15,    15,   123,
      65,    52,    74,    75,   547,   332,   377,   378,   379,    74,
      75,    30,    74,    75,    74,    75,   157,    52,    90,    52,
      52,    29,   158,   157,   157,    90,   157,    49,    90,    31,
      90,    49,    49,   182,    43,    65,    43,    43,   162,    62,
     164,   165,    52,   242,    74,    75,    55,   157,    55,    55,
      55,    74,    75,    43,   178,     1,    61,     3,     4,     5,
      90,    62,     8,    48,    10,    11,   190,    90,    65,    15,
      74,    75,    18,    74,    75,    21,    22,    74,    75,    43,
      43,    64,    43,    43,    55,    55,    90,   308,    43,    90,
      61,    55,    55,    90,    55,    55,   423,    55,    44,    54,
      46,   242,    48,    61,    50,    51,    52,    43,    48,    55,
      56,    43,    58,    59,    60,    61,    62,    49,    54,    43,
      66,   262,    68,    69,    70,    49,    72,    73,    74,    75,
       6,     7,    43,   333,    80,    81,    82,    83,    49,    27,
      28,    43,    61,    89,    90,    91,    52,    49,    94,    55,
      84,    85,    86,    48,    88,   101,    62,   299,   383,   308,
      52,    43,    43,    55,    48,    71,    43,    49,    49,    55,
      62,    43,    49,    27,    28,    43,   400,    49,    91,    71,
     400,    49,    21,    22,   308,    91,    64,   336,    94,    95,
      96,    97,    98,    99,    64,    43,    12,    13,    14,    91,
     336,    49,    94,    95,    96,    97,    98,    99,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      55,    43,    55,    43,     6,     7,    61,    49,   387,    49,
     376,   377,   378,   379,    55,   394,   395,    21,    22,    55,
      61,    55,   388,   394,   395,    61,    43,    61,    55,   400,
     394,   395,    49,   394,   395,   389,   400,    55,   404,   400,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,    15,   418,   419,   394,   395,    55,   411,   429,   413,
     400,   359,   360,   361,   362,   363,   364,   365,   366,   367,
     368,   369,    23,    24,    25,    26,    61,     3,     4,     5,
       6,     7,     8,    75,    10,    11,    61,    55,    43,    15,
      52,    53,    18,    61,    49,    21,    22,    11,    12,    13,
      14,    15,    16,    64,    65,    19,    20,   478,   183,   184,
     185,   186,   187,   188,    52,   481,   191,   192,   497,   490,
      46,    52,    48,    43,    50,    51,    52,    53,    52,    49,
      23,    24,    25,    26,   505,   135,   136,   137,   138,   133,
     134,   505,   139,   140,   505,   350,   351,   352,   353,    52,
     529,   394,   395,   519,   526,    15,    16,    17,   530,    12,
      13,    14,    45,    89,    90,   505,    92,    59,    94,    15,
      16,    17,   124,   125,   126,   394,   395,   127,   128,   129,
     339,   340,   341,   549,   555,   342,   343,   344,   348,   349,
      55,   555,    64,   547,   555,    43,   354,   355,   577,    18,
      20,    19,   568,    29,   576,    30,   578,    31,   574,    53,
      55,    48,   583,    48,    43,   555,   595,   589,    57,   583,
      90,    90,   583,    55,    55,    48,    64,    55,     4,    15,
      64,   610,    54,    43,    55,   606,   615,    52,   617,   605,
     612,    52,   606,   583,    52,   606,     3,     4,     5,    52,
      52,     8,    48,    10,    11,    55,    61,    64,    15,    52,
      49,    18,    15,    55,    21,    22,   606,    55,    15,    52,
       4,    62,    49,    43,    49,    49,    92,    64,     4,    64,
      49,    52,    49,    48,    64,    49,    49,    44,    48,    46,
      49,    48,    48,    50,    51,    52,    64,   383,    55,    56,
     333,    58,    59,    60,    61,    62,    63,   404,   336,    66,
     143,    68,    69,    70,   131,    72,    73,    74,    75,   132,
      77,   130,    79,    80,    81,    82,    83,   141,   573,   236,
     142,   478,    89,    90,    91,   391,   345,    94,     3,     4,
       5,    83,   583,     8,   101,    10,    11,   357,   346,   356,
      15,   347,   358,    18,    -1,    -1,    21,    22,    -1,    -1,
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
      -1,    -1,    55,    56,    -1,    58,    59,    60,    61,    -1,
      63,    -1,    -1,    66,    -1,    68,    69,    70,    -1,    72,
      73,    74,    75,    -1,    77,    78,    79,    80,    81,    82,
      83,    -1,    -1,    -1,    -1,    -1,    89,    90,    91,    -1,
      -1,    94,     3,     4,     5,    -1,    -1,     8,   101,    10,
      11,    -1,    -1,    -1,    15,    -1,    -1,    18,    -1,    -1,
      21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    44,    -1,    46,    -1,    48,    -1,    50,
      51,    52,    -1,    -1,    55,    56,    -1,    58,    59,    60,
      61,    -1,    63,    -1,    -1,    66,    -1,    68,    69,    70,
      -1,    72,    73,    74,    75,    -1,    77,    -1,    79,    80,
      81,    82,    83,    -1,    -1,    -1,    -1,    -1,    89,    90,
      91,    -1,    -1,    94,     3,     4,     5,    -1,    -1,     8,
     101,    10,    11,    -1,    -1,    -1,    15,    -1,    -1,    18,
      -1,    -1,    21,    22,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    44,    -1,    46,    -1,    48,
      -1,    50,    51,    52,    -1,    -1,    55,    56,    -1,    58,
      59,    60,    61,    62,    -1,    -1,    -1,    66,    -1,    68,
      69,    70,    -1,    72,    73,    74,    75,    -1,    -1,    -1,
      -1,    80,    81,    82,    83,    -1,    -1,    -1,    -1,    -1,
      89,    90,    91,    -1,    -1,    94,     3,     4,     5,    -1,
      -1,     8,   101,    10,    11,    -1,    -1,    -1,    15,    -1,
      -1,    18,    -1,    -1,    21,    22,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    -1,    46,
      -1,    48,    -1,    50,    51,    52,    -1,    -1,    55,    56,
      -1,    58,    59,    60,    61,    -1,    -1,    -1,    -1,    66,
      -1,    68,    69,    70,    -1,    72,    73,    74,    75,    -1,
      -1,    -1,    -1,    80,    81,    82,    83,    -1,    -1,    -1,
      -1,    -1,    89,    90,    91,    -1,    -1,    94,     3,     4,
       5,    -1,    -1,     8,   101,    10,    11,     3,     4,     5,
      15,    -1,     8,    18,    10,    11,    21,    22,    -1,    15,
      -1,    -1,    18,    -1,    -1,    21,    22,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    46,    -1,    48,    -1,    50,    51,    52,    53,    -1,
      46,    -1,    48,    -1,    50,    51,    52,     3,     4,     5,
      -1,    -1,     8,    -1,    10,    11,    -1,    -1,    -1,    15,
      -1,    -1,    18,    -1,    -1,    21,    22,    -1,    74,    75,
      -1,    -1,    -1,    -1,    89,    90,    -1,    92,    -1,    94,
      -1,    -1,    -1,    89,    90,    -1,    -1,    43,    94,    -1,
      46,    -1,    48,    -1,    50,    51,    52,    -1,    -1,     3,
       4,     5,    -1,    -1,     8,    61,    10,    11,    -1,    -1,
      -1,    15,    -1,    -1,    18,    -1,    -1,    21,    22,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    89,    90,    -1,    -1,    -1,    94,    -1,
      -1,    -1,    46,    -1,    48,    -1,    50,    51,    52,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    61,    62,     8,
      -1,    10,    11,    -1,    -1,    -1,    15,    -1,    -1,    18,
      -1,    -1,    21,    22,    -1,    -1,     3,     4,     5,    -1,
      -1,     8,    -1,    10,    11,    89,    90,    -1,    15,    -1,
      94,    18,    -1,    -1,    21,    22,    -1,    46,    -1,    48,
      -1,    50,    51,    52,    -1,    -1,    55,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      -1,    48,    -1,    50,    51,    52,    53,    -1,    -1,    -1,
      -1,     3,     4,     5,    -1,    -1,     8,    -1,    10,    11,
      89,    90,    -1,    15,    -1,    94,    18,    -1,    -1,    21,
      22,    -1,    -1,     3,     4,     5,    -1,    -1,     8,    -1,
      10,    11,    89,    90,    -1,    15,    -1,    94,    18,    -1,
      -1,    21,    22,    -1,    46,    -1,    48,    -1,    50,    51,
      52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    48,    -1,
      50,    51,    52,    -1,    54,    -1,    -1,    -1,     3,     4,
       5,    -1,    -1,     8,    -1,    10,    11,    89,    90,    -1,
      15,    -1,    94,    18,    -1,    -1,    21,    22,    -1,    -1,
       3,     4,     5,    -1,    -1,     8,    -1,    10,    11,    89,
      90,    -1,    15,    -1,    94,    18,    -1,    -1,    21,    22,
      -1,    46,    -1,    48,    -1,    50,    51,    52,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    46,    -1,    48,    -1,    50,    51,    52,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    64,    -1,    -1,    89,    90,    -1,    92,    -1,    94,
       3,     4,     5,    -1,    -1,     8,    -1,    10,    11,    -1,
      -1,    -1,    15,    -1,    -1,    18,    89,    90,    21,    22,
      -1,    94,     3,     4,     5,    -1,    -1,     8,    -1,    10,
      11,    -1,    -1,    -1,    15,    -1,    -1,    18,    -1,    -1,
      21,    22,    -1,    46,    -1,    48,    -1,    50,    51,    52,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    -1,    48,    -1,    50,
      51,    52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    89,    90,    -1,    -1,
      -1,    94,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,    90,
      -1,    -1,    -1,    94
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    84,    85,    86,    88,   178,   179,     3,     4,     5,
       8,    10,    11,    15,    18,    21,    22,    46,    48,    50,
      51,    52,    89,    90,    94,   115,   118,   119,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,    61,   177,   134,   135,   135,    44,    52,    55,
      56,    58,    59,    60,    66,    68,    69,    70,    72,    73,
      74,    75,    80,    81,    82,    83,    90,    91,   101,   109,
     113,   135,   136,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   159,   161,   162,   163,   165,   166,   167,   168,
     169,   173,   174,   175,   177,     0,    48,    52,    90,   120,
     136,   146,   147,   150,   151,   121,   121,   121,   121,   121,
     121,   135,   121,   121,     6,     7,     4,    52,     6,     7,
      48,    50,    51,    53,    12,    13,    14,    15,    16,    17,
      18,    20,    19,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    62,   163,   176,    43,    87,
     177,    64,    48,   163,    48,    48,    52,    90,    52,    90,
      52,    90,    52,    90,    55,    55,   135,    52,    48,     3,
       4,     5,     8,    10,    11,    15,    18,    21,    22,    46,
      48,    50,    51,    52,    53,    89,    90,    92,    94,   105,
     106,   107,   108,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,   192,   193,   194,   195,   196,
      61,    55,    55,    15,    52,   151,   153,   154,   155,   156,
     158,    64,    64,    55,    61,    55,    61,    55,    55,    55,
      74,    75,   136,   159,   167,   168,   163,   168,    55,   109,
     120,   135,    15,   151,    49,    52,    52,    52,    52,   116,
     117,   134,   136,   135,   122,   122,   122,   123,   123,   123,
     124,   125,   126,   127,   127,   128,   128,   128,   128,   129,
     129,   130,   131,   132,   133,   133,   133,   133,   133,   133,
     133,   133,   133,   133,   133,     1,    62,   163,   134,    45,
     135,    59,   135,   135,   164,    55,    55,   135,    48,    52,
     120,    90,   183,   183,   183,   183,   183,   183,   135,   183,
     183,     4,    92,   103,   104,     6,     7,    92,   106,   196,
       4,    52,    64,    43,     6,     7,    48,    50,    51,    12,
      13,    14,    15,    16,    17,    18,    20,    19,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
     105,    52,    55,    62,    71,    94,    95,    96,    97,    98,
      99,   109,   110,   114,    15,    52,    55,    48,    32,    53,
      48,    43,    90,    90,   143,   143,    52,    52,   153,    55,
      57,    55,    49,    49,    43,    49,    15,    49,    54,   177,
      49,    48,    49,    55,    49,   120,   135,    49,    21,    22,
      54,    52,    52,    64,   106,   107,    52,    52,   117,   136,
     184,   184,   184,   185,   185,   185,   186,   187,   188,   189,
     189,   190,   190,   190,   190,   191,   191,   192,   193,   194,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,    64,    93,    52,   112,     4,    52,   111,   134,   111,
     111,   111,     4,    62,   110,    15,    52,    65,   136,   157,
     160,    61,   134,   138,    54,   135,    15,   153,   158,    62,
     136,   142,   148,   149,    62,    55,    55,    48,   167,   168,
     116,    15,    49,   135,   164,    61,    49,    49,     4,    92,
     134,   134,   106,    49,    49,    43,    55,    64,    64,    43,
      55,    55,    55,    55,    43,    52,    49,   154,   156,    43,
      49,    43,   134,   137,   138,    54,    15,    52,   140,   141,
     154,    55,    65,   160,    15,    49,    49,    55,    62,    63,
      77,    79,   163,   170,   171,   172,    15,    15,    52,   134,
       4,    55,   177,    65,   157,    55,   177,    62,    43,    62,
      15,    52,    49,    43,    52,   139,    49,    43,    49,    49,
     164,    64,   134,    64,    64,    62,   171,    92,     4,    49,
      62,   134,   138,    52,    49,    48,   141,   134,    55,   177,
      65,    55,   177,    49,    64,    65,   172,    55,   177,    49,
      48,   160,    49,   134,    78,    48,   160,    43,    49,    55,
     177,    64,    64,   160,    49,    49
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
     135,   136,   136,   136,   136,   137,   137,   137,   137,   138,
     138,   138,   139,   140,   140,   141,   141,   142,   143,   143,
     143,   143,   144,   145,   144,   145,   144,   145,   146,   147,
     146,   147,   148,   148,   149,   149,   149,   150,   151,   152,
     153,   153,   153,   153,   154,   155,   155,   155,   155,   154,
     154,   154,   154,   154,   156,   156,   157,   157,   158,   158,
     158,   159,   160,   160,   160,   161,   161,   161,   161,   161,
     161,   161,   161,   162,   162,   162,   162,   162,   162,   162,
     162,   162,   162,   162,   162,   163,   163,   164,   164,   165,
     165,   166,   167,   167,   167,   168,   168,   168,   169,   170,
     171,   171,   171,   171,   171,   171,   172,   172,   173,   173,
     174,   175,   175,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   176,   176,   176,   177,   177,   178,   178,   179,
     179,   179,   179,   179,   180,   180,   180,   180,   180,   180,
     180,   180,   181,   181,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   184,   184,   184,   184,
     185,   185,   185,   185,   186,   186,   187,   187,   188,   188,
     189,   189,   189,   190,   190,   190,   190,   190,   191,   191,
     191,   192,   192,   193,   193,   194,   194,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   196
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
       3,     3,     2,     2,     2,     3,     2,     4,     4,     4,
       1,     2,     3,     4,     1,     0,     1,     2,     3,     8,
       9,    10,     4,     3,     3,     3,     2,     2,     3,     1,
       1,     2,     1,     3,     0,     1,     1,     1,     1,     2,
       2,     2,     2,     6,     8,     6,     7,     9,     7,     6,
       8,     6,     7,     9,     7,     1,     1,     1,     0,     4,
       8,     4,     2,     4,     2,     1,     4,     2,     6,     5,
       1,     2,     3,     2,     5,     1,     2,     1,     7,     6,
       1,     2,     3,     1,     1,     2,     3,     1,     3,     2,
       2,     1,     1,     2,     1,     3,     2,     2,     1,     1,
       4,     4,     1,     2,     2,     3,     2,     0,     2,     1,
       2,     3,     2,     2,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     3,     3,     3,     3,     2,
       2,     4,     4,     1,     2,     2,     4,     4,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     3,     3,     3,
       1,     3,     3,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     3,     1,     3,     3,     3,     3,     1,     3,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     1
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
#line 2100 "HolyC.tab.c"
    break;

  case 3:
#line 62 "HolyC.y"
                                         {
  yyval=AppendToSIB(yyvsp[-2],NULL,yyvsp[0],NULL);
  ReleaseAST(yyvsp[-1]);
}
#line 2109 "HolyC.tab.c"
    break;

  case 4:
#line 66 "HolyC.y"
                                                        {
  yyval=AppendToSIB(yyvsp[-4],yyvsp[-2],yyvsp[0],NULL);
  ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-1]);
}
#line 2118 "HolyC.tab.c"
    break;

  case 5:
#line 70 "HolyC.y"
                                                         {
  yyval=AppendToSIB(yyvsp[-4],yyvsp[0],yyvsp[-2],NULL);
  ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-1]);
}
#line 2127 "HolyC.tab.c"
    break;

  case 6:
#line 74 "HolyC.y"
                  {
  yyval=AppendToSIB(NULL,NULL,NULL,yyvsp[0]);
 }
#line 2135 "HolyC.tab.c"
    break;

  case 8:
#line 78 "HolyC.y"
                                   {
  yyval=AppendToSIB(yyvsp[-2],NULL,NULL,yyvsp[0]);
  ReleaseAST(yyvsp[-1]);
}
#line 2144 "HolyC.tab.c"
    break;

  case 9:
#line 82 "HolyC.y"
                                    {
  yyval=AppendToSIB(yyvsp[-2],NULL,NULL,yyvsp[0]);
  ReleaseAST(yyvsp[-1]);
}
#line 2153 "HolyC.tab.c"
    break;

  case 10:
#line 86 "HolyC.y"
                                                      {
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
  yyval=yyvsp[-1];
}
#line 2162 "HolyC.tab.c"
    break;

  case 12:
#line 91 "HolyC.y"
                            {
  yyval=AppendToSIB(yyvsp[0],NULL,NULL,yyvsp[-1]);
 }
#line 2170 "HolyC.tab.c"
    break;

  case 16:
#line 97 "HolyC.y"
                                 {
  yyval=yyvsp[0];
  CType *t=*map_get(&Compiler.types,yyvsp[-1]->name);
  yyval->asmAddr.width=TypeSize(t);
}
#line 2180 "HolyC.tab.c"
    break;

  case 17:
#line 102 "HolyC.y"
                                                     {
  CType *t=*map_get(&Compiler.types,yyvsp[-3]->name);
  yyvsp[0]->asmAddr.width=TypeSize(t);
  yyvsp[0]->asmAddr.segment=yyvsp[-2];
  yyval=yyvsp[0];
}
#line 2191 "HolyC.tab.c"
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
#line 2204 "HolyC.tab.c"
    break;

  case 19:
#line 116 "HolyC.y"
                                {yyval=yyvsp[0];}
#line 2210 "HolyC.tab.c"
    break;

  case 20:
#line 117 "HolyC.y"
                                                            {
  yyval=CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA);
  ReleaseAST(yyvsp[-1]);
}
#line 2219 "HolyC.tab.c"
    break;

  case 21:
#line 121 "HolyC.y"
                 {yyval=NULL;}
#line 2225 "HolyC.tab.c"
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
#line 2239 "HolyC.tab.c"
    break;

  case 23:
#line 131 "HolyC.y"
                                         {
  yyval=SOT(CreateExportedLabel(yyvsp[-1]),yyvsp[-1]);
  yyval->inAsmBlk=1;
  ReleaseAST(yyvsp[0]);
}
#line 2249 "HolyC.tab.c"
    break;

  case 24:
#line 136 "HolyC.y"
                                  {
  yyval=SOT(CreateLabel(yyvsp[-1]),yyvsp[-1]);
  yyval->inAsmBlk=1;
  ReleaseAST(yyvsp[0]);
 }
#line 2259 "HolyC.tab.c"
    break;

  case 25:
#line 141 "HolyC.y"
                                                {
  yyval=SOT(CreateLocalLabel(yyvsp[-1]),yyvsp[-2]);
  yyval->inAsmBlk=1;
  ReleaseAST(yyvsp[0]),ReleaseAST(yyvsp[-2]);
}
#line 2269 "HolyC.tab.c"
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
#line 2285 "HolyC.tab.c"
    break;

  case 28:
#line 159 "HolyC.y"
                                                {
  yyval=CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA);
  ReleaseAST(yyvsp[-1]);
}
#line 2294 "HolyC.tab.c"
    break;

  case 29:
#line 163 "HolyC.y"
                                                {
  yyval=SOT(CreateDU8(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2303 "HolyC.tab.c"
    break;

  case 30:
#line 167 "HolyC.y"
                                                 {
  yyval=SOT(CreateDU16(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2312 "HolyC.tab.c"
    break;

  case 31:
#line 171 "HolyC.y"
                                                 {
  yyval=SOT(CreateDU32(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2321 "HolyC.tab.c"
    break;

  case 32:
#line 175 "HolyC.y"
                                                 {
  yyval=SOT(CreateDU64(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2330 "HolyC.tab.c"
    break;

  case 35:
#line 181 "HolyC.y"
                                             {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA),yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
}
#line 2339 "HolyC.tab.c"
    break;

  case 36:
#line 185 "HolyC.y"
                                                       {
  yyval=SOT(CreateAsmImport(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2348 "HolyC.tab.c"
    break;

  case 37:
#line 189 "HolyC.y"
                          {
  yyval=SOT(CreateNop(),yyvsp[0]);
  ReleaseAST(yyvsp[0]);
 }
#line 2357 "HolyC.tab.c"
    break;

  case 38:
#line 193 "HolyC.y"
                                                           {
  yyval=SOT(CreateAsmAlign(yyvsp[-2],yyvsp[0]),yyvsp[-3]);
  ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-3]);
}
#line 2366 "HolyC.tab.c"
    break;

  case 39:
#line 197 "HolyC.y"
                                                   {
  yyval=SOT(CreateNop(),yyvsp[-2]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 2375 "HolyC.tab.c"
    break;

  case 41:
#line 202 "HolyC.y"
                                                   {
  yyval=AppendToStmts(yyvsp[-1],yyvsp[0]);
}
#line 2383 "HolyC.tab.c"
    break;

  case 42:
#line 205 "HolyC.y"
                                                                    {
  yyval=yyvsp[-1];
  yyval->type=AST_ASM_BLK;
  ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2393 "HolyC.tab.c"
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
#line 2406 "HolyC.tab.c"
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
#line 2420 "HolyC.tab.c"
    break;

  case 45:
#line 228 "HolyC.y"
                {yyval=SLE(yyvsp[0]);}
#line 2426 "HolyC.tab.c"
    break;

  case 46:
#line 229 "HolyC.y"
              {yyval=SLE(yyvsp[0]);}
#line 2432 "HolyC.tab.c"
    break;

  case 47:
#line 230 "HolyC.y"
               {yyval=SLE(yyvsp[0]);}
#line 2438 "HolyC.tab.c"
    break;

  case 48:
#line 231 "HolyC.y"
                 {yyval=SLE(yyvsp[0]);}
#line 2444 "HolyC.tab.c"
    break;

  case 49:
#line 232 "HolyC.y"
                    {
  yyval=SLE(yyvsp[0]);
}
#line 2452 "HolyC.tab.c"
    break;

  case 50:
#line 235 "HolyC.y"
               {
  yyval=SLE(yyvsp[0]);
}
#line 2460 "HolyC.tab.c"
    break;

  case 51:
#line 238 "HolyC.y"
              {yyval=NULL;}
#line 2466 "HolyC.tab.c"
    break;

  case 52:
#line 239 "HolyC.y"
                      {yyval=yyvsp[0];}
#line 2472 "HolyC.tab.c"
    break;

  case 53:
#line 240 "HolyC.y"
                                               {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA),yyvsp[-1]);
}
#line 2480 "HolyC.tab.c"
    break;

  case 54:
#line 243 "HolyC.y"
                          {yyval=yyvsp[0];}
#line 2486 "HolyC.tab.c"
    break;

  case 56:
#line 245 "HolyC.y"
                                                         {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2495 "HolyC.tab.c"
    break;

  case 58:
#line 254 "HolyC.y"
                                     {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2504 "HolyC.tab.c"
    break;

  case 59:
#line 258 "HolyC.y"
                                       {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2513 "HolyC.tab.c"
    break;

  case 60:
#line 262 "HolyC.y"
                                  {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2522 "HolyC.tab.c"
    break;

  case 61:
#line 266 "HolyC.y"
                                    {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2531 "HolyC.tab.c"
    break;

  case 62:
#line 270 "HolyC.y"
                                                                    {
  yyval=SOT(CreateArrayAccess(yyvsp[-3],yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2540 "HolyC.tab.c"
    break;

  case 63:
#line 274 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[-1],AST_POST_DEC),yyvsp[0]);
  ReleaseAST(yyvsp[0]);
}
#line 2549 "HolyC.tab.c"
    break;

  case 64:
#line 278 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[-1],AST_POST_INC),yyvsp[0]);
  ReleaseAST(yyvsp[0]);
}
#line 2558 "HolyC.tab.c"
    break;

  case 65:
#line 282 "HolyC.y"
                                                                {
  yyval=SOT(CreateFuncCall(yyvsp[-3],yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2567 "HolyC.tab.c"
    break;

  case 66:
#line 286 "HolyC.y"
                                                                 {
  yyval=CreateExplicitTypecast(yyvsp[-3],yyvsp[-1]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 2576 "HolyC.tab.c"
    break;

  case 67:
#line 290 "HolyC.y"
                                                                          {
  yyvsp[-2]->type2=CreateMultiLvlPtr(yyvsp[-2]->type2,1);
  yyval=CreateExplicitTypecast(yyvsp[-4],yyvsp[-2]);
  ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 2586 "HolyC.tab.c"
    break;

  case 68:
#line 295 "HolyC.y"
                                                                                   {
  yyvsp[-3]->type2=CreateMultiLvlPtr(yyvsp[-3]->type2,2);
  yyval=CreateExplicitTypecast(yyvsp[-5],yyvsp[-3]);
  ReleaseAST(yyvsp[-4]),ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 2596 "HolyC.tab.c"
    break;

  case 69:
#line 300 "HolyC.y"
                                                                                            {
  yyvsp[-4]->type2=CreateMultiLvlPtr(yyvsp[-4]->type2,3);
  yyval=CreateExplicitTypecast(yyvsp[-6],yyvsp[-4]);
  ReleaseAST(yyvsp[-5]),ReleaseAST(yyvsp[-3]),ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 2606 "HolyC.tab.c"
    break;

  case 70:
#line 308 "HolyC.y"
                             {yyval=yyvsp[0];}
#line 2612 "HolyC.tab.c"
    break;

  case 71:
#line 309 "HolyC.y"
                                        {
  CType *ptr =CreateMultiLvlPtr(yyvsp[-1]->type2,1);
  yyval=CreateTypeNode(ptr);
  ReleaseAST(yyvsp[0]);
}
#line 2622 "HolyC.tab.c"
    break;

  case 73:
#line 315 "HolyC.y"
                            {
  yyval=SOT(CreateSizeof(yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2631 "HolyC.tab.c"
    break;

  case 74:
#line 319 "HolyC.y"
                                   {
  yyval=SOT(CreateSizeof(yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2640 "HolyC.tab.c"
    break;

  case 75:
#line 323 "HolyC.y"
                                                                    {
  yyval=SOT(CreateSizeof(yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 2651 "HolyC.tab.c"
    break;

  case 76:
#line 329 "HolyC.y"
                                                                     {
  yyval=SOT(CreateSizeof(yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 2662 "HolyC.tab.c"
    break;

  case 77:
#line 335 "HolyC.y"
                           {
  yyval=SOT(CreateUnop(yyvsp[0],AST_ADDROF),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2671 "HolyC.tab.c"
    break;

  case 78:
#line 339 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_DERREF),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2680 "HolyC.tab.c"
    break;

  case 79:
#line 344 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_PRE_DEC),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2689 "HolyC.tab.c"
    break;

  case 80:
#line 348 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_PRE_INC),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2698 "HolyC.tab.c"
    break;

  case 81:
#line 352 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_POS),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2707 "HolyC.tab.c"
    break;

  case 82:
#line 356 "HolyC.y"
                          {
  yyval=SOT(CreateUnop(yyvsp[0],AST_NEG),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2716 "HolyC.tab.c"
    break;

  case 83:
#line 360 "HolyC.y"
                           {
  yyval=SOT(CreateUnop(yyvsp[0],AST_LNOT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2725 "HolyC.tab.c"
    break;

  case 84:
#line 364 "HolyC.y"
                           {
  yyval=SOT(CreateUnop(yyvsp[0],AST_BNOT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2734 "HolyC.tab.c"
    break;

  case 86:
#line 371 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_POW),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2743 "HolyC.tab.c"
    break;

  case 87:
#line 375 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SHL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2752 "HolyC.tab.c"
    break;

  case 88:
#line 379 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SHR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2761 "HolyC.tab.c"
    break;

  case 90:
#line 385 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_MUL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2770 "HolyC.tab.c"
    break;

  case 91:
#line 389 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_DIV),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2779 "HolyC.tab.c"
    break;

  case 92:
#line 393 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_MOD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2788 "HolyC.tab.c"
    break;

  case 94:
#line 399 "HolyC.y"
                                    {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2797 "HolyC.tab.c"
    break;

  case 96:
#line 405 "HolyC.y"
                                    {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2806 "HolyC.tab.c"
    break;

  case 98:
#line 411 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2815 "HolyC.tab.c"
    break;

  case 100:
#line 417 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ADD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2824 "HolyC.tab.c"
    break;

  case 101:
#line 421 "HolyC.y"
                                   {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SUB),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2833 "HolyC.tab.c"
    break;

  case 103:
#line 427 "HolyC.y"
                                  {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_LT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2842 "HolyC.tab.c"
    break;

  case 104:
#line 431 "HolyC.y"
                                  {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_GT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2851 "HolyC.tab.c"
    break;

  case 105:
#line 435 "HolyC.y"
                                  {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_LE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2860 "HolyC.tab.c"
    break;

  case 106:
#line 439 "HolyC.y"
                                  {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_GE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2869 "HolyC.tab.c"
    break;

  case 108:
#line 445 "HolyC.y"
                                     {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_EQ),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2878 "HolyC.tab.c"
    break;

  case 109:
#line 449 "HolyC.y"
                                     {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_NE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2887 "HolyC.tab.c"
    break;

  case 111:
#line 455 "HolyC.y"
                                       {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2896 "HolyC.tab.c"
    break;

  case 113:
#line 461 "HolyC.y"
                                       {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2905 "HolyC.tab.c"
    break;

  case 115:
#line 467 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2914 "HolyC.tab.c"
    break;

  case 117:
#line 473 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2923 "HolyC.tab.c"
    break;

  case 118:
#line 477 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SHL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2932 "HolyC.tab.c"
    break;

  case 119:
#line 481 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SHR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2941 "HolyC.tab.c"
    break;

  case 120:
#line 485 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_MUL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2950 "HolyC.tab.c"
    break;

  case 121:
#line 489 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_DIV),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2959 "HolyC.tab.c"
    break;

  case 122:
#line 493 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_MOD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2968 "HolyC.tab.c"
    break;

  case 123:
#line 497 "HolyC.y"
                                          {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2977 "HolyC.tab.c"
    break;

  case 124:
#line 501 "HolyC.y"
                                          {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2986 "HolyC.tab.c"
    break;

  case 125:
#line 505 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 2995 "HolyC.tab.c"
    break;

  case 126:
#line 509 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_ADD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 3004 "HolyC.tab.c"
    break;

  case 127:
#line 513 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SUB),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 3013 "HolyC.tab.c"
    break;

  case 129:
#line 520 "HolyC.y"
                                              {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 3022 "HolyC.tab.c"
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
#line 3037 "HolyC.tab.c"
    break;

  case 132:
#line 539 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 3043 "HolyC.tab.c"
    break;

  case 133:
#line 540 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 3049 "HolyC.tab.c"
    break;

  case 134:
#line 541 "HolyC.y"
                                      {
  yyval=yyvsp[0];
  AssignUnionBasetype(yyvsp[0],yyvsp[-1]);
}
#line 3058 "HolyC.tab.c"
    break;

  case 135:
#line 548 "HolyC.y"
                                          {yyval=AppendToArrLiteral(yyvsp[-2],yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3064 "HolyC.tab.c"
    break;

  case 136:
#line 549 "HolyC.y"
                                            {yyval=AppendToArrLiteral(yyvsp[-2],yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3070 "HolyC.tab.c"
    break;

  case 137:
#line 550 "HolyC.y"
              {yyval=AppendToArrLiteral(NULL,yyvsp[0]);}
#line 3076 "HolyC.tab.c"
    break;

  case 138:
#line 551 "HolyC.y"
                {yyval=AppendToArrLiteral(NULL,yyvsp[0]);}
#line 3082 "HolyC.tab.c"
    break;

  case 139:
#line 552 "HolyC.y"
                                                       {
  yyval=SOT(yyvsp[-1],yyvsp[-2]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3092 "HolyC.tab.c"
    break;

  case 140:
#line 557 "HolyC.y"
                                                                 {
  yyval=SOT(yyvsp[-2],yyvsp[-3]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3103 "HolyC.tab.c"
    break;

  case 141:
#line 563 "HolyC.y"
                                                      {
  yyval=NULL;
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3114 "HolyC.tab.c"
    break;

  case 142:
#line 572 "HolyC.y"
                             {
  yyval=AppendToMetaData(NULL,yyvsp[-1],yyvsp[0]);
}
#line 3122 "HolyC.tab.c"
    break;

  case 143:
#line 575 "HolyC.y"
                                                    {
  assert(yyvsp[0]->type==__AST_DECL_TAILS);
  CDeclTail b2=vec_last(&yyvsp[0]->declTail);
  AppendToDecls(yyvsp[-2],b2.basetype,b2.finalType,b2.name,b2.dft,b2.metaData);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 3134 "HolyC.tab.c"
    break;

  case 144:
#line 582 "HolyC.y"
                            {yyval=yyvsp[0];}
#line 3140 "HolyC.tab.c"
    break;

  case 145:
#line 583 "HolyC.y"
                              {
  AST *t=yyvsp[0];
  yyval=yyvsp[0];
}
#line 3149 "HolyC.tab.c"
    break;

  case 146:
#line 587 "HolyC.y"
                                          {
  AST *meta=yyvsp[-1]->declTail.data[0].metaData;
  meta=AppendToMetaData(meta,yyvsp[0]->metaData.data[0].name,yyvsp[0]->metaData.data[0].value);
  yyvsp[-1]->declTail.data[0].metaData=meta;
  yyval=yyvsp[-1];
}
#line 3160 "HolyC.tab.c"
    break;

  case 147:
#line 593 "HolyC.y"
                                    {
  DeclsFillInBasetype(yyvsp[-1]->type2,yyvsp[0]);
  yyval=yyvsp[0];
}
#line 3169 "HolyC.tab.c"
    break;

  case 148:
#line 597 "HolyC.y"
                                      {
 yyval=AppendToStmts(yyvsp[-2],yyvsp[-1]);
 ReleaseAST(yyvsp[0]);
}
#line 3178 "HolyC.tab.c"
    break;

  case 149:
#line 601 "HolyC.y"
                            {
 yyval=AppendToStmts(yyvsp[-1],yyvsp[0]);
}
#line 3186 "HolyC.tab.c"
    break;

  case 150:
#line 604 "HolyC.y"
                            {
 yyval=AppendToStmts(yyvsp[-1],yyvsp[0]);
}
#line 3194 "HolyC.tab.c"
    break;

  case 151:
#line 607 "HolyC.y"
       {yyval=NULL;}
#line 3200 "HolyC.tab.c"
    break;

  case 152:
#line 608 "HolyC.y"
                        {
  CType *t=CreateClassForwardDecl(NULL,NULL);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[0]);
}
#line 3210 "HolyC.tab.c"
    break;

  case 153:
#line 613 "HolyC.y"
                        {
  CType *t=CreateUnionForwardDecl(NULL,NULL);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[0]);
}
#line 3220 "HolyC.tab.c"
    break;

  case 154:
#line 618 "HolyC.y"
                                {
  CType *t=CreateClassForwardDecl(NULL,yyvsp[0]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-1]);
}
#line 3230 "HolyC.tab.c"
    break;

  case 155:
#line 623 "HolyC.y"
                                {
  CType *t=CreateUnionForwardDecl(NULL,yyvsp[0]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-1]);
}
#line 3240 "HolyC.tab.c"
    break;

  case 156:
#line 628 "HolyC.y"
                                    {
  CType *t=CreateClassForwardDecl(NULL,yyvsp[0]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-1]);
}
#line 3250 "HolyC.tab.c"
    break;

  case 157:
#line 633 "HolyC.y"
                               {
  CType *t=CreateUnionForwardDecl(NULL,yyvsp[0]);
  yyval=CreateTypeNode(t);
}
#line 3259 "HolyC.tab.c"
    break;

  case 158:
#line 637 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 3265 "HolyC.tab.c"
    break;

  case 159:
#line 638 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 3271 "HolyC.tab.c"
    break;

  case 160:
#line 639 "HolyC.y"
                                                 {
  CType *t=*map_get(&Compiler.types,yyvsp[0]->name);
  InheritFromType(yyvsp[-2]->type2,t);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 3282 "HolyC.tab.c"
    break;

  case 161:
#line 645 "HolyC.y"
                                                 {
  CType *t=*map_get(&Compiler.types,yyvsp[0]->name);
  InheritFromType(yyvsp[-2]->type2,t);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 3293 "HolyC.tab.c"
    break;

  case 162:
#line 651 "HolyC.y"
                              {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[0]);
}
#line 3302 "HolyC.tab.c"
    break;

  case 163:
#line 655 "HolyC.y"
                               {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[0]);
}
#line 3311 "HolyC.tab.c"
    break;

  case 164:
#line 659 "HolyC.y"
                               {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[0]);
}
#line 3320 "HolyC.tab.c"
    break;

  case 165:
#line 663 "HolyC.y"
                                            {
  yyval=yyvsp[-1];
  AssignUnionBasetype(yyvsp[-1],yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3330 "HolyC.tab.c"
    break;

  case 166:
#line 668 "HolyC.y"
                              {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[0]);
}
#line 3339 "HolyC.tab.c"
    break;

  case 167:
#line 673 "HolyC.y"
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
#line 3359 "HolyC.tab.c"
    break;

  case 168:
#line 688 "HolyC.y"
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
#line 3376 "HolyC.tab.c"
    break;

  case 169:
#line 704 "HolyC.y"
                                                   {
  yyval=CreateTry(yyvsp[-2],yyvsp[0]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-1]);
}
#line 3386 "HolyC.tab.c"
    break;

  case 170:
#line 712 "HolyC.y"
                   {
  CType *t =CreatePrimType(TYPE_I64);
  yyval=SOT(AppendToDecls(NULL,t,t,yyvsp[0],NULL,NULL),yyvsp[0]);
}
#line 3395 "HolyC.tab.c"
    break;

  case 171:
#line 716 "HolyC.y"
                            {
  CType *t =CreatePrimType(TYPE_I64);
  CType *ptr =CreateMultiLvlPtr(t,1);
  yyval=SOT(AppendToDecls(NULL,t,ptr,yyvsp[0],NULL,NULL),yyvsp[0]);
  ReleaseAST(yyvsp[-1]);
}
#line 3406 "HolyC.tab.c"
    break;

  case 172:
#line 722 "HolyC.y"
                                     {
  CType *t =CreatePrimType(TYPE_I64);
  CType *ptr =CreateMultiLvlPtr(t,2);
  yyval=SOT(AppendToDecls(NULL,t,ptr,yyvsp[0],NULL,NULL),yyvsp[0]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
}
#line 3418 "HolyC.tab.c"
    break;

  case 173:
#line 729 "HolyC.y"
                                              {
  CType *t =CreatePrimType(TYPE_I64);
  CType *ptr =CreateMultiLvlPtr(t,3);
  yyval=SOT(AppendToDecls(NULL,t,ptr,yyvsp[0],NULL,NULL),yyvsp[0]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
}
#line 3431 "HolyC.tab.c"
    break;

  case 175:
#line 738 "HolyC.y"
            {
    yyval=CreateI64(0);
}
#line 3439 "HolyC.tab.c"
    break;

  case 176:
#line 741 "HolyC.y"
                     {
  yyval=CreateI64(1);
  ReleaseAST(yyvsp[0]);
}
#line 3448 "HolyC.tab.c"
    break;

  case 177:
#line 745 "HolyC.y"
                              {
  yyval=CreateI64(2);
  ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 3457 "HolyC.tab.c"
    break;

  case 178:
#line 749 "HolyC.y"
                                       {
  yyval=CreateI64(3);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[-1]),ReleaseAST(yyvsp[0]);
}
#line 3466 "HolyC.tab.c"
    break;

  case 179:
#line 753 "HolyC.y"
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
#line 3483 "HolyC.tab.c"
    break;

  case 180:
#line 765 "HolyC.y"
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
#line 3501 "HolyC.tab.c"
    break;

  case 181:
#line 778 "HolyC.y"
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
#line 3520 "HolyC.tab.c"
    break;

  case 182:
#line 792 "HolyC.y"
                                                                                     {
  assert(yyvsp[-3]->type==__AST_DECL_TAILS);
  CType *bt=vec_last(&yyvsp[-3]->declTail).finalType;
  vec_last(&yyvsp[-3]->declTail).finalType=CreateArrayType(bt,yyvsp[-1]);
  yyval=yyvsp[-3];
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3533 "HolyC.tab.c"
    break;

  case 183:
#line 800 "HolyC.y"
                                                                     {
  assert(yyvsp[-2]->type==__AST_DECL_TAILS);
  CType *bt=vec_last(&yyvsp[-2]->declTail).finalType;
  vec_last(&yyvsp[-2]->declTail).finalType=CreateArrayType(bt,NULL);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 3546 "HolyC.tab.c"
    break;

  case 184:
#line 809 "HolyC.y"
                                                       {
  assert(yyvsp[-2]->type==__AST_DECL_TAILS);
  vec_last(&yyvsp[-2]->declTail).dft=yyvsp[0];
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 3557 "HolyC.tab.c"
    break;

  case 185:
#line 815 "HolyC.y"
                                                         {
  assert(yyvsp[-2]->type==__AST_DECL_TAILS);
  vec_last(&yyvsp[-2]->declTail).dft=yyvsp[0];
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 3568 "HolyC.tab.c"
    break;

  case 186:
#line 822 "HolyC.y"
                                                {
  DeclsFillInBasetype(yyvsp[-1]->type2,yyvsp[0]);
  yyval=yyvsp[0];
}
#line 3577 "HolyC.tab.c"
    break;

  case 187:
#line 826 "HolyC.y"
                                                    {
  DeclsFillInBasetype(yyvsp[-1]->type2,yyvsp[0]);
  yyval=yyvsp[0];
}
#line 3586 "HolyC.tab.c"
    break;

  case 188:
#line 831 "HolyC.y"
                                                            {
  assert(yyvsp[0]->type==__AST_DECL_TAILS);
  CDeclTail b2=vec_last(&yyvsp[0]->declTail);
  AppendToDecls(yyvsp[-2],b2.basetype,b2.finalType,b2.name,b2.dft,NULL);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[-1]);
}
#line 3598 "HolyC.tab.c"
    break;

  case 191:
#line 841 "HolyC.y"
                                                 {
  DeclsFillInBasetype(yyvsp[-1]->type2,yyvsp[0]);
  yyval=yyvsp[0];
}
#line 3607 "HolyC.tab.c"
    break;

  case 193:
#line 847 "HolyC.y"
                                                   {
  assert(yyvsp[0]->type==__AST_DECL_TAILS);
  CDeclTail b2=vec_last(&yyvsp[0]->declTail);
  yyval=AppendToDecls(yyvsp[-2],b2.basetype,b2.finalType,b2.name,b2.dft,NULL);
  ReleaseAST(yyvsp[-1]);
}
#line 3618 "HolyC.tab.c"
    break;

  case 194:
#line 853 "HolyC.y"
          {yyval=NULL;}
#line 3624 "HolyC.tab.c"
    break;

  case 195:
#line 857 "HolyC.y"
                     {yyval=CreateExternLinkage(NULL);ReleaseAST(yyvsp[0]);}
#line 3630 "HolyC.tab.c"
    break;

  case 196:
#line 858 "HolyC.y"
                     {yyval=CreateImportLinkage(NULL);ReleaseAST(yyvsp[0]);}
#line 3636 "HolyC.tab.c"
    break;

  case 197:
#line 859 "HolyC.y"
                     {yyval=NULL;}
#line 3642 "HolyC.tab.c"
    break;

  case 198:
#line 860 "HolyC.y"
                     {yyval=CreateStaticLinkage();ReleaseAST(yyvsp[0]);}
#line 3648 "HolyC.tab.c"
    break;

  case 199:
#line 861 "HolyC.y"
                              {yyval=CreateExternLinkage(yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3654 "HolyC.tab.c"
    break;

  case 200:
#line 862 "HolyC.y"
                              {yyval=CreateImportLinkage(yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3660 "HolyC.tab.c"
    break;

  case 201:
#line 863 "HolyC.y"
                                  {yyval=CreateExternLinkage(yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3666 "HolyC.tab.c"
    break;

  case 202:
#line 864 "HolyC.y"
                                  {yyval=CreateImportLinkage(yyvsp[0]);ReleaseAST(yyvsp[-1]);}
#line 3672 "HolyC.tab.c"
    break;

  case 203:
#line 868 "HolyC.y"
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
#line 3690 "HolyC.tab.c"
    break;

  case 204:
#line 881 "HolyC.y"
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
#line 3709 "HolyC.tab.c"
    break;

  case 205:
#line 895 "HolyC.y"
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
#line 3727 "HolyC.tab.c"
    break;

  case 206:
#line 908 "HolyC.y"
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
#line 3746 "HolyC.tab.c"
    break;

  case 207:
#line 922 "HolyC.y"
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
#line 3767 "HolyC.tab.c"
    break;

  case 208:
#line 938 "HolyC.y"
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
#line 3786 "HolyC.tab.c"
    break;

  case 209:
#line 952 "HolyC.y"
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
#line 3805 "HolyC.tab.c"
    break;

  case 210:
#line 966 "HolyC.y"
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
#line 3825 "HolyC.tab.c"
    break;

  case 211:
#line 981 "HolyC.y"
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
#line 3843 "HolyC.tab.c"
    break;

  case 212:
#line 994 "HolyC.y"
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
#line 3862 "HolyC.tab.c"
    break;

  case 213:
#line 1008 "HolyC.y"
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
#line 3883 "HolyC.tab.c"
    break;

  case 214:
#line 1024 "HolyC.y"
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
#line 3901 "HolyC.tab.c"
    break;

  case 215:
#line 1042 "HolyC.y"
                    {yyval=yyvsp[0];}
#line 3907 "HolyC.tab.c"
    break;

  case 216:
#line 1043 "HolyC.y"
                    {yyval=yyvsp[0];}
#line 3913 "HolyC.tab.c"
    break;

  case 218:
#line 1046 "HolyC.y"
          {yyval=NULL;}
#line 3919 "HolyC.tab.c"
    break;

  case 219:
#line 1049 "HolyC.y"
                                                                           {
  yyval=SOT(CreateWhile(yyvsp[-1],NULL),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3929 "HolyC.tab.c"
    break;

  case 220:
#line 1055 "HolyC.y"
                                                                                                                         {
  yyval=SOT(CreateFor(yyvsp[-5],yyvsp[-3],yyvsp[-1],NULL),yyvsp[-7]);
  ReleaseAST(yyvsp[-6]);
  ReleaseAST(yyvsp[-4]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3941 "HolyC.tab.c"
    break;

  case 221:
#line 1063 "HolyC.y"
                                                              {
  yyval=SOT(CreateIf(yyvsp[-1],NULL,NULL),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 3951 "HolyC.tab.c"
    break;

  case 222:
#line 1069 "HolyC.y"
                            {
  assert(yyvsp[-1]->type==AST_IF);
  yyval=yyvsp[-1];
  yyvsp[-1]->ifStmt.body=yyvsp[0];
}
#line 3961 "HolyC.tab.c"
    break;

  case 223:
#line 1074 "HolyC.y"
                                              {
  assert(yyvsp[-3]->type==AST_IF);
  yyval=yyvsp[-3];
  yyvsp[-3]->ifStmt.body=yyvsp[-2];
  yyvsp[-3]->ifStmt.elseBody=yyvsp[0];
  ReleaseAST(yyvsp[-1]);
}
#line 3973 "HolyC.tab.c"
    break;

  case 224:
#line 1081 "HolyC.y"
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
#line 3988 "HolyC.tab.c"
    break;

  case 226:
#line 1093 "HolyC.y"
                                              {
  assert(yyvsp[-3]->type==AST_IF);
  yyval=yyvsp[-3];
  yyvsp[-3]->ifStmt.body=yyvsp[-2];
  yyvsp[-3]->ifStmt.elseBody=yyvsp[0];
  ReleaseAST(yyvsp[-1]);
}
#line 4000 "HolyC.tab.c"
    break;

  case 227:
#line 1100 "HolyC.y"
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
#line 4015 "HolyC.tab.c"
    break;

  case 228:
#line 1111 "HolyC.y"
                                                                                     {
  yyval=SOT(CreateDo(yyvsp[-1],yyvsp[-4]),yyvsp[-5]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4026 "HolyC.tab.c"
    break;

  case 229:
#line 1117 "HolyC.y"
                                                                 {
  yyval=SOT(CreateSubswitch(yyvsp[-2]),yyvsp[-4]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4037 "HolyC.tab.c"
    break;

  case 230:
#line 1124 "HolyC.y"
                       {yyval=yyvsp[0];}
#line 4043 "HolyC.tab.c"
    break;

  case 231:
#line 1125 "HolyC.y"
                                         {
  yyval=SOT(CreateDefault(),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4052 "HolyC.tab.c"
    break;

  case 232:
#line 1129 "HolyC.y"
                                              {
  yyval=SOT(CreateCase(yyvsp[-1],NULL),yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4061 "HolyC.tab.c"
    break;

  case 233:
#line 1133 "HolyC.y"
                                      {
  yyval=SOT(CreateCase(NULL,NULL),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4070 "HolyC.tab.c"
    break;

  case 234:
#line 1137 "HolyC.y"
                                                                       {
  yyval=SOT(CreateCase(yyvsp[-3],yyvsp[-1]),yyvsp[-4]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);

}
#line 4081 "HolyC.tab.c"
    break;

  case 235:
#line 1143 "HolyC.y"
                              {yyval=yyvsp[0];}
#line 4087 "HolyC.tab.c"
    break;

  case 236:
#line 1144 "HolyC.y"
                                             {yyval=AppendToStmts(yyvsp[-1],yyvsp[0]);}
#line 4093 "HolyC.tab.c"
    break;

  case 237:
#line 1145 "HolyC.y"
                                {yyval=yyvsp[0];}
#line 4099 "HolyC.tab.c"
    break;

  case 238:
#line 1146 "HolyC.y"
                                                                                                                      {
  yyval=SOT(CreateSwitch(yyvsp[-4],yyvsp[-1]),yyvsp[-6]);
  ReleaseAST(yyvsp[-5]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4111 "HolyC.tab.c"
    break;

  case 239:
#line 1153 "HolyC.y"
                                                                                                       {
  yyval=SOT(CreateSwitch(yyvsp[-3],NULL),yyvsp[-5]);
  ReleaseAST(yyvsp[-4]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4123 "HolyC.tab.c"
    break;

  case 241:
#line 1161 "HolyC.y"
                            {
  AST *r=TD_MALLOC(sizeof(AST));
  r->type=AST_ASM_BLK;
  vec_init(&r->stmts);
  vec_push(&r->stmts,yyvsp[-1]);
  yyval=r;
  ReleaseAST(yyvsp[0]);
}
#line 4136 "HolyC.tab.c"
    break;

  case 242:
#line 1169 "HolyC.y"
                                          {
  vec_push(&yyvsp[-2]->stmts,yyvsp[-1]);
  yyval=yyvsp[-2];
  ReleaseAST(yyvsp[0]);
}
#line 4146 "HolyC.tab.c"
    break;

  case 245:
#line 1176 "HolyC.y"
                                       {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[0]);
}
#line 4155 "HolyC.tab.c"
    break;

  case 246:
#line 1180 "HolyC.y"
                                                  {
  yyval=ApplyLinkageToDecls(yyvsp[-2],yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4164 "HolyC.tab.c"
    break;

  case 248:
#line 1185 "HolyC.y"
                                         {
  yyval=SOT(CreateGoto(yyvsp[-1]),yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4173 "HolyC.tab.c"
    break;

  case 249:
#line 1189 "HolyC.y"
                                   {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[0]);
}
#line 4182 "HolyC.tab.c"
    break;

  case 250:
#line 1193 "HolyC.y"
                                       {
  yyval=yyvsp[-1];
  if(yyvsp[-1]->type==AST_STRING) {
    yyval=SOT(CreatePrint(yyvsp[-1]->string),yyvsp[-1]);
  }
  ReleaseAST(yyvsp[0]);
}
#line 4194 "HolyC.tab.c"
    break;

  case 251:
#line 1200 "HolyC.y"
                        {yyval=yyvsp[0];}
#line 4200 "HolyC.tab.c"
    break;

  case 252:
#line 1201 "HolyC.y"
                         {yyval=yyvsp[0];}
#line 4206 "HolyC.tab.c"
    break;

  case 253:
#line 1202 "HolyC.y"
                                  {
  yyval=SOT(CreateBreak(),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4215 "HolyC.tab.c"
    break;

  case 254:
#line 1206 "HolyC.y"
                         {
  yyval=CreateNop();
  ReleaseAST(yyvsp[0]);
}
#line 4224 "HolyC.tab.c"
    break;

  case 255:
#line 1210 "HolyC.y"
                                           {
  yyval=SOT(CreateReturn(yyvsp[-1]),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4233 "HolyC.tab.c"
    break;

  case 256:
#line 1214 "HolyC.y"
                                 {
  yyval=SOT(CreateReturn(NULL),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4242 "HolyC.tab.c"
    break;

  case 257:
#line 1218 "HolyC.y"
                                  {
  yyval=SOT(CreateLabel(yyvsp[-1]),yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4251 "HolyC.tab.c"
    break;

  case 258:
#line 1222 "HolyC.y"
                         {yyval=yyvsp[0];}
#line 4257 "HolyC.tab.c"
    break;

  case 259:
#line 1223 "HolyC.y"
                         {yyval=yyvsp[0];}
#line 4263 "HolyC.tab.c"
    break;

  case 260:
#line 1224 "HolyC.y"
                                                        {
  CType *t=CreateUnionForwardDecl(yyvsp[-3],yyvsp[-1]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4274 "HolyC.tab.c"
    break;

  case 261:
#line 1230 "HolyC.y"
                                                        {
  CType *t=CreateClassForwardDecl(yyvsp[-3],yyvsp[-1]);
  yyval=CreateTypeNode(t);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4285 "HolyC.tab.c"
    break;

  case 262:
#line 1236 "HolyC.y"
                    {yyval=yyvsp[0];}
#line 4291 "HolyC.tab.c"
    break;

  case 263:
#line 1237 "HolyC.y"
                             {yyval=AppendToStmts(yyvsp[-1],yyvsp[0]);}
#line 4297 "HolyC.tab.c"
    break;

  case 264:
#line 1238 "HolyC.y"
                   {YYABORT;}
#line 4303 "HolyC.tab.c"
    break;

  case 265:
#line 1239 "HolyC.y"
                                                    {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4313 "HolyC.tab.c"
    break;

  case 266:
#line 1244 "HolyC.y"
                                           {
  yyval=CreateNop();
  ReleaseAST(yyvsp[-1]);
  ReleaseAST(yyvsp[0]);
}
#line 4323 "HolyC.tab.c"
    break;

  case 267:
#line 1249 "HolyC.y"
                {yyval=NULL;}
#line 4329 "HolyC.tab.c"
    break;

  case 268:
#line 1250 "HolyC.y"
                                        {
  RunStatement(yyvsp[0]);
  ReleaseAST(yyvsp[0]);
  yyval=NULL;
}
#line 4339 "HolyC.tab.c"
    break;

  case 269:
#line 1255 "HolyC.y"
                                {
  yyval=NULL;
}
#line 4347 "HolyC.tab.c"
    break;

  case 270:
#line 1259 "HolyC.y"
                             {
  RunStatement(yyvsp[0]);
  YYACCEPT;
  yyval=NULL;
}
#line 4357 "HolyC.tab.c"
    break;

  case 271:
#line 1264 "HolyC.y"
                                      {
  RunPtr(CurFuncInfo,yyvsp[-1],CurFramePtr);
  YYACCEPT;
  yyval=NULL;
}
#line 4367 "HolyC.tab.c"
    break;

  case 272:
#line 1269 "HolyC.y"
                                     {
  RunPtr(NULL,yyvsp[0],NULL);
  YYACCEPT;
  yyval=NULL;
}
#line 4377 "HolyC.tab.c"
    break;

  case 273:
#line 1275 "HolyC.y"
                                  {
  RunPtr(CurFuncInfo,yyvsp[0],CurFramePtr);
  YYACCEPT;
  yyval=NULL;
}
#line 4387 "HolyC.tab.c"
    break;

  case 274:
#line 1284 "HolyC.y"
                             {
  yyval=SOT(yyvsp[0],yyvsp[-1]);
  char buffer[256];
  sprintf(buffer,"@@%s",yyval->name);
  TD_FREE(yyval->name);
  yyval->name=strdup(buffer);
  ReleaseAST(yyvsp[-1]);
}
#line 4400 "HolyC.tab.c"
    break;

  case 275:
#line 1292 "HolyC.y"
                               {
 //Make a name token out of $n
  char buffer[256];
  sprintf(buffer,"@@%li",yyvsp[0]->integer);
  AST *n=TD_MALLOC(sizeof(AST));
  n->type=AST_NAME,n->name=strdup(buffer);
  yyval=n;
  ReleaseAST(yyvsp[-1]);
}
#line 4414 "HolyC.tab.c"
    break;

  case 276:
#line 1301 "HolyC.y"
                 {yyval=SLE(yyvsp[0]);}
#line 4420 "HolyC.tab.c"
    break;

  case 277:
#line 1302 "HolyC.y"
               {yyval=SLE(yyvsp[0]);}
#line 4426 "HolyC.tab.c"
    break;

  case 278:
#line 1303 "HolyC.y"
                {yyval=SLE(yyvsp[0]);}
#line 4432 "HolyC.tab.c"
    break;

  case 279:
#line 1304 "HolyC.y"
                  {yyval=SLE(yyvsp[0]);}
#line 4438 "HolyC.tab.c"
    break;

  case 280:
#line 1305 "HolyC.y"
                     {
  yyval=SLE(yyvsp[0]);
}
#line 4446 "HolyC.tab.c"
    break;

  case 281:
#line 1308 "HolyC.y"
                {
  yyval=SLE(yyvsp[0]);
}
#line 4454 "HolyC.tab.c"
    break;

  case 283:
#line 1312 "HolyC.y"
                                                          {
  yyval=yyvsp[-1];
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 4463 "HolyC.tab.c"
    break;

  case 285:
#line 1317 "HolyC.y"
                                     {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4472 "HolyC.tab.c"
    break;

  case 286:
#line 1321 "HolyC.y"
                                        {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4481 "HolyC.tab.c"
    break;

  case 287:
#line 1325 "HolyC.y"
                                    {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4490 "HolyC.tab.c"
    break;

  case 288:
#line 1329 "HolyC.y"
                                      {
  yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4499 "HolyC.tab.c"
    break;

  case 289:
#line 1333 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[-1],AST_POST_DEC),yyvsp[0]);
  ReleaseAST(yyvsp[0]);
}
#line 4508 "HolyC.tab.c"
    break;

  case 290:
#line 1337 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[-1],AST_POST_INC),yyvsp[0]);
  ReleaseAST(yyvsp[0]);
}
#line 4517 "HolyC.tab.c"
    break;

  case 291:
#line 1341 "HolyC.y"
                                                                  {
  yyval=SOT(CreateFuncCall(yyvsp[-3],yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 4526 "HolyC.tab.c"
    break;

  case 292:
#line 1345 "HolyC.y"
                                                                   {
  yyval=CreateExplicitTypecast(yyvsp[-3],yyvsp[-1]);
  ReleaseAST(yyvsp[-2]),ReleaseAST(yyvsp[0]);
}
#line 4535 "HolyC.tab.c"
    break;

  case 294:
#line 1350 "HolyC.y"
                             {
  yyval=SOT(CreateSizeof(yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4544 "HolyC.tab.c"
    break;

  case 295:
#line 1354 "HolyC.y"
                                    {
  yyval=SOT(CreateSizeof(yyvsp[0]),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4553 "HolyC.tab.c"
    break;

  case 296:
#line 1358 "HolyC.y"
                                                                     {
  yyval=SOT(CreateSizeof(yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4564 "HolyC.tab.c"
    break;

  case 297:
#line 1364 "HolyC.y"
                                                                      {
  yyval=SOT(CreateSizeof(yyvsp[-1]),yyvsp[-3]);
  ReleaseAST(yyvsp[-3]);
  ReleaseAST(yyvsp[-2]);
  ReleaseAST(yyvsp[0]);
}
#line 4575 "HolyC.tab.c"
    break;

  case 298:
#line 1370 "HolyC.y"
                             {
  yyval=SOT(CreateUnop(yyvsp[0],AST_ADDROF),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4584 "HolyC.tab.c"
    break;

  case 299:
#line 1374 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[0],AST_DERREF),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4593 "HolyC.tab.c"
    break;

  case 300:
#line 1379 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[0],AST_PRE_DEC),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4602 "HolyC.tab.c"
    break;

  case 301:
#line 1383 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[0],AST_PRE_INC),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4611 "HolyC.tab.c"
    break;

  case 302:
#line 1387 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[0],AST_POS),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4620 "HolyC.tab.c"
    break;

  case 303:
#line 1391 "HolyC.y"
                            {
  yyval=SOT(CreateUnop(yyvsp[0],AST_NEG),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4629 "HolyC.tab.c"
    break;

  case 304:
#line 1395 "HolyC.y"
                             {
  yyval=SOT(CreateUnop(yyvsp[0],AST_LNOT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4638 "HolyC.tab.c"
    break;

  case 305:
#line 1399 "HolyC.y"
                             {
  yyval=SOT(CreateUnop(yyvsp[0],AST_BNOT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4647 "HolyC.tab.c"
    break;

  case 307:
#line 1406 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_POW),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4656 "HolyC.tab.c"
    break;

  case 308:
#line 1410 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SHL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4665 "HolyC.tab.c"
    break;

  case 309:
#line 1414 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SHR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4674 "HolyC.tab.c"
    break;

  case 311:
#line 1420 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_MUL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4683 "HolyC.tab.c"
    break;

  case 312:
#line 1424 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_DIV),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4692 "HolyC.tab.c"
    break;

  case 313:
#line 1428 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_MOD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4701 "HolyC.tab.c"
    break;

  case 315:
#line 1434 "HolyC.y"
                                       {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4710 "HolyC.tab.c"
    break;

  case 317:
#line 1440 "HolyC.y"
                                       {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4719 "HolyC.tab.c"
    break;

  case 319:
#line 1446 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4728 "HolyC.tab.c"
    break;

  case 321:
#line 1452 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ADD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4737 "HolyC.tab.c"
    break;

  case 322:
#line 1456 "HolyC.y"
                                      {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SUB),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4746 "HolyC.tab.c"
    break;

  case 324:
#line 1462 "HolyC.y"
                                     {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_LT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4755 "HolyC.tab.c"
    break;

  case 325:
#line 1466 "HolyC.y"
                                     {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_GT),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4764 "HolyC.tab.c"
    break;

  case 326:
#line 1470 "HolyC.y"
                                     {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_LE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4773 "HolyC.tab.c"
    break;

  case 327:
#line 1474 "HolyC.y"
                                     {
  yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_GE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4782 "HolyC.tab.c"
    break;

  case 329:
#line 1480 "HolyC.y"
                                        {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_EQ),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4791 "HolyC.tab.c"
    break;

  case 330:
#line 1484 "HolyC.y"
                                        {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_NE),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4800 "HolyC.tab.c"
    break;

  case 332:
#line 1490 "HolyC.y"
                                          {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4809 "HolyC.tab.c"
    break;

  case 334:
#line 1496 "HolyC.y"
                                          {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4818 "HolyC.tab.c"
    break;

  case 336:
#line 1502 "HolyC.y"
                                         {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4827 "HolyC.tab.c"
    break;

  case 338:
#line 1508 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4836 "HolyC.tab.c"
    break;

  case 339:
#line 1512 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SHL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4845 "HolyC.tab.c"
    break;

  case 340:
#line 1516 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SHR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4854 "HolyC.tab.c"
    break;

  case 341:
#line 1520 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_MUL),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4863 "HolyC.tab.c"
    break;

  case 342:
#line 1524 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_DIV),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4872 "HolyC.tab.c"
    break;

  case 343:
#line 1528 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_MOD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4881 "HolyC.tab.c"
    break;

  case 344:
#line 1532 "HolyC.y"
                                             {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BAND),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4890 "HolyC.tab.c"
    break;

  case 345:
#line 1536 "HolyC.y"
                                             {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BXOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4899 "HolyC.tab.c"
    break;

  case 346:
#line 1540 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BOR),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4908 "HolyC.tab.c"
    break;

  case 347:
#line 1544 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_ADD),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4917 "HolyC.tab.c"
    break;

  case 348:
#line 1548 "HolyC.y"
                                            {
  yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SUB),yyvsp[-1]);
  ReleaseAST(yyvsp[-1]);
}
#line 4926 "HolyC.tab.c"
    break;


#line 4930 "HolyC.tab.c"

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
#line 1554 "HolyC.y"

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
