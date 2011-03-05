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

#include <algorithm>
#include <QtDebug>
#include <QFile>
#include <QDataStream>
#include "CommonDefs.h"
#include "Airfield.h"
#include "WaypointVFR.h"
#include "WaypointContainer.h"
#include "SphereNavigation.h"
#include "NavAid.h"

namespace route {

// -------------------------------------------------------------------------
	
WaypointContainer::WaypointContainer()
{
	m_bDirty = false;
}

WaypointContainer::WaypointContainer(const WaypointContainer& C)
{
	Copy(C);
}

WaypointContainer::~WaypointContainer()
{
	Clear();
}
// -------------------------------------------------------------------------

void WaypointContainer::Copy(const WaypointContainer& C)
{
	Clear();
	m_bDirty = C.m_bDirty;
	foreach(const Waypoint* pWpt, C.m_con)
		m_con << pWpt->Clone();
	// TODO Copy sectors as well!
}

// -------------------------------------------------------------------------

void WaypointContainer::Clear()
{
	foreach(Waypoint* pWpt, m_con) {
		delete pWpt;
	}
	m_con.clear();
	m_bDirty = true;
}

// -------------------------------------------------------------------------

int WaypointContainer::LoadW2000(const QString& qsFile)
{
	QFile file(qsFile);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qWarning() << "Unable to open the" << qsFile << "file.";
		return 0;
	}
	
	QByteArray baLine;
	while(file.atEnd()==false) {
		baLine = file.readLine();
		if(baLine[0]!='$') {
			if(baLine.size() > 60) {
				if(IsAirfieldW2000(baLine)) {
					Airfield* pA = new Airfield(baLine);
					// Check if the airfield meet our needs.
					if(pA->IsValid()== false                        ||
						pA->GetCategory() == Airfield::catOutlanding ||
					   pA->GetCategory() == Airfield::catUnknown    ||
					   pA->GetRunwayLength() <= 400) {
					   delete pA;
					}
					else {
						m_con << pA;
					}
				}
/*				else {
					TurningPoint* pWpt = new TurningPoint(baLine);
					m_con.insert(pWpt->GetKey(), pWpt);
				}*/
			}
		}
	}
	m_bDirty = false;
	return m_con.count();
}

// -------------------------------------------------------------------------

bool WaypointContainer::IsAirfieldW2000(const QByteArray& baDataLine) const
{
	Q_ASSERT(baDataLine.size() > 6);
	
	//An airfield is recognised by characters "#" or "*" on position 23
	int iPos;
	iPos = baDataLine.indexOf('#');
	if(iPos == 23)
		return true;
	
	iPos = baDataLine.indexOf('*');
	if(iPos == 23)
		return true;
	
	return false;
	
	// The old code is below
/*	
	// An airfield is recognised by letter 1 as the sixth5 letter limit in the key, which is 
	// typically 6 characters long.
	QByteArray baKey = baDataLine.left(6).simplified();
	if(baKey.size() <= 5) {
		// Additional check, # or * must be present in the line
		if(baDataLine.contains('#') || baDataLine.contains('*'))
			return true;
	}
	
	// Some airstrips are unclear from "google-earth" and these are identified
	// as such by number 2 on the sixth place.
	if(baKey[5]=='2') {
		if(baDataLine.contains('#') || baDataLine.contains('*'))
// 		qDebug() << "Key = " << baKey << ", Line = " << baDataLine;
			return true;
	}
	 
// 	if(baKey[5]=='1') {
// 		qDebug() << "Key = " << baKey << ", Line = " << baDataLine;
// 		return false;
// 	}

	return false;	*/
}

// -------------------------------------------------------------------------

bool WaypointContainer::Save(const QString& qsFile) const
{
	QFile file(qsFile);
	
	if(file.open(QIODevice::WriteOnly)==false) {
		qDebug() << "WaypointContainer::Save failed to open the file: " 
				   << qsFile; 
		return false;
	}
	QDataStream out(&file);
	
	m_bDirty = false;
	return Write(out);
}

