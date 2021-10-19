/* A Bison parser, made by GNU Bison 3.7.4.  */

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
#define YYBISON 30704

/* Bison version string.  */
#define YYBISON_VERSION "3.7.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Substitute the type names.  */
#define YYSTYPE         DBGRSTYPE
/* Substitute the variable and function names.  */
#define yyparse         dbgrparse
#define yylex           dbgrlex
#define yyerror         dbgrerror
#define yydebug         dbgrdebug
#define yynerrs         dbgrnerrs
#define yylval          dbgrlval
#define yychar          dbgrchar

/* First part of user prologue.  */
#line 4 "dbgExp.y"

#include "3d.h"
#include <stdio.h>
int yylex();
void yyerror (char const *s);
#define SOT(a,tok) SetPosFromOther(a,tok)
#define SLE(a) SetPosFromLex(a)

#line 88 "y.tab.c"

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


/* Debug traces.  */
#ifndef DBGRDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define DBGRDEBUG 1
#  else
#   define DBGRDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define DBGRDEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined DBGRDEBUG */
#if DBGRDEBUG
extern int dbgrdebug;
#endif

/* Token kinds.  */
#ifndef DBGRTOKENTYPE
# define DBGRTOKENTYPE
enum dbgrtokentype
{
    DBGREMPTY = -2,
    DBGREOF = 0,                   /* "end of file"  */
    DBGRerror = 256,               /* error  */
    DBGRUNDEF = 257,               /* "invalid token"  */
    CHAR = 258,                    /* CHAR  */
    INT = 259,                     /* INT  */
    FLOAT = 260,                   /* FLOAT  */
    DOT = 261,                     /* DOT  */
    ARROW = 262,                   /* ARROW  */
    SIZEOF = 263,                  /* SIZEOF  */
    ADDR_OF = 264,                 /* ADDR_OF  */
    LNOT = 265,                    /* LNOT  */
    BNOT = 266,                    /* BNOT  */
    POW = 267,                     /* POW  */
    SHL = 268,                     /* SHL  */
    SHR = 269,                     /* SHR  */
    MUL = 270,                     /* MUL  */
    DIV = 271,                     /* DIV  */
    MOD = 272,                     /* MOD  */
    BAND = 273,                    /* BAND  */
    BOR = 274,                     /* BOR  */
    BXOR = 275,                    /* BXOR  */
    ADD = 276,                     /* ADD  */
    SUB = 277,                     /* SUB  */
    LT = 278,                      /* LT  */
    GT = 279,                      /* GT  */
    LE = 280,                      /* LE  */
    GE = 281,                      /* GE  */
    NE = 282,                      /* NE  */
    EQ = 283,                      /* EQ  */
    LAND = 284,                    /* LAND  */
    LXOR = 285,                    /* LXOR  */
    LOR = 286,                     /* LOR  */
    ASSIGN = 287,                  /* ASSIGN  */
    EQ_SHL = 288,                  /* EQ_SHL  */
    EQ_SHR = 289,                  /* EQ_SHR  */
    EQ_MUL = 290,                  /* EQ_MUL  */
    EQ_DIV = 291,                  /* EQ_DIV  */
    EQ_MOD = 292,                  /* EQ_MOD  */
    EQ_BAND = 293,                 /* EQ_BAND  */
    EQ_BXOR = 294,                 /* EQ_BXOR  */
    EQ_BOR = 295,                  /* EQ_BOR  */
    EQ_ADD = 296,                  /* EQ_ADD  */
    EQ_SUB = 297,                  /* EQ_SUB  */
    COMMA = 298,                   /* COMMA  */
    OTHER = 299,                   /* OTHER  */
    TRY = 300,                     /* TRY  */
    CATCH = 301,                   /* CATCH  */
    LASTCLASS = 302,               /* LASTCLASS  */
    U0 = 303,                      /* U0  */
    LEFT_PAREN = 304,              /* LEFT_PAREN  */
    RIGHT_PAREN = 305,             /* RIGHT_PAREN  */
    INC = 306,                     /* INC  */
    DEC = 307,                     /* DEC  */
    NAME = 308,                    /* NAME  */
    LEFT_SQAURE = 309,             /* LEFT_SQAURE  */
    RIGHT_SQAURE = 310,            /* RIGHT_SQAURE  */
    SEMI = 311,                    /* SEMI  */
    IF = 312,                      /* IF  */
    ELSE = 313,                    /* ELSE  */
    DO = 314,                      /* DO  */
    WHILE = 315,                   /* WHILE  */
    FOR = 316,                     /* FOR  */
    LEFT_CURLY = 317,              /* LEFT_CURLY  */
    RIGHT_CURLY = 318,             /* RIGHT_CURLY  */
    CASE = 319,                    /* CASE  */
    COLON = 320,                   /* COLON  */
    DOT_DOT_DOT = 321,             /* DOT_DOT_DOT  */
    EXTERN2 = 322,                 /* EXTERN2  */
    LOCK = 323,                    /* LOCK  */
    EXTERN = 324,                  /* EXTERN  */
    IMPORT = 325,                  /* IMPORT  */
    IMPORT2 = 326,                 /* IMPORT2  */
    STATIC = 327,                  /* STATIC  */
    PUBLIC = 328,                  /* PUBLIC  */
    CLASS = 329,                   /* CLASS  */
    UNION = 330,                   /* UNION  */
    INTERN = 331,                  /* INTERN  */
    START = 332,                   /* START  */
    END = 333,                     /* END  */
    DEFAULT = 334,                 /* DEFAULT  */
    BREAK = 335,                   /* BREAK  */
    RET = 336,                     /* RET  */
    GOTO = 337,                    /* GOTO  */
    BOOL = 338,                    /* BOOL  */
    U8 = 339,                      /* U8  */
    I8 = 340,                      /* I8  */
    U16 = 341,                     /* U16  */
    I16 = 342,                     /* I16  */
    U32 = 343,                     /* U32  */
    I32 = 344,                     /* I32  */
    U64 = 345,                     /* U64  */
    I64 = 346,                     /* I64  */
    F64 = 347,                     /* F64  */
    SWITCH = 348,                  /* SWITCH  */
    TYPENAME = 349,                /* TYPENAME  */
    STRING = 350                   /* STRING  */
};
typedef enum dbgrtokentype dbgrtoken_kind_t;
#endif
/* Token kinds.  */
#define DBGREMPTY -2
#define DBGREOF 0
#define DBGRerror 256
#define DBGRUNDEF 257
#define CHAR 258
#define INT 259
#define FLOAT 260
#define DOT 261
#define ARROW 262
#define SIZEOF 263
#define ADDR_OF 264
#define LNOT 265
#define BNOT 266
#define POW 267
#define SHL 268
#define SHR 269
#define MUL 270
#define DIV 271
#define MOD 272
#define BAND 273
#define BOR 274
#define BXOR 275
#define ADD 276
#define SUB 277
#define LT 278
#define GT 279
#define LE 280
#define GE 281
#define NE 282
#define EQ 283
#define LAND 284
#define LXOR 285
#define LOR 286
#define ASSIGN 287
#define EQ_SHL 288
#define EQ_SHR 289
#define EQ_MUL 290
#define EQ_DIV 291
#define EQ_MOD 292
#define EQ_BAND 293
#define EQ_BXOR 294
#define EQ_BOR 295
#define EQ_ADD 296
#define EQ_SUB 297
#define COMMA 298
#define OTHER 299
#define TRY 300
#define CATCH 301
#define LASTCLASS 302
#define U0 303
#define LEFT_PAREN 304
#define RIGHT_PAREN 305
#define INC 306
#define DEC 307
#define NAME 308
#define LEFT_SQAURE 309
#define RIGHT_SQAURE 310
#define SEMI 311
#define IF 312
#define ELSE 313
#define DO 314
#define WHILE 315
#define FOR 316
#define LEFT_CURLY 317
#define RIGHT_CURLY 318
#define CASE 319
#define COLON 320
#define DOT_DOT_DOT 321
#define EXTERN2 322
#define LOCK 323
#define EXTERN 324
#define IMPORT 325
#define IMPORT2 326
#define STATIC 327
#define PUBLIC 328
#define CLASS 329
#define UNION 330
#define INTERN 331
#define START 332
#define END 333
#define DEFAULT 334
#define BREAK 335
#define RET 336
#define GOTO 337
#define BOOL 338
#define U8 339
#define I8 340
#define U16 341
#define I16 342
#define U32 343
#define I32 344
#define U64 345
#define I64 346
#define F64 347
#define SWITCH 348
#define TYPENAME 349
#define STRING 350

