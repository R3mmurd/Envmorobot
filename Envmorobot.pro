#  This file is part of Ropab System.
#  Copyright (C) 2012 by Alejandro J. Mujica

#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.

#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.

#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#  Any user request of this software, write to

#  Alejandro Mujica

#  aledrums@gmail.com

# Adjust this path to your Aleph-w instalation
ALEPH = ../../../Aleph-w

QMAKE_CXX = clang++

QMAKE_CXXFLAGS_DEBUG += -O0 -g -DDEBUG

QMAKE_CXXFLAGS_RELEASE += -Ofast -DNDEBUG

CONFIG += c++14 warn_off

QT += widgets core

INCLUDEPATH += $${ALEPH}

LIBS += \
    -L$${ALEPH} \
    -lAleph \
    -lasprintf \
    -lgmp \
    -lmpfr \
    -lgsl \
    -lgslcblas

HEADERS += \
    obstacle.H \
    utils.H \
    buffer.H \
    enviroment.H \
    geometricmap.H \
    mappanel.H \
    mapframe.H \
    discretizewindow.H \
    infowindow.H \
    onefieldwindow.H

SOURCES += \
    main.C \
    obstacle.C \
    buffer.C \ 
    enviroment.C \
    geometricmap.C \
    discretizewindow.C \
    infowindow.C \
    mapframe.C \
    mappanel.C \
    onefieldwindow.C

OTHER_FILES += \
    debug.txt \
    Maps/mapa1.map \
    Maps/mapa2.map \
    Maps/mapa3.map \
    Maps/mapa4.map \
    README.md

RESOURCES += \
    images.qrc

FORMS += \
    one_field_form.ui \
    discretize_form.ui \
    info_dialog.ui
