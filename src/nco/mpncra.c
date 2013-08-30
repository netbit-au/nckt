/* $Header: /cvsroot/nco/nco/src/nco/mpncra.c,v 1.98 2010/02/09 21:30:40 zender Exp $ */

/* This single source file may be called as three separate executables:
   ncra -- netCDF running averager
   ncea -- netCDF ensemble averager
   ncrcat -- netCDF record concatenator */

/* Purpose: Compute averages or extract series of specified hyperslabs of 
   specfied variables of multiple input netCDF files and output them 
   to a single file. */

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
   ncra -n 3,4,1 -p ${HOME}/nco/data h0001.nc ~/foo.nc
   ncra -n 3,4,1 -p ${HOME}/nco/data -l ${HOME} h0001.nc ~/foo.nc
   ncra -n 3,4,1 -p /ZENDER/tmp -l ${HOME}/nco/data h0001.nc ~/foo.nc
   scp ~/nco/src/nco/ncra.c esmf.ess.uci.edu:nco/src/nco
   
   ncea in.nc in.nc ~/foo.nc
   ncea -n 3,4,1 -p ${HOME}/nco/data h0001.nc ~/foo.nc
   ncea -n 3,4,1 -p ${HOME}/nco/data -l ${HOME} h0001.nc ~/foo.nc
   ncea -n 3,4,1 -p /ZENDER/tmp -l ${HOME} h0001.nc ~/foo.nc */

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard C headers */
#include <assert.h> /* assert() debugging macro */ 
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

/* Internationalization i18n, Linux Journal 200211 p. 57--59 */
#ifdef I18N
#include <libintl.h> /* Internationalization i18n */
#include <locale.h> /* Locale setlocale() */
#define _(sng) gettext (sng)
#define gettext_noop(sng) (sng)
#define N_(sng) gettext_noop(sng)
#endif /* I18N */
#ifndef _LIBINTL_H
# define gettext(foo) foo
#endif /* _LIBINTL_H */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#ifdef ENABLE_MPI
#include <mpi.h> /* MPI definitions */
#include "nco_mpi.h" /* MPI utilities */
#endif /* !ENABLE_MPI */

/* Personal headers */
/* #define MAIN_PROGRAM_FILE MUST precede #include libnco.h */
#define MAIN_PROGRAM_FILE
#include "libnco.h" /* netCDF Operator (NCO) library */

#ifdef ENABLE_MPI
void checkpointMpi(int prc_rnk, int stage){
  int msg[]={0,0};
  int rcd; /* [rcd] Return code */
  FILE * const fp_stderr=stderr; /* [fl] stderr filehandle CEWI */
  
  if(prc_rnk == rnk_mgr){
    msg[0]=stage;
    msg[1]=stage;
  } /* endif */
  (void)fprintf(fp_stderr,"%d checkpointing at stage %d\n",prc_rnk,stage);
  /* make everyone continue from this point. */
  rcd=MPI_Bcast(msg,2,MPI_INT,rnk_mgr,MPI_COMM_WORLD);
  if(prc_rnk != rnk_mgr) {
    /* basic sanity check */
    assert(msg[0] == stage);
    assert(msg[1] == stage);
  } /* end if */
} /* end checkpointMpi() */
#endif /* !ENABLE_MPI */

int 
main(int argc,char **argv)
{
  nco_bool CNV_ARM;
  nco_bool CNV_CCM_CCSM_CF;
  nco_bool EXCLUDE_INPUT_LIST=False; /* Option c */
  nco_bool EXTRACT_ALL_COORDINATES=False; /* Option c */
  nco_bool EXTRACT_ASSOCIATED_COORDINATES=True; /* Option C */
  nco_bool FILE_RETRIEVED_FROM_REMOTE_LOCATION;
  nco_bool FL_LST_IN_APPEND=True; /* Option H */
  nco_bool FL_LST_IN_FROM_STDIN=False; /* [flg] fl_lst_in comes from stdin */
  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=False; /* Option O */
  nco_bool FORTRAN_IDX_CNV=False; /* Option F */
  nco_bool HISTORY_APPEND=True; /* Option h */
  nco_bool LAST_RECORD=False;
  nco_bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */
  nco_bool flg_cln=False; /* [flg] Clean memory prior to exit */
  
  char **fl_lst_abb=NULL; /* Option n */
  char **fl_lst_in;
  char **var_lst_in=NULL_CEWI;
  char *cmd_ln;
  char *fl_in=NULL;
  char *fl_out=NULL; /* Option o */
  char *fl_out_tmp=NULL_CEWI;
  char *fl_pth=NULL; /* Option p */
  char *fl_pth_lcl=NULL; /* Option l */
  char *lmt_arg[NC_MAX_DIMS];
  char *nco_op_typ_sng=NULL_CEWI; /* [sng] Operation type Option y */
  char *nco_pck_plc_sng=NULL_CEWI; /* [sng] Packing policy Option P */
  char *opt_crr=NULL; /* [sng] String representation of current long-option name */
  char *optarg_lcl=NULL; /* [sng] Local copy of system optarg */
  
  const char * const CVS_Id="$Id: mpncra.c,v 1.98 2010/02/09 21:30:40 zender Exp $"; 
  const char * const CVS_Revision="$Revision: 1.98 $";
  const char * const opt_sht_lst="346ACcD:d:FHhL:l:n:Oo:p:P:rRSt:v:xY:y:-:";
  
  dmn_sct **dim;
  dmn_sct **dmn_out;
  
  extern char *optarg;
  extern int optind;
  
  /* Using naked stdin/stdout/stderr in parallel region generates warning
     Copy appropriate filehandle to variable scoped shared in parallel clause */
  FILE * const fp_stderr=stderr; /* [fl] stderr filehandle CEWI */
  
  int *in_id_arr;

  int abb_arg_nbr=0;
  int dfl_lvl=0; /* [enm] Deflate level */
  int fl_idx;
  int fl_nbr=0;
  int fl_in_fmt; /* [enm] Input file format */
  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int fll_md_old; /* [enm] Old fill mode */
  int idx=int_CEWI;
  int in_id;  
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int nbr_dmn_fl;
  int nbr_dmn_xtr;
  int nbr_var_fix; /* nbr_var_fix gets incremented */
  int nbr_var_fl;
  int nbr_var_prc; /* nbr_var_prc gets incremented */
  int nbr_xtr=0; /* nbr_xtr won't otherwise be set for -c with no -v */
  int nco_op_typ=nco_op_avg; /* [enm] Default operation is averaging */
  int nco_pck_plc=nco_pck_plc_nil; /* [enm] Default packing is none */
  int opt;
  int out_id;  
  int rcd=NC_NOERR; /* [rcd] Return code */
  int rec_dmn_id=NCO_REC_DMN_UNDEFINED;
  int thr_idx; /* [idx] Index of current thread */
  int thr_nbr=int_CEWI; /* [nbr] Thread number Option t */
  int var_lst_in_nbr=0;
  
  lmt_sct **lmt=NULL_CEWI;
  lmt_sct *lmt_rec=NULL_CEWI;
  
  long idx_rec; /* [idx] Index of current record in current input file */
  long idx_rec_out=0L; /* [idx] Index of current record in output file (0 is first, ...) */
  
  nco_int base_time_srt=nco_int_CEWI;
  nco_int base_time_crr=nco_int_CEWI;
  
  nm_id_sct *dmn_lst;
  nm_id_sct *xtr_lst=NULL; /* xtr_lst may be alloc()'d from NULL with -c option */
  
  
  var_sct **var;
  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_out=NULL_CEWI;
  var_sct **var_prc;
  var_sct **var_prc_out;
  
#ifdef ENABLE_MPI
  /* Declare all MPI-specific variables here */
  MPI_Status mpi_stt; /* [enm] Status check to decode msg_tag_typ */
  
  nco_bool TKN_WRT_FREE=True; /* [flg] Write-access to output file is available */
  
  int fl_nm_lng; /* [nbr] Output file name length */
  int msg_bfr[msg_bfr_lng]; /* [bfr] Buffer containing var, idx, tkn_wrt_rsp */
  int jdx=0; /* [idx] MPI index for local variables */
  int lcl_idx_lst[60]; /* [arr] Array containing indices of variables processed at each Worker */
  int lcl_nbr_var=0; /* [nbr] Count of variables processes at each Worker */
  int msg_tag_typ; /* [enm] MPI message tag type */
  int prc_rnk; /* [idx] Process rank */
  int prc_nbr=0; /* [nbr] Number of MPI processes */
  int tkn_wrt_rnk=0; /* [idx] Rank of process holding write token */
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
      {"fl_lst_in",no_argument,0,'H'},
      {"file_list",no_argument,0,'H'},
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
      {"pack",required_argument,0,'P'},
      {"retain",no_argument,0,'R'},
      {"rtn",no_argument,0,'R'},
      {"revision",no_argument,0,'r'},
      {"suspend", no_argument,0,'S'},
      {"thr_nbr",required_argument,0,'t'},
      {"threads",required_argument,0,'t'},
      {"omp_num_threads",required_argument,0,'t'},
      {"variable",required_argument,0,'v'},
      {"exclude",no_argument,0,'x'},
      {"xcl",no_argument,0,'x'},
      {"pseudonym",required_argument,0,'Y'},
      {"program",required_argument,0,'Y'},
      {"prg_nm",required_argument,0,'Y'},
      {"math",required_argument,0,'y'},
      {"help",no_argument,0,'?'},
      {0,0,0,0}
    }; /* end opt_lng */
  int opt_idx=0; /* Index of current long option into opt_lng array */
  