/* Value type.  */
#if ! defined DBGRSTYPE && ! defined DBGRSTYPE_IS_DECLARED
typedef int DBGRSTYPE;
# define DBGRSTYPE_IS_TRIVIAL 1
# define DBGRSTYPE_IS_DECLARED 1
#endif


extern DBGRSTYPE dbgrlval;

int dbgrparse (void);


/* Symbol kind.  */
enum yysymbol_kind_t
{
    D_YYEMPTY = -2,
    D_YYEOF = 0,                          /* "end of file"  */
    D_YYerror = 1,                        /* error  */
    D_YYUNDEF = 2,                        /* "invalid token"  */
    D_CHAR = 3,                           /* CHAR  */
    D_INT = 4,                            /* INT  */
    D_FLOAT = 5,                          /* FLOAT  */
    D_DOT = 6,                            /* DOT  */
    D_ARROW = 7,                          /* ARROW  */
    D_SIZEOF = 8,                         /* SIZEOF  */
    D_ADDR_OF = 9,                        /* ADDR_OF  */
    D_LNOT = 10,                          /* LNOT  */
    D_BNOT = 11,                          /* BNOT  */
    D_POW = 12,                           /* POW  */
    D_SHL = 13,                           /* SHL  */
    D_SHR = 14,                           /* SHR  */
    D_MUL = 15,                           /* MUL  */
    D_DIV = 16,                           /* DIV  */
    D_MOD = 17,                           /* MOD  */
    D_BAND = 18,                          /* BAND  */
    D_BOR = 19,                           /* BOR  */
    D_BXOR = 20,                          /* BXOR  */
    D_ADD = 21,                           /* ADD  */
    D_SUB = 22,                           /* SUB  */
    D_LT = 23,                            /* LT  */
    D_GT = 24,                            /* GT  */
    D_LE = 25,                            /* LE  */
    D_GE = 26,                            /* GE  */
    D_NE = 27,                            /* NE  */
    D_EQ = 28,                            /* EQ  */
    D_LAND = 29,                          /* LAND  */
    D_LXOR = 30,                          /* LXOR  */
    D_LOR = 31,                           /* LOR  */
    D_ASSIGN = 32,                        /* ASSIGN  */
    D_EQ_SHL = 33,                        /* EQ_SHL  */
    D_EQ_SHR = 34,                        /* EQ_SHR  */
    D_EQ_MUL = 35,                        /* EQ_MUL  */
    D_EQ_DIV = 36,                        /* EQ_DIV  */
    D_EQ_MOD = 37,                        /* EQ_MOD  */
    D_EQ_BAND = 38,                       /* EQ_BAND  */
    D_EQ_BXOR = 39,                       /* EQ_BXOR  */
    D_EQ_BOR = 40,                        /* EQ_BOR  */
    D_EQ_ADD = 41,                        /* EQ_ADD  */
    D_EQ_SUB = 42,                        /* EQ_SUB  */
    D_COMMA = 43,                         /* COMMA  */
    D_OTHER = 44,                         /* OTHER  */
    D_TRY = 45,                           /* TRY  */
    D_CATCH = 46,                         /* CATCH  */
    D_LASTCLASS = 47,                     /* LASTCLASS  */
    D_U0 = 48,                            /* U0  */
    D_LEFT_PAREN = 49,                    /* LEFT_PAREN  */
    D_RIGHT_PAREN = 50,                   /* RIGHT_PAREN  */
    D_INC = 51,                           /* INC  */
    D_DEC = 52,                           /* DEC  */
    D_NAME = 53,                          /* NAME  */
    D_LEFT_SQAURE = 54,                   /* LEFT_SQAURE  */
    D_RIGHT_SQAURE = 55,                  /* RIGHT_SQAURE  */
    D_SEMI = 56,                          /* SEMI  */
    D_IF = 57,                            /* IF  */
    D_ELSE = 58,                          /* ELSE  */
    D_DO = 59,                            /* DO  */
    D_WHILE = 60,                         /* WHILE  */
    D_FOR = 61,                           /* FOR  */
    D_LEFT_CURLY = 62,                    /* LEFT_CURLY  */
    D_RIGHT_CURLY = 63,                   /* RIGHT_CURLY  */
    D_CASE = 64,                          /* CASE  */
    D_COLON = 65,                         /* COLON  */
    D_DOT_DOT_DOT = 66,                   /* DOT_DOT_DOT  */
    D_EXTERN2 = 67,                       /* EXTERN2  */
    D_LOCK = 68,                          /* LOCK  */
    D_EXTERN = 69,                        /* EXTERN  */
    D_IMPORT = 70,                        /* IMPORT  */
    D_IMPORT2 = 71,                       /* IMPORT2  */
    D_STATIC = 72,                        /* STATIC  */
    D_PUBLIC = 73,                        /* PUBLIC  */
    D_CLASS = 74,                         /* CLASS  */
    D_UNION = 75,                         /* UNION  */
    D_INTERN = 76,                        /* INTERN  */
    D_START = 77,                         /* START  */
    D_END = 78,                           /* END  */
    D_DEFAULT = 79,                       /* DEFAULT  */
    D_BREAK = 80,                         /* BREAK  */
    D_RET = 81,                           /* RET  */
    D_GOTO = 82,                          /* GOTO  */
    D_BOOL = 83,                          /* BOOL  */
    D_U8 = 84,                            /* U8  */
    D_I8 = 85,                            /* I8  */
    D_U16 = 86,                           /* U16  */
    D_I16 = 87,                           /* I16  */
    D_U32 = 88,                           /* U32  */
    D_I32 = 89,                           /* I32  */
    D_U64 = 90,                           /* U64  */
    D_I64 = 91,                           /* I64  */
    D_F64 = 92,                           /* F64  */
    D_SWITCH = 93,                        /* SWITCH  */
    D_TYPENAME = 94,                      /* TYPENAME  */
    D_STRING = 95,                        /* STRING  */
    D_YYACCEPT = 96,                      /* $accept  */
    D_expr0 = 97,                         /* expr0  */
    D__callargs = 98,                     /* _callargs  */
    D_callargs = 99,                      /* callargs  */
    D_expr1 = 100,                        /* expr1  */
    D_expr2 = 101,                        /* expr2  */
    D_expr3 = 102,                        /* expr3  */
    D_expr4 = 103,                        /* expr4  */
    D_expr4_5 = 104,                      /* expr4_5  */
    D_expr5 = 105,                        /* expr5  */
    D_expr6 = 106,                        /* expr6  */
    D_expr7 = 107,                        /* expr7  */
    D_expr8 = 108,                        /* expr8  */
    D_expr9 = 109,                        /* expr9  */
    D_expr10 = 110,                       /* expr10  */
    D_expr11 = 111,                       /* expr11  */
    D_expr12 = 112,                       /* expr12  */
    D_expr13 = 113,                       /* expr13  */
    D_expr14 = 114,                       /* expr14  */
    D_expr = 115,                         /* expr  */
    D_expr_comma = 116                    /* expr_comma  */
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
typedef yytype_uint8 yy_state_t;

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
         || (defined DBGRSTYPE_IS_TRIVIAL && DBGRSTYPE_IS_TRIVIAL)))

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
#define YYFINAL  83
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   152

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  96
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  21
/* YYNRULES -- Number of rules.  */
#define YYNRULES  77
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  130

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   350


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : D_YYUNDEF)

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
    95
};

