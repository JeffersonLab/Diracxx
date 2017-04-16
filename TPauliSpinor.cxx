//
// TPauliSpinor.cxx
//
// author:  Richard T. Jones  11/16/98
// version:  Dec. 12, 1998  v1.00
//
/*************************************************************************
 * Copyright(c) 1998, University of Connecticut, All rights reserved.    *
 * Author: Richard T. Jones, Asst. Prof. of Physics                      *
 *                                                                       *
 * Permission to use, copy, modify and distribute this software and its  *
 * documentation for non-commercial purposes is hereby granted without   *
 * fee, provided that the above copyright notice appears in all copies   *
 * and that both the copyright notice and this permission notice appear  *
 * in the supporting documentation. The author makes no claims about the *
 * suitability of this software for any purpose.                         *
 * It is provided "as is" without express or implied warranty.           *
 *************************************************************************/
//////////////////////////////////////////////////////////////////////////
//
// Pauli Spinor Algebra Package
//
// The present package implements all the basic algorithms dealing
// with Pauli spinors, which form a fundamental representation of the
// SU(2) group.  The basic classes are PauliSpinor and PauliMatrix,
// which are 2-vectors and 2x2 matrices, respectively, of complex
// numbers.  The generators of the group are in the standard Pauli
// sigma matrix representation.  This package has particular members
// to facilitate a quantum mechanical calculation in which the Pauli
// spinor describes the spin-state of a fermion and the QM operators
// are described by Pauli matrices.  Pauli matrices are also used to
// describe mixed states, ensembles that contain mixtures of particles
// described by more than one Pauli spinor.
//
// The standard Pauli matrices are generated by invoking the construc-
// to with an argument of enum type TPauliIndex.  A TPauliIndex can be
//    kPauliOne,    kPauliSigma1,    kPauliSimga2,    kPauliSigma3.
// Any 2x2 matrix can be expressed as a sum over this basis.
//
// Spinors and matrices can be transformed under rotations according
// to the commutation rules for the SU(2) group.  Rotations may be
// specified either by Euler angles or by a rotation axis, or by
// supplying a member of the TThreeRotation class defined in
// TFourVector.h.  All angles are assumed to be in radians.
//
// This package was developed at the University of Connecticut by
// Richard T. Jones
//
//////////////////////////////////////////////////////////////////////////
 
#include <iostream>
using namespace std;

#include "TThreeVectorComplex.h"
#include "TThreeRotation.h"
#include "TPauliSpinor.h"
#include "TPauliMatrix.h"

ClassImp(TPauliSpinor)


LDouble_t TPauliSpinor::fResolution = 1e-12;

TPauliSpinor &TPauliSpinor::SetPolar
                    (const LDouble_t &theta, const LDouble_t &phi)
{
// This representation for the state with a polarization vector along
// the direction (theta,phi) is defined by
//
//         Spinor = rotOpEuler(phi,theta,0).Invert * { 1, 0 }
//
// where the state { 1, 0 } is defined as the reference state polarized
// along the +z axis.

   const Complex_t i_(0,1);
   LDouble_t sinHalfTheta = sin(theta/2);
   LDouble_t cosHalfTheta = cos(theta/2);
   LDouble_t sinHalfPhi = sin(phi/2);
   LDouble_t cosHalfPhi = cos(phi/2);
   fSpinor[0] = cosHalfTheta * (cosHalfPhi - i_*sinHalfPhi);
   fSpinor[1] = sinHalfTheta * (cosHalfPhi + i_*sinHalfPhi);
   return *this;
}

TPauliSpinor &TPauliSpinor::Operate(const TPauliMatrix &xOp)
{
   TPauliSpinor temp(*this);
   fSpinor[0] = xOp.fMatrix[0][0]*temp.fSpinor[0] +
                xOp.fMatrix[0][1]*temp.fSpinor[1];
   fSpinor[1] = xOp.fMatrix[1][0]*temp.fSpinor[0] +
                xOp.fMatrix[1][1]*temp.fSpinor[1];
   return *this;
}

TPauliSpinor operator*(const TPauliMatrix &pmOp, const TPauliSpinor &vec)
{
   TPauliSpinor result(vec);
   return result.Operate(pmOp);
}

TPauliSpinor &TPauliSpinor::Rotate(const TThreeRotation &rotOp)
{
   TPauliMatrix pmR;
   pmR.SetRotation(rotOp);
   Operate(pmR);
   return *this;
}

TPauliSpinor &TPauliSpinor::Rotate(const LDouble_t &phi,
                                   const LDouble_t &theta,
                                   const LDouble_t &psi)
{
   TPauliMatrix pmR;
   pmR.SetRotation(phi,theta,psi);
   Operate(pmR);
   return *this;
}

TPauliSpinor &TPauliSpinor::Rotate(const TThreeVectorReal &axis)
{
   TPauliMatrix pmR;
   pmR.SetRotation(axis);
   Operate(pmR);
   return *this;
}

TPauliSpinor &TPauliSpinor::Rotate(const TUnitVector &axis,
                                   const LDouble_t angle)
{
   TPauliMatrix pmR;
   pmR.SetRotation(axis,angle);
   Operate(pmR);
   return *this;
}

void TPauliSpinor::Streamer(TBuffer &buf)
{
   // Put/get a Pauli spinor to/from stream buffer buf.
   // This method assmes that Complex_t is stored as LDouble_t[2].

   Double_t vector[4];
   if (buf.IsReading()) {
      buf.ReadStaticArray(vector);
      fSpinor[0] = Complex_t(vector[0], vector[1]);
      fSpinor[1] = Complex_t(vector[2], vector[3]);
   } else {
      vector[0] = fSpinor[0].real();
      vector[1] = fSpinor[0].imag();
      vector[2] = fSpinor[1].real();
      vector[3] = fSpinor[1].imag();
      buf.WriteArray(vector, 4);
   }
}

void TPauliSpinor::Print(Option_t *option)
{
   // Output an ascii representation for a Pauli spinor.

   cout << "TPauliSpinor is {" << fSpinor[0] << ","
        << fSpinor[1] << " }" << endl;
}

 
//______________________________________________________________________________


#ifdef R__HPUX

//______________________________________________________________________________
//  These functions should be inline
//______________________________________________________________________________

#endif
