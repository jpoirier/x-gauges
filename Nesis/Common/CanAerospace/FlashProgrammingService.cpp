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

#include "Defines.h"
#include "FlashProgrammingService.h"

namespace can {

// ##########################################################################

#if defined(USE_CAN_FPS_A)

FPS_A::FPS_A()
{
	Reset();
}

// --------------------------------------------------------------------------

void FPS_A::Reset()
{
	m_byNodeB		= 0;
	m_eState 		= sReady;
	m_bTimeout 		= false;
}

// --------------------------------------------------------------------------

/** This node (A) transmits a flash programming request to the node B. A 
	 timeout is also started and the node B has 1000 ms to send a responce.

	 \retval true Service request was submited successfully.
	 \retval false Service failed to start. The DDS is busy or transmit buffer is full.
 */

bool FPS_A::Start(
	AbstractUnit* pUnit,	//! Hardware depended unit object.
	uint8_t byNodeB 		//! Node B - the node to do the flash programming.
)
{
	// FPS step one (1)

	// Already in progress - can't start downloading.
	// The download can start if and only if the internal state is sReady.
 	if(m_eState != sReady)
		return false;

	// Kill any pending timeouts and reset the variables.
	pUnit->SetAFlashProgrammingTimeout(0, &m_bTimeout);
	Reset();
	
	// Send request download to the addressing node.
	MessageAerospace msg(
		Identifier(lPrimary, idNS_C00_Request, pUnit->GetNodeId(), byNodeB),
		dtNoData,				// we are sending no data (zeroes)
		sFlashProgramming,	// programming service 
		0xEC,						// safety code must be correct.
		0					
	);

	// Try to post the message.
	if(pUnit->PostMessage(msg)) {
		m_eState = sPending;
		m_byNodeB = byNodeB;
		// We will wait up to 1000 ms for the answer.
		pUnit->SetAFlashProgrammingTimeout(1000, &m_bTimeout);
		return true;
	}
	
	// Failed to post.
	return false;
}

// --------------------------------------------------------------------------

void FPS_A::ProcessResponse(
	AbstractUnit* pUnit, 			//! Hardware depended unit object.
	const MessageAerospace& msg	//! The response message
)
{
	PRINTF("State = %i", m_eState);
	// The messages can be handled only if we are in an active state.
	if(IsActive()) {
		// Is it correct node?
		if(msg.GetSender() == m_byNodeB) {
			// FPS step three (3) - the last step.
			// The node B response is in the message code uint8_t.
			pUnit->SetAFlashProgrammingTimeout(0, &m_bTimeout);

			int iCode = (char)msg.GetMessageCode();
			switch(iCode) {
			case fpsSuccess: 
				m_eState = sSuccess;	break;
			case fpsInvalidCode: 
				m_eState = sCodeError; break;
			default: 
				m_eState = sError; break;
			}
		}
		// Uncorrect node
		else {
			PRINTF("FPS A: Responce msg from incorrect node %i; Node %d was expected.\n",
				(int)msg.GetSender(), (int)m_byNodeB
			);
		}
	}
	// Inactive
	else {
		PRINTF("FPS A: Node %i, Response msg on flash programming service.\n",
			(int)msg.GetSender()
		);
	}
}

// --------------------------------------------------------------------------
/** Check if a timeout has appeared and take appropriate actions.
	 A timeout can appear when node A waits response message from 
	 node B (up to 1000 ms). 
 */
void FPS_A::CheckTimeout()
{
	if(m_bTimeout)
		m_eState = sTimeoutError;
}

#endif

// ##########################################################################

#if defined(USE_CAN_FPS_B)

FPS_B::FPS_B()
{
	Reset();
}

// --------------------------------------------------------------------------

void FPS_B::Reset()
{
	m_eState   = sReady;
}

// --------------------------------------------------------------------------

void FPS_B::ProcessRequest(
	AbstractUnit* pUnit, 			//! Hardware depended unit object.
	const MessageAerospace& msg	//! Message to be processed.
)
{
	// FPS step two (2)
	ASSERT(msg.GetDataType()==dtNoData);

	// We must send something back to node A.
	char chRet;
	if(IsBusy())
		chRet = (char)fpsBusy;	
	else if (msg.GetMessageCode() != 0xEC)
		chRet = (char)fpsInvalidCode;
	else 
		chRet = (pUnit->ProgramFlash() ? fpsSuccess : fpsFailed);
	
	// Make the response message to node A.
	MessageAerospace msgOut(
		Identifier(lPrimary, idNS_C00_Response, pUnit->GetNodeId(), msg.GetSender()),
		dtNoData, sFlashProgramming, chRet, 
		(uint32_t)0
	);
	if(pUnit->PostMessage(msgOut))
		PRINTF("FPS B - Step 2: byRet = %i\n", (int)chRet); 

	m_eState = sReady;
}

// --------------------------------------------------------------------------
#endif

} // can namespace
