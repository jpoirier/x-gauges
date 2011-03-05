#ifndef ROUTE_WAYPOINT_H
#define ROUTE_WAYPOINT_H
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

#include <utility>
#include <QString>
#include <QDataStream>
#include <QPoint>
#include <QPainter>
#include "Map/Mapper.h"

namespace route {

// -------------------------------------------------------------------------

//! Country two letter description according to ISO3166.
enum Country
{
	cInvalid, //!< Unknown country name.
   cAF, //!< Afghanistan
   cAL, //!< Albania
   cDZ, //!< Algeria
   cAS, //!< American Samoa
   cAD, //!< Andorra
   cAO, //!< Angola
   cAI, //!< Anguilla
   cAQ, //!< Antarctica
   cAG, //!< Antigua and Barbuda
   cAR, //!< Argentina
   cAM, //!< Armenia
   cAW, //!< Aruba
   cAU, //!< Australia
   cAT, //!< Austria
   cAZ, //!< Azerbaijan
   cBH, //!< Bahrain
   cBD, //!< Bangladesh
   cBB, //!< Barbados
   cBY, //!< Belarus
   cBE, //!< Belgium
   cBZ, //!< Belize
   cBJ, //!< Benin
   cBM, //!< Bermuda
   cBT, //!< Bhutan
   cBO, //!< Bolivia
   cBA, //!< Bosnia and Herzegovina
   cBW, //!< Botswana
   cBV, //!< Bouvet Island
   cBR, //!< Brazil
   cIO, //!< British Indian Ocean Territory
   cVG, //!< British Virgin Islands
   cBN, //!< Brunei
   cBG, //!< Bulgaria
   cBF, //!< Burkina Faso
   cBI, //!< Burundi
   cCI, //!< Côte d'Ivoire
   cKH, //!< Cambodia
   cCM, //!< Cameroon
   cCA, //!< Canada
   cCV, //!< Cape Verde
   cKY, //!< Cayman Islands
   cCF, //!< Central African Republic
   cTD, //!< Chad
   cCL, //!< Chile
   cCN, //!< China
   cCX, //!< Christmas Island
   cCC, //!< Cocos (Keeling) Islands
   cCO, //!< Colombia
   cKM, //!< Comoros
   cCG, //!< Congo
   cCK, //!< Cook Islands
   cCR, //!< Costa Rica
   cHR, //!< Croatia
   cCU, //!< Cuba
   cCY, //!< Cyprus
   cCZ, //!< Czech Republic
   cCD, //!< Democratic Republic of the Congo
   cDK, //!< Denmark
   cDJ, //!< Djibouti
   cDM, //!< Dominica
   cDO, //!< Dominican Republic
   cTL, //!< East Timor
   cEC, //!< Ecuador
   cEG, //!< Egypt
   cSV, //!< El Salvador
   cGQ, //!< Equatorial Guinea
   cER, //!< Eritrea
   cEE, //!< Estonia
   cET, //!< Ethiopia
   cFO, //!< Faeroe Islands
   cFK, //!< Falkland Islands
   cFJ, //!< Fiji
   cFI, //!< Finland
   cMK, //!< Former Yugoslav Republic of Macedonia
   cFR, //!< France
   cGF, //!< French Guiana
   cPF, //!< French Polynesia
   cTF, //!< French Southern Territories
   cGA, //!< Gabon
   cGE, //!< Georgia
   cDE, //!< Germany
   cGH, //!< Ghana
   cGI, //!< Gibraltar
   cGR, //!< Greece
   cGL, //!< Greenland
   cGD, //!< Grenada
   cGP, //!< Guadeloupe
   cGU, //!< Guam
   cGT, //!< Guatemala
   cGN, //!< Guinea
   cGW, //!< Guinea-Bissau
   cGY, //!< Guyana
   cHT, //!< Haiti
   cHM, //!< Heard Island and McDonald Islands
   cHN, //!< Honduras
   cHK, //!< Hong Kong
   cHU, //!< Hungary
   cIS, //!< Iceland
   cIN, //!< India
   cID, //!< Indonesia
   cIR, //!< Iran
   cIQ, //!< Iraq
   cIE, //!< Ireland
   cIL, //!< Israel
   cIT, //!< Italy
   cJM, //!< Jamaica
   cJP, //!< Japan
   cJO, //!< Jordan
   cKZ, //!< Kazakhstan
   cKE, //!< Kenya
   cKI, //!< Kiribati
   cKW, //!< Kuwait
   cKG, //!< Kyrgyzstan
   cLA, //!< Laos
   cLV, //!< Latvia
   cLB, //!< Lebanon
   cLS, //!< Lesotho
   cLR, //!< Liberia
   cLY, //!< Libya
   cLI, //!< Liechtenstein
   cLT, //!< Lithuania
   cLU, //!< Luxembourg 
   cMO, //!< Macau
   cMG, //!< Madagascar
   cMW, //!< Malawi
   cMY, //!< Malaysia
   cMV, //!< Maldives
   cML, //!< Mali
   cMT, //!< Malta
   cMH, //!< Marshall Islands
   cMQ, //!< Martinique
   cMR, //!< Mauritania
   cMU, //!< Mauritius
   cYT, //!< Mayotte
   cMX, //!< Mexico
   cFM, //!< Micronesia
   cMD, //!< Moldova
   cMC, //!< Monaco
   cMN, //!< Mongolia
	cME, //!< Montenegro
   cMS, //!< Montserrat
   cMA, //!< Morocco
   cMZ, //!< Mozambique
   cMM, //!< Myanmar
   cNA, //!< Namibia
   cNR, //!< Nauru
   cNP, //!< Nepal
   cNL, //!< Netherlands
   cAN, //!< Netherlands Antilles
   cNC, //!< New Caledonia
   cNZ, //!< New Zealand
   cNI, //!< Nicaragua
   cNE, //!< Niger
   cNG, //!< Nigeria
   cNU, //!< Niue
   cNF, //!< Norfolk Island
   cKP, //!< North Korea
   cMP, //!< Northern Marianas
   cNO, //!< Norway
   cOM, //!< Oman
   cPK, //!< Pakistan
   cPW, //!< Palau
   cPA, //!< Panama
   cPG, //!< Papua New Guinea
   cPY, //!< Paraguay
   cPE, //!< Peru
   cPH, //!< Philippines
   cPN, //!< Pitcairn Islands
   cPL, //!< Poland
   cPT, //!< Portugal
   cPR, //!< Puerto Rico
   cQA, //!< Qatar
   cRE, //!< Réunion
   cRO, //!< Romania
   cRU, //!< Russia
   cRW, //!< Rwanda
   cST, //!< São Tomé and Príncipe
   cSH, //!< Saint Helena
   cKN, //!< Saint Kitts and Nevis
   cLC, //!< Saint Lucia
   cPM, //!< Saint Pierre and Miquelon
   cVC, //!< Saint Vincent and the Grenadines
   cWS, //!< Samoa
   cSM, //!< San Marino
   cSA, //!< Saudi Arabia
   cSN, //!< Senegal
	cRS, //!< Serbia
   cSC, //!< Seychelles
   cSL, //!< Sierra Leone
   cSG, //!< Singapore
   cSK, //!< Slovakia
   cSI, //!< Slovenia
   cSB, //!< Solomon Islands
   cSO, //!< Somalia
   cZA, //!< South Africa
   cGS, //!< South Georgia and the South Sandwich Islands
   cKR, //!< South Korea
   cES, //!< Spain
   cLK, //!< Sri Lanka
   cSD, //!< Sudan
   cSR, //!< Suriname
   cSJ, //!< Svalbard and Jan Mayen
   cSZ, //!< Swaziland
   cSE, //!< Sweden
   cCH, //!< Switzerland
   cSY, //!< Syria
   cTW, //!< Taiwan
   cTJ, //!< Tajikistan
   cTZ, //!< Tanzania
   cTH, //!< Thailand
   cBS, //!< The Bahamas
   cGM, //!< The Gambia
   cTG, //!< Togo
   cTK, //!< Tokelau
   cTO, //!< Tonga
   cTT, //!< Trinidad and Tobago
   cTN, //!< Tunisia
   cTR, //!< Turkey
   cTM, //!< Turkmenistan
   cTC, //!< Turks and Caicos Islands
   cTV, //!< Tuvalu
   cVI, //!< US Virgin Islands
   cUG, //!< Uganda
   cUA, //!< Ukraine
   cAE, //!< United Arab Emirates
   cGB, //!< United Kingdom
   cUS, //!< United States
   cUM, //!< United States Minor Outlying Islands
   cUY, //!< Uruguay
   cUZ, //!< Uzbekistan
   cVU, //!< Vanuatu
   cVA, //!< Vatican City
   cVE, //!< Venezuela
   cVN, //!< Vietnam
   cWF, //!< Wallis and Futuna
   cEH, //!< Western Sahara
   cYE, //!< Yemen
   cYU, //!< Yugoslavia
   cZM, //!< Zambia
   cZW, //!< Zimbabwe
};

/** \brief Waypoint - data extracted from one line in the WELT2000.WGS file.

	 Data in WELT2000.WGS file are given line by line. They include turning
    points mainly used in gliding, but they also include most airfields in
    the World. This class extracts information from one line in WELT2000 file
    and provides it in a more usefull form.
  */
	
class Waypoint
{
public:
	//! Turning point type
	enum Type {
		tUser = 1,				//!< It can be anything.
		tAirfield = 2,			//!< Airfield, basicaly any airfield, even outlanding.
		tVFR = 4,				//!< VFR points (entry point, compulsory reporting points, ...)
		tNavAid = 8,			//!< Navigation aid - NDB, VOR, etc.
		tTurnpoint = 16		//!< A turning point (typically used in gliding)
	};
public:
	//! Default constructor - invalid and empty turning point.
	Waypoint();
	//! Copy constructor.
	Waypoint(const Waypoint& C);
	//! Constructor from QString object - one line in WELT2000.WGS
	Waypoint(const QByteArray& baDataLine);
	//! Destructor
	virtual ~Waypoint();