#ifdef _LIBINTL_H
  setlocale(LC_ALL,""); /* LC_ALL sets all localization tokens to same value */
  bindtextdomain("nco","/home/zender/share/locale"); /* ${LOCALEDIR} is e.g., /usr/share/locale */
  /* MO files should be in ${LOCALEDIR}/es/LC_MESSAGES */
  textdomain("nco"); /* PACKAGE is name of program */
#endif /* not _LIBINTL_H */
  
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
    case 'H': /* Toggle writing input file list attribute */
      FL_LST_IN_APPEND=!FL_LST_IN_APPEND;
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
    case 'n': /* NINTAP-style abbreviation of files to average */
      fl_lst_abb=nco_lst_prs_2D(optarg,",",&abb_arg_nbr);
      if(abb_arg_nbr < 1 || abb_arg_nbr > 5){
	(void)fprintf(stdout,gettext("%s: ERROR Incorrect abbreviation for file list\n"),prg_nm_get());
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
    case 'P': /* Packing policy */
      nco_pck_plc_sng=(char *)strdup(optarg);
      nco_pck_plc=nco_pck_plc_get(nco_pck_plc_sng);
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
      if(signal(SIGUSR1,nco_cnt_run) == SIG_ERR) (void)fprintf(fp_stderr,"%s: ERROR Could not install suspend handler.\n",prg_nm_get());
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
    case 'x': /* Exclude rather than extract variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
      break;
    case 'Y': /* Pseudonym */
      /* Call prg_prs to reset pseudonym */
      optarg_lcl=(char *)strdup(optarg);
      if(prg_nm) prg_nm=(char *)nco_free(prg_nm);
      prg_nm=prg_prs(optarg_lcl,&prg);
      optarg_lcl=(char *)nco_free(optarg_lcl);
      break;
    case 'y': /* Operation type */
      nco_op_typ_sng=(char *)strdup(optarg);
      if(prg == ncra || prg == ncea ) nco_op_typ=nco_op_typ_get(nco_op_typ_sng);
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
  
  /* Process positional arguments and fill in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&fl_nbr,&fl_out,&FL_LST_IN_FROM_STDIN);
  
  /* Make uniform list of user-specified dimension limits */
  if(lmt_nbr > 0) lmt=nco_lmt_prs(lmt_nbr,lmt_arg);
  
  /* Initialize thread information */
  thr_nbr=nco_openmp_ini(thr_nbr);
  in_id_arr=(int *)nco_malloc(thr_nbr*sizeof(int));

  /* Parse filename */
  fl_in=nco_fl_nm_prs(fl_in,0,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);
  /* Open file for reading */
  rcd=nco_open(fl_in,NC_NOWRITE,&in_id);
  
  /* Get number of variables, dimensions, and record dimension ID of input file */
  (void)nco_inq(in_id,&nbr_dmn_fl,&nbr_var_fl,(int *)NULL,&rec_dmn_id);
  (void)nco_inq_format(in_id,&fl_in_fmt);
  
  /* Form initial extraction list which may include extended regular expressions */
  xtr_lst=nco_var_lst_mk(in_id,nbr_var_fl,var_lst_in,EXCLUDE_INPUT_LIST,EXTRACT_ALL_COORDINATES,&nbr_xtr);
  
  /* Change included variables to excluded variables */
  if(EXCLUDE_INPUT_LIST) xtr_lst=nco_var_lst_xcl(in_id,nbr_var_fl,xtr_lst,&nbr_xtr);
  
  /* Is this an CCM/CCSM/CF-format history tape? */
  CNV_CCM_CCSM_CF=nco_cnv_ccm_ccsm_cf_inq(in_id);

  /* Add all coordinate variables to extraction list */
  if(EXTRACT_ALL_COORDINATES) xtr_lst=nco_var_lst_crd_add(in_id,nbr_dmn_fl,nbr_var_fl,xtr_lst,&nbr_xtr,CNV_CCM_CCSM_CF);
  
  /* Make sure coordinates associated extracted variables are also on extraction list */
  if(EXTRACT_ASSOCIATED_COORDINATES) xtr_lst=nco_var_lst_crd_ass_add(in_id,xtr_lst,&nbr_xtr,CNV_CCM_CCSM_CF);
  
  /* Sort extraction list by variable ID for fastest I/O */
  if(nbr_xtr > 1) xtr_lst=nco_lst_srt_nm_id(xtr_lst,nbr_xtr,False);
  
  /* We now have final list of variables to extract. Phew. */
  
  /* Find coordinate/dimension values associated with user-specified limits
     NB: nco_lmt_evl() with same nc_id contains OpenMP critical region */
  for(idx=0;idx<lmt_nbr;idx++) (void)nco_lmt_evl(in_id,lmt[idx],0L,FORTRAN_IDX_CNV);
  
  /* Find dimensions associated with variables to be extracted */
  dmn_lst=nco_dmn_lst_ass_var(in_id,xtr_lst,nbr_xtr,&nbr_dmn_xtr);
  
  /* Fill in dimension structure for all extracted dimensions */
  dim=(dmn_sct **)nco_malloc(nbr_dmn_xtr*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_xtr;idx++) dim[idx]=nco_dmn_fll(in_id,dmn_lst[idx].id,dmn_lst[idx].nm);
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
  
  /* Create stand-alone limit structure just for record dimension */
  if(prg == ncra || prg == ncrcat){
    if(rec_dmn_id == NCO_REC_DMN_UNDEFINED){
      (void)fprintf(stdout,gettext("%s: ERROR input file %s lacks a record dimension\n"),prg_nm_get(),fl_in);
      if(fl_nbr == 1)(void)fprintf(stdout,gettext("%s: HINT Use ncks instead of %s\n"),prg_nm_get(),prg_nm_get());
      nco_exit(EXIT_FAILURE);
    } /* endif */
    lmt_rec=nco_lmt_sct_mk(in_id,rec_dmn_id,lmt,lmt_nbr,FORTRAN_IDX_CNV);
  } /* endif */
  
  /* Is this an ARM-format data file? */
  CNV_ARM=nco_cnv_arm_inq(in_id);
  /* NB: nco_cnv_arm_base_time_get() with same nc_id contains OpenMP critical region */
  if(CNV_ARM) base_time_srt=nco_cnv_arm_base_time_get(in_id);
  
  /* Fill in variable structure list for all extracted variables */
  var=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));
  var_out=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));
  for(idx=0;idx<nbr_xtr;idx++){
    var[idx]=nco_var_fll(in_id,xtr_lst[idx].id,xtr_lst[idx].nm,dim,nbr_dmn_xtr);
    var_out[idx]=nco_var_dpl(var[idx]);
    (void)nco_xrf_var(var[idx],var_out[idx]);
    (void)nco_xrf_dmn(var_out[idx]);
  } /* end loop over idx */
  /* Extraction list no longer needed */
  xtr_lst=nco_nm_id_lst_free(xtr_lst,nbr_xtr);
  
  /* Divide variable lists into lists of fixed variables and variables to be processed */
  (void)nco_var_lst_dvd(var,var_out,nbr_xtr,CNV_CCM_CCSM_CF,nco_pck_plc_nil,nco_pck_map_nil,NULL,0,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc,&var_prc_out,&nbr_var_prc);
  
