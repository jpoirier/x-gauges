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
#include <QApplication>
#include "WidgetMain.h"
#include "CommonDefs.h"
#include "WidgetMessage.h"

#include "AbstractPanel.h"

AbstractPanel::AbstractPanel(QSize /*sz*/, QWidget *pParent)
 : QWidget(pParent)
{
	m_eMode = mDirect;

	// We may have focus.
	setFocusPolicy(Qt::StrongFocus);

	m_pwMain = dynamic_cast<WidgetMain*>(pParent);
	ASSERT(m_pwMain);

// 	m_pMenu = new QMenu();
// 
// 	m_pMenu->setActiveAction(m_pMenu->addAction(tr("Set Fuel")));
// 	m_pMenu->addAction(tr("Waypoint"));
// 	m_pMenu->addSeparator();
// 	m_pMenu->addAction(tr("Route"));
// 	m_pMenu->addAction(tr("Set Pitch"));
// 	m_pMenu->addAction(tr("Set QNH"));
// 	m_pMenu->addSeparator();
// 	m_pMenu->addAction(tr("Route"));
}


AbstractPanel::~AbstractPanel()
{
}
// --------------------------------------------------------------------

void AbstractPanel::Activate()
{
	setFocus(Qt::OtherFocusReason);
}

// --------------------------------------------------------------------

bool AbstractPanel::HandleNesisInputEvent(PanelButton ePB)
{
	if(m_eMode==mDirect) {
		if(ePB <= pbBtn5) {
			qDebug() << "Request panel" << (ePB-pbBtn1);
			GetWidgetMain()->ActivatePanel(ePB-pbBtn1);
		}
		else if(ePB==pbOk && IsMenuAvailable()) {
			m_eMode = mMenu;
			ShowButtons();
// 			ShowMenu();
		}
		else if(IsUsingZoom() && (ePB==pbKnobCW || ePB==pbKnobCCW)) {
			bool bOk = ChangeZoom(pbKnobCW==ePB);
			if(!bOk) {
				WidgetMessage::GetInstance()->SetText(
					tr("Requested zoom level is not ready yet."),
					3,
					WidgetMessage::lWarning
				);
			}
		}
		return true;
	}
	else if(m_eMode==mMenu) {
		if(ePB==pbCancel) {
			m_eMode = mDirect;
			HideButtons();
		}
		else if(ePB <= pbBtn5) {
			OnMenuButtonPressed(ePB);
		}
//		else if (ePB==pbKnobCW)
//			qApp->postEvent(m_pMenu, new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier));
//		else if (ePB==pbKnobCCW)
//			qApp->postEvent(m_pMenu, new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier));
		
	}
	return true;
}

// --------------------------------------------------------------------

void AbstractPanel::SetDirectMode()
{
	m_eMode = mDirect;
	HideButtons();
}

// --------------------------------------------------------------------

QStringList AbstractPanel::GetButtonLabels() const
{
	QStringList sl;
 	if(m_eMode == mMenu) {
		sl = GetMenuLabels();
 	}
	return sl;
}

// --------------------------------------------------------------------

QStringList AbstractPanel::GetMenuLabels() const
{
	QStringList sl;
	sl << "Invalid";
	return sl;
}

// --------------------------------------------------------------------

void AbstractPanel::OnMenuButtonPressed(PanelButton pb)
{
	qDebug() << "Warning: Menu button" << (int)pb << "not handled.";
}

// --------------------------------------------------------------------

void AbstractPanel::SetBackgroundColor(const QBrush& brush)
{
	// Set new palette to change the background
	QPalette palette(QApplication::palette());
	palette.setBrush(QPalette::Background, brush);
	setPalette(palette);
	setAutoFillBackground(true);
}

// --------------------------------------------------------------------

// void AbstractPanel::ShowMenu()
// {
// 	QSize sz = m_pMenu->sizeHint();
// 	sz = QSize(620, 400) - sz;
// 	m_pMenu->popup(QPoint(sz.width(), sz.height()));
// // 	m_pMenu->setFocus();
// }
