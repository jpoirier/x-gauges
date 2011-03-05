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

#include <QtDebug>
#include <QtGui>

#include "AbstractNesisInput.h"

// --------------------------------------------------------------------

QVector<QPushButton*> AbstractNesisInput::s_conBtn;

// --------------------------------------------------------------------

AbstractNesisInput::AbstractNesisInput()
{
}


AbstractNesisInput::~AbstractNesisInput()
{
}

// --------------------------------------------------------------------

void AbstractNesisInput::ShowButtons(bool bShow)
{
	const int N = s_conBtn.count();
	if(bShow==false) {
		foreach(QPushButton* pb, s_conBtn)
			pb->hide();
	}
	else {
		QStringList sl = GetButtonLabels();
		Q_ASSERT(sl.count() <= N);
		while(sl.count() < N)
			sl << QString();
			
		// Add any external labels, if they are present.
		Q_ASSERT(m_slExternalLabels.count() <= s_conBtn.count());
		for(int i=0; i<m_slExternalLabels.size(); i++) {
			if(sl[i].isEmpty())
				sl[i] = m_slExternalLabels.at(i);
		}
	
		for(int i=0; i<s_conBtn.count(); i++) {
			s_conBtn[i]->setText(sl[i]);
			s_conBtn[i]->setVisible(sl[i].isEmpty()==false);
		}
	}
}

// --------------------------------------------------------------------
