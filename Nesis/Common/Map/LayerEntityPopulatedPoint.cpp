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
#include "LayerEntityPopulatedPoint.h"

namespace map {
// ----------------------------------------------------------------------------

LayerEntityPopulatedPoint::LayerEntityPopulatedPoint(unsigned int uiParameterCount)
: LayerEntityPoint(uiParameterCount)
{
}

LayerEntityPopulatedPoint::~LayerEntityPopulatedPoint()
{
}

// ----------------------------------------------------------------------------

bool LayerEntityPopulatedPoint::Read(QDataStream& in, const QByteArray& baParam)
{
	bool bRet = LayerEntityPoint::Read(in, baParam);
	// read the coordinates
	ASSERT(m_conParameter.size()==3);
	
	m_qsName      = m_conParameter[0].toString();
	m_iImportance = m_conParameter[1].toInt();
	m_bIsCity     = m_conParameter[2].toBool();

	return bRet;
}

// ----------------------------------------------------------------------------

void LayerEntityPopulatedPoint::Draw(QPainter* pP, const Mapper* pM) const
{
	// Is Area point?
	// Area is already drawn ...
	if(m_bIsCity)
		return;
	
	// Get the coordinate
	QPoint pt = pM->ConvertToImage(QPointF(m_fX, m_fY));
	QRect rc(pt, QSize(6,6));
	rc.translate(-3,-3);
	// TODO A pixmap may be faster ...
	pP->drawEllipse(rc);
}

// ----------------------------------------------------------------------------

int LayerEntityPopulatedPoint::GetUsedMemory() const
{
	int iMem = LayerEntityPoint::GetUsedMemory();
	iMem += sizeof(bool) + sizeof(int) + sizeof(QString);
	iMem += 2*m_qsName.capacity();
	return iMem; 
}

// ----------------------------------------------------------------------------
}

