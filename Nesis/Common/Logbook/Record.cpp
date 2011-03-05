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
#include <QDir>
#include <QDate>
#include "Settings.h"
#include "NesisAerospaceDataManager.h"
#include "SuperFastHash.h"
#include "Entry.h"

#include "Record.h"


namespace logbook {

// --------------------------------------------------------------------------

void Record::Data::Write(QDataStream& out) const
{
	out << uiSec;
	out << fLatitude;
	out << fLongitude;
	out << iAltitude;

	out << iRoll;
	out << iPitch;
	out << uiYaw;

	out << uiIAS;
	out << uiRPM;
	out << uiSatCount;

	out << uiFuelFlow;
}

// --------------------------------------------------------------------------

void Record::Data::Read(QDataStream& in)
{
	in >> uiSec;
	in >> fLatitude;
	in >> fLongitude;
	in >> iAltitude;

	in >> iRoll;
	in >> iPitch;
	in >> uiYaw;

	in >> uiIAS;
	in >> uiRPM;
	in >> uiSatCount;

	in >> uiFuelFlow;
}

// --------------------------------------------------------------------------


Record::Record()
{
	m_bRecording = false;
}

Record::~Record()
{
	if(m_file.isOpen())
		Terminate();
}

// -----------------------------------------------------------------------
/** Check if the last record was terminated. This may take quite a
    while, since only the last record number is known and read
    from settings.

	\retval a valid filename if not terminated and
	\retval an empty string if it was terminated properly.
 */

QString Record::IsLastRecordTerminated() const
{
	Settings* pS = Settings::GetInstance();

	// Get the filename for the last id stored in settings.
	int iLastId = pS->GetLastLogRecordId();
	// This call takes some time, since the complete folder is loaded.
	QString qsFile = GetExistingRecordFilename(iLastId);

	// if the file is not found, we will assume it was properly closed.
	// It either didn't exist or it was copied into the SD card and removed.
	if(qsFile.isEmpty())
		return qsFile;

	// Ops, the file was found. Was it terminated as well?
	// The file is terminated, if the terminate byte is FF.
	QFile file(qsFile);
	file.open(QIODevice::ReadOnly);
	Q_ASSERT(file.isOpen()==true);
	file.seek(TERMINATE_FLAG_POS);
	uint8_t byTerm = 0;
	file.read((char*)&byTerm, 1);

	// TODO We could also check if the hash value matches the file.

	// 0xff indicates a match.
	return (byTerm == 0xff) ? QString() : qsFile;
}

// -----------------------------------------------------------------------
/** Search the log folder for the filenames. The first four letters
    in the filename correspond to the log id.

    \retval Requested filename or empty string.
 */
QString Record::GetExistingRecordFilename(
	int iLogRecordId	//!< The record.
) const
{
	Settings* pS = Settings::GetInstance();

	// Construct the search filter string.
	QString qsFilter;
	qsFilter.sprintf("%04i*.blog", iLogRecordId);


	// Files in the log folder.
	QDir dir(pS->GetLogPath(), qsFilter);
	if(dir.exists()==false) {
		qWarning() << "Folder"
					  << pS->GetLogPath()
					  << "does not exist.";
	}

	// Get the files ...
	QStringList slFiles = dir.entryList();
	// It should be only one or no at all.
	if(slFiles.count()==1) {
		return pS->GetLogPath()+slFiles[0];
	}

	return QString();
}

// -----------------------------------------------------------------------

QString Record::CreateRecordFilename(int iLogRecordId) const
{
	// Construct the search filter string.
	QString qsFile;
	qsFile.sprintf("%04i_", iLogRecordId);
	qDebug() << "Creating Log file" << iLogRecordId;
	// TODO Get date format form settings
	qsFile += QDate::currentDate().toString("dd.MM.yy");
	qsFile += ".blog";
	qsFile.prepend(Settings::GetInstance()->GetLogPath());

	return qsFile;
}

// -----------------------------------------------------------------------

void Record::Update(const Data& d)
{
	// the recording flag must be true here
	Q_ASSERT(m_bRecording == true);
	// Write data to the file.
	Q_ASSERT(m_file.isOpen()==true);

	m_uiSec = d.uiSec;

	QDataStream out(&m_file);
	d.Write(out);
	m_file.flush();
}

// -----------------------------------------------------------------------
/** Before we start a new record file, we make sure that there is
    enough space on the folder by removing the old files.

	 Then we increase the record number in settings and calculate
    the new filename. A header is writen, the file is kept open
    and the file pointer is set to the first record.

	 \retval true on success.
	 \retval false on failure (can't open the file).
 */
bool Record::StartNewRecordFile(
	int iLogRecordId	//! File with requested id. If iId == -1, automatic id will be used.
)
{
	// If we have automatic id
	if(iLogRecordId==-1) {
		Settings* pS = Settings::GetInstance();

		// Check the space occupied by the log files.
		QDir dirLog(pS->GetLogPath(), "*.blog");
		QFileInfoList fil = dirLog.entryInfoList();

		qint64 iAllFiles = 0;
		foreach(QFileInfo f, fil)
			iAllFiles += f.size();

		qDebug() << "Total binary log size [kB]" << iAllFiles/1024;

		// remove old files (identified by leading number) until
		// enough space is made.
		while(iAllFiles >= MAX_LOG_RECORD_SPACE) {
			QFileInfo f = fil.front();
			qDebug() << "Deleting" << f.fileName();
			fil.pop_front();
			iAllFiles -= f.size();
			dirLog.remove(f.absoluteFilePath());
		}
		// Create new log record id. It is automatically stored in settings.
		iLogRecordId = pS->IncreaseLogRecordId();
	}

	// Now create new filename and open it in binary mode for writing AND reading.
	Q_ASSERT(m_file.isOpen()==false);
	m_file.setFileName(CreateRecordFilename(iLogRecordId));
	if(m_file.open(QIODevice::ReadWrite)==false) {
		qWarning() << "Failed to open the " << m_file.fileName();
		return false;
	}

	// The file is opened now.  Write the header info.
	QDataStream out(&m_file);

	// Start with magic number
	out << (quint32) LOG_RECORD_MAGIC_NUMBER;
	// Version
	out << (quint16) 1;
	// Date (32+32+8 bits = julianday, miliseconds since midnight, UTC/local flag)
	m_dtStart = QDateTime::currentDateTime();
	out << m_dtStart;

	// reserve four bytes for future use
	Q_ASSERT(m_file.pos()==TERMINATE_FLAG_POS);

	// pad zeroes
	qint64 iPos = m_file.pos();
	Q_ASSERT(iPos < FIRST_RECORD_POS);
	iPos = FIRST_RECORD_POS-iPos; // zeros to write
	while(iPos-- > 0)
		out << (quint8)0;

	// First record starts at uint8_t FIRST_RECORD_POS.
	m_bRecording = true;

	return true;
}

// -----------------------------------------------------------------------

void Record::GetDataSnapshot(Data& d)
{
	// Time
	d.uiSec = m_dtStart.time().elapsed()/1000;

	NesisAerospaceDataManager* pND = NesisAerospaceDataManager::GetInstance();

	QPointF ptPos = pND->GetPosition();
	d.fLatitude   = ptPos.y();
	d.fLongitude  = ptPos.x();

	d.iAltitude   = qRound(pND->GetAltitude());

	d.iRoll       = qRound(pND->GetRoll()*10000);
	d.iPitch      = qRound(pND->GetPitch()*10000);
	d.uiYaw       = qRound(pND->GetYaw()*10000);

	d.uiIAS       = qRound(pND->GetIAS()*128);
	d.uiRPM       = pND->GetRPM();
	d.uiSatCount  = pND->GetSatelliteCount();

	d.uiFuelFlow  = qRound(pND->GetFuelFlow()*512);
}

// -----------------------------------------------------------------------

void Record::Terminate(QFile* pFile)
{
	if(pFile==NULL)
		pFile = &m_file;

	// The record must be open.
	Q_ASSERT(pFile->isOpen());

	// Go to the first record.
	pFile->seek(FIRST_RECORD_POS);

	// Any hash up to 128 bits (16 bytes) may be used.
	common::SuperFastHash sfh; // 32 bit hash.
	unsigned char acBuffer[1024];
	qint64 iBytesRead;

	do {
		iBytesRead = pFile->read ((char*)acBuffer, 1024);
		sfh.Update(acBuffer, iBytesRead);
	} while(iBytesRead == 1024);

	pFile->seek(TERMINATE_FLAG_POS);
	QDataStream out(pFile);
	out << (quint8)0xff;			// terminate uint8_t
	out << (quint32)sfh.Get();	// hash

	pFile->close();
	m_bRecording = false;
}

// -----------------------------------------------------------------------
/** Open the given file and test it to see if we can continue recording
    to it. The file must not be terminated.

	 \retval true We can continue recording.
	 \retval false No, we can't continue.
 */

bool Record::CanContinue(
	QFile& file
)
{
	// For this file we already know that the terminate byte is not set.
	QDataStream in(&file);

	// Check magic number.
	qint32 uiMagic;
	in >> uiMagic;
	if(uiMagic != LOG_RECORD_MAGIC_NUMBER) {
		qWarning() << "Invalid blog file " << file.fileName();
		return false;
	}

	// Check version
	qint16 uiVersion;
	in >> uiVersion;
	Q_ASSERT(uiVersion==1);

	// Get date&time of original creation
	QDateTime dtRecord;
	in >> dtRecord;

	// Get the last readable complete record.
/*	qint64 iSize = file.size();
	qint32 iData = sizeof(Data);*/

	qint32 iPos = ((file.size()-(FIRST_RECORD_POS))/FLIGHTDATA_SIZE-1)
						*FLIGHTDATA_SIZE
						+ FIRST_RECORD_POS;
	/*bool bSeek = */file.seek(iPos);
	Data d;
	d.Read(in);

	// If there was flying at the time of the last record and
	// if we are flying now ...
	int iIAS = NesisAerospaceDataManager::GetInstance()->GetIAS();

	if(d.uiIAS/128 > IAS_TRESHOLD && iIAS > IAS_TRESHOLD) {
		dtRecord.addSecs(d.uiSec);
		int iDeltaSec = dtRecord.secsTo(QDateTime::currentDateTime());
		if(iDeltaSec <= MAX_TIME_DIFFERENCE) {
			return true;
		}
	}
	return false;
}

// -----------------------------------------------------------------------

void Record::ContinueRecording(
	const QDateTime& dtStart,
	const QString& qsFile)
{
	m_dtStart = dtStart;
	Q_ASSERT(m_file.isOpen()==false);
	m_file.setFileName(qsFile);
	m_file.open(QIODevice::ReadWrite);
	Q_ASSERT(m_file.isOpen()==true);

	// Set correct file position.
	qint32 iLast = ((m_file.size()-FIRST_RECORD_POS)/FLIGHTDATA_SIZE)*FLIGHTDATA_SIZE+FIRST_RECORD_POS;
	m_file.seek(iLast);

	m_bRecording = true;
}

// -----------------------------------------------------------------------
}// namespace

