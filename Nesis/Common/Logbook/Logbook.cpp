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
#include <QFile>
#include <QDir>

#include "CommonDefs.h"
#include "Settings.h"
#include "SphereNavigation.h"
#include "NesisAerospaceDataManager.h"

#include "Logbook.h"


namespace logbook {

Logbook* Logbook::s_pInstance = NULL;

// --------------------------------------------------------------------------

Logbook::Logbook()
{
	m_iEngineCounter = m_iEngineTime = 0;
	m_iFlightCounter = m_iFlightTime = 0;
	Clear();
}

Logbook::~Logbook()
{
	StopRecording();
}
// -----------------------------------------------------------------------

void Logbook::Clear()
{
	m_iMinAlt		= 10000;
	m_iMaxAlt		= 0;
	m_fMaxSpeed    = 0.0f;
	m_fDistance		= 0.0f;
	m_fFuel			= 0.0f;
	m_iFuelCount   = 0;

	m_ptLastCoor = QPointF();

	m_ofFlight.first = m_ofFlight.second = QDateTime();
	m_iFlightCounter = m_iFlightTime = 0;
	m_ofEngine.first = m_ofEngine.second = QDateTime();
	m_iEngineCounter = m_iEngineTime = 0;
}

// -----------------------------------------------------------------------

int Logbook::Load(const QString& qsXMLLogFile)
{
	m_qsFileName = qsXMLLogFile;
	// Load data from the file.
	QFile file(qsXMLLogFile);
	QXmlInputSource inputSource(&file);

	QXmlSimpleReader reader;
	reader.setContentHandler(this);
	reader.setErrorHandler(this);

	// Set reader to the unknown state before parsing.
	reader.parse(inputSource);

	// Return number of entries in the logbook.
	return m_conBook.count();
}

// -----------------------------------------------------------------------

bool Logbook::startElement(
	const QString& /*qsNamespace*/,
	const QString& /*qsLocalName*/,
	const QString& qsName,
	const QXmlAttributes& qaAttr
)
{
// 	qDebug() << "Start:" << qsName;
	m_qsData.clear();

	if(qsName == TAG_ENTRY) {
		m_entry.Clear();
	}

	// Process the <engine> element. It needs two attributes "on" and "off".
	// Make some checks first.
	else if(qsName == TAG_ENGINE) {
		if(qaAttr.count()!=2) {
			qWarning() << "Logbook.xml error: Invalid number of attributes in the <engine> element.";
		}
		else {
			if(qaAttr.index(TAG_ON) == -1 || qaAttr.index(TAG_OFF) == -1) {
				qWarning() << "Logbook.xml error: 'on' and 'off' attributes are expected in the <engine> element.";
			}
			// Ok, get the on and off times
			OnOff ofEngine;
			ofEngine.first  = QDateTime::fromString(qaAttr.value(TAG_ON),  Qt::ISODate);
			ofEngine.second = QDateTime::fromString(qaAttr.value(TAG_OFF), Qt::ISODate);
			if(ofEngine.first.isValid()==false || ofEngine.second.isValid()==false) {
				qWarning() << "Logbook.xml error: invalid date_time format the <engine> element.";
			}
			m_entry.AddEngine(ofEngine);
		}
	}

	// Process the <flying> element. It needs two attributes "takeoff" and "landing".
	// Make some checks first.
	else if(qsName == TAG_FLIGHT) {
		if(qaAttr.count()!=2) {
			qWarning() << "Logbook.xml error: Invalid number of attributes in the <flight> element.";
		}
		else {
			if(qaAttr.index(TAG_TAKEOFF) == -1 || qaAttr.index(TAG_LANDING) == -1) {
				qWarning() << "Logbook.xml error: 'takeoff' and 'landing' attributes are expected in the <flight> element.";
			}
			// Ok, get the takeoff and landing times
			OnOff ofFlight;
			ofFlight.first  = QDateTime::fromString(qaAttr.value(TAG_TAKEOFF), Qt::ISODate);
			ofFlight.second = QDateTime::fromString(qaAttr.value(TAG_LANDING), Qt::ISODate);
			if(ofFlight.first.isValid()==false || ofFlight.second.isValid()==false) {
				qWarning() << "Logbook.xml error: invalid date_time format the <flight> element.";
			}
			m_entry.AddFlight(ofFlight);
		}
	}

	// Process ALTITUDE element. It has two attributes.
	else if(qsName == TAG_ALTITUDE) {
		if(qaAttr.index(TAG_MIN) == -1 || qaAttr.index(TAG_MAX) == -1) {
			qWarning() << "Logbook.xml error: 'min' and 'max' attributes are expected in the <altitude> element.";
		}
		m_entry.SetMinimalAltitude(qaAttr.value(TAG_MIN).toInt());
		m_entry.SetMaximalAltitude(qaAttr.value(TAG_MAX).toInt());
	}

	// Process SPEED element. It has two attributes.
	else if(qsName == TAG_SPEED) {
		if(qaAttr.index(TAG_MAX) == -1 || qaAttr.index(TAG_AVG) == -1) {
			qWarning() << "Logbook.xml error: 'max' and 'avg' attributes are expected in the <speed> element.";
		}
		m_entry.SetAverageSpeed(qaAttr.value(TAG_AVG).toFloat());
		m_entry.SetMaximalSpeed(qaAttr.value(TAG_MAX).toFloat());
	}

	// Process FUEL element. It has two attributes.
	else if(qsName == TAG_FUEL) {
		if(qaAttr.index(TAG_TOTAL) == -1 || qaAttr.index(TAG_SPECIFIC) == -1) {
			qWarning() << "Logbook.xml error: 'total' and 'specific' attributes are expected in the <fuel> element.";
		}
		m_entry.SetFuelConsumption(qaAttr.value(TAG_TOTAL).toFloat());
		m_entry.SetSpecificFuelConsumption(qaAttr.value(TAG_SPECIFIC).toFloat());
	}

	return true;
}

// -----------------------------------------------------------------------

bool Logbook::endElement(
	const QString& /*qsNamespace*/,
	const QString& /*qsLocalName*/,
	const QString& qsName
)
{
	// Was pilot?
	if(qsName == TAG_PILOT)
		m_entry.SetPilot(m_qsData);
	// was record?
	else if(qsName == TAG_RECORD) {
		// Take just the filename.
		QFileInfo file(m_qsData);
		m_entry.SetRecordFileName(file.fileName());
	}
	// was distance?
	else if(qsName == TAG_DISTANCE)
		m_entry.SetDistance(m_qsData.toInt());
	// was entry?
	else if(qsName == TAG_ENTRY) {
		// Check if the entry is valid.
		if(m_entry.IsValid()) {
			// Check the sort order.
			// Items in the file are sorted according to the date
			// old at front, new at back.
			if(m_conBook.isEmpty()==false) {
				if(m_entry.GetFirstTakeoff() < m_conBook.back().GetLastLanding()) {
					qWarning() << "Logbook.xml error: Entries are not sorted properly.";
					//return true;
				}
			}
			m_conBook.push_back(m_entry);
		}
		else {
			qWarning() << "Logbook.xml error: Invalid entry.";
			return true;
		}
	}

	return true;
}

// -----------------------------------------------------------------------

bool Logbook::characters(const QString& qs)
{
	m_qsData += qs;
	return true;
}

// -----------------------------------------------------------------------

bool Logbook::fatalError(const QXmlParseException& ex)
{
	qDebug() << "Logbook.xml error: Line=" << ex.lineNumber()
			   << " Col=" << ex.columnNumber()
			   << " Msg=" << ex.message();
	return false;
}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------

void Logbook::Update()
{
	NesisAerospaceDataManager* pND = NesisAerospaceDataManager::GetInstance();
	// Start the recording process if necessary.
	bool bEngineRunning = pND->GetRPM() > RPM_TRESHOLD;
	bool bIsMoving		  = pND->GetIAS() > IAS_TRESHOLD;

	// If not recording, try to start recording when conditions are met.
	if(IsRecording()==false) {
		// We do not just start - there must be at least some indicated
		// airspeed or engine turning.
		if(bEngineRunning || bIsMoving) {
			// Start is pretty complex and may take some time.
			m_entry.Clear();
			StartRecording();
		}
	}
	// If recording - check if we can stop.
	else {
		// We must have the flight and engine situation
		// clear - this means that the flight or engine recording
		// is not active.
		if(bEngineRunning == false
		   && bIsMoving == false
		   &&	m_ofFlight.first.isValid() == false
		   &&	m_ofEngine.first.isValid() == false)
		{
			StopRecording();
		}
		else {
			// Record update.
			Record::Data d;
			m_record.GetDataSnapshot(d);
			m_record.Update(d);

			// Current active entry update.
			UpdateEntry(d, m_record.GetStart());
		}
	}
}

// -----------------------------------------------------------------------
/** The recording start is a pretty complex process. There are
    four possibilites on the start:

    -# The last record was terminated. This can happen
       only if the instrument was turned off and logbook destructor was
       called (all files were closed properly). In this case we simply
       start new record with a new filename.

    -# The last record was not terminated. The record is scanned
       and tested for the "restart" condition. Is such condition is met,
       current logbook entry is updated and we continue writing into the
       same record.

    -# The last record was not terminated and "restart" condition
       is not met. In this case, the record is terminated and scanned
       for the entry update as well. If at least one "flying" condition
       is met, entry is written to the logbook and a new record is
       started (with a new filename.)

    -# The same as above, but no "flying" condition was found. The last record
       is deleted and the record number is reused. This situation is typical
       for on-ground instrument use and it does not have any sense to
       record such situations.
 */
void Logbook::StartRecording()
{
	// Clear internal variables.
	Clear();
	// Check if the previous record was terminated.
	QString qsFile = m_record.IsLastRecordTerminated();

	// If qsFile is empty, the record was terminated and we have
	// situation 1.
	if(qsFile.isEmpty()) {
		qDebug() << "Log:Start Case 1 (Last was correctly terminated, starting new.)";
		m_record.StartNewRecordFile();
	}
	// We have situations from 2 to 4.
	else {
		// Open the existing file for reading and writing.
		QFile file(qsFile);
		file.open(QIODevice::ReadWrite);
		Q_ASSERT(file.isOpen()==true);
		// Test if we can continue writing into existing record.
		if(m_record.CanContinue(file)) {
			qDebug() << "Log:Start Case 2 (Last opened, able to continue).";
			// Yes, we can - situation 2.
			// Update current entry with the past information.
			QDateTime dtStart = UpdateEntryFromFile(file, true);
			// Set the recoding flag and set the start time.
			Q_ASSERT(dtStart.isValid());
			file.close();
			m_record.ContinueRecording(dtStart, file.fileName());
		}
		// No, we can't continue on existing file.
		// Try to terminate the file.
		else {
			UpdateEntryFromFile(file, false);

			// If there was at least one flight.
			if(m_entry.GetFlightCount()>0) {
				qDebug() << "Log:Start Case 3. (Last was opened, terminated, started new).";
				// Terminate current record.
				TerminateRecord(&file);
				// Start with a new number.
				m_entry.Clear();
				Clear();
				m_record.StartNewRecordFile();
			}
			else {
				qDebug() << "Log:Start Case 4. (Last was opened, deleted, restarted).";
				// Delete the record and keep the number.
				file.close();
				remove(file.fileName().toAscii());
				m_entry.Clear();
				Clear();
				m_record.StartNewRecordFile(Settings::GetInstance()->GetLastLogRecordId());
			}
		}
	}
}
// -----------------------------------------------------------------------

void Logbook::TerminateRecord(QFile* pFile)
{
	qDebug() << "Terminating record: "
				<< Settings::GetInstance()->GetCurrentPilot();

	// Terminate the file. This will also close the file.
	m_record.Terminate(pFile);

	// Add record path.
	if(pFile)
		// file name must be without absolute path - just name [rok]
// 		m_entry.SetRecordFileName(QFileInfo(*pFile).fileName());
		m_entry.SetRecordFileName(pFile->fileName());
	else
		m_entry.SetRecordFileName(m_record.GetFileName());

	// Add pilot to the entry.
	m_entry.SetPilot(Settings::GetInstance()->GetCurrentPilot());

	// Add statistic data into entry.
	m_entry.SetDistance(GetDistance());
	m_entry.SetMinimalAltitude(GetMinAltitude());
	m_entry.SetMaximalAltitude(GetMaxAltitude());
	m_entry.SetMaximalSpeed(GetMaxSpeed());
	m_entry.SetAverageSpeed(GetAverageSpeed());
	m_entry.SetFuelConsumption(GetFuelConsumption());
	m_entry.SetSpecificFuelConsumption(GetSpecificFuelConsumption());

	// Write this entry into the logbook container and XML. Flush.
	if(m_entry.IsValid()==false)
		qDebug() << "Log: Invalid entry detected.";
	m_conBook.push_back(m_entry);

	// Update the XML file.
	WriteXML();
}

// -----------------------------------------------------------------------
/** Scan trough the file and update the m_entry object with engine on/off
    lift offs and landings.
 */
QDateTime Logbook::UpdateEntryFromFile(
	QFile& file,	//!< The file we are working on.
	bool bContinue	//!< Is this update for continue
)
{
	QDateTime dtStart;

	// Move file poiner to the beginning.
	file.seek(0);
	QDataStream in(&file);

	// Check magic number.
	qint32 uiMagic;
	in >> uiMagic;
	if(uiMagic != LOG_RECORD_MAGIC_NUMBER) {
		qWarning() << "Invalid blog file " << file.fileName();
		return dtStart; // Invalid one
	}

	// Check version
	qint16 uiVersion;
	in >> uiVersion;
	Q_ASSERT(uiVersion==1);

	// Get date&time of original creation
	in >> dtStart;

	// Get number of records in the file.
	qint32 iCount = ((file.size()-FIRST_RECORD_POS)/FLIGHTDATA_SIZE);

	m_ofFlight.first = m_ofFlight.second = QDateTime();
	m_iFlightCounter = m_iFlightTime = 0;
	m_ofEngine.first = m_ofEngine.second = QDateTime();
	m_iEngineCounter = m_iEngineTime = 0;

	Record::Data d;
	file.seek(FIRST_RECORD_POS);

	while(iCount-- > 0) {
		d.Read(in);
		UpdateEntry(d, dtStart);
	}

	// This entry will not continue and will terminate.
	if(bContinue == false) {
		// There is a chance that last engine & flight events are stil
		// opened at the last data in the record. We must close them.
		// E.G. Instrument was shut down while engine was still running.
		if(m_ofEngine.first.isValid() && m_ofEngine.second.isValid()==false) {
			m_ofEngine.second = dtStart.addSecs(d.uiSec);
			m_entry.AddEngine(m_ofEngine);
		}
		if(m_ofFlight.first.isValid() && m_ofFlight.second.isValid()==false) {
			m_ofFlight.second = dtStart.addSecs(d.uiSec);
			m_entry.AddFlight(m_ofFlight);
		}
	}

	return dtStart;
}
// -----------------------------------------------------------------------


void Logbook::UpdateEntry(const Record::Data& d, const QDateTime& dtStart)
{
	int iRPM  = d.uiRPM;
	int iTime = d.uiSec;
	int iIAS  = d.uiIAS/128;

	// Engine

	// We are searching for engine ON condition.
	if(m_ofEngine.first.isValid()==false) {
		// Do we have enough RPM?
		if(iRPM > RPM_TRESHOLD) {
			// If this is the first occasion, remember the time.
			if(m_iEngineCounter==0) {
				m_iEngineTime = iTime;
				m_iEngineCounter++;
			}
			// ok, increase the counter and test ...
			else {
				m_iEngineCounter++;
				// ... if the counter is high enough.
				if(m_iEngineCounter > LOOKBACK) {
					// Mark the engine start condition.
					m_ofEngine.first = dtStart.addSecs(m_iEngineTime);
					m_iEngineCounter = m_iEngineTime = 0;
					qDebug() << "Engine start" << m_ofEngine.first;
				}
			}
		}
		// Not enough RPM
		else {
			// Reset the counter and the startup time.
			m_iEngineCounter = m_iEngineTime = 0;
		}
	}
	// We are searching for engine OFF condition.
	else {
		// Do we have RPMs low enough?
		if(iRPM < RPM_TRESHOLD) {
			// If this is the first occasion, remember the engine off time candidate.
			if(m_iEngineCounter==0) {
				m_iEngineTime = iTime;
				m_iEngineCounter++;
			}
			// If we already have counter running ...
			else {
				m_iEngineCounter++;
				// ... and if the counter reached the requtested level.
				if(m_iEngineCounter > LOOKBACK) {
					// Mark the engine off condition.
					m_ofEngine.second = dtStart.addSecs(m_iEngineTime);
					qDebug() << "Engine stop" << m_ofEngine.second;
					// Append to container and ...
					m_entry.AddEngine(m_ofEngine);
					// .. reset search conditions for the new engine start
					m_ofEngine.first = m_ofEngine.second = QDateTime();
					Q_ASSERT(m_ofEngine.first.isValid()==false && m_ofEngine.second.isValid()==false);
					m_iEngineCounter = m_iEngineTime = 0;
				}
			}
		}
		// RPM to high
		else {
			// Reset
			m_iEngineCounter = m_iEngineTime = 0;
		}
	}

	// Flight

	// We are searching for lift off condition.
	if(m_ofFlight.first.isValid()==false) {
		// Do we have enough IAS?
		if(iIAS > IAS_TRESHOLD) {
			// If this is the first occasion, remember the time.
			if(m_iFlightCounter==0) {
				m_iFlightTime = iTime;
				m_iFlightCounter++;
			}
			// ok, increase the counter and test ...
			else {
				m_iFlightCounter++;
				// ... if the counter is high enough.
				if(m_iFlightCounter > LOOKBACK) {
					// Mark the lift off condition.
					m_ofFlight.first = dtStart.addSecs(m_iFlightTime);
					qDebug() << "Take off" << m_ofFlight.first;
					m_iFlightCounter = m_iFlightTime = 0;
				}
			}
		}
		// Not enough IAS
		else {
			// Reset the counter and the startup time.
			m_iFlightCounter = m_iFlightTime = 0;
		}
	}
	// We are searching for landing condition.
	else {
		// Do we have IAS low enough?
		if(iIAS < IAS_TRESHOLD) {
			// If this is the first occasion, remember the landing time candidate.
			if(m_iFlightCounter==0) {
				m_iFlightTime = iTime;
				m_iFlightCounter++;
			}
			// If we already have counter running ...
			else {
				m_iFlightCounter++;
				// ... and if the counter reached the requtested level.
				if(m_iFlightCounter > LOOKBACK) {
					// Mark the landing condition.
					m_ofFlight.second = dtStart.addSecs(m_iFlightTime);
					qDebug() << "Landed" << m_ofFlight.second;
					// Append to container and ...
					m_entry.AddFlight(m_ofFlight);
					// .. reset search conditions for the new flight
					m_ofFlight.first = m_ofFlight.second = QDateTime();
					Q_ASSERT(m_ofFlight.first.isValid()==false && m_ofFlight.second.isValid()==false);
					m_iFlightCounter = m_iFlightTime = 0;
				}
			}
		}
		// Not enough IAS
		else {
			// Reset
			m_iFlightCounter = m_iFlightTime = 0;
		}
	}

	// When we are flying ...
	if(iIAS > IAS_TRESHOLD) {
		// Distance update - we calculate Flat Earth distance from previous coordinate.
		AddDistance(QPointF(d.fLongitude, d.fLatitude));
		// Min max altitude update.
		UpdateAltitudeLimits(d.iAltitude);
		// Max speed update
		UpdateSpeedLimits(d.uiIAS/128.0f);
	}
	else {
		// Set last distance coordinate in entry to zero.
		ResetLastCoordinate();
	}

	// When the engine is running
	if(iRPM > RPM_TRESHOLD) {
		// Add fuel flow l/h
		AddFuel(d.uiFuelFlow/512.0f);
	}

}

// -----------------------------------------------------------------------

void Logbook::WriteXML()
{
	QFile file(m_qsFileName);
	file.open(QIODevice::WriteOnly | QIODevice::Text);

	xml::XmlWriter xw(&file);
	xw.SetAutoNewLine(true);

	// Start with the calibrate tag and the program version
	xw.WriteOpenTag(TAG_LOGBOOK, xml::AttrMap(TAG_VERSION, 1));

	// Write each entry
	for(int i=0; i<m_conBook.count(); i++) {
		m_conBook[i].Write(xw);
	}

	xw.WriteCloseTag(TAG_LOGBOOK);
}

// -----------------------------------------------------------------------
/** Update internal distance using flat earth model.
 */
void Logbook::AddDistance(const QPointF& ptNew)
{
	if(!m_ptLastCoor.isNull()) {
		m_fDistance += common::GetDistanceFlat(
			m_ptLastCoor.x(), m_ptLastCoor.y(),
			ptNew.x(), ptNew.y()
		) * EARTH_RADIUS_M;
	}
	m_ptLastCoor = ptNew;
}

// -----------------------------------------------------------------------

void Logbook::UpdateAltitudeLimits(int iAlt)
{
	m_iMinAlt = qMin(m_iMinAlt, iAlt);
	m_iMaxAlt = qMax(m_iMaxAlt, iAlt);
}

// -----------------------------------------------------------------------

void Logbook::UpdateSpeedLimits(float fSpeed)
{
	m_fMaxSpeed = qMax(m_fMaxSpeed, fSpeed);
}

// -----------------------------------------------------------------------

void Logbook::AddFuel(float fFuelFlow)
{
	m_fFuel += fFuelFlow/3600.0f; // convert l/h into l/s
	m_iFuelCount++;
}

// -----------------------------------------------------------------------
/** Calculate average ground speed. This is ratio between distance
    traveled and flight time. When an entry object is active (pending) the
    flight time is not correct because it gets recorded only when
    the speed drops below some limit for a certain period of time.
    In this case, the caller should provide extra flight time.
    The logbook object Logbook::m_ofFlight may be used to get this time.
 */
float Logbook::GetAverageSpeed(
	float fExtraSec //!< Extra seconds used in an "active" where flight time is not updated yet.
) const
{
	float fSpeed = 0.0f;
	float fFlight = fExtraSec > 0 ? fExtraSec : m_entry.GetFlightTimeSecs();

	if(fFlight > 0)
		fSpeed = m_fDistance/fFlight;

	return fSpeed;
}

// -----------------------------------------------------------------------
/** Calculate current fuel consumption for this flight.
    If this object is an active object, engine extra time should be passed
    in argument.

    \sa GetAverageSpeed
 */
float Logbook::GetFuelConsumption(
	float fExtraSec //!< Extra seconds used in an "active" where flight time is not updated yet.
) const
{
	float fEngine = fExtraSec > 0 ? fExtraSec : m_entry.GetEngineTimeSecs();

	if(m_iFuelCount < 1)
		return 0.0;
	float fdT = fEngine/m_iFuelCount;

	return m_fFuel*fdT;
}

// -----------------------------------------------------------------------
/** Calculate specific fuel comsumption per engine hour for this flight.
    If this object is an active object, engine extra time should be passed
    in argument.

    \sa GetAverageSpeed
 */
float Logbook::GetSpecificFuelConsumption(

	float fExtraSec //!< Extra seconds used in an "active" where flight time is not updated yet.
) const
{
	float fEngine = fExtraSec > 0 ? fExtraSec : m_entry.GetEngineTimeSecs();
	if(fEngine < 1.0)
		return 0.0;

	return GetFuelConsumption(fExtraSec)/fEngine * 3600;
}

// -----------------------------------------------------------------------

Logbook::Container Logbook::GetReverseContainer(
	int iMaxCount, // Max number of items in the container
	bool bCurrent	// Include current entry status
) const
{
	Container con;

	if(bCurrent && IsRecording()) {
		Entry e(m_entry);
		QDateTime dtNow = m_record.GetNow();
		OnOff oo;
		if(m_ofEngine.first.isValid() && m_ofEngine.second.isValid()==false) {
			oo = m_ofEngine;
			oo.second = dtNow;
			e.AddEngine(oo);
// 			qDebug() << "Current engine" << oo.first << oo.second;
		}
		if(m_ofFlight.first.isValid() && m_ofFlight.second.isValid()==false) {
			oo = m_ofFlight;
			oo.second = dtNow;
			e.AddFlight(oo);
// 			qDebug() << "Current Flight" << oo.first << oo.second;
		}

		float fEngineSec = e.GetEngineTimeSecs();
		float fFlightSec = e.GetFlightTimeSecs();
		e.SetPilot(QObject::tr("Current Flight"));
		e.SetRecordFileName(m_record.GetFileName());

		// Add statistic data into entry.
		e.SetDistance(GetDistance());
		e.SetMinimalAltitude(GetMinAltitude());
		e.SetMaximalAltitude(GetMaxAltitude());
		e.SetMaximalSpeed(GetMaxSpeed());
		e.SetAverageSpeed(GetAverageSpeed(fFlightSec));
		e.SetFuelConsumption(GetFuelConsumption(fEngineSec));
		e.SetSpecificFuelConsumption(GetSpecificFuelConsumption(fEngineSec));

		if(e.IsValid()) {
			iMaxCount--;
			con.push_back(e);
		}
	}

	for(int i=0; i<qMin(iMaxCount, m_conBook.count()); i++) {
		con.push_back(m_conBook[m_conBook.count()-i-1]);
	}
	return con;
}
// -----------------------------------------------------------------------

void Logbook::StopRecording()
{
	if(IsRecording()) {
		if(m_entry.GetFlightCount()>0) {
			// Terminate current record and update XML file.
			qDebug() << "Log:Stoped recording.";
			TerminateRecord(NULL);
		}
		else {
			qDebug() << "Log:Deleting non-flight entry.";
			// Delete the record and keep the number.
			m_record.Close();
			remove(m_record.GetFileName().toAscii());
			// This would left the last file id entry in settings with
			// no record. On the next run, the id will be increased, which
			// is not what we want.
			Settings::GetInstance()->IncreaseLogRecordId(-1);
		}
	}
}
// -----------------------------------------------------------------------

bool Logbook::DeleteAll()
{
	if(IsRecording())
		return false;

	Settings* pS = Settings::GetInstance();

	// Delete all blog files.
	QDir dirLog(pS->GetLogPath(), "*.blog");
	QFileInfoList fil = dirLog.entryInfoList();

	// remove files
	for(int i=0; i<fil.count(); i++)
		dirLog.remove(fil[i].absoluteFilePath());
	dirLog.remove(pS->GetLogPath()+"Logbook.xml");

	m_conBook.clear();
	return true;
}

// -----------------------------------------------------------------------
} // namespace logbook

