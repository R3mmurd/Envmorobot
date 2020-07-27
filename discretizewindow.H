/*
  Envmorobot.

  Author: Alejandro Mujica (aledrums@gmail.com)
*/

# ifndef DISCRETIZEWINDOW_H
# define DISCRETIZEWINDOW_H

# include <QWidget>
# include <ui_discretize_form.h>

class MapPanel;

class DiscretizeWindow : public QWidget
{
  Q_OBJECT

  Ui_Discretize_Form ui;

  MapPanel * panel;

protected:
  void closeEvent(QCloseEvent *);

public:
  explicit DiscretizeWindow(MapPanel *, QWidget * parent = 0);

signals:
  void signal_closed();
  void signal_accepted(const QString & message);

public slots:
  void discretize();

};

# endif // DISCRETIZEWINDOW_H
