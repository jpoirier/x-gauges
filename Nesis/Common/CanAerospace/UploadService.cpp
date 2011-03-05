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
#include "UploadService.h"


namespace can {

// ##########################################################################

#if defined(USE_CAN_DUS_A)

DUS_A::DUS_A()
{
	Reset();
}

// --------------------------------------------------------------------------

void DUS_A::Reset()
{
	m_byNodeB		= 0;
	m_eState 		= sReady;
	m_byMsgSize 	= 0;
	m_byMsgIndex	= 0;
	m_bTimeout 		= false;
	m_uiDataId		= 0;
	m_crc.Reset();
}

// --------------------------------------------------------------------------

void DUS_A::UpdateCRC(const Register& rData)
{
	m_crc.Update(rData.by[0]);
	m_crc.Update(rData.by[1]);
	m_crc.Update(rData.by[2]);
	m_crc.Update(rData.by[3]);
}

// --------------------------------------------------------------------------
/** This is step one in the Data Upload Service (DUS). This node (A) sends
	 request to the node B. Node B should start tramsnition of data to node A.

	 Only one DUS service can be active, hence we first check
	 if the upload service is available.

	 The node B has 100 ms to respond with a confirmation message.

	 \retval true Service request was submited successfully.
	 \retval false Service failed to start. The DUS is busy or transmit buffer is full.
 */

bool DUS_A::Start(
	AbstractUnit* pUnit,	//! Hardware depended unit object.
	uint8_t byNodeB, 		//! Node B.
	uint8_t byMsgSize, 	//! Number of CAN messages needed to transfer the data.
	uint32_t uiDataId		//! Data structure identifier (this id of the data - not the address).
)
{
	// DUS step one (1)

	// Already in progress - can't start downloading.
	// The download can start if and only if the internal state is sReady.
 	if(m_eState != sReady)
		return false;

	// Kill any pending timeouts and reset the variables.
	pUnit->SetAUploadTimeout(0, &m_bTimeout);
	Reset();
	
	// Send request download to the addressing node.
	MessageAerospace msg(
		Identifier(lPrimary, idNS_C00_Request, pUnit->GetNodeId(), byNodeB),
		dtMemId,					// we are sending the DataId
		sDataUpload,			// data download service 
		byMsgSize,				// how many messages we want to transmit.
		uiDataId					// data structure identifier (what will be transmited).
	);

	// Try to post the message.
	if(pUnit->PostMessage(msg)) {
		m_eState = sWaitingConfirm;
		m_byNodeB = byNodeB;
		m_uiDataId = uiDataId;
		m_byMsgSize = byMsgSize;
		// We will wait up to 200 ms for the answer.
		pUnit->SetAUploadTimeout(200, &m_bTimeout);
		return true;
	}
	
	// Failed to post.
	return false;
}

// --------------------------------------------------------------------------

void DUS_A::ProcessResponse(
	AbstractUnit* pUnit, 
	const MessageAerospace& msg
)
{
	// The messages can be handled only if we are in an active state.
	if(IsActive()) {
		// Is it correct node?
		if(msg.GetSender() == m_byNodeB) {
			// DUS step three (3)

			// Node A is in waiting state.
			// We receive message from node B. It can have three
			// returning values.
			if(m_eState == sWaitingConfirm) {
				PRINTF("DUS A: step three. State = %i, Data = %i\n", m_eState, msg.GetData().i32);
				// The answer is positive rXOn = 1. We can move on step four - sending data.
				if(msg.GetData().i32 == dusXOn) {
					//assert(msg.GetMessageCode() == m_byMsgSize);
					m_eState = sReceivingData;
					m_byMsgIndex = 1; // start with the first message - step 4.
					// We do not want to listen forever. Set the timeout.
					pUnit->SetAUploadTimeout(TRANSFER_TIMEOUT_MS(m_byMsgSize), &m_bTimeout);
				}
				// The answer is negative. We must go into error mode.
				else {
					m_eState = (msg.GetData().i32 == dusAbort ? sBusyB : sInvalidDataB); 
					PRINTF("DUS A: Node %i rejected upload. Response was %i.\n",
						(int) m_byNodeB, msg.GetData().i32);
				}
			}
			
			// DUS step five (5)

			// Node B is sending us the upload data.
			else if(m_eState == sReceivingData) {
				// Check the index
				uint8_t byIndex = msg.GetMessageCode();
				// Store the data (do something with the data)
				if(byIndex == m_byMsgIndex) {
					UpdateCRC(msg.GetData());
					pUnit->StoreUploadMessage(msg);
					if(m_byMsgIndex == m_byMsgSize)
						m_eState = sReceivingCRC;
					else
						m_byMsgIndex++;
				}
				// Indices don't match. Error.
				else {
					PRINTF("DUS A: Index mismatch. Received %i, expected %i.\n",
						(int) byIndex, (int) m_byMsgIndex);
					Reset();
					pUnit->SetAUploadTimeout(0, &m_bTimeout);
				}
			}

			// DUS step seven (7)
			else if(m_eState == sReceivingCRC) {
				if(msg.GetDataType()==dtChksum) {
					pUnit->SetAUploadTimeout(0, &m_bTimeout);
					if(m_crc.Get()==msg.GetData().ui32) {
						m_eState = sSuccess;
						PRINTF("Upload success, chksum=0x%x\n", msg.GetData().ui32);
					}
					else {
						m_eState = sCRCFailed;
					}
				}
				else {
					PRINTF("DUS A: Checksum from node %i in state %i expected.\n",
							(int) msg.GetSender(), (int) m_eState);
					m_eState = sInvalidState;
				}
			}

			// Error
			else {
				PRINTF("DUS A: Responce from node %i in state %i.\n",
						(int) msg.GetSender(), (int) m_eState);
				m_eState = sInvalidState;
			}
		}
		// Uncorrect node
		else {
			PRINTF("DDS A: Responce msg from incorrect node %i; Node %d was expected.\n",
				(int32_t)msg.GetSender(), (int32_t)m_byNodeB
			);
		}
	}
	// Inactive
	else {
		PRINTF("DDS A: Node %i, Response msg on inactive download service.\n",
			(int32_t)msg.GetSender()
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
void DUS_A::CheckTimeout()
{
	if(m_bTimeout) {
		switch(m_eState) {
		case sWaitingConfirm: 
			// Timeout on waiting confirm - go into error state.
			m_eState = sTimeoutConfirm; break;
		case sReceivingData: 
		case sReceivingCRC: 
			// Timeout on active/inactive transmit - go into error state.
			m_eState = sTimeoutTransfer; break;
		default:
			// Keep current state. We are either finnished or in an error mode.
			break;
		}
		m_bTimeout = false;
	}
}

#endif

// ##########################################################################

#if defined(USE_CAN_DUS_B)

DUS_B::DUS_B()
{
	Reset();
}

// --------------------------------------------------------------------------

void DUS_B::Reset()
{
	m_uiDataId 		= 0;
	m_byMsgIndex  	= 0;
	m_byMsgSize   	= 0;
	m_bTimeout 		= false;
	m_eState   		= sReady;
	m_byNodeA		= 0;
	m_crc.Reset();
}

// --------------------------------------------------------------------------

void DUS_B::UpdateCRC(const Register& rData)
{
	m_crc.Update(rData.by[0]);
	m_crc.Update(rData.by[1]);
	m_crc.Update(rData.by[2]);
	m_crc.Update(rData.by[3]);
}

// --------------------------------------------------------------------------

void DUS_B::ProcessRequest(AbstractUnit* pUnit, const MessageAerospace& msg)
{
	// DUS step two (2)
	PRINTF("Request received.\n");
	// dtMemId defines the initial situation.
	if(msg.GetDataType()==dtMemId) {
		// We must send something back to node A.
		// We will return one of three possibilities.
		// XOn     ... the transfer is accepted.
		// Abort   ... the transfer is busy (can't accept another).
		// Invalid ... this data structure id is not supported.

		int32_t iRet = dusXOn;
		if(IsBusy())
			iRet = dusAbort;			   // data id,			  number of messages
		else if(!pUnit->AcceptUpload(msg.GetData().ui32, msg.GetMessageCode()))
			iRet = dusInvalid;

		// Make the response message - all nodes will hear this.
		MessageAerospace msgOut(
			Identifier(lPrimary, idNS_C00_Response, pUnit->GetNodeId(), msg.GetSender()),
			dtLong, sDataUpload, msg.GetMessageCode(),
			Register(iRet)
		);
		// Remember some data if post was successful.
		if(pUnit->PostMessage(msgOut)) {
			PRINTF("DUS B - Step 2: iRet = %i\n", iRet); 
			if(iRet==dusXOn) {
				Reset();
				m_eState = sWaitingTimeout;
				m_byMsgIndex = 1;
				m_byMsgSize = msg.GetMessageCode(); // number of messages to accept
				m_uiDataId = msg.GetData().ui32; // data structure id
				m_byNodeA = msg.GetSender();		// who will receive the messages.
				// We should wait AT LEAST 10 ms before transmiting the first message.
				pUnit->SetBUploadTimeout(10, &m_bTimeout); 
				return;
			}
		}
		// Can't post a message.
		else {
			PRINTF("DUS B: Unable to post a confirmation message.\n");
		}
	}
	// We are not in receiving mode anymore - most probably due timeout.
	else {
		PRINTF("DUS B: Upload request invalid data type.\n"); 
	}
}
// --------------------------------------------------------------------------

int32_t DUS_B::SubmitResponse(AbstractUnit* pUnit)
{
	// DUS step four (4) ... continue from timeout

	// When in sending mode, we keep sending messages back to node A
	// until all data is sent.
	if(m_eState == sSendingData) {
		// Get the data message that we want to transmit
		Register rData;
		DataType eDataType;

		// Ask for the upload data to be transfered.
		if(pUnit->GetUploadData(m_uiDataId, m_byMsgIndex, eDataType, rData)) {
			// Compose the message.
			MessageAerospace msg(
				Identifier(lPrimary, idNS_C00_Response, pUnit->GetNodeId(), m_byNodeA),
				eDataType, sDataUpload, m_byMsgIndex,
				rData
			);

			// The is no harm if the PostMessage returns false. This
			// means that the outgoing message buffer is full and that
			// we should wait a little.
			if(pUnit->PostMessage(msg)) {
				UpdateCRC(rData);
				// Check if CRC is sent next.
				if(m_byMsgIndex == m_byMsgSize) {
					m_eState = sSendingCRC;
				}
				// Move index to next message. Next message will be 
				// posted in another loop.
				else {
					m_byMsgIndex++;	
				}
				return 1; // One message posted.
			}
/*			else {
				PRINTF("#");
			}*/
		}
		// The data is not available, but it should be. This is an error.
		else {
			PRINTF("DUS B: Upload msg data not available for DataId=%u, index=%i\n",
				m_uiDataId, (int32_t)m_byMsgIndex
			);
			// Go into ready mode.
			Reset();
			pUnit->SetBUploadTimeout(0, &m_bTimeout);
		}
	}
	// DUS step six (6)

	// The data upload is almost complete. CRC comes last.
	else if(m_eState == sSendingCRC) {
		MessageAerospace msg(
			Identifier(lPrimary, idNS_C00_Response, pUnit->GetNodeId(), m_byNodeA),
			dtChksum, sDataUpload, m_byMsgIndex,
			Register(m_crc.Get())
		);
		if(pUnit->PostMessage(msg)==false) {
			return 0;
		}
		// We are ready to accept new upload service.
		pUnit->SetBUploadTimeout(0, &m_bTimeout);
		PRINTF("DUS B: Checksum = 0x%x.\n", m_crc.Get());
		Reset();
		return 1;
	}
	return 0;
}

// --------------------------------------------------------------------------

void DUS_B::CheckTimeout(AbstractUnit* pUnit)
{
	if(m_bTimeout) {
		// DUS step four (4)
		// Finaly, we may start sending
		if(m_eState == sWaitingTimeout) {
			m_eState = sSendingData;
			m_bTimeout = false;
			// define new timeout
			pUnit->SetBUploadTimeout(TRANSFER_TIMEOUT_MS(m_byMsgSize), &m_bTimeout);
			return;
		}

		// stop sending and allow new start.
		PRINTF("DUS B: Timeout!\n");
		Reset();
	}
}

#endif

} // can namespace
