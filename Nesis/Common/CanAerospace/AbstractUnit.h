#ifndef _CAN_ABSTRACT_UNIT_H
#define _CAN_ABSTRACT_UNIT_H

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

#include "ApplicationDefines.h"

#include "MessageAerospace.h"
#include "CRC-32.h"

//-----------------------------------------------------------------------------
// TODO Fine tune the timeout settings

/** \brief Max time needed to transfer iMsgCount messages.

	  If we assume 8000 messages per second, this gives
	  0.125 ms per message. Assuming about 16 times slower
	  transfer rate (about 2 ms per message) we can calculate
	  some reasonable estimate for complete transfer given in
     milliseconds.
 */

#define TRANSFER_TIMEOUT_MS(iMsgCount)		(uint32_t)((iMsgCount)*2 + 100)

namespace can {

//-----------------------------------------------------------------------------
/** \brief Abstract class that acts as a virtual communication unit.

	 Here we define pure virtual member functions, which are needed
	 to handle node CANaerospace services.
 */

class AbstractUnit
{
protected:
	//! Constructor
	AbstractUnit() {;}
public:
	//! Destructor
	virtual ~AbstractUnit() {;}

public:
	// General - needed to send messages
	//! Post Message - by derived class.
	virtual bool PostMessage(const MessageAerospace& msg) const = 0;

	// Identification service
	
	//! Get hardware node id needed by CAN.
	virtual uint8_t GetNodeId() const = 0;
	//! Get software version.
	virtual uint8_t GetSWVersion() const = 0;
	//! Get hardware version.
	virtual uint8_t GetHWVersion() const = 0;
	
	// Sign of Life service (Kanardia specific)
	
	//! Process the "Sign Of Life" message.
	virtual void ProcessSignOfLife(const MessageAerospace& msg) = 0;

	// Data download service

	#if defined(USE_CAN_DDS_A)
	//! Set download timeout given in ms. On timeout, it must set the given flag.
	virtual void SetADownloadTimeout(uint32_t ms, bool* pbFlag) = 0;
	//! Get download data to be transmited based on uiDataId and current message index.
	virtual bool GetDownloadData(uint32_t uiDataId, uint8_t byIndex,
			DataType& eDataType, Register& rData) const = 0;
	#endif
	#if defined(USE_CAN_DDS_B)
	//! Set download timeout given in ms. On timeout, it must set the given flag.
	virtual void SetBDownloadTimeout(uint32_t ms, bool* pbFlag) = 0;
	//! Can we accept the requested download. Return true to accept.
	virtual bool AcceptDownload(uint32_t uiDataId, uint8_t byMsgCount) const = 0;
	//! The data was dowloaded. We need to store it somewhere. Return value has special meaning.
	virtual int32_t StoreDownloadMessage(uint32_t uiDataId, const MessageAerospace& msg) = 0;
	#endif

	// Flash programmig service

	#if defined(USE_CAN_FPS_A)
	//! Set flash programming timeout given in ms. On timeout, it must set the given flag.
	virtual void SetAFlashProgrammingTimeout(uint32_t ms, bool* pbFlag) = 0;
	#endif
	#if defined(USE_CAN_FPS_B)
	//! Program the flash (copy data from ram to flash).
	virtual bool ProgramFlash() = 0;
	#endif

	// Application programming service

	#if defined(USE_CAN_APS_A)
	//! Set application programming timeout given in ms. On timeout, it must set the given flag.
	virtual void SetAApplicationProgrammingTimeout(uint32_t ms, bool* pbFlag) = 0;
	//! Get number of application pages to program (for given node id).
	virtual int32_t GetAPSPageCount(uint8_t byNodeId) = 0;
	//! Get address of the page i. Each page is 512 bytes long.
	virtual uint32_t* GetAPSPageAddress(uint32_t uiPage) = 0;
	#endif
	#if defined(USE_CAN_APS_B)
	//! A bit uC specific call used to start programming mode of the bootloader.
	virtual void CallBootAppProgrammer(uint32_t uiPageCount, uint8_t byNodeA) = 0;
	#endif

	// Data upload service

	#if defined(USE_CAN_DUS_A)
	//! Set upload timeout given in ms. On timeout, it must set the given flag.
	virtual void SetAUploadTimeout(uint32_t ms, bool* pbFlag) = 0;
	//! Store uploaded data received in the message from node B.
	virtual bool StoreUploadMessage(const MessageAerospace& msg) = 0;
	#endif
	#if defined(USE_CAN_DUS_B)
	//! Set upload timeout given in ms. On timeout, it must set the given flag.
	virtual void SetBUploadTimeout(uint32_t ms, bool* pbFlag) = 0;
	//! Can we accept the requested upload. Return true to accept.
	virtual bool AcceptUpload(uint32_t uiDataId, uint8_t byMsgCount) const = 0;
	//! Get the data to be uploaded. Check the return value.
	virtual bool GetUploadData(uint32_t uiDataId, uint8_t byIndex,
			DataType& eDataType, Register& rData) = 0;
	#endif

	// Module information service

	#if defined(USE_CAN_MIS_A)
	//! Process the module information returned by node B.
	virtual void ProcessModuleInfoResponse(const MessageAerospace& msg) = 0;
	#endif
	#if defined(USE_CAN_MIS_B)
	//! Obtain module information required by node A.
	virtual bool GetModuleInfoData(uint8_t byQuestion, uint32_t uiInfo,
		DataType& eDataType, Register& rData) const = 0;
	#endif

	// Node id setting service

	#if defined(USE_CAN_NIS_A)
	//! This is responce from B. This implementation simply ignore the message.
	virtual void ProcessNodeIdSettingResponse(const MessageAerospace& /*msg*/) {};
	#endif
	#if defined(USE_CAN_NIS_B)
	//! The unit gets new node id. New ids must be 128 or above.
	virtual bool SetNodeId(uint8_t byNewNodeId, uint32_t uiSerial) = 0;
	#endif

	// Module configuration service

	#if defined(USE_CAN_MCS_A)
	//! Process the module configuration message returned by node B.
	virtual void ProcessModuleConfigResponse(const MessageAerospace& msg) = 0;
	#endif
	#if defined(USE_CAN_MCS_B)
	//! Configure some internal module settings. It may be module specific.
	virtual int32_t ConfigureModule(const MessageAerospace& msg) = 0;
	#endif

};

//-----------------------------------------------------------------------------
} // can namespace

#endif
