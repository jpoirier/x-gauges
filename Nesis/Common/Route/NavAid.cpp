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
#include "MathEx.h"
#include "NavAid.h"

namespace route {

// -------------------------------------------------------------------------

NavAid::NavAid()
: Waypoint()
{
	m_eType = tNavAid;
}
// -------------------------------------------------------------------------

NavAid::NavAid(const NavAid& C)
: Waypoint(C)
{
}

NavAid::NavAid(const QString& qsLine)
: Waypoint()
{
	// Now, parse the items.
	QStringList slItems = qsLine.split('\t');
	const int iCount = slItems.count();
	Q_ASSERT(iCount > 8);
	QString qs;
	
	// Set the waypoint type.
	m_eType = tNavAid;
	
	// State (country) is the first.
	qs = slItems[0].replace("\"", QString());
	m_eCountry = ParseCountry(qs.left(2).toAscii());
	
	// Key
	m_qsKey = slItems[1].replace("\"", QString());
	
	// Latitude
	m_fLatitude  = slItems[2].toFloat();
	m_fLatitude += slItems[3].toFloat()/60.0f;
	m_fLatitude  = common::Rad(m_fLatitude);
	
	// Longitude
	m_fLongitude  = slItems[4].toFloat();
	m_fLongitude += slItems[5].toFloat()/60.0f;
	m_fLongitude  = common::Rad(m_fLongitude);
	
	// VFR point type
	qs = slItems[6].replace("\"", QString());
	if(qs == "NDB")
		m_eTypeNA = tNDB;
	else if(qs == "VORDME")
		m_eTypeNA = tVORDME;
	else
		m_eTypeNA = tUnknown;
		
	// Frequency
	m_fFrequency = slItems[7].toFloat();
		
	// Description.
	if(iCount > 7) {
		m_qsDesc = slItems[8].replace("\"", QString());
	}
	
/*	qDebug() << m_qsKey << m_fLatitude << m_fLongitude 
				<< (int)m_eTypeNA << m_fFrequency << m_qsDesc;*/
}

// -------------------------------------------------------------------------

NavAid::~NavAid()
{
}

// -------------------------------------------------------------------------

void NavAid::Write(QDataStream& out) const
{
	Waypoint::Write(out);
	
	// VFR save class version
	out << (qint8)1;
	out << (qint8)m_eTypeNA;
	out << m_fFrequency;
}

// -------------------------------------------------------------------------

void NavAid::Read(QDataStream& in)
{
	Waypoint::Read(in);
	
	m_eType = tNavAid;
	// Read the version first.
	qint8 iVer;
	qint8 i8;
	
	in >> iVer; 
	in >> i8; m_eTypeNA = (TypeNA)i8;
	in >> m_fFrequency;	
}

// -------------------------------------------------------------------------

void NavAid::Draw(QPainter& P, const map::Mapper& M) const
{
	P.save();
	
	// Diamond for the NavAid.
	static QPoint aptD[4] = { QPoint(-4,0), QPoint(0,4), QPoint(4,0), QPoint(0,-4) };
	
	// Prepare colors.
	P.setPen(Qt::darkBlue);
	P.setBrush(Qt::darkBlue);
	
	// Move the point
	P.translate(M.ConvertToImage(GetCoordinate()));
	
	// Draw diamond.
	P.drawConvexPolygon(aptD, 4);
	P.restore();
}

// -------------------------------------------------------------------------

}

