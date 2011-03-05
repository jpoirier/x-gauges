/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>

#ifdef Q_WS_QWS

#include "qdecorationkanardia.h"

QT_BEGIN_NAMESPACE

#if !defined(QT_NO_QWS_DECORATION_DEFAULT) || defined(QT_PLUGIN)


/*!
  \class QDecorationKanardia
  \since 4.4
  \ingroup qws
  \brief The QDecorationKanardia class is a base class providing default window decorations.

  See the documentation for class QDecoration for a detailed
  description. This subclass of QDecoration provides standard
  icons for the decoration regions.

  Note that this class is non-portable and only available in
  \l{Qt for Embedded Linux}.
 */

/*!
  Default constructor.
 */
QDecorationKanardia::QDecorationKanardia()
    : QDecoration()
{
    menu_width = 20;
    help_width = 20;
    close_width = 20;
    minimize_width = 20;
    maximize_width = 20;
    normalize_width = 20;
}

/*!
  The constructor deletes the static pixmaps.
 */
QDecorationKanardia::~QDecorationKanardia()
{
}

/*!
  \fn const char **QDecorationKanardia::xpmForRegion(int region)

  Returns a pointer to the X pixmap for the icon specified by
  \a region. An X pixmap is an ASCII-text-based image. The value
  of \a region must be one of a subset of the values of enum
  DecorationRegion. The supported values are \e Help, \e Menu,
  \e Close, \e Minimize, \e Maximize, and \e Normalize. Other
  values of \a region cause zero to be returned.

  \sa QDecoration::DecorationRegion
 */
const char **QDecorationKanardia::xpmForRegion(int reg)
{
    Q_UNUSED(reg);

    return 0;
}

/*!
  \fn QPixmap QDecorationKanardia::pixmapFor(const QWidget *widget, int decorationRegion, int &xoff, int &yoff)

  Returns a pointer to the QPixmap for the widget specified by \a widget and
  \a decorationRegion. The returned QPixmap is constructed from the default
  X pixmap obtained from xpmForRegion().

  The value of \a decorationRegion must be one of a subset of the values
  of enum DecorationRegion. The supported values are \e Help,
  \e Menu, \e Close, \e Minimize, \e Maximize, and \e Normalize.
  Other values of \a decorationRegion return 0.

  \sa QDecoration::DecorationRegion

 */
QPixmap QDecorationKanardia::pixmapFor(const QWidget *widget,
                                      int decorationRegion,
                                      int &xoff,
                                      int &/*yoff*/)
{
	Q_UNUSED(widget);
	Q_UNUSED(decorationRegion);
	Q_UNUSED(xoff);
	return QPixmap();
}

/*!
  Returns the title bar height in pixels. It is the greater of 20,
  or the sum of the application font's line spacing value plus a
  border width fudge factor.
*/
int QDecorationKanardia::titleBarHeight(const QWidget *)
{
    return 0;
}

/*!
  Returns the region specified by \a decorationRegion for the
  top-level \a widget. \a rect specifies the rectangle the decoration
  wraps. The value of \a decorationRegion is a combination of the
  bitmask values of enum DecorationRegion.
 */
