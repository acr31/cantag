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

using namespace Total;

FitData::FitData(const int n) : mNumFields(n) {}
FitData::~FitData() {}

//----------------------------------------
// Get the number of VALID measurements
// left in the dataset
//----------------------------------------
int FitData::GetInputDataSize() {
  int n=0;
  for (int i=0; i<mMeasurements.size(); i++) {

    if (mValidity[i]) n++;
  }
  return n;
}


//----------------------------------------
// Convert an index into valid data
// to an index into all data (inc. invalid)
//----------------------------------------
int FitData::ActualIndex(int i) {
  int n=0;
  for (int j=0; j<mMeasurements.size(); j++) {
    if (n==i && mValidity[j]) return j;
    if (mValidity[j])n++;
  }
  throw IndexOutOfBounds();
}

//----------------------------------------
// Get measurement for index idx
//----------------------------------------
REAL  FitData::GetMeasurement(const int idx) {
  return mMeasurements[ActualIndex(idx)];
}

//----------------------------------------
// Get error for index idx
//----------------------------------------
REAL  FitData::GetSigma(const int idx) {
  return mSigma[ActualIndex(idx)];
}

//----------------------------------------
// Get validity of index idx
//----------------------------------------
bool   FitData::GetValidity(const int idx) {
  return mValidity[ActualIndex(idx)];
}


//----------------------------------------
// Get validity of index idx
//----------------------------------------
void  FitData::Invalidate(const int idx) {
  mValidity[ActualIndex(idx)]=false;
}

//----------------------------------------
// Get the data in index i, field f
//----------------------------------------
REAL FitData::GetData(const int f, const int idx) {
  if (f>=mNumFields) throw IndexOutOfBounds();
  return (mData[ActualIndex(idx)])[f];
}

//----------------------------------------
// Add a single datum to the store
//----------------------------------------
void FitData::AddDatum (Datum &data,
				 REAL measurement,
				 REAL error)
{
  if (data.size()!=mNumFields) throw InvalidData();
  mData.push_back(data);
  mMeasurements.push_back(measurement);
  mValidity.push_back(true);
  mSigma.push_back(error);
}
