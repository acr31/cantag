/*
  $Header$
  Copyright (C) 2004 Robert K. Harle

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Email: robert.harle@achilles.org
  Email: rkh23@cantab.net
*/

#ifndef PARAMETERS_HH
#define PARAMETERS_HH

// Do you want debug info?
//#define DEBUG_FLAG

#ifdef DEBUG_FLAG
# include <iostream>
# define DEBUG(x) std::cout << __FILE__ << ":" << __LINE__ << " " << x << std::endl
#else
# define DEBUG(x)
#endif


// What precision do you want?
#define REAL float


#endif
