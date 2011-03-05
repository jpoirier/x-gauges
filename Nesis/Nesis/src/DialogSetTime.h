#ifndef DIALOG_SET_TIME_H
#define DIALOG_SET_TIME_H

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

#include "NesisComboBox.h"
#include "AbstractNesisDialog.h"

class NesisSpinBox;
class QGridLayout;
class QFont;
class QLabel;

// --------------------------------------------------------------------------
//! Browser for the logbook.

class DialogSetTime : public AbstractNesisDialog
{
Q_OBJECT
public:
	//! Constructor
	DialogSetTime(QWidget* pParent);

	//! Destructor
	virtual ~DialogSetTime();

	//! Set focus to corresponding control, when activated.
	virtual void Activate();

private slots:
	//! External command handler.
	void OnExternalCmd(int);
	
	//! Repaint (update) the system time.
	void OnTimeUpdate();
private:
	//! Initialize local time values from the settings.
	void InitializeLocalTime();
	//! Get local time difference in minutes.
	int GetLocalTimeDifferenceMin() const;
	
	//! Close the panel and release the focus. Return true if the reboot is required.
	bool OnClose();
	//! The GPS time update was selected. Return true if the reboot is required.
	bool OnUseGPSTime();

private:
	//! Difference between UTC and local time, hours.
	NesisSpinBox* m_pspLocalTimeH;
	//! Difference between UTC and local time, minutes.
	NesisSpinBox* m_pspLocalTimeMin;
	
	//! Current UTC (system) time and date label.
	QLabel* m_plblCurrentUTC;
	//! Current LT time and date label.
	QLabel* m_plblCurrentLT;
	
	//! Current GPS time and date label.
	QLabel* m_plblGPSTime;
	//! Current GPS status (number of satellites).
	QLabel* m_plblGPSStatus;
	
	//! Manual time setting hours.
	NesisSpinBox* m_pspManualH;
	//! Manual time setting minutes.
	NesisSpinBox* m_pspManualMin;

	//! Manual date setting day.
	NesisSpinBox* m_pspManualDay;
	//! Manual date setting month.
	NesisSpinBox* m_pspManualMonth;
	//! Manual date setting year.
	NesisSpinBox* m_pspManualYear;
};


// --------------------------------------------------------------------------

#endif // DIALOG_SET_TIME_H
