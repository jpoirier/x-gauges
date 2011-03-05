#ifndef UNIT_INFO_DAQU_H
#define UNIT_INFO_DAQU_H

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

class UnitInfoDaqu : public UnitInfoBase
{
public:
	//! Constructor
	UnitInfoDaqu(uint8_t byNodeId=0, uint32_t uiSerial=0, common::JulianDay jdDate=common::JulianDay());
	//! Copy constructor
	UnitInfoDaqu(const UnitInfoBase& C);
	//! Destructor
   virtual ~UnitInfoDaqu();

   //! Return the fuel flow factor value.
   float GetFuelFlowFactor() const
   { return m_fFlowFactor; }
   //! Return current fuel flow K value.
   unsigned int GetFuelFlowK() const
   { return m_uiFlowK; }
   //! Return current RPM divisor.
   unsigned int GetRpmDivisor() const
   { return m_uiRpmDivisor; }

	//! Process MIS response
	void ProcessModuleInfoResponse(const can::MessageAerospace& msg);

protected:
	//! Setup download buffer.
	bool LoadDownloadBuffer(uint32_t /*uiDwnDataId*/);
	//! Update the sign of life status. We may also request some info.
	void UpdateSignOfLife(AbstractAerospaceDataManager *pADM);
	//! Is it a valid object?
	bool IsValid() const;

protected:
	//! Current flow factor correction used.
	float m_fFlowFactor;
	//! Current fuel flow K value.
	unsigned int m_uiFlowK;
	//! Current RPM divisor
	unsigned int m_uiRpmDivisor;
};

// --------------------------------------------------------------------------

} // namespace

#endif