	//! Clone the object, derived classes as well.
	virtual Waypoint* Clone() const
	{ return new Waypoint(*this); }
	
	//! Get the turning point type.
	Type GetType() const
	{ return m_eType; }
	
	//! Get the identification key.
	const QString& GetKey() const
	{ return m_qsKey; }
	//! Set the key.
	void SetKey(const QString& qsKey)
	{ m_qsKey = qsKey; }
	
	//! Get short name
	virtual QString GetShortName() const
	{ return GetKey(); }
	
	//! Get turning point description.
	const QString& GetDescription() const
	{ return m_qsDesc; }
	//! Set waypoint description.
	void SetDescription(const QString& qsDesc)
	{ m_qsDesc = qsDesc; }
	
	//! Get elevation in meters.
	int GetElevation() const 
	{ return m_iElevation; }
	//! Set the waypoint elevation in meters.
	void SetElevation(int iElevation_m) 
	{ m_iElevation = iElevation_m; }
	
	//! Get coordinate (longitude, latitude) in radians.
	QPointF GetCoordinate() const
	{ return QPointF(m_fLongitude, m_fLatitude); }
	//! Set coordinate (longitude, latitude) in radians.
	void SetCoordinate(const QPointF& pt)
	{ m_fLongitude = pt.x(); m_fLatitude = pt.y(); }
	
