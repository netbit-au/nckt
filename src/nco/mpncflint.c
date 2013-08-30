/* $Header: /cvsroot/nco/nco/src/nco/mpncflint.c,v 1.73 2010/01/26 13:06:25 zender Exp $ */

/* mpncflint -- netCDF file interpolator */

/* Purpose: Linearly interpolate a third netCDF file from two input files */

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
   ncflint -O -D 2 in.nc in.nc ~/foo.nc
   ncflint -O -i lcl_time_hr,9.0 -v lcl_time_hr /data/zender/arese/crm/951030_0800_arese_crm.nc /data/zender/arese/crm/951030_1100_arese_crm.nc ~/foo.nc; ncks -H foo.nc
   ncflint -O -w 0.66666,0.33333 -v lcl_time_hr /data/zender/arese/crm/951030_0800_arese_crm.nc /data/zender/arese/crm/951030_1100_arese_crm.nc ~/foo.nc; ncks -H foo.nc
   ncflint -O -w 0.66666 -v lcl_time_hr /data/zender/arese/crm/951030_0800_arese_crm.nc /data/zender/arese/crm/951030_1100_arese_crm.nc ~/foo.nc; ncks -H foo.nc

   ncdiff -O foo.nc /data/zender/arese/crm/951030_0900_arese_crm.nc foo2.nc;ncks -H foo2.nc | m */

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard C headers */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp. . . */
#include <sys/stat.h> /* stat() */
#include <time.h> /* machine time */
#include <unistd.h> /* all sorts of POSIX stuff */
#ifndef HAVE_GETOPT_LONG
#include "nco_getopt.h"
#else /* !NEED_GETOPT_LONG */ 
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif /* !HAVE_GETOPT_H */ 
#endif /* HAVE_GETOPT_LONG */

/* 3rd party vendors */
#ifdef ENABLE_MPI
#include <mpi.h> /* MPI definitions */
#include "nco_mpi.h" /* MPI utilities */
#endif /* !ENABLE_MPI */
#include <netcdf.h> /* netCDF definitions and C library */
/* #define MAIN_PROGRAM_FILE MUST precede #include libnco.h */
#define MAIN_PROGRAM_FILE
#include "libnco.h" /* netCDF Operator (NCO) library */

