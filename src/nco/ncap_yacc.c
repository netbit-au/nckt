
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         nco_yyparse
#define yylex           nco_yylex
#define yyerror         nco_yyerror
#define yylval          nco_yylval
#define yychar          nco_yychar
#define yydebug         nco_yydebug
#define yynerrs         nco_yynerrs


/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "ncap_yacc.y"
 /* $Header: /cvsroot/nco/nco/src/nco/ncap_yacc.y,v 1.56 2010/04/09 05:04:39 zender Exp $ -*-C-*- */
  
/* Begin C declarations section */
  
/* Purpose: Grammar parser for ncap */
  
/* Copyright (C) 1995--2010 Charlie Zender
     
   You may copy, distribute, and/or modify this software under the terms of the GNU General Public License (GPL) Version 2
   The full license text is at http://www.gnu.org/copyleft/gpl.html 
   and in the file nco/doc/LICENSE in the NCO source distribution.
   
   As a special exception to the terms of the GPL, you are permitted 
   to link the NCO source code with the HDF, netCDF, OPeNDAP, and UDUnits
   libraries and to distribute the resulting executables under the terms 
   of the GPL, but in addition obeying the extra stipulations of the 
   HDF, netCDF, OPeNDAP, and UDUnits licenses.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.
   
   The original author of this software, Charlie Zender, seeks to improve
   it with your suggestions, contributions, bug-reports, and patches.
   Please contact the NCO project at http://nco.sf.net or write to
   Charlie Zender
   Department of Earth System Science
   University of California, Irvine
   Irvine, CA 92697-3100 */

/* Usage:
   bison --output=${HOME}/nco/src/nco/ncap_yacc.c -d ~/nco/src/nco/ncap_yacc.y */

/* Example yacc text:
   Nie02 "A Compact Guide to Lex & Yacc" by Thomas Niemann, ePaper Press, URL:http://epaperpress.com/lexandyacc/index.html
   LMB92 ${DATA}/ora/lexyacc/ch3-05.y
   GCC c-parse.y
   GCC parser_build_binary_op() c-typeck.c
   Unidata ncgen.y */

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdarg.h> /* va_start, va_arg, va_end */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp. . . */
#include <assert.h>
/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "ncap.h" /* netCDF arithmetic processor-specific definitions (symbol table, ...) */

/* Bison adds routines which reference YY*LEX token to ncap_yacc.c 
   These routines generate warnings unless YY*LEX prototype appears above
   YYLEX prototype generates error unless YYSTYPE token is defined
   Thus must #include ncap_yacc.h solely to pre-define YY*STYPE for YY*LEX prototype
   There is no other reason for ncap_yacc.h to appear in ncap_yacc.y 
   Yes, this is rather circular */
/* Get YYSTYPE prior to prototyping scanner */
#include "ncap_yacc.h" /* ncap_yacc.h (ncap.tab.h) is produced from ncap_yacc.y by parser generator */

  #define YY_DECL int yylex(YYSTYPE *lval_ptr,prs_sct *prs_arg)
  YY_DECL;

/* Turn on parser debugging option (Bison manual p. 85) */
#define YYDEBUG 0
int yydebug=0; /* 0: Normal operation. 1: Print parser rules during execution */

/* Turns on more verbose errors than just plain "parse error" when yyerror() is called by parser */
#define YYERROR_VERBOSE 1
/* Bison manual p. 60 describes how to call yyparse() with arguments 
   Following two statements superceded 20051213 by parse-param and lex-param below */
/* #define YYPARSE_PARAM prs_arg */
/* #define YYLEX_PARAM prs_arg */

int rcd; /* [enm] Return value for function calls */

/* Global variables */
extern size_t ncap_ncl_dpt_crr; /* [nbr] Depth of current #include file (declared in ncap.c) */
extern size_t *ncap_ln_nbr_crr; /* [cnt] Line number (declared in ncap.c) */
extern char **ncap_fl_spt_glb; /* [fl] Script file (declared in ncap.c) */
extern char ncap_err_sng[200]; /* [sng] Buffer for error string (declared in ncap_lex.l) */

/* End C declarations section */


/* Line 189 of yacc.c  */
#line 172 "ncap_yacc.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


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

/* Line 214 of yacc.c  */
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



