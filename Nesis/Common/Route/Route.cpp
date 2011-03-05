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
#include "CommonDefs.h"
#include "SphereNavigation.h"
#include "Geometry/Line2D.h"

#include "Route.h"

namespace route {

// ------------------------------------------------------------------------
	
Route::Route(const QString& qsName)
{
	m_qsName = qsName;
}

Route::Route(const Route& C)
{
	m_qsName = C.m_qsName;
	m_conWpt.Copy(C.m_conWpt);
}

Route::~Route()
{
}

// ------------------------------------------------------------------------

void Route::Clear()
{
	m_conWpt.Clear();
	m_qsName.clear();
}

// -----------------------------------------------------------------------

QString Route::GetName(bool bTrueName) const
{
	if(bTrueName || (m_qsName.isEmpty() == false))
		return m_qsName;
	else
		return GetNameFromWaypoints();
}

// -----------------------------------------------------------------------

QString Route::GetNameFromWaypoints() const
{
	int iC = m_conWpt.GetCount();
	if(iC < 2)
		return "[auto]";
//		return m_qsName;

	QString qs;
	qs += m_conWpt[0]->GetShortName();
	qs += "-";
	qs += m_conWpt[iC-1]->GetShortName();
	return qs;
}

// ------------------------------------------------------------------------

void Route::Write(QDataStream& out) const
{
	// version
	out << (qint8)1;

	// Write route name
	out << m_qsName;

	// Write waypoints
	m_conWpt.Write(out);
}

// ------------------------------------------------------------------------

void Route::Read(QDataStream& in)
{
	Clear();
	// Read the version first.
	qint8 iVer;
	in >> iVer; 

	// Read the route name
	in >> m_qsName;
	
	// Read the waypoints
	m_conWpt.Read(in);
}
// ------------------------------------------------------------------------

std::pair<float, float> Route::GetBearingDistance(
	int iLeg	//!< The leg - it is 1 based (0 means and invalid leg)
) const
{
	if(iLeg < m_conWpt.GetCount() && iLeg > 0) {
		QPointF pt1 = m_conWpt[iLeg-1]->GetCoordinate();
		QPointF pt2 = m_conWpt[iLeg]->GetCoordinate();
		std::pair<float,float> bd = common::GetBearingDistanceGreatCircle(
			pt1.x(), pt1.y(),
			pt2.x(), pt2.y()
		);
		bd.second *= EARTH_RADIUS_M;
		return bd;
	}

	// failure.
	return std::make_pair(0.0f, 0.0f);
}

// ------------------------------------------------------------------------

void Route::Insert(int iPos, Waypoint* pWpt)
{
	m_conWpt.Insert(iPos, pWpt);
}

// ------------------------------------------------------------------------

void Route::Erase(int iPos)
{
	m_conWpt.Erase(iPos);
}

// ------------------------------------------------------------------------

float Route::GetTotalDistance() const
{
	float fD = 0.0f;
	for(int iLeg=1; iLeg<m_conWpt.GetCount(); iLeg++) {
		fD += GetBearingDistance(iLeg).second;
	}
	return fD;
}

// ------------------------------------------------------------------------

QVector<QPointF> Route::GetCoordinates() const
{
	QVector<QPointF> vCoor;
	for(int i=0; i<m_conWpt.GetCount(); i++) {
		vCoor << m_conWpt[i]->GetCoordinate();
	}
	return vCoor;
}

// ------------------------------------------------------------------------

const Waypoint* Route::GetWaypoint(int i) const
{
	if(i<m_conWpt.GetCount())
		return m_conWpt[i];
	return NULL;
}

// ------------------------------------------------------------------------

QVector<std::pair<float,float> > Route::GetTimeDistance(
	unsigned int uiActiveLeg, //!< Current active leg
	const QPointF& ptX, //!< Current point
	float fGS			  //!< Current ground speed.
) const
{
	QVector<std::pair<float,float> > vTD;
	Q_ASSERT(uiActiveLeg < GetWaypoints().GetCount());

	std::pair<float,float> bd = std::make_pair(0.0f, 0.0f);
	
	// All waypoints that were already reached get the zero time and zero distance.
	for(uint i=0; i<uiActiveLeg; i++)
		vTD << bd;

	// The first (active leg) is direct to waypoint
	bd = GetWaypoint(uiActiveLeg)->GetBearingDistance(ptX);
	if(fGS > 0.0f)
		bd.first = bd.second / fGS;
	else
		bd.first = 0.0;
	vTD << bd;
	
	// The remining legs (if any) are point to point distances.
	for(int i=uiActiveLeg+1; i<GetWaypoints().GetCount(); i++) {
		bd = GetBearingDistance(i);
		if(fGS > 0.0f)
			bd.first = bd.second / fGS;
		else
			bd.first = 0.0;

		bd.first += vTD.back().first;
		bd.second += vTD.back().second;

		vTD << bd;
	}

	return vTD;
}
// -----------------------------------------------------------------------

QPointF Route::GetNearestPoint(
	const QPointF& pt0, //!< Origin point (current point)
	int iLeg,			  //!< Active leg.
	float* pfRelPos,	  //!< Relative position of the result point on the line.
	float* pfDistance    //!< Distance from the origin to the relative point.
) const
{
	Q_ASSERT(iLeg < GetWaypoints().GetCount());
	// It is the current point if the leg is not defined.
	if(iLeg == 0) {
		if(pfRelPos)
			*pfRelPos = -1.0f;
		if(pfDistance)
			*pfDistance = 0.0f;
		return pt0;
	}
	
	// TODO Re-think the procedure for the GLOBE case.
	// Current solution assumes flat earth approximation at the
	// pt0 - origin point.
	
	using namespace geometry;
	float fCos = cosf(pt0.y());
	QPointF pt1 = GetWaypoint(iLeg-1)->GetCoordinate() - pt0;
	QPointF pt2 = GetWaypoint(iLeg)->GetCoordinate() - pt0;
	pt1.rx() *= fCos;
	pt2.rx() *= fCos;
	Point2D ptNull(0.0f,0.0f);
	
	Line2D L(pt1, pt2);
	Point2D ptR = L.GetNearestPoint(ptNull);
	
	// Where is this point?
	if(pfRelPos!=NULL)
		*pfRelPos = L.GetScalar(ptR);
		
	// How far is the point from the origin in meters!
	if(pfDistance) 
		*pfDistance = ptR.GetDistance(ptNull) * EARTH_RADIUS_M;
		
	// Now, we need to convert the result.
	return QPointF(ptR.GetX()/fCos + pt0.x(), ptR.GetY() + pt0.y());
}

// -----------------------------------------------------------------------

bool Route::IsValid() const
{
	// A route must have: 
	
	// a name
	// AK: Name is not necessary, since then it goes into auto mode.
/*	if(m_qsName.isEmpty())
		return false;*/
		
	// at least two waypoints
	if(m_conWpt.GetCount() < 2)
		return false;
	
	// a non-zero distance
	if(GetTotalDistance() <=10)
		return false;
		
	// Two neigbour points must not be the same
	for(int i=0; i<m_conWpt.GetCount()-1; i++) {
		if(m_conWpt[i]->GetCoordinate() == m_conWpt[i+1]->GetCoordinate())
			return false;
	}
	
	return true;
}

// -----------------------------------------------------------------------

void Route::Reverse()
{
	m_conWpt.Reverse();
}

// -----------------------------------------------------------------------

}

