#ifndef __MAT_MATRIX_H
#define __MAT_MATRIX_H

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
 
/*! \file Matrix.h 
    \brief Declration/implementation of Matrix class.

    \author Ales Krajnc 
    \date   1.5.2006
 */

// --------------------------------------------------------------------
//#define printf(...) ;

#include <assert.h>
#include <math.h>
//#include <iostream>

#include "Vector.h"

// --------------------------------------------------------------------

namespace smx {

//! Type of the operation on matrix used during some multiplication functions.
/*! This information is typicaly used during multiplication. The result
		of multiplication can be either assigned to the matrix, can be added to
		an existing values in the matrix or subtracted from an existing value. 
	*/
enum AfterOperator
{
	aoAssign,		//!< Assign the result to the corresponding cell.
	aoAdd,			//!< Add the result to the corresponding cell.
	aoSubtract		//!< Subtract the result from the corresponding cell.
};

// --------------------------------------------------------------------
//! The base class for a row based rectangle matrix. 
/*! This class performs most of the operations on the matrix, 
    but it can't be used directly. Derived classes must be used 
    instead. Most methods performs size matching in the debug mode only.
    This class uses only one array of pointers to access elements of the
    matrix (m_ppM). This is actualy a vector of pointers to matrix rows.
    
    Derived classes may be stack or heap based. 
     
    All methods are C (zero) based. The first row (column) starts at 0.
 */ 

template<class T> class MatrixBase
{
protected:
   //! Protected constructor prevents direct instantination.
   MatrixBase() 
   {
      m_ppM = 0;//NULL; 
   }
   
   //! Virtual destructor. Does nothing, but makes the compiler happy.
   virtual ~MatrixBase()
   {
   }
   
public:
   //! The derived class must implement number of rows.
   virtual unsigned int GetRowCount() const = 0;
   //! The derived class must implement number of cols.
   virtual unsigned int GetColCount() const = 0; 
   
   //! Sets all elements in the matrix to a specified value.
   void SetAllTo(const T& dX)
   { 
      for (unsigned int r=0; r<GetRowCount(); r++) {
         for(unsigned int c=0; c<GetColCount(); c++) {
           m_ppM[r][c] = dX; 
         }
      }
   }
   
   //! Sets diagonal elements of the matrix to the values specified in vector.
   void SetDiagonal(const VectorBase<T>& v)
   {
      // check the size
      assert(v.GetSize() == GetRowCount() && v.GetSize() == GetColCount());
      // set the diagonal
      for(unsigned int i=0; i<v.GetSize(); i++) {
         m_ppM[i][i] = v[i];
      }
   }

   //! Gets diagonal elements from the matrix and store them in vector.
   void GetDiagonal(VectorBase<T>& v)
   {
      // check the size
      assert(v.GetSize() == GetRowCount() && v.GetSize() == GetColCount());
      // get the diagonal
      for(unsigned int i=0; i<v.GetSize(); i++) {
         v[i] = m_ppM[i][i];
      }
   }

   //! Makes a copy of C. The sizes must match! 
   void Copy(const MatrixBase<T>& C)
   {
      assert(GetRowCount() == C.GetRowCount());
      assert(GetColCount() == C.GetColCount());
      
      T** ppC = C.m_ppM;
      const unsigned int nR = GetRowCount();
      const unsigned int nC = GetColCount();
      
      for (unsigned int r=0; r<nR; r++) {
         for(unsigned int c=0; c<nC; c++) {
            m_ppM[r][c] = ppC[r][c]; 
         }
      }
   }
   
   //! Returns the value stored at row r and column c.
   T At(unsigned int r, unsigned int c) const 
   {
      // Check the arguments. Debug only.
      assert(c < GetColCount() && r < GetRowCount());
      return m_ppM[r][c]; 
   } 
   
   //! Returns a reference (full access) to the value at row r and column c.
   T& At(unsigned int r, unsigned int c) 
   {
      // Check the arguments. Debug only.
      assert(c < GetColCount() && r < GetRowCount());
      return m_ppM[r][c]; 
   } 
   
   //! Returns the value stored at row r and column c.
   T operator()(unsigned int r, unsigned int c) const
   { return At(r,c); }
   
   //! Returns a reference (full access) to the value at row r and column c.
   T& operator()(unsigned int r, unsigned int c)
   { return At(r,c); }
   
   //! Returns the const acces to the row r.
   const T* operator[](unsigned int r) const 
   { 
      assert(r < GetRowCount());
      return m_ppM[r];
   }
   
   //! Returns the full acces to the row r.
   inline T* operator[](unsigned int r) 
   { 
      assert(r < GetRowCount());
      return m_ppM[r];
   }
   
   //! Returns true, if the matrix is a square matrix.
   bool IsSquare() const
   { return GetColCount()==GetRowCount(); }

   //! Creates the identity matrix. It must be square, of course.
   void SetIdentity()
   {
      // Make sure the sizes match. Debug only.
      assert(IsSquare());
      for (unsigned int r=0; r<GetRowCount(); r++) {
         for(unsigned int c=0; c<GetColCount(); c++) {
            m_ppM[r][c] = (r==c ? 1.0 : 0.0);
         }
      }
   }

   //! Adds values from matrix mB to the corresponding values in this matrix.
   void Add(const MatrixBase<T>& B) 
   {
      // Make sure the sizes match. Debug only.
      assert(B.GetColCount() == GetColCount() && 
             B.GetRowCount() == GetRowCount());

      T** ppB = B.m_ppM;
      const unsigned int nR = GetRowCount();
      const unsigned int nC = GetColCount();

      for (unsigned int r=0; r<nR; r++) {
         for(unsigned int c=0; c<nC; c++) {
           m_ppM[r][c] += ppB[r][c]; 
         }
      }
   }

   //! Subtracts values from matrix mB from the corresponding values in this matrix.
   void Subtract(const MatrixBase<T>& B) 
   {
      // Make sure the sizes match. Debug only.
      assert(B.GetColCount() == GetColCount() && 
             B.GetRowCount() == GetRowCount());

      T** ppB = B.m_ppM;
      const unsigned int nR = GetRowCount();
      const unsigned int nC = GetColCount();
      
      for (unsigned int r=0; r<nR; r++) {
         for(unsigned int c=0; c<nC; c++) {
           m_ppM[r][c] -= ppB[r][c]; 
         }
      }
   }

   //! Multiply all elements in a matrix by a specified constant.
   void Multiply(const T x)
   {
		const unsigned int nR = GetRowCount();
		const unsigned int nC = GetColCount();
	
      for (unsigned int r=0; r<nR; r++) {
         for(unsigned int c=0; c<nC; c++) {
           // At(r,c) *= x; 
				m_ppM[r][c] *= x;
         }
      }
   }
   
   //! Multiply two matrices A*B. The result is stored in this.
   /*! The matrices must have correct sizes to make this method
       work correctly. The sizes are checked in the debug mode only.
		 
		 The third argument introduces what to do with the multiplication
       result. The default argument (aoAssign) simply assigns the result into
		 this matrix. The aoAdd operator adds the multiplication result to
       the existing values and aoSubtract substracts the result from the
		 existing values.
    */
   void Multiply(
      const MatrixBase<T>& A, 		//!< First matrix A
      const MatrixBase<T>& B, 		//!< Second matrix B
		AfterOperator eao = aoAssign 	//!< What kind of operator to use after multiplication.
	)
   {
      // Here we do the check of sizes.
      // Number of colums in A must match number of rows in B
      assert(A.GetColCount()==B.GetRowCount());
      // The C matrix must have the same number of rows as A.
      assert(GetRowCount()==A.GetRowCount());
      // The C matrix must have the same number of cols as B.
      assert(GetColCount()==B.GetColCount());
      // Chack the addresses of matrices
      assert(this != &A && this != &B);
      
		const unsigned int nR = GetRowCount();
		const unsigned int nC = GetColCount();
		const unsigned int nCA = A.GetColCount();
      
      T** ppA = A.m_ppM;
      T** ppB = B.m_ppM;

      for(unsigned int r=0; r<nR; r++) {
         for(unsigned int c=0; c<nC; c++) {
            T sum = 0;
            for(unsigned int i=0; i<nCA; i++) {
//               sum += A(r,i)*B(i,c);
					sum += ppA[r][i]*ppB[i][c];
				}
            // Store the result according to the eao argument
				switch(eao) {
				case aoAssign   : m_ppM[r][c]  = sum; 
										break;
				case aoAdd      : m_ppM[r][c] += sum; 
										break;
				case aoSubtract : m_ppM[r][c] -= sum; 
										break;
				}
         }
      }
   }
	
	
   //! Multiply two matrices A*Transpose[B]. The result is stored in this.
   /*! The matrices must have correct sizes to make this method
	work correctly. The sizes are checked in the debug mode only.
		 
	The third argument introduces what to do with the multiplication
	result. The default argument (aoAssign) simply assigns the result into
	this matrix. The aoAdd operator adds the multiplication result to
	the existing values and aoSubtract substracts the result from the
	existing values.
	 */
	void Multiply_AxBT(
		const MatrixBase<T>& A, 		//!< First matrix A
		const MatrixBase<T>& B, 		//!< Second matrix B
		AfterOperator eao = aoAssign 	//!< What kind of operator to use after multiplication.
	)
	{
      // Here we do the check of sizes. Remember, we multiply Transpose[B]
      // Number of colums in A must match number of cols in B
		assert(A.GetColCount()==B.GetColCount());
      // The result matrix must have the same number of rows as A.
		assert(GetRowCount()==A.GetRowCount());
      // The result matrix must have the same number of cols as B.
		assert(GetColCount()==B.GetRowCount());
      // Chack the addresses of matrices
		assert(this != &A && this != &B);
      
		const unsigned int nR = GetRowCount();
		const unsigned int nC = GetColCount();
		const unsigned int nCA = A.GetColCount();
      
		T** ppA = A.m_ppM;
		T** ppB = B.m_ppM;

		for(unsigned int r=0; r<nR; r++) {
			for(unsigned int c=0; c<nC; c++) {
				T sum = 0;
				for(unsigned int i=0; i<nCA; i++) {
//               sum += A(r,i)*B(i,c);
					sum += ppA[r][i]*ppB[c][i];
				}
            // Store the result according to the eao argument
				switch(eao) {
					case aoAssign   : m_ppM[r][c]  = sum; 
					break;
					case aoAdd      : m_ppM[r][c] += sum; 
					break;
					case aoSubtract : m_ppM[r][c] -= sum; 
					break;
				}
			}
		}
	}
   
   //! Multiply two matrices A*B + B*Transpose[A]. The result is stored in this.
   /*! The matrices must have correct sizes to make this method
	work correctly. The sizes are checked in the debug mode only.
		 
	The third argument introduces what to do with the multiplication
	result. The default argument (aoAssign) simply assigns the result into
	this matrix. The aoAdd operator adds the multiplication result to
	the existing values and aoSubtract substracts the result from the
	existing values.
	 */
	void Multiply_AxB_BxAT(
		const MatrixBase<T>& A, 		//!< First matrix A
		const MatrixBase<T>& B, 		//!< Second matrix B
		AfterOperator eao = aoAssign 	//!< What kind of operator to use after multiplication.
	)
	{
		// Both matrices must be square and this must be square.
		assert(A.IsSquare() && B.IsSquare() && IsSquare());
      // Number of colums in A must match number of rows in B
		assert(A.GetColCount()==B.GetRowCount());
		// This must be large enough.
		assert(A.GetColCount()==GetColCount());
      // Check the addresses of matrices
		assert(this != &A && this != &B);
      
		const unsigned int n = GetColCount();
      
		T** ppA = A.m_ppM;
		T** ppB = B.m_ppM;

		for(unsigned int r=0; r<n; r++) {
			for(unsigned int c=0; c<n; c++) {
				T sum = 0;
				for(unsigned int i=0; i<n; i++) {
					sum += ppA[r][i]*ppB[i][c] +
							 ppB[r][i]*ppA[c][i];
				}
            // Store the result according to the eao argument
				switch(eao) {
					case aoAssign   : m_ppM[r][c]  = sum; 
					break;
					case aoAdd      : m_ppM[r][c] += sum; 
					break;
					case aoSubtract : m_ppM[r][c] -= sum; 
					break;
				}
			}
		}
	}
	
	
	//! Special multiplication version: X = A*B*AT 
	/*! This kind of multiplication is common in practice. In order to reduce
		 the size of storage, we can write all multiplications in one function.
    */
	void Multiply_AxBxAT(
      const MatrixBase<T>& A, 		//!< First matrix A and AT
      const MatrixBase<T>& B, 		//!< Second matrix B
		VectorBase<T>& v,             //!< Working vector needed internaly.
		AfterOperator eao = aoAssign	//!< What kind of operator to use after multiplication.
	)
   {
      // Here we do the check of sizes.
      // Number of colums in A must match number of rows in B
      assert(A.GetColCount()==B.GetRowCount());
      // Number of colums in A must match number of columns in B
		// This also implies that B is square.
      assert(A.GetColCount()==B.GetColCount());

		// The result (this) is a square matrix.
		assert(IsSquare());
		// Number of rows in A must match number of rows and cols in this.
		assert(A.GetRowCount() == GetRowCount());

      // Check the addresses of matrices
      assert(this != &A && this != &B);

		// Check the size of working vector
		assert(v.GetSize() == B.GetColCount());

		const unsigned int nR = GetRowCount();
		const unsigned int nC = GetColCount();
		const unsigned int nCB = B.GetColCount();
      
      T** ppA = A.m_ppM;
      T** ppB = B.m_ppM;
      T*  pv = &v[0];

      for(unsigned int r=0; r<nR; r++) {
			// First multiplication. The intermediate result is stored in v[i]
			v.SetAllTo(0.0);
         for(unsigned int c=0; c<nCB; c++) {
            T sum = 0;
            for(unsigned int i=0; i<nCB; i++) {
					sum += ppA[r][i]*ppB[i][c];
				}
				pv[c] = sum;
         }

			// Second matrix multiplication
			for(unsigned int c=0; c<nC; c++) {
				T sum = 0;
				for(unsigned int i=0; i<nCB; i++) {
					sum += pv[i]*ppA[c][i]; // = AT[i][c]
				}
				
				// Store  the result
				switch(eao) {
				case aoAssign   : m_ppM[r][c]  = sum; 
										break;
				case aoAdd      : m_ppM[r][c] += sum; 
										break;
				case aoSubtract : m_ppM[r][c] -= sum; 
										break;
				}
			}
      }
	}

   //! x (=, +=, -=) A*b : Multiply this matrix A with vector b and store result in x.
	/*! The eao operator tells what to do with the multiplication results.
    */
   void PostMultiply(
      const VectorBase<T>& b, 		//! Argument b.
      VectorBase<T>& x,					//! The result x.
		AfterOperator eao = aoAssign 	//!< Operator to use after multiplication.
   ) const
   {
      // check the size
      assert(b.GetSize() == GetColCount());
      assert(x.GetSize() == GetRowCount());
      
		const unsigned int nR = GetRowCount();
		const unsigned int nS = b.GetSize();
      
      T* px = &x[0];
      // access to the data pointer using conversion trick
      const T* pb = &(const_cast<VectorBase<T>&>(b))[0];

      // reset the result in the case of aoAssign (default)
		if(eao==aoAssign)
      	x.SetAllTo(0.0);

      for(unsigned int r=0; r<nR; r++) {
         for(unsigned int i=0; i<nS; i++) {
				if(eao==aoSubtract) 
	            px[r] -= m_ppM[r][i] * pb[i];
				else
            	px[r] += m_ppM[r][i] * pb[i];
				
			}
		}
	}
  
   
   //! x =/+=/-= b*A : Multiply vector b with this matrix A and store result in x.
	/*! The eao operator tells what to do with the multiplication results.
    */
   void PreMultiply(
      const VectorBase<T>& b, 		//! Argument b.
      VectorBase<T>& x,					//! The result x.
		AfterOperator eao = aoAssign 	//!< Operator to use after multiplication.
   ) const
   {
      // check the size
      assert(b.GetSize() == GetRowCount());
      assert(x.GetSize() == GetColCount());
      
      const unsigned int nC = GetColCount();
      const unsigned int nS = b.GetSize();
      
      T* px = &x[0];
      // access to the data pointer using conversion trick
      const T* pb = &(const_cast<VectorBase<T>&>(b))[0];
      
      // reset the result
		if(eao==aoAssign)
      	x.SetAllTo(0.0);

      for(unsigned int c=0; c<nC; c++) {
         for(unsigned int i=0; i<nS; i++) {  
				if(eao==aoSubtract) 
	            px[c] -= pb[i]*m_ppM[i][c];
				else
	            px[c] += pb[i]*m_ppM[i][c];
			}
		}
   }
   
   //! Calculates the outer product of two vectors and stores the result in this.
   /*! The matrix (this) must be created with the right size. Number of
       rows must match the size of the first vector and number of columns must
       match the second vector. 
       Outer product calculates all possible combinations of vectors elements.
		 The eao operator tells what to do with the multiplication results.
    */
   void Outer(
		const VectorBase<T>& a,			//! First vector (defines number of rows.) 
		const VectorBase<T>& b,			//! Second vector (defines number of cols.)
		AfterOperator eao = aoAssign 	//!< Operator to use after multiplication.
	)
   {
      // Check the size
      assert(GetRowCount() == a.GetSize());
      assert(GetColCount() == b.GetSize());
      
      const unsigned int nC = GetColCount();
      const unsigned int nR = GetRowCount();
      
      // access to the data pointer using conversion trick
      const T* pa = &(const_cast<VectorBase<T>&>(a))[0];
      const T* pb = &(const_cast<VectorBase<T>&>(b))[0];
      
      // the outer product ...
		// Note: We could gain some speed by moving the switch statement 
		// out of the outer for loop and write three different loops instead.
      for(unsigned int r=0; r<nR; r++) {
         for(unsigned int c=0; c<nC; c++) {
				T fx = pa[r]*pb[c];
				switch(eao) {
            case aoAssign   : m_ppM[r][c]  = fx;
										break;
            case aoAdd 		 : m_ppM[r][c] += fx;
										break;
            case aoSubtract : m_ppM[r][c] -= fx;
										break;
				}
         }
      }
   }
   
   //! Transpose the C matrix and store result into this.
   void Transpose(const MatrixBase<T>& C)
   {
      // check the size
      assert(C.GetRowCount()==GetColCount());
      assert(C.GetColCount()==GetRowCount());
      // check the address
      assert(this != &C);
      
      const unsigned int nR = GetRowCount();
      const unsigned int nC = GetColCount();
      
      T** ppC = C.m_ppM;
      
      for(unsigned int r=0; r<nR; r++)
         for(unsigned int c=0; c<nC; c++)
            m_ppM[r][c] = ppC[c][r];
   }
   
	//! Make square matrix symmetric by force.
	/*! In some cases, a symmetric matrix is required, but the initial
       matrix has sligtly nonsymmetric elements (e.g. due to roundoff errors).
		 This method makes them symmetric by averaging non-diagonal elements.
       The matrix must be a square matrix of course.
	 */
	void ForceSymmetry()
	{
      // check the size
      assert(IsSquare());
     
      const unsigned int n = GetRowCount();
      
      for(unsigned int r=1; r<n; r++)
         for(unsigned int c=0; c<r; c++)
            m_ppM[r][c] = m_ppM[c][r] = (m_ppM[r][c] + m_ppM[c][r])*0.5;
	}

   //! Prints the matrix components to stream.
   void Dump() const 
   {
      PRINTF("Matrix [%dx%d]", GetRowCount(), GetColCount());
      for(unsigned int r=0; r<GetRowCount(); r++)  {
			PRINTF(" \n");
         for(unsigned int c=0; c<GetColCount(); c++) {
				PRINTF("%g", (float) At(r,c));
            if(c+1!=GetColCount()) 
					PRINTF("  ");
         }
      }
		PRINTF(" \n");
   }
	
	//! Calculate matrix norm.
	T GetNorm(Norm eNorm) const
	{
		const unsigned int nR = GetRowCount();
		const unsigned int nC = GetColCount();
		
		T fMax = 0.0;
		T fSum;
		
		switch(eNorm) {
			case Norm1:
				for(int c=0; c<nC; c++) {
					fSum = 0.0;
					for(int r=0; r<nC; r++) {
						fSum += m_ppM[r][c] > 0 ? m_ppM[r][c] : -m_ppM[r][c];
					}
					if(fSum > fMax) 
						fMax = fSum;
				}
				return fMax;
				
			case NormInfinite:
				for(int r=0; r<nR; r++) {
					fSum = 0.0;
					for(int c=0; c<nC; c++) {
						fSum += m_ppM[r][c] > 0 ? m_ppM[r][c] : -m_ppM[r][c];
					}
					if(fSum > fMax) 
						fMax = fSum;
				}
				return fMax;
			
			case NormFrobenius:
				for(int r=0; r<nR; r++) {
					for(int c=0; c<nC; c++) {
						fSum += m_ppM[r][c]*m_ppM[r][c];
					}
				}
				return sqrt(fSum);
				
			default:
				return -1.0; // Invalid norm
		}
	}
	

private:
	//! Assignment operator - actually a copy
//	const MatrixBase& operator= (const MatrixBase<T>& C)
//	{
//		assert("Don't use = operator!"==0); 
//	} 

   
protected:
   //! Array of pointers to individual rows. 
   T** m_ppM;
};

//! Print the matrix to the stream
/*template<class T>
std::ostream& operator << (std::ostream& out, const MatrixBase<T>& X) 
{
   X.Dump(out);
   return out; 
}*/

// --------------------------------------------------------------------
//! Rectangle, fix sized matrix on the stack. 
/*! In many cases we know in advance the size of the matrix we need. 
    In this case we can store the matrix on the stack directly.
    Most methods are inheritated from the base class.
 */
 
template<typename T, int NR, int NC> class Matrix : public MatrixBase<T>
{
public:
   //! Default constructor. It resets the matrix and initializes the pointers.
   Matrix()
   {
      for(unsigned int i=0; i<NR; i++)
         m_pR[i] = (T*)&m_M[i]; 
      MatrixBase<T>::m_ppM = (T**)&m_pR;
      
      MatrixBase<T>::SetAllTo((T)0); 
   }
   
   //! Virtual destructor. Does nothing, but makes the compiler happy.
   virtual ~Matrix()
   {
   }
   
   //! Copy constructor. The sizes must match. (The compiler will complain.)
   Matrix(const Matrix<T,NR,NC>& C) : MatrixBase<T>() 
   {
      for(unsigned int i=0; i<NR; i++)
         m_pR[i] = (T*)&m_M[i]; 
      MatrixBase<T>::m_ppM = (T**)&m_pR;

      Copy(C);
   }
	
   //! The base class needs to know number of rows. 
   virtual unsigned int GetRowCount() const { return NR; }
   //! The base class needs to know number of columns. 
   virtual unsigned int GetColCount() const { return NC; }

	// FIXME There is some bug in the assignment operator on matrices.
	//! The assignment operator.
	const Matrix<T, NR, NC>& operator=(const Matrix<T, NR, NC>& C)
	{
      for(unsigned int i=0; i<NR; i++)
         m_pR[i] = (T*)&m_M[i]; 
      MatrixBase<T>::m_ppM = (T**)&m_pR;

      Copy(C);
		return *this;
	}

	//! Set
	void Set(const T aafM[NR][NC])
	{
		// TODO Check this
		// m_M = aafM;
		for(int r=0; r<NR; r++)
			for(int c=0; c<NC; c++)
				m_M[r][c] = aafM[r][c];
	}
   
public:
   //! The data - size varies on template parameters.
   T  m_M[NR][NC];
   //! Array (column) of pointers to individual rows.
   T* m_pR[NR];
};

} // namespace smx

//! Short definition for 3x3 matrix of floats
typedef smx::Matrix<float, 3,3> M3f;


#endif // __MAT_MATRIX_H
