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

#include "UnitInfoDaqu.h"

namespace can {

// ------------------------------------------------------------------------

UnitInfoDaqu::UnitInfoDaqu(
	uint8_t byNodeId, 
	uint32_t uiSerial, 
	common::JulianDay jdDate
)
: UnitInfoBase(byNodeId, KHU_DAQU, uiSerial, jdDate)
{
	m_fFlowFactor = 0.0;
	m_uiFlowK = 0;
	m_uiRpmDivisor = 0;
}

UnitInfoDaqu::UnitInfoDaqu(const UnitInfoBase& C)
: UnitInfoBase(C)
{
	m_byHardwareId = KHU_DAQU;
	m_fFlowFactor = 0.0;
	m_uiFlowK = 0;
	m_uiRpmDivisor = 0;
}

UnitInfoDaqu::~UnitInfoDaqu()
{
}

// ------------------------------------------------------------------------
	
bool UnitInfoDaqu::LoadDownloadBuffer(uint32_t /*uiDwnDataId*/)
{
	return false;
}

// ------------------------------------------------------------------------

void UnitInfoDaqu::ProcessModuleInfoResponse(const can::MessageAerospace& msg)
{
	uint8_t byQuestion = msg.GetMessageCode();

	switch(byQuestion) {
	case MIS_DAQU_FUEL_FLOW_FACTOR:
		Q_ASSERT(msg.GetDataType() == can::dtFloat);
		m_fFlowFactor = msg.GetData().f;
		break;
	case MIS_DAQU_FUEL_FLOW_K:
		Q_ASSERT(msg.GetDataType() == can::dtULong);
		m_uiFlowK = msg.GetData().ui32;
		break;
	case MIS_DAQU_RPM_DIVISOR:
		Q_ASSERT(msg.GetDataType() == can::dtULong);
		m_uiRpmDivisor = msg.GetData().ui32;
		break;
	default:
		UnitInfoBase::ProcessModuleInfoResponse(msg);
		break;
	}
}

// ------------------------------------------------------------------------

void UnitInfoDaqu::UpdateSignOfLife(
	AbstractAerospaceDataManager *pADM
)
{
	UnitInfoBase::UpdateSignOfLife(pADM);
	if(m_fFlowFactor <= 0.0f)
		pADM->RequestModuleInformation(GetNodeId(), MIS_DAQU_FUEL_FLOW_FACTOR);
	if(m_uiFlowK == 0)
		pADM->RequestModuleInformation(GetNodeId(), MIS_DAQU_FUEL_FLOW_K);
	if(m_uiRpmDivisor == 0)
		pADM->RequestModuleInformation(GetNodeId(), MIS_DAQU_RPM_DIVISOR);
}
// ------------------------------------------------------------------------

bool UnitInfoDaqu::IsValid() const
{
	if(UnitInfoBase::IsValid()) {
		return (m_fFlowFactor  > 0.0f) &&
		       (m_uiFlowK      > 0)    &&
		       (m_uiRpmDivisor > 0);
	}
	return false;
}

// ------------------------------------------------------------------------
} // namespace
