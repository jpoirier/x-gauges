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

#include "NesisTableView.h"

// -----------------------------------------------------------------------

NesisTableView::NesisTableView(QWidget *pParent)
: QTableView(pParent)
{
	m_eMode = mForwardFocus;
	setSelectionMode(QAbstractItemView::SingleSelection);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setAlternatingRowColors(true);
}

// -----------------------------------------------------------------------

NesisTableView::~NesisTableView()
{
}

// -----------------------------------------------------------------------

bool NesisTableView::HandleNesisInputEvent(PanelButton ePB)
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
				selectRow(m_miOld.row());
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

void NesisTableView::SetEditMode()
{
	if(m_eMode != mEdit) {
		m_eMode = mEdit;
		m_miOld = currentIndex();
		ShowButtons();
	}
}

// -----------------------------------------------------------------------

QStringList NesisTableView::GetButtonLabels() const
{
	QStringList sl;
	return sl;
}

// -----------------------------------------------------------------------

void NesisTableView::focusInEvent(QFocusEvent* pEvent)
{
	// Call base class handler.
	QTableView::focusInEvent(pEvent);
	ShowButtons(true);
}

// -----------------------------------------------------------------------

void NesisTableView::focusOutEvent(QFocusEvent* pEvent)
{
	QTableView::focusOutEvent(pEvent);
	HideButtons();
}

// -----------------------------------------------------------------------

void NesisTableView::GoLineUp()
{
	QModelIndex mi = currentIndex();
	selectRow(mi.row()-1);
}

// -----------------------------------------------------------------------

void NesisTableView::GoLineDown()
{
	QModelIndex mi = currentIndex();
	selectRow(mi.row()+1);
}


// -----------------------------------------------------------------------
// ***********************************************************************
// -----------------------------------------------------------------------

NesisTableWidget::NesisTableWidget(QWidget *pParent)
: QTableWidget(pParent)
{
	m_eMode = mForwardFocus;
	setSelectionMode(QAbstractItemView::SingleSelection);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setAlternatingRowColors(true);
}

// -----------------------------------------------------------------------

NesisTableWidget::~NesisTableWidget()
{
}

// -----------------------------------------------------------------------

bool NesisTableWidget::HandleNesisInputEvent(PanelButton ePB)
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
				selectRow(m_miOld.row());
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

QStringList NesisTableWidget::GetButtonLabels() const
{
	QStringList sl;
	return sl;
}

// -----------------------------------------------------------------------

void NesisTableWidget::focusInEvent(QFocusEvent* pEvent)
{
	// Call base class handler.
	QTableWidget::focusInEvent(pEvent);
	ShowButtons(true);
}

// -----------------------------------------------------------------------

void NesisTableWidget::focusOutEvent(QFocusEvent* pEvent)
{
	QTableWidget::focusOutEvent(pEvent);
	HideButtons();
}

// -----------------------------------------------------------------------

void NesisTableWidget::GoLineUp()
{
	QModelIndex mi = currentIndex();
	selectRow(mi.row()-1);
}

// -----------------------------------------------------------------------

void NesisTableWidget::GoLineDown()
{
	QModelIndex mi = currentIndex();
	selectRow(mi.row()+1);
}

// -----------------------------------------------------------------------

void NesisTableWidget::SetEditMode()
{
	if(m_eMode != mEdit) {
		m_eMode = mEdit;
		m_miOld = currentIndex();
		ShowButtons();
	}
}

// -----------------------------------------------------------------------
