#ifndef CAN_MAG_SPHERE_CALIBRATION_H
#define CAN_MAG_SPHERE_CALIBRATION_H

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

#include <QVector>
#include "Matrix/Vector.h"

namespace can {

// ------------------------------------------------------------------------

class MagSphereCalibration
{
public:
	//! Record is a set of measuremnts of one "circle".
	typedef QVector<V3f> Record;
	//! We need several Records for the calibration.
	typedef QVector<Record> ConRecord;
	
public:
	//! Constructor
	MagSphereCalibration();
	
	//! Reset the container of records.
	void Reset() 
	{ m_conRecord.clear(); }
	//! Get const access to the container.
	const ConRecord& GetContainer() const
	{ return m_conRecord; }
	//! Get number of records.
	int GetRecordCount() const
	{ return m_conRecord.count(); }	
	//! Undo - remove last record from container.
	void UndoRecord();
	
	//! Create new record into the container. Return the record index.
	int CreateRecord();
	//! Get const access to requested record.
	const Record& GetRecord(int iRec) const
	{ return m_conRecord[iRec]; }
	//! Get number of items in given record.
	int GetRecordItemCount(int iRec) const
	{ return m_conRecord[iRec].count(); }
	
	//! Add item into the record iRec. Returns true if the record was added.
	bool AddItem(int iRec, const V3f& v);
	//! Add item into the last record. The last record is an active one.
	bool AddItem(const V3f& v);
	//! Verify item
	void VerifyItem(const V3f& v, float fRoll=0.0f, float fPitch=0.0f);
	
	//! Solve - search for the magnetic sphere coefficients.
	bool Solve(float fLon, float fLat, float fAlt_km);
	
protected:
	//! All records are here.
	ConRecord m_conRecord;
	// Results 
	//! Bias vector - offset from origin in sensor coordinates.
	V3f m_vB;
	//! Scale vector - scale factor for each axis.
	V3f m_vS;
};

// --------------------------------------------------------------------------

} // namespace

#endif
