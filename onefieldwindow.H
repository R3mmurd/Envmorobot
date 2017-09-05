/*
  This file is part of Ropab System.
  Copyright (C) 2012 by Alejandro J. Mujica

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  Any user request of this software, write to 

  Alejandro Mujica

  aledrums@gmail.com
*/

# ifndef ONEFIELDWINDOW_H
# define ONEFIELDWINDOW_H

# include <QWidget>
# include <ui_one_field_form.h>
# include <enviroment.H>

class MapPanel;

class OnefieldWindow : public QWidget
{
  Q_OBJECT

  Ui_One_Field_Form ui;

  Algorithm algo;

  MapPanel * panel;

protected:
  void closeEvent(QCloseEvent *);

public:
  explicit OnefieldWindow(MapPanel *, const Algorithm &,
                          QWidget * parent = 0);

signals:
  void signal_closed();
  void signal_accepted(const QString & message);

public slots:
  void execute_algorithm();

};

# endif // ONEFIELDWINDOW_H
