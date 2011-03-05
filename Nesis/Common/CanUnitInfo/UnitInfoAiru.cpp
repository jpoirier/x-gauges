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

#include "UnitInfoAiru.h"

namespace can {

// ------------------------------------------------------------------------

UnitInfoAiru::UnitInfoAiru(
	uint8_t byNodeId, 
	uint32_t uiSerial, 
	common::JulianDay jdDate
)
: UnitInfoBase(byNodeId, KHU_AIRU, uiSerial, jdDate)
{
	m_vCoeffMountCorrection.resize(3);
	m_vCoeffMountCorrectionFlash.resize(3);
	m_vCoeffMountCorrection.fill(0);
	m_vCoeffMountCorrectionFlash.fill(0);
}

UnitInfoAiru::UnitInfoAiru(const UnitInfoBase& C)
: UnitInfoBase(C)
{
	m_byHardwareId = KHU_AIRU;
	m_vCoeffMountCorrection.resize(3);
	m_vCoeffMountCorrectionFlash.resize(3);
	m_vCoeffMountCorrection.fill(0);
	m_vCoeffMountCorrectionFlash.fill(0);
}

UnitInfoAiru::~UnitInfoAiru()
{
}

// ------------------------------------------------------------------------
#if defined (USE_CAN_DUS_A)

bool UnitInfoAiru::UploadMountCorrectionFlash(can::AbstractAerospaceDataManager* pADM)
{
	return Upload(pADM, UPLOAD_AIRU_MOUNT_CORR_COEFF_FLASH, m_vCoeffMountCorrectionFlash);
}

#endif
// ------------------------------------------------------------------------

bool UnitInfoAiru::LoadDownloadBuffer(uint32_t uiDwnDataId)
{
	if(uiDwnDataId == DDS_AIRU_MOUNT_CORR_COEFF) {
		m_vDownloadBuffer.resize(m_vCoeffMountCorrection.count());
		for(int i=0; i< m_vCoeffMountCorrection.count(); i++) {
			m_vDownloadBuffer[i].f = m_vCoeffMountCorrection[i];
			qDebug() << "f=" << m_vDownloadBuffer[i].f;
		}
		qDebug() << "Ok, size =" << m_vDownloadBuffer.count() << m_vCoeffMountCorrection.count();
		m_eDataType = can::dtFloat;
		return true;
	}
	qDebug() << "Failed";
	return false;
}

// ------------------------------------------------------------------------
#if defined (USE_CAN_DDS_A)

bool UnitInfoAiru::DownloadMountCorrection(can::AbstractAerospaceDataManager* pADM)
{
	return Download(pADM, DDS_AIRU_MOUNT_CORR_COEFF, m_vCoeffMountCorrection);
}

#endif
// ------------------------------------------------------------------------
} // namespace
