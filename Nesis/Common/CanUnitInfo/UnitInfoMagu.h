#ifndef UNIT_INFO_MAGU_H
#define UNIT_INFO_MAGU_H

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

#include "UnitInfoBase.h"
#include "MagSphereCalibration.h"

namespace can {

// ------------------------------------------------------------------------

class UnitInfoMagu : public UnitInfoBase
{
public:
	//! Constructor
	UnitInfoMagu(uint8_t byNodeId=0, uint32_t uiSerial=0, common::JulianDay jdDate=common::JulianDay());
	//! Copy constructor
	UnitInfoMagu(const UnitInfoBase& C);
	//! Destructor
   virtual ~UnitInfoMagu();

	//! Process the Magu specific responses.
	void ProcessModuleConfigResponse(const can::MessageAerospace& msg);
	
	//! Get calibration points count.
	int GetCalibrationPointsCount() const
	{ return m_iCalPointsCount; }
	//! Reset calibration points count.
	void ResetCalibrationPointsCount()
	{ m_iCalPointsCount = 0; }
	
	//! Get finish result.
	int GetFinishResult() const
	{ return m_iCalFinishResult; }
	//! Reset finish result.
	void ResetFinishResult() 
	{ m_iCalFinishResult = -999; }
	
	//! Are results valid (for display)
	bool AreResultsValid() const
	{ return m_vCoeff.count()==5 
			&& m_vData.count() >= 15 
			&& m_vData.count()%3==0; 
	}
	
	#if defined (USE_CAN_DUS_A)
	//! Get calibration coefficients from Magu.
	bool UploadCalibration(can::AbstractAerospaceDataManager* pADM);
	//! Get data coefficients from Magu.
	bool UploadData(can::AbstractAerospaceDataManager* pADM);
	//! Get data coefficients from Magu Flash.
	bool UploadCalibrationFlash(can::AbstractAerospaceDataManager* pADM);
	#endif
	
	//! Get coefficient
	float GetCoefficient(int i) const
	{ Q_ASSERT(i<m_vCoeff.count()); return m_vCoeff[i]; }

	//! Get calibration data (reference=0, measured=1, calibrated=2).
	float GetCalibrationData(int r, int c) const
	{	Q_ASSERT(r*3+c < m_vData.count()); 	return m_vData[r*3+c]; }

	//! Get flash coefficient data.
	float GetCoefficientFlash(int r, int c) const
	{ Q_ASSERT(r*5+c < m_vCoeffFlash.count()); return m_vCoeffFlash[r*5+c]; }
	
	//! Allow access to the 3D magnetic sphere calibration object.
	MagSphereCalibration& GetSphereCalibration()
	{ return m_msc; }
	//! Allow const access to the 3D magnetic sphere calibration object.
	const MagSphereCalibration& GetSphereCalibration() const
	{ return m_msc; }
	
protected:
	//! Setup download buffer.
	virtual bool LoadDownloadBuffer(uint32_t /*uiDwnDataId*/);

protected:
	//! Number of calibration points (data) in the magu memory.
	int m_iCalPointsCount;
	//! The result of the calibration solver.
	int m_iCalFinishResult;
	
	//! Calibration coefficients
	QVector<float> m_vCoeff;
	//! Calibration coefficients in flash.
	QVector<float> m_vCoeffFlash;
	//! Calibration data
	QVector<float> m_vData;
	
	//! 3D Magnetic calibration object for collecting data.
	MagSphereCalibration m_msc;
};

// --------------------------------------------------------------------------

} // namespace

#endif
