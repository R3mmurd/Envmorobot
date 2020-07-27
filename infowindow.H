/*
  Envmorobot.

  Author: Alejandro Mujica (aledrums@gmail.com)
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
