/* $Header: /cvsroot/nco/nco/src/nco/ncap.c,v 1.236 2010/04/16 21:04:18 zender Exp $ */

/* ncap -- netCDF arithmetic processor */

/* Purpose: Compute user-defined derived fields using forward algebraic notation applied to netCDF files */

/* Copyright (C) 1995--2010 Charlie Zender

   License: GNU General Public License (GPL) Version 3
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
   ncap -O -D 1 -S ${HOME}/nco/data/ncap.in ${HOME}/nco/data/in.nc ${HOME}/nco/data/foo.nc
   ncap -O -D 1 -s a=b+c -s "b=c-d/2." -S ncap.in in.nc ~/foo.nc
   ncap -O -D 1 -s two=one+two in.nc ~/foo.nc
   scp ~/nco/src/nco/ncap.c esmf.ess.uci.edu:nco/src/nco/ncap.c */

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard C headers */
#include <assert.h>  /* assert() debugging macro */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp. . . */
#include <sys/stat.h> /* stat() */
#include <time.h> /* machine time */
#include <unistd.h> /* all sorts of POSIX stuff */
/* GNU getopt() is independent system header on FREEBSD, LINUX, LINUXALPHA, LINUXAMD, LINUXARM, WIN32
   AT&T getopt() is in unistd.h or stdlib.h on AIX, CRAY, NECSX, SUNMP, SUN4SOL2
   fxm: I'm not sure what ALPHA and SGI do */
#ifndef HAVE_GETOPT_LONG
#include "nco_getopt.h"
#else /* !NEED_GETOPT_LONG */ 
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif /* !HAVE_GETOPT_H */ 
#endif /* HAVE_GETOPT_LONG */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h"  /* NCO wrappers for libnetcdf.a */

/* Personal headers */
/* #define MAIN_PROGRAM_FILE MUST precede #include libnco.h */
#define MAIN_PROGRAM_FILE
#include "ncap.h" /* netCDF arithmetic processor-specific definitions (symbol table, ...) */
#include "libnco.h" /* netCDF Operator (NCO) library */

/* Global variables */
size_t ncap_ncl_dpt_crr=0UL; /* [nbr] Depth of current #include file (incremented in ncap_lex.l) */
size_t *ncap_ln_nbr_crr; /* [cnt] Line number (incremented in ncap_lex.l) */
char **ncap_fl_spt_glb=NULL; /* [fl] Script file */

void 
glb_init_free /* [fnc] Initialize and free global variables (line numbers and include stuff) */
(nco_bool action); /* I [flg] Initialize */

