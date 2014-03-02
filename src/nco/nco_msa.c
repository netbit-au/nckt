/* $Header: /cvsroot/nco/nco/src/nco/nco_msa.c,v 1.78 2010/03/12 07:08:15 zender Exp $ */

/* Purpose: Multi-slabbing algorithm */

/* Copyright (C) 1995--2010 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_msa.h" /* Multi-slabbing algorithm */
#include "nco_var_utl.h" /* Variable utilities */

/* fxm: strings statically allocated with NCR_MAX_LEN_FMT_SNG chars may be susceptible to buffer overflow attacks */
/* Length should be computed at run time but doing so would be painful */
#define NCO_MAX_LEN_FMT_SNG 100

void *
nco_msa_rec_clc /* [fnc] Multi-slab algorithm (recursive routine, returns a single slab pointer */
(int dpt_crr, /* [nbr] Current depth, we start at 0 */
 int dpt_crr_max, /* [nbr] Maximium depth (i.e., number of dimensions in variable (does not change) */
 lmt_sct **lmt, /* [sct] Limits of current hyperslabs (these change as we recurse) */
 lmt_all_sct **lmt_lst, /* [sct] List of limits in each dimension (this remains STATIC as we recurse) */
 var_sct *vara) /* [sct] Information for routine to read variable information and pass information between calls */
{
  /* NB: nco_msa_rec_clc() with same nc_id contains OpenMP critical region */
  /* Purpose: Multi-slab algorithm (recursive routine, returns a single slab pointer */
  int idx;
  int nbr_slb;
  void *vp;

  if(dpt_crr == dpt_crr_max) goto read_lbl;

  nbr_slb=lmt_lst[dpt_crr]->lmt_dmn_nbr;

  if(nbr_slb == 1){
    lmt[dpt_crr]=lmt_lst[dpt_crr]->lmt_dmn[0];
    /* NB: nco_msa_rec_clc() with same nc_id contains OpenMP critical region */
    vp=nco_msa_rec_clc(dpt_crr+1,dpt_crr_max,lmt,lmt_lst,vara);
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
      /* NB: nco_msa_rec_clc() with same nc_id contains OpenMP critical region */
      cp_wrp[idx]=(char *)nco_msa_rec_clc(dpt_crr+1,dpt_crr_max,lmt,lmt_lst,vara);
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

} /* end nco_msa_rec_clc() */

void
nco_msa_prn_idx(lmt_all_sct *lmt_i)
{
  int slb_nbr;
  int idx;
  int size=lmt_i->lmt_dmn_nbr;
  long *indices;
  lmt_sct lmt;

  indices=(long *)nco_malloc(size *sizeof(long));

  printf("name=%s total size=%ld\n",lmt_i->dmn_nm,lmt_i->dmn_cnt);

  for(idx=0;idx<size;idx++) indices[idx]=lmt_i->lmt_dmn[idx]->srt;

  while(nco_msa_clc_idx(False,lmt_i,&indices[0],&lmt,&slb_nbr))
    printf("slb_nbr=%d srt=%ld end=%ld cnt=%ld srd=%ld\n",slb_nbr,lmt.srt,lmt.end,lmt.cnt,lmt.srd);
} /* end nco_msa_prn_idx() */

nco_bool /* [flg] There are more limits to process in the slab */
nco_msa_clc_idx
(nco_bool NORMALIZE,
 lmt_all_sct *lmt_a, /* I list of lmts for each dimension  */
 long *indices, /* I/O so routine can keep track of where its at */
 lmt_sct *lmt, /* O Output hyperslab */
 int *slb) /* slab which above limit refers to */
{
  /* A very messy unclear in-efficient routine that needs re-writing
     if NORMALIZE then It returns the slab number and the hyperslab in lmt- Note
     VERY IMPORTANT - This is the slab WHITH-IN the slab
     So the stride is ALWAYS 1 */
  int sz_idx;
  int size=lmt_a->lmt_dmn_nbr;
  nco_bool *mnm;
  nco_bool rcd;

  int prv_slb=0;
  int crr_slb=0;
  long crr_idx;
  long prv_idx=long_CEWI;
  long cnt=0L;

  mnm=(nco_bool *)nco_malloc(size*sizeof(nco_bool));

  lmt->srt=-1L;
  lmt->cnt=0L;
  lmt->srd=0L;

  while(++cnt){
    crr_idx=nco_msa_min_idx(indices,mnm,size);

    crr_slb=-1;
    for(sz_idx=0;sz_idx<size;sz_idx++)
      if(mnm[sz_idx]){crr_slb=sz_idx;break;}

    if(crr_slb == -1){
      if(lmt->srt == -1){
	rcd=False;
	goto cln_and_xit;
      }else break;
    } /* endif */

    if(mnm[prv_slb]) crr_slb=prv_slb;

    if(lmt->srt > -1 && crr_slb != prv_slb) break;

    if(lmt->cnt > 1L){
      (lmt->cnt)++;
      lmt->end=crr_idx;
    } /* end if */

    if(lmt->cnt == 1L){
      lmt->cnt=2L;
      lmt->srd=crr_idx-prv_idx;
      lmt->end=crr_idx;
    } /* end if */

    if(lmt->srt == -1L){
      lmt->srt=crr_idx;
      lmt->cnt=1L;
      lmt->end=crr_idx;
      lmt->srd=1L;
    } /* end if */

    for(sz_idx=0;sz_idx<size;sz_idx++){
      if(mnm[sz_idx]){
	indices[sz_idx]+=lmt_a->lmt_dmn[sz_idx]->srd;
	if(indices[sz_idx] > lmt_a->lmt_dmn[sz_idx]->end) indices[sz_idx]=-1;
      }
    } /* end loop over sz_idx */
    prv_idx=crr_idx;
    prv_slb=crr_slb;
  } /* end while */

  *slb=prv_slb;

  /* Normalize slab */
  if(NORMALIZE){
    lmt->srt=(lmt->srt-lmt_a->lmt_dmn[*slb]->srt)/(lmt_a->lmt_dmn[*slb]->srd);
    lmt->end=(lmt->end-lmt_a->lmt_dmn[*slb]->srt)/(lmt_a->lmt_dmn[*slb]->srd);
    lmt->srd=1L;
  } /* end if */

  rcd=True;

  /* Jump here if only one string */
 cln_and_xit:
  mnm=(nco_bool *)nco_free(mnm);

  return rcd;
} /* end nco_msa_clc_idx() */

void
nco_msa_ram_2_dsk /* convert hyperslab indices (in RAM) to hyperlsab indices relative */
(long *dmn_sbs_ram,   /* to disk. */
 lmt_all_sct **lmt_mult,
 int nbr_dim,
 long *dmn_sbs_dsk,
 nco_bool FREE){
  /*  It does not really convert RAM indices to disk indices, but given a set
      of RAM indices finds the next set of dsk incdices.
      So it only works if the indices fed to it are continuous */
  int idx;
  int jdx;
  int size;
  static int initialize;
  static long **dmn_indices;
  static long *dmn_sbs_prv;
  static nco_bool mnm[100];

  if(!initialize){
    dmn_sbs_prv=(long *)nco_malloc(nbr_dim*sizeof(long));
    dmn_indices=(long **)nco_malloc(nbr_dim*sizeof(long *));
    for(idx=0;idx<nbr_dim;idx++){
      dmn_indices[idx]=(long *)nco_malloc(lmt_mult[idx]->lmt_dmn_nbr*sizeof(long));
      for(jdx=0;jdx<lmt_mult[idx]->lmt_dmn_nbr;jdx++)
	dmn_indices[idx][jdx]=lmt_mult[idx]->lmt_dmn[jdx]->srt;
      dmn_sbs_prv[idx]=-1L;
    }
    initialize=1;
  }
  for(idx=0;idx <nbr_dim;idx ++){
    size=lmt_mult[idx]->lmt_dmn_nbr;
    if(dmn_sbs_ram[idx] == dmn_sbs_prv[idx]) continue;

    if(lmt_mult[idx]->BASIC_DMN){
      dmn_sbs_dsk[idx]=dmn_sbs_ram[idx];
      continue;
    }

    /* re-initialize indices if 0*/
    if(dmn_sbs_ram[idx] == 0)
      for(jdx=0;jdx<size;jdx++)
       	dmn_indices[idx][jdx]=lmt_mult[idx]->lmt_dmn[jdx]->srt;

    /* Deal with wrapping - we have 2 hyperlsbas to deal with */
    if(lmt_mult[idx]->WRP){
      if(dmn_indices[idx][0]<lmt_mult[idx]->dmn_sz_org){
	dmn_sbs_dsk[idx]=dmn_indices[idx][0];
	dmn_indices[idx][0]+=lmt_mult[idx]->lmt_dmn[0]->srd;
      }else{
	dmn_sbs_dsk[idx]=dmn_indices[idx][1];
	dmn_indices[idx][1]+=lmt_mult[idx]->lmt_dmn[1]->srd;
      }
      continue;
    }

    dmn_sbs_dsk[idx]=nco_msa_min_idx(dmn_indices[idx],mnm,size);

    for(jdx=0;jdx<size;jdx++){
      if(mnm[jdx]){
	dmn_indices[idx][jdx]+=lmt_mult[idx]->lmt_dmn[jdx]->srd;
	if(dmn_indices[idx][jdx] > lmt_mult[idx]->lmt_dmn[jdx]->end) dmn_indices[idx][jdx]=-1;
      }
    } /* end for  jdx */
  } /* end for idx */

  for(idx=0;idx<nbr_dim;idx++) dmn_sbs_prv[idx]=dmn_sbs_ram[idx];

  /* Free static space on last call */
  if(FREE){
    (void)nco_free(dmn_sbs_prv);
    for(idx=0;idx <nbr_dim;idx++)
      (void)nco_free(dmn_indices[idx]);
    (void)nco_free(dmn_indices);
    initialize=0;
  }
}

void
nco_msa_clc_cnt
(lmt_all_sct *lmt_lst)
{
  int idx;
  long cnt=0;
  int size=lmt_lst->lmt_dmn_nbr;
  long *indices;
  nco_bool *mnm;

  /* Degenerate case */
  if(size == 1){
    lmt_lst->dmn_cnt=lmt_lst->lmt_dmn[0]->cnt;
    return;
  } /* end if */

  /* If slabs remain in user-specified order */
  if(lmt_lst->MSA_USR_RDR){
    for(idx=0;idx<size;idx++) cnt+=lmt_lst->lmt_dmn[idx]->cnt;
    lmt_lst->dmn_cnt=cnt;
  }else{
    indices=(long *)nco_malloc(size*sizeof(long));
    mnm=(nco_bool *)nco_malloc(size*sizeof(nco_bool));

    /* Initialize indices with srt */
    for(idx=0;idx<size;idx++) indices[idx]=lmt_lst->lmt_dmn[idx]->srt;

    while(nco_msa_min_idx(indices,mnm,size) != LONG_MAX){
      for(idx=0;idx<size;idx++){
        if(mnm[idx]){
	  indices[idx]+=lmt_lst->lmt_dmn[idx]->srd;
	  if(indices[idx] > lmt_lst->lmt_dmn[idx]->end) indices[idx]=-1;
        } /* end if */
      } /* end loop over idx */
      cnt++;
    } /* end while */
    lmt_lst->dmn_cnt=cnt;

    indices=(long *)nco_free(indices);
    mnm=(nco_bool  *)nco_free(mnm);
  } /* end else */

  return; /* 20050109: return squelches erroneous gcc-3.4.2 warning */
} /* end nco_msa_clc_cnt() */

nco_bool
nco_msa_ovl
(lmt_all_sct *lmt_lst)
{
  /* Purpose: Return true if limits overlap
     NB: Assumes that limits have been sorted */

  long idx;
  long jdx;
  long sz=lmt_lst->lmt_dmn_nbr;

  lmt_sct **lmt;
  /* defererence */
  lmt=lmt_lst->lmt_dmn;

  for(idx=0; idx<sz; idx++)
    for(jdx=idx+1; jdx<sz ;jdx++)
      if( lmt[jdx]->srt <= lmt[idx]->end) return True;

  return False;
}

int /* O [enm] Comparison result [<,=,>] 0 iff val_1 [<,==,>] val_2 */
nco_cmp_lmt_srt /* [fnc] Compare two lmt_sct's by srt member */
(const void * vp1, /* I [sct] lmt_sct to compare */
 const void * vp2) /* I [sct] lmt_sct to compare */
{
  const lmt_sct * const lmt1=*((const lmt_sct * const *)vp1);
  const lmt_sct * const lmt2=*((const lmt_sct * const *)vp2);
  /* fxm: need to compiler warnings. will following work?
      const lmt_sct * const lmt1=(const lmt_sct *)vp1;
      const lmt_sct * const lmt2=(const lmt_sct *)vp2; */

  return lmt1->srt < lmt2->srt ? -1 : (lmt1->srt > lmt2->srt);
}

void nco_msa_qsort_srt
(lmt_all_sct *lmt_lst)
{
  lmt_sct **lmt;
  long sz;

  sz=lmt_lst->lmt_dmn_nbr;
  lmt=lmt_lst->lmt_dmn;

  if(sz <= 1) return;

  (void)qsort(lmt,(size_t)sz,sizeof(lmt_sct *),nco_cmp_lmt_srt);
}

void
nco_msa_wrp_splt /* [fnc] Split wrapped dimensions */
(lmt_all_sct *lmt_lst)
{
  int idx;
  int jdx;
  int size=lmt_lst->lmt_dmn_nbr;
  long dmn_sz_org=lmt_lst->dmn_sz_org;
  long srt;
  long cnt;
  long srd;
  long kdx=0; /*  */
  lmt_sct *lmt_wrp;

  for(idx=0;idx<size;idx++){

    if(lmt_lst->lmt_dmn[idx]->srt > lmt_lst->lmt_dmn[idx]->end){

      lmt_wrp=(lmt_sct *)nco_malloc(2*sizeof(lmt_sct));
      srt=lmt_lst->lmt_dmn[idx]->srt;
      cnt=lmt_lst->lmt_dmn[idx]->cnt;
      srd=lmt_lst->lmt_dmn[idx]->srd;

      for(jdx=0;jdx<cnt;jdx++){
	kdx=(srt+srd*jdx)%dmn_sz_org;
	if(kdx<srt) break;
      } /* end loop over jdx */

      lmt_wrp[0]=*(lmt_lst->lmt_dmn[idx]);
      lmt_wrp[1]=*(lmt_lst->lmt_dmn[idx]);
      lmt_wrp[0].srt=srt;

      if(jdx == 1){
	lmt_wrp[0].end=srt;
	lmt_wrp[0].cnt=1L;
	lmt_wrp[0].srd=1L;
      }else{
	lmt_wrp[0].end=srt+srd*(jdx-1);
	lmt_wrp[0].cnt=jdx;
	lmt_wrp[0].srd=srd;
      } /* end else */

      lmt_wrp[1].srt=kdx;
      lmt_wrp[1].cnt=cnt-lmt_wrp[0].cnt;
      if(lmt_wrp[1].cnt == 1L){
	lmt_wrp[1].end=kdx;
	lmt_wrp[1].srd=1L;
      }else{
	lmt_wrp[1].end=kdx+(lmt_wrp[1].cnt-1)*srd;
	lmt_wrp[1].srd=srd;
      } /* end else */

      /* Insert new limits into array */
      lmt_lst->lmt_dmn[idx]=lmt_wrp;
      lmt_lst->lmt_dmn=(lmt_sct **)nco_realloc(lmt_lst->lmt_dmn,((lmt_lst->lmt_dmn_nbr)+1)*sizeof(lmt_sct *));
      lmt_lst->lmt_dmn[(lmt_lst->lmt_dmn_nbr)++]=++lmt_wrp;
    } /* endif srt > end */
  } /* end loop over size */

  /* Check if genuine wrapped co-ordinate */
  if(size==1 && lmt_lst->lmt_dmn_nbr==2) lmt_lst->WRP=True;
} /* end nco_msa_wrp_splt() */

long /* O [idx] Minimum value */
nco_msa_min_idx /* [fnc] Find minimum values in current and return minimum value */
(const long * const current, /* I [idx] Current indices */
 nco_bool * const mnm, /* O [flg] Minimum */
 const int size) /* I [nbr] Size of current and mnm */
{
  int sz_idx;
  long min_val=LONG_MAX;

  for(sz_idx=0;sz_idx<size;sz_idx++)
    if(current[sz_idx] != -1 && current[sz_idx] < min_val) min_val=current[sz_idx];

  for(sz_idx=0;sz_idx<size;sz_idx++)
    mnm[sz_idx]=((current[sz_idx] != -1 && current[sz_idx] == min_val) ? True : False);

  return min_val;
} /* end nco_msa_min_idx() */

void
nco_msa_var_get    /* [fnc] Get var data from disk taking accound of multihyperslabs */
(const int in_id,  /* I [id] netCDF input file ID */
 var_sct *var_in,
 lmt_all_sct * const * lmt_lst, /* I multi-hyperslab limits */
 int nbr_dmn_fl) /* I [nbr] Number of multi-hyperslab limits */
{
int idx;
int jdx;
int nbr_dim;
nc_type typ_tmp;
void *void_ptr;
lmt_all_sct **lmt_mult;
lmt_sct **lmt;

  nbr_dim=var_in->nbr_dim;

  // Refresh nc_id with in_id, NB: makes OpenMP threading work
  // Should have been included in release 3.9.5
  // Fixes TODO nco956
  var_in->nc_id=in_id;
  /* Deal with scalar var */
  if(nbr_dim==0){
   var_in->val.vp=nco_malloc(nco_typ_lng(var_in->typ_dsk));
   (void)nco_get_var1(in_id,var_in->id,0L,var_in->val.vp,var_in->typ_dsk);
   goto do_unpck;
   /*return; */
  }

  lmt_mult=(lmt_all_sct **)nco_malloc(nbr_dim*sizeof(lmt_all_sct *));
  lmt=(lmt_sct **)nco_malloc(nbr_dim*sizeof(lmt_sct *));

/* Initialize lmt_mult with multi-limits from lmt_lst limits */
  for(idx=0;idx<nbr_dim;idx++){
    for(jdx=0;jdx<nbr_dmn_fl;jdx++){
      if(!strcmp(var_in->dim[idx]->nm,lmt_lst[jdx]->dmn_nm ) ){
	lmt_mult[idx]=lmt_lst[jdx];
        break;
      } /* end if */
    } /* end loop over jdx */
  } /* end idx */

  /* Call super-dooper recursive routine */
  /* NB: nco_msa_rec_clc() with same nc_id contains OpenMP critical region */
  typ_tmp=var_in->type;
  var_in->type=var_in->typ_dsk;
  void_ptr=nco_msa_rec_clc(0,nbr_dim,lmt,lmt_mult,var_in);

  var_in->type=typ_tmp;
  var_in->val.vp=void_ptr;

  (void)nco_free(lmt_mult);
  (void)nco_free(lmt);

do_unpck:
  /* Following code copied from nco_var_get() */

  if(var_in->pck_dsk) var_in=nco_cnv_mss_val_typ(var_in,var_in->typ_dsk);
  /*    var=nco_cnv_mss_val_typ(var,var->typ_dsk);*/

  /* Type of variable and missing value in memory are now same as type on disk */
  var_in->type=var_in->typ_dsk; /* [enm] Type of variable in RAM */

  /* Packing in RAM is now same as packing on disk pck_dbg
     fxm: This nco_pck_dsk_inq() call is never necessary for non-packed variables */
  (void)nco_pck_dsk_inq(in_id,var_in);

  /* Packing/Unpacking */
  if(nco_is_rth_opr(prg_get())){
    /* Arithmetic operators must unpack variables before performing arithmetic
       Otherwise arithmetic will produce garbage results */
    /* 20050519: Not sure why I originally made nco_var_upk() call SMP-critical
       20050629: Making this region multi-threaded causes no problems */
    if(var_in->pck_dsk) var_in=nco_var_upk(var_in);
  } /* endif arithmetic operator */

  return;
} /* end nco_msa_var_get */

void
nco_cpy_var_val_mlt_lmt /* [fnc] Copy variable data from input to output file */
(const int in_id, /* I [id] netCDF input file ID */
 const int out_id, /* I [id] netCDF output file ID */
 FILE * const fp_bnr, /* I [fl] Unformatted binary output file handle */
 const nco_bool NCO_BNR_WRT, /* I [flg] Write binary file */
 char *var_nm, /* I [sng] Variable name */
 lmt_all_sct * const * lmt_lst, /* I multi-hyperslab limits */
 int nbr_dmn_fl) /* I [nbr] Number of multi-hyperslab limits */
{
  /* NB: nco_cpy_var_val_mlt_lmt() contains OpenMP critical region */
  /* Purpose: Copy variable data from input netCDF file to output netCDF file
     Routine truncates dimensions in variable definition in output file according to user-specified limits.
     Routine copies variable-by-variable, old-style, used only by ncks */

  int *dmn_id;
  int idx;
  int jdx;
  int nbr_dim;
  int nbr_dmn_in;
  int nbr_dmn_out;
  int var_in_id;
  int var_out_id;

  /* For regular data */
  long *dmn_map_in;
  long *dmn_map_out;
  long *dmn_map_srt;
  long var_sz=1L;

  nc_type var_type;

  var_sct vara;/* To hold basic data in_id, var_id, nctype for recusive routine */

  void *void_ptr;

  lmt_all_sct **lmt_mult;
  lmt_sct **lmt;

  /* Get var_id for requested variable from both files */
  nco_inq_varid(in_id,var_nm,&var_in_id);
  nco_inq_varid(out_id,var_nm,&var_out_id);

  /* Get type and number of dimensions for variable */
  (void)nco_inq_var(out_id,var_out_id,(char *)NULL,&var_type,&nbr_dmn_out,(int *)NULL,(int *)NULL);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_type,&nbr_dmn_in,(int *)NULL,(int *)NULL);
  if(nbr_dmn_out != nbr_dmn_in){
    (void)fprintf(stderr,"%s: ERROR attempt to write %d dimensional input variable %s to %d dimensional space in output file\n",prg_nm_get(),nbr_dmn_in,var_nm,nbr_dmn_out);
    nco_exit(EXIT_FAILURE);
  } /* endif */
  nbr_dim=nbr_dmn_out;

  /* Deal with scalar variables */
  if(nbr_dim == 0){
    var_sz=1L;
    void_ptr=nco_malloc(nco_typ_lng(var_type));
    /* Block is critical/thread-safe for identical/distinct in_id's */
    { /* begin potential OpenMP critical */
      (void)nco_get_var1(in_id,var_in_id,0L,void_ptr,var_type);
    } /* end potential OpenMP critical */
    /* Block is always critical */
    { /* begin OpenMP critical */
      (void)nco_put_var1(out_id,var_out_id,0L,void_ptr,var_type);
    } /* end OpenMP critical */
    if(NCO_BNR_WRT) nco_bnr_wrt(fp_bnr,var_nm,var_sz,var_type,void_ptr);
    (void)nco_free(void_ptr);
    return;
  } /* end if */

  dmn_map_in=(long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_map_out=(long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_map_srt=(long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_id=(int *)nco_malloc(nbr_dim*sizeof(int));

  lmt_mult=(lmt_all_sct **)nco_malloc(nbr_dim*sizeof(lmt_all_sct *));
  lmt=(lmt_sct **)nco_malloc(nbr_dim*sizeof(lmt_sct *));

  /* Get dimension IDs from input file */
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_id);

  /* Initialize lmt_mult with multi-limits from lmt_lst limits */
  /* Get dimension sizes from input file */
  for(idx=0;idx<nbr_dim;idx++){
    for(jdx=0;jdx<nbr_dmn_fl;jdx++){
      if(dmn_id[idx] == lmt_lst[jdx]->lmt_dmn[0]->id){
	lmt_mult[idx]=lmt_lst[jdx];
        break;
      } /* end if */
    } /* end loop over jdx */
    /* Create maps now---they maybe useful later */
    (void)nco_inq_dimlen(in_id,dmn_id[idx],&dmn_map_in[idx]);
    dmn_map_out[idx]=lmt_mult[idx]->dmn_cnt;
    dmn_map_srt[idx]=0L;
  } /* end for */

  /* Initalize vara with in_id, var_in_id, nctype, etc. so recursive routine can read data */
  vara.nm=var_nm;
  vara.id=var_in_id;
  vara.nc_id=in_id;
  vara.type=var_type;

  /* Call super-dooper recursive routine */
  /* NB: nco_msa_rec_clc() with same nc_id contains OpenMP critical region */
  void_ptr=nco_msa_rec_clc(0,nbr_dim,lmt,lmt_mult,&vara);
  var_sz=vara.sz;

  /* Block is always critical */
  { /* begin OpenMP critical */
    (void)nco_put_vara(out_id,var_out_id,dmn_map_srt,dmn_map_out,void_ptr,var_type);
  } /* end OpenMP critical */

  if(NCO_BNR_WRT) nco_bnr_wrt(fp_bnr,var_nm,var_sz,var_type,void_ptr);

  (void)nco_free(void_ptr);
  (void)nco_free(dmn_map_in);
  (void)nco_free(dmn_map_out);
  (void)nco_free(dmn_map_srt);
  (void)nco_free(dmn_id);
  (void)nco_free(lmt_mult);
  (void)nco_free(lmt);

  return;
} /* end nco_cpy_var_val_mlt_lmt() */

void
nco_msa_c_2_f /* [fnc] Replace brackets with parentheses in a string */
(char *sng) /* [sng] String to change from C to Fortran notation */
{
  /* Purpose: Replace brackets with parentheses in a string */
  while(*sng){
    if(*sng == '[') *sng='(';
    if(*sng == ']') *sng=')';
    sng++;
  } /* end while */
} /* end nco_msa_c_2_f() */

void
nco_msa_prn_var_val   /* [fnc] Print variable data */
(const int in_id, /* I [id] netCDF input file ID */
 const char * const var_nm, /* I [sng] Variable name */
 lmt_all_sct * const *lmt_lst, /* I [sct] Dimension limits */
 const int lmt_nbr, /* I [nbr] number of dimensions with user-specified limits */
 char * const dlm_sng, /* I [sng] User-specified delimiter string, if any */
 const nco_bool FORTRAN_IDX_CNV, /* I [flg] Hyperslab indices obey Fortran convention */
 const nco_bool PRN_DMN_UNITS, /* I [flg] Print units attribute, if any */
 const nco_bool PRN_DMN_IDX_CRD_VAL, /* I [flg] Print dimension/coordinate indices/values */
 const nco_bool PRN_DMN_VAR_NM) /* I [flg] Print dimension/variable names */
{
  /* NB: nco_msa_prn_var_val() with same nc_id contains OpenMP critical region */
  /* Purpose:
     Get variable with limits from input file
     User supplied dlm_sng, print var (includes nbr_dim == 0)
     Get dimensional units
     if nbr_dim ==0 and dlm_sng==NULL  print variable
     if PRN.. = False print var taking account of FORTRAN (need var indices)
     if PRN_DMN_IDX_CRD_VAL then read in co-ord dims
     if PRN.. = True print var taking account of FORTRAN (Use dims to calculate var indices */

  nco_bool MALLOC_UNITS_SNG=False; /* [flg] Allocated memory for units string */
  char nul_chr='\0';
  char var_sng[NCO_MAX_LEN_FMT_SNG];
  char *unit_sng;

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
  /* NB: nco_msa_rec_clc() with same nc_id contains OpenMP critical region */
  var.val.vp=nco_msa_rec_clc(0,var.nbr_dim,lmt,lmt_mult,&var);
  /* Call also initializes var.sz with final size */

  /* Warn if variable is packed */
  if(nco_pck_dsk_inq(in_id,&var)) (void)fprintf(stderr,"%s: WARNING about to print packed contents of packed variable \"%s\". Consider unpacking it first using ncpdq -U.\n",prg_nm_get(),var_nm);

  /* User supplied dlm_sng, print var (includes nbr_dmn == 0) */
  if(dlm_sng){
    /* Print each element with user-supplied formatting code */
    /* Replace C language '\X' escape codes with ASCII bytes */
    (void)sng_ascii_trn(dlm_sng);

    /* Assume -s argument (dlm_sng) formats entire string
       Otherwise, one could assume that field will be printed with format nco_typ_fmt_sng(var.type),
       and that user is only allowed to affect text between fields.
       This would be accomplished with:
       (void)sprintf(var_sng,"%s%s",nco_typ_fmt_sng(var.type),dlm_sng);*/

    for(lmn=0;lmn<var.sz;lmn++){
      switch(var.type){
      case NC_FLOAT: (void)fprintf(stdout,dlm_sng,var.val.fp[lmn]); break;
      case NC_DOUBLE: (void)fprintf(stdout,dlm_sng,var.val.dp[lmn]); break;
      case NC_SHORT: (void)fprintf(stdout,dlm_sng,var.val.sp[lmn]); break;
      case NC_INT: (void)fprintf(stdout,dlm_sng,var.val.ip[lmn]); break;
      case NC_CHAR: (void)fprintf(stdout,dlm_sng,var.val.cp[lmn]); break;
      case NC_BYTE: (void)fprintf(stdout,dlm_sng,var.val.bp[lmn]); break;
      case NC_UBYTE: (void)fprintf(stdout,dlm_sng,var.val.ubp[lmn]); break;
      case NC_USHORT: (void)fprintf(stdout,dlm_sng,var.val.usp[lmn]); break;
      case NC_UINT: (void)fprintf(stdout,dlm_sng,var.val.uip[lmn]); break;
      case NC_INT64: (void)fprintf(stdout,dlm_sng,var.val.i64p[lmn]); break;
      case NC_UINT64: (void)fprintf(stdout,dlm_sng,var.val.ui64p[lmn]); break;
      case NC_STRING: (void)fprintf(stdout,dlm_sng,var.val.sngp[lmn]); break;
      default: nco_dfl_case_nc_type_err(); break;
      } /* end switch */
    } /* end loop over element */
    (void)fprintf(stdout,"\n");

  } /* end if dlm_sng */

  if(PRN_DMN_UNITS){
    const char units_nm[]="units"; /* [sng] Name of units attribute */
    int rcd_lcl; /* [rcd] Return code */
    int att_id; /* [id] Attribute ID */
    long att_sz;
    nc_type att_typ;

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
  } /* end if PRN_DMN_UNITS */

  if(var.nbr_dim == 0 && dlm_sng == NULL){
    /* Variable is scalar, byte, or character */
    lmn=0;
    if(PRN_DMN_VAR_NM) (void)sprintf(var_sng,"%%s = %s %%s\n",nco_typ_fmt_sng(var.type)); else (void)sprintf(var_sng,"%s\n",nco_typ_fmt_sng(var.type));
    if(PRN_DMN_VAR_NM){
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
    }else{ /* !PRN_DMN_VAR_NM */
      switch(var.type){
      case NC_FLOAT: (void)fprintf(stdout,var_sng,var.val.fp[lmn]); break;
      case NC_DOUBLE: (void)fprintf(stdout,var_sng,var.val.dp[lmn]); break;
      case NC_SHORT: (void)fprintf(stdout,var_sng,var.val.sp[lmn]); break;
      case NC_INT: (void)fprintf(stdout,var_sng,var.val.ip[lmn]); break;
      case NC_CHAR:
	if(var.val.cp[lmn] != '\0'){
	  (void)sprintf(var_sng,"'%s'\n",nco_typ_fmt_sng(var.type));
	  (void)fprintf(stdout,var_sng,var.val.cp[lmn]);
	}else{ /* Deal with NUL character here */
	  (void)fprintf(stdout, "\"\"\n");
	} /* end if */
	break;
      case NC_BYTE: (void)fprintf(stdout,var_sng,(unsigned char)var.val.bp[lmn]); break;
      case NC_UBYTE: (void)fprintf(stdout,var_sng,var.val.ubp[lmn]); break;
      case NC_USHORT: (void)fprintf(stdout,var_sng,var.val.usp[lmn]); break;
      case NC_UINT: (void)fprintf(stdout,var_sng,var.val.uip[lmn]); break;
      case NC_INT64: (void)fprintf(stdout,var_sng,var.val.i64p[lmn]); break;
      case NC_UINT64: (void)fprintf(stdout,var_sng,var.val.ui64p[lmn]); break;
      case NC_STRING: (void)fprintf(stdout,var_sng,var.val.sngp[lmn]); break;
      default: nco_dfl_case_nc_type_err(); break;
      } /* end switch */
    } /* !PRN_DMN_VAR_NM */
  } /* end if variable is scalar, byte, or character */

  if(var.nbr_dim > 0 && dlm_sng == NULL){
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
    for(idx=0;idx< var.nbr_dim;idx++) mod_map_out[idx]=1L;
    for(idx=0;idx< var.nbr_dim;idx++)
      for(jdx=idx;jdx<var.nbr_dim;jdx++)
	mod_map_out[idx]*=lmt_mult[jdx]->dmn_cnt;

    /* Read coordinate dimensions if required */
    if(PRN_DMN_IDX_CRD_VAL){
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
	/* NB: nco_msa_rec_clc() with same nc_id contains OpenMP critical region */
	dim[idx].val.vp=nco_msa_rec_clc(0,1,lmt,lmt_mult+idx,&var_crd);

        /* Typecast pointer before use */
        (void)cast_void_nctype(dim[idx].type,&dim[idx].val);
      }/* end for */
    } /* end if */

    for(lmn=0;lmn<var.sz;lmn++){

      /* Caculate RAM indices from current limit */
      for(idx=0;idx <var.nbr_dim;idx++)
	dmn_sbs_ram[idx]=(lmn%mod_map_out[idx])/(idx == var.nbr_dim-1 ? 1L : mod_map_out[idx+1]);
      /* Calculate disk indices from RAM indices */
      (void)nco_msa_ram_2_dsk(dmn_sbs_ram,lmt_mult,var.nbr_dim,dmn_sbs_dsk,(lmn==var.sz-1));

      /* Find variable index relative to disk */
      var_dsk=0;
      for(idx=0;idx <var.nbr_dim;idx++)	var_dsk+=dmn_sbs_dsk[idx]*mod_map_in[idx];

      /* Skip rest of loop unless element is first in string */
      if(var.type == NC_CHAR && dmn_sbs_ram[var.nbr_dim-1] > 0) goto lbl_chr_prn;

      /* Print dimensions with indices along with values if they are coordinate variables */
      if(PRN_DMN_IDX_CRD_VAL){
        int dmn_idx;
        long dmn_sbs_prn;
	long crd_idx_crr;
	char dmn_sng[NCO_MAX_LEN_FMT_SNG];

	/* Loop over dimensions whose coordinates are to be printed */
	for(idx=0;idx<var.nbr_dim;idx++){

	  /* Reverse dimension ordering for Fortran convention */
	  if(FORTRAN_IDX_CNV) dmn_idx=var.nbr_dim-1-idx; else dmn_idx=idx;

	  /* Format and print dimension part of output string for non-coordinate variables */
	  if(dim[dmn_idx].cid == var.id) continue; /* If variable is a coordinate then skip printing until later */
	  if(!dim[dmn_idx].is_crd_dmn){ /* If dimension is not a coordinate... */
	    if(PRN_DMN_VAR_NM){
	      if(FORTRAN_IDX_CNV) (void)fprintf(stdout,"%s(%ld) ",dim[dmn_idx].nm,dmn_sbs_dsk[dmn_idx]+1L); else (void)fprintf(stdout,"%s[%ld] ",dim[dmn_idx].nm,dmn_sbs_dsk[dmn_idx]);
	    } /* !PRN_DMN_VAR_NM */
	    continue;
	  } /* end if */

	  if(PRN_DMN_VAR_NM) (void)sprintf(dmn_sng,"%%s[%%ld]=%s ",nco_typ_fmt_sng(dim[dmn_idx].type)); else (void)sprintf(dmn_sng,"%s ",nco_typ_fmt_sng(dim[dmn_idx].type));
          dmn_sbs_prn=dmn_sbs_dsk[dmn_idx];

	  if(FORTRAN_IDX_CNV){
            (void)nco_msa_c_2_f(dmn_sng);
	    dmn_sbs_prn++;
	  } /* end if */

	  /* Account for hyperslab offset in coordinate values*/
	  crd_idx_crr=dmn_sbs_ram[dmn_idx];
	  if(PRN_DMN_VAR_NM){
	    switch(dim[dmn_idx].type){
	    case NC_FLOAT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.fp[crd_idx_crr]); break;
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
	    } /* end switch */
	  }else{ /* !PRN_DMN_VAR_NM */
	    switch(dim[dmn_idx].type){
	    case NC_FLOAT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.fp[crd_idx_crr]); break;
	    case NC_DOUBLE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.dp[crd_idx_crr]); break;
	    case NC_SHORT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.sp[crd_idx_crr]); break;
	    case NC_INT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.ip[crd_idx_crr]); break;
	    case NC_CHAR: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.cp[crd_idx_crr]); break;
	    case NC_BYTE: (void)fprintf(stdout,dmn_sng,(unsigned char)dim[dmn_idx].val.bp[crd_idx_crr]); break;
	    case NC_UBYTE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.ubp[crd_idx_crr]); break;
	    case NC_USHORT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.usp[crd_idx_crr]); break;
	    case NC_UINT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.uip[crd_idx_crr]); break;
	    case NC_INT64: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.i64p[crd_idx_crr]); break;
	    case NC_UINT64: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.ui64p[crd_idx_crr]); break;
	    case NC_STRING: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.sngp[crd_idx_crr]); break;
	    default: nco_dfl_case_nc_type_err(); break;
	    } /* end switch */
	  } /* !PRN_DMN_VAR_NM */
	} /* end loop over dimensions */
      } /* end if PRN_DMN_IDX_CRD_VAL */

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
	    (void)sprintf(var_sng,"%%s[%%ld--%%ld]=\"%%s\" %%s");
          }else{
            (void)sprintf(var_sng,"%%s[%%ld--%%ld]='%%s' %%s");
	    prn_sng[chr_cnt]='\0';
	    var_dsk_end=var_dsk;
          } /* end if */
          if(FORTRAN_IDX_CNV){
	    (void)nco_msa_c_2_f(var_sng);
	    var_dsk_srt++;
	    var_dsk_end++;
          } /* end if */
	  (void)fprintf(stdout,var_sng,var_nm,var_dsk_srt,var_dsk_end,prn_sng,unit_sng);
	  (void)fprintf(stdout,"\n");
	  (void)fflush(stdout);
	  (void)nco_free(prn_sng);
        } /* endif */
	continue;
      } /* end if NC_CHAR */

      /* Print variable name, index, and value */
      if(PRN_DMN_VAR_NM) (void)sprintf(var_sng,"%%s[%%ld]=%s %%s\n",nco_typ_fmt_sng(var.type)); else (void)sprintf(var_sng,"%s\n",nco_typ_fmt_sng(var.type));
      if(FORTRAN_IDX_CNV){
	(void)nco_msa_c_2_f(var_sng);
	var_dsk++;
      } /* end if FORTRAN_IDX_CNV */

      if(PRN_DMN_VAR_NM){
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
	} /* end switch */
      }else{ /* !PRN_DMN_VAR_NM */
	switch(var.type){
	case NC_FLOAT: (void)fprintf(stdout,var_sng,var.val.fp[lmn],unit_sng); break;
	case NC_DOUBLE: (void)fprintf(stdout,var_sng,var.val.dp[lmn],unit_sng); break;
	case NC_SHORT: (void)fprintf(stdout,var_sng,var.val.sp[lmn],unit_sng); break;
	case NC_INT: (void)fprintf(stdout,var_sng,var.val.ip[lmn],unit_sng); break;
	case NC_CHAR: (void)fprintf(stdout,var_sng,var.val.cp[lmn],unit_sng); break;
	case NC_BYTE: (void)fprintf(stdout,var_sng,(unsigned char)var.val.bp[lmn],unit_sng); break;
	case NC_UBYTE: (void)fprintf(stdout,var_sng,var.val.ubp[lmn],unit_sng); break;
	case NC_USHORT: (void)fprintf(stdout,var_sng,var.val.usp[lmn],unit_sng); break;
	case NC_UINT: (void)fprintf(stdout,var_sng,var.val.uip[lmn],unit_sng); break;
	case NC_INT64: (void)fprintf(stdout,var_sng,var.val.i64p[lmn],unit_sng); break;
	case NC_UINT64: (void)fprintf(stdout,var_sng,var.val.ui64p[lmn],unit_sng); break;
	case NC_STRING: (void)fprintf(stdout,var_sng,var.val.sngp[lmn],unit_sng); break;
	default: nco_dfl_case_nc_type_err(); break;
	} /* end switch */
      } /* !PRN_DMN_VAR_NM */
    } /* end loop over elements */

    (void)nco_free(mod_map_in);
    (void)nco_free(mod_map_out);
    (void)nco_free(dmn_sbs_ram);
    (void)nco_free(dmn_sbs_dsk);

    /* Additional newline between consecutive variables or final variable and prompt */
    (void)fprintf(stdout,"\n");
    (void)fflush(stdout);
  } /* end if variable has more than one dimension */
  var.val.vp=nco_free(var.val.vp);
  var.nm=(char *)nco_free(var.nm);

  if(MALLOC_UNITS_SNG) unit_sng=(char *)nco_free(unit_sng);
  if(var.nbr_dim > 0){
    (void)nco_free(dmn_id);
    (void)nco_free(lmt_mult);
    (void)nco_free(lmt);
  } /* end if */
  if(PRN_DMN_IDX_CRD_VAL && dlm_sng==NULL){
    for(idx=0;idx<var.nbr_dim;idx++)
      dim[idx].val.vp=nco_free(dim[idx].val.vp);

    dim=(dmn_sct *)nco_free(dim);
  } /* end if */

} /* end nco_msa_prn_var_val() */

