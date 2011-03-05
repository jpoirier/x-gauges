/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2008 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   Licence: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include <QDebug>
#include <QDate>
#include <fstream>

#include "BestFit/AbstractBestFitLinear.h"

#include "GeoMag/GeoMagModel.h"
#include "JulianDay.h"

#include "MagSphereCalibration.h"

#define MIN_MAGU_RAW_DISTANCE 				30

namespace can {

// ------------------------------------------------------------------------

MagSphereCalibration::MagSphereCalibration()
{
	m_vS.SetAllTo(1.0f);
}

// ------------------------------------------------------------------------

int MagSphereCalibration::CreateRecord()
{
	int id = m_conRecord.count();
	m_conRecord.push_back(Record());
	qDebug() << "Magu record" << id << "was created.";
	return id;
}
// ------------------------------------------------------------------------

void MagSphereCalibration::UndoRecord()
{ 
	if(!m_conRecord.isEmpty()) 
		m_conRecord.pop_back(); 
}

// ------------------------------------------------------------------------

bool MagSphereCalibration::AddItem(int iRec, const V3f& v)
{
	Q_ASSERT(iRec >= 0 && iRec < m_conRecord.count());
	
	// Check all items in the record.
	// This prevents forming groups on one spot.
	bool bReject = false;
	for(int i=0; i<m_conRecord[iRec].count(); i++) {
		V3f x(v);
		x.Subtract(m_conRecord[iRec][i]);
		// Check if the new item is far enough from all other items in the record.
		if(x.GetNorm() < MIN_MAGU_RAW_DISTANCE) {
			bReject = true;
			break;
		}
	}
	if(bReject) 
		return false;
	
	// Ok, we can accept new item.
	m_conRecord[iRec].push_back(v);
//  	qDebug() << "vM =" << v[0] << v[1] << v[2];
	return true;
}

// ------------------------------------------------------------------------

bool MagSphereCalibration::AddItem(const V3f& v)
{
	return AddItem(m_conRecord.count()-1, v);
}
// ------------------------------------------------------------------------

using namespace bestfit;
class MagLinFit3D : public AbstractBestFitLinear
{
public:
	MagLinFit3D() : AbstractBestFitLinear(6,3) {};
	
	double CalcFunction(const Vec& vC, const common::VectorD& vD) const
	{
		smx::Vector<double, 6> vX;
		CalcX(vD, vX);
		return vC.Multiply(vX);
	}
	
protected:
	//! Calculate the "Xi" vector for best fit function for given vD data point.
	virtual void CalcX(const common::VectorD& vM, smx::VectorBase<double>& vX) const
	{
		ASSERT(vX.GetSize() == m_vC.GetSize());
		vX[0] = vM[1]*vM[1];	// my2 * A
		vX[1] = vM[2]*vM[2]; // mz2 * B
		vX[2] = vM[0]; 		// mx  * C
		vX[3] = vM[1];			// my  * D
		vX[4] = vM[2];			// my  * E
		vX[5] = 1.0;			//  1  * F
	}
	//! The yi value out of the data.
	double CalcY(const common::VectorD& vM) const
	{ return -vM[0]*vM[0]; }
};

// ------------------------------------------------------------------------

class MagLinFit2D : public AbstractBestFitLinear
{
public:
	MagLinFit2D() : AbstractBestFitLinear(4,2) {};
	