int 
main(int argc,char **argv)
{
  nco_bool CNV_CCM_CCSM_CF;
  nco_bool CMD_LN_NTP_VAR=False; /* Option i */
  nco_bool CMD_LN_NTP_WGT=True; /* Option w */
  nco_bool DO_CONFORM=False; /* Did nco_var_cnf_dmn() find truly conforming variables? */
  nco_bool EXCLUDE_INPUT_LIST=False; /* Option c */
  nco_bool EXTRACT_ALL_COORDINATES=False; /* Option c */
  nco_bool EXTRACT_ASSOCIATED_COORDINATES=True; /* Option C */
  nco_bool FILE_1_RETRIEVED_FROM_REMOTE_LOCATION;
  nco_bool FILE_2_RETRIEVED_FROM_REMOTE_LOCATION;
  nco_bool FL_LST_IN_FROM_STDIN=False; /* [flg] fl_lst_in comes from stdin */
  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=False; /* Option O */
  nco_bool FORTRAN_IDX_CNV=False; /* Option F */
  nco_bool HISTORY_APPEND=True; /* Option h */
  nco_bool MUST_CONFORM=False; /* Must nco_var_cnf_dmn() find truly conforming variables? */
  nco_bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */
  nco_bool flg_cln=False; /* [flg] Clean memory prior to exit */
  
  char **fl_lst_abb=NULL; /* Option a */
  char **fl_lst_in;
  char **ntp_lst_in;
  char **var_lst_in=NULL_CEWI;
  char *cmd_ln;
  char *fl_in_1=NULL; /* fl_in_1 is nco_realloc'd when not NULL */
  char *fl_in_2=NULL; /* fl_in_2 is nco_realloc'd when not NULL */
  char *fl_out=NULL; /* Option o */
  char *fl_out_tmp=NULL; /* MPI CEWI */
  char *fl_pth=NULL; /* Option p */
  char *fl_pth_lcl=NULL; /* Option l */
  char *lmt_arg[NC_MAX_DIMS];
  char *ntp_nm=NULL; /* Option i */
  char *opt_crr=NULL; /* [sng] String representation of current long-option name */
  char *optarg_lcl=NULL; /* [sng] Local copy of system optarg */
  
  const char * const CVS_Id="$Id: mpncflint.c,v 1.73 2010/01/26 13:06:25 zender Exp $"; 
  const char * const CVS_Revision="$Revision: 1.73 $";
  const char * const opt_sht_lst="346ACcD:d:Fhi:L:l:Oo:p:rRSt:v:xw:-:";
  
  dmn_sct **dim;
  dmn_sct **dmn_out;
  
  double ntp_val_out=double_CEWI; /* Option i */
  double wgt_val_1=0.5; /* Option w */
  double wgt_val_2=0.5; /* Option w */
  
  extern char *optarg;
  extern int optind;
  
  /* Using naked stdin/stdout/stderr in parallel region generates warning
     Copy appropriate filehandle to variable scoped shared in parallel clause */
  FILE * const fp_stderr=stderr; /* [fl] stderr filehandle CEWI */
  FILE * const fp_stdout=stdout; /* [fl] stdout filehandle CEWI */
  
  int *in_id_1_arr;
  int *in_id_2_arr;

  int abb_arg_nbr=0;
  int dfl_lvl=0; /* [enm] Deflate level */
  int fl_idx;
  int fl_nbr=0;
  int fl_in_fmt_1; /* [enm] Input file format */
  int fl_in_fmt_2; /* [enm] Input file format */
  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int fll_md_old; /* [enm] Old fill mode */
  int has_mss_val=False;
  int idx;
  int in_id_1;  
  int in_id_2;  
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int nbr_dmn_fl;
  int nbr_dmn_xtr;
  int nbr_ntp;
  int nbr_var_fix; /* nbr_var_fix gets incremented */
  int nbr_var_fl;
  int nbr_var_prc; /* nbr_var_prc gets incremented */
  int nbr_xtr=0; /* nbr_xtr won't otherwise be set for -c with no -v */
  int opt;
  int out_id;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int thr_idx; /* [idx] Index of current thread */
  int thr_nbr=int_CEWI; /* [nbr] Thread number Option t */
  int var_lst_in_nbr=0;
  
  lmt_sct **lmt;
  
  nm_id_sct *dmn_lst;
  nm_id_sct *xtr_lst=NULL; /* xtr_lst may be alloc()'d from NULL with -c option */
  
  
  val_unn val_gnr_unn; /* Generic container for arrival point or weight */
  
  var_sct *wgt_1=NULL_CEWI;
  var_sct *wgt_2=NULL_CEWI;
  var_sct *wgt_out_1=NULL;
  var_sct *wgt_out_2=NULL;
  var_sct **var;
  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_out;
  var_sct **var_prc_1;
  var_sct **var_prc_2;
  var_sct **var_prc_out;
  
#ifdef ENABLE_MPI
  /* Declare all MPI-specific variables here */
  MPI_Status mpi_stt; /* [enm] Status check to decode msg_tag_typ */

  nco_bool TKN_WRT_FREE=True; /* [flg] Write-access to output file is available */
  
  const double tkn_wrt_rqs_ntv=0.04; /* [s] Token request interval */
  
  const int msg_bfr_lng=3; /* [nbr] Number of elements in msg_bfr */
  const int wrk_id_bfr_lng=1; /* [nbr] Number of elements in wrk_id_bfr */

  int fl_nm_lng; /* [nbr] Output file name length */
  int msg_bfr[msg_bfr_lng]; /* [bfr] Buffer containing var, idx, tkn_wrt_rsp */
  int msg_tag_typ; /* [enm] MPI message tag type */
  int prc_rnk; /* [idx] Process rank */
  int prc_nbr=0; /* [nbr] Number of MPI processes */
  int tkn_wrt_rsp; /* [enm] Response to request for write token */
  int var_wrt_nbr=0; /* [nbr] Variables written to output file until now */
  int rnk_wrk; /* [idx] Worker rank */
  int wrk_id_bfr[wrk_id_bfr_lng]; /* [bfr] Buffer for rnk_wrk */
#endif /* !ENABLE_MPI */

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
      {"dimension",required_argument,0,'d'},
      {"dmn",required_argument,0,'d'},
      {"fortran",no_argument,0,'F'},
      {"ftn",no_argument,0,'F'},
      {"history",no_argument,0,'h'},
      {"hst",no_argument,0,'h'},
      {"interpolate",required_argument,0,'i'},
      {"ntp",required_argument,0,'i'},
      {"dfl_lvl",required_argument,0,'L'}, /* [enm] Deflate level */
      {"deflate",required_argument,0,'L'}, /* [enm] Deflate level */
      {"local",required_argument,0,'l'},
      {"lcl",required_argument,0,'l'},
      {"overwrite",no_argument,0,'O'},
      {"ovr",no_argument,0,'O'},
      {"output",required_argument,0,'o'},
      {"fl_out",required_argument,0,'o'},
      {"path",required_argument,0,'p'},
      {"retain",no_argument,0,'R'},
      {"rtn",no_argument,0,'R'},
      {"revision",no_argument,0,'r'},
      {"suspend", no_argument,0,'S'},
      {"thr_nbr",required_argument,0,'t'},
      {"variable",required_argument,0,'v'},
      {"weight",required_argument,0,'w'},
      {"wgt_var",no_argument,0,'w'},
      {"help",no_argument,0,'?'},
      {0,0,0,0}
    }; /* end opt_lng */
  int opt_idx=0; /* Index of current long option into opt_lng array */
  
#ifdef ENABLE_MPI
  /* MPI Initialization */
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&prc_nbr);
  MPI_Comm_rank(MPI_COMM_WORLD,&prc_rnk);