QRegion QDecorationKanardia::region(const QWidget *widget,
                                   const QRect &rect,
                                   int decorationRegion)
{
	Qt::WindowFlags flags = widget->windowFlags();
	bool hasBorder = !widget->isMaximized();

	const int bw = hasBorder ? 3 : 0;

	QRegion region;
	switch (decorationRegion) {
		case All: {
				QRect r(rect.left() - bw,
						rect.top() - bw,
						rect.width() + 2 * bw,
						rect.height() + 2 * bw);
				region = r;
				region -= rect;
			}
			break;

		case Title:
			break;

		case Top: {
					QRect r(rect.left() + CORNER_GRAB,
							rect.top() - bw,
							rect.width() - 2 * CORNER_GRAB,
							bw);
					region = r;
			}
			break;

		case Left: {
					QRect r(rect.left() - bw,
							rect.top() + CORNER_GRAB,
							bw,
							rect.height() - 2 * CORNER_GRAB);
					region = r;
			}
			break;

		case Right: {
					QRect r(rect.right() + 1,
							rect.top() + CORNER_GRAB,
							bw,
							rect.height() - 2 * CORNER_GRAB);
					region = r;
			}
			break;

		case Bottom: {
					QRect r(rect.left() + CORNER_GRAB,
							rect.bottom() + 1,
							rect.width() - 2 * CORNER_GRAB,
							bw);
					region = r;
			}
			break;

		case TopLeft: {
					QRect r1(rect.left() - bw,
							rect.top() - bw,
							CORNER_GRAB + bw,
							bw);

					QRect r2(rect.left() - bw,
							rect.top() - bw,
							bw,
							CORNER_GRAB + bw);

					region = QRegion(r1) + r2;
			}
			break;

		case TopRight: {
					QRect r1(rect.right() - CORNER_GRAB,
							rect.top() - bw,
							CORNER_GRAB + bw,
							bw);

					QRect r2(rect.right() + 1,
							rect.top() - bw,
							bw,
							CORNER_GRAB + bw);

					region = QRegion(r1) + r2;
			}
			break;

		case BottomLeft: {
					QRect r1(rect.left() - bw,
							rect.bottom() + 1,
							CORNER_GRAB + bw,
							bw);

					QRect r2(rect.left() - bw,
							rect.bottom() - CORNER_GRAB,
							bw,
							CORNER_GRAB + bw);
					region = QRegion(r1) + r2;
			}
			break;

		case BottomRight: {
					QRect r1(rect.right() - CORNER_GRAB,
							rect.bottom() + 1,
							CORNER_GRAB + bw,
							bw);

					QRect r2(rect.right() + 1,
							rect.bottom() - CORNER_GRAB,
							bw,
							CORNER_GRAB + bw);
					region = QRegion(r1) + r2;
			}
			break;

		case Menu:
		case Help:
		case Minimize:
		case Maximize:
		case Normalize:
		case Close:
	
		default: {
			int i = 1;
			while (i) {
				if (i & decorationRegion)
					region += this->region(widget, rect, i);
				i <<= 1;
			}
		}
		break;
	}

	return region;
}

/*!
  Paints the border and title decoration for the top-level \a widget
  using the \a painter provided and the decoration \a state. The value
  of \a decorationRegion is a combination of the bitmask values of
  enum DecorationRegion.

  Note that Qt for Embedded Linux expects this function to return true if any of
  the widget's decorations are repainted; otherwise it returns false.
 */
bool QDecorationKanardia::paint(QPainter *painter,
                               const QWidget *widget,
                               int decorationRegion,
                               DecorationState state)
{
	if (decorationRegion == None)
		return false;

	const QPalette pal = QApplication::palette();
	QRegion oldClipRegion = painter->clipRegion();


	Qt::WindowFlags flags = widget->windowFlags();
	bool hasBorder = !widget->isMaximized();

	bool paintAll = (decorationRegion == int(All));
	bool handled = false;

	if ((paintAll || decorationRegion & Borders) && state == Normal && hasBorder) {
		QRect br = QDecoration::region(widget).boundingRect();
		painter->setBrush(QBrush(QColor(27,88,255)));
		painter->drawRect(br.adjusted(-1,-1,0,0));
		handled |= true;
	}

	return handled;
}

/*!
  Paints a region of the top-level \a widget. The region is
  painted in the specified decoration \a state using the
  \a painter provided. The region to be painted is specified
  by \a buttonRegion, which is a combination of the bitmask
  values of DecorationRegion. If the value of \a buttonRegion
  is one of \e Help, \e Menu, \e Close, \e Minimize, \e Maximize,
  and \e Normalize, the button pxmap for that region is painted.

  \sa pixmapFor()
 */
void QDecorationKanardia::paintButton(QPainter *painter,
                                     const QWidget *widget,
                                     int buttonRegion,
                                     DecorationState state,
                                     const QPalette &pal)
{

}

#endif // QT_NO_QWS_DECORATION_DEFAULT

QT_END_NAMESPACE

#endif
