#ifndef __NESIS_EDITBOX_2_H
#define __NESIS_EDITBOX_2_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2008 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   License: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

#include <QLineEdit>
#include "AbstractNesisInput.h"

// -------------------------------------------------------------------------

class QLabel;

class NesisEditBox2 : public QLineEdit, public AbstractNesisInput
{
Q_OBJECT
public:
	//! Internal mode used to process the input.
	enum Mode {
		mForwardFocus,	//!< The low level - forward mode
		mEdit				//!< Edit characted mode.
	};

public:
	//! Constructor
	NesisEditBox2(const QString& qsText, QWidget* pParent = 0);
	//! Destructor
	~NesisEditBox2();

	//! Handle the Nesis input here.
	bool HandleNesisInputEvent(PanelButton ePB);
	//! Get button labels according to current mode.
	QStringList GetButtonLabels() const;

	//! We need to show the buttons when the focus is obtained.
	void focusInEvent(QFocusEvent* pEvent);
	//! We need to hide the buttons when the focus is lost.
	void focusOutEvent(QFocusEvent* pEvent);

	//! Label buddy control displays current knob letter.
	void SetBuddy(QLabel* plBuddy)
	{ m_plBuddy = plBuddy; }

	//! Show/hide buddy, if buddy is present.
	void ShowBuddy(bool bShow=true);
	
	//! Set selectable characters.
	void SetCharacters(const QString& qs)
	{ m_qsSubset = qs; }

	//! Show/hide the buddy as well.
	virtual void ShowButtons(bool bShow=true);
	//! Set char selection mode string.
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
	//! Delete character at current cell position
	void DeleteChar();

private:
	//! Insert character (will call insert) but convert _ into space
	void Insert(const QString& qs);

protected:
	//! Current mode
	Mode m_eMode;
	//! Current character in the cell
	QChar m_chCell;

	//! Current subset of characters.
	QString m_qsSubset;
	//! Current selection level.
	int m_iLevel;
	//! Current button labels.
//	mutable QStringList m_slLabels;

	//! Buddy QLabel control, which displays current knob letter.
	QLabel* m_plBuddy;
	//! Knob index - a letter in the m_plBuddy taken from the m_qsSubset.
	int m_iKnob;
};

// -------------------------------------------------------------------------

#endif	// __NESIS_EDITBOX_H
