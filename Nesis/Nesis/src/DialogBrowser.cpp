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

#include "PanelMiscellaneous.h"
#include "NesisBrowser.h"

#include "DialogBrowser.h"

// -----------------------------------------------------------------------

DialogBrowser::DialogBrowser(const QUrl& urlHome, QWidget *pParent)
 : AbstractNesisDialog(pParent)
{
	m_bMoveToTop = true;
	setGeometry(0,0,620,400);

	// Main layout
	QVBoxLayout* pLayout = new QVBoxLayout;
	setLayout(pLayout);
	
	// The browser
	m_pwBrowser = new NesisBrowser;
	m_pwBrowser->setSource(urlHome);
	pLayout->addWidget(m_pwBrowser);

	// External labels for the browser widget.
	QStringList slExt;
	slExt << QString();
	slExt << QString();
	slExt << QString();
	slExt << QString();
	slExt << tr("Close");

	m_pwBrowser->SetExternalButtonLabels(slExt);

	connect(m_pwBrowser, SIGNAL(NesisButton(int)), this, SLOT(OnBrowserCmd(int)));
}

DialogBrowser::~DialogBrowser()
{
}

// -----------------------------------------------------------------------

void DialogBrowser::OnBrowserCmd(int iCmd)
{
	if(iCmd == AbstractNesisInput::pbBtn5)
		OnClose();
}

// -----------------------------------------------------------------------

void DialogBrowser::Activate()
{
	// This will also show buttons.
	setEnabled(true);
	m_pwBrowser->setFocus();
}

// -----------------------------------------------------------------------

void DialogBrowser::OnClose()
{
	accept();
}

// -----------------------------------------------------------------------
