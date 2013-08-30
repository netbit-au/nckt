/* $Header: /cvsroot/nco/nco/src/nco/nco.h,v 1.150 2010/03/15 18:02:58 zender Exp $ */

/* Purpose: netCDF Operator (NCO) definitions */

/* Copyright (C) 1995--2010 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco.h" *//* netCDF Operator (NCO) definitions */

#ifndef NCO_H /* Contents have not yet been inserted in current source file */
#define NCO_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */

/* Personal headers */
#include "nco_typ.h" /* Type definitions, opaque types */

  /* C pre-processor macros for instantiating variable values with string tokens
     Macros for token pasting described at http://www.parashift.com/c++-faq-lite
     Layer of indirection is required, use public macro to call private macro */
#define TKN2SNG_PRV(x) #x
#define TKN2SNG(x) TKN2SNG_PRV(x)

/* Encapsulate C++ const usage in C99-safe macro 
   C++ compilers will use type-safe version
   C89 and C99 compilers use less type-safe version that is standards-compliant */
#ifdef __cplusplus
# define CST_X_PTR_CST_PTR_CST_Y(x,y) const x * const * const y
# define X_CST_PTR_CST_PTR_Y(x,y) x const * const * y
#else /* !__cplusplus */
# define CST_X_PTR_CST_PTR_CST_Y(x,y) x * const * const y
# define X_CST_PTR_CST_PTR_Y(x,y) x * const * y
#endif /* !__cplusplus */

