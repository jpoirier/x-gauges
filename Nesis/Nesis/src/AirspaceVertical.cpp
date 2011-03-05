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
#include <QPainter>
#include <QTimer>
#include "MathEx.h"
#include "GraphicsSingleton.h"
#include "Chart/Label.h"
#include "Settings.h"
#include "NesisAerospaceDataManager.h"
#include "ActiveNavigation.h"
#include "FastUnitKeys.h"
#include "Map/World.h"
#include "CommonDefsQt.h"

#include "AirspaceVertical.h"

// -----------------------------------------------------------------------

AirspaceVertical::AirspaceVertical(QObject *pParent)
 : QObject(pParent)
{
	OnUpdateAirspaces();

	m_pixAirplane.load(Settings::GetInstance()->GetImagePath()+"Airplane-VS.png");

	QTimer* pUpdateASTimer = new QTimer(this);
	connect(pUpdateASTimer, SIGNAL(timeout()), this, SLOT(OnUpdateAirspaces()));
	pUpdateASTimer->start(1000*30);
}


AirspaceVertical::~AirspaceVertical()
{
}

// -----------------------------------------------------------------------

void AirspaceVertical::OnUpdateAirspaces()
{
	// TODO This can be done smarter. E.G. when position has changed significantly...
	NesisAerospaceDataManager* pND = NesisAerospaceDataManager::GetInstance();
	m_conAS.Update(pND->GetPosition());
	
	// Let's try to get current altitude.
	map::World* pW = map::World::TryLock();
	if(pW) {
// 		qDebug() << "Altitude =" << pW->GetElevation(common::Deg(pND->GetPosition()));
		pW->Unlock();
	}
	
}

// -----------------------------------------------------------------------

void AirspaceVertical::Draw(QPainter& P, const QRect& rc) const
{
	NesisAerospaceDataManager* pND  = NesisAerospaceDataManager::GetInstance();
	ActiveNavigation* 			pAN  = ActiveNavigation::GetInstance();
	FastUnitKeys*   				pFUK = FastUnitKeys::GetInstance();
	GraphicsSingleton* 			pGS  = GraphicsSingleton::GetInstance();

	QVector<const map::Airspace*> vA(m_conAS.GetAirspaces(pND->GetPosition()));

	QString qs;

	// get the widget size
	int iW = rc.width()+1;
	int iH = rc.height();
	int iHs = iH-20; // 20 pixels for heading

	// altitude drawing factor.
	float f = iHs/12000.0f;

	// Set font
	P.setFont(pGS->GetFont(GraphicsSingleton::fVASText));

	// draw airspace rectangles
	
	P.setPen(Qt::NoPen);
	P.setBrush(pGS->GetBrush(GraphicsSingleton::bVAirspaceBack));
	
	foreach(const map::Airspace* pA, vA) {
// 		qDebug() << "F=" << pA->GetFloor() << "C=" << pA->GetCeiling();
		const int iC = qMin((int)(pA->GetCeilingFt()*f), iHs);
		const int iF = qRound(pA->GetFloorFt() * f);

		if(iF < iH && iC > 0) {
			P.drawRect(0,iH-iC, iW,iC-iF);
		}
	}

	// draw airspace limits ...
	P.setPen(Qt::white);
	chart::Label lbl(QString(), P.font(), Qt::white);
	lbl.SetRotate(-90);
	lbl.SetLocalOrigin(QPointF(1.0f, 1.0f));

	foreach(const map::Airspace* pA, vA) {
		const int iC  = pA->GetCeilingFt();
		const int iCy = iH-qMin((int)(iC*f), iHs);
		const int iF  = pA->GetFloorFt();
		const int iFy = (int)(iH-iF*f);

		if(iFy > 0) {
			P.drawText(2,iFy-20, iW,20, Qt::AlignLeft | Qt::AlignBottom, pA->GetFloor());
			P.drawText(2,iCy, iW,20, Qt::AlignLeft | Qt::AlignTop, pA->GetCeiling());
			// TODO Put title back after FH.
			lbl.SetText(pA->GetTitle());
			lbl.Draw(P, iW-2, iCy+2);
		}
	}

	P.setBrush(pGS->GetBrush(GraphicsSingleton::bBannerBack));
	P.setPen(Qt::NoPen);
	// Draw text at the top
	P.drawRect(0,0, iW,19);
	P.setPen(Qt::white);
	P.drawText(0,2, iW,20, Qt::AlignCenter, tr("Feet, FL"));

	// Draw the airplane image and altitude
	int iA = (int)((pND->GetAltitude() * 3.2808398f+5)/10); // convert to feet
	iA *= 10;
	int iAy = (int)(iH-iA*f);

	P.drawPixmap(0, iAy-6, m_pixAirplane);
// 	P.drawLine(0,iAy, iW/2, iAy);
	P.drawRect(iW/2, iAy-10, iW/2, 20);
	P.drawText(iW/2-1, iAy-8, iW/2, 20, Qt::AlignCenter, qs.setNum(iA));

	// Draw altitude bug
	int iAB = qRound(pAN->GetAltitudeBug().GetAs(pFUK->iKey_feet));
	if(iAB > 0) {
		int iAy = (int)(iH-iAB*f);

		P.setPen(Qt::black);
		// TODO Change altitude bug background color according to the
		// difference between bug and actual altitude. 
		P.setBrush(QColor(255,245,111));
		P.drawRect(0, iAy-10, iW/2-1, 20);
		P.drawText(1, iAy-8,  iW/2-1, 20, Qt::AlignCenter, qs.setNum(iAB));
	}
}

// -----------------------------------------------------------------------
