/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  {description}                                                          *
 *  Copyright (C) 2020  Łukasz "Kuszki" Dróżdż  lukasz.kuszki@gmail.com    *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the  Free Software Foundation, either  version 3 of the  License, or   *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This  program  is  distributed  in the hope  that it will be useful,   *
 *  but WITHOUT ANY  WARRANTY;  without  even  the  implied  warranty of   *
 *  MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the   *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have  received a copy  of the  GNU General Public License   *
 *  along with this program. If not, see http://www.gnu.org/licenses/.     *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "chartobject.hpp"

ChartObject::ChartObject(QGraphicsItem* parent, Qt::WindowFlags wFlags)
	: QChart(QChart::ChartTypeCartesian, parent, wFlags)
	, xAxis(new QValueAxis())
	, yAxis(new QValueAxis())
{
	pSeries = new QSplineSeries(this);

	QPen green(Qt::red);
	green.setWidth(3);

	pSeries->setUseOpenGL(true);
	pSeries->setPen(green);

	addSeries(pSeries);

	addAxis(xAxis, Qt::AlignBottom);
	addAxis(yAxis, Qt::AlignLeft);

	pSeries->attachAxis(xAxis);
	pSeries->attachAxis(yAxis);

	xAxis->setRange(0, 1);
	yAxis->setRange(0, 1);

	legend()->setVisible(false);
}

ChartObject::~ChartObject(void) {}

void ChartObject::setLabelsBrush(const QBrush& brush)
{
	xAxis->setLabelsBrush(brush);
	yAxis->setLabelsBrush(brush);
}

void ChartObject::appendData(double value)
{
	pSeries->append(step += 1.0, value);

	if (qIsNaN(ymin)) ymin = value;
	else ymin = qMin(ymin, value);

	if (qIsNaN(ymax)) ymax = value;
	else ymax = qMax(ymax, value);
}

void ChartObject::format(void)
{
	xAxis->setRange(1, step);
	yAxis->setRange(ymin - 0.1*qAbs(ymin),
				 ymax + 0.1*qAbs(ymax));
}

void ChartObject::clear(void)
{
	pSeries->clear();

	step = 0.0;
	ymin = NAN;
	ymax = NAN;
}