/* Line 214 of yacc.c  */
#line 279 "ncap_yacc.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 291 "ncap_yacc.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  13
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   519

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  40
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  12
/* YYNRULES -- Number of rules.  */
#define YYNRULES  74
/* YYNRULES -- Number of states.  */
#define YYNSTATES  190

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   283

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    30,     2,     2,
      36,    37,    28,    26,    39,    27,     2,    29,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    35,
       2,    38,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    31,     2,     2,     2,     2,     2,
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
      25,    32,    33,    34
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,    10,    14,    18,    21,    24,
      30,    31,    32,    52,    56,    60,    64,    68,    72,    76,
      80,    84,    88,    92,    96,    99,   102,   106,   113,   118,
     123,   128,   132,   134,   136,   138,   142,   147,   154,   156,
     160,   164,   168,   172,   176,   180,   184,   188,   192,   196,
     200,   204,   208,   212,   216,   220,   224,   228,   232,   236,
     240,   244,   251,   258,   265,   268,   271,   276,   281,   286,
     291,   296,   300,   305,   307
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      41,     0,    -1,    42,    -1,    35,    -1,    42,    35,    -1,
      42,    43,    35,    -1,    42,     1,    35,    -1,    43,    35,
      -1,     1,    35,    -1,    21,    36,    50,    37,    43,    -1,
      -1,    -1,    21,    36,    50,    37,    43,    34,    43,    44,
      22,    36,    46,    37,    35,    45,    22,    36,    51,    37,
      35,    -1,    48,    38,    46,    -1,    48,    38,    49,    -1,
      48,    38,    51,    -1,    47,    38,    51,    -1,    47,    38,
      46,    -1,    47,    38,    49,    -1,    46,    26,    46,    -1,
      46,    27,    46,    -1,    46,    28,    46,    -1,    46,    29,
      46,    -1,    46,    30,    46,    -1,    27,    46,    -1,    26,
      46,    -1,    46,    31,    46,    -1,    18,    36,    46,    39,
      46,    37,    -1,    12,    36,    46,    37,    -1,     8,    36,
      46,    37,    -1,    11,    36,    46,    37,    -1,    36,    46,
      37,    -1,     6,    -1,     9,    -1,     3,    -1,    49,    26,
      49,    -1,    13,    36,    46,    37,    -1,    13,    36,    46,
      39,    49,    37,    -1,     7,    -1,    51,     4,    51,    -1,
      46,     4,    46,    -1,    51,     4,    46,    -1,    46,     4,
      51,    -1,    51,    26,    51,    -1,    51,    26,    46,    -1,
      46,    26,    51,    -1,    51,    27,    51,    -1,    51,    27,
      46,    -1,    46,    27,    51,    -1,    51,    28,    51,    -1,
      51,    28,    46,    -1,    46,    28,    51,    -1,    51,    29,
      51,    -1,    51,    29,    46,    -1,    46,    29,    51,    -1,
      51,    30,    51,    -1,    51,    30,    46,    -1,    46,    30,
      51,    -1,    51,    31,    51,    -1,    51,    31,    46,    -1,
      46,    31,    51,    -1,    18,    36,    51,    39,    51,    37,
      -1,    18,    36,    51,    39,    46,    37,    -1,    18,    36,
      46,    39,    51,    37,    -1,    27,    51,    -1,    26,    51,
      -1,    12,    36,    51,    37,    -1,    19,    36,    51,    37,
      -1,    17,    36,    51,    37,    -1,    20,    36,    51,    37,
      -1,     8,    36,    51,    37,    -1,    36,    51,    37,    -1,
      11,    36,    51,    37,    -1,    16,    -1,    10,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   166,   166,   170,   171,   172,   177,   178,   179,   188,
     192,   196,   192,   202,   238,   257,   284,   296,   336,   362,
     366,   370,   374,   378,   383,   387,   390,   403,   416,   419,
     429,   433,   434,   437,   440,   444,   453,   475,   497,   501,
     503,   505,   507,   513,   516,   519,   523,   526,   529,   539,
     542,   545,   549,   555,   558,   562,   565,   568,   572,   575,
     578,   582,   586,   589,   593,   599,   602,   605,   613,   626,
     636,   639,   642,   645,   648
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "OUT_ATT", "COMPARISON", "LHS_SBS",
  "SCV", "SNG", "FUNCTION", "OUT_VAR", "VAR", "CNV_TYPE", "ABS", "ATOSTR",
  "EPROVOKE", "IGNORE", "NAMED_CONSTANT", "PACK", "POWER", "RDC", "UNPACK",
  "IF", "PRINT", "OR", "NOT", "AND", "'+'", "'-'", "'*'", "'/'", "'%'",
  "'^'", "UMINUS", "LOWER_THAN_ELSE", "ELSE", "';'", "'('", "')'", "'='",
  "','", "$accept", "program", "stmt_lst", "stmt", "$@1", "$@2", "scv_xpr",
  "out_var_xpr", "out_att_xpr", "sng_xpr", "bln_xpr", "var_xpr", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,    43,    45,    42,    47,
      37,    94,   281,   282,   283,    59,    40,    41,    61,    44
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    40,    41,    42,    42,    42,    42,    42,    42,    43,
      44,    45,    43,    43,    43,    43,    43,    43,    43,    46,
      46,    46,    46,    46,    46,    46,    46,    46,    46,    46,
      46,    46,    46,    47,    48,    49,    49,    49,    49,    50,
      50,    50,    50,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    51
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     2,     3,     3,     2,     2,     5,
       0,     0,    19,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     2,     3,     6,     4,     4,
       4,     3,     1,     1,     1,     3,     4,     6,     1,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     6,     6,     6,     2,     2,     4,     4,     4,     4,
       4,     3,     4,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,    34,    33,     0,     3,     0,     0,     0,     0,
       0,     8,     0,     1,     0,     4,     0,     7,     0,     0,
      32,     0,    74,     0,     0,    73,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     6,     5,    38,     0,
      17,    18,    16,    13,    14,    15,     0,     0,     0,     0,
       0,     0,     0,    25,    65,    24,    64,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    31,    71,
      40,    42,    19,    45,    20,    48,    21,    51,    22,    54,
      23,    57,    26,    60,     9,    41,    39,    44,    43,    47,
      46,    50,    49,    53,    52,    56,    55,    59,    58,     0,
       0,     0,     0,     0,     0,     0,     0,    35,    29,    70,
      30,    72,    28,    66,    68,     0,     0,    67,    69,     0,
       0,     0,     0,     0,    25,    24,     0,     0,     0,     0,
       0,     0,     0,    36,     0,     0,     0,     0,     0,    10,
       0,     0,     0,     0,    19,    20,    21,    22,    23,    26,
       0,    27,    63,    62,    61,     0,     0,    37,     0,     0,
       0,     0,     0,    11,     0,     0,     0,     0,     0,    12
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     6,     7,     8,   175,   184,    82,     9,    10,    41,
      34,    35
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -35
static const yytype_int16 yypact[] =
{
      64,   -34,   -35,   -35,   -27,   -35,    14,     2,   -10,   -16,
      -6,   -35,   140,   -35,    10,   -35,    11,   -35,    70,    70,
     -35,    18,   -35,    27,    30,   -35,    32,    33,    34,    36,
     140,   140,   140,     0,    37,    13,   -35,   -35,   -35,    39,
      74,    53,   488,    74,    53,   488,   140,   140,   140,   140,
     140,   140,   140,   -35,   -35,   -35,   -35,   223,   235,   140,
     140,   140,   140,   140,   140,   140,    12,   140,   140,   140,
     140,   140,   140,   140,   191,     3,   247,   259,   271,   284,
     296,   308,    74,   320,   184,   202,   332,   344,   -35,   -35,
      74,   488,    63,    79,    63,    79,    67,    89,    67,    89,
      67,    89,    67,    89,    50,    74,   488,    63,    79,    63,
      79,    67,    89,    67,    89,    67,    89,    67,    89,    59,
      85,    86,    90,   191,   191,   191,    88,   -35,   -35,   -35,
     -35,   -35,   -35,   -35,   -35,   140,   140,   -35,   -35,    12,
     191,   191,   191,   191,   -35,   -35,   356,   191,   191,   191,
     191,   191,   191,   -35,     3,   368,   380,   392,   404,   -35,
     416,   428,   440,   208,   149,   149,   101,   101,   101,   101,
     -13,   -35,   -35,   -35,   -35,   111,   191,   -35,    98,   452,
     191,   464,   110,   -35,   125,   113,   140,   476,   126,   -35
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -35,   -35,   -35,     5,   -35,   -35,   -12,   -35,   -35,   -11,
     -35,   123
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -3
static const yytype_int16 yytable[] =
{
      33,    11,    -2,    14,    59,     2,    40,    43,    44,    12,
      38,     3,    16,    75,    13,     2,    39,    67,    53,    55,
      57,     3,    18,     4,   177,    17,    60,    61,    62,    63,
      64,    65,    19,     4,    76,    78,    80,    15,    84,    68,
      69,    70,    71,    72,    73,    36,    37,    90,    92,    94,
      96,    98,   100,   102,    46,   105,   107,   109,   111,   113,
     115,   117,   126,    47,   127,     1,    48,     2,    49,    50,
      51,   104,    52,     3,    66,    74,    20,    38,    21,    75,
      22,    23,    24,    39,   139,     4,    25,    26,    27,    28,
      29,    62,    63,    64,    65,   140,    30,    31,    65,     5,
      60,    61,    62,    63,    64,    65,    32,    70,    71,    72,
      73,   144,   145,   146,   147,   148,   149,   150,   151,   152,
      73,   141,   142,   155,   157,   153,   143,   154,   160,   161,
     162,   163,   152,   178,   180,   164,   165,   166,   167,   168,
     169,    42,    45,   170,   159,   183,    20,   185,    21,   186,
      22,    23,    24,    54,    56,    58,    25,    26,    27,    28,
      29,   189,     0,     0,   179,     0,    30,    31,   181,    77,
      79,    81,    83,    85,    86,    87,    32,   149,   150,   151,
     152,     0,    91,    93,    95,    97,    99,   101,   103,     0,
     106,   108,   110,   112,   114,   116,   118,    20,     0,   119,
       0,     0,   120,   121,     0,     0,     0,     0,     0,   122,
      60,    61,    62,    63,    64,    65,     0,   123,   124,     0,
       0,     0,     0,   135,     0,     0,     0,   125,    68,    69,
      70,    71,    72,    73,   147,   148,   149,   150,   151,   152,
       0,   136,     0,     0,     0,     0,     0,   176,     0,    60,
      61,    62,    63,    64,    65,     0,     0,     0,   156,   158,
      88,    68,    69,    70,    71,    72,    73,     0,     0,     0,
       0,     0,    89,    60,    61,    62,    63,    64,    65,     0,
       0,     0,     0,     0,   128,    68,    69,    70,    71,    72,
      73,     0,     0,     0,     0,     0,   129,    60,    61,    62,
      63,    64,    65,     0,     0,     0,     0,     0,   130,   187,
      68,    69,    70,    71,    72,    73,     0,     0,     0,     0,
       0,   131,    60,    61,    62,    63,    64,    65,     0,     0,
       0,     0,     0,   132,    68,    69,    70,    71,    72,    73,
       0,     0,     0,     0,     0,   133,    68,    69,    70,    71,
      72,    73,     0,     0,     0,     0,     0,   134,    68,    69,
      70,    71,    72,    73,     0,     0,     0,     0,     0,   137,
      68,    69,    70,    71,    72,    73,     0,     0,     0,     0,
       0,   138,   147,   148,   149,   150,   151,   152,     0,     0,
       0,     0,     0,    88,    60,    61,    62,    63,    64,    65,
       0,     0,     0,     0,     0,   171,    68,    69,    70,    71,
      72,    73,     0,     0,     0,     0,     0,   172,    60,    61,
      62,    63,    64,    65,     0,     0,     0,     0,     0,   173,
      68,    69,    70,    71,    72,    73,     0,     0,     0,     0,
       0,   174,   147,   148,   149,   150,   151,   152,     0,     0,
       0,     0,     0,   128,   147,   148,   149,   150,   151,   152,
       0,     0,     0,     0,     0,   130,   147,   148,   149,   150,
     151,   152,     0,     0,     0,     0,     0,   132,   147,   148,
     149,   150,   151,   152,     0,     0,     0,     0,     0,   171,
     147,   148,   149,   150,   151,   152,     0,     0,     0,     0,
       0,   182,    68,    69,    70,    71,    72,    73,     0,     0,
       0,     0,     0,   188,    68,    69,    70,    71,    72,    73
};

static const yytype_int16 yycheck[] =
{
      12,    35,     0,     1,     4,     3,    18,    19,    19,    36,
       7,     9,     7,    26,     0,     3,    13,     4,    30,    31,
      32,     9,    38,    21,    37,    35,    26,    27,    28,    29,
      30,    31,    38,    21,    46,    47,    48,    35,    50,    26,
      27,    28,    29,    30,    31,    35,    35,    59,    60,    61,
      62,    63,    64,    65,    36,    67,    68,    69,    70,    71,
      72,    73,    74,    36,    75,     1,    36,     3,    36,    36,
      36,    66,    36,     9,    37,    36,     6,     7,     8,    26,
      10,    11,    12,    13,    34,    21,    16,    17,    18,    19,
      20,    28,    29,    30,    31,    36,    26,    27,    31,    35,
      26,    27,    28,    29,    30,    31,    36,    28,    29,    30,
      31,   123,   124,   125,    26,    27,    28,    29,    30,    31,
      31,    36,    36,   135,   136,    37,    36,    39,   140,   141,
     142,   143,    31,    22,    36,   147,   148,   149,   150,   151,
     152,    18,    19,   154,   139,    35,     6,    22,     8,    36,
      10,    11,    12,    30,    31,    32,    16,    17,    18,    19,
      20,    35,    -1,    -1,   176,    -1,    26,    27,   180,    46,
      47,    48,    49,    50,    51,    52,    36,    28,    29,    30,
      31,    -1,    59,    60,    61,    62,    63,    64,    65,    -1,
      67,    68,    69,    70,    71,    72,    73,     6,    -1,     8,
      -1,    -1,    11,    12,    -1,    -1,    -1,    -1,    -1,    18,
      26,    27,    28,    29,    30,    31,    -1,    26,    27,    -1,
      -1,    -1,    -1,    39,    -1,    -1,    -1,    36,    26,    27,
      28,    29,    30,    31,    26,    27,    28,    29,    30,    31,
      -1,    39,    -1,    -1,    -1,    -1,    -1,    39,    -1,    26,
      27,    28,    29,    30,    31,    -1,    -1,    -1,   135,   136,
      37,    26,    27,    28,    29,    30,    31,    -1,    -1,    -1,
      -1,    -1,    37,    26,    27,    28,    29,    30,    31,    -1,
      -1,    -1,    -1,    -1,    37,    26,    27,    28,    29,    30,
      31,    -1,    -1,    -1,    -1,    -1,    37,    26,    27,    28,
      29,    30,    31,    -1,    -1,    -1,    -1,    -1,    37,   186,
      26,    27,    28,    29,    30,    31,    -1,    -1,    -1,    -1,
      -1,    37,    26,    27,    28,    29,    30,    31,    -1,    -1,
      -1,    -1,    -1,    37,    26,    27,    28,    29,    30,    31,
      -1,    -1,    -1,    -1,    -1,    37,    26,    27,    28,    29,
      30,    31,    -1,    -1,    -1,    -1,    -1,    37,    26,    27,
      28,    29,    30,    31,    -1,    -1,    -1,    -1,    -1,    37,
      26,    27,    28,    29,    30,    31,    -1,    -1,    -1,    -1,
      -1,    37,    26,    27,    28,    29,    30,    31,    -1,    -1,
      -1,    -1,    -1,    37,    26,    27,    28,    29,    30,    31,
      -1,    -1,    -1,    -1,    -1,    37,    26,    27,    28,    29,
      30,    31,    -1,    -1,    -1,    -1,    -1,    37,    26,    27,
      28,    29,    30,    31,    -1,    -1,    -1,    -1,    -1,    37,
      26,    27,    28,    29,    30,    31,    -1,    -1,    -1,    -1,
      -1,    37,    26,    27,    28,    29,    30,    31,    -1,    -1,
      -1,    -1,    -1,    37,    26,    27,    28,    29,    30,    31,
      -1,    -1,    -1,    -1,    -1,    37,    26,    27,    28,    29,
      30,    31,    -1,    -1,    -1,    -1,    -1,    37,    26,    27,
      28,    29,    30,    31,    -1,    -1,    -1,    -1,    -1,    37,
      26,    27,    28,    29,    30,    31,    -1,    -1,    -1,    -1,
      -1,    37,    26,    27,    28,    29,    30,    31,    -1,    -1,
      -1,    -1,    -1,    37,    26,    27,    28,    29,    30,    31
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     9,    21,    35,    41,    42,    43,    47,
      48,    35,    36,     0,     1,    35,    43,    35,    38,    38,
       6,     8,    10,    11,    12,    16,    17,    18,    19,    20,
      26,    27,    36,    46,    50,    51,    35,    35,     7,    13,
      46,    49,    51,    46,    49,    51,    36,    36,    36,    36,
      36,    36,    36,    46,    51,    46,    51,    46,    51,     4,
      26,    27,    28,    29,    30,    31,    37,     4,    26,    27,
      28,    29,    30,    31,    36,    26,    46,    51,    46,    51,
      46,    51,    46,    51,    46,    51,    51,    51,    37,    37,
      46,    51,    46,    51,    46,    51,    46,    51,    46,    51,
      46,    51,    46,    51,    43,    46,    51,    46,    51,    46,
      51,    46,    51,    46,    51,    46,    51,    46,    51,     8,
      11,    12,    18,    26,    27,    36,    46,    49,    37,    37,
      37,    37,    37,    37,    37,    39,    39,    37,    37,    34,
      36,    36,    36,    36,    46,    46,    46,    26,    27,    28,
      29,    30,    31,    37,    39,    46,    51,    46,    51,    43,
      46,    46,    46,    46,    46,    46,    46,    46,    46,    46,
      49,    37,    37,    37,    37,    44,    39,    37,    22,    46,
      36,    46,    37,    35,    45,    22,    36,    51,    37,    35
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (prs_arg, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, prs_arg)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, prs_arg); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, prs_sct *prs_arg)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, prs_arg)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    prs_sct *prs_arg;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (prs_arg);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, prs_sct *prs_arg)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, prs_arg)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    prs_sct *prs_arg;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, prs_arg);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule, prs_sct *prs_arg)
#else
static void
yy_reduce_print (yyvsp, yyrule, prs_arg)
    YYSTYPE *yyvsp;
    int yyrule;
    prs_sct *prs_arg;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       , prs_arg);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule, prs_arg); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
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
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
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
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
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
	    /* Fall through.  */
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

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, prs_sct *prs_arg)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, prs_arg)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    prs_sct *prs_arg;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (prs_arg);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (prs_sct *prs_arg);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */





