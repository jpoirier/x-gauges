#ifndef PACKET_H
#define PACKET_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2006 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *                                                                         *
 ***************************************************************************/

/*! \file packet.h
    \brief Structures used by Kanardia in the EIA-232 communication.

    \author  Ales Krajnc
 */

#include <stdint.h>

// ---------------------------------------------------------------------------
namespace packetcomm {

struct PacketAcknowledge
{
	//! Command that was acknowledged.
	uint8_t	m_byCommand;
	//! Command depended status information.
	int32_t 	m_iStatus;
};

struct PacketRaw
{
	//! Raw sensor altitude as 16 bit unsigned integer.
	uint16_t m_uiAltitude;
	//! Raw sensor velocity as 16 bit unsigned integer.
	uint16_t m_uiVelocity;

	//! Raw sensor vario as 16 bit signed integer.
	int16_t m_iVario;

	//! (P) X axis rate - raw sensor angular velocity as 16 bit unsigned integer.
	uint16_t m_uiP;
	//! (Q) Y axis rate - raw sensor angular velocity as 16 bit unsigned integer.
	uint16_t m_uiQ;
	//! (R) Z axis rate- raw sensor angular velocity as 16 bit unsigned integer.
	uint16_t m_uiR;

	//! Acc x - raw acceleration sensor as 16 bit unsigned integer.
	uint16_t m_uiAccX;
	//! Acc y - raw acceleration sensor as 16 bit unsigned integer.
	uint16_t m_uiAccY;
	//! Acc z - raw acceleration sensor as 16 bit unsigned integer.
	uint16_t m_uiAccZ;

	//! Magnetic x - raw magnetic sensor as 16 bit signed integer.
	int16_t m_iMagX;
	//! Magnetic y - raw magnetic sensor as 16 bit signed integer.
	int16_t m_iMagY;
	//! Magnetic z - raw magnetic sensor as 16 bit signed integer.
	int16_t m_iMagZ;

	//! On sensor board temperature raw sensor as 16 bit unsigned integer.
	uint16_t m_uiSensorTemp;
	//! Outside (air) temperature raw sensor as 16 bit unsigned integer.
	uint16_t m_uiAirTemp;
};

// --------------------------------------------------------------------

struct PacketPosition
{
	//! Inertal system Latitude [rad, N positive]
	float m_fLatitude;
	//! Inertial system Longitude [rad, E positive]
	float m_fLongitude;
};

// --------------------------------------------------------------------

struct PacketWind
{
	//! Wind north component (wind velocity vector projection) [m/s].
	float m_fNorth; // north/south wind component
	//! Wind east component (wind velocity vector projection) [m/s].
	float m_fEast; // east/west.
};

// --------------------------------------------------------------------

struct PacketAttitude
{
	//! Roll [rad]
	float m_fRoll;
	//! Pitch [rad]
	float m_fPitch;
	//! Yaw [rad]
	float m_fYaw;
};

// --------------------------------------------------------------------

struct PacketAcceleration
{
	//! Acc x [m/s2]
	float m_fX;
	//! Acc y [m/s2]
	float m_fY;
	//! Acc z [m/s2]
	float m_fZ;
};

// --------------------------------------------------------------------

struct PacketPQR
{
	//! x axis rotation rate [rad/s]
	float m_fP;
	//! y axis rotation rate [rad/s]
	float m_fQ;
	//! z axis rotation rate [rad/s]
	float m_fR;
};

// --------------------------------------------------------------------

struct PacketCAN
{
	//! Can message identifier.
	uint32_t m_uiIdentifier;
	//! Header information.
	uint32_t m_uiHeader;
	//! Data.
	uint32_t m_uiData;
	//! Counter used for debugging.
	uint16_t m_uiCount;
	//! Number of elements in the TX buffer.
	uint8_t m_uiTxWait;
	//! Number of elements in the RX buffer.
	uint8_t m_uiRxWait;
};

// --------------------------------------------------------------------

struct PacketAirdata
{
	//! Altitude [m]
	float m_fAltitude;
	//! Indicated AirSpeed [m/s]
	float m_fIAS;
	//! Vertical speed - vario [m/s]
	float m_fVario;
	//! Static pressure [hPa]
	float m_fStaticPressure;
	//! True AirSpeed [m/s]
 	float m_fTAS;
};

// --------------------------------------------------------------------

struct PacketGPS
{
	//! GPS Latitude [rad, N positive]
	float m_fLatitude;
	//! GPS Longitude [rad, E positive]
	float m_fLongitude;
	//! GPS Altitude [m]
	float m_fAltitude;

