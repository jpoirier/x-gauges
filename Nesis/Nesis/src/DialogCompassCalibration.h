#ifndef DIALOG_COMPASS_CALIBRATION_H
#define DIALOG_COMPASS_CALIBRATION_H

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
#include "Matrix/VectorHeap.h"

class QLabel;
class NesisTableWidget;
class QTableWidgetItem;

#define MAGNETIC_POINTS_COUNT    8
// --------------------------------------------------------------------------

class DialogCompassCalibration : public AbstractNesisDialog
{
Q_OBJECT
public:
	//! Constructor
	DialogCompassCalibration(QWidget* pParent);

	//! Destructor
	virtual ~DialogCompassCalibration();

	//! Set focus to corresponding control, when activated.
	virtual void Activate();

private slots:
	//! Recieves Compass commands.
	void OnCompassCommand(int iCmd);

	//! Take measurements.
	void OnMeasure();
	//! Calculate calibration coefficients.
	void OnCalculate();
	//! Reset to initial values - ask first.
	void OnReset();
	//! Save data to Senap - issue a warning first.
	void OnSave();
	//! Close 
	void OnClose();

private:
	//! Table of 8 measurements.
	float m_afMeasurement[MAGNETIC_POINTS_COUNT];
	//! Solution of the XY plane magnetic field compesation.
	smx::VectorHeap<double> m_vMagSolution;

	//! A table of compass direction and results.
	NesisTableWidget* m_ptwCompass;
	//! Number of measurements taken.
	int m_iMeasureCount;
	//! A flag which tells if we can burn results.
	bool m_bValidMagCalibration;
	
	//! Coeficients label
	QLabel* m_pwLblMagReport;
};

// --------------------------------------------------------------------------

#endif // DIALOG_COMPASS_CALIBRATION_H