	double CalcFunction(const Vec& vC, const common::VectorD& vD) const
	{
		smx::Vector<double, 4> vX;
		CalcX(vD, vX);
		return vC.Multiply(vX);
	}
	
protected:
	//! Calculate the "Xi" vector for best fit function for given vD data point.
	virtual void CalcX(const common::VectorD& vM, smx::VectorBase<double>& vX) const
	{
		ASSERT(vX.GetSize() == m_vC.GetSize());
		vX[0] = vM[1]*vM[1];	// my2 * A
		vX[1] = vM[0]; 		// mx  * B
		vX[2] = vM[1];			// my  * C
		vX[3] = 1.0;			//  1  * D
	}
	//! The yi value out of the data.
	double CalcY(const common::VectorD& vM) const
	{ return -vM[0]*vM[0]; }
};

// ------------------------------------------------------------------------
/*
using namespace numeric;
class MagNonFit3D : public BestFit
{
public:
	//! Fit Compass constructor
	MagNonFit3D(double dR) : BestFit(6) { m_dR=dR;}
	//! The fit function. (vX are coefficients, and vData is x,y argument.)
	double CalcFitFunction(const Vec& vX, const common::VectorD& vD) const
	{
		const double& mx = vD[0];
		const double& my = vD[1];
		const double& mz = vD[2];
		const double& bx = vX[0];
		const double& by = vX[1];
		const double& bz = vX[2];
		const double& sx = vX[3];
		const double& sy = vX[4];
		const double& sz = vX[5];
		
		return -m_dR*m_dR + pow((mx-bx)/sx, 2) + pow((my-by)/sy, 2) + pow((mz-bz)/sz, 2);
	}
	//! The derivatives over coefficients.
	void CalcFitDerivative(const Vec& vX, const common::VectorD& vD, Vec& vd) const
	{
		const double& mx = vD[0];
		const double& my = vD[1];
		const double& mz = vD[2];
		const double& bx = vX[0];
		const double& by = vX[1];
		const double& bz = vX[2];
		const double& sx = vX[3];
		const double& sy = vX[4];
		const double& sz = vX[5];
		
		vd.Resize(m_uiUnknowns);

		vd[0] = 2*(bx-mx)/(sx*sx);
		vd[1] = 2*(by-my)/(sy*sy);
		vd[2] = 2*(bz-mz)/(sz*sz);
		vd[3] = (-2*pow(bx-mx,2))/pow(sx,3);
		vd[4] = (-2*pow(by-my,2))/pow(sy,3);
		vd[5] = (-2*pow(bz-mz,2))/pow(sz,3);
	}
protected:
	//! Radius
	double m_dR;
};

// ------------------------------------------------------------------------

class MagNonFit2D : public BestFit
{
public:
	//! Fit Compass constructor
	MagNonFit2D(double dR) : BestFit(4) { m_dR=dR;}
	//! The fit function. (vX are coefficients, and vData is x,y argument.)
	double CalcFitFunction(const Vec& vX, const common::VectorD& vD) const
	{
		const double& mx = vD[0];
		const double& my = vD[1];
		const double& bx = vX[0];
		const double& by = vX[1];
		const double& sx = vX[2];
		const double& sy = vX[3];
		
		return -m_dR*m_dR + pow((mx-bx)/sx, 2) + pow((my-by)/sy, 2);
	}
	//! The derivatives over coefficients.
	void CalcFitDerivative(const Vec& vX, const common::VectorD& vD, Vec& vd) const
	{
		const double& mx = vD[0];
		const double& my = vD[1];
		const double& bx = vX[0];
		const double& by = vX[1];
		const double& sx = vX[2];
		const double& sy = vX[3];
		
		vd.Resize(m_uiUnknowns);

		vd[0] = 2*(bx-mx)/(sx*sx);
		vd[1] = 2*(by-my)/(sy*sy);
		vd[2] = (-2*pow(bx-mx,2))/pow(sx,3);
		vd[3] = (-2*pow(by-my,2))/pow(sy,3);
	}
protected:
	//! Radius
	double m_dR;
};

// ------------------------------------------------------------------------

class MagNonFitSqrt : public BestFit
{
public:
	//! Fit Compass constructor
	MagNonFitSqrt(double dR) : BestFit(6) { m_dR=dR;}
	//! The fit function. (vX are coefficients, and vData is x,y argument.)
	double CalcFitFunction(const Vec& vX, const common::VectorD& vD) const
	{
		const double& mx = vD[0];
		const double& my = vD[1];
		const double& mz = vD[2];
		const double& bx = vX[0];
		const double& by = vX[1];
		const double& bz = vX[2];
		const double& sx = vX[3];
		const double& sy = vX[4];
		const double& sz = vX[5];
		
		return -m_dR + sqrt(pow((mx-bx)/sx, 2) + pow((my-by)/sy, 2) + pow((mz-bz)/sz, 2));
	}
	//! The derivatives over coefficients.
	void CalcFitDerivative(const Vec& vX, const common::VectorD& vD, Vec& vd) const
	{
		const double& mx = vD[0];
		const double& my = vD[1];
		const double& mz = vD[2];
		const double& bx = vX[0];
		const double& by = vX[1];
		const double& bz = vX[2];
		const double& sx = vX[3];
		const double& sy = vX[4];
		const double& sz = vX[5];
		
		vd.Resize(m_uiUnknowns);
		double A = sqrt(pow((mx-bx)/sx, 2) + pow((my-by)/sy, 2) + pow((mz-bz)/sz, 2));

		vd[0] = 2*(bx-mx)/(sx*sx*A);
		vd[1] = 2*(by-my)/(sy*sy*A);
		vd[2] = 2*(bz-mz)/(sz*sz*A);
		vd[3] = (-2*pow(bx-mx,2))/(A*pow(sx,3));
		vd[4] = (-2*pow(by-my,2))/(A*pow(sy,3));
		vd[5] = (-2*pow(bz-mz,2))/(A*pow(sz,3));
	}
protected:
	//! Radius
	double m_dR;
};
*/
// ------------------------------------------------------------------------

bool MagSphereCalibration::Solve(float fLon, float fLat, float fAlt_km)
{
	using namespace common;
	
	// We need the geomag model for today and given geographic position.
	GeoMagModel gmm;
	JulianDay jd(QDate::currentDate().toJulianDay());
	gmm.SetDate(jd.GetDateAsDecimalYear());
	gmm.Evaluate(fLat, fLon, fAlt_km);
	// We need total radius and xy-plane radius.
	float fRxy = sqrt(pow(gmm.GetX(),2) + pow(gmm.GetY(),2));
	float fR, fI, fD;
	gmm.CalcIDH(fI, fD, fR);
	// Show some results
	qDebug() << "Total Earth magnetic field strenth"    << fR;
	qDebug() << "XY plane Earth magnetic field strenth" << fRxy;
	qDebug() << "Inclination =" << Deg(fI) << ", declination =" << Deg(fD);

	VectorD vX(3);
	MagLinFit3D mlf3D;
	MagLinFit2D mlf2D;
//	MagNonFit2D mnf2D(fRxy);

	// Merge the data and output the mathematica version
	std::ofstream out("MagSphereData.txt");
	for(int r=0; r<m_conRecord.count(); r++) {
		out << "\nPts" << r+1 << " = {\n";
		for(int i=0; i<m_conRecord[r].count(); i++) {
			mlf3D.AddData(m_conRecord[r][i]);
			// The first data record MUST be the xy plane. We take x and y only. 
			if(r==0) {
				mlf2D.AddData(
					m_conRecord[r][i][0], // x
					m_conRecord[r][i][1]  // y
				);
/*				mnf2D.AddData(
					m_conRecord[r][i][0], // x
					m_conRecord[r][i][1]  // y
				);*/
			}
			
			out << "{" 
				 << m_conRecord[r][i][0] << "," 
				 << m_conRecord[r][i][1] << ","
				 << m_conRecord[r][i][2];
			if(i==m_conRecord[r].count()-1)
				out << "}";
			else
				out << "},";
		}
		out << "};\n";
	}
	out.close();
	
	// The linear LSM 
	bool bSuccess3D = mlf3D.Solve();
	// Convert resulting LMS parameters into meaningful 3D results.
	const smx::VectorBase<double>& vC = mlf3D.GetParameters();
	double fBx3, fBy3, fBz3, fSx3, fSy3, fSz3;
	
	fBx3 = -vC[2]/2;
	fBy3 = -vC[3]/(2*vC[0]);
	fBz3 = -vC[4]/(2*vC[1]);
	
	fSx3 = (fBx3*fBx3 + vC[0]*fBy3*fBy3 + vC[1]*fBz3*fBz3 - vC[5])/(fR*fR);
	fSy3 = fSx3/vC[0];
	fSz3 = fSx3/vC[1];
	
	fSx3 = sqrt(fSx3);
	fSy3 = sqrt(fSy3);
	fSz3 = sqrt(fSz3);
	
	// Now, get the 2D linear LSM solution.
	bool bSuccess2D = mlf2D.Solve();
	// Convert resulting parameters into 2D results.
	const smx::VectorBase<double>& vE = mlf2D.GetParameters();
	double fBx2, fBy2, fSx2, fSy2;
	
	fBx2 = -vE[1]/2;
	fBy2 = -vE[2]/(2*vE[0]);
	
	fSx2 = (fBx2*fBx2 + vE[0]*fBy2*fBy2 - vE[3])/(fRxy*fRxy);
	fSy2 = fSx2/vE[0];
	
	fSx2 = sqrt(fSx2);
	fSy2 = sqrt(fSy2);
	
	// Store results in a bit unusual way.
	// x and y values are taken from 2D solution, while the z values are
	// taken from 3D solution.
	m_vB[0] = fBx2;
	m_vB[1] = fBy2;
	m_vB[2] = fBz3;
	m_vS[0] = fSx2;
	m_vS[1] = fSy2;
	m_vS[2] = fSz3;
	qDebug() << "Linear 3D solution:" 
				<< fBx3 << fBy3 << fBz3 << fSx3 << fSy3 << fSz3;
	qDebug() << "Linear 2D solution:"
				<< fBx2 << fBy2 << fSx2 << fSy2;
	
	// Nonliear 2D solution
/*	BestFit::Vec vY2(4);
	vY2[0] = fBx3;
	vY2[1] = fBy3;
	vY2[2] = fSx3;
	vY2[3] = fSy3;
	FitSolverLevenbergMarquard fslm2;
	FitSolverLevenbergMarquard::StoppingCriterion sc;
	sc = fslm2.Solve(mnf2D, vY2);

	qDebug() << "Nonlinear 2D solution:"
				<< vY2[0] << vY2[1] << vY2[2] << vY2[3];
	m_vB[0] = vY2[0];
	m_vB[1] = vY2[1];
	m_vS[0] = vY2[2];
	m_vS[1] = vY2[3];
	
	// Nonlinear solutions
	MagNonFit3D mnf(fR);

	// Kontrola
	const AbstractBestFit::Data& mD = mlf3D.GetData();
	for(unsigned int i=0; i<mD.size(); i++) {
		float fmx = mD[i][0];
		float fmy = mD[i][1];
		float fmz = mD[i][2];
		
		fmx = (fmx - fBx3)/fSx3;
		fmy = (fmy - fBy3)/fSy3;
		fmz = (fmz - fBz3)/fSz3;
		
		float fRC = sqrt(fmx*fmx + fmy*fmy + fmz*fmz);
		mnf.AddData(mD[i]);
	}
	
	qDebug() << "Second stage" << mnf.GetData().size() << "items";
	BestFit::Vec vY(6);
	vY[0] = fBx3;
	vY[1] = fBy3;
	vY[2] = fBz3;
	vY[3] = fSx3;
	vY[4] = fSy3;
	vY[5] = fSz3;
	FitSolverLevenbergMarquard fslm;
	//FitSolverLevenbergMarquard::StoppingCriterion sc;
	sc = fslm.Solve(mnf, vY);

	qDebug() << "Nonlinear 3D solution:"
				<< vY[0] << vY[1] << vY[2] << vY[3] << vY[4] << vY[5];*/
/*	m_vB[0] = vY[0];
	m_vB[1] = vY[1];
	m_vB[2] = vY[2];
	m_vS[0] = vY[3];
	m_vS[1] = vY[4];
	m_vS[2] = vY[5];*/
	
	// Kontrola
/*	const BestFit::Data& mD2 = mnf.GetData();
	for(unsigned int i=0; i<mD2.size(); i++) {
		float fmx = mD2[i][0];
		float fmy = mD2[i][1];
		float fmz = mD2[i][2];
		
		fmx = (fmx - fBx3)/fSx3;
		fmy = (fmy - fBy3)/fSy3;
		fmz = (fmz - fBz3)/fSz3;
		
		float fRC = sqrt(fmx*fmx + fmy*fmy + fmz*fmz);
// 		qDebug() << i << ":" << fmx << fmy << fmz << fRC << fabs(fR-fRC);
	}*/
		
/*	qDebug() << "Final solution:" 
				<< m_vB[0] << m_vB[1] << m_vB[2]
				<< m_vS[0] << m_vS[1] << m_vS[2];*/
				
	return bSuccess3D && bSuccess2D;
}

// ------------------------------------------------------------------------

void MagSphereCalibration::VerifyItem(
	const V3f& v, //!< Raw magnetic field sensor reading.
	float fRoll,  //!< Roll angle
	float fPitch  //!< Pitch angle
)
{
	// Get "true" magnetic vector.
	V3f m = v;
	m.Subtract(m_vB);
	for(int i=0; i<3; i++)
		m[i]/=m_vS[i];
	
	// Calculate heading - use pitch and roll correction.
	// Convert magnetic sensor values into xy plane first.
	float fSinR, fCosR;	// Roll
	float fSinP, fCosP;	// Pitch

// XXX:
//	sincosf(fRoll, &fSinR, &fCosR);
//	sincosf(fPitch, &fSinP, &fCosP);

	// We will uses sensor values ...
	float fXh = m[0]*fCosP + m[1]*fSinR*fSinP + m[2]*fCosR*fSinP;
	float fYh =              m[1]*fCosR       - m[2]*fSinR;

	qDebug("Xh = %f, Yh = %f", fXh, fYh);

	// Now, calculate heading ...
	float fYaw = -common::ATan2Safe(m[1],m[0]);
	if(fYaw < 0)
		fYaw += M_PI*2;

	qDebug("Yaw = %f", common::Deg(fYaw));
}
// ------------------------------------------------------------------------

} // namespace
