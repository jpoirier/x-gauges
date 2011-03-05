#ifndef CAN_MODULE_CONFIG_SERVICE_H
#define CAN_MODULE_CONFIG_SERVICE_H

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

#if defined(USE_CAN_MCS_A) || defined(USE_CAN_MCS_B)

	#define MCS_SERIAL_NUMBER							0x01
	#define MCS_SERIAL_DATE								0x02
	#define MCS_VENDOR_NUMBER							0x03
	#define MCS_CONSOLE_OUTPUT							0x04
	#define MCS_ENABLE_REPORTS							0x05
	#define MCS_RESET										0x06
	#define MCS_HARDWARE_VERSION						0x07

	// MAGU = 1

	#define MCS_MAGU_CAL_ADD							0x10
	#define MCS_MAGU_CAL_FINISH						0x11
	#define MCS_MAGU_CAL_RESET							0x12
	#define MCS_MAGU_CAL_RESTORE_FACTORY			0x13
	#define MCS_MAGU_REPORT_RAW						0x14

	// DAQU = 2
	#define MCS_DAQU_FUEL_LEVEL						0x20
	#define MCS_DAQU_FUEL_FLOW_FACTOR				0x21
	#define MCS_DAQU_FUEL_FLOW_K						0x22
	#define MCS_DAQU_RPM_DIVISOR						0x23

	// AIRU = 3
	#define MSC_AIRU_CALIBRATION_UPDATE				0x30
	#define MCS_AIRU_QNH									0x31

	// MABU
	#define MCS_MABU_ENGINE_TOTAL_TIME				0x40
	#define MCS_MABU_REFERENCE_RPM					0x41
	#define MCS_MABU_SECONDARY							0x42
	#define MCS_MABU_LOGGER_TRANSFER					0x43
	#define MCS_MABU_RECORD								0x44
	#define MCS_MABU_CELL_CAPACITY					0x45
	#define MCS_MABU_CHARGER_VOLT_F					0x46
	#define MCS_MABU_CELL_CHARGE						0x47

	// Mabu logger transfer use additional parameter. Please see code in the 
	// MabuUnitCAN.cpp file.

	// MABU
	#define MCS_KALU_ROTATE								0x80

	// Internal values for data register
	// Used by MCS_MAGU_CALIBRATE_FINISH

	#define FINISH_CALCULATE_ONLY						1
	#define FINISH_CALCULATE_SET						2
	#define FINISH_CALCULATE_SET_FACTORY			3

	// Internal values used for MCS_CONSOLE_OUTPUT

	#define CONSOLE_OUTPUT_DISABLE					0
	#define CONSOLE_OUTPUT_UART						1
	#define CONSOLE_OUTPUT_CAN							2

	// Non-zero means an error.
	#define MCS_SUCCESS									0
	#define MCS_FAILURE									1

	#define MCS_DISABLE									0
	#define MCS_ENABLE									1
	
	#define MCS_FALSE										0
	#define MCS_TRUE										1

	#define MCS_UPDATE_FROM_FLASH						0
	#define MCS_UPDATE_FROM_RAM						1
	
	//! MACRO for packing MABU transfer configuration service info. 
	// b=first block (27 bits), c=count (top 5 bits)
	#define MCS_MABU_PACK_TRANSFER_INFO(b,c)  ((b) & 0x07ffffff) + (((c) & 0x1f)<<27)

#endif


//-----------------------------------------------------------------------------

#if defined(USE_CAN_MCS_A)

/** \brief Module Configuration Service helper class for the node A.

	The class provides procedures needed to handle the
	Module Configuration Service (FPS) as it is roughly
	defined by the CANairspace protocol.

	Node A simply posts a request message to the node B with
	the MessageCode byte set to some specific value representing
	value to be set (configured). Node B prepares an answer to the question.
	It is assumed that only one message will be sent in response.

	\sa MCS_B
*/

class MCS_A {
public:
	//! Constructor - set initial state.
	MCS_A();

	//! Start the download service.
	bool Start(AbstractUnit* pUnit, uint8_t byNodeB, uint8_t byConfigId,
				  DataType eDataType, Register rData);
	//! Process response messages from node B.
	void ProcessResponse(AbstractUnit* pUnit, const MessageAerospace& msg);
};
#endif

//-----------------------------------------------------------------------------

#if defined(USE_CAN_MCS_B)
/** \brief Module Information Service helper class for the node B.

	The class provides procedures needed to handle the
	Module Configuration Service (FPS) as it is roughly
	defined by the CANairspace protocol.

	Node B prepares a response message to the configuration message sent by node A.
	It is assumed that only one message will be sent in response.

	\sa MCS_A
*/

class MCS_B {
public:
	//! Constructor - set initial state.
	MCS_B();

	//! Process request messages from the node A.
	void ProcessRequest(AbstractUnit* pUnit, const MessageAerospace& msg);
};
#endif

//-----------------------------------------------------------------------------
} // can namespace

#endif