int 
main(int argc,char **argv)
{
  extern int nco_yyparse(prs_sct *prs_arg); /* Prototype here as in bison.simple to avoid compiler warning */
  /* Following declaration gets rid of implicit declaration compiler warning
     It is a condensation of the lexer declaration from lex.yy.c:
     YY_BUFFER_STATE yy_scan_string YY_PROTO(( yyconst char *yy_str )); */
  extern int nco_yy_scan_string(const char *);
  
  extern FILE *nco_yyin; /* [fl] Input script file */

  /* fxm TODO nco652 */
  double rnd_nbr(double);
  
  nco_bool CNV_CCM_CCSM_CF;
  nco_bool EXCLUDE_INPUT_LIST=False; /* Option c */
  nco_bool EXTRACT_ALL_COORDINATES=False; /* Option c */
  nco_bool EXTRACT_ASSOCIATED_COORDINATES=True; /* Option C */
  nco_bool FILE_RETRIEVED_FROM_REMOTE_LOCATION;
  nco_bool FL_LST_IN_FROM_STDIN=False; /* [flg] fl_lst_in comes from stdin */
  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=False; /* Option O */
  nco_bool FORTRAN_IDX_CNV=False; /* Option F */
  nco_bool HISTORY_APPEND=True; /* Option h */
  nco_bool PRN_FNC_TBL=False; /* Option f */  
  nco_bool PROCESS_ALL_VARS=True; /* Option v */  
  nco_bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */
  nco_bool flg_cln=False; /* [flg] Clean memory prior to exit */

  char **fl_lst_abb=NULL; /* Option n */
  char **fl_lst_in;
  char **var_lst_in=NULL_CEWI;
  char *cmd_ln;
  char *fl_in=NULL;
  char *fl_out=NULL; /* Option o */
  char *fl_out_tmp;
  char *fl_pth=NULL; /* Option p */
  char *fl_pth_lcl=NULL; /* Option l */
  char *fl_spt_usr=NULL; /* Option s */
  char *lmt_arg[NC_MAX_DIMS];
  char *opt_crr=NULL; /* [sng] String representation of current long-option name */
#define NCAP_SPT_NBR_MAX 100
  char *spt_arg[NCAP_SPT_NBR_MAX]; /* fxm: Arbitrary size, should be dynamic */
  char *spt_arg_cat=NULL; /* [sng] User-specified script */

  const char * const CVS_Id="$Id: ncap.c,v 1.236 2010/04/16 21:04:18 zender Exp $"; 
  const char * const CVS_Revision="$Revision: 1.236 $";
  const char * const opt_sht_lst="346ACcD:FfhL:l:n:Oo:p:Rrs:S:vx-:"; /* [sng] Single letter command line options */

#if defined(__cplusplus) || defined(PGI_CC)
  ddra_info_sct ddra_info;
  ddra_info.flg_ddra=False;
#else /* !__cplusplus */
  ddra_info_sct ddra_info={.flg_ddra=False};
#endif /* !__cplusplus */

  dmn_sct **dmn_in=NULL_CEWI;  /* [lst] Dimensions in input file */
  dmn_sct **dmn_out=NULL_CEWI; /* [lst] Dimensions written to output file */
  dmn_sct **dmn_new=NULL_CEWI; /* [lst] Temporary dimensions to reduce referencing */

  extern char *optarg;
  extern int optind;
  
  /* Math float prototypes required by AIX, Solaris, but not by Linux, IRIX */
  /* Basic math: acos, asin, atan, cos, exp, fabs, log, log10, sin, sqrt, tan */
  
  /* GNU g++ barfs at these float declartions -- remove if g++ used */
#ifndef __GNUG__
  extern float acosf(float);
  extern float asinf(float);
  extern float atanf(float);
  extern float cosf(float);
  extern float expf(float);
  extern float fabsf(float); /* 20040629: Only AIX may need this */
  extern float logf(float);
  extern float log10f(float);
  extern float rnd_nbrf(float);
  extern float sinf(float);
  extern float sqrtf(float);
  extern float tanf(float);
  
  /* Advanced math: erf, erfc, gamma */
  extern float erff(float);
  extern float erfcf(float);
  extern float gammaf(float);

  /* Hyperbolic trigonometric: acosh, asinh, atanh, cosh, sinh, tanh */
  extern float acoshf(float);
  extern float asinhf(float);
  extern float atanhf(float);
  extern float coshf(float);
  extern float sinhf(float);
  extern float tanhf(float);
  
  /* Basic Rounding: ceil, floor */
  extern float ceilf(float);
  extern float floorf(float);
  
  /* Advanced Rounding: nearbyint, rint, round, trunc */
  extern float nearbyintf(float);
  extern float rintf(float);
  extern float roundf(float);
  extern float truncf(float);
#endif

  int abb_arg_nbr=0;
  int cnk_nbr=0; /* [nbr] Number of chunk sizes */
  int dfl_lvl=0; /* [enm] Deflate level */
  int fl_nbr=0;
  int fl_in_fmt; /* [enm] Input file format */
  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int fll_md_old; /* [enm] Old fill mode */
  int idx;
  int in_id;  
  int jdx;
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int nbr_att=0; /* [nbr] Number of attributes in script */
  int nbr_dmn_ass=int_CEWI;/* Number of dimensions in temporary list */
  int nbr_dmn_in=int_CEWI; /* Number of dimensions in dim_in */
  int nbr_dmn_out=0; /* [nbr] Number of dimensions in list dmn_out */
  int nbr_lst_a=0; /* size of xtr_lst_a */
  int nbr_spt=0; /* Option s. NB: nbr_spt gets incremented */
  int nbr_var_fix; /* nbr_var_fix gets incremented */
  int nbr_var_fl;/* number of vars in a file */
  int nbr_var_prc; /* nbr_var_prc gets incremented */
  int nbr_var_ycc=0; /* [nbr] Number of vars to be defined after 1st parse */
  int nbr_xtr=0; /* nbr_xtr will not otherwise be set for -c with no -v */
  int opt;
  int out_id;  
  int rcd=NC_NOERR; /* [rcd] Return code */
  int var_id;
  
  sym_sct **sym_tbl; /* [fnc] Symbol table for functions */
  int sym_tbl_nbr; /* [nbr] Size of symbol table */
  int sym_idx=0; /* [idx] Counter for symbols */
  
  lmt_sct **lmt=NULL_CEWI;
  
  nm_id_sct *dmn_lst=NULL;
  nm_id_sct *xtr_lst=NULL; /* Non-processed variables to copy to OUTPUT */
  nm_id_sct *xtr_lst_a=NULL; /* Initialize to ALL variables in OUTPUT file */
  
  size_t sng_lng;
  size_t spt_arg_lng=size_t_CEWI;
  
  var_sct **var;
  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_out;
  var_sct **var_prc;
  var_sct **var_prc_out;
  var_sct **var_ycc=NULL;  

  aed_sct **att_lst=NULL;
  
  prs_sct prs_arg; /* [sct] Global information required in parser routines */
  
  static struct option opt_lng[]=
    { /* Structure ordered by short option key if possible */
      /* Long options with no argument, no short option counterpart */
      {"cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
      {"clean",no_argument,0,0}, /* [flg] Clean memory prior to exit */
      {"mmr_cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
      {"drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
      {"dirty",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
      {"mmr_drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
      {"version",no_argument,0,0},
      {"vrs",no_argument,0,0},
      /* Long options with argument, no short option counterpart */
      {"fl_fmt",required_argument,0,0},
      {"file_format",required_argument,0,0},
      /* Long options with short counterparts */
      {"3",no_argument,0,'3'},
      {"4",no_argument,0,'4'},
      {"64bit",no_argument,0,'4'},
      {"netcdf4",no_argument,0,'4'},
      {"append",no_argument,0,'A'},
      {"coords",no_argument,0,'c'},
      {"crd",no_argument,0,'c'},
      {"no-coords",no_argument,0,'C'},
      {"no-crd",no_argument,0,'C'},
      {"debug",required_argument,0,'D'},
      {"dbg_lvl",required_argument,0,'D'},
      {"fnc_tbl",no_argument,0,'f'},
      {"prn_fnc_tbl",no_argument,0,'f'},
      {"ftn",no_argument,0,'F'},
      {"history",no_argument,0,'h'},
      {"hst",no_argument,0,'h'},
      {"dfl_lvl",required_argument,0,'L'}, /* [enm] Deflate level */
      {"deflate",required_argument,0,'L'}, /* [enm] Deflate level */
      {"local",required_argument,0,'l'},
      {"lcl",required_argument,0,'l'},
      {"nintap",required_argument,0,'n'},
      {"overwrite",no_argument,0,'O'},
      {"ovr",no_argument,0,'O'},
      {"output",required_argument,0,'o'},
      {"fl_out",required_argument,0,'o'},
      {"path",required_argument,0,'p'},
      {"retain",no_argument,0,'R'},
      {"rtn",no_argument,0,'R'},
      {"revision",no_argument,0,'r'},
      {"file",required_argument,0,'S'},
      {"script-file",required_argument,0,'S'},
      {"fl_spt",required_argument,0,'S'},
      {"spt",required_argument,0,'s'},
      {"script",required_argument,0,'s'},
      {"units",no_argument,0,'u'},
      {"variable",no_argument,0,'v'},
      {"exclude",no_argument,0,'x'},
      {"xcl",no_argument,0,'x'},
      {"help",no_argument,0,'?'},
      {0,0,0,0}
    }; /* end opt_lng */
  int opt_idx=0; /* Index of current long option into opt_lng array */
  
  /* Start timer and save command line */ 
  ddra_info.tmr_flg=nco_tmr_srt;
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
  ddra_info.tmr_flg=nco_tmr_mtd;
  cmd_ln=nco_cmd_ln_sng(argc,argv);
  
  /* Get program name and set program enum (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);
  
  /* Parse command line arguments */
  while(1){
    /* getopt_long_only() allows one dash to prefix long options */
    opt=getopt_long(argc,argv,opt_sht_lst,opt_lng,&opt_idx);
    /* NB: access to opt_crr is only valid when long_opt is detected */
    if(opt == EOF) break; /* Parse positional arguments once getopt_long() returns EOF */
    opt_crr=(char *)strdup(opt_lng[opt_idx].name);

    /* Process long options without short option counterparts */
    if(opt == 0){
      if(!strcmp(opt_crr,"cln") || !strcmp(opt_crr,"mmr_cln") || !strcmp(opt_crr,"clean")) flg_cln=True; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"drt") || !strcmp(opt_crr,"mmr_drt") || !strcmp(opt_crr,"dirty")) flg_cln=False; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"fl_fmt") || !strcmp(opt_crr,"file_format")) rcd=nco_create_mode_prs(optarg,&fl_out_fmt);
      if(!strcmp(opt_crr,"vrs") || !strcmp(opt_crr,"version")){
	(void)nco_vrs_prn(CVS_Id,CVS_Revision);
	nco_exit(EXIT_SUCCESS);
      } /* endif "vrs" */
    } /* opt != 0 */
    /* Process short options */
    switch(opt){
    case 0: /* Long options have already been processed, return */
      break;
    case '3': /* Request netCDF3 output storage format */
      fl_out_fmt=NC_FORMAT_CLASSIC;
      break;
    case '4': /* Catch-all to prescribe output storage format */
      if(!strcmp(opt_crr,"64bit")) fl_out_fmt=NC_FORMAT_64BIT; else fl_out_fmt=NC_FORMAT_NETCDF4; 
      break;
    case '6': /* Request netCDF3 64-bit offset output storage format */
      fl_out_fmt=NC_FORMAT_64BIT;
      break;
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'C': /* Extract all coordinates associated with extracted variables? */
      EXTRACT_ASSOCIATED_COORDINATES=False;
      break;
    case 'c':
      EXTRACT_ALL_COORDINATES=True;
      break;
    case 'D': /* Debugging level. Default is 0. */
      dbg_lvl=(unsigned short)strtol(optarg,(char **)NULL,10);
      break;
    case 'd': /* Copy limit argument for later processing */
      lmt_arg[lmt_nbr]=(char *)strdup(optarg);
      lmt_nbr++;
      break;
    case 'F': /* Toggle index convention. Default is 0-based arrays (C-style). */
      FORTRAN_IDX_CNV=!FORTRAN_IDX_CNV;
      break;
    case 'f': /* Print function table */
      PRN_FNC_TBL=True;
      break;
    case 'h': /* Toggle appending to history global attribute */
      HISTORY_APPEND=!HISTORY_APPEND;
      break;
    case 'L': /* [enm] Deflate level. Default is 0. */
      dfl_lvl=(int)strtol(optarg,(char **)NULL,10);
      break;
    case 'l': /* Local path prefix for files retrieved from remote file system */
      fl_pth_lcl=(char *)strdup(optarg);
      break;
    case 'n': /* NINTAP-style abbreviation of files to process */
      /* Currently not used in ncap but should be to allow processing multiple input files by same script */
      (void)fprintf(stderr,"%s: ERROR %s does not currently implement -n option\n",prg_nm_get(),prg_nm_get());
      fl_lst_abb=nco_lst_prs_2D(optarg,",",&abb_arg_nbr);
      if(abb_arg_nbr < 1 || abb_arg_nbr > 3){
	(void)fprintf(stderr,"%s: ERROR Incorrect abbreviation for file list\n",prg_nm);
	(void)nco_usg_prn();
	nco_exit(EXIT_FAILURE);
      } /* end if */
      break;
    case 'O': /* Toggle FORCE_OVERWRITE */
      FORCE_OVERWRITE=!FORCE_OVERWRITE;
      break;
    case 'o': /* Name of output file */
      fl_out=(char *)strdup(optarg);
      break;
    case 'p': /* Common file path */
      fl_pth=(char *)strdup(optarg);
      break;
    case 'R': /* Toggle removal of remotely-retrieved-files. Default is True. */
      REMOVE_REMOTE_FILES_AFTER_PROCESSING=!REMOVE_REMOTE_FILES_AFTER_PROCESSING;
      break;
    case 'r': /* Print CVS program information and copyright notice */
      (void)nco_vrs_prn(CVS_Id,CVS_Revision);
      (void)nco_lbr_vrs_prn();
      (void)nco_cpy_prn();
      (void)nco_cnf_prn();
      nco_exit(EXIT_SUCCESS);
      break;
    case 's': /* Copy command script for later processing */
      spt_arg[nbr_spt++]=(char *)strdup(optarg);
      if(nbr_spt == NCAP_SPT_NBR_MAX-1) (void)fprintf(stderr,"%s: WARNING No more than %d script arguments allowed. TODO #24\n",prg_nm_get(),NCAP_SPT_NBR_MAX);
      break;
    case 'S': /* Read command script from file rather than from command line */
      fl_spt_usr=(char *)strdup(optarg);
      break;
    case 'v': /* Variables to extract/exclude */
      PROCESS_ALL_VARS=False;
      nbr_xtr=0;
      break;
    case 'x': /* Exclude rather than extract variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
      if(EXCLUDE_INPUT_LIST) (void)fprintf(stderr,"%s: ERROR %s does not currently implement -x option\n",prg_nm_get(),prg_nm_get());
      nco_exit(EXIT_FAILURE);
      break;
    case '?': /* Print proper usage */
      (void)nco_usg_prn();
      nco_exit(EXIT_SUCCESS);
      break;
    case '-': /* Long options are not allowed */
      (void)fprintf(stderr,"%s: ERROR Long options are not available in this build. Use single letter options instead.\n",prg_nm_get());
      nco_exit(EXIT_FAILURE);
      break;
    default: /* Print proper usage */
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
      break;
    } /* end switch */
    if(opt_crr) opt_crr=(char *)nco_free(opt_crr);
  } /* end while loop */
  
  /* Append ";\n" to command-script arguments, then concatenate them */
  for(idx=0;idx<nbr_spt;idx++){
    sng_lng=strlen(spt_arg[idx]);
    if(idx == 0){
      spt_arg_cat=(char *)nco_malloc(sng_lng+3);
      strcpy(spt_arg_cat,spt_arg[idx]);
      strcat(spt_arg_cat,";\n");
      spt_arg_lng=sng_lng+3;
    }else{
      spt_arg_lng+=sng_lng+2;
      spt_arg_cat=(char *)nco_realloc(spt_arg_cat,spt_arg_lng);
      strcat(spt_arg_cat,spt_arg[idx]);
      strcat(spt_arg_cat,";\n");
    } /* end else */
  } /* end if */    
  
  /* Create function table */
  sym_tbl_nbr= /* fxm: Make this dynamic */
    +12 /* Basic math: acos, asin, atan, cos, exp, fabs, log, log10, rnd_nbr, sin, sqrt, tan */
    +1 /* Basic math synonyms: ln */
    +6 /* Hyperbolic trigonometric: acosh, asinh, atanh, cosh, sinh, tanh */
    +2 /* Basic Rounding: ceil, floor */
    +4 /* Advanced Rounding: nearbyint, rint, round, trunc */
    +3 /* Advanced math: erf, erfc, gamma */
    ;
  sym_tbl=(sym_sct **)nco_malloc(sizeof(sym_sct *)*sym_tbl_nbr);
  
  /* Basic math: acos, asin, atan, cos, exp, log, log10, rnd_nbr, sin, sqrt, tan */
  sym_tbl[sym_idx++]=ncap_sym_init("acos",acos,acosf);  
  sym_tbl[sym_idx++]=ncap_sym_init("asin",asin,asinf);
  sym_tbl[sym_idx++]=ncap_sym_init("atan",atan,atanf);
  sym_tbl[sym_idx++]=ncap_sym_init("cos",cos,cosf);  
  sym_tbl[sym_idx++]=ncap_sym_init("exp",exp,expf);
  sym_tbl[sym_idx++]=ncap_sym_init("fabs",fabs,fabsf);
  sym_tbl[sym_idx++]=ncap_sym_init("log",log,logf);
  sym_tbl[sym_idx++]=ncap_sym_init("log10",log10,log10f);
  sym_tbl[sym_idx++]=ncap_sym_init("rnd_nbr",rnd_nbr,rnd_nbrf);
  sym_tbl[sym_idx++]=ncap_sym_init("sin",sin,sinf);
  sym_tbl[sym_idx++]=ncap_sym_init("sqrt",sqrt,sqrtf);
  sym_tbl[sym_idx++]=ncap_sym_init("tan",tan,tanf);

  /* Basic math synonyms: ln */
  sym_tbl[sym_idx++]=ncap_sym_init("ln",log,logf); /* ln() is synonym for log() */
  
  /* Basic Rounding: ceil, floor */
  sym_tbl[sym_idx++]=ncap_sym_init("ceil",ceil,ceilf); /* Round up to nearest integer */
  sym_tbl[sym_idx++]=ncap_sym_init("floor",floor,floorf); /* Round down to nearest integer */
  
  /* fxm: Change whole function symbol table section to autotools format #if HAVE_ERF ... */

  /* Advanced math: erf, erfc, gamma
     LINUX*, MACOSX*, and SUN* provide these functions with C89
     20020122 and 20020422: AIX, CRAY, SGI*, WIN32 do not define erff(), erfcf(), gammaf() with C89
     20050610: C99 mandates support for erf(), erfc(), tgamma()
     Eventually users without C99 will forego ncap */
#if defined(LINUX) || defined(LINUXAMD64)  || defined(MACOSX)
  sym_tbl[sym_idx++]=ncap_sym_init("erf",erf,erff);
  sym_tbl[sym_idx++]=ncap_sym_init("erfc",erfc,erfcf);
  sym_tbl[sym_idx++]=ncap_sym_init("gamma",tgamma,tgammaf);
#else /* !LINUX */
  sym_tbl_nbr-=3; /* Advanced math: erf, erfc, gamma */
#endif /* !LINUX */

  /* Hyperbolic trigonometric: acosh, asinh, atanh, cosh, sinh, tanh
     20020703: AIX, SGI*, WIN32 do not define acoshf, asinhf, atanhf
     20050610: C99 mandates support for acosh(), asinh(), atanh(), cosh(), sinh(), tanh()
     Eventually users without C99 will forego ncap */
#if defined(LINUX) || defined(LINUXAMD64)
  sym_tbl[sym_idx++]=ncap_sym_init("acosh",acosh,acoshf);
  sym_tbl[sym_idx++]=ncap_sym_init("asinh",asinh,asinhf);
  sym_tbl[sym_idx++]=ncap_sym_init("atanh",atanh,atanhf);
  sym_tbl[sym_idx++]=ncap_sym_init("cosh",cosh,coshf);
  sym_tbl[sym_idx++]=ncap_sym_init("sinh",sinh,sinhf);
  sym_tbl[sym_idx++]=ncap_sym_init("tanh",tanh,tanhf);
#else /* !LINUX */
  sym_tbl_nbr-=6; /* Hyperbolic trigonometric: acosh, asinh, atanh, cosh, sinh, tanh */
#endif /* !LINUX */
  
  /* 20020703: AIX, MACOSX, SGI*, WIN32 do not define rintf
     Only LINUX* supplies all of these and I do not care about them enough
     to activate them on LINUX* but not on MACOSX* and SUN* */
  sym_tbl_nbr-=4; /* Advanced Rounding: nearbyint, rint, round, trunc */
  /* Advanced Rounding: nearbyint, round, trunc */
  /* sym_tbl[sym_idx++]=ncap_sym_init("nearbyint",nearbyint,nearbyintf); *//* Round to integer value in floating point format using current rounding direction, do not raise inexact exceptions */
  /* sym_tbl[sym_idx++]=ncap_sym_init("round",round,roundf); *//* Round to nearest integer away from zero */
  /* sym_tbl[sym_idx++]=ncap_sym_init("trunc",trunc,truncf); *//* Round to nearest integer not larger in absolute value */
  /* sym_tbl[sym_idx++]=ncap_sym_init("rint",rint,rintf); *//* Round to integer value in floating point format using current rounding direction, raise inexact exceptions */
  assert(sym_idx == sym_tbl_nbr);
  
  if(PRN_FNC_TBL){
    /* ncap TODO #43: alphabetize this list */ 
    (void)fprintf(stdout,"Maths functions available in %s:\n",prg_nm_get());
    (void)fprintf(stdout,"Name\tFloat\tDouble\n");          
    for(idx=0;idx<sym_tbl_nbr;idx++)
      (void)fprintf(stdout,"%s\t%c\t%c\n",sym_tbl[idx]->nm,(sym_tbl[idx]->fnc_flt ? 'y' : 'n'),(sym_tbl[idx]->fnc_dbl ? 'y' : 'n'));
    nco_exit(EXIT_SUCCESS);
  } /* end if PRN_FNC_TBL */
  
  /* Process positional arguments and fill in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&fl_nbr,&fl_out,&FL_LST_IN_FROM_STDIN);
  
  /* Make uniform list of user-specified dimension limits */
  if(lmt_nbr > 0) lmt=nco_lmt_prs(lmt_nbr,lmt_arg);
  
  /* Parse filename */
  fl_in=nco_fl_nm_prs(fl_in,0,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);
  /* Open file for reading */
  rcd=nco_open(fl_in,NC_NOWRITE,&in_id);
  (void)nco_inq_format(in_id,&fl_in_fmt);
  
  /* Form list of all dimensions in file */  
  dmn_lst=nco_dmn_lst(in_id,&nbr_dmn_in);
  
  dmn_in=(dmn_sct **)nco_malloc(nbr_dmn_in*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_in;idx++) dmn_in[idx]=nco_dmn_fll(in_id,dmn_lst[idx].id,dmn_lst[idx].nm);
  
  /* Merge hyperslab limit information into dimension structures */
  if(lmt_nbr > 0) (void)nco_dmn_lmt_mrg(dmn_in,nbr_dmn_in,lmt,lmt_nbr);
  
  /* Make output and input files consanguinous */
  if(fl_out_fmt == NCO_FORMAT_UNDEFINED) fl_out_fmt=fl_in_fmt;

  /* Verify output file format supports requested actions */
  (void)nco_fl_fmt_vet(fl_out_fmt,cnk_nbr,dfl_lvl);

  /* Open output file */
  fl_out_tmp=nco_fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&out_id);
  
  /* Copy global attributes */
  (void)nco_att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL,(nco_bool)True);
  
  /* Catenate time-stamped command line to "history" global attribute */
  if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);
  
  (void)nco_enddef(out_id);

  /* Set arguments for  script execution */
  prs_arg.fl_in=fl_in; /* [sng] Input data file */
  prs_arg.in_id=in_id; /* [id] Input data file ID */
  prs_arg.fl_out=fl_out; /* [sng] Output data file */
  prs_arg.out_id=out_id; /* [id] Output data file ID */
  prs_arg.att_lst=&att_lst; /* [sct] Attributes in script */
  prs_arg.nbr_att=&nbr_att; /* [nbr] Number of attributes in script */
  prs_arg.dmn_in=dmn_in; /* [dmn_in] List of all dimensions in input */
  prs_arg.nbr_dmn_in=nbr_dmn_in; /* [nbr] Number of  dimensions in input */
  prs_arg.dmn_out=&dmn_out;     /* pointer to list of dims in output */
  prs_arg.nbr_dmn_out=&nbr_dmn_out; /* number of dims in above list */
  prs_arg.sym_tbl=sym_tbl; /* [fnc] Symbol table for functions */
  prs_arg.sym_tbl_nbr=sym_tbl_nbr; /* [nbr] Number of functions in table */
  /* prs_arg.ntl_scn=False;   [flg] Initial scan of script */
  prs_arg.var_LHS=NULL; /* [var] LHS cast variable */
  prs_arg.var_lst=&var_ycc; /* [sct] Variables to be defined after 1st parse */
  prs_arg.nbr_var=&nbr_var_ycc; /* [nbr] Number of vars to be defined after 1st parse */
  prs_arg.nco_op_typ=nco_op_nil; /* [enm] Operation type */
  prs_arg.dfl_lvl=dfl_lvl; /* [enm] Deflate level */
  /* Timestamp end of metadata setup and disk layout */
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
  ddra_info.tmr_flg=nco_tmr_rgl;

  /* Parse twice: 1st parse defines variable dimensions in output file. 
     2nd parse initializes variable values */
  for(jdx=0;jdx<2;jdx++){
    
    prs_arg.ntl_scn=(jdx==0 ? True : False);
    
    if(fl_spt_usr == NULL){
      /* No script file specified, look for command-line scripts */
      if(nbr_spt == 0){
	(void)fprintf(stderr,"%s: ERROR no script file or command line scripts specified\n",prg_nm_get());
	(void)fprintf(stderr,"%s: HINT Use, e.g., -s \"foo=bar\"\n",prg_nm_get());
	nco_exit(EXIT_FAILURE);
      } /* end if */
      
      /* Print all command-line scripts */
      if(dbg_lvl_get() > nco_dbg_scl){
	for(idx=0;idx<nbr_spt;idx++) (void)fprintf(stderr,"spt_arg[%d] = %s\n",idx,spt_arg[idx]);
      } /* endif debug */
      
      /* Parse command line scripts */
      fl_spt_usr=(char *)strdup("Command-line script");
      nco_yy_scan_string(spt_arg_cat);
      
    }else{ /* ...endif command-line scripts, begin script file... */
      /* Open script file for reading */
      if((nco_yyin=fopen(fl_spt_usr,"r")) == NULL){
	(void)fprintf(stderr,"%s: ERROR Unable to open script file %s\n",prg_nm_get(),fl_spt_usr);
	nco_exit(EXIT_FAILURE);
      } /* end if */
    } /* end else script file */
    
    /* Initialize global variables */
    (void)glb_init_free(True); 
    ncap_fl_spt_glb[ncap_ncl_dpt_crr]=fl_spt_usr;
    
    /* Invoke parser */
    rcd=nco_yyparse(&prs_arg);
    
    /* Tidy up */  
    if(nbr_spt > 0) fl_spt_usr=(char*)nco_free(fl_spt_usr);
    (void)glb_init_free(False); 
    
    if(!prs_arg.ntl_scn) continue;
    
    (void)nco_redef(out_id);
    for(idx=0;idx<nbr_var_ycc;idx++){
      /* Define variables in output */
      /* Kill variables classified as undefined */
      if(dbg_lvl >= nco_dbg_var) (void)fprintf(stdout,"%s: Checking var_ycc[%d]->undefined for variable %s...\n",prg_nm_get(),idx,var_ycc[idx]->nm);
      if(var_ycc[idx]->undefined){
	/* 20060225: TODO nco680 free() list at end or risk double-free()'ing*/
	var_ycc[idx]=nco_var_free(var_ycc[idx]); 
	continue;
      } /* endif */
      (void)nco_def_var(out_id,var_ycc[idx]->nm,var_ycc[idx]->type,var_ycc[idx]->nbr_dim,var_ycc[idx]->dmn_id,&var_id);
      /* Set HDF Lempel-Ziv compression level, if requested */
      if(dfl_lvl > 0 && var_ycc[idx]->nbr_dim > 0) (void)nco_def_var_deflate(out_id,var_id,(int)True,(int)True,dfl_lvl);    
      var_ycc[idx]->val.vp=nco_free(var_ycc[idx]->val.vp);
    } /* end loop over idx */
    (void)nco_enddef(out_id);
    
  } /* end loop over jdx */
  
  /* Get number of variables in output file */
  rcd=nco_inq(out_id,(int *)NULL,&nbr_var_fl,(int *)NULL,(int*)NULL);
  
  /* Make list of all new variables in output_file */  
  xtr_lst_a=nco_var_lst_mk(out_id,nbr_var_fl,var_lst_in,False,False,&nbr_lst_a);
  
  if(PROCESS_ALL_VARS){
    /* Get number of variables in input file */
    rcd=nco_inq(in_id,(int *)NULL,&nbr_var_fl,(int *)NULL,(int *)NULL);
    
    /* Form initial list of all variables in input file */
    xtr_lst=nco_var_lst_mk(in_id,nbr_var_fl,var_lst_in,False,False,&nbr_xtr);
  }else{
    /* Make list of variables of new attributes whose parent variable is only in input file */
    xtr_lst=nco_att_lst_mk(in_id,out_id,att_lst,nbr_att,&nbr_xtr);
  } /* endif */
  
    /* Find dimensions associated with xtr_lst */
    /* Write to O only new dims
       Add apropriate coordinate variables to extraction list 
       options -c      -process all cordinates 
       i.e., add coordinates to var list 
       Also add their dims
       
       options --none   -process associated co-ords
       loop though dim_out and append to var list
       
       options -C         no co-ordinates   Do nothing */
  
    /* Subtract list A again */
    /* Finally extract variables on list */
  
  /* Subtract list A */
  if(nbr_lst_a > 0) xtr_lst=nco_var_lst_sub(xtr_lst,&nbr_xtr,xtr_lst_a,nbr_lst_a);
  
  /* Put file in define mode to allow metadata writing */
  (void)nco_redef(out_id);
  
  /* Free current list of all dimensions in input file */
  dmn_lst=nco_nm_id_lst_free(dmn_lst,nbr_dmn_in);

  /* Make list of dimensions of variables in xtr_lst */
  if(nbr_xtr > 0) dmn_lst=nco_dmn_lst_ass_var(in_id,xtr_lst,nbr_xtr,&nbr_dmn_ass);
  
  /* Find and add any new dimensions to output */
  for(idx=0;idx<nbr_dmn_ass;idx++)
    for(jdx=0;jdx<nbr_dmn_in;jdx++){
      /* If dimension is in list and is not yet defined */
      if(!strcmp(dmn_lst[idx].nm,dmn_in[jdx]->nm) && !dmn_in[jdx]->xrf){     
	/* Add dimension to output list dmn_prc */
	dmn_new=nco_dmn_out_grow(&prs_arg);
	*dmn_new=nco_dmn_dpl(dmn_in[jdx]);
	(void)nco_dmn_xrf(*dmn_new,dmn_in[jdx]);
	/* Write dimension to output */
	(void)nco_dmn_dfn(fl_out,out_id,dmn_new,1);
	break;
      } /* endif */
    } /* end loop over jdx */
  
  /* Free current list of all dimensions in input file */
  dmn_lst=nco_nm_id_lst_free(dmn_lst,nbr_dmn_ass);

  /* Dimensions for manually specified extracted variables are now defined in output file
     Add coordinate variables to extraction list
     If EXTRACT_ALL_COORDINATES then write associated dimension to output */
  if(EXTRACT_ASSOCIATED_COORDINATES){
    for(idx=0;idx<nbr_dmn_in;idx++){
      if(!dmn_in[idx]->is_crd_dmn) continue;
      
      if(EXTRACT_ALL_COORDINATES && !dmn_in[idx]->xrf){
	/* Add dimensions to output list dmn_out */
	dmn_new=nco_dmn_out_grow(&prs_arg);
	*dmn_new=nco_dmn_dpl(dmn_in[idx]);
	(void)nco_dmn_xrf(*dmn_new,dmn_in[idx]);
	/* Write dimension to output */
	(void)nco_dmn_dfn(fl_out,out_id,dmn_new,1);
      } /* end if */
      /* Add coordinate variable to extraction list, dimension has already been output */
      if(dmn_in[idx]->xrf){
	for(jdx=0;jdx<nbr_xtr;jdx++)
	  if(!strcmp(xtr_lst[jdx].nm,dmn_in[idx]->nm)) break;

	if(jdx != nbr_xtr) continue;
	/* If coordinate is not on list then add it to extraction list */
	xtr_lst=(nm_id_sct *)nco_realloc(xtr_lst,(nbr_xtr+1)*sizeof(nm_id_sct));
	xtr_lst[nbr_xtr].nm=(char *)strdup(dmn_in[idx]->nm);
	xtr_lst[nbr_xtr++].id=dmn_in[idx]->cid;
      } /* endif */
    } /* end loop over idx */	      
  } /* end if */ 
  
  /* Is this an CCM/CCSM/CF-format history tape? */
  CNV_CCM_CCSM_CF=nco_cnv_ccm_ccsm_cf_inq(in_id);

  /* Add coordinates defined by CF convention */
  if(CNV_CCM_CCSM_CF && (EXTRACT_ALL_COORDINATES || EXTRACT_ASSOCIATED_COORDINATES)) xtr_lst=nco_cnv_cf_crd_add(in_id,xtr_lst,&nbr_xtr);

  /* Subtract list A again (it may contain re-defined coordinates) */
  if(nbr_xtr > 0) xtr_lst=nco_var_lst_sub(xtr_lst,&nbr_xtr,xtr_lst_a,nbr_lst_a);
  
  /* Sort extraction list for faster I/O */
  if(nbr_xtr > 1) xtr_lst=nco_lst_srt_nm_id(xtr_lst,nbr_xtr,False);
  
  /* Write "fixed" variables */
  var=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));
  var_out=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));
  for(idx=0;idx<nbr_xtr;idx++){
    var[idx]=nco_var_fll(in_id,xtr_lst[idx].id,xtr_lst[idx].nm,dmn_in,nbr_dmn_in);
    var_out[idx]=nco_var_dpl(var[idx]);
    (void)nco_xrf_var(var[idx],var_out[idx]);
    (void)nco_xrf_dmn(var_out[idx]);
  } /* end loop over idx */

  /* NB: ncap is not well-suited for nco_var_lst_dvd() */
  /* Divide variable lists into lists of fixed variables and variables to be processed */
  (void)nco_var_lst_dvd(var,var_out,nbr_xtr,CNV_CCM_CCSM_CF,nco_pck_plc_nil,nco_pck_map_nil,(dmn_sct **)NULL,(int)0,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc,&var_prc_out,&nbr_var_prc);
  
  /* csz: Why not call this with var_fix? */
  /* Define non-processed vars */
  (void)nco_var_dfn(in_id,fl_out,out_id,var_out,nbr_xtr,(dmn_sct **)NULL,(int)0,nco_pck_plc_nil,nco_pck_map_nil,dfl_lvl);
  
  /* Write out new attributes possibly overwriting old ones */
  for(idx=0;idx<nbr_att;idx++){
    /* NB: These attributes should probably be written prior to last data mode */
    rcd=nco_inq_varid_flg(out_id,att_lst[idx]->var_nm,&var_id);
    if(rcd != NC_NOERR) continue;
    att_lst[idx]->mode=aed_overwrite;
    (void)nco_aed_prc(out_id,var_id,*att_lst[idx]);
  } /* end for */
  
  /* Turn off default filling behavior to enhance efficiency */
  rcd=nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
  
  /* Take output file out of define mode */
  (void)nco_enddef(out_id);
  
  /* Copy non-processed vars */
  (void)nco_var_val_cpy(in_id,out_id,var_fix,nbr_var_fix);
  
  /* Close input netCDF file */
  rcd=nco_close(in_id);
  
  /* Remove local copy of file */
  if(FILE_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)nco_fl_rm(fl_in);
  
  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);
  
  /* Clean memory unless dirty memory allowed */
  if(flg_cln){
    /* ncap-specific memory */
    /* fxm: ncap-specific memory freeing instructions go here */
    for(idx=0;idx<sym_tbl_nbr;idx++){
      sym_tbl[idx]->nm=(char*)nco_free(sym_tbl[idx]->nm);
      sym_tbl[idx]=(sym_sct *)nco_free(sym_tbl[idx]);
    } /* end loop */
    sym_tbl=(sym_sct **)nco_free(sym_tbl);
    if(fl_spt_usr) fl_spt_usr=(char *)nco_free(fl_spt_usr);
    
    /* Free variable list: some in var_ycc may have been previously free()'d */
    /* fxm: TODO nco680 */
    if(nbr_var_ycc > 0) var_ycc=nco_var_lst_free(var_ycc,nbr_var_ycc);
    
    /* Free attribute list */  
    for(idx=0;idx<nbr_att;idx++){
      att_lst[idx]->att_nm=(char *)nco_free(att_lst[idx]->att_nm);
      att_lst[idx]->var_nm=(char *)nco_free(att_lst[idx]->var_nm);
      att_lst[idx]->val.vp=(void *)nco_free(att_lst[idx]->val.vp);
      att_lst[idx]=(aed_sct *)nco_free(att_lst[idx]);
    } /* end loop */
    if(nbr_att >0 ) att_lst=(aed_sct **)nco_free(att_lst);
    
    /* Free extraction lists */ 
    xtr_lst=nco_nm_id_lst_free(xtr_lst,nbr_xtr);
    xtr_lst_a=nco_nm_id_lst_free(xtr_lst_a,nbr_lst_a);
    
    /* Free command line algebraic arguments, if any */
    for(idx=0;idx<nbr_spt;idx++) spt_arg[idx]=(char *)nco_free(spt_arg[idx]);
    if(spt_arg_cat) spt_arg_cat=(char *)nco_free(spt_arg_cat);
    
    /* bad things happen with this line 
       if(yyin !=NULL) yyin=(FILE *)nco_free(yyin); */
    
    /* NCO-generic clean-up */
    /* Free individual strings/arrays */
    if(cmd_ln) cmd_ln=(char *)nco_free(cmd_ln);
    if(fl_in) fl_in=(char*)nco_free(fl_in);
    if(fl_out) fl_out=(char *)nco_free(fl_out);
    if(fl_out_tmp) fl_out_tmp=(char *)nco_free(fl_out_tmp);
    if(fl_pth) fl_pth=(char *)nco_free(fl_pth);
    if(fl_pth_lcl) fl_pth_lcl=(char *)nco_free(fl_pth_lcl);
    /* Free lists of strings */
    if(fl_lst_in && fl_lst_abb == NULL) fl_lst_in=nco_sng_lst_free(fl_lst_in,fl_nbr); 
    if(fl_lst_in && fl_lst_abb) fl_lst_in=nco_sng_lst_free(fl_lst_in,1);
    if(fl_lst_abb) fl_lst_abb=nco_sng_lst_free(fl_lst_abb,abb_arg_nbr);
    /* Free limits */
    for(idx=0;idx<lmt_nbr;idx++) lmt_arg[idx]=(char *)nco_free(lmt_arg[idx]);
    if(lmt_nbr > 0) lmt=nco_lmt_lst_free(lmt,lmt_nbr);
    /* Free dimension lists */
    if(nbr_dmn_in > 0) dmn_in=nco_dmn_lst_free(dmn_in,nbr_dmn_in);
    if(nbr_dmn_out > 0) dmn_out=nco_dmn_lst_free(dmn_out,nbr_dmn_out);
    /* Free variable lists */
    if(nbr_xtr > 0) var=nco_var_lst_free(var,nbr_xtr);
    if(nbr_xtr > 0) var_out=nco_var_lst_free(var_out,nbr_xtr);
    var_prc=(var_sct **)nco_free(var_prc);
    var_prc_out=(var_sct **)nco_free(var_prc_out);
    var_fix=(var_sct **)nco_free(var_fix);
    var_fix_out=(var_sct **)nco_free(var_fix_out);
  } /* !flg_cln */
  
  /* End timer */ 
  ddra_info.tmr_flg=nco_tmr_end; /* [enm] Timer flag */
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);

  if(rcd != NC_NOERR) nco_err_exit(rcd,"main");
  nco_exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */

void
glb_init_free /* [fnc] Initialize and free global variables (line numbers and include stuff) */
(nco_bool action) /* I [flg] Initialize */
{
  /* Purpose: Initialize and free global variables (line numbers and include stuff) */
  
  if(action){
    ncap_ncl_dpt_crr=0UL; 
    ncap_ln_nbr_crr=(size_t *)nco_realloc(ncap_ln_nbr_crr,(ncap_ncl_dpt_crr+1UL)*sizeof(size_t)); 
    ncap_ln_nbr_crr[ncap_ncl_dpt_crr]=1UL; 
    ncap_fl_spt_glb=(char **)nco_realloc(ncap_fl_spt_glb,(ncap_ncl_dpt_crr+1UL)*sizeof(char *)); 
  } /* endif action */
  
  if(!action){
    ncap_ncl_dpt_crr=0UL; 
    ncap_ln_nbr_crr=(size_t *)nco_free(ncap_ln_nbr_crr);
    ncap_fl_spt_glb=(char **)nco_free(ncap_fl_spt_glb);
  } /* endif not action */
  
} /* end glb_init_free() */
