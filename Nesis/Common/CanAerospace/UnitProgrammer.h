#ifndef UNIT_BIN_H
#define UNIT_BIN_H

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

#include <QString>
#include <QFile>
#include <QThread>
#include "CanAerospace/AbstractAerospaceDataManager.h"


namespace can {

// ------------------------------------------------------------------------
/** \brief Handles unit program in binary format.
*/

class UnitProgrammer : public QThread
{
Q_OBJECT
public:
	//! Bin info structure stores basic information about bin files to be updated.
	struct BinInfo {
		BinInfo() { m_uiSV = m_uiSVN = m_uiRHV = 0; m_bBin = m_bMD5 = false; }
		unsigned int m_uiSV;		//!< Software version.
		unsigned int m_uiSVN;	//!< SVN revision.
		unsigned int m_uiRHV;	//!< Required hardware version.
		bool			 m_bBin;		//!< Does bin exist?
		bool			 m_bMD5;		//!< Md5 check test.
	};
public:
	//! Constructor
	UnitProgrammer(const QString& qsBinPath=QString(), QObject* pParent=0);
	//! Destructor
	~UnitProgrammer();

	//! Set CANAerospace data manager.
	void SetAerospaceDataManager(can::AbstractAerospaceDataManager* pADM)
	{ m_pADM = pADM; }

	//! Set path to the folder with bin files.
	void SetBinPath(const QString& qsBinPath);
	
	//! Set the extension for the program binaries. (default = bin)
	void SetBinExtension(const QString& qsBinExt)
	{ m_qsBinExt = qsBinExt; }
	
	//! Set path to the file with md5 sums.
	void SetMD5File(const QString& qsMD5File)
	{ m_qsMD5File = qsMD5File; }

	//! Start programming some unit. Returns number of pages. Zero on error.
	uint32_t Start(uint8_t byNodeId, bool bSpi = false);
	
	//! Reset the programmer.
	void Reset();

	//! Return access to the data in requested page. NULL on error.
	uint32_t* GetPage(int iPage);

	//! Return Unit name for given hardware id.
	QString GetUnitName(uint8_t byNodeId) const;
	
	//! Return info for given id. This call is slow - we read this from file.
	BinInfo GetBinInfo(uint8_t byNodeId) const;
	
	//! Get file name from node id.
	QString GetFileName(uint8_t byNodeId, const char* pExt=NULL) const;

signals:
	//! Application programming started - number of pages.
	void APSStartReport(quint8 byHardwareId, quint32 uiPages);
	//! Page was requested.
	void APSPageRequestReport(quint32 uiPage);
	//! Application programming stoped - state.
	void APSFinishReport(qint32 iState);

private:
	//! We keep sending data in its own thread.
	void run();

	//! Is unit currently active? It is active if the file is open.
	bool IsActive() const
	{ return m_fBin.isOpen(); }
	//! Load unit with given id. Previous unit must be closed.
	bool Load(uint8_t byNodeId);
	//! Return number of pages required.
	uint32_t GetPageCount() const;
	//! Calculate hash.
	void CalculateHash();
	
private:
	//! Path to the folder with binaries.
	QString m_qsBinPath;
	//! Path to the md5 file.
	mutable QString m_qsMD5File;
	//! File - alowing read only access to individual pages.
	QFile m_fBin;
	//! The program binary extension. Default = bin.
	QString m_qsBinExt;
	//! One page - page currently loaded.
	char m_achPage[512];
	//! Hash is calculated when the file is loaded.
	uint32_t m_uiHash;
	//! Hash offset - hash position.
	int32_t m_iHashPos;
	//! Offset of the first page in the binary file.
	int32_t m_iPageOffset;

	//! Access to the CANAerospace object.
	can::AbstractAerospaceDataManager* m_pADM;
};

} // namespace
#endif
