#ifndef __DIALOG_ALTITUDE_H
#define __DIALOG_ALTITUDE_H

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

class NesisSpinBox;

class DialogAltitude : public AbstractNesisDialog
{
Q_OBJECT
public:
	//! Constructor (takes current altitude bug in meters)
	DialogAltitude(unit::Value uvAlt, QWidget *pParent = 0);
	//! Destructor
	~DialogAltitude();
	
	//! Return altitude bug.
	const unit::Value& GetAltitude() const;
	
private:
	//! Units that we are using in this dialog.
	mutable unit::Value m_uvAlt;
	//! Thousands control.
	NesisSpinBox* m_pwThousands;	
	//! Hundred control.
	NesisSpinBox* m_pwHundreds;	
};

#endif