void         /* Initilaize lmt_all_sct's */
nco_msa_lmt_all_int
(int in_id,
 nco_bool MSA_USR_RDR,
 lmt_all_sct **lmt_all_lst,
 int nbr_dmn_fl,
 lmt_sct** lmt,
 int lmt_nbr)
{
  int idx;
  int jdx;
  int rec_dmn_id=NCO_REC_DMN_UNDEFINED;
  long dmn_sz;
  char dmn_nm[NC_MAX_NAME];

  lmt_sct *lmt_rgl;
  lmt_all_sct * lmt_all_crr;

  (void)nco_inq(in_id,(int*)NULL,(int*)NULL,(int *)NULL,&rec_dmn_id);

  /* Unlimited dimensions are stored in */
  // lmt_rgl=(lmt_sct **)nco_malloc(nbr_dmn_fl*sizeof(lmt_sct*));

  for(idx=0;idx<nbr_dmn_fl;idx++){
    (void)nco_inq_dim(in_id,idx,dmn_nm,&dmn_sz);
    lmt_all_crr=lmt_all_lst[idx]=(lmt_all_sct *)nco_malloc(sizeof(lmt_all_sct));
    lmt_all_crr->lmt_dmn=(lmt_sct **)nco_malloc(sizeof(lmt_sct *));
    lmt_all_crr->dmn_nm=strdup(dmn_nm);
    lmt_all_crr->lmt_dmn_nbr=1;
    lmt_all_crr->dmn_sz_org=dmn_sz;
    lmt_all_crr->WRP=False;
    lmt_all_crr->BASIC_DMN=True;
    lmt_all_crr->MSA_USR_RDR=False;

    lmt_all_crr->lmt_dmn[0]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));
    /* Dereference */
    lmt_rgl=lmt_all_crr->lmt_dmn[0];
    lmt_rgl->nm=strdup(lmt_all_crr->dmn_nm);
    lmt_rgl->id=idx;

    /* NB: nco_lmt_evl() may alter this */
    if(idx==rec_dmn_id) lmt_rgl->is_rec_dmn=True; else lmt_rgl->is_rec_dmn=False;

    lmt_rgl->srt=0L;
    lmt_rgl->end=dmn_sz-1L;
    lmt_rgl->cnt=dmn_sz;
    lmt_rgl->srd=1L;
    lmt_rgl->min_sng=NULL;
    lmt_rgl->max_sng=NULL;
    lmt_rgl->srd_sng=NULL;
    lmt_rgl->re_bs_sng=NULL;
    lmt_rgl->origin=0.0;
    /* A hack so we know structure has been initialized */
    lmt_rgl->lmt_typ=-1;
  } /* end loop over dimensions */

  /* fxm: subroutine-ize this MSA code block for portability TODO nco926 */
  /* Add user specified limits lmt_all_lst */
  for(idx=0;idx<lmt_nbr;idx++){
    for(jdx=0;jdx<nbr_dmn_fl;jdx++) {
      if(!strcmp(lmt[idx]->nm,lmt_all_lst[jdx]->dmn_nm)){
	lmt_all_crr=lmt_all_lst[jdx];
	lmt_all_crr->BASIC_DMN=False;
	if(lmt_all_crr->lmt_dmn[0]->lmt_typ == -1) {
          /* free defualt limit set above structure first */
          lmt_all_crr->lmt_dmn[0]=(lmt_sct*)nco_lmt_free(lmt_all_crr->lmt_dmn[0]);
	  lmt_all_crr->lmt_dmn[0]=lmt[idx];
	}else{
	  lmt_all_crr->lmt_dmn=(lmt_sct**)nco_realloc(lmt_all_crr->lmt_dmn,((lmt_all_crr->lmt_dmn_nbr)+1)*sizeof(lmt_sct *));
	  lmt_all_crr->lmt_dmn[(lmt_all_crr->lmt_dmn_nbr)++]=lmt[idx];
	} /* endif */
	break;
      } /* end if */
    } /* end loop over dimensions */
    /* Dimension in limit not found */
    if(jdx == nbr_dmn_fl){
      (void)fprintf(stderr,"Unable to find limit dimension %s in list\n",lmt[idx]->nm);
      nco_exit(EXIT_FAILURE);
    } /* end if err */
  } /* end loop over idx */

  /* fxm: subroutine-ize this MSA code block for portability TODO nco926 */
  for(idx=0;idx<nbr_dmn_fl;idx++){
    nco_bool bovl;

	/* nb progs ncra/ncrcat only one limit for the record dimension so skip evaluation*/
	/* otherwise this messes up multi-file operation */
	if(lmt_all_lst[idx]->lmt_dmn[0]->is_rec_dmn && (prg_get()==ncra || prg_get()==ncrcat))
	  continue;

    /* Split-up wrapped limits */
    (void)nco_msa_wrp_splt(lmt_all_lst[idx]);

    /* NB: Wrapped hyperslabs are dimensions broken into the "wrong" order,
       e.g., from -d time,8,2 broken into -d time,8,9 -d time,0,2
       WRP flag set only when list contains dimensions split as above */
    if(lmt_all_lst[idx]->WRP == True){
      /* Find and store size of output dim */
      (void)nco_msa_clc_cnt(lmt_all_lst[idx]);
      continue;
    } /* endif */

    /* Single slab---no analysis needed */
    if(lmt_all_lst[idx]->lmt_dmn_nbr == 1){
      (void)nco_msa_clc_cnt(lmt_all_lst[idx]);
      continue;
    } /* endif */

    if(MSA_USR_RDR){
      lmt_all_lst[idx]->MSA_USR_RDR=True;
      /* Find and store size of output dimension */
      (void)nco_msa_clc_cnt(lmt_all_lst[idx]);
      continue;
    } /* endif */

    /* Sort limits */
    (void)nco_msa_qsort_srt(lmt_all_lst[idx]);
    /* Check for overlap */
    bovl=nco_msa_ovl(lmt_all_lst[idx]);
    if(bovl==False) lmt_all_lst[idx]->MSA_USR_RDR=True;

    /* Find and store size of output dimension */
    (void)nco_msa_clc_cnt(lmt_all_lst[idx]);
    if(dbg_lvl_get() >1 ) {
      if(bovl) (void)fprintf(stdout,"%s: dimension \"%s\" has overlapping hyperslabs\n",prg_nm_get(),lmt_all_lst[idx]->dmn_nm); else (void)fprintf(stdout,"%s: dimension \"%s\" has distinct hyperslabs\n",prg_nm_get(),lmt_all_lst[idx]->dmn_nm);
    } /* endif */

  } /* end idx */

} /* end nco_msa_lmt_all_int() */

