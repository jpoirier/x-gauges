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

#include <QDebug>
#include <QTimer>

#include "KanardiaHardwareUnits.h"
#include "ApplicationDefines.h"
#include "PacketComm/packet.h"
#include "PacketComm/AbstractEIAPackage.h"
#include "CRC-8.h"
#include "Settings.h"
#include "NesisAerospaceDataManager.h"

#include "NesisUnitCAN.h"

// --------------------------------------------------------------------

NesisUnitCAN* NesisUnitCAN::s_pInstance = NULL;

NesisUnitCAN::NesisUnitCAN(QObject *pParent)
 : QObject(pParent)
{
	m_pPort = NULL;
	m_pbFlagDDS_A = NULL;
	m_pbFlagDUS_A = NULL;
	m_pbFlagFPS_A = NULL;
	m_pbFlagAPS_A = NULL;
	
	Settings* pS = Settings::GetInstance();

	m_programmer.SetBinPath(pS->GetFirmwarePath());
	m_programmer.SetMD5File(pS->GetMD5File());
	m_programmer.SetBinExtension("fw");
	qDebug() << "NesisUnitCAN constructor: firmware path =" << pS->GetFirmwarePath();

	m_pTimerDDS_A = new QTimer(this);
	m_pTimerDDS_A->setSingleShot(true);
	m_pTimerDUS_A = new QTimer(this);
	m_pTimerDUS_A->setSingleShot(true);
	m_pTimerFPS_A = new QTimer(this);
	m_pTimerFPS_A->setSingleShot(true);
	m_pTimerAPS_A = new QTimer(this);
	m_pTimerAPS_A->setSingleShot(true);

	connect(m_pTimerDDS_A, SIGNAL(timeout()), this, SLOT(OnTimerDDS_A()));
	connect(m_pTimerDUS_A, SIGNAL(timeout()), this, SLOT(OnTimerDUS_A()));
	connect(m_pTimerFPS_A, SIGNAL(timeout()), this, SLOT(OnTimerFPS_A()));
	connect(m_pTimerAPS_A, SIGNAL(timeout()), this, SLOT(OnTimerAPS_A()));
}

NesisUnitCAN::~NesisUnitCAN()
{
}

// --------------------------------------------------------------------

bool NesisUnitCAN::PostMessage(const can::MessageAerospace& msg) const
{
	Q_ASSERT(m_pPort);

	#if defined(QT_ARCH_ARM)
		common::CRC8 crc;

		uint8_t abyBuf[14];
		abyBuf[0] = 0x78;
		memcpy(&abyBuf[1], &msg.GetIdentifier().ui32, 4);
		memcpy(&abyBuf[5], &msg.GetRegisterA().ui32, 4);
		memcpy(&abyBuf[9], &msg.GetData().ui32, 4);

		crc.Update(&abyBuf[1], 12);
		abyBuf[13] = crc.Get();

		return m_pPort->Write((const char*)abyBuf, 14);
	#else
		packetcomm::PacketCAN pkt;
		pkt.m_uiIdentifier = msg.GetIdentifier().ui32;
		pkt.m_uiHeader = msg.GetRegisterA().ui32;
		pkt.m_uiData   = msg.GetData().ui32;

		QByteArray ba = packetcomm::AbstractEIAPackage::Create(EIA_CAN_MESSAGE, (uint8_t*)&pkt, sizeof(pkt));
		return m_pPort->AbstractPort::Write(ba);
	#endif
}

// --------------------------------------------------------------------

uint8_t NesisUnitCAN::GetNodeId() const
{
	return Settings::GetInstance()->IsPrimary() ? KHU_NESIS : KHU_NESIS_SECONDARY;
}

// --------------------------------------------------------------------

uint8_t NesisUnitCAN::GetSWVersion() const
{
	return SOFTWARE_VERSION;
}

// --------------------------------------------------------------------

uint8_t NesisUnitCAN::GetHWVersion() const
{
	return 0; //HARDWARE_VERSION;
}

// --------------------------------------------------------------------

void NesisUnitCAN::SetADownloadTimeout(
	uint ms,
	bool* pbFlag
)
{
	m_pbFlagDDS_A = pbFlag;
	if(ms == 0)
		m_pTimerDDS_A->stop();
	else
		m_pTimerDDS_A->start(ms);
}

// --------------------------------------------------------------------

bool NesisUnitCAN::GetDownloadData(
	uint32_t /*uiDataId*/,
	uint8_t byIndex,
	can::DataType& eDataType,
	can::Register& rData
) const
{
	can::UnitInfoBase* pI = m_conUnitInfo.Find(
		m_conUnitInfo.GetAerospaceDataManager()->GetDownloadReceiver()
	);

	if(pI)
		return pI->GetDownloadData(byIndex, eDataType, rData);
	else
		return false;
}

// --------------------------------------------------------------------

void NesisUnitCAN::OnTimerDDS_A()
{
	qDebug() << "Timeout DDS A";
	Q_ASSERT(m_pbFlagDDS_A);
	*m_pbFlagDDS_A = true;
}

