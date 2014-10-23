nckt - NetCDF kettle toaster

Work in progress!


A fork of netCDF Kitchen Sink with Postgres support -  We like to keep the project .. alive

http://nco.sourceforge.net/nco.html#ncks-netCDF-Kitchen-Sink


====


nckt combines selected features of ncdump, ncextr, and the nccut and ncpaste specifications into one versatile utility. nckt extracts a subset of the data from input-file and either prints it as ASCII text to stdout, or writes (or pastes) it to output-file, or both.

nckt will insert data into Postgres, or print netCDF data in ASCII format to stdout, like ncdump, but with these differences: ncks prints data in a tabular format intended to be easy to search for the data you want, one datum per screen line, with all dimension subscripts and coordinate values (if any) preceding the datum. Option -s allows the user the format the data using C-style format strings.

Usage:

    -3, --3, --fl_fmt=classic       Output file in netCDF3 CLASSIC (32-bit offset) storage format
    -6, --64, --fl_fmt=64bit        Output file in netCDF3 64-bit offset storage format
    -B, --bnr, --binary     Write data to unformatted binary file
    -b, --fl_bnr, --binary-file fl_bnr      Unformatted binary file to write
    -C, --nocoords          Associated coordinate variables should not be processed
    -c, --crd, --coords     Coordinate variables will all be processed

        --cnk_dmn, --chunk_dimension nm,sz  Chunksize of dimension nm is sz

        --cnk_map, --chunk_map map          Chunking map [dmn,rd1,prd,xpl]

        --cnk_plc, --chunk_policy plc       Chunking policy [all,g2d,g3d,xpl,uck]

        --cnk_scl, --chunk_scalar sz        Chunksize scalar (for all dimensions)

    -D, --dbg_lvl, --debug-level dbg_lvl    Debugging level
    -d, --dmn, --dimension dim,[min][,[max]][,[stride]] Dimension's limits and stride in hyperslab
    -F, --ftn, --fortran    Fortran indexing conventions (1-based) for I/O

        --fix_rec_dmn       Change record dimension into fixed dimension in output file

    -H, --huh, --hmm        Toggle printing data
    -h, --hst, --history    Do not append to "history" global attribute

    --mk_rec_dmn dim    Define dim as record dimension in output file

    -o, --output, --fl_out fl_out   Output file name (or use last positional argument)
    -O, --ovr, --overwrite  Overwrite existing output file, if any
    -P, --prn, --print      Print data, metadata, and units. Abbreviation for -C -H -M -m -u.
    -p, --pth, --path path  Path prefix for all input filenames
    -pd, --pgdb database    Postgres database (implies -H)
    -ph, --pghost hostname  Postgres hostname (default localhost)  
    -pu, --pguser user      Postgres username (default postgres)
    -pp, --pgpass password  Postgres password (default trust)
    -Q,                     Toggle printing of dimension indices and coordinate values
    -q, --quiet             Turn off all printing to screen
    -r, --revision, --vrs, --version        Compile-time configuration and/or program version
    -u, --units             Toggle printing units of variables, if any
    -X, --auxiliary lon_min,lon_max,lat_min,lat_max Auxiliary coordinate bounding box
    -x, --xcl, --exclude    Extract all variables EXCEPT those specified with -v
    in.nc                   Input file name
    out.nc                  Output file name (or use -o switch)

License: GNU General Public License (GPL) Version 3
The full license text is at http://www.gnu.org/copyleft/gpl.html
and in the file nco/doc/LICENSE in the NCO source distribution.

As a special exception to the terms of the GPL, you are permitted
to link the NCO source code with the HDF, netCDF, OPeNDAP, and UDUnits
libraries and to distribute the resulting executables under the terms
of the GPL, but in addition obeying the extra stipulations of the
HDF, netCDF, OPeNDAP, and UDUnits licenses.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.