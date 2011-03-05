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

#include <QtDebug>
#include <QtGui>

#include "GraphicsSingleton.h"
#include "Settings.h"

#include "DialogShutDown.h"

#define EVENTS_TO_SHUTDOWN			10

// -----------------------------------------------------------------------

DialogShutDown::DialogShutDown(
	QWidget *pParent
)
	: AbstractNesisDialog(pParent)
{
//  	setGeometry(180,100,280, 120);

	QVBoxLayout* pLayout = new QVBoxLayout;
	setLayout(pLayout);

	const QFont& font = GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fDialog);

	// Label
	m_pLbl = new QLabel;
	m_pLbl->setFont(font);
	pLayout->addWidget(m_pLbl);

	// Progress bar
	m_pProgress = new QProgressBar;
	pLayout->addWidget(m_pProgress);

	// Info label
	pLayout->addWidget(new QLabel(tr("Press any key to cancel.")));

	// Timer
	QTimer *pTimer = new QTimer(this);

   connect(pTimer, SIGNAL(timeout()), this, SLOT(OnUpdate()));
   pTimer->start(500);
   m_uiCounter = EVENTS_TO_SHUTDOWN;
   m_pProgress->setRange(0,EVENTS_TO_SHUTDOWN);

	resize(280,120);

	setFocus();
}

// -----------------------------------------------------------------------

DialogShutDown::~DialogShutDown()
{
}

// -----------------------------------------------------------------------

void DialogShutDown::OnUpdate()
{
	m_uiCounter--;
	if(m_uiCounter==0)
		accept();
	else {
		m_pLbl->setText(tr("Shutting down in %1 seconds").arg(m_uiCounter/2 + 1));
		m_pProgress->setValue(m_uiCounter);
	}
}

// -----------------------------------------------------------------------

bool DialogShutDown::HandleNesisInputEvent(PanelButton /*pb*/)
{
	reject();
	return true;
}

// -----------------------------------------------------------------------

QStringList DialogShutDown::GetButtonLabels() const
{
	return QStringList();
}

// -----------------------------------------------------------------------
