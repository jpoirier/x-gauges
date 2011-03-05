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
#include "Airfield.h"

namespace route {

// ------------------------------------------------------------------------
	
Airfield::Airfield()
{
	m_eCategory = catUnknown;
	m_eRunway   = rwyUnknown;
	m_iLength   = 0;
	m_iDir1     = 0;
	m_iDir2     = 0;
	m_iAirfieldInfo = aiNoInfo;
}
// ------------------------------------------------------------------------

Airfield::Airfield(const Airfield& C)
: Waypoint(C)
{
	m_eCategory 		= C.m_eCategory;
	m_eRunway			= C.m_eRunway;
	m_iLength 			= C.m_iLength;
	m_iDir1				= C.m_iDir1;
	m_iDir2				= C.m_iDir2;
	m_iAirfieldInfo	= C.m_iAirfieldInfo;
	m_baFrequency		= C.m_baFrequency;
	m_baICAO				= C.m_baICAO;
}

// ------------------------------------------------------------------------

Airfield::Airfield(const QByteArray& baDataLine)
: Waypoint(baDataLine)
{
// 	qDebug() << "Airfield:" << baDataLine;
	// The base class has already parsed most of the common stuff.
	// We do arifield specific parsing here.
	m_eType = tAirfield;
	
	// It is silly, but all the airfield information is compressed into
	// 34 charactes used as "text". This truncates the airfield description
	// significantly.
	QByteArray baParse = baDataLine.mid(7,34);
// 	qDebug() << "Parse" << baParse;
	
	// We search for the # or * mark. It can be almost anywhere in the text.
	int iPos = baParse.indexOf('#');
	if(iPos == -1) 
		iPos = baParse.indexOf('*');
	// still not found
	if(iPos == -1) {
		qWarning() << "# or * not found in the airfield description : " << baDataLine;
		m_eCategory = catUnknown;
		return;
	}
	
	// Split text and data
	m_qsDesc = baParse.left(iPos).simplified();
// 	qDebug() << "Text" << m_baText;
	QByteArray baAirfiledData = baParse.mid(iPos);
	
	// Try to identify category.
	m_eCategory = IdentifyCategory(baAirfiledData, m_qsKey);
// 	qDebug() << "Category" << m_eCategory;

/*	if(m_eCategory == catUnknown) {
		qDebug() << "Unknown category: " << baDataLine; 
	}*/
	
	// Each airfield has some data (or maybe not?)
	m_iAirfieldInfo = ParseAirfieldFlags(baAirfiledData);
//	qDebug() << "Info" << m_iAirfieldInfo;

	// Get ICAO sign if available
	if(m_iAirfieldInfo & aiICAO) {
		m_baICAO = baAirfiledData.mid(1,4).simplified();
// 		qDebug() << "ICAO" << m_baICAO;
		Q_ASSERT(m_baICAO.size()>=3);
	}
/*	else {
		qDebug() << "NO ICAO!";
	}*/
	
	// Get runway type if available
	if(m_iAirfieldInfo & aiRunwayType) {
		// ACLSYGVD
// 		qDebug() << "Runway type" << baAirfiledData[5];
		switch(baAirfiledData[5]) {
			case 'A': 
				m_eRunway = rwyAsphalt;
				break;
			case 'C': 
				m_eRunway = rwyConcrete;
				break;
			case 'L': 
				m_eRunway = rwyLoam; // a soil consisting of a friable mixture 
				break;					// of varying proportions of clay, silt, and sand.
			case 'S': 
				m_eRunway = rwySand;
				break;
			case 'Y': 
				m_eRunway = rwyClay;
				break;
			case 'G': 
				m_eRunway = rwyGrass;
				break;
			case 'V': 
				m_eRunway = rwyGravel;
				break;
			case 'D': 
				m_eRunway = rwyDirt;
				break;
			default:
				m_eRunway = rwyUnknown;
				break;
		}
	}
	
	// Get length
	m_iLength = 0; // Unknown
	if(m_iAirfieldInfo & aiLength) {
		bool bOk;
		m_iLength = 10*baAirfiledData.mid(6,3).simplified().toInt(&bOk);
// 		qDebug() << "Length" << m_iLength;
		Q_ASSERT(bOk);
	}
	
	// Get direction
	m_iDir1 = m_iDir2 = 0;
	if(m_iAirfieldInfo & aiDirection) {
		bool bOk;
		m_iDir1 = baAirfiledData.mid(9,2).toInt(&bOk);
		Q_ASSERT(bOk);
		m_iDir2 = baAirfiledData.mid(11,2).toInt(&bOk);
		Q_ASSERT(bOk);
// 		qDebug() << "Direction" << m_iDir1 << m_iDir2;
	}	
	
	// Get frequency of available
	if(m_iAirfieldInfo & aiFrequency) {
		m_baFrequency = baAirfiledData.mid(13,5);	
		m_baFrequency.insert(3,'.');
//  		qDebug() << m_baFrequency;
	}
	
	// "Remove 1 from the key[6].
	if(m_qsKey.size()==6) {
		if(m_qsKey[5] == '1') {
			m_qsKey.chop(1);
		}
	}
}

// ------------------------------------------------------------------------

Airfield::~Airfield()
{
}

// ------------------------------------------------------------------------

QString Airfield::GetShortName() const
{
	if(GetICAO().isEmpty())
		return Waypoint::GetShortName();
	return GetICAO();
}

// ------------------------------------------------------------------------

Airfield::Category Airfield::IdentifyCategory(
	const QByteArray& baData, 
	const QString& qsKey
)
{
	// All outlanding fields have 2 in column 6
	if(qsKey.size()==6) {
		if(qsKey[5] == '2' || baData.left(4) == "*WIE") {
// 			qDebug() << "Outlanding" << baKey << " : " << baData;
			return catOutlanding;
		}
	}

	// check for the glider place
	if(baData.left(3) == "# S" || baData.left(3) == "* S") {
		return catGlider;
	}
	// check for the ultralight place
	if(baData.left(5) == "# ULM") {
		return catUltralight;
	}
	// check for the outlanding/ultralight place
	if(baData.left(4) == "*ULM") {
		return catUltraLightOutlanding;
	}
	
	// check for outlanding fields
	if(baData.left(3) == "*FL" || baData.left(5) == "*FELD") {
//		qDebug() << baData;
		return catOutlanding;
	}
	
	// Check which information is available for airfields
	int iAirfieldInfo = ParseAirfieldFlags(baData);
	
	// if we have ICAO
	if(iAirfieldInfo & aiICAO)
		return catStandard;
	
	// if ICAO is not known, check the length and direction
	if((iAirfieldInfo & aiLength) && (iAirfieldInfo & aiDirection)) {
		return catStandard;
	}

	// Others are identified as unknown
	return catUnknown;
}
// ------------------------------------------------------------------------

int Airfield::ParseAirfieldFlags(const QByteArray& baData)
{
	// Check which information is available for airfields
	int m_iAirfieldInfo = aiNoInfo;
		
	// check ICAO
	if(baData[0] == '#') {
		QByteArray baICAO = baData.mid(1,4).simplified();
		if(baICAO.size()>=4) {
			// all chars must be alphas.
			bool bAllAlpha = true;
			for(int i=0; i<4; i++) {
				if(!isalpha(baICAO[i])) {
					bAllAlpha = false;
					break;
				}
			}
			if(bAllAlpha)
				m_iAirfieldInfo |= aiICAO;
		}
	}

	// check the runway type (A,C,L,S,Y,G,V,D)
	static QByteArray baRwy("ACLSYGVD");
	if(baRwy.indexOf(baData[5]) != -1) {
		m_iAirfieldInfo |= aiRunwayType;
	}
/*	else 
		qDebug() << "No runway type: " << baData;*/
	
	// check the length
	bool bOk;
	int iLen = baData.mid(6,3).simplified().toInt(&bOk);
	if(iLen > 0 && bOk)
		m_iAirfieldInfo |= aiLength;
	
	// check direction
	bool bOk2;
	int iDir1 = baData.mid(9,2).simplified().toInt(&bOk);
	int iDir2 = baData.mid(11,2).simplified().toInt(&bOk2);
	if(bOk && bOk2 && (iDir1 != 0 || iDir2 != 0)) 
		m_iAirfieldInfo |= aiDirection;
/*	else {
		qDebug() << "No direction: " << baData;
		int i=4;
		i++;
	}*/
	
	// check the frequency
	int iFreq = baData.mid(13,5).toInt(&bOk);
	// Note: A lot of airfields has frequency set to 1.
	if(bOk && iFreq > 1)
		m_iAirfieldInfo |= aiFrequency;
/*	else
		qDebug() << "No freq: " << baData;*/
	
	return m_iAirfieldInfo;
}
// -------------------------------------------------------------------------

bool Airfield::IsValid() const
{
	if(Waypoint::IsValid()) {
		if(m_iLength > 100)
			return true;
	}
	return false;
}
// -------------------------------------------------------------------------

bool Airfield::AreTwoRunways() const
{
	return m_iDir1+18 != m_iDir2; 
}

// -------------------------------------------------------------------------

void Airfield::Write(QDataStream& out) const
{
	Waypoint::Write(out);
	
	// airfield save class version
	out << (qint8)1;
	
	out << (qint8) m_eCategory;
	out << (qint8) m_eRunway;
	
	out << m_iLength;
	out << m_iDir1;
	out << m_iDir2;
	
	out << m_iAirfieldInfo;
	out << m_baFrequency;
	out << m_baICAO;
}

// -------------------------------------------------------------------------

void Airfield::Read(QDataStream& in)
{
	Waypoint::Read(in);
	
	// Read the version first.
	qint8 iVer;
	in >> iVer; 
	
	m_eType = tAirfield;
	
	qint8 ienum;
	in >> ienum; m_eCategory = (Category)ienum;
	in >> ienum; m_eRunway   = (RunwayType)ienum;
	
	in >> m_iLength;
	in >> m_iDir1;
	in >> m_iDir2;
	
	in >> m_iAirfieldInfo;
	in >> m_baFrequency;
	in >> m_baICAO;
}


// ------------------------------------------------------------------------

void Airfield::Draw(QPainter& P, const map::Mapper& M) const
{
	P.save();
	
	QColor col(Qt::black);
	// Get the coordinate
	QPen pen(col, 1);
	P.setPen(pen);
	P.setBrush(Qt::transparent);
	
	QPoint pt = M.ConvertToImage(GetCoordinate());
	P.translate(pt);
	
	// Circle ...
	QRect rcE(QPoint(-8,-8), QSize(16,16));
	P.drawEllipse(rcE);
	
	// Runway
	if(IsPaved())
		P.setBrush(col);
	QRect rcR(QPoint(-12,-2), QSize(24,3));
	if(AreTwoRunways()) {
		P.save();
		P.rotate(m_iDir2*10-90);
		P.drawRect(rcR);
		P.restore();
	}
//	P.setBrush(col);
	P.rotate(m_iDir1*10-90);
	P.drawRect(rcR);
	
	P.restore();
}

// ------------------------------------------------------------------------

}

