/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2007 by Kanardia d.o.o. [see www.kanardia.eu]           *
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
#include <QTime>
#include <QApplication>

#include "UnitInfoAiru.h"
#include "UnitInfoMagu.h"
#include "UnitInfoDaqu.h"
#include "UnitInfoMabu.h"

#include "UnitInfoBaseContainer.h"

#define SOL_TIME_TO_CHECK_ACTIVE_MS		1000
#define SOL_TIME_TO_DELETE_MS				5000

namespace can {

// ------------------------------------------------------------------------

UnitInfoBaseContainer::UnitInfoBaseContainer()
{
	m_eState = sIdle;
	m_pADM   = NULL;
	m_tLastActivityCheck.start();
}

// ------------------------------------------------------------------------

UnitInfoBaseContainer::~UnitInfoBaseContainer()
{
	Reset();
}
// ------------------------------------------------------------------------

void UnitInfoBaseContainer::Reset()
{
	foreach(UnitInfoBase* pI, m_con)
		delete pI;
	m_con.clear();
}
// ------------------------------------------------------------------------

UnitInfoBase* UnitInfoBaseContainer::CreateUnit(
	const MessageAerospace& msg
)
{
	UnitInfoBase* pUnitInfo;
	switch(msg.GetSender()) {
	default:
		pUnitInfo = new UnitInfoBase(msg.GetSender(), msg.GetSender(), msg.GetData().ui32, 0);
		break;
	}
	return pUnitInfo;
}
// ------------------------------------------------------------------------

UnitInfoBase* UnitInfoBaseContainer::Find(
	uint8_t byNodeId
)
{
	Container::iterator i;
	for(i=m_con.begin(); i!=m_con.end(); i++)
		if((*i)->GetNodeId() == byNodeId)
			return (*i);
/*	foreach(UnitInfoBase* pI, m_con)
		if(pI->GetNodeId() == byNodeId)
			return pI;
*/
	return NULL;
}

// ------------------------------------------------------------------------

QVector<UnitInfoBase*> UnitInfoBaseContainer::GetUnits(
	uint8_t byHardwareFilter,
	bool bCheckSerial
) const
{
	QVector<UnitInfoBase*> vI;

	foreach(UnitInfoBase* pI, m_con)
		if(pI->GetHardwareId() == byHardwareFilter) {
			if(bCheckSerial && pI->GetSerial()==0)
				continue;
			vI << pI;
		}
	return vI;
}

// ------------------------------------------------------------------------

void UnitInfoBaseContainer::Wait(qint32 msToWait)
{
	UnitInfoBase::Wait(m_pADM, msToWait);
}
// --------------------------------------------------------------------

uint8_t UnitInfoBaseContainer::GetMaxNodeId() const
{
	uint8_t byMaxId = 0;
	foreach(UnitInfoBase* pI, m_con)
		byMaxId = qMax(pI->GetNodeId(), byMaxId);
	return byMaxId;
}

// --------------------------------------------------------------------

void UnitInfoBaseContainer::ProcessModuleInfoResponse(const can::MessageAerospace& msg)
{
	uint8_t byQuestion = msg.GetMessageCode();

	// During the scan serial number is used to identify individual units.
	if(m_eState == sScan && byQuestion == MIS_SERIAL_NUMBER) {
		// Due to SPI<->CAN marshalling problems we send serial number request
		// two times, however only one responce is needed for given serial
		// number. 
		bool bFound = FindSerial(msg.GetData().ui32);
		if(!bFound) {
			m_con.push_back(CreateUnit(msg));
			
			qDebug() << "Id =" << msg.GetSender() 
						<< "Serial = " << m_con.back()->GetSerial() 
						<< "Id2 =" << m_con.back()->GetNodeId();
		}
	}
	else {
		for(int i=0; i<m_con.count(); i++) {
			UnitInfoBase* pI = m_con[i];
			if(msg.GetSender() == pI->GetNodeId()) {
				if(byQuestion == MIS_HARDWARE_ID) {
/*					qDebug() << "Node =" << msg.GetNodeId()
								<< "Data Hardvare id =" << msg.GetData().ui32
								<< "Current hardvare id =" << m_con[i]->GetHardwareId();*/
					m_con[i] = RecreateUnit(pI, msg.GetData().ui32 & 0xFF);
// 					qDebug() << "Recreated unit=" << m_con[i];
				}
				else {
					pI->ProcessModuleInfoResponse(msg);
				}
			}
		}
	}
}

// ------------------------------------------------------------------------

void UnitInfoBaseContainer::ProcessModuleConfigResponse(
	const can::MessageAerospace& msg
)
{
	// Find the node ...
	can::UnitInfoBase* pI = Find(msg.GetSender());
	// Not found
	if(pI == NULL) {
		qWarning() << "can::UnitInfoBase object not found for node" 
					  << msg.GetSender()
					  << "MCS Message Code" << msg.GetMessageCode();
		return;
	}
	// The module object should process the response.
	pI->ProcessModuleConfigResponse(msg);
}

// --------------------------------------------------------------------

void UnitInfoBaseContainer::Scan(bool bSetHighId)
{
	m_eState = sScan;
	// Clear any previous nodes.
	m_con.clear();

	// Submit request on all known uC units.
	// Due to SPI<->CAN marshalling problems we send request two times	
	for(int id=1; id<=KHU_LAST_uC_UNIT; id++) {
		m_pADM->RequestModuleInformation(id, MIS_SERIAL_NUMBER);
		Wait(2);
		m_pADM->RequestModuleInformation(id, MIS_SERIAL_NUMBER);
	}
//	if(bSetHighId) {
		for(int id=128; id<128+32; id++) {
			m_pADM->RequestModuleInformation(id, MIS_SERIAL_NUMBER);
			Wait(2);
			m_pADM->RequestModuleInformation(id, MIS_SERIAL_NUMBER);
		}
//	}

	// We must give units some time to respond.
	// This wait loop will fill the container.
	Wait(200);

	// Printout what we got.
/*	foreach(UnitInfoBase* pI, m_con) {
		qDebug() << "Found " << pI->GetHardwareId() << pI->GetSerial();
	}*/
	
	if(bSetHighId) {
		// What was the max id.
		int iNewId = qMax(GetMaxNodeId()+1,128);
	
		// Now go and change all units with valid serials
		foreach(UnitInfoBase* pI, m_con) {
			if(pI->GetSerial() > 0 && pI->GetNodeId() < 128) {
				m_pADM->ChangeNodeId(pI->GetNodeId(), iNewId, pI->GetSerial());
				pI->SetNodeId(iNewId);
				iNewId++;
				Wait(10);
			}
		}
		// Give units some time to respond
		Wait(150);
	}


	// Ask each unit
	foreach(UnitInfoBase* pI, m_con) {
		if((pI->GetNodeId() >= 128 && bSetHighId) || bSetHighId==false) {
			m_pADM->RequestModuleInformation(pI->GetNodeId(), MIS_HARDWARE_ID);
			m_pADM->RequestModuleInformation(pI->GetNodeId(), MIS_SERIAL_DATE);
			m_pADM->RequestModuleInformation(pI->GetNodeId(), MIS_VERSION);
			m_pADM->RequestModuleInformation(pI->GetNodeId(), MIS_VENDOR_NUMBER);
			m_pADM->RequestModuleInformation(pI->GetNodeId(), MIS_SVN_REVISION);
			Wait(10);
			m_pADM->RequestModuleInformation(pI->GetNodeId(), MIS_HARDWARE_ID);
			m_pADM->RequestModuleInformation(pI->GetNodeId(), MIS_SERIAL_DATE);
			m_pADM->RequestModuleInformation(pI->GetNodeId(), MIS_VERSION);
			m_pADM->RequestModuleInformation(pI->GetNodeId(), MIS_VENDOR_NUMBER);
			m_pADM->RequestModuleInformation(pI->GetNodeId(), MIS_SVN_REVISION);
		}
	}

	// Give units some time to respond
	Wait(200);
	// Now we have all basic scan info.

	qDebug() << "Done.";
	m_eState = sIdle;
}

// ------------------------------------------------------------------------

QVector<int> UnitInfoBaseContainer::GetDuplicateEmptyNodes() const
{
	QVector<int> conDuplicates;

	for(int id=1; id<KHU_LAST_uC_UNIT; id++) {
		int iCount = 0;
		foreach(UnitInfoBase* pI, m_con) {
			if(pI->GetNodeId()==id && pI->GetSerial()==0)
				iCount++;
		}
		if(iCount > 1)
			conDuplicates << id;
	}

	return conDuplicates;
}

// ------------------------------------------------------------------------

int UnitInfoBaseContainer::DispatchModuleConfig(
	uint8_t byHardwareFilter,
	uint8_t byConfigId,
	can::DataType eDataType,
	can::Register rData
)
{
	int iCount = 0;
	foreach(UnitInfoBase* pI, m_con) {
		if(pI->GetHardwareId() == byHardwareFilter) {
			m_pADM->ConfigureModule(
				pI->GetNodeId(),
				byConfigId,
				eDataType,
				rData
			);
			iCount++;
		}
	}
	return iCount;	// Number of messages sent
}

// ------------------------------------------------------------------------

UnitInfoBase* UnitInfoBaseContainer::FindSerial(uint32_t uiSerial)
{
	Container::iterator i;
	for(i=m_con.begin(); i!=m_con.end(); i++)
		if((*i)->GetSerial() == uiSerial)
			return (*i);
	return NULL;
}
// ------------------------------------------------------------------------

UnitInfoBase* UnitInfoBaseContainer::CreateUnitFromSignOfLife(const MessageAerospace& msg)
{
	UnitInfoBase* pU;
	// This is the hardware id
	uint8_t byNodeId 	= msg.GetSender();
	uint8_t byHwId 	= msg.GetMessageCode();
	uint32_t uiSerial = msg.GetData().ui32;
	
	switch(byHwId) {
	case KHU_AIRU:
		qDebug() << "Creating AIRU object.";
		pU = new UnitInfoAiru(byNodeId, uiSerial, 0);
		break;
	case KHU_MAGU:
		qDebug() << "Creating MAGU object.";
		pU = new UnitInfoMagu(byNodeId, uiSerial, 0);
		break;
	case KHU_DAQU:
		qDebug() << "Creating DAQU object.";
		pU = new UnitInfoDaqu(byNodeId, uiSerial, 0);
		break;
	case KHU_MABU:
		qDebug() << "Creating MABU object.";
		pU = new UnitInfoMabu(byNodeId, uiSerial, 0);
		break;
	default:
		qWarning() << "Unknown CAN device HW =" << byHwId << "- default unit was created.";
		pU = new UnitInfoBase(byNodeId, byHwId, uiSerial, 0);
	}
	return pU;
}

// ------------------------------------------------------------------------
/** Here we need to create and destroy the can units (devices) as they
    appear on the can bus or they are removed from the can bus.
    
    Each device must transmit the "sign of life" message. We wait for
    this message and update the status of internal list of devices. Each device
    may be in one of three possible states:
     - Active: The device is present, active and fully operational.
     - Pending: The device is here, but it is still waiting for some information.
     - Inactive: The device is not active anymore - it is about to be deleted.
     - Deleted: The device object is not present in the container anymore.
 */
void UnitInfoBaseContainer::ProcessSignOfLife(const MessageAerospace& msg)
{
	uint8_t bySender = msg.GetSender();
	// Do we have sender on the list?
	UnitInfoBase* pU = Find(bySender);
	
// 	qDebug() << "SOL:" << bySender << msg.GetMessageCode() << msg.GetData().ui32 << m_con.count();
		
	// Yes, we have the sender. 
	if(pU) {
		// This updates the time and maybe a status.
		pU->UpdateSignOfLife(m_pADM);
	}

	// No, the sender is not on the list.
	else {
		pU = CreateUnitFromSignOfLife(msg);
		pU->UpdateSignOfLife(m_pADM);
		m_con.push_back(pU);
	}
	
	// Do we need to check for the non-active devices.
	if(m_tLastActivityCheck.elapsed() > SOL_TIME_TO_CHECK_ACTIVE_MS) {
		m_tLastActivityCheck.start(); // restart the timing.
	
		// Check the time since the last unit activity.
		Container::iterator i=m_con.begin();
		while(i!=m_con.end()) {
			if((*i)->GetTimeSinceSignOfLife() > SOL_TIME_TO_DELETE_MS) {
				qDebug() << "SOL: Deleting" << (*i)->GetNodeId();
				delete(*i);
				i=m_con.erase(i);
				continue;
			}
			i++;
		}
	}
}

// ------------------------------------------------------------------------

} // namespace
