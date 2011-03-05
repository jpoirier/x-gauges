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
#include "LayerEntityPoint.h"

namespace map {
// ----------------------------------------------------------------------------

LayerEntityPoint::LayerEntityPoint(unsigned int uiParameterCount)
: AbstractLayerEntity(uiParameterCount)
{
}

LayerEntityPoint::~LayerEntityPoint()
{
}

// ----------------------------------------------------------------------------

bool LayerEntityPoint::Read(QDataStream& in, const QByteArray& baParam)
{
	// read the coordinates
	in >> m_fX;
	in >> m_fY;

	// read the parameters, if any present
	for(int i=0; i<baParam.size(); i++) {
		// FTString,  FTInteger, FTDouble, FTLogical, FTInvalid
		if(baParam[i] == FTString) {
			QString qsString;
			in >> qsString;
//			qDebug() << qsString;
			m_conParameter[i] = QVariant(qsString);
		}
		else if(baParam[i] == FTInteger) {
			qint32 iData;
			in >> iData;
//			qDebug() << iData;
			m_conParameter[i] = QVariant(iData);
		}
		else if(baParam[i] == FTDouble) {
			double dData;
			in >> dData;
			m_conParameter[i] = QVariant(dData);
		}
	}
	
	return (in.status() == QDataStream::Ok);
}

// ----------------------------------------------------------------------------

void LayerEntityPoint::Draw(QPainter* pP, const Mapper* pM) const
{
	
	// Get the coordinate
	QPoint pt = pM->ConvertToImage(QPointF(m_fX, m_fY));
	QRect rc(pt, QSize(6,6));
	rc.translate(-3,-3);
	pP->drawEllipse(rc);
	
//	QFont font("Trebuchet MS", 9);
/*	QFont font("Utah MT", 9);
	font.setStyleHint(QFont::SansSerif);
	font.setStyleStrategy(QFont::PreferAntialias);
	pP->setPen(QColor("black"));
	pP->setBrush(QColor("black"));

	QPainterPath myPath;
	pt += QPoint(3,-3);
	myPath.addText(pt, font, m_conParameter.front().toString());
	pP->drawPath(myPath);	
//	pP->drawText(pt, m_conParameter.front().toString());*/
}

// ----------------------------------------------------------------------------

int LayerEntityPoint::GetUsedMemory() const
{
	int iMem = AbstractLayerEntity::GetUsedMemory();
	iMem += 2*sizeof(float);
	return iMem; 
}

// ----------------------------------------------------------------------------

}