#endif /* !ENABLE_MPI */
  
  /* Start clock and save command line */ 
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
    case 'D': /* The debugging level. Default is 0. */
      dbg_lvl=(unsigned short)strtol(optarg,(char **)NULL,10);
      break;
    case 'd': /* Copy limit argument for later processing */
      lmt_arg[lmt_nbr]=(char *)strdup(optarg);
      lmt_nbr++;
      break;
    case 'F': /* Toggle index convention. Default is 0-based arrays (C-style). */
      FORTRAN_IDX_CNV=!FORTRAN_IDX_CNV;
      break;
    case 'h': /* Toggle appending to history global attribute */
      HISTORY_APPEND=!HISTORY_APPEND;
      break;
    case 'i':
      /* Name of variable to guide interpolation. Default is none */
      ntp_lst_in=nco_lst_prs_2D(optarg,",",&nbr_ntp);
      if(nbr_ntp > 2){
	(void)fprintf(stdout,"%s: ERROR too many arguments to -i\n",prg_nm_get());
	(void)nco_usg_prn();
	nco_exit(EXIT_FAILURE);
      } /* end if */
      ntp_nm=ntp_lst_in[0];
      ntp_val_out=strtod(ntp_lst_in[1],(char **)NULL);
      CMD_LN_NTP_VAR=True;
      CMD_LN_NTP_WGT=False;
      break;
    case 'L': /* [enm] Deflate level. Default is 0. */
      dfl_lvl=(int)strtol(optarg,(char **)NULL,10);
      break;
    case 'l': /* Local path prefix for files retrieved from remote file system */
      fl_pth_lcl=(char *)strdup(optarg);
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
#ifdef ENABLE_MPI
    case 'S': /* Suspend with signal handler to facilitate debugging */
      if(signal(SIGUSR1,nco_cnt_run) == SIG_ERR) (void)fprintf(fp_stdout,"%s: ERROR Could not install suspend handler.\n",prg_nm);
      while(!nco_spn_lck_brk) usleep(nco_spn_lck_us); /* Spinlock. fxm: should probably insert a sched_yield */
      break;