#if DBGRDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
    0,    51,    51,    52,    53,    54,    55,    58,    59,    60,
    63,    64,    65,    67,    68,    71,    74,    77,    80,    83,
    87,    88,    91,    96,   101,   104,   108,   111,   114,   117,
    120,   123,   128,   129,   132,   135,   139,   140,   143,   146,
    150,   151,   155,   156,   160,   161,   165,   166,   169,   173,
    174,   177,   180,   183,   187,   188,   191,   195,   196,   200,
    201,   205,   206,   210,   211,   214,   217,   220,   223,   226,
    229,   232,   235,   238,   241,   245,   247,   250
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if DBGRDEBUG || 0
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
    "TYPENAME", "STRING", "$accept", "expr0", "_callargs", "callargs",
    "expr1", "expr2", "expr3", "expr4", "expr4_5", "expr5", "expr6", "expr7",
    "expr8", "expr9", "expr10", "expr11", "expr12", "expr13", "expr14",
    "expr", "expr_comma", YY_NULLPTR
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
    345,   346,   347,   348,   349,   350
};
#endif

#define YYPACT_NINF (-64)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
    57,   -64,   -64,   -64,    -3,    57,    57,    57,    57,    57,
    57,    57,    57,    57,   -64,   -64,   -64,   -64,    31,   -64,
    39,    54,   -12,    11,    21,    52,    10,    -2,    16,    24,
    25,    79,   -64,     4,     6,   -64,   -64,   -64,   -64,   -64,
    -64,   -64,   -64,   -21,   -64,   -64,    43,    44,    57,   -64,
    -64,    57,    57,    57,    57,    57,    57,    57,    57,    57,
    57,    57,    57,    57,    57,    57,    57,    57,    57,    57,
    57,    57,    57,    57,    57,    57,    57,    57,    57,    57,
    57,    57,    57,   -64,    57,    48,   -64,   -64,   -64,   -64,
    -20,   -64,   -23,   -64,   -64,   -64,   -64,   -64,   -64,   -64,
    -64,   -64,   -64,   -64,   -64,   -64,   -64,   -64,   -64,   -64,
    -64,   -64,   -64,   -64,   -64,   -64,   -64,   -64,   -64,   -64,
    -64,   -64,   -64,   -64,   -64,   -64,    57,   -64,   -64,   -64
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
    0,     4,     3,     2,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     6,     5,    11,    13,    20,    32,
    36,    40,    42,    44,    46,    49,    54,    57,    59,    61,
    63,    75,    77,     0,     0,    22,    21,    30,    31,    25,
    24,    28,    29,     0,    27,    26,     0,     0,     7,    18,
    17,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     1,     0,     0,    12,    14,    15,    10,
    0,     8,     0,    33,    34,    35,    37,    38,    39,    41,
    43,    45,    47,    48,    50,    51,    52,    53,    56,    55,
    58,    60,    62,    64,    65,    66,    67,    68,    69,    70,
    71,    72,    73,    74,    76,    23,     7,    19,    16,     9
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -64,   -64,   -63,   -64,   -64,   -64,   129,    34,    38,     8,
        40,    42,    15,   -22,    22,    35,    14,    32,    50,   -45,
        13
    };

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
    -1,    16,    89,    90,    17,    18,    19,    20,    21,    22,
        23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
        43
    };

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
    1,     2,     3,    91,    83,     4,    58,     5,     6,     1,
    2,     3,     7,    33,     4,     8,     5,     6,     9,    10,
    84,     7,    84,   126,     8,    67,    68,     9,    10,    86,
    127,    59,   128,    63,    64,    65,    66,    46,    47,   124,
    60,   104,   105,   106,   107,    69,    34,    84,    12,    13,
    14,    52,    53,    54,    70,    11,    71,    12,    13,    14,
    1,     2,     3,   129,    92,     4,    99,     5,     6,    55,
    56,    57,     7,    61,    62,     8,   102,   103,     9,    10,
    48,    91,    49,    50,   111,    51,    93,    94,    95,   108,
    109,    35,    15,    96,    97,    98,    87,    88,   125,   100,
    85,    15,   101,   112,   110,     0,    11,     0,    12,    13,
    14,    72,    73,    74,    75,    76,    77,    78,    79,    80,
    81,    82,   113,   114,   115,   116,   117,   118,   119,   120,
    121,   122,   123,    36,    37,    38,    39,    40,    41,    42,
    0,    44,    45,     0,     0,     0,     0,     0,     0,     0,
    0,     0,    15
};

