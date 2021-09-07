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

#include "formatdialog.hpp"
#include "ui_formatdialog.h"

FormatDialog::FormatDialog(QWidget *parent)
: QDialog(parent), ui(new Ui::FormatDialog)
{
	ui->setupUi(this);

	QSettings Settings("K-OSP", "Serial-Terminal");

	Settings.beginGroup("Format");
	ui->orderCombo->setCurrentIndex(Settings.value("order", 0).toInt());
	ui->typeCombo->setCurrentIndex(Settings.value("type", 0).toInt());
	ui->worldsSpin->setValue(Settings.value("worlds", 1).toInt());
	Settings.endGroup();

	connect(ui->worldsSpin, qOverload<int>(&QSpinBox::valueChanged),
		   this, &FormatDialog::wordsChanged);
}

FormatDialog::~FormatDialog(void)
{
	delete ui;
}

void FormatDialog::accept(void)
{
	const int type = ui->typeCombo->currentIndex();
	const int worlds = ui->worldsSpin->value();
	const int base = ui->baseSipn->value();
	const int order = ui->orderCombo->currentIndex();

	QSettings Settings("K-OSP", "Serial-Terminal");

	Settings.beginGroup("Format");
	Settings.setValue("order", order);
	Settings.setValue("type", type);
	Settings.setValue("worlds", worlds);
	Settings.endGroup();

	QDialog::accept(); emit onAccepted(type, worlds, base, order);
}

void FormatDialog::wordsChanged(int size)
{
	const bool OK = size == 1 ||
				 size == 2 ||
				 size == 4 ||
				 size == 8;

	ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(OK);
}

