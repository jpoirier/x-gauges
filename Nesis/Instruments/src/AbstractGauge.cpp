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

#include <QDebug>
#include <QPainter>
#include "CommonDefsQt.h"
#include "AbstractGauge.h"

namespace instrument {

// -------------------------------------------------------------------------

AbstractGauge::AbstractGauge(QWidget* pParent)
 : QWidget(pParent)
{
	setAttribute(Qt::WA_NoSystemBackground, true);
// 	setAttribute(Qt::WA_OpaquePaintEvent, true);
}


AbstractGauge::~AbstractGauge()
{
}

// -------------------------------------------------------------------------

void AbstractGauge::resizeEvent(QResizeEvent* /* pEvent*/)
{
	ResizeBackground();
}

// -------------------------------------------------------------------------

void AbstractGauge::ResizeBackground()
{
	if(m_pxBackground.size() != size()) {
		m_pxBackground = CreateBackground(size());
		if(m_pxBackground.size() != size()) {
			m_pxBackground = m_pxBackground.scaled(
				size(), 
				Qt::IgnoreAspectRatio, 
				Qt::SmoothTransformation
			);
		}
	}
}

// -------------------------------------------------------------------------

void AbstractGauge::paintEvent(QPaintEvent* /* pEvent*/)
{
	QPainter P(this);
	DrawBackground(P);
	DrawForeground(P);
}

// -------------------------------------------------------------------------

void AbstractGauge::DrawBackground(QPainter& P)
{
	P.drawPixmap(0,0, m_pxBackground);
}

// -------------------------------------------------------------------------
/** We can't do much here. Derived classes may decide to ignore
    this.
 */
void AbstractGauge::DrawForeground(QPainter& P)
{
	P.setRenderHints(QPainter::Antialiasing, true);
	P.translate(width()/2, height()/2);
}

// -------------------------------------------------------------------------

} // namespace
