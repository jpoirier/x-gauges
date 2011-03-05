#ifndef __QUATERNION_H
#define __QUATERNION_H

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
 
/*! \file Quaternion.h 
    \brief Quaternion algebra, which represents rotations in 3D space. 

    \author Ales Krajnc 
    \date   July 2006
 */

// --------------------------------------------------------------------

#include "Matrix.h"
#include "MathEx.h"

// --------------------------------------------------------------------

//! Simple MatriX namespace
namespace smx {

#define Q Vector<T,4>::m_V 

// --------------------------------------------------------------------
//! The class implements some basic quaternion algebra used in our projects.
/*! Quaternion class is derived from the Vector class with four elements.
	 The class specializes vector of four elements and adds several member
    functions that are used for conversion to/from DCM and Euler angles,
	 as well as some basic quaternion algebra.
 */
template<class T>
class Quaternion : public Vector<T, 4>
{
	enum Index { w=0, x, y, z };
public:
	//! Default constructor. It defines unit quaternion.
	Quaternion() 
	{
		Q[w] = 1.0; 
	}

	//! Constructor takes arguments directly.
	Quaternion(T w, T x, T y, T z) 
	{
		Set(w,x,y,z);
	}

	//! Constructor takes Euler angles directly.
	Quaternion(T fRoll, T fPitch, T fYaw) 
	{
		SetEuler(fRoll, fPitch, fYaw);
	}

	//! Constructor takes Euler angles in vector form directly.
	Quaternion(const Vector<T, 3>& vEuler) 
	{
		SetEuler(vEuler);
	}

   //! Virtual destructor. Does nothing, but makes the compiler happy.
   virtual ~Quaternion()
   {
   }
   
   //! Copy constructor
   Quaternion(const Quaternion<T>& C)
	: Vector<T, 4>()
   {
      Copy(C);
   }

   //! Assignment operator. Same as copy.
	const Quaternion<T>& operator=(const Quaternion<T>& C) 
	{
      Copy(C);
		return *this;
	}

	//! Sets values of the Quaternion directly.
	void Set(T fw, T fx, T fy, T fz)
	{ 
		Q[w] = fw;
		Q[x] = fx;
		Q[y] = fy;
		Q[z] = fz;
	}

	//! Calculates (gets) a DCM from current values of quaternion.
	void GetDCMBaseToEarth(Matrix<T,3,3>& mDCM) const
   {
      T q0_2 = Q[w]*Q[w];
      T q1_2 = Q[x]*Q[x];
      T q2_2 = Q[y]*Q[y];
      T q3_2 = Q[z]*Q[z];
      
      T q01 = Q[w]*Q[x];
      T q02 = Q[w]*Q[y];
      T q03 = Q[w]*Q[z];
      T q12 = Q[x]*Q[y];
      T q13 = Q[x]*Q[z];
      T q23 = Q[y]*Q[z];
      
      mDCM[0][0] = q0_2 + q1_2 - q2_2 - q3_2;
      mDCM[1][0] = 2*(q12 + q03);
      mDCM[2][0] = 2*(-(q02) + q13);
      
      mDCM[0][1] = 2*(q12 - q03);
      mDCM[1][1] = q0_2 - q1_2 + q2_2 - q3_2;
      mDCM[2][1] = 2*(q01 + q23);
       
      mDCM[0][2] = 2*(q02 + q13);
      mDCM[1][2] = 2*(-(q01) + q23);
      mDCM[2][2] = q0_2 - q1_2 - q2_2 + q3_2;
   }  
   
   //! Calculates (gets) a DCM from current values of quaternion.
   void GetDCMEarthToBase(Matrix<T,3,3>& mDCM) const
   {
      T q0_2 = Q[w]*Q[w];
      T q1_2 = Q[x]*Q[x];
      T q2_2 = Q[y]*Q[y];
      T q3_2 = Q[z]*Q[z];
      
      T q01 = Q[w]*Q[x];
      T q02 = Q[w]*Q[y];
      T q03 = Q[w]*Q[z];
      T q12 = Q[x]*Q[y];
      T q13 = Q[x]*Q[z];
      T q23 = Q[y]*Q[z];
      
      mDCM[0][0] = q0_2 + q1_2 - q2_2 - q3_2;
      mDCM[0][1] = 2*(q12 + q03);
      mDCM[0][2] = 2*(-(q02) + q13);
     
      mDCM[1][0] = 2*(q12 - q03);
      mDCM[1][1] = q0_2 - q1_2 + q2_2 - q3_2;
      mDCM[1][2] = 2*(q01 + q23);
       
      mDCM[2][0] = 2*(q02 + q13);
      mDCM[2][1] = 2*(-(q01) + q23);
      mDCM[2][2] = q0_2 - q1_2 - q2_2 + q3_2;
   }  

