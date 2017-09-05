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

# ifndef MAPFRAME_H
# define MAPFRAME_H

# include <QMainWindow>
# include <QMenu>
# include <QScrollArea>
# include <QToolBar>
# include <onefieldwindow.H>
# include <discretizewindow.H>

# include <mappanel.H>

class MapFrame : public QMainWindow
{
  Q_OBJECT

  QScrollArea * scroll_area;

  MapPanel * panel;
  GeometricMap map;

  void adjust_scroll_bar(QScrollBar *, const double &);

  QMenu file_menu;
  QMenu view_menu;
  QMenu operations_menu;
  QMenu help_menu;

  QToolBar file_tools_bar;
  QToolBar view_tools_bar;
  QToolBar operations_tools_bar;

  QAction * action_zoom_in;
  QAction * action_zoom_out;
  QAction * action_zoom_original;

  QAction * action_show_rulers;
  QAction * action_show_arcs;

  QAction * action_discretize;
  QAction * action_build_cells;
  QAction * action_build_quad_tree;
  QAction * action_visibility_graph;
  QAction * action_compute_min_path;
  QAction * action_select_mission;
  QAction * action_clean;

  QAction * action_close;

  QAction * action_show_file_tools_bar;
  QAction * action_show_view_tools_bar;
  QAction * action_show_operations_tools_bar;

  QAction * action_open_map;
  QAction * action_save_enviroment;
  QAction * action_export_path;
  QAction * action_exit;

  QAction * action_show_info;
  QAction * action_about;
  QAction * action_about_qt;

  void create_tools_bars();

  void init_gui();

  void init_actions();

  void init_menu();

  void set_working_actions_enable(bool);

protected:
  void closeEvent(QCloseEvent *);

public:
  explicit MapFrame(QWidget * parent = 0);

signals:
  void signal_close();

public slots:
  void slot_open_map();

  void slot_save_enviroment();

  void slot_export_min_path();

  void slot_zoom_in();

  void slot_zoom_out();

  void slot_normal_size();

  void slot_show_rulers();

  void slot_show_arcs();

  void slot_discretize();

  void slot_build_cells();

  void slot_build_quad_tree();

  void slot_visibility_graph();

  void slot_compute_min_path();

  void slot_clean();

  void child_closed();

  void slot_show_tools_bars();

  void slot_close_current_work();

  void slot_about();

  void slot_about_qt();

  void slot_show_info();

  void slot_child_accepted(const QString & message);

  void slot_start_select_mission();

  void slot_finish_select_mission();

  void slot_update_status_bar(const QString &, int);
};

# endif // MAPFRAME_H

