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
#include "CommonDefs.h"
#include "MathEx.h"
#include "SphereNavigation.h"
#include "Map/MapDefs.h"

#include "Waypoint.h"

namespace route {

// -------------------------------------------------------------------------

Waypoint::Waypoint()
{
	m_eType      = tUser;
	m_iElevation = -99;
	
	m_fLongitude = 0;
	m_fLatitude  = M_PI_2;
	
	m_eCountry   = cInvalid;
}
// -------------------------------------------------------------------------

Waypoint::Waypoint(const Waypoint& C)
{
	m_eType 						= C.m_eType;
	m_qsKey 						= C.m_qsKey;
	m_qsDesc 					= C.m_qsDesc;
	m_iElevation 				= C.m_iElevation;
	m_fLongitude 				= C.m_fLongitude;
	m_fLatitude  				= C.m_fLatitude;
	m_eCountry 					= C.m_eCountry;
}

// -------------------------------------------------------------------------

Waypoint::Waypoint(const QByteArray& baDataLine)
{
	// It is a turning point by default - derived class may change this
	// in its constructor.
	m_eType = tUser;
	
/*	if(baDataLine.contains("*FIRE")) {
		qDebug() << "Fire mark " << baDataLine;
	}*/
	// extract the key: bytes 0-6
	m_qsKey = baDataLine.mid(0,6).simplified();
	
	// char 12 for Cander is on position 7 (byte 6), if non empty;
	// There is no such entry in the database (7.3.2007).
/*	char ch7 = baDataLine[6];
	if(ch7 != ' ') {
 		qDebug() << "Cander mark " << baDataLine;
	}*/
	
	// Grab the text
	m_qsDesc = baDataLine.mid(7,34).simplified();
	
	// Elevation
	bool bOK;
	m_iElevation = baDataLine.mid(41,4).toInt(&bOK);
	if(!bOK) qDebug() << "Key = " << m_qsKey << " failed - elevation: " << baDataLine;
	
	// There was no such situation for airfields at 7.3.2007
	bool bElevationNotProved = baDataLine[41] == '0';
	if(bElevationNotProved) {
		qDebug() << "Key = " << m_qsKey << " unproved elevation =" << m_iElevation << baDataLine;
	}
	
	// Latitude 
	// degrees
	QByteArray baPos = baDataLine.mid(45,7).toUpper();
	m_fLatitude = baPos.mid(1,2).toInt(&bOK);
	if(!bOK) qDebug() << "Key = " << m_qsKey << " failed - latitude degress.";
	// minutes
	m_fLatitude += baPos.mid(3,2).toInt(&bOK)/60.0f;
	if(!bOK) qDebug() << "Key = " << m_qsKey << " failed - latitude minutes.";
	// seconds
	m_fLatitude += baPos.mid(5,2).toInt(&bOK)/3600.0f;
	if(!bOK) qDebug() << "Key = " << m_qsKey << " failed - latitude seconds.";
	// North +, South -
	if(baPos[0]=='S')
		m_fLatitude *= -1;
	// convert it into radians.
	m_fLatitude = common::Rad(m_fLatitude);

	// Longitude 
	// degrees
	baPos = baDataLine.mid(52,8).toUpper();
	m_fLongitude = baPos.mid(1,3).toInt(&bOK);
	if(!bOK) qDebug() << "Key = " << m_qsKey << " failed - longitude degress.";
	// minutes
	m_fLongitude += baPos.mid(4,2).toInt(&bOK)/60.0f;
	if(!bOK) qDebug() << "Key = " << m_qsKey << " failed - longitude minutes.";
	// seconds
	m_fLongitude += baPos.mid(6,2).toInt(&bOK)/3600.0f;
	if(!bOK) qDebug() << "Key = " << m_qsKey << " failed - longitude seconds.";
	// North +, South -
	if(baPos[0]=='W')
		m_fLongitude *= -1;
	m_fLongitude = common::Rad(m_fLongitude);

//	m_bBetterThanOfficial = (ch7 == '-');
/*	if(m_bBetterThanOfficial) {
		qDebug() << "Better than official mark for: " << baDataLine;
	}*/
	
	// country name?
	m_eCountry = ParseCountry(baDataLine.mid(60,2).constData());
	if(m_eCountry == cInvalid) {
		qDebug() << "Invalid country name was detected." << baDataLine;
	}
}


Waypoint::~Waypoint()
{
}
// -------------------------------------------------------------------------
	
bool Waypoint::IsValid() const
{
	return    m_qsKey.isEmpty() == false 
			&& (m_fLongitude <= M_PI && m_fLongitude >= -M_PI)
			&& (m_fLatitude <= M_PI_2 && m_fLatitude >= -M_PI_2);
}

// -------------------------------------------------------------------------

quint16 Waypoint::GetSectorId() const
{
	return map::GetIdFromLonLat(
		common::Deg(m_fLongitude), 
		common::Deg(m_fLatitude)
	);
}

// -------------------------------------------------------------------------

void Waypoint::Write(QDataStream& out) const
{
	// Mark the version first.
	out << (qint8)2; 
	
	out << m_qsKey;
	out << m_qsDesc;
	
	out << m_iElevation;
	
	out << m_fLongitude;
	out << m_fLatitude;
	
	out << (qint16)m_eCountry;
}

// -------------------------------------------------------------------------

void Waypoint::Read(QDataStream& in)
{
	// Read the version first.
	qint8 iVer;
	in >> iVer; 
	
	m_eType = tUser;
	bool bDummy;
	
	if(iVer==1) {
		QByteArray baText;
		in >> baText;
		m_qsKey = baText;
		in >> baText;
		m_qsDesc = baText;
	}
	else {
		in >> m_qsKey;
		in >> m_qsDesc;
	}
	
	in >> m_iElevation;
	if(iVer==1) in >> bDummy; //m_bElevationNotProved;
	
	in >> m_fLongitude;
	in >> m_fLatitude;
	
	if(iVer==1) in >> bDummy; //m_bBetterThanOfficial;
	
	qint16 i16;
	in >> i16; m_eCountry = (Country)i16;
}

// -------------------------------------------------------------------------

Country Waypoint::ParseCountry(const char* pCN) const
{
	struct TC {
		char str[3];
		Country cnt;
	};
	
	static TC tc[] = {
	{ "AF", cAF },
   { "AL", cAL },
	{ "DZ", cDZ },
	{ "AS", cAS },
	{ "AD", cAD },
	{ "AO", cAO },
	{ "AI", cAI },
	{ "AQ", cAQ },
	{ "AG", cAG },
	{ "AR", cAR },
	{ "AM", cAM },
	{ "AW", cAW },
	{ "AU", cAU },
	{ "AT", cAT },
	{ "AZ", cAZ },
	{ "BH", cBH },
	{ "BD", cBD },
	{ "BB", cBB },
	{ "BY", cBY },
	{ "BE", cBE },
	{ "BZ", cBZ },
	{ "BJ", cBJ },
	{ "BM", cBM },
	{ "BT", cBT },
	{ "BO", cBO },
	{ "BA", cBA },
	{ "BW", cBW },
	{ "BV", cBV },
	{ "BR", cBR },
	{ "IO", cIO },
	{ "VG", cVG },
	{ "BN", cBN },
	{ "BG", cBG },
	{ "BF", cBF },
	{ "BI", cBI },
	{ "CI", cCI },
	{ "KH", cKH },
	{ "CM", cCM },
	{ "CA", cCA },
	{ "CV", cCV },
	{ "KY", cKY },
	{ "CF", cCF },
	{ "TD", cTD },
	{ "CL", cCL },
	{ "CN", cCN },
	{ "CX", cCX },
	{ "CC", cCC },
	{ "CO", cCO },
	{ "KM", cKM },
	{ "CG", cCG },
	{ "CK", cCK },
	{ "CR", cCR },
	{ "HR", cHR },
	{ "CU", cCU },
	{ "CY", cCY },
	{ "CZ", cCZ },
	{ "CD", cCD },
	{ "DK", cDK },
	{ "DJ", cDJ },
	{ "DM", cDM },
	{ "DO", cDO },
	{ "TL", cTL },
	{ "EC", cEC },
	{ "EG", cEG },
	{ "SV", cSV },
	{ "GQ", cGQ },
	{ "ER", cER },
	{ "EE", cEE },
	{ "ET", cET },
	{ "FO", cFO },
	{ "FK", cFK },
	{ "FJ", cFJ },
	{ "FI", cFI },
	{ "MK", cMK },
	{ "FR", cFR },
	{ "GF", cGF },
	{ "PF", cPF },
	{ "TF", cTF },
	{ "GA", cGA },
	{ "GE", cGE },
	{ "DE", cDE },
	{ "GH", cGH },
	{ "GI", cGI },
	{ "GR", cGR },
	{ "GL", cGL },
	{ "GD", cGD },
	{ "GP", cGP },
	{ "GU", cGU },
	{ "GT", cGT },
	{ "GN", cGN },
	{ "GW", cGW },
	{ "GY", cGY },
	{ "HT", cHT },
	{ "HM", cHM },
	{ "HN", cHN },
	{ "HK", cHK },
	{ "HU", cHU },
	{ "IS", cIS },
	{ "IN", cIN },
	{ "ID", cID },
	{ "IR", cIR },
	{ "IQ", cIQ },
	{ "IE", cIE },
	{ "IL", cIL },
	{ "IT", cIT },
	{ "JM", cJM },
	{ "JP", cJP },
	{ "JO", cJO },
	{ "KZ", cKZ },
	{ "KE", cKE },
	{ "KI", cKI },
	{ "KW", cKW },
	{ "KG", cKG },
	{ "LA", cLA },
	{ "LV", cLV },
	{ "LB", cLB },
	{ "LS", cLS },
	{ "LR", cLR },
	{ "LY", cLY },
	{ "LI", cLI },
	{ "LT", cLT },
	{ "LU", cLU },
	{ "MO", cMO },
	{ "MG", cMG },
	{ "MW", cMW },
	{ "MY", cMY },
	{ "MV", cMV },
	{ "ML", cML },
	{ "MT", cMT },
	{ "MH", cMH },
	{ "MQ", cMQ },
	{ "MR", cMR },
	{ "MU", cMU },
	{ "YT", cYT },
	{ "MX", cMX },
	{ "FM", cFM },
	{ "MD", cMD },
	{ "MC", cMC },
	{ "MN", cMN },
	{ "MS", cMS },
	{ "MA", cMA },
	{ "MZ", cMZ },
	{ "MM", cMM },
	{ "NA", cNA },
	{ "NR", cNR },
	{ "NP", cNP },
	{ "NL", cNL },
	{ "AN", cAN },
	{ "NC", cNC },
	{ "NZ", cNZ },
	{ "NI", cNI },
	{ "NE", cNE },
	{ "NG", cNG },
	{ "NU", cNU },
	{ "NF", cNF },
	{ "KP", cKP },
	{ "MP", cMP },
	{ "NO", cNO },
	{ "OM", cOM },
	{ "PK", cPK },
	{ "PW", cPW },
	{ "PA", cPA },
	{ "PG", cPG },
	{ "PY", cPY },
	{ "PE", cPE },
	{ "PH", cPH },
	{ "PN", cPN },
	{ "PL", cPL },
	{ "PT", cPT },
	{ "PR", cPR },
	{ "QA", cQA },
	{ "RE", cRE },
	{ "RO", cRO },
	{ "RU", cRU },
	{ "RW", cRW },
	{ "ST", cST },
	{ "SH", cSH },
	{ "KN", cKN },
	{ "LC", cLC },
	{ "PM", cPM },
	{ "VC", cVC },
	{ "WS", cWS },
	{ "SM", cSM },
	{ "SA", cSA },
	{ "SN", cSN },
	{ "SC", cSC },
	{ "SL", cSL },
	{ "SG", cSG },
	{ "SK", cSK },
	{ "SI", cSI },
	{ "SB", cSB },
	{ "SO", cSO },
	{ "ZA", cZA },
	{ "GS", cGS },
	{ "KR", cKR },
	{ "ES", cES },
	{ "LK", cLK },
	{ "SD", cSD },
	{ "SR", cSR },
	{ "SJ", cSJ },
	{ "SZ", cSZ },
	{ "SE", cSE },
	{ "CH", cCH },
	{ "SY", cSY },
	{ "TW", cTW },
	{ "TJ", cTJ },
	{ "TZ", cTZ },
	{ "TH", cTH },
	{ "BS", cBS },
	{ "GM", cGM },
	{ "TG", cTG },
	{ "TK", cTK },
	{ "TO", cTO },
	{ "TT", cTT },
	{ "TN", cTN },
	{ "TR", cTR },
	{ "TM", cTM },
	{ "TC", cTC },
	{ "TV", cTV },
	{ "VI", cVI },
	{ "UG", cUG },
	{ "UA", cUA },
	{ "AE", cAE },
	{ "GB", cGB },
	{ "US", cUS },
	{ "UM", cUM },
	{ "UY", cUY },
	{ "UZ", cUZ },
	{ "VU", cVU },
	{ "VA", cVA },
	{ "VE", cVE },
	{ "VN", cVN },
	{ "WF", cWF },
	{ "EH", cEH },
	{ "YE", cYE },
	{ "YU", cYU },
	{ "ZM", cZM },
	{ "ZW", cZW },
	
	// duplicates - wrong country code in the database
	{ "UK", cGB },
	{ "RS", cRS },
	{ "ME", cME },
	
	};
	
	for(unsigned int i=0; i<COUNTOF(tc); i++) {
		if(strcmp(tc[i].str, pCN)==0)
			return tc[i].cnt; 
	}
	return cInvalid;
}
// -------------------------------------------------------------------------

float Waypoint::GetDistance(float fLon0, float fLat0) const
{
	return common::GetDistanceGreatCircle(fLon0, fLat0, m_fLongitude, m_fLatitude)
			 * EARTH_RADIUS_M;	
}

// -------------------------------------------------------------------------

std::pair<float, float> Waypoint::GetBearingDistance(
	float fLon0, 
	float fLat0
) const
{
	std::pair<float,float> bd = common::GetBearingDistanceGreatCircle(
		fLon0, fLat0, m_fLongitude, m_fLatitude
	);
	bd.second *= EARTH_RADIUS_M;
	return bd;
}

// -------------------------------------------------------------------------

void Waypoint::Draw(QPainter& P, const map::Mapper& M) const
{
	P.save();
	
	// Prepare colors.
	P.setPen(QPen(Qt::darkBlue,2));
	
	// Move the point
	P.translate(M.ConvertToImage(GetCoordinate()));
	
	// Draw the cross.
	P.drawLine(-5,0, 5,0);
	P.drawLine(0,-5, 0,5);
	P.setPen(Qt::NoPen);
	P.setBrush(Qt::white);
	P.drawEllipse(-2, -2, 4, 4);
	
	P.restore();
}

// -------------------------------------------------------------------------

}