static const yytype_int8 yycheck[] =
{
    3,     4,     5,    48,     0,     8,    18,    10,    11,     3,
    4,     5,    15,     0,     8,    18,    10,    11,    21,    22,
    43,    15,    43,    43,    18,    27,    28,    21,    22,    50,
    50,    20,    55,    23,    24,    25,    26,     6,     7,    84,
    19,    63,    64,    65,    66,    29,    49,    43,    51,    52,
    53,    12,    13,    14,    30,    49,    31,    51,    52,    53,
    3,     4,     5,   126,    51,     8,    58,    10,    11,    15,
    16,    17,    15,    21,    22,    18,    61,    62,    21,    22,
    49,   126,    51,    52,    70,    54,    52,    53,    54,    67,
    68,    94,    95,    55,    56,    57,    53,    53,    50,    59,
    94,    95,    60,    71,    69,    -1,    49,    -1,    51,    52,
    53,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    41,    42,    72,    73,    74,    75,    76,    77,    78,    79,
    80,    81,    82,     4,     5,     6,     7,     8,     9,    10,
    -1,    12,    13,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    95
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
    0,     3,     4,     5,     8,    10,    11,    15,    18,    21,
    22,    49,    51,    52,    53,    95,    97,   100,   101,   102,
    103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
    113,   114,   115,   116,    49,    94,   102,   102,   102,   102,
    102,   102,   102,   116,   102,   102,     6,     7,    49,    51,
    52,    54,    12,    13,    14,    15,    16,    17,    18,    20,
    19,    21,    22,    23,    24,    25,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,    42,     0,    43,    94,    50,    53,    53,    98,
    99,   115,   116,   103,   103,   103,   104,   104,   104,   105,
    106,   107,   108,   108,   109,   109,   109,   109,   110,   110,
    111,   112,   113,   114,   114,   114,   114,   114,   114,   114,
    114,   114,   114,   114,   115,    50,    43,    50,    55,    98
};

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
    0,    96,    97,    97,    97,    97,    97,    98,    98,    99,
    99,   100,   100,   101,   101,   101,   101,   101,   101,   101,
    102,   102,   102,   102,   102,   102,   102,   102,   102,   102,
    102,   102,   103,   103,   103,   103,   104,   104,   104,   104,
    105,   105,   106,   106,   107,   107,   108,   108,   108,   109,
    109,   109,   109,   109,   110,   110,   110,   111,   111,   112,
    112,   113,   113,   114,   114,   114,   114,   114,   114,   114,
    114,   114,   114,   114,   114,   115,   116,   116
};

/* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
    0,     2,     1,     1,     1,     1,     1,     0,     1,     3,
    1,     1,     3,     1,     3,     3,     4,     2,     2,     4,
    1,     2,     2,     4,     2,     2,     2,     2,     2,     2,
    2,     2,     1,     3,     3,     3,     1,     3,     3,     3,
    1,     3,     1,     3,     1,     3,     1,     3,     3,     1,
    3,     3,     3,     3,     1,     3,     3,     1,     3,     1,
    3,     1,     3,     1,     3,     3,     3,     3,     3,     3,
    3,     3,     3,     3,     3,     1,     3,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = DBGREMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == DBGREMPTY)                                        \
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
   Use DBGRerror or DBGRUNDEF. */
#define YYERRCODE DBGRUNDEF


/* Enable debugging if requested.  */
#if DBGRDEBUG

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
    YYUSE (yyoutput);
    if (!yyvaluep)
        return;
# ifdef YYPRINT
    if (yykind < YYNTOKENS)
        YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
    YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
    YYUSE (yykind);
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
#else /* !DBGRDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !DBGRDEBUG */


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
    YYUSE (yyvaluep);
    if (!yymsg)
        yymsg = "Deleting";
    YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

    YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
    YYUSE (yykind);
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
    yysymbol_kind_t yytoken = D_YYEMPTY;
    /* The variables used to return semantic value and location from the
       action routines.  */
    YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

    /* The number of symbols on the RHS of the reduced rule.
       Keep to zero when no symbol should be popped.  */
    int yylen = 0;

    YYDPRINTF ((stderr, "Starting parse\n"));

    yychar = DBGREMPTY; /* Cause a token to be read.  */
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
    if (yychar == DBGREMPTY)
    {
        YYDPRINTF ((stderr, "Reading a token\n"));
        yychar = yylex ();
    }

    if (yychar <= DBGREOF)
    {
        yychar = DBGREOF;
        yytoken = D_YYEOF;
        YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
    else if (yychar == DBGRerror)
    {
        /* The scanner already issued an error message, process directly
           to error recovery.  But do not keep the error token as
           lookahead, it is too special and may lead us to an endless
           loop in error recovery. */
        yychar = DBGRUNDEF;
        yytoken = D_YYerror;
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
    yychar = DBGREMPTY;
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
#line 51 "dbgExp.y"
        {
            yyval=SLE(yyvsp[0]);
        }
#line 1529 "y.tab.c"
        break;

    case 3: /* expr0: INT  */
#line 52 "dbgExp.y"
        {
            yyval=SLE(yyvsp[0]);
        }
#line 1535 "y.tab.c"
        break;

    case 4: /* expr0: CHAR  */
#line 53 "dbgExp.y"
        {
            yyval=SLE(yyvsp[0]);
        }
#line 1541 "y.tab.c"
        break;

    case 5: /* expr0: STRING  */
#line 54 "dbgExp.y"
        {
            yyval=SLE(yyvsp[0]);
        }
#line 1547 "y.tab.c"
        break;

    case 6: /* expr0: NAME  */
#line 55 "dbgExp.y"
        {
            yyval=SLE(yyvsp[0]);
        }
#line 1555 "y.tab.c"
        break;

    case 7: /* _callargs: %empty  */
#line 58 "dbgExp.y"
        {
            yyval=NULL;
        }
#line 1561 "y.tab.c"
        break;

    case 8: /* _callargs: expr  */
#line 59 "dbgExp.y"
        {
            yyval=yyvsp[0];
        }
#line 1567 "y.tab.c"
        break;

    case 9: /* callargs: callargs COMMA _callargs  */
#line 60 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA),yyvsp[-1]);
        }
#line 1575 "y.tab.c"
        break;

    case 10: /* callargs: _callargs  */
#line 63 "dbgExp.y"
        {
            yyval=yyvsp[0];
        }
#line 1581 "y.tab.c"
        break;

    case 12: /* expr1: LEFT_PAREN expr_comma RIGHT_PAREN  */
#line 65 "dbgExp.y"
        {
            yyval=yyvsp[-1];
        }
#line 1587 "y.tab.c"
        break;

    case 14: /* expr2: expr2 DOT NAME  */
#line 68 "dbgExp.y"
        {
            yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
        }
#line 1595 "y.tab.c"
        break;

    case 15: /* expr2: expr2 ARROW NAME  */
#line 71 "dbgExp.y"
        {
            yyval=SOT(CreateMemberAccess(yyvsp[-2],yyvsp[0]),yyvsp[-1]);
        }
#line 1603 "y.tab.c"
        break;

    case 16: /* expr2: expr2 LEFT_SQAURE expr_comma RIGHT_SQAURE  */
#line 74 "dbgExp.y"
        {
            yyval=SOT(CreateArrayAccess(yyvsp[-3],yyvsp[-1]),yyvsp[-3]);
        }
#line 1611 "y.tab.c"
        break;

    case 17: /* expr2: expr2 DEC  */
#line 77 "dbgExp.y"
        {
            yyval=SOT(CreateUnop(yyvsp[-1],AST_POST_DEC),yyvsp[0]);
        }
#line 1619 "y.tab.c"
        break;

    case 18: /* expr2: expr2 INC  */
#line 80 "dbgExp.y"
        {
            yyval=SOT(CreateUnop(yyvsp[-1],AST_POST_INC),yyvsp[0]);
        }
#line 1627 "y.tab.c"
        break;

    case 19: /* expr2: expr2 LEFT_PAREN callargs RIGHT_PAREN  */
#line 83 "dbgExp.y"
        {
            yyval=CreateFuncCall(yyvsp[-3],yyvsp[-1]);
        }
#line 1635 "y.tab.c"
        break;

    case 21: /* expr3: SIZEOF expr3  */
#line 88 "dbgExp.y"
        {
            yyval=SOT(CreateSizeof(yyvsp[0]),yyvsp[-1]);
        }
#line 1643 "y.tab.c"
        break;

    case 22: /* expr3: SIZEOF TYPENAME  */
#line 91 "dbgExp.y"
        {
            CType *t=*map_get(&Compiler.types,yyvsp[0]->name);
            AST *node AF_AST=CreateTypeNode(t);
            yyval=SOT(CreateSizeof(node),yyvsp[-1]);
        }
#line 1653 "y.tab.c"
        break;

    case 23: /* expr3: SIZEOF LEFT_PAREN TYPENAME RIGHT_PAREN  */
#line 96 "dbgExp.y"
        {
            CType *t=*map_get(&Compiler.types,yyvsp[-1]->name);
            AST *node AF_AST=CreateTypeNode(t);
            yyval=SOT(CreateSizeof(node),yyvsp[-3]);
        }
#line 1663 "y.tab.c"
        break;

    case 24: /* expr3: BAND expr3  */
#line 101 "dbgExp.y"
        {
            yyval=SOT(CreateUnop(yyvsp[0],AST_ADDROF),yyvsp[-1]);
        }
#line 1671 "y.tab.c"
        break;

    case 25: /* expr3: MUL expr3  */
#line 104 "dbgExp.y"
        {
            yyval=SOT(CreateUnop(yyvsp[0],AST_DERREF),yyvsp[-1]);
        }
#line 1679 "y.tab.c"
        break;

    case 26: /* expr3: DEC expr3  */
#line 108 "dbgExp.y"
        {
            yyval=SOT(CreateUnop(yyvsp[0],AST_PRE_DEC),yyvsp[-1]);
        }
#line 1687 "y.tab.c"
        break;

    case 27: /* expr3: INC expr3  */
#line 111 "dbgExp.y"
        {
            yyval=SOT(CreateUnop(yyvsp[0],AST_PRE_INC),yyvsp[-1]);
        }
#line 1695 "y.tab.c"
        break;

    case 28: /* expr3: ADD expr3  */
#line 114 "dbgExp.y"
        {
            yyval=SOT(CreateUnop(yyvsp[0],AST_POS),yyvsp[-1]);
        }
#line 1703 "y.tab.c"
        break;

    case 29: /* expr3: SUB expr3  */
#line 117 "dbgExp.y"
        {
            yyval=SOT(CreateUnop(yyvsp[0],AST_NEG),yyvsp[-1]);
        }
#line 1711 "y.tab.c"
        break;

    case 30: /* expr3: LNOT expr3  */
#line 120 "dbgExp.y"
        {
            yyval=SOT(CreateUnop(yyvsp[0],AST_LNOT),yyvsp[-1]);
        }
#line 1719 "y.tab.c"
        break;

    case 31: /* expr3: BNOT expr3  */
#line 123 "dbgExp.y"
        {
            yyval=SOT(CreateUnop(yyvsp[0],AST_BNOT),yyvsp[-1]);
        }
#line 1727 "y.tab.c"
        break;

    case 33: /* expr4: expr4 POW expr4  */
#line 129 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_POW),yyvsp[-1]);
        }
