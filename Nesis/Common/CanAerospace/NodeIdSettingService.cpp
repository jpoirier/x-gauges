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
#include "NodeIdSettingService.h"

namespace can {

// ##########################################################################

#if defined(USE_CAN_NIS_A)

NIS_A::NIS_A()
{
}

// --------------------------------------------------------------------------

/** This node (A) transmits a node id change request to the node B.

	 \retval true Service request was submited successfully.
	 \retval false Service failed to start. The buffer is full.
 */

bool NIS_A::Start(
	AbstractUnit* pUnit,	//! Hardware depended unit object.
	uint8_t byNodeB, 		//! Node B - the node which should change id.
	uint8_t byNewNodeId, //! New node id must >= 128.
	uint32_t uiSerial		//! Serial number.
)
{
	// Send request download to the addressing node.
	MessageAerospace msg(
		Identifier(lPrimary, idNS_C00_Request, pUnit->GetNodeId(), byNodeB),
		dtULong,					// we are serial number in the data.
		sNodeIdSetting,		// module information service
		byNewNodeId,			// new node id >= 128
		uiSerial					// node serial number
	);

	// Try to post the message.
	return pUnit->PostMessage(msg);
}

// --------------------------------------------------------------------------

void NIS_A::ProcessResponse(
	AbstractUnit* pUnit, 			//! Hardware depended unit object.
	const MessageAerospace& msg	//! The response message
)
{
	PRINTF("NIS A: Responce is %i.\n", (int)msg.GetMessageCode());
	// forward the received response to the unit.
	pUnit->ProcessNodeIdSettingResponse(msg);
}

#endif

// ##########################################################################

#if defined(USE_CAN_NIS_B)

NIS_B::NIS_B()
{
}

// --------------------------------------------------------------------------

void NIS_B::ProcessRequest(
	AbstractUnit* pUnit, 			//! Hardware depended unit object.
	const MessageAerospace& msg	//! Message to be processed.
)
{
	ASSERT(msg.GetDataType()==dtULong);

	uint8_t byNewId   = msg.GetMessageCode();
	uint32_t uiSerial = msg.GetData().ui32;

	PRINTF("NIS B: new id = %i\n", (int)byNewId);

	char chResponse = (char)-6; // failed
	if(pUnit->SetNodeId(byNewId, uiSerial)) {
		chResponse = 0;
	}
	// Prepare the message ...
	MessageAerospace msgOut(
		Identifier(lPrimary, idNS_C00_Response, pUnit->GetNodeId(), msg.GetSender()),
		dtULong, sNodeIdSetting, (uint8_t)chResponse,
		uiSerial
	);

	// and post it.
	pUnit->PostMessage(msgOut);
}

// --------------------------------------------------------------------------
#endif

} // can namespace
