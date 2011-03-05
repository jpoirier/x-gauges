#ifndef DIALOG_MOUNT_H
#define DIALOG_MOUNT_H

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

class NesisDoubleSpinBox;
class QLabel;

namespace can {
	class UnitInfoAiru;
}

// --------------------------------------------------------------------------
//! Browser for the logbook.

class DialogMount : public AbstractNesisDialog
{
Q_OBJECT
public:
	//! Constructor
	DialogMount(QWidget* pParent);

	//! Destructor
	virtual ~DialogMount();

	//! Set focus to corresponding control, when activated.
	virtual void Activate();

private:
	//! Write the selected values into airu.
	void OnSave();
	//! Close the dialog.
	void OnClose();
	
private slots:
	//! External command handler.
	void OnExternalCmd(int);
	//! Update roll and pitch from airu.
	void OnUpdateAttitude();

private:
	//! Yaw mount angle.
	NesisDoubleSpinBox* m_psbYaw;
	//! Pitch mount angle.
	NesisDoubleSpinBox* m_psbPitch;
	//! Roll mount angle.
	NesisDoubleSpinBox* m_psbRoll;
	
	//! Current indicated pitch value.
	QLabel* m_plblCurrentPitch;
	//! Current indicated roll value.
	QLabel* m_plblCurrentRoll;
};

// --------------------------------------------------------------------------
/** \brief A dialog used to enter mount corrections for the Nesis.
 */
class DialogMountCorrection : public AbstractNesisDialog
{
Q_OBJECT
public:
	//! Constructor
	DialogMountCorrection(QWidget* pParent);

	//! Destructor
	virtual ~DialogMountCorrection();

	//! Set focus to corresponding control, when activated.
	virtual void Activate();

private:
	//! Read current correction values from Airu flash.
	void ReadFlash();
	
	//! Write the selected values into airu.
	void OnSave();
	//! Close the dialog.
	void OnClose();
	//! Roll button was pressed.
	void OnRollAndPitch();
	//! Pitch button was pressed.
	void OnPitch();
	
	//! Send mount correction to airu.
	bool SendMountCorrection(float fR, float fP, float fY, bool bBurn = false);

private slots:
	//! External command handler.
	void OnExternalCmd(int);
	//! Update roll and pitch from airu.
	void OnUpdateAttitude();
	
	//! Yaw has changed (knob was turned).
	void OnYawChanged(double dYaw);

private:
	//! The airu object
	can::UnitInfoAiru* m_pA;
	//! New values for correction and bools which tell what has been changed.
	QPair<float,bool> m_afbMC[3];

	//! Current indicated yaw value.
	QLabel* m_plblCurrentYaw;
	//! Current indicated pitch value.
	QLabel* m_plblCurrentPitch;
	//! Current indicated roll value.
	QLabel* m_plblCurrentRoll;

	//! Yaw mount angle.
	NesisDoubleSpinBox* m_psbYaw;
	//! New indicated pitch value.
	QLabel* m_plblNewPitch;
	//! New indicated roll value.
	QLabel* m_plblNewRoll;
	
	//! Comment label
	QLabel* m_plblComment;
};
// --------------------------------------------------------------------------

#endif // DIALOG_MOUNT_H
