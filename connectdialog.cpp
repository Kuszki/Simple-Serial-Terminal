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
	ui->setupUi(this); reloadDevices();

	QSettings Settings("K-OSP", "Serial-Terminal");

	Settings.beginGroup("Connection");
	ui->portCombo->setCurrentText(Settings.value("device").toString());
	ui->parityCombo->setCurrentIndex(Settings.value("parity", 0).toInt());
	ui->stopSpin->setValue(Settings.value("stop", 1).toInt());
	ui->sizeSpin->setValue(Settings.value("data", 8).toInt());
	ui->baudSpin->setValue(Settings.value("baud", 9600).toInt());
	Settings.endGroup();
}

ConnectDialog::~ConnectDialog(void)
{
	delete ui;
}

void ConnectDialog::reloadDevices(const QString& last)
{
	ui->portCombo->clear();

	for (const auto& Info : QSerialPortInfo::availablePorts())
	{
		ui->portCombo->addItem(Info.portName());
	}

	ui->portCombo->setCurrentText(last);
}

void ConnectDialog::open(void)
{
	const QString Last = ui->portCombo->currentText();

	reloadDevices(Last); QDialog::open();
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

	const QString port = ui->portCombo->currentText();
	const int baud = ui->baudSpin->value();

	const int pari = ui->parityCombo->currentIndex();
	const int stpv = ui->stopSpin->value();
	const int sizv = ui->sizeSpin->value();

	const auto par = Parity.value(pari, QSerialPort::UnknownParity);
	const auto stp = Stop.value(stpv, QSerialPort::UnknownStopBits);
	const auto siz = Size.value(sizv, QSerialPort::UnknownDataBits);

	QSettings Settings("K-OSP", "Serial-Terminal");

	Settings.beginGroup("Connection");
	Settings.setValue("device", port);
	Settings.setValue("parity", pari);
	Settings.setValue("stop", stpv);
	Settings.setValue("data", sizv);
	Settings.setValue("baud", baud);
	Settings.endGroup();

	QDialog::accept(); emit onAccepted(port, baud, par, stp, siz);
}
