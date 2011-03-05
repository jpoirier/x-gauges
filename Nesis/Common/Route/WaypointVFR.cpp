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
#include "WaypointVFR.h"

namespace route {

// -------------------------------------------------------------------------

WaypointVFR::WaypointVFR()
: Waypoint()
{
	m_eType = tVFR;
}
// -------------------------------------------------------------------------

WaypointVFR::WaypointVFR(const WaypointVFR& C)
: Waypoint(C)
{
	m_slRefersTo = C.m_slRefersTo;
}

WaypointVFR::WaypointVFR(const QString& qsLine)
: Waypoint()
{
	// Now, parse the items.
	QStringList slItems = qsLine.split('\t');
	const int iCount = slItems.count();
	Q_ASSERT(iCount > 6);
	QString qs;
	
	// Set the waypoint type.
	m_eType = tVFR;
	
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
	m_eTypeVFR = (TypeVFR)slItems[6].toInt();
	
	// Description.
	if(iCount > 7) {
		m_qsDesc = slItems[7].replace("\"", QString());
	}
	// Refers to ...
	for(int i=8; i<iCount; i++) {
		qs = slItems[i];
		if(!qs.isEmpty())
			m_slRefersTo << qs;
	}
	
/*	qDebug() << m_qsKey << m_fLatitude << m_fLongitude 
				<< (int)m_eTypeVFR << m_qsDesc << m_slRefersTo;*/
}

// -------------------------------------------------------------------------

WaypointVFR::~WaypointVFR()
{
}

// -------------------------------------------------------------------------

void WaypointVFR::Write(QDataStream& out) const
{
	Waypoint::Write(out);
	
	// VFR save class version
	out << (qint8)1;
	out << (qint8)m_eTypeVFR;
	out << m_slRefersTo;
}

// -------------------------------------------------------------------------

void WaypointVFR::Read(QDataStream& in)
{
	Waypoint::Read(in);
	
	m_eType = tVFR;
	// Read the version first.
	qint8 iVer;
	qint8 i8;
	
	in >> iVer; 
	in >> i8; m_eTypeVFR = (TypeVFR)i8;
	in >> m_slRefersTo;	
}

// -------------------------------------------------------------------------

void WaypointVFR::Draw(QPainter& P, const map::Mapper& M) const
{
	P.save();
	
	// Triangle for the VFR points.
	static QPoint aptT[3] = { QPoint(0,-6), QPoint(-5,3), QPoint( 5,3) };
	
	// Prepare colors.
	P.setPen(Qt::darkBlue);
	bool bComp = (m_eTypeVFR == tCompulsory) || (m_eTypeVFR == tCompulsoryHeli);
	P.setBrush(bComp ? Qt::darkBlue : Qt::white);
	
	// Move the point
	P.translate(M.ConvertToImage(GetCoordinate()));
	
	// Draw triangle.
	P.drawConvexPolygon(aptT, 3);
	if(bComp) {
		P.setPen(Qt::NoPen);
		P.setBrush(Qt::white);
		P.drawEllipse(-1.5f, -1.5f, 3.0f, 3.0f);
	}
	
	P.restore();
}

// -------------------------------------------------------------------------

}

