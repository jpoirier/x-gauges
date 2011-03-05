#ifndef __LOGBOOK_RECORD_H
#define __LOGBOOK_RECORD_H

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

#include <QTime>
#include <QFile>
#include <QDateTime>

//! Maximal size on disk used by "blog" files.
#define MAX_LOG_RECORD_SPACE        2*1024*1024
//! A magic number that helps to identify the file.
#define LOG_RECORD_MAGIC_NUMBER     0x401a9636

//! Number of items to look back.
#define LOOKBACK							6
//! Max elapsed time between the last record and now.
#define MAX_TIME_DIFFERENCE			5*60

//! Position of the first record.
#define FIRST_RECORD_POS				32
//! Position of the termination byte.
#define TERMINATE_FLAG_POS				15
//! Position of the hash value in the header.
#define HASH_POS							16

//! Size of data written on file
#define FLIGHTDATA_SIZE					25

// -------------------------------------------------------------------------
namespace logbook {

class Record
{
public:
	//! Helper data struct. This is what gets recorded.
	struct Data {
		quint16 uiSec;				//!< Time [s] since the recording started.

		float  fLatitude;			//!< Latitude [rad]
		float  fLongitude; 		//!< Longitude [rad]
		qint16 iAltitude;			//!< Altitude [m]

		qint16  iRoll;				//!< Roll (16 bits)
		qint16  iPitch;			//!< Pitch (16 bits)
		quint16 uiYaw;				//!< Yaw (16 bits)

		qint16 uiIAS;				//!< Indicated air speed (/128)
		qint16 uiRPM;				//!< Engine RPM
		quint8 uiSatCount; 		//!< Number of GPS satelites

		quint16 uiFuelFlow;		//!< Used to calculate fuel consumption (1/512).

		// Don'f forget to adjust FLIGHTDATA_SIZE if struct is changed.

		// Write data into stream
		void Write(QDataStream& out) const;
		// Read data from stream
		void Read(QDataStream& in);
	};

public:
	//! Constructor - resets the state.
	Record();

	//! Destructor.
	~Record();

	//! Update record with current data snapshot.
	void Update(const Data& d);

	//! Return true if we are already recoding.
	bool IsRecording() const
	{ return m_bRecording; }

	//! Return filename of the current file.
	QString GetFileName() const
	{ return m_file.fileName(); }
	//! Close the file.
	void Close()
	{ m_file.close(); m_bRecording = false; }

	//! Is the last record terminated?
	QString IsLastRecordTerminated() const;

	//! Can we continue writing into the same record ("restart").
	bool CanContinue(QFile& file);

	//! Starts new record in the log folder. It also makes several checks and the writes header.
	bool StartNewRecordFile(int iId=-1);

	//! Continue recording on existing file.
	void ContinueRecording(const QDateTime& dtStart, const QString& qsFile);

	//! Terminate the record (close the file, calculate hash and write it into header.)
	void Terminate(QFile* pFile=NULL);

	//! Get data snapshot - fill the data structure.
	void GetDataSnapshot(Data& d);

	//! Return the time and date when the recording started.
	const QDateTime& GetStart() const
	{ return m_dtStart; }
	
	//! Return current time-date based on the record start and seconds elapsed.
	QDateTime GetNow() const
	{ return m_dtStart.addSecs(m_uiSec); }
	
	//! Get seconds elapsed since record start.
	unsigned int GetSecondsElapsed() const
	{ return m_uiSec; }
private:

	//! Get the existing filename string of the given log record id. It can be empty.
	QString GetExistingRecordFilename(int iLogRecordId) const;

	//! Create a filename for given log record id.
	QString CreateRecordFilename(int iLogRecordId) const;

private:
	//! Is recording active. (Constructor sets this to false).
	bool m_bRecording;

	//! Marks the time and date of the start.
	QDateTime m_dtStart;

	//! The file that we are recording to.
	QFile m_file;
	
	//! Current time (secs) elapsed since the record start.
	unsigned int m_uiSec;
};

// -------------------------------------------------------------------------
} // namespace
#endif