#endif /* !ENABLE_MPI */
    case 't': /* Thread number */
      thr_nbr=(int)strtol(optarg,(char **)NULL,10);
      break;
    case 'v': /* Variables to extract/exclude */
      /* Replace commas with hashes when within braces (convert back later) */
      optarg_lcl=(char *)strdup(optarg);
      (void)nco_lst_comma2hash(optarg_lcl);
      var_lst_in=nco_lst_prs_2D(optarg_lcl,",",&var_lst_in_nbr);
      optarg_lcl=(char *)nco_free(optarg_lcl);
      nbr_xtr=var_lst_in_nbr;
      break;
    case 'w':
      /* Weight(s) for interpolation.  Default is wgt_val_1=wgt_val_2=0.5 */
      ntp_lst_in=nco_lst_prs_2D(optarg,",",&nbr_ntp);
      if(nbr_ntp > 2){
	(void)fprintf(stdout,"%s: ERROR too many arguments to -w\n",prg_nm_get());
	(void)nco_usg_prn();
	nco_exit(EXIT_FAILURE);
      }else if(nbr_ntp == 2){
	wgt_val_1=strtod(ntp_lst_in[0],(char **)NULL);
	wgt_val_2=strtod(ntp_lst_in[1],(char **)NULL);
      }else if(nbr_ntp == 1){
	wgt_val_1=strtod(ntp_lst_in[0],(char **)NULL);
	wgt_val_2=1.0-wgt_val_1;
      } /* end else */
      CMD_LN_NTP_WGT=True;
      break;
    case 'x': /* Exclude rather than extract variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
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
  
  if(CMD_LN_NTP_VAR && CMD_LN_NTP_WGT){
    (void)fprintf(stdout,"%s: ERROR interpolating variable (-i) and fixed weight(s) (-w) both set\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  }else if(!CMD_LN_NTP_VAR && !CMD_LN_NTP_WGT){
    (void)fprintf(stdout,"%s: ERROR interpolating variable (-i) or fixed weight(s) (-w) must be set\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* end else */
  
  /* Process positional arguments and fill in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&fl_nbr,&fl_out,&FL_LST_IN_FROM_STDIN);
  
  /* Make uniform list of user-specified dimension limits */
  lmt=nco_lmt_prs(lmt_nbr,lmt_arg);
    
  /* Initialize thread information */
  thr_nbr=nco_openmp_ini(thr_nbr);
  in_id_1_arr=(int *)nco_malloc(thr_nbr*sizeof(int));
  in_id_2_arr=(int *)nco_malloc(thr_nbr*sizeof(int));

  /* Parse filenames */
  fl_idx=0; /* Input file _1 */
  fl_in_1=nco_fl_nm_prs(fl_in_1,fl_idx,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  if(dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"\nInput file %d is %s; ",fl_idx,fl_in_1);
  /* Make sure file is on local system and is readable or die trying */
  fl_in_1=nco_fl_mk_lcl(fl_in_1,fl_pth_lcl,&FILE_1_RETRIEVED_FROM_REMOTE_LOCATION);
  if(dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"local file %s:\n",fl_in_1);
  /* Open file once per thread to improve caching */
  for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) rcd=nco_open(fl_in_1,NC_NOWRITE,in_id_1_arr+thr_idx);
  in_id_1=in_id_1_arr[0];

  fl_idx=1; /* Input file _2 */
  fl_in_2=nco_fl_nm_prs(fl_in_2,fl_idx,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  if(dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"\nInput file %d is %s; ",fl_idx,fl_in_2);
  /* Make sure file is on local system and is readable or die trying */
  fl_in_2=nco_fl_mk_lcl(fl_in_2,fl_pth_lcl,&FILE_2_RETRIEVED_FROM_REMOTE_LOCATION);
  if(dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"local file %s:\n",fl_in_2);
  /* Open file once per thread to improve caching */
  for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) rcd=nco_open(fl_in_2,NC_NOWRITE,in_id_2_arr+thr_idx);
  in_id_2=in_id_2_arr[0];
  
  /* Get number of variables and dimensions in file */
  (void)nco_inq(in_id_1,&nbr_dmn_fl,&nbr_var_fl,(int *)NULL,(int *)NULL);
  (void)nco_inq_format(in_id_1,&fl_in_fmt_1);
  (void)nco_inq_format(in_id_2,&fl_in_fmt_2);
  
  /* Form initial extraction list which may include extended regular expressions */
  xtr_lst=nco_var_lst_mk(in_id_1,nbr_var_fl,var_lst_in,EXCLUDE_INPUT_LIST,EXTRACT_ALL_COORDINATES,&nbr_xtr);
  
  /* Change included variables to excluded variables */
  if(EXCLUDE_INPUT_LIST) xtr_lst=nco_var_lst_xcl(in_id_1,nbr_var_fl,xtr_lst,&nbr_xtr);
  
  /* Is this an CCM/CCSM/CF-format history tape? */
  CNV_CCM_CCSM_CF=nco_cnv_ccm_ccsm_cf_inq(in_id_1);

  /* Add all coordinate variables to extraction list */
  if(EXTRACT_ALL_COORDINATES) xtr_lst=nco_var_lst_crd_add(in_id_1,nbr_dmn_fl,nbr_var_fl,xtr_lst,&nbr_xtr,CNV_CCM_CCSM_CF);
  
  /* Make sure coordinates associated extracted variables are also on extraction list */
  if(EXTRACT_ASSOCIATED_COORDINATES) xtr_lst=nco_var_lst_crd_ass_add(in_id_1,xtr_lst,&nbr_xtr,CNV_CCM_CCSM_CF);
  
  /* Sort extraction list by variable ID for fastest I/O */
  if(nbr_xtr > 1) xtr_lst=nco_lst_srt_nm_id(xtr_lst,nbr_xtr,False);
  
  /* We now have final list of variables to extract. Phew. */
  
  /* Find coordinate/dimension values associated with user-specified limits
     NB: nco_lmt_evl() with same nc_id contains OpenMP critical region */
  for(idx=0;idx<lmt_nbr;idx++) (void)nco_lmt_evl(in_id_1,lmt[idx],0L,FORTRAN_IDX_CNV);
  
  /* Find dimensions associated with variables to be extracted */
  dmn_lst=nco_dmn_lst_ass_var(in_id_1,xtr_lst,nbr_xtr,&nbr_dmn_xtr);
  
  /* Fill in dimension structure for all extracted dimensions */
  dim=(dmn_sct **)nco_malloc(nbr_dmn_xtr*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_xtr;idx++) dim[idx]=nco_dmn_fll(in_id_1,dmn_lst[idx].id,dmn_lst[idx].nm);
  /* Dimension list no longer needed */
  dmn_lst=nco_nm_id_lst_free(dmn_lst,nbr_dmn_xtr);
  
  /* Merge hyperslab limit information into dimension structures */
  if(lmt_nbr > 0) (void)nco_dmn_lmt_mrg(dim,nbr_dmn_xtr,lmt,lmt_nbr);
  
  /* Duplicate input dimension structures for output dimension structures */
  dmn_out=(dmn_sct **)nco_malloc(nbr_dmn_xtr*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_xtr;idx++){
    dmn_out[idx]=nco_dmn_dpl(dim[idx]);
    (void)nco_dmn_xrf(dim[idx],dmn_out[idx]); 
  } /* end loop over idx */
  
  /* Fill in variable structure list for all extracted variables */
  var=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));
  var_out=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));
  for(idx=0;idx<nbr_xtr;idx++){
    var[idx]=nco_var_fll(in_id_1,xtr_lst[idx].id,xtr_lst[idx].nm,dim,nbr_dmn_xtr);
    var_out[idx]=nco_var_dpl(var[idx]);
    (void)nco_xrf_var(var[idx],var_out[idx]);
    (void)nco_xrf_dmn(var_out[idx]);
  } /* end loop over idx */
  /* Extraction list no longer needed */
  xtr_lst=nco_nm_id_lst_free(xtr_lst,nbr_xtr);
  
  /* Divide variable lists into lists of fixed variables and variables to be processed */
  (void)nco_var_lst_dvd(var,var_out,nbr_xtr,CNV_CCM_CCSM_CF,nco_pck_plc_nil,nco_pck_map_nil,(dmn_sct **)NULL,0,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc_1,&var_prc_out,&nbr_var_prc);
  
  /* Assign zero-start and unity-stride vectors to output variables */
  (void)nco_var_srd_srt_set(var_out,nbr_xtr);
  
#ifdef ENABLE_MPI
  if(prc_rnk == rnk_mgr){ /* MPI manager code */
#endif /* !ENABLE_MPI */
    /* Make output and input files consanguinous */
    if(fl_out_fmt == NCO_FORMAT_UNDEFINED) fl_out_fmt=fl_in_fmt_1;

    /* Open output file */
    fl_out_tmp=nco_fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&out_id);
    
    /* Copy global attributes */
    (void)nco_att_cpy(in_id_1,out_id,NC_GLOBAL,NC_GLOBAL,(nco_bool)True);
    
    /* Catenate time-stamped command line to "history" global attribute */
    if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);
    
    if(thr_nbr > 0 && HISTORY_APPEND) (void)nco_thr_att_cat(out_id,thr_nbr);
    
