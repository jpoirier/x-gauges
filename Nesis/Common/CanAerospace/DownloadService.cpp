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
#include "DownloadService.h"


namespace can {

// ##########################################################################

#if defined(USE_CAN_DDS_A)

DDS_A::DDS_A()
{
	Reset();
}

// --------------------------------------------------------------------------

void DDS_A::Reset()
{
	m_byNodeB		= 0;
	m_eState 		= sReady;
	m_bySize 		= 0;
	m_byIndex 		= 0;
	m_bTimeout 		= false;
	m_uiDataId		= 0;
	m_crc.Reset();
}

// --------------------------------------------------------------------------

void DDS_A::UpdateCRC(const Register& rData)
{
	m_crc.Update(rData.by[0]);
	m_crc.Update(rData.by[1]);
	m_crc.Update(rData.by[2]);
	m_crc.Update(rData.by[3]);
}

// --------------------------------------------------------------------------
/** This is step one in the Data Download Service (DDS). This node (A) tries
    to start transmition to the node B. Thus we send transmition request to
	 the node B.

	 Only one transmitting DDS service can be active, hence we first check
	 if download service is available.

	 The node B has 100 ms to respond with a confirmation message.

	 \retval true Service request was submited successfully.
	 \retval false Service failed to start. The DDS is busy or transmit buffer is full.
 */

bool DDS_A::Start(
	AbstractUnit* pUnit,	//! Hardware depended unit object.
	uint8_t byNodeB, 			//! Node B - the node to receive the data.
	uint8_t byMsgSize, 		//! Number of CAN messages needed to transfer the data.
	uint32_t uiDataId			//! Data structure identifier (this id of the data - not the address).
)
{
	// DDS step one (1)

	// Already in progress - can't start downloading.
	// The download can start if and only if the internal state is sReady.
 	if(m_eState != sReady)
		return false;

	// Kill any pending timeouts and reset the variables.
	pUnit->SetADownloadTimeout(0, &m_bTimeout);
	Reset();

	// Send request download to the addressing node.
	MessageAerospace msg(
		Identifier(lPrimary, idNS_C00_Request, pUnit->GetNodeId(), byNodeB),
		dtMemId,					// we are sending the DataId
		sDataDownload,			// data download service
		byMsgSize,				// how many messages we want to transmit.
		uiDataId					// data structure identifier (what will be transmited).
	);

	// Try to post the message.
	if(pUnit->PostMessage(msg)) {
		m_eState = sWaitingConfirm;
		m_byNodeB = byNodeB;
		m_uiDataId = uiDataId;
		m_bySize = byMsgSize;
		// We will wait up to 100 ms for the answer.
		pUnit->SetADownloadTimeout(100, &m_bTimeout);
		return true;
	}

	// Failed to post.
	return false;
}

// --------------------------------------------------------------------------

int32_t DDS_A::SubmitRequest(AbstractUnit* pUnit)
{
	// DDS step four (4)

	// Keep sending messages until all data is send.
	if(m_eState == sSendingDataOn) {
		// Get the data message that we want to transmit
		Register rData;
		DataType eDataType;

		// Ask for the download data to be transfered.
		if(pUnit->GetDownloadData(m_uiDataId, m_byIndex, eDataType, rData)) {
			// Compose the message.
			MessageAerospace msg(
				Identifier(lPrimary, idNS_C00_Request, pUnit->GetNodeId(), m_byNodeB),
				eDataType,
				sDataDownload,
				m_byIndex,
				rData
			);

			// The is no harm if the PostMessage returns false. This
			// means that the outgoing message buffer is full and that
			// we should wait a little.
			if(pUnit->PostMessage(msg)) {
				// If the message is posted successfully,
				UpdateCRC(rData);
				// check if we reached the last message.
				if(m_byIndex == m_bySize) {
					// In this case we should wait for the CRC from node B.
					// We temporary stop sending (there is nothing to send anymore)
					// Note that the timeout is still counting down.
					m_eState = sSendingDataOff;
				}
				else {
					// No, move index to next message. Next message will be
					// posted in another loop.
					m_byIndex++;
				}
				return 1; // One message posted.
			}
		}
		// The data is not available, but it should be. This is an error.
		else {
			PRINTF("DDS A: Download msg data not available for DataId=%u, index=%i\n",
				m_uiDataId, (int32_t)m_byIndex
			);
			// Report error and stop sending data.
			m_eState = sInvalidDataA;
		}
	}
	return 0;
}

// --------------------------------------------------------------------------

void DDS_A::ProcessResponse(
	AbstractUnit* pUnit,
	const MessageAerospace& msg
)
{
	// The messages can be handled only if we are in an active state.
	if(IsActive()) {
		// Is it correct node?
		if(msg.GetSender() == m_byNodeB) {
			// DDS step three (3)

			// Node A is in waiting state.
			// We receive message from node B. It can have three
			// returning values.
			if(m_eState == sWaitingConfirm) {
// 				PRINTF("DDS A: step three. State = %i, Data = %i\n", m_eState, msg.GetData().i32);
				// The answer is positive rXOn = 1. We can move on step four - sending data.
				if(msg.GetData().i32 == ddsXOn) {
					//assert(msg.GetMessageCode() == m_bySize);
					m_eState = sSendingDataOn;
					m_byIndex = 1; // start with the first message - step 4.
					// We do not want to listen forever. Set the timeout.
					pUnit->SetADownloadTimeout(TRANSFER_TIMEOUT_MS(m_bySize), &m_bTimeout);
				}
				// The answer is negative. We must go into error mode.
				else {
					m_eState = (msg.GetData().i32 == ddsAbort ? sBusyB : sInvalidDataB);
					PRINTF("DDS A:Node %i rejected download. Response was %i.\n",
						(int32_t) m_byNodeB, msg.GetData().i32);
				}
			}

			// DDS step six (6)

			// Node B sends some response during node A data transmition.
			else /*if(m_eState == sSendingDataOn || m_eState == sSendingDataOff)*/ {
				// Did we get some response during sending the data?
				if(msg.GetDataType() == dtLong) {
					switch(msg.GetData().i32) {
					// Continue sending data at given index.
					// Note that node B may change the index and that
					// some data needs to be resent.
					case ddsXOn:
						PRINTF("DDS A:XOn\n");
						m_eState = sSendingDataOn;
						m_byIndex = msg.GetMessageCode();
						break;
					// Stop sending data. Timeout is still pending.
					// Node B must reenable transmistion by sending rXOn
					// and new starting index.
					case ddsXOff:
						PRINTF("DDS A:XOff\n");
						m_eState = sSendingDataOff;
						break;

					// Node B responded by Invalid. Go into error condition.
					case ddsInvalid:
						PRINTF("DDS A:X Invalid\n");
						m_eState = sInvalidDataB;
						break;
					// Node B responded by Abort. Go into error condition.
					case ddsAbort:
						PRINTF("DDS A:X Abort\n");
						m_eState = sAbortB;
						break;
					}
				}

				// DDS step eight (8) - the final step
				// Did we get the Checksum?
				else if(msg.GetDataType() == dtChksum) {
// 					PRINTF("DDS A: Checksum = 0x%x\n", m_crc.Get());
					// it must be the last number
					if(msg.GetMessageCode() == m_bySize) {
						uint32_t uiCrcReceived = msg.GetData().ui32;
						// compare the checksums.
						if(m_crc.Get() == uiCrcReceived) {
							// This makes the download complete.
							m_eState = sSuccess;
// 							PRINTF("DDS A: Success.\n");
							// Reset any pending timouts
							pUnit->SetADownloadTimeout(0, &m_bTimeout);
						}
						else {
							m_eState = sCRCFailed;
							PRINTF("DDS A: Node %i, Checksum failed, DataId=%u.\n",
								(int32_t)msg.GetSender(), m_uiDataId
							);
						}
					}
					else {
						PRINTF("DDS A: Node %i, checksum received on msg %i, but was expected on msg %i.\n",
							(int32_t)msg.GetSender(), (int32_t)msg.GetMessageCode(), (int32_t)m_bySize
						);
					}
				}
				// unexpected data type
				else {
					PRINTF("DDS A: Node %i, Unexpected data type %i.\n",
						(int32_t)msg.GetSender(), (int32_t)msg.GetDataType()
					);
				}
			}
		}
		// Uncorrect node
		else {
			PRINTF("DDS A: Responce msg from incorrect node %i; Node %d was expected.\n",
				(int32_t)msg.GetSender(), (int32_t)m_byNodeB
			);
		}
	}
/*	// Inactive
	else {
		PRINTF("DDS A: Node %i, Response msg on inactive download service.\n",
			(int32_t)msg.GetNodeId()
		);
	}*/
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
void DDS_A::CheckTimeout()
{
	if(m_bTimeout) {
		switch(m_eState) {
		case sWaitingConfirm:
			// Timeout on waiting confirm - go into error state.
			m_eState = sTimeoutConfirm; break;
		case sSendingDataOn:
		case sSendingDataOff:
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

#if defined(USE_CAN_DDS_B)

DDS_B::DDS_B()
{
	m_bySender = 0;
	Reset();
}

// --------------------------------------------------------------------------

void DDS_B::Reset()
{
	m_uiDataId = 0;
	m_byIndex  = 0;
	m_bySize   = 0;
	m_bTimeout = false;
	m_eState   = sReady;
//	m_bySender = 0; // Don't reset the sender. It will remain valid until next download is accepted.
	m_crc.Reset();
}

// --------------------------------------------------------------------------

void DDS_B::UpdateCRC(const Register& rData)
{
	m_crc.Update(rData.by[0]);
	m_crc.Update(rData.by[1]);
	m_crc.Update(rData.by[2]);
	m_crc.Update(rData.by[3]);
}

// --------------------------------------------------------------------------

void DDS_B::ProcessRequest(AbstractUnit* pUnit, const MessageAerospace& msg)
{
	// DDS step two (2)

	// dtMemId defines the initial situation.
	if(msg.GetDataType()==dtMemId) {
		// We must send something back to node A.
		// We will return one of three possibilities.
		// XOn     ... the transfer is accepted.
		// Abort   ... the transfer is busy (can't accept another).
		// Invalid ... this data structure id is not supported.

		int32_t iRet = ddsXOn;
		if(IsBusy())
			iRet = ddsAbort;			// data id,			number of messages
		else if(!pUnit->AcceptDownload(msg.GetData().ui32, msg.GetMessageCode()))
			iRet = ddsInvalid;

		// Make the response message - all nodes will hear this.
		MessageAerospace msgOut(
			Identifier(lPrimary, idNS_C00_Response, pUnit->GetNodeId(), msg.GetSender()),
			dtLong, sDataDownload, msg.GetMessageCode(),
			Register(iRet)
		);
		// Remember some data if post was successful.
		if(pUnit->PostMessage(msgOut)) {
			PRINTF("DDS B - Step 2: iRet = %i\n", iRet);
			if(iRet==ddsXOn) {
				pUnit->SetBDownloadTimeout(0, &m_bTimeout); // clear any pending timeout.
				Reset();
				m_eState = sReceivingData;
				m_bySize = msg.GetMessageCode(); // number of messages to accept
				m_uiDataId = msg.GetData().ui32; // data structure id
				m_bySender = msg.GetSender();		// remember the sender.
				// We do not want to listen forever. Set the timeout.
				pUnit->SetBDownloadTimeout(TRANSFER_TIMEOUT_MS(m_bySize), &m_bTimeout);
				return;
			}
		}
		// Can't post a message.
		else {
			PRINTF("DDS B: Unable to post a confirmation message.\n");
		}
	}

	// DDS step five (5)

	// Receiving data messages and storing them. We may return
	// a message that will pause/resume or abort the download.
	// If this is the last message to be received, a CRC is sent
	// back. See step seven.
	else if(m_eState == sReceivingData) {
		//ASSERT(msg.GetMessageCode() == m_byIndex+1);
		if(msg.GetMessageCode() != m_byIndex+1) {
			// TODO verify this.
			m_eState = sInvalidIndex;
		}
		else {
			int32_t iRet = pUnit->StoreDownloadMessage(m_uiDataId, msg);
			// The data msg was accepted.
			if(iRet == ddsXOn) {
				m_byIndex++;
				UpdateCRC(msg.GetData());
	
				// DDS step seven (7)
	
				// This message termiates the download service by
				// returning CRC value of downloaded data.
				if(m_byIndex == m_bySize) {
					MessageAerospace msgOut(
						Identifier(lPrimary, idNS_C00_Response, pUnit->GetNodeId(), msg.GetSender()),
						dtChksum, sDataDownload, msg.GetMessageCode(),
						Register(m_crc.Get())
					);
					if(pUnit->PostMessage(msgOut)==false) {
						PRINTF("DDS B: Unable to post final checksum message.\n");
						return;
					}
					// We are ready to accept new download service.
					pUnit->SetBDownloadTimeout(0, &m_bTimeout);
					PRINTF("DDS B: Checksum = 0x%x\n", m_crc.Get());
					Reset();
				}
			}
			// We need to send iRet, because it is not rXOn.
			// Here we actually implementing step five.
			else {
				// TODO Če je xOn po xOff, potem moramo poslati s katerim
				// indeksom naj node A nadaljuje.
				MessageAerospace msgOut(
					Identifier(lPrimary, idNS_C00_Response, pUnit->GetNodeId(), msg.GetSender()),
					dtLong, sDataDownload, msg.GetMessageCode(),
					Register(iRet)
				);
				// Is posted successfully, remember some data
				if(pUnit->PostMessage(msgOut)==false) {
					PRINTF("DDS B: Unable to post message.\n");
				}
			}
		}
	}
	// We are not in receiving mode anymore - most probaby due timeout.
	else {
		PRINTF("DDS B: Message received in sReady mode.\n");
	}
}

// --------------------------------------------------------------------------

void DDS_B::CheckTimeout()
{
	if(m_bTimeout) {
		// stop receiving and allow new start.
		PRINTF("DDS B: Timeout!\n");
		Reset();
	}
}

#endif

} // can namespace
