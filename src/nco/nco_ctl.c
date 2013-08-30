/* $Header: /cvsroot/nco/nco/src/nco/nco_ctl.c,v 1.264 2010/06/24 17:48:38 zender Exp $ */

/* Purpose: Program flow control functions */

/* Copyright (C) 1995--2010 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_ctl.h" /* Program flow control functions */

const char * /* O [sng] Compiler and version */
nco_cmp_get(void) /* [fnc] Return compiler and version */
{ 
  /* Purpose: Return string containing compiler and version */
  const char fnc_nm[]="nco_cmp_get()";
#if defined(_AIX) && !defined(__GNUC__) && !defined(__xlC__)
  static const char cmp_nm[]="xlc"; /* [sng] Compiler name */
  static const char cmp_sng[]="Token _AIX defined in nco_cmp_get(), probably compiled with AIX xlc_r or xlc"; /* [sng] Compiler string */
#endif /* !_AIX */
#if defined(__xlC__)
  static const char cmp_nm[]="xlC"; /* [sng] Compiler name */
  static const char cmp_sng[]="Token __xlC__ defined in nco_cmp_get(), probably compiled with AIX xlC_r or xlC"; /* [sng] Compiler string */
#endif /* !__xlC__ */
#if defined(__GNUC__) && !defined(__INTEL_COMPILER) && !defined(__PATHCC__) && !defined(PGI_CC)
  /* Testing for GCC macros early is dangerous because some compilers, 
     including Intel's, define GCC macros for compatibility */
  static const char cmp_nm[]="gcc"; /* [sng] Compiler name */
  static const char cmp_sng[]="Token __GNUC__ defined in nco_cmp_get(), probably compiled with GNU gcc"; /* [sng] Compiler string */
  static const char cmp_vrs[]=TKN2SNG(__VERSION__); // [sng] Compiler version
  static const char cmp_vrs_mjr[]=TKN2SNG(__GNUC__); // [sng] Compiler major version
  static const char cmp_vrs_mnr[]=TKN2SNG(__GNUC_MINOR__); // [sng] Compiler minor version
  static const char cmp_vrs_pch[]=TKN2SNG(__GNUC_PATCHLEVEL__); // [sng] Compiler patch version

  if(dbg_lvl_get() >= nco_dbg_scl){
    (void)fprintf(stderr,"%s: INFO GCC major version is %s\n",prg_nm_get(),cmp_vrs_mjr);
    (void)fprintf(stderr,"%s: INFO GCC minor version is %s\n",prg_nm_get(),cmp_vrs_mnr);
    (void)fprintf(stderr,"%s: INFO GCC patch version is %s\n",prg_nm_get(),cmp_vrs_pch);
    (void)fprintf(stderr,"%s: INFO GCC version is %s\n",prg_nm_get(),cmp_vrs);
  } /* endif dbg */
#endif /* !__GNUC__ */
#ifdef __INTEL_COMPILER
  /* Some compilers, including icc, also define __GNUC__ by default */
  static const char cmp_nm[]="icc";
  static const char cmp_sng[]="Token __INTEL_COMPILER defined in nco_cmp_get(), probably compiled with Intel icc"; /* [sng] Compiler string */
#endif /* !__INTEL_COMPILER */
#ifdef __PATHCC__
  /* Some compilers, including pathcc, also define __GNUC__ by default */
  static const char cmp_nm[]="pathcc";
  static const char cmp_sng[]="Token __PATHCC__ defined in nco_cmp_get(), probably compiled with PathScale (Qlogic) pathcc"; /* [sng] Compiler string */
#endif /* !__PATHCC__ */
#ifdef PGI_CC
  static const char cmp_nm[]="pgcc";
  static const char cmp_sng[]="Token PGI_CC defined in nco_cmp_get(), probably compiled with PGI pgcc"; /* [sng] Compiler string */
#endif /* !PGI_CC */

  /* In case none of the above tokens matched */
#if !defined(_AIX) && !defined(__GNUC__) && !defined(__INTEL_COMPILER) && !defined(__PATHCC__) && !defined(PGI_CC)
  /* Unknown compiler */
  static const char cmp_nm[]="unknown"; /* [sng] Compiler name */
  static const char cmp_sng[]="Unknown compiler tokens in nco_cmp_get(), compiler is unknown"; /* [sng] Compiler string */
#endif /* !unknown */

  if(dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO %s reports compiler name is \"%s\"\n%s\n",prg_nm_get(),fnc_nm,cmp_nm,cmp_sng);

  return cmp_nm;
} /* end nco_cmp_get() */

const char * /* O [sng] MPI implementation */
nco_mpi_get(void) /* [fnc] Return MPI implementation */
{ 
  /* Purpose: Return string containing MPI implementation information */
  const char fnc_nm[]="nco_mpi_get()";

#ifdef _H_MPI
  static const char mpi_nm[]="PPE"; /* [sng] MPI name */
  static const char mpi_sng[]="Token _H_MPI defined in nco_mpi_get(), MPI environment is probably AIX PPE MPI"; /* [sng] MPI string */
#endif /* !_H_MPI */
#ifdef LAM_MPI
  static const char mpi_nm[]="LAM"; /* [sng] MPI name */
  static const char mpi_sng[]="Token LAM_MPI defined in nco_mpi_get(), MPI environment is probably LAM-MPI"; /* [sng] MPI string */
#endif /* !LAM_MPI */
#if MPICH_NAME == '1'
  static const char mpi_nm[]="MPICH"; /* [sng] MPI name */
  static const char mpi_sng[]="Token MPICH_NAME defined to 1 in nco_mpi_get(), MPI environment is probably MPICH version 1"; /* [sng] MPI string */
#endif /* !MPICH_NAME */
#ifdef MPICH2
  static const char mpi_nm[]="MPICH2"; /* [sng] MPI name */
  static const char mpi_sng[]="Token MPICH2 defined in nco_mpi_get(), MPI environment is probably MPICH2, i.e., MPICH version 2"; /* [sng] MPI string */
#endif /* !MPICH2 */

  /* In case no token matched */
#if !defined(_H_MPI) && !defined(LAM_MPI) && (MPICH_NAME != '1') && !defined(MPICH2)
#ifndef MPI_VERSION
  /* MPI is not installed */
  static const char mpi_nm[]="none"; /* [sng] MPI name */
  static const char mpi_sng[]="No MPI tokens found in nco_mpi_get(), MPI environment is not active"; /* [sng] MPI string */
#else /* MPI_VERSION */
  /* Unknown MPI implementation */
  static const char mpi_nm[]="unknown"; /* [sng] MPI name */
  static const char mpi_sng[]="Unknown MPI tokens found in nco_mpi_get(), MPI environment is present and of unknown pedigree"; /* [sng] MPI string */
#endif /* MPI_VERSION */
#endif /* !unknown */

  if(dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO %s reports MPI implementation name is \"%s\"\n%s\n",prg_nm_get(),fnc_nm,mpi_nm,mpi_sng);
  return mpi_nm;
} /* end nco_mpi_get() */

