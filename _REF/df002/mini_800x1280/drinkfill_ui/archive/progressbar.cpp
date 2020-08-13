//***************************************
//
// mainwindow.cpp
//
// model of custom progress bar which inherits from the default progress bar
// base class
//
// created: 01-04-2019
// by: Phur Tenzin Sherpa
//
// copyright 2019 by drinkfill
// all rights reserved




#include "progressbar.h"
#include <QPainter>

class QColor;

ProgressBar::ProgressBar(QWidget *parent) : QWidget(parent)
{

}
void ProgressBar::paintEvent( QPaintEvent* )
{
		 QPainter p( this );
		 p.setPen( Qt::NoPen );

		 qreal blockWidth = rect().width() / 10;
		 qreal blockSpacing = blockWidth / 10;
		 blockWidth -= blockSpacing;

		 qreal blockHeight = rect().height();

		 qreal percentageValue = (2- 0) / (473 - 0);

		 for( int i = 0; i < 20; ++i )
		 {
					//Qt::Color color = percentageValue >= (i+1)*10 ? Qt::green : Qt::white;
					p.setBrush( Qt::color0 );
					int x = i * (blockWidth + blockSpacing);
					int y = 0;
					p.drawPolygon(QPolygon(20));
					//p.drawRect( QRect(x,i++,blockWidth,blockHeight) );
		 }
}

