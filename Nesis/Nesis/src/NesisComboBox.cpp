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
#include <QtGui>

#include "NesisListView.h"

#include "NesisComboBox.h"

// -----------------------------------------------------------------------

NesisComboBox::NesisComboBox(QWidget *pParent)
: QComboBox(pParent)
{
	m_eMode = mForwardFocus;

	NesisListView* pLV = new NesisPopupView();
	connect(this, SIGNAL(PopupMove(bool)), pLV, SLOT(OnKnob(bool)));
	setView(pLV);
}

// -----------------------------------------------------------------------

NesisComboBox::~NesisComboBox()
{
}

// -----------------------------------------------------------------------

bool NesisComboBox::HandleNesisInputEvent(PanelButton ePB)
{
	// We forward all Knob events and buttons.
	if(m_eMode == mForwardFocus) {
		switch(ePB) {
			case pbOk:
				SetEditMode();
				break;
			case pbCancel:
				emit NesisCancel();
				break;
				
			case pbKnobCW:
				emit NesisKnob(true); // true = CW
				break;
			case pbKnobCCW:
				emit NesisKnob(false);
				break;
				
			default:
				emit NesisButton(ePB-pbBtn1);
				break;
		}
		return true;
	}
	// In the edit mode we are changing the text.
	else /*(m_eMode == mEdit)*/ {
		QPalette pl;
		switch(ePB) {
			case pbOk:
				m_eMode = mForwardFocus;
				pl.setColor(QPalette::Base, QColor("white"));
				setPalette(pl);
				hidePopup();
				emit NesisOk();
				break;
			case pbCancel:
				m_eMode = mForwardFocus;
				setCurrentIndex(m_iOldIndex);
				pl.setColor(QPalette::Base, QColor("white"));
				setPalette(pl);
				hidePopup();
				break;
				
			case pbKnobCW:
				GoLineDown();
				emit PopupMove(true);
				break;
			case pbKnobCCW:
				GoLineUp();
				emit PopupMove(false);
				break;
				
			default:
			emit NesisButton(ePB-pbBtn1);
				break;
		}
		return true;
	}

	return true;
}

// -----------------------------------------------------------------------

void NesisComboBox::SetEditMode()
{
	if(m_eMode != mEdit) {
		m_eMode=mEdit;
		ShowButtons();
		m_iOldIndex = currentIndex();
		
		showPopup();
		// steel focus
		setFocus(Qt::OtherFocusReason);
		
		QPalette pl;
		pl.setBrush(QPalette::Base, palette().highlight());
		setPalette(pl);
	}
	
	update();
}

// -----------------------------------------------------------------------

QStringList NesisComboBox::GetButtonLabels() const
{
	QStringList sl;
	return sl;
}

// -----------------------------------------------------------------------

void NesisComboBox::focusInEvent(QFocusEvent* pEvent)
{
	// Call base class handler.
	QComboBox::focusInEvent(pEvent);
	ShowButtons(true);
}

// -----------------------------------------------------------------------

void NesisComboBox::focusOutEvent(QFocusEvent* pEvent)
{
	QComboBox::focusOutEvent(pEvent);
	HideButtons();
}

// -----------------------------------------------------------------------

void NesisComboBox::GoLineUp()
{
	qApp->postEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier));
}

// -----------------------------------------------------------------------

void NesisComboBox::GoLineDown()
{
	qApp->postEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier));
}

// -----------------------------------------------------------------------
