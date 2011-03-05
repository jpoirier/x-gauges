/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2009 by Kanardia d.o.o. [see www.kanardia.eu]           *
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
#include "Unit/Manager.h"
#include "XMLTags.h"
#include "XMLGaugeRoundAltitude.h"

#include "ParseHelper.h"

namespace instrument {

// --------------------------------------------------------------------------

int ParseUnitKey(const QDomElement& el, const QString& qsTag)
{
	unit::Manager* pM = unit::Manager::GetInstance();
	QString qsA = el.attribute(qsTag);

	if(qsA.isEmpty()==false) {
		int iKey = pM->GetKey(qsA);
		if(iKey < 0) {
			qCritical() << "Unit" << qsA << "is not recognised by the unit manager."; 
		}
		return iKey;
	}
	else {
		qCritical() << "Attribute" << qsTag 
						<< "was not found in element" << el.tagName();
		return -1;
	}
}

// --------------------------------------------------------------------------

QPair<float,float> ParseRange(
	const QDomElement& elParent, 
	const QString& qsTag, 
	int iSystemKey
)
{
	unit::Manager* pM = unit::Manager::GetInstance();
	QPair<float,float> P = qMakePair(0.0f, 0.0f);

	QDomElement ef = elParent.firstChildElement(qsTag);
	if(!ef.isNull()) {
		int iKey = ParseUnitKey(ef, TAG_UNIT);
		if(iKey >= 0) {
			P.first  = ef.attribute(TAG_LOW, "0").toFloat();
			P.second = ef.attribute(TAG_HIGH, "0").toFloat();

			// If the units are given in something else than the system,
			// we must convert them into system.
			if(iKey != iSystemKey) {
				P.first  = pM->Convert(P.first, iKey, iSystemKey);
				P.second = pM->Convert(P.second, iKey, iSystemKey);
			}
		}
	}
	return P;
}

// --------------------------------------------------------------------------

QString ParseAttribute(const QDomElement& el, const QString& qsTag)
{
	QString qsA = el.attribute(qsTag);
	if(qsA.isEmpty())
		qCritical() << "Attribute" << qsTag << "was not found in "
						<< el.tagName() << "element.";
	return qsA;
}

// --------------------------------------------------------------------------

XMLGaugeRound* CreateXMLGauge(const QDomElement& el, QWidget* pParent)
{
	XMLGaugeRound* pGauge = NULL;
		
 	// Get type
 	QString qsA = ParseAttribute(el, TAG_TYPE);
	if(qsA == QLatin1String(TAG_GENERIC))
		pGauge = new XMLGaugeRound(el, pParent);
	else if(qsA == QLatin1String(TAG_ALTITUDE))
		pGauge = new XMLGaugeRoundAltitude(el, pParent);
	
	return pGauge;
}

// --------------------------------------------------------------------------

}