	//! GPS Velocity [m/s]
	float m_fVelocity;
	//! GPS tracking [rad]
	float m_fTracking;

	//! UTC Date in Julian Day format.
	uint32_t m_uiJulianDay;
	//! UTC Time seconds since the midnight.
	float m_fSeconds;
	//! Number of visible GPS satelites
	uint8_t m_bySatCount;
};

// ---------------------------------------------------------------------------

struct PacketTemperature
{
	//! On sensor board temperature [C].
	float m_fSensor;
	//! Outside temperature [C].
	float m_fOutside;
};

// ---------------------------------------------------------------------------

struct PacketMagneticField
{
	// Note values are modified by mount matrix.
	//! Magnetic x - (averaged) magnetic sensor as float.
	float m_fX;
	//! Magnetic y - (averaged) magnetic sensor as float.
	float m_fY;
	//! Magnetic z - (averaged) magnetic sensor as float.
	float m_fZ;
};

// ---------------------------------------------------------------------------

struct PacketTelemetry
{
	//! Altitude [m]
	float m_fAltitude;
	//! Static pressure [hPa]
	float m_fStaticPressure;
	//! Vertical speed - vario [m/s]
	float m_fVario;

	//! Indicated AirSpeed [m/s]
	float m_fIAS;
	//! True AirSpeed [m/s]
	float m_fTAS;

	//! x axis rotation rate [rad/s]
	float m_fP;
	//! y axis rotation rate [rad/s]
	float m_fQ;
	//! z axis rotation rate [rad/s]
	float m_fR;

	//! Roll [rad]
	float m_fRoll;
	//! Pitch [rad]
	float m_fPitch;
	//! Yaw [rad]
	float m_fYaw;

	//! Acc x [m/s2]
	float m_fAccX;
	//! Acc y [m/s2]
	float m_fAccY;
	//! Acc z [m/s2]
	float m_fAccZ;

	//! On sensor board temperature [C].
	float m_fSensorTemp;
	//! Outside temperature [C].
	float m_fOutsideTemp;

	//! Inertal system Latitude [rad, N positive]
	float m_fLatitude;
	//! Inertial system Longitude [rad, E positive]
	float m_fLongitude;

	//! GPS Latitude [rad, N positive]
	float m_fLatitudeGPS;
	//! GPS Longitude [rad, E positive]
	float m_fLongitudeGPS;
	//! GPS Altitude [m]
	float m_fAltitudeGPS;
	//! GPS Velocity [m/s]
	float m_fVelocityGPS;
	//! GPS Vario [m/s]
	float m_fVarioGPS;

	//! Number of visible GPS satelites
	unsigned char m_bySatCount;
};

// ---------------------------------------------------------------------------

struct PacketWMM
{
	//! Type (0 = standard report, 1 = external request report).
	uint8_t m_byType;
	//! North component [nT].
	float m_fNorth;
	//! East component [nT].
	float m_fEast;
	//! Down component [nT].
	float m_fDown;
	//! Declination [rad] (eastern declination is positive).
	float m_fDeclination;
	//! Inclinatin [rad].
	float m_fInclination;
};

// ---------------------------------------------------------------------------

struct PacketHeading
{
	//! Magnetic heading [rad].
	float m_fMagnetic;
	//! True heading [rad].
	float m_fTrue;
};

// ---------------------------------------------------------------------------

struct PacketUnitInfo
{
	//! Id of the unit.
	uint16_t m_uiUnitId;
	//! Serial number.
	uint32_t m_uiSerial;
	//! Software version.
	uint16_t m_uiSoftware;
	//! Hardware version.
	uint16_t m_uiHardware;
	//! Vendor id.
	uint16_t m_uiVendor;
};

// ---------------------------------------------------------------------------

struct PacketZlagalnik
{
	//! X axis
	int32_t m_iX;
	//! Y axis
	int32_t m_iY;
	//! Command on position.
	uint32_t m_uiCommand;
};

// ---------------------------------------------------------------------------

struct PacketOven
{
	//! X axis
	int16_t m_iTempInside;
	//! Y axis
	int16_t m_iTempBoard;
	//! Y axis
	uint32_t m_uiTime;
	//! Command on position.
	uint8_t m_uiStatus;
};

// ---------------------------------------------------------------------------

} // namespace

#endif