int /* O [rcd] Return code */
nco_ddra /* [fnc] Count operations */
(const char * const var_nm, /* I [sng] Variable name */
 const char * const wgt_nm, /* I [sng] Weight name */
 const ddra_info_sct * const ddra_info) /* I [sct] DDRA information */
{
  /* Purpose: Estimate operation counts required */

  /* 20070509 CEWI RUVICFFU: ntg_nbr_rdc_dfl, ntg_nbr_nrm_dfl, flp_nbr_rdc_dfl, flp_nbr_nrm_dfl, rnk_avg */

  const char fnc_nm[]="nco_ddra()";
  
  /* Following speed parameter estimates are for clay.ess.uci.edu 
     clay is the dual opteron used in ppr_ZeM07 */
  const float ntg_nbr_brd_fdg_fct=1.8; /* [frc] Empirical correction to broadcasting */
  const float spd_flp_ncwa=153e6; /* [# s-1] Floating point operation speed */
  const float spd_ntg_ncwa=200e6; /* [# s-1] Integer operation speed */
  const float spd_flp_ncbo=353.2e6; /* [# s-1] Floating point operation speed */
  const float spd_ntg_ncbo=1386.54e6; /* [# s-1] Integer operation speed */
  const float spd_rd=63.375e6; /* [B s-1] Disk read bandwidth */
  const float spd_wrt=57.865e6; /* [B s-1] Disk write bandwidth */

  /* 20060602: 
     hjm estimates faster speeds for ncbo:     
     spd_flp_ncbo = 353.2e6: Based on unoptimized operations in tacg function
     spd_ntg_ncbo = 1386.54e6: Based on counting unoptimized operations in tacg function
     hjm estimates slower speeds for ncwa:     
     spd_flp_ncwa = 153e6: Based on direct PAPI measures
     spd_ntg_ncwa = 200e6: Based on direct measure, using large data set, calculating backwards */

  float spd_flp=float_CEWI; /* [# s-1] Floating point operation speed */
  float spd_ntg=float_CEWI; /* [# s-1] Integer operation speed */
  int rcd=NC_NOERR; /* [rcd] Return code */

  /* Timer information */
  static clock_t tm_obs_old; /* [us] Microseconds initially */
  static float tm_obs_ttl=0.0; /* [s] Total seconds elapsed */
  clock_t tm_obs_crr; /* [us] Microseconds currently */
  float tm_obs_dff; /* [s] Seconds elapsed */
  
  /* Cumulative file costs */
  static long long lmn_nbr_ttl=0LL; /* I/O [nbr] Cumulative variable size */
  static long long ntg_nbr_ttl=0LL; /* I/O [nbr] Cumulative integer operations */
  static long long flp_nbr_ttl=0LL; /* I/O [nbr] Cumulative floating point operations */

  /* Cumulative times */
  static float tm_ntg_ttl=0.0f; /* I/O [s] Cumulative integer time */
  static float tm_flp_ttl=0.0f; /* I/O [s] Cumulative floating point time */
  static float tm_rd_ttl=0.0f; /* I/O [s] Cumulative read time */
  static float tm_wrt_ttl=0.0f; /* I/O [s] Cumulative write time */
  static float tm_io_ttl=0.0f; /* [s] I/O time */
  static float tm_ttl=0.0f; /* I/O [s] Cumulative time */
  
  /* Current variable costs */
  float tm_ntg; /* [s] Integer time */
  float tm_flp; /* [s] Floating point time */
  float tm_rd; /* [s] Read time */
  float tm_wrt; /* [s] Write time */
  float tm_io=float_CEWI; /* [s] I/O time */
  float tm_crr=float_CEWI; /* [s] Time for this variable */
  long long ntg_nbr=long_long_CEWI; /* [nbr] Integer operations */
  long long flp_nbr=long_long_CEWI; /* [nbr] Floating point operations */
  long long rd_nbr_byt=long_long_CEWI; /* [B] Bytes read */
  long long wrt_nbr_byt=long_long_CEWI; /* [B] Bytes written */
  
  /* Summary statistics */
  float tm_frc_flp_ttl=float_CEWI; /* [frc] Floating point time fraction */
  float tm_frc_io_ttl=float_CEWI; /* [frc] I/O time fraction */
  float tm_frc_ntg_ttl=float_CEWI; /* [frc] Integer time fraction */
  float tm_frc_rd_ttl=float_CEWI; /* [frc] Read time fraction */
  float tm_frc_wrt_ttl=float_CEWI; /* [frc] Write time fraction */

  /* Default algorithm costs if invoked */
  long long ntg_nbr_byt_swp_dfl; /* [nbr] Integer operations for byte-swap */
  long long ntg_nbr_brd_dfl; /* [nbr] Integer operations for broadcasting */
  long long ntg_nbr_clc_dfl; /* [nbr] Integer operations for collection */
  /* long long ntg_nbr_rdc_dfl; */ /* [nbr] Integer operations for reduction */
  /* long long ntg_nbr_nrm_dfl; */ /* [nbr] Integer operations for normalization */
  long long flp_nbr_bnr_dfl; /* [nbr] Floating point operations for binary arithmetic */
  /* long long flp_nbr_rdc_dfl; */ /* [nbr] Floating point operations for reduction */
  /* long long flp_nbr_nrm_dfl; */ /* [nbr] Floating point operations for normalization */

  /* Initialize all algorithm counts for this variable to zero then increment */
  long long ntg_nbr_byt_swp=0LL; /* [nbr] Integer operations for byte-swap */
  long long ntg_nbr_brd=0LL; /* [nbr] Integer operations for broadcasting */
  long long ntg_nbr_clc=0LL; /* [nbr] Integer operations for collection */
  long long ntg_nbr_rdc=0LL; /* [nbr] Integer operations for reduction */
  long long ntg_nbr_nrm=0LL; /* [nbr] Integer operations for normalization */
  long long flp_nbr_bnr=0LL; /* [nbr] Floating point operations for binary arithmetic */
  long long flp_nbr_rdc=0LL; /* [nbr] Floating point operations for reduction */
  long long flp_nbr_nrm=0LL; /* [nbr] Floating point operations for normalization */

  /* Copies of DDRA information */
  int nco_op_typ; /* [enm] Operation type */
  /* int rnk_avg; */ /* [nbr] Rank of averaging space */
  int rnk_var; /* [nbr] Variable rank (in input file) */
  int rnk_wgt; /* [nbr] Rank of weight */
  int var_idx=int_CEWI; /* [enm] Index */
  int wrd_sz; /* [B] Bytes per element */
  long long lmn_nbr=int_CEWI; /* [nbr] Variable size */
  long long lmn_nbr_avg; /* [nbr] Averaging block size */
  long long lmn_nbr_wgt; /* [nbr] Weight size */
  nco_bool MRV_flg; /* [flg] Avergaging dimensions are MRV dimensions */
  nco_bool wgt_brd_flg; /* [flg] Broadcast weight for this variable */

  /* Locals */
  long long lmn_nbr_out=long_long_CEWI; /* [nbr] Output elements */

  /* Where possible, work in terms of "default" counts per algorithm
     Algorithms (e.g., reduction, byte-swapping) are generally subroutines
     Record hand-counted counts in only one place
     This makes it easier and less error-prone to propagate optimizations
     Also, easier to think in terms of procedures than raw counts

     Algorithms for subtraction:
     "byte_swap+byte_swap+binary_arithmetic+byte_swap"
     Algorithms for non-weighted, non-MRV averaging:
     "byte_swap(input)+collection+reduction+normalization+byte_swap(output)"
     Algorithms for non-weighted, MRV averaging:
     "byte_swap(input)+reduction+normalization+byte_swap(output)"
     Algorithms for weighted, non-MRV averaging:
     "byte_swap(input)+byte_swap(weight)+broadcast(weight)+
     collection(numerator)+collection(denominator)+
     binary_arithmetic(weight*value)+
     reduction(numerator)+reduction(denominator)+
     normalization(numerator)+normalization(denominator)+byte_swap(output)"
     Units: 
     [nbr] = Operation counts for lmn_nbr elements
     [nbr nbr-1] = Operation counts per element */

  switch(ddra_info->tmr_flg){
  case nco_tmr_srt: /* [enm] Initialize timer (first timer call) */
    tm_obs_crr=clock();
    tm_obs_old=tm_obs_crr;
    return rcd; /* [rcd] Return code */
    break;
  case nco_tmr_mtd: /* [enm] Metadata timer (second timer call) */
  case nco_tmr_end: /* [enm] Close timer (last timer call) */
    goto update_timers;
    break;
  case nco_tmr_rgl: /* [enm] Regular timer call (main loop timer call) */
    break;
  default: nco_dfl_case_tmr_typ_err(); break;
  } /* end switch */

  /* Decode input */
  MRV_flg=ddra_info->MRV_flg; /* [flg] Avergaging dimensions are MRV dimensions */
  lmn_nbr=ddra_info->lmn_nbr; /* [nbr] Variable size */
  lmn_nbr_avg=ddra_info->lmn_nbr_avg; /* [nbr] Averaging block size */
  lmn_nbr_wgt=ddra_info->lmn_nbr_wgt; /* [nbr] Weight size */
  nco_op_typ=ddra_info->nco_op_typ; /* [enm] Operation type */
  /* rnk_avg=ddra_info->rnk_avg; */ /* [nbr] Rank of averaging space */
  rnk_var=ddra_info->rnk_var; /* [nbr] Variable rank (in input file) */
  rnk_wgt=ddra_info->rnk_wgt; /* [nbr] Rank of weight */
  var_idx=ddra_info->var_idx; /* [enm] Index */
  wgt_brd_flg=ddra_info->wgt_brd_flg; /* [flg] Broadcast weight for this variable */
  wrd_sz=ddra_info->wrd_sz; /* [B] Bytes per element */

  /* Derived variables */
  switch(nco_op_typ){
    /* Types used in ncbo(), ncflint() */
  case nco_op_add: /* [enm] Add file_1 to file_2 */
  case nco_op_dvd: /* [enm] Divide file_1 by file_2 */
  case nco_op_mlt: /* [enm] Multiply file_1 by file_2 */
  case nco_op_sbt: /* [enm] Subtract file_2 from file_1 */
    spd_flp=spd_flp_ncbo; /* [# s-1] Floating point operation speed */
    spd_ntg=spd_ntg_ncbo; /* [# s-1] Integer operation speed */
    lmn_nbr_out=lmn_nbr; /* [nbr] Output elements */
    break;
    /* Types used in ncra(), ncrcat(), ncwa(): */
  case nco_op_avg: /* [enm] Average */
  case nco_op_min: /* [enm] Minimum value */
  case nco_op_max: /* [enm] Maximum value */
  case nco_op_ttl: /* [enm] Linear sum */
  case nco_op_sqravg: /* [enm] Square of mean */
  case nco_op_avgsqr: /* [enm] Mean of sum of squares */
  case nco_op_sqrt: /* [enm] Square root of mean */
  case nco_op_rms: /* [enm] Root-mean-square (normalized by N) */
  case nco_op_rmssdn: /* [enm] Root-mean square normalized by N-1 */
    spd_flp=spd_flp_ncwa; /* [# s-1] Floating point operation speed */
    spd_ntg=spd_ntg_ncwa; /* [# s-1] Integer operation speed */
    lmn_nbr_out=lmn_nbr/lmn_nbr_avg; /* [nbr] Output elements */
    break;
  case nco_op_nil: /* [enm] Nil or undefined operation type  */
    break;
  default:
    (void)fprintf(stdout,"%s: ERROR Illegal nco_op_typ in %s\n",prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
    break;
  } /* end switch */

  flp_nbr_bnr_dfl=lmn_nbr; /* [nbr] Floating point operations for binary arithmetic */
  /* flp_nbr_nrm_dfl=lmn_nbr_out; */ /* [nbr] Floating point operations for normalization */
  /* flp_nbr_rdc_dfl=lmn_nbr; */ /* [nbr] Floating point operations for reduction */

  /* Integer operations for broadcasting weight */
  ntg_nbr_brd_dfl=(long long)(ntg_nbr_brd_fdg_fct*lmn_nbr*(6*rnk_var+8*rnk_wgt+2)); /* [nbr] N(6R+8R_w+2) */

  /* Byte-swap integer operations per element */
  ntg_nbr_byt_swp_dfl=wrd_sz+2; /* [nbr nbr-1] W+2 */

  /* Integer operations for collection */
  ntg_nbr_clc_dfl=lmn_nbr*(14*rnk_var+4); /* [nbr] N(14R+4) */

  /* Integer operations for normalization */
  /* ntg_nbr_nrm_dfl=4*lmn_nbr_out; */ /* [nbr] 4N/N_A = 4N_O */

  /* Integer operations for reduction */
  /* ntg_nbr_rdc_dfl=lmn_nbr*6+lmn_nbr_out; */ /* [nbr] N(6+N/N_A) */

  switch(nco_op_typ){
    /* Types used in ncbo(), ncflint() */
  case nco_op_add: /* [enm] Add file_1 to file_2 */
  case nco_op_dvd: /* [enm] Divide file_1 by file_2 */
  case nco_op_mlt: /* [enm] Multiply file_1 by file_2 */
  case nco_op_sbt: /* [enm] Subtract file_2 from file_1 */
    /* Subtraction computation assumes variables are same size
       fxm: Account for broadcasting */
    /* One floating point (add/subtract/multiply/divide) per element */
    flp_nbr_bnr=flp_nbr_bnr_dfl;
    /* Byte-swap elements from two input files and one output file */
    ntg_nbr_byt_swp=3*lmn_nbr*ntg_nbr_byt_swp_dfl; /* 3N(W+2) */
    rd_nbr_byt=2*lmn_nbr*wrd_sz; /* [B] Bytes read */
    wrt_nbr_byt=lmn_nbr_out*wrd_sz; /* [B] Bytes written */
    break;
    /* Types used in ncra(), ncrcat(), ncwa(): */
  case nco_op_avg: /* [enm] Average */
  case nco_op_min: /* [enm] Minimum value */
  case nco_op_max: /* [enm] Maximum value */
  case nco_op_ttl: /* [enm] Linear sum */
  case nco_op_sqravg: /* [enm] Square of mean */
  case nco_op_avgsqr: /* [enm] Mean of sum of squares */
  case nco_op_sqrt: /* [enm] Square root of mean */
  case nco_op_rms: /* [enm] Root-mean-square (normalized by N) */
  case nco_op_rmssdn: /* [enm] Root-mean square normalized by N-1 */
    rd_nbr_byt=lmn_nbr*wrd_sz; /* [B] Bytes read */
    wrt_nbr_byt=lmn_nbr_out*wrd_sz; /* [B] Bytes written */
    /* One floating point add per input element to sum numerator */
    flp_nbr_rdc=lmn_nbr;
    /* One floating point divide per output element to normalize numerator by denominatro (tally) */
    flp_nbr_nrm=lmn_nbr_out;
    /* Byte-swap elements from one input file and one (rank-reduced) output file */
    ntg_nbr_byt_swp=(lmn_nbr+lmn_nbr_out)*ntg_nbr_byt_swp_dfl;
    if(!MRV_flg){
      /* Collection required for numerator */
      ntg_nbr_clc+=ntg_nbr_clc_dfl;
    } /* !MRV_flg */
    if(wgt_nm){
      if(var_idx == 0){
	/* Set cost = 0 after first variable since only read weight once */
	rd_nbr_byt+=lmn_nbr_wgt*wrd_sz; /* [B] Bytes read */
	/* Byte-swap cost for first weight input is usually negligible */
	ntg_nbr_byt_swp+=lmn_nbr_wgt*ntg_nbr_byt_swp_dfl;
      } /* var_idx != 0 */
      /* One floating point multiply per input element for weight*value in numerator,
	 and one floating point add per input element to sum weight in denominator */
      flp_nbr_rdc+=2*lmn_nbr;
      /* One floating point divide per output element to normalize denominator by tally */
      flp_nbr_nrm+=lmn_nbr_out;
      if(wgt_brd_flg){
	/* fxm: Charge for broadcasting weight at least once */
	/* Broadcasting cost for weight */
	ntg_nbr_brd=ntg_nbr_brd_dfl;
      } /* !wgt_brd_flg */
      if(!MRV_flg){
	/* Collection required for denominator */
	ntg_nbr_clc+=ntg_nbr_clc_dfl;
      } /* !MRV_flg */
    } /* !wgt_nm */
    break;
  case nco_op_nil: /* [enm] Nil or undefined operation type  */
    break;
  default:
    (void)fprintf(stdout,"%s: ERROR Illegal nco_op_typ in %s\n",prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
    break;
  } /* end switch */

  flp_nbr= /* [nbr] Floating point operations */
    flp_nbr_bnr+ /* [nbr] Floating point operations for binary arithmetic */
    flp_nbr_rdc+ /* [nbr] Floating point operations for reduction */
    flp_nbr_nrm; /* [nbr] Floating point operations for normalization */

  ntg_nbr= /* [nbr] Integer operations */
    ntg_nbr_byt_swp+ /* [nbr] Integer operations for byte-swap */
    ntg_nbr_brd+ /* [nbr] Integer operations for broadcasting */
    ntg_nbr_clc+ /* [nbr] Integer operations for collection */
    ntg_nbr_rdc+ /* [nbr] Integer operations for reduction */
    ntg_nbr_nrm; /* [nbr] Integer operations for normalization */

  tm_ntg=ntg_nbr/spd_ntg; /* [s] Integer time */
  tm_flp=flp_nbr/spd_flp; /* [s] Floating point time */
  tm_rd=rd_nbr_byt/spd_rd; /* [s] Read time */
  tm_wrt=wrt_nbr_byt/spd_wrt; /* [s] Write time */

  tm_io=tm_rd+tm_wrt; /* [s] I/O time */
  tm_crr=tm_ntg+tm_flp+tm_rd+tm_wrt; /* [s] Time for this variable */

  /* Increment running totals */
  lmn_nbr_ttl+=lmn_nbr; /* [nbr] Cumulative variable size */
  flp_nbr_ttl+=flp_nbr; /* [nbr] Cumulative floating point operations */
  ntg_nbr_ttl+=ntg_nbr; /* [nbr] Cumulative integer operations */
  
  tm_ntg_ttl+=tm_ntg; /* [s] Cumulative integer time */
  tm_flp_ttl+=tm_flp; /* [s] Cumulative floating point time */
  tm_rd_ttl+=tm_rd; /* [s] Cumulative read time */
  tm_wrt_ttl+=tm_wrt; /* [s] Cumulative write time */
  tm_io_ttl+=tm_io; /* [s] Cumulative I/O time */
  tm_ttl+=tm_crr; /* [s] Cumulative time */

  tm_frc_flp_ttl=tm_flp_ttl/tm_ttl; /* [frc] Floating point time fraction */
  tm_frc_io_ttl=tm_io_ttl/tm_ttl; /* [frc] I/O time fraction */
  tm_frc_ntg_ttl=tm_ntg_ttl/tm_ttl; /* [frc] Integer time fraction */
  tm_frc_rd_ttl=tm_rd_ttl/tm_ttl; /* [frc] Read time fraction */
  tm_frc_wrt_ttl=tm_wrt_ttl/tm_ttl; /* [frc] Write time fraction */

  if(var_idx == 0){
    /* Table headings */
    (void)fprintf(stdout,"%3s %8s %8s %8s %8s %5s %5s %8s %8s %8s %4s %4s %4s %4s %4s %7s %7s\n",
		  "idx"," var_nm ","   lmn  ","   flp  ","   ntg  ","tm_io","  tm "," lmn_ttl"," flp_ttl"," ntg_ttl"," flp"," ntg","  rd"," wrt"," io"," tm_ttl"," tm_obs");
    (void)fprintf(stdout,"%3s %8s %8s %8s %8s %5s %5s %8s %8s %8s %4s %4s %4s %4s %4s %7s %7s\n",
		  "   ","        ","    #   ","    #   ","    #   ","  s  ","  s  ","   #    ","   #    ","    #   ","  % ","  % ","  % ","  % ","  % ","   s   ","   s   ");
  } /* var_idx != 0 */

  /* Update timers */
 update_timers:
  /* NB: POSIX requires CLOCKS_PER_SEC equals 1000000 so resolution = 1 microsecond */
  tm_obs_crr=clock();
  tm_obs_dff=(float)(tm_obs_crr-tm_obs_old)/CLOCKS_PER_SEC;
  tm_obs_old=tm_obs_crr;
  tm_obs_ttl+=tm_obs_dff;

  switch(ddra_info->tmr_flg){
  case nco_tmr_mtd: /* [enm] Metadata timer (second timer call) */
    if(ddra_info->flg_ddra || dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: TIMER Metadata setup and file layout before main loop took %7.2f s\n",prg_nm_get(),tm_obs_ttl);
    break;
  case nco_tmr_rgl: /* [enm] Regular timer call (main loop timer call) */
    (void)fprintf(stdout,
		  "%3d %8s %8.2e %8.2e %8.2e %5.2f %5.2f %8.2e %8.2e %8.2e %4.1f %4.1f %4.1f %4.1f %4.1f %7.2f %7.2f\n",
		  var_idx,var_nm,(float)lmn_nbr,(float)flp_nbr,(float)ntg_nbr,tm_io,tm_crr,(float)lmn_nbr_ttl,(float)flp_nbr_ttl,(float)ntg_nbr_ttl,100.0*tm_frc_flp_ttl,100.0*tm_frc_ntg_ttl,100.0*tm_frc_rd_ttl,100.0*tm_frc_wrt_ttl,100.0*tm_frc_io_ttl,tm_ttl,tm_obs_ttl);
    break;
  case nco_tmr_end: /* [enm] Close timer (last timer call) */
    if(ddra_info->flg_ddra || dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: TIMER Wallclock-elapsed time for command is %7.2f s\n",prg_nm_get(),tm_obs_ttl);
    break;
  default: nco_dfl_case_tmr_typ_err(); break;
  } /* end switch */

  return rcd; /* [rcd] Return code */
} /* nco_ddra() */

void 
nco_dfl_case_tmr_typ_err(void) /* [fnc] Print error and exit for illegal switch(tmr_typ) case */
{
  /* Purpose: Convenience routine for printing error and exiting when
     switch(tmr_typ) statement receives an illegal default case

     Placing this in its own routine also has the virtue of saving many lines 
     of code since this function is used in many many switch() statements. */
  const char fnc_nm[]="nco_dfl_case_tmr_typ_err()";
  (void)fprintf(stdout,"%s: ERROR switch(tmr_typ) statement fell through to default case, which is unsafe. This catch-all error handler ensures all switch(tmr_typ) statements are fully enumerated. Exiting...\n",fnc_nm);
  nco_err_exit(0,fnc_nm);
} /* end nco_dfl_case_tmr_typ_err() */

void
nco_exit /* [fnc] Wrapper for exit() */
(int rcd) /* I [enm] Return code */
{
  /* Purpose: Wrapper for exit() */
  const char fnc_nm[]="nco_exit()";
#ifdef NCO_ABORT_ON_ERROR
  const char exit_nm[]="abort()";
#else /* !NCO_ABORT_ON_ERROR */
  const char exit_nm[]="exit(EXIT_FAILURE)";
#endif /* !NCO_ABORT_ON_ERROR */

  if(rcd == EXIT_SUCCESS){
    exit(rcd);
  }else{
    if(dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"%s: ERROR exiting through %s which will now call %s\n",prg_nm_get(),fnc_nm,exit_nm);
#ifdef NCO_ABORT_ON_ERROR
    abort();
#else /* !NCO_ABORT_ON_ERROR */
    exit(rcd);
#endif /* !NCO_ABORT_ON_ERROR */
  } /* endif rcd */
} /* nco_exit() */

void 
nco_exit_gracefully(void) /* [fnc] Clean up timers, file descriptors, memory, then exit */
{
  /* Purpose: Clean up timers, file descriptors, memory, then exit */
  (void)fclose(stderr);
  (void)fclose(stdin);
  (void)fclose(stdout);

  /* This should be penultimate command in program 
     After program name is free'd, calls to prg_nm_get() will fail */
  (void)nco_free(prg_nm_get());

  nco_exit(EXIT_SUCCESS);
} /* end nco_exit_gracefully() */

nco_bool /* [flg] Program is multi-file operator */
nco_is_mlt_fl_opr /* [fnc] Query whether program is multi-file operator */
(const int prg_id) /* [enm] Program ID */
{
  /* Purpose: Is this a multi-file operator (MFO)?
     MFOs take arbitrarily large numbers of input files
     ncbo and ncflint are not MFOs because they take a fixed number (two) of input files
     MFOs attempt to read input filenames from stdin when input filenames are not positional arguments */
  switch(prg_id){
  case ncea:
  case ncecat: 
  case ncra:
  case ncrcat: 
    return True;
    break;
  case ncap: 
  case ncatted: 
  case ncbo: 
  case ncflint: 
  case nckt: 
  case ncks: 
  case ncpdq: 
  case ncrename: 
  case ncwa: 
    return False;
    break;
  default: nco_dfl_case_prg_id_err(); break;
  } /* end switch */
  return False;
} /* end nco_is_mlt_fl_opr() */

nco_bool /* [flg] Program does arithmetic */
nco_is_rth_opr /* [fnc] Query whether program does arithmetic */
(const int prg_id) /* [enm] Program ID */
{
  /* Purpose: Is operator arithmetic?
     For purposes of this function, an arithmetic operator is one which changes values
     Concatenators (ncrcat, ncecat) are not arithmetic because they just glue data
     Permutor (ncpdq) is not arithmetic because it just re-arranges values
     Packer (ncpdq) is not arithmetic because it re-represents values
     Attributors (ncrename, ncatted) are not arithmetic because they change metadata, not data
     nco_is_rth_opr() flag help control packing behavior
     Clearly, arithmetic operators must operate on unpacked values
     Hence, one use of nco_is_rth_opr() is to tell arithmetic operators to 
     automatically unpack variables when reading them
     Non-arithmetic operators do not need to unpack variables */
  switch(prg_id){
  case ncap: 
  case ncbo:
  case ncea:
  case ncflint:
  case ncra:
  case ncwa:
    return True;
    break;
  case ncatted: 
  case ncecat: 
  case ncks: 
  case nckt: 
  case ncpdq: 
  case ncrcat: 
  case ncrename: 
    return False;
    break;
  default: nco_dfl_case_prg_id_err(); break;
  } /* end switch */
  return False;
} /* end nco_is_rth_opr() */

nco_bool /* [flg] Program does arithmetic and preserves rank */
nco_is_rnk_prs_rth_opr /* [fnc] Is program rank-preserving arithmetic operator? */
(const int prg_id) /* [enm] Program ID */
{
  /* Purpose: Is program rank-preserving arithmetic operator?
     For purposes of this function, arithmetic operators change values
     Concatenators (ncrcat, ncecat) are not arithmetic because they just glue data
     Permutor (ncpdq) is not arithmetic because it just re-arranges values
     Packer (ncpdq) is not arithmetic because it re-represents values
     Attributors (ncrename, ncatted) are not arithmetic because they change metadata, not data
     One use of nco_is_rnk_prs_rth_opr() is to tell which operators should
     not process multidimensional coordinate values.
     For example, we want ncwa to act of coordinates that are reduced 
     But we do not want ncea, ncbo, or ncflint, for example, to load and process
     single or multi-dimensional coordinate variables.
     They are best treated as "fixed" variables to be copied directly from the input to the output file. */ 
  switch(prg_id){
  case ncap: 
  case ncbo:
  case ncea:
  case ncflint:
  case ncra:
    return True;
    break;
  case ncatted: 
  case ncecat: 
  case ncks: 
  case nckt: 
  case ncpdq: 
  case ncrcat: 
  case ncrename: 
  case ncwa:
    return False;
    break;
  default: nco_dfl_case_prg_id_err(); break;
  } /* end switch */
  return False;
} /* end nco_is_rnk_prs_rth_opr() */

void
nco_lbr_vrs_prn(void) /* [fnc] Print netCDF library version */
{
  /* Purpose: Print netCDF library version */

  char *cmp_dat_sng;
  char *dlr_ptr;
  char *lbr_sng;
  char *lbr_vrs_sng;
  char *of_ptr;

  size_t cmp_dat_sng_lng;
  size_t lbr_vrs_sng_lng;

  /* Behavior of nc_inq_libvers() depends on library versions. Return values are:
     netCDF 3.4--3.6.x: "3.4 of May 16 1998 14:06:16 $"
     netCDF 4.0-alpha1--4.0-alpha10: NULL
     netCDF 4.0-alpha11--4.0-alpha16: "4.0-alpha11"
     netCDF 4.0-beta1--present: "4.0-beta1" */  
  lbr_sng=(char *)strdup(nc_inq_libvers());
  /* (void)fprintf(stderr,"%s: nco_lbr_vrs_prn() returns %s\n",prg_nm_get(),lbr_sng);*/
  of_ptr=strstr(lbr_sng," of ");
  if(of_ptr == NULL){
    (void)fprintf(stderr,"%s: WARNING nco_lbr_vrs_prn() reports of_ptr == NULL\n",prg_nm_get());
    lbr_vrs_sng_lng=(size_t)strlen(lbr_sng);
  }else{
    lbr_vrs_sng_lng=(size_t)(of_ptr-lbr_sng);
  } /* endif */
  lbr_vrs_sng=(char *)nco_malloc(lbr_vrs_sng_lng+1ul);
  strncpy(lbr_vrs_sng,lbr_sng,lbr_vrs_sng_lng);
  lbr_vrs_sng[lbr_vrs_sng_lng]='\0'; /* NUL-terminate */

  dlr_ptr=strstr(lbr_sng," $");
  if(of_ptr && dlr_ptr){
    cmp_dat_sng_lng=(size_t)(dlr_ptr-of_ptr-4ul); /* 4 is the length of " of " */
    cmp_dat_sng=(char *)nco_malloc(cmp_dat_sng_lng+1ul);
    strncpy(cmp_dat_sng,of_ptr+4ul,cmp_dat_sng_lng); /* 4 is the length of " of " */
    cmp_dat_sng[cmp_dat_sng_lng]='\0'; /* NUL-terminate */
  }else{
    cmp_dat_sng=(char *)strdup("Unknown");
  } /* endif */

  (void)fprintf(stderr,"Linked to netCDF library version %s, compiled %s\n",lbr_vrs_sng,cmp_dat_sng);

  cmp_dat_sng=(char *)nco_free(cmp_dat_sng);
  lbr_vrs_sng=(char *)nco_free(lbr_vrs_sng);
  lbr_sng=(char *)nco_free(lbr_sng);
} /* end nco_lbr_vrs_prn() */

void
nco_cnf_prn(void) /* [fnc] Print NCO configuration and help text */
{
  /* Purpose: Print NCO configuration and help text */

  (void)fprintf(stdout,"Homepage URL: http://nco.sf.net\n");
  (void)fprintf(stdout,"User's Guide: http://nco.sf.net/nco.html\n");
  /* fxm: TKN2YESNO breaks when TKN is undefined
     Full macro language like M4 might be useful here, though probably too much trouble */
#define TKN2YESNO(x) ((x+0) ? ("No"):("Yes"))
  /* NB: Keep configuration option tokens consistent among configure.in, bld/Makefile, and nco_ctl.c
     Alphabetize list by first word in English text description of token */
  (void)fprintf(stderr,"Configuration Option:\tActive?\tMeaning or Reference:\nCheck _FillValue\t%s\thttp://nco.sf.net/nco.html#mss_val\nCheck missing_value\t%s\thttp://nco.sf.net/nco.html#mss_val\nCompressed netCDF3\t%s\thttp://nco.sf.net/nco.html#znetcdf (pre-alpha)\nDAP clients (libdap)\t%s\thttp://nco.sf.net/nco.html#dap\nDAP clients (libnetcdf)\t%s\thttp://nco.sf.net/nco.html#dap\nDebugging: Custom\t%s\tPedantic, bounds checking (slowest execution)\nDebugging: Symbols\t%s\tProduce symbols for debuggers (e.g., dbx, gdb)\nGNU Scientific Library\t%s\thttp://nco.sf.net/nco.html#gsl\nInternationalization\t%s\thttp://nco.sf.net/nco.html#i18n (pre-alpha)\nMPI parallelization\t%s\thttp://nco.sf.net/nco.html#mpi (beta)\nnetCDF3 64-bit files\t%s\thttp://nco.sf.net/nco.html#lfs\nnetCDF4/HDF5\t\t%s\thttp://nco.sf.net/nco.html#nco4\nOpenMP SMP threading\t%s\thttp://nco.sf.net/nco.html#omp\nOptimization: run-time\t%s\tFastest execution possible (slowest compilation)\nParallel netCDF3\t%s\thttp://nco.sf.net/nco.html#pnetcdf (pre-alpha)\nShared libraries built\t%s\tSmall, dynamically linked executables\nStatic libraries built\t%s\tLarge executables with private namespaces\nUDUnits conversions\t%s\thttp://nco.sf.net/nco.html#udunits\nUDUnits2 conversions\t%s\thttp://nco.sf.net/nco.html#udunits\nWildcarding (regex)\t%s\thttp://nco.sf.net/nco.html#rx\n%s",
		(!strcmp("_FillValue",nco_mss_val_sng_get())) ? "Yes" : "No",
		(!strcmp("missing_value",nco_mss_val_sng_get())) ? "Yes" : "No",
#if defined(ENABLE_ZNETCDF) && (ENABLE_ZNETCDF)
		"Yes",
#else /* !ENABLE_ZNETCDF */
		"No",
#endif /* !ENABLE_ZNETCDF */
#if defined(ENABLE_DAP_OPENDAP) && (ENABLE_DAP_OPENDAP)
		"Yes",
#else /* !ENABLE_DAP_OPENDAP */
		"No",
#endif /* !ENABLE_DAP_OPENDAP */
#if defined(ENABLE_DAP_NETCDF) && (ENABLE_DAP_NETCDF)
		"Yes",
#else /* !ENABLE_DAP_NETCDF */
		"No",
#endif /* !ENABLE_DAP_NETCDF */
#if defined(ENABLE_DEBUG_CUSTOM) && (ENABLE_DEBUG_CUSTOM)
		"Yes",
#else /* !ENABLE_DEBUG_CUSTOM */
		"No",
#endif /* !ENABLE_DEBUG_CUSTOM */
#if defined(ENABLE_DEBUG_SYMBOLS) && (ENABLE_DEBUG_SYMBOLS)
		"Yes",
#else /* !ENABLE_DEBUG_SYMBOLS */
		"No",
#endif /* !ENABLE_DEBUG_SYMBOLS */
#if defined(ENABLE_GSL) && (ENABLE_GSL)
		"Yes",
#else /* !ENABLE_GSL */
		"No",
#endif /* !ENABLE_GSL */
#if defined(I18N) && (I18N)
		"Yes",
#else /* !I18N */
		"No",
#endif /* !I18N */
#if defined(ENABLE_MPI) && (ENABLE_MPI)
		"Yes",
#else /* !ENABLE_MPI */
		"No",
#endif /* !ENABLE_MPI */
#if defined(NC_64BIT_OFFSET) && (NC_64BIT_OFFSET)
		"Yes",
#else /* !NC_64BIT_OFFSET */
		"No",
#endif /* !NC_64BIT_OFFSET */
#if defined(ENABLE_NETCDF4) && (ENABLE_NETCDF4)
		"Yes",
#else /* !ENABLE_NETCDF4 */
		"No",
#endif /* !ENABLE_NETCDF4 */
#if defined(_OPENMP) && (_OPENMP)
		"Yes",
#else /* !_OPENMP */
		"No",
#endif /* !_OPENMP */
#if defined(ENABLE_OPTIMIZE_CUSTOM) && (ENABLE_OPTIMIZE_CUSTOM)
		"Yes",
#else /* !ENABLE_OPTIMIZE_CUSTOM */
		"No",
#endif /* !ENABLE_OPTIMIZE_CUSTOM */
#if defined(ENABLE_PNETCDF) && (ENABLE_PNETCDF)
		"Yes",
#else /* !ENABLE_PNETCDF */
		"No",
#endif /* !ENABLE_PNETCDF */
#if defined(ENABLE_SHARED) && (ENABLE_SHARED)
		"Yes",
#else /* !ENABLE_SHARED */
		"No",
#endif /* !ENABLE_SHARED */
#if defined(ENABLE_STATIC) && (ENABLE_STATIC)
		"Yes",
#else /* !ENABLE_STATIC */
		"No",
#endif /* !ENABLE_STATIC */
#if defined(ENABLE_UDUNITS) && (ENABLE_UDUNITS)
		"Yes",
#else /* !ENABLE_UDUNITS */
		"No",
#endif /* !ENABLE_UDUNITS */
#if defined(HAVE_UDUNITS2_H) && (HAVE_UDUNITS2_H)
		"Yes",
#else /* !HAVE_UDUNITS2_H */
		"No",
#endif /* !HAVE_UDUNITS2_H */
#if defined(NCO_HAVE_REGEX_FUNCTIONALITY) && (NCO_HAVE_REGEX_FUNCTIONALITY)
		"Yes",
#else /* !NCO_HAVE_REGEX_FUNCTIONALITY */
		"No",
#endif /* !NCO_HAVE_REGEX_FUNCTIONALITY */
		""); /* End of print statement marker */
  (void)fprintf(stderr,"\n%s",nco_nmn_get());
} /* end nco_cnf_prn() */

const char * /* O [sng] Mnemonic that describes current NCO version */
nco_nmn_get(void) /* [fnc] Return mnemonic that describes current NCO version */
{ 
  /* Purpose: Return mnemonic describing current NCO version */
  return "Avatar: The time of great sorrow\n";
} /* end nco_nmn_get() */

char * /* O [sng] nm_in stripped of any path (i.e., program name stub) */ 
prg_prs /* [fnc] Strip program name to stub and return program ID */
(const char * const nm_in, /* I [sng] Name of program, i.e., argv[0] (may include path prefix) */
 int * const prg_lcl) /* O [enm] Enumerated number corresponding to nm_in */
{
  /* Purpose: Set program name and enum */
  char *nm_out;
  char *nm_out_tmp;
  char *nm_out_orig;

  /* Get program name (use strrchr() first in case nm_in contains a path) */
  nm_out_orig=nm_out_tmp=(char *)strdup(nm_in);
  if(strrchr(nm_out_tmp,'/')) nm_out_tmp=strrchr(nm_out_tmp,'/')+1;

  /* Skip possible libtool prefix */
  if(!strncmp(nm_out_tmp,"lt-",3)){nm_out_tmp+=3;}

  /* Classify calling program */
  /* ncap and acceptable synonyms (symbolic links): */
  if(!strcmp(nm_out_tmp,"ncap")){*prg_lcl=ncap;}
  else if(!strcmp(nm_out_tmp,"ncap2")){*prg_lcl=ncap;}
  /* ncatted and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncatted")){*prg_lcl=ncatted;}
  /* ncbo and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"mpncbo")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out_tmp,"mpncdiff")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out_tmp,"ncadd")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out_tmp,"ncbo")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out_tmp,"ncdiff")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out_tmp,"ncdivide")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out_tmp,"ncmult")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out_tmp,"ncmultiply")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out_tmp,"ncsub")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out_tmp,"ncsubtract")){*prg_lcl=ncbo;}
  /* ncea and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncea")){*prg_lcl=ncea;}
  else if(!strcmp(nm_out_tmp,"mpncea")){*prg_lcl=ncea;}
  /* ncecat and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncecat")){*prg_lcl=ncecat;}
  else if(!strcmp(nm_out_tmp,"mpncecat")){*prg_lcl=ncecat;}
  /* ncflint and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncflint")){*prg_lcl=ncflint;}
  else if(!strcmp(nm_out_tmp,"mpncflint")){*prg_lcl=ncflint;}
  /* ncks and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncks")){*prg_lcl=ncks;}
  else if(!strcmp(nm_out_tmp,"nckt")){*prg_lcl=ncks;}
  /* ncpdq and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncpdq")){*prg_lcl=ncpdq;}
  else if(!strcmp(nm_out_tmp,"mpncpdq")){*prg_lcl=ncpdq;}
  else if(!strcmp(nm_out_tmp,"ncpack")){*prg_lcl=ncpdq;}
  else if(!strcmp(nm_out_tmp,"ncunpack")){*prg_lcl=ncpdq;}
  /* ncra and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncra")){*prg_lcl=ncra;}
  else if(!strcmp(nm_out_tmp,"mpncra")){*prg_lcl=ncra;}
  /* ncrcat and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncrcat")){*prg_lcl=ncrcat;}
  else if(!strcmp(nm_out_tmp,"mpncrcat")){*prg_lcl=ncrcat;}
  /* ncrename and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncrename")){*prg_lcl=ncrename;}
  /* ncwa and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncwa")){*prg_lcl=ncwa;}
  else if(!strcmp(nm_out_tmp,"mpncwa")){*prg_lcl=ncwa;}
  else{
    (void)fprintf(stdout,"%s: ERROR executable name %s not registered in prg_prs()\n",nm_out_tmp,nm_out_tmp);
    nco_exit(EXIT_FAILURE);
  } /* end else */

  /* Duplicate stub for returning */
  nm_out=(char *)strdup(nm_out_tmp);
  /* Free copy of argv[0] */
  nm_out_orig=(char *)nco_free(nm_out_orig);
  return nm_out;

} /* end prg_prs() */

void 
nco_usg_prn(void)
{
  /* Purpose: Print correct command-line usage (currently to stdout) */

  char *opt_sng=NULL_CEWI;

  int prg_lcl;

  prg_lcl=prg_get();

  switch(prg_lcl){
  case ncap:
    opt_sng=(char *)strdup("[-3] [-4] [-6] [-A] [-C] [-c] [-D dbg_lvl] [-F] [-f] [-h] [-L lvl] [-l path] [-O] [-o out.nc] [-p path] [-R] [-r] [-s algebra] [-S fl.nco] [-t thr_nbr] [-v] in.nc [out.nc]\n");
    break;
  case ncatted:
    opt_sng=(char *)strdup("[-a ...] [-D dbg_lvl] [-h] [--hdr_pad nbr] [-l path] [-O] [-o out.nc] [-p path] [-R] [-r] in.nc [[out.nc]]\n");
    break;
  case ncbo:
    opt_sng=(char *)strdup("[-3] [-4] [-6] [-A] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-h] [-L lvl] [-l path] [-n ...] [-O] [-o out.nc] [-p path] [-R] [-r] [-t thr_nbr] [-v ...] [-X box] [-x] [-y op_typ] in_1.nc in_2.nc [out.nc]\n");
    break;
  case ncflint:
    opt_sng=(char *)strdup("[-3] [-4] [-6] [-A] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-h] [-i var,val] [-L lvl] [-l path] [-O] [-o out.nc] [-p path] [-R] [-r] [-t thr_nbr] [-v ...] [-X box] [-x] [-w wgt_1[,wgt_2]] in_1.nc in_2.nc [out.nc]\n");
    break;
  case ncks:
    opt_sng=(char *)strdup("[-3] [-4] [-6] [-A] [-a] [-B] [-b fl_bnr] [-C] [-c] [--cnk_dmn nm,sz] [--cnk_map map] [--cnk_plc plc] [--cnk_scl sz] [-D dbg_lvl] [-d ...] [-F] [--fix_rec_dmn] [-H] [-h] [--hdr_pad nbr] [-L lvl] [-l path] [-m] [--mk_rec_dmn dim] [-M] [-O] [-o out.nc] [-P] [-p path] [-Q] [-q] [-R] [-r] [-s format] [-u] [-v ...] [-X box] [-x] in.nc [[out.nc]]\n");
    break;
  case ncpdq:
    opt_sng=(char *)strdup("[-3] [-4] [-6] [-A] [-a ...] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-h] [-L lvl] [-l path] [-M pck_map][-O] [-o out.nc] [-P pck_plc] [-p path] [-R] [-r] [-t thr_nbr] [-v ...] [-U] [-X box] [-x] in.nc [out.nc]\n");
    break;
  case ncra:
  case ncea:
    opt_sng=(char *)strdup("[-3] [-4] [-6] [-A] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-H] [-h] [-L lvl] [-l path] [-n ...] [-O] [-o out.nc] [-p path] [-R] [-r] [-t thr_nbr] [-v ...] [-X box] [-x] [-y op_typ] in.nc [...] [out.nc]\n");
    break;
  case ncrcat:
    opt_sng=(char *)strdup("[-3] [-4] [-6] [-A] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-H] [-h] [-L lvl] [-l path] [-n ...] [-O] [-o out.nc] [-p path] [-R] [-r] [-t thr_nbr] [-v ...] [-X box] [-x] in.nc [...] [out.nc]\n");
    break;
  case ncecat:
    opt_sng=(char *)strdup("[-3] [-4] [-6] [-A] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-H] [-h] [-L lvl] [-l path] [-M] [-n ...] [-O] [-o out.nc] [-p path] [-R] [-r] [-t thr_nbr] [-u ulm_nm] [-v ...] [-X box] [-x] in.nc [...] [out.nc]\n");
    break;
  case ncrename:
    opt_sng=(char *)strdup("[-a ...] [-D dbg_lvl] [-d ...] [-h] [--hdr_pad nbr] [-l path] [-O] [-o out.nc] [-p path] [-R] [-r] [-v ...] in.nc [[out.nc]]\n");
    break;
  case ncwa:
    opt_sng=(char *)strdup("[-3] [-4] [-6] [-A] [-a ...] [-B mask_cond] [-b] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-h] [-I] [-L lvl] [-l path] [-m mask] [-M mask_val] [-N] [-O] [-o out.nc] [-p path] [-R] [-r] [-T mask_comp] [-t thr_nbr] [-v ...] [-w wgt] [-x] [-y op_typ] in.nc [out.nc]\n");
    break;
  default: nco_dfl_case_prg_id_err(); break;
  } /* end switch */
  
  /* We now have command-specific command line option string */
  (void)fprintf(stdout,"%s command line options cheat-sheet:\n",prg_nm_get());
  (void)fprintf(stdout,"%s %s\n",prg_nm_get(),opt_sng);

  if(strstr(opt_sng,"-3")) (void)fprintf(stdout,"-3, --3, --fl_fmt=classic\tOutput file in netCDF3 CLASSIC (32-bit offset) storage format\n");
#ifdef ENABLE_NETCDF4
  if(strstr(opt_sng,"-4")) (void)fprintf(stdout,"-4, --4, --netcdf4\t\tOutput file in netCDF4 (HDF5) storage format\n");
#endif /* !ENABLE_NETCDF4 */
  if(strstr(opt_sng,"-6")) (void)fprintf(stdout,"-6, --64, --fl_fmt=64bit\tOutput file in netCDF3 64-bit offset storage format\n");
  if(strstr(opt_sng,"-A")) (void)fprintf(stdout,"-A, --apn, --append\tAppend to existing output file, if any\n");
  if(strstr(opt_sng,"-a")){
    if(prg_lcl == ncatted) (void)fprintf(stdout,"-a, --attribute att_nm,var_nm,mode,att_typ,att_val Attribute specification:\n\t\t\tmode = a,c,d,m,o and att_typ = f,d,l,s,c,b\n");
#ifdef ENABLE_NETCDF4
    if(prg_lcl == ncatted) (void)fprintf(stdout,"\t\t\tnetCDF4 att_typ's = ub,us,u,ll,ull\n");
#endif /* !ENABLE_NETCDF4 */
    if(prg_lcl == ncks) (void)fprintf(stdout,"-a, --abc, --alphabetize\tDisable alphabetization of extracted variables\n");
    if(prg_lcl == ncpdq) (void)fprintf(stdout,"-a, --arrange, --permute, --reorder, --rdr [-]rdr_dim1[,[-]rdr_dim2[...]] Re-order dimensions\n");
    if(prg_lcl == ncrename) (void)fprintf(stdout,"-a, --attribute old_att,new_att Attribute's old and new names\n");
    if(prg_lcl == ncwa) (void)fprintf(stdout,"-a, --avg, --average avg_dim1[,avg_dim2[...]] Averaging dimensions\n");
  } /* end if */
  if(strstr(opt_sng,"-B")){
    if(prg_lcl == ncks) (void)fprintf(stdout,"-B, --bnr, --binary\tWrite data to unformatted binary file\n");
    if(prg_lcl == ncwa) (void)fprintf(stdout,"-B, --msk_cnd, --mask_condition mask_cond\tMask condition (e.g., \"ORO < 1\")\n");
  } /* end if -B */
  if(strstr(opt_sng,"-b")){
    if(prg_lcl == ncks) (void)fprintf(stdout,"-b, --fl_bnr, --binary-file fl_bnr\tUnformatted binary file to write\n");
    if(prg_lcl == ncwa) (void)fprintf(stdout,"-b, --rdd, --retain-degenerate-dimensions\tRetain degenerate dimensions\n");
  } /* end if -b */
  if(strstr(opt_sng,"-C")) (void)fprintf(stdout,"-C, --nocoords\t\tAssociated coordinate variables should not be processed\n");
  if(strstr(opt_sng,"-c")) (void)fprintf(stdout,"-c, --crd, --coords\tCoordinate variables will all be processed\n");
  if(strstr(opt_sng,"--cnk_dmn")) (void)fprintf(stdout,"    --cnk_dmn, --chunk_dimension nm,sz\tChunksize of dimension nm is sz\n");
  if(strstr(opt_sng,"--cnk_map")) (void)fprintf(stdout,"    --cnk_map, --chunk_map map\t\tChunking map [dmn,rd1,prd,xpl]\n");
  if(strstr(opt_sng,"--cnk_plc")) (void)fprintf(stdout,"    --cnk_plc, --chunk_policy plc\tChunking policy [all,g2d,g3d,xpl,uck]\n");
  if(strstr(opt_sng,"--cnk_scl")) (void)fprintf(stdout,"    --cnk_scl, --chunk_scalar sz\tChunksize scalar (for all dimensions)\n");
  if(strstr(opt_sng,"-D")) (void)fprintf(stdout,"-D, --dbg_lvl, --debug-level dbg_lvl\tDebugging level\n");
  if(strstr(opt_sng,"-d")){
    if(prg_lcl == ncrename) (void)fprintf(stdout,"-d, --dmn, --dimension old_dim,new_dim Dimension's old and new names\n");
    else (void)fprintf(stdout,"-d, --dmn, --dimension dim,[min][,[max]][,[stride]] Dimension's limits and stride in hyperslab\n");
  } /* end if -d */
  if(strstr(opt_sng,"-F")) (void)fprintf(stdout,"-F, --ftn, --fortran\tFortran indexing conventions (1-based) for I/O\n");
  if(strstr(opt_sng,"[-f]")) (void)fprintf(stdout,"-f, --fnc_tbl, --prn_fnc_tbl\tPrint function table\n");
  if(strstr(opt_sng,"--fix_rec_dmn")) (void)fprintf(stdout,"    --fix_rec_dmn\tChange record dimension into fixed dimension in output file\n");
#ifdef ENABLE_NETCDF4
  if(strstr(opt_sng,"--fl_fmt")) (void)fprintf(stdout,"--fl_fmt, --file_format format\tFile format [classic,64bit,netcdf4,netcdf4_classic]\n");
#else
  if(strstr(opt_sng,"--fl_fmt")) (void)fprintf(stdout,"--fl_fmt, --file_format format\tFile format [classic,64bit]\n");
#endif /* !ENABLE_NETCDF4 */
  if(strstr(opt_sng,"-H")){
    if(prg_lcl == ncks) (void)fprintf(stdout,"-H, --huh, --hmm\tToggle printing data\n");
    if(nco_is_mlt_fl_opr(prg_lcl)) (void)fprintf(stdout,"-H, --fl_lst_in, --file_list\tDo not create \"input_file_list\" global attribute\n");
  } /* end if -H */
  /* Disambiguate -h from --hdr_pad */
  if(strstr(opt_sng,"[-h]")) (void)fprintf(stdout,"-h, --hst, --history\tDo not append to \"history\" global attribute\n");
  if(strstr(opt_sng,"--hdr_pad")) (void)fprintf(stdout,"    --hdr_pad, --header_pad\tPad output header with nbr bytes\n");
  if(strstr(opt_sng,"-i")) (void)fprintf(stdout,"-i, --ntp, --interpolate var,val\tInterpolant and value\n");
  if(strstr(opt_sng,"-I")) (void)fprintf(stdout,"-I, --wgt_msk_crd_var\tDo not weight or mask coordinate variables\n");
#ifdef ENABLE_NETCDF4
  if(strstr(opt_sng,"-L")) (void)fprintf(stdout,"-L, --dfl_lvl, --deflate lvl\tLempel-Ziv deflation (lvl=0..9) for netCDF4 output\n");
#endif /* !ENABLE_NETCDF4 */
  if(strstr(opt_sng,"-l")) (void)fprintf(stdout,"-l, --lcl, --local path\tLocal storage path for remotely-retrieved files\n");
  if(strstr(opt_sng,"-M")){
    if(prg_lcl == ncecat) (void)fprintf(stdout,"-M, --glb_mtd_spr\tDo not copy global metadata\n");
    if(prg_lcl == ncks) (void)fprintf(stdout,"-M, --Mtd, --Metadata\tToggle printing global metadata\n");
    if(prg_lcl == ncpdq) (void)fprintf(stdout,"-M, --pck_map, --pack_map, --map pck_map\tPack map [flt_sht,flt_byt,hgh_sht,hgh_byt,nxt_lsr]\n");
    if(prg_lcl == ncwa) (void)fprintf(stdout,"-M, --msk_val, --mask-value, --mask_value mask_val\tMasking value (default is 1.0)\n");
  } /* end if */
  if(strstr(opt_sng,"-m")){
    if(prg_lcl == ncwa) (void)fprintf(stdout,"-m, --msk_nm, --msk_var, --mask-variable, --mask_variable mask_var\tMasking variable name\n");
    if(prg_lcl == ncks) (void)fprintf(stdout,"-m, --mtd, --metadata\tToggle printing variable metadata\n");
  } /* end if */
  if(strstr(opt_sng,"--mk_rec_dmn")) (void)fprintf(stdout,"    --mk_rec_dmn dim\tDefine dim as record dimension in output file\n");
  if(strstr(opt_sng,"-N")) (void)fprintf(stdout,"-N, --nmr, --numerator\tNo normalization\n");
  if(strstr(opt_sng,"-n")){
    /*    if(prg_lcl == ncwa) (void)fprintf(stdout,"-n\t\tNormalize by tally but not weight\n");*/
    if(prg_lcl != ncwa) (void)fprintf(stdout,"-n, --nintap nbr_files,[nbr_numeric_chars[,increment]] NINTAP-style abbreviation of file list\n");
  } /* end if -n */
  if(strstr(opt_sng,"-o")) (void)fprintf(stdout,"-o, --output, --fl_out fl_out\tOutput file name (or use last positional argument)\n");
  if(strstr(opt_sng,"-O")) (void)fprintf(stdout,"-O, --ovr, --overwrite\tOverwrite existing output file, if any\n");
  if(strstr(opt_sng,"-P")){
    if(prg_lcl == ncks) (void)fprintf(stdout,"-P, --prn, --print\tPrint data, metadata, and units. Abbreviation for -C -H -M -m -u.\n");
    if(prg_lcl == ncpdq) (void)fprintf(stdout,"-P, --pck_plc, --pack_policy pck_plc\tPacking policy [all_new,all_xst,xst_new,upk]\n");
  } /* end if -P */
  if(strstr(opt_sng,"-p")) (void)fprintf(stdout,"-p, --pth, --path path\tPath prefix for all input filenames\n");
  if(strstr(opt_sng,"-Q")) (void)fprintf(stdout,"-Q, \t\t\tToggle printing of dimension indices and coordinate values\n");
  if(strstr(opt_sng,"-q")) (void)fprintf(stdout,"-q, --quiet\t\tTurn off all printing to screen\n");
  if(strstr(opt_sng,"-R")) (void)fprintf(stdout,"-R, --rtn, --retain\tRetain remotely-retrieved files after use\n");
  if(strstr(opt_sng,"-r")) (void)fprintf(stdout,"-r, --revision, --vrs, --version\tCompile-time configuration and/or program version\n");
  if(strstr(opt_sng,"-s")){
    if(prg_lcl != ncap) (void)fprintf(stdout,"-s, --sng_fmt, --string format\tString format for text output\n");
    if(prg_lcl == ncap) (void)fprintf(stdout,"-s, --spt, --script algebra\tAlgebraic command defining single output variable\n");
  } /* end if */
  if(strstr(opt_sng,"-S")) (void)fprintf(stdout,"-S, --fl_spt, --script-file fl.nco\tScript file containing multiple algebraic commands\n");
  if(strstr(opt_sng,"-T")) (void)fprintf(stdout,"-T, --mask_comparator, --msk_cmp_typ, --op_rlt comparator\tComparator for mask condition: eq,ne,ge,le,gt,lt\n");
  if(strstr(opt_sng,"-t")) (void)fprintf(stdout,"-t, --thr_nbr, --threads, --omp_num_threads thr_nbr\tThread number for OpenMP\n");
  if(strstr(opt_sng,"-u")){
    if(prg_lcl == ncks) (void)fprintf(stdout,"-u, --units\t\tToggle printing units of variables, if any\n");
    if(prg_lcl == ncecat) (void)fprintf(stdout,"-u, --ulm_nm --rcd_nm\t\tNew unlimited (record) dimension name\n");
  } /* end if */
  if(strstr(opt_sng,"-U")) (void)fprintf(stdout,"-U, --upk, --unpack\tUnpack input file\n");
  if(strstr(opt_sng,"-v")){
    if(prg_lcl == ncrename) (void)fprintf(stdout,"-v, --variable old_var,new_var Variable's old and new names\n");
	if(prg_lcl == ncap) (void)fprintf(stdout,"-v, --variable \t\tOutput file includes ONLY user-defined variables\n");
    if(prg_lcl != ncrename && prg_lcl != ncap) (void)fprintf(stdout,"-v, --variable var1[,var2[...]] Variable(s) to process (regular expressions supported)\n");
  } /* end if */
  /*  if(strstr(opt_sng,"-W")) (void)fprintf(stdout,"-W\t\tNormalize by weight but not tally\n");*/
  if(strstr(opt_sng,"-w")){
    if(prg_lcl == ncwa) (void)fprintf(stdout,"-w, --wgt_var, --weight wgt\tWeighting variable name\n");
    if(prg_lcl == ncflint) (void)fprintf(stdout,"-w, --wgt_var, --weight wgt_1[,wgt_2] Weight(s) of file(s)\n");
  } /* end if */
  if(strstr(opt_sng,"-X")) (void)fprintf(stdout,"-X, --auxiliary lon_min,lon_max,lat_min,lat_max\tAuxiliary coordinate bounding box\n");
  if(strstr(opt_sng,"-x")) (void)fprintf(stdout,"-x, --xcl, --exclude\tExtract all variables EXCEPT those specified with -v\n");
  if(strstr(opt_sng,"-y")){
    if(prg_lcl == ncbo)(void)fprintf(stdout,"-y, --op_typ, --operation op_typ\tBinary arithmetic operation: add,sbt,mlt,dvd (+,-,*,/)\n");
    if(prg_lcl == ncra || prg_lcl == ncea || prg_lcl == ncwa)(void)fprintf(stdout,"-y, --op_typ, --operation op_typ\tArithmetic operation: avg,min,max,ttl,sqravg,avgsqr,sqrt,rms,rmssdn\n");
  }
  /* All operators have input files, no need to strstr(in.nc) */
  if(prg_lcl == ncbo || prg_lcl == ncflint){
    (void)fprintf(stdout,"in_1.nc in_2.nc\t\tInput file names\n");
  }else{
    if(nco_is_mlt_fl_opr(prg_lcl)) (void)fprintf(stdout,"in.nc [...]\t\tInput file names\n"); else (void)fprintf(stdout,"in.nc\t\t\tInput file name\n");
  } /* endif in.nc */
  if(strstr(opt_sng,"[out.nc]")) (void)fprintf(stdout,"out.nc\t\t\tOutput file name (or use -o switch)\n");
/*  if(strstr(opt_sng,"-")) (void)fprintf(stdout,"-\n");*/

  /* Free the space holding option string */
  opt_sng=(char *)nco_free(opt_sng);

  /* Public service announcements */
  (void)fprintf(stdout,"\nEight ways to find more help on %s and/or NCO:\n",prg_nm_get());
  (void)fprintf(stdout,"1. Concrete %s examples: http://nco.sf.net/nco.html#xmp_%s\n",prg_nm_get(),prg_nm_get());
  (void)fprintf(stdout,"2. Reference manual for %s: http://nco.sf.net/nco.html#%s\n",prg_nm_get(),prg_nm_get());
  (void)fprintf(stdout,"3. User's Guide: http://nco.sf.net#RTFM\n");
  (void)fprintf(stdout,"4. Man pages: \'man %s\', \'man nco\'\n",prg_nm_get());
  (void)fprintf(stdout,"5. Homepage: http://nco.sf.net\n");
  (void)fprintf(stdout,"6. FAQ: http://nco.sf.net#FAQ\n");
  (void)fprintf(stdout,"7. Help Forum: http://sf.net/forum/forum.php?forum_id=9830\n");
  (void)fprintf(stdout,"8. NCO-related publications: http://nco.sf.net#pub\n");
  (void)fprintf(stdout,"Post questions, suggestions, patches at http://sf.net/projects/nco\n");

} /* end nco_usg_prn() */