// --------------------------------------------------------------------

void NesisUnitCAN::SetAFlashProgrammingTimeout(uint ms, bool* pbFlag)
{
	m_pbFlagFPS_A = pbFlag;
	if(ms == 0)
		m_pTimerFPS_A->stop();
	else
		m_pTimerFPS_A->start(ms);
}

// --------------------------------------------------------------------

void NesisUnitCAN::OnTimerFPS_A()
{
	qDebug() << "Timeout FPS A";
	Q_ASSERT(m_pbFlagFPS_A);
	*m_pbFlagFPS_A = true;
}

// --------------------------------------------------------------------

void NesisUnitCAN::SetAApplicationProgrammingTimeout(uint ms, bool* pbFlag)
{
	m_pbFlagAPS_A = pbFlag;
	if(ms == 0)
		m_pTimerAPS_A->stop();
	else
		m_pTimerAPS_A->start(ms);
}

// --------------------------------------------------------------------

void NesisUnitCAN::OnTimerAPS_A()
{
	qDebug() << "Timeout APS A";
	Q_ASSERT(m_pbFlagAPS_A);
	*m_pbFlagAPS_A = true;
}

// --------------------------------------------------------------------

int32_t NesisUnitCAN::GetAPSPageCount(
	uint8_t byNodeId
)
{
// 	qDebug() << "CalibrateCanUnit::GetAPSPageCount";
	return m_programmer.Start(byNodeId);
}

// --------------------------------------------------------------------

uint32_t* NesisUnitCAN::GetAPSPageAddress(
	uint32_t uiPage
)
{
// 	qDebug() << "CalibrateCanUnit::GetAPSPageAddress";
	return m_programmer.GetPage(uiPage);
}

// --------------------------------------------------------------------

void NesisUnitCAN::OnTimerDUS_A()
{
	qDebug() << "Timeout DUS A";
	Q_ASSERT(m_pbFlagDUS_A);
	*m_pbFlagDUS_A = true;
}

// --------------------------------------------------------------------

void NesisUnitCAN::SetAUploadTimeout(
	uint32_t ms,
	bool* pbFlag
)
{
	m_pbFlagDUS_A = pbFlag;
	if(ms == 0)
		m_pTimerDUS_A->stop();
	else
		m_pTimerDUS_A->start(ms);
}

// --------------------------------------------------------------------

bool NesisUnitCAN::StoreUploadMessage(
	const can::MessageAerospace& msg
)
{
	// Who sent the message?
	can::UnitInfoBase* pI = m_conUnitInfo.Find(msg.GetSender());
	if(pI)
		return pI->StoreUploadMessage(msg);
	else {
		qDebug() << "Upload message addressed to non-existing object"
					<< msg.GetReceiver();
		return false;
	}
}

// --------------------------------------------------------------------

void NesisUnitCAN::ProcessModuleInfoResponse(
	const can::MessageAerospace& msg
)
{
	m_conUnitInfo.ProcessModuleInfoResponse(msg);
}

// --------------------------------------------------------------------

void NesisUnitCAN::ProcessModuleConfigResponse(
	const can::MessageAerospace& msg
)
{
	m_conUnitInfo.ProcessModuleConfigResponse(msg);
/*	// Which unit sent the message?
	can::UnitInfoBase* pI = m_conUnitInfo.Find(msg.GetSender());
	// Let the unit info handle the message.
	if(pI)
		pI->ProcessModuleConfigResponse(msg);*/
}

// --------------------------------------------------------------------

void NesisUnitCAN::ProgramMabu()
{
	// start the process. iP tells number of pages to send.
	int iP = m_programmer.Start(KHU_MABU, true);
	qDebug("Programming Mabu device - %i pages to download", iP);
	
	if(iP == 0) 
		return;
			
	// Build the bin string.
	QByteArray baBin;
	baBin.append(QByteArray::fromRawData("******MABU0", 11));
	// This is not very clever, but will work as long iP > 100 && < 1000
	baBin.append(QByteArray::number(iP));
			
	// The string gets all pages in one pass.
	for (int i=0; i<iP; i++)
		baBin.append(QByteArray::fromRawData((char*)m_programmer.GetPage(i), 512));

	// Add 2048 bytes of something (not needed in theoriy, but it does
	// not work without them). 
	for (int i=0; i<2048; i++)
		baBin.append(0xa1);
			
	qDebug() << "Start writing to SPI port.";
	emit ProgramMabu(true);
			
	// Write this to the port.
	GetPort()->Write(baBin.constData(), baBin.count());
	GetUnitInfoContainer().Wait(3000);

	qDebug() << "Finished writing to SPI port.";
	emit ProgramMabu(false);
	m_programmer.Reset();
}

// --------------------------------------------------------------------

void NesisUnitCAN::ProcessSignOfLife(const can::MessageAerospace& msg)
{
	// Well, here it is the responsibility of the Unit Container to
	// implement the details. Hence we simply forward the message to the
	// container object.
	GetUnitInfoContainer().ProcessSignOfLife(msg);
}

// --------------------------------------------------------------------
