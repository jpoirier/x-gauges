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

#include <QSize>
#include <QIcon>
#include <algorithm>
#include "CommonDefs.h"
#include "MathEx.h"
#include "Route/Airfield.h"
#include "Unit/Value.h"
#include "FastUnitKeys.h"
#include "Settings.h"
#include "PixmapDirection.h"

#include "ModelWaypointTable.h"


ModelWaypointTable::ModelWaypointTable(QObject *pParent)
 : QAbstractTableModel(pParent)
{
	m_qlHeader << tr("Key");
	m_qlHeader << tr("Name");
	m_qlHeader << tr("Bear.");
	m_qlHeader << tr("Dist.");
	m_fHeading = -1; // Invalid heading.
}


ModelWaypointTable::~ModelWaypointTable()
{
}

// -------------------------------------------------------------------------

int ModelWaypointTable::rowCount(const QModelIndex& /*parent*/) const
{
	return m_conWayPts.count();
}

// -------------------------------------------------------------------------

int ModelWaypointTable::columnCount(const QModelIndex& /*parent*/) const
{
	// code, name, distance, bearing
	return m_qlHeader.count(); 
}

// -------------------------------------------------------------------------

QVariant ModelWaypointTable::data(
	const QModelIndex & index, 
	int iRole
) const
{
	FastUnitKeys* pFUK 	= FastUnitKeys::GetInstance();
	Settings* pS 			= Settings::GetInstance();
	using namespace route;
	
	if(!index.isValid())
		return QVariant();

	const int iR = index.row();
	const int iC = index.column();
	
	if(iRole == Qt::DisplayRole) {
		if(iR < m_conWayPts.count() && iC < m_qlHeader.count()) {
			const Waypoint* pTP = m_conWayPts[iR].m_pTP;
			QString qs;
			
			if(iC==0) {
				QString qs = pTP->GetKey();
				bool bAirfield = pTP->GetType() == Waypoint::tAirfield;
				if(bAirfield) {
					const Airfield* pA = (const Airfield*)pTP;
					if(pA->GetInfoFlags() & Airfield::aiICAO) {
						qs =  pA->GetICAO() + " *";
					}
				}				
				return QVariant(qs);
			}
			else if(iC==1) {
				return QVariant(pTP->GetDescription());
			}
			else if(iC==2) {
				QString qs = QString::number(qRound(common::Deg(m_conWayPts[iR].m_fBearing)));
				//qs += QChar(0xba);
				return qs;
			}
			else if(iC==3) {
				unit::Value uvD(pFUK->iKey_m, m_conWayPts[iR].m_fDistance);
				uvD.ConvertTo(pS->GetDistanceUnit());
				return uvD.Format(0);
				//return QString("%1 km").arg(qRound(m_conWayPts[iR].m_fDistance/1000));
			}
		}
	}
	else if(iRole == Qt::TextAlignmentRole) {
		if(iC > 1)
			return QVariant(Qt::AlignHCenter);
	}
	else if(iRole == Qt::DecorationRole && m_fHeading >= 0) {
		if(iC == 2) {
			return QVariant(QIcon(PixmapDirection::GetInstance()->GetPixmap(m_fHeading,m_conWayPts[iR].m_fBearing)));
		}
	}
	return QVariant();
}

// -------------------------------------------------------------------------

QVariant ModelWaypointTable::headerData (
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
			static int ix[] = { 80, 140, 50, 50 };
			ASSERT(iSection < (int)COUNTOF(ix));
			return QVariant(QSize(ix[iSection], 25));
		}
	}
	
	if(orientation == Qt::Vertical) {
		if(iRole == Qt::DisplayRole) {
			return QVariant(iSection+1);
		}
	}
	return QVariant();
}

// -------------------------------------------------------------------------

void ModelWaypointTable::SetWaypoints(
	const QVector<const route::Waypoint*>& conWayPts
)
{
	std::pair<float,float> bd;
	m_conWayPts.clear();
	m_conWayPts.reserve(conWayPts.size());
	 
	Data data;
	foreach(const route::Waypoint* pTP, conWayPts) {
		bd = pTP->GetBearingDistance(m_ptX.x(), m_ptX.y());
		data.m_pTP = pTP;
		data.m_fBearing = bd.first;
		data.m_fDistance = bd.second;
		m_conWayPts << data;
	}
	
	std::sort(m_conWayPts.begin(), m_conWayPts.end(), data);
	
	// limit number of items to 99
	if(m_conWayPts.size() > 99)
		m_conWayPts.erase(m_conWayPts.begin()+99, m_conWayPts.end());
	
	reset();
}

// -------------------------------------------------------------------------
	
const route::Waypoint* ModelWaypointTable::GetWaypoint(int index) const
{
	ASSERT(index < m_conWayPts.size());
	return m_conWayPts[index].m_pTP;
}
