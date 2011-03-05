#ifndef MAP_AIRSPACE_CONTAINER_H
#define MAP_AIRSPACE_CONTAINER_H

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

#include <QVector>
#include "Airspace.h"
//#include "TextLabel.h"

// Forward declarations
class QPainter;

namespace map {
	
// Forward declarations
class Mapper;
	
// ------------------------------------------------------------------------ 
/*! \brief AirspaceContainer class - a container class for the Airspace objects.
	
	The container owns stored Airspace objects.
 */

class AirspaceContainer {
private:
	//! Internal container declaration.
	typedef QVector<Airspace*> Container;
	//! Internal const container declaration.
	typedef QVector<const Airspace*> ConstContainer;
	//! Section container version - it is a QHash based container.
	typedef QHash<quint16, ConstContainer> SectionContainer;
	
	//! Parser class for the SUA format airspace.
	class SUAParser {
	public:
		enum Keyword {
			kInclude = 0,
			kTitle,
			kType,
			kClass,
			kActive,
			kRadio,
			kTops,
			kBase,
			kPoint,
			kClockwise,
			kAntiClockwise,
			kCircle,
			kWidth,
			kAwy,
			kEnd,
			kInvalid
		};
		
		Airspace* Parse(QTextStream& in, int& iLine);
		
	private:
		//! Convert a line of text into keyword and value.
		Keyword GetKeyword(const QString& qsLine) const;
		//! Get value - a string on the right side of '=' char.
		QString GetValue(const QString& qsIn) const;
		//! Get value on the right of '=' and convert it into type.
		Airspace::Type GetType(const QString& qsIn) const;
		//! Get already parsed value and convert it to WGS point in radians.
		QPointF GetPoint(const QString& qsPoint) const;
		
		//! Parse kCircle keyword and generate points.
		QPolygonF GetCircle(const QString& qsIn) const;
		//! Parse kClockwise, kAntiClockwise keywords and generate points. 
		QPolygonF GetArc(const QString& qsIn, bool bClockwise) const;
		
	private:
		//! List of keywords.
		static const char* s_apcKeywords[];
		//! List of airspace type keywords - long, short.
		static const char* s_apcTypes[][2];
		
		//! Airspace - working variable.
		Airspace m_A;
	};
	
	//! Parser class for the OpenAir format airspace.
	class OAParser {
	public:
		enum Keyword {
			kClass,
			kName,
			kCeiling,
			kFloor,
			kLabelCoordinate,
			kVariable,
			kPoint,
			kArc,
			kArc2P,
			kCircle,
			kAirway,
			kSelectBrush,
			kSelectPen,
			kTextColor,
			kInvalid
		};
		
		Airspace* Parse(QTextStream& in, int& iLine);
		
	private:
		//! Convert a line of text into keyword and value.
		Keyword GetKeyword(const QString& qsLine) const;
		//! Get value - a string on the right side of the keyword.
		QString GetValue(const QString& qsIn) const;
		//! Convert already parsed value to WGS point in radians.
		QPointF GetPoint(const QString& qsPoint) const;
		//! Convert string (d)dd:mm:ss into radians.
		float GetRadians(const QString& qsDMS) const; 
		//! Parse variable command. Updates internal variables.
		void ParseVariable(const QString& qsV);
		//! Parse kCircle keyword and generate points.
		QPolygonF GetCircle(const QString& qsC) const;
		//! Parse kArc keyword and generate points. 
		QPolygonF GetArc(const QString& qsA) const;
		//! Parse another version of the kArc keyword and generate points. 
		QPolygonF GetArc2P(const QString& qsA) const;
		
	private:
		//! List of keywords.
		static const char* s_apcKeywords[];
		//! List of airspace type keywords - long, short.
		static const char* s_apcTypes[][2];
		
		//! Airspace - working variable.
		Airspace m_A;
		//! Current parsing line
		int m_iLine;
		
		// We have some specific variables
		//! Arc direction.
		bool m_bArcDirClockwise;
		//! Center coordinate
		QPointF m_ptX;
		//! Airway width in NM.
		float m_fAwyWidth;
		//! Zoom level limit
		float m_fZoom;
	};
	
	//! AirspaceContainer contructor.
	AirspaceContainer();
public:
	//! Access to the singleton object.
	static AirspaceContainer* GetInstance()
	{ 
		static AirspaceContainer asc;
		return &asc;
	}
			
	//! Destructor.
	~AirspaceContainer();
	
	//! Clear - reset the container (delete existing airspaces).
	void Clear();
	
	//! Load airspaces into container from given file in SUA format.
	int LoadSUA(const QString& qsFile);
	
	//! Load airspaces into container from given file in OpenAir format.
	int LoadOpenAir(const QString& qsFile);
	
	//! Save loaded airspaces in the native binary format.
	int Save(const QString& qsFile) const;
	
	//! Load airspaces in the native binary format. It keeps previosly loaded airspaces.
	int Load(const QString& qsFile);
	
	//! Get airspaces inside the given rectangle area.
	QVector<const Airspace*> GetAirspaces(
			const QRectF& rc, int iClassFlag = Airspace::cAll, int iBaseLimit=12500) const;
	//! Get airspaces which contain given point.
/*	QVector<const Airspace*> GetAirspaces(
		const QPointF& ptX, int iClassFlag, int iBaseLimit=12500, bool bStrict=true) const;
	*/	
	//! Create sector container (hash the container)
	void CreateSectorContainer();
	
private:
	//! Container of all loaded airspaces.
	Container m_con;
	//! Section container (QHash), the key is block id.
	SectionContainer m_sec;
};


} // map namespace

#endif	// MAP_AIRSPACE_CONTAINER_H