#line 1735 "y.tab.c"
        break;

    case 34: /* expr4: expr4 SHL expr4  */
#line 132 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SHL),yyvsp[-1]);
        }
#line 1743 "y.tab.c"
        break;

    case 35: /* expr4: expr4 SHR expr4  */
#line 135 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SHR),yyvsp[-1]);
        }
#line 1751 "y.tab.c"
        break;

    case 37: /* expr4_5: expr4_5 MUL expr4_5  */
#line 140 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_MUL),yyvsp[-1]);
        }
#line 1759 "y.tab.c"
        break;

    case 38: /* expr4_5: expr4_5 DIV expr4_5  */
#line 143 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_DIV),yyvsp[-1]);
        }
#line 1767 "y.tab.c"
        break;

    case 39: /* expr4_5: expr4_5 MOD expr4_5  */
#line 146 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_MOD),yyvsp[-1]);
        }
#line 1775 "y.tab.c"
        break;

    case 41: /* expr5: expr5 BAND expr5  */
#line 151 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BAND),yyvsp[-1]);
        }
#line 1783 "y.tab.c"
        break;

    case 43: /* expr6: expr6 BXOR expr6  */
#line 156 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BXOR),yyvsp[-1]);
        }
#line 1791 "y.tab.c"
        break;

    case 45: /* expr7: expr7 BOR expr7  */
