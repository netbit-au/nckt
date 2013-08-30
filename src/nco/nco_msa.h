/* $Header: /cvsroot/nco/nco/src/nco/nco_msa.h,v 1.34 2010/01/05 20:02:18 zender Exp $ */

/* Purpose: Multi-slabbing algorithm */

/* Copyright (C) 1995--2010 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_msa.h" *//* Multi-slabbing algorithm */

#ifndef NCO_MSA_H
#define NCO_MSA_H

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, qsort */
#include <limits.h> /* need LONG_MAX */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_bnr.h" /* Binary write utilities */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_mmr.h" /* Memory management */
#include "nco_prn.h" /* print format functions */
#include "nco_sng_utl.h" /* sng_ascii_trn */
#include "nco_lmt.h"     /* free lmt_sct */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void
nco_cpy_var_val_mlt_lmt /* [fnc] Copy variable data from input to output file */
(const int in_id, /* I [id] netCDF input file ID */
 const int out_id, /* I [id] netCDF output file ID */
 FILE * const fp_bnr, /* I [fl] Unformatted binary output file handle */
 const nco_bool NCO_BNR_WRT, /* I [flg] Write binary file */
 char *var_nm, /* I [sng] Variable name */
 lmt_all_sct * const * lmt_lst, /* I multi-hyperslab limits */
 int nbr_dmn_fl); /* I [nbr] Number of multi-hyperslab limits */
  
void 
nco_msa_c_2_f /* [fnc] Replace brackets with parentheses in a string */
(char *sng); /* [sng] String to change from C to Fortran notation */

nco_bool /* if false then we are at the end of the slab */
nco_msa_clc_idx
(nco_bool NORMALIZE,         /* Return indices of slab within the slab */
 lmt_all_sct *lmt_a,         /* I list of lmts for each dimension  */
 long *indices,          /* I/O so routine can keep track of where its at */
 lmt_sct* lmt_out,      /* O  output hyperslab */
 int *slb );             /* slab which the above limit refers to */ 

void 
nco_msa_prn_idx    /* [fnc] Print multiple hyperslab indices  */
(lmt_all_sct * lmt_lst); 
  
void 
nco_msa_clc_cnt    /* [fnc] Calculate size of  multiple hyperslab */ 
(lmt_all_sct *lmt_lst); 

void
nco_msa_wrp_splt /* [fnc] Split wrapped dimensions */
(lmt_all_sct *lmt_lst);
  
nco_bool           /* O return true if limits overlap */
nco_msa_ovl        /* [fnc] see if limits overlap */ 
(lmt_all_sct *lmt_lst);

int               /* O [enm] Comparison result [<,=,>] 0 iff val_1 [<,==,>] val_2 */ 
nco_cmp_lmt_srt   /* [fnc]  Sort comparison operator */
(const void *vp1,
 const void* vp2);

void             
nco_msa_qsort_srt  /* [fnc] sort limits by srt values  */
(lmt_all_sct *lmt_lst);

void      
nco_msa_lmt_all_int     /* [fnc] Initilaize lmt_all_sct's */ 
(int in_id,
 nco_bool MSA_USR_RDR,
 lmt_all_sct **lmt_all_lst,
 int nbr_dmn_fl,
 lmt_sct** lmt,
 int lmt_nbr);


void *          /* O pointer to malloced slab */
nco_msa_rec_clc /* Multi slab algorithm (recursive routine) */
(int i,             /* current depth, we start at 0 */
 int imax,          /* maximium depth (i.e the number of dims in variable (does not change)*/		 
 lmt_sct **lmt,    /* limits of the current hyperslab these change as we recurse */
 lmt_all_sct **lmt_lst, /* list of limits in each dimension (this remains STATIC as we recurse */
 var_sct *var1);    /* Infor for routine to read var (should not change */
  
long /* O [idx] Minimum value */
nco_msa_min_idx /* [fnc] Find minimum values in current and return minimum value */
(const long * const current, /* I [idx] Current indices */
 nco_bool * const mnm, /* O [flg] Minimum */
 const int size); /* I [nbr] Size of current and min */

void            
nco_msa_ram_2_dsk   /* [fnc] convert hyperlsab indices into indices relative to disk */ 
(long *dmn_sbs_ram,   /* Input indices */
 lmt_all_sct** lmt_mult,   /* input hyperlab limits     */
 int nbr_dmn,         /* number of dimensions */    
 long *dmn_sbs_dsk,  /* Output - indices relative to disk */
 nco_bool FREE);        /* Free static space on last call */

void
nco_msa_prn_var_val   /* [fnc] Print variable data */
(const int in_id, /* I [id] netCDF input file ID */
 const char * const var_nm, /* I [sng] Variable name */
 lmt_all_sct *  const * lmt_lst, /* I [sct] Dimension limits */
 const int lmt_nbr, /* I [nbr] number of dimensions with user-specified limits */
 char * const dlm_sng, /* I [sng] User-specified delimiter string, if any */
 const nco_bool FORTRAN_IDX_CNV, /* I [flg] Hyperslab indices obey Fortran convention */
 const nco_bool PRN_DMN_UNITS, /* I [flg] Print units attribute, if any */
 const nco_bool PRN_DMN_IDX_CRD_VAL, /* I [flg] Print dimension/coordinate indices/values */
 const nco_bool PRN_DMN_VAR_NM); /* I [flg] Print dimension/variable names */

void
nco_msa_var_get  /* [fnc] Get var data from disk taking accound of multihyperslabs */
(const int in_id,  /* I [id] netCDF input file ID */
 var_sct *var_in, 
 lmt_all_sct * const * lmt_lst, /* I multi-hyperslab limits */
 int nbr_dmn_fl); /* I [nbr] Number of multi-hyperslab limits */

void
nco_msa_var_val_cpy /* [fnc] Copy variables data from input to output file */
(const int in_id, /* I [enm] netCDF file ID */
 const int out_id, /* I [enm] netCDF output file ID */
 var_sct ** const var, /* I/O [sct] Variables to copy to output file */
 const int nbr_var,  /* I [nbr] Number of variables */
 lmt_all_sct * const * lmt_lst, /* I multi-hyperslab limits */
 int nbr_dmn_fl); /* I [nbr] Number of multi-hyperslab limits */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_MSA_H */
