/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2007 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include <QtDebug>
#include <QFileInfo>
#include "SphereNavigation.h"
#include "Logbook.h"

#include "Entry.h"

namespace logbook {

// --------------------------------------------------------------------------

Entry::Entry()
{
	Clear();
}

Entry::Entry(const Entry& C)
{
	m_qsPilot		= C.m_qsPilot;
	m_qsRecord		= C.m_qsRecord;
	m_conFlight		= C.m_conFlight;
	m_conEngine		= C.m_conEngine;

	m_iDistance 	= C.m_iDistance;
	m_iMaxAltitude = C.m_iMaxAltitude;
	m_iMinAltitude = C.m_iMinAltitude;
	m_fMaxSpeed 	= C.m_fMaxSpeed;
	m_fAvgSpeed 	= C.m_fAvgSpeed;
	m_fFuel 			= C.m_fFuel;
	m_fSpecFuel 	= C.m_fSpecFuel;
}

Entry::~Entry()
{
}
// -----------------------------------------------------------------------

void Entry::Clear()
{
	m_qsPilot.clear();
	m_qsRecord.clear();
	m_conFlight.clear();
	m_conEngine.clear();

	m_iDistance 	= 0;
	m_iMaxAltitude = 0;
	m_iMinAltitude = 0;
	m_fMaxSpeed 	= 0;
	m_fAvgSpeed 	= 0;
	m_fFuel 			= 0;
	m_fSpecFuel 	= 0;
}

// -----------------------------------------------------------------------

bool Entry::IsValid() const
{
	bool bValid = 	m_qsPilot.isEmpty()==false &&
			 			m_qsRecord.isEmpty()==false &&
			 			GetFlightTimeSecs() > 0 &&
			 			GetEngineTimeSecs() > 0;

	// Report only invalid entries.
	if(!bValid)
		qWarning() << "Entry::IsValid: "
					<< m_qsPilot << m_qsRecord << GetFlightTimeSecs() << GetEngineTimeSecs();

	return bValid;
}

// -----------------------------------------------------------------------

void Entry::AddFlight(const OnOff& ofFlight)
{
	m_conFlight.push_back(ofFlight);
}
// -----------------------------------------------------------------------

void Entry::AddEngine(const OnOff& ofEngine)
{
	m_conEngine.push_back(ofEngine);
}

// -----------------------------------------------------------------------
/*! For all flights in this log entry sum the seconds of each flight.
	 \retval seconds of the total flying time.
 */
int Entry::GetFlightTimeSecs() const
{
	int iSecs = 0;
	for(int i=0; i<m_conFlight.count(); i++) {
		iSecs += m_conFlight[i].first.secsTo(m_conFlight[i].second);
	}
	return iSecs;
}

// -----------------------------------------------------------------------
/*! For all cases when an engine was running sum the seconds of each run.
	 \retval seconds of the total engine time.
 */
int Entry::GetEngineTimeSecs() const
{
	int iSecs = 0;
	for(int i=0; i<m_conEngine.count(); i++) {
		iSecs += m_conEngine[i].first.secsTo(m_conEngine[i].second);
	}
	return iSecs;
}
// -----------------------------------------------------------------------

void Entry::Write(xml::XmlWriter& xw) const
{
	// Don't write invalid entries.
	if(IsValid() == false) {
		qWarning() << "Entry::Write: Invalid logbook entry. Skipped.";
		return;
	}

	xml::AttrMap attr;

	// The entry is valid.

	xw.WriteOpenTag(TAG_ENTRY);

	// pilot
	xw.WriteTaggedString(TAG_PILOT, m_qsPilot);
	// engine
	for(int i=0; i<m_conEngine.count(); i++) {
		attr.clear();
		attr.insert(TAG_ON,  m_conEngine[i].first.toString(Qt::ISODate));
		attr.insert(TAG_OFF, m_conEngine[i].second.toString(Qt::ISODate));
		xw.WriteAtomTag(TAG_ENGINE, attr);
	}
	// flights
	for(int i=0; i<m_conFlight.count(); i++) {
		attr.clear();
		attr.insert(TAG_TAKEOFF, m_conFlight[i].first.toString(Qt::ISODate));
		attr.insert(TAG_LANDING, m_conFlight[i].second.toString(Qt::ISODate));
		xw.WriteAtomTag(TAG_FLIGHT, attr);
	}
	// Fuel used
	attr.clear();
	attr.insert(TAG_TOTAL, QString::number(GetFuelConsumption()));
	attr.insert(TAG_SPECIFIC, QString::number(GetSpecificFuelConsumption()));
	xw.WriteAtomTag(TAG_FUEL, attr);
	// speed
	attr.clear();
	attr.insert(TAG_AVG, QString::number(GetAverageSpeed()));
	attr.insert(TAG_MAX, QString::number(GetMaximalSpeed()));
	xw.WriteAtomTag(TAG_SPEED, attr);
	// altitude
	attr.clear();
	attr.insert(TAG_MIN, QString::number(GetMinimalAltitude()));
	attr.insert(TAG_MAX, QString::number(GetMaximalAltitude()));
	xw.WriteAtomTag(TAG_ALTITUDE, attr);
	// distance
	xw.WriteTaggedString(TAG_DISTANCE, QString::number(GetDistance()));
	// record
	// file name must be without absolute path - just name [rok]
	xw.WriteTaggedString(TAG_RECORD, QFileInfo(m_qsRecord).fileName());

	xw.WriteCloseTag(TAG_ENTRY);
}

} // namespace logbook
