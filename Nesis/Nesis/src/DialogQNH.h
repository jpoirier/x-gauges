#ifndef DIALOG_QNH_H
#define DIALOG_QNH_H

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

// -------------------------------------------------------------------------

class DialogQNH : public AbstractNesisDialog
{
Q_OBJECT
public:
	//! Constructor (takes iQNH in hPa)
	DialogQNH(const unit::Value& uvQNH, QWidget *pParent = 0);
	//! Destructor
	~DialogQNH();
	
	//! Return QNH value in the spinbox.
	unit::Value GetQNH() const;

private slots:
	//! Update the QFE altitude
	void OnQNHValueChanged(double dQNH);

private:
	//! Internal unit
	mutable unit::Value m_uvQNH;
	//! Store QNH settins.
	NesisDoubleSpinBox* m_pwSpinQNH;
	//! QFE label
	QLabel* m_pLblQFE;
};

// -------------------------------------------------------------------------

#endif	// __DIALOG_QNH_H
