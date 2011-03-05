#ifndef __MAT_LU_H
#define __MAT_LU_H

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
 
/*! \file LU.h 
    \brief Declration/implementation of LU decomposition class.

    \author Ales Krajnc 
    \date   1.5.2006
 */

// --------------------------------------------------------------------

#include "Matrix.h"

// --------------------------------------------------------------------

namespace smx {


//! Class for LU Matrix Decomposition.
/*! For an m-by-n matrix A with m >= n, the LU decomposition is an m-by-n
    unit lower triangular matrix L, an n-by-n upper triangular matrix U,
    and a permutation vector piv of length m so that A(piv,:) = L*U.
    If m < n, then L is m-by-m and U is m-by-n.
      
    The LU decompostion with pivoting always exists, even if the matrix is
    singular, so the constructor will never fail.  The primary use of the
    LU decomposition is in the solution of square systems of simultaneous
    linear equations.  This will fail if isNonsingular() returns false.
    
    This class does not store any internal matrix by itself, but
    stores a pointer to an outside matrix and permutation vector (pivot)
    which is needed in the process. This is dangerous, because the class
    is unable to prevent the unvanted changes on the LU matrix and P vector. 
*/
template <class R, class I>
class LU
{
public:
   LU(MatrixBase<R>& LU, VectorBase<I>& P) : m_LU(LU), m_P(P)
   {
      assert(m_LU.GetRowCount()==m_P.GetSize());
   }
      

	//! Performs decomposition of matrix A into L lower and U upper matrix.
	/*! Decomposes given matrix into lower/upper matrix stored in m_L.
       This class stores only reference to the m_L matrix (given in the constructor).
       This means that care must be taken that m_L is not changed for the time
		 that decomposed values are needed.

		 The decomposition follows a "left-looking", dot-product, Crout/Doolittle algorithm.

		 \param A a matrix to be decomposed. A is not altered by the process.
		 \param vColj a working vector needed by the algorhitm (column j) to make faster
              dot product.
    */

   void Decompose(const MatrixBase<R>& A, VectorBase<R>& vColj)   
   {
      // Make a copy. This will also check the size.
      m_LU.Copy(A);
      
      const unsigned int m = m_LU.GetRowCount();
      const unsigned int n = m_LU.GetColCount();
      
      assert(m_P.GetSize()==m_LU.GetRowCount());
      assert(vColj.GetSize()==m);
     
      // Use a "left-looking", dot-product, Crout/Doolittle algorithm.
       
      // Initialize permutation (pivot) vector
      for (unsigned int i = 0; i < m; i++) 
         m_P[i] = i;
      
      m_iPivSign = 1;
      
		// pointer to current row
      R* pRow = 0;//NULL;

      // Outer loop (columns).
      for (unsigned int c = 0; c < n; c++) {

         // Make a copy of the j-th column to localize references.
			// This suppose to make the code faster ...
         for (unsigned int i = 0; i < m; i++)
            vColj[i] = m_LU[i][c];

         // Apply previous transformations.
         for (unsigned int r = 0; r < m; r++) {
            pRow = m_LU[r];

            // Most of the time is spent in the following dot product.
            const unsigned int kMax = r<c ? r : c;
            double dSum = 0.0;
            for (unsigned int k = 0; k < kMax; k++) {
               dSum += pRow[k]*vColj[k];
            }

            pRow[c] = vColj[r] -= dSum;
         }
   
         // Find pivot and exchange if necessary.
         unsigned int p = c;
         for (unsigned int i = c+1; i < m; i++) {
            if (fabs(vColj[i]) > fabs(vColj[p])) {
               p = i;
            }
         }
         if (p != c) {
            int k=0;
            for (k = 0; k < (int)n; k++) {
               double t = m_LU[p][k]; 
               m_LU[p][k] = m_LU[c][k]; 
               m_LU[c][k] = t;
            }
            k = m_P[p]; 
            m_P[p] = m_P[c]; 
            m_P[c] = k;
            m_iPivSign = -m_iPivSign;
         }

         // Compute multipliers.
         if ((c < m) && (m_LU[c][c] != 0.0)) {
            for (int i = c+1; i < (int)m; i++) {
               m_LU[i][c] /= m_LU[c][c];
            }
         }
      }
   }

   //! Is the matrix nonsingular? 
	/*! The matrix must be decomposed before this call.
   	 \retval  true  The upper triangular factor U (and hence A) is nonsingular.
		 \retval  false A is singular.
   */

   bool IsNonsingular() 
	{
      const unsigned int n = m_LU.GetColCount();

      for (unsigned int j=0; j<n; j++) {
         if (m_LU[j][j] == 0)
            return false;
      }
      return true;
   }

