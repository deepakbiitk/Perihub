//! \file elastic_bond_based.h

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
#ifndef ELASTIC_BOND_BASED_H
#define ELASTIC_BOND_BASED_H

namespace MATERIAL_EVALUATION {

//! Computes contributions to the internal force resulting from owned points.
template<typename ScalarT>
void computeInternalForceElasticBondBased
(
    const double* xOverlapPtr,
    const ScalarT* yOverlapPtr,
    const double* volumeOverlapPtr,
    const double* bondDamage,
    const int* localNeighborList,
    const int numOwnedPoints,
    const double BULK_MODULUS,
    const double horizon,
    const bool applyThermalStrain,
    const double alpha,
    const double* temperature,
    const double refTemperature,
    ScalarT* fInternalOverlapPtr,
    ScalarT* partialStressPtr
);
void computeHeatFlowState(    
    const double* modelCoord,
    const int numOwnedPoints,
    const int* neighborhoodList,
    const double* temperature,
    const double* horizon,
    const double lambda,
    const double* volume,
    const double* detachedNodes,
    const double* bondDamage,
    const bool twoD,
    const bool applyThermalPrintBedFlow,
    const double lambdaBed,
    const double TBed,
    double* heatFlowState
    );
void computeHeatTransfer(   
    const double* modelCoordinates,
    const int numOwnedPoints,
    const int* neighborhoodList,
    const double* volume,
    const double* temperature,
    const double* horizon,
    const double* detachedNodes,
    const double* bondDamage,
    const bool twoD,
    const double alpha,
    const double Tenv,
    const double factor,
    const double surfaceCorrection,
    const double limit,
    const bool applyThermalPrintBedFlow,
    double* specificVolume,
    double* heatFlowState
    );
}

#endif // ELASTIC_BOND_BASED_H