/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (prs_sct *prs_arg)
#else
int
yyparse (prs_arg)
    prs_sct *prs_arg;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

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
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
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

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:

/* Line 1455 of yacc.c  */
#line 170 "ncap_yacc.y"
    { ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 171 "ncap_yacc.y"
    { ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 172 "ncap_yacc.y"
    {
  /* Purpose: Actions to be performed at end-of-statement go here */
  /* Clean up from and exit LHS_cst mode */
  (void)nco_var_free_wrp(&((prs_sct *)prs_arg)->var_LHS);
}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 177 "ncap_yacc.y"
    {(void)nco_var_free_wrp(&((prs_sct *)prs_arg)->var_LHS);}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 178 "ncap_yacc.y"
    {(void)nco_var_free_wrp(&((prs_sct *)prs_arg)->var_LHS);}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 179 "ncap_yacc.y"
    {(void)nco_var_free_wrp(&((prs_sct *)prs_arg)->var_LHS);}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 188 "ncap_yacc.y"
    {
  /* LMB92 p. 234 */
  ;
}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 192 "ncap_yacc.y"
    {
  /* LMB92 p. 234 */
  ;
}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 196 "ncap_yacc.y"
    {
  ;
}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 199 "ncap_yacc.y"
    {
  ;
}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 202 "ncap_yacc.y"
    { 
  aed_sct *ptr_aed;
  
  ptr_aed=ncap_aed_lookup((yyvsp[(1) - (3)].aed).var_nm,(yyvsp[(1) - (3)].aed).att_nm,((prs_sct *)prs_arg),True);
  
  ptr_aed->val=ncap_scv_2_ptr_unn((yyvsp[(3) - (3)].scv));
  ptr_aed->type=(yyvsp[(3) - (3)].scv).type;
  ptr_aed->sz=1L;
  (void)cast_nctype_void(ptr_aed->type,&ptr_aed->val);    
  if(dbg_lvl_get() > 0) (void)sprintf(ncap_err_sng,"Saving attribute %s@%s to %s",(yyvsp[(1) - (3)].aed).var_nm,(yyvsp[(1) - (3)].aed).att_nm,((prs_sct *)prs_arg)->fl_out);
  (void)nco_yyerror(prs_arg,ncap_err_sng);
  
  if(dbg_lvl_get() > 1){
    (void)fprintf(stderr,"Saving in array attribute %s@%s=",(yyvsp[(1) - (3)].aed).var_nm,(yyvsp[(1) - (3)].aed).att_nm);
    switch((yyvsp[(3) - (3)].scv).type){
  /* NB: Format depends on opaque type of nco_int
     Until 200911, nco_int was C-type long, and now nco_int is C-type int
     case NC_INT: (void)fprintf(stderr,"%ld\n",$3.val.i); break; */
    case NC_FLOAT: (void)fprintf(stderr,"%G\n",(yyvsp[(3) - (3)].scv).val.f); break;
    case NC_DOUBLE: (void)fprintf(stderr,"%.5G\n",(yyvsp[(3) - (3)].scv).val.d);break;
    case NC_INT: (void)fprintf(stderr,"%d\n",(yyvsp[(3) - (3)].scv).val.i); break;
    case NC_SHORT: (void)fprintf(stderr,"%hi\n",(yyvsp[(3) - (3)].scv).val.s); break;
    case NC_BYTE: (void)fprintf(stderr,"%hhi\n",(yyvsp[(3) - (3)].scv).val.b); break;
    case NC_UBYTE: (void)fprintf(stderr,"%hhu\n",(yyvsp[(3) - (3)].scv).val.ub); break;
    case NC_USHORT: (void)fprintf(stderr,"%hu\n",(yyvsp[(3) - (3)].scv).val.us); break;
    case NC_UINT: (void)fprintf(stderr,"%u\n",(yyvsp[(3) - (3)].scv).val.ui); break;
    case NC_INT64: (void)fprintf(stderr,"%lld\n",(yyvsp[(3) - (3)].scv).val.i64); break;
    case NC_UINT64: (void)fprintf(stderr,"%llu\n",(yyvsp[(3) - (3)].scv).val.ui64); break;
    case NC_CHAR: (void)fprintf(stderr,"%c\n",(yyvsp[(3) - (3)].scv).val.c); break;
    case NC_STRING: (void)fprintf(stderr,"%s\n",(yyvsp[(3) - (3)].scv).val.sng); break;
    default: break;
    } /* end switch */
  } /* end if */
  (yyvsp[(1) - (3)].aed).var_nm=(char *)nco_free((yyvsp[(1) - (3)].aed).var_nm);
  (yyvsp[(1) - (3)].aed).att_nm=(char *)nco_free((yyvsp[(1) - (3)].aed).att_nm);
}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 239 "ncap_yacc.y"
    {
  aed_sct *ptr_aed;
  size_t sng_lng;
  
  sng_lng=strlen((yyvsp[(3) - (3)].sng));
  ptr_aed=ncap_aed_lookup((yyvsp[(1) - (3)].aed).var_nm,(yyvsp[(1) - (3)].aed).att_nm,((prs_sct *)prs_arg),True);
  ptr_aed->type=NC_CHAR;
  ptr_aed->sz=(long)((sng_lng+1)*nco_typ_lng(NC_CHAR));
  ptr_aed->val.cp=(nco_char *)nco_malloc((sng_lng+1)*nco_typ_lng(NC_CHAR));
  strcpy((char *)(ptr_aed->val.cp),(yyvsp[(3) - (3)].sng));
  (void)cast_nctype_void((nc_type)NC_CHAR,&ptr_aed->val);    
  
  if(dbg_lvl_get() > 0) (void)sprintf(ncap_err_sng,"Saving attribute %s@%s=%s",(yyvsp[(1) - (3)].aed).var_nm,(yyvsp[(1) - (3)].aed).att_nm,(yyvsp[(3) - (3)].sng));
  (void)nco_yyerror(prs_arg, ncap_err_sng);
  (yyvsp[(1) - (3)].aed).var_nm=(char *)nco_free((yyvsp[(1) - (3)].aed).var_nm);
  (yyvsp[(1) - (3)].aed).att_nm=(char *)nco_free((yyvsp[(1) - (3)].aed).att_nm);
  (yyvsp[(3) - (3)].sng)=(char *)nco_free((yyvsp[(3) - (3)].sng));
}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 258 "ncap_yacc.y"
    { 
  /* Storing 0-dimensional variables in attribute is OK */ 
  aed_sct *ptr_aed;
  
  if((yyvsp[(3) - (3)].var)->nbr_dim < 2){
    ptr_aed=ncap_aed_lookup((yyvsp[(1) - (3)].aed).var_nm,(yyvsp[(1) - (3)].aed).att_nm,((prs_sct *)prs_arg),True);
    ptr_aed->sz=(yyvsp[(3) - (3)].var)->sz;
    ptr_aed->type=(yyvsp[(3) - (3)].var)->type;
    /* if inital scan then fill attribute with zeros */
    if( ((prs_sct*)prs_arg)->ntl_scn) {
    ptr_aed->val.vp=(void*)nco_calloc( ptr_aed->sz,nco_typ_lng(ptr_aed->type));
    } else {
    ptr_aed->val.vp=(void*)nco_malloc((ptr_aed->sz)*nco_typ_lng(ptr_aed->type));
    (void)var_copy(ptr_aed->type,ptr_aed->sz,(yyvsp[(3) - (3)].var)->val,ptr_aed->val);
    }
    /* cast_nctype_void($3->type,&ptr_aed->val); */
    if(dbg_lvl_get() > 0) (void)sprintf(ncap_err_sng,"Saving attribute %s@%s %d dimensional variable",(yyvsp[(1) - (3)].aed).var_nm,(yyvsp[(1) - (3)].aed).att_nm,(yyvsp[(3) - (3)].var)->nbr_dim);
    (void)yyerror(prs_arg,ncap_err_sng); 
  }else{
    (void)sprintf(ncap_err_sng,"Warning: Cannot store in attribute %s@%s a variable with dimension %d",(yyvsp[(1) - (3)].aed).var_nm,(yyvsp[(1) - (3)].aed).att_nm,(yyvsp[(3) - (3)].var)->nbr_dim);
    (void)yyerror(prs_arg,ncap_err_sng);
  } /* endif */
  (yyvsp[(1) - (3)].aed).var_nm=(char *)nco_free((yyvsp[(1) - (3)].aed).var_nm);
  (yyvsp[(1) - (3)].aed).att_nm=(char *)nco_free((yyvsp[(1) - (3)].aed).att_nm);
  (void)nco_var_free((yyvsp[(3) - (3)].var)); 
}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 285 "ncap_yacc.y"
    {
  ((yyvsp[(3) - (3)].var)->nm)=(char*)nco_free((yyvsp[(3) - (3)].var)->nm);
  (yyvsp[(3) - (3)].var)->nm=strdup((yyvsp[(1) - (3)].var_nm_LHS));
  (void)ncap_var_write((yyvsp[(3) - (3)].var),(prs_sct *)prs_arg);
  
  /* Print mess only for defined variables */
  if(dbg_lvl_get() > 0 && !(yyvsp[(3) - (3)].var)->undefined){(void)sprintf(ncap_err_sng,"Saving variable %s to %s",(yyvsp[(1) - (3)].var_nm_LHS),((prs_sct *)prs_arg)->fl_out);
  (void)yyerror(prs_arg,ncap_err_sng);
  } /* endif */
  (yyvsp[(1) - (3)].var_nm_LHS)=(char *)nco_free((yyvsp[(1) - (3)].var_nm_LHS));
}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 297 "ncap_yacc.y"
    {
  var_sct *var;
  var_sct *var_tmp;  
  if(dbg_lvl_get() > 5) (void)fprintf(stderr,"%s: DEBUG out_var_xpr = scv_xpr rule for %s\n",prg_nm_get(),(yyvsp[(1) - (3)].var_nm_LHS));
  
  /* Turn attribute into temporary variable for writing */
  var=(var_sct *)nco_malloc(sizeof(var_sct));
  /* Set defaults */
  (void)var_dfl_set(var); /* [fnc] Set defaults for each member of variable structure */
  /* Overwrite with attribute expression information */
  var->nm=strdup((yyvsp[(1) - (3)].var_nm_LHS));
  var->nbr_dim=0;
  var->sz=1;
  var->type=(yyvsp[(3) - (3)].scv).type;
  var->val=ncap_scv_2_ptr_unn((yyvsp[(3) - (3)].scv));
  
  if(((prs_sct *)prs_arg)->var_LHS != NULL){
    /* User intends LHS to cast RHS to same dimensionality
       Stretch newly initialized variable to size of LHS template */
    /*    (void)ncap_var_cnf_dmn(&$$,&(((prs_sct *)prs_arg)->var_LHS));*/
    var_tmp=var;
    (void)ncap_var_stretch(&var_tmp,&(((prs_sct *)prs_arg)->var_LHS));
    if(var_tmp != var) {
      var=nco_var_free(var);
      var=var_tmp;
	}

    if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: Stretching former scv_xpr defining %s with LHS template: Template var->nm %s, var->nbr_dim %d, var->sz %li\n",prg_nm_get(),(yyvsp[(1) - (3)].var_nm_LHS),((prs_sct *)prs_arg)->var_LHS->nm,((prs_sct *)prs_arg)->var_LHS->nbr_dim,((prs_sct *)prs_arg)->var_LHS->sz);
  } /* endif LHS_cst */

  var->undefined=False;
  (void)ncap_var_write(var,(prs_sct *)prs_arg);
  
  if(dbg_lvl_get() > 0 ) (void)sprintf(ncap_err_sng,"Saving variable %s to %s",(yyvsp[(1) - (3)].var_nm_LHS),((prs_sct *)prs_arg)->fl_out);
  (void)yyerror(prs_arg,ncap_err_sng);
  
  
  (yyvsp[(1) - (3)].var_nm_LHS)=(char *)nco_free((yyvsp[(1) - (3)].var_nm_LHS));
}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 337 "ncap_yacc.y"
    {
  
  var_sct *var;
  
  var=(var_sct *)nco_calloc((size_t)1,sizeof(var_sct));
  var->nm=strdup((yyvsp[(1) - (3)].var_nm_LHS));
  var->nbr_dim=0;
  var->dmn_id=(int *)NULL;
  var->sz=strlen((yyvsp[(3) - (3)].sng))+1;
  var->val.cp=(nco_char *)strdup((yyvsp[(3) - (3)].sng));
  var->type=NC_CHAR;
  var->undefined=False;
  (void)cast_nctype_void((nc_type)NC_CHAR,&var->val);
  (void)ncap_var_write(var,(prs_sct *)prs_arg);
  
  if(dbg_lvl_get() > 0) (void)sprintf(ncap_err_sng,"Saving variable %s to %s",(yyvsp[(1) - (3)].var_nm_LHS),((prs_sct *)prs_arg)->fl_out);
  (void)yyerror(prs_arg,ncap_err_sng);
  
  (yyvsp[(1) - (3)].var_nm_LHS)=(char *)nco_free((yyvsp[(1) - (3)].var_nm_LHS));
  (yyvsp[(3) - (3)].sng)=(char *)nco_free((yyvsp[(3) - (3)].sng));
}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 362 "ncap_yacc.y"
    {
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&(yyvsp[(1) - (3)].scv),&(yyvsp[(3) - (3)].scv));
  (yyval.scv)=ncap_scv_clc((yyvsp[(1) - (3)].scv),'+',(yyvsp[(3) - (3)].scv));                            
}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 366 "ncap_yacc.y"
    {
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&(yyvsp[(1) - (3)].scv),&(yyvsp[(3) - (3)].scv)); 
  (yyval.scv)=ncap_scv_clc((yyvsp[(1) - (3)].scv),'-',(yyvsp[(3) - (3)].scv));
}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 370 "ncap_yacc.y"
    {
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&(yyvsp[(1) - (3)].scv),&(yyvsp[(3) - (3)].scv));
  (yyval.scv)=ncap_scv_clc((yyvsp[(1) - (3)].scv),'*',(yyvsp[(3) - (3)].scv));
}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 374 "ncap_yacc.y"
    {
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&(yyvsp[(1) - (3)].scv),&(yyvsp[(3) - (3)].scv)); 
  (yyval.scv)=ncap_scv_clc((yyvsp[(1) - (3)].scv),'/',(yyvsp[(3) - (3)].scv));  
}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 378 "ncap_yacc.y"
    {
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&(yyvsp[(1) - (3)].scv),&(yyvsp[(3) - (3)].scv));
  
  (yyval.scv)=ncap_scv_clc((yyvsp[(1) - (3)].scv),'%',(yyvsp[(3) - (3)].scv));  
}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 383 "ncap_yacc.y"
    {
  (void)ncap_scv_minus(&(yyvsp[(2) - (2)].scv));
  (yyval.scv)=(yyvsp[(2) - (2)].scv);
}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 387 "ncap_yacc.y"
    {
  (yyval.scv)=(yyvsp[(2) - (2)].scv);
}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 390 "ncap_yacc.y"
    {
  if(nco_rth_prc_rnk((yyvsp[(1) - (3)].scv).type) <= nco_rth_prc_rnk_float && nco_rth_prc_rnk((yyvsp[(3) - (3)].scv).type) <= nco_rth_prc_rnk_float) {
    (void)nco_scv_cnf_typ((nc_type)NC_FLOAT,&(yyvsp[(1) - (3)].scv));
    (void)nco_scv_cnf_typ((nc_type)NC_FLOAT,&(yyvsp[(3) - (3)].scv));
    (yyval.scv).val.f=powf((yyvsp[(1) - (3)].scv).val.f,(yyvsp[(3) - (3)].scv).val.f);
    (yyval.scv).type=NC_FLOAT;
  }else{
    (void)nco_scv_cnf_typ((nc_type)NC_DOUBLE,&(yyvsp[(1) - (3)].scv));
    (void)nco_scv_cnf_typ((nc_type)NC_DOUBLE,&(yyvsp[(3) - (3)].scv));
    (yyval.scv).val.d=pow((yyvsp[(1) - (3)].scv).val.d,(yyvsp[(3) - (3)].scv).val.d);
    (yyval.scv).type=NC_DOUBLE; 
  } /* end else */
}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 403 "ncap_yacc.y"
    { /* fxm: ncap52 this is identical to previous clause except for argument numbering, should be functionalized to use common code */
  if(nco_rth_prc_rnk((yyvsp[(3) - (6)].scv).type) <= nco_rth_prc_rnk_float && nco_rth_prc_rnk((yyvsp[(5) - (6)].scv).type) <= nco_rth_prc_rnk_float) {
    (void)nco_scv_cnf_typ((nc_type)NC_FLOAT,&(yyvsp[(3) - (6)].scv));
    (void)nco_scv_cnf_typ((nc_type)NC_FLOAT,&(yyvsp[(5) - (6)].scv));
    (yyval.scv).val.f=powf((yyvsp[(3) - (6)].scv).val.f,(yyvsp[(5) - (6)].scv).val.f);
    (yyval.scv).type=NC_FLOAT;
  }else{ 
    (void)nco_scv_cnf_typ((nc_type)NC_DOUBLE,&(yyvsp[(3) - (6)].scv));
    (void)nco_scv_cnf_typ((nc_type)NC_DOUBLE,&(yyvsp[(5) - (6)].scv));
    (yyval.scv).val.d=pow((yyvsp[(3) - (6)].scv).val.d,(yyvsp[(5) - (6)].scv).val.d);
    (yyval.scv).type=NC_DOUBLE; 
  } /* end else */
}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 416 "ncap_yacc.y"
    {
  (yyval.scv)=ncap_scv_abs((yyvsp[(3) - (4)].scv));
}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 419 "ncap_yacc.y"
    {
  if(nco_rth_prc_rnk((yyvsp[(3) - (4)].scv).type) <= nco_rth_prc_rnk_float) {
    (void)nco_scv_cnf_typ((nc_type)NC_FLOAT,&(yyvsp[(3) - (4)].scv));
    (yyval.scv).val.f=(*((yyvsp[(1) - (4)].sym)->fnc_flt))((yyvsp[(3) - (4)].scv).val.f);
    (yyval.scv).type=NC_FLOAT;
  }else{
    (yyval.scv).val.d=(*((yyvsp[(1) - (4)].sym)->fnc_dbl))((yyvsp[(3) - (4)].scv).val.d);
    (yyval.scv).type=NC_DOUBLE;
  } /* end else */
}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 429 "ncap_yacc.y"
    {
  (void)nco_scv_cnf_typ((yyvsp[(1) - (4)].cnv_type),&(yyvsp[(3) - (4)].scv));
  (yyval.scv)=(yyvsp[(3) - (4)].scv);
}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 433 "ncap_yacc.y"
    {(yyval.scv)=(yyvsp[(2) - (3)].scv);}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 434 "ncap_yacc.y"
    {(yyval.scv)=(yyvsp[(1) - (1)].scv);}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 437 "ncap_yacc.y"
    {(yyval.var_nm_LHS)=(yyvsp[(1) - (1)].var_nm_LHS);}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 440 "ncap_yacc.y"
    {(yyval.aed)=(yyvsp[(1) - (1)].aed);}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 444 "ncap_yacc.y"
    {
  size_t sng_lng;
  sng_lng=strlen((yyvsp[(1) - (3)].sng))+strlen((yyvsp[(3) - (3)].sng));
  (yyval.sng)=(char*)nco_malloc((sng_lng+1)*sizeof(char));
  strcpy((yyval.sng),(yyvsp[(1) - (3)].sng));
  strcat((yyval.sng),(yyvsp[(3) - (3)].sng));
  (yyvsp[(1) - (3)].sng)=(char *)nco_free((yyvsp[(1) - (3)].sng));
  (yyvsp[(3) - (3)].sng)=(char *)nco_free((yyvsp[(3) - (3)].sng));
}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 453 "ncap_yacc.y"
    {
  char bfr[50];
  switch ((yyvsp[(3) - (4)].scv).type){
  /* NB: Format depends on opaque type of nco_int
     Until 200911, nco_int was C-type long, and now nco_int is C-type int
     case NC_INT: sprintf(bfr,"%ld",$3.val.i); break; */
  case NC_DOUBLE: sprintf(bfr,"%.10G",(yyvsp[(3) - (4)].scv).val.d); break;
  case NC_FLOAT: sprintf(bfr,"%G",(yyvsp[(3) - (4)].scv).val.f); break;
  case NC_INT: sprintf(bfr,"%d",(yyvsp[(3) - (4)].scv).val.i); break;
  case NC_SHORT: sprintf(bfr,"%hi",(yyvsp[(3) - (4)].scv).val.s); break;
  case NC_BYTE: sprintf(bfr,"%hhi",(yyvsp[(3) - (4)].scv).val.b); break;
  case NC_UBYTE: sprintf(bfr,"%hhu",(yyvsp[(3) - (4)].scv).val.ub); break;
  case NC_USHORT: sprintf(bfr,"%hu",(yyvsp[(3) - (4)].scv).val.us); break;
  case NC_UINT: sprintf(bfr,"%u",(yyvsp[(3) - (4)].scv).val.ui); break;
  case NC_INT64: sprintf(bfr,"%lld",(yyvsp[(3) - (4)].scv).val.i64); break;
  case NC_UINT64: sprintf(bfr,"%llu",(yyvsp[(3) - (4)].scv).val.ui64); break;
  case NC_CHAR: sprintf(bfr,"%c",(yyvsp[(3) - (4)].scv).val.c); break;
  case NC_STRING: sprintf(bfr,"%s",(yyvsp[(3) - (4)].scv).val.sng); break;
  default:  break;
  } /* end switch */
  (yyval.sng)=strdup(bfr);      
}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 475 "ncap_yacc.y"
    {
  char bfr[150];
  /* Format string according to string expression */
  /* User decides which format corresponds to which type */
  switch ((yyvsp[(3) - (6)].scv).type){
  case NC_DOUBLE: sprintf(bfr,(yyvsp[(5) - (6)].sng),(yyvsp[(3) - (6)].scv).val.d); break;
  case NC_FLOAT: sprintf(bfr,(yyvsp[(5) - (6)].sng),(yyvsp[(3) - (6)].scv).val.f); break;
  case NC_INT: sprintf(bfr,(yyvsp[(5) - (6)].sng),(yyvsp[(3) - (6)].scv).val.i); break;
  case NC_SHORT: sprintf(bfr,(yyvsp[(5) - (6)].sng),(yyvsp[(3) - (6)].scv).val.s); break;
  case NC_BYTE: sprintf(bfr,(yyvsp[(5) - (6)].sng),(yyvsp[(3) - (6)].scv).val.b); break;
  case NC_UBYTE: sprintf(bfr,(yyvsp[(5) - (6)].sng),(yyvsp[(3) - (6)].scv).val.ub); break;
  case NC_USHORT: sprintf(bfr,(yyvsp[(5) - (6)].sng),(yyvsp[(3) - (6)].scv).val.us); break;
  case NC_UINT: sprintf(bfr,(yyvsp[(5) - (6)].sng),(yyvsp[(3) - (6)].scv).val.ui); break;
  case NC_INT64: sprintf(bfr,(yyvsp[(5) - (6)].sng),(yyvsp[(3) - (6)].scv).val.i64); break;
  case NC_UINT64: sprintf(bfr,(yyvsp[(5) - (6)].sng),(yyvsp[(3) - (6)].scv).val.ui64); break;
  case NC_CHAR: sprintf(bfr,(yyvsp[(5) - (6)].sng),(yyvsp[(3) - (6)].scv).val.c); break;
  case NC_STRING: sprintf(bfr,(yyvsp[(5) - (6)].sng),(yyvsp[(3) - (6)].scv).val.sng); break;
  default:  break;
  } /* end switch */
  (yyvsp[(5) - (6)].sng)=(char *)nco_free((yyvsp[(5) - (6)].sng));
  (yyval.sng)=strdup(bfr);      
}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 497 "ncap_yacc.y"
    {(yyval.sng)=(yyvsp[(1) - (1)].sng);}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 501 "ncap_yacc.y"
    {
}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 503 "ncap_yacc.y"
    {
}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 505 "ncap_yacc.y"
    {
}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 507 "ncap_yacc.y"
    {
}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 513 "ncap_yacc.y"
    { /* Begin Addition */
  (yyval.var)=ncap_var_var_add((yyvsp[(1) - (3)].var),(yyvsp[(3) - (3)].var)); 
}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 516 "ncap_yacc.y"
    {
  (yyval.var)=ncap_var_scv_add((yyvsp[(1) - (3)].var),(yyvsp[(3) - (3)].scv));
}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 519 "ncap_yacc.y"
    {
  /* Addition commutes so swap arguments and use S+V = V+S */
  (yyval.var)=ncap_var_scv_add((yyvsp[(3) - (3)].var),(yyvsp[(1) - (3)].scv));
}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 523 "ncap_yacc.y"
    { /* Begin Subtraction */
  (yyval.var)=ncap_var_var_sub((yyvsp[(1) - (3)].var),(yyvsp[(3) - (3)].var));
}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 526 "ncap_yacc.y"
    {
  (yyval.var)=ncap_var_scv_sub((yyvsp[(1) - (3)].var),(yyvsp[(3) - (3)].scv));
}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 529 "ncap_yacc.y"
    { 
  /* Subtraction is non-commutative, do not swap arguments and/or re-use V-S subtraction function
     Use anti-symmetric property that V-S=-(S-V) */
  scv_sct minus;
  minus.val.b=-1;
  minus.type=NC_BYTE;
  (void)nco_scv_cnf_typ((yyvsp[(3) - (3)].var)->type,&minus);
  (void)ncap_var_scv_sub((yyvsp[(3) - (3)].var),(yyvsp[(1) - (3)].scv));
  (yyval.var)=ncap_var_scv_mlt((yyvsp[(3) - (3)].var),minus);
}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 539 "ncap_yacc.y"
    { /* Begin Multiplication */
  (yyval.var)=ncap_var_var_mlt((yyvsp[(1) - (3)].var),(yyvsp[(3) - (3)].var)); 
}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 542 "ncap_yacc.y"
    {
  (yyval.var)=ncap_var_scv_mlt((yyvsp[(1) - (3)].var),(yyvsp[(3) - (3)].scv));
}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 545 "ncap_yacc.y"
    {
  /* Addition commutes so swap arguments and use S*V = V*S */
  (yyval.var)=ncap_var_scv_mlt((yyvsp[(3) - (3)].var),(yyvsp[(1) - (3)].scv));
}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 549 "ncap_yacc.y"
    { /* Begin Division */
  /* NB: Order was important (keeping denominator as I/O variable)
     This is no longer true with ncbo
     Maybe rewrite to keep argument ordering consitent with multiplication, addition */
  (yyval.var)=ncap_var_var_dvd((yyvsp[(3) - (3)].var),(yyvsp[(1) - (3)].var));
}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 555 "ncap_yacc.y"
    { /* Keep V as I/O */
  (yyval.var)=ncap_var_scv_dvd((yyvsp[(1) - (3)].var),(yyvsp[(3) - (3)].scv));
}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 558 "ncap_yacc.y"
    {
 /* Division is non-commutative, use S/V not V/S division function */
  (yyval.var)=ncap_scv_var_dvd((yyvsp[(1) - (3)].scv),(yyvsp[(3) - (3)].var));
}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 562 "ncap_yacc.y"
    { /* Begin Modulo */
  (yyval.var)=ncap_var_var_mod((yyvsp[(1) - (3)].var),(yyvsp[(3) - (3)].var));
}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 565 "ncap_yacc.y"
    {
  (yyval.var)=ncap_var_scv_mod((yyvsp[(1) - (3)].var),(yyvsp[(3) - (3)].scv));
}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 568 "ncap_yacc.y"
    {
  /* Modulo is non-commutative, use S%V not V%S modulo function */
  (yyval.var)=ncap_scv_var_mod((yyvsp[(1) - (3)].scv),(yyvsp[(3) - (3)].var));
}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 572 "ncap_yacc.y"
    { /* Begin Empowerment of form x^y */
  (yyval.var)=ncap_var_var_pwr((yyvsp[(1) - (3)].var),(yyvsp[(3) - (3)].var));
}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 575 "ncap_yacc.y"
    {
  (yyval.var)=ncap_var_scv_pwr((yyvsp[(1) - (3)].var),(yyvsp[(3) - (3)].scv));
}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 578 "ncap_yacc.y"
    {
  /* Empowerment is non-commutative, use S^V not V^S empowerment function */
  (yyval.var)=ncap_scv_var_pwr((yyvsp[(1) - (3)].scv),(yyvsp[(3) - (3)].var));
}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 582 "ncap_yacc.y"
    { /* Begin Empowerment of form pow(x,y) */
  /* fxm: TODO ncap52 Combine pow() with ^ parsing in parser ncap_yacc.y */
  (yyval.var)=ncap_var_var_pwr((yyvsp[(3) - (6)].var),(yyvsp[(5) - (6)].var));
}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 586 "ncap_yacc.y"
    {
  (yyval.var)=ncap_var_scv_pwr((yyvsp[(3) - (6)].var),(yyvsp[(5) - (6)].scv));
}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 589 "ncap_yacc.y"
    {
  /* Empowerment is non-commutative, use S^V not V^S empowerment function */
  (yyval.var)=ncap_scv_var_pwr((yyvsp[(3) - (6)].scv),(yyvsp[(5) - (6)].var));
}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 593 "ncap_yacc.y"
    { /* Begin Unary Subtraction */
  scv_sct minus;
  minus.val.b=-1;
  minus.type=NC_BYTE;
  (yyval.var)=ncap_var_scv_mlt((yyvsp[(2) - (2)].var),minus);
}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 599 "ncap_yacc.y"
    { /* Begin Unary Addition */
  (yyval.var)=(yyvsp[(2) - (2)].var);
}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 602 "ncap_yacc.y"
    {
  (yyval.var)=ncap_var_abs((yyvsp[(3) - (4)].var));
}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 605 "ncap_yacc.y"
    {
  (yyval.var)=ncap_var_abs((yyvsp[(3) - (4)].var));
  /* fxm Finish avg,min,max,ttl */
  /* $$=nco_var_avg($3,dim,dmn_nbr,nco_op_typ); */
  /* if(prs_arg->nco_op_typ == nco_op_avg) (void)nco_var_dvd(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,wgt_avg->val,var_prc_out[idx]->val); */
  (void)fprintf(stderr,"%s: WARNING RDC tokens not implemented yet\n",prg_nm_get());
  /* $3 is freed in nco_var_avg() */
}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 613 "ncap_yacc.y"
    {
  /* Packing variable does not create duplicate so DO NOT free $3 */
  const nc_type nc_typ_pck_dfl=NC_SHORT; /* [enm] Default type to pack to */
  nco_bool PCK_VAR_WITH_NEW_PCK_ATT;
  
  if(((prs_sct*)prs_arg)->ntl_scn){
    (yyvsp[(3) - (4)].var)->undefined=True;
    (yyval.var)=(yyvsp[(3) - (4)].var);
  }else{ 
    (yyval.var)=nco_var_pck((yyvsp[(3) - (4)].var),nc_typ_pck_dfl,&PCK_VAR_WITH_NEW_PCK_ATT);
    PCK_VAR_WITH_NEW_PCK_ATT=PCK_VAR_WITH_NEW_PCK_ATT; /* CEWI */
  } /* end else */
}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 626 "ncap_yacc.y"
    {
  /* Unpacking variable does not create duplicate so DO NOT free $3 */
  /* Do not unpack on first pass */
  if(((prs_sct*)prs_arg)->ntl_scn){
    (yyvsp[(3) - (4)].var)->undefined=True;
    (yyval.var)=(yyvsp[(3) - (4)].var);
  }else{ 
    (yyval.var)=nco_var_upk((yyvsp[(3) - (4)].var));
  } /* end else */
}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 636 "ncap_yacc.y"
    {
  (yyval.var)=ncap_var_fnc((yyvsp[(3) - (4)].var),(yyvsp[(1) - (4)].sym));
}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 639 "ncap_yacc.y"
    {
  (yyval.var)=(yyvsp[(2) - (3)].var);
}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 642 "ncap_yacc.y"
    {
  (yyval.var)=nco_var_cnf_typ((yyvsp[(1) - (4)].cnv_type),(yyvsp[(3) - (4)].var));
}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 645 "ncap_yacc.y"
    { /* Terminal symbol action */
  /* fxm: Allow commands like a=M_PI*rds^2; to work */
}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 648 "ncap_yacc.y"
    { /* Terminal symbol action */
  
  var_sct *var;
  var_sct *var_tmp;
  prs_sct *prs_drf; /*Pointer for de-referencing */
  
  prs_drf=(prs_sct*)prs_arg;
  
  var=ncap_var_init((yyvsp[(1) - (1)].var_nm_RHS),prs_drf);
  var->undefined=False;
  
  if(prs_drf->ntl_scn == True && prs_drf->var_LHS != NULL){
    var_tmp=nco_var_dpl(prs_drf->var_LHS);
    var_tmp->id=var->id;
    var_tmp->nm=(char*)nco_free(var_tmp->nm);
    var_tmp->nm=strdup((yyvsp[(1) - (1)].var_nm_RHS));
    var_tmp->type=var->type;
    var_tmp->typ_dsk=var->typ_dsk;
    var_tmp->undefined=False;
    var_tmp->val.vp=(void*)NULL;
    var=nco_var_free(var);
    var=var_tmp;
  } /* endif ntl_scn */
  
  if(prs_drf->ntl_scn == False && prs_drf->var_LHS != NULL){
    /* User intends LHS to cast RHS to same dimensionality
       Stretch newly initialized variable to size of LHS template */
    /*    (void)ncap_var_cnf_dmn(&$$,&(((prs_sct *)prs_arg)->var_LHS));*/
    var_tmp=var;
    (void)ncap_var_stretch(&var_tmp,&(prs_drf->var_LHS));
    if(var_tmp != var) { 
      var=nco_var_free(var); 
      var=var_tmp;
    }
  
    if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: Stretching variable %s with LHS template: Template var->nm %s, var->nbr_dim %d, var->sz %li\n",prg_nm_get(),var->nm,prs_drf->var_LHS->nm,prs_drf->var_LHS->nbr_dim,prs_drf->var_LHS->sz);
    var->undefined=False;
  } /* endif LHS_cst */
  (yyvsp[(1) - (1)].var_nm_RHS)=(char*)nco_free((yyvsp[(1) - (1)].var_nm_RHS));
  (yyval.var)=var;
  /* Sanity check */
  if ((yyval.var)==(var_sct *)NULL) YYERROR;
}
    break;



