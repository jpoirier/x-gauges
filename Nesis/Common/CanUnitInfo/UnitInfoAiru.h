#ifndef UNIT_INFO_AIRU_H
#define UNIT_INFO_AIRU_H

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

namespace can {

// ------------------------------------------------------------------------

class UnitInfoAiru : public UnitInfoBase
{
public:
	//! Constructor
	UnitInfoAiru(uint8_t byNodeId=0, uint32_t uiSerial=0, common::JulianDay jdDate=common::JulianDay());
	//! Copy constructor
	UnitInfoAiru(const UnitInfoBase& C);
	//! Destructor
   virtual ~UnitInfoAiru();

	//! Set value of the mount Euler correction coefficient.
	void SetMountCorrectionCoefficients(float fRoll, float fPitch, float fYaw)
	{ 
		m_vCoeffMountCorrection[0] = fRoll; 
		m_vCoeffMountCorrection[1] = fPitch; 
		m_vCoeffMountCorrection[2] = fYaw; 
	}
	//! Get value of the mount Euler coefficient.
	float GetMountCorrectionCoefficient(int i) const
	{ return m_vCoeffMountCorrection[i]; }

	//! Get flash value of the mount Euler coefficient.
	float GetMountCorrectionCoefficientFlash(int i) const
	{ return m_vCoeffMountCorrectionFlash[i]; }
	
	#if defined (USE_CAN_DUS_A)
	//! Upload of Mount Correcton Flash coefficients (read from Airu).
	bool UploadMountCorrectionFlash(can::AbstractAerospaceDataManager* pADM);
	#endif
	
	#if defined (USE_CAN_DDS_A)
	//! Download Mount Correcton coefficients (write to Airu).
	bool DownloadMountCorrection(can::AbstractAerospaceDataManager* pADM);
	#endif

protected:
	//! Setup download buffer.
	virtual bool LoadDownloadBuffer(uint32_t uiDwnDataId);

protected:
	//! Mount correction Euler (roll, pitch, yaw) coefficients stored in flash.
	QVector<float> m_vCoeffMountCorrectionFlash;
	//! Mount correction Euler (roll, pitch, yaw) coefficients.
	QVector<float> m_vCoeffMountCorrection;

};

// --------------------------------------------------------------------------

} // namespace

#endif