#ifdef ENABLE_MPI
  if(prc_rnk == rnk_mgr){ /* MPI manager code */
#endif /* !ENABLE_MPI */
    /* Make output and input files consanguinous */
  if(fl_out_fmt == NCO_FORMAT_UNDEFINED) fl_out_fmt=fl_in_fmt;

  /* Open output file */
    fl_out_tmp=nco_fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&out_id);
    
    /* Copy global attributes */
    (void)nco_att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL,(nco_bool)True);
    
    /* Catenate time-stamped command line to "history" global attribute */
    if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);
    
    /* Add input file list global attribute */
    if(FL_LST_IN_APPEND && HISTORY_APPEND && FL_LST_IN_FROM_STDIN) (void)nco_fl_lst_att_cat(out_id,fl_lst_in,fl_nbr);
    
#ifdef ENABLE_MPI
    /* Initialize MPI task information */
    if(prc_nbr > 0 && HISTORY_APPEND) (void)nco_mpi_att_cat(out_id,prc_nbr);
#endif /* !ENABLE_MPI */
    
    if(thr_nbr > 0 && HISTORY_APPEND) (void)nco_thr_att_cat(out_id,thr_nbr);
    
    /* Define dimensions in output file */
    (void)nco_dmn_dfn(fl_out,out_id,dmn_out,nbr_dmn_xtr);
    
    /* Define variables in output file, copy their attributes */
    (void)nco_var_dfn(in_id,fl_out,out_id,var_out,nbr_xtr,(dmn_sct **)NULL,(int)0,nco_pck_plc_nil,nco_pck_map_nil,dfl_lvl);
    
    /* Turn off default filling behavior to enhance efficiency */
    (void)nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
    
    /* Take output file out of define mode */
    (void)nco_enddef(out_id);
#ifdef ENABLE_MPI
  } /* prc_rnk != rnk_mgr */
  
  /* Manager obtains output filename and broadcasts to workers */
  if(prc_rnk == rnk_mgr) fl_nm_lng=(int)strlen(fl_out_tmp); 
  MPI_Bcast(&fl_nm_lng,1,MPI_INT,0,MPI_COMM_WORLD);
  if(prc_rnk != rnk_mgr) fl_out_tmp=(char *)malloc((fl_nm_lng+1)*sizeof(char));
  MPI_Bcast(fl_out_tmp,fl_nm_lng+1,MPI_CHAR,0,MPI_COMM_WORLD);
#endif /* !ENABLE_MPI */

  /* Pre-processor token spaghetti here is necessary so that 
     1. UP/SMP/MPI codes all zero srt vectors before calling nco_var_val_cpy() 
     2. No codes zero srt vectors more than once */
  /* Assign zero-start and unity-stride vectors to output variables */
  (void)nco_var_srd_srt_set(var_out,nbr_xtr);

#ifdef ENABLE_MPI
  if(prc_rnk == rnk_mgr){ /* MPI manager code */
    TKN_WRT_FREE=False;
#endif /* !ENABLE_MPI */
    /* Copy variable data for non-processed variables */
    (void)nco_var_val_cpy(in_id,out_id,var_fix,nbr_var_fix);
#ifdef ENABLE_MPI
    /* Close output file so workers can open it */
    nco_close(out_id);
    TKN_WRT_FREE=True;
  } /* prc_rnk != rnk_mgr */
#else /* !ENABLE_MPI */
  /* Close first input netCDF file (SMP only since MPI code immediate re-opens) */
  (void)nco_close(in_id);
#endif /* !ENABLE_MPI */
  
  /* Allocate and, if necesssary, initialize accumulation space for processed variables */
  for(idx=0;idx<nbr_var_prc;idx++){
    if(prg == ncra || prg == ncrcat){
      /* Allocate space for only one record */
      var_prc_out[idx]->sz=var_prc[idx]->sz=var_prc[idx]->sz_rec;
    } /* endif */
    if(prg == ncra || prg == ncea){
      var_prc_out[idx]->tally=var_prc[idx]->tally=(long *)nco_malloc(var_prc_out[idx]->sz*sizeof(long));
      (void)nco_zero_long(var_prc_out[idx]->sz,var_prc_out[idx]->tally);
      var_prc_out[idx]->val.vp=(void *)nco_malloc(var_prc_out[idx]->sz*nco_typ_lng(var_prc_out[idx]->type));
      (void)nco_var_zero(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->val);
    } /* end if */
  } /* end loop over idx */
  
