#ifndef CAN_MODULE_INFO_SERVICE_H
#define CAN_MODULE_INFO_SERVICE_H

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

#if defined(USE_CAN_MIS_A) || defined(USE_CAN_MIS_B)

	// General commands (for all units)
	#define MIS_SERIAL_NUMBER					0x01
	#define MIS_SERIAL_DATE						0x02
	#define MIS_VENDOR_NUMBER					0x03
	#define MIS_HARDWARE_ID						0x04
	#define MIS_VERSION							0x05
	#define MIS_SVN_REVISION					0x06
	
	// Magu = 1; Magu specific commands
	#define MIS_MAGU_CAL_DATA_COUNT			0x10
	
	// Daqu =  2; Daqu specific commands
	#define MIS_DAQU_FUEL_FLOW_FACTOR		0x20
	#define MIS_DAQU_FUEL_FLOW_K				0x21
	#define MIS_DAQU_RPM_DIVISOR				0x22
	
	// Airu = 3; Airu specific commands
	#define MIS_AIRU_TEMP_RAW					0x30
	#define MIS_AIRU_AR_RAW_X					0x31
	#define MIS_AIRU_AR_RAW_Y					0x32
	#define MIS_AIRU_AR_RAW_Z					0x33
	#define MIS_AIRU_ACC_RAW_X					0x34
	#define MIS_AIRU_ACC_RAW_Y					0x35
	#define MIS_AIRU_ACC_RAW_Z					0x36
	#define MIS_AIRU_ALT_RAW					0x37
	#define MIS_AIRU_VEL_RAW					0x38
	
	// Mabu = 4; Mabu specific commands
	#define MIS_MABU_LOG_BACK					0x40
	#define MIS_MABU_STARTUP_FUEL				0x41
	#define MIS_MABU_SECONDARY					0x42
	#define MIS_MABU_QNH							0x43
	#define MIS_MABU_CELL_CAPACITY			0x44
	#define MIS_MABU_REFERENCE_RPM			0x45
	#define MIS_MABU_CHARGER_VOLT_F			0x46
#endif


//-----------------------------------------------------------------------------

#if defined(USE_CAN_MIS_A)

/** \brief Module Information Service helper class for the node A.
	
	The class provides procedures needed to handle the 
	Module Information Service (FPS) as it is roughly
	defined by the CANairspace protocol. 
	
	Node A simply posts a request message to the node B with 
	the MessageCode byte set to some specific value representing
	information request. Node B prepares an answer to the question.
	It is assumed that only one message will be sent in response.

	\sa MIS_B
*/

class MIS_A {
public:
	//! Constructor - set initial state.
	MIS_A();

	//! Start the MIS service.
	bool Start(AbstractUnit* pUnit, uint8_t byNodeB, uint8_t byQuestion, uint32_t uiSerial);
	//! Process response messages from node B.
	void ProcessResponse(AbstractUnit* pUnit, const MessageAerospace& msg);
}; 
#endif

//-----------------------------------------------------------------------------

#if defined(USE_CAN_MIS_B)
/** \brief Module Information Service helper class for the node B.
	
	The class provides procedures needed to handle the 
	Module Information Service (FPS) as it is roughly
	defined by the CANairspace protocol. 
	
	Node B prepares a response message to the request sent by node A.
	It is assumed that only one message will be sent in response.

	\sa MIS_A
*/


class MIS_B {
public:
	//! Constructor - set initial state.
	MIS_B();

	//! Process request messages from the node A.
	void ProcessRequest(AbstractUnit* pUnit, const MessageAerospace& msg);
};
#endif

//-----------------------------------------------------------------------------
} // can namespace

#endif
