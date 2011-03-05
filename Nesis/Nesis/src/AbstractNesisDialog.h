#ifndef __ABSTRACT_NESIS_DIALOG_H
#define __ABSTRACT_NESIS_DIALOG_H

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

#include <QtGui>

class AbstractNesisDialog : public QDialog
{
Q_OBJECT
public:
	//! Constructor
	AbstractNesisDialog(QWidget *pParent = 0);
	//! Destructor
	~AbstractNesisDialog();

	//! This will move the dialog to the top if the flag was set.
	void resizeEvent(QResizeEvent* /*pEvent*/);
protected slots:
	//! Move focus to next or previous child according to the argument.
	void FocusNextPrevChild(bool bNext);
	//! Move focus to next child.
	void FocusNextChild();
	//! Move focus to previous child.
	void FocusPreviousChild();
	
protected:
	//! Paint the widget.
	void paintEvent(QPaintEvent* pPE);
	
protected:
	//! Should we move the dialog to the top?
	bool m_bMoveToTop;
};

#endif
