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

#ifndef CHARTOBJECT_HPP
#define CHARTOBJECT_HPP

#include <QtCharts>

class ChartObject : public QChart
{

		Q_OBJECT

	private:

		QLineSeries* pSeries;
		QValueAxis* xAxis;
		QValueAxis* yAxis;

		QRect rubberRect;
		QPointF fromPoint;
		QPointF toPoint;

		double scale = 0.0;
		double step = 0.0;

		double ymin = NAN;
		double ymax = NAN;

	public:

		explicit ChartObject(bool spline = true,
				QGraphicsItem *parent = nullptr,
				Qt::WindowFlags wFlags = {});
		virtual ~ChartObject(void) override;

		void setLabelsBrush(const QBrush& brush);

		QVector<double> getValues(void) const;

		double getScale(void) const;

	public slots:

		void bandChanged(const QRect& rubberBandRect,
					  const QPointF& fromScenePoint,
					  const QPointF& toScenePoint);

		void appendData(double value);
		void setScale(double newScale);

		void format(void);
		void clear(void);

	signals:

		void onValueMouseover(const QPointF&);

};

#endif // CHARTOBJECT_HPP
