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
#include <QMutexLocker>

#include "CommonDefs.h"
#include "MathEx.h"
#include "NesisUnitCAN.h"
#include "PacketComm/packet.h"
#include "Matrix/Vector.h"
#include "Map/FlatEarth.h"
#include "Settings.h"
#include "WidgetMessage.h"

#include "NesisAerospaceDataManager.h"


//-----------------------------------------------------------------------------

#define SPI_MESSAGE_SIZE	14

//! The lowest airspeed that will be shown [m/s]. Lower values will be set to zero.
#define AIRSPEED_LOW_LIMIT			6
//! The lowest groundspeed that will be shown [m/s]. Lower values will be set to zero.
#define GROUNDSPEED_LOW_LIMIT		3

//-----------------------------------------------------------------------------

// Singleton object constructor
// std::auto_ptr<NesisAerospaceDataManager> NesisAerospaceDataManager::m_apNADM;
NesisAerospaceDataManager* NesisAerospaceDataManager::m_apNADM = NULL;

//-----------------------------------------------------------------------------

NesisAerospaceDataManager::NesisAerospaceDataManager()
{
	m_pUnit = NesisUnitCAN::GetInstance();

	// Flight State & Air Data struct.
	m_pFSAD 	= &m_FSAD;
	// Controls Data struct.
	m_pFCD 	= &m_FCD;
	// Engine Fuel Supply Data struct.
	m_pEFSD 	= &m_EFSD;
	// Power Transmition Data struct.
	m_pPTD 	= &m_PTD;
	// Hydraulic Data struct.
	m_pHD 	= &m_HD;
	// Electric Data struct.
	m_pED 	= &m_ED;
	// Nav(igation) Data struct.
	m_pND 	= &m_ND;
	// Landing Gear Data struct.
	m_pLGD 	= &m_LGD;
	// Miscellaneous Data struct.
	m_pMD 	= &m_MD;

	// Kanardia specific! DAQU data for single engine.
	m_pKD 	= &m_KD;

	// **********************************

	m_FSAD.fBaroCorrection = 1013.25f;

//	// Fake values
//	m_FSAD.fBodyPitchAngle = 0.02;
//	m_FSAD.fBodyRollAngle = 0.12;
//	m_ND.fGPSGroundSpeed  = 57.2;
//	m_FSAD.fHeadingAngle = common::Rad(287.0f);
//	m_ND.fGPSTrueTrack = common::Rad(278.0f);
////	m_ND.fGPSAircraftLongitude = 0.0;
////	m_ND.fGPSAircraftLatitude = 0.0;
//	m_ND.uiGPSModeOfOperation = 6;
//	m_FSAD.fTrueAirspeed  = 61.3;
//	m_FSAD.fIndicatedAirspeed  = 59.4;
// 	m_FSAD.fAltitudeRate = 1.3;
// 	m_FSAD.fStandardAltitude = 830.0;
// 	m_FSAD.fBaroCorrectedAltitude = 852.0;
// 	m_FSAD.fWindSpeed = 8.3;
// 	m_FSAD.fWindDirection = common::Rad(340.0f);
// 	m_FSAD.fStaticPressure = 970.0;
//
//	m_EFSD.afEngineN1[0] = 4032;
// 	m_EFSD.afFuelSystemPressure[0] = 0.22;
// 	m_EFSD.afEngineOilPressure[0] = 4.2;
// 	m_EFSD.afEngineOilTemperature[0] = 80;
// 	m_EFSD.afEngineCoolantTemperature[0] = 92;
// 	m_EFSD.afEngineManifoldPressure[0] = 0.87;
//
// 	m_EFSD.afEngineFuelFlowRate[0] = 18.0;
// 	m_EFSD.afFuelTankQuantity[0] = 33.7;
// 	m_KD.fAverageFuelFlow = 14.35;
// 	m_KD.fEngineTotalTime = 854.75f;
//
// 	m_ED.afDCSystemVoltage[0] = 13.8;
// 	m_ED.afDCSystemCurrent[0] = 3.5;
//
// 	m_KD.afCHT[0] = 115;
// 	m_KD.afCHT[1] = 65;
////
// 	m_KD.afEGT[0] = 780;
// 	m_KD.afEGT[1] = 675;
// 	m_KD.afEGT[2] = 688;
// 	m_KD.afEGT[3] = 785;
////
// 	m_KD.abyBackupBatteryStatus[0] = 3;
// 	m_KD.abyBackupBatteryStatus[1] = 121;
// 	m_KD.abyBackupBatteryStatus[2] = 28;
// 	m_KD.abyBackupBatteryStatus[3] = 34;
//
// 	m_KD.fCarburetorTemp = 1.1f;

	// Debug
	// Friedrichshafen
//	m_ptDefaultPos = QPointF(common::Rad(9.76),common::Rad(47.64)); 
//	m_ptDefaultPos = QPointF(common::Rad(13.76),common::Rad(44.64)); 
	
//	m_ptDefaultPos = QPointF(common::Rad(53.97),common::Rad(26.53)); // Kish Island airshow Iran
//	m_ptDefaultPos = QPointF(common::Rad(45.07),common::Rad(37.55)); // Urmia, Iran (TODO Change name! Reza Iyeh into Urmia)
//	m_ptDefaultPos = QPointF(common::Rad(11.2),common::Rad(46.7)); // Lesce
//	m_ptDefaultPos = QPointF(common::Rad(15.2775),common::Rad(46.2340)); // Celje, Kanardia
	m_ptDefaultPos = QPointF(common::Rad(15.60),common::Rad(46.30)); // Maribor
//	m_ptDefaultPos = QPointF(common::Rad(-8.03),common::Rad(39.21));// Ponte de Sôr (N039º12´25``  W008º031`30``)
//	QPointF ptFix(common::Rad(14.1),common::Rad(49.82)); // Pribram
//	m_ptDefaultPos = QPointF (common::Rad(18.5),common::Rad(54.324)); // Poland

//	QPointF ptFix(common::Rad(14.1),common::Rad(45.82));
//	QPointF ptFix(common::Rad(14.1),common::Rad(49.72));
//	QPointF ptFix(common::Rad(14.178),common::Rad(49.9744));

}
//-----------------------------------------------------------------------------

