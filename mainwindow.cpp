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
#include "qtimer.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* Parent)
: QMainWindow(Parent)
, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	QSettings Settings("K-OSP", "Serial-Terminal");

	QActionGroup* group = new QActionGroup(this);
	group->addAction(ui->actionRawmode);
	group->addAction(ui->actionPlotmode);
	group->addAction(ui->actionTextmode);
	group->setExclusive(true);

	Serial = new QSerialPort(this);
	Connect = new ConnectDialog(this);
	About = new AboutDialog(this);

	Updater.setSingleShot(true);
	Updater.setInterval(250);

	Settings.beginGroup("Chart");
	Chart = new ChartObject(Settings.value("spline", false).toBool());
	Chart->setBackgroundBrush(palette().brush(QPalette::Window));
	Chart->setLabelsBrush(palette().brush(QPalette::Text));
	Chart->setPalette(palette());
	Settings.endGroup();

	textBrowser = new QTextBrowser(this);
	chartView = new ChartView(Chart, this);
	scaleSpin = new QDoubleSpinBox(this);

	scaleSpin->setPrefix(tr("Scale "));
	scaleSpin->setMinimum(0.0);
	scaleSpin->setMaximum(10000);
	scaleSpin->setDecimals(10);

	chartView->setRenderHint(QPainter::Antialiasing);
	chartView->setDragMode(QGraphicsView::RubberBandDrag);
	chartView->setInteractive(true);
	chartView->setVisible(false);

	ui->horizontalLayout->addWidget(textBrowser);
	ui->horizontalLayout->addWidget(chartView);

	ui->mainTool->addSeparator();
	ui->mainTool->addWidget(scaleSpin);

	Settings.beginGroup("Eval");
	scaleSpin->setValue(Settings.value("scale", 1.0).toDouble());
	Settings.endGroup();

	Chart->setScale(scaleSpin->value());

	Settings.beginGroup("Window");
	restoreGeometry(Settings.value("geometry").toByteArray());
	restoreState(Settings.value("state").toByteArray());
	Settings.endGroup();

	Settings.beginGroup("Widgets");
	ui->actionAutoclearedit->setChecked(Settings.value("autoclear", true).toBool());
	ui->actionAutoclearview->setChecked(Settings.value("cutview", false).toBool());
	ui->actionAutoscroll->setChecked(Settings.value("autoscroll", true).toBool());
	ui->actionRawmode->setChecked(Settings.value("rawmode", false).toBool());
	ui->actionPlotmode->setChecked(Settings.value("plotmode", false).toBool());
	ui->actionTextmode->setChecked(Settings.value("textmode", true).toBool());
	Settings.endGroup();

	Settings.beginGroup("Format");
	msbf = Settings.value("order", 0).toInt();
	wtype = Settings.value("type", 0).toInt();
	words = Settings.value("worlds", 1).toInt();
	Settings.endGroup();

	Settings.beginGroup("Display");
	timer = Settings.value("timer", false).toBool();
	Settings.endGroup();

	ui->sendButton->setEnabled(false);
	ui->textEdit->setEnabled(false);

	ui->breakCombo->setItemData(0, QString());
	ui->breakCombo->setItemData(1, QString("\n"));
	ui->breakCombo->setItemData(2, QString("\r"));

	connect(ui->actionOpen, &QAction::triggered, Connect, &ConnectDialog::open);
	connect(ui->actionAbout, &QAction::triggered, About, &AboutDialog::open);

	connect(Connect, &ConnectDialog::onAccepted, this, &MainWindow::configureSerial);

	connect(Serial, &QSerialPort::readyRead, this, &MainWindow::readData);
	connect(Serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);

	connect(Chart, &ChartObject::onValueMouseover, this, &MainWindow::updateTooltip);

	connect(chartView, &QChartView::rubberBandChanged, Chart, &ChartObject::bandChanged);
	connect(chartView, &ChartView::onValueMouseover, this, &MainWindow::updateTooltip);

	connect(group, &QActionGroup::triggered, this, &MainWindow::switchMode);

	connect(ui->actionZoomfit, &QAction::triggered, Chart,
	[this] (void) -> void
	{
		Chart->zoomReset();
		Chart->format();
	});

	connect(scaleSpin, &QDoubleSpinBox::editingFinished, this,
	[this] (void) -> void
	{
		Chart->setScale(scaleSpin->value());
	});

	connect(ui->actionZoomin, &QAction::triggered, Chart,
	[this] (void) -> void
	{
		Chart->zoom(1.1);
	});

	connect(ui->actionZoomout, &QAction::triggered, Chart,
	[this] (void) -> void
	{
		Chart->zoom(0.9);
	});

	connect(&Updater, &QTimer::timeout,
	[this] (void) -> void
	{
		chartView->setUpdatesEnabled(true);
		textBrowser->setUpdatesEnabled(true);
	});

	switchMode();
}

