#ifndef UNIT_INFO_BASE_CONTAINER_H
#define UNIT_INFO_BASE_CONTAINER_H

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

#include "UnitInfoBase.h"

namespace can {

// --------------------------------------------------------------------------

class UnitInfoBaseContainer
{
public:
	//! Container of units.
	typedef QVector<UnitInfoBase*> Container;
protected:
enum State { 
	sScan,
	sIdle,
	sInvalid
};
public:
	//! Constructor
	UnitInfoBaseContainer();
	//! Destructor
	virtual ~UnitInfoBaseContainer();
	
	//! Reset the container
	void Reset();
	
	//! Set CANAerospace data manager.
	void SetAerospaceDataManager(AbstractAerospaceDataManager* pADM)
	{ m_pADM = pADM; }

	//! Get CANAerospace data manager.
	AbstractAerospaceDataManager* GetAerospaceDataManager()
	{ return m_pADM; }
	//! Get CANAerospace data manager.
	const AbstractAerospaceDataManager* GetAerospaceDataManager() const
	{ return m_pADM; }

	//! Scan nodes - build list of connected nodes.
	virtual void Scan(bool bSetHighId=true);
	//! Check if any duplicate units with the same id and empty serial exists.
	QVector<int> GetDuplicateEmptyNodes() const;

	//! Find node info object of given node id.
	UnitInfoBase* Find(uint8_t byNodeId);
	
	//! Find node info object of given serial number.
	UnitInfoBase* FindSerial(uint32_t uiSerial);

	//! Access operator.
	const UnitInfoBase* operator[](int i) const
	{ return m_con[i]; }
	//! Full access operator.
	UnitInfoBase* operator[](int i)
	{ return m_con[i]; }

	//! Return number of nodes.
	int GetCount() const
	{ return m_con.count(); }
	
	//! Get units of selected hardware.
 	QVector<UnitInfoBase*> GetUnits(
 		uint8_t byHardwareFilter, bool bCheckSerial=true) const;

	//! Wait in a loop, but allow GUI processing
	void Wait(qint32 msToWait);

	//! Dispach message to specific units.
	int DispatchModuleConfig(
		uint8_t byHardwareFilter, uint8_t byConfigId, 
		DataType eDataType, Register rData
	);

	//! Process MIS response
	virtual void ProcessModuleInfoResponse(const MessageAerospace& msg);
	
	//! Process MCS response
	virtual void ProcessModuleConfigResponse(const MessageAerospace& msg);
	
	//! The sign of life processing.
	void ProcessSignOfLife(const MessageAerospace& msg);

protected:
	//! Find max node id.
	uint8_t GetMaxNodeId() const;
	
	//! Unit factory - create a unit from the SOL message.
	virtual UnitInfoBase* CreateUnitFromSignOfLife(const MessageAerospace& msg);
	
	//! Factory - creates a unit info object.
	virtual UnitInfoBase* CreateUnit(const MessageAerospace& msg);
	//! Factory - recreate a unit based on the hardware id.
 	virtual UnitInfoBase* RecreateUnit(UnitInfoBase* pOld, uint8_t byHardwareId)
 	{ pOld->SetHardwareId(byHardwareId); return pOld; }

protected:
	//! The container of nodes
	Container m_con;
	//! Pointer to the CANAerosapce data manager.
	AbstractAerospaceDataManager* m_pADM;
	//! State variable
	State m_eState;
	
	//! Time of the last device "activity" check.
	QTime m_tLastActivityCheck;
};

// --------------------------------------------------------------------------

} // namespace 
#endif
