#ifndef DIALOG_FUEL_LEVEL_H
#define DIALOG_FUEL_LEVEL_H

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

#include "AbstractNesisDialog.h"
#include "Unit/Value.h"

class NesisDoubleSpinBox;
class QLabel;
class QProgressBar;

// -------------------------------------------------------------------------

class DialogFuelLevel : public AbstractNesisDialog
{
Q_OBJECT
public:
	//! Constructor (takes fuel level)
	DialogFuelLevel(const unit::Value& uvQNH, QWidget *pParent = 0);
	//! Destructordouble
	~DialogFuelLevel();
	
	//! Return the fuel level in the spinbox.
	unit::Value GetFuel() const;

private slots:
	//! Update the QFE altitude
	void OnValueChanged(double dQNH);

private:
	//! Internal unit
	mutable unit::Value m_uvFuel;
	//! Store QNH settins.
	NesisDoubleSpinBox* m_pwSpin;
	//! Vertical progress bar.
	QProgressBar* m_pwBar;
};

// -------------------------------------------------------------------------

#endif	// DIALOG_FUEL_LEVEL_H
