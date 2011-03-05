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
#include "TextLabel.h"

namespace map {

// ----------------------------------------------------------------------------

TextLabel::TextLabel()
{
	m_iFlag = 0;
}
	
// ----------------------------------------------------------------------------

TextLabel::TextLabel(const QString& qsText, const QPointF& ptWGS, int iFlag)
{
	m_qsText = qsText;
	m_ptWGS  = ptWGS;
	m_iFlag  = iFlag;
}

// ----------------------------------------------------------------------------

TextLabel::TextLabel(const TextLabel& C)
{
	Copy(C);
}

// ----------------------------------------------------------------------------

TextLabel::~TextLabel()
{
}
	
// ----------------------------------------------------------------------------

void TextLabel::Copy(const TextLabel& C)
{
	m_qsText = C.m_qsText;
	m_ptWGS  = C.m_ptWGS;
	m_ptImg  = C.m_ptImg;
	m_iFlag  = C.m_iFlag;
}

// ----------------------------------------------------------------------------
}