#ifdef ENABLE_MPI
  /* NB: Only manager code manipulates value of TKN_WRT_FREE
     Pass 1: Workers construct local persistant variable lists
             Open first file
             mpncra and mpncrcat process first record only
	     mpncea ingests complete file
	     Workers create local list of their variables 
     Pass 2: Complete record/file loops with local variable lists
             Workers skip first timestep (mpncra/mpncrcat) 
             Workers process only variables in their local list from Pass 1 
             This variable persistance is necessary for mpncra and mpncea
	     since their workers must maintain running tallies for each variable.
             Variable persistance is not necessary for mpncrcat 
	     However, we do it anyway to keep mpncrcat and mpncra similar
	     mpncrcat writes records as it reads them and finishes after pass 2
     Pass 3: 
             mpncea and mpncra require a final loop to normalize and write
             Write-token for this loop is passed sequentially through the ranks */

  /* Begin Pass 1: Workers construct local persistant variable lists */
  fl_idx=0;
  
  /* Variables may have different ID, missing_value, type, in each file */
  for(idx=0;idx<nbr_var_prc;idx++) (void)nco_var_mtd_refresh(in_id,var_prc[idx]);
  
  /* Each file can have a different number of records to process
     NB: nco_lmt_evl() with same nc_id contains OpenMP critical region */
  if(prg == ncra || prg == ncrcat) (void)nco_lmt_evl(in_id,lmt_rec,idx_rec_out,FORTRAN_IDX_CNV);
  
  /* NB: nco_cnv_arm_base_time_get() with same nc_id contains OpenMP critical region */
  if(CNV_ARM) base_time_crr=nco_cnv_arm_base_time_get(in_id);
  
  /* Perform various error-checks on input file */
  if(False) (void)nco_fl_cmp_err_chk();
  
  if(prg == ncra || prg == ncrcat){ /* ncea jumps to else branch */
    /* Loop over each record in current file */
    if(dbg_lvl >= nco_dbg_std && lmt_rec->srt > lmt_rec->end) (void)fprintf(stdout,gettext("%s: WARNING %s (input file %d) is superfluous\n"),prg_nm_get(),fl_in,fl_idx);
    idx_rec=lmt_rec->srt;
    if(fl_idx == fl_nbr-1 && idx_rec >= 1L+lmt_rec->end-lmt_rec->srd) LAST_RECORD=True;
    /* Process all variables in first record */
    if(dbg_lvl > nco_dbg_scl) (void)fprintf(stderr,gettext("Record %ld of %s is input record %ld\n"),idx_rec,fl_in,idx_rec_out);
    
    if(prc_rnk == rnk_mgr){ /* MPI manager code */
      /* Compensate for incrementing on each worker's first message */
      var_wrt_nbr=-prc_nbr+1;
      idx=0;
      /* While variables remain to be processed or written... */
      while(var_wrt_nbr < nbr_var_prc){
	/* Receive any message from any worker */
	MPI_Recv(wrk_id_bfr,wrk_id_bfr_lng,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&mpi_stt);
	/* Obtain MPI message tag type */
	msg_tag_typ=mpi_stt.MPI_TAG;
	/* Get sender's prc_rnk */
	rnk_wrk=wrk_id_bfr[0];
	
	/* Allocate next variable, if any, to worker */
	if(msg_tag_typ == msg_tag_wrk_rqs){
	  var_wrt_nbr++; /* [nbr] Number of variables written */
	  /* Worker closed output file before sending msg_tag_wrk_rqs */
	  if(prg == ncrcat) TKN_WRT_FREE=True; /* File written to at this point only for ncrcat */
	  
	  if(idx > nbr_var_prc-1){
	    msg_bfr[0]=idx_all_wrk_ass; /* [enm] All variables already assigned */
	    msg_bfr[1]=out_id; /* Output file ID */
	  }else{
	    /* Tell requesting worker to allocate space for next variable */
	    msg_bfr[0]=idx; /* [idx] Variable to be processed */
	    /* csz: fxm Workers do not need to know Master's out_id */
	    msg_bfr[1]=out_id; /* Output file ID */
	    msg_bfr[2]=var_prc_out[idx]->id; /* [id] Variable ID in output file */
	    /* Point to next variable on list */
	    idx++;
	  } /* endif idx */
	  MPI_Send(msg_bfr,msg_bfr_lng,MPI_INT,rnk_wrk,msg_tag_wrk_rsp,MPI_COMM_WORLD);
	}else if(msg_tag_typ == msg_tag_tkn_wrt_rqs && prg == ncrcat){ /* msg_tag_typ != msg_tag_wrk_rqs */
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
      /* csz: fxm delete redundant statement with two lines further down */
      wrk_id_bfr[0]=prc_rnk;
      var_wrt_nbr=0;
      while(1){ /* While work remains... */
        /* Send msg_tag_wrk_rqs */
        wrk_id_bfr[0]=prc_rnk;
        MPI_Send(wrk_id_bfr,wrk_id_bfr_lng,MPI_INT,rnk_mgr,msg_tag_wrk_rqs,MPI_COMM_WORLD);
        /* Receive msg_tag_wrk_rsp */
        MPI_Recv(msg_bfr,msg_bfr_lng,MPI_INT,rnk_mgr,msg_tag_wrk_rsp,MPI_COMM_WORLD,&mpi_stt);
        idx=msg_bfr[0];
	/* csz: fxm dangerous---workers must get and use their own out_id's, not master's out_id */
        out_id=msg_bfr[1];
        if(idx == idx_all_wrk_ass){
	  break;
	}else{ /* idx != idx_all_wrk_ass */
	  /* Assign this variable to this worker for rest of program */
	  lcl_idx_lst[lcl_nbr_var]=idx;
	  /* csz: got to here reading logic */
	  lcl_nbr_var++;
          var_prc_out[idx]->id=msg_bfr[2];
	  if(dbg_lvl >= nco_dbg_var) rcd+=nco_var_prc_crr_prn(idx,var_prc[idx]->nm);
	  if(dbg_lvl >= nco_dbg_var) (void)fflush(fp_stderr);
	  /* Update hyperslab start indices to current record for each variable */
	  var_prc[idx]->srt[0]=idx_rec;
	  var_prc[idx]->end[0]=idx_rec;
	  var_prc[idx]->cnt[0]=1L;
	  /* Retrieve variable from disk into memory */
	  /* NB: nco_var_get() with same nc_id contains OpenMP critical region */
	  (void)nco_var_get(in_id,var_prc[idx]);
	  if(prg == ncra){
	    /* Convert char, short, long, int types to doubles before arithmetic */
	    /* Output variable type is "sticky" so only convert on first record */
	    if(idx_rec_out == 0) var_prc_out[idx]=nco_typ_cnv_rth(var_prc_out[idx],nco_op_typ);
	    /* Convert var_prc to type of var_prc_out in case type of variable on disk has changed */
	    var_prc[idx]=nco_var_cnf_typ(var_prc_out[idx]->type,var_prc[idx]);
	    /* Perform arithmetic operations: avg, min, max, ttl, ... */
	    nco_opr_drv(idx_rec_out,nco_op_typ,var_prc[idx],var_prc_out[idx]);
	  } /* !ncra */
	  
	  /* Append current record to output file */
	  if(prg == ncrcat){
	    var_prc_out[idx]->srt[0]=var_prc_out[idx]->end[0]=idx_rec_out;
	    var_prc_out[idx]->cnt[0]=1L;
	    /* Replace this time_offset value with time_offset from initial file base_time */
	    if(CNV_ARM && !strcmp(var_prc[idx]->nm,"time_offset")) var_prc[idx]->val.dp[0]+=(base_time_crr-base_time_srt);
	    
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
	      if(var_prc_out[idx]->sz_rec > 1) (void)nco_put_vara(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc[idx]->val.vp,var_prc_out[idx]->type);
	      else (void)nco_put_var1(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc[idx]->val.vp,var_prc_out[idx]->type);
	      /* Close output file and increment written counter */
	      nco_close(out_id);
	      var_wrt_nbr++;
	    } /* endif tkn_wrt_rqs_xcp */
	  } /* end if ncrcat */
	  
	  /* Make sure record coordinate, if any, is monotonic */
	  if(prg == ncrcat && var_prc[idx]->is_crd_var) (void)rec_crd_chk(var_prc[idx],fl_in,fl_out,idx_rec,idx_rec_out);
	  /* Convert missing_value, if any, back to unpacked type */
	  if(var_prc[idx]->has_mss_val && var_prc[idx]->type != var_prc[idx]->typ_upk && !LAST_RECORD)
	    var_prc[idx]=nco_cnv_mss_val_typ(var_prc[idx],var_prc[idx]->typ_upk);
	  /* Free current input buffer */
	  var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);
	  
	  if(dbg_lvl >= nco_dbg_var) (void)fprintf(stderr,"\n");
	} /* !idx_all_wrk_ass */
      } /* while(1) loop requesting work/token in Worker */
      idx_rec_out++; /* [idx] Index of current record in output file (0 is first, ...) */
    } /* endif Worker */
    printf("DEBUG: End of first pass of ncra/ncrcat at node %d\n",prc_rnk);
    
    /* End of ncra, ncrcat section */
  }else{ /* ncea */
    
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
	  /* TKN_WRT_FREE=True; ncea does not do file write here */
	  
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
	} /* msg_tag_typ != msg_tag_wrk_rqs */
      } /* end while var_wrt_nbr < nbr_var_prc */
    }else{ /* prc_rnk != rnk_mgr, end Manager code begin Worker code */
      while(1){ /* While work remains... */
	/* Send msg_tag_wrk_rqs */
	wrk_id_bfr[0]=prc_rnk;
	MPI_Send(wrk_id_bfr,wrk_id_bfr_lng,MPI_INT,rnk_mgr,msg_tag_wrk_rqs,MPI_COMM_WORLD);
	/* Receive msg_tag_wrk_rsp */
	MPI_Recv(msg_bfr,msg_bfr_lng,MPI_INT,rnk_mgr,msg_tag_wrk_rsp,MPI_COMM_WORLD,&mpi_stt);
	idx=msg_bfr[0];
	out_id=msg_bfr[1];
	if(idx == idx_all_wrk_ass) break;
	else{
	  lcl_idx_lst[lcl_nbr_var]=idx; /* storing the indices for subsequent processing by the worker */
	  lcl_nbr_var++;
	  var_prc_out[idx]->id=msg_bfr[2];
	  if(dbg_lvl >= nco_dbg_var) rcd+=nco_var_prc_crr_prn(idx,var_prc[idx]->nm);
	  if(dbg_lvl >= nco_dbg_var) (void)fflush(fp_stderr);
	  /* Retrieve variable from disk into memory */
	  /* NB: nco_var_get() with same nc_id contains OpenMP critical region */
	  (void)nco_var_get(in_id,var_prc[idx]);
	  
	  /* Convert char, short, long, int types to doubles before arithmetic */
	  /* var_prc[idx]=nco_typ_cnv_rth(var_prc[idx],nco_op_typ); */
	  /* Output variable type is "sticky" so only convert on first record */
	  if(fl_idx == 0) var_prc_out[idx]=nco_typ_cnv_rth(var_prc_out[idx],nco_op_typ);
	  /* Convert var_prc to type of var_prc_out in case type of variable on disk has changed */
	  var_prc[idx]=nco_var_cnf_typ(var_prc_out[idx]->type,var_prc[idx]);
	  /* Perform arithmetic operations: avg, min, max, ttl, ... */ /* Note: fl_idx not idx_rec_out! */
	  nco_opr_drv(fl_idx,nco_op_typ,var_prc[idx],var_prc_out[idx]);
	  
	  /* Free current input buffer */
	  var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);
	} /* !idx_all_wrk_ass */
      } /* while(1) loop requesting work/token in Worker */
    } /* endif Worker */
  } /* end else ncea */
  
  if(dbg_lvl > nco_dbg_scl) (void)fprintf(stderr,"\n");
  
  /* Close input netCDF file */
  nco_close(in_id);
  
