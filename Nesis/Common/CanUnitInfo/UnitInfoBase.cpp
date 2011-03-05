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

#include <QDebug>
#include <QTime>
#include <QApplication>
#include <QFile>
#include <QSettings>

#include "UnitInfoBase.h"

namespace can {

// ------------------------------------------------------------------------

UnitInfoBase::UnitInfoBase(
	uint8_t byNodeId,
	uint8_t byHwId,
	uint32_t uiSerial, 
	common::JulianDay jdDate
)
{
	m_byNodeId 			= byNodeId;
	m_byHardwareId 	= byHwId;
	m_jdDate   			= jdDate;
	m_uiSerial 			= uiSerial;
	m_byHWVersion  	= 0;
	m_bySWVersion  	= 0;
	m_uiVendorNumber 	= 0;
	m_uiSVN        	= 0;
	m_tLastSOL.start();
}

UnitInfoBase::UnitInfoBase(const UnitInfoBase& C)
{
	m_byNodeId 			= C.m_byNodeId;
	m_byHardwareId 	= C.m_byHardwareId;
	m_jdDate   			= C.m_jdDate;
	m_uiSerial 			= C.m_uiSerial;
	m_byHWVersion  	= C.m_byHWVersion;
	m_bySWVersion  	= C.m_bySWVersion;
	m_uiVendorNumber 	= C.m_uiVendorNumber;
	m_uiSVN 				= C.m_uiSVN;
	m_tLastSOL			= C.m_tLastSOL;
}

UnitInfoBase::~UnitInfoBase()
{
}

// ------------------------------------------------------------------------

bool UnitInfoBase::IsValid() const
{
	return 
		(m_uiSerial     != 0) &&
		(m_byHardwareId != 0) &&
		(m_jdDate.GetJulianDay() != 0) &&
		(m_byHWVersion  != 0) &&
		(m_bySWVersion  != 0) &&
		(m_uiSVN        != 0) &&
		(m_uiVendorNumber != 0);
}

// ------------------------------------------------------------------------
/** Keep waiting for requested time, but allow Qt message processing,
    check can timeouts, process any can outgoing messages. Any incomming
    can messages are processed by Qt signal/slot mechanism.
 */
void UnitInfoBase::Wait(
	can::AbstractAerospaceDataManager* pADM, //!< The manager object
	qint32 msToWait								  //!< Time to wait in ms
)
{
//	uint uiCount = 0;
	Q_ASSERT(pADM);
	QTime tiStopwatch;
	tiStopwatch.start();
	do {
//		uiCount++;
		qApp->processEvents();
		pADM->CheckTimeouts();
		pADM->ProcessOutgoingMessages();
	} while(tiStopwatch.elapsed() < msToWait);
//	qDebug() << "Wait loop exexcuted" << uiCount << "times.";
}

// ------------------------------------------------------------------------

void UnitInfoBase::SetVersion(uint16_t uiVersions)
{
	m_byHWVersion = uiVersions & 0xFF;
	m_bySWVersion = (uiVersions & 0xFF00) >> 8;
}
// ------------------------------------------------------------------------

QString UnitInfoBase::GetHardwareVersionString() const
{
	return QString("%1.%2").arg(m_byHWVersion>>4).arg(m_byHWVersion & 0x0f);
}

// ------------------------------------------------------------------------

QString UnitInfoBase::GetSoftwareVersionString() const
{
	return QString("%1.%2").arg(m_bySWVersion>>4).arg(m_bySWVersion & 0x0f);
}

// ------------------------------------------------------------------------
#if defined (USE_CAN_DUS_A)

bool UnitInfoBase::Upload(
	can::AbstractAerospaceDataManager* pADM, //! Low level implementation
	uint32_t uiUplDataId							  //! What are we downloading.
)
{
	// Wait up to 1 second for any pending upload to finish.
	int iCount = 0;
	bool bBusy;
	do {
		can::DUS_A::State eUpState = pADM->GetUploadState();
		qDebug() << "Upload state =" << ((int)eUpState);
		bBusy = (eUpState > can::DUS_A::sReady && eUpState < can::DUS_A::sSuccess);
		if(bBusy)
			Wait(pADM, 100);
	} while(bBusy && iCount++ < 20);

	if(bBusy) {
		qWarning() << "Upload service is busy.";
		return false;
	}

	// Ask the incomming buffer for number of requested messages
	uint8_t byMsgCount = m_vUploadBuffer.count();

	// OK, we can start upload
	pADM->ResetUpload();
	if (pADM->StartUploadService(GetNodeId(), byMsgCount, uiUplDataId)) {
		// Wait to finish the upload or timeout
		iCount = 0;
		can::DUS_A::State eUpState;
		do {
			// When a message arrives the StoreUploadMessage() member will be 
			// called implicitly.
			Wait(pADM, 100);
			eUpState = pADM->GetUploadState();
			bBusy = (eUpState > can::DUS_A::sReady && eUpState < can::DUS_A::sSuccess);
		} while(bBusy && iCount++ < 20);

		if(bBusy || eUpState != can::DUS_A::sSuccess) {
			qWarning() << "Upload transfer failed. State = " << eUpState;
			return false;
		}

		// The uploaded date should be in the buffer now.
		return true;
	}
	qWarning() << "Failed to start the upload service.";
	return false;
}

// ------------------------------------------------------------------------

bool UnitInfoBase::StoreUploadMessage(const can::MessageAerospace& msg)
{
	uint8_t uiIndex = msg.GetMessageCode()-1;
	if(uiIndex < m_vUploadBuffer.count()) {
		m_vUploadBuffer[uiIndex] = msg.GetData();
		return true;
	}

	qWarning() << "Upload index is out of range.";
	return false;
}

// ------------------------------------------------------------------------

bool UnitInfoBase::Upload(
	can::AbstractAerospaceDataManager* pADM,
	uint32_t uiUplDataId,
	QVector<float>& vData,
	int iSize
)
{
	if(iSize > 0)
		vData.resize(iSize);
		
	Q_ASSERT(vData.isEmpty()==false);
	Q_ASSERT(m_vUploadBuffer.isEmpty());
	
	m_vUploadBuffer.resize(vData.count());
	if(Upload(pADM, uiUplDataId)) {
		for(int i=0; i<vData.count(); i++)
			vData[i] = m_vUploadBuffer[i].f;

		m_vUploadBuffer.clear();
		return true;
	}
	m_vUploadBuffer.clear();
	return false;
}
// ------------------------------------------------------------------------

#endif

// ------------------------------------------------------------------------

#if defined (USE_CAN_DDS_A)

bool UnitInfoBase::Download(
	can::AbstractAerospaceDataManager* pADM, 
	uint32_t uiDataId
)
{
/*	if(pADM->IsDownloadActive() || !m_vDownloadBuffer.isEmpty()) {
		qWarning() << "Download is active. Please try again.";
		return false;
	}

	// Load the buffer (derived class must implement this call)
	LoadDownloadBuffer(uiDataId);*/

	// Start the download.
	qDebug() << "Try to download id=" << uiDataId << "count" << m_vDownloadBuffer.count();
	pADM->ResetDownload();
	if(pADM->StartDownloadService(
		GetNodeId(),
		m_vDownloadBuffer.count(),
		uiDataId) == false) {
		qWarning() << "Could not start the download.";
	  	return false;
	}
	
	// Error, this could be endless loop.
	// We count on the timeout to But the timeout should 
	qDebug() << "Waiting for download to start.";
	// Wait until active
	do {
		Wait(pADM, 1);
	} while(!pADM->IsDownloadActive());

	// Loop while active
	qDebug() << "Downloading.";
	do {
		Wait(pADM, 5);
	} while(pADM->IsDownloadActive());
	qDebug() << "Complete.";

	// Reset the data and return the success state.
	m_vDownloadBuffer.clear();
	return (pADM->GetDownloadState() == can::DDS_A::sSuccess);
}
// ------------------------------------------------------------------------

bool UnitInfoBase::Download(
	can::AbstractAerospaceDataManager* pADM,
	uint32_t uiDwnDataId, 
	const QVector<float>& vData
)
{
	if(pADM->IsDownloadActive() || !m_vDownloadBuffer.isEmpty()) {
		qWarning() << "Download is active. Please try again.";
		return false;
	}

	m_vDownloadBuffer.resize(vData.count());
	for(int i=0; i< vData.count(); i++) {
		m_vDownloadBuffer[i].f = vData[i];
	}
	m_eDataType = can::dtFloat;
	
	return Download(pADM, uiDwnDataId);
}

// ------------------------------------------------------------------------

bool UnitInfoBase::GetDownloadData(
	uint8_t byIndex, 
	can::DataType& eDataType, 
	can::Register& rData
)
{
	if(byIndex <= m_vDownloadBuffer.count() && byIndex > 0) {
		eDataType = m_eDataType;
		rData     = m_vDownloadBuffer[byIndex-1];
		return true;
	}
	return false;
}

#endif 
// ------------------------------------------------------------------------
/** This is the base class implementation. It simply prints the response
    into the debug stream. Derived classes should implement their own
    response.
 */
void UnitInfoBase::ProcessModuleConfigResponse(
	const can::MessageAerospace& msg	//!< The response message.
)
{
	// Display message if responce is not MCS_SUCCESS. This
	// does not mean an error. 
	if(msg.GetData().ui32 != MCS_SUCCESS) {
		qDebug() << "Response MCS: Sender =" << msg.GetSender()
					<< "Config Id ="            << msg.GetMessageCode()
					<< "Return ="               << msg.GetData().ui32;
	}
}

// ------------------------------------------------------------------------

void UnitInfoBase::ProcessModuleInfoResponse(const can::MessageAerospace& msg)
{
	uint8_t byQuestion = msg.GetMessageCode();

	switch(byQuestion) {
	case MIS_SERIAL_NUMBER:
		SetSerial(msg.GetData().ui32);
		qDebug() << "Serial received from device =" << GetSerial();
		break;
	case MIS_SERIAL_DATE:
		SetDate(common::JulianDay(msg.GetData().ui32));
		break;
	case MIS_VERSION:
		SetVersion(msg.GetData().ui32 & 0xFFFF);
		break;
	case MIS_VENDOR_NUMBER:
		SetVendorNumber(msg.GetData().ui32);
		break;
	case MIS_SVN_REVISION:
		SetSVN(msg.GetData().ui32);
		break;
	default:
		qWarning() << "UnitInfoBase: Unprocessed MIS Response" << (int)byQuestion;
		break;
	}
}
// ------------------------------------------------------------------------

int UnitInfoBase::GetTimeSinceSignOfLife() const
{
	return m_tLastSOL.elapsed();
}

// ------------------------------------------------------------------------

void UnitInfoBase::UpdateSignOfLife(AbstractAerospaceDataManager *pADM)
{
	m_tLastSOL.start();
	Q_ASSERT(m_uiSerial != 0);
	Q_ASSERT(m_byHardwareId != 0);
	
	if(!m_jdDate.GetJulianDay())
		pADM->RequestModuleInformation(GetNodeId(), MIS_SERIAL_DATE);
	if(!m_bySWVersion || !m_byHWVersion)
		pADM->RequestModuleInformation(GetNodeId(), MIS_VERSION);
	if(!m_uiVendorNumber)
		pADM->RequestModuleInformation(GetNodeId(), MIS_VENDOR_NUMBER);
	if(!m_uiSVN)
		pADM->RequestModuleInformation(GetNodeId(), MIS_SVN_REVISION);
}

// ------------------------------------------------------------------------

} // namespace
