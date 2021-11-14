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
    HC_TRY = 299,                  /* TRY  */
    HC_CATCH = 300,                /* CATCH  */
    HC_LASTCLASS = 301,            /* LASTCLASS  */
    HC_U0 = 302,                   /* U0  */
    HC_LEFT_PAREN = 303,           /* LEFT_PAREN  */
    HC_RIGHT_PAREN = 304,          /* RIGHT_PAREN  */
    HC_INC = 305,                  /* INC  */
    HC_DEC = 306,                  /* DEC  */
    HC_NAME = 307,                 /* NAME  */
    HC_LEFT_SQAURE = 308,          /* LEFT_SQAURE  */
    HC_RIGHT_SQAURE = 309,         /* RIGHT_SQAURE  */
    HC_SEMI = 310,                 /* SEMI  */
    HC_IF = 311,                   /* IF  */
    HC_ELSE = 312,                 /* ELSE  */
    HC_DO = 313,                   /* DO  */
    HC_WHILE = 314,                /* WHILE  */
    HC_FOR = 315,                  /* FOR  */
    HC_LEFT_CURLY = 316,           /* LEFT_CURLY  */
    HC_RIGHT_CURLY = 317,          /* RIGHT_CURLY  */
    HC_CASE = 318,                 /* CASE  */
    HC_COLON = 319,                /* COLON  */
    HC_DOT_DOT_DOT = 320,          /* DOT_DOT_DOT  */
    HC_EXTERN2 = 321,              /* EXTERN2  */
    HC_LOCK = 322,                 /* LOCK  */
    HC_EXTERN = 323,               /* EXTERN  */
    HC_IMPORT = 324,               /* IMPORT  */
    HC_IMPORT2 = 325,              /* IMPORT2  */
    HC_ASM_IMPORT = 326,           /* ASM_IMPORT  */
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
    HC_SWITCH = 338,               /* SWITCH  */
    HC_EVAL_NOCOMMA = 339,         /* EVAL_NOCOMMA  */
    HC_EXE = 340,                  /* EXE  */
    HC_EVAL = 341,                 /* EVAL  */
    HC_NL = 342,                   /* NL  */
    HC_DBG = 343,                  /* DBG  */
    HC_STRING = 344,               /* STRING  */
    HC_TYPENAME = 345,             /* TYPENAME  */
    HC_OPCODE = 346,               /* OPCODE  */
    HC_REGISTER = 347,             /* REGISTER  */
    HC_DOUBLE_COLON = 348,         /* DOUBLE_COLON  */
    HC_DOUBLE_AT = 349,            /* DOUBLE_AT  */
    HC_DU8 = 350,                  /* DU8  */
    HC_DU16 = 351,                 /* DU16  */
    HC_DU32 = 352,                 /* DU32  */
    HC_DU64 = 353,                 /* DU64  */
    HC_ALIGN = 354,                /* ALIGN  */
    HC_BINFILE = 355,              /* BINFILE  */
    HC_ASM = 356                   /* ASM  */
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
