nckt
====

A fork of ncks with Postgres support

ncks combines selected features of ncdump, ncextr, and the nccut and ncpaste specifications into one versatile utility. ncks extracts a subset of the data from input-file and either prints it as ASCII text to stdout, or writes (or pastes) it to output-file, or both.

ncks will print netCDF data in ASCII format to stdout, like ncdump, but with these differences: ncks prints data in a tabular format intended to be easy to search for the data you want, one datum per screen line, with all dimension subscripts and coordinate values (if any) preceding the datum. Option -s allows the user the format the data using C-style format strings.

Options -a, -F, -H, -M, -m, -q, -s, and -u control the formatted appearance of the data.

ncks will extract (and optionally create a new netCDF file comprised of) only selected variable from the input file, like ncextr but with these differences: Only variables and coordinates may be specifically included or excluded---all global attributes and any attribute associated with an extracted variable will be copied to the screen and/or output netCDF file. Options -c, -C, -v, and -x control which variables are extracted.
