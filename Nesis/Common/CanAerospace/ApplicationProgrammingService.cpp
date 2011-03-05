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

#include "Defines.h"
#include "ApplicationProgrammingService.h"

#define PAGE_STARTUP_TIMEOUT			6000
#define PAGE_SENDING_TIMEOUT			4500

namespace can {

// ##########################################################################

#if defined (USE_CAN_APS_A)
APS_A::APS_A()
{
	Reset();
}

// --------------------------------------------------------------------------

void APS_A::Reset()
{
	m_byNodeB		= 0;
	m_eState 		= sReady;
	m_uiPageCount	= 0;
	m_iMsgIndex 	= 0;
	m_bTimeout 		= false;
	m_uiPageId		= 0;
	m_puiPage		= 0;
	m_byBootLoaderVer = 0;	
}

// --------------------------------------------------------------------------

/** This is the step one in the Application Programming Service (APS).
	 This node (A) sends programming request to the node B.

	 Only one transmitting APS service can be active, hence we first check
	 if download service is available.

	 The node B has quite some time to respond with a confirmation message.
	 Namely, node B may need to erase complete flash before it sends
	 response (step 2) indicating tha page id to download.

	 \retval true Service request was submited successfully.
	 \retval false Service failed to start. The APS is busy or transmit buffer is full.
 */

bool APS_A::Start(
	AbstractUnit* pUnit,	//! Hardware depended unit object.
	uint8_t byNodeB, 		//! Node B - the node to receive the data.
	uint8_t byHardwareId,//! Type of the hardware that we want to program.
	uint8_t byBootVer		//! The bootloader version.
)
{
	// APS step one (1)

	// Already in progress - can't start new service.
	// The service can start if and only if the internal state is sReady.
 	if(m_eState != sReady)
		return false;

	// Kill any pending timeouts and reset the variables.
	pUnit->SetAApplicationProgrammingTimeout(0, &m_bTimeout);
	Reset();

	// Get number of pages required by the node.
	uint32_t uiPageCount = pUnit->GetAPSPageCount(byHardwareId);
	if(uiPageCount == 0)
		return false;

	// Send request download to the addressing node.
	// request on channel 100 - used for service only,
	MessageAerospace msg(
		Identifier(lPrimary, idNS_APS_Request, pUnit->GetNodeId(), byNodeB),
		dtULong,							// we are sending the number of pages (uint32_t)
		sApplicationProgramming, 	// data download service
		(uint8_t)0xff,					// must be 0xff.
		uiPageCount						// how many pages will be transmited.
	);

	// Try to post the message.
	if(pUnit->PostMessage(msg)) {
		m_eState = sWaitingPage;
		m_byNodeB = byHardwareId;
		m_uiPageCount = uiPageCount;
		m_byBootLoaderVer = byBootVer;
		// We will wait up to some time for the answer.
		// The node B must erase complete flash before the responce will be send
		// and this may take considerable amount of time.
		pUnit->SetAApplicationProgrammingTimeout(PAGE_STARTUP_TIMEOUT, &m_bTimeout);
		return true;
	}

	// Failed to post.
	return false;
}

// --------------------------------------------------------------------------

int APS_A::SubmitRequest(AbstractUnit* pUnit)
{
	// APS step four (4)

	// Keep sending messages until all data is send.
	if(IsActive()) {
		if(m_eState == sSendingPage) {
			// Get the data message that we want to transmit
			Register rData(m_puiPage[m_iMsgIndex]);
			// Compose the message.
			MessageAerospace msg(
				Identifier(lPrimary, idNS_APS_Request, pUnit->GetNodeId(), m_byNodeB),
				dtULong,
				sApplicationProgramming,
				(uint8_t)m_iMsgIndex,
				rData
			);

			// The is no harm if the PostMessage returns false. This
			// means that the outgoing message buffer is full and that
			// we should wait a little.
			if(pUnit->PostMessage(msg)) {
				m_iMsgIndex++;
				// This was the last message in the page
				if(m_iMsgIndex == 128) {
					m_uiPageId++;
					// If there are any new pages to transfer, wait for the node B
					// to request a new page.
					if(m_uiPageId < m_uiPageCount) {
						m_eState = sWaitingPage;
					}
					// This was the last page
					else {
						pUnit->SetAApplicationProgrammingTimeout(0, &m_bTimeout);
						m_eState = sSuccess;
					}
				}
				return 1; // one message was posted.
			}
		}
	}
	// The data is not available, but it should be. This is an error.
	else if(m_eState != sReady) {
		PRINTF("APS A: SubmitRequest called on an invalid state %u.\n", m_eState);
		// Report error and stop sending data.
		m_eState = sError;
	}
	return 0; // no messages were posted.
}

// --------------------------------------------------------------------------

void APS_A::ProcessResponse(
	AbstractUnit* pUnit,
	const MessageAerospace& msg
)
{
	// The messages can be handled only if we are in an active state.
	if(IsActive()) {
		// Is it correct node?
		if(msg.GetSender() == m_byNodeB) {
			// APS step three (3)

			// Node A is in sWaitingPage state.
			// We received message from node B. It holds the new page id.
			if(m_eState == sWaitingPage) {
				ASSERT(msg.GetDataType() == dtMemId);
				m_uiPageId = msg.GetData().i32;
				// get address of data for given page. Address must point to
				// block which is at least (128 x 4 = 512) bytes long.
				m_puiPage = pUnit->GetAPSPageAddress(m_uiPageId);
				if(m_puiPage==NULL) {
					PRINTF("APS A: Page %i is not available.\n", m_uiPageId);
					m_eState = sInvalidPage;
					pUnit->SetAApplicationProgrammingTimeout(0, &m_bTimeout);
				}
				else {
					m_iMsgIndex = 0; // reset message
					m_eState = sSendingPage;
					// Time limit for sending one page to node B.
					pUnit->SetAApplicationProgrammingTimeout(PAGE_SENDING_TIMEOUT, &m_bTimeout);
				}
			}

			else {
				PRINTF("APS A: Node A not in waiting state, but msg from node %i was received.\n",
					(int)m_byNodeB
				);
			}
		}
		// Incorrect node
		else {
			PRINTF("APS A: Response from node %i; Node %d was expected.\n",
				(int)msg.GetSender(), (int)m_byNodeB
			);
		}
	}
	// Inactive
	else {
		PRINTF("APS A: Node %i, Response msg on inactive download service.\n",
			(int)msg.GetSender()
		);
	}
}

// --------------------------------------------------------------------------
/** Check if a timeout has appeared and take appropriate actions.
    The timeout on node A is important in two cases:

	 - When node A waits confirmation from node B (up to 100 ms).
	 - When node A is transmitting messages and the process is not
      completed soon enough.
    - In any other case, the transmition is already in some static
      state and the timeout is ignored.
 */
void APS_A::CheckTimeout()
{
	if(m_bTimeout) {
		if(IsActive()) {
			if(m_uiPageId == 0 && m_iMsgIndex == 0)
				m_eState = sTimeoutStartup;
			else
				m_eState = sTimeoutSending;
		}
		m_bTimeout = false;
	}
}

#endif

// ##########################################################################

#if defined (USE_CAN_APS_B)

APS_B::APS_B()
{
}

// --------------------------------------------------------------------------

void APS_B::ProcessRequest(AbstractUnit* pUnit, const MessageAerospace& msg)
{
	// APS step two (2)

	// check if the message is right
	if(msg.GetDataType() == dtULong && msg.GetMessageCode() == 0xff) {
		// Jump into bootloader programming code here.
		PRINTF("Call Boot_APS(%u).\n", msg.GetData().ui32);
		pUnit->CallBootAppProgrammer(msg.GetData().ui32, msg.GetSender());
	}
}
#endif

// --------------------------------------------------------------------------

} // can namespace
