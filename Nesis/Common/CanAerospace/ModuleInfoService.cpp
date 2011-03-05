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
#include "ModuleInfoService.h"

namespace can {

// ##########################################################################

#if defined(USE_CAN_MIS_A)

MIS_A::MIS_A()
{
}

// --------------------------------------------------------------------------

/** This node (A) transmits an information request to the node B.

	 Serial number is optional. When given (non-zero) only node which
    serial number matches given argument will respond.

	 \retval true Service request was submited successfully.
	 \retval false Service failed to start. The buffer is full.
 */

bool MIS_A::Start(
	AbstractUnit* pUnit,	//! Hardware depended unit object.
	uint8_t byNodeB, 		//! Node B - the addressing node.
	uint8_t byQuestion,	//! The MIS question.
	uint32_t uiInfo		//! Information - optional (could be serial number).
)
{
	// Send request download to the addressing node.
	MessageAerospace msg(
		Identifier(lPrimary,	idNS_C00_Request, pUnit->GetNodeId(), byNodeB),
		dtULong,					// serial is ULong
		sModuleInformation,	// module information service
		byQuestion,				// safety code must be correct.
		uiInfo					// optional info  or serial (0) means no serial
	);

	// Try to post the message.
	return pUnit->PostMessage(msg);
}

// --------------------------------------------------------------------------

void MIS_A::ProcessResponse(
	AbstractUnit* pUnit, 			//! Hardware depended unit object.
	const MessageAerospace& msg	//! The response message
)
{
	// forward the received response to the unit.
	pUnit->ProcessModuleInfoResponse(msg);
}

#endif

// ##########################################################################

#if defined(USE_CAN_MIS_B)

MIS_B::MIS_B()
{
}

// --------------------------------------------------------------------------

void MIS_B::ProcessRequest(
	AbstractUnit* pUnit, 			//! Hardware depended unit object.
	const MessageAerospace& msg	//! Message to be processed.
)
{
	uint8_t byQuestion = msg.GetMessageCode();

	DataType eDataType;
	Register rData;

	if(pUnit->GetModuleInfoData(byQuestion, msg.GetData().ui32, eDataType, rData)) {
		// Prepare the message ...
		MessageAerospace msgOut(
			Identifier(lPrimary, idNS_C00_Response, pUnit->GetNodeId(), msg.GetSender()),
			eDataType, sModuleInformation, byQuestion,
			rData
		);

		// and post it.
		pUnit->PostMessage(msgOut);
	}
	else {
		PRINTF("MIS B:Information not available, Q=%i.\n", byQuestion);
	}
}

// --------------------------------------------------------------------------
#endif

} // can namespace