#line 161 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_BOR),yyvsp[-1]);
        }
#line 1799 "y.tab.c"
        break;

    case 47: /* expr8: expr8 ADD expr8  */
#line 166 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ADD),yyvsp[-1]);
        }
#line 1807 "y.tab.c"
        break;

    case 48: /* expr8: expr8 SUB expr8  */
#line 169 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_SUB),yyvsp[-1]);
        }
#line 1815 "y.tab.c"
        break;

    case 50: /* expr9: expr9 LT expr9  */
#line 174 "dbgExp.y"
        {
            yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_LT),yyvsp[-1]);
        }
#line 1823 "y.tab.c"
        break;

    case 51: /* expr9: expr9 GT expr9  */
#line 177 "dbgExp.y"
        {
            yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_GT),yyvsp[-1]);
        }
#line 1831 "y.tab.c"
        break;

    case 52: /* expr9: expr9 LE expr9  */
#line 180 "dbgExp.y"
        {
            yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_LE),yyvsp[-1]);
        }
#line 1839 "y.tab.c"
        break;

    case 53: /* expr9: expr9 GE expr9  */
#line 183 "dbgExp.y"
        {
            yyval=SOT(AppendToRange(yyvsp[-2],yyvsp[0],AST_GE),yyvsp[-1]);
        }
