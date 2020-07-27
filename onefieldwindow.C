/*
  Envmorobot.

  Author: Alejandro Mujica (aledrums@gmail.com)
*/

# include <onefieldwindow.H>
# include <QMessageBox>

# include <mapframe.H>
# include <mappanel.H>

OnefieldWindow::OnefieldWindow(MapPanel * _panel, const Algorithm & _algo,
                               QWidget * parent)
  : QWidget(parent, Qt::Tool), algo(_algo), panel(_panel)
{
  ui.setupUi(this);

  switch(algo)
    {
    case Algorithm::Building_Square_Cells:
      setWindowTitle("Build square cells");
      ui.label->setText("Cells radius");
      break;
    case Algorithm::Building_Quad_Tree:
      setWindowTitle("Build Quad Tree");
      ui.label->setText("Robot radius");
      break;
    case Algorithm::Building_Visibility_Graph:
      setWindowTitle("Build visibility graph");
      ui.label->setText("Robot radius");
    }

  ui.lineEdit->setValidator(new QDoubleValidator);
  ui.lineEdit->setFocus();

  connect(ui.btn_exec, SIGNAL(clicked()), this, SLOT(execute_algorithm()));
}

void OnefieldWindow::execute_algorithm()
{
  if (ui.lineEdit->text().isEmpty())
    {
      QMessageBox::critical(this, "Error",
                            "You must introduce the required value");
      ui.lineEdit->setFocus();
      return;
    }

  const double value = ui.lineEdit->text().toDouble();

  if (value <= 0)
    {
      QMessageBox::critical(this, "Error",
                            "The input must be greater than zero");
      ui.lineEdit->setFocus();
      return;
    }

  try
  {
    QString message;

    switch(algo)
      {
      case Algorithm::Building_Square_Cells:
        message = "Building square cells algorithm";
        panel->exec_build_cells_algo(value);
        break;
      case Algorithm::Building_Quad_Tree:
        message = "Building quad tree algorithm";
        panel->exec_quad_tree_algo(value);
        break;
      case Algorithm::Building_Visibility_Graph:
        message = "Building visibility graph algorithm";
        panel->exec_build_visibility_graph_algo(value);
      }

    emit signal_accepted(message);
    close();
  }
  catch(const std::exception & e)
  {
    QMessageBox::critical(this, "Error", e.what());
  }
}

void OnefieldWindow::closeEvent(QCloseEvent *)
{
  emit signal_closed();
}