	//! Calculates (gets) Euler angles from quaternion.
	void GetEuler(T& fRoll, T& fPitch, T& fYaw) const
	{
		fRoll  = common::ATan2Safe(
			2*(Q[y]*Q[z] + Q[w]*Q[x]), 
			(Q[w]*Q[w] + Q[z]*Q[z] - Q[x]*Q[x] - Q[y]*Q[y]) 
		);
		fPitch = common::ASinSafe(-2*(Q[x]*Q[z]-Q[w]*Q[y]));
		fYaw   = common::ATan2Safe(
			2*(Q[x]*Q[y] + Q[w]*Q[z]) , 
			(Q[w]*Q[w] + Q[x]*Q[x] - Q[y]*Q[y] - Q[z]*Q[z]) 
		);
      if(fYaw < 0)
         fYaw += 2*M_PI;
/*		fRoll  = atan2(
			   2*(Q[y]*Q[z] + Q[w]*Q[x]), 
			(1-2*(Q[x]*Q[x] + Q[y]*Q[y])) 
		);
		fPitch = asin(-2*(Q[x]*Q[z]-Q[w]*Q[y]));
		fYaw   = atan2(
			   2*(Q[x]*Q[y] + Q[w]*Q[z]) , 
			(1-2*(Q[y]*Q[y] + Q[z]*Q[z])) 
		);
      if(fYaw < 0)
         fYaw += 2*M_PI;*/
	}

	//! Get Roll from the quaternion.
	T GetRoll() const
	{ 
		return common::ATan2Safe(
			   2*(Q[y]*Q[z] + Q[w]*Q[x]), 
				(Q[w]*Q[w] + Q[z]*Q[z] - Q[x]*Q[x] - Q[y]*Q[y]) 
		);
/*		return atan2(
			   2*(Q[y]*Q[z] + Q[w]*Q[x]), 
			(1-2*(Q[x]*Q[x] + Q[y]*Q[y])) 
		);*/
	}

	//! Get Pitch from the quaternion.
	T GetPitch() const
	{ 
		return common::ASinSafe(-2*(Q[x]*Q[z]-Q[w]*Q[y]));
	}

	//! Get Yaw (heading) from the quaternion.
	T GetYaw() const
	{
		float fYaw   = common::ATan2Safe(
		   2*(Q[x]*Q[y] + Q[w]*Q[z]) , 
			(Q[w]*Q[w] + Q[x]*Q[x] - Q[y]*Q[y] - Q[z]*Q[z]) 
		);
/*		float fYaw   = atan2(
			   2*(Q[x]*Q[y] + Q[w]*Q[z]) , 
			(1-2*(Q[y]*Q[y] + Q[z]*Q[z])) 
		);*/
      if(fYaw < 0)
         fYaw += 2*M_PI;
		return fYaw;
	}
     
   //! Returns Euler angles from quaternion.
   smx::Vector<T,3> GetEuler() const
   {
      smx::Vector<T,3> vE;
      GetEuler(vE[0], vE[1], vE[2]);
      return vE;
   }

	//! Sets quaternion from given Euler angles. Angles are in radians.
	/*! The rotation follows the yaw, pitch, roll sequence.
	 */
	void SetEuler(T fRoll, T fPitch, T fYaw)
	{
		fPitch /= 2.0;
		fRoll  /= 2.0;
		fYaw   /= 2.0;

		T fCP = cos(fPitch);
		T fCR = cos(fRoll);
		T fCY = cos(fYaw);
		T fSP = sin(fPitch);
		T fSR = sin(fRoll);
		T fSY = sin(fYaw);

		Q[w] = fCP*fCR*fCY + fSP*fSR*fSY;
		Q[x] = fCP*fCY*fSR - fCR*fSP*fSY;
		Q[y] = fCR*fCY*fSP + fCP*fSR*fSY;
		Q[z] = fCP*fCR*fSY - fCY*fSP*fSR; 

		assert(VectorBase<T>::GetNorm()>0.99999 && VectorBase<T>::GetNorm()<1.00001);
	}
   
