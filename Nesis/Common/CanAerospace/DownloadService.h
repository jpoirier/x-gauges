#ifndef CAN_DDS_SERVICE_H
#define CAN_DDS_SERVICE_H

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
#include "CRC-32.h"

namespace can {

//-----------------------------------------------------------------------------
#if defined(USE_CAN_DDS_A) || defined(USE_CAN_DDS_B)
	//! Possible responses in DDS tramission.
	enum DDS_Response {
		ddsXOn = 1,				//!< Transfer is ON.
		ddsXOff = 0,			//!< Transfer should wait a little.
		ddsAbort = -1,			//!< Transfer is aborted.
		ddsInvalid = -2		//!< Transfer is invalid.
	};


	// Magu = 1; Magu specific commands
	// Daqu =  2; Daqu specific commands
	#define DDS_DAQU_CONFIG_FLASH				0x20

	// Airu = 3; Airu specific commands
	#define DDS_AIRU_ALT_COEFF					0x30
	#define DDS_AIRU_VEL_COEFF					0x31
	#define DDS_AIRU_ACC_COEFF					0x32
	#define DDS_AIRU_PQR_COEFF					0x33
	#define DDS_AIRU_MOUNT_COEFF				0x34
	#define DDS_AIRU_MOUNT_CORR_COEFF		0x35

	// Mabu = 4; Mabu specific commands
	#define DDS_MABU_TRANSFER_BLOCK			0x40
#endif


#if defined(USE_CAN_DDS_A)
//-----------------------------------------------------------------------------
/** \brief Data Download Service helper class for the node A.

	The class provides procedures needed to handle the
	Download Data Service (DDS) as it is defined by the
	CANairspace protocol.
	This service is used to transfer the data from node A to
	node B. The class implements the procedures for the node A,
	which is the node that starts the service.
	Procedures for the node B are implemented in class DDS_B.

	\sa DDS_B
*/

class DDS_A {
public:
	//! State variable for downloading service in node A.
	enum State {
		sReady,					//!< Ready for download.
		sWaitingConfirm,		//!< Waiting for node B confirming message.
		sSendingDataOn,		//!< Sending data to node B.
		sSendingDataOff,		//!< Sending data to node B, but waiting for a while.
		sSuccess,				//!< The transfer was successful. Note the state is still active.
		sCRCFailed,				//!< Error: processing checksum failed.
		sTimeoutConfirm,		//!< Error: timeout before confirm.
		sTimeoutTransfer,		//!< Error: timeout before complete transfer.
		sInvalidDataA,			//!< Error: invalid or no data to transfer - reported by node A.
		sInvalidDataB,			//!< Error: invalid data id - reported by node B.
		sAbortB,					//!< Error: transmition was aborted by node B.
		sBusyB,					//!< Error: node B was busy - thus the data transfer was rejected.
	};

public:
	//! Constructor - set initial state.
	DDS_A();
	//! Reset the download service to initial state.
	void Reset();

	//! Return the receiver.
	uint8_t GetReceiver() const
	{ return m_byNodeB; }

	//! Return true if download service is active.
	bool IsActive() const { return m_eState > sReady && m_eState < sSuccess; }
	//! Return true if download has found in error state.
	bool IsError() const { return m_eState > sSuccess; }
	//! Return true if we are in an active data sending mode.

	//! Start the download service.
	bool Start(AbstractUnit* pUnit,
		uint8_t byNodeB, uint8_t byMsgSize, uint32_t uiDataId);
	//! Submit request messages during download to the node B.
	int32_t SubmitRequest(AbstractUnit* pUnit);
	//! Process response messages from node B.
	void ProcessResponse(AbstractUnit* pUnit, const MessageAerospace& msg);
	//! Check timeout.
	void CheckTimeout();

	//! Update internal CRC with the transmited data.
	void UpdateCRC(const Register& rData);

public:
	//! Data id of the structure that is downloading.
	uint32_t m_uiDataId;
	//! Node B - the addressed node - the node which receives the data.
	uint8_t m_byNodeB;
	//! Current state of download service.
	State m_eState;
	//! How many data messages will be transmitted.
	uint8_t m_bySize;
	//! Current message index. 1 is the first msg; 0 is not used.
	uint8_t m_byIndex;
	//! Timeout flag.
	bool m_bTimeout;
	//! Checksum value for the data sent.
	common::CRC32 m_crc;
};
#endif

#if defined(USE_CAN_DDS_B)
//-----------------------------------------------------------------------------
/** \brief Data Download Service helper class for the node B.

	The class provides procedures needed to handle the
	Download Data Service (DDS) as it is defined by the
	CANairspace protocol.
	This service is used to transfer the data from node A to
	node B. The class implements the procedures for the node B,
	which is the node that is receiving the data messages.
	Procedures for the node A are implemented in the class DDS_A.

	\sa DDS_A
*/

class DDS_B {
public:
	//! State variable for downloading service in node B.
	enum State {
		sReady,					//!< Ready for download.
		sReceivingData,		//!< Receiving data from node A.
		sInvalidIndex,			//!< Invalid index in msg from A.
	};
public:
	//! Constructor - set initial state.
	DDS_B();
	//! Reset the download service to initial state.
	void Reset();

	//! Process request messages from the node A.
	void ProcessRequest(AbstractUnit* pUnit, const MessageAerospace& msg);

	//! Return true if DDS for node B is in busy state.
	bool IsBusy() const
	{ return m_eState != sReady; }
	//! Check timeout.
	void CheckTimeout();

	//! Return the last accepted sender.
	uint8_t GetSender() const
	{ return m_bySender; }

	//! Update internal CRC with the received data.
	void UpdateCRC(const Register& rData);
public:
	//! The data structure identifier.
 	uint32_t m_uiDataId;
	//! Current message received.
	uint8_t m_byIndex;
	//! Total number of messages to be received.
	uint8_t m_bySize;
	//! The sender of the download messages.
	uint8_t m_bySender;
	//! Checksum value for the data received.
	common::CRC32 m_crc;
	//! Timeout flag.
	bool m_bTimeout;
	//! Current internal state.
	State m_eState;
};
#endif

//-----------------------------------------------------------------------------
} // can namespace

#endif