#ifdef ENABLE_MPI
  /* This barrier ensures that all nodes have reached this point together.
     Otherwise, the manager code should be altered so it can deal with 
     nodes in different stages of execution at any time.
     Daniel: I think we should be convinced of this parallelization 
     structure before bothering with implementing the code restructuring in
     the manager that would let us remove the barrier.  The barrier 
     should only negligibly impact performance. */
  checkpointMpi(prc_rnk, 1);
#endif  /* ENABLE_MPI */

  /* End Pass 1: Workers construct local persistant variable lists */
  printf("DEBUG: prc_rnk %d is done with 1st pass\n",prc_rnk);
  /* Begin Pass 2: Complete record/file loops with local variable lists */
  
#endif /* !ENABLE_MPI */
  
  /* Loop over input files */
  for(fl_idx=0;fl_idx<fl_nbr;fl_idx++){
    /* Parse filename */
    if(fl_idx != 0) fl_in=nco_fl_nm_prs(fl_in,fl_idx,(int *)NULL,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
    if(dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,gettext("\nInput file %d is %s; "),fl_idx,fl_in);
    /* Make sure file is on local system and is readable or die trying */
    if(fl_idx != 0) fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);
    if(dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,gettext("local file %s:\n"),fl_in);
    
    /* Open file once per thread to improve caching */
    for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) rcd=nco_open(fl_in,NC_NOWRITE,in_id_arr+thr_idx);
    in_id=in_id_arr[0];
#ifdef ENABLE_MPI
    printf("DEBUG: input file opened in prc_rnk %d inside the loop\n",prc_rnk);
