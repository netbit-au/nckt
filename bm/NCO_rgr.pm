package NCO_rgr;

# $Header: /cvsroot/nco/nco/bm/NCO_rgr.pm,v 1.100 2010/06/21 13:45:25 hmb Exp $

# Purpose: All REGRESSION tests for NCO operators
# BENCHMARKS are coded in "NCO_benchmarks.pm"

# NB: When adding tests, _be sure to use -O to overwrite files_
# Otherwise, script hangs waiting for interactive response to overwrite queries

require 5.6.1 or die "This script requires Perl version >= 5.6.1, stopped";
use English; # WCS96 p. 403 makes incomprehensible Perl errors sort of comprehensible
use Cwd 'abs_path';
use strict;

use NCO_bm qw(dbg_msg tst_run
	      $pfx_cmd $drc_dat @fl_mtd_sct $opr_sng_mpi $opr_nm $dsc_sng $prsrv_fl $nsr_xpc $srv_sd
	      );

require Exporter;
our @ISA = qw(Exporter);
our @EXPORT = qw (
		  tst_rgr
		  $fl_out $dodap $dbg_lvl $pfx_cmd $opr_sng_mpi $opr_nm $dsc_sng $prsrv_fl $nsr_xpc
		  $foo1_fl $foo_fl $foo_tst $fl_out_rgn $foo_avg_fl $foo_x_fl $foo_y_fl $foo_yx_fl
		  $foo_xy_fl  $foo_xymyx_fl $pth_rmt_scp_tst $omp_flg $nco_D_flg %NCO_RC
		  );
use vars qw(
	    $dodap $dbg_lvl $dsc_sng $dust_usr $fl_fmt $fl_pth $foo1_fl $foo2_fl $foo_avg_fl
	    $foo_fl $foo_tst $foo_x_fl $foo_xy_fl
	    $foo_xymyx_fl $foo_y_fl $foo_yx_fl $mpi_prc $nco_D_flg $localhostname
	    $nsr_xpc $omp_flg $opr_nm $opr_rgr_mpi $fl_out_rgn
	    $fl_out $pth_rmt_scp_tst $prsrv_fl @tst_cmd $USER %NCO_RC
	    );

