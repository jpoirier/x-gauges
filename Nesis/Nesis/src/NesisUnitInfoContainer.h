#ifndef NESIS_UNIT_INFO_CONTAINER_H
#define NESIS_UNIT_INFO_CONTAINER_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2007 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   Licence: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include "CanUnitInfo/UnitInfoAiru.h"
#include "CanUnitInfo/UnitInfoMagu.h"
#include "CanUnitInfo/UnitInfoMabu.h"
#include "CanUnitInfo/UnitInfoDaqu.h"

#include "CanUnitInfo/UnitInfoBaseContainer.h"

// --------------------------------------------------------------------------

class NesisUnitInfoContainer : public can::UnitInfoBaseContainer
{
public:
	//! Default Constructor
	NesisUnitInfoContainer();
	
	//! Get the Airu info object.
	can::UnitInfoAiru* GetAiru();
	//! Get the Magu info object.
	can::UnitInfoMagu* GetMagu();
	//! Get the Daqu info object.
	can::UnitInfoDaqu* GetDaqu();
	//! Get the Mabu info object (this will be secondary mabu on secondary unit).
	can::UnitInfoMabu* GetMabu();
	
	//! After the base class scan set also direct access to some info objects.
	void Scan(bool bSetHighId=false);
protected:
	//! Factory - recreate a unit based on the hardware id.
	can::UnitInfoBase* RecreateUnit(can::UnitInfoBase* pOld, uint8_t byHardwareId);
};

// --------------------------------------------------------------------------

#endif
