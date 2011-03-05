#ifndef __MAP_CONTOUR_COLOR_H
#define __MAP_CONTOUR_COLOR_H
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

#include <memory>
#include <QtGlobal>
#include <QVector>
#include <QColor>
#include "CommonDefs.h"
#include "MapDefs.h"

namespace map {

// -----------------------------------------------------------------------
/** \brief A class that calculates contour color for the given elevation.
	 
	Contour color is calculated according to the color ramp. 
	Currently only default internal ramp may be used.

	This class is a singleton class.

	Currently the class is not thread safe. Potential problems are:
   * the constructor is called by two threads simultaniously (very unlikely).
   * SetRamp is pending in one process, while GetColor is used in another.
 */

// TODO Make the ContourColor class thread safe.
class ContourColor
{
private:
	//! Constructor.
	ContourColor();
public:
	//! The only access to the contour object.
	static ContourColor* GetInstance();

	//! Destructor.
	virtual ~ContourColor();

	//! Get correct color for the given elevation.
	QRgb GetColor(int iElevation) const;

	//! Get correct colors for a elevation vector.
	void GetColor(const QVector<qint16>& vElev, QVector<QRgb>& vColor) const;

private:
	//! The singleton object.
	static std::auto_ptr<ContourColor> s_apInstance;
	//! Ten meter color ramp.
	QRgb m_acolRamp[900];
};

// -----------------------------------------------------------------------

inline QRgb ContourColor::GetColor(int iElevation) const
{
	return m_acolRamp[iElevation / 10];
}

// --------------------------------------------------------------------

} // map namespace

#endif	// __MAP_PAINTER
