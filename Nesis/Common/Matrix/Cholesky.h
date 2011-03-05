#ifndef __MAT_CHOLESKY_H
#define __MAT_CHOLESKY_H

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
 
/*! \file Cholesky.h 
    \brief Declration/implementation of Cholesky decomposition.

    \author Ales Krajnc 
    \date   1.5.2006
 */

// --------------------------------------------------------------------

#include "Matrix.h"

// --------------------------------------------------------------------

namespace smx {

//! This class implements Cholesky decomposition.
/*! The matrix must be symmetric and positive definite in order to use this
    class. The class stores only status variable and reference to the matrix.
    The result of decomposition is stored in another L matrix. 
    Only lower part is significant. The upper part is set to zero.
 */  

template<class T> class Cholesky
{
public:
   //! Default, simple constructor.
   Cholesky(MatrixBase<T>& L) : m_L(L)
   {
      m_bSPD = false;
   }
   
   //! Decomposition of symmetric, positive semi-definite matrix A = L*L'.
   /*! The result is stored in a lower triangular matrix L, such that L*L'= A.
       If A is not symmetric positive-definite (SPD), only a
       partial factorization is performed.  If IsSPD()
       evalutates true then the factorizaiton was successful.
       
       \retval true if decomposition was successful. This also means that
               A was symmetric positive semi-definite.
       \retval false the decomposition failed. The matrix was not 
               symmetric and positive semi-definite.
    */ 
   bool Decompose(const MatrixBase<T>& A) 
   {
      // check the size
      assert(A.GetRowCount()==m_L.GetRowCount());
      assert(A.GetColCount()==m_L.GetColCount());
      // Check the address
      assert(&A != &m_L);
   
      const int n = A.GetRowCount();
      
      //m_L.SetAllTo((T)0.0);
      
      m_bSPD = true; // symmetric positive semi-definite

      // Main loop.
      for (int j = 0; j < n; j++) {
         // We keep double here even in the float case.
         // This helps to increase the precision of the float calculation. 
         double d = 0.0; 
         for (int k = 0; k < j; k++) {
            T s = 0.0;
            for (int i = 0; i < k; i++) {
               s += m_L[k][i]*m_L[j][i];
            }
            m_L[j][k] = s = (A[j][k] - s)/m_L[k][k];
            d = d + s*s;
            // is symmetric? 
            m_bSPD = m_bSPD && (A[k][j] == A[j][k]); 
         }
         d = A[j][j] - d;
         // is positive definite?
         m_bSPD = m_bSPD && (d > 0.0);
         
         m_L[j][j] = sqrt(d > 0.0 ? d : 0.0);
         for (int k = j+1; k < n; k++) {
            m_L[j][k] = 0.0;
         }
      }
      return true;
   }
   
   //! Solve a linear system A*X = B using previously calculated L (decompose).
   /*! Both matrices are given as parameters. The Cholesky class does not
       store anything. Rows of B must match rows of L. The result overwrites
       the B matrix.

       \param L Lower triangle matrix that was previously decomposed from the
                original matrix A.
       \param B On input, a Matrix B with as many rows as A and any number of columns.
                On output, X so that L*L'*X = B.
    */
   void Solve(MatrixBase<T>& B)
   {
      // Check the size
      assert(m_L.GetRowCount() == B.GetRowCount());
      // Check the address
      assert(&m_L != &B);
      
      const int n = m_L.GetRowCount();

      int nx = B.GetColCount();

      // Solve L*y = b;
      for (int j=0; j< nx; j++) {
         for (int k = 0; k < n; k++) {
            for (int i = 0; i < k; i++) 
               B[k][j] -= B[i][j]*m_L[k][i];
            B[k][j] /= m_L[k][k];
         }
      }

      // Solve L'*X = Y;
      for (int j=0; j<nx; j++) {
         for (int k = n-1; k >= 0; k--) {
            for (int i = k+1; i < n; i++) 
               B[k][j] -= B[i][j]*m_L[i][k];
            B[k][j] /= m_L[k][k];
         }
      }
   }
   
   //! Solve a linear system A*x = b using previously calculated L (decompose).
   /*! Size of vector b must match rows of L. The result overwrites
       the b vector.

       \param L Lower triangle matrix that was previously decomposed from the
                original matrix A.
       \param b On input, a vector b with as many rows as A.
                On output, x so that L*L'*x = b.
    */
   void Solve(VectorBase<T>& b)
   {
      // Check the size
      assert(m_L.GetRowCount() == b.GetSize());
      
      const int n = m_L.GetRowCount();

      // Solve L*y = b;
      for (int k = 0; k < n; k++) {
         for (int i = 0; i < k; i++) 
            b[k] -= b[i]*m_L[k][i];
         b[k] /= m_L[k][k];
      }

      // Solve L'*x = y;
      for (int k = n-1; k >= 0; k--) {
         for (int i = k+1; i < n; i++) 
            b[k] -= b[i]*m_L[i][k];
         b[k] /= m_L[k][k];
      }
   }

   //! Reports the status. True means symmetric positive definite
   bool IsSPD() const
   { return m_bSPD; }

	//! Copy the lower triangular part to the upper, to make it a full matrix.
	void CopyL2U() 
	{
      const int n = m_L.GetRowCount();
		for(int r=0; r<n-1; r++) {
			for(int c=r+1; c<n; c++) {
				m_L[r][c] = m_L[c][r];
			}
		}
	}

protected:
	//! Access to the decomposed L matrix. It is an external matrix!
   MatrixBase<T>& m_L;
   //! Status of the decomposition.
   bool m_bSPD;
};

} // namespace smx

#endif // __MAT_CHOLESKY_H
