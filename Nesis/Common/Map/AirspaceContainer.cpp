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
#include <QFile>
#include <QStringList>
#include <algorithm>
#include "CommonDefs.h"
#include "CommonDefsQt.h"
#include "MathEx.h"
#include "Geometry/Polygon2D.h"
#include "Geometry/Angle.h"
#include "Map/MapDefs.h"

#include "AirspaceContainer.h"



namespace map {

// ----------------------------------------------------------------------------

AirspaceContainer::AirspaceContainer()
{
}

AirspaceContainer::~AirspaceContainer()
{
	Clear();
}

// ----------------------------------------------------------------------------

void AirspaceContainer::Clear()
{
	foreach(Airspace* pA, m_con)
		delete pA;
	m_con.clear();
}

// ----------------------------------------------------------------------------
	
int AirspaceContainer::LoadSUA(const QString& qsFile)
{
	// open the file
	QFile file(qsFile);
	if(file.open(QIODevice::ReadOnly | QIODevice::Text)==false) {
		qDebug() << "Failed to open airspace file '" << qsFile << "'.";
		return 0; 
	}
	
	// use QTextStream
	QTextStream in(&file);
	int iLine = 0;
	int iCount = 0;
	
	Airspace* pA;
	SUAParser P;
	do {
		pA = P.Parse(in, iLine);
		if(pA) {
			m_con << pA;
			iCount++;
		}
	} while(pA);
	
	return iCount;
}
// ----------------------------------------------------------------------------

int AirspaceContainer::LoadOpenAir(const QString& qsFile)
{
	// open the file
	QFile file(qsFile);
	if(file.open(QIODevice::ReadOnly | QIODevice::Text)==false) {
		qDebug() << "Failed to open airspace file '" << qsFile << "'.";
		return 0; 
	}
	
	// use QTextStream
	QTextStream in(&file);
	int iLine = 0;
	int iCount = 0;
	
	Airspace* pA;
	OAParser P;
	do {
		pA = P.Parse(in, iLine);
		if(pA) {
			m_con << pA;
			// qDebug() << pA->GetTitle();
			iCount++;
		}
	} while(pA);
	
	return iCount;
}

// ----------------------------------------------------------------------------

QVector<const Airspace*> AirspaceContainer::GetAirspaces(
	const QRectF& rc, 
   int iClassFlag, 
	int iBaseLimit
) const
{
	using namespace geometry;
	using namespace common;
// 	qDebug() << "Search Request" << Deg(rc) << iClassFlag << iBaseLimit;
	
	ConstContainer vA;
	
	// If we do not have objects in SectionContainer, we must use 
	// a full linear search over all airspaces.
	if(m_sec.isEmpty()) {
		Container::const_iterator i;
		for(i=m_con.begin(); i!=m_con.end(); i++) {
/*			qDebug() << (*i)->GetTitle() 
						<< Deg((*i)->GetBounding()) << ((*i)->GetBounding().intersects(rc))
						<< (*i)->GetClass() << (*i)->GetFloorFt();*/
			if( (*i)->IntersectsBounding(rc) 
				&& (iClassFlag & (*i)->GetClass())
				&& (*i)->GetFloorFt() <= iBaseLimit
			) {
				vA << (*i);
			}
		}
	}
	// It may be faster using SectionContainer information.
	// Actually, this is significantly faster if the database is large and
	// the search area is small. In this case it is about 250 times faster.
	// However, if the search area is large it is only about 2 times fater.
	else {
		QVector<quint16> vIds = map::GetIdsInRect(common::Deg(rc));
		
		// Build a list of airspaces.
		QVector<quint16>::const_iterator k;
		for(k=vIds.begin(); k!=vIds.end(); k++)
			vA += m_sec.value(*k);
			
		std::sort(vA.begin(), vA.end());
		ConstContainer::iterator e = std::unique(vA.begin(), vA.end());
		
		// Finaly, apply the filter on the result
		ConstContainer::iterator i;
		for(i=vA.begin(); i!=e; i++) {
// 		Write NULL over the airspaces that are not of our interest.
// 		NULLs will be removed later.
			if(!(iClassFlag & (*i)->GetClass()) || (*i)->GetFloorFt() > iBaseLimit) {
				*i = NULL;
			}
		}
		e = std::remove_if(vA.begin(), e, std::bind2nd(std::equal_to<const Airspace*>(), NULL));
		vA.erase(e,vA.end());
	}
	return vA;
}
// ----------------------------------------------------------------------------
/*
QVector<const Airspace*> AirspaceContainer::GetAirspaces(
	const QPointF& ptX, 
	int iClassFlag, 
	int iBaseLimit, 
	bool bStrict
) const
{
	using namespace geometry;
	QVector<const Airspace*> vA;
	// If we do not have objects in SectionContainer, we must use 
	// a full linear search over all airspaces.
	if(m_sec.isEmpty()) {
		Container::const_iterator i;
		for(i = m_con.begin(); i!=m_con.end(); i++) {
			if( (*i)->IsInsideBounding(ptX) 
				&& (iClassFlag & (*i)->GetClass())
				&& (*i)->GetFloorFt() <= iBaseLimit
			) {
				if(bStrict) {
					// Convert QPolygon into geometry::polygon.
					if((*i)->GetPolygon().IsInside(ptX))
						vA << *i;
				}
				else {
					vA << *i;
				}
			}
		}
	}
	// We have a valid sector container object. So, the search goes much faster.
	else {
		QPointF ptDeg = common::Deg(ptX);
		quint16 uid = map::GetIdFromLonLat((float)ptDeg.x(), (float)ptDeg.y());
		vA = m_sec.value(uid);
		// Now, search only these
		ConstContainer::iterator i;
		for(i = vA.begin(); i!=vA.end(); i++) {
			if( (*i)->IsInsideBounding(ptX) 
				&& (iClassFlag & (*i)->GetClass())
				&& (*i)->GetFloorFt() <= iBaseLimit
			) {
				if(bStrict) {
					// Convert QPolygon into geometry::polygon.
					if((*i)->GetPolygon().IsInside(ptX))
						continue;
				}
				else {
					continue;
				}
			}
			else {
				*i = NULL;
			}
		}
		i = std::remove_if(
			vA.begin(), vA.end(), 
			std::bind2nd(std::equal_to<const Airspace*>(), NULL)
		);
		vA.erase(i,vA.end());
	}
	
	return vA;
	
}
*/
// ----------------------------------------------------------------------------

void AirspaceContainer::CreateSectorContainer()
{
	m_sec.clear();
	Container::const_iterator i;
	for(i = m_con.begin(); i!=m_con.end(); i++) {
		QVector<quint16> vS = (*i)->GetSectors();
		//int iCount = vS.count();
		QVector<quint16>::const_iterator j;
		for(j=vS.begin(); j!=vS.end(); j++) {
			m_sec[*j] << *i;
		}
	}
	
	// Second loop, sort pointers in the hash
	SectionContainer::iterator h;
	for(h=m_sec.begin(); h!=m_sec.end(); h++) {
		std::sort(h->begin(), h->end());
	}
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// SUA Type airspace
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

const char* AirspaceContainer::SUAParser::s_apcKeywords[] =
{
	"INCLUDE",
	"TITLE",
	"TYPE",
	"CLASS",
	"ACTIVE",
	"RADIO",
	"TOPS",
	"BASE",
	"POINT",
	"CLOCKWISE",
	"ANTI-CLOCKWISE",
	"CIRCLE",
	"WIDTH",
	"AWY",
	"END"
};

const char* AirspaceContainer::SUAParser::s_apcTypes[][2] = 
{
	{ "CTA/CTR",			"C" },
	{ "AIRWAYS",			"A" },
	{ "RESTRICTED", 		"R" },
	{ "PROHIBITED",		"P" },
	{ "DANGER"				"D" },
	{ "OTHER",				"O" },
	{ "TRAINING ZONE"		"Z" },
	{ "TRAFFIC INFO"		"I" },
	{ "GSEC"					"G" },
	{ "MATZ"					"M" },
	{ "TMZ"					"T" },
	{ "BOUNDARY"			"B" }
};

// ----------------------------------------------------------------------------

Airspace* AirspaceContainer::SUAParser::Parse(QTextStream& in, int& iLine)
{
	// A line of text, keyword and value variables
	QString qsLine;
	QString qsKeyword;
	QString qsValue;
	
	// Assume that the include status in true by default.
	bool bInclude = true;
	
	// The main loop
	while(in.atEnd()==false) {
		iLine++;
		qsLine = in.readLine().simplified();
//  		qDebug() << qsLine;
		if(qsLine.isEmpty())
			continue;
		
		// a line with single "#" mark indicates end of airspace data.
		if(qsLine=="#") {
			if(m_A.IsValid()) {
				Airspace* pA = new Airspace(m_A);
				m_A.Clear();
/*				m_A.SetTitle(QString());
				m_A.SetFloor(QString());
				m_A.SetCeiling(QString());
				m_A.ClearPolygon();*/
				return pA;
			}
		}
		// a line which begins with "#" mark + any non-white-space character
		// indicates a comment.
		if(qsLine[0]=='#')
			continue;
		
		// Get the keyword 
		Keyword eKey = GetKeyword(qsLine);
		if(eKey == kInvalid) {
			qDebug() << "Line " << iLine 
					<< " - Failed to find a keyword in the text: " << qsLine;
			continue;
		}
		
		// Should we quit?
		else if(eKey == kEnd) {
			return NULL;
		}
		// Check for the INCLUDE 
		else if(eKey == kInclude) {
			qsValue = GetValue(qsLine);
			if(qsValue.compare("YES", Qt::CaseInsensitive)==0) {
				bInclude = true;
			}
			else if(qsValue.compare("NO", Qt::CaseInsensitive)==0) {
				bInclude = false;
			}
			else {
				qDebug() << "Line " << iLine 
						<< " - Invalid value for the keyword INCLUDE: " << qsValue;
			}
		}
		// Check if we should parse at all?
		else if(bInclude==false) {
			continue;
		}
		
		// Is it TITLE
		else if(eKey == kTitle) {
			m_A.SetTitle(GetValue(qsLine));
		} 
		// Is it TYPE
		else if(eKey == kType) {
			m_A.SetType(GetType(qsLine));
		}
		// Is it BASE / Floor
		else if(eKey == kBase) {
			m_A.SetFloor(GetValue(qsLine));
		}
		// Is it TOPS
		else if(eKey == kTops) {
			m_A.SetCeiling(GetValue(qsLine));
		}
		// Is it CLASS		
		else if(eKey == kClass) {
			m_A.SetClass(GetValue(qsLine));
		}
		// Is it ACTIVE
		else if(eKey == kActive) {
			m_A.SetActive(GetValue(qsLine));
		}
		// Is it RADIO
		else if(eKey == kRadio) {
			m_A.SetRadio(GetValue(qsLine));
		}
		// Is it POINT
		else if(eKey == kPoint) {
			m_A.Append(GetPoint(GetValue(qsLine)));
		}
		// Is it CLOCKWISE
		else if(eKey == kClockwise) {
			m_A.Append(GetArc(qsLine, true));
		}
		else if(eKey == kAntiClockwise) {
			m_A.Append(GetArc(qsLine, false));
		}
		else if(eKey == kCircle) {
			m_A.Append(GetCircle(qsLine));
		}
		else if(eKey == kWidth) {
			qDebug() << "WIDTH keyword has not been implemented yet.";
		}
		else if(eKey == kWidth) {
			qDebug() << "WIDTH keyword has not been implemented yet.";
		}
		else if(eKey == kAwy) {
			qDebug() << "AWY keyword has not been implemented yet.";
		}
		else {
			qDebug() << "Unknown keyword: " << qsLine;
		}
		
	} 
	return NULL;
}

// ----------------------------------------------------------------------------

AirspaceContainer::SUAParser::Keyword 
AirspaceContainer::SUAParser::GetKeyword(
	const QString& qsLine
) const
{
	for(unsigned int i=0; i<COUNTOF(s_apcKeywords); i++) {
		if(qsLine.indexOf(s_apcKeywords[i])==0)
			return (Keyword)i;
	}
	return kInvalid;
}

// ----------------------------------------------------------------------------

QString AirspaceContainer::SUAParser::GetValue(const QString& qsIn) const
{
	int iPos = qsIn.indexOf('=');
	if(iPos == -1)
		return QString();
	
	return qsIn.mid(iPos+1).simplified();
}
// ----------------------------------------------------------------------------

Airspace::Type AirspaceContainer::SUAParser::GetType(const QString& qsIn) const
{
	QString qsType = GetValue(qsIn);
	
	for(unsigned int i=0; i<COUNTOF(s_apcTypes); i++) {
		if(qsType.compare(s_apcTypes[i][0], Qt::CaseInsensitive)==0 ||
		   qsType.compare(s_apcTypes[i][1], Qt::CaseInsensitive)==0
		) return (Airspace::Type) i;
	}
	return Airspace::tUnknown;
}
// ----------------------------------------------------------------------------

QPointF AirspaceContainer::SUAParser::GetPoint(const QString& qsPoint) const
{
	Q_ASSERT(qsPoint.size()==16);
	float fLat = qsPoint.mid(1,2).toFloat();
	fLat      += qsPoint.mid(3,2).toFloat()/60;
	fLat      += qsPoint.mid(5,2).toFloat()/3600;
	fLat       = common::Rad(fLat);
	fLat      *= (qsPoint[0]=='N'? 1.0 : -1.0);
	
	float fLon = qsPoint.mid(9 ,3).toFloat();
	fLon      += qsPoint.mid(12,2).toFloat()/60;
	fLon      += qsPoint.mid(14,2).toFloat()/3600;
	fLon       = common::Rad(fLon);
	fLon      *= (qsPoint[8]=='E'? 1.0 : -1.0);
	
	return QPointF(fLon, fLat);
}

// ----------------------------------------------------------------------------

QPolygonF AirspaceContainer::SUAParser::GetCircle(const QString& qsIn) const
{
	// We expect to find pattern in the form: CIRCLE RADIUS=x.xx CENTRE=Nddmmss Edddmmss
	QPolygonF pg;
	
	// Locate radius
	int iR = qsIn.indexOf("RADIUS");
	// Locate centre
	int iC = qsIn.indexOf("CENTRE");
	
	// both are here
	if(iR >= 0 && iC > 0) {
		int iE = qsIn.indexOf('=', iR+6);
		float fR = 0.0f;
		if(iE < iC) {
			// Radius is given in NM
			fR = qsIn.mid(iE+1, iC-iE-1).toFloat();
			if(fR > 0.0f) {
				// Get centre point
				iE = qsIn.indexOf('=', iC+6);
				if(iE > 0) {
					QPointF ptC = GetPoint(qsIn.mid(iE+1));
					if(!ptC.isNull()) {
//						qDebug() << "Success: " << ptC;
						// Convert radius in NM into arclen.
						fR /= 3440;//(180*60); // One NM equals one minute on meradian.
						// Correction factor for longitudes assuming flat Earth with
						// a tangent plane in the center.
						float f = 1.0f/cos(ptC.y());
						// Generate points 
						for(int i=0; i<=16; i++) {
							float fA = i*M_PI*2/16.0f;
							QPointF pt(ptC);
							pt.rx() += sin(fA)*f*fR;
							pt.ry() += cos(fA)*fR;
							pg << pt;
						}
						return pg;
					}
				}
			}
		}
	}
	qDebug() << "Parsing CENTER keyword failed: " << qsIn;
	
	return pg;
}
// ----------------------------------------------------------------------------

QPolygonF AirspaceContainer::SUAParser::GetArc(const QString& qsIn, bool bClockwise) const
{
	using namespace common;
	// We expect to find pattern in the form: ...WISE RADIUS=x.xx CENTRE=Nddmmss Edddmmss TO=Nddmmss Edddmmss
	QPolygonF pg;
	
	// Locate radius
	int iR = qsIn.indexOf("RADIUS");
	// Locate centre
	int iC = qsIn.indexOf("CENTRE");
	// Locate To
	int iT = qsIn.indexOf("TO");
	
	// all are here
	if(iR >= 0 && iC > iR && iT > iC) {
		int iE = qsIn.indexOf('=', iR+6);
		float fR = 0.0f;
		if(iE < iC) {
			// Radius is given in NM
			fR = qsIn.mid(iE+1, iC-iE-1).toFloat();
			if(fR > 0.0f) {
				// Get centre point
				iE = qsIn.indexOf('=', iC+6);
				if(iE > 0) {
					QPointF ptC = GetPoint(qsIn.mid(iE+1,16));
					if(!ptC.isNull()) {
						// Get TO point
						iE = qsIn.indexOf('=', iT+2);
						if(iE > 0) {
							QPointF ptT = GetPoint(qsIn.mid(iE+1));
							QPointF ptL = m_A.GetBackPoint();
							if(!ptT.isNull() && !ptL.isNull()) {
								using namespace geometry;
// 								qDebug() << "Success: C=" << ptC << ", T=" << ptT << ", L=" << ptL;
								// Convert radius in NM into arclen.
								fR /= 3440;//(180*60); // One NM equals one minute on meradian.
								// Correction factor for longitudes assuming flat Earth with
								// a tangent plane in the center.
								float f = 1.0f/cos(ptC.y());
								
								// Start angle
								// diference in latitude
								float fdy = ptL.y()-ptC.y();
								// diference in longitude (corrected)
								float fdx = (ptL.x()-ptC.x())/f;
								// get angle (note -role of x and y are changed!)
								Angle aS(ATan2Safe(fdx,fdy));
								
								// End angle
								fdy = ptT.y()-ptC.y();
								// diference in longitude (corrected)
								fdx = (ptT.x()-ptC.x())/f;
								// get angle (note -role of x and y are changed!)
								// thus clockwise actually means counter-clockwise.
								Angle aE(ATan2Safe(fdx,fdy));
								
								float fda = Angle::GetDifference(aS, aE, bClockwise);
								// number of relative angles.
								const int ir=qMax((int) (fda/(M_PI*2) * 16), 2);
								// Generate points 
								for(int i=1; i<ir; i++) {
									float fRel = (float)i/(float)ir;
									float fA = Angle::GetRelative(fRel, aS, aE, bClockwise);
									QPointF pt(ptC);
									pt.rx() += sin(fA)*f*fR;
									pt.ry() += cos(fA)*fR;
									pg << pt;
								}
								pg << ptT;
								return pg;
							}
						}
					}
				}
			}
		}
	}
	qDebug() << "Parsing (ANTI-)CLOCKWISE keyword failed: " << qsIn;
	return pg;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Open Air type airspace
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------


const char* AirspaceContainer::OAParser::s_apcKeywords[] =
{
	"AC",		// kClass,
	"AN",		// kName,
	"AH",		// kCeiling,
	"AL",		// kFloor,
	"AT",		// kLabelCoordinate,
	"V",		// kVariable,
	"DP",		// kPoint,
	"DA",		// kArc,
	"DB",		// kArc2P
	"DC",		// kCircle,
	"DY",  	// kAirway,
	"SB",		// kSelectBrush,
	"SP",		// kSelectPen
	"TC"		// kTextColor
};

// ----------------------------------------------------------------------------

Airspace* AirspaceContainer::OAParser::Parse(QTextStream& in, int& iLine)
{
	// A line of text, keyword and value variables
	QString qsLine;
	QString qsKeyword;
	QString qsValue;
	
	m_iLine = iLine;
	
	// The main loop
	while(in.atEnd()==false) {
		m_iLine++;
		qsLine = in.readLine().simplified();
//   		qDebug() << qsLine;
		
		// Empty line terminates an airspace.
		if(qsLine.isEmpty() || qsLine=="*" 
			|| qsLine.left(2)=="**" || qsLine.left(2)=="*="
			|| qsLine.left(4)=="* >>") {
			if(m_A.IsValid()) {
				Airspace* pA = new Airspace(m_A);
				pA->Update();
				m_A.Clear();
				m_bArcDirClockwise = true;
				iLine = m_iLine;
				return pA;
			}
			else {
				m_A.Clear();
				continue;
			}
		}
		// a line which begins with "*" mark 
		// indicates a comment.
		if(qsLine[0]=='*')
			continue;
		
		// Get the keyword 
		Keyword eKey = GetKeyword(qsLine);
		if(eKey == kInvalid) {
			qDebug() << "L " << m_iLine 
					<< " - Failed to find a keyword in the text: " << qsLine;
			continue;
		}
		// Is it CLASS		
		else if(eKey == kClass) {
			m_A.SetClass(GetValue(qsLine));
		}
		
		// Is it NAME
		else if(eKey == kName) {
			if(m_A.GetTitle().isEmpty()==false) {
				qDebug() << "L " << m_iLine 
							<< "Title already exists.";
			}
			m_A.SetTitle(GetValue(qsLine));
			if(m_A.GetTitle()=="ED-R1 Garching H24")
				qsLine = "";
		} 
		// Is it FLOOR
		else if(eKey == kFloor) {
			m_A.SetFloor(GetValue(qsLine));
		}
		// Is it CEILING
		else if(eKey == kCeiling) {
			m_A.SetCeiling(GetValue(qsLine));
		}
		// Is it VARIABLE
		else if(eKey == kVariable) {
			ParseVariable(GetValue(qsLine));
		}
/*		// Is it ACTIVE
		else if(eKey == kActive) {
			m_A.SetActive(GetValue(qsLine));
		}*/
/*		// Is it RADIO
		else if(eKey == kRadio) {
			m_A.SetRadio(GetValue(qsLine));
		}*/
		// Is it POINT
		else if(eKey == kPoint) {
			m_A.Append(GetPoint(GetValue(qsLine)));
		}
		// Is it ARC
		else if(eKey == kArc) {
			m_A.Append(GetArc(GetValue(qsLine)));
		}
		// Is it ARC using 2 points
		else if(eKey == kArc2P) {
			m_A.Append(GetArc2P(GetValue(qsLine)));
		}
		else if(eKey == kCircle) {
			m_A.Append(GetCircle(GetValue(qsLine)));
		}
		else if(eKey == kLabelCoordinate) {
// 			qDebug() << "TA (Label coordinate) ignored." << qsLine;
		}
		else if(eKey == kSelectPen || eKey == kSelectBrush || eKey == kTextColor) {
// 			qDebug() << "SP, SB are ignored." << qsLine;
		}
		else {
			qDebug() << "L" << m_iLine  << "Unknown keyword: " << qsLine;
		}
		
	} 
	iLine = m_iLine;
	return NULL;
}
// ----------------------------------------------------------------------------

AirspaceContainer::OAParser::Keyword 
AirspaceContainer::OAParser::GetKeyword(const QString& qsLine) const
{
	for(unsigned int i=0; i<COUNTOF(s_apcKeywords); i++) {
		if(qsLine.indexOf(s_apcKeywords[i])==0)
			return (Keyword)i;
		}
	return kInvalid;
}

// ----------------------------------------------------------------------------

QString AirspaceContainer::OAParser::GetValue(const QString& qsIn) const
{
	int iPos = qsIn.indexOf(' ');
	if(iPos == -1)
		return QString();
	
	QString qsValue = qsIn.mid(iPos+1).simplified();
	
	// if there a comment in the string?
	iPos = qsValue.indexOf('*');
	if(iPos > 0) {
		// remove it.
		qsValue = qsValue.left(iPos).simplified();
	}
	return qsValue;
}

// ----------------------------------------------------------------------------

QPointF AirspaceContainer::OAParser::GetPoint(const QString& qsPoint) const
{
	QString qs = qsPoint.simplified().toUpper();
	qs.remove(' ');
	
	// Latitude
	int iM = 1;
	int i = qs.indexOf('N');
	if(i==-1) {
		i = qs.indexOf('S');
		if(i==-1) {
			qDebug() << "L" << m_iLine << "Invalid point :" << qsPoint;
			return QPointF();
		}
		iM = -1;
	}
	float fLat = iM*GetRadians(qs.left(i));
	qs = qs.mid(i+1);
	
	// Longitude
	iM = 1;
	i = qs.indexOf('E');
	if(i==-1) {
		i = qs.indexOf('W');
		if(i==-1) {
			qDebug() << "L" << m_iLine << "Invalid point :" << qsPoint;
			return QPointF();
		}
		iM = -1;
	}
	float fLon = iM*GetRadians(qs.left(i));
	
	return QPointF(fLon,fLat);
/*



	QStringList ql = qsPoint.split(' ');
	// We should get 4 strings ...
	if(ql.size()==4) {
		// Convert the first string into radians
		float fLat = GetRadians(ql[0]);
		// The second string is either "N" or "S"
		if(ql[1]=="S")
			fLat = -fLat;
		
		// The third string is longitude
		float fLon = GetRadians(ql[2]);
		// The fourth string is either "E" or "W".
		if(ql[3]=="W")
			fLon = -fLon;
		
		return QPointF(fLon, fLat);
	}
	// We should get 2 strings ...
	else if(ql.size()==2) {
		// Latitude
		QString qs = ql[0].simplified();
		QString qsE = qs.right(1);
		qs.chop(1);
		float fLat = GetRadians(qs);
		// The second string is either "N" or "S"
		if(qsE=="S")
			fLat = -fLat;
		
		// Longitude
		qs = ql[1].simplified();
		qsE = qs.right(1);
		qs.chop(1);
		float fLon = GetRadians(qs);
		// The second string is either "N" or "S"
		if(qsE=="W")
			fLon = -fLon;
		
		return QPointF(fLon, fLat);
	}
	// there are other possibilities, ...
	else {
		qDebug() << "Invalid point: " << qsPoint;
		return QPointF();
	}*/
}

// ----------------------------------------------------------------------------

float AirspaceContainer::OAParser::GetRadians(const QString& qsDMS) const
{
// 	qDebug() << qsDMS;
	int iM = qsDMS.indexOf(':');
	if(iM > 0) {
		float fRad = qsDMS.left(iM).toFloat();
		int iS = qsDMS.indexOf(':', iM+1);
		if(iS > 0) {
			fRad += qsDMS.mid(iM+1, iS-iM-1).toFloat()/60;
			fRad += qsDMS.mid(iS+1).toFloat()/3600;
			return common::Rad(fRad);
		}
		// Maybe there are decimal minutes.
		bool bOk = false;
		fRad += qsDMS.mid(iM+1).toFloat(&bOk)/60;
		if(bOk) {
// 			qWarning() << "L" << m_iLine << "Decimal minutes were used" << qsDMS;
			return common::Rad(fRad);
		}
	}
	// invalid value
	qCritical() << "L" << m_iLine << "Invalid coordiante" << qsDMS;
	return -999.0f;
}
// ----------------------------------------------------------------------------

void AirspaceContainer::OAParser::ParseVariable(const QString& qsV)
{
	int iE = qsV.indexOf('=');
	QString qsRight = qsV.mid(iE+1).simplified();
	
	Q_ASSERT(iE==1);
	
	// ArcDirection
	if(qsV[0]=='D') {
		// We only need sign
		if(qsV[iE+1] == '-')
			m_bArcDirClockwise = false;
		else
			m_bArcDirClockwise = true;
	}
	// Center coordinate
	else if(qsV[0]=='X') {
		m_ptX = GetPoint(qsRight);
	}
	else if(qsV[0]=='W') {
		m_fAwyWidth = qsRight.toFloat();
	}
	else if(qsV[0]=='Z') {
		m_fZoom = qsRight.toFloat();
	}
	else {
		qDebug() << "L" << m_iLine << "Invalid variable detected." << qsV;
	}
}
// ----------------------------------------------------------------------------

QPolygonF AirspaceContainer::OAParser::GetCircle(const QString& qsR) const
{
	QPolygonF pg;
	
	float fR = qsR.toFloat();
	if(fR > 0) {
		fR /= 3440;//(180*60); // One NM equals one minute on meradian.
		// Correction factor for longitudes assuming flat Earth with
		// a tangent plane in the center.
		float f = 1.0f/cos(m_ptX.y());
		// Generate points 
		for(int i=0; i<=16; i++) {
			float fA = i*M_PI*2/16.0f;
			QPointF pt(m_ptX);
			pt.rx() += sin(fA)*f*fR;
			pt.ry() += cos(fA)*fR;
			pg << pt;
		}
	}
	return pg;
}
// ----------------------------------------------------------------------------

QPolygonF AirspaceContainer::OAParser::GetArc(const QString& qsA) const
{
	// We expect to find pattern in the form: r, aS, aE
	QPolygonF pg;
	
	// Locate aS
	int iS = qsA.indexOf(',');
	if(iS > 0) {
		// Locate aE
		int iE = qsA.indexOf(',', iS+1);
		if(iE > 0) {
			using namespace geometry;
			float fR = qsA.left(iS).toFloat();
			float fS = qsA.mid(iS+1, iE-iS-1).simplified().toFloat();
			float fE = qsA.mid(iE+1).simplified().toFloat();
			
			if(fS==0 && fE==360)
				return GetCircle(QString::number(fR));
			
			Angle aS(common::Rad(fS));
			Angle aE(common::Rad(fE));
			
			// Convert radius in NM into arclen.
			fR /= 3440;//(180*60); // One NM equals one minute on meradian.
			// Correction factor for longitudes assuming flat Earth with
			// a tangent plane in the center.
			float f = 1.0f/cos(m_ptX.y());
								
			float fda = Angle::GetDifference(aS, aE, m_bArcDirClockwise);
			// number of relative angles.
			const int ir=qMax((int) (fda/(M_PI*2) * 16), 2);
			// Generate points 
			for(int i=0; i<=ir; i++) {
				float fRel = (float)i/(float)ir;
				float fA = Angle::GetRelative(fRel, aS, aE, m_bArcDirClockwise);
				QPointF pt(m_ptX);
				pt.rx() += sin(fA)*f*fR;
				pt.ry() += cos(fA)*fR;
				pg << pt;
			}
		}
	}
	return pg;
}
// ----------------------------------------------------------------------------

QPolygonF AirspaceContainer::OAParser::GetArc2P(const QString& qsA) const
{
	using namespace common;
	QPolygonF pg;
	// We expect to find pattern in the form: point1, point2
	// search for comma.
	
	int iC = qsA.indexOf(',');
	if(iC > 0) {
		QPointF ptFrom = GetPoint(qsA.left(iC).simplified());
		if(ptFrom.x() != -999.0f && ptFrom.y() != -999.0f) {
			QPointF ptTo = GetPoint(qsA.mid(iC+1).simplified());
			if(ptTo.x() != -999.0f && ptTo.y() != -999.0f) {
				using namespace geometry;
				
				float f = 1.0f/cos(m_ptX.y());
//				qDebug() << "Success: C=" << ptC << ", T=" << ptT << ", L=" << ptL;
								
				// Start angle
				// diference in latitude
				float fdy = ptFrom.y()-m_ptX.y();
				// diference in longitude (corrected)
				float fdx = (ptFrom.x()-m_ptX.x())/f;
				// get angle (note -role of x and y are changed!)
				Angle aS(ATan2Safe(fdx,fdy));
				float fR = sqrt(fdy*fdy + fdx*fdx);
								
				// End angle
				fdy = ptTo.y()-m_ptX.y();
				// diference in longitude (corrected)
				fdx = (ptTo.x()-m_ptX.x())/f;
				// get angle (note -role of x and y are changed!)
				// thus clockwise actually means counter-clockwise.
				Angle aE(ATan2Safe(fdx,fdy));
								
				float fda = Angle::GetDifference(aS, aE, m_bArcDirClockwise);
				// number of relative angles.
				const int ir=qMax((int) (fda/(M_PI*2) * 16), 2);
				
				// Generate points
				pg << ptFrom; 
				for(int i=1; i<ir; i++) {
					float fRel = (float)i/(float)ir;
					float fA = Angle::GetRelative(fRel, aS, aE, m_bArcDirClockwise);
					QPointF pt(m_ptX);
					pt.rx() += sin(fA)*f*fR;
					pt.ry() += cos(fA)*fR;
					pg << pt;
				}
				pg << ptTo;
			}
		}
	}
	return pg;
}

// ----------------------------------------------------------------------------

int AirspaceContainer::Save(const QString& qsFile) const
{
	// open the file in binary mode
	QFile file(qsFile);
	if(file.open(QIODevice::WriteOnly)==false) {
		qDebug() << "Failed to open airspace file '" << qsFile << "' for writing.";
		return 0; 
	}
	
	// use QDataStream
	QDataStream out(&file);
	
	// Write version
	out << (quint8) 1;
	// number of airspaces
	out << (quint16)m_con.count();
	// airspace
	foreach(const Airspace* pA, m_con)
		pA->Write(out);
	return m_con.count();
}

// ----------------------------------------------------------------------------

int AirspaceContainer::Load(const QString& qsFile)
{
	// Open the file in binary mode
	QFile file(qsFile);
	if(file.open(QIODevice::ReadOnly)==false) {
		qDebug() << "Failed to open airspace file '" << qsFile << "' for reading.";
		return 0; 
	}
	
	// use QDataStream
	QDataStream in(&file);
	
	// Read version
	quint8 iVer;
	in >> iVer;
	
	// number of airspaces
	quint16 iCount;
	in >> iCount;
	
	// airspace
	for(uint i=0; i<iCount; i++) {
		Airspace* pA = new Airspace;
		pA->Read(in);
		if(pA->IsValid()==false) {
			qDebug() << "Invalid airspace" << pA->GetTitle();
			delete pA;
		}
		else {
			m_con << pA;
		}
	}
	return iCount;
}

} // namespace