// -------------------------------------------------------------------------

bool WaypointContainer::Write(QDataStream& out) const
{
	// version first
	out << (qint8)1;
	
	// container size
	out << (qint32) m_con.size();
	
	// item by item ...
	foreach(Waypoint* pWpt, m_con) {
		out << (qint8)pWpt->GetType();
		pWpt->Write(out);
	}
	
	return true;
}

// -------------------------------------------------------------------------

int WaypointContainer::Load(
	const QString& qsFile,
	const QPointF& ptX, 
	int iRadiusKm
)
{
	int iCount = m_con.count();
	QFile file(qsFile);
	
	if(file.open(QIODevice::ReadOnly)==false) {
		qDebug() << "WaypointContainer::Load failed to open the file: " 
				<< qsFile; 
		return false;
	}
	QDataStream in(&file);

	m_bDirty = false;
	if(Read(in, ptX, iRadiusKm)==false) {
		qDebug() << "WaypointContainer::Load error during reading.";
	}
	return m_con.count() - iCount;
}

// -------------------------------------------------------------------------

bool WaypointContainer::Read(
	QDataStream& in,
	const QPointF& ptX, 
	int iRadiusKm
)
{	
	// version first
	qint8 iVer;
	in >> iVer;
	
	// container size
	qint32 iCount;
	in >> iCount;
	
	qint8 i8;
	Waypoint* pWpt;
	Waypoint::Type eType;
	
	while(iCount-- > 0) {
		// Get type
		in >> i8; eType = (Waypoint::Type)i8;
		
		// Create right object
		if(eType==Waypoint::tAirfield)
			pWpt = new Airfield();
		else if(eType == Waypoint::tVFR)
			pWpt = new WaypointVFR;
		else if(eType == Waypoint::tNavAid)
			pWpt = new NavAid();
		else
			pWpt = new Waypoint();
		
		// Load data
		pWpt->Read(in);
		// Validity check.
		if(pWpt->IsValid()) {
			// Check radius
			if(ptX.isNull()==false && iRadiusKm > 0) {
	// 			qDebug() << pWpt->GetDistance(ptX)/1000;
				if(pWpt->GetDistance(ptX)/1000 <= iRadiusKm)
					m_con << pWpt;
				else
					delete pWpt;
			}
			// no check
			else {
				m_con << pWpt;
			}
		}
		// Not valid!
		else {
			qDebug() << "Waypoint" << pWpt->GetKey() << "is invalid.";
			delete pWpt;
		}
	}
	
	m_bDirty = false;
	return true;
}

// -------------------------------------------------------------------------

QVector<const Waypoint*> WaypointContainer::GetPointsInRect(const QRectF& rcRad) const
{
	QVector<const Waypoint*> vWpt;
	foreach(const Waypoint* pWpt, m_con) {
		if(rcRad.contains(pWpt->GetCoordinate()))
			vWpt.push_back(pWpt);
	}
	return vWpt;
}

// -------------------------------------------------------------------------

