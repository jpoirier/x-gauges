#ifndef DIALOG_SETTINGS_H
#define DIALOG_SETTINGS_H

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
class NesisSpinBox;
class NesisEditBox2;
class NesisDoubleSpinBox;
class QGridLayout;
class QFont;

// --------------------------------------------------------------------------
//! Browser for the logbook.

class DialogSettings : public AbstractNesisDialog
{
Q_OBJECT
public:
	//! Constructor
	DialogSettings(QWidget* pParent);

	//! Destructor
	virtual ~DialogSettings();

	//! Set focus to corresponding control, when activated.
	virtual void Activate();

private:
	//! Initalize list of languages.
	void InitializeLanguage();
	//! Initalize list of map types.
	void InitializeHSIMapType();
	//! Initialize units and controls;
	void InitializeUnits(QGridLayout* pUnitLayout, const QFont* pFont);

	//! Get current item data from combobox.
	int GetCurrentData(const NesisComboBox* pcb);
	//! Select correct item - accordin to the data value.
	void SelectItem(NesisComboBox* pcb, int iData);
	
private slots:
	//! External command handler.
	void OnExternalCmd(int);
	//! Close the panel and release the focus.
	void OnClose();
	//! One of the units has changed.
	void OnSetDirty();

private:
	//! Dirty flag indicates that settings were changed and need to be saved.
	bool m_bDirty;
	//! List of languages in combo box.
	NesisComboBox* m_pcbLanguage;
	
	//! Altitude units combo.
	NesisComboBox* m_pcbAltitude;
	//! Airspace altitude units combo.
	NesisComboBox* m_pcbAirspace;
	//! Speed unit combo.
	NesisComboBox* m_pcbSpeed;
	//! QNH pressure unit.
	NesisComboBox* m_pcbQNH;
	//! Vario unit.
	NesisComboBox* m_pcbVario;
	//! Distance unit.
	NesisComboBox* m_pcbDistance;
	//! Pressure unit (enigne pressures mostly)
	NesisComboBox* m_pcbPressure;
	//! Temperature unit
	NesisComboBox* m_pcbTemperature;
	//! Mass units (kg, pounds)
	NesisComboBox* m_pcbMass;
	//! Fuel units (volume);
	NesisComboBox* m_pcbFuel;
	//! FuelFlow units (volume/time).
	NesisComboBox* m_pcbFuelFlow;
	//! Wind speed units
	NesisComboBox* m_pcbWindspeed;
	
	//! HSI Map type
	NesisComboBox* m_pcbHSIMapType;
	
	//! Airplane registration
	NesisEditBox2* m_pleRegistration;
};

// --------------------------------------------------------------------------

#endif // DIALOG_SETTINGS_H