sub tst_rgr {
# Tests are in alphabetical order by operator name
    
# The following tests are organized and laid out as follows:
# - $tst_cmd[] holds command lines for each operator being tested
#   the last 2 lines are the expected value and the serverside string, either:
#       "NO_SS" - No Serverside allowed or (all regr are NO_SS still)
#       "SS_OK" - OK to send it serverside. (has to be requested with '--serverside'
# - $dsc_sng still holds test description line
# - tst_run() is function which executes each test
    
    my $in_pth="../data";
    my $in_pth_arg="-p $in_pth";
    $prsrv_fl=0;
    
# csz++
# fxm: pass as arguments or use exporter/importer instead?
    *dbg_lvl=*main::dbg_lvl;
    *dodap=*main::dodap;
    *fl_fmt=*main::fl_fmt;
    *fl_out=*main::fl_out;
    *localhostname=*main::localhostname;
    *nco_D_flg=*main::nco_D_flg;
    *omp_flg=*main::omp_flg;
    
    NCO_bm::dbg_msg(1,"in package NCO_rgr, \$dbg_lvl = $dbg_lvl");
    NCO_bm::dbg_msg(1,"in package NCO_rgr, \$dodap = $dodap");
    NCO_bm::dbg_msg(1,"in package NCO_rgr, \$fl_fmt = $fl_fmt");
    NCO_bm::dbg_msg(1,"in package NCO_rgr, \$fl_out = $fl_out");
    NCO_bm::dbg_msg(1,"in package NCO_rgr, \$localhostname = $localhostname");
    NCO_bm::dbg_msg(1,"in package NCO_rgr, \$nco_D_flg = $nco_D_flg");
    NCO_bm::dbg_msg(1,"in package NCO_rgr, \$omp_flg = $omp_flg");
# csz--
    
# in general, $fl_out    -> %tempf_00%
#             $foo_fl     -> %tempf_01%
#             $foo_tst    -> %tempf_02%
#             $foo_avg_fl -> %tempf_03%
#             $foo1_fl    -> %tempf_01%
#             $foo2_fl    -> %tempf_02%
    
    if ($dodap ne "FALSE") {
	print "DEBUG: in tst_rgr(), \$dodap = $dodap \n";
	if ($dodap ne "" && $fl_pth =~ /http/ ) { $in_pth_arg = "-p $fl_pth"; }
	if ($dodap eq "") { $in_pth_arg = "-p http://dust.ess.uci.edu/cgi-bin/dods/nph-dods/dodsdata"; }
    }
    NCO_bm::dbg_msg(1,"-------------  REGRESSION TESTS STARTED from tst_rgr()  -------------");
    
    if (0) {} #################  SKIP THESE #####################
    
    
####################
#### ncap2 tests ####
####################
    $opr_nm='ncap2';
####################
    
# This stanza will not map to the way the SS is done - needs a %stdouterr% added but all the rest of them
# have an ncks which triggers this addition from the sub tst_run() -> gnarly_pything.
# this stanza also requires a script on the SS.
    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -S ncap2.in $in_pth_arg in.nc %tempf_00% %stdouterr%";
    $dsc_sng="running ncap2.in script in nco_bm.pl (failure expected on netcdf4 ncap81)";
    $tst_cmd[1]="ncks -C -H -v b2 -s '%d' %tempf_00%";
    $tst_cmd[2]="999";
#	$tst_cmd[3]="NO_SS";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
# printf("paused @ [%s:%d]  - hit return to continue\n", __FILE__, __LINE__); my $wait = <STDIN>;
    
    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -C -v -s 'tpt_mod=tpt%273.0f' $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -v tpt_mod -s '%.1f ' %tempf_00%";
    $dsc_sng="Testing float modulo float";
    $tst_cmd[2]="0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0 ";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
#printf("paused @ [%s:%d]  - hit return to continue\n", __FILE__, __LINE__); my $wait = <STDIN>;

    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -C -v -s 'foo=log(e_flt)^1' $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -v foo -s '%.6f\\n' %tempf_00%";
    $dsc_sng="Testing foo=log(e_flt)^1 (fails on AIX TODO ncap57)";
    $tst_cmd[2]="1.000000";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
#print "paused - hit return to continue"; my $wait = <STDIN>;
    
# where did e_dbl tst_run??  it's in in.cdl but gets lost thru the rgrs...?
    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -C -v -s 'foo=log(e_dbl)^1' $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%.12f\\n' %tempf_00%";
    $dsc_sng="Testing foo=log(e_dbl)^1";
    $tst_cmd[2]="1.000000000000";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -C -v -s 'foo=4*atan(1)' $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%.12f\\n' %tempf_00%";
    $dsc_sng="Testing foo=4*atan(1)";
    $tst_cmd[2]="3.141592741013";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -C -v -s 'foo=erf(1)' $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%.12f\\n' %tempf_00%";
    $dsc_sng="Testing foo=erf(1) (fails on AIX TODO ncap57)";
    $tst_cmd[2]="0.842701";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    #fails - wrong result ???
    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -C -v -s 'foo=gamma(0.5)' $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%.12f\\n' %tempf_00%";
    $dsc_sng="Testing foo=gamma(0.5) (fails on AIX TODO ncap57)";
    $tst_cmd[2]="1.772453851";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -C -v -s 'pi=4*atan(1);foo=sin(pi/2)' $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -v foo -s '%.12f\\n' %tempf_00%";
    $dsc_sng="Testing foo=sin(pi/2)";
    $tst_cmd[2]="1.000000000000";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -C -v -s 'pi=4*atan(1);foo=cos(pi)' $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -v foo -s '%.12f\\n' %tempf_00%";
    $dsc_sng="Testing foo=cos(pi)";
    $tst_cmd[2]="-1.000000000000";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array\
    
    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -C -v -s 'defdim(\"a\",3);defdim(\"b\",4); a[\$a,\$b]=10;c=a(1,1);' $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -v c -s '%i' %tempf_00%";
    $dsc_sng="Casting variable with same name as dimension (failure expected on netcdf4 ncap81)";
    ##TODO ncap81
    $tst_cmd[2]="10";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array

    
    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -C -v -s 'defdim(\"a\",1); b[\$a]=10;c=b(0:0);' $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncwa -h -O $fl_fmt $nco_D_flg -C  -a a %tempf_00% %tempf_01%";
    $tst_cmd[2]="ncks -C -H -v b -s '%i' %tempf_01%";
    $dsc_sng="Casting variable with a single dim of sz=1 ";
    ##TODO ncap81
    $tst_cmd[3]="10";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array




# printf("paused @ %s:%d  - hit return to continue", __FILE__ , __LINE__); my $wait=<STDIN>;
    
    if ($dodap eq "FALSE") {
####################
#### ncatted tests #
####################
	$opr_nm="ncatted";
####################
	# FAILS!
	$tst_cmd[0]="ncatted -h -O $nco_D_flg -a units,,m,c,'meter second-1' $in_pth_arg in.nc %tempf_00%";
	$tst_cmd[1]="ncks -C -m -v lev %tempf_00% | grep units | cut -d' ' -f 11-12"; ## daniel:fixme cut/ncks but how to do grep?
	$dsc_sng="Modify all existing units attributes to meter second-1";
	$tst_cmd[2]="meter second-1";
	$tst_cmd[3]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0;  # Reset array
	
#printf("paused @ %s:%d  - hit return to continue", __FILE__ , __LINE__); my $wait = <STDIN>;
	
	$tst_cmd[0]="ncatted -h -O $nco_D_flg -a _FillValue,val_one_mss,m,f,0.0 $in_pth_arg in.nc %tempf_00%";
	$tst_cmd[1]="ncks -C -H -s '%g' -d lat,1 -v val_one_mss %tempf_00%";
	$dsc_sng="Change _FillValue attribute from 1.0e36 to 0.0";
        ###TODO 665
	$tst_cmd[2]="0";
	$tst_cmd[3]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0;  # Reset array

	$tst_cmd[0]="ncatted -h -O $nco_D_flg -a _FillValue,wgt_one,c,f,200.0 $in_pth_arg in.nc %tempf_00%";
	$tst_cmd[1]="ncks -C -H -s '%g' -d lat,1 -v wgt_one %tempf_00%";
	$dsc_sng="Create new _FillValue attribute ";
	$tst_cmd[2]="1";
	$tst_cmd[3]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0;  # Reset array


	
# Fragile: This test fails when length of command changes
	$tst_cmd[0]="ncatted -O --hdr_pad=1000 $nco_D_flg -a missing_value,val_one_mss,m,f,0.0 $in_pth_arg in.nc %tempf_00%";
	$tst_cmd[1]="ncks -M %tempf_00% | grep hdr_pad | wc > %tempf_01%";
	$tst_cmd[2]="cut -c 14-15  %tempf_01%"; ## Daniel:fxm cut/ncks, but how to do grep and wc???
	$dsc_sng="Pad header with 1000B extra for future metadata (failure OK/expected since test depends on command-line length)";
	$tst_cmd[3]="27";
	$tst_cmd[4]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0;  # Reset array

	$tst_cmd[0]="ncatted -O $nco_D_flg -a nw1,'^three*',c,i,999  $in_pth_arg in.nc %tempf_00%";
	$tst_cmd[1]="ncap2 -v -C -O -s 'n2=three_dmn_var_int\@nw1;' %tempf_00% %tempf_01%";
	$tst_cmd[2]="ncks -O -C -H -s '%i'  -v n2 %tempf_01%";
	$dsc_sng="Check ncatted variable wildcarding -(Failure expected when NOT built with regex)";
	$tst_cmd[3]="999";
	$tst_cmd[4]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0;  # Reset array
    }
    
# printf("paused @ %s:%d  - hit return to continue", __FILE__ , __LINE__); my $wait = <STDIN>;
    
####################
#### ncbo tests ####
####################
    $opr_nm="ncbo";
####################
#if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_rgr_mpi =~ /$opr_nm/)) {
    $tst_cmd[0]="ncbo $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_dbl_mss_val_dbl_pck $in_pth_arg in.nc in.nc %tempf_00%";;
    $tst_cmd[1]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg %tempf_00% %tempf_03%";;
    $tst_cmd[2]="ncks -C -H -s '%g' -v rec_var_dbl_mss_val_dbl_pck %tempf_03%";
    $dsc_sng="Difference a packed variable and be sure everything is zero or _FillValue by average whole remainder and comparing to zero.) FAILS with netcdf4";
    $tst_cmd[3]="0.0";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array - ok

    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -a time $in_pth_arg in.nc %tempf_03%";;
    $tst_cmd[1]="ncbo $omp_flg -h -O $fl_fmt $nco_D_flg $in_pth/in.nc %tempf_03% %tempf_00%";;
    $tst_cmd[2]="ncks -C -H -d time,9 -s '%g' -v rec_var_flt %tempf_00%";
    $dsc_sng="Whole file difference with broadcasting (OK to fail here fxm TODO nco757. Works with --mmr_drt, triggers segfault on OSs like FC Linux which have C-library armor. Harmless Failure caused by free'ing dangling pointer during memory cleanup.)";
    $tst_cmd[3]="4.5";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array - ok

    $tst_cmd[0]="ncbo $omp_flg -h -O $fl_fmt $nco_D_flg --op_typ='-' -v mss_val_scl $in_pth_arg in.nc in.nc %tempf_00%";;
    $tst_cmd[1]="ncks -C -H -s '%g' -v mss_val_scl %tempf_00%";
    $dsc_sng="difference scalar missing value";
    $tst_cmd[2]="1.0e36";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array - ok
    
    $tst_cmd[0]="ncbo $omp_flg -h -O $fl_fmt $nco_D_flg --op_typ='-' -d lon,1 -v mss_val $in_pth_arg in.nc in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%g' -v mss_val %tempf_00%";
    $dsc_sng="difference with missing value attribute";
    $tst_cmd[2]=1.0e36;
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array ok
    
    $tst_cmd[0]="ncbo $omp_flg -h -O $fl_fmt $nco_D_flg --op_typ='-' -d lon,0 -v no_mss_val $in_pth_arg in.nc in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v no_mss_val %tempf_00%";
    $dsc_sng="difference without missing value attribute";
    $tst_cmd[2]="0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array ok
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -v mss_val_fst $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncrename -h -O $nco_D_flg -v mss_val_fst,mss_val %tempf_00%";
    $tst_cmd[2]="ncbo $omp_flg -h -O $fl_fmt $nco_D_flg -y '-' -v mss_val %tempf_00% ../data/in.nc %tempf_01% 2> %tempf_02%";
    $tst_cmd[3]="ncks -C -H -s '%f,' -v mss_val %tempf_01%";
    $dsc_sng="_FillValues differ between files";
    $tst_cmd[4]="-999.000000,-999.000000,-999.000000,-999.000000";
    $tst_cmd[5]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array ok
    
    $tst_cmd[0]="ncrename -h $nco_D_flg -O -v no_mss_val,one_dmn_arr_with_and_without_mss_val $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncrename -h $nco_D_flg -O -v mss_val,one_dmn_arr_with_and_without_mss_val $in_pth_arg in.nc %tempf_01%";
    $tst_cmd[2]="ncbo $omp_flg  -h -O $fl_fmt $nco_D_flg -y '-' -v one_dmn_arr_with_and_without_mss_val %tempf_00% %tempf_01% %tempf_02% 2> %tempf_03%";
    $tst_cmd[3]="ncks -C -H -d lon,0 -s '%f' -v one_dmn_arr_with_and_without_mss_val %tempf_02%";
    $dsc_sng="_FillValue in one variable (not both variables)";
    $tst_cmd[4]=0.0;
    $tst_cmd[5]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array ok
    
    $tst_cmd[0]="ncdiff $omp_flg -h -O $fl_fmt $nco_D_flg -d lon,1 -v mss_val $in_pth_arg in.nc in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%g' -v mss_val %tempf_00%";
    $dsc_sng="ncdiff symbolically linked to ncbo";
    $tst_cmd[2]=1.0e36;
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array ok
    
    $tst_cmd[0]="ncdiff $omp_flg -h -O $fl_fmt $nco_D_flg -d lon,1 -v mss_val $in_pth_arg in.nc in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%g' -v mss_val %tempf_00%";
    $dsc_sng="difference with missing value attribute";
    $tst_cmd[2]=1.0e36;
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array ok
    
    $tst_cmd[0]="ncdiff $omp_flg -h -O $fl_fmt $nco_D_flg -d lon,0 -v no_mss_val $in_pth_arg in.nc in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v no_mss_val %tempf_00%";
    $dsc_sng="Difference without missing value attribute";
    $tst_cmd[2]="0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array ok
    
    $tst_cmd[0]="ncwa $omp_flg -C -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc %tempf_03%";
    $tst_cmd[1]="ncbo $omp_flg -C -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_dbl $in_pth/in.nc %tempf_03% %tempf_00%";
    $tst_cmd[2]="ncks -C -H -d time,3 -s '%f' -v rec_var_flt_mss_val_dbl %tempf_00%";
    $dsc_sng="Difference which tests broadcasting and changing variable IDs";
    $tst_cmd[3]="-1.0";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array

    
    $tst_cmd[0]="ncks -C -O $fl_fmt $nco_D_flg -v three_dmn_var_dbl $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncap2 -C -O $fl_fmt $nco_D_flg -v -s 'three_dmn_var_dbl[lon]={0.0,1,2,3};' $in_pth_arg in.nc %tempf_01%";
    $tst_cmd[2]="ncbo $omp_flg -C -h -O $fl_fmt $nco_D_flg  %tempf_00% %tempf_01% %tempf_02%";
    $tst_cmd[3]="ncwa $omp_flg -C -h -O $fl_fmt $nco_D_flg -y ttl -v three_dmn_var_dbl %tempf_02% %tempf_03%";
    $tst_cmd[4]="ncks -C -H  -s '%f' -v three_dmn_var_dbl %tempf_03%";
    $dsc_sng="Difference which tests 3D broadcasting";
    $tst_cmd[5]="2697";
    $tst_cmd[6]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
	
	
    $tst_cmd[0]="ncks -C -O $fl_fmt $nco_D_flg -v three_dmn_var_dbl $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncap2 -C -O $fl_fmt $nco_D_flg -v -s 'three_dmn_var_dbl[lon]={0.0,1,2,3};' $in_pth_arg in.nc %tempf_01%";
    $tst_cmd[2]="ncbo $omp_flg -C -h -O $fl_fmt $nco_D_flg -d time,0,4 -d time,9 -d lon,0 -d lon,3 %tempf_00% %tempf_01% %tempf_02%";
    $tst_cmd[3]="ncwa $omp_flg -C -h -O $fl_fmt $nco_D_flg -y ttl -v three_dmn_var_dbl %tempf_02% %tempf_03%";
    $tst_cmd[4]="ncks -C -H  -s '%f' -v three_dmn_var_dbl %tempf_03%";
    $dsc_sng="Addition + 3D broadcasting+ MSA";
    $tst_cmd[5]="422";
    $tst_cmd[6]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array


    
#} # endif $mpi_prc == 0...
    
#printf("paused @ %s:%d  - hit return to continue", __FILE__ , __LINE__); my $wait=<STDIN>;
    
####################
#### ncea tests #### - OK !
####################
    $opr_nm='ncea';
####################
    
    $tst_cmd[0]="ncra -Y ncea $omp_flg -h -O $fl_fmt $nco_D_flg -v one_dmn_rec_var -d time,4 $in_pth_arg in.nc in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v one_dmn_rec_var %tempf_00%";
    $dsc_sng="ensemble mean of int across two files";
    $tst_cmd[2]="5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncra -Y ncea $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_flt -d time,0 $in_pth_arg in.nc in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%g' -v rec_var_flt_mss_val_flt %tempf_00%";
    $dsc_sng="ensemble mean with missing values across two files";
    $tst_cmd[2]="1.0e36";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="/bin/rm -f %tempf_00%";
    $tst_cmd[1]="ncra -Y ncea $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v rec_var_flt_mss_val_dbl -d time,1 $in_pth_arg in.nc in.nc %tempf_00%";
    $tst_cmd[2]="ncks -C -H -s '%e' -v rec_var_flt_mss_val_dbl %tempf_00%";
    $dsc_sng="ensemble min of float across two files";
    $tst_cmd[3]="2";
    $tst_cmd[4]="NO_SS";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="/bin/rm -f %tempf_00%";
    $tst_cmd[1]="ncra -Y ncea $omp_flg -h -O $fl_fmt $nco_D_flg -C -v pck $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[2]="ncks -C -H -s '%e' -v pck %tempf_00%";
    $dsc_sng="scale factor + add_offset packing/unpacking";
    $tst_cmd[3]="3";
    $tst_cmd[4]="NO_SS";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="/bin/rm -f %tempf_00%";
    $tst_cmd[1]="ncra -Y ncea $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_int_mss_val_int $in_pth_arg in.nc in.nc %tempf_00%";
    $tst_cmd[2]="ncks -C -H -s '%d ' -v rec_var_int_mss_val_int %tempf_00%";
    $dsc_sng="ensemble mean of integer with integer missing values across two files";
    $tst_cmd[3]="-999 2 3 4 5 6 7 8 -999 -999";
    $tst_cmd[4]="NO_SS";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
	
    $tst_cmd[0]="/bin/rm -f %tempf_00%";
    $tst_cmd[1]="ncra -Y ncea $omp_flg -h -O $fl_fmt $nco_D_flg -C -d time,0,2 -d lon,0 -d lon,3 -v three_dmn_var_dbl  $in_pth_arg in.nc in.nc %tempf_00%";
    $tst_cmd[2]="ncwa $omp_flg -C -h -O $fl_fmt $nco_D_flg -y ttl -v three_dmn_var_dbl %tempf_00% %tempf_01%";
    $tst_cmd[3]="ncks -C -H -s '%3.f' -v three_dmn_var_dbl %tempf_01%";
    $dsc_sng="ensemble mean of 3D variable across two files with MSA";
    $tst_cmd[4]="150";
    $tst_cmd[5]="NO_SS";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
	
    $tst_cmd[0]="/bin/rm -f %tempf_00%";
    $tst_cmd[1]="ncra -Y ncea $omp_flg -h -O $fl_fmt $nco_D_flg -C $in_pth_arg lrg_bm.nc lrg_bm1.nc %tempf_00%";
    $tst_cmd[2]="ncra -Y ncea -t1 -h -O $fl_fmt $nco_D_flg -C $in_pth_arg lrg_bm.nc lrg_bm1.nc %tempf_01%";
    $tst_cmd[3]="ncbo -C -h -O   %tempf_00% %tempf_01% %tempf_02%";
    $tst_cmd[4]="ncwa  -t1 -O -C  %tempf_02% %tempf_03% 2>%tempf_05%";
    $dsc_sng="Check op with OpenMP";
#    $tst_cmd[5]="ncks -C -H -s '%d' -v R %tempf_03%";
    $tst_cmd[5]="ncap2 -t1 -h -v -O -s 'n2= ( (fabs(R)<0.01) && (fabs(S)<0.01) && (fabs(T)<0.01) && (fabs(U)<0.01) && (fabs(V)<0.01) && (fabs(W)<0.01) && (fabs(X)<0.01) );print(n2)' %tempf_03% %tempf_04%";
    $tst_cmd[6]="n2 = 1";
    $tst_cmd[7]="NO_SS";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
# print "paused - hit return to continue"; my $wait=<STDIN>;
    
####################
## ncecat tests #### OK !
####################
    $opr_nm='ncecat';
####################
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -v one $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -v one $in_pth_arg in.nc %tempf_01%";
    $tst_cmd[2]="ncecat $omp_flg -h -O $fl_fmt $nco_D_flg %tempf_00% %tempf_01% %tempf_02%";
    $tst_cmd[3]="ncks -C -H -s '%6.3f, ' -v one %tempf_02%";
    $dsc_sng="concatenate two files containing only scalar variables";
    $tst_cmd[4]=" 1.000, "; # is this effectively equal to the previous " 1.000,  1.000, "
    $tst_cmd[5]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array

    $tst_cmd[0]="ncks -C -h -O $fl_fmt $nco_D_flg -v three_dmn_var_int $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncap2 -C -v -O  $fl_fmt $nco_D_flg -s 'three_dmn_var_int+=100;' $in_pth_arg in.nc %tempf_01%";
    $tst_cmd[2]="ncecat -C -h -O $omp_flg  $fl_fmt $nco_D_flg -d time,0,3 -d time,8,9 -d lon,0,1 -d lon,3,3 -v three_dmn_var_int %tempf_00% %tempf_01% %tempf_02%";
    $tst_cmd[3]="ncwa -C -h -O  $omp_flg $fl_fmt $nco_D_flg -y avg -v three_dmn_var_int %tempf_02% %tempf_03%";
    $tst_cmd[4]="ncks -C -O -H -s '%d' -v three_dmn_var_int  %tempf_03%";
    $dsc_sng="concatenate two 3D vars with multihyperslabbing";
    $tst_cmd[5]="84"; 
    $tst_cmd[6]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array

    
#print "paused - hit return to continue"; my $wait=<STDIN>;
    
#####################
## ncflint tests #### OK !
#####################
    $opr_nm='ncflint';
####################
    $tst_cmd[0]="ncflint $omp_flg -h -O $fl_fmt $nco_D_flg -w 3,-2 -v one $in_pth_arg in.nc in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%e' -v one %tempf_00%";
    $dsc_sng="identity weighting";
    $tst_cmd[2]="1.0";
    $tst_cmd[3]="NO_SS";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    if ($dodap eq "FALSE"){
	$tst_cmd[0]="ncrename -h -O $nco_D_flg -v zero,foo $in_pth_arg in.nc %tempf_01%";
	$tst_cmd[1]="ncrename -h -O $nco_D_flg -v one,foo $in_pth_arg in.nc %tempf_00%";
	$tst_cmd[2]="ncflint $omp_flg -h -O $fl_fmt $nco_D_flg -i foo,0.5 -v two %tempf_01% %tempf_00% %tempf_02%";
	$tst_cmd[3]="ncks -C -H -s '%e' -v two %tempf_02%";
	$dsc_sng="identity interpolation";
	$tst_cmd[4]="2.0";
	$tst_cmd[5]="NO_SS";
	$tst_cmd[5]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0;  # Reset array
    }
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -d lon,1 -v mss_val $in_pth_arg in.nc %tempf_01%";
    $tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -C -d lon,0 -v mss_val $in_pth_arg in.nc %tempf_02%";
    $tst_cmd[2]="ncflint $omp_flg -h -O $fl_fmt $nco_D_flg -w 0.5,0.5 %tempf_01% %tempf_02% %tempf_03%";
    $tst_cmd[3]="ncflint $omp_flg -h -O $fl_fmt $nco_D_flg -w 0.5,0.5  %tempf_02% %tempf_01%  %tempf_04%  $foo_y_fl $foo_x_fl $foo_yx_fl";
    $tst_cmd[4]="ncdiff $omp_flg -h -O $fl_fmt $nco_D_flg %tempf_03% %tempf_04% %tempf_05%";
    $tst_cmd[5]="ncks -C -H -s '%g' -v mss_val %tempf_05% ";
    $dsc_sng="switch order of occurrence to test for commutivity";
    $tst_cmd[6]="1e+36";
    $tst_cmd[7]="NO_SS";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    
####################
#### ncks tests #### OK !
####################
    $opr_nm='ncks';
####################
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -v lat_T42,lon_T42,gw_T42 $in_pth_arg in.nc %tempf_03%";
    $tst_cmd[1]="ncrename -h -O $nco_D_flg -d lat_T42,lat -d lon_T42,lon -v lat_T42,lat -v gw_T42,gw -v lon_T42,lon %tempf_03%";
    $tst_cmd[2]="ncap2 -h -O $fl_fmt $nco_D_flg -s 'one[lat,lon]=lat*lon*0.0+1.0' -s 'zero[lat,lon]=lat*lon*0.0' %tempf_03% %tempf_04%";
    $tst_cmd[3]="ncks -C -H -s '%g' -v one -F -d lon,128 -d lat,64 %tempf_04% ";
    $dsc_sng="Create T42 variable named one, uniformly 1.0 over globe in %tempf_03%. (FAILURE with NETCDF4 ncrename nco821)";
    $tst_cmd[4]=1;
    $tst_cmd[5]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    #passes, but returned string includes tailing NULLS (<nul> in nedit)
    $tst_cmd[0]="ncks -C -H -s '%c' -v fl_nm $in_pth_arg in.nc";
    $dsc_sng="extract filename string";
    $tst_cmd[1]="/home/zender/nco/data/in.cdl";
    $tst_cmd[2]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -v lev $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%f,' -v lev %tempf_00%";
    $dsc_sng="extract a dimension";
    $tst_cmd[2]="100.000000,500.000000,1000.000000";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -v three_dmn_var $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v three_dmn_var -d lat,1,1 -d lev,2,2 -d lon,3,3 %tempf_00%";
    $dsc_sng="extract a variable with limits";
    $tst_cmd[2]="23";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -v int_var $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v int_var %tempf_00%";
    $dsc_sng="extract variable of type NC_INT";
    $tst_cmd[2]="10";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v three_dmn_var -d lat,1,1 -d lev,0,0 -d lev,2,2 -d lon,0,,2 -d lon,1,,2 $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%4.1f,' -v three_dmn_var %tempf_00%";
    $dsc_sng="Multi-slab lat and lon with srd";
    $tst_cmd[2]="12.0,13.0,14.0,15.0,20.0,21.0,22.0,23.0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v three_dmn_var -d lat,1,1 -d lev,2,2 -d lon,0,3 -d lon,1,3 $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%4.1f,' -v three_dmn_var %tempf_00%";
    $dsc_sng="Multi-slab with redundant hyperslabs";
    $tst_cmd[2]="20.0,21.0,22.0,23.0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v three_dmn_var -d lat,1,1 -d lev,2,2 -d lon,0.,,2 -d lon,90.,,2 $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%4.1f,' -v three_dmn_var %tempf_00%";
    $dsc_sng="Multi-slab with coordinates";
    $tst_cmd[2]="20.0,21.0,22.0,23.0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v three_dmn_var -d lat,1,1 -d lev,800.,200. -d lon,270.,0. $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%4.1f,' -v three_dmn_var %tempf_00%";
    $dsc_sng="Double-wrapped hyperslab";
    $tst_cmd[2]="23.0,20.0,15.0,12.0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -d time_udunits,'1999-12-08 12:00:0.0','1999-12-09 00:00:0.0' $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%6.0f' -d time_udunits,'1999-12-08 18:00:0.0','1999-12-09 12:00:0.0',2 -v time_udunits $in_pth_arg in.nc";
    $dsc_sng="dimension slice using UDUnits library (fails without UDUnits library support)";
    $tst_cmd[2]="876018";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    

    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v lon_cal -d lon_cal,'1964-3-1 0:00:0.0','1964-3-4 00:00:0.0' $in_pth_arg in.nc %tempf_00%";

    $tst_cmd[1]="ncap2 -O -v -C -s 'lon_cal_ttl=lon_cal.total();print(lon_cal_ttl)' %tempf_00% %tempf_01%";
    $tst_cmd[2]="lon_cal_ttl = 10";

    $dsc_sng="dim slice using UDUnits library and cal 365_days (fails without UDUnits library support)";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array


    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v lat_cal -d lat_cal,'1964-3-1 0:00:0.0','1964-3-4 00:00:0.0' $in_pth_arg in.nc %tempf_00%";

    $tst_cmd[1]="ncap2 -O -v -C -s 'lat_cal_ttl=lat_cal.total();print(lat_cal_ttl)' %tempf_00% %tempf_01%";
    $tst_cmd[2]="lat_cal_ttl = 18";

    $dsc_sng="dim slice using UDUnits library and cal 360_days (fails without UDUnits library support)";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    


    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -H -v wvl -d wvl,'0.4 micron','0.7 micron' -s '%3.1e' $in_pth_arg in.nc";
    $dsc_sng="dimension slice using UDUnit conversion (fails without UDUnits library support)";
    $tst_cmd[1]="1.0e-06";
    $tst_cmd[2]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    #fails
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v '^three_*' $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -C -v three %tempf_00%";
    $dsc_sng="variable wildcards A (fails without regex library)";
    $tst_cmd[2]="3";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v '^[a-z]{3}_[a-z]{3}_[a-z]{3,}\$' $in_pth_arg in.nc %tempf_00%";
    # for this test, the regex is mod'ed                       ^
    $tst_cmd[1]="ncks -C -H -s '%d' -C -v val_one_int %tempf_00%";
    $dsc_sng="variable wildcards B (fails without regex library)";
    $tst_cmd[2]="1";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -d time,0,1 -v time $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%g' -C -d time,2, %tempf_00%";
    $dsc_sng="Offset past end of file (OK. TODO nco693. ncks behaves perfectly here. Unfortunately, the perl test script does not recognize the expected answer string as being valid)";
    $tst_cmd[2]="ncks: ERROR User-specified dimension index range 2 <= time <= 1 does not fall within valid dimension index range 0 <= time <= 1";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncks -C -H -s '%d' -v byte_var $in_pth_arg in.nc";
    $dsc_sng="Print byte value";
    $tst_cmd[1]="122";
    $tst_cmd[2]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array

    $tst_cmd[0]="ncks -O -v cnv_CF_crd $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%g' -v lon_gds -d gds_crd,2 %tempf_00%";
    $dsc_sng="CF convention coordinates attribute";
    $tst_cmd[2]="180";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array

    $tst_cmd[0]="ncatted -O -a _FillValue,global,c,l,222 $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -O %tempf_00% %tempf_01%";
    $tst_cmd[2]="ncap2 -v -O -s 'n2=global\@_FillValue;' %tempf_01% %tempf_02%";
    $tst_cmd[3]="ncks -C -H -s '%d' -v n2 %tempf_02%";
    $dsc_sng="Check creation/copying of global _FillValue";
    $tst_cmd[4]="222";
    $tst_cmd[5]="SS_OK";

    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array

#####################
#### ncpdq tests #### -OK !
#####################
    $opr_nm='ncpdq';
####################
    
    $tst_cmd[0]="ncpdq $omp_flg -h -O $fl_fmt $nco_D_flg -a -lat -v lat $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%g' -v lat -d lat,0 %tempf_00%";
    $dsc_sng="reverse coordinate";
    $tst_cmd[2]="90";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncpdq $omp_flg -h -O $fl_fmt $nco_D_flg -a -lat,-lev,-lon -v three_dmn_var $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v three_dmn_var -d lat,0 -d lev,0 -d lon,0 %tempf_00%";
    $dsc_sng="reverse three dimensional variable";
    $tst_cmd[2]=23;
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncpdq $omp_flg -h -O $fl_fmt $nco_D_flg -a lon,lat -v three_dmn_var $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v three_dmn_var -d lat,0 -d lev,2 -d lon,3 %tempf_00%";
    $dsc_sng="re-order three dimensional variable";
    $tst_cmd[2]="11";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncpdq $omp_flg -h -O -C  $fl_fmt $nco_D_flg -a lat,lon,time -v three_dmn_var_dbl -d time,0,3 -d time,9,9 -d lon,0,0 -d lon,3,3 $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v three_dmn_var_dbl -d lat,0 -d lon,1 -d time,2 %tempf_00%";
    $dsc_sng="re-order 3D variable with multihyperslabbing";
    $tst_cmd[2]="20";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0;  # Reset array

    $tst_cmd[0]="ncpdq $omp_flg -h -O -C  $fl_fmt $nco_D_flg -a lat,lon,-time -v three_dmn_var_dbl -d time,0,3 -d time,9,9 -d lon,0,0 -d lon,3,3 $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v three_dmn_var_dbl -d lat,1 -d lon,1 -d time,4 %tempf_00%";
    $dsc_sng="re-order 3D variable with MSA+ reversal of time dim";
    $tst_cmd[2]="8";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0;  # Reset array

     $tst_cmd[0]="ncpdq $omp_flg -h -O -C  $fl_fmt $nco_D_flg -a lon,lat -v three_dmn_var_dbl -d time,0,2 -d time,4 -d lat,1 -d lat,1 --msa_usr_rdr $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%2.f,' -v three_dmn_var_dbl -d time,1 -d lon,0  %tempf_00%";
    $dsc_sng="re-order 3D variable with MSA and --msa_usr_rdr flag";
    $tst_cmd[2]="13,13";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0;  # Reset array

    $tst_cmd[0]="ncpdq $omp_flg -h -O $fl_fmt $nco_D_flg -P all_new -v upk $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncpdq $omp_flg -h -O $fl_fmt $nco_D_flg -P upk -v upk %tempf_00% %tempf_00%";
    $tst_cmd[2]="ncks -C -H -s '%g' -v upk %tempf_00%";
    $dsc_sng="Pack and then unpack scalar (uses only add_offset)";
    $tst_cmd[3]="3";
    $tst_cmd[4]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0;  # Reset array

    $tst_cmd[0]="ncpdq $omp_flg -h -O -C  $fl_fmt $nco_D_flg -P upk -v rec_var_dbl_mss_val_dbl_pck -d time,0,4  -d time,6  $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_dbl_mss_val_dbl_pck -d time,5 %tempf_00%";
    $dsc_sng="unpack 1D variable with MSA (failure expected with NETCDF4 TODO nco772)";
    $tst_cmd[2]="7";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0;  # Reset array


    $tst_cmd[0]="ncpdq $omp_flg -h -O -C $fl_fmt $nco_D_flg -P all_xst -v three_dmn_var_dbl -d time,0,2 -d time,8,9 -d lon,0 -d lon,1 -d lat,1 $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%i' -v three_dmn_var_dbl -d time,2 -d lon,1 -d lat,0 %tempf_00%";
    $dsc_sng="Pack 3D double variable with MSA (failure expected with netCDF4)";
    ##### TODO 880
    $tst_cmd[2]="17505";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0;  # Reset array

#print "paused - hit return to continue"; my $wait=<STDIN>;
    
####################
#### ncrcat tests ## OK !
####################
    $opr_nm='ncrcat';
####################
#if ($mpi_prc == 0) { # fxm test hangs because of ncrcat TODO 593
    $tst_cmd[0]="ncra -Y ncrcat $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc %tempf_00% 2> %tempf_02%";
    $tst_cmd[1]="ncks -C -H -d time,11 -s '%f' -v rec_var_flt_mss_val_dbl %tempf_00%";
    $dsc_sng="Concatenate float with double missing values across two files";
    $tst_cmd[2]="2";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array

    $tst_cmd[0]="ncra -Y ncrcat $omp_flg -h -O $fl_fmt $nco_D_flg -v three_dmn_var_dbl $in_pth_arg -d time,,2 -d lat,0,0 -d lon,0,0 -d lon,3,3 in.nc in.nc %tempf_00% 2> %tempf_02%";
    $tst_cmd[1]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max  %tempf_00% %tempf_01%"; 
    $tst_cmd[2]="ncks -C -H  -s '%f' -v three_dmn_var_dbl %tempf_01%";
    $dsc_sng="Concatenate float variable with multi-hyperlsbs across two files";
    $tst_cmd[3]="20";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array

    $tst_cmd[0]="ncra -Y ncrcat $omp_flg -h -O $fl_fmt $nco_D_flg -v three_dmn_var_int $in_pth_arg -d time,,6 -d lat,0,0 -d lon,0,0 -d lon,3,3 in.nc in.nc %tempf_00% 2> %tempf_02%";
    $tst_cmd[1]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg %tempf_00% %tempf_01%"; 
    $tst_cmd[2]="ncks -C -H  -s '%li' -v three_dmn_var_int %tempf_01%";
    $dsc_sng="Concatenate int variable with multi-hyperlsbs across two files";
    $tst_cmd[3]="27";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array


    $tst_cmd[0]="ncra -Y ncrcat $omp_flg -h -O $fl_fmt $nco_D_flg -v four_dmn_rec_var $in_pth_arg -d time,0,,4 -d lat,0,0 -d lon,0,1 -d lon,3,3  in.nc in.nc %tempf_00% 2> %tempf_02%";
    $tst_cmd[1]="ncwa $omp_flg -h -y max -O $fl_fmt $nco_D_flg %tempf_00% %tempf_01%"; 
    $tst_cmd[2]="ncks -C -H  -s '%f' -v four_dmn_rec_var  %tempf_01%";
    $dsc_sng="Concatenate float 4D variable with multi-hyperlsbs across two files";
    $tst_cmd[3]="204";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array

    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time+=10;' $in_pth_arg in.nc %tempf_00%";    
    $tst_cmd[1]="ncra -Y ncrcat -O $omp_flg $fl_fmt $nco_D_flg -C -v time -d time,0,,4  $in_pth/in.nc %tempf_00%  %tempf_01% 2> %tempf_02%";
    $tst_cmd[2]="ncks -C -H  -s '%2.f,' -v time  %tempf_01%";
    $dsc_sng="Concatenate 1D variable with stride across two files";
    $tst_cmd[3]=" 1, 5, 9,13,17";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array

    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time+=10;' $in_pth_arg in.nc %tempf_00%"; 
    $tst_cmd[1]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time+=20;' $in_pth_arg in.nc %tempf_01%";
    $tst_cmd[2]="ncra -Y ncrcat -O $omp_flg $fl_fmt $nco_D_flg -C -v time -d time,0,,8 $in_pth/in.nc %tempf_00%  %tempf_01% %tempf_02% 2> %tempf_03%";
    $tst_cmd[3]="ncks -C -H  -s '%2.f,' -v time  %tempf_02%";
    $dsc_sng="Concatenate 1D variable with stride across three files";
    $tst_cmd[4]=" 1, 9,17,25";
    $tst_cmd[5]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array

    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time+=10;' $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks $fl_fmt $nco_D_flg -O -C -v time -d time,0,4 %tempf_00% %tempf_01%";  
    $tst_cmd[2]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time+=20;' $in_pth_arg in.nc %tempf_02%";
    $tst_cmd[3]="ncks $fl_fmt $nco_D_flg -O -C -v time -d time,5,7 %tempf_02% %tempf_03%";
    $tst_cmd[4]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,10,,2 $in_pth/in.nc %tempf_01%  %tempf_03% %tempf_04% 2> %tempf_05%";
    $tst_cmd[5]="ncks -C -H  -s '%2.f,' -v time  %tempf_04%";
    $dsc_sng="Concatentate 1D variable with stride across files with different record dimensions";
    $tst_cmd[6]="11,13,15,27";
    $tst_cmd[7]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array

    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time\@units=\"days since 1964-03-22 12:09:00 -9:00\"' $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time  $in_pth/in.nc %tempf_00% %tempf_01% 2> %tempf_05%";
    $tst_cmd[2]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tempf_01% %tempf_02%";
    $tst_cmd[3]="time_ttl = 210";
    $dsc_sng="Concatenate 1D variable across two files no limits. Requires UDUnits.";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array

    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time\@units=\"days since 1964-03-22 12:09:00 -9:00\"' $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time\@units=\"days since 1964-04-01 12:09:00 -9:00\"' $in_pth_arg in.nc %tempf_01%";
    $tst_cmd[2]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,8.0,9.0  $in_pth/in.nc %tempf_00% %tempf_01% %tempf_02% 2> %tempf_05%";
    $tst_cmd[3]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tempf_02% %tempf_03%";
    $tst_cmd[4]="time_ttl = 17";
    $dsc_sng="Concatenate 1D variable across three files double limits. Requires UDUnits.";
    $tst_cmd[5]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array

    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time\@units=\"days since 1964-03-22 12:09:00 -9:00\"' $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time\@units=\"days since 1964-04-01 12:09:00 -9:00\"' $in_pth_arg in.nc %tempf_01%";
    $tst_cmd[2]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,11.0,20.0  $in_pth/in.nc %tempf_00% %tempf_01% %tempf_02% 2> %tempf_05%";
    $tst_cmd[3]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tempf_02% %tempf_03%";
    $tst_cmd[4]="time_ttl = 155";
    $dsc_sng="Concatenate 1D variable across three files double limits2. Requires UDUnits.";
    $tst_cmd[5]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array

    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time\@units=\"days since 1964-03-22 12:09:00 -9:00\"' $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time\@units=\"days since 1964-04-01 12:09:00 -9:00\"' $in_pth_arg in.nc %tempf_01%";
    $tst_cmd[2]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,'1964-03-22 12:00','1964-03-25 12:00'   $in_pth/in.nc %tempf_00% %tempf_01% %tempf_02% 2> %tempf_05%";
    $tst_cmd[3]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tempf_02% %tempf_03%";
    $tst_cmd[4]="time_ttl = 33";
    $dsc_sng="Concatenate 1D variable across three files time stamp limits2. Requires UDUnits.";
    $tst_cmd[5]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array



    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1\"' $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1 10:0:0\"' $in_pth_arg in.nc %tempf_01%";
    $tst_cmd[2]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1 20:0:0\"' $in_pth_arg in.nc %tempf_02%";
    $tst_cmd[3]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,0.0,,11  %tempf_00% %tempf_01% %tempf_02% %tempf_03% 2> %tempf_05%";
    $tst_cmd[4]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tempf_03% %tempf_04%";
    $tst_cmd[5]="time_ttl = 33";
    $dsc_sng="Conc 1D var over 3 files with  Large stride Requires UDUnits.";
    $tst_cmd[6]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array



    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1\"' $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1 10:0:0\"' $in_pth_arg in.nc %tempf_01%";
    $tst_cmd[2]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1 20:0:0\"' $in_pth_arg in.nc %tempf_02%";
    $tst_cmd[3]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,22.0,,7  %tempf_00% %tempf_01% %tempf_02% %tempf_03% 2> %tempf_05%";
    $tst_cmd[4]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tempf_03% %tempf_04%";
    $tst_cmd[5]="time_ttl = 51";
    $dsc_sng="Conc 1D var over 3 files coo-rd limits+stride  Requires UDUnits.";
    $tst_cmd[6]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array



    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1\"' $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1 10:0:0\"' $in_pth_arg in.nc %tempf_01%";
    $tst_cmd[2]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1 20:0:0\"' $in_pth_arg in.nc %tempf_02%";
    $tst_cmd[3]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,'1990-1-1 22:0:0',,7  %tempf_00% %tempf_01% %tempf_02% %tempf_03% 2> %tempf_05%";
    $tst_cmd[4]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tempf_03% %tempf_04%";
    $tst_cmd[5]="time_ttl = 51";
    $dsc_sng="Conc 1D var over three files date-stamp limits+stride  Requires UDUnits.";
    $tst_cmd[6]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array



    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1\"' $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1 10:0:0\"' $in_pth_arg in.nc %tempf_01%";
    $tst_cmd[2]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1 20:0:0\"' $in_pth_arg in.nc %tempf_02%";
    $tst_cmd[3]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,'1990-1-1 ',,9  %tempf_00% %tempf_01% %tempf_02% %tempf_03% 2> %tempf_05%";
    $tst_cmd[4]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tempf_03% %tempf_04%";
    $tst_cmd[5]="time_ttl = 54";
    $dsc_sng="Conc 1D var over 3 files date-stamp limits+stride  Requires UDUnits.";
    $tst_cmd[6]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array



    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1\"' $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1 10:0:0\"' $in_pth_arg in.nc %tempf_01%";
    $tst_cmd[2]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1 20:0:0\"' $in_pth_arg in.nc %tempf_02%";
    $tst_cmd[3]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,,,20  %tempf_00% %tempf_01% %tempf_02% %tempf_03% 2> %tempf_05%";
    $tst_cmd[4]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tempf_03% %tempf_04%";
    $tst_cmd[5]="time_ttl = 20";
    $dsc_sng="Conc 1D variable over 3 files dmn limits+stride  Requires UDUnits.";
    $tst_cmd[6]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array


    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0 $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-1' %tempf_00%";   
    $tst_cmd[2]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0,1 $in_pth_arg in.nc %tempf_01%";
    $tst_cmd[3]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-2' %tempf_01%";   
    $tst_cmd[4]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0,3 $in_pth_arg in.nc %tempf_02%";
    $tst_cmd[5]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-4' %tempf_02%";   
    $tst_cmd[6]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0,7 $in_pth_arg in.nc %tempf_03%";
    $tst_cmd[7]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-8' %tempf_03%";   
    $tst_cmd[8]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,7,   %tempf_00% %tempf_01% %tempf_02% %tempf_03% %tempf_04% 2> %tempf_06%";
    $tst_cmd[9]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tempf_04% %tempf_05%";
    $tst_cmd[10]="time_ttl = 92";
    $dsc_sng="Conc 1D var over 4 files - different rec sizes  Requires UDUnits.";
    $tst_cmd[11]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array


    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0 $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-1' %tempf_00%";   
    $tst_cmd[2]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0,1 $in_pth_arg in.nc %tempf_01%";
    $tst_cmd[3]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-2' %tempf_01%";   
    $tst_cmd[4]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0,3 $in_pth_arg in.nc %tempf_02%";
    $tst_cmd[5]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-4' %tempf_02%";   
    $tst_cmd[6]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0,7 $in_pth_arg in.nc %tempf_03%";
    $tst_cmd[7]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-8' %tempf_03%";   
    $tst_cmd[8]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,,,4   %tempf_00% %tempf_01% %tempf_02% %tempf_03% %tempf_04% 2> %tempf_06%";
    $tst_cmd[9]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tempf_04% %tempf_05%";
    $tst_cmd[10]="time_ttl = 28";
    $dsc_sng="Conc 1D var over 4 files - with srd=4  different rec sizes  Requires UDUnits.";
    $tst_cmd[11]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array


    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0 $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-1' %tempf_00%";   
    $tst_cmd[2]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0,1 $in_pth_arg in.nc %tempf_01%";
    $tst_cmd[3]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-2' %tempf_01%";   
    $tst_cmd[4]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0,3 $in_pth_arg in.nc %tempf_02%";
    $tst_cmd[5]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-4' %tempf_02%";   
    $tst_cmd[6]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0,7 $in_pth_arg in.nc %tempf_03%";
    $tst_cmd[7]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-8' %tempf_03%";   
    $tst_cmd[8]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,'1981-2-4 ','1981-2-20 ',3   %tempf_00% %tempf_01% %tempf_02% %tempf_03% %tempf_04% 2> %tempf_06%";
    $tst_cmd[9]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tempf_04% %tempf_05%";
    $tst_cmd[10]="time_ttl = 45";
    $dsc_sng="Conc 1D var over 4 files - with srd=3 str/end timestamps  Requires UDUnits.";
    $tst_cmd[11]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array




#    } else { print "NB: Current mpncrcat test skipped because it hangs fxm TODO nco593.\n";}
    
####################
#### ncra tests #### OK!
####################
    $opr_nm='ncra';
####################
    
#        if ($mpi_prc == 0 || ($mpi_prc > 0 && $localhostname !~ /sand/)) { # test hangs because of ncrcat TODO nco772
    $tst_cmd[0]="ncra -Y ncrcat $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc %tempf_01% 2> %tempf_02%";
    $tst_cmd[1]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y avg -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc %tempf_00%";
    $tst_cmd[2]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -a time %tempf_00% %tempf_00%";
    $tst_cmd[3]="ncdiff $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_dbl %tempf_01% %tempf_00% %tempf_00%";
    $tst_cmd[4]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y rms -v rec_var_flt_mss_val_dbl %tempf_00% %tempf_00%";
    $tst_cmd[5]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl %tempf_00%";
    $dsc_sng="record sdn of float with double missing values across two files";
    $tst_cmd[6]="2";
    $tst_cmd[7]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
#    } else { print "NB: Current mpncra test skipped on sand because mpncrcat step hangs fxm TODO nco593\n";}
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v one_dmn_rec_var $in_pth_arg in.nc in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v one_dmn_rec_var %tempf_00%";
    $dsc_sng="record mean of int across two files";
    $tst_cmd[2]="5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl %tempf_00%";
    $dsc_sng="record mean of float with double missing values";
    $tst_cmd[2]="5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_int $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_int %tempf_00%";
    $dsc_sng="record mean of float with integer missing values";
    $tst_cmd[2]="5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_int_mss_val_int $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v rec_var_int_mss_val_int %tempf_00%";
    $dsc_sng="record mean of integer with integer missing values";
    $tst_cmd[2]="5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_int_mss_val_int $in_pth_arg in.nc in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v rec_var_int_mss_val_int %tempf_00%";
    $dsc_sng="record mean of integer with integer missing values across two files";
    $tst_cmd[2]="5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_int_mss_val_flt $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v rec_var_int_mss_val_flt %tempf_00%";
    $dsc_sng="record mean of integer with float missing values";
    $tst_cmd[2]="5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_int_mss_val_flt $in_pth_arg in.nc in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v rec_var_int_mss_val_flt %tempf_00%";
    $dsc_sng="record mean of integer with float missing values across two files";
    $tst_cmd[2]="5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_dbl_mss_val_dbl_pck $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_dbl_mss_val_dbl_pck %tempf_00%";
    $dsc_sng="record mean of packed double with double missing values (FAILURE with NETCDF4 nco945) ";
    $tst_cmd[2]="5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_dbl_pck $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_dbl_pck %tempf_00%";
    $dsc_sng="record mean of packed double to test precision";
    $tst_cmd[2]="100.55";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_flt_pck $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%3.2f' -v rec_var_flt_pck %tempf_00%";
    $dsc_sng="record mean of packed float to test precision";
    $tst_cmd[2]="100.55";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v pck,one_dmn_rec_var $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v pck %tempf_00%";
    $dsc_sng="pass through non-record (i.e., non-processed) packed data to output";
    $tst_cmd[2]="1";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y avg -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl %tempf_00%";
    $dsc_sng="record mean of float with double missing values across two files";
    $tst_cmd[2]="5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl %tempf_00%";
    $dsc_sng="record min of float with double missing values across two files";
    $tst_cmd[2]="2";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y max -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl %tempf_00%";
    $dsc_sng="record max of float with double missing values across two files";
    $tst_cmd[2]="8";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y ttl -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl %tempf_00%";
    $dsc_sng="record ttl of float with double missing values across two files";
    $tst_cmd[2]="70";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y rms -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%1.5f' -v rec_var_flt_mss_val_dbl %tempf_00%";
    $dsc_sng="record rms of float with double missing values across two files";
    $tst_cmd[2]="5.38516";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y ttl -v time,one_dmn_rec_var $in_pth_arg in.nc in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v time %tempf_00%";
    $dsc_sng="Return mean time coordinate (when computing totals) across two files";
    $tst_cmd[2]="5.5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y ttl -v time,one_dmn_rec_var $in_pth_arg in.nc in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v one_dmn_rec_var %tempf_00%";
    $dsc_sng="Return total record (when computing totals) across two files";
    $tst_cmd[2]="110";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array

    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -X 0.,180.,-30.,30. -v gds_3dvar $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%8.1f' -v gds_3dvar %tempf_00%";
    $dsc_sng="Return record average of cell-based geodesic data";
    $tst_cmd[2]="   277.7   277.8   277.9   278.0   278.1   278.2";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
	
    $tst_cmd[0]="/bin/rm -f %tempf_00%";
    $tst_cmd[1]="ncra  $omp_flg -h -O $fl_fmt $nco_D_flg -C $in_pth_arg lrg_bm.nc lrg_bm1.nc %tempf_00%";
    $tst_cmd[2]="ncra  -t1 -h -O $fl_fmt $nco_D_flg -C $in_pth_arg lrg_bm.nc lrg_bm1.nc %tempf_01%";
    $tst_cmd[3]="ncbo -C -h -O   %tempf_00% %tempf_01% %tempf_02%";
    $tst_cmd[4]="ncwa  -t1 -O -C  %tempf_02% %tempf_03% 2>%tempf_05%";
    $dsc_sng="Check op with OpenMP";
    $tst_cmd[5]="ncap2 -t1 -h -v -O -s 'n2= ( (fabs(R)<0.01) && (fabs(S)<0.01) && (fabs(T)<0.01) && (fabs(U)<0.01) && (fabs(V)<0.01) && (fabs(W)<0.01) && (fabs(X)<0.01) );print(n2)' %tempf_03% %tempf_04%";
    $tst_cmd[6]="n2 = 1";
    $tst_cmd[7]="NO_SS";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
#print "paused - hit return to continue"; my $wait=<STDIN>;
#print "<<<STOP>>>- hit return to continue"; my $wait=<STDIN>;
    
####################
#### ncwa tests #### OK!
####################
    $opr_nm='ncwa';
####################
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -v lat_T42,lon_T42,gw_T42 $in_pth_arg in.nc %tempf_03%";
    $tst_cmd[1]="ncrename -h -O $nco_D_flg -d lat_T42,lat -d lon_T42,lon -v lat_T42,lat -v gw_T42,gw -v lon_T42,lon %tempf_03%";
    $tst_cmd[2]="ncap2 -h -O $fl_fmt $nco_D_flg -s 'one[lat,lon]=lat*lon*0.0+1.0' -s 'zero[lat,lon]=lat*lon*0.0' %tempf_03% %tempf_04%";
    $tst_cmd[3]="ncks -C -H -s '%g' -v one -F -d lon,128 -d lat,64 %tempf_04%";
    $dsc_sng="Creating %tempf_03% again (FAILURE netcdf4 ncrename nco821) ";
    $tst_cmd[4]="1";
    $tst_cmd[5]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.

    push(@tst_cmd, "ncks -h -O $fl_fmt $nco_D_flg -v lat_T42,lon_T42,gw_T42 $in_pth_arg in.nc %tempf_03%");
    push(@tst_cmd, "ncrename -h -O $nco_D_flg -d lat_T42,lat -d lon_T42,lon -v lat_T42,lat -v gw_T42,gw -v lon_T42,lon %tempf_03%");
    push(@tst_cmd, "ncap2 -h -O $fl_fmt $nco_D_flg -s 'one[lat,lon]=lat*lon*0.0+1.0' -s 'zero[lat,lon]=lat*lon*0.0' %tempf_03% %tempf_04%");
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -a lat,lon -w gw -d lat,0.0,90.0 %tempf_04% %tempf_00%");
    push(@tst_cmd, "ncks -C -H -s '%g' -v one %tempf_00%");
    $dsc_sng="normalize by denominator upper hemisphere (FAILURE netcdf4 ncrename nco821)";
    $prsrv_fl=1; # save previously generated files.
#	$nsr_xpc= 1;
# tst_run();
    push(@tst_cmd, 1);
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=();  # Reset array
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v lat $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%g' -v lat %tempf_00%";
    $dsc_sng="minimize coordinate variable";
    $tst_cmd[2]="-90";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array

    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -a time -v pck,one_dmn_rec_var $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v pck %tempf_00%";
    $dsc_sng="pass through non-averaged (i.e., non-processed) packed data to output";
    $tst_cmd[2]="1";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncwa -N $omp_flg -h -O $fl_fmt $nco_D_flg -a lat,lon -w gw $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v mask %tempf_00%";
    $dsc_sng="do not normalize by denominator (FAILURE netcdf4 nco946)";
    $tst_cmd[2]="50";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -a lon -v mss_val $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v mss_val %tempf_00%";
    $dsc_sng="average with missing value attribute";
    $tst_cmd[2]="73";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -a lon -v no_mss_val $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%g' -v no_mss_val %tempf_00%";
    $dsc_sng="average without missing value attribute";
    $tst_cmd[2]="5.0e35";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v lat -m lat -M 90.0 -T eq -a lat $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%e' -v lat %tempf_00%";
    $dsc_sng="average masked coordinate";
    $tst_cmd[2]="90.0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v lat_var -m lat -M 90.0 -T eq -a lat $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%e' -v lat_var %tempf_00%";
    $dsc_sng="average masked variable";
    $tst_cmd[2]="2.0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v lev -m lev -M 100.0 -T eq -a lev -w lev_wgt $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%e' -v lev %tempf_00%";
    $dsc_sng="average masked, weighted coordinate";
    $tst_cmd[2]="100.0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v lev_var -m lev -M 100.0 -T gt -a lev -w lev_wgt $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%e' -v lev_var %tempf_00%";
    $dsc_sng="average masked, weighted variable";
    $tst_cmd[2]="666.6667";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v lat -a lat -w gw -d lat,0 $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%e' -v lat %tempf_00%";
    $dsc_sng="weight conforms to variable first time";
    $tst_cmd[2]="-90.0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v mss_val_all -a lon -w lon $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%g' -v mss_val_all %tempf_00%";
    $dsc_sng="average all missing values with weights";
    $tst_cmd[2]="1.0e36";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v val_one_mss -a lat -w wgt_one $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%e' -v val_one_mss %tempf_00%";
    $dsc_sng="average some missing values with unity weights";
    $tst_cmd[2]="1.0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v msk_prt_mss_prt -m msk_prt_mss_prt -M 1.0 -T lt -a lon $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%e' -v msk_prt_mss_prt %tempf_00%";
    $dsc_sng="average masked variable with some missing values";
    $tst_cmd[2]="0.5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc %tempf_00% 2> %tempf_02%";
    $tst_cmd[1]="ncks -C -H -s '%e' -v rec_var_flt_mss_val_dbl %tempf_00%";
    $dsc_sng="min switch on type double, some missing values";
    $tst_cmd[2]="2";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
# will fail SS - ncks not the last cmd
    @tst_cmd=(); # really reset array. $#tst_cmd=0; sets last index=0 --> list has one element.
    push(@tst_cmd, "ncwa $omp_flg  -h -O $fl_fmt $nco_D_flg -y min -v three_dmn_var_dbl -a lon $in_pth_arg in.nc %tempf_00%");
    push(@tst_cmd, "ncks -C -H -s '%f' -v three_dmn_var_dbl -d time,3 -d lat,0 %tempf_00%");
    # used to cut for field 7. (1 + 3x2 + 0x1=7)
    $dsc_sng="Dimension reduction with min switch and missing values";
    push(@tst_cmd, "-99");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=();  # Reset array
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg  -h -O $fl_fmt $nco_D_flg -y min -v three_dmn_var_dbl -a lon $in_pth_arg in.nc %tempf_00%");
    push(@tst_cmd, "ncks -C -H -s '%f' -v three_dmn_var_dbl -d time,9 -d lat,1 %tempf_00%");
    # used to cut for field 20. (1 + 9x2 + 1x1 = 20)
    $dsc_sng="Dimension reduction with min switch";
    $prsrv_fl=1; ## this is not needed anymore-- now independent from prev test-- remove this line soon
    push(@tst_cmd,"77");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=();  # Reset array
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v three_dmn_var_int -a lon $in_pth_arg in.nc %tempf_00%");
    push(@tst_cmd, "ncks -C -H -s '%d' -v three_dmn_var_int -d time,2 -d lat,0 %tempf_00%");
    # used to cut field 5: ( 1 + 2x2 + 0x1 = 5) 
    $dsc_sng="Dimension reduction on type int with min switch and missing values";
    push(@tst_cmd, "-99");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=();  # Reset array
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v three_dmn_var_int -a lon $in_pth_arg in.nc %tempf_00%");
    push(@tst_cmd, "ncks -C -H -s '%d' -v three_dmn_var_int -d time,3 -d lat,0 %tempf_00%");
    # used to cut field 7: ( 1 + 3x2 + 0x1 = 7) 
    $dsc_sng="Dimension reduction on type int variable";
    $prsrv_fl=1;
    push(@tst_cmd, "25");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=();  # Reset array
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v three_dmn_var_sht -a lon $in_pth_arg in.nc %tempf_00%");
    push(@tst_cmd, "ncks -C -H -s '%d' -v three_dmn_var_sht -d time,9 -d lat,1 %tempf_00%");
    # used to cut field 20: ( 1 + 9x2 + 1x1 = 20) 
    $dsc_sng="Dimension reduction on type short variable with min switch and missing values";
    push(@tst_cmd, "-99");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=();  # Reset array
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v three_dmn_var_sht -a lon $in_pth_arg in.nc %tempf_00%");
    push(@tst_cmd, "ncks -C -H -s '%d' -v three_dmn_var_sht -d time,3 -d lat,1 %tempf_00%");
    # used to cut field 8: ( 1 + 3x2 + 1x1 = 8) 
    $dsc_sng="Dimension reduction on type short variable";
    $prsrv_fl=1;
    push(@tst_cmd, "29");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=();  # Reset array
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v three_dmn_rec_var $in_pth_arg in.nc %tempf_00% 2> %tempf_02%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v three_dmn_rec_var %tempf_00%";
     $dsc_sng="Dimension reduction with min flag on type float variable";
    $tst_cmd[2]="1";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=();  # Reset array
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -v four_dmn_rec_var $in_pth_arg in.nc %tempf_00% 2> %tempf_02%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v four_dmn_rec_var %tempf_00%";
    $dsc_sng="Max flag on type float variable";
    $tst_cmd[2]="240";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=();  # Reset array
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -v three_dmn_var_dbl -a lat,lon $in_pth_arg in.nc %tempf_00%");
    push(@tst_cmd, "ncks -C -H -s '%f' -v three_dmn_var_dbl -d time,3 %tempf_00%");
    # used to cut field 4: ( 1 + 3x1=4) 
    $dsc_sng="Dimension reduction on type double variable with max switch and missing values";
    push(@tst_cmd, "-99");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=();  # Reset array
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -v three_dmn_var_dbl -a lat,lon $in_pth_arg in.nc %tempf_00%");
    push(@tst_cmd, "ncks -C -H -s '%f' -v three_dmn_var_dbl -d time,4 %tempf_00%"); 
    # used to cut field 5: ( 1 + 4x1=5) 
    $dsc_sng="Dimension reduction on type double variable";
    $prsrv_fl=1;
    push(@tst_cmd, "40");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=();  # Reset array
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -v three_dmn_var_int -a lat $in_pth_arg in.nc %tempf_00%");
    push(@tst_cmd, "ncks -C -H -s '%d' -v three_dmn_var_int -d time,2 -d lon,0 %tempf_00%");
    # used to cut field 9: ( 1 + 2x4 + 0x1=9) 
    $dsc_sng="Dimension reduction on type int variable with min switch and missing values";
    push(@tst_cmd, "-99");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=();  # Reset array
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -v three_dmn_var_int -a lat $in_pth_arg in.nc %tempf_00%");
    push(@tst_cmd, "ncks -C -H -s '%d' -v three_dmn_var_int -d time,3 -d lon,0 %tempf_00%");
    # used to cut field 13 = 1 + 3x4 + 0x1
    $dsc_sng="Dimension reduction on type int variable";
    $prsrv_fl=1;
    push(@tst_cmd, "29");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=();  # Reset array
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -v three_dmn_var_sht -a lat $in_pth_arg in.nc %tempf_00%");
    push(@tst_cmd, "ncks -C -H -s '%d' -v three_dmn_var_sht -d time,9 -d lon,0 %tempf_00%");
    # used to cut field 37 = 1 + 9x4 + 0x1
    $dsc_sng="Dimension reduction on type short variable with max switch and missing values";
    push(@tst_cmd, "-99");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=();  # Reset array
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -v three_dmn_var_sht -a lat $in_pth_arg in.nc %tempf_00%");
    push(@tst_cmd, "ncks -C -H -s '%d' -v three_dmn_var_sht -d time,8 -d lon,0 %tempf_00%");
    # used to cut field 33 = 1 + 8x4 + 0x1
    $dsc_sng="Dimension reduction on type short, max switch variable";
    $prsrv_fl=1;
    push(@tst_cmd, "69");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=();  # Reset array
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y rms -w lat_wgt -v lat_cpy $in_pth_arg in.nc %tempf_00% 2> %tempf_02%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v lat_cpy %tempf_00%";;
    $dsc_sng="rms with weights";
    $tst_cmd[2]="90";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -w val_half_half -v val_one_one_int $in_pth_arg in.nc %tempf_00% 2> %tempf_02%";
    $tst_cmd[1]="ncks -C -H -s '%ld' -v val_one_one_int %tempf_00%";;
    $dsc_sng="weights would cause SIGFPE without dbl_prc patch";
    $tst_cmd[2]="1";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y avg -v val_max_max_sht $in_pth_arg in.nc %tempf_00% 2> %tempf_02%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v val_max_max_sht %tempf_00%";;
    $dsc_sng="avg would overflow without dbl_prc patch";
    $tst_cmd[2]="17000";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y ttl -v val_max_max_sht $in_pth_arg in.nc %tempf_00% 2> %tempf_02%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v val_max_max_sht %tempf_00%";
    $dsc_sng="test wrapped ttl with dbl_prc patch (harmless failure expected/OK on all chips since wrap behavior is not IEEE-specified)";
#	$nsr_xpc= -31536 ; # Expected on Pentium IV GCC Debian 3.4.3-13, PowerPC xlc
#    $nsr_xpc= -32768 ; # Expected on Xeon GCC Fedora 3.4.2-6.fc3
#    $nsr_xpc= -32768 ; # Expected on Opteron
#    $nsr_xpc= -32768 ; # Expected on PentiumIII (Coppermine) gcc 3.4 MEPIS
#    $nsr_xpc= -31536 ; # Expected on Power4 xlc AIX
    $tst_cmd[2]="-32768";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -a lat -v lat_cpy -w gw $in_pth_arg in.nc %tempf_00%";;
    $tst_cmd[1]="ncks -C -H -s '%g' -v lat_cpy %tempf_00%";;
    $dsc_sng="min with weights";
    $tst_cmd[2]="-900";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -a lat -v lat_cpy -w gw $in_pth_arg in.nc %tempf_00%";;
    $tst_cmd[1]="ncks -C -H -s '%g' -v lat_cpy %tempf_00%";;
    $dsc_sng="max with weights";
    $tst_cmd[2]="900";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y ttl -a time -w one -v time,one_dmn_rec_var $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v time %tempf_00%";
    $dsc_sng="Return mean time coordinate (when computing totals)";
    $tst_cmd[2]="5.5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y ttl -a time -w one -v time,one_dmn_rec_var $in_pth_arg in.nc %tempf_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v one_dmn_rec_var %tempf_00%";
    $dsc_sng="Return total record (when computing totals)";
    $tst_cmd[2]="55";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    
#print "paused - hit return to continue"; my $wait=<STDIN>;
    
####################
##### net tests #### OK ! (ones that can be done by non-zender)
####################
    $opr_nm='net';
####################
# test 1
    $tst_cmd[0]="/bin/rm -f /tmp/in.nc";
    $tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -s '%e' -v one -p ftp://dust.ess.uci.edu/pub/zender/nco -l /tmp in.nc | tail -1";
    $dsc_sng="Anonymous FTP protocol (requires anonymous FTP access to dust.ess.uci.edu)";
    $tst_cmd[2]="1.000000e+00";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
# test 2
    my $sftp_url="sftp://dust.ess.uci.edu:/home/ftp/pub/zender/nco";
    if ($dust_usr ne ""){ # if we need to connect as another user (hmangalm@esmf -> hjm@dust))
	$sftp_url =~ s/dust/$dust_usr\@dust/;
    }
#sftp://dust.ess.uci.edu:/home/ftp/pub/zender/nco
    $tst_cmd[0]="/bin/rm -f /tmp/in.nc";
    $tst_cmd[1]="ncks -O $nco_D_flg -v one -p $sftp_url -l /tmp in.nc";
    $tst_cmd[2]="ncks -H $nco_D_flg -s '%e' -v one -l /tmp in.nc";
    $dsc_sng="Secure FTP (SFTP) protocol (requires SFTP access to dust.ess.uci.edu)";
    $tst_cmd[3]="1.000000e+00";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
# test 3
    if ($dust_usr ne ""){ # if we need to connect as another user (hmangalm@esmf -> hjm@dust))
	$pth_rmt_scp_tst=$dust_usr . '@' . $pth_rmt_scp_tst;
    }
    $tst_cmd[0]="/bin/rm -f /tmp/in.nc";
    $tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg  -s '%e' -v one -p $pth_rmt_scp_tst -l /tmp in.nc | tail -1";
    $dsc_sng="SSH protocol (requires authorized SSH/scp access to dust.ess.uci.edu)";
    $tst_cmd[2]=1;
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    $tst_cmd[0]="ncks -C -O -d lon,0 -s '%e' -v lon -p http://www.cdc.noaa.gov/cgi-bin/nph-nc/Datasets/ncep.reanalysis.dailyavgs/surface air.sig995.1975.nc";
    $dsc_sng="OPeNDAP protocol (requires OPeNDAP/DODS-enabled NCO)";
    $tst_cmd[1]="0";
    $tst_cmd[2]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0;  # Reset array
    
    if($USER eq 'zender'){
	$tst_cmd[0]="/bin/rm -f /tmp/etr_A4.SRESA1B_9.CCSM.atmd.2000_cat_2099.nc";
	$tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -s '%e' -d time,0 -v time -p ftp://climate.llnl.gov//sresa1b/atm/yr/etr/ncar_ccsm3_0/run9 -l /tmp etr_A4.SRESA1B_9.CCSM.atmd.2000_cat_2099.nc";
	$dsc_sng="Password-protected FTP protocol (requires .netrc-based FTP access to climate.llnl.gov)";
	$tst_cmd[2]="182.5";
	$tst_cmd[3]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0;  # Reset array
	
	$tst_cmd[0]="/bin/rm -f /tmp/in.nc";
	$tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -v one -p mss:/ZENDER/nc -l /tmp in.nc";
	$tst_cmd[2]="ncks -C -H -s '%e' -v one %tempf_00%";
	$dsc_sng="msrcp protocol (requires msrcp and authorized access to NCAR MSS)";
	$tst_cmd[3]="1";
	$tst_cmd[4]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0;  # Reset array
	
    } else { print "WARN: Skipping net tests of mss: and password protected FTP protocol retrieval---user not zender\n";}
    
    if($USER eq 'zender' || $USER eq 'hjm'){
	$tst_cmd[0]="/bin/rm -f /tmp/in.nc";
	$tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -s '%e' -v one -p wget://dust.ess.uci.edu/nco -l /tmp in.nc";
	$dsc_sng="HTTP protocol (requires developers to implement wget in NCO nudge nudge wink wink)";
 	$tst_cmd[2]="1";
	$tst_cmd[3]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0;  # Reset array
	
    } else { print "WARN: Skipping net test wget: protocol retrieval---not implemented yet\n";}
    
} # end of perform_test()
