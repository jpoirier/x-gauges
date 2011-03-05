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
#include <QTimer>
#include "GraphicsSingleton.h"

#include "WidgetMessage.h"

// --------------------------------------------------------------------

WidgetMessage* WidgetMessage::s_pInstance = NULL;

// --------------------------------------------------------------------

WidgetMessage::WidgetMessage(QWidget *parent)
 : QWidget(parent)
{
	m_pLblMessage = new QLabel("", this);
	m_pLblMessage->setAlignment(Qt::AlignCenter);
	GraphicsSingleton* pGS = GraphicsSingleton::GetInstance();
	m_pLblMessage->setFont(pGS->GetFont(GraphicsSingleton::fMessageText));

	m_pTimer = new QTimer();
	m_pTimer->setSingleShot(true);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(hide()));
	
//	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
}

WidgetMessage::~WidgetMessage()
{
}
// --------------------------------------------------------------------

void WidgetMessage::CreateInstance(QWidget* pParent)
{
	if(s_pInstance)
		qDebug() << "An instance of WidgetMessage already exists.";
	else
		s_pInstance = new WidgetMessage(pParent);
}

// --------------------------------------------------------------------

void WidgetMessage::resizeEvent(QResizeEvent* /*pEvent*/)
{
	m_pLblMessage->setGeometry(0,0, width(), height());
}

// --------------------------------------------------------------------

void WidgetMessage::paintEvent (QPaintEvent* /*pEvent*/)
{
	int iH = height();
	int iW = width();

	QPainter P(this);
	P.setRenderHint(QPainter::Antialiasing, true);
	QColor colPen = (m_eLevel == lNormal)  ? QColor(30, 144, 255, 200) :    // blue
			          (m_eLevel == lWarning) ? QColor(255, 215, 0, 200)  :    // gold
						    							  QColor(255, 20,  0, 200);		// red
	QColor colBrush = (m_eLevel == lNormal)  ? QColor(176, 224, 230, 200) :
                     (m_eLevel == lWarning) ? QColor(238, 232, 170, 200)  :
															 QColor(255, 127, 80, 200);
	QPen pen(QBrush(colPen), 4);
	P.setPen(pen);
	P.setBrush(colBrush);
	P.drawRoundRect(2,2, iW-4,iH-4, 10,70);
}

// --------------------------------------------------------------------

void WidgetMessage::SetText(
	const QString& csText,
	int iSeconds,
	Level eLevel)
{
	m_pLblMessage->setText(csText);
	m_eLevel = eLevel;
	show();
	m_pTimer->start(iSeconds*1000);
}

// --------------------------------------------------------------------
