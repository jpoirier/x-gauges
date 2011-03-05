#ifndef LOGBOOK_ENTRY_H
#define LOGBOOK_ENTRY_H

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

#include <QXmlDefaultHandler>
#include <QFile>
#include <QDateTime>
#include <QPoint>

#include "XmlWriter.h"

namespace logbook {

//! Start, Finish - On, Off
typedef std::pair<QDateTime, QDateTime> OnOff;
//! A vector container of OnOff objects.
typedef QVector<OnOff> ContainerOnOff;

// -------------------------------------------------------------------------
/** Describe one entry in the logbook.

	 Just basic facts are stored while details should be revealed from
    a seperate binary file record, if the record is available. The
    class takes care for the special cases when a takeoff and a landing
    happen on a different day. One flight may have several takeoffs and
    landings (e.g. when flying traffic patterns). We may also have
    several engine on/offs in one flight (e.g. motorgliders).
 */

class Entry
{
public:
	//! Constructor
	Entry();
	//! Copy constructor.
	Entry(const Entry& C);
	//! Destructor.
	~Entry();
	
	//! Clear (reset) the content
	void Clear();

	//! Write the entry into the xml file.
	void Write(xml::XmlWriter& xw) const;

	//!Return true if the entry is a valid one. 
	bool IsValid() const;
	
	//! Set the pilot name.
	void SetPilot(const QString& qsPilot)
	{ m_qsPilot = qsPilot; }
	//! Get the pilot name.
	const QString& GetPilot() const
	{ return m_qsPilot; }

	//! Set the record file.
	void SetRecordFileName(const QString& qsRecord)
	{ m_qsRecord = qsRecord; }
	//! Get the record file name.
/*	const QString& GetRecordFileName() const
	{ return m_qsRecord; }*/
	
	//! Get the date. Date is the date of the takeoff.
	QDate GetDate() const
	{	return m_conFlight.isEmpty() ? QDate() : m_conFlight.front().first.date(); }
	
	//! Add one takeoff - landing pair = flight.
	void AddFlight(const OnOff& ofFlight);
	//! Get first take off.
	QDateTime GetFirstTakeoff() const
	{ return m_conFlight.isEmpty() ? QDateTime() : m_conFlight.front().first; }
	//! Get last landing.
	QDateTime GetLastLanding() const
	{ return m_conFlight.isEmpty() ? QDateTime() : m_conFlight.back().second; }
	//! Get const access to the flying time container object.
	const ContainerOnOff& GetFlights() const
	{ return m_conFlight; }
	//! Calculate flying time (sum of all flights in seconds.)
	int GetFlightTimeSecs() const;
	//! Get number of flights cycles.
	int GetFlightCount() const
	{ return m_conFlight.size(); }

	//! Add one engine on off pair.
	void AddEngine(const OnOff& ofEngine);
	//! Get first engine on.
	QDateTime GetFirstEngineOn() const
	{ return m_conEngine.isEmpty() ? QDateTime() : m_conEngine.front().first; }
	//! Get last engine off.
	QDateTime GetLastEngineOff() const
	{ return m_conEngine.isEmpty() ? QDateTime() : m_conEngine.back().second; }
	//! Get const access to the engine time container object.
	const ContainerOnOff& GetEngine() const
	{ return m_conEngine; }
	//! Calculate engine time (sum of all engine runs in seconds.)
	int GetEngineTimeSecs() const;
	//! Get number of engine cycles.
	int GetEngineCycles() const
	{ return m_conEngine.size(); }
	
	//! Set distance [m].
	void SetDistance(int iD)
	{ m_iDistance = iD; }
	//! Get distance [m].
	int GetDistance() const
	{ return m_iDistance; }
	
	//! Set minimal altitude [m].
	void SetMinimalAltitude(int iAlt)
	{ m_iMinAltitude = iAlt; }
	//! Get minimal altitude [m].
	int GetMinimalAltitude() const 
	{ return m_iMinAltitude; }
	
	//! Set maximal altitude [m].
	void SetMaximalAltitude(int iAlt)
	{ m_iMaxAltitude = iAlt; }
	//! Get maximal altitude [m].
	int GetMaximalAltitude() const 
	{ return m_iMaxAltitude; }
	
	//! Set maximal speed [m/s].
	void SetMaximalSpeed(float fSpeed)
	{ m_fMaxSpeed = fSpeed; }
	//! Get maximal speed [m/s].
	float GetMaximalSpeed() const
	{ return m_fMaxSpeed; }

	//! SetAverage speed [m/s].
	void SetAverageSpeed(float fSpeed)
	{ m_fAvgSpeed = fSpeed; }
	//! Get average speed [m/s].
	float GetAverageSpeed() const
	{ return m_fAvgSpeed; }
	
	//! Set fuel consumption [l].
	void SetFuelConsumption(float fFuel)
	{ m_fFuel = fFuel; }
	//! Get fuel consumption [l].
	float GetFuelConsumption() const
	{ return m_fFuel; }

	//! Set specific fuel consumption [l/h].
	void SetSpecificFuelConsumption(float fFuel_hour)
	{ m_fSpecFuel = fFuel_hour; }
	//! Get fuel consumption [l].
	float GetSpecificFuelConsumption() const
	{ return m_fSpecFuel; }
	
private:
	//! Pilot
	QString m_qsPilot;
	//! Name of the log record file.
	QString m_qsRecord;

	//! Flying time - takeoffs and landings.
	ContainerOnOff	m_conFlight;
	//! Engine time - engine on-offs.
	ContainerOnOff m_conEngine;
	
	//! Maximal speed [m/s] during the flight,
	float m_fMaxSpeed;
	//! Average ground speed [m/s]. This is not point to point speed.
	float m_fAvgSpeed;
	//! Fuel consumption [l] during the flight.
	float m_fFuel;
	//! Specific fuel consumption [l/h] during the flight.
	float m_fSpecFuel;
	//! Distance traveled [m] - this is not point to point distance.
	int m_iDistance;
	//! Minimal altitude reached [m].
	int m_iMinAltitude;
	//! Maximal altitude reached [m].
	int m_iMaxAltitude;
};

// -------------------------------------------------------------------------
} // namespace
#endif
