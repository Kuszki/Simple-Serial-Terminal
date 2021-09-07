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

#ifndef FORMATDIALOG_HPP
#define FORMATDIALOG_HPP

#include <QtWidgets>
#include <QtCore>
#include <QtGui>

namespace Ui
{
	class FormatDialog;
}

class FormatDialog : public QDialog
{

		Q_OBJECT

	private:

		Ui::FormatDialog *ui;

	public:

		explicit FormatDialog(QWidget *parent = nullptr);
		virtual ~FormatDialog(void) override;

	public slots:

		virtual void accept(void) override;

	private slots:

		void wordsChanged(int size);

	signals:

		void onAccepted(int, int, int, bool);

};

#endif // FORMATDIALOG_HPP