QVector<const Waypoint*> WaypointContainer::Search(
	const QPointF& ptCoor, 
	int iTypes, 
	int iRadius_km, 
	const QString& qsName
) const
{
	QVector<const Waypoint*> vWpt;

	// Name filter
	QString qsFilt = qsName.left(5);
	qDebug() << qsFilt;
	int iPos = qsFilt.indexOf('*');
	if(iPos != -1)
		qsFilt = qsName.left(iPos);
	bool bFilter = !qsFilt.isEmpty();
	
	// Distance check
	int iRadius = iRadius_km*1000;
	
	// Box for rought check (should make searching faster unless
	// a large radius is selected.
	float fy = iRadius/EARTH_RADIUS_M;
	float fc = qMax(cos(ptCoor.y()), 0.1);
	float fx = fy/fc;
	QRectF rBox(ptCoor.x()-fx, ptCoor.y()-fy, 2*fx, 2*fy);
	
	foreach(const Waypoint* pWpt, m_con) {
		// check type
		if((iTypes & pWpt->GetType())) {
			// check Name
			bool bName = true; // assume true for empty filter.

			// for non-empty filter check key.
			if(bFilter) {
				bName = (pWpt->GetKey().indexOf(qsFilt)==0);
				// for airfields we also check ICAO 
				if(!bName && pWpt->GetType()==Waypoint::tAirfield) {
					bName = ((const Airfield*)pWpt)->GetICAO().indexOf(qsFilt)==0;
				}
			}

			// check radius - this call is expensive
			if(bName) {
				// Before we check radius, we will check box. This
				// check is much faster.
				if(rBox.contains(pWpt->GetCoordinate())) {
					// distance calculation is pretty slow
					float fDist = pWpt->GetDistance(ptCoor);
					if(fDist < iRadius) {
						vWpt << pWpt;
					}
				}
			}
		}
	}	

	return vWpt;
}

// -------------------------------------------------------------------------

void WaypointContainer::Insert(int iPos, Waypoint* pWpt)
{
	// check
	if(iPos > m_con.size())
		iPos=-1;
	
	// append
	if(iPos < 0)
		m_con << pWpt;
	else
		m_con.insert(iPos, pWpt);
	
	m_bDirty = true;
}

// -------------------------------------------------------------------------

void WaypointContainer::Erase(int iPos)
{
	if(iPos >=0 && iPos < m_con.size()) {
		delete m_con[iPos];
		m_con.erase(m_con.begin()+iPos);
	}
	m_bDirty = true;
}
// -------------------------------------------------------------------------
	
void WaypointContainer::Reverse()
{
	std::reverse(m_con.begin(), m_con.end());
}

// -------------------------------------------------------------------------

void WaypointContainer::CreateSectorContainer()
{
	m_sec.clear();
	
	Container::const_iterator i;
	for(i=m_con.begin(); i!=m_con.end(); i++) {
		quint16 id = (*i)->GetSectorId();
		m_sec[id] += (*i);
	}
	qDebug() << "Section Container has" << m_sec.size() << "items.";
}

// -------------------------------------------------------------------------

WaypointContainer::ConstContainer WaypointContainer::GetSectorWaypoints(quint16 uiSector) const
{
	return m_sec[uiSector];
}

// -------------------------------------------------------------------------

int WaypointContainer::LoadVFRCSV(
	const QString & qsFile		//!< The CSV file name
)
{
	int iOldSize = m_con.size();
	QFile file(qsFile);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qWarning() << "Unable to open the" << qsFile << "file.";
		return 0;
	}
	QTextStream in(&file);
	
	// Skip the first line
	QString qsLine = in.readLine();
	
	while(in.atEnd()==false) {
		qsLine = in.readLine();
		WaypointVFR* pW = new WaypointVFR(qsLine);
		if(pW->IsValid()) {
			m_con << pW;
		}
		else {
			delete pW;
			qDebug() << "Invalid VFR line:" << qsLine;
		}
	}
	return m_con.size() - iOldSize;
}
// -------------------------------------------------------------------------

int WaypointContainer::LoadNavAidsCSV(const QString& qsFile)
{
	int iOldSize = m_con.size();
	QFile file(qsFile);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qWarning() << "Unable to open the" << qsFile << "file.";
		return 0;
	}
	QTextStream in(&file);
	
	// Skip the first line
	QString qsLine = in.readLine();
	
	while(in.atEnd()==false) {
		qsLine = in.readLine();
		NavAid* pN = new NavAid(qsLine);
		if(pN->IsValid()) {
			m_con << pN;
		}
		else {
			delete pN;
			qDebug() << "Invalid NavAid line:" << qsLine;
		}
	}
	return m_con.size() - iOldSize;
}

// -------------------------------------------------------------------------
}

