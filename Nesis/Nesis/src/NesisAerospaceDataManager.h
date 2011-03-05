#ifndef NESIS_AEROSPACE_DATA_MANAGER_H
#define NESIS_AEROSPACE_DATA_MANAGER_H

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

#include <QMutex>
#include <QPoint>
#include <QDateTime>

#include <deque>

#include "Route/Route.h"
#include "CanAerospace/AbstractAerospaceDataManager.h"
#include "PacketComm/AbstractProcessor.h"
#include "JulianDay.h"

//-----------------------------------------------------------------------------

//! Define RPM treshold
#define RPM_TRESHOLD						500
//! The indicated speed treshold [m/s]
#define IAS_TRESHOLD						12


//-----------------------------------------------------------------------------
/** \brief A class that handles ALL CANaerospace messages and services used in Nesis.
 */

class NesisAerospaceDataManager : public can::AbstractAerospaceDataManager,
											 public packetcomm::AbstractProcessor
{
friend class WidgetMain;
protected:
	//! Constructor
	NesisAerospaceDataManager();

public:
	//! Destructor
	virtual ~NesisAerospaceDataManager();

	//! We allow only one instance of this class.
	static NesisAerospaceDataManager* GetInstance();

	//! Pure virtual request data processing.
	virtual void Process(
		const unsigned char* pucBuff, //!< Buffer with the data received.
		int iCount							//!< Number of bytes in the buffer.
	);
	//! Process incomming message.
	void ProcessCanMessage(uint rI, uint rA, uint rB);

	// ACCESS member functions

	// Attitude

	//! Get roll angle. (based on Kalman filter). [rad]
	float GetRoll() const
	{ return m_FSAD.fBodyRollAngle; }
	//! Get pitch angle. (based on Kalman filter). [rad]
	float GetPitch() const
	{ return m_FSAD.fBodyPitchAngle; }
	//! Get heading angle - yaw (from Airu - based on kalman filter. [rad]
	float GetYaw() const
	{ return m_FSAD.fHeadingAngle; }

	//! Get heading (yaw) rate [rad/s]
	float GetYawRate() const
	{ return m_FSAD.fBodyYawRate; }

	// Ball - slip angle (based on accelometers)

	//! Get slip angle [rad] (angle between ay and |a|)
	float GetBallSlip() const;

	// Position and wind

	//! Get current position.
	QPointF GetPosition() const;
	//! Get tracking course (direction in radians).
	float GetTracking() const;
	//! Get prediction coordinate t seconds ahead (wgs84, radians).
 	QPointF PredictPosition(float fTime_s);

	//! Get windspeed [m/s]
	float GetWindSpeed() const
  	{ return m_FSAD.fWindSpeed; }
	//! Get wind direction [rad]
 	float GetWindDirection() const
	{ return m_FSAD.fWindDirection; }
//	{ return 0.0; }
 	//! Get north and east wind component
	void GetWindNE(float& fWindN, float& fWindE) const;

	// Velocities and altitudes

	//! Get indicated airspeed [m/s].
	float GetIAS() const;
	//! Get true airspeed [m/s].
	float GetTAS() const;
	//! Get ground speed [m/s].
 	float GetGroundSpeed() const;
	//! Return outside air temperature. 
	float GetOutsideAirTemperature() const;
	//! Return true if instrument is flying.
	bool IsFlying() const
	{ return GetIAS() > IAS_TRESHOLD; }

	//! Get variometer (altitude rate)
	float GetVario() const
	{ return m_FSAD.fAltitudeRate; }

	//! Get current altitude [m].
	float GetAltitude() const
	{ return m_FSAD.fBaroCorrectedAltitude; }
	//! Get current QNH value [hPa]
	float GetQNH() const
	{ return m_FSAD.fBaroCorrection; }

	//! Get current static pressure
	float GetStaticPressure() const
	{ return  m_FSAD.fStaticPressure; }

	// GPS

	//! Get current satellite count - number of currently visible GPS sats.
	unsigned int GetSatelliteCount() const
	{ return  (m_ND.uiGPSModeOfOperation & 0xFF); }
	//! Get current GPS position fix status.
	unsigned int GetGPSFix() const
	{ return ((m_ND.uiGPSModeOfOperation >> 16) & 0xFF); }

	// Time and date

	//! Get current date and time.
	QDateTime GetDateTimeUTC() const;

	// Fuel
	//! Get current fuel flow [l/h]
	float GetFuelFlow() const
	{ return m_EFSD.afEngineFuelFlowRate[0]; }
	//! Get average fuel flow [l/h].
	float GetAverageFuelFlow() const
	{ return m_KD.fAverageFuelFlow; }
	//! Get fuel pressure [hPa]
	float GetFuelPressure() const
	{ return m_EFSD.afFuelSystemPressure[0]; }
// 	{ return 0.0; }

	//! Get fuel level [l] - total fuel level.
	float GetTotalFuelLevel() const;

	//! Get or compute endurance. [s]
	float GetEndurance() const;
	//! Get or compute range. [m]
	float GetRange() const;

	// Engine
	//! Set the EGT count.
	void SetEGTCount(int iEGTCount)
	{ m_iEGTCount = iEGTCount; }
	//! Set the CHT count.
	void SetCHTCount(int iCHTCount)
	{ m_iCHTCount = iCHTCount; }
	
	//! Get manifold pressure [hPa].
	float GetManifoldPressure() const
	{ return m_EFSD.afEngineManifoldPressure[0]; }

	//! Get temperature of cylinder \a uiCylinder.
	float GetCHT(int uiCylinder) const
	{ return m_KD.afCHT[uiCylinder]; }
	//! Get the maximal CHT temperature
	float GetMaxCHT() const;

	//! Get temperature of exhaust on cylinder \a uiCylinder.
	float GetEGT(int uiCylinder) const
	{ return m_KD.afEGT[uiCylinder]; }
	//! Get the maximal EGT temperature
	float GetMaxEGT() const;

	//! Get current rotational speed of the motor.
	float GetRPM() const
	{ return m_EFSD.afEngineN1[0]; }
	//! Is engine running?
	bool IsEngineRunning() const 
	{ return GetRPM() > RPM_TRESHOLD; }

	//! Get total engine time [h]
	float GetEngineTotalTime() const
	{ return m_KD.fEngineTotalTime; }

	//! Get current oil pressure [hPa].
	float GetOilPressure() const
	{ return m_EFSD.afEngineOilPressure[0]; }

	//! Get current rotational speed of the motor.
	float GetOilTemperature() const
	{ return m_EFSD.afEngineOilTemperature[0]; }

	//! Get current coolant (water) temperature.
	float GetWaterTemperature() const
	{ return m_EFSD.afEngineCoolantTemperature[0]; }

	//! Get current carburretor temperature.
	float GetCarburretorTemperature() const;

	// Electric

/*	//! Get voltage of the electric system
	float GetVoltage(int iElSystem) const
	{ return m_ED.afDCSystemVoltage[iElSystem]; }
	//! Get amperes of the electric system
	float GetAmperes(int iElSystem) const
	{ return m_ED.afDCSystemCurrent[iElSystem]; }*/
	//! Get voltage of the electric system
	float GetVoltage() const
	{ return m_ED.afDCSystemVoltage[0]; }
	//! Get amperes of the electric system
	float GetAmperes() const
	{ return m_ED.afDCSystemCurrent[0]; }
	
	// Backup (Internal) Battery
	
	//! Get backup battery mode (idle, charge, discharge, on-battery).
	int GetBackupBatteryMode() const
	{ return m_KD.abyBackupBatteryStatus[0]; }
	//! Get backup battery cell voltage.
	float GetBackupBatteryCellVoltage() const
	{ return m_KD.abyBackupBatteryStatus[1]/100.0f; }
	//! Get backup battery cell temperature.
	int GetBackupBatteryCellTemperature() const
	{ return m_KD.abyBackupBatteryStatus[2]; }
	//! Get backup battery capacity in [%].
	int GetBackupBatteryCapacity() const
	{ return m_KD.abyBackupBatteryStatus[3]; }

	// Magnetic heading

	//! Get current magnetic heading [rad]
// 	float GetMagneticHeading() const
// 	{ return m_ND.fMagneticHeading; }

	// D E B U G

	//! Debug only
	void SetDefaultPosition(QPointF ptDefaultPos)
	{ m_ptDefaultPos = ptDefaultPos; }

	//! Just test certain arguments for valid ranges then call the base class
	void ProcessIncommingMessage(const can::MessageAerospace& msg);

protected:
	//! Process bytes in the buffer and try to locate the packages.
	bool ProcessBuffer();

	//! We need to handle one GPS message in a special way.
// 	void HandleNormalOperationData(const MessageAerospace& msg);

	//! Handle Emergency Event Data
	void HandleEmergencyEventData(const can::MessageAerospace& msg);

protected:
	//! We need mutex in order to prevent simultaneous read/write access.
	mutable QMutex m_mutex;

	//! Incomming buffer of bytes.
	std::deque<uint8_t> m_dqBuffer;

	//! Flight State & Air Data struct.
	can::FlightStateAirData 		m_FSAD;
	//! Controls Data struct.
	can::FlightControlsData 		m_FCD;
	//! Engine Fuel Supply Data struct.
	can::EngineFuelSupplyData 		m_EFSD;
	//! Power Transmition Data struct.
	can::PowerTransmitionData 		m_PTD;
	//! Hydraulic Data struct.
	can::HydraulicData 				m_HD;
	//! Electric Data struct.
	can::ElectricData 				m_ED;
	//! Nav(igation) Data struct.
	can::NavData 						m_ND;
	//! Landing Gear Data struct.
	can::LandingGearData 			m_LGD;
	//! Miscellaneous Data struct.
	can::MiscellaneousData 			m_MD;

	//! Kanardia specific! DAQU data for single engine.
	can::KanardiaData				   m_KD;

	//! We remember the time of the last valid GPS status message
	QTime m_tLastValidGPS;
	
	//! How many EGTs are we monitoring?
	int m_iEGTCount;
	//! How many CHTs are we monitoring?
	int m_iCHTCount;

	//! Debug only : default postion
	QPointF m_ptDefaultPos;
private:
// 	static std::auto_ptr<NesisAerospaceDataManager> m_apNADM;
	static NesisAerospaceDataManager* m_apNADM;
};

//-----------------------------------------------------------------------------

#endif
