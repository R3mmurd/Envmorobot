/*
  Envmorobot.

  Author: Alejandro Mujica (aledrums@gmail.com)
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


