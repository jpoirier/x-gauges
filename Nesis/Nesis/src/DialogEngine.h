#ifndef DIALOG_ENGINE_H
#define DIALOG_ENGINE_H

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

class NesisComboBox;
class NesisEditBox2;
class NesisDoubleSpinBox;

// --------------------------------------------------------------------------
//! Browser for the logbook.

class DialogEngine : public AbstractNesisDialog
{
Q_OBJECT
public:
	//! Constructor
	DialogEngine(QWidget* pParent);

	//! Destructor
	virtual ~DialogEngine();

	//! Set focus to corresponding control, when activated.
	virtual void Activate();

private:
	//! Initalize list of engine types.
	void InitializeAirplaneEngine();
	
private slots:
	//! External command handler.
	void OnExternalCmd(int);
	//! Close the panel and release the focus without saving.
	void OnClose();
	//! Save the settings, close the panel and release the focus.
	void OnSave();

private:
	//! List of airplane-engine types in a combo box.
	NesisComboBox* m_pcbAirplaneEngine;
	//! Fuel flow correction factor.
	NesisDoubleSpinBox* m_psbFuelFlowCorrection;
	//! Fuel flow K factor.
	NesisEditBox2* m_pedFuelFlowK;
	//! RPM Divisor
	NesisDoubleSpinBox* m_psbRpmDivisor;
};

// --------------------------------------------------------------------------

#endif // DIALOG_ENGINE_H
