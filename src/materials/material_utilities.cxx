//! \file material_utilities.cxx

//@HEADER
// ************************************************************************
//
//                             Peridigm
//                 Copyright (2011) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions?
// David J. Littlewood   djlittl@sandia.gov
// John A. Mitchell      jamitch@sandia.gov
// Michael L. Parks      mlparks@sandia.gov
// Stewart A. Silling    sasilli@sandia.gov
//
// ************************************************************************
//@HEADER

#include "material_utilities.h"
#include <cmath>
#include <vector>
#include <Sacado.hpp>

namespace MATERIAL_EVALUATION {

void computeAndStoreInfluenceFunctionValues
(
    const double* xOverlap,
    double* influenceFunctionValues,
    int myNumPoints,
    const int* localNeighborList,
    double horizon,
    const FunctionPointer OMEGA
){
  double coord[3], neighborCoord[3], distance, influenceFunctionValue;
  int numNeighbors, neighborIndex, neighborListIndex(0), influenceFunctionIndex(0);
  for(int p=0 ; p<myNumPoints ; p++){
    coord[0] = xOverlap[3*p];
    coord[1] = xOverlap[3*p+1];
    coord[2] = xOverlap[3*p+2];
    numNeighbors = localNeighborList[neighborListIndex++];
    for(int i=0 ; i<numNeighbors ; i++){
      neighborIndex = localNeighborList[neighborListIndex++];
      neighborCoord[0] = xOverlap[3*neighborIndex];
      neighborCoord[1] = xOverlap[3*neighborIndex+1];
      neighborCoord[2] = xOverlap[3*neighborIndex+2];
      distance = std::sqrt( (coord[0] - neighborCoord[0])*(coord[0] - neighborCoord[0]) +
                            (coord[1] - neighborCoord[1])*(coord[1] - neighborCoord[1]) +
                            (coord[2] - neighborCoord[2])*(coord[2] - neighborCoord[2]) );
      influenceFunctionValue = OMEGA(distance, horizon);
      influenceFunctionValues[influenceFunctionIndex++] = influenceFunctionValue;
    }
  }
}

/**
 * Call this function on a single point 'X'
 * NOTE: neighPtr to should point to 'numNeigh' for 'X'
 * and thus describe the neighborhood list as usual
 * NOTE: this function will overwrite entries in 'yOverlap'
 * for all of 'X' neighbors
 * OUTPUT: yOverlap such that there is a state of pure
 * shear at 'X'
 */

void set_pure_shear
(
    const int *neighPtr,
    const double *X,
    const double *xOverlap,
    double *yOverlap,
    PURE_SHEAR mode,
    double gamma
)
{

  /*
   * Pure shear centered at X
   * X has no displacement
   */
  int numNeigh=*neighPtr; neighPtr++;
  for(int n=0;n<numNeigh;n++, neighPtr++){
    int localId = *neighPtr;
    const double *XP = &xOverlap[3*localId];

    double dx = XP[0]-X[0];
    double dy = XP[1]-X[1];
    double dz = XP[2]-X[2];

    /*
     * Pure shear
     */
    double zx(0.0), xy(0.0), yz(0.0);
    double xz(0.0), yx(0.0), zy(0.0);
    switch(mode){
    case ZX:
      zx = gamma * dx;
      xz = gamma * dz;
      break;
    case XY:
      xy = gamma * dy;
      yx = gamma * dx;
      break;
    case YZ:
      yz = gamma * dz;
      zy = gamma * dy;
      break;
    }

    double *YP = &yOverlap[3*localId];
    YP[0] = XP[0] + xy + xz;
    YP[1] = XP[1] + yz + yx;
    YP[2] = XP[2] + zx + zy;

  }

}
template void setPartialStresses<Sacado::Fad::DFad<double> >
(
  const Sacado::Fad::DFad<double> TX, 
  const Sacado::Fad::DFad<double> TY, 
  const Sacado::Fad::DFad<double> TZ, 
  const double X_dx, 
  const double X_dy, 
  const double X_dz,
  const double vol, 
  Sacado::Fad::DFad<double> *partialStressPtr);
template void setPartialStresses<double>(
    const double TX,
    const double TY,
    const double TZ,
    const double X_dx,
    const double X_dy,
    const double X_dz,
    const double vol,
    double *partialStressPtr
    );

template<typename ScalarT>
void getProjectedForces(
  const ScalarT t,
  const ScalarT* Y_dx,
  const ScalarT dY,
  const int dof,
  ScalarT* f
)
{
  for (int i=0;i<dof;i++){
    f[i] = t * Y_dx[i] / dY;
  }
}

template<typename ScalarT>
void PDTensorProduct(
  const ScalarT scal,
  const int dof,
  const ScalarT* A,
  const double* B,
  ScalarT* C
)
{
  for(int i=0; i<dof; i++){
     for(int j=0; j<dof; j++){
       *(C + i*dof + j) += scal * A[i] * B[j];
     }
  }
}
template void PDTensorProduct<double>(
  const double scal,
  const int dof,
  const double* A,
  const double* B,
  double* C
);
template void PDTensorProduct<Sacado::Fad::DFad<double>>(
  const Sacado::Fad::DFad<double> scal,
  const int dof,
  const Sacado::Fad::DFad<double>* A,
  const double* B,
  Sacado::Fad::DFad<double>* C
);
template void getProjectedForces<double>(
  const double t,
  const double* Y_dx,
  const double dY,
  const int dof,
  double* f
  );
template void getProjectedForces<Sacado::Fad::DFad<double>>(
  const Sacado::Fad::DFad<double> t,
  const Sacado::Fad::DFad<double>* Y_dx,
  const Sacado::Fad::DFad<double> dY,
  const int dof,
  Sacado::Fad::DFad<double>* f
  );
template<typename ScalarT>
ScalarT getStretch(
  const double A, 
  const ScalarT B
  )
  {return B-A;}

template double getStretch<double>(
  const double A, 
  const double B
  );

template Sacado::Fad::DFad<double> getStretch<Sacado::Fad::DFad<double>>(
  const double A,
  const Sacado::Fad::DFad<double> B
  );

template<typename ScalarT>
ScalarT getDiffAndLen(
  const ScalarT* A, 
  const ScalarT* B, 
  const int dof,
  ScalarT* C
  )
{
  ScalarT len = 0.0;
  for(int idof=0;idof<dof;idof++){
    C[idof] = B[idof]-A[idof];
    len += C[idof]*C[idof];
  }
  return sqrt(len);
}
 template double getDiffAndLen<double>(
  const double* A, 
  const double* B, 
  const int dof,
  double* C
  );
  template Sacado::Fad::DFad<double>  getDiffAndLen<Sacado::Fad::DFad<double> >
(
  const Sacado::Fad::DFad<double>* A, 
  const Sacado::Fad::DFad<double>* B, 
  const int dof,
  Sacado::Fad::DFad<double>* C
  );

template<typename ScalarT>
void setForces(
  const ScalarT TX, 
  const ScalarT TY, 
  const ScalarT TZ, 
  const double vol, 
  const double volNeigh, 
  ScalarT *fOwned,
  ScalarT *fNeigh
  )
{
  *(fOwned+0) += TX*volNeigh;
  *(fOwned+1) += TY*volNeigh;
  *(fOwned+2) += TZ*volNeigh;
  *(fNeigh+0) -= TX*vol;
  *(fNeigh+1) -= TY*vol;
  *(fNeigh+2) -= TZ*vol;
}

template void setForces<Sacado::Fad::DFad<double> >
(
  const Sacado::Fad::DFad<double> TX, 
  const Sacado::Fad::DFad<double> TY, 
  const Sacado::Fad::DFad<double> TZ, 
  const double vol, 
  const double volNeigh, 
  Sacado::Fad::DFad<double> *fOwned,
  Sacado::Fad::DFad<double> *fNeigh 
);
template void setForces<double>(
  const double TX,
  const double TY,
  const double TZ,
  const double vol, 
  const double volNeigh, 
  double *fOwned,
  double *fNeigh
);

template<typename ScalarT>
void setPartialStresses(
  const ScalarT TX, 
  const ScalarT TY, 
  const ScalarT TZ, 
  const double X_dx, 
  const double X_dy, 
  const double X_dz,
  const double vol, 
  ScalarT *partialStressPtr
  )
{
  *(partialStressPtr)   += TX*X_dx*vol;
  *(partialStressPtr+1) += TX*X_dy*vol;
  *(partialStressPtr+2) += TX*X_dz*vol;
  *(partialStressPtr+3) += TY*X_dx*vol;
  *(partialStressPtr+4) += TY*X_dy*vol;
  *(partialStressPtr+5) += TY*X_dz*vol;
  *(partialStressPtr+6) += TZ*X_dx*vol;
  *(partialStressPtr+7) += TZ*X_dy*vol;
  *(partialStressPtr+8) += TZ*X_dz*vol;
}
double scalarInfluenceFunction
(
        double zeta,
        double horizon
)
{
  static PeridigmNS::InfluenceFunction::functionPointer influenceFunction = NULL;
  if(!influenceFunction)
    influenceFunction = PeridigmNS::InfluenceFunction::self().getInfluenceFunction();

  return influenceFunction(zeta, horizon);
}

double computeWeightedVolume
(
    const double *X,
    const double *xOverlap,
    const double* volumeOverlap,
    const int* localNeighborList,
        double horizon,
    const FunctionPointer omega
){

  double m=0.0;
  double cellVolume;
  const int *neighPtr = localNeighborList;
  int numNeigh = *neighPtr; neighPtr++;
  const int dof = PeridigmNS::dof();
  std::vector<double> dxVector(dof); double* dx = &dxVector[0];
  for(int n=0;n<numNeigh;n++,neighPtr++){
    int localId = *neighPtr;
    cellVolume = volumeOverlap[localId];
    const double *XP = &xOverlap[3*localId];
    double d = getDiffAndLen(X, XP, dof, dx);
    m+=omega(d,horizon)*(d*d)*cellVolume;
  }

  return m;
}

void computeDeviatoricDilatation
(
    const double* xOverlap,
    const double* yOverlap,
    const double *mOwned,
    const double* volumeOverlap,
    const double* bondDamage,
    const double* epd,
    double* dilatationOwned,
    const int* localNeighborList,
    int numOwnedPoints,
    double horizon,
    const FunctionPointer OMEGA
)
{
  const double *xOwned = xOverlap;
  const double *yOwned = yOverlap;
  const double *m = mOwned;
  const double *v = volumeOverlap;
  double *theta = dilatationOwned;
  double cellVolume;
  const int *neighPtr = localNeighborList;
  const int dof = PeridigmNS::dof();
  std::vector<double> dxVector(dof); double* dx = &dxVector[0];
  for(int p=0; p<numOwnedPoints;p++, xOwned+=3, yOwned+=3, m++, theta++){
    int numNeigh = *neighPtr; neighPtr++;
    const double *X = xOwned;
    //const double *Y = yOwned;
    *theta = 0;
    for(int n=0;n<numNeigh;n++,neighPtr++,bondDamage++,epd++){
      int localId = *neighPtr;
      cellVolume = v[localId];
      const double *XP = &xOverlap[3*localId];
      //const double *YP = &yOverlap[3*localId];

      double d = getDiffAndLen(X, XP, dof, dx);
      double omega = OMEGA(d,horizon);
      double e = (*epd);
      *theta += 3.0*omega*(1.0-*bondDamage)*d*e*cellVolume/(*m);
    }

  }
}

template<typename ScalarT>
void computeDilatation
(
    const double* xOverlap,
    const ScalarT* yOverlap,
    const double *mOwned,
    const double* volumeOverlap,
    const double* bondDamage,
    ScalarT* dilatationOwned,
    const int* localNeighborList,
    int numOwnedPoints,
    double horizon,
    const FunctionPointer OMEGA,
    double thermalExpansionCoefficient,
    const double* deltaTemperature
)
{
  const double *xOwned = xOverlap;
  const ScalarT *yOwned = yOverlap;
  const double *deltaT = deltaTemperature;
  const double *m = mOwned;
  const double *v = volumeOverlap;
  ScalarT *theta = dilatationOwned;
  double cellVolume;
  const int *neighPtr = localNeighborList;
  const int dof = PeridigmNS::dof();
  std::vector<double> dxVector(dof); double* dxdump = &dxVector[0];
  std::vector<ScalarT> dyVector(dof); ScalarT* dydump = &dyVector[0];
  for(int p=0; p<numOwnedPoints;p++, xOwned+=3, yOwned+=3, deltaT++, m++, theta++){
    int numNeigh = *neighPtr; neighPtr++;
    const double *X = xOwned;
    const ScalarT *Y = yOwned;
    *theta = ScalarT(0.0);

    for(int n=0;n<numNeigh;n++,neighPtr++,bondDamage++){
      int localId = *neighPtr;
      cellVolume = v[localId];
      const double *XP = &xOverlap[3*localId];
      const ScalarT *YP = &yOverlap[3*localId];

      ScalarT dY = getDiffAndLen(Y, YP, dof, dydump);
      double d = getDiffAndLen(X, XP, dof, dxdump);
      ScalarT e = dY - d;
      
      if(deltaTemperature)
        e -= thermalExpansionCoefficient*(*deltaT)*d;
      double omega = OMEGA(d,horizon);
      *theta += 3.0*omega*(1.0-*bondDamage)*d*e*cellVolume/(*m);
    }

  }
}

/** Explicit template instantiation for double. */
template
void computeDilatation<double>
(
    const double* xOverlap,
    const double* yOverlap,
    const double *mOwned,
    const double* volumeOverlap,
    const double* bondDamage,
    double* dilatationOwned,
    const int* localNeighborList,
    int numOwnedPoints,
    double horizon,
    const FunctionPointer OMEGA,
    double thermalExpansionCoefficient,
    const double* deltaTemperature
 );


/** Explicit template instantiation for Sacado::Fad::DFad<double>. */
template
void computeDilatation<Sacado::Fad::DFad<double> >
(
    const double* xOverlap,
    const Sacado::Fad::DFad<double>* yOverlap,
    const double *mOwned,
    const double* volumeOverlap,
    const double* bondDamage,
    Sacado::Fad::DFad<double>* dilatationOwned,
    const int* localNeighborList,
    int numOwnedPoints,
        double horizon,
    const FunctionPointer OMEGA,
        double thermalExpansionCoefficient,
        const double* deltaTemperature
 );

/**
 * Call this function on a single point 'X'
 * NOTE: neighPtr to should point to 'numNeigh' for 'X'
 * and thus describe the neighborhood list as usual
 */
double computeDilatation
(
    const int *neighPtr,
    const double *X,
    const double *xOverlap,
    const double *Y,
    const double *yOverlap,
    const double *volumeOverlap,
    double weightedVolume,
    double horizon,
    const FunctionPointer OMEGA
)
{
  double bondDamage=0.0;
  const double *v = volumeOverlap;
  double m = weightedVolume;
  double theta = 0.0;
  const int dof = PeridigmNS::dof();
  std::vector<double> dumbVector(dof); double* dump = &dumbVector[0];

  int numNeigh=*neighPtr; neighPtr++;
  for(int n=0;n<numNeigh;n++,neighPtr++){

    int localId = *neighPtr;
    double cellVolume = v[localId];

    const double *XP = &xOverlap[3*localId];
    const double *YP = &yOverlap[3*localId];
    double dY = getDiffAndLen(Y, YP, dof, dump);
    double d = getDiffAndLen(X, XP, dof, dump);
    double omega = OMEGA(d,horizon);
    double e = dY-d;
    theta += 3.0*omega*(1.0-bondDamage)*d*e*cellVolume/m;

  }
  return theta;
}


double compute_norm_2_deviatoric_extension
(
    const int *neighPtr,
    const double *X,
    const double *xOverlap,
    const double *Y,
    const double *yOverlap,
    const double *volumeOverlap,
    double weighted_volume,
    double horizon,
    const FunctionPointer OMEGA
)
{

  const double *v = volumeOverlap;
  double cellVolume, zeta, dY, ed;
  const int dof = PeridigmNS::dof();
  std::vector<double> dumbVector(dof); double* dump = &dumbVector[0];
  /*
   * Compute weighted volume
   */
  double m = weighted_volume;
//  double m = computeWeightedVolume(X,xOverlap,volumeOverlap,neighPtr);
//  std::cout << NAMESPACE << "probeShearModulusScaleFactor weighted volume = " << m << std::endl;

  /*
   * Compute dilatation
   */
  double theta = computeDilatation(neighPtr,X,xOverlap,Y,yOverlap,volumeOverlap,m,horizon);
//  std::cout << NAMESPACE << "probeShearModulusScaleFactor theta = " << theta << std::endl;

  /*
   * Pure shear centered at X
   * X has no displacement
   */
  double ed_squared=0.0;
  int numNeigh=*neighPtr; neighPtr++;
  for(int n=0;n<numNeigh;n++, neighPtr++){
    int localId = *neighPtr;
    cellVolume = v[localId];
    const double *XP = &xOverlap[3*localId];

    /*
     * Deformation State
     */
    const double *YP = &yOverlap[3*localId];

    dY = getDiffAndLen(Y, YP, dof, dump);
    zeta = getDiffAndLen(X, XP, dof, dump);

    /*
     * Deviatoric extension state
     */
    ed = dY-zeta-theta*zeta/3;

    /*
     * Accumulate norm
     */
    double omega=OMEGA(zeta,horizon);
    ed_squared += ed * omega * ed * cellVolume;

  }

  return ed_squared;
}


void computeShearCorrectionFactor
(
    int numOwnedPoints,
    int lengthYOverlap,
    const double *xOverlap,
    double *yOverlap,
    const double *volumeOverlap,
    const double *owned_weighted_volume,
    const int*  localNeighborList,
    double horizon,
    double *shearCorrectionFactorOwned
){
  double gamma=1.0e-6;
  // currently un-used but may be helpful in further studies
//  double reference = 4.0 * value_of_pi() * gamma * gamma * pow(horizon,5) / 75.0;
  const int *neighPtr = localNeighborList;
  const double *xOwned = xOverlap;
  double *yOwned = yOverlap;
  double *scaleFactor = shearCorrectionFactorOwned;
  PURE_SHEAR mode;
  for(int p=0;p<numOwnedPoints;p++, xOwned+=3, yOwned+=3, scaleFactor++, owned_weighted_volume++){
    int numNeigh = *neighPtr;
    const double *X = xOwned;
    double *Y = yOwned;
    double m = *owned_weighted_volume;
    double scf, max_dsf;

    mode = XY;
    Y[0] = X[0]; Y[1] = X[1]; Y[2] = X[2];
    set_pure_shear(neighPtr,xOwned,xOverlap,yOverlap,mode,gamma);
    scf=compute_norm_2_deviatoric_extension(neighPtr,X,xOverlap,Y,yOverlap,volumeOverlap,m,horizon);
    max_dsf=scf;

    mode = ZX;
    Y[0] = X[0]; Y[1] = X[1]; Y[2] = X[2];
    set_pure_shear(neighPtr,xOwned,xOverlap,yOverlap,mode,gamma);
    scf=compute_norm_2_deviatoric_extension(neighPtr,X,xOverlap,Y,yOverlap,volumeOverlap,m,horizon);
    if(scf>max_dsf) max_dsf=scf;

    mode = YZ;
    Y[0] = X[0]; Y[1] = X[1]; Y[2] = X[2];
    set_pure_shear(neighPtr,xOwned,xOverlap,yOverlap,mode,gamma);
    scf=compute_norm_2_deviatoric_extension(neighPtr,X,xOverlap,Y,yOverlap,volumeOverlap,m,horizon);
    if(scf>max_dsf) max_dsf=scf;

    // We need to reset the location before
    //   iterating to the next point; OTHERWISE its a BUG!
    Y[0] = X[0]; Y[1] = X[1]; Y[2] = X[2];
    scf=max_dsf;
    *scaleFactor = 4.0 * gamma * gamma  * m / scf /15.0;
    neighPtr+=(numNeigh+1);
  }
}

void computeWeightedVolume
(
    const double* xOverlap,
    const double* volumeOverlap,
    double *mOwned,
    int myNumPoints,
    const int* localNeighborList,
    double horizon,
    const FunctionPointer OMEGA
){
  double *m = mOwned;
  const double *xOwned = xOverlap;
  const int *neighPtr = localNeighborList;
  for(int p=0;p<myNumPoints;p++, xOwned+=3, m++){
    int numNeigh = *neighPtr;
    const double *X = xOwned;
    *m=MATERIAL_EVALUATION::computeWeightedVolume(X,xOverlap,volumeOverlap,neighPtr,horizon);
    neighPtr+=(numNeigh+1);
  }
}


namespace WITH_BOND_VOLUME {

/**
 * Call this function on a single point 'X'
 * NOTE: neighPtr to should point to 'numNeigh' for 'X'
 * and thus describe the neighborhood list as usual
 */

double computeWeightedVolume
(
    const double *X,
    const double *xOverlap,
    const double* bondVolume,
    const int* localNeighborList,
    double horizon,
    const FunctionPointer OMEGA
){

  double m=0.0;
  const int *neighPtr = localNeighborList;
  const double *bond_volume = bondVolume;
  const int dof = PeridigmNS::dof();
  std::vector<double> dumbVector(dof); double* dump = &dumbVector[0];
  int numNeigh = *neighPtr; neighPtr++;
//  std::cout << NAMESPACE <<"computeWeightedVolume\n";
//  std::cout << "\tnumber of neighbors = " << numNeigh << std::endl;
  for(int n=0;n<numNeigh;n++,neighPtr++,bond_volume++){
    int localId = *neighPtr;
    const double *XP = &xOverlap[3*localId];
    double d = getDiffAndLen(X, XP, dof, dump);
    double omega = OMEGA(d,horizon);
    m+=omega*(d*d)*(*bond_volume);
  }

  return m;
}


/**
 * Call this function on a single point 'X'
 * NOTE: neighPtr to should point to 'numNeigh' for 'X'
 * and thus describe the neighborhood list as usual
 * NOTE: bondVolume is layed out like the neighborhood list; length
 * of bondVolume is numNeigh
 */

double computeDilatation
(
    const int *neighPtr,
    const double *X,
    const double *xOverlap,
    const double *Y,
    const double *yOverlap,
    const double *bondVolume,
    double weightedVolume,
    double horizon,
    const FunctionPointer OMEGA
)
{
  double bondDamage=0.0;
  double m = weightedVolume;
  double theta = 0.0;
  const int dof = PeridigmNS::dof();
  std::vector<double> dumbVector(dof); double* dump = &dumbVector[0];
  int numNeigh=*neighPtr; neighPtr++;
  for(int n=0;n<numNeigh;n++,neighPtr++, bondVolume++){

    int localId = *neighPtr;

    const double *XP = &xOverlap[3*localId];
    double d = getDiffAndLen(X, XP, dof, dump);
    const double *YP = &yOverlap[3*localId];
    double dY = getDiffAndLen(Y, YP, dof, dump);
    double omega = OMEGA(d,horizon);
    double e = dY-d;
    theta += 3.0*omega*(1.0-bondDamage)*d*e*(*bondVolume)/m;

  }
  return theta;
}

double compute_norm_2_deviatoric_extension
(
    const int *neighPtr,
    const double *X,
    const double *xOverlap,
    const double *Y,
    const double *yOverlap,
    const double *bondVolume,
    double weighted_volume,
    double horizon,
    const FunctionPointer OMEGA
)
{

  double zeta, dY, ed;
  const int dof = PeridigmNS::dof();
  std::vector<double> dumbVector(dof); double* dump = &dumbVector[0];
  /*
   * Compute weighted volume
   */
  double m = weighted_volume;
//  double m = computeWeightedVolume(X,xOverlap,volumeOverlap,neighPtr);
//  std::cout << NAMESPACE << "probeShearModulusScaleFactor weighted volume = " << m << std::endl;

  /*
   * Compute dilatation
   */
  double theta = computeDilatation(neighPtr,X,xOverlap,Y,yOverlap,bondVolume,m,horizon);
//  std::cout << NAMESPACE << "probeShearModulusScaleFactor theta = " << theta << std::endl;

  /*
   * Pure shear centered at X
   * X has no displacement
   */
  double ed_squared=0.0;
  int numNeigh=*neighPtr; neighPtr++;
  for(int n=0;n<numNeigh;n++, neighPtr++, bondVolume++){
    int localId = *neighPtr;

    const double *XP = &xOverlap[3*localId];
    zeta = getDiffAndLen(X, XP, dof, dump);
    const double *YP = &yOverlap[3*localId];
    dY = getDiffAndLen(Y, YP, dof, dump);

    /*
     * Deviatoric extension state
     */
    ed = dY-zeta-theta*zeta/3;

    /*
     * Accumulate norm
     */
    double omega=OMEGA(zeta,horizon);
    ed_squared += ed * omega * ed * (*bondVolume);

  }

  return ed_squared;
}

}
}
