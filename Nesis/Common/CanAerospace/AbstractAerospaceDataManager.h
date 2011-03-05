#ifndef CAN_ABSTRACT_AEROSPACE_DATA_MANAGER_H
#define CAN_ABSTRACT_AEROSPACE_DATA_MANAGER_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2007 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *      Rok Markovic [rok.markovic@kanardia.eu]                            *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: MIT - MIT License                                            *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include "AbstractUnit.h"
#include "AerospaceData.h"
#include "DownloadService.h"
#include "FlashProgrammingService.h"
#include "ApplicationProgrammingService.h"
#include "UploadService.h"
#include "ModuleInfoService.h"
#include "ModuleConfigService.h"
#include "NodeIdSettingService.h"
#include "Message.h"

#include "CRC-32.h"

namespace can {

//-----------------------------------------------------------------------------
/** \brief Abstract class that handles CANaerospace messages and services.

	This class only stores pointers to individual CANaerospace structs.
	The derived class must declare CANaerospace structs that it actually
	needs and set pointers to those structs in its constructor. When such
	structures are available, the class is able to handle most CANaerospace
	incomming messages.

	The class also handles some complex services on channel 0, like download,
	upload, flash programming, etc. Several other classes are used in order to
	help the class handling and bookkeeping the details.

	DDS (Download Data Service)

	DDS is used to transfer some data from node A to node B, where node A starts
	the service. Two help classes are used to handle this situation:
	DDS_A - helps node A, while DDS_B helps node B.
	The DDS approximately follows this procedure:

	-# Node A starts Download by transmitting a message to B.
	-# Node B must return a confirmation message within 100 ms.
	-# Node A accepts the return from B and changes the mode to transmitting.
	-# Node A is transmitting the messages and
	-# Node B is receiving them and may stop, cancel or resume the
	   transmission at any time.
	-# Node A must be able to handle this situation.
	-# When node B receives the last massage, it transmits the checksum back
	  to the node A.
	-# Node A receives the checksum and verifies it. It sets the corresponding flag.
 */

class AbstractAerospaceDataManager
{
public:
	//! Constructor
	AbstractAerospaceDataManager();
	//! Destructor
	virtual ~AbstractAerospaceDataManager() {;}

	//! Return this hardware unit node id.
	uint8_t GetNodeId() const
	{ return m_pUnit->GetNodeId(); }

	//! Access to the abstract unit object.
	AbstractUnit* GetUnit()
	{ return m_pUnit; }

	//! Check for any processing timeouts.
	void CheckTimeouts();
	//! Process standard aerospace messages and incomming services.
	virtual void ProcessIncommingMessage(const MessageAerospace& msgIn);
	//! Process any outgoing message required by services. Return number of messages send.
	virtual int32_t ProcessOutgoingMessages();

	// Download service
	#if defined (USE_CAN_DDS_A)
	//! Start data download service (note the name is misleading).
	bool StartDownloadService(uint8_t byToNode, uint8_t byMsgCount, uint32_t uiDataId);
	//! Is the download still active (node A will answer this question.)
	bool IsDownloadActive() const
	{ return m_ddsA.IsActive(); }
	//! What is current status (node A) of the download.
	DDS_A::State GetDownloadState() const
	{ return m_ddsA.m_eState; }
	//! Reset the download and make it ready again for new start. Node A only.
	void ResetDownload()
	{ m_ddsA.Reset(); }
	//! Get the download receiver (node B)
	uint8_t GetDownloadReceiver() const
	{ return m_ddsA.GetReceiver(); }
	#endif

	#if defined (USE_CAN_DDS_B)
	//! Is the download still active (node B will answer this question.)
	bool IsDownloadActive_B() const
	{ return m_ddsB.IsBusy(); }
	//! Get the sender - the unit who was trasmitting the last accepted download.
	uint8_t GetDownloadSender_B() const
	{ return m_ddsB.GetSender(); }
	#endif

	#if defined (USE_CAN_FPS_A)
	// Flash programming service

	//! Send message - start flash programming service.
	bool ProgramFlash(uint8_t byNodeB);
	//! What is current status (node A) of the flash program.
	FPS_A::State GetProgramFlashState() const
	{ return m_fpsA.m_eState; }
	//! Reset the flash programming service and make it ready again for new start. Node A only.
	void ResetProgramFlash()
	{ m_fpsA.Reset(); }
	#endif

	#if defined (USE_CAN_APS_A)
	// Application programming service

	//! Send message - start application programming service. Tell how many pages will be sent.
	bool StartAPS(uint8_t byNodeB, uint8_t byHardwareId);
	//! What is current status (node A) of the application programming.
	APS_A::State GetAPSState() const
	{ return m_apsA.m_eState; }
	//! Reset the application programming service and make it ready again for a new start. Node A only.
	void ResetAPS()
	{ m_apsA.Reset(); }
	//! Is the APS still active (node A will answer this question.)
	bool IsAPSActive() const
	{ return m_apsA.IsActive(); }
	//! Return bootloader version.
	uint8_t GetAPSBootloaderVer() const
	{ return m_apsA.GetBootloaderVer(); }
	#endif

	#if defined (USE_CAN_DUS_A)
	//! Start data upload service (note the name is misleading).
	bool StartUploadService(uint8_t byNodeB, uint8_t byMsgSize, uint32_t uiDataId);
	//! Reset the application programming service and make it ready again for a new start. Node A only.
	void ResetUpload()
	{ m_dusA.Reset(); }
	//! What is current status (node A) of the upload service.
	DUS_A::State GetUploadState() const
	{ return m_dusA.m_eState; }
	#endif

	#if defined (USE_CAN_MIS_A)
	//! Send MIS request.
	bool RequestModuleInformation(uint8_t byNodeB, uint8_t byRequest, uint32_t uiInfo=0);
	#endif

	#if defined (USE_CAN_NIS_A)
	//! Send NIS request.
	bool ChangeNodeId(uint8_t byNodeB, uint8_t byNewId, uint32_t uiSerial);
	#endif

	#if defined (USE_CAN_MCS_A)
	//! Send module configuration service request.
	bool ConfigureModule(uint8_t byNodeB, uint8_t byConfigId, DataType eDataType, Register rData);
	#endif

protected:
	// virtuals

	//! Handle Emergency Event Data
	virtual void HandleEmergencyEventData(const MessageAerospace& msg);
	//! Handle High Priority Node Service Data
	virtual void HandleHighPriorityNodeServiceData(const MessageAerospace& msg);
	//! Handle High Priority User Defined Data
	virtual void HandleHighPriorityUserDefinedData(const MessageAerospace& /*msg*/) {;}
	//! Handle Normal Operation Data -- most of it is handled here!
	virtual void HandleNormalOperationData(const MessageAerospace& msg);
	//! Handle Low Priority User Defined Data
	virtual void HandleLowPriorityUserDefinedData(const MessageAerospace& /*msg*/) {;}
	//! Handle Debug Service Data
	virtual void HandleDebugServiceData(const MessageAerospace& /*msg*/) {;}
	//! Handle Low Priority Node Service Data
	virtual void HandleLowPriorityNodeServiceData(const MessageAerospace& /*msg*/);

	//! Copy message data into correct address in some structure.
	void CopyData(int32_t iFirstValidId, int32_t iLastValidId, int32_t iId,
					  uint32_t* pFirstAddress, uint32_t uiMsgData,
					  void* pStructAddr, uint32_t sizeStruct);

	//! Is id inside given range.
	bool IsInRange(int32_t idLow, int32_t id, int32_t idHigh)
	{ return idLow <= id && idHigh >= id; }

protected:
	//! Abstract node handler object.
	AbstractUnit* m_pUnit;

	//! Pointer to the Flight State & Air Data struct.
	FlightStateAirData* 		m_pFSAD;
	//! Pointer to the Flight Controls Data struct.
	FlightControlsData* 		m_pFCD;
	//! Pointer to the Engine Fuel Supply Data struct.
	EngineFuelSupplyData* 	m_pEFSD;
	//! Pointer to the Power Transmition Data struct.
	PowerTransmitionData* 	m_pPTD;
	//! Pointer to the Hydraulic Data struct.
	HydraulicData* 			m_pHD;
	//! Pointer to the Electric Data struct.
	ElectricData* 				m_pED;
	//! Pointer to the Nav(igation) Data struct.
	NavData* 					m_pND;
	//! Pointer to the Landing Gear Data struct.
	LandingGearData* 			m_pLGD;
	//! Pointer to the Miscellaneous Data struct.
	MiscellaneousData* 		m_pMD;

	//! Kanardia specific! Pointer to DAQU data for single engine.
	KanardiaData*				m_pKD;

	#if defined (USE_CAN_DDS_A)
	//! DDS helper object for node A.
	DDS_A m_ddsA;
	#endif
	#if defined (USE_CAN_DDS_B)
	//! DDS helper object for node B.
	DDS_B m_ddsB;
	#endif

	#if defined (USE_CAN_FPS_A)
	//! FPS helper object for node A.
	FPS_A m_fpsA;
	#endif
	#if defined (USE_CAN_FPS_B)
	//! FPS helper object for node B.
	FPS_B m_fpsB;
	#endif

	#if defined (USE_CAN_APS_A)
	//! APS helper object for node A.
	APS_A m_apsA;
	#endif
	#if defined (USE_CAN_APS_B)
	//! APS helper object for node B.
	APS_B m_apsB;
	#endif

	#if defined (USE_CAN_DUS_A)
	//! DUS helper object for node A.
	DUS_A m_dusA;
	#endif
	#if defined (USE_CAN_DUS_B)
	//! DUS helper object for node B.
	DUS_B m_dusB;
	#endif

	#if defined (USE_CAN_MIS_A)
	//! MIS helper object for node A.
	MIS_A m_misA;
	#endif
	#if defined (USE_CAN_MIS_B)
	//! MIS helper object for node B.
	MIS_B m_misB;
	#endif

	#if defined (USE_CAN_NIS_A)
	//! NIS helper object for node A.
	NIS_A m_nisA;
	#endif
	#if defined (USE_CAN_NIS_B)
	//! NIS helper object for node B.
	NIS_B m_nisB;
	#endif

	#if defined (USE_CAN_MCS_A)
	//! MCS helper object for node A.
	MCS_A m_mcsA;
	#endif
	#if defined (USE_CAN_MCS_B)
	//! MCS helper object for node B.
	MCS_B m_mcsB;
	#endif
};


//-----------------------------------------------------------------------------
} // can namespace

#endif
