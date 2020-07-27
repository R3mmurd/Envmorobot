/*
  Envmorobot.

  Author: Alejandro Mujica (aledrums@gmail.com)
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
