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

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QSerialPort>
#include <QMainWindow>
#include <QByteArray>
#include <QScrollBar>
#include <QSettings>

#include "connectdialog.hpp"
#include "formatdialog.hpp"
#include "aboutdialog.hpp"

namespace Ui
{
	class MainWindow;
}

class MainWindow : public QMainWindow
{

		Q_OBJECT

	private:

		QByteArray Rawdata;

		QSerialPort* Serial;

		ConnectDialog* Connect;
		AboutDialog* About;

		Ui::MainWindow* ui;

		int words = 1;
		int wtype = 0;
		int wbase = 10;
		bool msbf = 0;

	public:

		explicit MainWindow(QWidget* Parent = nullptr);
		virtual ~MainWindow(void) override;

	private:

		template <typename Data>
		static QStringList convertAs(const void* ptr,
							    size_t len,
							    int base = 10,
							    bool reverse = false);

		template <typename Data>
		static Data reverseEn(const Data& data);

	private slots:

		void configureSerial(const QString& Port, int Baud,
						 QSerialPort::Parity Parity,
						 QSerialPort::StopBits Stop,
						 QSerialPort::DataBits Size);

		void errorMessage(const QString& Message);
		void handleError(QSerialPort::SerialPortError Error);

		void appendData(const QByteArray& data);

		void switchFormat(int Type, int Words, int Base, bool Order);
		void switchMode(bool mode);

		void writeData(void);
		void readData(void);
		void clearData(void);
		void saveData(void);
		void formatData(void);

		void closeClicked(void);

		void scrollDown(void);

};

#endif // MAINWINDOW_HPP