   //! Sets quaternion from given Euler angles. Angles are in radians.
   void SetEuler(const Vector<T,3>& vE)
   {
      SetEuler(vE[0], vE[1], vE[2]);
   }

	//! Set quaternion from given rotation vector.
   /*! First compute the angle from the magnitude of the vector, then find
       the rotation axis (unit vector) and then convert the angle and the
       unit vector into the quaternion.
    */
	void SetRotationVector(const Vector<T,3>& vR)
	{
		T fAngle = vR.GetNorm(); // Euclidian norm
		if(fAngle == 0) {
			Set(1,0,0,0);
		}
		else {
			T fSinA  = sin(fAngle/2.0) / fAngle;
			Set(cos(fAngle/2.0), vR[0]*fSinA, vR[1]*fSinA, vR[2]*fSinA);
		}
	}

	//! Set quaternion from angular rates and given time.
	/*! A rotation vector is created from angular rate vector which is 
       assumed to be constant over given fdT time. The rotation vector is
       stored in a quaternion form and may be used to rotate quaternions
       using quaternion multiplication.
    */ 
	void SetAngularRateTime(
		T fP, 		//!< angular rate around the roll axis 
		T fQ, 		//!< angular rate around the pitch axis
		T fR, 		//!< angular rate around the yaw axis
		T fT	 		//!< the integration time interval
	)
	{
		T fNorm = sqrt(fP*fP + fQ*fQ + fR*fR);
		T fA_2  = fNorm*fT*0.5;
		T fsA;
		T fcA;
		// True
  		if(fA_2 > 0.005) {
			fsA = sin(fA_2)/fNorm;
			fcA = cos(fA_2);
		}
		// First order for small angles
		else {
			fsA = fT*0.5;
			fcA = 1.0-fA_2*fA_2*0.5;
		}
		Set(fcA, fsA*fP, fsA*fQ, fsA*fR);
/*		T fNorm = sqrt(fP*fP + fQ*fQ + fR*fR);
		T fA    = fNorm*fT;
		T fsA2  = fA > 0 ? sin(fA/2)/fNorm : 0.0;
		Set(cos(fA/2.0), fsA2*fP, fsA2*fQ, fsA2*fR);*/
	}

	void SetAngularRateTime(
		const Vector<T,3>& vPQR, 	//!< angular rate vector
		T fT	 							//!< the integration time interval
	)
	{
		SetAngularRateTime(vPQR[0], vPQR[1], vPQR[2], fT);
	}

	//! Invert - calculate the inverse quaternion on self.
	void Invert()
	{ 
		Q[x] = -Q[x]; 
		Q[y] = -Q[y]; 
		Q[z] = -Q[z]; 
	} 

	//! Multiplication of A and B. The result is stored in this.
	void Multiply(
		const Quaternion<T>& A, 
		const Quaternion<T>& B
	) 
	{
		assert(this != &A && this != &B);
		Q[w] = (A.Q[w]*B.Q[w] - A.Q[x]*B.Q[x] - A.Q[y]*B.Q[y] - A.Q[z]*B.Q[z]);
 		Q[x] = (A.Q[w]*B.Q[x] + A.Q[x]*B.Q[w] + A.Q[y]*B.Q[z] - A.Q[z]*B.Q[y]);
 		Q[y] = (A.Q[w]*B.Q[y] - A.Q[x]*B.Q[z] + A.Q[y]*B.Q[w] + A.Q[z]*B.Q[x]);
 		Q[z] = (A.Q[w]*B.Q[z] + A.Q[x]*B.Q[y] - A.Q[y]*B.Q[x] + A.Q[z]*B.Q[w]);
	}

