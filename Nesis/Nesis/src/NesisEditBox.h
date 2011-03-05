#ifndef __NESIS_EDITBOX_H
#define __NESIS_EDITBOX_H

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

#include <QLineEdit>
#include "AbstractNesisInput.h"

// -------------------------------------------------------------------------

class QLabel;

class NesisEditBox : public QLineEdit, public AbstractNesisInput
{
Q_OBJECT
public:
	//! Internal mode used to process the input.
	enum Mode {
		mStart,		//!< The low level, start mode.
		mCell,		//!< The cell selection mode.
		mChar			//!< Character selection mode.
	};
	//! What kind of input do we allow?
	enum AllowedInput {
		aiAlpha   		= 1,		//!< Capital letters
		aiNumeric 		= 2, 		//!< Numerals
		aiSymbol  		= 4,		//!< Symbols
		aiAll     		= 7,		//!< Letters, numerals and symbols
		aiAlphaNumeric = 3 		//!< Letters and numerals but no symbols.
	};

public:
	//! Constructor
	NesisEditBox(const QString& qsText, int iAllowedInput = aiAll, Mode eMode=mStart, QWidget* pParent = 0);
	//! Destructor
	~NesisEditBox();

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

	//! Show/hide the buddy as well.
	virtual void ShowButtons(bool bShow=true);

signals:
	//! Nesis knob was pressed.
	void NesisKnob(bool bCW);
	//! Nesis Ok button was pressed.
	void NesisOk();
	//! Nesis Cancel was pressed.
	void NesisCancel();
	//! Nesis Button was pressed.
	void NesisButton(int iBtn);

protected:
	//! Split given qsSource into string list of 5 strings (to be displayed on buttons).
	QStringList SplitCharacters(const QString& qsSource) const;
	//! Delete character at current cell position
	void DeleteChar();
	//! Set char selection mode string.
	void SetCharSelectionMode(const QString& qsSelection);
	//! Set cell selection mode.
	void SetCellSelectionMode();

private:
	//! Insert character (will call insert) but convert _ into space
	void Insert(const QString& qs);

protected:
	//! Default mode.
	Mode m_eDefaultMode;
	//! Current mode
	Mode m_eMode;
	//! Allowed input
	uint m_uiAllowedInput;
	//! Current character in the cell
	QChar m_chCell;

	//! Current subset of characters.
	QString m_qsSubset;
	//! Current selection level.
	int m_iLevel;
	//! Current button labels.
	mutable QStringList m_slLabels;

	//! Buddy QLabel control, which displays current knob letter.
	QLabel* m_plBuddy;
	//! Knob index - a letter in the m_plBuddy taken from the m_qsSubset.
	int m_iKnob;
};

// -------------------------------------------------------------------------

#endif	// __NESIS_EDITBOX_H
