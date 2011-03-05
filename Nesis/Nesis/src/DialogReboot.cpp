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

#include <QtGui>
#include "CommonDefs.h"
#include "GraphicsSingleton.h"
#include "Settings.h"

#include "DialogReboot.h"

// ------------------------------------------------------------------------

DialogReboot::DialogReboot(
	const QString& qsDesc, 
	int iSec, 
	QWidget* pParent
)
 : AbstractNesisDialog(pParent)
{
	GraphicsSingleton* pGS = GraphicsSingleton::GetInstance();
	QFont font = pGS->GetFont(GraphicsSingleton::fDialog);
	QFont fontSm = pGS->GetFont(GraphicsSingleton::fDialogSmall);

	// Main vertical layout
	QVBoxLayout* pLayout = new QVBoxLayout;
	setLayout(pLayout);

	// Description Label
	QLabel* pLbl = new QLabel(qsDesc);
	pLbl->setFont(font);
	pLayout->addWidget(pLbl, 0, Qt::AlignHCenter);
	
	// Reboot msg
	pLbl = new QLabel(tr("Nesis needs to reboot."));
	pLbl->setFont(font);
	pLayout->addWidget(pLbl, 0, Qt::AlignHCenter);
	
	// Timer
	QTimer::singleShot(iSec*1000, this, SLOT(accept()));
}

DialogReboot::~DialogReboot()
{
}

// ------------------------------------------------------------------------