#ifdef ENABLE_MPI
    /* Initialize MPI task information */
    if(prc_nbr > 0 && HISTORY_APPEND) (void)nco_mpi_att_cat(out_id,prc_nbr);
#endif /* !ENABLE_MPI */
    
    /* Define dimensions in output file */
    (void)nco_dmn_dfn(fl_out,out_id,dmn_out,nbr_dmn_xtr);
    
    /* Define variables in output file, copy their attributes */
    (void)nco_var_dfn(in_id_1,fl_out,out_id,var_out,nbr_xtr,(dmn_sct **)NULL,(int)0,nco_pck_plc_nil,nco_pck_map_nil,dfl_lvl);
    
    /* Turn off default filling behavior to enhance efficiency */
    rcd=nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
    
    /* Take output file out of define mode */
    (void)nco_enddef(out_id);
    
#ifdef ENABLE_MPI
  } /* prc_rnk != rnk_mgr */
  
  /* Manager obtains output filename and broadcasts to workers */
  if(prc_rnk == rnk_mgr) fl_nm_lng=(int)strlen(fl_out_tmp); 
  MPI_Bcast(&fl_nm_lng,1,MPI_INT,0,MPI_COMM_WORLD);
  if(prc_rnk != rnk_mgr) fl_out_tmp=(char *)malloc((fl_nm_lng+1)*sizeof(char));
  MPI_Bcast(fl_out_tmp,fl_nm_lng+1,MPI_CHAR,0,MPI_COMM_WORLD);
  
  if(prc_rnk == rnk_mgr){ /* MPI manager code */
    TKN_WRT_FREE=False;
#endif /* !ENABLE_MPI */
    /* Copy variable data for non-processed variables */
    (void)nco_var_val_cpy(in_id_1,out_id,var_fix,nbr_var_fix);
#ifdef ENABLE_MPI
    /* Close output file so workers can open it */
    nco_close(out_id);
    TKN_WRT_FREE=True;
  } /* prc_rnk != rnk_mgr */
#endif /* !ENABLE_MPI */
  
  /* Perform various error-checks on input file */
  if(False) (void)nco_fl_cmp_err_chk();
  
  /* ncflint-specific stuff: */
  /* Find the weighting variable in input file */
  if(CMD_LN_NTP_VAR){
    int ntp_id_1;
    int ntp_id_2;
    
    var_sct *ntp_1;
    var_sct *ntp_2;
    var_sct *ntp_var_out;
    
    /* Turn arrival point into pseudo-variable */
    val_gnr_unn.d=ntp_val_out; /* Generic container for arrival point or weight */
    ntp_var_out=scl_mk_var(val_gnr_unn,NC_DOUBLE);
    
    rcd=nco_inq_varid(in_id_1,ntp_nm,&ntp_id_1);
    rcd=nco_inq_varid(in_id_2,ntp_nm,&ntp_id_2);
    
    ntp_1=nco_var_fll(in_id_1,ntp_id_1,ntp_nm,dim,nbr_dmn_xtr);
    ntp_2=nco_var_fll(in_id_2,ntp_id_2,ntp_nm,dim,nbr_dmn_xtr);
    
    /* Currently, only support scalar variables */
    if(ntp_1->sz > 1 || ntp_2->sz > 1){
      (void)fprintf(stdout,"%s: ERROR interpolation variable %s must be scalar\n",prg_nm_get(),ntp_nm);
      nco_exit(EXIT_FAILURE);
    } /* end if */
    
    /* Retrieve interpolation variable */
    /* NB: nco_var_get() with same nc_id contains OpenMP critical region */
    (void)nco_var_get(in_id_1,ntp_1);
    (void)nco_var_get(in_id_2,ntp_2);
    
    /* Weights must be NC_DOUBLE */
    ntp_1=nco_var_cnf_typ((nc_type)NC_DOUBLE,ntp_1);
    ntp_2=nco_var_cnf_typ((nc_type)NC_DOUBLE,ntp_2);
    
    /* Check for degenerate case */
    if(ntp_1->val.dp[0] == ntp_2->val.dp[0]){
      (void)fprintf(stdout,"%s: ERROR Interpolation variable %s is identical (%g) in input files, therefore unable to interpolate.\n",prg_nm_get(),ntp_nm,ntp_1->val.dp[0]);
      nco_exit(EXIT_FAILURE);
    } /* end if */
    
    /* Turn weights into pseudo-variables */
    wgt_1=nco_var_dpl(ntp_2);
    wgt_2=nco_var_dpl(ntp_var_out);
    
    /* Subtract to find interpolation distances */
    (void)nco_var_sbt(ntp_1->type,ntp_1->sz,ntp_1->has_mss_val,ntp_1->mss_val,ntp_var_out->val,wgt_1->val);
    (void)nco_var_sbt(ntp_1->type,ntp_1->sz,ntp_1->has_mss_val,ntp_1->mss_val,ntp_1->val,wgt_2->val);
    (void)nco_var_sbt(ntp_1->type,ntp_1->sz,ntp_1->has_mss_val,ntp_1->mss_val,ntp_1->val,ntp_2->val);
    
    /* Normalize to obtain final interpolation weights */
    (void)nco_var_dvd(wgt_1->type,wgt_1->sz,wgt_1->has_mss_val,wgt_1->mss_val,ntp_2->val,wgt_1->val);
    (void)nco_var_dvd(wgt_2->type,wgt_2->sz,wgt_2->has_mss_val,wgt_2->mss_val,ntp_2->val,wgt_2->val);
    
    if(ntp_1) ntp_1=nco_var_free(ntp_1);
    if(ntp_2) ntp_2=nco_var_free(ntp_2);
    if(ntp_var_out) ntp_var_out=nco_var_free(ntp_var_out);
  } /* end if CMD_LN_NTP_VAR */
  
  if(CMD_LN_NTP_WGT){
    val_gnr_unn.d=wgt_val_1; /* Generic container for arrival point or weight */
    wgt_1=scl_mk_var(val_gnr_unn,NC_DOUBLE);
    val_gnr_unn.d=wgt_val_2; /* Generic container for arrival point or weight */
    wgt_2=scl_mk_var(val_gnr_unn,NC_DOUBLE);
  } /* end if CMD_LN_NTP_WGT */
  
  if(dbg_lvl > nco_dbg_scl) (void)fprintf(stderr,"wgt_1 = %g, wgt_2 = %g\n",wgt_1->val.dp[0],wgt_2->val.dp[0]);
  
  /* Create structure list for second file */
  var_prc_2=(var_sct **)nco_malloc(nbr_var_prc*sizeof(var_sct *));
  
  /* Loop over each interpolated variable */