#line 1847 "y.tab.c"
        break;

    case 55: /* expr10: expr10 EQ expr10  */
#line 188 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_EQ),yyvsp[-1]);
        }
#line 1855 "y.tab.c"
        break;

    case 56: /* expr10: expr10 NE expr10  */
#line 191 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_NE),yyvsp[-1]);
        }
#line 1863 "y.tab.c"
        break;

    case 58: /* expr11: expr11 LAND expr11  */
#line 196 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LAND),yyvsp[-1]);
        }
#line 1871 "y.tab.c"
        break;

    case 60: /* expr12: expr12 LXOR expr12  */
#line 201 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LXOR),yyvsp[-1]);
        }
#line 1879 "y.tab.c"
        break;

    case 62: /* expr13: expr13 LOR expr13  */
#line 206 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_LOR),yyvsp[-1]);
        }
#line 1887 "y.tab.c"
        break;

    case 64: /* expr14: expr14 ASSIGN expr14  */
#line 211 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN),yyvsp[-1]);
        }
#line 1895 "y.tab.c"
        break;

    case 65: /* expr14: expr14 EQ_SHL expr14  */
#line 214 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SHL),yyvsp[-1]);
        }
#line 1903 "y.tab.c"
        break;

    case 66: /* expr14: expr14 EQ_SHR expr14  */
#line 217 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SHR),yyvsp[-1]);
        }
#line 1911 "y.tab.c"
        break;

    case 67: /* expr14: expr14 EQ_MUL expr14  */
#line 220 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_MUL),yyvsp[-1]);
        }
#line 1919 "y.tab.c"
        break;

    case 68: /* expr14: expr14 EQ_DIV expr14  */
#line 223 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_DIV),yyvsp[-1]);
        }
#line 1927 "y.tab.c"
        break;

    case 69: /* expr14: expr14 EQ_MOD expr14  */
#line 226 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_MOD),yyvsp[-1]);
        }
#line 1935 "y.tab.c"
        break;

    case 70: /* expr14: expr14 EQ_BAND expr14  */
#line 229 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BAND),yyvsp[-1]);
        }
#line 1943 "y.tab.c"
        break;

    case 71: /* expr14: expr14 EQ_BXOR expr14  */
#line 232 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BXOR),yyvsp[-1]);
        }
#line 1951 "y.tab.c"
        break;

    case 72: /* expr14: expr14 EQ_BOR expr14  */
#line 235 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_BOR),yyvsp[-1]);
        }
#line 1959 "y.tab.c"
        break;

    case 73: /* expr14: expr14 EQ_ADD expr14  */
#line 238 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_ADD),yyvsp[-1]);
        }
#line 1967 "y.tab.c"
        break;

    case 74: /* expr14: expr14 EQ_SUB expr14  */
#line 241 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_ASSIGN_SUB),yyvsp[-1]);
        }
#line 1975 "y.tab.c"
        break;

    case 76: /* expr_comma: expr_comma COMMA expr  */
#line 247 "dbgExp.y"
        {
            yyval=SOT(CreateBinop(yyvsp[-2],yyvsp[0],AST_COMMA),yyvsp[-1]);
        }
#line 1983 "y.tab.c"
        break;


#line 1987 "y.tab.c"

    default:
        break;
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
    yytoken = yychar == DBGREMPTY ? D_YYEMPTY : YYTRANSLATE (yychar);
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

        if (yychar <= DBGREOF)
        {
            /* Return failure if at end of input.  */
            if (yychar == DBGREOF)
                YYABORT;
        }
        else
        {
            yydestruct ("Error: discarding",
                        yytoken, &yylval);
            yychar = DBGREMPTY;
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
            yyn += D_YYerror;
            if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == D_YYerror)
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
    if (yychar != DBGREMPTY)
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

