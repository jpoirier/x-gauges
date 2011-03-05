#ifndef __CAN_NODE_ID_SETTING_SERVICE_H
#define __CAN_NODE_ID_SETTING_SERVICE_H

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

//-----------------------------------------------------------------------------

#if defined(USE_CAN_NIS_A)

/** \brief Node Id Setting Service helper class for the node A.
	
	The class provides procedures needed to handle the 
	Node Id Setting Service (NIS) as it is defined by the 
	CANairspace protocol. We added a change to this service. Namely
	the data type must be ULong and we pass serial number in the
	data field. New node id must >= 128.
	
	This class implements methods for node A.

	\sa NIS_B
*/

class NIS_A {
public:
	//! Constructor - set initial state.
	NIS_A();

	//! Start the service.
	bool Start(AbstractUnit* pUnit, uint8_t byNodeB, uint8_t byNewId, uint32_t uiSerial);
	//! Process response messages from node B.
	void ProcessResponse(AbstractUnit* pUnit, const MessageAerospace& msg);
}; 
#endif

//-----------------------------------------------------------------------------

#if defined(USE_CAN_NIS_B)
/** \brief Node Id Setting Service helper class for the node B.
	
	The class provides procedures needed to handle the 
	Node Id Setting Service (NIS) as it is defined by the 
	CANairspace protocol. We added a change to this service. Namely
	the data type must be ULong and we pass serial number in the
	data field. New node id must >= 128.
	
	This class implements methods for node B.
	
	\sa NIS_A
*/


class NIS_B {
public:
	//! Constructor - set initial state.
	NIS_B();

	//! Process request messages from the node A.
	void ProcessRequest(AbstractUnit* pUnit, const MessageAerospace& msg);
};
#endif

//-----------------------------------------------------------------------------
} // can namespace

#endif
