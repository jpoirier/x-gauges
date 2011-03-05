#ifndef LOGBOOK_H
#define LOGBOOK_H

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
#include "Entry.h"
#include "Record.h"

#define TAG_LOGBOOK				"logbook"
#define TAG_VERSION				"version"
#define TAG_ENTRY					"entry"
#define TAG_PILOT					"pilot"
#define TAG_ENGINE				"engine"
#define TAG_FLIGHT				"flight"
#define TAG_RECORD				"record"
#define TAG_FUEL					"fuel"
#define TAG_SPEED					"speed"
#define TAG_ALTITUDE				"altitude"
#define TAG_DISTANCE				"distance"

#define TAG_ON						"on"
#define TAG_OFF					"off"
#define TAG_TAKEOFF				"takeoff"
#define TAG_LANDING				"landing"
#define TAG_MIN					"min"
#define TAG_MAX					"max"
#define TAG_AVG					"avg"
#define TAG_TOTAL					"total"
#define TAG_SPECIFIC				"specific"

namespace logbook {

// -------------------------------------------------------------------------
/** Shared (singletone) logbook object.

	 It is very important, that the system time is not changed when the 
	 logging is performed. This may result in an invalid log.
 */

class Logbook : public QXmlDefaultHandler
{
public:
	//! Container of logbook entries.
	typedef QVector<Entry> Container;
	//! Constructor is private in order to prevent direct class creation.
	Logbook();
public:
	//! Destructor.
	~Logbook();

	//! Access point to the class.
	static Logbook* GetInstance()
	{
		if(s_pInstance == NULL)
			s_pInstance = new Logbook();
		return s_pInstance;
	}
	
	//! Delete all blog records and xml file. Posible only if not recording.
	bool DeleteAll();
	
	//! Update the information (updates current record and entry.)
	void Update();
	
	//! Return true, if the system is recording.
	bool IsRecording() const
	{ return m_record.IsRecording(); }

	//! Get a copy of the logbook container.
	Container GetReverseContainer(int iMaxCount, bool bCurrent) const;
	
	//! Load the XML logbook file.
	int Load(const QString& qsXMLLogFile);

private:
	//! Clear internal variables and set them into initial state.
	void Clear();
	//! Start the recording process.
	void StartRecording();
	//! Stop recording process.
	void StopRecording();
	//! Write current content to the XML file.
	void WriteXML();
	
	//! Update current entry from given file.
	QDateTime UpdateEntryFromFile(QFile& file, bool bContinue);
	//! Update current entry with data.
	void UpdateEntry(const Record::Data& d, const QDateTime& dtStart);
	//! Terminate current recording and entry.
	void TerminateRecord(QFile* pFile);

	//! XML simple parser - starting element
	bool startElement(const QString& qsNamespace,
							const QString& qsLocalName,
							const QString& qsName,
							const QXmlAttributes& qaAttributes);
	//! XML simple parser - closing element.
	bool endElement(	const QString& qsNamespace,
							const QString& qsLocalName,
							const QString& qsName);

	
	//! XML simple parser - characters between start and end.
	bool characters(const QString& qs);
	//! XML simple parser - the error handler.
	bool fatalError(const QXmlParseException& exception);


	//! Add distance from last point to this point (lon, lat) in [rad].
	void AddDistance(const QPointF& ptNew);
	//! Update altitude limits [m].
	void UpdateAltitudeLimits(int iAlt);
	//! Update speed limits [m/s].
	void UpdateSpeedLimits(float fSpeed);
	//! Add (integrate) fuel flow [l/s] to get fuel consumption.
	void AddFuel(float fFuelFlow);
	
	//! Reset last coordinate.
	void ResetLastCoordinate()
	{ m_ptLastCoor = QPointF(); }
	
	//! Get minimal recorded altitude [m].
	int GetMinAltitude() const
	{ return m_iMinAlt; }
	//! Get maximal recorded altitude [m].
	int GetMaxAltitude() const
	{ return m_iMaxAlt; }
	//! Get maximal airspeed [m/s].
	float GetMaxSpeed() const
	{ return m_fMaxSpeed; }
	//! Get distance traveled [m](this is not point to point distance).
	float GetDistance() const
	{ return m_fDistance; }
	//! Get average speed [m/s] (distance/airborn time).
	float GetAverageSpeed(float fExtraSec=0.0f) const;
	//! Get fuel used in this flight [l].
	float GetFuelConsumption(float fExtraSec=0.0f) const;
	//! Get specific fuel consumption [l/h].
	float GetSpecificFuelConsumption(float fExtraSec=0.0f) const;


private:
	//! Instance of the logbook object.
	static Logbook* s_pInstance;

	//! Remember the file name.
	QString m_qsFileName;

	//! Store all the logbook entries.
	Container m_conBook;

	//! Working entry variable - works in sync with the record.
	Entry m_entry;
	//! Recording object - records to the file.
	Record m_record;

//	//! State of the reader.
// 	XmlReaderState m_eState;
	//! Current content of xml tag.
	QString m_qsData;

	// helper variables used to maintain the state of the active entry.
	OnOff m_ofFlight; 	   //!< Current flight status
	int m_iFlightCounter; 	//!< A counter used to detect true flight conditions.
	int m_iFlightTime; 		//!< Elapsed time at the detection time even.

	OnOff m_ofEngine;			//!< Current engine status
	int m_iEngineCounter;	//!< A counter used to detect true on/off conditions.
	int m_iEngineTime;		//!< Elapsed time at the detection time even.
	
	// Some statistical data variables
	
	//! minimal altitude [m].
	int m_iMinAlt;
	//! maximal altitude [m].
	int m_iMaxAlt;
	
	//! maximal speed [m/s]
	float m_fMaxSpeed;
	
	//! distance traveled [m]
	float m_fDistance;
	
	//! fuel used [l] (Just a sum of fuel flow values.)
	float m_fFuel;
	//! Number of fuel add events. Used to calculate dT.
	int m_iFuelCount;
	
	//! Last coordinate (long, lat) in [rad].
	QPointF m_ptLastCoor;
};

} // namespace
// -------------------------------------------------------------------------
#endif