/* Line 1455 of yacc.c  */
#line 2622 "ncap_yacc.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (prs_arg, YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (prs_arg, yymsg);
	  }
	else
	  {
	    yyerror (prs_arg, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
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
		      yytoken, &yylval, prs_arg);
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
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
		  yystos[yystate], yyvsp, prs_arg);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (prs_arg, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, prs_arg);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, prs_arg);
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
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 694 "ncap_yacc.y"

/* Begin User Functions section */

aed_sct *  /* O [idx] Location of attribute in list */
ncap_aed_lookup /* [fnc] Find location of existing attribute or add new attribute */
(const char * const var_nm, /* I [sng] Variable name */
 const char * const att_nm, /* I [sng] Attribute name */
 prs_sct *  prs_arg,   /* contains attribute list */       
 const nco_bool update) /* I [flg] Delete existing value or add new attribute to list */
{
  int idx;
  int size;
  aed_sct *ptr_aed;
  
  size=*(prs_arg->nbr_att);
  
  for(idx=0;idx<size;idx++){
    ptr_aed=(*(prs_arg->att_lst))[idx];
    
    if(strcmp(ptr_aed->att_nm,att_nm) || strcmp(ptr_aed->var_nm,var_nm)) 
      continue; 
    
    if(update) ptr_aed->val.vp=nco_free(ptr_aed->val.vp);   
    /* Return pointer to list element */
    return ptr_aed;
    
  } /* end for */
  
  if(!update) return (aed_sct *)NULL;
  
  *(prs_arg->att_lst)=(aed_sct **)nco_realloc(*(prs_arg->att_lst),(size+1)*sizeof(aed_sct*));
  ++*(prs_arg->nbr_att);
  (*(prs_arg->att_lst))[size]=(aed_sct *)nco_malloc(sizeof(aed_sct));
  (*(prs_arg->att_lst))[size]->var_nm=strdup(var_nm);
  (*(prs_arg->att_lst))[size]->att_nm=strdup(att_nm);
  
  return (*(prs_arg->att_lst))[size];
} /* end ncap_aed_lookup() */

