#ifndef ROUTE_AIRFIELD_H
#define ROUTE_AIRFIELD_H

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

#include "Waypoint.h"

namespace route {

// -------------------------------------------------------------------------
	
class Airfield : public Waypoint
{
public:
	enum Category {
		catStandard,
		catGlider,
		catUltralight,
		catUltraLightOutlanding,
		catOutlanding,
		catUnknown
	};
	
	enum RunwayType {
		rwyUnknown,
		rwyAsphalt,
		rwyConcrete,
		rwyLoam,
		rwySand,
		rwyClay,
		rwyGrass,
		rwyGravel,
		rwyDirt
	};
	
	enum AirfieldInfo {
		aiNoInfo     = 0,
		aiRunwayType = 1,
		aiLength     = 2,
		aiDirection  = 4,
		aiFrequency  = 8,
		aiICAO		 = 16
	};
	
public:
	//! Default constructor.
	Airfield();
	//! Copy constructor.
	Airfield(const Airfield& C);
	//! Constructor from the QByteArray data.
	Airfield(const QByteArray& baDataLine);
	//! Destructor
	virtual ~Airfield();

	//! Clone the object, derived classes as well.
	virtual Waypoint* Clone() const
	{ return new Airfield(*this); }

	//! Get short version of the name
	virtual QString GetShortName() const;
	
	//! Get ICAO sign.
	const QByteArray& GetICAO() const
	{ return m_baICAO; }
	//! Get Frequency.
	const QByteArray& GetFrequency() const
	{ return m_baFrequency; }
	
	//! Get runway type.
	RunwayType GetRunwayType() const
	{ return m_eRunway; }
	//! Get airfield category (from small aviation view).
	Category GetCategory() const
	{ return m_eCategory; }
	
	//! Get runway direction 1;
	qint16 GetRunwayDir1() const
	{ return m_iDir1; }
	//! Get runway direction 2;
	qint16 GetRunwayDir2() const
	{ return m_iDir2; }
	
	//! Return true, if landing/takeof in one direction is highly recommended. 
	bool IsOneDirectionOnly() const 
	{ return m_iDir1==m_iDir2; }
	//! Return true if we have two runways.
	bool AreTwoRunways() const;
	//! Return true if the runway is paved.
	bool IsPaved() const
	{ return (m_eRunway == rwyAsphalt) || (m_eRunway == rwyConcrete); }
	
	//! Get length of the runway.
	qint16 GetRunwayLength() const
	{ return m_iLength; }
	
	//! Get airfield info flags.
	qint32 GetInfoFlags() const
	{ return m_iAirfieldInfo; }
	
	//! Save the data into given stream.
	void Write(QDataStream& out) const;
	//! Load the data from given stream.
	void Read(QDataStream& in);

	//! Return true if this object is a valid object.
	bool IsValid() const;
	
	//! Draw the airfield symbol
	void Draw(QPainter& P, const map::Mapper& M) const;

protected:
	//! Identify category form the given airfield data.
	Category IdentifyCategory(const QByteArray& baData, const QString& baKey);
	//! Parse airfield information flags from given airfield data.
	int ParseAirfieldFlags(const QByteArray& baData);

protected:
	//! The category of airfield.
	Category m_eCategory;
	//! Runway type.
	RunwayType m_eRunway;
	
	//! Runway length in meters.
	qint16 m_iLength;
	//! Runway direction 1
	qint16 m_iDir1;
	//! Runway direction 2
	qint16 m_iDir2;
	
	//! Airfield information (flags).
	qint32 m_iAirfieldInfo;
	
	//! Frequency - it could be empty.
	QByteArray m_baFrequency;
	
	//! The ICAO code, if available. It can be empty.
	QByteArray m_baICAO;
};

// -------------------------------------------------------------------------

} // namespace

#endif
