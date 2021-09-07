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

	QSettings Settings("K-OSP", "Serial-Terminal");

	Settings.beginGroup("Window");
	restoreGeometry(Settings.value("geometry").toByteArray());
	restoreState(Settings.value("state").toByteArray());
	Settings.endGroup();

	Settings.beginGroup("Widgets");
	ui->actionAutoclear->setChecked(Settings.value("autoclear", true).toBool());
	ui->actionAutoscroll->setChecked(Settings.value("autoscroll", true).toBool());
	ui->actionTextmode->setChecked(Settings.value("textmode", true).toBool());
	Settings.endGroup();

	Settings.beginGroup("Format");
	msbf = Settings.value("order", 0).toInt();
	wtype = Settings.value("type", 0).toInt();
	words = Settings.value("worlds", 1).toInt();
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
	QSettings Settings("K-OSP", "Serial-Terminal");

	Settings.beginGroup("Window");
	Settings.setValue("state", saveState());
	Settings.setValue("geometry", saveGeometry());
	Settings.endGroup();

	Settings.beginGroup("Widgets");
	Settings.setValue("autoclear", ui->actionAutoclear->isChecked());
	Settings.setValue("autoscroll", ui->actionAutoscroll->isChecked());
	Settings.setValue("textmode", ui->actionTextmode->isChecked());
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

void MainWindow::appendData(const QByteArray& data)
{
	QStringList lines;

	if (ui->actionTextmode->isChecked())
	{
		ui->textBrowser->append(QString::fromLocal8Bit(data));
	}
	else
	{
		std::function<QStringList (const void*, size_t, int, bool)> fn;

		const int count = Rawdata.size();
		const int rest = count % words;
		QByteArray newdata = data;

		for (int i = 0; i < rest; ++i)
		{
			newdata.push_front(Rawdata[count - 1 - i]);
		}

		switch (wtype)
		{
			case 0: switch (words)
				{
					case 8: fn = convertAs<int64_t>; break;
					case 4: fn = convertAs<int32_t>; break;
					case 2: fn = convertAs<int16_t>; break;
					case 1: fn = convertAs<int8_t>; break;
				}
			break;
			case 1: switch (words)
				{
					case 8: fn = convertAs<uint64_t>; break;
					case 4: fn = convertAs<uint32_t>; break;
					case 2: fn = convertAs<uint16_t>; break;
					case 1: fn = convertAs<uint8_t>; break;
				}
			break;
		}

		if (newdata.size() / words > 0)
			ui->textBrowser->append(fn(newdata.data(),
								  newdata.length(),
								  10, msbf)
							    .join('\n'));
	}

	if (!lines.isEmpty())
	{
		ui->textBrowser->append(lines.join(' '));
		ui->textBrowser->append("\n");
	}

	Rawdata.append(data);
}

void MainWindow::switchFormat(int Type, int Words, int Base, bool Order)
{
	if (!(wtype != Type || words != Words ||
		 wbase != Base || msbf != Order)) return;

	wtype = Type;
	words = Words;
	wbase = Base;
	msbf = Order;

	if (!ui->actionTextmode->isChecked())
	{
		const auto data = Rawdata;

		ui->textBrowser->clear();
		Rawdata.clear();

		appendData(data);
	}
}

void MainWindow::switchMode(bool mode)
{
	if (sender() != ui->actionTextmode)
		if (mode == ui->actionTextmode->isChecked())
			return;

	const auto data = Rawdata;

	ui->textBrowser->clear();
	Rawdata.clear();

	appendData(data);
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
	appendData(Serial->readAll());
	scrollDown();
}

void MainWindow::clearData(void)
{
	Rawdata.clear();
}

void MainWindow::saveData(void)
{
	const QString path = QFileDialog::getSaveFileName(this,
		tr("Save data"), QString(), tr("Text files (*.txt);;All files (*.*)"));

	if (path.isEmpty()) return;

	QFile file(path); QTextStream stream(&file);

	if (!file.open(QFile::WriteOnly | QFile::Text)) return;
	else stream << ui->textBrowser->toPlainText();
}

void MainWindow::formatData(void)
{
	FormatDialog* dialog = new FormatDialog(this); dialog->open();

	connect(dialog, &FormatDialog::onAccepted, this, &MainWindow::switchFormat);
	connect(dialog, &FormatDialog::finished, dialog, &FormatDialog::deleteLater);
}

void MainWindow::closeClicked(void)
{
	if (Serial->isOpen()) Serial->close();

	ui->sendButton->setEnabled(false);
	ui->textEdit->setEnabled(false);
}

void MainWindow::scrollDown(void)
{
	auto Scrool = ui->textBrowser->verticalScrollBar();

	if (ui->actionAutoscroll->isChecked())
	{
		Scrool->setValue(Scrool->maximum());
	}
}

template<typename Data>
QStringList MainWindow::convertAs(const void* ptr, size_t len, int base, bool reverse)
{
	const Data* data = static_cast<const Data*>(ptr);
	const size_t size = len / sizeof(Data);

	QStringList list;

	for (size_t i = 0; i < size; ++i)
	{
		list.append(QString::number(
					  reverse ?
						  reverseEn(data[i]) :
						  data[i],
					  base));
	}

	return list;
}

template<typename Data>
Data MainWindow::reverseEn(const Data& data)
{
	const size_t size = sizeof(Data);
	int j = size - 1;

	Data mask = 0xFF, res = 0;

	for (size_t i = j+1; i != 0; --i)
	{
		if (j >= 0)
		{
			res |= (data & mask) << (j*8);
		}
		else
		{
			res |= (data & mask) >> (-j*8);
		}


		mask = mask << 8;
		j = j - 2;
	}

	return res;
}
