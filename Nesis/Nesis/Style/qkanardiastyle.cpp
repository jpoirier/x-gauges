/****************************************************************************
**
** Copyright (C) 1992-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.0, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** In addition, as a special exception, Trolltech, as the sole copyright
** holder for Qt Designer, grants users of the Qt/Eclipse Integration
** plug-in the right for the Qt/Eclipse Integration to link to
** functionality provided by Qt Designer and its related libraries.
**
** Trolltech reserves all rights not expressly granted herein.
**
** Trolltech ASA (c) 2007
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "qkanardiastyle.h"

#if defined(QT_PLUGIN)

#include "qmenu.h"
#include "qapplication.h"
#include "qpainter.h"
#include "qdrawutil.h"
#include "qpixmap.h"
#include "qpalette.h"
#include "qwidget.h"
#include "qpushbutton.h"
#include "qscrollbar.h"
#include "qtabbar.h"
#include "qtabwidget.h"
#include "qlistview.h"
#include "qsplitter.h"
#include "qslider.h"
#include "qcombobox.h"
#include "qlineedit.h"
#include "qprogressbar.h"
#include "qimage.h"
#include "qfocusframe.h"
#include "qpainterpath.h"
#include "qdebug.h"
#include <limits.h>
#include <QTextCharFormat>
#include <qpixmapcache.h>

#define ROUND_SLIDER 90

/*!
    \class QKanardiaStyle
    \brief The QKanardiaStyle class provides a nesis look and feel.
*/

QKanardiaStyle::QKanardiaStyle(bool /*useHighlightCols*/)
		: QMotifStyle(true) {}

QKanardiaStyle::~QKanardiaStyle() {}

int QKanardiaStyle::pixelMetric(PixelMetric metric, const QStyleOption *option,
										  const QWidget *widget) const {
	int ret = 0;

	switch (metric) {

//   case PM_LayoutHorizontalSpacing:
//    ret = 8;
//    break;
//   case PM_IndicatorWidth:

		case PM_ScrollBarSliderMin:
			ret = 20;
			break;

		case PM_ProgressBarChunkWidth:
			ret = 6;
			break;

		case PM_DialogButtonsSeparator:
			ret = 5;
			break;

		case PM_TabBarTabShiftHorizontal:
			ret = 2;
			break;

		case PM_FocusFrameVMargin:

		case PM_FocusFrameHMargin:
			ret = 0;
			break;

		case PM_ButtonMargin:
			ret = 6;
			break;

		case PM_TabBarBaseOverlap:
			ret = 3;
			break;

//   case PM_TabBarTabHSpace:
//    ret = 30;
//    break;

		case PM_MenuBarPanelWidth:

		case PM_ComboBoxFrameWidth:

		case PM_DefaultFrameWidth:

		case PM_MenuPanelWidth:

		case PM_SpinBoxFrameWidth:

		case PM_MenuBarVMargin:

		case PM_MenuBarHMargin:

//   case PM_ButtonDefaultIndicator:

		case PM_DockWidgetFrameWidth:
			ret = 3;
			break;

		case PM_ScrollBarExtent:
			ret = 14;
			break;

		case PM_TextCursorWidth:
			ret = 2;
			break;
			
		default:
			ret = QCommonStyle::pixelMetric(metric, option, widget);
			break;
	}

	return ret;
}


/* XPM */
static const char* check_xpm[] = {
	"22 22 25 1",
	"  c None",
	". c #024A03",
	"+ c #186313",
	"@ c #047000",
	"# c #1F821C",
	"$ c #0C8C02",
	"% c #1EA010",
	"& c #389733",
	"* c #52974B",
	"= c #25BA13",
	"- c #35B42D",
	"; c #66AF5D",
	"> c #77AD74",
	", c #3FCF2F",
	"' c #5CC456",
	") c #9BC89A",
	"! c #71DB69",
	"~ c #5BF049",
	"{ c #6BF75F",
	"] c #7BF36E",
	"^ c #A1EC9A",
	"/ c #C0E7BB",
	"( c #9EFD93",
	"_ c #BCF1B6",
	": c #E5F8DF",
	"                      ",
	"                      ",
	"                 ##   ",
	"                $)>@  ",
	"               $>))+. ",
	"              $>/;@>+ ",
	"             $*/'$$&. ",
	"   %$       $*_]%%%+. ",
	"  $;)$     %&/]==%+.  ",
	"  &::>$   =&_],=-+.   ",
	" *//_:;%  &_(~,=+.    ",
	"@*'^_^:;%&_({,,#+     ",
	" #*-!^(:;_({~,&@      ",
	"  +&,!^(_^]~~-@       ",
	"   @%~{((]~{,#        ",
	"    #-~{]{{,$         ",
	"     $-{{]{#          ",
	"      $'(]&           ",
	"       $'&(           ",
	"        #]            ",
	"                      ",
	"                      "
};


/* XPM */
static const char* uncheck_xpm[] = {
	"22 22 25 1",
	"  c None",
	". c #000100",
	"+ c #310A08",
	"@ c #6C0505",
	"# c #A50002",
	"$ c #5C1417",
	"% c #BE0000",
	"& c #D00001",
	"* c #E20005",
	"= c #452C36",
	"- c #622C3C",
	"; c #C11818",
	"> c #FC1216",
	", c #E21A1D",
	"' c #913138",
	") c #B42A2E",
	"! c #DB2C30",
	"~ c #FD4044",
	"{ c #D45356",
	"] c #F3555D",
	"^ c #9C7B88",
	"/ c #E28085",
	"( c #FC7E87",
	"_ c #FE969B",
	": c #F9BEC6",
	"                      ",
	"                      ",
	"    :_          {:    ",
	"   '::;        )::'   ",
	"  $(((_!      !_((_'. ",
	"=^(]]]](!#   !]]]]]]'=",
	"-^]>!!]]]!) ;~~!!!,~'-",
	" .+,>*,~~>((>>***>>$. ",
	"  .$,*%&*>~>>*%%*>@.  ",
	"    $,%#%&*&%%#%&@+   ",
	"     @;%######%%@     ",
	"      {;######;)@     ",
	"     #;########;#     ",
	"    #;##########;#    ",
	"   #,##%%%,,%%%#%,#   ",
	"  ;,%%%%%*!]*&%%%%,;  ",
	"/{&%%&&*>%##%>*&&%%&!)",
	"{{%%&**>;#  #;>**&%%!)",
	" #%,**>;%    %;>**,,% ",
	"  %*>,;%       ;>>,%  ",
	"   &({&         )]&   ",
	"    )!           )    "
};


