/*
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

#include <total/NLNonLinearModel.hh>
#include <total/NLNLMAPExceptions.hh>
#include <total/NLGaussJordanEliminator.hh>

#include <cmath>

using namespace Total;

//----------------------------------------
// Constructor.  Takes a FitFunction 
// object that is used to specify
// the function form to fit to
//----------------------------------------
NonLinearModel::NonLinearModel(FitFunction *ff,
			       FitData *fd) :
    mFitFunction(ff), 
    mFitData(fd),
    mDataSize(fd->GetInputDataSize()),
    mNumParams(ff->GetNumParameters()),
    mLambda(0.001), 
    mChiSq(1000),
    mLastCall(false),
    mIter(0)
{
  // Allocate
  mAlpha     = new REAL*[mNumParams];
  mAlphadash = new REAL*[mNumParams];
  mBeta = new REAL[mNumParams];
  // Note mDa is a REAL**
  // for the Gauss Jordan elimination
  mDa = new  REAL*[mNumParams];
  for (int i=0; i<mNumParams; i++) {
    mAlpha[i]=new REAL[mNumParams];
    mAlphadash[i]=new REAL[mNumParams];
    mDa[i]=new REAL[1];
  }
  mParams = mFitFunction->GetParams();
}



//----------------------------------------
// Destructor.  Clean up allocations
//----------------------------------------
NonLinearModel::~NonLinearModel() {
  for (int i=0; i<mNumParams; i++) {
    delete[] mAlpha[i];
    delete[] mAlphadash[i];
    delete[] mDa[i];
  }
  delete mAlpha;
  delete mAlphadash;
  delete mDa;
  delete mBeta;
}




//----------------------------------------
// Start the fitting process
// Specify:
// max_it :  maximum number of iterations before giving up
// min_delta  :  minimum % change in chi-squared to be valid
//----------------------------------------
void NonLinearModel::Fit(
			 const int max_it,
		 	 const REAL min_delta) {
  mIter=0;
  // Want to iterate:
  // + while lambda is increasing
  // + until chi squared decreases negligibly
  // + until max_iterations is reached
  REAL delta=1;
  REAL lastLambda=-1.0;
  REAL lastChiSq=-1.0;
  while (mIter<max_it && ((delta>min_delta) || mLambda>lastLambda)) {
    lastLambda=mLambda;
    lastChiSq=mChiSq;
    SingleMarquardtIteration();

    // Floating point imprecision means we may
    // need these tests to end the algorithm
    // at the right time
    if ((mChiSq==lastChiSq) && (mLambda<=lastLambda)) break;

    delta = (lastChiSq==-1.0) ? 1 : fabs((mChiSq-lastChiSq)/lastChiSq);
    mIter++;
  }
  if (mIter==max_it) throw MaxIterations();
  mLastCall=true;
  SingleMarquardtIteration();

}



//----------------------------------------
// Calculate an estimate of the one-sigma
// standard deviation of the surrent model
//----------------------------------------
REAL NonLinearModel::GetStdErr() {
  return sqrt(mChiSq/(mDataSize - mNumParams));
}



//----------------------------------------
// Perform a single iteration of the
// Marquardt nonlinear regression method
//----------------------------------------
int NonLinearModel::SingleMarquardtIteration() {
  // Compute the support data for the
  // current parameters
  ComputeSupportData(mParams);
  
  // Copy alpha to alpha'
  for (int i=0; i<mNumParams; i++) {
    mDa[i][0]=mBeta[i];
    for (int j=0; j<mNumParams; j++) {
      mAlphadash[i][j]=mAlpha[i][j];
      if (i==j) mAlphadash[i][j] *= (1.0+mLambda);
    }
  }

  // Solve the equations
  GaussJordanEliminator::Eliminate(mAlphadash,
				   mNumParams,
				   mDa,
				   1
				   );

  if (mLastCall) {
    return 0;
  }

  // Need to evaluate the trial
  REAL paramsdash[mNumParams];
  for (int i=0; i<mNumParams; i++) {
    paramsdash[i]=mParams[i]+mDa[i][0];
  }

  // Evaluate again
  REAL chisq = mChiSq;
  ComputeSupportData(paramsdash);

  if (mChiSq < chisq) {
    // This is a good trial!
    mLambda*=0.1;
    for (int i=0; i<mNumParams; i++) {
      for (int j=0; j<mNumParams; j++) {
	mAlpha[i][j]=mAlphadash[i][j];
      }
      mBeta[i]=mDa[i][0];
      mParams[i]=paramsdash[i];
    } 
    return 1;
  }
  else {
    mLambda*=10.0;
    mChiSq=chisq;
  }
  return 0; 
}



//----------------------------------------
// This function sets up the alpha array
// and the Beta vector and also records
// the value of chi squared
//----------------------------------------
void NonLinearModel::ComputeSupportData(REAL *params) {
  // Set up the arrays
  for (int i=0; i<mNumParams; i++) {
    for (int j=0; j<mNumParams; j++) {
      mAlpha[i][j]=0.0;
    }
    mBeta[i]=0.0;
  }
  mChiSq=0.0;

  for (int i=0;i<mDataSize; i++) {
    // Compute the current function value
    REAL result = mFitFunction->Evaluate(i,params,mFitData);
    
    // Check for nans
    if (result!=result) throw NANException();
    for (int n=0; n<mNumParams; n++) {
      if (mFitFunction->GetCurrentDerivative(n) !=
	mFitFunction->GetCurrentDerivative(n)) throw NANException();
    }

    REAL dm = mFitData->GetMeasurement(i)-result;
    REAL sigma = mFitData->GetSigma(i);
    mChiSq += dm*dm/(sigma*sigma);

    for (int p=0;p<mNumParams; p++) {
      // First increment beta
      mBeta[p] += dm*mFitFunction->GetCurrentDerivative(p)/(sigma*sigma);
      // Now alpha (ignore second derivative)
      for (int p2=0;p2<mNumParams; p2++) {
	mAlpha[p][p2] += mFitFunction->GetCurrentDerivative(p)*
	  mFitFunction->GetCurrentDerivative(p2)/(sigma*sigma);
      }
    }
  }
}  















