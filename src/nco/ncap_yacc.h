
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     OUT_ATT = 258,
     COMPARISON = 259,
     LHS_SBS = 260,
     SCV = 261,
     SNG = 262,
     FUNCTION = 263,
     OUT_VAR = 264,
     VAR = 265,
     CNV_TYPE = 266,
     ABS = 267,
     ATOSTR = 268,
     EPROVOKE = 269,
     IGNORE = 270,
     NAMED_CONSTANT = 271,
     PACK = 272,
     POWER = 273,
     RDC = 274,
     UNPACK = 275,
     IF = 276,
     PRINT = 277,
     OR = 278,
     NOT = 279,
     AND = 280,
     UMINUS = 281,
     LOWER_THAN_ELSE = 282,
     ELSE = 283
   };
#endif
/* Tokens.  */
#define OUT_ATT 258
#define COMPARISON 259
#define LHS_SBS 260
#define SCV 261
#define SNG 262
#define FUNCTION 263
#define OUT_VAR 264
#define VAR 265
#define CNV_TYPE 266
#define ABS 267
#define ATOSTR 268
#define EPROVOKE 269
#define IGNORE 270
#define NAMED_CONSTANT 271
#define PACK 272
#define POWER 273
#define RDC 274
#define UNPACK 275
#define IF 276
#define PRINT 277
#define OR 278
#define NOT 279
#define AND 280
#define UMINUS 281
#define LOWER_THAN_ELSE 282
#define ELSE 283




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 108 "ncap_yacc.y"

  char *sng; /* [sng] String value */
  char *var_nm_LHS; /* [sng] Variables on LHS */
  char *var_nm_RHS; /* [sng] Variables on RHS */
  aed_sct aed; /* [sct] Attribute */
  sym_sct *sym; /* [sct] Intrinsic function name */
  scv_sct scv; /* [sct] Scalar value */
  var_sct *var; /* [sct] Variable */
  nm_lst_sct *sbs_lst; /* [sct] Subscript list */
  int nco_rlt_opr; /* [enm] Comparison operator type */
  nc_type cnv_type;  /* [enm] Used for type conversion functions */



/* Line 1676 of yacc.c  */
#line 123 "ncap_yacc.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