static const char* reload_xpm[] = {
	"22 22 23 1",
	"  c None",
	". c #302B14",
	"+ c #1E4E1A",
	"@ c #763C0F",
	"# c #19630B",
	"$ c #76540B",
	"% c #AD5503",
	"& c #1F8D11",
	"* c #3B8835",
	"= c #D36A08",
	"- c #1EB715",
	"; c #C3883F",
	"> c #50AB4B",
	", c #D28929",
	"' c #F58E17",
	") c #79BF39",
	"! c #34E220",
	"~ c #FFA825",
	"{ c #F4AE47",
	"] c #5DE24E",
	"^ c #FFC138",
	"/ c #F4C88C",
	"( c #97EF8F",
	"              @.      ",
	"              $/@     ",
	"              @//@    ",
	" ;;;,,,,{,{,,,%/{;$   ",
	" ;{{{^^^^^^^~~,,,,=$  ",
	" ;{{{^^^^^^^~~''====$ ",
	" %,''~~~^{^^~~''==%%@ ",
	" %,='''~{{{{{~''==%@# ",
	" %,='%%%%%%%%%%~'=$   ",
	" %''~@        ,{'$] # ",
	" %''~$]>      ,/$]!-& ",
	" %''{)(>      %=#]]-& ",
	" %,')](>        &]]-& ",
	" @,&(]]>*********]-&# ",
	" $#>--](((((((((()-## ",
	" +**&--]!!!!!!!!!-&## ",
	" +***>&---!!!!!!--&## ",
	"  .****>&--------&&#+ ",
	"   .***#+++++++++++++ ",
	"    .**#              ",
	"     .*+              ",
	"      ..              "
};


static void qt_kanardia_draw_frame(QPainter *painter, const QRect &rect, const QStyleOption *option, const QWidget *widget = NULL);

// static void qt_plastique_draw_gradient(QPainter *painter, const QRect &rect, const QColor &gradientStart,
//                                        const QColor &gradientStop)
// {
//     QString gradientName;
//     gradientName.sprintf("%dx%d-%x-%x", rect.width(), rect.height(), gradientStart.rgba(), gradientStop.rgba());
//     QPixmap cache;
//     QPainter *p = painter;
//     QRect r = rect;
// 
//     bool doPixmapCache = true	&&
//     			painter->deviceTransform().isIdentity() &&
//     			painter->worldMatrix().isIdentity();
//     			
//     if (doPixmapCache && QPixmapCache::find(gradientName, cache)) {
//         painter->drawPixmap(rect, cache);
//     } else {
//         if (doPixmapCache) {
//             cache = QPixmap(rect.size());
//             cache.fill(Qt::transparent);
//             p = new QPainter(&cache);
// 	    r = QRect(0, 0, rect.width(), rect.height());
//         }
// 
//         int x = r.center().x();
//         QLinearGradient gradient(x, r.top(), x, r.bottom());
//         gradient.setColorAt(0, gradientStart);
//         gradient.setColorAt(1, gradientStop);
//         p->fillRect(r, gradient);
// 
//         if (doPixmapCache) {
// 	    p->end();
// 	    delete p;
// 	    painter->drawPixmap(rect, cache);
//             QPixmapCache::insert(gradientName, cache);
// 	}
//     }
// }

// ---------------------------------------------------------------------------

static void qt_kanardia_draw_frame(
	QPainter* painter, 
	const QRect& rect, 
	const QStyleOption* option, 
	const QWidget* /*widget*/
)
{
	QPen oldPen = painter->pen();
	QColor border;

	bool bFocus = option->state & QStyle::State_HasFocus;
// 	bool bEnabled = option->state & QStyle::State_Enabled;

	if (bFocus) {
		border = QColor("red");
	} else {
		border = option->palette.shadow().color();
	}

	QLine lines[4];

	QPoint points[8];

	// Opaque corner lines
	painter->setPen(border);

	lines[0] = QLine(rect.left() + 2, rect.top(), rect.right() - 2, rect.top());
	lines[1] = QLine(rect.left() + 2, rect.bottom(), rect.right() - 2, rect.bottom());
	lines[2] = QLine(rect.left(), rect.top() + 2, rect.left(), rect.bottom() - 2);
	lines[3] = QLine(rect.right(), rect.top() + 2, rect.right(), rect.bottom() - 2);

	painter->drawLines(lines, 4);

	// Opaque corner dots
	points[0] = QPoint(rect.left() + 1, rect.top() + 1);
	points[1] = QPoint(rect.left() + 1, rect.bottom() - 1);
	points[2] = QPoint(rect.right() - 1, rect.top() + 1);
	points[3] = QPoint(rect.right() - 1, rect.bottom() - 1);

	painter->drawPoints(points, 4);

	if (bFocus) {
		painter->drawRect(rect.adjusted(1, 1, -2, -2));
		points[0] = QPoint(rect.left() + 2, rect.top() + 2);
		points[1] = QPoint(rect.left() + 2, rect.bottom() - 2);
		points[2] = QPoint(rect.right() - 2, rect.top() + 2);
		points[3] = QPoint(rect.right() - 2, rect.bottom() - 2);
		painter->drawPoints(points, 4);
	}

	painter->setPen(oldPen);
}

// ---------------------------------------------------------------------------

