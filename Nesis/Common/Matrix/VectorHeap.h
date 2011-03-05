#ifndef __MAT_VECTOR_HEAP_H
#define __MAT_VECTOR_HEAP_H

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

#include "Vector.h"
#include "../MathEx.h"

// --------------------------------------------------------------------

//! Simple MatriX namespace
namespace smx {

 //! Heap based implementation of vector.
 /*! Heap version of vector is more flexible than the stack based,
	  where the size of the vector can be determined at the runtime.
	  Since we avoid using heap in uC code, this class should not be
     used in uC programming.

	  Additionally, we assume that the size of VectorHeap will not be 
	  changed frequently. Hence, we will not use the std::vector as
	  the base but we will simply reserve as much space as we need.

     In the case of the vector resizing, previous content will be 
     preserved, when possible. 
  */
 
template<class T> class VectorHeap : public VectorBase<T>
{
public:
   //! Default constructor
   VectorHeap(unsigned int uiSize = 0)  : VectorBase<T>()
   {
		m_uiSize = 0;
		if(uiSize > 0)
			Resize(uiSize);
   }
   
   //! Virtual destructor. Does the clean up.
   virtual ~VectorHeap()
   {
		delete[] VectorBase<T>::m_pV;
   }

	//! Resize the dynamic array. Keep as much of the old vector as possible.
	void Resize(unsigned int uiSize, bool bKeep=false)
	{
		// nothing to do
		if(uiSize == m_uiSize)
			return;

		T* pNew = new T[uiSize];

		// Keep what you can ...
		if(bKeep) {
			for(unsigned int i=0; i<common::GetMin(uiSize,m_uiSize); i++)
				pNew[i] = VectorBase<T>::m_pV[i];
	
			// Fill the new part with zeros
			if(uiSize > m_uiSize)
				for(unsigned int i=m_uiSize; i<uiSize; i++)
					pNew[i] = 0.0;
		}

		// delete the old vector
		delete[] VectorBase<T>::m_pV;

		// assign the new one
		VectorBase<T>::m_pV = pNew;
		m_uiSize = uiSize;
	}
   
	// The copy member function
	void Copy(const VectorBase<T>& C)
	{
		Resize(C.GetSize());
		VectorBase<T>::Copy(C);
	}

   //! Copy constructor
   VectorHeap(const VectorHeap<T>& C) : VectorBase<T>()
   {
		m_uiSize = 0;
      VectorHeap<T>::Copy(C);
   }

	const VectorHeap<T>& operator=(const VectorHeap<T>& C) 
	{
      VectorHeap<T>::Copy(C);
		return *this;
	}
   
   //! Requred (by VectorBase) function that returns number of vector elements.
   virtual unsigned int GetSize() const 
   {
      return m_uiSize;
   }
   
protected:
   //! We need to keep number of the elements.
   unsigned int m_uiSize;
};

} // namespace smx

#endif // __MAT_VECTOR_HEAP_H