NesisAerospaceDataManager* NesisAerospaceDataManager::GetInstance()
{
	if (m_apNADM == NULL)
		m_apNADM = new NesisAerospaceDataManager();
	return m_apNADM;
}


//-----------------------------------------------------------------------------

NesisAerospaceDataManager::~NesisAerospaceDataManager()
{
	qDebug("NADM exit");
}

//-----------------------------------------------------------------------------

void NesisAerospaceDataManager::Process(
	const unsigned char* pucBuffer, //!< Buffer with the data received.
	int iCount							//!< Number of bytes in the buffer.
)
{
	// read all the data from the port
	std::copy(pucBuffer, pucBuffer+iCount, std::back_inserter(m_dqBuffer));

	// Process the data in the buffer (deque).
	// Repeat the call as long as the ProcessBuffer keeps returning true.
	while(ProcessBuffer()) ;
}

// ---------------------------------------------------------------------------

bool NesisAerospaceDataManager::ProcessBuffer()
{
	// end marker
	const std::deque<uint8_t>::iterator e = m_dqBuffer.end();
	// working iterators
	std::deque<uint8_t>::iterator b, f;

	b = f = m_dqBuffer.begin();

	// try to locate first byte
	while (*f != 0x78) {
		if ( f==e )
			break;
		f++;
	}

	// If the first byte is not found, the whole content can be ignored
	if(f==e) {
// 		Dump(m_dqBuffer.begin(), m_dqBuffer.end());
		m_dqBuffer.clear();
		return false; // stop processing
	}

	// OK, the first byte was found.
	// First dump all bytes before the first byte, if any
	if(f!=b) {
// 		Dump(b,f);
		b = m_dqBuffer.erase(b,f);
	}

	// If we have got whole packet.
	if(m_dqBuffer.size() < SPI_MESSAGE_SIZE) {
		// keep this head and wait for more bytes to come from serial port.
		return false;
	}

 	common::VectorB vbData(
 			b+1,  // first byte after command byte.
 			e-1   // stop byte.
 	);

 	uint32_t auiM[3];
	memcpy(auiM, &vbData[0], 12);

// 	qDebug("Got package: i=%d", auiM[0] >> 16);

	ProcessCanMessage(auiM[0],auiM[1],auiM[2]);

	m_dqBuffer.erase(b,b+SPI_MESSAGE_SIZE);

	if(m_dqBuffer.size() < SPI_MESSAGE_SIZE)
		return false;
	else
		return true; // try once, but now locate another head
}
//-----------------------------------------------------------------------------

