#ifndef __MAT_VECTOR_H
#define __MAT_VECTOR_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2007 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: MIT - License                                                *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/
 
/*! \file Vector.h
    \brief Implementation of math vectors to be used in matrix operations

    \author Ales Krajnc
    \date   17.4.2006
 */

// --------------------------------------------------------------------
//#define printf(...) ;

#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "Defines.h"

// --------------------------------------------------------------------

//! Simple MatriX namespace
namespace smx {

//! Different norms that can be calculated using VectorBase class.
enum Norm {
   Norm1,      	//!< The 1 norm. Sum of absolute values of vector elements.
   Norm2,      	//!< Is equal to Euclidean norm.
   NormInfinite, 	//!< The infinite norm. The largest absolute value in vector.
   NormEuclidean,	//!< The Euclidean norm. SQRT of sum of squares.
	NormFrobenius	//!< The Frobenius norm; used for matrices only.
};

// --------------------------------------------------------------------
//! Abstract vector base class. It implements most of the vector operations.
/*! The derived classes must implement storage and memory management of
    vector elements. Typicaly, two derived classes will exist. The first one
    implements stack based fixed size storage, the second one implements
    heap based dynamic storage.
 */
template <class T> class VectorBase
{
public:
protected:
   //! Protected constructor prevents direct instantination.
   VectorBase()
   {
      m_pV = 0;//NULL;
   }

   //! Virtual destructor. Does nothing, but makes the compiler happy.
   virtual ~VectorBase()
   {
   }

public:
   //! Derives classes must tell the size of the vector.
   virtual unsigned int GetSize() const = 0;

   //! Sets all elements in the vector to specified value.
   void SetAllTo(const T& x)
   {
      for(unsigned int i=0; i<GetSize(); i++)
         m_pV[i] = x;
   }

	//! Reset all elements to zero.
	void Reset()
	{ SetAllTo((T)0); }

   //! Makes a copy of C. The sizes must match!
   void Copy(const VectorBase<T>& C)
   {
      // Check the size
      assert(GetSize()==C.GetSize());
      for(unsigned int i=0; i<GetSize(); i++)
         m_pV[i] = C[i];
   }

   //! Makes a partial copy of elements from C. First element to copy is iFrom.
   void Copy(const VectorBase<T>& C, int iFrom, const int iCount)
   {
      // Check the size
      assert(GetSize() >= iCount);
      assert(C.GetSize() >= iFrom+iCount);
      for(unsigned int i=0; i<iCount; i++)
         m_pV[i] = C[iFrom++];
   }

   //! Const access (a copy) of the element at position i.
   T At(unsigned int i) const
   {
      // Check the index argument.
      assert(i<GetSize());
      return m_pV[i];
   }

   //! Full access of the element at position i.
   T& At(unsigned int i)
   {
      // Check the index argument.
      assert(i<GetSize());
      return m_pV[i];
   }

   //! Const access (a copy) of the element at position i.
   T operator[](unsigned int i) const
   {
      return At(i);
   }

   //! Full access of the element at position i.
   T& operator[](unsigned int i)
   {
      return At(i);
   }

   //! Adds components of given vector to this. The sizes must match.
   void Add(const VectorBase<T>& b)
   {
      assert(GetSize()==b.GetSize());
      for(unsigned int i=0; i<GetSize(); i++)
         m_pV[i] += b[i];
   }

   //! Subtract components of given vector from this. The sizes must match.
   void Subtract(const VectorBase<T>& b)
   {
      assert(GetSize()==b.GetSize());
      for(unsigned int i=0; i<GetSize(); i++)
         m_pV[i] -= b[i];
   }

   //! Changes sign of the vector components in this.
   void ChangeSign()
   {
      for(unsigned int i=0; i<GetSize(); i++)
         m_pV[i] -= m_pV[i];
   }

   //! Multiplies vector components with given scalar value.
   void Multiply(const T& x)
   {
      for(unsigned int i=0; i<GetSize(); i++)
         m_pV[i] *= x;
   }

   //! "Dot multiply" this with given vector. Sizes must match.
   T Multiply(const VectorBase<T>& b) const
   {
      assert(GetSize()==b.GetSize());
      T sum = 0.0;
      const T *pb = b.m_pV;
      for(unsigned int i=0; i<GetSize(); i++)
         sum += m_pV[i] * pb[i];
      return sum;
   }

	//! "Cross" product of two vectors stored into this. Valid only for size = 3.
	void Cross(const VectorBase<T> &a, const VectorBase<T> &b)
	{
		// it is valid for 3D vectors only
		assert(GetSize()==3 && a.GetSize()==3 && b.GetSize()==3);
		m_pV[0] = a[1]*b[2]-a[2]*b[1]; // ex
		m_pV[1] = a[2]*b[0]-a[0]*b[2]; // ey
		m_pV[2] = a[0]*b[1]-a[1]*b[0]; // ez
	}

	//! Returns true if all elements are 0 (or below given treshold limit)
	bool IsNull(T tTol = 1.0E-7) const
	{
		for(unsigned int i=0; i<GetSize(); i++) {
			if(!((m_pV[i] > -tTol) && (m_pV[i] < tTol)))
				return false;
		}
		return true;
	}

   //! Calculates norm given in the argument. Euclidean is the default.
   T GetNorm(Norm eN=NormEuclidean) const
   {
      T norm = 0.0;
      switch(eN) {
      // Norm1 returns sum of absolute values
      case Norm1:
         for(unsigned int i=0; i<GetSize(); i++)
            norm += fabs(m_pV[i]);
         break;
      // Norm2 i.e. Euclidean ... returns square root of sum of squares
      case Norm2:
      case NormEuclidean:
         for(unsigned int i=0; i<GetSize(); i++)
            norm += m_pV[i]*m_pV[i];
         norm = sqrt(norm);
         break;
      // NormInfinite returns maximal absolute value
      case NormInfinite:
         for(unsigned int i=0; i<GetSize(); i++) {
            T x = fabs(m_pV[i]);
            if(x > norm)
               norm = x;
         }
         break;
			default:
				norm = -1.0; // Invalid norm.
		}
      return norm;
   }

   //! Normalizes the vector using Euclidean norm
   void Normalize()
   {
      assert(GetNorm(NormEuclidean) > 0.0);
      Multiply(1.0/GetNorm(NormEuclidean));
   }

   //! Prints the vector components to stream.
   void Dump() const
   {
      PRINTF("Vector [%d] : ",  GetSize());
      for(unsigned int i=0; i<GetSize(); i++)  {
			PRINTF("%f ", (float)At(i));
      }
//      printf("\n");
   }

protected:
   //! Pointer to the vector elements.
   T* m_pV;
};

/*template <class T>
std::ostream& operator<< (std::ostream& out, const VectorBase<T>& x)
{
   x.Dump(out);
   return out;
}*/



