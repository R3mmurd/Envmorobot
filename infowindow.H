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

# ifndef INFOWINDOW_H
# define INFOWINDOW_H

# include <QDialog>
# include <ui_info_dialog.h>

class InfoWindow : public QDialog
{
  Q_OBJECT

  Ui_Info_DIalog ui;

protected:
  void closeEvent(QCloseEvent *);

public:
  explicit InfoWindow(QWidget * parent = 0);

  QPlainTextEdit * plain_text_edit();

signals:
  void signal_closed(const QString & message);

public slots:
  void slot_save_info();

};

# endif // INFOWINDOW_H
