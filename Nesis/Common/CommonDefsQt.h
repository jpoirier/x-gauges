#ifndef COMMONDEFS_QT_H
#define COMMONDEFS_QT_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2006 by Kanardia d.o.o. [see www.kanardia.eu]           *
 *   Writen by:                                                            *
 *      Ales Krajnc [ales.krajnc@kanardia.eu]                              *
 *                                                                         *
 *   Status: Open Source                                                   *
 *                                                                         *
 *   Licence: GPL - GNU General Public License                             *
 *            See 'COPYING.html' for more details about the license.       *
 *                                                                         *
 ***************************************************************************/

/*! \file CommonDefs.h
    \brief Declaration and implementation of some commonly used
           defines, constants and functions that are Qt specific.

    The functions are grouped in the common namespace.

    \author Ales Krajnc
 */
// -------------------------------------------------------------------------

#include "CommonDefs.h"

// Qt
#include <QString>
#include <QDataStream>
#include <QPolygon>
#include <QPair>

#include <QPointF>
#include <QRectF>

// -------------------------------------------------------------------------
// Some typedefs

namespace common {

// -------------------------------------------------------------------------
//! Capitalize words in the QString - put it into "Title Case Form".
QString Capitalize(const QString& qs);

// -------------------------------------------------------------------------

//! stl vector of QString objects
typedef std::vector<QString>        VectorQS;

// -------------------------------------------------------------------------

//! Create polygon chamfer points from the given rectangle and iCx, iCy.
QPolygon GetChamferPolygon(QRect rc, int iCx, int iCy=0);

// -------------------------------------------------------------------------
//! Convertion from float (qreal) into QString for given number of decimals.
QString Float2String(float f, unsigned int uiDec=0);

// -------------------------------------------------------------------------

//! Return true if given valud is inside the ragne.
template<typename T>
bool IsInRange(const QPair<T,T>& r, const T& f)
{ return f>=r.first && f<=r.second; }

//! Return true if given valud is inside the ragne.
template<typename T>
bool IsInRange(const T& low, const T& high, const T& f)
{ return f>=low && f<=high; }

// -------------------------------------------------------------------------

//! Returns true if the QDataStream ds is ready for storing (writing).
inline bool IsStoring(QDataStream& ds)
{
   const QIODevice* pD = ds.device();
   Q_ASSERT(pD);
   return pD->isWritable();
}

// --------------------------------------------------------------------
//! Returns true if the QDataStream ds is ready for reading.
inline bool IsReading(QDataStream& ds)
{
   const QIODevice* pD = ds.device();
   Q_ASSERT(pD);
   return pD->isReadable();
}

// --------------------------------------------------------------------

//! Compare (binary) two files.
bool CompareFiles(const QString& qsFile1, const QString& qsFile2);

// --------------------------------------------------------------------

//! Copy one file into another.
bool CopyFile(const QString& qsSource, const QString& qsDest);

// --------------------------------------------------------------------

//! Get the MD5 of given file.
QByteArray GetMD5(const QString qsFile);

// --------------------------------------------------------------------
//! Used to simplify (and shorten) the internal versioning used in classes.
/*! We are using unsigned char (byte) for versioning. One byte should be
    enough, since I can't imagine a class which has more than 256 different
    versions.
 */
inline unsigned char SerializeVersion(QDataStream& ds, unsigned char ucVer)
{
   if(IsStoring(ds))
      ds << ucVer;
   else
      ds >> ucVer;
   return ucVer;
}

// --------------------------------------------------------------------
//! Serialize the std::string object. We need this often.
void Serialize(QDataStream& ds, std::string& ss);

// --------------------------------------------------------------------
//! Serialize the QString object.
void Serialize(QDataStream& ds, QString& cs);

// --------------------------------------------------------------------
//! Convert point to radians.
inline QPointF Rad(const QPointF& ptDeg) 
{ 
	return ptDeg*(M_PI/180.0); 
}
// --------------------------------------------------------------------
//! Convert size to radians.
inline QSizeF Rad(const QSizeF& szDeg) 
{ 
	return szDeg*(M_PI/180.0); 
}
// --------------------------------------------------------------------
//! Convert rect to radians.
inline QRectF Rad(const QRectF& rcDeg) 
{ 
	return QRectF(Rad(rcDeg.topLeft()), Rad(rcDeg.size())); 
}
// --------------------------------------------------------------------
//! Convert point to degrees.
inline QPointF Deg(const QPointF& ptRad) 
{ 
	return ptRad*(180.0/M_PI); 
}
// --------------------------------------------------------------------
//! Convert size to degrees.
inline QSizeF Deg(const QSizeF& szRad) 
{ 
	return szRad*(180.0/M_PI); 
}
// --------------------------------------------------------------------
//! Convert rect to degrees.
inline QRectF Deg(const QRectF& rcRad) 
{ 
	return QRectF(Deg(rcRad.topLeft()), Deg(rcRad.size())); 
}

} // namespace common

#endif // __COMMONDEFS_QT_H

// Konec Datoteke
