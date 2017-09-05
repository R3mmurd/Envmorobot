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

# include <infowindow.H>

# include <QFileDialog>
# include <QMessageBox>

# include <fstream>

InfoWindow::InfoWindow(QWidget * parent)
  : QDialog(parent, Qt::Tool)
{
  ui.setupUi(this);
  setFixedSize(size());
  connect(ui.btn_save, SIGNAL(clicked()), this, SLOT(slot_save_info()));
}

void InfoWindow::closeEvent(QCloseEvent *)
{
  emit signal_closed("");
}

QPlainTextEdit * InfoWindow::plain_text_edit()
{
  return ui.text;
}

void InfoWindow::slot_save_info()
{
  QString file_name = QFileDialog::getSaveFileName(this, "Guardar",
                                                   QDir::currentPath(), "*.*");

  if (file_name.isEmpty())
    return;

  std::ofstream file(file_name.toStdString().c_str());

  if (not file)
    {
      QMessageBox::critical(this, "Error", "No puede crear el archivo");
      return;
    }

  file << ui.text->toPlainText().toStdString() << std::endl;

  file.close();
}