#endif /* !ENABLE_MPI */
    
    /* Variables may have different IDs and missing_values in each file */
    for(idx=0;idx<nbr_var_prc;idx++) (void)nco_var_mtd_refresh(in_id,var_prc[idx]);
    
    /* Each file can have a different number of records to process
       NB: nco_lmt_evl() with same nc_id contains OpenMP critical region */
    if(prg == ncra || prg == ncrcat) (void)nco_lmt_evl(in_id,lmt_rec,idx_rec_out,FORTRAN_IDX_CNV);
    
    /* NB: nco_cnv_arm_base_time_get() with same nc_id contains OpenMP critical region */
    if(CNV_ARM) base_time_crr=nco_cnv_arm_base_time_get(in_id);
    
    /* Perform various error-checks on input file */
    if(False) (void)nco_fl_cmp_err_chk();
    
    if(prg == ncra || prg == ncrcat){ /* ncea jumps to else branch */
      /* Loop over each record in current file */
      if(dbg_lvl >= nco_dbg_std && lmt_rec->srt > lmt_rec->end) (void)fprintf(stdout,gettext("%s: WARNING %s (input file %d) is superfluous\n"),prg_nm_get(),fl_in,fl_idx);
      for(idx_rec=lmt_rec->srt;idx_rec<=lmt_rec->end;idx_rec+=lmt_rec->srd){
	if(fl_idx == fl_nbr-1 && idx_rec >= 1L+lmt_rec->end-lmt_rec->srd) LAST_RECORD=True;
	
#ifdef ENABLE_MPI
	if(fl_idx == 0 && idx_rec == lmt_rec->srt){
	  /* MPI operators processed first record in first-stage loop */
	  continue;
	}else{ /* a loop of idx = stored indices */    
	  if(prc_rnk == rnk_mgr){ /* For ncrcat, Manager gives write access for each record in each file */
	    if(prg == ncrcat){ /* Give Write access to write current record */
	      /* var_wrt_nbr=-prc_nbr+1; */
	      var_wrt_nbr=0;
	      while(var_wrt_nbr < nbr_var_prc){ /* Give write access to Workers who have some variables; wrong condn? */
		/* Receive message from any worker */
		MPI_Recv(wrk_id_bfr,wrk_id_bfr_lng,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&mpi_stt);
		/* Obtain MPI message tag type */
		msg_tag_typ=mpi_stt.MPI_TAG;
		/* Get sender's prc_rnk */
		rnk_wrk=wrk_id_bfr[0];
		if(msg_tag_typ == msg_tag_wrk_done) TKN_WRT_FREE=True;
		if(msg_tag_typ == msg_tag_tkn_wrt_rqs){
		  if(rnk_wrk == tkn_wrt_rnk){ /* Prev write completed */
		    TKN_WRT_FREE=True;
		  } /* rnk_wrk != tkn_wrt_rnk */
		  /* Allocate token if free, else ask worker to try later */
		  if(TKN_WRT_FREE){
		    TKN_WRT_FREE=False;
		    msg_bfr[0]=tkn_wrt_rqs_xcp; /* Accept request for write token */
		    tkn_wrt_rnk=rnk_wrk; /* To track who has the token */
		    var_wrt_nbr++;
		  }else{
		    msg_bfr[0]=tkn_wrt_rqs_dny; /* Deny request for write token */
		  } /* !TKN_WRT_FREE */
		  MPI_Send(msg_bfr,msg_bfr_lng,MPI_INT,rnk_wrk,msg_tag_tkn_wrt_rsp,MPI_COMM_WORLD);
		} /* msg_tag_typ != msg_tag_tkn_wrt_rqs */
	      } /* End-while token request loop */
	    } /* !ncrcat */
	  }else{ /* prc_rnk != rnk_mgr, end Manager code begin Worker code */
            wrk_id_bfr[0]=prc_rnk;
	    var_wrt_nbr=0;
	    /* if(fl_idx == 0 && idx_rec == lmt_rec->srt) continue;
	       else  a loop of idx = stored indices */    
	    for(jdx=0;jdx<lcl_nbr_var;jdx++){
	      idx=lcl_idx_lst[jdx];
#endif /* !ENABLE_MPI */
	      /* Process all variables in current record */
	      if(dbg_lvl > nco_dbg_scl) (void)fprintf(stderr,gettext("Record %ld of %s is input record %ld\n"),idx_rec,fl_in,idx_rec_out);
#if 0
	      /* NB: Immediately preceding MPI for scope confounds Emacs indentation
		 Fake end scope restores correct indentation, simplifies code-checking */
	    } /* fake end for */
#endif /* !0 */
#ifndef ENABLE_MPI
#ifdef _OPENMP
#pragma omp parallel for default(none) private(idx,in_id) shared(CNV_ARM,base_time_crr,base_time_srt,dbg_lvl,fl_in,fl_out,idx_rec,idx_rec_out,in_id_arr,LAST_RECORD,nbr_var_prc,nco_op_typ,out_id,prg,rcd,var_prc,var_prc_out)
#endif /* !_OPENMP */
	    /* UP and SMP codes main loop over variables */
	    for(idx=0;idx<nbr_var_prc;idx++){
#endif /* ENABLE_MPI */
	      in_id=in_id_arr[omp_get_thread_num()];
	      if(dbg_lvl >= nco_dbg_var) rcd+=nco_var_prc_crr_prn(idx,var_prc[idx]->nm);
	      if(dbg_lvl >= nco_dbg_var) (void)fflush(fp_stderr);
	      /* Update hyperslab start indices to current record for each variable */
	      var_prc[idx]->srt[0]=idx_rec;
	      var_prc[idx]->end[0]=idx_rec;
	      var_prc[idx]->cnt[0]=1L;
	      /* Retrieve variable from disk into memory */
	      /* NB: nco_var_get() with same nc_id contains OpenMP critical region */
	      (void)nco_var_get(in_id,var_prc[idx]);
	      if(prg == ncra){
		/* Convert char, short, long, int types to doubles before arithmetic */
		var_prc[idx]=nco_typ_cnv_rth(var_prc[idx],nco_op_typ);
		/* Output variable type is "sticky" so only convert on first record */
		if(idx_rec_out == 0) var_prc_out[idx]=nco_typ_cnv_rth(var_prc_out[idx],nco_op_typ);
		/* Convert var_prc to type of var_prc_out in case type of variable on disk has changed */
		var_prc[idx]=nco_var_cnf_typ(var_prc_out[idx]->type,var_prc[idx]);
		/* Perform arithmetic operations: avg, min, max, ttl, ... */
		nco_opr_drv(idx_rec_out,nco_op_typ,var_prc[idx],var_prc_out[idx]);
	      } /* end if ncra */
	      
	      /* Append current record to output file */
	      if(prg == ncrcat){
		var_prc_out[idx]->srt[0]=var_prc_out[idx]->end[0]=idx_rec_out;
		var_prc_out[idx]->cnt[0]=1L;
		/* Replace this time_offset value with time_offset from initial file base_time */
		if(CNV_ARM && !strcmp(var_prc[idx]->nm,"time_offset")) var_prc[idx]->val.dp[0]+=(base_time_crr-base_time_srt);
		
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
		  if(var_prc_out[idx]->sz_rec > 1) (void)nco_put_vara(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc[idx]->val.vp,var_prc_out[idx]->type);
		  else (void)nco_put_var1(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc[idx]->val.vp,var_prc_out[idx]->type);
#ifdef ENABLE_MPI
		  /* Close output file and increment written counter */
		  nco_close(out_id);
		  var_wrt_nbr++;
		} /* endif tkn_wrt_rqs_xcp */
#endif /* !ENABLE_MPI */
	      } /* end if ncrcat */
	      /* Make sure record coordinate, if any, is monotonic */
	      if(prg == ncrcat && var_prc[idx]->is_crd_var) (void)rec_crd_chk(var_prc[idx],fl_in,fl_out,idx_rec,idx_rec_out);
	      /* Convert missing_value, if any, back to disk type */
	      if(var_prc[idx]->has_mss_val && var_prc[idx]->type != var_prc[idx]->typ_upk && !LAST_RECORD)
		var_prc[idx]=nco_cnv_mss_val_typ(var_prc[idx],var_prc[idx]->typ_upk);
	      /* Free current input buffer */
	      var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);
	    } /* end (OpenMP Parallel for) loop over variables */
#ifdef ENABLE_MPI
	    if(prg == ncrcat){
	      /* Return token after writing record's last variable */
	      wrk_id_bfr[0]=prc_rnk;
	      MPI_Send(wrk_id_bfr,wrk_id_bfr_lng,MPI_INT,rnk_mgr,msg_tag_wrk_done,MPI_COMM_WORLD);
	    } /* !ncrcat */
#endif /* !ENABLE_MPI */
	    idx_rec_out++; /* [idx] Index of current record in output file (0 is first, ...) */
	    if(dbg_lvl >= nco_dbg_var) (void)fprintf(stderr,"\n");
#ifdef ENABLE_MPI
	  } /* !Worker */
	} /* end else ! fl_idx=0,idx_rec=srt */
#endif /* !ENABLE_MPI */
      } /* end loop over idx_rec */
#ifdef ENABLE_MPI
      if(prc_rnk != rnk_mgr){ /* Only Worker */
#endif /* !ENABLE_MPI */
	/* Warn if fewer than number of requested records were read and final file has been processed */
	if(lmt_rec->lmt_typ == lmt_dmn_idx && lmt_rec->is_usr_spc_min && lmt_rec->is_usr_spc_max){
	  long rec_nbr_rqs; /* Number of records user requested */
	  rec_nbr_rqs=1L+(lmt_rec->max_idx-lmt_rec->min_idx)/lmt_rec->srd;
	  if(dbg_lvl >= nco_dbg_std && fl_idx == fl_nbr-1 && rec_nbr_rqs != idx_rec_out) (void)fprintf(stdout,gettext("%s: WARNING User requested %li records but only %li were found\n"),prg_nm_get(),rec_nbr_rqs,idx_rec_out);
	} /* end if */
	/* Error if no records were read and final file has been processed */
	if(idx_rec_out <= 0 && fl_idx == fl_nbr-1){
	  (void)fprintf(stdout,gettext("%s: ERROR No records lay within specified hyperslab\n"),prg_nm_get());
	  nco_exit(EXIT_FAILURE);
	} /* end if */
#ifdef ENABLE_MPI
      } /* !Worker */
      printf("DEBUG: prc_rnk %d at the end of ncra/rcat\n",prc_rnk);