	//! Updates this quaternion exposed to angular velocity for given time.
	/*! The update is done using the fourth order Runge-Kutta integration.
		 The angular velocities are stored in the Om(4,4) matrix, which
       is post multiplied with this quaternion. The PDE to solve is

		     dQ/dt = Omega * Q

		 It is assumed that the angular velocities are constant during
       fT period.
    */
	void IntegrateAngularRateRK(
		T fP, 		//!< angular rate around the roll axis 
		T fQ, 		//!< angular rate around the pitch axis
		T fR, 		//!< angular rate around the yaw axis
		T fT, 		//!< the integration time interval
		int iSteps=1//!< number of internal interval divisions
	) 
	{
		assert(iSteps>0);

		// Create the omega matrix used during the integration.
		Matrix<T,4,4> Om;
		fP /= 2.0;	
		fQ /= 2.0;	
		fR /= 2.0;	
	
							 Om[0][1] = -fP; Om[0][2] = -fQ; Om[0][3] = -fR;
		Om[1][0] =  fP;                 Om[1][2] =  fR; Om[1][3] = -fQ;
		Om[2][0] =  fQ; Om[2][1] = -fR;                 Om[2][3] =  fP;
		Om[3][0] =  fR; Om[3][1] =  fQ; Om[3][2] = -fP;
	
		// just to be sure
		assert(Om(0,0)==0);
		assert(Om(1,1)==0);
		assert(Om(2,2)==0);
		assert(Om(3,3)==0);

		// split the time interval according to the given number of steps
		fT /= iSteps;
		for(int j=0; j<iSteps; j++) {
			// The Qrk quarternion will hold the result of integration
			// Runge-Kutta starts here
			Quaternion Qrk;
			Qrk = *this;

			// Helper vectors
			smx::Vector<T,4> ka, kb;

			// k1
			Om.PostMultiply(*this,ka);
			ka.Multiply(fT); // k1
			//printf("K1 = "); ka.Dump();

			// k2
			kb = ka;
			kb.Multiply(1./2.);
			kb.Add(*this);
			ka.Multiply(1./6.);
			Qrk.Add(ka); // Update rk with k1
			Om.PostMultiply(kb,ka);
			ka.Multiply(fT); // = k2
			//printf("K2 = "); ka.Dump();

			// k3
			kb = ka;
			kb.Multiply(1./2.);
			kb.Add(*this);
			ka.Multiply(1./3.);
			Qrk.Add(ka); // Update rk with k2
			Om.PostMultiply(kb,ka);
			ka.Multiply(fT); // = k3
			//printf("K3 = "); ka.Dump();
	
			// k4
			kb = ka;
			kb.Add(*this);
			ka.Multiply(1./3.);
			Qrk.Add(ka); // Update rk with k3
			Om.PostMultiply(kb,ka);
			ka.Multiply(fT/6.); // = k4 + update factor
			//printf("K4 = "); ka.Dump();

			// Final update
			//	ka.Multiply(1./6.); // = k4 + update factor
			Qrk.Add(ka);

			// Update the quarternion
			*this = Qrk;
		} 
	}

	//! Updates this quaternion exposed to angular velocity for given time.
	/*! The update is done using quaternion multiplication. First a rotation
       quaternion is constructed from the angular rate vector and than
       current quaternion is rotated around angular rate vector.
    */

	void IntegrateAngularRate(
		T fP, 		//!< angular rate around the roll axis 
		T fQ, 		//!< angular rate around the pitch axis
		T fR, 		//!< angular rate around the yaw axis
		T fT 			//!< the integration time interval
	)
	{
		Quaternion<T> qR;
		qR.SetAngularRateTime(fP,fQ,fR,fT);
		Multiply(Quaternion<T>(*this),qR);
	}

	//! Updates this quaternion exposed to angular velocity for given time.
	void IntegrateAngularRate(
		const Vector<T,3>& vPQR, 	//!< angular rates
		T fT 								//!< the integration time interval
	)
	{
		IntegrateAngularRate(vPQR[0], vPQR[1], vPQR[2], fT);
	}

};
#undef Q

} // namespace smx

//! Short definition for the Quaternion of floats.
typedef smx::Quaternion<float> Qf;

#endif // __QUATERNION_H
