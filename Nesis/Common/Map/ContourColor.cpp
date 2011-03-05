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

#include "ContourColor.h"

namespace map {

std::auto_ptr<ContourColor> ContourColor::s_apInstance;

// ----------------------------------------------------------------------------

ContourColor::ContourColor()
{
	QRgb aDefRgb[] = {
		qRgb(  91, 113,  85 ),	// 0 m
		qRgb( 100, 131,  81 ),
		qRgb( 110, 149,  77 ),
		qRgb( 134, 166,  90 ),
		qRgb( 158, 184, 103 ),
		qRgb( 182, 201, 116 ),
		qRgb( 206, 219, 129 ),
		qRgb( 230, 237, 142 ),
		qRgb( 255, 255, 156 ),
		qRgb( 243, 236, 144 ),
		qRgb( 231, 217, 132 ),	// 1000 m
		qRgb( 219, 198, 120 ),
		qRgb( 207, 180, 108 ),
		qRgb( 195, 161,  96 ),
		qRgb( 183, 143,  84 ),
		qRgb( 171, 124,  72 ),
		qRgb( 159, 106,  60 ),
		qRgb( 162, 116,  76 ),
		qRgb( 166, 126,  92 ),
		qRgb( 169, 136, 108 ),
		qRgb( 173, 147, 125 ),	// 2000 m
		qRgb( 176, 157, 141 ),
		qRgb( 180, 167, 158 ),
		qRgb( 183, 177, 174 ),
		qRgb( 187, 188, 190 ),
		qRgb( 196, 196, 196 ),
		qRgb( 205, 205, 205 ),
		qRgb( 212, 212, 212 ),
		qRgb( 220, 220, 220 )		// 2800 m
	};
	
	// Generate the ramp to 5000 m
	QRgb C1 = aDefRgb[0];
	QRgb C2;
	for(unsigned int iA = 1; iA<=50; iA++) {
		if(iA < COUNTOF(aDefRgb)) {
			C2 = aDefRgb[iA];
		}
		else {
			int x = 221+(iA-COUNTOF(aDefRgb));
			C2 = qRgb(x,x,x);
		}
			
		for(int i=0; i<10; i++) {
			m_acolRamp[(iA-1)*10+i] = qRgb(
				qRed(C1)   + (qRed(C2)  -qRed(C1)  )*i/10, 
				qGreen(C1) + (qGreen(C2)-qGreen(C1))*i/10,
				qBlue(C1)  + (qBlue(C2) -qBlue(C1) )*i/10
			);
		}
		C1 = C2;
	}
	
	// Above 5000 m
	for(int i=500; i<900; i++)
		m_acolRamp[i] = qRgb(254,254,254);

	// 8990 is used for SEA COLOR
	m_acolRamp[899] = qRgb(30, 144, 255);
}

ContourColor::~ContourColor()
{
}
// --------------------------------------------------------------------

ContourColor* ContourColor::GetInstance()
{
	if(s_apInstance.get()==NULL)
		s_apInstance = std::auto_ptr<ContourColor>(new ContourColor);
	return s_apInstance.get();
}

// --------------------------------------------------------------------

void ContourColor::GetColor(const QVector<qint16>& vElev, QVector<QRgb>& vColor) const
{
	ASSERT(vElev.size() == vColor.size());
	QVector<qint16>::const_iterator i = vElev.begin();
	const QVector<qint16>::const_iterator e = vElev.end();
	QVector<QRgb>::iterator c = vColor.begin();
	
	while(i!=e) {
		*c++ = GetColor(*i++); 
	}
}

} // namespace
