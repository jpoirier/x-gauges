#ifndef CAN_MESSAGE_AEROSPACE_HELPER_H
#define CAN_MESSAGE_AEROSPACE_HELPER_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2007 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *      Rok Markovic [rok.markovic@kanardia.eu]                            *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/


#include "MessageAerospace.h"

namespace can {

const char* GetDataTypeText(DataType eDataType);
const char* GetHardwareName(uint8_t byId);

// Vendor stuff

//! Number of known vendors (harcoded in the program)
int 			GetVendorCount();
//! The hash number of the vendor i.
uint32_t 	GetVendorHash(int i);
//! The name of the vendor i.
const char* GetVendorName(int i);
//! Get vendor index for known vendor hash. Returns -1 if not found.
int 			FindVendorIndex(uint32_t uiHash);

} // can namespace

#endif
