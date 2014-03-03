/* $Header: /cvsroot/nco/nco/src/nco/nco_sql.c,v 1.78 2010/03/12 07:08:15 zender Exp $ */

/* Purpose: Multi-slabbing algorithm */

/* Copyright (C) 1995--2010 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_sql.h" /* Multi-slabbing algorithm */
#include "nco_var_utl.h" /* Variable utilities */
#include <libpq-fe.h>

/* fxm: strings statically allocated with NCR_MAX_LEN_FMT_SNG chars may be susceptible to buffer overflow attacks */
/* Length should be computed at run time but doing so would be painful */
#define NCO_MAX_LEN_FMT_SNG 200

/* [fnc] Multi-slab algorithm (recursive routine, returns a single slab pointer */
/* [nbr] Current depth, we start at 0 */
/* [nbr] Maximium depth (i.e., number of dimensions in variable (does not change) */
/* [sct] Limits of current hyperslabs (these change as we recurse) */
/* [sct] List of limits in each dimension (this remains STATIC as we recurse) */
/* [sct] Information for routine to read variable information and pass information between calls */
void * nco_sql_rec_clc (int dpt_crr, int dpt_crr_max, lmt_sct **lmt, lmt_all_sct **lmt_lst, var_sct *vara) 
{
  /* NB: nco_sql_rec_clc() with same nc_id contains OpenMP critical region */
  /* Purpose: Multi-slab algorithm (recursive routine, returns a single slab pointer */
  int idx;
  int nbr_slb;
  void *vp;

  if(dpt_crr == dpt_crr_max) goto read_lbl;

  nbr_slb=lmt_lst[dpt_crr]->lmt_dmn_nbr;

  if(nbr_slb == 1){
    lmt[dpt_crr]=lmt_lst[dpt_crr]->lmt_dmn[0];
    /* NB: nco_sql_rec_clc() with same nc_id contains OpenMP critical region */
    vp=nco_sql_rec_clc(dpt_crr+1,dpt_crr_max,lmt,lmt_lst,vara);
    return vp;
  } /* end if */

  /* Here we deal with multiple hyperslabs */
  if(nbr_slb > 1){
    int slb_idx;
    long var_sz=1L;
    long lcnt;
    long *cp_sz;
    long *indices;

    ptrdiff_t slb_sz;
    ptrdiff_t slb_stp;
    ptrdiff_t cp_inc;
    ptrdiff_t cp_max;
    ptrdiff_t cp_fst;

    char **cp_wrp;
    char *cp_stp;
    char *slb;
    lmt_sct lmt_ret;

    cp_sz=(long *)nco_malloc(nbr_slb*sizeof(long));
    indices=(long *)nco_malloc(nbr_slb*sizeof(long));
    cp_wrp=(char **)nco_malloc(nbr_slb*sizeof(void *));

    for(idx=0;idx<nbr_slb;idx++){
      lmt[dpt_crr]=lmt_lst[dpt_crr]->lmt_dmn[idx];
      /* NB: nco_sql_rec_clc() with same nc_id contains OpenMP critical region */
      cp_wrp[idx]=(char *)nco_sql_rec_clc(dpt_crr+1,dpt_crr_max,lmt,lmt_lst,vara);
      cp_sz[idx]=vara->sz;
    } /* end loop over idx */

    for(idx=0;idx<dpt_crr_max;idx++)
      var_sz*=(idx<dpt_crr ? lmt[idx]->cnt : lmt_lst[idx]->dmn_cnt);

    vp=(void *)nco_calloc((size_t)var_sz,nco_typ_lng(vara->type));

    lcnt=nco_typ_lng(vara->type);
    for(idx=dpt_crr+1;idx<dpt_crr_max;idx++)
      lcnt*=lmt_lst[idx]->dmn_cnt;

    cp_inc=(ptrdiff_t)(lcnt*lmt_lst[dpt_crr]->dmn_cnt);

    cp_max=(ptrdiff_t)(var_sz*nco_typ_lng(vara->type));

    for(idx=0;idx<nbr_slb;idx++) indices[idx]=lmt_lst[dpt_crr]->lmt_dmn[idx]->srt;

    cp_fst=0L;

    /* Deal first with wrapped dimensions
       True if wrapped dims or slabs DONT Overlap or user specified order */
    if(lmt_lst[dpt_crr]->WRP || lmt_lst[dpt_crr]->MSA_USR_RDR){

      for(slb_idx=0;slb_idx<nbr_slb;slb_idx++){
      	cp_stp=(char *)vp+cp_fst;
      	slb=cp_wrp[slb_idx];
      	slb_sz=(ptrdiff_t)(lcnt*(lmt_lst[dpt_crr]->lmt_dmn[slb_idx]->cnt));
        while(cp_stp-(char *)vp < cp_max){
      	  (void)memcpy(cp_stp,slb,(size_t)slb_sz);
      	  slb+=slb_sz;
      	  cp_stp+=cp_inc;
        } /* end while */
        cp_fst+=slb_sz;
      } /* end loop over two slabs */
    } else {
      /* Multiple hyper-slabs */
      while(nco_msa_clc_idx(True,lmt_lst[dpt_crr],&indices[0],&lmt_ret,&slb_idx)){
      	cp_stp=(char *)vp+cp_fst;
      	slb=cp_wrp[slb_idx]+(ptrdiff_t)(lmt_ret.srt*lcnt);
      	slb_stp=(ptrdiff_t)(lcnt*(lmt_lst[dpt_crr]->lmt_dmn[slb_idx]->cnt));
      	slb_sz=(ptrdiff_t)(lmt_ret.cnt*lcnt);

        while(cp_stp-(char *)vp < cp_max){
          (void)memcpy(cp_stp,slb,(size_t)slb_sz);
          slb+=slb_stp;
	        cp_stp+=cp_inc;
	      } /* end while */
	      
        cp_fst+=slb_sz;
      } /* end while */
    } /* end else */

    indices=(long *)nco_free(indices);
    cp_sz=(long *)nco_free(cp_sz);
    for(idx=0;idx<nbr_slb;idx++) cp_wrp[idx]=(char *)nco_free(cp_wrp[idx]);
    cp_wrp=(char **)nco_free(cp_wrp);

    vara->sz=var_sz;
    return vp;
  } /* endif multiple hyperslabs */

  read_lbl:
  {
    long var_sz=1L;
    long mult_srd=1L;
    long *dmn_srt;
    long *dmn_cnt;
    long *dmn_srd;

    dmn_srt=(long *)nco_malloc(dpt_crr_max*sizeof(long));
    dmn_cnt=(long *)nco_malloc(dpt_crr_max*sizeof(long));
    dmn_srd=(long *)nco_malloc(dpt_crr_max*sizeof(long));

    for(idx=0;idx<dpt_crr_max;idx++){
      dmn_srt[idx]=lmt[idx]->srt;
      dmn_cnt[idx]=lmt[idx]->cnt;
      dmn_srd[idx]=lmt[idx]->srd;
      var_sz*=dmn_cnt[idx];
      mult_srd*=lmt[idx]->srd;
    } /* end loop over idx */

    vp=(void *)nco_malloc(var_sz*nco_typ_lng(vara->type));

    /* Block is critical/thread-safe for identical/distinct in_id's */
    { /* begin potential OpenMP critical */
      /* Check for stride */
      if(var_sz > 0){ /* Allow for zero-size record variables TODO nco711 */
        if(mult_srd == 1L)
        (void)nco_get_vara(vara->nc_id,vara->id,dmn_srt,dmn_cnt,vp,vara->type);
	      else
        (void)nco_get_varm(vara->nc_id,vara->id,dmn_srt,dmn_cnt,dmn_srd,(long *)NULL,vp,vara->type);
      } /* end if var_sz */
    } /* end potential OpenMP critical */

    dmn_srt=(long *)nco_free(dmn_srt);
    dmn_cnt=(long *)nco_free(dmn_cnt);
    dmn_srd=(long *)nco_free(dmn_srd);

    /* Put size into vara */
    vara->sz=var_sz;
    return vp;
  }/* end read_lbl */
} /* end nco_sql_rec_clc() */

