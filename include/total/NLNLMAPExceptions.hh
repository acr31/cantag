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
#ifndef NLM_EXCEPTIONS_HH
#define NLM_EXCEPTIONS_HH

#include <exception>
#include <cstring>

namespace Total {

///
/// Base exception class for NLMaP
///
class NLMAPException : public std::exception {
public:
  virtual char * what() { return "Exception thrown in NLMAP"; }
};



///
/// General failure of NLM to converge
///
class FailedToConverge : public NLMAPException {
public:
  char * what() { return "Nonlinear model failed to converge"; }
};

///
/// Model failed to converge fast enough
///
class MaxIterations : public NLMAPException {
public:
  char * what() { return "Nonlinear model took too many iterations"; }
};
 

///
/// Encountered a singular matrix
///
class SingularMatrix : public NLMAPException {
public:
  char * what() { return "Singular matrix encountered"; }
};

///
/// General error in the FitFunction implementation
///
class FitFunctionException : public NLMAPException {
public:
  char * what() { return "Exception in FitFunction implementation"; }
};

///
/// NAN from FitFunction
///
class NANException : public NLMAPException {
public:
  char * what() { return "Evaluate() call resulted in NAN in derivates or values"; }
};


///
/// Encountered an array out-of-bounds index
///
class IndexOutOfBounds : public NLMAPException {
public:
  char * what() { return "Attempt to access non-existent data"; }
};


///
/// General error in the ResidualSorter implementation
///
class ResidualSorterException : public NLMAPException {
public:
  char * what() { return "Exception when trying to evaluate residuals"; }
};


///
/// General error in interative modeling
///
class ModelingError : public NLMAPException {
public:
  ModelingError(char *f){strcpy(msg,f);}
  ModelingError() {strcpy(msg,"Error in iterative modeling algorithm");}
  char * what() { return msg;}
private: 
  char  msg[512];
};


///
/// Unable to meet accuracy
///
class ModelingFailure : public NLMAPException {
public:
  char * what() { return "Unable to model data to sufficient accuracy"; }
};

///
/// Input data was invalid
///
class InvalidData : public NLMAPException {
public:
  char * what() { return "Attempt to add invalid data"; }
};

};



#endif
