// $Header: /cvsroot/nco/nco/src/nco++/sdo_utl.cc,v 1.3 2008/03/02 14:25:33 zender Exp $ 

// Purpose: Implementation (declaration) of SDO stand-alone utilities 

/* Copyright (C) 1997--2005 Charlie Zender
   This software is distributed under the terms of the GNU General Public License Version 3
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "sdo_utl.hh" // SDO stand-alone utilities: dbg/err/wrn_prn()

// Global functions with C linkages begin

// Declare global functions with C++ linkages

void 
dbg_prn(std::string prg_nm,std::string fnc_nm,std::string msg) // [fnc] Print uniform debugging message
{
  // Purpose: Print a uniform debugging message
  std::cerr << prg_nm << ": DEBUG " << fnc_nm << "(): "+msg << std::endl;
} // end dbg_prn()

void 
dbg_prn(std::string fnc_nm,std::string msg) // [fnc] Print uniform debugging message
{
  // Purpose: Print a uniform debugging message
  std::cerr << prg_nm_get() << ": DEBUG " << fnc_nm << "(): "+msg << std::endl;
} // end dbg_prn()

void 
dbg_prn(std::string msg) // [fnc] Print uniform debugging message
{
  // Purpose: Print a uniform debugging message
  std::cerr << "DEBUG: "+msg << std::endl;
} // end dbg_prn()

void 
err_prn(std::string prg_nm,std::string fnc_nm,std::string msg) // [fnc] Print uniform error message and exit 
{
  // Purpose: Print a uniform error message and exit 
  std::cerr << prg_nm << ": ERROR " << fnc_nm << "(): "+msg << std::endl;
#ifdef ABORT_ON_ERROR
  // abort() produces a core dump and traceback information useful to debuggers
  std::abort(); // [fnc] Exit with core dump
#else // !ABORT_ON_ERROR
  // exit() produces no core dump or useful debugger information
  std::exit(EXIT_FAILURE); // [fnc] Exit nicely
#endif // !ABORT_ON_ERROR
} // end err_prn()

void 
err_prn(std::string fnc_nm,std::string msg) // [fnc] Print uniform error message and exit 
{
  // Purpose: Print a uniform error message and exit 
  std::cerr << prg_nm_get() << ": ERROR " << fnc_nm << "(): "+msg << std::endl;
#ifdef ABORT_ON_ERROR
  std::abort(); // [fnc] Produce core dump
#else
  std::exit(EXIT_FAILURE); // [fnc] Exit nicely
#endif // !ABORT_ON_ERROR
} // end err_prn()

void 
err_prn(std::string msg) // [fnc] Print uniform error message and exit
{
  // Purpose: Print uniform error message and exit 
  std::cerr << msg << std::endl;
#ifdef ABORT_ON_ERROR
  std::abort(); // [fnc] Produce core dump
#else
  std::exit(EXIT_FAILURE); // [fnc] Exit nicely
#endif // !ABORT_ON_ERROR
} // end err_prn()

void 
wrn_prn(std::string prg_nm,std::string fnc_nm,std::string msg) // [fnc] Print uniform warning message and exit
{
  // Purpose: Print a uniform warning message
  std::cerr << prg_nm << ": WARNING " << fnc_nm << "(): "+msg << std::endl;
} // end wrn_prn()

void 
wrn_prn(std::string fnc_nm,std::string msg) // [fnc] Print uniform warning message and exit
{
  // Purpose: Print a uniform warning message
  std::cerr << prg_nm_get() << ": WARNING " << fnc_nm << "(): "+msg << std::endl;
} // end wrn_prn()
