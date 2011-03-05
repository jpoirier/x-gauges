#ifndef DIALOG_REBOOT_H
#define DIALOG_REBOOT_H

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

#include "AbstractNesisDialog.h"


class DialogReboot : public AbstractNesisDialog
{
Q_OBJECT
public:
	//! Constructor
	DialogReboot(const QString& qsDesc, int iSec=3, QWidget* pParent=NULL);
	//! Destructor
	~DialogReboot();
};

#endif