   //! Copy lower triangular factor into given matrix L.
  	void GetLower(MatrixBase<R>& L) const
	{
		// check the size
		assert(L.GetRowCount()==m_LU.GetRowCount());
		assert(L.GetColCount()==m_LU.GetColCount());

		// copy values
		const unsigned int m = m_LU.GetRowCount();
		const unsigned int n = m_LU.GetColCount();

      for (unsigned int i=0; i<m; i++) {
         for (unsigned int j=0; j<n; j++) {
            if (i > j) {
               L[i][j] = m_LU[i][j];
            } 
				else if (i==j) {
               L[i][j] = 1.0;
            } 
				else {
               L[i][j] = 0.0;
            }
         }
      }
   }

   //! Copy upper triangular factors into given matrix U.
	void GetUpper(MatrixBase<R>& U) 
	{
		// check the size
		assert(U.GetColCount()==m_LU.GetColCount());

		// copy values
		const unsigned int n = m_LU.GetColCount();

      for (unsigned int i=0; i<n; i++) {
         for (unsigned int j=0; j<n; j++) {
            if (i<=j) {
               U[i][j] = m_LU[i][j];
            } 
				else {
               U[i][j] = 0.0;
            }
         }
      }
   }


   //! Compute determinant using LU factors.
   /*! \return  determinant of A, or 0 if A is not square.
    */

   R CalcDeterminant() const
	{
		const unsigned int m = m_LU.GetRowCount();
		const unsigned int n = m_LU.GetColCount();

      if (m != n) {
         return R(0);
      }
      R d = R(m_iPivSign);
      for (unsigned int j=0; j<n; j++) {
         d *= m_LU[j][j];
      }
      return d;
   }

   //! Solve A*X = B
	/*! Solves right hand sides given in matrix B. The solution overwrites 
       the B matrix. The algorithm needs a working vector for pivoting.
   	 \param B On input a matrix with as many rows as A and any number of columns.
                On output stores the solution matrix X. 
		 \param x Working vector used for (de)pivoting process.
   */

	void Solve(MatrixBase<R>& B, VectorBase<R>& x) 
   {
		// check dimensions
		assert(B.GetRowCount() == x.GetSize());
		assert(m_LU.GetRowCount() == B.GetRowCount());
		assert(&B != &m_LU);

		// the size
		const unsigned int n = m_LU.GetRowCount();
		
		// loop over columns
		for(unsigned int c=0; c<B.GetColCount(); c++) {
	   	// make a copy into x and apply pivoting
			for(unsigned int i=0; i<n; i++)
				x[i] = B[m_P[i]][c];

	      // Solve L*Y = B(piv)
      	for (unsigned int k = 0; k < n; k++) {
	         for (unsigned int i = k+1; i < n; i++) {
   	         x[i] -= x[k]*m_LU[i][k];
      	   }
         }
      
     		// Solve U*X = Y;
      	for (int k = n-1; k >= 0; k--) {
            x[k] /= m_LU[k][k];
            for (int i = 0; i < k; i++) 
               x[i] -= x[k]*m_LU[i][k];
      	}

			// Ok, the x has the solution. Copy it back into B	
			for(unsigned int i=0; i<n; i++)
				B[i][c] = x[i];
   	}
	}


	//! Solve the A*x = b. 
	/*! x and b are vectors of length equal to the number of rows in A.
		 A must be decomposed before this call. The operation uses the
       result of the decomposition to calculate the final solution.

		 \param b right hand size vector.
		 \param x the solution vector.
    */

   void Solve (const VectorBase<R> &b, VectorBase<R> &x) 
   {
		// check dimensions
		assert(b.GetSize() == x.GetSize());
		assert(m_LU.GetRowCount() == b.GetSize());
		assert(&x != &b);

		// the size
		const unsigned int n = m_LU.GetRowCount();
		
   	for(unsigned int i=0; i<n; i++)
			x[i] = b[m_P[i]];

      // Solve L*Y = B(piv)
      for (unsigned int k = 0; k < n; k++) {
         for (unsigned int i = k+1; i < n; i++) {
               x[i] -= x[k]*m_LU[i][k];
            }
         }
      
     // Solve U*X = Y;
      for (int k = n-1; k >= 0; k--) { // k must be signed!
            x[k] /= m_LU[k][k];
            for (int i = 0; i < k; i++) 
               x[i] -= x[k]*m_LU[i][k];
      }
   }

protected:
   //! External matrix used to store the result of factorisation.
   MatrixBase<R>& m_LU;
   //! External pivot vector used for pivoting.
   VectorBase<I>& m_P;
	//! Keeps sign of the pivot. Needed for determinant.
	int m_iPivSign;

}; 

} // namespace smx

#endif // __MAT_LU_H
