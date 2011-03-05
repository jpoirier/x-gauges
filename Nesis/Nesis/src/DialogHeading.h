#ifndef __DIALOG_HEADING_H
#define __DIALOG_HEADING_H

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

class NesisSpinBox;

class DialogHeading : public AbstractNesisDialog
{
Q_OBJECT
public:
	//! Constructor
	DialogHeading(int iHeading, QWidget* pParent=0);
	//! Destructor
	~DialogHeading();
	
	int GetHeading() const;


private:
	NesisSpinBox* m_pwHundreds;
	NesisSpinBox* m_pwTens;
	NesisSpinBox* m_pwOnes;
};

#endif
