/* $Header: /cvsroot/nco/nco/src/nco/nco_var_scv.h,v 1.23 2010/01/05 20:02:18 zender Exp $ */

/* Purpose: Arithmetic between variables and scalar values */

/* Copyright (C) 1995--2010 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_var_scv.h" *//* Arithmetic between variables and scalar values */

#ifndef NCO_VAR_SCV_H
#define NCO_VAR_SCV_H

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, printf */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_rth_flt.h" /* Float-precision arithmetic */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  void
  var_scv_add /* [fnc] Add scalar to variable */
  (const nc_type type, /* I [enm] netCDF type of operands */
   const long sz,/* I [nbr] Size (in elements) of array operand */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   ptr_unn op1, /* I/O [val] Values of first operand */
   scv_sct *scv); /* I [val] Pointer to scalar value (second operand) */
  
  void
  var_scv_mlt /* [fnc] Multiply variable by scalar */
  (const nc_type type, /* I [enm] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of array operand */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   ptr_unn op1, /* I/O [val] Values of first operand */
   scv_sct *scv); /* I [val] Pointer to scalar value (second operand) */
  
  void
  var_scv_mod /* [fnc] Modulo variable by scalar */
  (const nc_type type, /* I [enm] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of array operands */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   ptr_unn op1, /* I/O [val] Values of first operand */
   scv_sct *scv); /* I [val] Pointer to scalar value (second operand) */
  
  void
  scv_var_mod /* [fnc] Modulo scalar by variable */
  (const nc_type type, /* I [enm] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of array operands */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   scv_sct *scv, /* I [val] Pointer to scalar value (first operand) */
   ptr_unn op2); /* I/O [val] Values of second operand */
  
  void
  var_scv_dvd /* [fnc] Divide variable by scalar */
  (const nc_type type, /* I [enm] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of array operands */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [flg] Value of missing value */
   ptr_unn op1, /* I/O [val] Values of first operand */
   scv_sct *scv); /* I [val] Pointer to scalar value (second operand) */
  
  void
  scv_var_dvd /* [fnc] Divide scalar by variable */
  (const nc_type type, /* I [enm] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of array operands */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [flg] Value of missing value */
   scv_sct *scv, /* I [val] Pointer to scalar value (first operand) */
   ptr_unn op2); /* I/O [val] Values of second operand */
  
  void
  var_scv_pwr /* [fnc] Empower variable by scalar */
  (const nc_type type, /* I [enm] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of array operands */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [flg] Value of missing value */
   ptr_unn op1, /* I/O [val] Values of first operand */
   scv_sct *scv); /* I [val] Pointer to scalar value (second operand) */
  
  void
  scv_var_pwr /* [fnc] Empower scalar by variable */
  (const nc_type type, /* I [enm] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of array operands */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [flg] Value of missing value */
   scv_sct *scv, /* I [val] Pointer to scalar value (first operand) */
   ptr_unn op2); /* I/O [val] Values of second operand */
  
  void
  var_scv_sub /* [fnc] Subtract scalar from variable */
  (const nc_type type, /* I [enm] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of array operand */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   ptr_unn op1, /* I/O [val] Values of first operand */
   scv_sct *scv); /* I [val] Pointer to scalar value (second operand) */
  
#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_VAR_SCV_H */