void QKanardiaStyle::drawControl(
	ControlElement element,
	const QStyleOption *opt,
	QPainter *p,
	const QWidget *widget
) const
{
	switch (element) {

		case CE_TabBarTab:

			if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *> (opt)) {
				drawControl(CE_TabBarTabShape, tab, p, widget);
				drawControl(CE_TabBarTabLabel, tab, p, widget);
			}

			break;

		case CE_TabBarTabShape:

			if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *> (opt)) {
				bool selected = tab->state & State_Selected;
/*				bool onlyOne = tab->position == QStyleOptionTab::OnlyOneTab;
				int tabOverlap = onlyOne ? 0 : pixelMetric(PM_TabBarTabOverlap, opt, widget);*/
				QRect tr = tab->rect;
				QLine lines[6];
				QPoint points[4];

				if (!selected)
					tr.adjust(0, 3, 0, 0);

				lines[0] = QLine(tr.left() + 2, tr.top(), tr.right() - 2, tr.top());

				lines[1] = QLine(tr.left(), tr.top() + 2, tr.left(), tr.bottom() - 2);

				lines[2] = QLine(tr.right(), tr.top() + 2, tr.right(), tr.bottom() - 2);

				points[0] = QPoint(tr.left() + 1, tr.top() + 1);

				points[1] = QPoint(tr.right() - 1, tr.top() + 1);


				if (tab->state & State_HasFocus) {
					p->fillRect(tr.adjusted(2, 2, -2, 0), tab->palette.light());
					lines[3] = QLine(tr.left() + 2, tr.top() + 1, tr.right() - 2, tr.top() + 1);
					lines[4] = QLine(tr.left() + 1, tr.top() + 2, tr.left() + 1, tr.bottom() - 2);
					lines[5] = QLine(tr.right() - 1, tr.top() + 2, tr.right() - 1, tr.bottom() - 2);
					points[2] = QPoint(tr.left() + 2, tr.top() + 2);
					points[3] = QPoint(tr.right() - 2, tr.top() + 2);
					p->setPen(Qt::red);
					p->drawLines(lines, 6);
					p->drawPoints(points, 4);
				} else {
					if (selected)
						p->fillRect(tr.adjusted(1, 1, -1, 0), opt->palette.button());

					p->drawLines(lines, 3);

					p->drawPoints(points, 2);
				}
			}

			break;

		case CE_TabBarTabLabel:

			if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *> (opt)) {
				QStyleOptionTabV2 tabV2(*tab);
				QRect tr = tabV2.rect;
				bool verticalTabs = tabV2.shape == QTabBar::RoundedEast
										  || tabV2.shape == QTabBar::RoundedWest
										  || tabV2.shape == QTabBar::TriangularEast
										  || tabV2.shape == QTabBar::TriangularWest;
				bool selected = tabV2.state & State_Selected;

				if (verticalTabs) {
					p->save();
					int newX, newY, newRot;

					if (tabV2.shape == QTabBar::RoundedEast || tabV2.shape == QTabBar::TriangularEast) {
						newX = tr.width();
						newY = tr.y();
						newRot = 90;
					} else {
						newX = 0;
						newY = tr.y() + tr.height();
						newRot = -90;
					}

					tr.setRect(0, 0, tr.height(), tr.width());

					QTransform m;
					m.translate(newX, newY);
					m.rotate(newRot);
					p->setTransform(m, true);
				}

				tr.adjust(2, 0, pixelMetric(QStyle::PM_TabBarTabShiftHorizontal, tab, widget),

							 pixelMetric(QStyle::PM_TabBarTabShiftVertical, tab, widget));

				if (selected) {
					tr.setBottom(tr.bottom() - pixelMetric(QStyle::PM_TabBarTabShiftVertical, tab,
																		widget));
					tr.setRight(tr.right() - pixelMetric(QStyle::PM_TabBarTabShiftHorizontal, tab,
																	 widget));
				}

				int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;

				if (!tabV2.icon.isNull()) {
					QSize iconSize = tabV2.iconSize;

					if (!iconSize.isValid()) {
						int iconExtent = pixelMetric(PM_SmallIconSize);
						iconSize = QSize(iconExtent, iconExtent);
					}

					QPixmap tabIcon = tabV2.icon.pixmap(iconSize,

																	(tabV2.state & State_Enabled) ? QIcon::Normal
																	: QIcon::Disabled);
					p->drawPixmap(tr.left() + 6, tr.center().y() - tabIcon.height() / 2, tabIcon);
					tr.setLeft(tr.left() + iconSize.width() + 4);
				}

				drawItemText(p, tr, alignment, tab->palette, tab->state & State_Enabled, tab->text, QPalette::WindowText);

				if (verticalTabs)
					p->restore();

				/*            if (tabV2.state & State_HasFocus) {
				                const int OFFSET = 1 + pixelMetric(PM_DefaultFrameWidth);

				                int x1, x2;
				                x1 = tabV2.rect.left();
				                x2 = tabV2.rect.right() - 1;

				                QStyleOptionFocusRect fropt;
				                fropt.QStyleOption::operator=(*tab);
				                fropt.rect.setRect(x1 + 1 + OFFSET, tabV2.rect.y() + OFFSET,
				                                   x2 - x1 - 2*OFFSET, tabV2.rect.height() - 2*OFFSET);
				                drawPrimitive(PE_FrameFocusRect, &fropt, p, widget);
				            }*/
			}

			break;

		case CE_CheckBox:

			if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *> (opt)) {
				p->fillRect(btn->rect.adjusted(1, 1, -1, -1), opt->palette.base());
				bool isRadio = (element == CE_RadioButton);
				QStyleOptionButton subopt = *btn;
				subopt.rect = subElementRect(isRadio ? SE_RadioButtonIndicator
													  : SE_CheckBoxIndicator, btn, widget);
				subopt.rect.adjust(5, 0, 5, 0);
				drawPrimitive(isRadio ? PE_IndicatorRadioButton : PE_IndicatorCheckBox,
								  &subopt, p, widget);
				subopt.rect = subElementRect(isRadio ? SE_RadioButtonContents
													  : SE_CheckBoxContents, btn, widget);
				subopt.rect.adjust(5, 0, 5, 0);
				drawControl(isRadio ? CE_RadioButtonLabel : CE_CheckBoxLabel, &subopt, p, widget);

//     if (btn->state & State_HasFocus) {
				qt_kanardia_draw_frame(p, btn->rect, opt);
//     }
			}

			break;


		case CE_ScrollBarSubLine:

		case CE_ScrollBarAddLine:

		case CE_ScrollBarSubPage:

		case CE_ScrollBarAddPage:
			break;

		case CE_FocusFrame:
//    p->fillRect(opt->rect, QBrush(QColor("magenta")));
			// focus frame
