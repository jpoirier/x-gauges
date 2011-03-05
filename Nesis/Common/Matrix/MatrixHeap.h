#ifndef __MAT_MATRIX_HEAP_H
#define __MAT_MATRIX_HEAP_H

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
 
/*! \file MatrixHeap.h 
    \brief Implementation of heap version of matices to be used in matrix operations.

    \author Ales Krajnc 
    \date   17.4.2006
 */

// --------------------------------------------------------------------
//#define printf(...) ;

#include "Matrix.h"
#include "../CommonDefs.h"

// --------------------------------------------------------------------

//! Simple MatriX namespace
namespace smx {

 //! Heap based implementation of a matrix.
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
 
template<class T> class MatrixHeap : public MatrixBase<T>
{
public:
   //! Default constructor
   MatrixHeap(unsigned int uiRows = 0, unsigned int uiCols = 0) : MatrixBase<T>()
   {
		m_uiRows = m_uiCols = 0;
		if(uiRows > 0 && uiCols > 0)
			Resize(uiRows, uiCols);
		MatrixBase<T>::SetAllTo(0.0);
   }
   
   //! Virtual destructor. Does the clean up.
   virtual ~MatrixHeap()
   {
		for(unsigned int r=0; r<m_uiRows; r++)
			delete[] MatrixBase<T>::m_ppM[r];
		delete[] MatrixBase<T>::m_ppM;
   }

	//! Resize the dynamic array. Keep as much of the old vector as possible.
	void Resize(unsigned int uiRows, unsigned int uiCols, bool bKeep=false)
	{
		using namespace common;
		// nothing to do
		if((uiRows == m_uiRows) && (uiCols == m_uiCols))
			return;

		T** ppM = new T*[uiRows];
		for(unsigned int r=0; r<uiRows; r++)
			ppM[r] = new T[uiCols];

		// Keep what you can ...
		if(bKeep) {
			for(unsigned int r=0; r<uiRows; r++)
				for(unsigned int c=0; c<uiCols; c++)
					if(r<m_uiRows && c<m_uiCols)
						ppM[r][c] = MatrixBase<T>::m_ppM[r][c];
					else
						ppM[r][c] = 0;
		}

		// delete the matrix 
		for(unsigned int r=0; r<m_uiRows; r++)
			delete[] MatrixBase<T>::m_ppM[r];
		delete[] MatrixBase<T>::m_ppM;

		// assign the new one
		MatrixBase<T>::m_ppM = ppM;
		m_uiRows = uiRows;
		m_uiCols = uiCols;
	}
   
	// The copy member function
	void Copy(const MatrixHeap<T>& C)
	{
		Resize(C.GetRowCount(),C.GetColCount());
		MatrixBase<T>::Copy(C);
	}

   //! Copy constructor
   MatrixHeap(const MatrixHeap<T>& C) : MatrixBase<T>()
   {
		m_uiRows = m_uiCols = 0;
      Copy(C);
   }

	const MatrixHeap<T>& operator=(const MatrixHeap<T>& C) 
	{
      MatrixHeap<T>::Copy(C);
		return *this;
	}
   
   //! Returns number of rows in matrix. Requred by MatrixBase. 
   virtual unsigned int GetRowCount() const 
   {
      return m_uiRows;
   }

   //! Returns number of columns in matrix. Requred by MatrixBase. 
   virtual unsigned int GetColCount() const 
   {
      return m_uiCols;
   }
   
protected:
   //! We need to keep number of rows.
   unsigned int m_uiRows;
   //! We need to keep number of columns.
   unsigned int m_uiCols;
};

} // namespace smx

#endif // __MAT_MATRIX_HEAP_H
