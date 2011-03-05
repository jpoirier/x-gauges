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
#include "Settings.h"
#include "WidgetMain.h"
#include "GraphicsSingleton.h"
#include "NesisListView.h"

#include "PanelCheckList.h"


// -----------------------------------------------------------------------

#define TAG_CHECKLIST		"checklist"
#define TAG_ITEM				"item"
#define TAG_CAPTION			"caption"
#define TAG_CHECK				"check"
#define TAG_ACTION			"action"

#define TOP_MARGIN			5
#define BOTTOM_MARGIN		5
#define LEFT_MARGIN			9
#define RIGHT_MARGIN			9

// -----------------------------------------------------------------------
// TODO Put into separate files
// -----------------------------------------------------------------------
// Special widget class for checkslist

WidgetChecklist::WidgetChecklist(QWidget* pParent)
: QFrame(pParent)
{
	setFrameStyle(QFrame::StyledPanel | QFrame::Plain);//Raised
}

void WidgetChecklist::paintEvent(QPaintEvent *pEvent)
{
	GraphicsSingleton* pG = GraphicsSingleton::GetInstance();
	QFrame::paintEvent(pEvent);
	
	int iH = height() - (TOP_MARGIN + BOTTOM_MARGIN);
	int iW = width()  - (LEFT_MARGIN + RIGHT_MARGIN);
	
	const int n = m_slItems.count();
	
	// Get height of one line.
	const QFont& font = pG->GetFont(GraphicsSingleton::fDialog);
	QFontMetrics fm(font);
	int ifh = fm.height(); // font height
	
	// check space between lines
	int idh = qMin((iH-n*ifh)/(n-1) + ifh, 2*ifh);
	
	// Draw the items - prepare some objects
	QPainter P(this);
	P.setFont(font);
	P.translate(LEFT_MARGIN, TOP_MARGIN);
	QPen pen(Qt::DotLine);
	P.setPen(pen);
	
	QRect rc1, rc2;
	for(int i=0; i<n; i++) {
		QRect rcText(0, i*idh, iW, ifh);
		QStringList sl = m_slItems[i].split("\t");
		ASSERT(sl.count()==2);
		// Draw text left (check)
		P.drawText(rcText, Qt::AlignLeft  | Qt::AlignTop, sl[0], &rc1);
		// Draw text right (action)
		P.drawText(rcText, Qt::AlignRight | Qt::AlignTop, sl[1], &rc2);
		// Draw doted line
		if(rc2.left()-rc1.right() > 3*LEFT_MARGIN) {
			P.drawLine(
				rc1.right()+LEFT_MARGIN, i*idh + ifh/2, 
				rc2.left()-LEFT_MARGIN, i*idh + ifh/2
			);
		}
	}
}

// -----------------------------------------------------------------------

PanelChecklist::PanelChecklist(QSize sz, QWidget *pParent)
 : AbstractPanel(sz, pParent)
{
	setFixedSize(sz);

	GraphicsSingleton* pG = GraphicsSingleton::GetInstance();
	const QFont& font = pG->GetFont(GraphicsSingleton::fDialog);

	// Main layout
	QHBoxLayout* pMainLayout = new QHBoxLayout;
	setLayout(pMainLayout);

	// Check list (left column) layout
	QVBoxLayout* pCheckLayout = new QVBoxLayout;
// 	pCheckLayout->addWidget(new QLabel(tr("Checklist")));
	m_plwCheck = new NesisListWidget();
	m_plwCheck->setMaximumWidth(180);
	m_plwCheck->setFont(font);
	pCheckLayout->addWidget(m_plwCheck);
	pMainLayout->addLayout(pCheckLayout);

	QVBoxLayout* pContentLayout = new QVBoxLayout;
// 	pContentLayout->addWidget(new QLabel(tr("Content")));
	m_pwContent = new WidgetChecklist(this);
	pContentLayout->addWidget(m_pwContent);
	pContentLayout->setStretchFactor(m_pwContent, 100);
	pMainLayout->addLayout(pContentLayout);

	// Tune the layout.
	pMainLayout->setStretchFactor(pContentLayout, 100);

	// Load data from the file.
	Settings* pSettings = Settings::GetInstance();
	QFile file(pSettings->GetCheckListPath() + QString("DefaultCheck.xml"));
	QXmlInputSource inputSource(&file);
	QXmlSimpleReader reader;
	reader.setContentHandler(this);
	reader.setErrorHandler(this);
	m_pCheckItem   = NULL;
	reader.parse(inputSource);

	// Connections
	connect(
		m_plwCheck->selectionModel(),
		SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)),
		this, SLOT(OnCurrentRowChanged(const QModelIndex&, const QModelIndex&))
	);
	
	// Connect main widget with direct buttons
	connect(m_plwCheck, SIGNAL(NesisButton(int)), 
			  GetWidgetMain(), SLOT(ActivatePanel(int)));

	// Set initial selection
	m_plwCheck->setCurrentRow(0);
	
// 	qDebug() << "CL:C:Finish";
}

PanelChecklist::~PanelChecklist()
{
}
// -----------------------------------------------------------------------

void PanelChecklist::Activate()
{ 
	m_plwCheck->setFocus(Qt::OtherFocusReason);
	m_plwCheck->SetEditMode();
}

// -----------------------------------------------------------------------
void PanelChecklist::OnCurrentRowChanged(
	const QModelIndex& miNew,
	const QModelIndex& /*miOld*/
)
{
	// Display content of the checklist in the table view.
	ASSERT(miNew.row() < m_conChecklist.count());
	m_pwContent->SetItems(m_conChecklist[miNew.row()]);
}

// -----------------------------------------------------------------------

void PanelChecklist::Draw(bool /*bMajor*/)
{
	// This is not a dynamic view - we have nothing to draw here.
}

// -----------------------------------------------------------------------

bool PanelChecklist::startElement(
	const QString& /*qsNamespace*/,
	const QString& /*qsLocalName*/,
	const QString& qsName,
	const QXmlAttributes& qaAttributes
)
{
	if(qsName == TAG_CHECKLIST) {
		m_pCheckItem = new QListWidgetItem;
		m_pCheckItem->setText(qaAttributes.value(TAG_CAPTION));
// 		qDebug() << "Checklist: " << qaAttributes.value(TAG_CAPTION);
	}
	else if(qsName == TAG_ITEM) {
		QString qs;
		qs += qaAttributes.value(TAG_CHECK);
		qs += '\t';
		qs += qaAttributes.value(TAG_ACTION);
// 		qDebug() << "item = " << qs;
		m_slContent << qs;
	}

//	qDebug() << qsName;
	return true;
}

// -----------------------------------------------------------------------

bool PanelChecklist::endElement(
	const QString& /*qsNamespace*/,
	const QString& /*qsLocalName*/,
	const QString& qsName
)
{
	if(qsName == TAG_CHECKLIST) {
		if(m_pCheckItem && m_pCheckItem->text().isEmpty()==false) {
			m_plwCheck->insertItem(m_plwCheck->count(), m_pCheckItem);
			m_conChecklist << m_slContent;
			m_pCheckItem = NULL;
			m_slContent.clear();
		}
	}
	else if(qsName == TAG_ITEM) {
	}

//	qDebug() << qsName;
	return true;
}

// -----------------------------------------------------------------------

bool PanelChecklist::characters(const QString& /*qs*/)
{
//	qDebug() << "characters = " << qs;
	return true;
}

// -----------------------------------------------------------------------

bool PanelChecklist::fatalError(const QXmlParseException& /*exception*/)
{
	return false;
}

// -----------------------------------------------------------------------

