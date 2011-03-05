#ifndef INSTRUMENT_NEEDLE_H
#define INSTRUMENT_NEEDLE_H

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

#include <QPainter>

// --------------------------------------------------------------------------

namespace instrument {

/** \brief Special class that draws a needle.

	The needle is drawn in horizontal position. Depending to the needle shape, 
	the needle may be drawn directly using painter commands or indirectly by
	precomputing a pixmap and copying the pixmap over the backgroud.
	
	Everytime, the size changes, needle pixmap (when used) must be recomputed
	using the SetSize method.
	
	Before painting the needle, the QPainter object must translate its origing
	into the needle rotation center.
 */

class Needle
{
public: 
	enum Type {
		tPlain,			//!< Standard, simple plain needle.
		tPointed, 		//!< Like standard, but slightly more pointed at the end.
		tFatShort,		//!< Fat and short needle (used for 1000 in altimeter)
		t10000,			//!< Special shaped 10000 ft needle.
		tPlainLabel		//!< Standard thin needle with a label disk
	};
public:
	//! Default constructor.
	Needle();
	//! Destructor
	~Needle();

	//! Draw needle.
	void Draw(QPainter& P, qreal fAngleDeg);

	//! Set the needle size (width=length, height=thickness).
	void SetSize(QSize sz, int iOffset);

	//! Set the needle type.
	void SetType(Type eType)
	{ m_eType = eType; }

protected:
	//! Draw the needle pixmap.
	void CreatePixmap();

protected:
	//! Needle type.
	Type m_eType;

	//! Needle size (in pixels).
	QSize m_size;
	//! Needle offset (in pixels).
	int m_iOffset;

	//! Needle pixmap.
	QPixmap m_px;
};

}

#endif
