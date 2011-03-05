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
#include <QPainter>
#include "Map/Mapper.h"
#include "Map/MapDefs.h"
#include "CommonDefsQt.h"
#include "MathEx.h"
#include "Geometry/Contour2D.h"

#include "Airspace.h"


namespace map {

// ----------------------------------------------------------------------------

Airspace::Airspace()
{
	m_eType  = tUnknown;
	m_eClass = cOther;
	m_iCeiling = m_iFloor = AS_SURFACE;
}

Airspace::Airspace(const Airspace& C)
{
	Copy(C);
}

Airspace::~Airspace()
{
}

// ----------------------------------------------------------------------------

void Airspace::Copy(const Airspace& C)
{
	m_pgArea = C.m_pgArea;
	m_rcBounding = C.m_rcBounding;
	
	m_qsFloor   = C.m_qsFloor;
	m_iFloor    = C.m_iFloor;
	m_qsCeiling = C.m_qsCeiling;
	m_iCeiling  = C.m_iCeiling;
	
	m_qsActive = C.m_qsActive;
	m_qsRadio  = C.m_qsRadio;
	
	m_qsTitle  = C.m_qsTitle;
	m_eType    = C.m_eType;
	m_eClass   = C.m_eClass;
}

// ----------------------------------------------------------------------------

void Airspace::Clear()
{
	m_eType  = tUnknown;
	m_eClass = cOther;

	m_pgArea.Clear();
	m_rcBounding = QRect();
	m_vSectors.clear();
	m_qsFloor.clear();
	m_qsCeiling.clear();
	m_iCeiling = m_iFloor = AS_SURFACE;
	m_qsActive.clear();
	m_qsRadio.clear();
	m_qsTitle.clear();
}
// ----------------------------------------------------------------------------

void Airspace::Append(const QPolygonF& pg)
{
	for(int i=0; i<pg.size(); i++)
		m_pgArea.Append(pg[i]);
}

// ----------------------------------------------------------------------------

bool Airspace::IsValid() const
{
	// must have a title
	if(m_qsTitle.isEmpty())
		return false;
	// must have area (two points are enough for airway)
	if(m_pgArea.GetCount() < 2)
		return false;
/*	if(m_pgArea.GetCount() > 3) {
		if(m_pgArea.IsTwisted()) {
			qDebug() << GetTitle() << "is twisted";
			return false;
		}
	}*/
	
	// must have a type
/*	if(m_eType==tUnknown)
		return false;*/
	// must have base and top
	if(m_qsFloor.isEmpty() || m_qsCeiling.isEmpty())
		return false;
	
	return true;
}

// ----------------------------------------------------------------------------
/*! First we convert the polygon points into the image coordinates.
    Then we draw the polygon using current pen and brush. The
    caller is responcible to set correct pen and brush before the 
    call. 

	 We do not draw and text here.
 */
void Airspace::Draw(QPainter* pP, const Mapper* pM) const
{
	QPen   pen(QColor(100, 149, 237, 150)); // cornflowerblue
	QBrush brush(Qt::NoBrush);
	
	if(m_eClass == cCTR) {
		pen.setStyle(Qt::DashLine);
/*		pen.setStyle(Qt::NoPen);
		pen.setColor(QColor(230,10,10,150));*/
		brush = QBrush(QColor(230,10,10,50));
	}
	else if(m_eClass == cC || m_eClass == cD) {
		pen.setColor(QColor(50, 205, 50, 150));
	}
	else if(m_eClass == cR || m_eClass == cGP) {
		brush = QBrush(QColor(100, 149, 237, 50));
	}
	
	int fW = qMax((int)(4*pM->GetScale()* 1000000.0f + 0.5), 2);
	
	pen.setWidth((int)fW);
	
	pP->setPen(pen);
	pP->setBrush(brush);
	
	const int iCount = m_pgArea.GetCount();
	if(iCount == 2) {
		pP->drawLine(
			pM->ConvertToImage(m_pgArea.GetVertex(0)),
			pM->ConvertToImage(m_pgArea.GetVertex(1))
		);
	}
	else {
		QPolygon pg(iCount);
		for(int i=0; i<iCount; i++) {
			pg[i] = pM->ConvertToImage(m_pgArea.GetVertex(i));
		}
		pP->drawPolygon(pg);
	}
}

// ----------------------------------------------------------------------------

QPointF Airspace::GetBackPoint() const
{
	if(m_pgArea.IsEmpty())
		return QPointF();
	
	return m_pgArea.GetBack();
}
// ----------------------------------------------------------------------------

void Airspace::Update()
{
	using namespace map;
	using namespace common;
	
/*	if(GetTitle()=="ED-R1 Garching H24") {
		std::cout << m_pgArea;
	}*/
	
	m_rcBounding = m_pgArea.GetSurroundingRectangle();
	
	// build the list of blocks inside the rectangle.
	QRectF rc(m_rcBounding.topLeft()*(180/M_PI), m_rcBounding.size()*(180/M_PI));
	
/*	if(rc.isValid()==false) {
		qDebug() << "Invalid bounding rect" << GetTitle();
		qDebug() << "RC=LRTB" << rc.left() << rc.right() << rc.top() << rc.bottom()
					<< ":" << rc;
	}*/
	
	if(rc.left()<-180 || rc.right()>180 || rc.top()<-90 || rc.bottom()>90) {
		qDebug() << "Invalid bounding rect values" << GetTitle();
		qDebug() << "RC=LRTB" << rc.left() << rc.right() << rc.top() << rc.bottom()
					<< ":" << rc;
	}
	
	m_vSectors = map::GetIdsInRect(rc);
	Q_ASSERT(m_vSectors.count() > 0);
	
	// Check, block by block. There may be blocks which are
	// not in the airspace polygon.
	QVector<quint16>::iterator i = m_vSectors.begin(); 
	while(i!=m_vSectors.end()) {
		if(IntersectsPolygon(Rad(GetRectFromId(*i)))==false)
			i = m_vSectors.erase(i);
		else
			i++;	
	}
	if(m_vSectors.isEmpty()) {
		qCritical() << "No sectors:" << GetTitle() << m_rcBounding; 
	}
	
	if(m_vSectors.capacity()-m_vSectors.count() > 5) {
		m_vSectors.squeeze();
	}
}

// ----------------------------------------------------------------------------

void Airspace::SetClass(const QString& qsClass)
{
	QStringList sl = qsClass.split(' ');
	QString qsC = sl[0];
	if(qsC.size()==1) {
		const char chC = qsC[0].toAscii();
		switch (chC) {
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
				m_eClass = (Class)(0x01 << (chC - 'A'));
				break;
			case 'R':
				m_eClass = cRestricted;
				break;
			case 'Q':
				m_eClass = cDanger;
				break;
			case 'P':
				m_eClass = cProhibited;
				break;
			case 'W':
				m_eClass = cWaveWindow;
				break;
			default:
				break;
		}
	}
	else if(qsC=="CTR")
		m_eClass = cCTR;
	else if(qsC=="TMZ")
		m_eClass = cCTR;
	else if(qsC=="GP")
		m_eClass = cGP;
	else {
		qDebug() << "Unknown Class: " << qsC;
		m_eClass = cOther;
	}
	
	if(sl.count() > 1) {
		qDebug() << "Suspisious cmd AC:" << qsClass;
	}
	
}
// ----------------------------------------------------------------------------

void Airspace::SetFloor(const QString& qsFloor)
{
	m_qsFloor = qsFloor;
	m_iFloor  = ParseAltitude(qsFloor);
}
// ----------------------------------------------------------------------------

void Airspace::SetCeiling(const QString& qsCeiling)
{
	m_qsCeiling = qsCeiling;
	m_iCeiling  = ParseAltitude(qsCeiling);
}
// ----------------------------------------------------------------------------

int Airspace::ParseAltitude(const QString& qsAltitude)
{
	QString qsAlt = qsAltitude;
// 	qDebug() << qsAlt;
		
	// altitude can be a lot of things.
	
	// empty
	if(qsAlt.isEmpty())
		return 0;
	
	// ground, surface
	if(qsAlt.compare("SFC", Qt::CaseInsensitive)==0 ||
		qsAlt.compare("MSL", Qt::CaseInsensitive)==0 ||
		qsAlt.compare("GND", Qt::CaseInsensitive)==0 ) {
		return AS_SURFACE; // indicates 0 AGL
	}
	
	// check for unlimited.
	if(qsAlt.compare("UNLTD", Qt::CaseInsensitive)==0 ||
		qsAlt.compare("UNLIM", Qt::CaseInsensitive)==0 ||
		qsAlt.compare("UNL", Qt::CaseInsensitive)==0 ) {
		return AS_UNLIMITED;
	}
	
	// check for NOTAM
	if(qsAlt.compare("NOTAM", Qt::CaseInsensitive)==0) {
		return AS_NOTAM;
	}
	
	// remove all spaces in the string.
	qsAlt.remove(' ');
	// Check for flight level
	int iF = qsAlt.indexOf("FL", 0, Qt::CaseInsensitive);
	if(iF>=0) {
		return qsAlt.mid(iF+2).toInt() * 100;
	}
	
	// Check for AGL (Above ground level)
	iF = qsAlt.indexOf("AGL", 0, Qt::CaseInsensitive);
	if(iF > 0) {
		return -qsAlt.left(iF).toInt();
	}
	iF = qsAlt.indexOf("GND", 0, Qt::CaseInsensitive);
	if(iF > 0) {
		return -qsAlt.left(iF).toInt();
	}
	
	// Check for AMSL (Mean Sea Level)
	iF = qsAlt.indexOf("AMSL", 0, Qt::CaseInsensitive);
	if(iF > 0) {
		return qsAlt.left(iF).toInt();
	}
	// Check for MSL (Mean Sea Level)
	iF = qsAlt.indexOf("MSL", 0, Qt::CaseInsensitive);
	if(iF > 0) {
		return qsAlt.left(iF).toInt();
	}
	
	// Check for F AMSL (Mean Sea Level)
	iF = qsAlt.indexOf("FAMSL", 0, Qt::CaseInsensitive);
	if(iF > 0) {
		return qsAlt.left(iF).toInt();
	}
	
	// ALT extension
	iF = qsAlt.indexOf("ALT", 0, Qt::CaseInsensitive);
	if(iF > 0) {
		return qsAlt.left(iF).toInt();
	}
	
	// Check for FEET
	iF = qsAlt.indexOf("FT", 0, Qt::CaseInsensitive);
	if(iF>=0) {
		return qsAlt.left(iF).toInt();
	}
	iF = qsAlt.indexOf("F", 0, Qt::CaseInsensitive);
	if(iF>=0) {
		return qsAlt.left(iF).toInt();
	}
	
	// default - assume MSL
 	qDebug() << "Warning: Default conversion for Airspace altitude ->" << qsAltitude;
	bool bOk;
	int iAlt = qsAlt.toInt(&bOk);
	Q_ASSERT(bOk);
	return iAlt;
}

// ----------------------------------------------------------------------------

void Airspace::Write(QDataStream& out) const
{
	out << (qint8)1; // Version
	
	out << m_pgArea;
	out << m_rcBounding;
	
	out << m_qsFloor;
	out << m_iFloor;
	
	out << m_qsCeiling;
	out << m_iCeiling;
	
	out << m_qsActive;
	out << m_qsRadio;
	
	out << m_qsTitle;
	out << (quint32) m_eType;
	out << (quint32) m_eClass;
}

// ----------------------------------------------------------------------------

void Airspace::Read(QDataStream& in)
{
	qint8 iVer;
	in >> iVer; 
	
	in >> m_pgArea;
	in >> m_rcBounding;
	
	in >> m_qsFloor;
	in >> m_iFloor;
	
	in >> m_qsCeiling;
	in >> m_iCeiling;
	
	in >> m_qsActive;
	in >> m_qsRadio;
	
	in >> m_qsTitle;
	quint32 uiEnum;
	in >> uiEnum; m_eType = static_cast<Type>(uiEnum);
	in >> uiEnum; m_eClass = static_cast<Class>(uiEnum);
	
	// Update will build the list of blocks (sectors) used by the polygon.
	Update();
}

// ----------------------------------------------------------------------------
	
bool Airspace::IsInside(
	const QPointF& ptX, 			//!< The point to be checked (radians).
	bool bCheckBoundingFirst	//!< Should we perform the bounding check first?
) const
{
	// This may speed up, but sometimes it is not necessary.
	if(bCheckBoundingFirst) {
		if(IsInsideBounding(ptX) == false)
			return false;
	}
	
	// The polygon version.
	return m_pgArea.IsInside(ptX);
}

// ----------------------------------------------------------------------------
	
bool Airspace::IntersectsPolygon(
	const QRectF& rc, 			//!< The rect to be checked (radians).
	bool bCheckBoundingFirst	//!< Should we perform the bounding check first?
) const
{
//	if(GetTitle()=="D118A Belfort") {
//	if(GetTitle()=="SH-S: 124.7") {
/*	if(GetTitle()=="HOEVENEN PARA") {
		qDebug() << m_rcBounding << rc << m_pgArea.IsTwisted();
		std::cout << m_pgArea;
	}*/
	
	Q_ASSERT(rc.isValid());
	// This may speed up, but sometimes it is not necessary.
	if(bCheckBoundingFirst && m_rcBounding.isValid()) {
		if(IntersectsBounding(rc) == false)
			return false;
	}
	
	using namespace geometry;
	Polygon2D pgR(rc);
	// If we have a valid polygon ...
	if(m_pgArea.GetCount() > 2) {
		// ... check the intersection of the polygon and rectangle.
		Contour2D cR(pgR);		// Rectangle contour
		Contour2D cA(m_pgArea); // Polygon contour
		// The intersection (pcX)
		Contour2D* pcX = cA.Clip(Contour2D::cIntersection, cR);
		bool bYes = pcX->GetCount() > 0; // Is it non-empty.
		delete pcX;
		return bYes;
	}
	// We do not have a true polygon, but merely a line.
	else {
		// Check the line intersections trough the rect, but
		// return only points that are inside the line.
		Line2D l=m_pgArea.GetLine(0);
		QVector<Point2D> v = pgR.GetIntersection(l, true, false);
		// If at least one such point is found, then we know that at least 
		// part of the line is inside the rect.
		if(v.isEmpty()==false) {
			return true;
		}
		// The line is either completely outside or completely inside the rect.
		else {
			return pgR.IsInside(l.GetA(), false);
		}
	}	
	

/*	if(m_pgArea.IsInside(rc.topLeft()))
		return true;
	if(m_pgArea.IsInside(rc.topRight()))
		return true;
	if(m_pgArea.IsInside(rc.bottomLeft()))
		return true;
	if(m_pgArea.IsInside(rc.bottomRight()))
		return true;*/
		
	return false;
}

// ----------------------------------------------------------------------------
} // namespace
