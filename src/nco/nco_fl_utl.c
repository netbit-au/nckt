/* $Header: /cvsroot/nco/nco/src/nco/nco_fl_utl.c,v 1.119 2010/04/18 18:14:30 zender Exp $ */

/* Purpose: File manipulation */

/* Copyright (C) 1995--2010 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_fl_utl.h" /* File manipulation */

int /* O [enm] Mode flag for nco_create() call */
nco_create_mode_mrg /* [fnc] Merge clobber mode with user-specified file format */
(const int clobber_mode, /* I [enm] Clobber mode (NC_CLOBBER or NC_NOCLOBBER) */
 const int fl_out_fmt) /* I [enm] Output file format */
{
  /* Purpose: Merge clobber mode with flag determined by fl_out_fmt
     to produce output mode flag
     clobber_mode: Either NC_CLOBBER or NC_NOCLOBBER
     nccreate_mode: clobber_mode OR'd with (user-specified) file format mode */

  int nccreate_mode; /* O [enm] Mode flag for nco_create() call */

  if(clobber_mode != NC_CLOBBER && clobber_mode != NC_NOCLOBBER){
    (void)fprintf(stderr,"%s: ERROR nco_create_mode_mrg() received unknown clobber_mode\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* endif */

  nccreate_mode=clobber_mode;
  if(fl_out_fmt == NC_FORMAT_64BIT){
    nccreate_mode|=NC_64BIT_OFFSET;
  }else if(fl_out_fmt == NC_FORMAT_NETCDF4){
    nccreate_mode|=NC_NETCDF4;
  }else if(fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC){
    nccreate_mode|=NC_NETCDF4|NC_CLASSIC_MODEL;
  }else if(fl_out_fmt == NC_COMPRESS){ /* fxm: is NC_COMPRESS legal? */
    nccreate_mode|=NC_COMPRESS;
  }else if(fl_out_fmt != NC_FORMAT_CLASSIC){
    (void)fprintf(stderr,"%s: ERROR nco_create_mode_mrg() received unknown file format = %d\n",prg_nm_get(),fl_out_fmt);
    nco_exit(EXIT_FAILURE);
  } /* end else fl_out_fmt */

  return nccreate_mode;
} /* end nco_create_mode_mrg() */

int /* [rcd] Return code */
nco_create_mode_prs /* [fnc] Parse user-specified file format */
(const char * const fl_fmt_sng, /* I [sng] User-specified file format string */
 int * const fl_fmt_enm) /* O [enm] Output file format */
{
  int rcd=NC_NOERR; /* [rcd] Return code */

  /* Careful! Some valid format strings are subsets of other valid format strings */
  if(strstr("classic",fl_fmt_sng) && !strstr(fl_fmt_sng,"netcdf4")){
    /* If "classic" contains string and string does not contain "netcdf4" */
    *fl_fmt_enm=NC_FORMAT_CLASSIC;
  }else if(strstr("64bit",fl_fmt_sng)){
    /* If "64bit" contains string */
    *fl_fmt_enm=NC_FORMAT_64BIT;
  }else if(strstr(fl_fmt_sng,"netcdf4")){
#ifdef ENABLE_NETCDF4
    if(strstr("netcdf4",fl_fmt_sng)){
      /* If "netcdf4" contains string */
      *fl_fmt_enm=NC_FORMAT_NETCDF4;
    }else if(strstr("netcdf4_classic",fl_fmt_sng)){
      /* If "netcdf4_classic" contains string */
      *fl_fmt_enm=NC_FORMAT_NETCDF4_CLASSIC;
    } /* endif NETCDF4 */
#else /* !ENABLE_NETCDF4 */
    (void)fprintf(stderr,"%s: ERROR This NCO was not built with netCDF4 and cannot create the requested netCDF4 file format. HINT: Re-try with netCDF3 file format, either by omitting the filetype specification, or by explicitly specifying the \"-3\", \"--fl_fmt=classic\", \"-6\",  or \"--fl_fmt=64 bit\" options.\n",prg_nm_get());
#endif /* !ENABLE_NETCDF4 */
  }else if(strstr("znetcdf",fl_fmt_sng)){
#ifdef ENABLE_ZNETCDF
    /* If "znetcdf" contains string */
    *fl_fmt_enm=NC_COMPRESS;
#else /* !ENABLE_ZNETCDF */
    (void)fprintf(stderr,"%s: ERROR This NCO was not built with znetCDF (http://snow.cit.cornell.edu/noon/z_netcdf.html) and cannot create the requested znetCDF file format. HINT: Re-try with different (or no) specified file format, such as \"classic\" or \"64bit\".\n",prg_nm_get());
#endif /* !ENABLE_ZNETCDF */
  }else{
    (void)fprintf(stderr,"%s: ERROR Unknown output file format \"%s\" requested. Valid formats are (unambiguous leading characters of) \"classic\", \"64bit\", \"netcdf4\", and \"netcdf4_classic\".\n",prg_nm_get(),fl_fmt_sng);
    nco_exit(EXIT_FAILURE);
  } /* endif fl_fmt_enm */

  return rcd; /* [rcd] Return code */
} /* end nco_create_mode_prs() */

void
nco_fl_cmp_err_chk(void) /* [fnc] Perform error checking on file */
{
  /* Purpose: Perform error checking on file */
} /* end nco_fl_cmp_err_chk() */

void
nco_fl_fmt_vet /* [fnc] Verify output file format supports requested actions */
(const int fl_fmt, /* I [enm] Output file format */
 const int cnk_nbr, /* I [nbr] Number of chunksizes specified */
 const int dfl_lvl) /* I [enm] Deflate level [0..9] */
{
  /* Purpose: Verify output file format supports requested actions */
  if(cnk_nbr > 0 && !(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC)) (void)fprintf(stdout,"%s: WARNING Attempt to chunk variables in output file which has netCDF format %s. Chunking is only supported by netCDF filetypes NC_FORMAT_NETCDF4 and NC_FORMAT_NETCDF4_CLASSIC. Command will attempt to complete but without chunking. HINT: re-run command and change output type to netCDF4 using \"-4\", \"--fl_fmt=netcdf4\", or \"--fl_fmt=netcdf4_classic\" option.\n",prg_nm_get(),nco_fmt_sng(fl_fmt));
  if(dfl_lvl > 0 && !(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC)) (void)fprintf(stdout,"%s: WARNING Attempt to deflate (compress) variables in output file which has netCDF format %s. Deflation is only supported by netCDF filetypes NC_FORMAT_NETCDF4 and NC_FORMAT_NETCDF4_CLASSIC. Command will attempt to complete but without deflation. HINT: re-run command and change output type to netCDF4 using \"-4\", \"--fl_fmt=netcdf4\", or \"--fl_fmt=netcdf4_classic\" option.\n",prg_nm_get(),nco_fmt_sng(fl_fmt));
} /* end nco_nco_fl_fmt_vet() */

void
nco_fl_cp /* [fnc] Copy first file to second */
(const char * const fl_src, /* I [sng] Name of source file to copy */
 const char * const fl_dst) /* I [sng] Name of destination file */
{
  /* Purpose: Copy first file to second */
  char *cp_cmd;
  const char cp_cmd_fmt[]="cp %s %s";

  int rcd;
  const int fmt_chr_nbr=4;

  /* Construct and execute copy command */
  cp_cmd=(char *)nco_malloc((strlen(cp_cmd_fmt)+strlen(fl_src)+strlen(fl_dst)-fmt_chr_nbr+1UL)*sizeof(char));
  if(dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"Copying %s to %s...",fl_src,fl_dst);
  (void)sprintf(cp_cmd,cp_cmd_fmt,fl_src,fl_dst);
  rcd=system(cp_cmd);
  if(rcd == -1){
    (void)fprintf(stdout,"%s: ERROR nco_fl_cp() is unable to execute cp command \"%s\"\n",prg_nm_get(),cp_cmd);
    nco_exit(EXIT_FAILURE);
  } /* end if */
  cp_cmd=(char *)nco_free(cp_cmd);
  if(dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"done\n");
} /* end nco_fl_cp() */

char * /* O [sng] Canonical file name*/
nco_fl_info_get /* [fnc] Determine canonical filename and properties */
(const char * const fl_nm_lcl) /* I [sng] Name of file */
{
  /* Purpose: Return canonical filename of by removing all symbolic links */
  int rcd;
  char *fl_nm_cnc=NULL; /* [sng] Canonical file name */
  struct stat stat_sct;

  /* Does file exist on local system? */
  rcd=stat(fl_nm_lcl,&stat_sct);
  if(rcd == -1) (void)fprintf(stderr,"%s: INFO File %s does not exist on local system\n",prg_nm_get(),fl_nm_lcl);

  /* Is file a symbolic link? */
  rcd=lstat(fl_nm_lcl,&stat_sct);
#ifndef NECSX
  if(rcd != -1 && S_ISLNK(stat_sct.st_mode)){
#else /* NECSX */
  if(rcd != -1 && ((stat_sct.st_mode & S_IFMT) == S_IFLNK)){ /* fxm: This is BSD not POSIX */
#endif /* NECSX */
#if (defined HAVE_CANONICALIZE_FILE_NAME) && 0
    /* 20040619: Function prototype is not found (perhaps due to aggressive standard switches) by NCO, althouth configure.in finds it */
    /* Remember to free() fl_nm_cnc after using it */
    fl_nm_cnc=(char *)canonicalize_file_name(fl_nm_lcl);
    if(fl_nm_cnc){
      (void)fprintf(stderr,"%s: INFO Local file %s is symbolic link to %s",prg_nm_get(),fl_nm_lcl,fl_nm_cnc);
      fl_nm_cnc=(char *)nco_free(fl_nm_cnc);
    }else{
      (void)fprintf(stderr,"%s: INFO Local file %s is symbolic link but does not canonicalize",prg_nm_get(),fl_nm_lcl);
    } /* endif link canonicalizes */
#else /* !HAVE_CANONICALIZE_FILE_NAME */
    (void)fprintf(stderr,"%s: INFO File %s is a symbolic link\n",prg_nm_get(),fl_nm_lcl);
#endif /* !HAVE_CANONICALIZE_FILE_NAME */
  } /* endif symbolic link */

  return fl_nm_cnc;
} /* end nco_fl_info_get() */

char ** /* O [sng] List of user-specified filenames */
nco_fl_lst_mk /* [fnc] Create file list from command line positional arguments */
(CST_X_PTR_CST_PTR_CST_Y(char,argv), /* I [sng] Argument list */
 const int argc, /* I [nbr] Argument count */
 int arg_crr, /* I [idx] Index of current argument */
 int * const fl_nbr, /* O [nbr] Number of files in input file list */
 char ** const fl_out, /* I/O [sng] Name of output file */
 nco_bool *FL_LST_IN_FROM_STDIN) /* O [flg] fl_lst_in comes from stdin */
{
  /* Purpose: Parse positional arguments on command line
     Name of calling program plays a role in this */

  /* Assume command-line switches have been digested already (e.g., by getopt())
     Assume argv[arg_crr] points to first positional argument (i.e., first argument
     following all switches and their arugments).
     fl_out is filled in if it was not specified as a command line switch
     Multi-file operators take input filenames from positional arguments, if any
     Otherwise, multi-file operators try to get input filenames from stdin */

  nco_bool FL_OUT_FROM_PSN_ARG=True; /* [flg] fl_out comes from positional argument */

  char **fl_lst_in=NULL_CEWI; /* [sng] List of user-specified filenames */

  int idx;
  int fl_nm_sz_wrn=255;
  int psn_arg_fst=0; /* [nbr] Offset for expected number of positional arguments */
  int psn_arg_nbr; /* [nbr] Number of remaining positional arguments */

  int prg_id; /* Program ID */
  prg_id=prg_get(); /* [enm] Program ID */
  psn_arg_nbr=argc-arg_crr; /* [nbr] Number of remaining positional arguments */

  /* Is output file already known from command line switch (i.e., -o fl_out)? */
  if(*fl_out){
    /* fl_out is already known so interpret all remaining positional arguments as input files */
    FL_OUT_FROM_PSN_ARG=False;
    /* Offset expected number of positional arguments by one to account for fl_out being known */
    psn_arg_fst=1;
  } /* end if */

  /* Might there be problems with any specified files? */
  for(idx=arg_crr;idx<argc;idx++){
    if((int)strlen(argv[idx]) >= fl_nm_sz_wrn) (void)fprintf(stderr,"%s: WARNING filename %s is very long (%ld characters) and may not be portable to older operating systems\n",prg_nm_get(),argv[idx],(long)strlen(argv[idx]));
  } /* end loop over idx */

  /* All operators except multi-file operators must have at least one positional argument */
  if(!nco_is_mlt_fl_opr(prg_id) && psn_arg_nbr == 0){
    (void)fprintf(stdout,"%s: ERROR received %d filenames; need at least one\n",prg_nm_get(),psn_arg_nbr);
    (void)nco_usg_prn();
    nco_exit(EXIT_FAILURE);
  } /* end if */

  switch(prg_id){
  case ncks:
  case ncatted:
  case ncrename:
    /* Operators with single fl_in and optional fl_out */
    if(psn_arg_nbr > 2-psn_arg_fst){
      if(FL_OUT_FROM_PSN_ARG) (void)fprintf(stdout,"%s: ERROR received %d filenames; need no more than two\n",prg_nm_get(),psn_arg_nbr); else (void)fprintf(stdout,"%s: ERROR received %d input filenames; need no more than one (output file was specified with -o switch)\n",prg_nm_get(),psn_arg_nbr);
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
    } /* end if */
    fl_lst_in=(char **)nco_malloc(sizeof(char *)); /* fxm: free() this memory sometime */
    fl_lst_in[(*fl_nbr)++]=(char *)strdup(argv[arg_crr++]);
    /* Output file is optional for these operators */
    if(arg_crr == argc-1) *fl_out=(char *)strdup(argv[arg_crr]);
    return fl_lst_in;
    /* break; *//* NB: break after return in case statement causes SGI cc warning */
  case ncbo:
  case ncflint:
    /* Operators with dual fl_in and required fl_out */
    if(psn_arg_nbr != 3-psn_arg_fst){
      if(FL_OUT_FROM_PSN_ARG) (void)fprintf(stdout,"%s: ERROR received %d filenames; need exactly three\n",prg_nm_get(),psn_arg_nbr); else (void)fprintf(stdout,"%s: ERROR received %d input filenames; need exactly two (output file was specified with -o switch)\n",prg_nm_get(),psn_arg_nbr);
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
    } /* end if */
    break;
  case ncap:
  case ncpdq:
  case ncwa:
    /* Operators with single fl_in and required fl_out */
    if(psn_arg_nbr != 2-psn_arg_fst){
      if(FL_OUT_FROM_PSN_ARG) (void)fprintf(stdout,"%s: ERROR received %d filenames; need exactly two\n",prg_nm_get(),psn_arg_nbr); else (void)fprintf(stdout,"%s: ERROR received %d input filenames; need exactly one (output file was specified with -o switch)\n",prg_nm_get(),psn_arg_nbr);
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
    } /* end if */
    break;
  case ncra:
  case ncea:
  case ncrcat:
  case ncecat:
    /* Operators with multiple fl_in and required fl_out */
    if(psn_arg_nbr < 2-psn_arg_fst){

      /* If multi-file operator has no positional arguments for input files... */
      if(nco_is_mlt_fl_opr(prg_id) && ((!FL_OUT_FROM_PSN_ARG && psn_arg_nbr == 0) || (FL_OUT_FROM_PSN_ARG && psn_arg_nbr == 1))){
	/* ...then try to obtain input files from stdin... */
	char *fl_in=NULL; /* [sng] Input file name */
	FILE *fp_in; /* [enm] Input file handle */
	char *bfr_in; /* [sng] Temporary buffer for stdin filenames */
	int cnv_nbr; /* [nbr] Number of scanf conversions performed this scan */
	long fl_lst_in_lng; /* [nbr] Number of characters in input file name list */
	char fmt_sng[10];
	size_t fl_nm_lng; /* [nbr] Filename length */

	if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: DEBUG nco_fl_lst_mk() reports input files not specified as positional arguments. Attempting to read from stdin instead...\n",prg_nm_get());

	/* Initialize information to read stdin */
	fl_lst_in_lng=0L; /* [nbr] Number of characters in input file name list */

	if(fl_in == NULL){
	  fp_in=stdin; /* [enm] Input file handle */
	}else{
	  if((fp_in=fopen(fl_in,"r")) == NULL){
	    (void)fprintf(stderr,"%s: ERROR opening file containing input filename list %s\n",prg_nm_get(),fl_in);
	    nco_exit(EXIT_FAILURE);
	  } /* endif err */
	} /* endelse */

	/* Allocate temporary space for input buffer */
#define FL_NM_IN_MAX_LNG 256 /* [nbr] Maximum length of input file name */
#define FL_LST_IN_MAX_LNG 1000000 /* [nbr] Maximum length of input file list */
	bfr_in=(char *)nco_malloc((FL_NM_IN_MAX_LNG+1L)*sizeof(char));
	(void)sprintf(fmt_sng,"%%%ds\n",FL_NM_IN_MAX_LNG);

	/* Assume filenames are whitespace-separated
	   Format string "%256s\n" tells scanf() to:
	   1. Skip any initial whitespace
	   2. Read first block of non-whitespace characters (up to 256 of them) into buffer
	   3. The \n allows the entries to be separated by carriage returns */
	while(((cnv_nbr=fscanf(fp_in,fmt_sng,bfr_in)) != EOF) && (fl_lst_in_lng < FL_LST_IN_MAX_LNG)){
	  if(cnv_nbr == 0){
	    (void)fprintf(stdout,"%s: ERROR stdin input not convertable to filename. HINT: Maximum length for input filenames is %d characters. HINT: Separate filenames with whitespace. Carriage returns are automatically stripped out.\n",prg_nm_get(),FL_NM_IN_MAX_LNG);
	    nco_exit(EXIT_FAILURE);
	  } /* endif err */
	  fl_nm_lng=strlen(bfr_in);
	  fl_lst_in_lng+=fl_nm_lng;
	  (*fl_nbr)++;
	  if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: DEBUG input file #%d is \"%s\", filename length=%li\n",prg_nm_get(),*fl_nbr,bfr_in,(long)fl_nm_lng);
	  /* Increment file number */
	  fl_lst_in=(char **)nco_realloc(fl_lst_in,(*fl_nbr*sizeof(char *)));
	  fl_lst_in[(*fl_nbr)-1]=(char *)strdup(bfr_in);
	} /* end while */
	/* comp.lang.c 20000212 and http://www.eskimo.com/~scs/C-faq/q12.18.html
	   C FAQ Author Steve Summit explains why not to use fflush()
	   and how best to manually clean stdin of unwanted residue */
#if 0
	/* 20040621: Following flusher does no harm on Linux
	   However, AIX gets caught in an infinite loop here */
	/* Discard characters remainining in stdin */
	char chr_foo;
	while((chr_foo=getchar()) != '\n' && chr_foo != EOF){
	  if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: DEBUG Read and discarded \'%c\'\n",prg_nm_get(),chr_foo);
	} /* end while */
#endif /* endif 0 */

	/* Free temporary buffer */
	bfr_in=(char *)nco_free(bfr_in);

	if(fl_lst_in_lng >= FL_LST_IN_MAX_LNG){
	  (void)fprintf(stdout,"%s: ERROR Total length of fl_lst_in from stdin exceeds %d characters. Possible misuse of feature. If your input file list is really this long, send request to help@nco.sf.net to expand FL_LST_IN_MAX_LNG\n",prg_nm_get(),FL_LST_IN_MAX_LNG);
	  nco_exit(EXIT_FAILURE);
	} /* endif err */

	if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: DEBUG Read %d filenames in %li characters from stdin\n",prg_nm_get(),*fl_nbr,(long)fl_lst_in_lng);
	if(*fl_nbr > 0) *FL_LST_IN_FROM_STDIN=True; else (void)fprintf(stderr,"%s: WARNING Tried and failed to get input filenames from stdin\n",prg_nm_get());

      } /* endif multi-file operator without positional arguments for fl_in */

      if(!*FL_LST_IN_FROM_STDIN){
	if(FL_OUT_FROM_PSN_ARG) (void)fprintf(stdout,"%s: ERROR received %d filenames; need at least two\n",prg_nm_get(),psn_arg_nbr); else (void)fprintf(stdout,"%s: ERROR received %d input filenames; need at least one (output file was specified with -o switch)\n",prg_nm_get(),psn_arg_nbr);
	(void)nco_usg_prn();
	nco_exit(EXIT_FAILURE);
      } /* FL_LST_IN_FROM_STDIN */

    } /* end Operators with multiple fl_in and required fl_out */
    break;
  default: nco_dfl_case_prg_id_err(); break;
  } /* end switch */

  /* If input files are required but have not been obtained yet from stdin */
  if(!*FL_LST_IN_FROM_STDIN){
    /* Fill in input file list from positional arguments */
    /* fxm: valgrind unfree'd memory in nco_fl_lst_mk() (nco_fl_utl.c:264) */
    fl_lst_in=(char **)nco_malloc((psn_arg_nbr-1+psn_arg_fst)*sizeof(char *));
    while(arg_crr < argc-1+psn_arg_fst) fl_lst_in[(*fl_nbr)++]=(char *)strdup(argv[arg_crr++]);
  } /* FL_LST_IN_FROM_STDIN */

  if(*fl_nbr == 0){
    (void)fprintf(stdout,"%s: ERROR Must specify input filename.\n",prg_nm_get());
    (void)nco_usg_prn();
    nco_exit(EXIT_FAILURE);
  } /* end if */

  /* Assign output file from positional argument */
  if(FL_OUT_FROM_PSN_ARG) *fl_out=(char *)strdup(argv[argc-1]);

  return fl_lst_in;

} /* end nco_fl_lst_mk() */

char * /* O [sng] Filename of locally available file */
nco_fl_mk_lcl /* [fnc] Retrieve input file and return local filename */
(char *fl_nm, /* I/O [sng] Current filename, if any (destroyed) */
 const char * const fl_pth_lcl, /* I [sng] Local storage area for files retrieved from remote locations */
 nco_bool * const FILE_RETRIEVED_FROM_REMOTE_LOCATION) /* O [flg] File was retrieved from remote location */
{
  /* Purpose: Locate input file, retrieve it from remote storage system if necessary,
     create local storage directory if neccessary, check file for read-access,
     return name of file on local system */

  FILE *fp_in;
  nco_bool DAP_URL=False; /* DAP handles netCDF API, no retrieval necessary */
  nco_bool FTP_URL=False; /* Retrieve remote file via FTP */
  nco_bool FTP_NETRC=False; /* Retrieve remote file via FTP with .netrc file */
  nco_bool FTP_OR_SFTP_URL; /* FTP or SFTP */
  nco_bool HTTP_URL=False; /* Retrieve remote file via wget */
  nco_bool SFTP_URL=False; /* Retrieve remote file via SFTP */
  nco_bool PTH_LCL_DRV=False; /* Local path was derived (not user-specified) */
  char *cln_ptr; /* [ptr] Colon pointer */
  char *fl_nm_lcl;
  char *fl_nm_lcl_tmp;
  char *fl_nm_stub;
  char *fl_pth_lcl_tmp=NULL;

  const char fnc_nm[]="nco_fl_mk_lcl"; /* [sng] Function name */
  const char ftp_url_sng[]="ftp://";
  const char http_url_sng[]="http://";
  const char sftp_url_sng[]="sftp://";
  int rcd;
  size_t url_sng_lng=0L; /* CEWI */
  struct stat stat_sct;
  int prg_id; /* Program ID */
  prg_id=prg_get(); /* [enm] Program ID */

  /* Assume local filename is input filename */
  fl_nm_lcl=(char *)strdup(fl_nm);

  /* Remove any URL and machine-name components from local filename */
  if(strstr(fl_nm_lcl,sftp_url_sng) == fl_nm_lcl){
    SFTP_URL=True;
    url_sng_lng=strlen(sftp_url_sng);
  }else if(strstr(fl_nm_lcl,ftp_url_sng) == fl_nm_lcl){ /* !sftp */
    FTP_URL=True;
    url_sng_lng=strlen(ftp_url_sng);
  } /* !ftp */
  FTP_OR_SFTP_URL=FTP_URL || SFTP_URL;

  if(FTP_OR_SFTP_URL){
    /* If FTP rearrange fl_nm_lcl to remove ftp://hostname part, and
       if SFTP rearrange fl_nm_lcl to remove sftp://hostname: part,
       before searching for file on local disk */
    fl_pth_lcl_tmp=strchr(fl_nm_lcl+url_sng_lng,'/');
    fl_nm_lcl_tmp=fl_nm_lcl;
    fl_nm_lcl=(char *)nco_malloc(strlen(fl_pth_lcl_tmp)+1UL);
    (void)strcpy(fl_nm_lcl,fl_pth_lcl_tmp);
    fl_nm_lcl_tmp=(char *)nco_free(fl_nm_lcl_tmp);
  }else if(strstr(fl_nm_lcl,http_url_sng) == fl_nm_lcl){
    /* Filename starts with "http://": Try DAP first (if available), then wget. */

#ifdef ENABLE_DAP
    /* Filename has http:// prefix so try DAP access to unadulterated filename */
    int in_id; /* [id] Temporary input file ID */

    /* Attempt nc_open() on HTTP protocol files. Success means DAP found file. */
    rcd=nco_open_flg(fl_nm_lcl,NC_NOWRITE,&in_id);
    
    if(rcd == NC_NOERR){
      /* Fail gracefully if ncatted or ncrename try to use DAP */
      switch(prg_id){
      case ncatted:
      case ncrename:
	(void)fprintf(stderr,"%s: ERROR nco_fl_mk_lcl() reminds you that ncatted and ncrename must process truly local (i.e., not read via DAP) files (fxm TODO nco664)\n",prg_nm_get());
	nco_exit(EXIT_FAILURE);
	break;
      default:
	/* All other operators work with DAP correctly */
	break;
      } /* end switch */
      
      /* Close file to prevent accumulating dangling open files on DAP server */
      rcd=nco_close(in_id);

      /* Great! DAP worked so file has earned DAP identification */
      DAP_URL=True; 

      /* Set rcd=0 to agree with successful stat() so rest of function treats file as local
	 (DAP treats HTTP protocol files as local files) */
      rcd=0;

    }else{ /* DAP-access failed */
      (void)fprintf(stderr,"%s: INFO DAP access to %s failed: Server does not respond, file does not exist, or user does not have read permission\n",prg_nm_get(),fl_nm_lcl);
    } /* DAP-access failed */

#else /* !ENABLE_DAP */
    (void)fprintf(stderr,"%s: INFO Access to %s may (because filename starts with \"http://\") require DAP, but NCO was not enabled with DAP. Instead NCO will try to search for the file locally, and then will try wget.\n",prg_nm_get(),fl_nm_lcl); 
    (void)fprintf(stderr,"%s: HINT: Obtain or build DAP-enabled NCO, e.g., with configure --enable-dap-netcdf ...\n",prg_nm_get());
#endif /* !ENABLE_DAP */

    if(DAP_URL == False){ /* DAP access to http:// file failed */
      /* Attempt to retrieve URLs directly when DAP access fails. Tests:
	 ncks -D 2 -M http://dust.ess.uci.edu/nco/in.nc # wget
	 ncks -D 2 -M -l . http://dust.ess.uci.edu/nco/in.nc # wget
	 ncks -D 2 -M -l . -p http://dust.ess.uci.edu/nco in.nc # wget
	 ncks -D 2 -M -p http://dust.ess.uci.edu/cgi-bin/dods/nph-dods/dodsdata in.nc # DAP */
      if(dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Will first attempt to find on local disk and, if unsuccessful, will then attempt retrieve remote file to local client using wget\n",prg_nm_get());
      
      /* DAP cannot open file so leave DAP_URL=FALSE and set HTTP_URL=True
	 Later we will attempt to wget file to local system */
      HTTP_URL=True;
      url_sng_lng=strlen(http_url_sng);

      /* If HTTP then rearrange fl_nm_lcl to remove http://hostname part
	 before searching for file on local disk */
      fl_pth_lcl_tmp=strchr(fl_nm_lcl+url_sng_lng,'/');
      fl_nm_lcl_tmp=fl_nm_lcl;
      fl_nm_lcl=(char *)nco_malloc(strlen(fl_pth_lcl_tmp)+1UL);
      (void)strcpy(fl_nm_lcl,fl_pth_lcl_tmp);
      fl_nm_lcl_tmp=(char *)nco_free(fl_nm_lcl_tmp);

    } /* !DAP_URL, HTTP_URL */

  }else if((cln_ptr=strchr(fl_nm_lcl,':'))){
    /* 19990804
       Colon separates machine name from filename in rcp, scp, and sftp requests
       However, colon is also legal in _any_ UNIX filename
       Thus whether colon signifies rcp or scp request is somewhat ambiguous
       NCO treats names with more than one colon as regular filenames
       In order for colon to be interpreted as machine name delimiter,
       it must be preceded by period within three or four spaces, e.g., uci.edu: */
    if(((cln_ptr-4 >= fl_nm_lcl) && *(cln_ptr-4) == '.') ||
       ((cln_ptr-3 >= fl_nm_lcl) && *(cln_ptr-3) == '.')){
      /* Rearrange fl_nm_lcl to remove hostname: part */
      fl_pth_lcl_tmp=strchr(fl_nm_lcl+url_sng_lng,'/');
      fl_nm_lcl_tmp=fl_nm_lcl;
      fl_nm_lcl=(char *)nco_malloc(strlen(fl_pth_lcl_tmp)+1UL);
      (void)strcpy(fl_nm_lcl,fl_pth_lcl_tmp);
      fl_nm_lcl_tmp=(char *)nco_free(fl_nm_lcl_tmp);
    } /* endif period is three or four characters from colon */
  } /* end if */

  /* Does file exist on local system? */
  if(!DAP_URL) rcd=stat(fl_nm_lcl,&stat_sct);
  if(rcd == -1 && (dbg_lvl_get() >= nco_dbg_fl)) (void)fprintf(stderr,"%s: INFO stat() #1 failed: %s does not exist\n",prg_nm_get(),fl_nm_lcl);

  /* If not, check if file exists on local system under same path interpreted relative to current working directory */
  if(rcd == -1){
    if(fl_nm_lcl[0] == '/'){
      rcd=stat(fl_nm_lcl+1UL,&stat_sct);
      if(rcd == -1 && (dbg_lvl_get() >= nco_dbg_fl)) (void)fprintf(stderr,"%s: INFO stat() #2 failed: %s does not exist\n",prg_nm_get(),fl_nm_lcl+1UL);
    } /* end if */
    if(rcd == 0){
      /* NB: Adding one to filename pointer is like deleting initial slash on filename
	 Then free(fl_nm_lcl) would miss this initial byte (memory is lost)
	 Hence must copy new name into its own memory space */
      fl_nm_lcl_tmp=(char *)strdup(fl_nm_lcl+1UL);
      fl_nm_lcl=(char *)nco_free(fl_nm_lcl);
      fl_nm_lcl=fl_nm_lcl_tmp;
      (void)fprintf(stderr,"%s: WARNING not searching for %s on remote filesystem, using local file %s instead\n",prg_nm_get(),fl_nm,fl_nm_lcl+1UL);
    } /* end if */
  } /* end if */

  /* Finally, check if file exists locally in directory for remotely retrieved files
     This occurs when previous program invocations have already retrieved some files */
  if(rcd == -1){
    /* Where does filename stub begin? 
       NB: Assume local filename has a slash (because remote file system always has a slash) */
    fl_nm_stub=strrchr(fl_nm_lcl,'/')+1UL;

    /* Construct local filename from user-supplied local file path and existing file stub */
    if(fl_pth_lcl){
      fl_nm_lcl_tmp=fl_nm_lcl;
      /* Allocate enough room for the joining slash '/' and the terminating NUL */
      fl_nm_lcl=(char *)nco_malloc((strlen(fl_pth_lcl)+strlen(fl_nm_stub)+2)*sizeof(char));
      (void)strcpy(fl_nm_lcl,fl_pth_lcl);
      (void)strcat(fl_nm_lcl,"/");
      (void)strcat(fl_nm_lcl,fl_nm_stub);
      /* Free old filename space */
      fl_nm_lcl_tmp=(char *)nco_free(fl_nm_lcl_tmp);
    } /* end if */

    /* At last, check for file in local storage directory */
    rcd=stat(fl_nm_lcl,&stat_sct);
    if(rcd != -1) (void)fprintf(stderr,"%s: WARNING not searching for %s on remote filesystem, using local file %s instead\n",prg_nm_get(),fl_nm,fl_nm_lcl);
    if(rcd == -1 && (dbg_lvl_get() >= nco_dbg_fl)) (void)fprintf(stderr,"%s: INFO stat() #3 failed: %s does not exist\n",prg_nm_get(),fl_nm_lcl);
  } /* end if */

  /* File was not found locally and is not DAP-accessible, try to fetch file from remote filesystem */
  if(rcd == -1){

    typedef struct{ /* [enm] Remote fetch command structure */
      const char *fmt; /* [] Format */
      const int fmt_chr_nbr; /* [nbr] Number of formatting characters */
      const int transfer_mode; /* [enm] Transfer mode */
      const int file_order; /* [enm] File order */
    } rmt_fch_cmd_sct;

    char *cmd_sys;
    char *fl_nm_rmt;
    char *fmt_ftp=NULL; /* [sng] Declare outside FTP block scope for easier freeing */

    /* fxm: use autoconf HAVE_MKDIR_M rather than SUN4 token TODO nco293 */
#ifndef SUN4
    const char cmd_mkdir[]="mkdir -m 777 -p";
#else /* SUN4 */
    const char cmd_mkdir[]="mkdir -p";
#endif /* SUN4 */

    enum{ /* [enm] Transfer mode */
      synchronous, /* Syncronous transfer */
      asynchronous}; /* Asynchronous transfer */

    enum{ /* [enm] File order */
      lcl_rmt, /* Local file argument before remote file argument */
      rmt_lcl}; /* Remote file argument before local file argument */

    size_t fl_pth_lcl_lng=size_t_CEWI; /* CEWI */

    rmt_fch_cmd_sct *rmt_cmd=NULL;
    /* fxm: Initialize structure contents as const */
    rmt_fch_cmd_sct msread={"msread -R %s %s",4,synchronous,lcl_rmt};
    rmt_fch_cmd_sct msrcp={"msrcp mss:%s %s",4,synchronous,rmt_lcl};
    rmt_fch_cmd_sct nrnet={"nrnet msget %s r flnm=%s l mail=FAIL",4,asynchronous,lcl_rmt};
    /* rmt_fch_cmd_sct rcp={"rcp -p %s %s",4,synchronous,rmt_lcl};*/
    /* wget -r: re-download file (clobber existing file of same name, if any) */
    rmt_fch_cmd_sct http={"wget -r --output-document=%s %s",4,synchronous,lcl_rmt};
    rmt_fch_cmd_sct scp={"scp -p %s %s",4,synchronous,rmt_lcl};
    rmt_fch_cmd_sct sftp={"sftp %s %s",4,synchronous,rmt_lcl};
    /* Fill in ftp structure fmt element dynamically later */
    rmt_fch_cmd_sct ftp={"",4,synchronous,rmt_lcl};

    /* Why did stat() command fail? */
    /* (void)perror(prg_nm_get());*/

    /* Remote filename is input filename by definition */
    fl_nm_rmt=fl_nm;

    /* URL specifier in filename unambiguously signals to use FTP */
    if(rmt_cmd == NULL){
      if(FTP_URL){
	/* fxm: use autoconf HAVE_XXX rather than WIN32 token TODO nco292 */
#ifdef WIN32
/* #ifndef HAVE_NETWORK fxm */
	/* I have no idea how networking calls work in MS Windows, so just exit */
	(void)fprintf(stdout,"%s: ERROR Networking required to obtain %s is not supported by this operating system\n",prg_nm_get(),fl_nm_rmt);
	nco_exit(EXIT_FAILURE);
#else /* !WIN32 */
	char *usr_nm;
	char *host_nm_lcl;
	char *host_nm_rmt;
	char *usr_email=NULL; /* CEWI */
	char *fl_nm_netrc;

	const char fl_stb_netrc[]="/.netrc";
	const char ftp_cmd_anonymous[]="/usr/bin/ftp -i -p -n"; /* -n turns off using .netrc */
	const char ftp_cmd_netrc[]="/usr/bin/ftp -i -p"; /* Allow FTP to use .netrc */
	const char fmt_ftp_anonymous_tpl[]="%s %s << END\nuser anonymous %s\nbin\nget %s %s\nquit\nEND";
	const char fmt_ftp_netrc_tpl[]="%s %s << END\nbin\nget %s %s\nquit\nEND";

	struct passwd *usr_pwd;

	uid_t usr_uid;

	rmt_cmd=&ftp;

	/* Get UID to get password structure which contains home directory, login name
	   Home directory needed to search for .netrc
	   Login name used to construct e-mail address for anonymous FTP */
	usr_uid=getuid();
	usr_pwd=getpwuid(usr_uid);
	usr_nm=usr_pwd->pw_name;

	/* Construct remote hostname and filename now since:
	   1. .netrc, if any, will soon be searched for remote hostname
	   2. Remote hostname and filename always needed for remote retrieval */

	/* Remote hostname begins directly after "[s]ftp://" */
	host_nm_rmt=fl_nm_rmt+url_sng_lng;
	/* Filename begins after slash */
	fl_nm_rmt=strstr(fl_nm_rmt+url_sng_lng,"/")+1UL;
	/* NUL-terminate hostname by overwriting colon in hostname:/filename syntax */
	*(fl_nm_rmt-1)='\0';

	/* Use anonymous FTP unless .netrc file exists on local system */
	fl_nm_netrc=(char *)strdup(usr_pwd->pw_dir);
	/* Create space for full path to ${HOME}/.netrc */
	fl_nm_netrc=(char *)nco_realloc(fl_nm_netrc,(strlen(fl_nm_netrc)+strlen(fl_stb_netrc)+2UL)*sizeof(char));
	fl_nm_netrc=(char *)strcat(fl_nm_netrc,fl_stb_netrc);
	rcd=stat(fl_nm_netrc,&stat_sct);
	if(!rcd){
	  /* Search .netrc file for remote host name */
	  char *host_nm_rmt_psn;
	  char *fl_netrc_bfr;
	  FILE *fp_netrc; /* [fl] .netrc inpu file handle */
	  if((fp_netrc=fopen(fl_nm_netrc,"r")) == NULL){
	    (void)fprintf(stderr,"%s: ERROR unable to open user's .netrc file %s\n",prg_nm_get(),fl_nm_netrc);
	    /* Why did fopen() command fail? */
	    (void)perror(prg_nm_get());
	    nco_exit(EXIT_FAILURE);
	  } /* end if */
	  /* Add one for NUL-terminator */
	  fl_netrc_bfr=(char *)nco_malloc((1UL+stat_sct.st_size)*sizeof(char));
	  /* NUL-terminate buffer */
	  fl_netrc_bfr[stat_sct.st_size]='\0';
	  rcd=fread((void *)fl_netrc_bfr,stat_sct.st_size,1,fp_netrc);
	  if(rcd <= 0){
	    (void)fprintf(stderr,"%s: ERROR reading %s\n",prg_nm_get(),fl_nm_netrc);
	    /* Why did fread() command fail? */
	    (void)perror(prg_nm_get());
	    /* Check for feof() vs. ferror() here? */
	    rcd=fclose(fp_netrc);
	    nco_exit(EXIT_FAILURE);
	  } /* end if */
	  rcd=fclose(fp_netrc);
	  host_nm_rmt_psn=strstr(fl_netrc_bfr,host_nm_rmt);
	  if(host_nm_rmt_psn){
	    FTP_NETRC=True;
	    if(dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s() will use .netrc file at %s instead of anonymous FTP\n",prg_nm_get(),fnc_nm,fl_nm_netrc);
	  } /* endif host_nm_rmt_psn */
	  fl_netrc_bfr=(char *)nco_free(fl_netrc_bfr);
	} /* endif rcd */

	if(!FTP_NETRC){
	  /* DEBUG: 256 should be replaced by MAXHOSTNAMELEN from <sys/param.h>, but
	     MAXHOSTNAMELEN isn't in there on Solaris */
	  host_nm_lcl=(char *)nco_malloc((256UL+1UL)*sizeof(char));
	  (void)gethostname(host_nm_lcl,256UL+1UL);
	  /* fxm: move to gethostbyname() next */
	  if(strchr(host_nm_lcl,'.') == NULL){
/* #ifdef HAVE_RES_ */
	    /* Returned hostname did not include fully qualified Internet domain name (FQDN) */
	    (void)res_init();
	    (void)strcat(host_nm_lcl,".");
	    (void)strcat(host_nm_lcl,_res.defdname);
/* #endif HAVE_RES_ */
	  } /* end if */

	  /* Add one for joining "@" and one for NUL byte */
	  usr_email=(char *)nco_malloc((strlen(usr_nm)+1UL+strlen(host_nm_lcl)+1UL)*sizeof(char));
	  (void)sprintf(usr_email,"%s@%s",usr_nm,host_nm_lcl);
	  /* Free hostname space */
	  host_nm_lcl=(char *)nco_free(host_nm_lcl);
	} /* FTP_NETRC */

	if(FTP_NETRC){
	  /* Subtract four characters for two "percent s" formats replaced by new strings, add one for NUL byte */
	  fmt_ftp=(char *)nco_malloc((strlen(fmt_ftp_netrc_tpl)+strlen(ftp_cmd_netrc)+strlen(host_nm_rmt)-4UL+1UL)*sizeof(char));
	  (void)sprintf(fmt_ftp,fmt_ftp_netrc_tpl,ftp_cmd_netrc,host_nm_rmt,"%s","%s");
	}else{
	  /* Subtract six characters for three "percent s" formats replaced by new strings, add one for NUL byte */
	  fmt_ftp=(char *)nco_malloc((strlen(fmt_ftp_anonymous_tpl)+strlen(ftp_cmd_anonymous)+strlen(host_nm_rmt)+strlen(usr_email)-6UL+1UL)*sizeof(char));
	  (void)sprintf(fmt_ftp,fmt_ftp_anonymous_tpl,ftp_cmd_anonymous,host_nm_rmt,usr_email,"%s","%s");
	} /* !FTP_NETRC */
	rmt_cmd->fmt=fmt_ftp;
	/* Free space, if any, holding user's E-mail address */
	if(!FTP_NETRC) usr_email=(char *)nco_free(usr_email);
	/* Always free .netrc filename space */
	fl_nm_netrc=(char *)nco_free(fl_nm_netrc);
#endif /* !WIN32 */
      } /* end if FTP_URL */
    } /* end if rmt_cmd */

    /* Currently, sftp transfers are indicated by FTP-style URLs
       NB: Unlike FTP, SFTP does not have a recognized URL format
       Hence actual transfer via SFTP uses scp syntax (for single files)
       Multiple file transfer via SFTP can use FTP-like scripts, requires more work
       NCO SFTP file specification must have colon separating hostname from filename */
    if(rmt_cmd == NULL){
      if(SFTP_URL){
	/* Remote filename begins after URL but includes hostname */
	fl_nm_rmt+=url_sng_lng;
	/* Make sure that SFTP hostname looks like a hostname */
	if((cln_ptr=strchr(fl_nm_rmt,':'))){
	  if(((cln_ptr-4 >= fl_nm_rmt) && *(cln_ptr-4) == '.') ||
	     ((cln_ptr-3 >= fl_nm_rmt) && *(cln_ptr-3) == '.')){
	    rmt_cmd=&sftp;
	  } /* end if */
	} /* end if colon */
      } /* end if SFTP */
    } /* end if rmt_cmd */

    /* Attempt wget on files that contain http:// prefix and are not accessible via DAP */
    if(rmt_cmd == NULL){
      if(HTTP_URL){
	rmt_cmd=&http;
	(void)fprintf(stderr,"%s: INFO Will now attempt wget on the full filepath. wget will likely fail if the file is truly behind a DAP server. Unfortunately, even a failed wget attempt creates a rather long pathname in the current directory. fxm TODO nco970, nco971. On the other hand, wget should succeed if the file is stored in any publicly-accessible web location.\n",prg_nm_get());
      } /* end if HTTP */
    } /* end if rmt_cmd */

    /* Otherwise, single colon preceded by period in filename signals rcp or scp
       Determining whether to try scp instead of rcp is difficult
       Ideally, NCO would test remote machine for rcp/scp priveleges with system command like, e.g., "ssh echo ok"
       To start we use scp which has its own fall-through to rcp */
    if(rmt_cmd == NULL){
      if((cln_ptr=strchr(fl_nm_rmt,':'))){
	if(((cln_ptr-4 >= fl_nm_rmt) && *(cln_ptr-4) == '.') ||
	   ((cln_ptr-3 >= fl_nm_rmt) && *(cln_ptr-3) == '.')){
	  rmt_cmd=&scp;
	} /* end if */
      } /* end if colon */
    } /* end if rmt_cmd */

    if(rmt_cmd == NULL){
      /* Does msrcp command exist on local system? */
      rcd=stat("/usr/local/bin/msrcp",&stat_sct); /* SCD Dataproc, Ouray */
      if(rcd != 0) rcd=stat("/usr/bin/msrcp",&stat_sct); /* ACD Linux */
      if(rcd != 0) rcd=stat("/opt/local/bin/msrcp",&stat_sct); /* CGD */
      if(rcd != 0) rcd=stat("/usr/local/dcs/bin/msrcp",&stat_sct); /* ACD */
      if(rcd == 0) rmt_cmd=&msrcp;
    } /* end if */

    if(rmt_cmd == NULL){
      /* Does msread command exist on local system? */
      rcd=stat("/usr/local/bin/msread",&stat_sct);
      if(rcd == 0) rmt_cmd=&msread;
    } /* end if */

    if(rmt_cmd == NULL){
      /* Does nrnet command exist on local system? */
      rcd=stat("/usr/local/bin/nrnet",&stat_sct);
      if(rcd == 0) rmt_cmd=&nrnet;
    } /* end if */

    /* Before we look for file on remote system, make sure
       filename has correct syntax to exist on remote system */
    if(rmt_cmd == &msread || rmt_cmd == &nrnet || rmt_cmd == &msrcp){
      if (fl_nm_rmt[0] != '/' || fl_nm_rmt[1] < 'A' || fl_nm_rmt[1] > 'Z'){
	(void)fprintf(stderr,"%s: ERROR %s is not on local filesystem and is not a syntactically valid filename on remote file system\n",prg_nm_get(),fl_nm_rmt);
	nco_exit(EXIT_FAILURE);
      } /* end if */
    } /* end if */

    if(rmt_cmd == NULL){
      (void)fprintf(stderr,"%s: ERROR %s neither exists locally nor matches remote filename patterns\n",prg_nm_get(),fl_nm_rmt);
      nco_exit(EXIT_FAILURE);
    } /* end if */

    if(fl_pth_lcl == NULL){
      /* Derive path for storing local file from remote filename */
      (void)fprintf(stderr,"%s: INFO deriving local filepath from remote filename\n",prg_nm_get());
      fl_nm_stub=strrchr(fl_nm_lcl,'/')+1UL;
      if(HTTP_URL){
	/* Strip leading slash from fl_nm_lcl for HTTP files so, e.g., 
	   http://dust.ess.uci.edu/nco/in.nc produces local path "nco" not "/nco" */
	fl_nm_lcl_tmp=(char *)strdup(fl_nm_lcl+1UL);
	fl_nm_lcl=(char *)nco_free(fl_nm_lcl);
	fl_nm_lcl=fl_nm_lcl_tmp;
      } /* !HTTP_URL */
      /* Construct local storage filepath name */
      fl_pth_lcl_lng=strlen(fl_nm_lcl)-strlen(fl_nm_stub)-1UL;
      /* Allocate enough room for terminating NUL */
      fl_pth_lcl_tmp=(char *)nco_malloc((fl_pth_lcl_lng+1UL)*sizeof(char));
      (void)strncpy(fl_pth_lcl_tmp,fl_nm_lcl,fl_pth_lcl_lng);
      fl_pth_lcl_tmp[fl_pth_lcl_lng]='\0';
      PTH_LCL_DRV=True; /* Local path was derived (not user-specified) */
      /* Tell user what local filepath was derived */
      (void)fprintf(stderr,"%s: INFO Retrieved files will be stored in directory ./%s\n",prg_nm_get(),fl_pth_lcl_tmp);
    }else{
      /* Copy user-specified local path to unite following code in terms of fl_pth_lcl_tmp */
      fl_pth_lcl_tmp=(char *)strdup(fl_pth_lcl);
    } /* fl_pth_lcl */
    
    /* Does local filepath exist already on local system? */
    rcd=stat(fl_pth_lcl_tmp,&stat_sct);
    /* If not, then create local filepath */
    if(rcd != 0){
      /* Allocate enough room for joining space ' ' and terminating NUL */
      cmd_sys=(char *)nco_malloc((strlen(cmd_mkdir)+fl_pth_lcl_lng+2UL)*sizeof(char));
      (void)strcpy(cmd_sys,cmd_mkdir);
      (void)strcat(cmd_sys," ");
      (void)strcat(cmd_sys,fl_pth_lcl_tmp);
      rcd=system(cmd_sys);
      if(rcd != 0){
	(void)fprintf(stderr,"%s: ERROR Unable to create local directory %s\n",prg_nm_get(),fl_pth_lcl_tmp);
	if(fl_pth_lcl == NULL) (void)fprintf(stderr,"%s: HINT Use -l option\n",prg_nm_get());
	nco_exit(EXIT_FAILURE);
      } /* end if */
      if(dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Created local directory ./%s\n",prg_nm_get(),fl_pth_lcl_tmp);
      /* Free local command space */
      cmd_sys=(char *)nco_free(cmd_sys);
    } /* end if */

    /* Free local path space, if any */
    fl_pth_lcl_tmp=(char *)nco_free(fl_pth_lcl_tmp);

    /* Allocate enough room for joining space ' ' and terminating NUL */
    cmd_sys=(char *)nco_malloc((strlen(rmt_cmd->fmt)-rmt_cmd->fmt_chr_nbr+strlen(fl_nm_lcl)+strlen(fl_nm_rmt)+2)*sizeof(char));
    if(rmt_cmd->file_order == lcl_rmt){
      (void)sprintf(cmd_sys,rmt_cmd->fmt,fl_nm_lcl,fl_nm_rmt);
    }else{
      (void)sprintf(cmd_sys,rmt_cmd->fmt,fl_nm_rmt,fl_nm_lcl);
    } /* end else */
    if(dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: Retrieving file from remote location with command:\n%s\n",prg_nm_get(),cmd_sys);
    (void)fflush(stderr);
    /* Fetch file from remote file system */
    rcd=system(cmd_sys);
    /* Free local command space */
    cmd_sys=(char *)nco_free(cmd_sys);

    /* Free dynamically allocated FTP script memory */
    if(rmt_cmd == &ftp) fmt_ftp=(char *)nco_free(fmt_ftp);

    if(rmt_cmd->transfer_mode == synchronous){
      if(rcd != 0){
	(void)fprintf(stderr,"%s: ERROR Synchronous fetch command failed\n",prg_nm_get());
	nco_exit(EXIT_FAILURE);
      } /* end if */
    }else{
      /* This is appropriate place to insert invocation of shell command
	 to retrieve file asynchronously and return status to NCO synchronously. */

      int fl_sz_crr=-2;
      int fl_sz_ntl=-1;
      int tm_nbr=100; /* Maximum number of sleep periods before error exit */
      int tm_idx;
      int tm_sleep_scn=10; /* [s] Seconds per stat() check for successful return */

      /* Asynchronous retrieval uses sleep-and-poll technique */
      for(tm_idx=0;tm_idx<tm_nbr;tm_idx++){
	rcd=stat(fl_nm_lcl,&stat_sct);
	if(rcd == 0){
	  /* What is current size of file? */
	  fl_sz_ntl=fl_sz_crr;
	  fl_sz_crr=stat_sct.st_size;
	  /* If file size does not change during entire sleep period, assume
	     file is completely retrieved. */
	  if(fl_sz_ntl == fl_sz_crr){
	    break;
	  } /* end if */
	} /* end if */
	/* Sleep for specified time */
	(void)sleep((unsigned)tm_sleep_scn);
	if(dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,".");
	(void)fflush(stderr);
      } /* end for */
      if(tm_idx == tm_nbr){
	(void)fprintf(stderr,"%s: ERROR Maximum time (%d seconds = %.1f minutes) for asynchronous file retrieval exceeded.\n",prg_nm_get(),tm_nbr*tm_sleep_scn,tm_nbr*tm_sleep_scn/60.0);
	nco_exit(EXIT_FAILURE);
      } /* end if */
      if(dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"\n%s Retrieval successful after %d sleeps of %d seconds each = %.1f minutes\n",prg_nm_get(),tm_idx,tm_sleep_scn,tm_idx*tm_sleep_scn/60.0);
    } /* end else transfer mode is asynchronous */
    *FILE_RETRIEVED_FROM_REMOTE_LOCATION=True;
  }else{ /* end if input file did not exist locally */
    *FILE_RETRIEVED_FROM_REMOTE_LOCATION=False;
  } /* end if file was already on the local system */

  if(dbg_lvl_get() >= nco_dbg_fl)
    if(DAP_URL && fl_pth_lcl)
      (void)fprintf(stderr,"%s: INFO User-specified option \"-l %s\" was not used since input file was not retrieved from remote location\n",prg_nm_get(),fl_pth_lcl);

  if(!DAP_URL){
    /* File is (now, anyway) truly local---does local system have read permission? */
    if((fp_in=fopen(fl_nm_lcl,"r")) == NULL){
      (void)fprintf(stderr,"%s: ERROR User does not have read permission for %s, or file does not exist\n",prg_nm_get(),fl_nm_lcl);
      nco_exit(EXIT_FAILURE);
    }else{
      (void)fclose(fp_in);
    } /* end else */
    
      /* For local files, perform optional file diagnostics */
    if(dbg_lvl_get() >= nco_dbg_std){
      char *fl_nm_cnc=NULL; /* [sng] Canonical file name */
      /* Determine canonical filename and properties */
      fl_nm_cnc=nco_fl_info_get(fl_nm_lcl);
      if(fl_nm_cnc) fl_nm_cnc=(char *)nco_free(fl_nm_cnc);
    } /* endif dbg */
    
  } /* end if file is truly local */
  
    /* Free input filename space */
  fl_nm=(char *)nco_free(fl_nm);

  /* Return local filename */
  return(fl_nm_lcl);

} /* end nco_fl_mk_lcl() */

void
nco_fl_mv /* [fnc] Move first file to second */
(const char * const fl_src, /* I [sng] Name of source file to move */
 const char * const fl_dst) /* I [sng] Name of destination file */
{
  /* Purpose: Move first file to second */
  char *mv_cmd;
  const char mv_cmd_fmt[]="mv -f %s %s";

  int rcd;
  const int fmt_chr_nbr=4;

  /* Construct and execute copy command */
  mv_cmd=(char *)nco_malloc((strlen(mv_cmd_fmt)+strlen(fl_src)+strlen(fl_dst)-fmt_chr_nbr+1UL)*sizeof(char));
  if(dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Moving %s to %s...",prg_nm_get(),fl_src,fl_dst);
  (void)sprintf(mv_cmd,mv_cmd_fmt,fl_src,fl_dst);
  rcd=system(mv_cmd);
  if(rcd == -1){
    (void)fprintf(stdout,"%s: ERROR nco_fl_mv() unable to execute mv command \"%s\"\n",prg_nm_get(),mv_cmd);
    nco_exit(EXIT_FAILURE);
  } /* end if */
  mv_cmd=(char *)nco_free(mv_cmd);
  if(dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"done\n");
} /* end nco_fl_mv() */

char * /* O [sng] Name of file to retrieve */
nco_fl_nm_prs /* [fnc] Construct file name from input arguments */
(char *fl_nm, /* I/O [sng] Current filename, if any */
 const int fl_idx, /* I [nbr] Ordinal index of file in input file list */
 int * const fl_nbr, /* I/O [nbr] Number of files to be processed */
 char * const * const fl_lst_in, /* I [sng] User-specified filenames */
 const int abb_arg_nbr, /* I [nbr] Number of abbreviation arguments */
 CST_X_PTR_CST_PTR_CST_Y(char,fl_lst_abb), /* I [sng] NINTAP-style arguments, if any */
 const char * const fl_pth) /* I [sng] Path prefix for files in fl_lst_in */
{
  /* Purpose: Construct file name from various input arguments and switches
     Routine implements NINTAP-style specification by using static
     memory to avoid repetition in construction of filename */

  static short FIRST_INVOCATION=True;

  static char *fl_nm_1st_dgt;
  static char *fl_nm_nbr_sng;
  static char fl_nm_nbr_sng_fmt[10];

  static int fl_nm_nbr_crr;
  static int fl_nm_nbr_dgt;
  static int fl_nm_nbr_ncr;
  static int fl_nm_nbr_max;
  static int fl_nm_nbr_min;
  static int fl_nm_nbr_ttl;

  /* Free any old filename space */
  fl_nm=(char *)nco_free(fl_nm);

  /* Construct filename from NINTAP-style arguments and input name */
  if(fl_lst_abb){
    if(FIRST_INVOCATION){
      int fl_nm_sfx_lng=0;

      /* Parse abbreviation list analogously to CCM Processor ICP "NINTAP" */
      if(fl_nbr) *fl_nbr=(int)strtol(fl_lst_abb[0],(char **)NULL,10);
      fl_nm_nbr_ttl=*fl_nbr;

      if(abb_arg_nbr > 1){
	fl_nm_nbr_dgt=(int)strtol(fl_lst_abb[1],(char **)NULL,10);
      }else{
	fl_nm_nbr_dgt=3;
      }/* end if */

      if(abb_arg_nbr > 2){
	fl_nm_nbr_ncr=(int)strtol(fl_lst_abb[2],(char **)NULL,10);
      }else{
	fl_nm_nbr_ncr=1;
      } /* end if */

      if(abb_arg_nbr > 3){
	fl_nm_nbr_max=(int)strtol(fl_lst_abb[3],(char **)NULL,10);
      }else{
	fl_nm_nbr_max=0;
      } /* end if */

      if(abb_arg_nbr > 4){
	fl_nm_nbr_min=(int)strtol(fl_lst_abb[4],(char **)NULL,10);
      }else{
	fl_nm_nbr_min=1;
      } /* end if */

      /* Is there a .nc, .cdf, .hdf, or .hd5 suffix? */
      if(strncmp(fl_lst_in[0]+strlen(fl_lst_in[0])-3,".nc",3) == 0)
	fl_nm_sfx_lng=3;
      else if(strncmp(fl_lst_in[0]+strlen(fl_lst_in[0])-4,".cdf",4) == 0)
	fl_nm_sfx_lng=4;
      else if(strncmp(fl_lst_in[0]+strlen(fl_lst_in[0])-4,".hdf",4) == 0)
	fl_nm_sfx_lng=4;
      else if(strncmp(fl_lst_in[0]+strlen(fl_lst_in[0])-4,".hd5",4) == 0)
	fl_nm_sfx_lng=4;

      /* Initialize static information useful for future invocations */
      fl_nm_1st_dgt=fl_lst_in[0]+strlen(fl_lst_in[0])-fl_nm_nbr_dgt-fl_nm_sfx_lng;
      fl_nm_nbr_sng=(char *)nco_malloc((size_t)(fl_nm_nbr_dgt+1UL)*sizeof(char));
      fl_nm_nbr_sng=strncpy(fl_nm_nbr_sng,fl_nm_1st_dgt,(size_t)fl_nm_nbr_dgt);
      fl_nm_nbr_sng[fl_nm_nbr_dgt]='\0';
      fl_nm_nbr_crr=(int)strtol(fl_nm_nbr_sng,(char **)NULL,10);
      (void)sprintf(fl_nm_nbr_sng_fmt,"%%0%dd",fl_nm_nbr_dgt);

      /* First filename is always specified on command line anyway... */
      fl_nm=(char *)strdup(fl_lst_in[0]);

      /* Set flag that this routine has already been invoked at least once */
      FIRST_INVOCATION=False;

    }else{ /* end if FIRST_INVOCATION */
      /* Create current filename from previous filename */
      fl_nm_nbr_crr+=fl_nm_nbr_ncr;
      if(fl_nm_nbr_max)
	if(fl_nm_nbr_crr > fl_nm_nbr_max)
	  fl_nm_nbr_crr=fl_nm_nbr_min;
      (void)sprintf(fl_nm_nbr_sng,fl_nm_nbr_sng_fmt,fl_nm_nbr_crr);
      fl_nm=(char *)strdup(fl_lst_in[0]);
      (void)strncpy(fl_nm+(fl_nm_1st_dgt-fl_lst_in[0]),fl_nm_nbr_sng,(size_t)fl_nm_nbr_dgt);
      if(fl_idx == fl_nm_nbr_ttl-1) fl_nm_nbr_sng=(char *)nco_free(fl_nm_nbr_sng);
    } /* end if not FIRST_INVOCATION */
  }else{ /* end if abbreviation list */
    fl_nm=(char *)strdup(fl_lst_in[fl_idx]);
  } /* end if no abbreviation list */

  /* Prepend path prefix */
  if(fl_pth){
    char *fl_nm_stub;

    fl_nm_stub=fl_nm;

    /* Allocate enough room for joining slash '/' and terminating NUL */
    fl_nm=(char *)nco_malloc((strlen(fl_nm_stub)+strlen(fl_pth)+2)*sizeof(char));
    (void)strcpy(fl_nm,fl_pth);
    (void)strcat(fl_nm,"/");
    (void)strcat(fl_nm,fl_nm_stub);

    /* Free filestub space */
    fl_nm_stub=(char *)nco_free(fl_nm_stub);
  } /* end if */

  /* Return new filename */
  return(fl_nm);
} /* end nco_fl_nm_prs() */

char * /* O [sng] Name of temporary file actually opened */
nco_fl_out_open /* [fnc] Open output file subject to availability and user input */
(const char * const fl_out, /* I [sng] Name of file to open */
 const nco_bool FORCE_APPEND, /* I [flg] Append to existing file, if any */
 const nco_bool FORCE_OVERWRITE, /* I [flg] Overwrite existing file, if any */
 const int fl_out_fmt, /* I [enm] Output file format */
 int * const out_id) /* O [id] File ID */
{
  /* Purpose: Open output file subject to availability and user input
     In accord with netCDF philosophy, open temporary file named according
     to fl_out and process ID so that errors cannot infect intended output file.
     Calling routine has responsibility to close and free fl_out_tmp */

  char *fl_out_tmp;
  char *pid_sng; /* String containing decimal representation of PID */

  const char tmp_sng_1[]="pid"; /* Extra string appended to temporary filenames */
  const char tmp_sng_2[]="tmp"; /* Extra string appended to temporary filenames */

  int nccreate_mode; /* [enm] Mode flag for nco_create() call */
  int rcd; /* [rcd] Return code */

  long fl_out_tmp_lng; /* [nbr] Length of temporary file name */
  long pid_sng_lng; /* [nbr] Theoretical length of decimal representation of this PID */
  long pid_sng_lng_max; /* [nbr] Maximum length of decimal representation of any PID */

  pid_t pid; /* Process ID */

  struct stat stat_sct;

  /* Make sure output is possible */
#ifndef ENABLE_NETCDF4
  if(fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC){
    (void)fprintf(stdout,"%s: ERROR Requested netCDF4-format output file but NCO was not built with netCDF4 support\n",prg_nm_get());
    (void)fprintf(stdout,"%s: HINT: Obtain or build a netCDF4-enabled version of NCO.  Try, e.g., ./configure --enable-netcdf4 ...;make;make install\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* netCDF4 */
#endif /* ENABLE_NETCDF4 */

  /* Set default clobber mode (clobber) then modify for specified file format */
  nccreate_mode=NC_CLOBBER; /* [enm] Mode flag for nco_create() call */

  /* [fnc] Merge clobber mode with user-specified file format */
  nccreate_mode=nco_create_mode_mrg(nccreate_mode,fl_out_fmt);

  if(FORCE_OVERWRITE && FORCE_APPEND){
    (void)fprintf(stdout,"%s: ERROR FORCE_OVERWRITE and FORCE_APPEND are both set\n",prg_nm_get());
    (void)fprintf(stdout,"%s: HINT: Overwrite (-O) and Append (-A) options are mutually exclusive. Re-run your command, setting at most one of these switches.\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* end if */

  /* Generate unique temporary file name
     System routines tempnam(), tmpname(), mktemp() perform a similar function, but are OS dependent
     Maximum length of PID depends on pid_t
     Until about 1995 most OSs set pid_t = short = 16 or 32 bits
     Now some OSs have /usr/include/sys/types.h set pid_t = long = 32 or 64 bits
     20000126: Use sizeof(pid_t) rather than hardcoded size to fix longstanding bug on SGIs
  */
  /* Maximum length of decimal representation of PID is number of bits in PID times log10(2) */
  pid_sng_lng_max=(long)ceil(8*sizeof(pid_t)*log10(2.0));
  pid_sng=(char *)nco_malloc((pid_sng_lng_max+1UL)*sizeof(char));
  pid=getpid();
  (void)sprintf(pid_sng,"%ld",(long)pid);
  /* Theoretical length of decimal representation of PID is 1+ceil(log10(PID)) where the 1 is required iff PID is an exact power of 10 */
  pid_sng_lng=1L+(long)ceil(log10((double)pid));
  /* NCO temporary file name is user-specified file name + "." + tmp_sng_1 + PID + "." + prg_nm + "." + tmp_sng_2 + NUL */
  fl_out_tmp_lng=strlen(fl_out)+1UL+strlen(tmp_sng_1)+strlen(pid_sng)+1UL+strlen(prg_nm_get())+1UL+strlen(tmp_sng_2)+1UL;
  /* NB: Calling routine has responsibility to free() this memory */
  fl_out_tmp=(char *)nco_malloc(fl_out_tmp_lng*sizeof(char));
  (void)sprintf(fl_out_tmp,"%s.%s%s.%s.%s",fl_out,tmp_sng_1,pid_sng,prg_nm_get(),tmp_sng_2);
  if(dbg_lvl_get() > 5) (void)fprintf(stdout,"%s: nco_fl_out_open() reports sizeof(pid_t) = %d bytes, pid = %ld, pid_sng_lng = %ld bytes, strlen(pid_sng) = %ld bytes, fl_out_tmp_lng = %ld bytes, strlen(fl_out_tmp) = %ld, fl_out_tmp = %s\n",prg_nm_get(),(int)sizeof(pid_t),(long)pid,pid_sng_lng,(long)strlen(pid_sng),fl_out_tmp_lng,(long)strlen(fl_out_tmp),fl_out_tmp);
  rcd=stat(fl_out_tmp,&stat_sct);

  /* Free temporary memory */
  pid_sng=(char *)nco_free(pid_sng);

  if(dbg_lvl_get() == 8){
  /* Use built-in system routines to generate temporary filename
     This allows file to be built in fast directory like /tmp rather than local
     directory which could be a slow, NFS-mounted directories like /fs/cgd

     There are many options:
     tmpnam() uses P_tmpdir, does not allow specfication of drc
     tempnam(const char *drc, const char *pfx) uses writable $TMPDIR, else drc, else P_tmpdir, else /tmp and prefixes returned name with up to five characters from pfx, if supplied
     mkstemp(char *tpl) generates a filename and creates file in mode 0600

     Many sysadmins do not make /tmp large enough for huge temporary data files
     tempnam(), however, allows $TMPDIR or drc to be set to override /tmp
     We tried tempnam() but as of 20001010 GCC 2.96 this causes a warning: "the use of `tempnam' is dangerous, better use `mkstemp'"
   */
    int fl_out_hnd; /* Temporary file */
    char *fl_out_tmp_sys; /* System-generated unique temporary filename */
    fl_out_tmp_sys=(char *)nco_malloc((strlen(fl_out)+7)*sizeof(char));
    fl_out_tmp_sys[0]='\0'; /* NUL-terminate */
    fl_out_tmp_sys=strcat(fl_out_tmp_sys,fl_out);
    fl_out_tmp_sys=strcat(fl_out_tmp_sys,"XXXXXX");
#ifdef HAVE_MKSTEMP
    fl_out_hnd=mkstemp(fl_out_tmp_sys);
#else /* !HAVE_MKSTEMP */
    /* 20020812: Cray does not support mkstemp() */
    fl_out_hnd=creat(mktemp(fl_out_tmp_sys),0600);
#endif /* !HAVE_MKSTEMP */
    fl_out_hnd=fl_out_hnd; /* Removes compiler warning on SGI */
    if(dbg_lvl_get() > 2) (void)fprintf(stdout,"%s: nco_fl_out_open() reports strlen(fl_out_tmp_sys) = %ld, fl_out_tmp_sys = %s, \n",prg_nm_get(),(long)strlen(fl_out_tmp_sys),fl_out_tmp_sys);
    fl_out_tmp_sys=(char *)nco_free(fl_out_tmp_sys);
  } /* endif dbg */

  /* If temporary file already exists, prompt user to remove temporary files and exit */
  if(rcd != -1){
    (void)fprintf(stdout,"%s: ERROR temporary file %s already exists, remove and try again\n",prg_nm_get(),fl_out_tmp);
    nco_exit(EXIT_FAILURE);
  } /* end if */

  if(FORCE_OVERWRITE){
    rcd=nco_create(fl_out_tmp,nccreate_mode,out_id);
    /*    rcd=nco_create(fl_out_tmp,nccreate_mode|NC_SHARE,out_id);*/
    return fl_out_tmp;
  } /* end if */

  if(False){
    if(prg_get() == ncrename){
      /* ncrename and ncatted allow single filename without question */
      /* Incur expense of copying current file to temporary file */
      (void)nco_fl_cp(fl_out,fl_out_tmp);
      rcd=nco_open(fl_out_tmp,NC_WRITE,out_id);
      (void)nco_redef(*out_id);
      return fl_out_tmp;
    } /* end if */
  } /* end if false */

  rcd=stat(fl_out,&stat_sct);

  /* If permanent file already exists, query user whether to overwrite, append, or exit */
  if(rcd != -1){
    char *rcd_fgets=NULL; /* Return code from fgets */
#define USR_RPL_MAX_LNG 10 /* [nbr] Maximum length for user reply */
#define USR_RPL_MAX_NBR 10 /* [nbr] Maximum number of chances for user to reply */
    char usr_rpl[USR_RPL_MAX_LNG];
    int usr_rpl_int;
    short nbr_itr=0;
    size_t usr_rpl_lng;

    /* Initialize user reply string */
    usr_rpl[0]='z';
    usr_rpl[1]='\0';

    if(FORCE_APPEND){
      /* Incur expense of copying current file to temporary file */
      (void)nco_fl_cp(fl_out,fl_out_tmp);
      rcd=nco_open(fl_out_tmp,NC_WRITE,out_id);
      (void)nco_redef(*out_id);
      return fl_out_tmp;
    } /* end if */

    /* Ensure one exit condition for each valid switch in following case statement */
    while(strcasecmp(usr_rpl,"o") && strcasecmp(usr_rpl,"a") && strcasecmp(usr_rpl,"e")){
      /* int cnv_nbr; *//* [nbr] Number of scanf conversions performed this scan */
      if(nbr_itr++ > USR_RPL_MAX_NBR){
	(void)fprintf(stdout,"\n%s: ERROR %hd failed attempts to obtain valid interactive input. Assuming non-interactive shell and exiting.\n",prg_nm_get(),nbr_itr-1);
	nco_exit(EXIT_FAILURE);
      } /* end if */
      if(nbr_itr > 1) (void)fprintf(stdout,"%s: ERROR Invalid response.\n",prg_nm_get());
      (void)fprintf(stdout,"%s: %s exists---`e'xit, `o'verwrite (i.e., delete existing file), or `a'ppend (i.e., replace duplicate variables in and add new variables to existing file) (e/o/a)? ",prg_nm_get(),fl_out);
      (void)fflush(stdout);
      /*       fgets() reads (at most one less than USR_RPL_MAX_LNG) to first newline or EOF */
      rcd_fgets=fgets(usr_rpl,USR_RPL_MAX_LNG,stdin);
      /*       fscanf() reads ... */
      /*      while((cnv_nbr=fscanf(stdin,"%9s",usr_rpl)) != EOF) continue;*/
      /*      while((rcd_fgets=fgets(usr_rpl,USR_RPL_MAX_LNG,stdin)) == NULL){*/
	/*	if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: DEBUG Read \"%s\" while waiting for non-NULL on stdin...\n",prg_nm_get(),(rcd_fgets == NULL) ? "NULL" : usr_rpl);*/
      /*      continue;}*/

      /* Ensure last character in input string is \n and replace that with \0 */
      usr_rpl_lng=strlen(usr_rpl);
      if(usr_rpl_lng >= 1)
	if(usr_rpl[usr_rpl_lng-1] == '\n')
	    usr_rpl[usr_rpl_lng-1]='\0';

      if(dbg_lvl_get() == 3) (void)fprintf(stdout,"%s: INFO nco_fl_out_open() reports that fgets() read \"%s\" (after removing trailing newline) from stdin\n",prg_nm_get(),(rcd_fgets == NULL) ? "NULL" : usr_rpl);
    } /* end while */

    /* Ensure one case statement for each exit condition in preceding while loop */
    usr_rpl_int=(int)usr_rpl[0];
    switch(usr_rpl_int){
    case 'E':
    case 'e':
      nco_exit(EXIT_SUCCESS);
      break;
    case 'O':
    case 'o':
      rcd=nco_create(fl_out_tmp,nccreate_mode,out_id);
      /*    rcd=nco_create(fl_out_tmp,nccreate_mode|NC_SHARE,out_id);*/
      break;
    case 'A':
    case 'a':
      /* Incur expense of copying current file to temporary file */
      (void)nco_fl_cp(fl_out,fl_out_tmp);
      rcd=nco_open(fl_out_tmp,NC_WRITE,out_id);
      (void)nco_redef(*out_id);
      break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */

  }else{ /* Output file does not yet already exist */
    nccreate_mode=NC_NOCLOBBER;
    nccreate_mode=nco_create_mode_mrg(nccreate_mode,fl_out_fmt);
    rcd=nco_create(fl_out_tmp,nccreate_mode,out_id);
    /*    rcd=nco_create(fl_out_tmp,nccreate_mode|NC_SHARE,out_id);*/
  } /* end if output file does not already exist */

  return fl_out_tmp;

} /* end nco_fl_out_open() */

void
nco_fl_out_cls /* [fnc] Close temporary output file, move it to permanent output file */
(const char * const fl_out, /* I [sng] Name of permanent output file */
 const char * const fl_out_tmp, /* I [sng] Name of temporary output file to close and move to permanent output file */
 const int nc_id) /* I [id] File ID of fl_out_tmp */
{
  /* Purpose: Close temporary output file, move it to permanent output file */
  int rcd; /* [rcd] Return code */

  rcd=nco_close(nc_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stdout,"%s: ERROR nco_fl_out_cls() is unable to nco_close() file %s\n",prg_nm_get(),fl_out_tmp);
    nco_exit(EXIT_FAILURE);
  } /* end if */

  (void)nco_fl_mv(fl_out_tmp,fl_out);

} /* end nco_fl_out_cls() */

void
nco_fl_rm /* [fnc] Remove file */
(char *fl_nm) /* I [sng] File to be removed */
{
  /* Purpose: Remove specified file from local system */
  int rcd;
  char *rm_cmd;
  const char rm_cmd_sys_dep[]="rm -f";

  /* Remember to add one for the space and one for the terminating NUL character */
  rm_cmd=(char *)nco_malloc((strlen(rm_cmd_sys_dep)+1UL+strlen(fl_nm)+1UL)*sizeof(char));
  (void)sprintf(rm_cmd,"%s %s",rm_cmd_sys_dep,fl_nm);

  if(dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: DEBUG Removing %s with %s\n",prg_nm_get(),fl_nm,rm_cmd);
  rcd=system(rm_cmd);
  if(rcd == -1) (void)fprintf(stderr,"%s: WARNING unable to remove %s, continuing anyway...\n",prg_nm_get(),fl_nm);

  rm_cmd=(char *)nco_free(rm_cmd);
} /* end nco_fl_rm() */