	//! Get latitude in radians.
	float GetLatitude() const
	{ return m_fLatitude; }
	//! Get longitude in radians.
	float GetLongitude() const
	{ return m_fLongitude; }
	//! Get country enum.
	Country GetCountry() const
	{ return m_eCountry; }
	
	//! Get sector id from known internal coordiantes.
	quint16 GetSectorId() const;
	
	//! Get distance from given coordinate in meters.
	float GetDistance(float fLon0, float fLat0) const;
	//! Get distance from given coordinate in meters - overloaded.
	float GetDistance(const QPointF& ptCoor) const
	{ return GetDistance(ptCoor.x(), ptCoor.y()); }

	//! Bearing and distance to waypoint point from given point.
	std::pair<float, float> GetBearingDistance(float fLon0, float fLat0) const;
	//! Bearing and distance to waypoint point from given point.
	std::pair<float, float> GetBearingDistance(const QPointF& ptCoor) const
	{ return GetBearingDistance(ptCoor.x(), ptCoor.y()); }
	
	//! Save the data into given binary stream.
	virtual void Write(QDataStream& out) const;
	//! Load the data from given binary stream.
	virtual void Read(QDataStream& in);
	
	//! Return true if this object is a valid object.
	virtual bool IsValid() const;
	
	//! Don't draw anything by default.
	virtual void Draw(QPainter& P, const map::Mapper& M) const;
	
protected:
	//! Get country enum out of the iso name.
	Country ParseCountry(const char* pCN) const;
	
protected:
	//! Type of the object.
	Type m_eType;
	//! The waypoint key. 
	QString m_qsKey;
	//! The waypoint description text
	QString m_qsDesc;
	
	//! Elevation in meters
	qint16 m_iElevation;
	
	//! Longitude - radians, east positive.
	float m_fLongitude;
	//! Latitude - radians, north positive.
	float m_fLatitude;
	
	//! Country
	Country m_eCountry;
};

// -------------------------------------------------------------------------
} // namespace

#endif
