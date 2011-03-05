#ifndef __DIALOG_LINEEDIT_H
#define __DIALOG_LINEEDIT_H

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
#include "NesisEditBox2.h"

/** \brief A class used to obtain one line of text.
 */

class DialogLineEdit : public AbstractNesisDialog
{
Q_OBJECT
public:
	//! Constructor.
	DialogLineEdit(const QString& qsLblText, 
						const QString& qsInitialInputText = QString(),
						QWidget *pParent = 0);
	//! Destructor.
   virtual ~DialogLineEdit();
	
	//! Get string that was entered.
	QString GetInputText() const;
	
private:
	//! Edit line
	NesisEditBox2* m_pEdt;
};

#endif