var_sct * /*I [sct] varibale in list */
ncap_var_lookup
(var_sct *var, /* I  [sct] variable  */
 prs_sct *prs_arg, /* I/O [sct] contains var list */
 const nco_bool add) /* I if not in list then add to list */          
{
  int idx;
  int size;
  
  var_sct *ptr_var; 
  
  size = *(prs_arg->nbr_var);
  
  for(idx=0;idx<size;idx++){
    
    ptr_var=(*(prs_arg->var_lst))[idx];
    /*
      assert(var->nm);
      assert(ptr_var->nm);
      if(!strcmp(var->nm,ptr_var->nm)) return ptr_var;    
    */
    if(ptr_var==NULL || strcmp(var->nm,ptr_var->nm) ) continue;
    
    return ptr_var;
  } /* end loop over idx */
  
  if(!add) return (var_sct *)NULL;
  
  *(prs_arg->var_lst)=(var_sct **)nco_realloc(*(prs_arg->var_lst),(size+1)*sizeof(var_sct*));
  ++*(prs_arg->nbr_var);
  (*(prs_arg->var_lst))[size]=var;
  
  return (var_sct *)NULL;
} /* end ncap_var_lookup() */

int /* [rcd] Return code */
yyerror /* [fnc] Print error/warning/info messages generated by parser */
(prs_sct *prs_arg, const char * const err_sng_lcl) /* [sng] Message to print */
{
  /* Purpose: Print error/warning/info messages generated by parser
     Use eprokoke_skip to skip error message after sending error message from yylex()
     Stop provoked error message from yyparse being printed */
  
  static nco_bool eprovoke_skip;
  
  prs_arg=prs_arg; /* CEWI otherwise unused parameter error */

  /* if(eprovoke_skip){eprovoke_skip=False ; return 0;} */
  if(dbg_lvl_get() > 0){
    (void)fprintf(stderr,"%s: %s line %lu",prg_nm_get(),ncap_fl_spt_glb[ncap_ncl_dpt_crr],(unsigned long)ncap_ln_nbr_crr[ncap_ncl_dpt_crr]);
    if(dbg_lvl_get() > 1) (void)fprintf(stderr," %s",err_sng_lcl);
    (void)fprintf(stderr,"\n");
    (void)fflush(stderr);
  } /* endif dbg */
  
  if(err_sng_lcl[0] == '#') eprovoke_skip=True;
  eprovoke_skip=eprovoke_skip; /* Do nothing except avoid compiler warnings */
  return 0;
} /* end yyerror() */

