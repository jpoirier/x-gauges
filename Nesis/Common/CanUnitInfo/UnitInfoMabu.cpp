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

#include "UnitInfoMabu.h"

namespace can {

// ------------------------------------------------------------------------

UnitInfoMabu::UnitInfoMabu(
	uint8_t byNodeId, 
	uint32_t uiSerial, 
	common::JulianDay jdDate
)
: UnitInfoBase(byNodeId, KHU_MABU, uiSerial, jdDate)
{
	m_bSecondary = false;
	m_uiRefRPM   = 9999;
	m_uiCellCap  = 0;
	m_fChargerVoltageFactor = 0.0;
}

UnitInfoMabu::UnitInfoMabu(const UnitInfoBase& C)
: UnitInfoBase(C)
{
	m_bSecondary = false;
	m_uiRefRPM   = 9999;
	m_uiCellCap  = 0;
	m_fChargerVoltageFactor = 0.0;
	
	// TODO Verify. This may be problematic. What about secondary mabu?
	m_byHardwareId = KHU_MABU;
}

UnitInfoMabu::~UnitInfoMabu()
{
}

// ------------------------------------------------------------------------
	
bool UnitInfoMabu::LoadDownloadBuffer(uint32_t /*uiDwnDataId*/)
{
	return false;
}

// ------------------------------------------------------------------------

void UnitInfoMabu::ProcessModuleInfoResponse(const can::MessageAerospace& msg)
{
	uint8_t byQuestion = msg.GetMessageCode();

	switch(byQuestion) {
	case MIS_MABU_SECONDARY:
		Q_ASSERT(msg.GetDataType() == can::dtULong);
		m_bSecondary = msg.GetData().ui32 != 0 ? true : false;
		break;
	case MIS_MABU_CELL_CAPACITY:
		Q_ASSERT(msg.GetDataType() == can::dtULong);
		m_uiCellCap = msg.GetData().ui32;
		break;
	case MIS_MABU_REFERENCE_RPM:
		Q_ASSERT(msg.GetDataType() == can::dtULong);
		m_uiRefRPM = msg.GetData().ui32;
		break;
	case MIS_MABU_CHARGER_VOLT_F:
		Q_ASSERT(msg.GetDataType() == can::dtFloat);
		m_fChargerVoltageFactor = msg.GetData().f;
		break;
		
	default:
		UnitInfoBase::ProcessModuleInfoResponse(msg);
		break;
	}
}
// ------------------------------------------------------------------------

} // namespace
