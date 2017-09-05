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

# include <discretizewindow.H>
# include <QMessageBox>

# include <mappanel.H>

DiscretizeWindow::DiscretizeWindow(MapPanel * _panel, QWidget * parent)
  : QWidget(parent, Qt::Tool), panel(_panel)
{
  ui.setupUi(this);

  ui.edt_length->setValidator(new QDoubleValidator);
  ui.edt_radius->setValidator(new QDoubleValidator);

  connect(ui.btn_exec, SIGNAL(clicked()), this, SLOT(discretize()));

  setFixedSize(size());

}

void DiscretizeWindow::discretize()
{
  if (ui.edt_length->text().isEmpty())
    {
      QMessageBox::critical(this, "Error",
                            "You must set the distance between dots");
      ui.edt_length->setFocus();
      return;
    }

  const double length = ui.edt_length->text().toDouble();

  if (length <= 0)
    {
      QMessageBox::critical(this, "Error",
                            "The value of length must be greater than zero");
      ui.edt_length->setFocus();
      return;
    }

  if (ui.edt_radius->text().isEmpty())
    {
      QMessageBox::critical(this, "Error", "You must set the robot radius");
      ui.edt_radius->setFocus();
      return;
    }

  const double radius = ui.edt_radius->text().toDouble();

  if (radius <= 0)
    {
      QMessageBox::critical(this, "Error",
                            "The value of radius must be greater than zero");
      ui.edt_radius->setFocus();
      return;
    }

  try
  {
    panel->exec_discretize_algo(length, radius);
    emit signal_accepted("Discretization algorithm");
    close();
  }
  catch(const std::exception & e)
  {
    QMessageBox::critical(this, "Error", e.what());
  }
}

void DiscretizeWindow::closeEvent(QCloseEvent *)
{
  emit signal_closed();
}