/* [fnc] PG Insert variable data */
/* PQ connection details */
/* in_id I [id] netCDF input file ID */    
/* I [sng] Variable name */    
/* I [sct] Dimension limits */    
/* I [nbr] number of dimensions with user-specified limits */    
/* I [sng] User-specified delimiter string, if any */    

void nco_sql_ins_vals (PGconn *conn, char * pgschema, char * pgtable, const int in_id, const char * const var_nm, lmt_all_sct * const *lmt_lst, const int lmt_nbr) 
{
  /* Purpose:
     Get variable with limits from input file
     Get dimensional units
     if PRN.. = False print var taking account of FORTRAN (need var indices)
     if PRN_DMN_IDX_CRD_VAL then read in co-ord dims
     if PRN.. = True print var taking account of FORTRAN (Use dims to calculate var indices */

  nco_bool MALLOC_UNITS_SNG=False; /* [flg] Allocated memory for units string */
  char nul_chr='\0';
  char var_sng[NCO_MAX_LEN_FMT_SNG];
  char *unit_sng;
  char qry[NCO_MAX_LEN_FMT_SNG];
  const char units_nm[]="units"; /* [sng] Name of units attribute */
  float lat = 0;
  float lng = 0;
  int rcd_lcl; /* [rcd] Return code */
  int att_id; /* [id] Attribute ID */
  long att_sz;
  nc_type att_typ;

  PGresult *res;

  /* Get variable with limits from input file */
  int rcd;
  int idx;
  int jdx;
  int *dmn_id=NULL_CEWI;

  /* For regular data */
  long lmn;

  dmn_sct *dim=NULL_CEWI;

  lmt_all_sct **lmt_mult=NULL_CEWI;
  lmt_sct **lmt=NULL_CEWI;

  var_sct var;

  /* set defaults */
  var_dfl_set(&var);

  /* Initialize units string, overwrite later if necessary */
  unit_sng=&nul_chr;

  /* Get var_id for requested variable */
  var.nm=(char *)strdup(var_nm);
  var.nc_id=in_id;
  nco_inq_varid(in_id,var_nm,&var.id);

  /* Get type and number of dimensions for variable */
  (void)nco_inq_var(in_id,var.id,(char *)NULL,&var.type,&var.nbr_dim,(int *)NULL,(int *)NULL);

  if (!pgtable)
    pgtable = var_nm;

  /* Deal with scalar variables */
  if(var.nbr_dim == 0){
    var.sz=1L;
    var.val.vp=nco_malloc(nco_typ_lng(var.type));
    /* Block is critical/thread-safe for identical/distinct in_id's */
    { /* begin potential OpenMP critical */
      (void)nco_get_var1(in_id,var.id,0L,var.val.vp,var.type);
    } /* end potential OpenMP critical */
  } /* end if */

  dmn_id=(int *)nco_malloc(var.nbr_dim*sizeof(int));
  lmt_mult=(lmt_all_sct **)nco_malloc(var.nbr_dim*sizeof(lmt_all_sct *));
  lmt=(lmt_sct **)nco_malloc(var.nbr_dim*sizeof(lmt_sct *));

  /* Get dimension IDs from input file */
  (void)nco_inq_vardimid(in_id,var.id,dmn_id);

  /* Initialize lmt_mult with multi-limits from lmt_lst limits */
  /* Get dimension sizes from input file */
  for(idx=0;idx< var.nbr_dim;idx++)
    for(jdx=0;jdx< lmt_nbr;jdx++){
      if(dmn_id[idx] == lmt_lst[jdx]->lmt_dmn[0]->id){
        lmt_mult[idx]=lmt_lst[jdx];
        break;
      } /* end if */
    } /* end loop over jdx */

  /* Call super-dooper recursive routine */
  /* NB: nco_sql_rec_clc() with same nc_id contains OpenMP critical region */
  var.val.vp=nco_sql_rec_clc(0,var.nbr_dim,lmt,lmt_mult,&var);
  /* Call also initializes var.sz with final size */

  /* Warn if variable is packed */
  if(nco_pck_dsk_inq(in_id,&var)) (void)fprintf(stderr,"%s: WARNING about to print packed contents of packed variable \"%s\". Consider unpacking it first using ncpdq -U.\n",prg_nm_get(),var_nm);  

  /* Does variable have character attribute named units_nm? */
  rcd_lcl=nco_inq_attid_flg(in_id,var.id,units_nm,&att_id);
  if(rcd_lcl == NC_NOERR){
    (void)nco_inq_att(in_id,var.id,units_nm,&att_typ,&att_sz);
    if(att_typ == NC_CHAR){
       unit_sng=(char *)nco_malloc((att_sz+1)*nco_typ_lng(att_typ));
       (void)nco_get_att(in_id,var.id,units_nm,unit_sng,att_typ);
       unit_sng[(att_sz+1)*nco_typ_lng(att_typ)-1]='\0';
       MALLOC_UNITS_SNG=True; /* [flg] Allocated memory for units string */
    } /* end if */
  } /* end if */

  if(var.nbr_dim == 0){
    /* Variable is scalar, byte, or character */
    lmn=0;
    (void)sprintf(var_sng,"%%s = %s %%s\n",nco_typ_fmt_sng(var.type)); 

    switch(var.type){
      case NC_FLOAT: (void)fprintf(stdout,var_sng,var_nm,var.val.fp[lmn],unit_sng); break;
      case NC_DOUBLE: (void)fprintf(stdout,var_sng,var_nm,var.val.dp[lmn],unit_sng); break;
      case NC_SHORT: (void)fprintf(stdout,var_sng,var_nm,var.val.sp[lmn],unit_sng); break;
      case NC_INT: (void)fprintf(stdout,var_sng,var_nm,var.val.ip[lmn],unit_sng); break;
      case NC_CHAR:
        if(var.val.cp[lmn] != '\0'){
          (void)sprintf(var_sng,"%%s = '%s' %%s\n",nco_typ_fmt_sng(var.type));
          (void)fprintf(stdout,var_sng,var_nm,var.val.cp[lmn],unit_sng);
        }else{ /* Deal with NUL character here */
          (void)fprintf(stdout, "%s = \"\" %s\n",var_nm,unit_sng);
        } /* end if */
      break;
      case NC_BYTE: (void)fprintf(stdout,var_sng,var_nm,(unsigned char)var.val.bp[lmn],unit_sng); break;
      case NC_UBYTE: (void)fprintf(stdout,var_sng,var_nm,var.val.ubp[lmn],unit_sng); break;
      case NC_USHORT: (void)fprintf(stdout,var_sng,var_nm,var.val.usp[lmn],unit_sng); break;
      case NC_UINT: (void)fprintf(stdout,var_sng,var_nm,var.val.uip[lmn],unit_sng); break;
      case NC_INT64: (void)fprintf(stdout,var_sng,var_nm,var.val.i64p[lmn],unit_sng); break;
      case NC_UINT64: (void)fprintf(stdout,var_sng,var_nm,var.val.ui64p[lmn],unit_sng); break;
      case NC_STRING: (void)fprintf(stdout,var_sng,var_nm,var.val.sngp[lmn],unit_sng); break;
      default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */      
  } /* end if variable is scalar, byte, or character */

  if(var.nbr_dim > 0){
    long *mod_map_in;
    long *mod_map_out;
    long *dmn_sbs_ram; /* Indices in hyperslab */
    long *dmn_sbs_dsk; /* Indices of hyperslab relative to original on disk */
    long var_dsk;

    mod_map_in=(long *)nco_malloc(var.nbr_dim*sizeof(long));
    mod_map_out=(long *)nco_malloc(var.nbr_dim*sizeof(long));
    dmn_sbs_ram=(long *)nco_malloc(var.nbr_dim*sizeof(long));
    dmn_sbs_dsk=(long *)nco_malloc(var.nbr_dim*sizeof(long));

    /* Create mod_map_in */
    for(idx=0;idx<var.nbr_dim;idx++) mod_map_in[idx]=1L;
    for(idx=0;idx<var.nbr_dim;idx++)
      for(jdx=idx+1;jdx<var.nbr_dim;jdx++)
        mod_map_in[idx]*=lmt_mult[jdx]->dmn_sz_org;

    /* Create mod_map_out */
    for(idx=0;idx< var.nbr_dim;idx++) 
      mod_map_out[idx]=1L;

    for(idx=0;idx< var.nbr_dim;idx++)
      for(jdx=idx;jdx<var.nbr_dim;jdx++)
        mod_map_out[idx]*=lmt_mult[jdx]->dmn_cnt;

    /* Read coordinate dimensions */
    var_sct var_crd;

    dim=(dmn_sct *)nco_malloc(var.nbr_dim*sizeof(dmn_sct));
    for(idx=0;idx <var.nbr_dim;idx++){
      dim[idx].val.vp=NULL;
      dim[idx].nm=lmt_mult[idx]->dmn_nm;
      rcd=nco_inq_varid_flg(in_id,dim[idx].nm,&dim[idx].cid);
      
      /* If not a variable */
      if(rcd != NC_NOERR){
        dim[idx].is_crd_dmn=False;
        dim[idx].cid=-1;
        continue;
      } /* end if */

      dim[idx].is_crd_dmn=True;
      (void)nco_inq_vartype(in_id,dim[idx].cid,&dim[idx].type);
      var_crd.nc_id=in_id;
      var_crd.nm=dim[idx].nm;
      var_crd.type=dim[idx].type;
      var_crd.id=dim[idx].cid;
      /* Read coordinate variable with limits applied */
      /* NB: nco_sql_rec_clc() with same nc_id contains OpenMP critical region */
      dim[idx].val.vp=nco_sql_rec_clc(0,1,lmt,lmt_mult+idx,&var_crd);

      /* Typecast pointer before use */
      (void)cast_void_nctype(dim[idx].type,&dim[idx].val);
    }/* end for */
  

    for(lmn=0;lmn<var.sz;lmn++){
      //(void) fprintf(stderr, "\nCalc var\n");        
      /* Caculate RAM indices from current limit */
      for(idx=0;idx <var.nbr_dim;idx++)
	      dmn_sbs_ram[idx]=(lmn%mod_map_out[idx])/(idx == var.nbr_dim-1 ? 1L : mod_map_out[idx+1]);

      /* Calculate disk indices from RAM indices */
      (void)nco_msa_ram_2_dsk(dmn_sbs_ram,lmt_mult,var.nbr_dim,dmn_sbs_dsk,(lmn==var.sz-1));

      /* Find variable index relative to disk */
      var_dsk=0;
      for(idx=0;idx <var.nbr_dim;idx++)	
        var_dsk+=dmn_sbs_dsk[idx]*mod_map_in[idx];

      /* Skip rest of loop unless element is first in string */
      if(var.type == NC_CHAR && dmn_sbs_ram[var.nbr_dim-1] > 0) 
        goto lbl_chr_prn;

      /* Print dimensions with indices along with values if they are coordinate variables */
      int dmn_idx;
      long dmn_sbs_prn;
      long crd_idx_crr;
      char dmn_sng[NCO_MAX_LEN_FMT_SNG];

      /* Loop over dimensions whose coordinates are to be printed */
      for(idx=0;idx<var.nbr_dim;idx++){
        /* Reverse dimension ordering for Fortran convention */
        dmn_idx=idx; 

  	    /* Format and print dimension part of output string for non-coordinate variables */
  	    if(dim[dmn_idx].cid == var.id) continue; /* If variable is a coordinate then skip printing until later */
        if(!dim[dmn_idx].is_crd_dmn){ /* If dimension is not a coordinate... */
          (void)fprintf(stdout,"%s[%ld] ",dim[dmn_idx].nm,dmn_sbs_dsk[dmn_idx]);  	   
  	      continue;
  	    } /* end if */

    	  (void)sprintf(dmn_sng,"Iter %u %%s[%%ld]=%s ",idx, nco_typ_fmt_sng(dim[dmn_idx].type)); 
        dmn_sbs_prn=dmn_sbs_dsk[dmn_idx];

        /* Account for hyperslab offset in coordinate values*/
        crd_idx_crr=dmn_sbs_ram[dmn_idx];
        if (dmn_idx == 1)
          lat = dim[dmn_idx].val.fp[crd_idx_crr];
        else if (dmn_idx == 2)
          lng = dim[dmn_idx].val.fp[crd_idx_crr];
      
        switch(dim[dmn_idx].type){
          case NC_FLOAT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.fp[crd_idx_crr]);  break;
    	    case NC_DOUBLE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.dp[crd_idx_crr]); break;
    	    case NC_SHORT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.sp[crd_idx_crr]); break;
    	    case NC_INT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.ip[crd_idx_crr]); break;
    	    case NC_CHAR: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.cp[crd_idx_crr]); break;
    	    case NC_BYTE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,(unsigned char)dim[dmn_idx].val.bp[crd_idx_crr]); break;
    	    case NC_UBYTE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.ubp[crd_idx_crr]); break;
    	    case NC_USHORT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.usp[crd_idx_crr]); break;
    	    case NC_UINT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.uip[crd_idx_crr]); break;
    	    case NC_INT64: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.i64p[crd_idx_crr]); break;
    	    case NC_UINT64: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.ui64p[crd_idx_crr]); break;
    	    case NC_STRING: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.sngp[crd_idx_crr]); break;
    	    default: nco_dfl_case_nc_type_err(); break;
        } 
      } /* end loop over dimensions */
  
      (void) sprintf(qry, "INSERT INTO %s.%s (id, lat, long, prec_text) VALUES (nextval('cdfdata_gid_seq'), %g, %g, %g);", pgschema, pgtable, lat, lng, var.val.fp[lmn]);
    
      /*res = PQexec(conn,qry);
      if (PQresultStatus(res) != PGRES_COMMAND_OK)
      {
          fprintf(stderr, "Insert failed:\n%s\n", PQerrorMessage(conn));
          PQclear(res);
    	 PQfinish(conn);
          exit(1);
      }
      else
        PQclear(res);*/

      /* Print all characters in last dimension each time penultimate dimension subscript changes to its start value */
      lbl_chr_prn:
    
        if(var.type == NC_CHAR){
          static nco_bool NULL_IN_SLAB;
          static char *prn_sng;
          static int chr_cnt;
          static long dmn_sz;
          static long var_dsk_srt;
          static long var_dsk_end;

          /* At beginning of character array */
          if(dmn_sbs_ram[var.nbr_dim-1] == 0L) {
            dmn_sz=lmt_mult[var.nbr_dim-1]->dmn_cnt;
            prn_sng=(char *)nco_malloc((size_t)dmn_sz+1UL);
    	      var_dsk_srt=var_dsk;
            var_dsk_end=var_dsk;
            chr_cnt=0;
            NULL_IN_SLAB=False;
    	    } /* end if */

    	    /* In middle of array---save characters to prn_sng */
          prn_sng[chr_cnt++]=var.val.cp[lmn];
          if(var.val.cp[lmn] == '\0' && !NULL_IN_SLAB){
            var_dsk_end=var_dsk;
            NULL_IN_SLAB=True;
          } /* end if */

          /* At end of character array */
          if(dmn_sbs_ram[var.nbr_dim-1] == dmn_sz-1 ){
            if(NULL_IN_SLAB){
              (void)sprintf(var_sng,"Findme1 %%s[%%ld--%%ld]=\"%%s\" %%s");
            }else{
              (void)sprintf(var_sng,"Findme2 %%s[%%ld--%%ld]='%%s' %%s");
              prn_sng[chr_cnt]='\0';
              var_dsk_end=var_dsk;
            } /* end if */

            (void)printf("precursor: ");
            (void)fprintf(stdout,var_sng,var_nm,var_dsk_srt,var_dsk_end,prn_sng,unit_sng);
            (void)fprintf(stdout,"x\nx");
            (void)fflush(stdout);
            (void)nco_free(prn_sng);
          } /* endif */	    
          continue;
        } /* end if NC_CHAR */

        (void)sprintf(var_sng,"4 %%s[%%ld]=%s %%s\n",nco_typ_fmt_sng(var.type));    

      	switch(var.type){
        	case NC_FLOAT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.fp[lmn],unit_sng); break;
        	case NC_DOUBLE: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.dp[lmn],unit_sng); break;
        	case NC_SHORT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.sp[lmn],unit_sng); break;
        	case NC_INT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.ip[lmn],unit_sng); break;
        	case NC_CHAR: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.cp[lmn],unit_sng); break;
        	case NC_BYTE: (void)fprintf(stdout,var_sng,var_nm,var_dsk,(unsigned char)var.val.bp[lmn],unit_sng); break;
        	case NC_UBYTE: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.ubp[lmn],unit_sng); break;
        	case NC_USHORT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.usp[lmn],unit_sng); break;
        	case NC_UINT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.uip[lmn],unit_sng); break;
        	case NC_INT64: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.i64p[lmn],unit_sng); break;
        	case NC_UINT64: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.ui64p[lmn],unit_sng); break;
        	case NC_STRING: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.sngp[lmn],unit_sng); break;
        	default: nco_dfl_case_nc_type_err(); break;
      	} // end switch
      } /* end loop over elements */

      (void) sprintf(qry, "INSERT INTO %s.%s (id, lat, long, prec_text) VALUES (nextval('cdfdata_gid_seq'), %g, %g, %g);", pgschema, pgtable, lat, lng, var.val.fp[lmn]);
      // (void) fprintf(stderr, "PGEXEC\n");
      res = PQexec(conn,qry);
      if (PQresultStatus(res) != PGRES_COMMAND_OK)
      {
          fprintf(stderr, "Insert failed:\n%s\n", PQerrorMessage(conn));
          PQclear(res);
       PQfinish(conn);
          exit(1);
      }
      else
        PQclear(res);
      (void)nco_free(mod_map_in);
      (void)nco_free(mod_map_out);
      (void)nco_free(dmn_sbs_ram);
      (void)nco_free(dmn_sbs_dsk);

      /* Additional newline between consecutive variables or final variable and prompt */
      (void)fflush(stdout);
  } /* end if variable has more than one dimension */

  var.val.vp=nco_free(var.val.vp);
  var.nm=(char *)nco_free(var.nm);

  if(MALLOC_UNITS_SNG) 
    unit_sng=(char *)nco_free(unit_sng);

  if(var.nbr_dim > 0){
    (void)nco_free(dmn_id);
    (void)nco_free(lmt_mult);
    (void)nco_free(lmt);
  } /* end if */
  
  for(idx=0;idx<var.nbr_dim;idx++)
    dim[idx].val.vp=nco_free(dim[idx].val.vp);

  dim=(dmn_sct *)nco_free(dim);

} /* end nco_sql_ins_var_val() */