//       p->save();
//       p->setBrush(QColor ( "orangered" ));
//       p->drawRect ( opt->rect );
//       p->restore();
			break;

		case CE_PushButton:

			if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *> (opt)) {
				drawControl(CE_PushButtonBevel, btn, p, widget);
				QStyleOptionButton subopt = *btn;
				drawControl(CE_PushButtonLabel, &subopt, p, widget);
			}

			break;

		case CE_PushButtonBevel:

			if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *> (opt)) {
				p->setRenderHint(QPainter::Antialiasing, true);
//				p->setBrush(QBrush(QColor(0xa2, 0xcd, 0xde)));
				p->setBrush(QBrush(QColor(27,88,255)));
// 				p->setOpacity(0.5);
				p->setPen(QPen(Qt::NoPen));
				p->drawEllipse(opt->rect);
//				p->setPen(QPen(QColor("black")));
//				p->setBrush(QBrush(QColor(0x49, 0xBB, 0xC5)));
				p->setBrush(QBrush(Qt::white));
				p->drawEllipse(opt->rect.adjusted(4, 4, -4, -4));
				p->setRenderHint(QPainter::Antialiasing, false);
			}
			break;

		case CE_HeaderSection:

			if (const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(opt)) {
				p->fillRect(header->rect, opt->palette.light());

				QLine lines[4];
				p->setPen(opt->palette.dark().color());
				lines[0] = QLine(opt->rect.right(), opt->rect.top(), opt->rect.right(), opt->rect.bottom());
				lines[1] = QLine(opt->rect.left() , opt->rect.bottom(), opt->rect.right() , opt->rect.bottom());
				p->drawLines(lines, 2);
				break;
			}

		case CE_ScrollBarSlider:
			p->fillRect(opt->rect.adjusted(1, 1, -1, -1), opt->palette.light());
			qt_kanardia_draw_frame(p, opt->rect, opt);
			break;

		case CE_ProgressBar:
			if (const QStyleOptionProgressBar *pb
					= qstyleoption_cast<const QStyleOptionProgressBar *>(opt)) {
				QStyleOptionProgressBarV2 subopt = *pb;
				subopt.rect = subElementRect(SE_ProgressBarGroove, pb, widget);
				drawControl(CE_ProgressBarGroove, &subopt, p, widget);
				subopt.rect = subElementRect(SE_ProgressBarContents, pb, widget);
				drawControl(CE_ProgressBarContents, &subopt, p, widget);

				if (pb->textVisible) {
					subopt.rect = subElementRect(SE_ProgressBarLabel, pb, widget);
					drawControl(CE_ProgressBarLabel, &subopt, p, widget);
				}
			}

			break;

		case CE_ProgressBarGroove:
			p->fillRect(opt->rect.adjusted(1, 1, -1, -1), opt->palette.light());
			qt_kanardia_draw_frame(p, opt->rect, opt);
			break;

		case CE_ProgressBarLabel:

			if (const QStyleOptionProgressBar *pb = qstyleoption_cast<const QStyleOptionProgressBar *>(opt)) {
				bool vertical = false;

//     if (const QStyleOptionProgressBarV2 *pb2 = qstyleoption_cast<const QStyleOptionProgressBarV2 *>(opt)) {
//      vertical = (pb2->orientation == Qt::Vertical);
//     }
//
//     if (!vertical) {
//      QPalette::ColorRole textRole = QPalette::NoRole;
//
//      if ((pb->textAlignment & Qt::AlignCenter) && pb->textVisible
//        && ((qint64(pb->progress) - qint64(pb->minimum)) * 2 >= (qint64(pb->maximum) - qint64(pb->minimum)))) {
//       textRole = QPalette::HighlightedText;
//       //Draw text shadow, This will increase readability when the background of same color
//       QRect shadowRect(pb->rect);
//       shadowRect.translate(1, 1);
//       QColor shadowColor = (pb->palette.color(textRole).value() <= 128)
//              ? QColor(255, 255, 255, 160) : QColor(0, 0, 0, 160);
//       QPalette shadowPalette = pb->palette;
//       shadowPalette.setColor(textRole, shadowColor);
//       drawItemText(p, shadowRect, Qt::AlignCenter | Qt::TextSingleLine, shadowPalette,
//            pb->state & State_Enabled, pb->text, textRole);
//      }
//
//      drawItemText(p, pb->rect, Qt::AlignCenter | Qt::TextSingleLine, pb->palette,
//
//           pb->state & State_Enabled, pb->text, textRole);
//     }
			}

			break;

		case CE_ProgressBarContents:

			if (const QStyleOptionProgressBar *pb = qstyleoption_cast<const QStyleOptionProgressBar *>(opt)) {
				QRect rect = pb->rect;
				bool vertical = false;
				bool inverted = false;
				qint64 minimum = qint64(pb->minimum);
				qint64 maximum = qint64(pb->maximum);
				qint64 progress = qint64(pb->progress);

				// Get extra style options if version 2
				const QStyleOptionProgressBarV2 *pb2 = qstyleoption_cast<const QStyleOptionProgressBarV2 *>(opt);

				if (pb2) {
					vertical = (pb2->orientation == Qt::Vertical);
					inverted = pb2->invertedAppearance;
				}

				QMatrix m;

				if (vertical) {
					rect = QRect(rect.y(), rect.x(), rect.height(), rect.width()); // flip width and height
					m.rotate(90);
					m.translate(0, -(rect.height() + rect.y()*2));
				}

				QPalette pal2 = pb->palette;

				// Correct the highlight color if it is the same as the background

//     if (pal2.highlight() == pal2.background())
//      pal2.setColor(QPalette::Highlight, pb->palette.color(QPalette::Active,
//            QPalette::Highlight));

				bool reverse = ((!vertical && (pb->direction == Qt::RightToLeft)) || vertical);

				if (inverted)
					reverse = !reverse;

				int w = rect.width() - 5;

				if (pb->minimum == 0 && pb->maximum == 0) {
					// draw busy indicator
					int x = (progress - minimum) % (w * 2);

					if (x > w)
						x = 2 * w - x;

					x = reverse ? rect.right() - x : x + rect.x();

					p->setPen(QPen(pal2.highlight().color(), 4));

					p->drawLine(x, rect.y(), x, rect.height());
				} else {
					const int unit_width = pixelMetric(PM_ProgressBarChunkWidth, pb, widget);

					if (!unit_width)
						return;

					int u;

					if (unit_width > 1)
						u = ((rect.width() + unit_width) / unit_width);
					else
						u = w / unit_width;

					qint64 p_v = progress - minimum;

					qint64 t_s = (maximum - minimum) ? (maximum - minimum) : qint64(1);

					if (u > 0 && p_v >= INT_MAX / u && t_s >= u) {
						// scale down to something usable.
						p_v /= u;
						t_s /= u;
					}

					// nu < tnu, if last chunk is only a partial chunk
					int tnu, nu;

					tnu = nu = p_v * u / t_s;

					if (nu * unit_width > w)
						--nu;

					// Draw nu units out of a possible u of unit_width
					// width, each a rectangle bordered by background
					// color, all in a sunken panel with a percentage text
					// display at the end.
					int x = 0;

					int x0 = reverse ? rect.right() - ((unit_width > 1) ? unit_width : 0)
										: rect.x();

					QStyleOptionProgressBarV2 pbBits = *pb;

					pbBits.rect = rect;

					pbBits.palette = pal2;

					int myY = pbBits.rect.y();

					int myHeight = pbBits.rect.height();

					pbBits.state = State_None;

					for (int i = 0; i < nu; ++i) {
						pbBits.rect.setRect(x0 + x, myY, unit_width, myHeight);
						pbBits.rect = m.mapRect(QRectF(pbBits.rect)).toRect();
						drawPrimitive(PE_IndicatorProgressChunk, &pbBits, p, widget);
						x += reverse ? -unit_width : unit_width;
					}

					// Draw the last partial chunk to fill up the
					// progress bar entirely
					if (nu < tnu) {
						int pixels_left = w - (nu * unit_width);
						int offset = reverse ? x0 + x + unit_width - pixels_left : x0 + x;
						pbBits.rect.setRect(offset, myY, pixels_left, myHeight);
						pbBits.rect = m.mapRect(QRectF(pbBits.rect)).toRect();
						drawPrimitive(PE_IndicatorProgressChunk, &pbBits, p, widget);
					}
				}
			}

			break;
    
		case CE_MenuItem:
			// Draws one item in a popup menu.
			if (const QStyleOptionMenuItem *menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) {
				p->save();
				QBrush textBrush;
				if (opt->palette.resolve() & (1 << QPalette::ButtonText))
					textBrush = opt->palette.buttonText();
				else
					textBrush = opt->palette.windowText(); // KDE uses windowText rather than buttonText for menus

				if (menuItem->menuItemType == QStyleOptionMenuItem::Separator) {
					p->fillRect(menuItem->rect, opt->palette.background().color());

					int w = 0;
// 					if (!menuItem->text.isEmpty()) {
// 						p->setFont(menuItem->font);
// 						drawItemText(p, menuItem->rect.adjusted(5, 0, -5, 0), Qt::AlignLeft | Qt::AlignVCenter,
// 										menuItem->palette, menuItem->state & State_Enabled, menuItem->text,
// 										QPalette::Text);
// 						w = menuItem->fontMetrics.width(menuItem->text) + 5;
// 					}

					p->setPen(opt->palette.windowText().color());
					bool reverse = menuItem->direction == Qt::RightToLeft;
					p->drawLine(menuItem->rect.left() + 5 + (reverse ? 0 : w), menuItem->rect.center().y(),
											menuItem->rect.right() - 5 - (reverse ? w : 0), menuItem->rect.center().y());

					p->restore();
					break;
				}

				bool selected = menuItem->state & State_Selected;
				bool checkable = menuItem->checkType != QStyleOptionMenuItem::NotCheckable;
				bool checked = menuItem->checked;
				bool enabled = menuItem->state & State_Enabled;

				if (selected && enabled) {
					p->fillRect(opt->rect, opt->palette.highlight().color().lighter(130));

					p->setPen(opt->palette.highlight().color().lighter(110));
					p->drawLine(opt->rect.topLeft(), opt->rect.topRight());
					p->setPen(opt->palette.highlight().color().darker(115));
					p->drawLine(opt->rect.bottomLeft(), opt->rect.bottomRight());
				}
				else {
					p->fillRect(opt->rect, opt->palette.background().color());
				}
		
///////////////
            // Check
//             QRect checkRect(opt->rect.left() + 7, opt->rect.center().y() - 6, 13, 13);
//             checkRect = visualRect(menuItem->direction, menuItem->rect, checkRect);
//             if (checkable) {
//                 if ((menuItem->checkType & QStyleOptionMenuItem::Exclusive) && menuItem->icon.isNull()) {
//                     QStyleOptionButton button;
//                     button.rect = checkRect;
//                     button.state = menuItem->state;
//                     if (checked)
//                         button.state |= State_On;
//                     button.palette = menuItem->palette;
//                     drawPrimitive(PE_IndicatorRadioButton, &button, p, widget);
//                 } else {
//                     if (menuItem->icon.isNull()) {
//                         QStyleOptionButton button;
//                         button.rect = checkRect;
//                         button.state = menuItem->state;
//                         if (checked)
//                             button.state |= State_On;
//                         button.palette = menuItem->palette;
//                         drawPrimitive(PE_IndicatorCheckBox, &button, p, widget);
//                     } else if (checked) {
//                         int iconSize = qMax(menuItem->maxIconWidth, 20);
//                         QRect sunkenRect(opt->rect.left() + 1,
//                                          opt->rect.top() + (opt->rect.height() - iconSize) / 2 + 1,
//                                          iconSize, iconSize);
//                         sunkenRect = visualRect(menuItem->direction, menuItem->rect, sunkenRect);
// 
// //                         QStyleOption option = *opt;
// //                         option.state |= State_Sunken;
// //                         option.rect = sunkenRect;
// //                         qt_kanardia_draw_frame(p, &option, false, widget);
//                     }
//                 }
//             }

            // Text and icon, ripped from windows style
            bool dis = !(menuItem->state & State_Enabled);
            bool act = menuItem->state & State_Selected;
//             const QStyleOption *opt = opt;
            const QStyleOptionMenuItem *menuitem = menuItem;
            int checkcol = qMax(menuitem->maxIconWidth, 20);
            QRect vCheckRect = visualRect(opt->direction, menuitem->rect,
                                          QRect(menuitem->rect.x(), menuitem->rect.y(),
                                                checkcol, menuitem->rect.height()));
            if (!menuItem->icon.isNull()) {
                QIcon::Mode mode = dis ? QIcon::Disabled : QIcon::Normal;
                if (act && !dis)
                    mode = QIcon::Active;
                QPixmap pixmap;
                if (checked)
                    pixmap = menuItem->icon.pixmap(pixelMetric(PM_SmallIconSize, opt, widget), mode, QIcon::On);
                else
                    pixmap = menuItem->icon.pixmap(pixelMetric(PM_SmallIconSize, opt, widget), mode);
                int pixw = pixmap.width();
                int pixh = pixmap.height();

                QRect pmr(0, 0, pixw, pixh);
                pmr.moveCenter(vCheckRect.center());
                p->setPen(textBrush.color());
                if (checkable && checked)
                    p->drawPixmap(QPoint(pmr.left() + 1, pmr.top() + 1), pixmap);
                else
                    p->drawPixmap(pmr.topLeft(), pixmap);
            }

            if (selected) {
                p->setPen(menuItem->palette.highlightedText().color());
            } else {
                p->setPen(textBrush.color());
            }
            int x, y, w, h;
            menuitem->rect.getRect(&x, &y, &w, &h);
            int tab = menuitem->tabWidth;
            QColor discol;
            if (dis) {
                discol = textBrush.color();
                p->setPen(discol);
            }
            int xm = 2 + checkcol + 2;
            int xpos = menuitem->rect.x() + xm;
            QRect textRect(xpos, y + 2, w - xm - 2 - tab + 1, h - 2 * 2);
            QRect vTextRect = visualRect(opt->direction, menuitem->rect, textRect);
            QString s = menuitem->text;
            if (!s.isEmpty()) {                     // draw text
                p->save();
                int t = s.indexOf(QLatin1Char('\t'));
                int text_flags = Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
//                 if (!styleHint(SH_UnderlineShortcut, menuitem, widget))
//                     text_flags |= Qt::TextHideMnemonic;
                text_flags |= Qt::AlignLeft;
                if (t >= 0) {
                    QRect vShortcutRect = visualRect(opt->direction, menuitem->rect,
                        QRect(textRect.topRight(), QPoint(menuitem->rect.right(), textRect.bottom())));
//                     if (dis && !act && styleHint(SH_EtchDisabledText, opt, widget)) {
//                         p->setPen(menuitem->palette.light().color());
//                         p->drawText(vShortcutRect.adjusted(1,1,1,1), text_flags, s.mid(t + 1));
//                         p->setPen(discol);
//                     }
                    p->drawText(vShortcutRect, text_flags, s.mid(t + 1));
                    s = s.left(t);
                }
                QFont font = menuitem->font;
                if (menuitem->menuItemType == QStyleOptionMenuItem::DefaultItem)
                    font.setBold(true);
                p->setFont(font);
//                 if (dis && !act && styleHint(SH_EtchDisabledText, opt, widget)) {
//                     p->setPen(menuitem->palette.light().color());
//                     p->drawText(vTextRect.adjusted(1,1,1,1), text_flags, s.left(t));
//                     p->setPen(discol);
//                 }
                p->drawText(vTextRect, text_flags, s.left(t));
                p->restore();
            }

            // Arrow
            if (menuItem->menuItemType == QStyleOptionMenuItem::SubMenu) {
            	// draw sub menu arrow
					int dim = (menuItem->rect.height() - 4) / 2;
					PrimitiveElement arrow;
					arrow = (opt->direction == Qt::RightToLeft) ? PE_IndicatorArrowLeft : PE_IndicatorArrowRight;
					int xpos = menuItem->rect.left() + menuItem->rect.width() - 6 - 2 - dim;
					QRect vSubMenuRect = visualRect(
						opt->direction, menuItem->rect,
						QRect(xpos, menuItem->rect.top() + menuItem->rect.height() / 2 - dim / 2,
						dim, dim)
					);
					QStyleOptionMenuItem newMI = *menuItem;
					newMI.rect = vSubMenuRect;
					newMI.state = opt->state & State_Enabled;
					if (selected)
						newMI.palette.setColor(QPalette::ButtonText,
														newMI.palette.highlightedText().color());
					else
						newMI.palette.setColor(QPalette::ButtonText, textBrush.color());
					drawPrimitive(arrow, &newMI, p, widget);
            }

            p->restore();
			}
			break;
			
		default:
			QCommonStyle::drawControl(element, opt, p, widget);
			break;
	}
}