#ifdef __cplusplus
/* Use C-bindings so C++-compiled and C-compiled libraries are compatible */
extern "C" {
#endif /* !__cplusplus */

/* Uncomment next line to remove C99 restrict type-qualifier from all source */
/* #define restrict */

/* Replace restrict by __restrict in g++ compiler
   Substitute whitespace for __restrict in all other C++ compilers */
#ifdef __cplusplus
# ifdef __GNUG__
#  define restrict __restrict
# else /* !__GNUG__ */
#  define restrict
# endif /* !__GNUG__ */
#endif /* !__cplusplus */

/* Boolean values */
/* From Wikipedia:
   "On a recent C compiler (supporting the C99 standard), there is a _Bool type, which is used to define bool by the stdbool.h header: 

   #include <stdbool.h>
   bool b = false;
   ...
   b = true;

   During its standardization process, the C++ programming language introduced the bool, true and false keywords, adding a native datatype to support boolean data.
   Preprocessor macros may be used to turn bool into _Bool, false into 0 and true into 1, allowing compatibility with the aforementioned C99 use of the stdbool.h header." */
#define nco_bool int
#ifndef __cplusplus
# ifndef bool
#  define bool int
# endif /* bool */
#endif /* __cplusplus */
#ifndef True
# define True 1
#endif /* True */
#ifndef False
# define False 0
#endif /* False */
  
  /* Variables marked CEWI "Compiler Error Warning Initialization" are initialized
     to prevent spurious "warning: `float foo' might be used uninitialized in 
     this function" warnings when, e.g., GCC -Wuninitialized is turned on.
     Note that these warning messages are compiler- and OS-dependent
     GCC warning on Alpha, e.g., cannot be removed by this trick */
#define NULL_CEWI NULL
#define char_CEWI '\0'
#define double_CEWI 0.0
#define float_CEWI 0.0
#define int_CEWI 0
#define long_CEWI 0L
#define long_long_CEWI ((long long)0LL)
#define nco_bool_CEWI 0
#define nco_byte_CEWI ((signed char)0)
#define nco_char_CEWI '\0'
#define nco_int_CEWI (0L)
#define nco_short_CEWI ((short)0)
#define nco_ubyte_CEWI ((unsigned char)0)
#define nco_ushort_CEWI ((unsigned short)0)
#define nco_uint_CEWI (0U)
#define nco_int64_CEWI ((long long)0LL)
#define nco_uint64_CEWI ((unsigned long long)0ULL)
#define nco_string_CEWI NULL
#define short_CEWI 0
#define size_t_CEWI 0UL
  
  /* netcdf.h NC_GLOBAL is, strictly, the variable ID for global attributes
     NCO_REC_DMN_UNDEFINED is dimension ID of record dimension iff record dimension is undefined
     Normally using -1 for this ID is fine, but token makes meaning clearer
     NB: nc_inq() family returns -1 for missing record dimensions */
#define NCO_REC_DMN_UNDEFINED -1

  /* netcdf.h defines four NC_FORMAT tokens: NC_FORMAT_CLASSIC, ...
     The values are (currently) enumerated from one to four
     Operators need to check if fl_out_fmt has been user-specified
     Saftest way is to compare current value of fl_out_fmt to initial value 
     Initial value should be a number that will never be a true netCDF format */
#define NCO_FORMAT_UNDEFINED 0
  
  /* Debugging level that quiets all non-requested informational messages
     This value is compared against user-selected dbg_lvl 
     Running operators with --quiet automatically sets dbg_lvl=NCO_DBG_QUIET */
#define NCO_DBG_QUIET 0

  /* NCO_MSS_VAL_SNG names attribute whose value is "skipped" by arithmetic
     Must be either "missing_value" or "_FillValue" */
#ifndef NCO_MSS_VAL_SNG
  /* 20070831: TKN2SNG technique inserts quotes into string though same test code in c.c does not produce extra quotes. TODO nco905. */
  /*# define NCO_MSS_VAL_SNG missing_value */
  /*# define NCO_MSS_VAL_SNG _FillValue*/
  /*char nco_mss_val_sng[]=TKN2SNG(NCO_MSS_VAL_SNG);*/ /* [sng] Missing value attribute name */
# define NCO_USE_FILL_VALUE
# ifdef NCO_USE_FILL_VALUE
  /* NCO_NETCDF4_AND_FILLVALUE tells whether netCDF4 restrictions on 
     _FillValue operations (must be defined before variable written,
     cannot be changed after variable written) can affect output file */ 
#  ifdef ENABLE_NETCDF4
#   define NCO_NETCDF4_AND_FILLVALUE
#  endif /* !ENABLE_NETCDF4 */
# endif /* !NCO_USE_FILL_VALUE */
# define NCO_MSS_VAL_SNG
#endif /* NCO_MSS_VAL_SNG */

  /* Prototype global functions before defining them in next block */
  char *nco_mss_val_sng_get(void); /* [sng] Missing value attribute name */
  char *nco_not_mss_val_sng_get(void); /* [sng] Not missing value attribute name */
  char *prg_nm_get(void);
  int prg_get(void);
  unsigned short dbg_lvl_get(void);

#ifdef MAIN_PROGRAM_FILE /* Current file contains main() */
  
  /* Tokens and variables with scope limited to main.c, and global variables allocated here */
  
  int prg; /* [enm] Program ID */
  int prg_get(void){return prg;} /* [enm] Program ID */
  
  char *prg_nm; /* [sng] Program name */
  char *prg_nm_get(void){return prg_nm;} /* [sng] Program name */
  
  unsigned short dbg_lvl=0; /* [enm] Debugging level */
  unsigned short dbg_lvl_get(void){return dbg_lvl;} /* [enm] Debugging level */

# ifdef NCO_USE_FILL_VALUE
  char nco_mss_val_sng[]="_FillValue"; /* [sng] Missing value attribute name */
  char nco_not_mss_val_sng[]="missing_value"; /* [sng] Not missing value attribute name */
# else /* !NCO_USE_FILL_VALUE */
  char nco_mss_val_sng[]="missing_value"; /* [sng] Missing value attribute name */
  char nco_not_mss_val_sng[]="_FillValue"; /* [sng] Not missing value attribute name */
# endif /* !NCO_USE_FILL_VALUE */
  char *nco_mss_val_sng_get(void){return nco_mss_val_sng;} /* [sng] Missing value attribute name */
  char *nco_not_mss_val_sng_get(void){return nco_not_mss_val_sng;} /* [sng] Not missing value attribute name */

#else /* MAIN_PROGRAM_FILE is NOT defined, i.e., current file does not contain main() */
  
  /* External references to global variables are declared as extern here
     Variables with local file scope in all files except main.c are allocated here */
  
#endif /* MAIN_PROGRAM_FILE is NOT defined, i.e., the current file does not contain main() */

/* Compatibility tokens for when NCO compiled with older netcdf.h
   It is hard to track where/when many tokens defined
   Easiest to individually check for pre-definition of each */
/* Three compatibility tokens new to netCDF4 netcdf.h: */
#ifndef NC_NETCDF4
# define NC_NETCDF4     (0x1000) /* Use netCDF-4/HDF5 format */
#endif
#ifndef NC_CHUNKED
# define NC_CHUNKED (0)
#endif
#ifndef NC_CONTIGUOUS
# define NC_CONTIGUOUS (1)
#endif
#ifndef NC_NOCHECKSUM
# define NC_NOCHECKSUM 0
#endif
#ifndef NC_FLETCHER32
# define NC_FLETCHER32 1
#endif

/* Six compatibility tokens not all available until netCDF 3.6.1 netcdf.h
   NC_64BIT_OFFSET is used (so far) only in nco_fl_utl.c */
#ifndef NC_64BIT_OFFSET
# define NC_64BIT_OFFSET (0x0200) /* Use large (64-bit) file offsets */
#endif
#ifndef NC_CLASSIC_MODEL
# define NC_CLASSIC_MODEL (0x0008) /* Enforce strict netcdf-3 rules. */
#endif
#ifndef NC_FORMAT_CLASSIC
# define NC_FORMAT_CLASSIC (1)
#endif
#ifndef NC_FORMAT_64BIT
# define NC_FORMAT_64BIT   (2)
#endif
#ifndef NC_FORMAT_NETCDF4
# define NC_FORMAT_NETCDF4 (3)
#endif
#ifndef NC_FORMAT_NETCDF4_CLASSIC
# define NC_FORMAT_NETCDF4_CLASSIC  (4) /* create netcdf-4 files, with NC_CLASSIC_MODEL. */
#endif
/* Endif older netcdf.h */

  /* Define compatibility tokens when user does not have znetCDF */
#ifndef ENABLE_ZNETCDF
# ifndef NC_COMPRESS
#  define NC_COMPRESS 0x200 /* bn file is compressed */
# endif
#endif /* !ENABLE_ZNETCDF */

#ifndef EXIT_SUCCESS /* Most likely this is a SUN4 machine */
# define EXIT_SUCCESS 0
#endif /* SUN4 */
#ifndef EXIT_FAILURE /* Most likely this is a SUN4 machine */
# define EXIT_FAILURE 1
#endif /* SUN4 */

  enum prg{ /* [enm] Key value for all netCDF operators */
    ncap,
    ncatted,
    ncbo,
    ncea,
    ncecat,
    ncflint,
    ncks,
    ncpdq,
    ncra,
    ncrcat,
    ncrename,
    ncwa
  }; /* end prg enum */
  
  enum aed{ /* [enm] Attribute editor mode */
    aed_append,
    aed_create,
    aed_delete,
    aed_modify,
    aed_overwrite
  }; /* end aed enum */
  
  enum nco_dbg_typ_enm{ /* [enm] Debugging levels */
    /* List in increasing levels of verbosity */
    nco_dbg_quiet, /* 0 [enm] Quiet all non-error messages */
    nco_dbg_std, /* 1 [enm] Standard mode. Minimal, but some, messages */
    nco_dbg_fl, /* 2 [enm] Filenames */
    nco_dbg_scl, /* 3 [enm] Scalars, other per-file information  */
    nco_dbg_var, /* 4 [enm] Variables, highest level per-file loop information */
    nco_dbg_crr, /* 5 [enm] Current task */
    nco_dbg_sbr, /* 6 [enm] Subroutine names on entry and exit */
    nco_dbg_io, /* 7 [enm] Subroutine I/O */
    nco_dbg_vec, /* 8 [enm] Entire vectors */
    nco_dbg_vrb, /* 9 [enm] Verbose, print everything possible */
    nco_dbg_old, /* 10 [enm] Old debugging blocks not used anymore */
    nco_dbg_nbr /* 10 [enm] Number of debugging types (equals last enumerated value) */
  }; /* end nco_dbg_typ_enm */

  typedef enum { /* [enm] Memory allocation type */
    nco_mmr_calloc, /* [enm] nco_calloc() */
    nco_mmr_free, /* [enm] nco_free() */
    nco_mmr_malloc, /* [enm] nco_malloc() */
    nco_mmr_realloc /* [enm] nco_realloc() */
  } nco_mmr_typ_enm; /* end nco_mmr_typ enum */

  typedef enum { /* [enm] Arithmetic precision rank */
    /* Ranked ordering of "arithmetic precision" from lowest to highest */
    nco_rth_prc_rnk_string, /* [enm] */ /* Least precise */
    nco_rth_prc_rnk_char, /* [enm] */
    nco_rth_prc_rnk_ubyte, /* [enm] */
    nco_rth_prc_rnk_byte, /* [enm] */
    nco_rth_prc_rnk_ushort, /* [enm] */
    nco_rth_prc_rnk_short, /* [enm] */
    nco_rth_prc_rnk_uint, /* [enm] */
    nco_rth_prc_rnk_int, /* [enm] */
    nco_rth_prc_rnk_uint64, /* [enm] */
    nco_rth_prc_rnk_int64, /* [enm] */
    nco_rth_prc_rnk_float, /* [enm] */
    nco_rth_prc_rnk_double /* [enm] */ /* Most precise */
  } nco_rth_prc_rnk_enm; /* end nco_rth_prc_rnk enum */

  enum nco_op_typ{ /* [enm] Operation type */
    /* Types used in ncbo(): */
    nco_op_add, /* [enm] Add file_1 to file_2 */
    nco_op_dvd, /* [enm] Divide file_1 by file_2 */
    nco_op_mlt, /* [enm] Multiply file_1 by file_2 */
    nco_op_sbt, /* [enm] Subtract file_2 from file_1 */
    /* Types used in ncra(), ncrcat(), ncwa(): */
    nco_op_avg, /* [enm] Average */
    nco_op_min, /* [enm] Minimum value */
    nco_op_max, /* [enm] Maximum value */
    nco_op_ttl, /* [enm] Linear sum */
    nco_op_sqravg, /* [enm] Square of mean */
    nco_op_avgsqr, /* [enm] Mean of sum of squares */
    nco_op_sqrt, /* [enm] Square root of mean */
    nco_op_rms, /* [enm] Root-mean-square (normalized by N) */
    nco_op_rmssdn, /* [enm] Root-mean square normalized by N-1 */
    nco_op_nil /* [enm] Nil or undefined operation type  */
  }; /* end nco_op_typ enum */

  enum nco_rlt_opr{ /* [enm] Arithmetic relations (comparisons) for masking */
    nco_op_eq, /* Equality */
    nco_op_ne, /* Inequality */
    nco_op_lt, /* Less than */
    nco_op_gt, /* Greater than */
    nco_op_le, /* Less than or equal to */
    nco_op_ge /* Greater than or equal to */
  }; /* end nco_rlt_opr enum */
  
  typedef enum { /* [enm] Timer flag */
    nco_tmr_srt, /* [enm] Initialize timer (first timer call) */
    nco_tmr_mtd, /* [enm] Metadata timer (second timer call) */
    nco_tmr_rgl, /* [enm] Regular timer call (main loop timer call) */
    nco_tmr_end /* [enm] Close timer (last timer call) */
  } nco_tmr_flg; /* [enm] Timer flag */

  typedef enum { /* [enm] Node enumerator Nie02 nodeEnum */
    typ_scv, /* [enm] Scalar value */
    typ_sym, /* [enm] Symbol identifier */
    typ_opr /* [enm] Operator */
  } nod_typ_enm; /* end Node enumerator */
  /* end enumeration section */

  /* Limit structure */
  typedef struct { /* lmt_sct */
    char *nm; /* [sng] Dimension name */
    int lmt_typ; /* crd_val or dmn_idx */
    /* Following four flags are used only by multi-file operators ncra and ncrcat: */
    nco_bool is_usr_spc_lmt; /* True if any part of limit is user-specified, else False */
    nco_bool is_usr_spc_min; /* True if user-specified, else False */
    nco_bool is_usr_spc_max; /* True if user-specified, else False */
    nco_bool is_rec_dmn; /* True if record dimension, else False */
    long rec_skp_vld_prv; /* Records skipped since prevous good one (multi-file record dimension only) */
    long rec_skp_nsh_spf; /* Sum of records in already processed files (multi-file record dimension only) */
    char *min_sng; /* User-specified string for dimension minimum */
    char *max_sng; /* User-specified string for dimension maximum */
    char *srd_sng; /* User-specified string for dimension stride */
    int id; /* Dimension ID */
    long min_idx; /* Index of minimum requested value in dimension */
    long max_idx; /* Index of maximum requested value in dimension */
    double min_val; /* Double precision representation of minimum value of coordinate requested or implied */
    double max_val; /* Double precision representation of maximum value of coordinate requested or implied */
    long srt; /* Index to start of hyperslab */
    long end; /* Index to end of hyperslab */
    long cnt; /* # of valid elements in this dimension (including effects of stride and wrapping) */
    long srd; /* Stride of hyperslab */
    double origin;   /* used for ncra,ncrcat to time rebase co-ordinate var */
    char *re_bs_sng; /* used for ncra,ncrcat to time rebase co-ordinate -holds unit attribue from first file */
    int lmt_cal; /* used for ncra,ncrcat to store an enum of the calendar type attribute */
  } lmt_sct;

  /* Container holding all limit structures indexible by dimension */
  typedef struct { /* lmt_all_sct */
    char *dmn_nm; /* [sng] Dimension name */
    long dmn_cnt; /* [nbr] Total number of hyperslabs to extract */
    long dmn_sz_org; /* [nbr] Size of dimension in INPUT file */
    int lmt_dmn_nbr; /* [nbr] Number of lmt args */
    nco_bool BASIC_DMN; /* [flg] Limit is same as dimension in input file */
    nco_bool WRP; /* [flg] Limit is wrapped (true iff wrapping, lmt_dmn_nbr==2) */ 
    nco_bool MSA_USR_RDR; /* [flg] Multi-slabbing algorithm leaves hyperslabs in user order */
    lmt_sct **lmt_dmn; /* [sct] List of limit structures associated with each dimension */  
  } lmt_all_sct;
  
  /* Name ID structure */
  typedef struct{ /* nm_id_sct */
    char *nm;
    int id;
  } nm_id_sct;
  
  /* Rename structure */
  typedef struct{ /* rnm_sct */
    char *old_nm;
    char *new_nm;
    int id;
  } rnm_sct;

  /* DDRA information structure */
  typedef struct{ /* ddra_info_sct */
    int nco_op_typ; /* [enm] Operation type */
    int rnk_avg; /* [nbr] Rank of averaging space */
    int rnk_var; /* [nbr] Variable rank (in input file) */
    int rnk_wgt; /* [nbr] Rank of weight */
    int var_idx; /* [enm] Index */
    int wrd_sz; /* [B] Bytes per element */
    long long lmn_nbr; /* [nbr] Variable size */
    long long lmn_nbr_avg; /* [nbr] Averaging block size */
    long long lmn_nbr_wgt; /* [nbr] Weight size */
    nco_bool flg_ddra; /* [flg] Run DDRA diagnostics */
    nco_bool MRV_flg; /* [flg] Avergaging dimensions are MRV dimensions */
    nco_bool wgt_brd_flg; /* [flg] Broadcast weight for this variable */
    nco_tmr_flg tmr_flg; /* [enm] Timer flag */
  } ddra_info_sct;
  
  /* Pointer union */
  typedef union{ /* ptr_unn */
    float * restrict fp;
    double * restrict dp;
    nco_int * restrict ip;
    short * restrict sp;
    nco_char * restrict cp;
    nco_byte * restrict bp;
    nco_ubyte * restrict ubp;
    nco_ushort * restrict usp;
    nco_uint * restrict uip;
    nco_int64 * restrict i64p;
    nco_uint64 * restrict ui64p;
    nco_string * restrict sngp;
    void * restrict vp;
  } ptr_unn;

  /* Value union */
  typedef union{ /* val_unn */
    float f;
    double d;
    nco_int i;
    short s;
    nco_char c;
    nco_byte b;
    nco_ubyte ub;
    nco_ushort us;
    nco_uint ui;
    nco_int64 i64;
    nco_uint64 ui64;
    nco_string sng;
  } val_unn;

  /* Scalar value structure */
  typedef struct{ /* scv_sct */
    val_unn val; /* [sct] Value */
    nc_type type; /* [enm] netCDF type */
    nod_typ_enm nod_typ; /* [enm] Node type */
  } scv_sct;      

  /* Attribute editing structure */
  typedef struct{ /* aed_sct */
    char *att_nm; /* Name of attribute */
    char *var_nm; /* Name of variable, or NULL for global attribute */
    int id; /* Variable ID or NC_GLOBAL ( = -1) for global attribute */
    long sz; /* Number of elements in attribute */
    nc_type type; /* Type of attribute */
    ptr_unn val; /* Pointer to attribute value */
    short mode; /* Action to perform with attribute */
  } aed_sct;
  
  /* Attribute structure */
  typedef struct{ /* att_sct */
    char *nm;
    nc_type type;
    long sz;
    char fmt[5];
    ptr_unn val;
  } att_sct;
  
  /* Chunking structure */
  typedef struct{ /* cnk_sct */
    char *nm; /* [sng] Dimension name */
    size_t sz; /* [nbr] Chunk size */
    nco_bool is_usr_spc_cnk; /* [flg] Chunk size was user-specified */
  } cnk_sct;
  
  /* Fill actual value of dmn_sct structure in in nco_dmn_fll()
     free() each pointer member of dmn_sct structure in nco_dmn_free()
     deep-copy each pointer member of dmn_sct structure in nco_dmn_dpl() */
  /* Dimension structure */
  typedef struct dmn_sct_tag{ /* dmn_sct */
    char *nm; /* [sng] Dimension name */
    int id; /* [id] Dimension ID */
    int nc_id; /* [id] File ID */
    long sz; /* [nbr] Full size of dimension in file (NOT the hyperslabbed size) */
    short is_rec_dmn; /* [flg] Is this the record dimension? */
    short is_crd_dmn; /* [flg] Is this a coordinate dimension? */
    int cid; /* [id] Variable ID of associated coordinate, if any */
    nc_type type; /* [enm] Type of coordinate, if applicable */
    char fmt[5]; /* [sng] Hint for printf()-style formatting */
    long srt; /* [idx] Index to start of hyperslab */
    long end; /* [idx] Index to end of hyperslab */
    long cnt; /* [nbr] Number of valid elements in this dimension (including effects of stride and wrapping) */
    long srd; /* [nbr] Stride of hyperslab */
    size_t cnk_sz; /* [nbr] Chunk size */
    ptr_unn val; /* [sct] Buffer to hold hyperslab fxm: is this ever used? */
    struct dmn_sct_tag *xrf; /* [sct] Cross-reference to associated dimension structure (usually the structure for dimension on output) */
  } dmn_sct; /* end dmn_sct_tag */
  
  /* Initialize default value of each member of var_sct structure in var_dfl_set()
     Fill actual value of var_sct structure in nco_var_fll()
     free() each pointer member of var_sct structure in nco_var_free()
     deep-copy each pointer member of var_sct structure in nco_var_dpl() */
  /* Variable structure */
  typedef struct var_sct_tag{ /* var_sct */
    char *nm; /* [sng] Variable name */
    int id; /* [id] Variable ID */
    int nc_id; /* [id] File ID */
    int nbr_dim; /* [nbr] Number of dimensions of variable in input file */
    nc_type type; /* [enm] Type of variable in RAM */
    nc_type typ_dsk; /* [enm] Type of variable on disk (never changes) */
    short is_rec_var; /* [flg] Is this a record variable? */
    short is_crd_var; /* [flg] Is this a coordinate variable? */
    long sz; /* [nbr] Number of elements (NOT bytes) in hyperslab (NOT full size of variable in input file!) */
    long sz_rec; /* [nbr] Number of elements in one record of hyperslab */
    int nbr_att; /* [nbr] Number of attributes */
    int has_dpl_dmn; /* [flg] Variable has duplicate copies of same dimension */
    int has_mss_val; /* [flg] Is there a missing_value attribute? */
    ptr_unn mss_val; /* [frc] Value of missing_value attribute, if any (mss_val stored in this structure must be same type as variable) */
    int cid; /* [id] Dimension ID of associated coordinate, if any */
    char fmt[5]; /* [sng] Hint for printf()-style formatting */
    dmn_sct **dim; /* [sct] Pointers to full dimension structures */
    int *dmn_id; /* [id] Contiguous vector of dimension IDs */
    size_t *cnk_sz; /* [id] Contiguous vector of chunk sizes */
    long *srt; /* [id] Contiguous vector of indices to start of hyperslab */
    long *end; /* [id] Contiguous vector of indices to end of hyperslab */
    long *cnt; /* [id] Contiguous vector of lengths of hyperslab */
    long *srd; /* [id] Contiguous vector of stride of hyperslab */
    ptr_unn val; /* [bfr] Buffer to hold hyperslab */
    long *tally; /* [nbr] Number of valid operations performed so far */
    struct var_sct_tag *xrf; /* [sct] Cross-reference to associated variable structure (usually structure for variable on output) fxm: deprecate! TODO nco226 */
    int pck_dsk; /* [flg] Variable is packed on disk (valid scale_factor, add_offset, or both attributes exist) */
    int pck_ram; /* [flg] Variable is packed in memory (valid scale_factor, add_offset, or both attributes exist) */
    int has_scl_fct; /* [flg] Valid scale_factor attribute exists */
    int has_add_fst; /* [flg] Valid add_offset attribute exists */
    ptr_unn scl_fct; /* [frc] Value of scale_factor attribute of type typ_upk */
    ptr_unn add_fst; /* [frc] Value of add_offset attribute of type typ_upk */
    nc_type typ_pck; /* [enm] Type of variable when packed (on disk). typ_pck = typ_dsk except in cases where variable is packed in input file and unpacked in output file. */
    nc_type typ_upk; /* [enm] Type of variable when unpacked (expanded) (in memory) */
    int undefined; /* [flg] Variable is still undefined (in first parser pass) */
    int is_fix_var; /* [flg] Is this a fixed (non-processed) variable? */
  } var_sct; /* end var_sct_tag */
  
#ifdef __cplusplus
} /* end extern "C" */
#endif /* !__cplusplus */

#endif /* NCO_H */
