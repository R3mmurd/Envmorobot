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

# ifndef MAPPANEL_H
# define MAPPANEL_H

# include <QWidget>
# include <QPainter>
# include <QKeyEvent>
# include <QMouseEvent>

# include <geometricmap.H>
# include <enviroment.H>

# include <fstream>

# define WIDTH 1000
# define HEIGHT 650

# define W_CENTER WIDTH / 2
# define H_CENTER HEIGHT / 2

# define H_MARGIN 50
# define V_MARGIN 50

# define NODE_RADIUS 1

class MapPanel : public QWidget
{
  Q_OBJECT

  enum class MissionStatus { Waiting, Start, End, Completed } mission_status;

  QPoint mission_pos;

  QSize dim;

  GeometricMap & map;

  double zoom_factor;

  double scale;

  double x_init;

  double y_init;

  double robot_radius;

  bool debug;

  bool __show_rulers;

  bool __show_arcs;

  EnviromentGraph enviroment_graph;

  DynList<Point> min_path;

  void draw_rulers(QPainter &);

  void draw_margins(QPainter &);

  void draw_mission_point(QPainter &, QColor, QPointF);

  void draw_graph(QPainter &);

  Algorithm algo;

  void draw_obstacle(Obstacle &, QPainter &);

  QPointF point2qpointf(const Point &);

  Point qpoint2point(const QPoint &);

protected:
  void paintEvent(QPaintEvent *);

  void mouseMoveEvent(QMouseEvent *);

  void mousePressEvent(QMouseEvent *);

public:
  MapPanel(GeometricMap &, QWidget * parent = 0);

  const double & get_zoom_factor() const;

  void zoom(const double &);

  void show_rulers(bool);

  void show_arcs(bool);

  void exec_discretize_algo(const double &, const double &);

  void exec_build_cells_algo(const double &);

  void exec_quad_tree_algo(const double &);

  void exec_build_visibility_graph_algo(const double &);

  const Algorithm & get_algorithm() const;

  void compute_min_path();

  bool is_there_min_path();

  void save_enviroment(const QString &);

  void save_min_path(const QString &);

  QString get_info();

  void clean();

  void start_select_mission();

signals:
  void finish_select_mission();

  void to_status_bar(const QString &, int);

public slots:

};

# endif // MAPPANEL_H

