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

ChartObject::ChartObject(bool spline, QGraphicsItem* parent, Qt::WindowFlags wFlags)
	: QChart(QChart::ChartTypeCartesian, parent, wFlags)
	, xAxis(new QValueAxis())
	, yAxis(new QValueAxis())
{
	if (!spline) pSeries = new QLineSeries(this);
	else pSeries = new QSplineSeries(this);

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

	xAxis->setLabelFormat("%d");

	legend()->setVisible(false);

	connect(pSeries, &QLineSeries::clicked,
		   this, &ChartObject::onValueMouseover);
}

ChartObject::~ChartObject(void) {}

void ChartObject::setLabelsBrush(const QBrush& brush)
{
	xAxis->setLabelsBrush(brush);
	yAxis->setLabelsBrush(brush);
}

QVector<double> ChartObject::getValues(void) const
{
	const auto points = pSeries->points();

	QVector<double> values;
	values.reserve(points.size());

	for (const auto& v : points)
	{
		values.append(v.y());
	}

	return values;
}

void ChartObject::appendData(double value)
{
	value = value * scale;

	pSeries->append(step += 1.0, value);

	if (qIsNaN(ymin)) ymin = value;
	else ymin = qMin(ymin, value);

	if (qIsNaN(ymax)) ymax = value;
	else ymax = qMax(ymax, value);
}

void ChartObject::format(void)
{
	const auto span = 0.05*(ymax - ymin);

	xAxis->setRange(1, step);
	yAxis->setRange(ymin - span,
				 ymax + span);
}

void ChartObject::clear(void)
{
	pSeries->clear();

	step = 0.0;
	ymin = NAN;
	ymax = NAN;
}

double ChartObject::getScale(void) const
{
	return scale;
}

void ChartObject::bandChanged(const QRect& rubberBandRect, const QPointF& fromScenePoint, const QPointF& toScenePoint)
{
	if (rubberBandRect.isNull() &&
	    fromScenePoint.isNull() &&
	    toScenePoint.isNull())
	{
		zoomIn({fromPoint , toPoint});
	}
	else
	{
		rubberRect = rubberBandRect;
		fromPoint = fromScenePoint;
		toPoint = toScenePoint;
	}
}

void ChartObject::setScale(double newScale)
{
	if (qAbs(newScale - scale) < 1E-10) return;

	const double mul = newScale / scale;
	auto list = pSeries->points();
	ymin = ymax = NAN;

	for (auto& p : list)
	{
		p.ry() = p.y() * mul;

		if (qIsNaN(ymin)) ymin = p.y();
		else ymin = qMin(ymin, p.y());

		if (qIsNaN(ymax)) ymax = p.y();
		else ymax = qMax(ymax, p.y());
	}

	pSeries->replace(list);

	scale = newScale;
}
