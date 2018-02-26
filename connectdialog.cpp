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

#include "connectdialog.hpp"
#include "ui_connectdialog.h"

ConnectDialog::ConnectDialog(QWidget* Parent)
: QDialog(Parent), ui(new Ui::ConnectDialog)
{
	ui->setupUi(this);
}

ConnectDialog::~ConnectDialog(void)
{
	delete ui;
}

void ConnectDialog::open(void)
{
	const QString Last = ui->portCombo->currentText();

	ui->portCombo->clear();

	for (const auto& Info : QSerialPortInfo::availablePorts())
	{
		ui->portCombo->addItem(Info.portName());
	}

	ui->portCombo->setCurrentText(Last);

	QDialog::open();
}

void ConnectDialog::accept(void)
{
	static const QMap<int, QSerialPort::Parity> Parity =
	{
		{ 0, QSerialPort::NoParity },
		{ 1, QSerialPort::EvenParity },
		{ 2, QSerialPort::OddParity }
	};

	static const QMap<int, QSerialPort::StopBits> Stop =
	{
		{ 1, QSerialPort::OneStop },
		{ 2, QSerialPort::TwoStop }
	};

	static const QMap<int, QSerialPort::DataBits> Size =
	{
		{ 5, QSerialPort::Data5 },
		{ 6, QSerialPort::Data6 },
		{ 7, QSerialPort::Data7 },
		{ 8, QSerialPort::Data8 }
	};

	QDialog::accept();

	emit onAccepted(
			ui->portCombo->currentText(), ui->baudSpin->value(),
			Parity.value(ui->parityCombo->currentIndex(), QSerialPort::UnknownParity),
			Stop.value(ui->stopSpin->value(), QSerialPort::UnknownStopBits),
			Size.value(ui->sizeSpin->value(), QSerialPort::UnknownDataBits));
}