#endif /* !ENABLE_MPI */
      /* End of ncra, ncrcat section */
    }else{ /* ncea */
#ifdef ENABLE_MPI
      if(prc_rnk != rnk_mgr){ /* Only Worker does the ncea processing */
	if(fl_idx == 0){
	  continue;
	}else{ /* a loop of idx = stored indices */
	  for(jdx=0;jdx<lcl_nbr_var;jdx++){
	    idx=lcl_idx_lst[jdx];
#else /* !ENABLE_MPI */
#ifdef _OPENMP
#pragma omp parallel for default(none) private(idx,in_id) shared(dbg_lvl,fl_idx,in_id_arr,nbr_var_prc,nco_op_typ,rcd,var_prc,var_prc_out)
#endif /* !_OPENMP */
	    for(idx=0;idx<nbr_var_prc;idx++){ /* Process all variables in current file */
#endif /* !ENABLE_MPI */	
	      in_id=in_id_arr[omp_get_thread_num()];
	      if(dbg_lvl >= nco_dbg_var) rcd+=nco_var_prc_crr_prn(idx,var_prc[idx]->nm);
	      if(dbg_lvl >= nco_dbg_var) (void)fflush(fp_stderr);
	      /* Retrieve variable from disk into memory */
	      /* NB: nco_var_get() with same nc_id contains OpenMP critical region */
	      (void)nco_var_get(in_id,var_prc[idx]);
	      
	      /* Convert char, short, long, int types to doubles before arithmetic */
	      /* var_prc[idx]=nco_typ_cnv_rth(var_prc[idx],nco_op_typ); */
	      /* Output variable type is "sticky" so only convert on first record */
	      if(fl_idx == 0) var_prc_out[idx]=nco_typ_cnv_rth(var_prc_out[idx],nco_op_typ);
	      /* Convert var_prc to type of var_prc_out in case type of variable on disk has changed */
	      var_prc[idx]=nco_var_cnf_typ(var_prc_out[idx]->type,var_prc[idx]);
	      /* Perform arithmetic operations: avg, min, max, ttl, ... */ /* Note: fl_idx not idx_rec_out! */
	      nco_opr_drv(fl_idx,nco_op_typ,var_prc[idx],var_prc_out[idx]);
	      
	      /* Free current input buffer */
	      var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);
	    } /* end (OpenMP parallel for) loop over idx */
#ifdef ENABLE_MPI
	  } /* end else !fl_idx=0 */
	} /* !Worker */
#endif /* !ENABLE_MPI */
      } /* end else ncea */
      
      if(dbg_lvl > nco_dbg_scl) (void)fprintf(stderr,"\n");
      
      /* Close input netCDF file */
      for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) nco_close(in_id_arr[thr_idx]);

      /* Dispose local copy of file */
      if(FILE_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)nco_fl_rm(fl_in);
    } /* end loop over fl_idx */
#ifdef ENABLE_MPI
    printf("DEBUG: prc_rnk %d is out of file idx loop\n",prc_rnk); 
#endif /* !ENABLE_MPI */
    /* Normalize, multiply, etc where necessary */
    if(prg == ncra || prg == ncea){
#ifdef ENABLE_MPI
      if(prc_rnk != rnk_mgr){ /* Only workers have indices of variables to process */
	for(jdx=0;jdx<lcl_nbr_var;jdx++){
	  idx=lcl_idx_lst[jdx];
#if 0
	  /* NB: Immediately preceding MPI if/for scopes confound Emacs indentation
	     Fake end scopes restore correct indentation, simplify code-checking */
	} /* fake end for */
      } /* fake end if */
#endif /* !0 */
#else /* !ENABLE_MPI */
#ifdef _OPENMP
#pragma omp parallel for default(none) private(idx) shared(nbr_var_prc,nco_op_typ,var_prc,var_prc_out)
#endif /* !_OPENMP */
      for(idx=0;idx<nbr_var_prc;idx++){
#endif /* !ENABLE_MPI */
	if(var_prc[idx]->is_crd_var){
	  /* Return linear averages of coordinates unless computing extrema
	     Prevent coordinate variables from encountering nco_var_nrm_sdn() */
	  if((nco_op_typ != nco_op_min) && (nco_op_typ != nco_op_max)) (void)nco_var_nrm(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,var_prc[idx]->tally,var_prc_out[idx]->val);
	}else{ /* !var_prc[idx]->is_crd_var */
	  switch(nco_op_typ){
	  case nco_op_avg: /* Normalize sum by tally to create mean */
	  case nco_op_sqrt: /* Normalize sum by tally to create mean */
	  case nco_op_sqravg: /* Normalize sum by tally to create mean */
	  case nco_op_rms: /* Normalize sum of squares by tally to create mean square */
	  case nco_op_avgsqr: /* Normalize sum of squares by tally to create mean square */
	    (void)nco_var_nrm(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,var_prc[idx]->tally,var_prc_out[idx]->val);
	    break;
	  case nco_op_rmssdn: /* Normalize sum of squares by tally-1 to create mean square for sdn */
	    (void)nco_var_nrm_sdn(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,var_prc[idx]->tally,var_prc_out[idx]->val);
	    break;
	  case nco_op_min: /* Minimum is already in buffer, do nothing */
	  case nco_op_max: /* Maximum is already in buffer, do nothing */
	  case nco_op_ttl: /* Total is already in buffer, do nothing */
	  default:
	    break;
	  } /* end switch */
	  /* Some operations require additional processing */
	  switch(nco_op_typ){
	  case nco_op_rms: /* Take root of mean of sum of squares to create root mean square */
	  case nco_op_rmssdn: /* Take root of sdn mean of sum of squares to create root mean square for sdn */
	  case nco_op_sqrt: /* Take root of mean to create root mean */
	    (void)nco_var_sqrt(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,var_prc[idx]->tally,var_prc_out[idx]->val,var_prc_out[idx]->val);
	    break;
	  case nco_op_sqravg: /* Square mean to create square of the mean (for sdn) */
	    (void)nco_var_mlt(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->val,var_prc_out[idx]->val);
	    break;
	  default:
	    break;
	  } /* end switch */
	} /* !var_prc[idx]->is_crd_var */
	var_prc_out[idx]->tally=var_prc[idx]->tally=(long *)nco_free(var_prc[idx]->tally);
      } /* end (OpenMP parallel for) loop over variables */
#ifdef ENABLE_MPI
      printf("DEBUG: End of Normzn at prc_rnk %d\n",prc_rnk);
    } /* prc_rnk == rnk_mgr */
    for(idx = 0; idx < nbr_var_prc; idx++) {
      assert(var_prc_out[idx]->tally == var_prc[idx]->tally);
      if (var_prc_out[idx]->tally == 0) continue;
      printf("DEBUG: node %d reset idx %d tally for var_prc(out) (cleanup)\n",prc_rnk,idx);
      var_prc_out[idx]->tally=var_prc[idx]->tally=(long *)nco_free(var_prc[idx]->tally);
    }
    printf("DEBUG: Mgr shud prnt this too, prc_rnk %d\n",prc_rnk);
#endif /* !ENABLE_MPI */
  } /* !ncra/ncea */