void 
nco_var_free_wrp /* [fnc] Safely free variable */
(var_sct **var) /* I/O [sct] Variable */
{
  /* Purpose: Safely free variable
     Routine is wrapper for nco_var_free() that simplifies code in calling routine */
  if(*var != NULL) *var=nco_var_free(*var);
} /* end nco_var_free_wrp() */

nodeType * /* O [unn] Syntax tree node */
opr_ctl /* [fnc] Operation controller function Nie02 opr() */
(int opr_tkn, /* I [enm] Operator token */
 int arg_nbr, /* I [nbr] Number of optional arguments to malloc() wrapper */
 ...) /* I [llp] Ellipsis defined in stdarg.h */
{
  /* Purpose: Create and return syntax tree node */
  va_list arg_lst; /* [] Variable argument list */
  nodeType *nod; /* [sct] Syntax tree node */
  size_t nod_sz; /* [nbr] Node size */
  int arg_idx; /* [idx] Argument index */
  
  /* Operator node requires space for token and arguments */
  nod_sz=sizeof(opr_nod_sct)+(arg_nbr-1)*sizeof(nodeType *);
  nod=(nodeType *)nco_malloc(nod_sz);
  /* Copy information into new node */
  nod->nod_typ=typ_opr; /* [enm] Node type */
  nod->opr.opr_tkn=opr_tkn; /* [enm] Operator token */
  nod->opr.arg_nbr=arg_nbr; /* [nbr] Number of arguments */
  /* Begin variable argument list access */
  va_start(arg_lst,arg_nbr);
  for(arg_idx=0;arg_idx<nod->opr.arg_nbr;arg_idx++) nod->opr.arg[arg_idx]=va_arg(arg_lst,nodeType); /* NB: Nie02 p. 27 has typo in va_arg() */
  /* End variable argument list access */
  va_end(arg_lst);
  return nod;
} /* end opr_ctl() */

void
freeNode /* [fnc] Free syntax tree node Nie02 freeNode() */
(nodeType *nod) /* I/O [sct] Syntax tree node to free */
{
  /* Purpose: Free syntax tree node */
  int arg_idx; /* [idx] Argument index */
  
  if(!nod) return;
  
  /* Operator nodes have copies of arguments. Free these first. */
  if(nod->nod_typ == typ_opr){
    /* Recursive call to freeNode continue until statement is reduced */
    for(arg_idx=0;arg_idx<nod->opr.arg_nbr;arg_idx++) freeNode(nod->opr.arg+arg_idx); /* Nie02 p. 28 has typo and passes node not node pointer */
  } /* endif */
  /* Free node itself */
  nod=(nodeType *)nco_free(nod);
  
  return; /* 20050109: fxm added return to void function to squelch erroneous gcc-3.4.2 warning */ 
} /* end freeNode() */

/* End User Functions section */

