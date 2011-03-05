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

#include  <QtXml>
#include "MathEx.h"
#include "FastUnitKeys.h"
#include "CommonDefsQt.h"
#include "AbstractGauge.h"
#include "XMLTags.h"

#include "ParseHelper.h"
#include "ParameterContainer.h"
#include "Unit/Manager.h"
#include "Frame.h"


namespace instrument {

// -------------------------------------------------------------------------

Label::Label(const QDomElement& el)
{
	Q_ASSERT(el.tagName()==TAG_LABEL);
	m_iFontSize = GR_FONT_SIZE_TITLE;
	
	// check for unit first.
	m_bUnit  = false;
	m_qsText = el.attribute(TAG_UNIT);
	if(m_qsText.isEmpty()==false) {
		Parameter* pPar = ParameterContainer::GetInstance()->GetParameter(m_qsText);
		if(pPar) {
			m_bUnit = true;
			m_qsText = unit::Manager::GetInstance()->GetUnit(pPar->GetUnitKeyUser())->GetSignature();
			m_iFontSize = GR_FONT_SIZE_UNIT;
		}
	}
	else {
		m_qsText = ParseAttribute(el, TAG_TEXT);
	}
	m_fR		= ParseAttribute(el, TAG_R).toFloat();
	m_fPhi	= ParseAttribute(el, TAG_PHI).toFloat();
	
	QString qsA = el.attribute(TAG_SIZE);
	if(qsA == QLatin1String(TAG_BIG))
		m_iFontSize = GR_FONT_SIZE_TITLE_BIG;
	else if(!qsA.isEmpty())
		m_iFontSize = qsA.toInt();
		
}

// --------------------------------------------------------------------------

void Label::Draw(QPainter& P, int iS)
{
	int iX =  m_fR*iS/200*cos(common::Rad(m_fPhi));
	int iY = -m_fR*iS/200*sin(common::Rad(m_fPhi));
		
	QString qsA = m_qsText;
	
	QFont font(GR_FONT);
	font.setPixelSize(m_iFontSize);
	font.setBold(m_bUnit);
	P.setFont(font);
	P.drawText(QRect(iX-100,iY-15, 200,30), Qt::AlignCenter, qsA);
}

// --------------------------------------------------------------------------

Frame::Frame(const QDomElement& el)
{
	m_bPosKnown = false;

	Q_ASSERT(el.tagName()==TAG_FRAME);
	Q_ASSERT(!el.attribute(TAG_PARAM).isEmpty());
	
	m_pPar = ParameterContainer::GetInstance()->GetParameter(ParseAttribute(el, TAG_PARAM));
	
	// Get radius and phi
	m_fR = ParseAttribute(el,TAG_R).toFloat();
	m_fPhi = ParseAttribute(el,TAG_PHI).toFloat();
	// Get decimals
	m_iDec = el.attribute(TAG_DECIMALS).toInt();
		
	// Get width
	m_fWidth = 24; // %!
	QString qsA = el.attribute(TAG_WIDTH);
	if(qsA.isEmpty()==false)
		m_fWidth = qsA.toFloat();
}
// --------------------------------------------------------------------------

void Frame::Draw(QPainter& P, int iS)
{
	if(m_bPosKnown == false) {
		int iX =  m_fR*iS/200*cos(common::Rad(m_fPhi));
		int iY = -m_fR*iS/200*sin(common::Rad(m_fPhi));
		int iW = m_fWidth*iS/100;
		m_rc = QRect(iX-iW/2, iY-GR_LABEL_BOX_HEIGHT/2, iW, GR_LABEL_BOX_HEIGHT);
		m_bPosKnown = true;
	};
		
	m_pPar->UpdateValueCallback();
	float fV = m_pPar->GetValueUser();
	QString qsNew = common::Float2String(fV,m_iDec);
//	if(qsNew != m_qsOld) {
		// Draw background
		P.setPen(QPen(Qt::darkGray, 1.3));
		P.setBrush(QColor(GR_LABEL_BOX_BACKGROUND));
		P.drawRect(m_rc);
	
		// Draw text
		QFont font(GR_FONT, GR_LABEL_BOX_TEXT_SIZE, QFont::Bold);
		font.setPixelSize(GR_LABEL_BOX_TEXT_SIZE);
		P.setFont(font);
		QRect rc = m_rc.translated(0,1);
		P.setPen(GR_BLACK);
		P.drawText(rc, Qt::AlignCenter, qsNew);
		m_qsOld = qsNew;
//	}
	
}
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

Bar::Bar(const QDomElement& el)
{
	m_bPosKnown = false;

	Q_ASSERT(el.tagName()==TAG_BAR);
	Q_ASSERT(!el.attribute(TAG_PARAM).isEmpty());
	
	m_pPar = ParameterContainer::GetInstance()->GetParameter(
		ParseAttribute(el,TAG_PARAM)
	);
	
	// Get radius and phi
	m_fR = ParseAttribute(el, TAG_R).toFloat();
	m_fPhi = ParseAttribute(el, TAG_PHI).toFloat();
	m_fWidth = ParseAttribute(el, TAG_WIDTH).toFloat();
	m_fHeight = ParseAttribute(el, TAG_HEIGHT).toFloat();
	m_fGap = el.attribute(TAG_GAP, "0.4").toFloat();
	m_bLabel = el.attribute(TAG_TEXT, TAG_NO) == QLatin1String(TAG_YES);
	
	// Layout
	QString qsA = el.attribute(TAG_LAYOUT);
	if(qsA.isEmpty())
		qDebug() << "Missing" << TAG_LAYOUT << "bar attribute.";	
	if(qsA == QLatin1String(TAG_RIGHT_LEFT))
		m_eBarType = Bar::tRightLeft;	
	else if(qsA == QLatin1String(TAG_TOP_BOTTOM))
		m_eBarType = Bar::tTopBottom;	
	else if(qsA == QLatin1String(TAG_BOTTOM_TOP))
		m_eBarType = Bar::tBottomTop;	
	else
		m_eBarType = Bar::tLeftRight;	
}

// -------------------------------------------------------------------------

Bar::Bar(
	Type eType, 
	float fR, 
	float fPhi, 
	float fW, 
	float fH, 
	bool bLabel,
	float fGap
)
{
	m_pPar     = NULL;
	m_eBarType = eType;
	m_fR       = fR;
	m_fPhi     = fPhi;
	m_fWidth   = fW;
	m_fHeight  = fH;
	m_bLabel   = bLabel;
	m_fGap     = fGap;
}

// -------------------------------------------------------------------------

void Bar::Draw(QPainter& P, int iS)
{
	P.save();
	if(m_pxBack.isNull()) {
		CreateBackground(iS);
	}
	
	P.translate(m_rc.topLeft());
	// Draw background pixmap
	P.drawPixmap(0,0, m_pxBack);
	
	// Draw bars
	m_pPar->UpdateValueCallback();
	for(int i=0; i<m_vR.count(); i++) {
		DrawBar(P, m_pPar, m_eBarType, m_vR[i], m_pPar->GetValueSystem(i), 180);
		if(m_bLabel) 
			DrawLabel(P, m_vR[i], m_pPar->GetValueUser(i));
	}
	P.restore();
}

// --------------------------------------------------------------------------
// TODO Use this pixmap over existing pixmap and only draw bars later.
void Bar::CreateBackground(int iS)
{
	int iX =  m_fR*iS/200*cos(common::Rad(m_fPhi));
	int iY = -m_fR*iS/200*sin(common::Rad(m_fPhi));
	int iW = m_fWidth*iS/100;
	int iH = m_fHeight*iS/100;
	
	m_rc = QRect(iX-iW/2, iY-iH/2, iW, iH);
	
	// Create the background pixmap
	m_pxBack = QPixmap(iW, iH);
	m_pxBack.fill(Qt::transparent);
	QPainter P(&m_pxBack);
 	P.setRenderHints(
 		QPainter::Antialiasing |
 		QPainter::SmoothPixmapTransform |
 		QPainter::TextAntialiasing,
 		true
 	);

	P.setPen(QPen(Qt::white, 1, Qt::SolidLine/*, Qt::RoundCap, Qt::RoundJoin*/));
	QLinearGradient gBack; 
	gBack.setColorAt(0, Qt::white);
	gBack.setColorAt(0.5, Qt::gray);
	gBack.setColorAt(1, Qt::white);

	const int n = m_pPar->GetCount();
	m_vR.resize(n);
	int iT;
	int iG;
	GetGapAndT(iG, iT);
	
	for(int i=0; i<n; i++) {
		if(IsVertical()) {
			m_vR[i] = QRect(i*(iT+iG),0, iT, iH);
			gBack.setStart(m_vR[i].left(),0);
			gBack.setFinalStop(m_vR[i].right(),0);
		}
		else {
			m_vR[i] = QRect(0, i*(iT+iG), iW, iT);
			gBack.setStart(0, m_vR[i].top());
			gBack.setFinalStop(0, m_vR[i].bottom());
		}
		P.setBrush(gBack);
		P.drawRect(m_vR[i]);
		m_vR[i].adjust(1,1,-1,-1);	
	}
}
// --------------------------------------------------------------------------

void Bar::GetGapAndT(int& iGap, int& iThickness)
{
	int iS = IsVertical() ? m_rc.width() : m_rc.height();
	int n = m_pPar->GetCount();
	iThickness = qRound(iS/(n+(n-1)*m_fGap));
	iGap = n>1 ? (iS-iThickness*n)/(n-1) : 0;
}

// --------------------------------------------------------------------------

void Bar::DrawBar(
	QPainter& P, 		//!< The painter object.
	const Parameter* pPar, //!< The parameter object.
	Type eBarType,		//!< Type of bar.
	const QRect& rc,	//!< Destination rectangle.
	float fS,			//!< System value of parameter.
	int iAlpha
)
{
	QPair<float,float> R = pPar->GetRange();
	fS = qBound(R.first, fS, R.second);

	// Relative position according to the bar (scale) limits.
	float fRel = (fS-R.first)/(R.second-R.first);
	
	// Get the color
	QColor col = pPar->GetColor(fS, Qt::white);
	
	// Set the alpha chanel
	col.setAlpha(iAlpha);
	
	// Adjust rect and draw bar
	P.setBrush(QBrush(col));
	P.setPen(Qt::NoPen);
	
	QRect rc1 = rc;
	switch(eBarType) {
	case tLeftRight:
		rc1.setRight(rc1.left()+rc1.width()*fRel-1);
		break;
	case tRightLeft:
		rc1.setLeft(rc1.right()-rc1.width()*fRel+1);
		break;
	case tBottomTop:
		rc1.setTop(rc1.bottom()-rc1.height()*fRel+1);
		break;
	case tTopBottom:
		rc1.setBottom(rc1.top()+rc1.height()*fRel-1);
		break;
	}
	P.drawRect(rc1);	
}
// --------------------------------------------------------------------------

void Bar::DrawLabel(QPainter& P, const QRect& rc, float fU)
{
	P.setPen(Qt::black);
	if(IsHorizontal())
		P.drawText(rc, Qt::AlignCenter, common::Float2String(fU));
	else {
		P.save();
		P.translate(rc.center());
		P.rotate(-90.0f);
		P.drawText(
			QRect(-rc.height()/2, -rc.width()/2, rc.height(), rc.width()), 
			Qt::AlignCenter, common::Float2String(fU));
		P.restore();
	}
}

// --------------------------------------------------------------------------
}
