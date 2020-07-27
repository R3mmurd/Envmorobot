/*
  Envmorobot.

  Author: Alejandro Mujica (aledrums@gmail.com)
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

