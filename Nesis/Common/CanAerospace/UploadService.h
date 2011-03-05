#ifndef __CAN_UPLOAD_SERVICE_H
#define __CAN_UPLOAD_SERVICE_H

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
#if defined(USE_CAN_DUS_A) || defined(USE_CAN_DUS_B)
//! Possible responses in DUS tramission.
enum DUS_Response {
	dusXOn = 1,				//!< Transfer is ON.
	dusXOff = 0,			//!< Transfer should wait a little.
	dusAbort = -1,			//!< Transfer is aborted.
	dusInvalid = -2		//!< Transfer is invalid.
};

// Magu = 1

#define UPLOAD_MAGU_COEFF							0x10
#define UPLOAD_MAGU_DATA							0x11
#define UPLOAD_MAGU_COEFF_FLASH					0x12

// Daqu = 2

#define UPLOAD_DAQU_CONFIG_FLASH					0x20

// Airu = 3

#define UPLOAD_AIRU_ALT_COEFF_FLASH				0x30
#define UPLOAD_AIRU_VEL_COEFF_FLASH				0x31
#define UPLOAD_AIRU_ACC_COEFF_FLASH				0x32
#define UPLOAD_AIRU_PQR_COEFF_FLASH				0x33
#define UPLOAD_AIRU_MOUNT_COEFF_FLASH			0x34
#define UPLOAD_AIRU_MOUNT_CORR_COEFF_FLASH	0x35


#endif

#if defined(USE_CAN_DUS_A)
//-----------------------------------------------------------------------------
/** \brief Data Upload Service helper class for the node A.
	
	The class provides procedures needed to handle the 
	Data Upload Service (DUS) as it is defined by the
	CANairspace protocol. 
	This service is used to transfer the data from node B to 
	node A. The class implements the procedures for the node A, 
	which is the node that starts the service. 
	Procedures for the node B are implemented in class DUS_B.

	\sa DUS_B
*/

class DUS_A {
public:
	//! State variable for downloading service in node A.
	enum State {
		sReady,					//!< Ready for upload.
		sWaitingConfirm,		//!< Waiting for node B confirming message.
		sReceivingData,		//!< Receiving data from node B.
		sReceivingCRC,			//!< Receiving CRC from node B.
		sSuccess,				//!< The transfer was successful. Note the state is still active.
		sCRCFailed,				//!< Error: processing checksum failed.
		sTimeoutConfirm,		//!< Error: timeout before confirm.
		sTimeoutTransfer,		//!< Error: timeout before complete transfer.
		sInvalidDataA,			//!< Error: invalid or no data to transfer - reported by node A.
		sInvalidDataB,			//!< Error: invalid data id - reported by node B.
		sAbortB,					//!< Error: transmition was aborted by node B.
		sBusyB,					//!< Error: node B was busy - thus the data transfer was rejected.
		sInvalidState			//!< Error: a msg received in invalid state.
	};
	
public:
	//! Constructor - set initial state.
	DUS_A();
	//! Reset the upload service into initial state.
	void Reset();

	//! Return true if upload service is active.
	bool IsActive() const { return m_eState > sReady && m_eState < sSuccess; }
	//! Return true if service is in an error state.
	bool IsError() const { return m_eState > sSuccess; }

	//! Start the upload service.
	bool Start(AbstractUnit* pUnit,
		uint8_t byNodeB, uint8_t byMsgSize, uint32_t uiDataId);		
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
	//! How many data messages will be received.
	uint8_t m_byMsgSize;
	//! Current message index. 1 is the first msg; 0 is not used.
	uint8_t m_byMsgIndex;
	//! Timeout flag.
	bool m_bTimeout;
	//! Checksum value for the data sent.
	common::CRC32 m_crc;
}; 
#endif

#if defined(USE_CAN_DUS_B)
//-----------------------------------------------------------------------------
/** \brief Data Upload Service helper class for the node B.
	
	The class provides procedures needed to handle the 
	Data Upload Service (DUS) as it is defined by the
	CANairspace protocol. 
	This service is used to transfer the data from node B to 
	node A. The class implements the procedures for the node B, 
	which is the node that is sending the data messages.
	Procedures for the node A are implemented in the class DUS_A.

	\sa DUS_A
*/

class DUS_B {
public:
	//! State variable for upload service in node B.
	enum State {
		sReady,				//!< Ready for upload.
		sWaitingTimeout,  //!< Wait some time before start sending messages.
		sSendingData,		//!< Sending data to node A.
		sSendingCRC,		//!< Sending CRC to node A.
	};
public:
	//! Constructor - set initial state.
	DUS_B();
	//! Reset the download service to initial state.
	void Reset();

	//! Process request messages from the node A.
	void ProcessRequest(AbstractUnit* pUnit, const MessageAerospace& msg);
	//! Submit responce messages during upload back to node A.
	int32_t SubmitResponse(AbstractUnit* pUnit);

	//! Return true if DUS for node B is in busy state.
	bool IsBusy() const
	{ return m_eState != sReady; }
	//! Check timeout.
	void CheckTimeout(AbstractUnit* pUnit);

	//! Update internal CRC with the sent data.
	void UpdateCRC(const Register& rData);
public:
	//! The data structure identifier.
 	uint32_t m_uiDataId;
	//! Current message sent.
	uint8_t m_byMsgIndex;
	//! Total number of messages to be sent.
	uint8_t m_byMsgSize;
	//! Checksum value for the data sent.
	common::CRC32 m_crc;
	//! Timeout flag.
	bool m_bTimeout;
	//! Current internal state.
	State m_eState;
	uint8_t m_byNodeA;
};
#endif

//-----------------------------------------------------------------------------
} // can namespace

#endif
