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
#include "ModuleConfigService.h"

namespace can {

// ##########################################################################

#if defined(USE_CAN_MCS_A)

MCS_A::MCS_A()
{
}

// --------------------------------------------------------------------------
/** This node (A) transmits a configuration message to the node B. 

	 \retval true Service request was submited successfully.
	 \retval false Service failed to start. The buffer is full.
 */

bool MCS_A::Start(
	AbstractUnit* pUnit,	//! Hardware depended unit object.
	uint8_t byNodeB, 		//! Node B - the receiver of the message.
	uint8_t byConfigId,	//! Configure id - what to configure.
	DataType eDataType,	//! Type of the data passed in register.
	Register rData			//! The data.
)
{
	// Send request download to the addressing node.
	MessageAerospace msg(
		Identifier(lPrimary, idNS_C00_Request, pUnit->GetNodeId(), byNodeB), 
		eDataType,				// type of the data
		sModuleConfiguration,// module confuguration service 
		byConfigId,				// what to configure.
		rData						// the data.
	);

	// Try to post the message.
	return pUnit->PostMessage(msg);
}

// --------------------------------------------------------------------------

void MCS_A::ProcessResponse(
	AbstractUnit* pUnit, 			//! Hardware depended unit object.
	const MessageAerospace& msg	//! The response message
)
{
	// forward the received response to the unit.
	pUnit->ProcessModuleConfigResponse(msg);
}

#endif

// ##########################################################################

#if defined(USE_CAN_MCS_B)

MCS_B::MCS_B()
{
}

// --------------------------------------------------------------------------

void MCS_B::ProcessRequest(
	AbstractUnit* pUnit, 			//! Hardware depended unit object.
	const MessageAerospace& msg	//! Message to be processed.
)
{
//	PRINTF("MCS_B::ProcessRequest\n");
	// Prepare the message ...
	MessageAerospace msgOut(
		Identifier(lPrimary, idNS_C00_Response, pUnit->GetNodeId(), msg.GetSender()),
		dtLong, sModuleConfiguration, msg.GetMessageCode(), 
		pUnit->ConfigureModule(msg)
	);
	
	// and post it.
	pUnit->PostMessage(msgOut);
}

// --------------------------------------------------------------------------
#endif

} // can namespace
