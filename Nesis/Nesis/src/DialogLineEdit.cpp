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
#include "GraphicsSingleton.h"

#include "DialogLineEdit.h"

DialogLineEdit::DialogLineEdit(
	const QString& qsLblText, 
	const QString& qsInitialInputText,
	QWidget *pParent)
 : AbstractNesisDialog(pParent)
{
	QVBoxLayout* pMainLayout = new QVBoxLayout;
	setLayout(pMainLayout);
	
	const QFont& font = GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fDialog);
	const QFont& fontBuddy = GraphicsSingleton::GetInstance()->GetFont(GraphicsSingleton::fKnobBuddy);
	
	// Label on top
	QLabel* pLbl;
	pLbl = new QLabel(qsLblText);
	pLbl->setFont(font);
	pMainLayout->addWidget(pLbl);
	
	// Edit and filter + buddy
	QHBoxLayout* pEdtLayout = new QHBoxLayout;
	pMainLayout->addLayout(pEdtLayout);
	// Edit
	m_pEdt = new NesisEditBox2(QString());
	m_pEdt->SetEditMode();
	m_pEdt->setText(qsInitialInputText);
	m_pEdt->setFont(font);
	pEdtLayout->addWidget(m_pEdt);
	// buddy
	QLabel* plBuddy = new QLabel;
	plBuddy->setFont(fontBuddy);
	plBuddy->hide();
	m_pEdt->SetBuddy(plBuddy);
	pEdtLayout->addWidget(plBuddy);

	connect(m_pEdt, SIGNAL(NesisOk()), this, SLOT(accept()));
	connect(m_pEdt, SIGNAL(NesisCancel()), this, SLOT(reject()));
}


DialogLineEdit::~DialogLineEdit()
{
}

// --------------------------------------------------------------------

QString DialogLineEdit::GetInputText() const
{ 
	return m_pEdt->text(); 
}

// --------------------------------------------------------------------