MainWindow::~MainWindow(void)
{
	QSettings Settings("K-OSP", "Serial-Terminal");

	Settings.beginGroup("Window");
	Settings.setValue("state", saveState());
	Settings.setValue("geometry", saveGeometry());
	Settings.endGroup();

	Settings.beginGroup("Widgets");
	Settings.setValue("autoclear", ui->actionAutoclearedit->isChecked());
	Settings.setValue("cutview", ui->actionAutoclearview->isChecked());
	Settings.setValue("autoscroll", ui->actionAutoscroll->isChecked());
	Settings.setValue("rawmode", ui->actionRawmode->isChecked());
	Settings.setValue("plotmode", ui->actionPlotmode->isChecked());
	Settings.setValue("textmode", ui->actionTextmode->isChecked());
	Settings.endGroup();

	Settings.beginGroup("Eval");
	Settings.setValue("scale", scaleSpin->value());
	Settings.endGroup();

	Settings.beginGroup("Display");
	Settings.setValue("timer", timer);
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

void MainWindow::updateTooltip(const QPointF& point)
{
	ui->statusBar->showMessage(QString("x = %1, y = %2")
	                           .arg(point.x())
	                           .arg(point.y()));
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
	if (timer)
	{
		chartView->setUpdatesEnabled(false);
		textBrowser->setUpdatesEnabled(false);
	}

	if (ui->actionTextmode->isChecked())
	{
		textBrowser->append(QString::fromLocal8Bit(data));

		if (ui->actionAutoscroll->isChecked()) scrollDown();
	}
	else
	{
		std::function<QStringList (const void*, size_t, int, bool)> fnConvert;
		std::function<QVector<double> (const void*, size_t, bool)> fnCast;

		const int count = Rawdata.size();
		const int rest = count % words;
		QByteArray newdata = data;

		for (int i = 0; i < rest; ++i)
		{
			newdata.push_front(Rawdata[count - 1 - i]);
		}

		if (ui->actionRawmode->isChecked()) switch (wtype)
		{
			case 0: switch (words)
				{
					case 8: fnConvert = convertAs<int64_t>; break;
					case 4: fnConvert = convertAs<int32_t>; break;
					case 2: fnConvert = convertAs<int16_t>; break;
					case 1: fnConvert = convertAs<int8_t>; break;
				}
			break;
			case 1: switch (words)
				{
					case 8: fnConvert = convertAs<uint64_t>; break;
					case 4: fnConvert = convertAs<uint32_t>; break;
					case 2: fnConvert = convertAs<uint16_t>; break;
					case 1: fnConvert = convertAs<uint8_t>; break;
				}
			break;
		}
		else switch (wtype)
		{
			case 0: switch (words)
				{
					case 8: fnCast = castAs<int64_t>; break;
					case 4: fnCast = castAs<int32_t>; break;
					case 2: fnCast = castAs<int16_t>; break;
					case 1: fnCast = castAs<int8_t>; break;
				}
			break;
			case 1: switch (words)
				{
					case 8: fnCast = castAs<uint64_t>; break;
					case 4: fnCast = castAs<uint32_t>; break;
					case 2: fnCast = castAs<uint16_t>; break;
					case 1: fnCast = castAs<uint8_t>; break;
				}
			break;
		}

		if (newdata.size() / words > 0)
		{
			if (ui->actionRawmode->isChecked())
			{
				textBrowser->append(fnConvert(newdata.data(),
				                              newdata.length(),
				                              10, msbf)
				                    .join('\n'));

				if (ui->actionAutoscroll->isChecked()) scrollDown();
			}
			else
			{
				for (const auto& d : fnCast(newdata.data(),
				                            newdata.length(),
				                            msbf))
				{
					Chart->appendData(d);
				}

				Chart->format();
			}
		}
	}

	if (timer && !Updater.isActive()) Updater.start();

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

		clearData();
		appendData(data);
	}
}

void MainWindow::switchMode(void)
{
	textBrowser->setVisible(ui->actionRawmode->isChecked() ||
	                        ui->actionTextmode->isChecked());

	chartView->setVisible(ui->actionPlotmode->isChecked());

	const auto data = Rawdata;

	clearData();
	appendData(data);
}

void MainWindow::writeData(void)
{
	if (!Serial->isOpen()) return;

	QString Data = ui->textEdit->text();

	Data.append(ui->breakCombo->currentData().toString());

	if (Data.isEmpty()) return;

	if (ui->actionAutoclearview->isChecked())
	{
		clearData();
	}

	Serial->write(Data.toUtf8());

	if (ui->actionAutoclearedit->isChecked())
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
	textBrowser->clear();
	Chart->clear();
	Rawdata.clear();
}

void MainWindow::saveData(void)
{
	const QString path = QFileDialog::getSaveFileName(this,
	                                                  tr("Save data"), QString(), tr("Text files (*.txt);;All files (*.*)"));

	if (path.isEmpty()) return;

	QFile file(path); QTextStream stream(&file);

	if (!file.open(QFile::WriteOnly | QFile::Text)) return;
	else
	{
		if (!ui->actionPlotmode->isChecked()) stream << textBrowser->toPlainText();
		else for (const auto& v : Chart->getValues()) stream << v << Qt::endl;
	}
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

void MainWindow::statsClicked(void)
{
	const auto vs = Chart->getValues();

	if (vs.isEmpty()) return;

	const double mul = 1.0 / vs.size();
	double
	          max = vs.first(),
	          min = vs.first(),
	          mean = 0.0,
	          var = 0.0;

	for (const auto& v : vs)
	{
		if (max < v) max = v;
		if (min > v) min = v;

		mean = mean + mul * v;
	}

	for (const auto& v : vs)
	{
		const double pw = v - mean;
		var = var + mul * pw * pw;
	}

	const double stdev = qSqrt(var);

	ui->statusBar->showMessage(tr("min = %1; max = %2; mean = %3; stdev = %4; var = %5")
	                           .arg(min).arg(max).arg(mean).arg(stdev).arg(var));
}

void MainWindow::scrollDown(void)
{
	auto Scrool = textBrowser->verticalScrollBar();

	Scrool->setValue(Scrool->maximum());
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

template<typename Data>
QVector<double> MainWindow::castAs(const void* ptr, size_t len, bool reverse)
{
	const Data* data = static_cast<const Data*>(ptr);
	const size_t size = len / sizeof(Data);

	QVector<double> list;
	list.reserve(len);

	for (size_t i = 0; i < size; ++i)
	{
		list.append(reverse ?
		                 reverseEn(data[i]) :
		                 data[i]);
	}

	return list;
}
