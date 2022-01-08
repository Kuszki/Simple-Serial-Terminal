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

#include "chartview.hpp"

ChartView::ChartView(QChart* chart, QWidget* parent)
	: QChartView(chart, parent)
{
	setMouseTracking(true);
}

ChartView::~ChartView(void) {}

void ChartView::mousePressEvent(QMouseEvent* event)
{
	if (event->buttons() == Qt::MiddleButton)
	{
		m_lastMousePos = event->pos();
		event->accept();
	}
	else if (event->buttons() == Qt::RightButton)
	{
		emit onValueMouseover(chart()->mapToValue(event->pos()));
	}
	else QChartView::mousePressEvent(event);
}

void ChartView::mouseMoveEvent(QMouseEvent* event)
{
	if (event->buttons() == Qt::MiddleButton)
	{
		auto dPos = event->pos() - m_lastMousePos;
		chart()->scroll(-dPos.x(), dPos.y());

		m_lastMousePos = event->pos();
		event->accept();

		QApplication::restoreOverrideCursor();
	}
	else if (event->buttons() == Qt::RightButton)
	{
		emit onValueMouseover(chart()->mapToValue(event->pos()));
	}

	QChartView::mouseMoveEvent(event);
}

void ChartView::wheelEvent(QWheelEvent* event)
{
	if (event->angleDelta().y() > 0) chart()->zoom(1.1);
	if (event->angleDelta().y() < 0) chart()->zoom(0.9);
}

