#ifndef __NESIS_LIST_VIEW_H
#define __NESIS_LIST_VIEW_H

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

#include <QListView>
#include <QListWidget>
#include "AbstractNesisInput.h"

// -------------------------------------------------------------------------

class NesisListView : public QListView, public AbstractNesisInput
{
Q_OBJECT
public:
	//! Local behaviour mode of the control.
	enum Mode { 
		mForwardFocus,	//!< Default mode.
		mEdit				//!< Knob mode is active.
	};

public:
	//! Constructor
	NesisListView(QWidget* pParent = 0);
	//! Destructor
	~NesisListView();
	
	//! Handle the Nesis input here.
	bool HandleNesisInputEvent(PanelButton ePB);

	//! We need to show the buttons when the focus is obtained.
	void focusInEvent(QFocusEvent* pEvent);
	//! We need to hide the buttons when the focus is lost.
	void focusOutEvent(QFocusEvent* pEvent);
public slots:
	//! Set edit mode.
	void SetEditMode();

signals:
	//! Nesis knob was pressed.
	void NesisKnob(bool bCW);
	//! Nesis Ok button was pressed.
	void NesisOk();
	//! Nesis Cancel was pressed.
	void NesisCancel();
	//! Nesis Cancel was pressed in edit mode.
	void NesisEditCancel();
	//! Nesis Button was pressed.
	void NesisButton(int iBtn);
	
protected:
	//! One line up helper.
	void GoLineUp();
	//! One line down helper.
	void GoLineDown();

protected:
	//! Current mode
	Mode m_eMode;	
	//! Initial selection
	QModelIndex m_miOld;
};


class NesisPopupView : public NesisListView
{
Q_OBJECT
public:
	//! Constructor
	NesisPopupView(QWidget* pParent = 0);
	//! Destructor
	~NesisPopupView();

public slots:
	//! Set edit mode.
	void OnKnob(bool bCW);
};

// -------------------------------------------------------------------------

class NesisListWidget : public QListWidget, public AbstractNesisInput
{
Q_OBJECT
public:
	//! Local behaviour mode of the control.
	enum Mode { 
		mForwardFocus,	//!< Forward knob event and try to move focus to next.
		mEdit				//!< Knob mode is active.
	};

public:
	//! Constructor
	NesisListWidget(bool bEditModeOnly = false, QWidget* pParent = 0);

	//! Destructor
	~NesisListWidget();
	
	//! Handle the Nesis input here.
	bool HandleNesisInputEvent(PanelButton ePB);

	//! We need to show the buttons when the focus is obtained.
	void focusInEvent(QFocusEvent* pEvent);
	//! We need to hide the buttons when the focus is lost.
	void focusOutEvent(QFocusEvent* pEvent);

public slots:
	//! Set edit mode.
	void SetEditMode();
				
	signals:
	//! Nesis knob was pressed.
	void NesisKnob(bool bCW);
	//! Nesis Ok button was pressed.
	void NesisOk();
	//! Nesis Cancel was pressed.
	void NesisCancel();
	//! Nesis Cancel was pressed in edit mode.
	void NesisEditCancel();
	//! Nesis Button was pressed.
	void NesisButton(int iBtn);
	
protected:
	//! One line up helper.
	void GoLineUp();
	//! One line down helper.
	void GoLineDown();

protected:
	//! Current mode
	Mode m_eMode;
	//! Is edit mode the only possible mode.
	bool m_bEditModeOnly;	
	//! Old selection
	int m_iOldIndex;	
};

// -------------------------------------------------------------------------

#endif	// __NESIS_LIST_VIEW_H