 // -------------------------------------------------------------------
 //! Stacked based fixed size vector implementation.
 /*! This class is typicaly used, when heap based operations are not
     welcome. E.g. in uC programs. The class is template based. The first
     template argument T defines the type of the vector elements (double,
     float, int), while the second argument defines the fixed size of the
     vector. This size can't be changed during runtime.
  */

template<class T, int S> class Vector : public VectorBase<T>
{
public:
   //! Default constructor
   Vector()
   {
      VectorBase<T>::m_pV = m_V;
      VectorBase<T>::Reset();
   }

   //! Virtual destructor. Does nothing, but makes the compiler happy.
   virtual ~Vector()
   {
   }

   //! Copy constructor
   Vector(const Vector<T,S>& C) : VectorBase<T>()
   {
      VectorBase<T>::m_pV = m_V;
      Copy(C);
   }

	const Vector<T,S>& operator=(const Vector<T,S>& C)
	{
//       VectorBase<T>::m_pV = m_V;
      Copy(C);
		return *this;
	}

   //! Requred (by VectorBase) function that returns number of vector elements.
   virtual unsigned int GetSize() const
   {
      return S;
   }

protected:
   //! Fixed, stack based vector of elements T.
   T m_V[S];
};

} // namespace smx

// Lets set a few typedefs outside the smx namespace

//! Short definition for 2D vector of floats.
typedef smx::Vector<float, 2> V2f;
//! Short definition for 3D vector of floats.
typedef smx::Vector<float, 3> V3f;
//! Short definition for 4D vector of floats.
typedef smx::Vector<float, 4> V4f;


#endif // __MAT_VECTOR_H
