#!/usr/bin/perl -w
# Henry Butowsky 20080104
# Usage: nco_antlr_pst_prc.pl < in_fl > out_fl   

# Removes #ifdef code where __cplusplus is checked.
# The struct in ncoParserTokenTypes.hpp is deleted
# so that all remains is a global an anonymous enum

my($bskp);
$bskp=0;

while( <> ){
    if( /__cplusplus/ )   { $bskp=1;next;}
    if( /^\#endif/ && $bskp) { $bskp=0;next;} 
    s/ncoParser/ncoEnum/;
    if(!$bskp) {print;} 
}
