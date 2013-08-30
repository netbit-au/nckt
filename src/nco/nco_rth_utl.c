/* $Header: /cvsroot/nco/nco/src/nco/nco_rth_utl.c,v 1.37 2010/01/27 09:36:31 zender Exp $ */

/* Purpose: Arithmetic controls and utilities */

/* Copyright (C) 1995--2010 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_rth_utl.h" /* Arithmetic controls and utilities */

nco_rth_prc_rnk_enm /* [enm] Ranked precision of arithmetic type */
nco_rth_prc_rnk /* [fnc] Rank precision of arithmetic type */
(const nc_type nco_typ) /* I [enm] netCDF type of operand */
{
  /* Purpose: Ranked precision of arithmetic type */
  switch(nco_typ){
  case NC_FLOAT: return nco_rth_prc_rnk_float;
  case NC_DOUBLE: return nco_rth_prc_rnk_double;
  case NC_INT: return nco_rth_prc_rnk_int;
  case NC_SHORT: return nco_rth_prc_rnk_short;
  case NC_CHAR: return nco_rth_prc_rnk_char;
  case NC_BYTE: return nco_rth_prc_rnk_byte;
  case NC_UBYTE: return nco_rth_prc_rnk_ubyte;
  case NC_USHORT: return nco_rth_prc_rnk_ushort;
  case NC_UINT: return nco_rth_prc_rnk_uint;
  case NC_INT64: return nco_rth_prc_rnk_int64;
  case NC_UINT64: return nco_rth_prc_rnk_uint64;
  case NC_STRING: return nco_rth_prc_rnk_string;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (nco_rth_prc_rnk_enm)0;
} /* end nco_rth_prc_rnk() */

void 
nco_opr_drv /* [fnc] Intermediate control of arithmetic operations for ncra/ncea */
(const long idx_rec, /* I [idx] Index of current record */
 const int nco_op_typ, /* I [enm] Operation type */
 const var_sct * const var_prc, /* I [sct] Variable in input file */
 var_sct * const var_prc_out) /* I/O [sct] Variable in output file */
{
  /* Purpose: Perform appropriate ncra/ncea operation (avg, min, max, ttl, ...) on operands
     nco_opr_drv() is called within the record loop of ncra, and within file loop of ncea
     These operations perform some, but not all, of necessary operations for each procedure
     Most arithmetic operations require additional procedures such as normalization be performed after all files/records have been processed */
  
  /* NCO's paradigm is that coordinate variables represent grid axes
     Reducing such grids to a single-value must be done
     The most representative value of the grid is the average 
     The total, min, max, rms, etc. of the grid usually makes no sense
     Users are most interested in the mean grid coordinate */
  if(var_prc->is_crd_var){
    (void)nco_var_add_tll_ncra(var_prc->type,var_prc->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->tally,var_prc->val,var_prc_out->val);
    return;
  } /* !var_prc->is_crd_var */

  /* var_prc_out->type and var_prc->type should be equal and thus interchangeable
     var_prc_out->sz and var_prc->sz should be equal and thus interchangeable */
  switch (nco_op_typ){
  case nco_op_min: /* Minimum */
    /* On first loop, simply copy variables from var_prc to var_prc_out */
    if(idx_rec == 0) (void)var_copy(var_prc->type,var_prc->sz,var_prc->val,var_prc_out->val); else	  
      (void)nco_var_min_bnr(var_prc_out->type,var_prc_out->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->val,var_prc_out->val);
    break;
  case nco_op_max: /* Maximium */
    /* On first loop, simply copy variables from var_prc to var_prc_out */
    if(idx_rec == 0) (void)var_copy(var_prc->type,var_prc->sz,var_prc->val,var_prc_out->val); else
      (void)nco_var_max_bnr(var_prc_out->type,var_prc_out->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->val,var_prc_out->val);
    break;	
  case nco_op_avg: /* Average */
  case nco_op_sqrt: /* Squareroot will produce the squareroot of the mean */
  case nco_op_ttl: /* Total */
  case nco_op_sqravg: /* Square of the mean */
    (void)nco_var_add_tll_ncra(var_prc->type,var_prc->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->tally,var_prc->val,var_prc_out->val);
    break;
  case nco_op_rms: /* Root mean square */
  case nco_op_rmssdn: /* Root mean square normalized by N-1 */
  case nco_op_avgsqr: /* Mean square */
    /* Square values in var_prc first */
    nco_var_mlt(var_prc->type,var_prc->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->val,var_prc->val);
    /* Sum the squares */
    (void)nco_var_add_tll_ncra(var_prc_out->type,var_prc_out->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->tally,var_prc->val,var_prc_out->val);
    break;
  } /* end switch */
} /* end nco_opr_drv() */

int /* O [enm] Arithmetic operation */
nco_op_typ_get /* [fnc] Convert user-specified operation into operation key */
(const char * const nco_op_sng) /* I [sng] User-specified operation */
{
  /* Purpose: Process '-y' command line argument
     Convert user-specified string to enumerated operation type */
  const char fnc_nm[]="nco_op_typ_get()"; /* [sng] Function name */
  char *prg_nm; /* [sng] Program name */
  int prg_id; /* [enm] Program ID */

  prg_nm=prg_nm_get(); /* [sng] Program name */
  prg_id=prg_get(); /* [enm] Program ID */

  if(nco_op_sng == NULL){
    /* If nco_op_typ_get() is called when user-specified option string is NULL, 
       then operation type may be implied by program name itself */
    if(!strcmp(prg_nm,"ncadd")) return nco_op_add;
    if(!strcmp(prg_nm,"mpncbo")) return nco_op_sbt;
    if(!strcmp(prg_nm,"mpncdiff")) return nco_op_sbt;
    if(!strcmp(prg_nm,"ncbo")) return nco_op_sbt;
    if(!strcmp(prg_nm,"ncdiff")) return nco_op_sbt;
    if(!strcmp(prg_nm,"ncsub")) return nco_op_sbt;
    if(!strcmp(prg_nm,"ncsubtract")) return nco_op_sbt;
    if(!strcmp(prg_nm,"ncmult")) return nco_op_mlt;
    if(!strcmp(prg_nm,"ncmultiply")) return nco_op_mlt;
    if(!strcmp(prg_nm,"ncdivide")) return nco_op_dvd;
    (void)fprintf(stderr,"%s: ERROR %s reports empty user-specified operation string in conjunction with unknown or ambiguous executable name %s\n",prg_nm,fnc_nm,prg_nm);
    nco_exit(EXIT_FAILURE);
  } /* endif */

  if(!strcmp(nco_op_sng,"avg")) return nco_op_avg;
  if(!strcmp(nco_op_sng,"avgsqr")) return nco_op_avgsqr;
  if(!strcmp(nco_op_sng,"max")) return nco_op_max;
  if(!strcmp(nco_op_sng,"min")) return nco_op_min;
  if(!strcmp(nco_op_sng,"rms")) return nco_op_rms;
  if(!strcmp(nco_op_sng,"rmssdn")) return nco_op_rmssdn;
  if(!strcmp(nco_op_sng,"sqravg")) return nco_op_sqravg;
  if(!strcmp(nco_op_sng,"sqrt")) return nco_op_sqrt;
  if(!strcmp(nco_op_sng,"total") || !strcmp(nco_op_sng,"ttl")) return nco_op_ttl;

  if(!strcmp(nco_op_sng,"add") || !strcmp(nco_op_sng,"+") || !strcmp(nco_op_sng,"addition")) return nco_op_add;
  if(!strcmp(nco_op_sng,"sbt") || !strcmp(nco_op_sng,"-") || !strcmp(nco_op_sng,"dff") || !strcmp(nco_op_sng,"diff") || !strcmp(nco_op_sng,"sub") || !strcmp(nco_op_sng,"subtract") || !strcmp(nco_op_sng,"subtraction")) return nco_op_sbt;
  if(!strcmp(nco_op_sng,"dvd") || !strcmp(nco_op_sng,"/") || !strcmp(nco_op_sng,"divide") || !strcmp(nco_op_sng,"division")) return nco_op_dvd;
  if(!strcmp(nco_op_sng,"mlt") || !strcmp(nco_op_sng,"*") || !strcmp(nco_op_sng,"mult") || !strcmp(nco_op_sng,"multiply") || !strcmp(nco_op_sng,"multiplication")) return nco_op_mlt;

  (void)fprintf(stderr,"%s: ERROR %s reports unknown user-specified operation type %s\n",prg_nm,fnc_nm,nco_op_sng);
  (void)fprintf(stderr,"%s: HINT Valid operation type (op_typ) choices:\n",prg_nm);
  if(prg_id == ncbo) (void)fprintf(stderr,"addition: add,+,addition\nsubtration: sbt,-,dff,diff,sub,subtract,subtraction\nmultiplication: mlt,*,mult,multiply,multiplication\ndivision: dvd,/,divide,division\n"); else (void)fprintf(stderr,"min,max,ttl,total,sqrt,sqravg,avgsqr,rms,rmssdn");
  nco_exit(EXIT_FAILURE);
  return False; /* Statement should not be reached */
} /* end nco_op_typ_get() */

int /* O [enm] Relational operation */
nco_op_prs_rlt /* [fnc] Convert Fortran abbreviation for relational operator into NCO operation key */
(const char * const op_sng) /* I [sng] Fortran representation of relational operator */
{
  /* Purpose: Convert Fortran abbreviation for relational operator into NCO operation key */

  /* Classify the relation */
  if(!strcmp(op_sng,"eq")){
    return nco_op_eq;
  }else if(!strcmp(op_sng,"ne")){
    return nco_op_ne;
  }else if(!strcmp(op_sng,"lt")){
    return nco_op_lt;
  }else if(!strcmp(op_sng,"gt")){
    return nco_op_gt;
  }else if(!strcmp(op_sng,"le")){
    return nco_op_le;
  }else if(!strcmp(op_sng,"ge")){
    return nco_op_ge;
  }else{
    (void)fprintf(stdout,"%s: ERROR %s not registered in nco_op_prs_rlt()\n",prg_nm_get(),op_sng);
    nco_exit(EXIT_FAILURE);
  } /* end else */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return False; /* Statement should not be reached */
} /* end nco_op_prs_rlt() */

void
vec_set /* [fnc] Fill every value of first operand with value of second operand */
(const nc_type type, /* I [enm] netCDF type of operand */
 const long sz, /* I [nbr] size (in elements) of operand */
 ptr_unn op1, /* I [sct] Values of first operand */
 const double op2) /* I [frc] Value to fill vector with */
{
  /* Purpose: Fill every value of first operand with value of second operand */
  long idx;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  switch(type){
  case NC_FLOAT:
    for(idx=0;idx<sz;idx++) op1.fp[idx]=op2;
    break;
  case NC_DOUBLE:
    for(idx=0;idx<sz;idx++) op1.dp[idx]=op2;
    break;
  case NC_INT:
    for(idx=0;idx<sz;idx++) op1.ip[idx]=(nco_int)op2; /* Coerce to avoid C++ compiler assignment warning */
    break;
  case NC_SHORT:
    for(idx=0;idx<sz;idx++) op1.sp[idx]=(nco_short)op2; /* Coerce to avoid C++ compiler assignment warning */
    break;
  case NC_USHORT:
    for(idx=0;idx<sz;idx++) op1.usp[idx]=(nco_ushort)op2; /* Coerce to avoid C++ compiler assignment warning */
    break;
  case NC_UINT:
    for(idx=0;idx<sz;idx++) op1.uip[idx]=(nco_uint)op2; /* Coerce to avoid C++ compiler assignment warning */
    break;
  case NC_INT64:
    for(idx=0;idx<sz;idx++) op1.i64p[idx]=(nco_int64)op2; /* Coerce to avoid C++ compiler assignment warning */
    break;
  case NC_UINT64:
    for(idx=0;idx<sz;idx++) op1.ui64p[idx]=(nco_uint64)op2; /* Coerce to avoid C++ compiler assignment warning */
    break;
  case NC_BYTE: break; /* Do nothing */
  case NC_UBYTE: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
    default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end vec_set() */

void
nco_zero_long /* [fnc] Zero all values of long array */
(const long sz, /* I [nbr] Size (in elements) of operand */
 long * restrict const op1) /* I/O [nbr] Array to be zeroed */
{
  /* Purpose: Zero all values of long array */

  size_t sz_byt; /* [B] Number of bytes in variable buffer */
  sz_byt=(size_t)sz*sizeof(long);
  (void)memset((void *)op1,(int)0,sz_byt);

#if 0
  /* Presumably this old method used until 20050321 is slower because of pointer de-referencing */
  long idx;
  if(op1 == NULL){
    (void)fprintf(stdout,"%s: ERROR nco_zero_long() asked to zero NULL pointer\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* endif */
  for(idx=0;idx<sz;idx++) op1[idx]=0L;
#endif /* !0 */

} /* end nco_zero_long() */

