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

#include <QDebug>
#include <QFileInfo>

#include "CommonDefsQt.h"
#include "KanardiaHardwareUnits.h"
#include "SuperFastHash.h"
#include "StaticHelper.h"
#include "UnitProgrammer.h"

// See loader.c for structure where these offsets come from.
#define BOOT_HEADER_ADDRESS							0x4000
#define APPLICATION_HEADER_ADDRESS					(BOOT_HEADER_ADDRESS + 128)

// BOOT_HEADER_ADDRESS +
#define OFFSET_HASH_START								0x04
#define OFFSET_HASH_END									0x08

// APPLICATION_HEADER_ADDRESS +
#define OFFSET_SOFTWARE_VERSION						0x18
#define OFFSET_REQUIRED_HARDWARE_VERSION			0x1c
#define OFFSET_REVISION_SVN							0x20

namespace can {

// --------------------------------------------------------------------

UnitProgrammer::UnitProgrammer(const QString& qsBinPath, QObject* pParent)
: QThread(pParent)
{
	m_iPageOffset = BOOT_HEADER_ADDRESS;
	m_pADM = NULL;
	m_qsBinExt = "bin";
	SetBinPath(qsBinPath);
}

// --------------------------------------------------------------------

UnitProgrammer::~UnitProgrammer()
{
}

void UnitProgrammer::SetBinPath(const QString& qsBinPath)
{ 
	m_qsBinPath = qsBinPath; 
	qDebug() << "UnitProgrammer bin path =" << m_qsBinPath;
}

// --------------------------------------------------------------------

uint32_t UnitProgrammer::Start(uint8_t byNodeId, bool bSpi)
{
	int32_t iCount = 0;
	// Ask the programmer for the number of pages.
	if(Load(byNodeId)) {
		qDebug() << "Programmer::Start node =" << (int)byNodeId; 
		if (bSpi == false) {
			iCount = GetPageCount();
			// If the number is positive, start the thread, which keeps sending
			// messages. When the thread is finished, it will close the file
			// and reset the APS state.
			Q_ASSERT(isRunning()==false);
			emit APSStartReport(byNodeId, iCount);
			if(iCount > 0) {
				start();
			}
		}
		else {
			iCount = GetPageCount();
			emit APSStartReport(byNodeId, iCount);
			qDebug() << "SPI mode";
		}
	}
	return iCount;
}

// ----------------------------------------------------------------------
/** Load the file and calculate the hash value.
 */
bool UnitProgrammer::Load(
	uint8_t byNodeId	//! Node id.
)
{
	if(IsActive()) {
		qWarning() << "UnitProgrammer::Load : A file is already opened. Close it first.";
		return false;
	}

	m_fBin.setFileName(GetFileName(byNodeId));
	if(m_fBin.open(QIODevice::ReadOnly)==false) {
		qWarning() << "Failed to open the bin file" << m_fBin.fileName();
		return false;
	}
	
	// versions
	m_fBin.seek(APPLICATION_HEADER_ADDRESS+OFFSET_SOFTWARE_VERSION);
	uint32_t uiSW, uiHW, uiSVN;
	m_fBin.read((char*)&uiSW,  4);
	m_fBin.read((char*)&uiHW,  4);
	m_fBin.read((char*)&uiSVN, 4);
	qDebug() << "Unit" << byNodeId 
				<< "versions (SW, RHW, SVN):" << uiSW << uiHW << uiSVN
				<< m_fBin.fileName()
				<< "Bootloader" << (int)m_pADM->GetAPSBootloaderVer();

	// Calculate the hash and store it.
	// We will need to replace a part of the page later.
	CalculateHash();

	return true;
}

// --------------------------------------------------------------------

QString UnitProgrammer::GetUnitName(uint8_t byNodeId) const
{
	const char* pcName = can::GetHardwareName(byNodeId);

	if(pcName)
		return pcName;
	else
		return QString("?%1").arg((int)byNodeId);
}

// --------------------------------------------------------------------

QString UnitProgrammer::GetFileName(
	uint8_t byNodeId,
	const char* pExt
) const
{

	QString qs(m_qsBinPath);
	qs += GetUnitName(byNodeId);
	qs += ".";
	qs += (pExt==NULL) ? m_qsBinExt : QString(pExt);
	return qs;
}

// --------------------------------------------------------------------

uint32_t UnitProgrammer::GetPageCount() const
{
	Q_ASSERT(IsActive());
	qint32 iSize = m_fBin.size()-m_iPageOffset;
	return (iSize-1)/512 + 1;
}

// --------------------------------------------------------------------

uint32_t* UnitProgrammer::GetPage(int iPage)
{
	Q_ASSERT(IsActive());
	quint32 iPageAddr = iPage*512+m_iPageOffset;
	if(m_fBin.seek(iPageAddr)) {
		int iBytesRead = m_fBin.read(m_achPage, 512);
		// pad with zeros when necessary
		if(iBytesRead < 512) {
			for(int i=iBytesRead; i<512; i++) {
				m_achPage[i] = 0;
			}
		}
		// Do we need to insert the hash?
		int iX = m_iHashPos - iPageAddr;
		if(iX >= 0 && iX <= 512-4) {
			// Yes - put it into proper place
			*((quint32*)(&m_achPage[iX])) = m_uiHash;
// 			qDebug("Hash 0x%x was inserted.", m_uiHash);
		}
		emit APSPageRequestReport(iPage);
		return (uint32_t*)&m_achPage[0];
	}
	return NULL;
}

// --------------------------------------------------------------------

void UnitProgrammer::CalculateHash()
{
	Q_ASSERT(IsActive());
	quint32 uiHashStart;
	quint32 uiHashEnd;

	m_iHashPos = m_iPageOffset;

	m_fBin.seek(m_iPageOffset+OFFSET_HASH_START);
	m_fBin.read((char*)&uiHashStart, 4); // Start
	m_fBin.read((char*)&uiHashEnd, 4);	 // End

	Q_ASSERT(uiHashStart < uiHashEnd);
	Q_ASSERT(uiHashEnd < m_fBin.size());

	quint32 uiSize   = uiHashEnd - uiHashStart;

	m_fBin.seek(uiHashStart);
	QByteArray baBin = m_fBin.read(uiSize);

	common::SuperFastHash sfh;
	sfh.Update((const unsigned char*)baBin.constData(), uiSize);

	m_uiHash = sfh.Get();

//  	qDebug("Hash is 0x%x.", m_uiHash);
}

// --------------------------------------------------------------------------

void UnitProgrammer::run()
{
	// Wait until APS becomes active.
	while(m_pADM->IsAPSActive()==false) {
		msleep(200);
	}

// 	qDebug() << "APS Activated.";
	// Keep sending messages.
	while(m_pADM->IsAPSActive()) {
		m_pADM->CheckTimeouts();
		m_pADM->ProcessOutgoingMessages();
// 		msleep(2);
	}

// 	qDebug() << "APS completed. State =" << m_pADM->GetAPSState();
	emit APSFinishReport(m_pADM->GetAPSState());
	Reset();
}

// --------------------------------------------------------------------------
/** We use this method to get the information from the firmware binary file
    that is waiting to be accepted and programmed into a device.
    
    \retval true Bin file exists, has correct md5 and versions were obtained.
    \retval false Something went wrong. The file should not be used.
 */
UnitProgrammer::BinInfo UnitProgrammer::GetBinInfo(
	uint8_t byNodeId		//!< The hardware node id.
) const
{
	QString qsBinFile = GetFileName(byNodeId, "bin");
// For debug only
// 	if(m_qsMD5File.isEmpty())
// 		m_qsMD5File = QString("/cm-x270/nesis/updatelist");

	// Set invalid values.
	BinInfo bi;
	
	// We check Md5. However, we check this only if file with the MD5 list is 
	// given, which should be the Nesis case.
	// In the case of "Calibrate" and "Serial number" this might be skipped.
	if(m_qsMD5File.isEmpty()==false) {
		// Get the Md5 of bin file using the md5sum linux command.
		QByteArray baMd5 = common::GetMD5(qsBinFile);
		// We should have a valid md5 unless the bin file is missing.
		if(baMd5.isEmpty()) {
			return bi;
		}
			
		// Get the md5 from the summary md5 file.
		QFile fMd5(m_qsMD5File);
		if(fMd5.open(QIODevice::ReadOnly | QIODevice::Text)==false) {
			qWarning() << "Failed to open the md5 summary file" << fMd5.fileName();
			return bi;
		}
		
		// Read line by line and search for the correct device file name.
		QByteArray baLine = fMd5.readLine().simplified();
		QString qsSearchFile = QFileInfo(qsBinFile).fileName();
		while(baLine.isEmpty()==false) {
			QList<QByteArray> lsBa = baLine.split(' ');
			if(lsBa.count()==3) {
				// The first one is the file name and the last one is Md5
				if(QFileInfo(lsBa[0]).fileName()==qsSearchFile) {
					if(baMd5 == lsBa[2]) {
						bi.m_bMD5 = bi.m_bBin = true;
						break;
					}
					else {
						qDebug() << qsBinFile << ": MD5 check failed!";
					   qDebug() << "bin=" << baMd5 << "list=" << lsBa[2];
						return bi;
					}
				}
			}
			baLine = fMd5.readLine().simplified();
		}
		qDebug() << qsBinFile << "MD5 success.";
	}
	// MD5 check is not necessary
	else {
		bi.m_bMD5 = true; // Assume valid.
	}
	
	// Open the bin file and read some application values
	
	QFile fBin(qsBinFile);
	if(fBin.open(QIODevice::ReadOnly)==false) {
		qWarning() << "Failed to open the bin file" << fBin.fileName();
		return bi;
	}
	
	// Versions
	fBin.seek(APPLICATION_HEADER_ADDRESS+OFFSET_SOFTWARE_VERSION);
	fBin.read((char*)&bi.m_uiSV,  4);
	fBin.read((char*)&bi.m_uiRHV, 4);
	fBin.read((char*)&bi.m_uiSVN, 4);
	// qDebug() << "Versions (SW, RHW, SVN):" << bi.m_uiSV << bi.m_uiRHV << bi.m_uiSVN; 

	fBin.close();
	
	return bi;
}

// --------------------------------------------------------------------------

void UnitProgrammer::Reset()
{
	if(IsActive())
		m_fBin.close();
	m_pADM->ResetAPS();
}

// --------------------------------------------------------------------------

} // namespace can
