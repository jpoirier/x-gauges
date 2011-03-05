/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2008 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include <typeinfo>
#include <QtDebug>

#include "Settings.h"
#include "NesisUnitInfoContainer.h"

// ------------------------------------------------------------------------

NesisUnitInfoContainer::NesisUnitInfoContainer()
{
}

// ------------------------------------------------------------------------

void NesisUnitInfoContainer::Scan(bool /*bSetHighId*/)
{
}
// 	using namespace can;
	
	// Disable reports
/*	m_pADM->ConfigureModule(0, MCS_ENABLE_REPORTS, can::dtLong, MCS_DISABLE);
	Wait(10); */
/*	m_pADM->ConfigureModule(0, MCS_ENABLE_REPORTS, can::dtLong, MCS_DISABLE);
	Wait(10);
	// Scan the units.
	UnitInfoBaseContainer::Scan(bSetHighId);*/
	
/*	for(int i=0; i<m_con.count(); i++)
		qDebug() << m_con[i]->GetHardwareId();*/
	
	// Let's get direct links to individual objects.
/*	QVector<UnitInfoBase*> vU = GetUnits(KHU_AIRU);
	if(vU.count() > 0) 
		m_pAiru = dynamic_cast<UnitInfoAiru*>(vU[0]);
	else 
		qWarning() << "Warning: Airu was not detected.";
	
	vU = GetUnits(KHU_MAGU);
	if(vU.count() > 0) 
		m_pMagu = dynamic_cast<UnitInfoMagu*>(vU[0]);
	else 
		qWarning() << "Warning: Magu was not detected.";
	
	vU = GetUnits(KHU_DAQU);
	if(vU.count() > 0) 
		m_pDaqu = dynamic_cast<UnitInfoDaqu*>(vU[0]);
	else 
		qWarning() << "Warning: Daqu was not detected.";
	
	// If this is primary nesis, we request pointer to the primary mabu.
	// or if this is secondary nesis, we request pointer to the secondary mabu.
	vU = GetUnits(KHU_MABU);
	uint8_t byMabuId = (Settings::GetInstance()->IsPrimary() ? KHU_MABU : KHU_MABU_SECONDARY);
	for(int i=0; i<vU.count(); i++) {
		if(vU[i]->GetNodeId()==byMabuId) {
			m_pMabu = dynamic_cast<UnitInfoMabu*>(vU[i]);
		}
	}
	if(m_pMabu) {
		// This will set the secondary flag.
		m_pADM->RequestModuleInformation(m_pMabu->GetNodeId(), MIS_MABU_SECONDARY);
	}
	else {
		qWarning() << "Warning: Mabu was not detected.";
	}
		
	// We also ask DAQU for the fuel flow factor.
	if(m_pDaqu) {
		m_pADM->RequestModuleInformation(m_pDaqu->GetNodeId(), MIS_DAQU_FUEL_FLOW_FACTOR);
		m_pADM->RequestModuleInformation(m_pDaqu->GetNodeId(), MIS_DAQU_FUEL_FLOW_K);
		m_pADM->RequestModuleInformation(m_pDaqu->GetNodeId(), MIS_DAQU_RPM_DIVISOR);
	}
*/
	// Enable reports
/*	Wait(10);
	m_pADM->ConfigureModule(0, MCS_ENABLE_REPORTS, can::dtLong, MCS_ENABLE);*/
/*	Wait(20);
	m_pADM->ConfigureModule(0, MCS_ENABLE_REPORTS, can::dtLong, MCS_ENABLE);
	Wait(20);
	m_pADM->ConfigureModule(0, MCS_ENABLE_REPORTS, can::dtLong, MCS_ENABLE);*/
// }

// ------------------------------------------------------------------------

can::UnitInfoBase* NesisUnitInfoContainer::RecreateUnit(
	can::UnitInfoBase* pOld, 
	uint8_t byHardwareId
)
{
	using namespace can;
	
	UnitInfoBase* pUnitInfo = pOld;
	switch(byHardwareId) {
	
	case KHU_MAGU:
		if(typeid(*pOld) != typeid(UnitInfoMagu)) {
			pUnitInfo = new UnitInfoMagu(*pOld);
			delete pOld;
			qDebug() << "Magu (re)created.";
		}
		break;
		
	case KHU_MABU:
		if(typeid(*pOld) != typeid(UnitInfoMabu)) {
			pUnitInfo = new UnitInfoMabu(*pOld);
			delete pOld;
			qDebug() << "Mabu (re)created.";
		}
		break;
		
	case KHU_AIRU:
		if(typeid(*pOld) != typeid(UnitInfoAiru)) {
			pUnitInfo = new UnitInfoAiru(*pOld);
			delete pOld;
			qDebug() << "Airu (re)created.";
		}
		break;
	
	case KHU_DAQU:
		if(typeid(*pOld) != typeid(UnitInfoDaqu)) {
			pUnitInfo = new UnitInfoDaqu(*pOld);
			delete pOld;
			qDebug() << "Daqu (re)created.";
		}
		break;
		
	default:
		pUnitInfo = pOld;
		break;
	}
	return pUnitInfo;
}

// ------------------------------------------------------------------------

can::UnitInfoAiru* NesisUnitInfoContainer::GetAiru() 
{
	can::UnitInfoBase* pU = Find(KHU_AIRU);
	if(pU) {
		if(pU->IsValid())
			return (can::UnitInfoAiru*) pU;
	}
	return NULL; 
}

// ------------------------------------------------------------------------

can::UnitInfoMagu* NesisUnitInfoContainer::GetMagu() 
{ 
	can::UnitInfoBase* pU = Find(KHU_MAGU);
	if(pU) {
		if(pU->IsValid())
			return (can::UnitInfoMagu*) pU;
	}
	return NULL; 
}

// ------------------------------------------------------------------------

can::UnitInfoDaqu* NesisUnitInfoContainer::GetDaqu() 
{ 
	can::UnitInfoBase* pU = Find(KHU_DAQU);
	if(pU) {
		if(pU->IsValid())
			return (can::UnitInfoDaqu*) pU;
	}
	return NULL; 
}

// ------------------------------------------------------------------------

can::UnitInfoMabu* NesisUnitInfoContainer::GetMabu() 
{ 
	uint8_t byMabuId = (Settings::GetInstance()->IsPrimary() ? KHU_MABU : KHU_MABU_SECONDARY);
	can::UnitInfoBase* pU = Find(byMabuId);
	if(pU) {
		if(pU->IsValid())
			return (can::UnitInfoMabu*) pU;
	}
	return NULL; 
}

// ------------------------------------------------------------------------
