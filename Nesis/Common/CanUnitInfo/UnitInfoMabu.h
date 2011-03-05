#ifndef UNIT_INFO_MABU_H
#define UNIT_INFO_MABU_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2008 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   Licence: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include "UnitInfoBase.h"

namespace can {

// ------------------------------------------------------------------------

class UnitInfoMabu : public UnitInfoBase
{
public:
	//! Constructor
	UnitInfoMabu(uint8_t byNodeId=0, uint32_t uiSerial=0, common::JulianDay jdDate=common::JulianDay());
	//! Copy constructor
	UnitInfoMabu(const UnitInfoBase& C);
	//! Destructor
   virtual ~UnitInfoMabu();

	//! Process MIS response
	virtual void ProcessModuleInfoResponse(const can::MessageAerospace& msg);
	
	//! Return true if the Mabu is the secondary Mabu.
	bool IsSecondary() const
	{ return m_bSecondary; }
	//! Return the reference RPM value.
	unsigned int GetReferenceRPM() const
	{ return m_uiRefRPM; }
	//! Return declared cell capacity.
	unsigned int GetDeclaredCellCapacity() const
	{ return m_uiCellCap; }
	//! Return charger voltage factor.
	float GetChargerVoltageFactor() const
	{ return m_fChargerVoltageFactor; }
	
protected:
	//! Setup download buffer.
	virtual bool LoadDownloadBuffer(uint32_t /*uiDwnDataId*/);

protected:
	//! The secondary flag.
	bool m_bSecondary;
	//! Reference engine RPM
	unsigned int m_uiRefRPM;
	//! Declared battery cell capacity.
	unsigned int m_uiCellCap;
	//! Charger voltage factor.
	float m_fChargerVoltageFactor;
};

// --------------------------------------------------------------------------

} // namespace

#endif
