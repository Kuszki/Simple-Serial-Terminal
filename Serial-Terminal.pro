# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#                                                                         *
#  Simple Serial terminal                                                 *
#  Copyright (C) 2015  Łukasz "Kuszki" Dróżdż  l.drozdz@openmailbox.org   *
#                                                                         *
#  This program is free software: you can redistribute it and/or modify   *
#  it under the terms of the GNU General Public License as published by   *
#  the  Free Software Foundation, either  version 3 of the  License, or   *
#  (at your option) any later version.                                    *
#                                                                         *
#  This  program  is  distributed  in the hope  that it will be useful,   *
#  but WITHOUT ANY  WARRANTY;  without  even  the  implied  warranty of   *
#  MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the   *
#  GNU General Public License for more details.                           *
#                                                                         *
#  You should have  received a copy  of the  GNU General Public License   *
#  along with this program. If not, see http://www.gnu.org/licenses/.     *
#                                                                         *
# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

QT		+=	core gui widgets serialport charts

TARGET	=	Serial-Terminal
TEMPLATE	=	app

CONFIG	+=	c++19

SOURCES	+=	main.cpp \
			chartobject.cpp \
			chartview.cpp \
			formatdialog.cpp \
			mainwindow.cpp \
			aboutdialog.cpp \
			connectdialog.cpp

HEADERS	+=	mainwindow.hpp \
			aboutdialog.hpp \
			chartobject.hpp \
			chartview.hpp \
			connectdialog.hpp \
			formatdialog.hpp

FORMS	+=	mainwindow.ui \
			aboutdialog.ui \
			connectdialog.ui \
			formatdialog.ui

RESOURCES	+=	resources.qrc

TRANSLATIONS	+=	serial_terminal_pl.ts

QMAKE_CXXFLAGS	+=	-s -march=native
