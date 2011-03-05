/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2006 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include <QDataStream>
#include <QtDebug>
#include <limits>

#include "CommonDefs.h"

#include "AbstractLayerEntity.h"


namespace map {
// ----------------------------------------------------------------------------

AbstractLayerEntity::AbstractLayerEntity(unsigned int uiParameterCount)
{
	m_conParameter.resize(uiParameterCount);
}

AbstractLayerEntity::~AbstractLayerEntity()
{
}

// ----------------------------------------------------------------------------

int AbstractLayerEntity::GetUsedMemory() const
{
	int iMem = sizeof(AbstractLayerEntity);
	// let's assume 40 bytes per parameter.
	iMem += m_conParameter.size()*40;
	return iMem;
}

// ----------------------------------------------------------------------------

}
