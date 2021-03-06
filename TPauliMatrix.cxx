// 
// TPauliMatrix.cxx
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
// to with an argument of enum type EPauliIndex.  A EPauliIndex can be
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

ClassImp(TPauliMatrix)


LDouble_t TPauliMatrix::fResolution = 1e-12;

TPauliMatrix::TPauliMatrix(const EPauliIndex i)
{
// Initializes a standard Pauli matrix from the following list:
//    i=kPauliOne    : unit matrix
//    i=kPauliSigma1 : sigma_1
//    i=kPauliSigma2 : sigma_2
//    i=kPauliSigma3 : sigma_3

   const Complex_t i_(0,1);
   switch (i) {
   case kPauliOne:
        fMatrix[0][0] = 1;    fMatrix[0][1] = 0;
        fMatrix[1][0] = 0;    fMatrix[1][1] = 1;
      break;
   case kPauliSigma1:
        fMatrix[0][0] = 0;    fMatrix[0][1] = 1;
        fMatrix[1][0] = 1;    fMatrix[1][1] = 0;
      break;
   case kPauliSigma2:
        fMatrix[0][0] = 0;    fMatrix[0][1] = -i_;
        fMatrix[1][0] = i_;    fMatrix[1][1] = 0;
      break;
   case kPauliSigma3:
        fMatrix[0][0] = 1;    fMatrix[0][1] = 0;
        fMatrix[1][0] = 0;    fMatrix[1][1] = -1;
      break;
   default:
      Zero();
   }
}

void TPauliMatrix::Decompose(LDouble_t &a, TThreeVectorReal &b) const
{
   if (! IsHermetian()) {
      Error("TPauliMatrix::Decompose",
            "real arguments with non-Hermetian matrix");
      a = b[1] = b[2] = b[3] = 0;
      return;
   }
   a    = real(fMatrix[0][0] + fMatrix[1][1])/2;
   b[1] = real(fMatrix[1][0] + fMatrix[0][1])/2;
   b[2] = imag(fMatrix[1][0] - fMatrix[0][1])/2;
   b[3] = real(fMatrix[0][0] - fMatrix[1][1])/2;
}

void TPauliMatrix::Decompose(Complex_t &a, TThreeVectorComplex &b) const
{
   const Complex_t i_(0,1);
   a    = (fMatrix[0][0] + fMatrix[1][1])/2.L;
   b[1] = (fMatrix[1][0] + fMatrix[0][1])/2.L;
   b[2] = (fMatrix[1][0] - fMatrix[0][1])/(2.L*i_);
   b[3] = (fMatrix[0][0] - fMatrix[1][1])/2.L;
}

TPauliMatrix &TPauliMatrix::operator*=(const TPauliMatrix &source)
{
   TPauliMatrix copy(*this);
   fMatrix[0][0] = copy.fMatrix[0][0]*source.fMatrix[0][0] +
                   copy.fMatrix[0][1]*source.fMatrix[1][0] ;
   fMatrix[0][1] = copy.fMatrix[0][0]*source.fMatrix[0][1] +
                   copy.fMatrix[0][1]*source.fMatrix[1][1] ;
   fMatrix[1][0] = copy.fMatrix[1][0]*source.fMatrix[0][0] +
                   copy.fMatrix[1][1]*source.fMatrix[1][0] ;
   fMatrix[1][1] = copy.fMatrix[1][0]*source.fMatrix[0][1] +
                   copy.fMatrix[1][1]*source.fMatrix[1][1] ;
   return *this;
}

Bool_t TPauliMatrix::IsDiagonal() const
{
   LDouble_t limit = Resolution();
   if (abs(fMatrix[0][1]) >= limit ||
       abs(fMatrix[1][0]) >= limit )
      return 0;
   else 
      return 1;
}

Bool_t TPauliMatrix::operator==(const TPauliMatrix &other) const
{
   if ( abs(fMatrix[0][0] - other.fMatrix[0][0]) >= Resolution() ||
        abs(fMatrix[0][1] - other.fMatrix[0][1]) >= Resolution() ||
        abs(fMatrix[1][0] - other.fMatrix[1][0]) >= Resolution() ||
        abs(fMatrix[1][1] - other.fMatrix[1][1]) >= Resolution())
      return 0;
   else 
      return 1;
}

TPauliMatrix &TPauliMatrix::Invert()
{
   Complex_t determ = Determ();
   if (abs(determ) < Resolution()) {
      Error("TPauliMatrix::Invert","matrix is singular");
      return *this;
   }
   Complex_t temp = fMatrix[0][0];
   fMatrix[0][0] = fMatrix[1][1]/determ;
   fMatrix[0][1] = -fMatrix[0][1]/determ;
   fMatrix[1][0] = -fMatrix[1][0]/determ;
   fMatrix[1][1] = temp/determ;
   return *this;
}