void
nco_msa_var_val_cpy /* [fnc] Copy variables data from input to output file */
(const int in_id, /* I [enm] netCDF file ID */
 const int out_id, /* I [enm] netCDF output file ID */
 var_sct ** const var, /* I/O [sct] Variables to copy to output file */
 const int nbr_var,  /* I [nbr] Number of variables */
 lmt_all_sct * const * lmt_lst, /* I multi-hyperslab limits */
 int nbr_dmn_fl) /* I [nbr] Number of multi-hyperslab limits */
{
  /* NB: nco_msa_var_val_cpy() contains OpenMP critical region */
  /* Purpose: Copy every variable in input variable structure list
     from input file to output file. Only data (not metadata) are copied. */

  int idx;
  int jdx;
  int kdx;
  int nbr_dim;
  /* int dmn_idx; */
  /* long srd_prd=1L; */ /* [nbr] Product of strides */

  for(idx=0;idx<nbr_var;idx++){

    nbr_dim=var[idx]->nbr_dim;

  /* GET VARIABLE DATA */

  /* Deal with scalar var */
  if(nbr_dim==0){
   var[idx]->val.vp=nco_malloc(nco_typ_lng(var[idx]->type));
   (void)nco_get_var1(in_id,var[idx]->id,0L,var[idx]->val.vp,var[idx]->type);
  }else{
    lmt_all_sct **lmt_mult;
	lmt_sct **lmt;

    lmt_mult=(lmt_all_sct **)nco_malloc(nbr_dim*sizeof(lmt_all_sct *));
    lmt=(lmt_sct **)nco_malloc(nbr_dim*sizeof(lmt_sct *));

    /* Initialize lmt_mult with multi-limits from lmt_lst limits */
    for(jdx=0;jdx<nbr_dim;jdx++){
      for(kdx=0;kdx<nbr_dmn_fl;kdx++){
      if(!strcmp(var[idx]->dim[jdx]->nm,lmt_lst[kdx]->dmn_nm ) ){
	   lmt_mult[jdx]=lmt_lst[kdx];
        break;
       } /* end if */
      } /* end loop over jdx */
    } /* end idx */

    /* Call super-dooper recursive routine */
    /* NB: nco_msa_rec_clc() with same nc_id contains OpenMP critical region */
    var[idx]->val.vp=nco_msa_rec_clc(0,nbr_dim,lmt,lmt_mult,var[idx]);

    (void)nco_free(lmt_mult);
    (void)nco_free(lmt);
   } /* end else nbr_dim */

    /*(void)nco_msa_var_get(in_id,var[idx],lmt_lst,nbr_dmn_fl); */

	/* PUT VARIABLE DATA */

    if(var[idx]->nbr_dim == 0)
      nco_put_var1(out_id,var[idx]->xrf->id,var[idx]->xrf->srt,var[idx]->val.vp,var[idx]->type);
    else{ /* end if variable is scalar */
      if(var[idx]->sz > 0){ /* Do nothing for zero-size record variables */
	/* Is stride > 1? */
        /*
	for(dmn_idx=0;dmn_idx<var[idx]->nbr_dim;dmn_idx++) srd_prd*=var[idx]->srd[dmn_idx];

	if(srd_prd == 1L)
	  nco_put_vara(out_id,var[idx]->xrf->id,var[idx]->xrf->srt,var[idx]->xrf->cnt,var[idx]->val.vp,var[idx]->type);
	else
        */
	  (void)nco_put_varm(out_id,var[idx]->xrf->id,var[idx]->xrf->srt,var[idx]->xrf->cnt,var[idx]->xrf->srd,(long *)NULL,var[idx]->val.vp,var[idx]->type);

      } /* end if var_sz */
    } /* end if variable is an array */
    var[idx]->val.vp=var[idx]->xrf->val.vp=nco_free(var[idx]->val.vp);
  } /* end loop over idx */

} /* end nco_msa_var_val_cpy() */