void NesisAerospaceDataManager::ProcessCanMessage(uint rI, uint rA, uint rB)
{
	m_mutex.lock();
	ProcessIncommingMessage(can::Message(rI, rA, rB));
	m_mutex.unlock();
}

//-----------------------------------------------------------------------------

/*! Return current position.
 */
QPointF NesisAerospaceDataManager::GetPosition() const
{
 	QMutexLocker ml(&m_mutex);
// 	QPointF ptCoor(m_pND->fGPSAircraftLongitude, m_pND->fGPSAircraftLatitude);
 	QPointF ptCoor(m_pND->fINSAircraftLongitude, m_pND->fINSAircraftLatitude);

 	if(ptCoor.isNull() || ISNAN(ptCoor.x()) || ISNAN(ptCoor.y())) {
 		return m_ptDefaultPos;
 	}
	return ptCoor;
}

//-----------------------------------------------------------------------------

float NesisAerospaceDataManager::GetIAS() const
{
	float fAS = m_FSAD.fIndicatedAirspeed;
	if(fAS < AIRSPEED_LOW_LIMIT)
		return 0.0;
	return fAS;
}

//-----------------------------------------------------------------------------

float NesisAerospaceDataManager::GetTAS() const
{
	float fAS = m_FSAD.fTrueAirspeed;
	if(fAS < AIRSPEED_LOW_LIMIT)
		return 0.0;
	return fAS;
}

//-----------------------------------------------------------------------------
// TODO Ground speed should be from from KF and not from GPS.
float NesisAerospaceDataManager::GetGroundSpeed() const
{
	float fGS = m_ND.fGPSGroundSpeed;
	if(fGS < GROUNDSPEED_LOW_LIMIT)
		return 0.0;
	return fGS;
}

//-----------------------------------------------------------------------------

float NesisAerospaceDataManager::GetBallSlip() const
{
	QMutexLocker ml(&m_mutex);
	V3f a;
	a[0] = m_FSAD.fBodyLongitudinalAcceleration;
	a[1] = m_FSAD.fBodyLateralAcceleration;
	a[2] = m_FSAD.fBodyNormalAcceleration;

	float fN = a.GetNorm();
	if(fN > 0.0f)
		return a[1]/fN;
	return 0.0f;
}

//-----------------------------------------------------------------------------
// TODO Move the code into Airu

float NesisAerospaceDataManager::GetTracking() const
{
	if(m_ND.fGPSGroundSpeed > 3.0f)
 		return m_ND.fGPSTrueTrack;
 	else
		return 0.0;
/*	float fTAS = GetTAS();
	float fYaw = GetYaw();

	if(fTAS <= 0.0f)
		return fYaw;

	float sy, cy;

// XXX:
//	sincosf(fYaw, &sy, &cy);

	// Wind plus current heading
	float fvN, fvE;
	GetWindNE(fvN, fvE);
	fvN += fTAS * cy;
	fvE += fTAS * sy;

	return atan2(fvE, fvN);*/
}
// -----------------------------------------------------------------------

void NesisAerospaceDataManager::GetWindNE(float& fWindN, float& fWindE) const
{
/*	float s,c;

// XXX:
//	sincosf(GetWindDirection(), &s, &c);
	float w = -GetWindSpeed();
	fWindN = w*c;
	fWindE = w*s;*/
	fWindN = 1.0;
	fWindE = 1.0;
}

// -----------------------------------------------------------------------

QPointF NesisAerospaceDataManager::PredictPosition(float fTime_s)
{
	QPointF ptO = GetPosition();
	float fT    = GetTracking();
	float fGS   = GetGroundSpeed();

	map::FlatEarth fe(ptO);
	return fe.GetCoordinate(fT, fGS * fTime_s);
}

//-----------------------------------------------------------------------------

float NesisAerospaceDataManager::GetTotalFuelLevel() const
{
	// TODO Sum over all tanks.
	return m_EFSD.afFuelTankQuantity[0];
}

//-----------------------------------------------------------------------------

