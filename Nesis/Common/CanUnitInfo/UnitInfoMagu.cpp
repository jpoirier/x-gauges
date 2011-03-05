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

#include "UnitInfoMagu.h"

namespace can {

// ------------------------------------------------------------------------

UnitInfoMagu::UnitInfoMagu(
	uint8_t byNodeId, 
	uint32_t uiSerial, 
	common::JulianDay jdDate
)
: UnitInfoBase(byNodeId, KHU_MAGU, uiSerial, jdDate)
{
	m_iCalPointsCount = 0;
}

UnitInfoMagu::UnitInfoMagu(const UnitInfoBase& C)
: UnitInfoBase(C)
{
	m_byHardwareId = KHU_MAGU;
	m_iCalPointsCount = 0;
}

UnitInfoMagu::~UnitInfoMagu()
{
}
// ------------------------------------------------------------------------

bool UnitInfoMagu::LoadDownloadBuffer(uint32_t uiDwnDataId)
{
	Q_ASSERT(m_vDownloadBuffer.isEmpty());

	m_eDataType = can::dtFloat;
	switch(uiDwnDataId) {
	default:
		qWarning("Invalid download request %u.", uiDwnDataId);
		return false;
	}
	return true;
}

// ------------------------------------------------------------------------

void UnitInfoMagu::ProcessModuleConfigResponse(
	const can::MessageAerospace& msg
)
{
	uint8_t byConfigId = msg.GetMessageCode();
	switch(byConfigId) {
		case MCS_MAGU_CAL_ADD:
			m_iCalPointsCount = msg.GetData().ui32;
			break;
		case MCS_MAGU_CAL_FINISH:
			m_iCalFinishResult = msg.GetData().ui32;
			break;
		default:
			UnitInfoBase::ProcessModuleConfigResponse(msg);
			break;
	}
}

// ------------------------------------------------------------------------
#if defined (USE_CAN_DUS_A)
bool UnitInfoMagu::UploadCalibration(can::AbstractAerospaceDataManager* pADM)
{
	return Upload(pADM, UPLOAD_MAGU_COEFF, m_vCoeff, 5);
}
// ------------------------------------------------------------------------

bool UnitInfoMagu::UploadData(can::AbstractAerospaceDataManager* pADM)
{
	return Upload(pADM, UPLOAD_MAGU_DATA, m_vData, 3*8);
}

// ------------------------------------------------------------------------

bool UnitInfoMagu::UploadCalibrationFlash(can::AbstractAerospaceDataManager* pADM)
{
	return Upload(pADM, UPLOAD_MAGU_COEFF_FLASH, m_vCoeffFlash, 2*5);
}
#endif
// ------------------------------------------------------------------------
} // namespace
