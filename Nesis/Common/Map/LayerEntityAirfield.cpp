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

#include <QtGui>
#include "Map/MapDefs.h"
#include "Map/Mapper.h"
#include "LayerEntityAirfield.h"

namespace map {
// ----------------------------------------------------------------------------

LayerEntityAirfield::LayerEntityAirfield(unsigned int uiParameterCount)
: LayerEntityPoint(uiParameterCount)
{
}

LayerEntityAirfield::~LayerEntityAirfield()
{
}

// ----------------------------------------------------------------------------

bool LayerEntityAirfield::Read(QDataStream& in, const QByteArray& baParam)
{
	using namespace route;
	if(LayerEntityPoint::Read(in, baParam)==false)
		return false;

	// read the coordinates
	ASSERT(m_conParameter.size()==12);
	
	m_qsKey  = m_conParameter[0].toString();		//  1 - key
	m_qsName = m_conParameter[1].toString();		//  2 - name
	m_qsICAO = m_conParameter[2].toString();		//  3 - icao
	m_qsFreq = m_conParameter[3].toString();		//  4 - freq
	
//	qDebug() << m_qsName;
		
	m_eRunway = (Airfield::RunwayType)m_conParameter[4].toInt(); //  5 - runway type
	m_eCategory = (Airfield::Category)m_conParameter[5].toInt(); //  6 - category
	m_iRwyDir1 = m_conParameter[6].toInt();      //  7 - direction 1 
	m_iRwyDir2 = m_conParameter[7].toInt();      //  8 - direction 2 
	m_iRwyLength = m_conParameter[8].toInt();    //  9 - length
	m_iElevation = m_conParameter[9].toInt();    // 10 - elevation
	m_iInfoFlags =  m_conParameter[10].toInt();  // 11 - info flags
	m_eCountry = (Country)m_conParameter[11].toInt(); // 12 - country
	
	return (in.status() == QDataStream::Ok);
}

// ----------------------------------------------------------------------------

int LayerEntityAirfield::GetImportance() const
{ 
	using namespace route;
	switch(m_eCategory) {
	case Airfield::catStandard:
		return 1;
	case Airfield::catGlider:
	case Airfield::catUltralight:
		return 3; 
	default:
		return 5;
	}
}

// ----------------------------------------------------------------------------

void LayerEntityAirfield::Draw(QPainter* pP, const Mapper* pM) const
{
	pP->save();
	
	QColor col = GetAirfieldColor();
	// Get the coordinate
	QPen pen(col, 1);
	pP->setPen(pen);
	pP->setBrush(Qt::transparent);
	
	QPoint pt = pM->ConvertToImage(QPointF(m_fX, m_fY));
	pP->translate(pt);
	
	// Circle ...
	QRect rcE(QPoint(-8,-8), QSize(16,16));
	pP->drawEllipse(rcE);
	
	// Runway
	QRect rcR(QPoint(-12,-2), QSize(24,4));
	if(IsCross()) {
		pP->save();
		pP->rotate(m_iRwyDir2*10-90);
		pP->drawRect(rcR);
		pP->restore();
	}
	pP->setBrush(col);
	pP->rotate(m_iRwyDir1*10-90);
	pP->drawRect(rcR);
	
	pP->restore();
}

// ----------------------------------------------------------------------------

int LayerEntityAirfield::GetUsedMemory() const
{
	int iMem = LayerEntityPoint::GetUsedMemory();
	iMem += 2*sizeof(float);
	return iMem; 
}

// ----------------------------------------------------------------------------

QColor LayerEntityAirfield::GetAirfieldColor() const
{
	using namespace route;
	
	static QColor colGrass("darkgreen");
	static QColor colOther("darkorange");
	
	switch(m_eRunway) {
		case Airfield::rwyAsphalt:
		case Airfield::rwyConcrete:
			return Qt::black;
		case Airfield::rwyGrass:
			return colGrass;
		default:
			return colOther;
	}
}

// ----------------------------------------------------------------------------

TextLabel LayerEntityAirfield::GetTextLabel() const
{
	QString qs(m_qsName);
	if(!m_qsICAO.isEmpty()) {
		qs += " [";	// used to be "\n["
		qs += m_qsICAO;
		qs += "]";
	}
	return TextLabel(qs, QPointF(m_fX, m_fY), GetImportance()); 
}

// ----------------------------------------------------------------------------

}

