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
#include <QtGui>
#include "CommonDefs.h"
#include "MathEx.h"
#include "Route/Route.h"
#include "Route/Airfield.h"
#include "Unit/Value.h"
#include "FastUnitKeys.h"
#include "Settings.h"
#include "NesisAerospaceDataManager.h"

#include "ModelRouteWptTable.h"

ModelRouteWptTable::ModelRouteWptTable(QObject *pParent)
 : QAbstractTableModel(pParent)
{
	m_fSpeed = 0.0f;
	m_pRoute = NULL;
	m_qlHeader << tr("Key");
	m_qlHeader << tr("Name");
	m_qlHeader << tr("Bear.");
	m_qlHeader << tr("Dist.");
	m_qlHeader << tr("Time");
	m_qlHeader << tr("ETA");
}

ModelRouteWptTable::~ModelRouteWptTable()
{
}

// -------------------------------------------------------------------------

void ModelRouteWptTable::SetRoute(route::Route* pRoute)
{
	m_pRoute = pRoute;
	reset();
}

// -------------------------------------------------------------------------

int ModelRouteWptTable::rowCount(const QModelIndex& /*parent*/) const
{
	if(m_pRoute)
		return m_pRoute->GetWaypoints().GetCount()+1;
	else
		return 0;
}

// -------------------------------------------------------------------------

int ModelRouteWptTable::columnCount(const QModelIndex& /*parent*/) const
{
	// key, name, distance, bearing, time
	return m_qlHeader.count();
}

// -------------------------------------------------------------------------

QVariant ModelRouteWptTable::data(
	const QModelIndex & index,
	int iRole
) const
{
	ASSERT(m_fSpeed > 0);
	using namespace route;

	FastUnitKeys* pFUK 	= FastUnitKeys::GetInstance();
	Settings* pS 			= Settings::GetInstance();

	if(!index.isValid())
		return QVariant();

	const int iR = index.row();
	const int iC = index.column();

	// Check for the invalid situation.
	if(!m_pRoute && iRole==Qt::DisplayRole) {
 		if(index.column()==1)
			return tr("Warning: Empty route.");
		else
			return QVariant();
	}

	// Normal situation.
	if(iRole == Qt::DisplayRole) {
		if(iR < m_pRoute->GetWaypoints().GetCount() && iC < m_qlHeader.count()) {
			const Waypoint* pWpt = m_pRoute->GetWaypoints().GetWaypoint(iR);
			QString qs;

			// Key
			if(iC==0) {
				QString qs = pWpt->GetKey();
				bool bAirfield = pWpt->GetType() == Waypoint::tAirfield;
				if(bAirfield) {
					const Airfield* pA = (const Airfield*)pWpt;
					if(pA->GetInfoFlags() & Airfield::aiICAO) {
						qs =  pA->GetICAO() + " *";
					}
				}
				return QVariant(qs);
			}
			// Name
			else if(iC==1) {
				return QVariant(pWpt->GetDescription());
			}

			if(iC>=2 && iC<=4) {
				if(iR==0)
					return QVariant();

				// first - bearing in radians, second - distance in meters.
				common::PairFF bd = m_pRoute->GetBearingDistance(iR);

				// Bearing is always in degrees.
				if(iC==2) {
					bd.first = common::Deg(bd.first);
					return QString::number(bd.first, 'f', 0);
				}
				// Distance
				else if(iC==3){
					unit::Value uvD(pFUK->iKey_m, bd.second);
					uvD.ConvertTo(pS->GetDistanceUnit());
					return uvD.Format(0);
				}
				// Duration - Time
				else {
					QTime time(0,0);
					time = time.addSecs(bd.second/m_fSpeed);
					return time.toString("H:mm");
				}
			}
			// ETA
			else if(iC==5)  {
				return ("-");
			}
		}
		// Total row
		else {
			// Total distance
			if(iC==3) {
				unit::Value uvD(pFUK->iKey_m, m_pRoute->GetTotalDistance());
				uvD.ConvertTo(pS->GetDistanceUnit());
				return uvD.Format(0);
			}
			// Total time
			else if(iC==4) {
				QTime time(0,0);
				time = time.addSecs(m_pRoute->GetTotalDistance()/m_fSpeed);
				return time.toString("H:mm");
			}
		}
	}
	// Center items after waypoint name
	else if(iRole == Qt::TextAlignmentRole) {
		if(iC > 1)
			return QVariant(Qt::AlignHCenter);
	}

	else if(iRole == Qt::BackgroundRole) {
		if(m_pRoute)
			if(iR==m_pRoute->GetWaypoints().GetCount())
				return QBrush(Qt::yellow);
	}

	return QVariant();
}

// -------------------------------------------------------------------------

QVariant ModelRouteWptTable::headerData (
	int iSection,
	Qt::Orientation orientation,
	int iRole
) const
{
	if(orientation == Qt::Horizontal) {
		if(iRole == Qt::DisplayRole) {
			ASSERT(iSection < m_qlHeader.count());
			return QVariant(m_qlHeader[iSection]);
		}
		else if(iRole == Qt::SizeHintRole) {
			static int ix[] = { 10, 140, 50, 50, 50, 50 };
			ASSERT(iSection < (int)COUNTOF(ix));
			return QVariant(QSize(ix[iSection], 25));
		}
	}

	if(orientation == Qt::Vertical) {
		if(iRole == Qt::DisplayRole) {
			if(iSection < m_pRoute->GetWaypoints().GetCount())
				return QVariant(iSection+1);
			else
				return "T";
		}
		else if(iRole == Qt::SizeHintRole) {
			return QSize(20,22);
		}
	}
	return QVariant();
}

// -------------------------------------------------------------------------

QModelIndex ModelRouteWptTable::Insert(int iPos, const route::Waypoint* pW)
{
	ASSERT(m_pRoute);
	// Make a clone.
	route::Waypoint* pWCopy = pW->Clone();
	m_pRoute->Insert(iPos, pWCopy);
	reset();

	if(iPos < 0 || iPos >= m_pRoute->GetWaypoints().GetCount())
		iPos = m_pRoute->GetWaypoints().GetCount()-1;
	return createIndex(iPos,0);
}

// -------------------------------------------------------------------------

QModelIndex ModelRouteWptTable::Erase(int iPos)
{
	ASSERT(m_pRoute);
	m_pRoute->Erase(iPos);
	reset();

	if(iPos >= m_pRoute->GetWaypoints().GetCount())
		iPos = m_pRoute->GetWaypoints().GetCount()-1;
	return createIndex(iPos,0);
}

// -------------------------------------------------------------------------

QPointF ModelRouteWptTable::GetCoordinates(int iPos) const
{
	ASSERT(m_pRoute);
	if(m_pRoute->GetWaypoints().GetCount()==0)
		return NesisAerospaceDataManager::GetInstance()->GetPosition();

	if(iPos < 0 || iPos >= m_pRoute->GetWaypoints().GetCount())
		iPos = m_pRoute->GetWaypoints().GetCount()-1;

	return m_pRoute->GetWaypoints().GetWaypoint(iPos)->GetCoordinate();
}

// -------------------------------------------------------------------------

void ModelRouteWptTable::SetSpeed(float fSpeed)
{
	Q_ASSERT(fSpeed > 10);
	m_fSpeed = fSpeed;
	reset();
}

// -------------------------------------------------------------------------