QDateTime NesisAerospaceDataManager::GetDateTimeUTC() const
{
	// We can return valid time only if we have a valid and fresh
	// GPS information.
	// TODO Check time of last GPS info here
	//qDebug() << "GPS sec =" << m_KD.fTimeSecGPS;
	QDateTime dtGPS(
		QDate::fromJulianDay(m_KD.uiJulianDayGPS),
		QTime().addSecs(m_KD.fTimeSecGPS),
		Qt::UTC
	);
//	qDebug() << "Date = " << dtGPS.toString();
	return dtGPS;
}

//-----------------------------------------------------------------------------

void NesisAerospaceDataManager::ProcessIncommingMessage(
	const can::MessageAerospace& msg
)
{
	// Check floating point arguments for nan or inf
	#if !defined(QT_NO_DEBUG)
	if(msg.GetDataType() == can::dtFloat) {
		if(isnan(msg.GetData().f))
			qWarning() << "NaN in msg id" << msg.GetId()
			           << "from node" << msg.GetSender();
		else if(isinf(msg.GetData().f))
			qWarning() << "Inf in msg id" << msg.GetId()
			           << "from node" << msg.GetSender();
	}
	#endif
	AbstractAerospaceDataManager::ProcessIncommingMessage(msg);
}

//-----------------------------------------------------------------------------

void NesisAerospaceDataManager::HandleEmergencyEventData(const can::MessageAerospace& msg)
{
	// This is bootloader error message. The node wants to get the new programm.
	if(msg.GetId()==can::idEE_BootloadError) {
		qDebug("Boot error request from node %i.", msg.GetSender());
		// SPI version - the service code must be 1
		if (msg.GetServiceCode() == 1) {
			qDebug() << "(SPI) Bootloader version =" << (int)msg.GetDataType()
			         << "Hardware version =" << msg.GetMessageCode();
			
			// Ask NesisUnitCAN to program mabu
			if(msg.GetSender() == KHU_MABU) {
				dynamic_cast<NesisUnitCAN*>(m_pUnit)->ProgramMabu();
			}
			return;
		}
		//	Secondary nesis can't handle the update request of the firmware.
		// This is necessary to avoid response conflicts.
		if(Settings::GetInstance()->IsPrimary()) {
			qDebug() << "(CAN) Bootloader version =" << (int)msg.GetDataType()
			         << "Hardware version =" << msg.GetMessageCode();
			AbstractAerospaceDataManager::HandleEmergencyEventData(msg);
		}
	}
	// Other id's
	else {
		AbstractAerospaceDataManager::HandleEmergencyEventData(msg);
	}
}

//-----------------------------------------------------------------------------
/** Here we wait for the GPS status message in order to remember the
	 time it was received. This may
 */
// void NesisAerospaceDataManager::HandleNormalOperationData(
// 	const MessageAerospace& msg
// )
// {
// 	//
// }

//-----------------------------------------------------------------------------


float NesisAerospaceDataManager::GetEndurance() const
{
	float fFlow = GetAverageFuelFlow();

	if (fFlow >= 5.0) {
		return GetTotalFuelLevel() / fFlow * 3600.0;
	}
	else {
		return -1000.0;
	}
}

//-----------------------------------------------------------------------------

float NesisAerospaceDataManager::GetRange() const
{
	float fSpeed = GetGroundSpeed();
	float fE = GetEndurance();

	if (fSpeed >= 20.0 && fE >= 0.0) {
		return fE * fSpeed;
	}
	else {
		return -1000.0;
	}
}

//-----------------------------------------------------------------------------

float NesisAerospaceDataManager::GetCarburretorTemperature() const
{
	return m_KD.fCarburetorTemp;
}

//-----------------------------------------------------------------------------

float NesisAerospaceDataManager::GetOutsideAirTemperature() const 
{ 
	return m_FSAD.fOutsideAirTemperature; 
}

//-----------------------------------------------------------------------------

float NesisAerospaceDataManager::GetMaxCHT() const
{
	float fMax = GetCHT(0);
	for(int i=1; i<m_iCHTCount; i++)
		fMax = qMax(fMax, GetCHT(i));
		
	return fMax;
}

//-----------------------------------------------------------------------------

float NesisAerospaceDataManager::GetMaxEGT() const
{
	float fMax = GetEGT(0);
	for(int i=1; i<m_iEGTCount; i++)
		fMax = qMax(fMax, GetEGT(i));
		
	return fMax;
}

//-----------------------------------------------------------------------------

