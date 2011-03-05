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

// -----------------------------------------------------------------------

NesisListView::NesisListView(QWidget *pParent)
: QListView(pParent)
{
	m_eMode = mForwardFocus;
	setSelectionMode(QAbstractItemView::SingleSelection);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setResizeMode(QListView::Fixed);
	setAlternatingRowColors(true);
	QPalette pl;
	pl.setColor(QPalette::Base, QColor("white"));
	setPalette(pl);
}

// -----------------------------------------------------------------------

NesisListView::~NesisListView()
{
}

// -----------------------------------------------------------------------

bool NesisListView::HandleNesisInputEvent(PanelButton ePB)
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
		switch(ePB) {
			case pbOk:
				m_eMode = mForwardFocus;
				emit NesisOk();
				break;
			case pbCancel:
				m_eMode = mForwardFocus;
				// setCurrentRow(m_iOldIndex);
				emit NesisEditCancel();
				break;

			case pbKnobCW:
				GoLineDown();
				break;
			case pbKnobCCW:
				GoLineUp();
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

void NesisListView::SetEditMode()
{
	if(m_eMode != mEdit) {
		m_eMode = mEdit;
		ShowButtons();
		// TODO Store index somehow.
	}
}

// -----------------------------------------------------------------------

void NesisListView::focusInEvent(QFocusEvent* pEvent)
{
	// Call base class handler.
	QListView::focusInEvent(pEvent);
	ShowButtons(true);
}

// -----------------------------------------------------------------------

void NesisListView::focusOutEvent(QFocusEvent* pEvent)
{
	QListView::focusOutEvent(pEvent);
	HideButtons();
}

// -----------------------------------------------------------------------

void NesisListView::GoLineUp()
{
	qApp->postEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier));
}

// -----------------------------------------------------------------------

void NesisListView::GoLineDown()
{
	qApp->postEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier));
}

// ***********************************************************************


NesisListWidget::NesisListWidget(bool bEditModeOnly, QWidget *pParent)
	: QListWidget(pParent)
{
	m_bEditModeOnly = bEditModeOnly;
	m_eMode = mForwardFocus;
	setSelectionMode(QAbstractItemView::SingleSelection);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setResizeMode(QListView::Fixed);
	setAlternatingRowColors(true);
	
	if(m_bEditModeOnly)
		SetEditMode();	
}

// -----------------------------------------------------------------------

NesisListWidget::~NesisListWidget()
{
}

// -----------------------------------------------------------------------

bool NesisListWidget::HandleNesisInputEvent(PanelButton ePB)
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
		switch(ePB) {
			case pbOk:
				if(m_bEditModeOnly == false)
					m_eMode = mForwardFocus;
				emit NesisOk();
				break;
			case pbCancel:
				if(m_bEditModeOnly)
					emit NesisCancel();
				else {
					m_eMode = mForwardFocus;
					setCurrentRow(m_iOldIndex);
				}
				break;
				
			case pbKnobCW:
				GoLineDown();
				break;
			case pbKnobCCW:
				GoLineUp();
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

void NesisListWidget::focusInEvent(QFocusEvent* pEvent)
{
	// Call base class handler.
// 	qDebug() << "Focus in";
	QListWidget::focusInEvent(pEvent);
	ShowButtons(true);
}

// -----------------------------------------------------------------------

void NesisListWidget::focusOutEvent(QFocusEvent* pEvent)
{
// 	qDebug() << "Focus out";
	QListWidget::focusOutEvent(pEvent);
	HideButtons();
}

// -----------------------------------------------------------------------

void NesisListWidget::GoLineUp()
{
	int iNext = currentRow()-1;
	if(iNext < 0)
		iNext = count()-1;
	setCurrentRow(iNext);
}

// -----------------------------------------------------------------------

void NesisListWidget::GoLineDown()
{
	int iNext = currentRow()+1;
	if(iNext >= count())
		iNext = 0;
	setCurrentRow(iNext);
}

// -----------------------------------------------------------------------

void NesisListWidget::SetEditMode()
{
	if(m_eMode != mEdit) {
		m_eMode = mEdit;
		m_iOldIndex = currentRow();
		ShowButtons();
	}
}

// ***********************************************************************

NesisPopupView::~NesisPopupView()
{
}

// -----------------------------------------------------------------------

NesisPopupView::NesisPopupView(QWidget * pParent)
{
	SetEditMode();
}

// -----------------------------------------------------------------------

void NesisPopupView::OnKnob(bool bCW)
{
	if (bCW)
		GoLineDown();
	else
		GoLineUp();
}

// -----------------------------------------------------------------------
