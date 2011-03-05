#ifndef DIALOG_STARTUP_H
#define DIALOG_STARTUP_H

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
#include "AbstractNesisInput.h"
#include "Unit/Value.h"

class NesisComboBox;
class NesisDoubleSpinBox;
class QProgressBar;
class QLabel;
class QStackedLayout;

// --------------------------------------------------------------------------
//! Browser for the logbook.

class DialogStartup : public AbstractNesisDialog, public AbstractNesisInput
{
Q_OBJECT
public:
	//! Constructor
	DialogStartup(const unit::Value& uvFuel, QWidget *pParent=NULL);

	//! Destructor
	virtual ~DialogStartup();
	
	//! We need to hanle OK, Cancel keys.
	bool HandleNesisInputEvent(PanelButton pb);
	
	//! Return the fuel level in the spinbox.
	unit::Value GetFuel() const;
	//! Return selected pilot.
	QString GetPilot() const;

private slots:
	//! Update the Fuel level
	void OnValueChanged(double dLevel);
	
	//! Set focus and edit to the fuel.
	void SetFuelFocusEdit();
private:
	//! Load list of pilots.
	void InitializePilotList();

private:
	//! The layout
	QStackedLayout* m_pLayout;

	// Welcome
	//! The welcome warning displaying a VFR and Fligth planning only message.
	QLabel* m_pwlWarning;
	
	// Pilot
	//! Used to select pilots.
	NesisComboBox* m_pwcPilots;
	
	// Fuel level
	//! Internal unit
	mutable unit::Value m_uvFuel;
	//! Select fuel level using.
	NesisDoubleSpinBox* m_pwsFuel;
	//! Vertical progress bar.
	QProgressBar* m_pwBar;
};

// --------------------------------------------------------------------------

#endif // DIALOG_STARTUP_H