TPauliMatrix &TPauliMatrix::Compose
             (const LDouble_t a, const TThreeVectorReal &polar)
{
   const Complex_t i_(0,1);
   fMatrix[0][0] = a + polar[3];
   fMatrix[0][1] = polar[1] - i_*polar[2];
   fMatrix[1][0] = polar[1] + i_*polar[2];
   fMatrix[1][1] = a - polar[3];
   return *this;
}

TPauliMatrix &TPauliMatrix::Compose
             (const Complex_t &a, const TThreeVectorComplex &polar)
{
   const Complex_t i_(0,1);
   fMatrix[0][0] = a + polar[3];
   fMatrix[0][1] = polar[1] - i_*polar[2];
   fMatrix[1][0] = polar[1] + i_*polar[2];
   fMatrix[1][1] = a - polar[3];
   return *this;
}

TPauliMatrix &TPauliMatrix::SetRotation(const TThreeRotation &rotOp)
{
// Note concerning phase of the Pauli rotation operator:
// The TThreeRotation class defines a rotation in a path-independent way, by
// finding a rotation axis with an angle on the interval [-pi,pi].  In this
// way defining TPauliMatrix::SetRotation directly with axis,angle or Euler
// arguments is not identical to defining TThreeRotation::SetEuler or ::SetAxis
// and then defining TPauliMatrix::SetRotation(TThreeRotation&); they may differ
// by a 360 degree rotation.  In Pauli algebra, a rotation by 360 degrees
// yields an additional phase factor of -1.

   TUnitVector axis;
   LDouble_t angle(0);
   rotOp.GetAxis(axis,angle);
   return SetRotation(axis,angle);
}

TPauliMatrix &TPauliMatrix::SetRotation(const LDouble_t phi,
                                        const LDouble_t theta,
                                        const LDouble_t psi)
{
   TPauliMatrix pmR;
   const TUnitVector yhat(0,1,0), zhat(0,0,1);
   SetRotation(zhat,psi);
   *this *= pmR.SetRotation(yhat,theta);
   *this *= pmR.SetRotation(zhat,phi);
   return *this;
}

TPauliMatrix &TPauliMatrix::SetRotation
             (const TUnitVector &axis, const LDouble_t angle)
{
   const Complex_t i_(0,1);
   Complex_t a = cos(angle/2);
   TThreeVectorComplex b(axis);
   b.Normalize(1);
   b *= i_*sin(angle/2);
   return Compose(a,b);
}

TPauliMatrix &TPauliMatrix::SimTransform(const TPauliMatrix &m)
{
// Similarity transform is defined as A' = M A Minverse

   TPauliMatrix oldOp(*this);
   *this = m;
   *this *= oldOp;
   return (*this /= m);
}

TPauliMatrix &TPauliMatrix::UniTransform(const TPauliMatrix &m)
{
// Unitary transform is defined as A' = M A Mdagger

   TPauliMatrix mtemp(m);
   TPauliMatrix oldOp(*this);
   *this = m;
   *this *= oldOp;
   return (*this *= mtemp.Adjoint());
}

void TPauliMatrix::Streamer(TBuffer &buf)
{
   // Put/get a Pauli matrix to/from stream buffer buf.
   // This method assumes that Complex_t is stored as LDouble_t[2].

   Double_t vector[8];
   if (buf.IsReading()) {
      buf.ReadStaticArray(vector);
      fMatrix[0][0] = Complex_t(vector[0], vector[1]);
      fMatrix[0][1] = Complex_t(vector[2], vector[3]);
      fMatrix[1][0] = Complex_t(vector[4], vector[5]);
      fMatrix[1][1] = Complex_t(vector[6], vector[7]);
   } else {
      vector[0] = fMatrix[0][0].real();
      vector[1] = fMatrix[0][0].imag();
      vector[2] = fMatrix[0][1].real();
      vector[3] = fMatrix[0][1].imag();
      vector[4] = fMatrix[1][0].real();
      vector[5] = fMatrix[1][0].imag();
      vector[6] = fMatrix[1][1].real();
      vector[7] = fMatrix[1][1].imag();
      buf.WriteArray(vector, 8);
   }
}

void TPauliMatrix::Print(Option_t *option)
{
   // Output a Pauli matrix in ascii form.

   cout << "TPauliMatrix is" << endl;
   cout << "(" << fMatrix[0][0] << "   " << fMatrix[0][1] << ")" << endl;
   cout << "(" << fMatrix[1][0] << "   " << fMatrix[1][1] << ")" << endl;
}
 
//______________________________________________________________________________


#ifdef R__HPUX

//______________________________________________________________________________
//  These functions should be inline
//______________________________________________________________________________

#endif
