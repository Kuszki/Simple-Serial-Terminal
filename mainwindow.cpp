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

#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* Parent)
: QMainWindow(Parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	Serial = new QSerialPort(this);
	Connect = new ConnectDialog(this);
	About = new AboutDialog(this);

	QSettings Settings("Simple-Serial-Terminal");

	Settings.beginGroup("Window");
	restoreGeometry(Settings.value("geometry").toByteArray());
	restoreState(Settings.value("state").toByteArray());
	Settings.endGroup();

	Settings.beginGroup("Widgets");
	ui->actionAutoclear->setChecked(Settings.value("autoclear", true).toBool());
	Settings.endGroup();

	connect(Connect, &ConnectDialog::onAccepted, this, &MainWindow::configureSerial);

	connect(ui->actionOpen, &QAction::triggered, Connect, &ConnectDialog::open);
	connect(ui->actionClose, &QAction::triggered, this, &MainWindow::closeClicked);
	connect(ui->actionAbout, &QAction::triggered, About, &AboutDialog::open);

	connect(Serial, &QSerialPort::readyRead, this, &MainWindow::readData);
	connect(Serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);

	ui->sendButton->setEnabled(false);
	ui->textEdit->setEnabled(false);

	ui->breakCombo->setItemData(0, QString());
	ui->breakCombo->setItemData(1, QString("\n"));
	ui->breakCombo->setItemData(2, QString("\r"));
}

MainWindow::~MainWindow(void)
{
	QSettings Settings("Simple-Serial-Terminal");

	Settings.beginGroup("Window");
	Settings.setValue("state", saveState());
	Settings.setValue("geometry", saveGeometry());
	Settings.endGroup();

	Settings.beginGroup("Widgets");
	Settings.setValue("autoclear", ui->actionAutoclear->isChecked());
	Settings.endGroup();

	delete ui;
}

void MainWindow::configureSerial(const QString& Port, int Baud, QSerialPort::Parity Parity, QSerialPort::StopBits Stop, QSerialPort::DataBits Size)
{
	if (Serial->isOpen()) Serial->close();

	Serial->setPortName(Port);
	Serial->setBaudRate(Baud);
	Serial->setParity(Parity);
	Serial->setStopBits(Stop);
	Serial->setDataBits(Size);

	if (Serial->open(QSerialPort::ReadWrite))
	{
		ui->sendButton->setEnabled(true);
		ui->textEdit->setEnabled(true);
	}
	else
	{
		ui->sendButton->setEnabled(false);
		ui->textEdit->setEnabled(false);

		errorMessage(Serial->errorString());
	}
}

void MainWindow::errorMessage(const QString& Message)
{
	QMessageBox::critical(this, tr("Error"), Message);
}

void MainWindow::handleError(QSerialPort::SerialPortError Error)
{
	if (Error == QSerialPort::ResourceError)
	{
		errorMessage(Serial->errorString());

		if (Serial->isOpen()) Serial->close();

		ui->sendButton->setEnabled(false);
		ui->textEdit->setEnabled(false);
	}
}

void MainWindow::writeData(void)
{
	if (!Serial->isOpen()) return;

	QString Data = ui->textEdit->text();

	Data.append(ui->breakCombo->currentData().toString());

	if (Data.isEmpty()) return;

	Serial->write(Data.toUtf8());

	if (ui->actionAutoclear->isChecked())
	{
		ui->textEdit->clear();
	}
}

void MainWindow::readData(void)
{
	QString Text = ui->textBrowser->document()->toPlainText();

	Text.append(Serial->readAll());

	ui->textBrowser->document()->setPlainText(Text);
}

void MainWindow::closeClicked(void)
{
	if (Serial->isOpen()) Serial->close();

	ui->sendButton->setEnabled(false);
	ui->textEdit->setEnabled(false);
}
