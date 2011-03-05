#ifndef _CAN_FLASH_PROGRAMMING_SERVICE_H
#define _CAN_FLASH_PROGRAMMING_SERVICE_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2007 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: MIT - MIT License                                            *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include "AbstractUnit.h"
#include "MessageAerospace.h"

namespace can {

//-----------------------------------------------------------------------------
#if defined(USE_CAN_FPS_A) || defined(USE_CAN_FPS_B)
//! Possible responses in FPS
enum FPS_Response {
	fpsSuccess     = 0,		//!< Flash programming was successful.
	fpsFailed      = -1,	//!< Programer failed to program flash.
	fpsInvalidCode = -3,	//!< Invalid security code.
	fpsBusy        = -4		//!< Already in the programming mode.
};
#endif


#if defined(USE_CAN_FPS_A)

/** \brief Flash Programming Service helper class for the node A.
	
	The class provides procedures needed to handle the 
	Flash Programming Service (FPS) as it is defined by the
	CANairspace protocol. 
	
	Node A simply posts a message to the node B with the flash
   programming request. After this, node A waits for a responce
   from node B. Flash programming may take a while, so no immidiate
   responce is requred.

	\sa FPS_B
*/

class FPS_A {
public:
	//! State variable for flash programming service in node A.
	enum State {
		sReady,					//!< Ready for start.
		sPending,				//!< Waiting for node B responce.
		sSuccess,				//!< Programming was completed successfully
		sCodeError,				//!< Programming failed due to invalid safety code.
		sTimeoutError,			//!< A response did not arrived in time.
		sError					//!< Programming failed for some other reason.
	};
	
public:
	//! Constructor - set initial state.
	FPS_A();
	//! Reset the download service to initial state.
	void Reset();

	//! Return true if download service is active.
	bool IsActive() const { return m_eState == sPending; }
	//! Return true if download has found in error state.
	bool IsError() const { return m_eState > sSuccess; }
	//! Return true if we are in an active data sending mode.

	//! Start the download service.
	bool Start(AbstractUnit* pUnit, uint8_t byNodeB);
	//! Process response messages from node B.
	void ProcessResponse(AbstractUnit* pUnit, const MessageAerospace& msg);
	//! Check timeout.
	void CheckTimeout();

public:
	//! Node B - the addressed node - the node which receives the data.
	uint8_t m_byNodeB;
	//! Current state of download service.
	State m_eState;
	//! Timeout flag.
	bool m_bTimeout;
}; 
#endif

//-----------------------------------------------------------------------------

#if defined(USE_CAN_FPS_B)
/** \brief Flash Programming Service helper class for the node B.
	
	The class provides procedures needed to handle the 
	Flash Programming Service (FPS) as it is defined by the
	CANairspace protocol. 
	
	Node B is waiting for the service request message from node A.
	When such message is received, safety code is checked and
	the unit is asked to burn the flash. When completed, a response
	message is sent back to node A indicating the status of the
	operation.

	\sa FPS_A
*/

class FPS_B {
public:
	//! State variable for flash programming in node B.
	enum State {
		sReady,					//!< Ready for program.
		sProgramming,			//!< We are programming.
	};

public:
	//! Constructor - set initial state.
	FPS_B();
	//! Reset the download service to initial state.
	void Reset();

	//! Process request messages from the node A.
	void ProcessRequest(AbstractUnit* pUnit, const MessageAerospace& msg);

	//! Return true if DDS for node B is in busy state.
	bool IsBusy() const
	{ return m_eState != sReady; }
public:
	//! Current internal state.
	State m_eState;
};
#endif

//-----------------------------------------------------------------------------
} // can namespace

#endif
