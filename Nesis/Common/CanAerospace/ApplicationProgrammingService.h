#ifndef CAN_APPLICATION_PROGRAMMING_SERVICE_H
#define CAN_APPLICATION_PROGRAMMING_SERVICE_H

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

#include "AerospaceData.h"
#include "AbstractUnit.h"
#include "MessageAerospace.h"

namespace can {

#if defined (USE_CAN_APS_A)
//-----------------------------------------------------------------------------
/** \brief Application Programming Service helper class for the node A.
	
	The class provides procedures needed to handle the 
	Application Programming Service (ADS). This service is not defined
	in CANairspace protocol. It is Kanardia specific protocol.
	 
	This service is used to transfer the uC program from node A to 
	node B. The class implements the procedures for the node A, 
	which is the node that starts the service and sends the program. 
	Procedures for the node B are implemented in class APS_B.

   This means that APS_A will almost never be implemented in uC code.
	Node B (APS_B), however, should be always implemented in uC.

	\sa APS_B
*/

class APS_A {
public:
	//! State variable for downloading service in node A.
	enum State {
		sReady,					//!< Ready for start.
		sWaitingPage,			//!< Waiting for node B confirming message - which page?.
		sSendingPage,			//!< Sending page data to node B.
		sSuccess,				//!< The transfer was successful. Note the state is still active.
		sTimeoutStartup,		//!< Error: timeout on the startup.
		sTimeoutSending,		//!< Error: timeout before complete transfer.
		sInvalidPage,			//!< Error: Page is invalid - not available.
		sError,					//!< General error.
	};
	
public:
	//! Constructor - set initial state.
	APS_A();
	//! Reset the service to initial state.
	void Reset();

	//! Return true if service is active.
	bool IsActive() const { return m_eState > sReady && m_eState < sSuccess; }
	//! Return true if download has found in error state.
	bool IsError() const { return m_eState > sSuccess; }
	//! Return true if we are in an active data sending mode.

	//! Start the download service.
	bool Start(AbstractUnit* pUnit, 
		uint8_t byNodeB, uint8_t byHardwareId, uint8_t byBootVer=0);		
	//! Submit request messages during download to the node B.
	int SubmitRequest(AbstractUnit* pUnit);
	//! Process response messages from node B.
	void ProcessResponse(AbstractUnit* pUnit, const MessageAerospace& msg);
	//! Check timeout.
	void CheckTimeout();
	//! Return the bootloader version of node B.
	uint8_t GetBootloaderVer() const
	{ return m_byBootLoaderVer; }

public:
	//! Current page id that is downloading.
	uint32_t m_uiPageId;
	//! Node B - the addressed node - the node which receives the data.
	uint8_t m_byNodeB;
	//! Current state of download service.
	State m_eState;
	//! How many pages will be transmitted.
	uint32_t m_uiPageCount;
	//! Current zero based index in the page.
	int32_t m_iMsgIndex;
	//! Timeout flag.
	bool m_bTimeout;
	//! Pointer to the page data
	uint32_t* m_puiPage;
	//! Bootloader version
	uint8_t m_byBootLoaderVer;
}; 
#endif

#if defined (USE_CAN_APS_B)
//-----------------------------------------------------------------------------
/** \brief Application programming service helper class for the node B.
	
	The class provides procedures needed to handle the 
	Application Programming Service (ADS). This service is not defined
	in CANairspace protocol. It is Kanardia specific protocol.
	 
	This service is used to transfer the uC program from node A to 
	node B. The class implements the procedures for the node B, 
	which is the node that accepts the service and receives
	page by page sent by the node A. Current implementation of node B
	is pretty simple - we accept the service and we call pure C
	code in the .boot_loader section.

	Procedures for the node A are implemented in class APS_A.
   APS_B class should be implemented in uC code only.

	\sa APS_B
*/

class APS_B {
public:
	//! Constructor - set initial state.
	APS_B();

	//! Process request messages from the node A.
	void ProcessRequest(AbstractUnit* pUnit, const MessageAerospace& msg);
};
#endif

//-----------------------------------------------------------------------------
} // can namespace

#endif
