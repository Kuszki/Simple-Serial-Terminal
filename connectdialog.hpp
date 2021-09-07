/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Simple Serial terminal                                                 *
 *  Copyright (C) 2018  Łukasz "Kuszki" Dróżdż  l.drozdz@openmailbox.org   *
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

#ifndef CONNECTDIALOG_HPP
#define CONNECTDIALOG_HPP

#include <QSerialPortInfo>
#include <QSerialPort>
#include <QSettings>
#include <QDialog>

namespace Ui
{
	class ConnectDialog;
}

class ConnectDialog : public QDialog
{

		Q_OBJECT

	private:

		Ui::ConnectDialog* ui;

	public:

		explicit ConnectDialog(QWidget* Parent = nullptr);
		virtual ~ConnectDialog(void) override;

	protected:

		void reloadDevices(const QString& last = QString());

	public slots:

		virtual void open(void) override;

		virtual void accept(void) override;

	signals:

		void onAccepted(const QString&, int,
					 QSerialPort::Parity,
					 QSerialPort::StopBits,
					 QSerialPort::DataBits);

};

#endif // CONNECTDIALOG_HPP