#ifdef ENABLE_MPI
  printf("DEBUG: After all processing; Before barrier, prc_rnk %d\n",prc_rnk);
  if(prc_rnk == rnk_mgr){ /* Only Manager */
    rcd=nco_open(fl_out_tmp,NC_WRITE|NC_SHARE,&out_id);
    printf("DEBUG: prc_rnk %d opened out file\n",prc_rnk);
#endif /* !ENABLE_MPI */
    /* Manually fix YYMMDD date which was mangled by averaging */
    if(CNV_CCM_CCSM_CF && prg == ncra) (void)nco_cnv_ccm_ccsm_cf_date(out_id,var_out,nbr_xtr);
    
    /* End Pass 2: Complete record/file loops with local variable lists */
    /* Begin Pass 3:  */
    /* End Pass 3:  */

    /* Add time variable to output file
       NB: nco_cnv_arm_time_install() contains OpenMP critical region */
    if(CNV_ARM && prg == ncrcat) (void)nco_cnv_arm_time_install(out_id,base_time_srt,dfl_lvl);
#ifdef ENABLE_MPI
    nco_close(out_id); 
    printf("DEBUG: Mgr prc_rnk %d closed out file %d after fixing date, time \n", prc_rnk, out_id);
    MPI_Send(msg_bfr,msg_bfr_lng,MPI_INT,prc_rnk+1,msg_tag_tkn_wrt_rsp,MPI_COMM_WORLD);
    printf("DEBUG: Mgr sent token to worker 1 for final write\n");
  }else{ /* Workers */
    printf("DEBUG: prc_rnk %d waiting for msg from Mgr for final write\n",prc_rnk);
    MPI_Recv(msg_bfr,msg_bfr_lng,MPI_INT,prc_rnk-1,msg_tag_tkn_wrt_rsp,MPI_COMM_WORLD,&mpi_stt);
    printf("DEBUG: prc_rnk %d got token for final write to %d\n",prc_rnk, out_id);
    if(prg == ncra || prg == ncea){
      /* Copy averages to output file and free averaging buffers */
      rcd=nco_open(fl_out_tmp,NC_WRITE|NC_SHARE,&out_id);
      printf("DEBUG: prc_rnk %d opened output file for final write\n",prc_rnk);
      for(jdx=0;jdx<lcl_nbr_var;jdx++){
	idx=lcl_idx_lst[jdx];
	/* Revert any arithmetic promotion but leave unpacked (for now) */
	/*	printf("DEBUG: Before nco_var_cnf_typ prc_rnk %d var val %f\n",prc_rnk,var_prc_out[idx]->val.ip[0]); */
	var_prc_out[idx]=nco_var_cnf_typ(var_prc_out[idx]->typ_upk,var_prc_out[idx]);
	/*        printf("DEBUG: After nco_var_cnf_typ prc_rnk %d var val %f\n",prc_rnk,var_prc_out[idx]->val.ip[0]); */
	/* Packing/Unpacking */
	if(nco_pck_plc == nco_pck_plc_all_new_att) var_prc_out[idx]=nco_put_var_pck(out_id,var_prc_out[idx],nco_pck_plc);
	printf("DEBUG: prc_rnk %d to final write var %s with idx %d val %g\n",prc_rnk,var_prc_out[idx]->nm,idx,var_prc_out[idx]->val.fp[0]);
	if(var_prc_out[idx]->nbr_dim == 0){
	  (void)nco_put_var1(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);
	}else{ /* end if variable is scalar */
	  /* Size of record dimension is one in output file */
	  if(prg == ncra) var_prc_out[idx]->cnt[0]=1L;
	  (void)nco_put_vara(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);
	} /* end if variable is an array */
	var_prc_out[idx]->val.vp=nco_free(var_prc_out[idx]->val.vp);
      } /* end loop over jdx */
      /* Close output file */
      nco_close(out_id); 
      printf("DEBUG: prc_rnk %d closed out file after writing\n",prc_rnk);
      /* Send Token to Manager */
    } /* end if */
    if(prc_rnk == prc_nbr-1) MPI_Send(msg_bfr,msg_bfr_lng,MPI_INT,rnk_mgr,msg_tag_tkn_wrt_rsp,MPI_COMM_WORLD); else MPI_Send(msg_bfr,msg_bfr_lng,MPI_INT,prc_rnk+1,msg_tag_tkn_wrt_rsp,MPI_COMM_WORLD);
  } /* !Workers */
  if(prc_rnk == rnk_mgr){ /* Only Manager */
    MPI_Recv(msg_bfr,msg_bfr_lng,MPI_INT,prc_nbr-1,msg_tag_tkn_wrt_rsp,MPI_COMM_WORLD,&mpi_stt);
    (void)nco_fl_mv(fl_out_tmp,fl_out);
  } /* !Manager */
  
  MPI_Finalize();
  
#else /* !ENABLE_MPI */
  /* Copy averages to output file and free averaging buffers */
  if(prg == ncra || prg == ncea){
    for(idx=0;idx<nbr_var_prc;idx++){
      /* Revert any arithmetic promotion but leave unpacked (for now) */
      var_prc_out[idx]=nco_var_cnf_typ(var_prc_out[idx]->typ_upk,var_prc_out[idx]);
      /* Packing/Unpacking */
      if(nco_pck_plc == nco_pck_plc_all_new_att) var_prc_out[idx]=nco_put_var_pck(out_id,var_prc_out[idx],nco_pck_plc);
      if(var_prc_out[idx]->nbr_dim == 0){
	(void)nco_put_var1(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);
      }else{ /* end if variable is scalar */
	/* Size of record dimension is 1 in output file */
	if(prg == ncra) var_prc_out[idx]->cnt[0]=1L;
	(void)nco_put_vara(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);
      } /* end if variable is an array */
      var_prc_out[idx]->val.vp=nco_free(var_prc_out[idx]->val.vp);
    } /* end loop over idx */
  } /* end if */
  
  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);
  
#endif /* !ENABLE_MPI */
  
  /* Clean memory unless dirty memory allowed */
  if(flg_cln){
    /* ncra-specific memory cleanup */
    if(prg == ncra || prg == ncrcat) lmt_rec=nco_lmt_free(lmt_rec);
    
    /* NCO-generic clean-up */
    /* Free individual strings/arrays */
    if(cmd_ln) cmd_ln=(char *)nco_free(cmd_ln);
    if(fl_in) fl_in=(char *)nco_free(fl_in);
    if(fl_out) fl_out=(char *)nco_free(fl_out);
    if(fl_out_tmp) fl_out_tmp=(char *)nco_free(fl_out_tmp);
    if(fl_pth) fl_pth=(char *)nco_free(fl_pth);
    if(fl_pth_lcl) fl_pth_lcl=(char *)nco_free(fl_pth_lcl);
    if(in_id_arr) in_id_arr=(int *)nco_free(in_id_arr);
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
#if 1
    /* Free variable lists */
    if(nbr_xtr > 0) var=nco_var_lst_free(var,nbr_xtr);
    if(nbr_xtr > 0) var_out=nco_var_lst_free(var_out,nbr_xtr);
    var_prc=(var_sct **)nco_free(var_prc);
    var_prc_out=(var_sct **)nco_free(var_prc_out);
    var_fix=(var_sct **)nco_free(var_fix);
    var_fix_out=(var_sct **)nco_free(var_fix_out);
#endif /* !1 */
#if 0
    /* 20051027: Try ncwa free()'ing technique to avoid freeing dangling pointers */
    if(nbr_xtr > 0) var=nco_var_lst_free(var,nbr_xtr); 
    /* ncwa uses nco_var_lst_free() on var_prc_out because var_out has dangling pointers */
    if(nbr_var_fix > 0) var_fix_out=nco_var_lst_free(var_fix_out,nbr_var_fix);
    if(nbr_var_prc > 0) var_prc_out=nco_var_lst_free(var_prc_out,nbr_var_prc);
    var_prc=(var_sct **)nco_free(var_prc);
    var_fix=(var_sct **)nco_free(var_fix);
    var_out=(var_sct **)nco_free(var_out);
#endif /* !0 */
  } /* !flg_cln */
  
  nco_exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */
