/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

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
    HC_EXE = 339,
    HC_EVAL = 340,
    HC_NL = 341,
    HC_DBG = 342,
    HC_STRING = 343,
    HC_TYPENAME = 344,
    HC_OPCODE = 345,
    HC_REGISTER = 346,
    HC_DOUBLE_COLON = 347,
    HC_DOUBLE_AT = 348,
    HC_DU8 = 349,
    HC_DU16 = 350,
    HC_DU32 = 351,
    HC_DU64 = 352,
    HC_ALIGN = 353,
    HC_BINFILE = 354,
    HC_ASM = 355
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
