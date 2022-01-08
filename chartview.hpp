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

#ifndef CHARTVIEW_HPP
#define CHARTVIEW_HPP

#include <QtWidgets>
#include <QtCharts>
#include <QtCore>
#include <QtGui>

class ChartView : public QChartView
{

		Q_OBJECT

	private:

		QPointF m_lastMousePos;

	public:

		explicit ChartView(QChart *chart, QWidget *parent = nullptr);
		virtual ~ChartView(void) override;

	protected:

		virtual void mousePressEvent(QMouseEvent *event) override;
		virtual void mouseMoveEvent(QMouseEvent *event) override;
		virtual void wheelEvent(QWheelEvent *event) override;

	signals:

		void onValueMouseover(const QPointF&);

};

#endif // CHARTVIEW_HPP
