#ifndef NESIS_UNIT_CAN_H
#define NESIS_UNIT_CAN_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2007 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include "CanAerospace/AbstractAerospaceDataManager.h"

#include <QObject>
#include "PacketComm/Serial.h"
#include "CanAerospace/AbstractUnit.h"
#include "CanAerospace/UnitProgrammer.h"
#include "NesisUnitInfoContainer.h"

class QTimer;

/**
 * \brief Representation of the NESIS hardware unit on the can bus.
 *
*/

class NesisUnitCAN : public QObject, public can::AbstractUnit
{
Q_OBJECT
public:
	//! Constructor.
	NesisUnitCAN(QObject *pParent = 0);
	//! Destructor
	~NesisUnitCAN();

	//! We allow only one instance of this class.
	static NesisUnitCAN* GetInstance()
	{
		if(s_pInstance==NULL) {
			s_pInstance = new NesisUnitCAN();
		}
		return s_pInstance;
	}

	//! Set the serial port.
	void SetPort(packetcomm::AbstractPort* pPort)
	{ m_pPort = pPort; }

	//! Some objects need access to the CanAerospace data manager.
	void SetAerospaceDataManager(can::AbstractAerospaceDataManager* pADM)
	{
		m_programmer.SetAerospaceDataManager(pADM);
		m_conUnitInfo.SetAerospaceDataManager(pADM);
	}

	//! Get access to the Can unit info container.
	NesisUnitInfoContainer& GetUnitInfoContainer()
	{ return m_conUnitInfo; }
	//! Get access to the Can unit programmer.
	can::UnitProgrammer& GetCanUnitProgrammer()
	{ return m_programmer; }

	packetcomm::AbstractPort* GetPort()
	{ return m_pPort; }
	
	//! Program Mabu.
	void ProgramMabu();
	
	
signals:
	//! Signal that SPI program mode has changed.
	void ProgramMabu(bool bWriting);

protected:
	//! Post Message - by derived class.
	virtual bool PostMessage(const can::MessageAerospace& msg) const;

	// Identification service
	//! Get hardware node id needed by CAN.
	virtual uint8_t GetNodeId() const;
	//! Get software version.
	virtual uint8_t GetSWVersion() const;
	//! Get hardware version.
	virtual uint8_t GetHWVersion() const;

	//! Process the "Sign Of Life" message.
	virtual void ProcessSignOfLife(const can::MessageAerospace& msg);

	// Data download service
	//! Set download timeout given in ms. On timeout, it must set the given flag.
	virtual void SetADownloadTimeout(uint ms, bool* pbFlag);
	//! Get download data to be transmited based on uiDataId and current message index.
	virtual bool GetDownloadData(uint32_t uiDataId, uint8_t byIndex,
			can::DataType& eDataType, can::Register& rData) const;

	// Data upload
	//! Set upload timeout given in ms. On timeout, it must set the given flag.
	void SetAUploadTimeout(uint32_t ms, bool* pbFlag);
	//! Store uploaded data received in the message from node B.
	bool StoreUploadMessage(const can::MessageAerospace& msg);

	// Flash programming services
	//! Program the flash - should never be called in Nesis.
	virtual bool ProgramFlash() { return false; }
	//! Set flash programming timeout given in ms. On timeout, it must set the given flag.
	virtual void SetAFlashProgrammingTimeout(uint ms, bool* pbFlag);

	// Application programming services
	//! Set application programming timeout given in ms. On timeout, it must set the given flag.
	virtual void SetAApplicationProgrammingTimeout(uint ms, bool* pbFlag);
	//! Get number of application pages to program (for given node id).
	virtual int32_t GetAPSPageCount(uint8_t byNodeId);
	//! Get address of the page i. Each page is 512 bytes long.
	virtual uint32_t* GetAPSPageAddress(uint uiPage);

	// Module information
	//! Forwards responce to the ConUnitInfoContainer object.
	void ProcessModuleInfoResponse(const can::MessageAerospace& msg);

	// Module configuration service
	//! Responses on module config service.
	void ProcessModuleConfigResponse(const can::MessageAerospace& msg);

private slots:
	//! On download data service for node A timer timeout.
	void OnTimerDDS_A();
	//! On upload data service for node A timer timeout.
	void OnTimerDUS_A();
	//! On flash programming service for node A timer timeout.
	void OnTimerFPS_A();
	//! On application programming service for node A timer timeout.
	void OnTimerAPS_A();

private:
	static NesisUnitCAN* s_pInstance;

	//! We need access to a valid and open port. We do not own the port.
	packetcomm::AbstractPort* m_pPort;

	//! The download A timer for DownloadDataService.
	QTimer* m_pTimerDDS_A;
	//! Pointer to the flag which needs to be set on DDS_A timeout.
	bool* m_pbFlagDDS_A;

	//! The upload A timer for UploadDataService.
	QTimer* m_pTimerDUS_A;
	//! Pointer to the flag which needs to be set on DUS_A timeout.
	bool* m_pbFlagDUS_A;

	//! The flash programming timer for Flash Programming Service.
	QTimer* m_pTimerFPS_A;
	//! Pointer to the flag which needs to be set on FPS_A timeout.
	bool* m_pbFlagFPS_A;

	//! The application programming A timer for APS.
	QTimer* m_pTimerAPS_A;
	//! Pointer to the flag which needs to be set on DDS_B timeout.
	bool* m_pbFlagAPS_A;

	//! We need to maintain a container of nodes found on last scan.
	mutable NesisUnitInfoContainer m_conUnitInfo;

	//! Automatically programs units where bootloader detected invalid hash.
	can::UnitProgrammer m_programmer;
};

#endif
