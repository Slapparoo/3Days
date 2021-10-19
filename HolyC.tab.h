/* A Bison parser, made by GNU Bison 3.7.6.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

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

/* Token kinds.  */
#ifndef HC_TOKENTYPE
# define HC_TOKENTYPE
  enum HC_tokentype
  {
    HC_HC_EMPTY = -2,
    HC_YYEOF = 0,                  /* "end of file"  */
    HC_HC_error = 256,             /* error  */
    HC_HC_UNDEF = 257,             /* "invalid token"  */
    HC_CHAR = 258,                 /* CHAR  */
    HC_INT = 259,                  /* INT  */
    HC_FLOAT = 260,                /* FLOAT  */
    HC_DOT = 261,                  /* DOT  */
    HC_ARROW = 262,                /* ARROW  */
    HC_SIZEOF = 263,               /* SIZEOF  */
    HC_ADDR_OF = 264,              /* ADDR_OF  */
    HC_LNOT = 265,                 /* LNOT  */
    HC_BNOT = 266,                 /* BNOT  */
    HC_POW = 267,                  /* POW  */
    HC_SHL = 268,                  /* SHL  */
    HC_SHR = 269,                  /* SHR  */
    HC_MUL = 270,                  /* MUL  */
    HC_DIV = 271,                  /* DIV  */
    HC_MOD = 272,                  /* MOD  */
    HC_BAND = 273,                 /* BAND  */
    HC_BOR = 274,                  /* BOR  */
    HC_BXOR = 275,                 /* BXOR  */
    HC_ADD = 276,                  /* ADD  */
    HC_SUB = 277,                  /* SUB  */
    HC_LT = 278,                   /* LT  */
    HC_GT = 279,                   /* GT  */
    HC_LE = 280,                   /* LE  */
    HC_GE = 281,                   /* GE  */
    HC_NE = 282,                   /* NE  */
    HC_EQ = 283,                   /* EQ  */
    HC_LAND = 284,                 /* LAND  */
    HC_LXOR = 285,                 /* LXOR  */
    HC_LOR = 286,                  /* LOR  */
    HC_ASSIGN = 287,               /* ASSIGN  */
    HC_EQ_SHL = 288,               /* EQ_SHL  */
    HC_EQ_SHR = 289,               /* EQ_SHR  */
    HC_EQ_MUL = 290,               /* EQ_MUL  */
    HC_EQ_DIV = 291,               /* EQ_DIV  */
    HC_EQ_MOD = 292,               /* EQ_MOD  */
    HC_EQ_BAND = 293,              /* EQ_BAND  */
    HC_EQ_BXOR = 294,              /* EQ_BXOR  */
    HC_EQ_BOR = 295,               /* EQ_BOR  */
    HC_EQ_ADD = 296,               /* EQ_ADD  */
    HC_EQ_SUB = 297,               /* EQ_SUB  */
    HC_COMMA = 298,                /* COMMA  */
    HC_OTHER = 299,                /* OTHER  */
    HC_TRY = 300,                  /* TRY  */
    HC_CATCH = 301,                /* CATCH  */
    HC_LASTCLASS = 302,            /* LASTCLASS  */
    HC_U0 = 303,                   /* U0  */
    HC_LEFT_PAREN = 304,           /* LEFT_PAREN  */
    HC_RIGHT_PAREN = 305,          /* RIGHT_PAREN  */
    HC_INC = 306,                  /* INC  */
    HC_DEC = 307,                  /* DEC  */
    HC_NAME = 308,                 /* NAME  */
    HC_LEFT_SQAURE = 309,          /* LEFT_SQAURE  */
    HC_RIGHT_SQAURE = 310,         /* RIGHT_SQAURE  */
    HC_SEMI = 311,                 /* SEMI  */
    HC_IF = 312,                   /* IF  */
    HC_ELSE = 313,                 /* ELSE  */
    HC_DO = 314,                   /* DO  */
    HC_WHILE = 315,                /* WHILE  */
    HC_FOR = 316,                  /* FOR  */
    HC_LEFT_CURLY = 317,           /* LEFT_CURLY  */
    HC_RIGHT_CURLY = 318,          /* RIGHT_CURLY  */
    HC_CASE = 319,                 /* CASE  */
    HC_COLON = 320,                /* COLON  */
    HC_DOT_DOT_DOT = 321,          /* DOT_DOT_DOT  */
    HC_EXTERN2 = 322,              /* EXTERN2  */
    HC_LOCK = 323,                 /* LOCK  */
    HC_EXTERN = 324,               /* EXTERN  */
    HC_IMPORT = 325,               /* IMPORT  */
    HC_IMPORT2 = 326,              /* IMPORT2  */
    HC_STATIC = 327,               /* STATIC  */
    HC_PUBLIC = 328,               /* PUBLIC  */
    HC_CLASS = 329,                /* CLASS  */
    HC_UNION = 330,                /* UNION  */
    HC_INTERN = 331,               /* INTERN  */
    HC_START = 332,                /* START  */
    HC_END = 333,                  /* END  */
    HC_DEFAULT = 334,              /* DEFAULT  */
    HC_BREAK = 335,                /* BREAK  */
    HC_RET = 336,                  /* RET  */
    HC_GOTO = 337,                 /* GOTO  */
    HC_BOOL = 338,                 /* BOOL  */
    HC_U8 = 339,                   /* U8  */
    HC_I8 = 340,                   /* I8  */
    HC_U16 = 341,                  /* U16  */
    HC_I16 = 342,                  /* I16  */
    HC_U32 = 343,                  /* U32  */
    HC_I32 = 344,                  /* I32  */
    HC_U64 = 345,                  /* U64  */
    HC_I64 = 346,                  /* I64  */
    HC_F64 = 347,                  /* F64  */
    HC_SWITCH = 348,               /* SWITCH  */
    HC_EXE = 349,                  /* EXE  */
    HC_EVAL = 350,                 /* EVAL  */
    HC_NL = 351,                   /* NL  */
    HC_DBG = 352,                  /* DBG  */
    HC_STRING = 353,               /* STRING  */
    HC_TYPENAME = 354              /* TYPENAME  */
  };
  typedef enum HC_tokentype HC_token_kind_t;
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