#ifdef ENABLE_MPI
  if(prc_rnk == rnk_mgr){ /* MPI manager code */
    /* Compensate for incrementing on each worker's first message */
    var_wrt_nbr=-prc_nbr+1;
    idx=0;
    /* While variables remain to be processed or written... */
    while(var_wrt_nbr < nbr_var_prc){
      /* Receive message from any worker */
      MPI_Recv(wrk_id_bfr,wrk_id_bfr_lng,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&mpi_stt);
      /* Obtain MPI message tag type */
      msg_tag_typ=mpi_stt.MPI_TAG;
      /* Get sender's prc_rnk */
      rnk_wrk=wrk_id_bfr[0];
      
      /* Allocate next variable, if any, to worker */
      if(msg_tag_typ == msg_tag_wrk_rqs){
        var_wrt_nbr++; /* [nbr] Number of variables written */
        /* Worker closed output file before sending msg_tag_wrk_rqs */
        TKN_WRT_FREE=True;
	
        if(idx > nbr_var_prc-1){
          msg_bfr[0]=idx_all_wrk_ass; /* [enm] All variables already assigned */
          msg_bfr[1]=out_id; /* Output file ID */
        }else{
          /* Tell requesting worker to allocate space for next variable */
          msg_bfr[0]=idx; /* [idx] Variable to be processed */
          msg_bfr[1]=out_id; /* Output file ID */
          msg_bfr[2]=var_prc_out[idx]->id; /* [id] Variable ID in output file */
          /* Point to next variable on list */
          idx++;
        } /* endif idx */
        MPI_Send(msg_bfr,msg_bfr_lng,MPI_INT,rnk_wrk,msg_tag_wrk_rsp,MPI_COMM_WORLD);
        /* msg_tag_typ != msg_tag_wrk_rqs */
      }else if(msg_tag_typ == msg_tag_tkn_wrt_rqs){
        /* Allocate token if free, else ask worker to try later */
        if(TKN_WRT_FREE){
          TKN_WRT_FREE=False;
          msg_bfr[0]=tkn_wrt_rqs_xcp; /* Accept request for write token */
        }else{
          msg_bfr[0]=tkn_wrt_rqs_dny; /* Deny request for write token */
        } /* !TKN_WRT_FREE */
        MPI_Send(msg_bfr,msg_bfr_lng,MPI_INT,rnk_wrk,msg_tag_tkn_wrt_rsp,MPI_COMM_WORLD);
      } /* msg_tag_typ != msg_tag_tkn_wrt_rqs */
    } /* end while var_wrt_nbr < nbr_var_prc */
  }else{ /* prc_rnk != rnk_mgr, end Manager code begin Worker code */
    wrk_id_bfr[0]=prc_rnk;
    while(1){ /* While work remains... */
      /* Send msg_tag_wrk_rqs */
      wrk_id_bfr[0]=prc_rnk;
      MPI_Send(wrk_id_bfr,wrk_id_bfr_lng,MPI_INT,rnk_mgr,msg_tag_wrk_rqs,MPI_COMM_WORLD);
      /* Receive msg_tag_wrk_rsp */
      MPI_Recv(msg_bfr,msg_bfr_lng,MPI_INT,0,msg_tag_wrk_rsp,MPI_COMM_WORLD,&mpi_stt);
      idx=msg_bfr[0];
      out_id=msg_bfr[1];
      if(idx == idx_all_wrk_ass) break;
      else{
        var_prc_out[idx]->id=msg_bfr[2];
	/* Process this variable same as UP code */
#else /* !ENABLE_MPI */
#ifdef _OPENMP
	/* OpenMP notes:
	   shared(): msk and wgt are not altered within loop
	   private(): wgt_avg does not need initialization */
#pragma omp parallel for default(none) firstprivate(wgt_1,wgt_2,wgt_out_1,wgt_out_2) private(DO_CONFORM,idx,in_id_1,in_id_2,has_mss_val) shared(MUST_CONFORM,dbg_lvl,dim,fl_in_1,fl_in_2,fl_out,in_id_1_arr,in_id_2_arr,nbr_dmn_xtr,nbr_var_prc,out_id,prg_nm,var_prc_1,var_prc_2,var_prc_out)
#endif /* !_OPENMP */
	/* UP and SMP codes main loop over variables */
	for(idx=0;idx<nbr_var_prc;idx++){
#endif /* !ENABLE_MPI */
	  if(dbg_lvl >= nco_dbg_var) (void)fprintf(fp_stderr,"%s, ",var_prc_1[idx]->nm);
	  if(dbg_lvl >= nco_dbg_var) (void)fflush(fp_stderr);
	  
	  in_id_1=in_id_1_arr[omp_get_thread_num()];
	  in_id_2=in_id_2_arr[omp_get_thread_num()];

	  var_prc_2[idx]=nco_var_dpl(var_prc_1[idx]);
	  (void)nco_var_mtd_refresh(in_id_2,var_prc_2[idx]);
	  
	  /* NB: nco_var_get() with same nc_id contains OpenMP critical region */
	  (void)nco_var_get(in_id_1,var_prc_1[idx]);
	  (void)nco_var_get(in_id_2,var_prc_2[idx]);
	  
	  wgt_out_1=nco_var_cnf_dmn(var_prc_1[idx],wgt_1,wgt_out_1,MUST_CONFORM,&DO_CONFORM);
	  wgt_out_2=nco_var_cnf_dmn(var_prc_2[idx],wgt_2,wgt_out_2,MUST_CONFORM,&DO_CONFORM);
	  
	  var_prc_1[idx]=nco_var_cnf_typ((nc_type)NC_DOUBLE,var_prc_1[idx]);
	  var_prc_2[idx]=nco_var_cnf_typ((nc_type)NC_DOUBLE,var_prc_2[idx]);
	  
	  /* Allocate and, if necesssary, initialize space for processed variable */
	  var_prc_out[idx]->sz=var_prc_1[idx]->sz;
	  /* NB: must not try to free() same tally buffer twice */
	  /*    var_prc_out[idx]->tally=var_prc_1[idx]->tally=(long *)nco_malloc(var_prc_out[idx]->sz*sizeof(long));*/
	  var_prc_out[idx]->tally=(long *)nco_malloc(var_prc_out[idx]->sz*sizeof(long));
	  (void)nco_zero_long(var_prc_out[idx]->sz,var_prc_out[idx]->tally);
	  
	  /* Weight variable by taking product of weight with variable */
	  (void)nco_var_mlt(var_prc_1[idx]->type,var_prc_1[idx]->sz,var_prc_1[idx]->has_mss_val,var_prc_1[idx]->mss_val,wgt_out_1->val,var_prc_1[idx]->val);
	  (void)nco_var_mlt(var_prc_2[idx]->type,var_prc_2[idx]->sz,var_prc_2[idx]->has_mss_val,var_prc_2[idx]->mss_val,wgt_out_2->val,var_prc_2[idx]->val);
	  /* Change missing_value of var_prc_2, if any, to missing_value of var_prc_1, if any */
	  has_mss_val=nco_mss_val_cnf(var_prc_1[idx],var_prc_2[idx]);
	  /* NB: fxm: use tally to determine when to "unweight" answer? TODO  */
	  (void)nco_var_add_tll_ncflint(var_prc_1[idx]->type,var_prc_1[idx]->sz,has_mss_val,var_prc_1[idx]->mss_val,var_prc_out[idx]->tally,var_prc_1[idx]->val,var_prc_2[idx]->val);
	  
	  /* Re-cast output variable to original type */
	  var_prc_2[idx]=nco_var_cnf_typ(var_prc_out[idx]->type,var_prc_2[idx]);
	  
#ifdef ENABLE_MPI
	  /* Obtain token and prepare to write */
	  while(1){ /* Send msg_tag_tkn_wrt_rqs repeatedly until token obtained */
	    wrk_id_bfr[0]=prc_rnk;
	    MPI_Send(wrk_id_bfr,wrk_id_bfr_lng,MPI_INT,rnk_mgr,msg_tag_tkn_wrt_rqs,MPI_COMM_WORLD);
	    MPI_Recv(msg_bfr,msg_bfr_lng,MPI_INT,rnk_mgr,msg_tag_tkn_wrt_rsp,MPI_COMM_WORLD,&mpi_stt);
	    tkn_wrt_rsp=msg_bfr[0];
	    /* Wait then re-send request */
	    if(tkn_wrt_rsp == tkn_wrt_rqs_dny) sleep(tkn_wrt_rqs_ntv); else break;
	  } /* end while loop waiting for write token */
	  
	  /* Worker has token---prepare to write */
	  if(tkn_wrt_rsp == tkn_wrt_rqs_xcp){
	    rcd=nco_open(fl_out_tmp,NC_WRITE|NC_SHARE,&out_id);
	    /* Turn off default filling behavior to enhance efficiency */
	    rcd=nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
#else /* !ENABLE_MPI */
#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
#endif /* !ENABLE_MPI */
            /* Common code for UP, SMP, and MPI */
	    { /* begin OpenMP critical */
	      /* Copy interpolations to output file */
	      if(var_prc_out[idx]->nbr_dim == 0){
		(void)nco_put_var1(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_2[idx]->val.vp,var_prc_2[idx]->type);
	      }else{ /* end if variable is scalar */
		(void)nco_put_vara(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc_2[idx]->val.vp,var_prc_2[idx]->type);
	      } /* end else */
	    } /* end OpenMP critical */
	    
	    /* Free dynamically allocated buffers */
	    if(var_prc_1[idx]) var_prc_1[idx]=nco_var_free(var_prc_1[idx]);
	    if(var_prc_2[idx]) var_prc_2[idx]=nco_var_free(var_prc_2[idx]);
	    if(var_prc_out[idx]) var_prc_out[idx]=nco_var_free(var_prc_out[idx]);
	    
#ifdef ENABLE_MPI
	    /* Close output file and increment written counter */
	    nco_close(out_id);
	    var_wrt_nbr++;
	  } /* endif tkn_wrt_rqs_xcp */
	} /* end else !idx_all_wrk_ass */
      } /* end while loop requesting work/token */
    } /* endif Worker */
#else /* !ENABLE_MPI */
  } /* end (OpenMP parallel for) loop over idx */
