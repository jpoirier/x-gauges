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

#include "string.h"

#include "Defines.h"
#include "AbstractAerospaceDataManager.h"

#include <cstring>

namespace can {

// --------------------------------------------------------------------------
/** Reset all pointers to structs to NULL. Derived classes must set the
    pointers that they want to use.
 */
AbstractAerospaceDataManager::AbstractAerospaceDataManager()
{
 	m_pUnit =   NULL;

	m_pFSAD = 	NULL;
	m_pFCD =		NULL;
	m_pEFSD = 	NULL;
	m_pPTD = 	NULL;
	m_pHD = 		NULL;
	m_pED = 		NULL;
	m_pND = 		NULL;
	m_pLGD = 	NULL;
	m_pMD = 		NULL;

	m_pKD = 		NULL; // Kanardia specific
}

// --------------------------------------------------------------------------

void AbstractAerospaceDataManager::CheckTimeouts()
{
	// Check download eService timeouts.
	#if defined (USE_CAN_DDS_A)
	m_ddsA.CheckTimeout();
	#endif

	#if defined (USE_CAN_DDS_B)
	m_ddsB.CheckTimeout();
	#endif

	// Check flash programming timeout.
	#if defined (USE_CAN_FPS_A)
	m_fpsA.CheckTimeout();
	#endif

	// Check application programming timout
	#if defined (USE_CAN_APS_A)
	m_apsA.CheckTimeout();
	#endif

	// Check application programming timout
	#if defined (USE_CAN_DUS_B)
	m_dusB.CheckTimeout(m_pUnit);
	#endif
}

// --------------------------------------------------------------------------
/*! This is a helper function used to store the data (\a uiMsgData) into
    correct address. The address is calculated assuming that each
    variable in the structure is 4 bytes long.
 */
void AbstractAerospaceDataManager::CopyData(
	int32_t iFirstValidId, 		//!< Id of the first variable in the structure.
	int32_t iLastValidId,  		//!< Id of the last valid variable in the structure.
	int32_t iId, 					//!< Message id.
	uint32_t* pFirstAddress, 	//!< Pointer to the first valiable in the structure.
	uint32_t uiMsgData,			//!< Message data to be copied.
	void* pStructAddr,			//!< Address of the receiving structure. Testing only.
	uint32_t sizeStruct			//!< Size of the structure.
)
{
	// Check range
	if(iId >= iFirstValidId && iId <= iLastValidId) {
		pFirstAddress += (iId - iFirstValidId);
		if(((uint8_t*)pFirstAddress) < ((uint8_t*)pStructAddr) ||
		   ((uint8_t*)pFirstAddress) > (((uint8_t*)pStructAddr)+sizeStruct-4))
			PRINTF("Invalid memory address for can id = %i\n", iId);
		else
			memcpy(pFirstAddress, &uiMsgData, 4);
	}
	else {
		PRINTF(
			"Id (%i) is not in required range [%i,%i].\n",
			iId, iFirstValidId, iLastValidId
		);
	}
}


// --------------------------------------------------------------------------
/*! The main entry point for the CANaerospace message processing.
    Given message is handled by a corresponding group handler.
    All group handlers are virtual functions and derived classes
    may alter their default behaviour.

    Since some handlers will try to post some messages, it is
    important that the output message buffer is not full before
    this function is called.
 */
void AbstractAerospaceDataManager::ProcessIncommingMessage(
	const MessageAerospace& msg	//!< Incomming message
)
{
	// This unit id.
	uint8_t byUnitId = m_pUnit->GetNodeId();
	// Get the sender of the message and exit if we sent this message.
	if(msg.GetSender() == byUnitId)
		return;

	// Get the receiver of the message.
	uint8_t byReceiver = msg.GetReceiver();
	// Accept msg addressed to EVERYBODY or msg addressed to this node.
	if(byReceiver == EVERYBODY || byReceiver == byUnitId) {
		uint32_t id = msg.GetId();
		// let's find a group which will handle the id.

		// Is it Emergency Event Data?
		if(id < 128) {
			HandleEmergencyEventData(msg);
		}

		// Is it High Priority Node Service Data?
		else if(id < 200) {
			HandleHighPriorityNodeServiceData(msg);
		}

		// Is it High Priority User Defined Data?
		else if(id < 300) {
			HandleHighPriorityUserDefinedData(msg);
		}

		// Is it Normal Operation Data?
		else if(id < 1800) {
			HandleNormalOperationData(msg);
		}

		// Is it Low Priority User Defined Data?
		else if(id < 1900) {
			HandleLowPriorityUserDefinedData(msg);
		}

		// Is it Debug Service Data?
		else if(id < 2000) {
			HandleDebugServiceData(msg);
		}

		// Is it Low Priority Node Service Data?
		else {
			HandleLowPriorityNodeServiceData(msg);
		}
	}
}

//-----------------------------------------------------------------------------

int32_t AbstractAerospaceDataManager::ProcessOutgoingMessages()
{
	int32_t iMsgCount = 0;

	#if defined (USE_CAN_DDS_A)
	iMsgCount += m_ddsA.SubmitRequest(m_pUnit);
	#endif

	#if defined (USE_CAN_APS_A)
	iMsgCount += m_apsA.SubmitRequest(m_pUnit);
	#endif

	#if defined (USE_CAN_DUS_B)
	iMsgCount += m_dusB.SubmitResponse(m_pUnit);
	#endif

	return iMsgCount;
}

//-----------------------------------------------------------------------------

void AbstractAerospaceDataManager::HandleEmergencyEventData(const MessageAerospace& msg)
{
#if defined (USE_CAN_APS_A)
	// This is bootloader error message. The node wants to get the new programm.
	if(msg.GetId()==can::idEE_BootloadError) {
		if(m_apsA.IsActive()==false) {
			PRINTF("CAN Boot error request from node %i.\n", msg.GetSender());
			ResetAPS();
			StartAPS(msg.GetSender(), msg.GetSender());
			//, (uint8_t)msg.GetDataType());
		}
	}
#endif
}

//-----------------------------------------------------------------------------

void AbstractAerospaceDataManager::HandleHighPriorityNodeServiceData(
	const MessageAerospace& msg
)
{
	const uint32_t uId   	 = msg.GetId();			// Msg id.
	const uint8_t byReceiver = msg.GetReceiver(); 	// Node id.
	Service eService = (Service)msg.GetServiceCode(); // Get eService code

// 	PRINTF(
// 		"HPNSD - id = %i, node = %i, eService = %i.\n",
// 		uId, m_pUnit->GetNodeId(), eService
// 	);

	// Here we accept all responses on channel zero
	if(uId == idNS_C00_Response) {
		// Handle data download eService
		switch(eService) {
		#if defined (USE_CAN_DDS_A)
		case sDataDownload: 			m_ddsA.ProcessResponse(m_pUnit, msg);	break;
		#endif
		#if defined (USE_CAN_FPS_A)
		case sFlashProgramming: 	m_fpsA.ProcessResponse(m_pUnit, msg);	break;
		#endif
		#if defined (USE_CAN_DUS_A)
		case sDataUpload: 			m_dusA.ProcessResponse(m_pUnit, msg); 	break;
		#endif
		#if defined (USE_CAN_MIS_A)
		case sModuleInformation: 	m_misA.ProcessResponse(m_pUnit, msg);	break;
		#endif
		#if defined (USE_CAN_NIS_A)
		case sNodeIdSetting: 		m_nisA.ProcessResponse(m_pUnit, msg);	break;
		#endif
		#if defined (USE_CAN_MCS_A)
		case sModuleConfiguration:	m_mcsA.ProcessResponse(m_pUnit, msg);	break;
		#endif
		default:
			break;
		}
	}

	else if(uId == idNS_C00_Request) {
		// here we accept only broadcasts (0) or this hardware id.
		if(byReceiver != 0 && byReceiver != m_pUnit->GetNodeId())
			return; // not handled.

		MessageAerospace msgOut;
		switch(eService) {
		// Handle identification eService here
		case sIdentification:
			msgOut.Set(
				Identifier(lPrimary, idNS_C00_Response, m_pUnit->GetNodeId(), msg.GetSender()),
				dtUChar4, 0, msg.GetMessageCode(),
			   Register(m_pUnit->GetHWVersion(), m_pUnit->GetSWVersion(), 0,0)
			);
			PRINTF("ID request -> id = %i", m_pUnit->GetNodeId());
			m_pUnit->PostMessage(msgOut);
			break;

		#if defined (USE_CAN_DDS_B)
		case sDataDownload: m_ddsB.ProcessRequest(m_pUnit, msg); break;
		#endif
		#if defined (USE_CAN_FPS_B)
		case sFlashProgramming: m_fpsB.ProcessRequest(m_pUnit, msg); break;
		#endif
		#if defined (USE_CAN_DUS_B)
		case sDataUpload: m_dusB.ProcessRequest(m_pUnit, msg); break;
		#endif
		#if defined (USE_CAN_MIS_B)
		case sModuleInformation: m_misB.ProcessRequest(m_pUnit, msg); break;
		#endif
		#if defined (USE_CAN_NIS_B)
		case sNodeIdSetting: m_nisB.ProcessRequest(m_pUnit, msg); break;
		#endif
		#if defined (USE_CAN_MCS_B)
		case sModuleConfiguration: m_mcsB.ProcessRequest(m_pUnit, msg); break;
		#endif
		
		// The sign of life processing.
		case sSignOfLife: 
			m_pUnit->ProcessSignOfLife(msg); 
			break;
		
		default:
			PRINTF("HPN Service id=%u (from node %u) is not supported.\n", 
				(uint16_t)eService,
				(uint16_t)msg.GetSender()
			);
			break;
		}
	}
}

//-----------------------------------------------------------------------------

void AbstractAerospaceDataManager::HandleNormalOperationData(
	const MessageAerospace& msg
)
{
	// Rather than one big switch, we have several switches inside if blocks.
	// This seems to be faster.
	const uint32_t uId = msg.GetId();
//	const DataType eDataType = msg.GetDataType();

	// Source data pointer used in memcpy.
	uint32_t uiMsgData = msg.GetData().ui32;
// 	float f = msg.GetData(); // used for debug

	// Flight State Air Data block (m_fsad)
	if(m_pFSAD && IsInRange(idFirstValidFlightStateAirData, uId, idLastValidFlightStateAirData)) {
		CopyData(
			idFirstValidFlightStateAirData, idLastValidFlightStateAirData,
			uId, (uint32_t*)m_pFSAD, uiMsgData,
			m_pFSAD, sizeof(FlightStateAirData)
		);
	}

	// Flight Controls Data (m_fcd)
	else if(m_pFCD && IsInRange(idFirstValidFlightControlsData, uId, idLastValidFlightControlsData)) {
		CopyData(
			idFirstValidFlightControlsData, idLastValidFlightControlsData,
			uId, (uint32_t*)m_pFCD, uiMsgData,
			m_pFCD, sizeof(FlightControlsData)
		);
	}

	// Engine Fuel Supply Data (m_efsd)
	else if(m_pEFSD && IsInRange(idFirstValidEngineFuelSupplyData, uId, idLastValidEngineFuelSupplyData)) {
		// Here we have two separate CopyData calls because the
		// struct AerospaceData.h has "B" variables commented out.
		if(uId <= idEngineStatus2_3) {
			CopyData(
				idFirstValidEngineFuelSupplyData, idEngineStatus2_3,
				uId, (uint32_t*)m_pEFSD, uiMsgData,
				m_pEFSD, sizeof(EngineFuelSupplyData)
			);
		}
		// skip engine B and go straight to the fuel
		else if(uId >= idFuelPumpFlowRate_0) {
			CopyData(
				idFuelPumpFlowRate_0, idLastValidEngineFuelSupplyData,
				uId, (uint32_t*)(&(m_pEFSD->afFuelPumpFlowRate[0])), uiMsgData,
				m_pEFSD, sizeof(EngineFuelSupplyData)
			);
		}
	}

	// Power Transmition Data (m_ptd)
	else if(m_pPTD && IsInRange(idFirstValidPowerTransmitionData, uId, idLastValidPowerTransmitionData)) {
		CopyData(
			idFirstValidPowerTransmitionData, idLastValidPowerTransmitionData,
			uId, (uint32_t*)m_pPTD, uiMsgData,
			m_pPTD, sizeof(PowerTransmitionData)
		);
	}

	// Hydraulic Data (m_hd)
	else if(m_pHD && IsInRange(idFirstValidHydraulicData, uId, idLastValidHydraulicData)) {
		CopyData(
			idFirstValidHydraulicData, idLastValidHydraulicData,
			uId, (uint32_t*)m_pHD, uiMsgData,
			m_pHD, sizeof(HydraulicData)
		);
	}

	// Electric Data (m_ed)
	else if(m_pED && IsInRange(idFirstValidElectricData, uId, idLastValidElectricData)) {
		CopyData(
			idFirstValidElectricData, idLastValidElectricData,
			uId, (uint32_t*)m_pED, uiMsgData,
			m_pED, sizeof(ElectricData)
		);
	}

	// Navigation Data (m_nd)
	else if(m_pND && IsInRange(idFirstValidNavData, uId, idLastValidNavData)) {
		CopyData(
			idFirstValidNavData, idLastValidNavData,
			uId, (uint32_t*)m_pND, uiMsgData,
			m_pND, sizeof(NavData)
		);
	}

	// Landing Gear Data (m_lgd)
	else if(m_pLGD && IsInRange(idFirstValidLandingGearData, uId, idLastLandingGearData)) {
		CopyData(
			idFirstValidLandingGearData, idLastValidLandingGearData,
			uId, (uint32_t*)m_pLGD, uiMsgData,
			m_pLGD, sizeof(LandingGearData)
		);
	}

	// Miscellaneous Data (m_md)
	else if(m_pMD && IsInRange(idFirstValidMiscellaneousData, uId, idLastValidMiscellaneousData)) {
		CopyData(
			idFirstValidMiscellaneousData, idLastValidMiscellaneousData,
			uId, (uint32_t*)m_pMD, uiMsgData,
			m_pMD, sizeof(MiscellaneousData)
		);
	}

	// Kanardia extension
	else if(IsInRange(idFirstValidKanardiaNOD, uId, idLastValidKanardiaNOD)) {
		if(m_pKD) {
			// This is a bit specific
			uint32_t iCylinder = msg.GetServiceCode();
			switch(uId) {
				case idKanDaquCHT:
					if(iCylinder >= MAX_DAQU_CYLINDER)
						PRINTF("Invalid CHT cylinder # = %i", iCylinder);
					else
						m_pKD->afCHT[iCylinder] = msg.GetData();
					break;

				case idKanDaquEGT:
					if(iCylinder >= MAX_DAQU_CYLINDER)
						PRINTF("Invalid EGT cylinder # = %i", iCylinder);
					else
						m_pKD->afEGT[iCylinder] = msg.GetData();
					break;

				default:
					CopyData(
						idKanDateJulianDay, idLastValidKanardiaNOD,
						uId, (uint32_t*)&m_pKD->uiJulianDayGPS, uiMsgData,
						m_pKD, sizeof(KanardiaData)
					);
/*					if(uId==idKanDateJulianDay)
						PRINTF("JD=%u.\n", m_pKD->uiJulianDayGPS);
					else if(uId==idKanTimeSec)
						PRINTF("TS=%f.\n", m_pKD->fTimeSecGPS);*/
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------

void AbstractAerospaceDataManager::HandleLowPriorityNodeServiceData(
	const MessageAerospace& msg
)
{
	// Currently only APS is running on the channel 100

	const uint32_t uId   	 = msg.GetId();		 	 // Msg id.
	const uint8_t byReceiver = msg.GetReceiver(); 	 // Who is to receive this message.
	Service eService = (Service)msg.GetServiceCode(); // Get eService code

	// Here we accept all responses on channel 100 (reserved for APS)
	if(uId == idNS_APS_Response) {
		// It should be APS eService
		if(eService == sApplicationProgramming) {
			// The download helper class will handle the message.
			#if defined (USE_CAN_APS_A)
			m_apsA.ProcessResponse(m_pUnit, msg);
			#endif
		}
	}
	// Here we accept all requests on channel 100 (reserved for APS)
	else if(uId == idNS_APS_Request) {
		// here we accept only broadcasts (0) or this hardware id.
		if(byReceiver != m_pUnit->GetNodeId())
			return; // not handled.

		if(eService == sApplicationProgramming) {
			#if defined (USE_CAN_APS_B)
			m_apsB.ProcessRequest(m_pUnit, msg);
			#endif
		}
	}
	else {
		PRINTF("LPN Service id=%i is not supported.", uId);
	}
}

//-----------------------------------------------------------------------------
#if defined (USE_CAN_DDS_A)
/** Start the Data Download Service (DDS). We forward this call to the
    DDS_A class.

	 \retval true Service has started successfully.
	 \retval false Service failed to start. The DDS is busy or transmit buffer is full.
 */

bool AbstractAerospaceDataManager::StartDownloadService(
	uint8_t byNodeB, 		//! Node B - the node to receive the data.
	uint8_t byMsgSize, 	//! Number of CAN messages needed to transfer the data.
	uint32_t uiDataId		//! Data identifier (this id of the data - not the address).
)
{
	// forward the call to the node A object.
	return m_ddsA.Start(m_pUnit, byNodeB, byMsgSize, uiDataId);
}
#endif


//-----------------------------------------------------------------------------
#if defined (USE_CAN_FPS_A)
/** A message is constructed and send to the node B.
	 \sa FPS_A, FPS_B

	 \retval true Message was posted successfully.
	 \retval flase Command failed.
 */
bool AbstractAerospaceDataManager::ProgramFlash(
	uint8_t byNodeB 		 //! Node B - the node to receive the command.
)
{
	// forward the call to the helper object
	return m_fpsA.Start(m_pUnit, byNodeB);
}
#endif

//-----------------------------------------------------------------------------
#if defined (USE_CAN_APS_A)

bool AbstractAerospaceDataManager::StartAPS(
	uint8_t byNodeB,			//! Node B - the node to receive the command.
	uint8_t byHardwareId 	//! Type of the hardware that we want to program.
)
{
	return m_apsA.Start(m_pUnit, byNodeB, byHardwareId);
}
#endif

//-----------------------------------------------------------------------------

#if defined (USE_CAN_DUS_A)
/** Start the Data Upload Service (DDS). We forward this call to the
    DUS_A class.

	 \retval true Service has started successfully.
	 \retval false Service failed to start. The DUS is busy or transmit buffer is full.
 */

bool AbstractAerospaceDataManager::StartUploadService(
	uint8_t byNodeB, 		//! Node B - the node which will send the data.
	uint8_t byMsgSize, 	//! Number of CAN messages needed to transfer the data.
	uint32_t uiDataId		//! Data identifier (this id of the data - not the address).
)
{
	// forward the call to the node A object.
	return m_dusA.Start(m_pUnit, byNodeB, byMsgSize, uiDataId);
}
#endif

//-----------------------------------------------------------------------------

#if defined (USE_CAN_MIS_A)

/** Serial number is optional. When given (non-zero) only node which
    serial number matches given argument will respond.
 */

bool AbstractAerospaceDataManager::RequestModuleInformation(
	uint8_t byNodeB, 		//! Node B - the node which should respond.
	uint8_t byRequest,	//! Question code.
	uint32_t uiInfo		//! Optional information (could be serial number).
)
{
	return m_misA.Start(m_pUnit, byNodeB, byRequest, uiInfo);
}
#endif

//-----------------------------------------------------------------------------

#if defined (USE_CAN_NIS_A)
bool AbstractAerospaceDataManager::ChangeNodeId(
	uint8_t byNodeB,
	uint8_t byNewId,
	uint32_t uiSerial
)
{
	return m_nisA.Start(m_pUnit, byNodeB, byNewId, uiSerial);
}
#endif

//-----------------------------------------------------------------------------

#if defined (USE_CAN_MCS_A)
bool AbstractAerospaceDataManager::ConfigureModule(
	uint8_t byNodeB,
	uint8_t byConfigId,
	DataType eDataType,
	Register rData)
{
	return m_mcsA.Start(m_pUnit, byNodeB, byConfigId, eDataType, rData);
}
#endif

} // can namespace