// --------------------------------------------------------------------------

QSize QKanardiaStyle::sizeFromContents(
	ContentsType type,
	const QStyleOption *opt,
	const QSize &size,
	const QWidget *widget) const
{
	QSize newSize(size);

	switch (type) {
 		case CT_ComboBox:
			newSize = sizeFromContents(CT_PushButton, opt, size, widget);
			newSize += QSize(0, 6);
			break;
// 			newSize += QSize(15, 0);
		
		case CT_PushButton:
			if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *> (opt)) {
				int w = size.width();
				int h = size.height();
				int bm = pixelMetric(PM_ButtonMargin, btn, widget);
				int fw = pixelMetric(PM_DefaultFrameWidth, btn, widget) * 2;

				w += bm + fw;
				h += bm + fw;

				if (btn->features & QStyleOptionButton::AutoDefaultButton) {
					int dbw = pixelMetric(PM_ButtonDefaultIndicator, btn, widget) * 2;
					w += dbw;
					h += dbw;
				}

				newSize = QSize(w, h);
			}

			break;

		case CT_ScrollBar:
			if (const QStyleOptionSlider *scrollBar = qstyleoption_cast<const QStyleOptionSlider *> (opt)) {
				int scrollBarExtent = pixelMetric(PM_ScrollBarExtent, opt, widget);
				int scrollBarSliderMinimum = pixelMetric(PM_ScrollBarSliderMin, opt, widget);

				if (scrollBar->orientation == Qt::Horizontal) {
					newSize = QSize(16 * 3 + scrollBarSliderMinimum, scrollBarExtent);
				} else {
					newSize = QSize(scrollBarExtent, 16 * 3 + scrollBarSliderMinimum);
				}
			}

			break;

		case CT_LineEdit:
			newSize = sizeFromContents(CT_PushButton, opt, size, widget);
			newSize += QSize(0, 6);
			break;

		case CT_RadioButton:
		case CT_CheckBox:
			if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
				bool isRadio = (type == CT_RadioButton);
				QRect irect = visualRect(btn->direction, btn->rect,
												 subElementRect(isRadio ? SE_RadioButtonIndicator
																	 : SE_CheckBoxIndicator, btn, widget));
// 				int h = pixelMetric(isRadio ? PM_ExclusiveIndicatorHeight
// 										  : PM_IndicatorHeight, btn, widget);

				int margins = 16;
				// we add 4 pixels for label margins

				if (btn->icon.isNull() || !btn->text.isEmpty())
					margins += pixelMetric(isRadio ? PM_RadioButtonLabelSpacing
												  : PM_CheckBoxLabelSpacing, opt, widget);

				newSize += QSize(irect.width() + margins, 10);

			}
			break;

		case CT_SpinBox:
			newSize = sizeFromContents(CT_PushButton, opt, size, widget);
			newSize += QSize(4, -13); // was QSize(4, -5);
			break;

		case CT_TabWidget:
			newSize += QSize(4, 4);
			break;


		case CT_HeaderSection:

			if (const QStyleOptionHeader *hdr = qstyleoption_cast<const QStyleOptionHeader *>(opt)) {
				bool nullIcon = hdr->icon.isNull();
				int margin = pixelMetric(QStyle::PM_HeaderMargin, hdr, widget);
				int iconSize = nullIcon ? 0 : pixelMetric(QStyle::PM_SmallIconSize, hdr, widget);
				QSize txt = hdr->fontMetrics.size(0, hdr->text);
				newSize.setHeight(margin + qMax(iconSize, txt.height()) + margin);
				newSize.setWidth((nullIcon ? 0 : margin) + iconSize
									  + (hdr->text.isNull() ? 0 : margin) + txt.width() + margin);
			}


			break;


		case CT_MenuBar:

// 		case CT_Menu:

		case CT_MenuItem:
			if (const QStyleOptionMenuItem *menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) {
				newSize = QCommonStyle::sizeFromContents(type, opt, size, widget);
				if (menuItem->menuItemType == QStyleOptionMenuItem::Separator)
					newSize.setHeight(5);
				else
					newSize.rwidth() += 20 + 20;
			}
			break;
        
		case CT_Q3Header:

		case CT_Slider:

		case CT_ProgressBar:

		case CT_TabBarTab:

		default:
// 			qDebug("CT not handeled");
			newSize = QCommonStyle::sizeFromContents(type, opt, size, widget);
			break;
	}

	return newSize;
}



void QKanardiaStyle::drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p,
											  const QWidget *widget) const {
	switch (pe) {

		case PE_FrameTabWidget:
//    if (opt->state & State_HasFocus)
			p->fillRect(opt->rect.adjusted(1, 1, -1, -1), opt->palette.button());
			qt_kanardia_draw_frame(p, opt->rect, opt);
			break;

//   case PE_FrameTabBarBase:
//    p->fillRect(opt->rect.adjusted(1, 1, -1, -1), QBrush(QColor("green")));
//        break;

		case PE_PanelLineEdit:

			if (const QStyleOptionFrame *panel = qstyleoption_cast<const QStyleOptionFrame *>(opt)) {
				// Panel of a line edit inside combo box or spin box is drawn in CC_ComboBox and CC_SpinBox
				if (widget) {
					if (qobject_cast<const QComboBox *> (widget->parentWidget()))
						break;

					if (qobject_cast<const QAbstractSpinBox *> (widget->parentWidget()))
						break;
				}

				p->fillRect(opt->rect.adjusted(1, 1, -1, -1), opt->palette.base());

				drawPrimitive(PE_FrameLineEdit, panel, p, widget);
			}

			break;

		case PE_FrameLineEdit:
			qt_kanardia_draw_frame(p, opt->rect, opt);
			break;

		case PE_Widget:
//    qt_kanardia_draw_frame(p, opt->rect, opt);
			break;

		case PE_FrameFocusRect:
//    if ( const QStyleOptionFocusRect *fropt = qstyleoption_cast<const QStyleOptionFocusRect *> ( opt ) ) {
//     qt_kanardia_draw_frame(p, opt->rect, opt);
//    }
			break;

		case PE_IndicatorProgressChunk: {
			bool vertical = false;

			if (const QStyleOptionProgressBarV2 *pb2 = qstyleoption_cast<const QStyleOptionProgressBarV2 *>(opt))
				vertical = (pb2->orientation == Qt::Vertical);

			if (!vertical) {
				p->fillRect(opt->rect.x() + 1, opt->rect.y() + 1, opt->rect.width() - 1, opt->rect.height() - 2,
								opt->palette.highlight().color().darker(130));
			} else {
				p->fillRect(opt->rect.x() + 1, opt->rect.y() + 1, opt->rect.width() - 2, opt->rect.height() - 1,
								opt->palette.highlight().color().darker(130));
			}
		}

		break;

		case PE_IndicatorCheckBox:

			if (opt->state & State_On) {
				// make static pixmap
				static QPixmap check = QPixmap(check_xpm).scaled(
												  opt->rect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
				p->drawPixmap(opt->rect, check);
			} else
				if (opt->state & State_Off) {
					// make static pixmap
					static QPixmap uncheck = QPixmap(uncheck_xpm).scaled(
														 opt->rect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
					p->drawPixmap(opt->rect, uncheck);
				} else {
					p->fillRect(opt->rect, opt->palette.brush(QPalette::Button));
					p->drawRect(opt->rect);
				}

			break;

//-----------------------------------------------------------------------------

//   case PE_FrameButtonTool:

//   case PE_FrameMenu:

		case PE_FrameGroupBox:

			if (const QStyleOptionFrame *frame = qstyleoption_cast<const QStyleOptionFrame *>(opt)) {
				QStyleOptionFrameV2 frameV2(*frame);

				drawPrimitive(PE_Frame, &frameV2, p, widget);

				/*    if (frameV2.features & QStyleOptionFrameV2::Flat) {
				     QPen oldPen = painter->pen();
				     painter->setPen(borderColor);
				     painter->drawLine(frameV2.rect.topLeft(), frameV2.rect.topRight());
				     painter->setPen(oldPen);
				    }
				    else {
				     frameV2.state &= ~(State_Sunken | State_HasFocus);
				     drawPrimitive(PE_Frame, &frameV2, painter, widget);
				    }*/
			}

			break;

		case PE_Frame:
//    p->fillRect(opt->rect.adjusted(1, 1, -1, -1), QBrush(QColor(Qt::yellow)));
//    p->setPen(opt->palette.base().background());
//    p->drawRect(opt->rect.adjusted(1, 1, -1, -1));
			p->fillRect(opt->rect.adjusted(1, 1, -1, -1), QBrush(QColor(0xA2, 0xDC, 0xDE)));
			qt_kanardia_draw_frame(p, opt->rect, opt);
			break;

			/*        if ( const QStyleOptionFrame *frame = qstyleoption_cast<const QStyleOptionFrame *> ( opt ) ) {
			            if ( pe == PE_FrameMenu || ( frame->state & State_Sunken ) || ( frame->state & State_Raised ) ) {
			                qDrawShadePanel ( p, frame->rect, frame->palette, frame->state & State_Sunken,
			                                  frame->lineWidth );
			            } else {*/
			/*     p->fillRect(frame->rect.adjusted(1, 1, -1, -1), opt->palette.light());
			     qt_kanardia_draw_frame(p, opt->rect, opt, QFrame::Sunken);*/
//                 qDrawPlainRect ( p, frame->rect, frame->palette.foreground().color(), frame->lineWidth );
//             }
//         }
			break;

		default:
// 			qDebug("PE Not implemented");
        QCommonStyle::drawPrimitive ( pe, opt, p, widget );
	}
}

//-----------------------------------------------------------------------------

void QKanardiaStyle::drawComplexControl(
	ComplexControl cc,
	const QStyleOptionComplex * opt,
	QPainter * p,
	const QWidget * widget
) const {
	switch (cc) {

		case CC_ScrollBar:
			p->fillRect(opt->rect, opt->palette.background().color());
			qDrawShadePanel(p, opt->rect.adjusted(5, 5, -5, -5), opt->palette, true, 2);

			if (const QStyleOptionSlider *scrollbar = qstyleoption_cast<const QStyleOptionSlider *> (opt)) {
				// Make a copy here and reset it for each primitive.
				QStyleOptionSlider newScrollbar = *scrollbar;
				State saveFlags = scrollbar->state;

				if (scrollbar->minimum == scrollbar->maximum)
					saveFlags |= State_Enabled;

				//    QCommonStyle::drawComplexControl ( cc, opt, p, widget );
				if (scrollbar->subControls & SC_ScrollBarSlider) {
					newScrollbar.rect = scrollbar->rect;
					newScrollbar.state = saveFlags;
					newScrollbar.rect = subControlRect(cc, &newScrollbar, SC_ScrollBarSlider, widget);

					if (newScrollbar.rect.isValid()) {
						drawControl(CE_ScrollBarSlider, &newScrollbar, p, widget);
					}
				}
			}

			break;

		case CC_ComboBox:

			if (const QStyleOptionComboBox *comboBox = qstyleoption_cast<const QStyleOptionComboBox *> (opt)) {
				p->fillRect(opt->rect.adjusted(1, 1, -1, -1), opt->palette.base());
				qt_kanardia_draw_frame(p, opt->rect, opt);
				int h = opt->rect.height();
				QRect rSpin = QRect(opt->rect.right() - h, opt->rect.top(), h, h).adjusted(6, 6, -6, -6);

				p->drawLine(opt->rect.right() - h, opt->rect.top(), opt->rect.right() - h, opt->rect.bottom());

				// make static pixmap
				static QIcon reload_a = QIcon(QPixmap(reload_xpm));

				if (comboBox->state & State_Enabled)
					p->drawPixmap(rSpin, reload_a.pixmap(rSpin.size(), QIcon::Normal));
				else
					p->drawPixmap(rSpin, reload_a.pixmap(rSpin.size(), QIcon::Disabled));

				qt_kanardia_draw_frame(p, opt->rect, opt);
			}
			break;

		case CC_SpinBox:

			if (const QStyleOptionSpinBox *spinbox = qstyleoption_cast<const QStyleOptionSpinBox *> (opt)) {
// 				if (spinbox->state & State_HasFocus)
					p->fillRect(opt->rect.adjusted(1, 1, -1, -1), opt->palette.base());
// 				else
// 					p->fillRect(opt->rect.adjusted(1, 1, -1, -1), opt->palette.window());

				qt_kanardia_draw_frame(p, opt->rect, opt);
			}

			break;

		case CC_GroupBox:

			if (const QStyleOptionGroupBox *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(opt)) {
				// Draw frame
				QRect textRect = subControlRect(CC_GroupBox, opt, SC_GroupBoxLabel, widget);
				QRect checkBoxRect = subControlRect(CC_GroupBox, opt, SC_GroupBoxCheckBox, widget);

				if (groupBox->subControls & QStyle::SC_GroupBoxFrame) {
					QStyleOptionFrameV2 frame;
					frame.QStyleOption::operator=(*groupBox);
					frame.features = groupBox->features;
					frame.lineWidth = groupBox->lineWidth;
					frame.midLineWidth = groupBox->midLineWidth;
					frame.rect = subControlRect(CC_GroupBox, opt, SC_GroupBoxFrame, widget);
					p->save();
					QRegion region(groupBox->rect);

					if (!groupBox->text.isEmpty()) {
						bool ltr = groupBox->direction == Qt::LeftToRight;
						QRect finalRect;

						if (groupBox->subControls & QStyle::SC_GroupBoxCheckBox) {
							finalRect = checkBoxRect.united(textRect);
							finalRect.adjust(ltr ? -4 : 0, 0, ltr ? 0 : 4, 0);
						} else {
							finalRect = textRect;
						}

						region -= finalRect;
					}

					p->setClipRegion(region);

					drawPrimitive(PE_FrameGroupBox, &frame, p, widget);
					p->restore();
				}

				// Draw title
				if ((groupBox->subControls & QStyle::SC_GroupBoxLabel) && !groupBox->text.isEmpty()) {
					QColor textColor = groupBox->textColor;

					if (textColor.isValid())
						p->setPen(textColor);

					int alignment = int(groupBox->textAlignment);

// 					QStyleHintReturn
// 					if (!styleHint(QStyle::SH_UnderlineShortcut, opt, widget))
// 						alignment |= Qt::TextHideMnemonic;

					drawItemText(p, textRect,  Qt::TextShowMnemonic | Qt::AlignHCenter | alignment,
									 groupBox->palette, groupBox->state & State_Enabled, groupBox->text,
									 textColor.isValid() ? QPalette::NoRole : QPalette::WindowText);

					if (groupBox->state & State_HasFocus) {
						QStyleOptionFocusRect fropt;
						fropt.QStyleOption::operator=(*groupBox);
						fropt.rect = textRect;
						drawPrimitive(PE_FrameFocusRect, &fropt, p, widget);
					}
				}

				// Draw checkbox
				if (groupBox->subControls & SC_GroupBoxCheckBox) {
					QStyleOptionButton box;
					box.QStyleOption::operator=(*groupBox);
					box.rect = checkBoxRect;
					drawPrimitive(PE_IndicatorCheckBox, &box, p, widget);
				}
			}
			break;

		case CC_TitleBar:
			break;
		
		default:
			QCommonStyle::drawComplexControl( cc, opt, p, widget );
			break;
	}
}

//-----------------------------------------------------------------------------

QPalette QKanardiaStyle::standardPalette() const {
//	QColor background(0x50, 0xBf, 0xC7);QColor(27,88,255)
	QColor background(QColor(27,88,255).lighter(180));
	QColor light = background.lighter();
	QColor mid = background.darker(150);
	QColor dark("darkslategray");
	QPalette palette(Qt::black, light, light, dark, mid, Qt::black, Qt::white);
// 	QPalette palette(Qt::white, Qt::black, light, dark, mid, Qt::black, Qt::white);
	palette.setColor(QPalette::AlternateBase, light.lighter(132));
	palette.setColor(QPalette::Highlight, QColor("orange").lighter(130));
	palette.setBrush(QPalette::Disabled, QPalette::Highlight, QColor("darkgray"));
	palette.setColor(QPalette::HighlightedText, Qt::black);
	palette.setColor(QPalette::WindowText, Qt::black);
	palette.setColor(QPalette::Window, background);
	palette.setBrush(QPalette::Disabled, QPalette::WindowText, mid);
	palette.setBrush(QPalette::Disabled, QPalette::Text, dark);
	palette.setBrush(QPalette::Disabled, QPalette::ButtonText, dark);
	palette.setBrush(QPalette::Disabled, QPalette::Button, background);
	palette.setBrush(QPalette::Disabled, QPalette::Base, background);
	palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, background);
	palette.setBrush(QPalette::Active, QPalette::Button, QBrush(light));

	// 3D bevel and shadow effects
	palette.setColor(QPalette::Light, Qt::white);
	palette.setColor(QPalette::Midlight, light);
	palette.setColor(QPalette::Dark, dark);
	palette.setColor(QPalette::Mid, mid);
	palette.setColor(QPalette::Shadow, Qt::black);

// 	palette.setBrush(QPalette::Active, QPalette::WindowText, Qt::white);
// 	palette.setBrush(QPalette::Active, QPalette::Text, Qt::white);
// 	palette.setBrush(QPalette::Active, QPalette::ButtonText, dark);
// 	palette.setBrush(QPalette::Active, QPalette::Button, background);
// 	palette.setBrush(QPalette::Active, QPalette::Base, dark);
// 	palette.setBrush(QPalette::Active, QPalette::AlternateBase, background);

	return palette;
}

//-----------------------------------------------------------------------------

void QKanardiaStyle::polish(QPalette& /*pal*/) 
{
}

//-----------------------------------------------------------------------------

int QKanardiaStyle::styleHint(StyleHint hint, const QStyleOption *opt, const QWidget *widget,
										QStyleHintReturn *returnData) const {
	int ret;

	switch (hint) {

		case SH_LineEdit_PasswordCharacter:
			ret = '*';
			break;

		case SH_FocusFrame_AboveWidget:
			ret = false;
			break;

		case SH_BlinkCursorWhenTextSelected:
			ret = false;
			break;

		case SH_ItemView_ChangeHighlightOnFocus:
			ret = true;
			break;

		default:
			ret = QCommonStyle::styleHint(hint, opt, widget, returnData);
			break;
	}

	return ret;
}


/*!
    \internal
*/
QIcon QKanardiaStyle::standardIconImplementation(StandardPixmap standardIcon, const QStyleOption *opt,
		const QWidget *widget) const {
	return QCommonStyle::standardIconImplementation(standardIcon, opt, widget);
}

#endif