#endif /* !ENABLE_MPI */
  
  if(dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"\n");
  
  /* Close input netCDF files */
  for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) nco_close(in_id_1_arr[thr_idx]);
  for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) nco_close(in_id_2_arr[thr_idx]);
  
#ifdef ENABLE_MPI
  /* Manager moves output file (closed by workers) from temporary to permanent location */
  if(prc_rnk == rnk_mgr) (void)nco_fl_mv(fl_out_tmp,fl_out);
#else /* !ENABLE_MPI */
  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);
#endif /* end !ENABLE_MPI */
  
  /* Remove local copy of file */
  if(FILE_1_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)nco_fl_rm(fl_in_1);
  if(FILE_2_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)nco_fl_rm(fl_in_2);
  
  /* Clean memory unless dirty memory allowed */
  if(flg_cln){
    /* ncflint-specific memory */
    if(fl_in_1) fl_in_1=(char *)nco_free(fl_in_1);
    if(fl_in_2) fl_in_2=(char *)nco_free(fl_in_2);
    var_prc_1=(var_sct **)nco_free(var_prc_1);
    var_prc_2=(var_sct **)nco_free(var_prc_2);
    if(wgt_1) wgt_1=(var_sct *)nco_var_free(wgt_1);
    if(wgt_2) wgt_2=(var_sct *)nco_var_free(wgt_2);
    if(wgt_out_1) wgt_out_1=(var_sct *)nco_var_free(wgt_out_1);
    if(wgt_out_2) wgt_out_2=(var_sct *)nco_var_free(wgt_out_2);
    
    /* NCO-generic clean-up */
    /* Free individual strings/arrays */
    if(cmd_ln) cmd_ln=(char *)nco_free(cmd_ln);
    if(fl_out) fl_out=(char *)nco_free(fl_out);
    if(fl_out_tmp) fl_out_tmp=(char *)nco_free(fl_out_tmp);
    if(fl_pth) fl_pth=(char *)nco_free(fl_pth);
    if(fl_pth_lcl) fl_pth_lcl=(char *)nco_free(fl_pth_lcl);
    
    /* Free lists of strings */
    if(fl_lst_in && fl_lst_abb == NULL) fl_lst_in=nco_sng_lst_free(fl_lst_in,fl_nbr); 
    if(fl_lst_in && fl_lst_abb) fl_lst_in=nco_sng_lst_free(fl_lst_in,1);
    if(fl_lst_abb) fl_lst_abb=nco_sng_lst_free(fl_lst_abb,abb_arg_nbr);
    if(var_lst_in_nbr > 0) var_lst_in=nco_sng_lst_free(var_lst_in,var_lst_in_nbr);
    /* Free limits */
    for(idx=0;idx<lmt_nbr;idx++) lmt_arg[idx]=(char *)nco_free(lmt_arg[idx]);
    if(lmt_nbr > 0) lmt=nco_lmt_lst_free(lmt,lmt_nbr);
    /* Free dimension lists */
    if(nbr_dmn_xtr > 0) dim=nco_dmn_lst_free(dim,nbr_dmn_xtr);
    if(nbr_dmn_xtr > 0) dmn_out=nco_dmn_lst_free(dmn_out,nbr_dmn_xtr);
    /* Free variable lists */
    /* ncflint free()s _prc variables at end of main loop */
    var=(var_sct **)nco_free(var);
    var_out=(var_sct **)nco_free(var_out);
    var_prc_out=(var_sct **)nco_free(var_prc_out);
    if(nbr_var_fix > 0) var_fix=nco_var_lst_free(var_fix,nbr_var_fix);
    if(nbr_var_fix > 0) var_fix_out=nco_var_lst_free(var_fix_out,nbr_var_fix);
  } /* !flg_cln */
  
#ifdef ENABLE_MPI
  MPI_Finalize();
#endif /* !ENABLE_MPI */
  
  if(rcd != NC_NOERR) nco_err_exit(rcd,"main");
  nco_exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */
