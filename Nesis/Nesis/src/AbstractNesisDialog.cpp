/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2007 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   Licence: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include <QtGui>
#include <QtDebug>
#include <QApplication>
#include "CommonDefs.h"
#include "AbstractNesisDialog.h"

// -------------------------------------------------------------------------

AbstractNesisDialog::AbstractNesisDialog(QWidget *pParent)
 : QDialog(pParent, Qt::Dialog /*| Qt::CustomizeWindowHint*/)
{
	m_bMoveToTop = false;
}

AbstractNesisDialog::~AbstractNesisDialog()
{
}

// --------------------------------------------------------------------

void AbstractNesisDialog::FocusNextPrevChild(bool bNext)
{
	focusNextPrevChild(bNext);
}

// --------------------------------------------------------------------

void AbstractNesisDialog::FocusNextChild()
{
	focusNextChild();
}

// --------------------------------------------------------------------

void AbstractNesisDialog::FocusPreviousChild()
{
	focusPreviousChild();
}

// -------------------------------------------------------------------------

// TODO Read the display size constraint from some class or defines.

void AbstractNesisDialog::resizeEvent(QResizeEvent* /*pEvent*/)
{
	if(m_bMoveToTop) {
		int ix = (640 - width())/2;
		int iy = (410 - height())/2;
		if(iy<0)
			iy = 0;

		move(ix, iy);
	}
}

// -------------------------------------------------------------------------

void AbstractNesisDialog::paintEvent(QPaintEvent * pPE)
{
	QDialog::paintEvent(pPE);
	
/*	int iH = height();
	int iW = width();

	QPainter P(this);
	P.setRenderHint(QPainter::Antialiasing, true);
	QColor colPen = QColor(30, 144, 255, 200);  // blue

	QColor colBrush = QColor(176, 224, 230, 200);
	QPen pen(QBrush(colPen), 4);
	P.setPen(pen);
	P.setBrush(colBrush);
	P.drawRoundRect(2,2, iW-4,iH-4, 10,10);*/
}
