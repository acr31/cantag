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

#ifndef NON_LINEAR_MODEL_HH
#define NON_LINEAR_MODEL_HH

#include <total/NLParameters.hh>
#include <exception>
#include <vector>

namespace Total {

///
/// Class to handle input data
///
class FitData {
 public:
  ///
  /// Constructor
  /// @param numFields Number of field in datum (3 for x,y,z etc)
  ///
  FitData(const int numFields);
  virtual ~FitData();

  typedef std::vector<REAL> Datum;

  ///
  /// Get number of VALID measurments in set
  ///
  virtual int   GetInputDataSize();

  ///
  /// Get the idxth index that is valid
  /// @param idx The idxth valid index to get
  ///
  virtual REAL  GetMeasurement(const int idx);

  ///
  /// Get the error estimate of VALID measurement idx
  ///
  virtual REAL  GetSigma(const int idx);

  ///
  /// Get validity of specific measurement
  /// @param idx idx'th measurement
  ///
  virtual bool  GetValidity(const int idx);

  ///
  /// Invalidate a datum
  /// @param n n'th VALID index
  ///
  virtual void  Invalidate(const int n);

  ///
  /// Get data
  /// @param field Field index into Datum
  /// @param idx idx'th VALID index
  /// @returns Requested value
  ///
  virtual REAL  GetData(const int field, const int idx);

  ///
  /// Add some new data
  /// @param data A vector of data (e.g. x,y,z)
  /// @param measurement The measured value for the datum
  /// @param error The estimated error in measurement parameter
  ///
  virtual void  AddDatum (Datum &data,
			  REAL measurement,
			  REAL error);

protected:
  ///
  /// Convert a reference to the j'th VALID
  /// index to the actual index into the data
  /// vectors
  /// @param j Input index
  /// @return Actual array index
  ///
  /// to an index into all data (inc. invalid)
  int  ActualIndex(int j);

  int                  mNumFields;
  std::vector< Datum > mData;
  std::vector< REAL >  mMeasurements;
  std::vector< REAL >  mSigma;
  std::vector< bool >  mValidity;
};



///
/// Pure virtual class overridden
/// with a custom function for fitting
///
class FitFunction {
 public:
  ///
  /// Constructor
  /// @param nparams Number of parameters to solve for
  ///
  FitFunction(const int nparams);
  virtual ~FitFunction();

  ///
  /// Return the residual between two values
  /// Default is just val1-val2 but this can be
  /// overridden in derived classes if needed
  /// @param val1 First value
  /// @param val2 Second value
  ///
  virtual REAL CalculateResidual(REAL val1, REAL val2);

  ///
  /// Return the number of parameters in the fitting function
  ///
  virtual int   GetNumParameters();

  ///
  /// Initialise the parameters
  ///
  virtual void  InitialiseParameters(FitData *fd)=0;

  ///
  /// Return the parameter estimates
  /// @return Parameter array
  ///
  virtual REAL* GetParams();

  ///
  /// Gets the derivative wrt the idx'th parameter
  /// of the last datum to be processed by Evaluate
  /// i.e. Call Evaluate on the datum before retrieving
  /// the derivative with this function
  /// @param idx Parameter index (e.g. 0,1,2 for x,y,z estimates)
  ///
  virtual REAL  GetCurrentDerivative(const int idx);

  ///
  /// Evaluate the fitting function for the idx'th
  /// valid datum, based on the parameters in array
  /// parameters
  /// @param idx idx'th valid datum
  /// @param parameters Array of current parameter values
  /// @param fd the fitting data
  ///
  virtual REAL  Evaluate(const int idx, REAL *parameters, FitData *fd)=0;

  ///
  /// Get the overall standard error of the model parameters
  /// @return 1-sigma error
  ///
  virtual REAL  GetError();

  ///
  /// Set the std error of the model parameters;
  /// @param s 1-sigma error estimate
  ///
  virtual void  SetError(const REAL s);
protected:
  int      mNumParams;
  REAL     mError;
  REAL    *mParams;
  REAL    *mDeriv;
};




///
/// Form a non-linear model (NLM)
///
class NonLinearModel {
public:
  ///
  /// Constructor
  /// @param ff FitFunction to use
  /// @param fd Fit Data to use
  ////
  NonLinearModel(FitFunction *ff, FitData *fd);
  virtual ~NonLinearModel();

  ///
  /// Get the model error estimate
  /// @return Error value
  ///
  REAL GetStdErr();

  ///
  /// Get the number of iterations completed
  /// @return Number of iterations
  ///
  int GetNumIter() { return mIter;}

  ///
  /// Perform the fit
  /// @param max_it Maximum number of iterations for model
  /// @param min_delta Minimum delta value to achieve
  ///
  void Fit(int max_it,
	   REAL min_delta);

protected:

  ///
  /// Perform a single iteration of
  /// of the Levenberg-Marquardt method
  ///
  int SingleMarquardtIteration();

  ///
  /// Compute alpha, beta, chisq
  /// for a specific set of parameter values
  /// @param params Array of parameter values
  ///
  void ComputeSupportData(REAL *params);

  FitFunction    *mFitFunction;
  FitData        *mFitData;
  int             mDataSize;
  int             mNumParams;
  REAL            mChiSq;
  REAL            mLambda;
  REAL          **mAlpha;
  REAL          **mAlphadash;
  REAL           *mBeta;
  REAL           *mParams;
  REAL          **mDa;
  REAL          **mSingleDa;
  bool            mLastCall;
  int             mIter;
};

};

#endif


