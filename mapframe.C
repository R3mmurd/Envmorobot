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

# include <sstream>

# include <mapframe.H>
# include <QMenuBar>
# include <QScrollBar>
# include <QMessageBox>
# include <QFileDialog>
# include <QDir>
# include <QStatusBar>

# include <infowindow.H>

MapFrame::MapFrame(QWidget * parent)
  : QMainWindow(parent), panel(nullptr)
{
  init_gui();
}

void MapFrame::init_gui()
{
  setWindowIcon(QIcon(":/icons/robot-image"));

  init_actions();
  create_tools_bars();
  init_menu();
  statusBar()->show();

  showMaximized();
}

void MapFrame::init_actions()
{
  action_open_map = new QAction(QIcon(":/icons/open-icon"), "Open map", this);
  action_open_map->setShortcut(tr("Ctrl+o"));
  connect(action_open_map, SIGNAL(triggered()), this, SLOT(slot_open_map()));

  action_save_enviroment = new QAction(QIcon(":/icons/save-icon"),
                                       "Save enviroment", this);
  action_save_enviroment->setShortcut(tr("Ctrl+s"));
  connect(action_save_enviroment, SIGNAL(triggered(bool)),
          this, SLOT(slot_save_enviroment()));
  action_save_enviroment->setEnabled(false);

  action_export_path = new QAction("Export min path", this);
  connect(action_export_path, SIGNAL(triggered()),
          this, SLOT(slot_export_min_path()));
  action_export_path->setEnabled(false);

  action_exit = new QAction(QIcon(":/icons/exit-icon"), "Exit", this);
  action_exit->setShortcut(tr("Ctrl+q"));
  connect(action_exit, SIGNAL(triggered()), this, SLOT(close()));

  action_zoom_in = new QAction(QIcon(":/icons/zoom-in-icon"), "Zoom in", this);
  action_zoom_in->setShortcut(tr("Ctrl++"));
  action_zoom_in->setEnabled(false);
  connect(action_zoom_in, SIGNAL(triggered()), this, SLOT(slot_zoom_in()));

  action_zoom_out = new QAction(QIcon(":/icons/zoom-out-icon"),
                                "Zoom out", this);
  action_zoom_out->setShortcut(tr("Ctrl+-"));
  action_zoom_out->setEnabled(false);
  connect(action_zoom_out, SIGNAL(triggered()), this, SLOT(slot_zoom_out()));

  action_zoom_original = new QAction(QIcon(":/icons/zoom-original-icon"),
                                     "Normal size", this);
  action_zoom_original->setEnabled(false);
  connect(action_zoom_original, SIGNAL(triggered()),
          this, SLOT(slot_normal_size()));

  action_show_rulers = new QAction(QIcon(":/icons/rulers-icon"),
                                   "Show rulers", this);
  action_show_rulers->setEnabled(false);
  action_show_rulers->setCheckable(true);
  connect(action_show_rulers, SIGNAL(triggered()),
          this, SLOT(slot_show_rulers()));

  action_show_file_tools_bar = new QAction("File", this);
  action_show_file_tools_bar->setCheckable(true);
  action_show_file_tools_bar->setChecked(true);
  connect(action_show_file_tools_bar, SIGNAL(triggered()),
          this, SLOT(slot_show_tools_bars()));

  action_show_view_tools_bar = new QAction("View", this);
  action_show_view_tools_bar->setCheckable(true);
  action_show_view_tools_bar->setChecked(true);
  connect(action_show_view_tools_bar, SIGNAL(triggered()),
          this, SLOT(slot_show_tools_bars()));

  action_show_operations_tools_bar = new QAction("Operations", this);
  action_show_operations_tools_bar->setCheckable(true);
  action_show_operations_tools_bar->setChecked(true);
  connect(action_show_operations_tools_bar, SIGNAL(triggered()),
          this, SLOT(slot_show_tools_bars()));

  action_close = new QAction(QIcon(":/icons/close-icon"), "Close", this);
  action_close->setEnabled(false);
  connect(action_close, SIGNAL(triggered()),
          this, SLOT(slot_close_current_work()));

  action_discretize = new QAction(QIcon(":/icons/discretize-icon"),
                                  "Discretize", this);
  action_discretize->setEnabled(false);
  connect(action_discretize, SIGNAL(triggered()),
          this, SLOT(slot_discretize()));

  action_build_cells = new QAction(QIcon(":/icons/cells-icon"),
                                   "Build square cells", this);
  action_build_cells->setEnabled(false);
  connect(action_build_cells, SIGNAL(triggered()),
          this, SLOT(slot_build_cells()));

  action_build_quad_tree = new QAction(QIcon(":/icons/qtree-icon"),
                                       "Build quad tree", this);
  action_build_quad_tree->setEnabled(false);
  connect(action_build_quad_tree, SIGNAL(triggered()),
          this, SLOT(slot_build_quad_tree()));

  action_visibility_graph = new QAction(QIcon(":/icons/vgraph-icon"),
                                        "Build visibility graph", this);
  action_visibility_graph->setEnabled(false);
  connect(action_visibility_graph, SIGNAL(triggered()),
          this, SLOT(slot_visibility_graph()));

  action_compute_min_path = new QAction(QIcon(":/icons/path-icon"),
                                        "Compute min path", this);
  action_compute_min_path->setEnabled(false);
  connect(action_compute_min_path, SIGNAL(triggered()),
          this, SLOT(slot_compute_min_path()));

  action_select_mission = new QAction(QIcon(":/icons/mission-icon"),
                                      "Select mission points", this);
  connect(action_select_mission, SIGNAL(triggered(bool)),
          this, SLOT(slot_start_select_mission()));
  action_select_mission->setEnabled(false);

  action_clean = new QAction(QIcon(":icons/clean-icon"), "Clean map", this);
  action_clean->setEnabled(false);
  connect(action_clean, SIGNAL(triggered()), this, SLOT(slot_clean()));

  action_show_arcs = new QAction(QIcon(":/icons/connectivity-icon"),
                     "View connectivity", this);
  action_show_arcs->setEnabled(false);
  action_show_arcs->setCheckable(true);
  connect(action_show_arcs, SIGNAL(triggered()), this, SLOT(slot_show_arcs()));

  action_about = new QAction("About", this);
  connect(action_about, SIGNAL(triggered()), this, SLOT(slot_about()));

  action_about_qt = new QAction("About Qt", this);
  connect(action_about_qt, SIGNAL(triggered()), this, SLOT(slot_about_qt()));

  action_show_info = new QAction(QIcon(":icons/info-icon"),
                                 "Show information", this);
  action_show_info->setEnabled(false);
  connect(action_show_info, SIGNAL(triggered()), this, SLOT(slot_show_info()));
}

void MapFrame::create_tools_bars()
{
  file_tools_bar.addAction(action_open_map);
  file_tools_bar.addAction(action_save_enviroment);
  file_tools_bar.addSeparator();
  file_tools_bar.addAction(action_close);
  file_tools_bar.setIconSize(QSize(24, 24));
  addToolBar(&file_tools_bar);

  view_tools_bar.addAction(action_zoom_in);
  view_tools_bar.addAction(action_zoom_out);
  view_tools_bar.addAction(action_zoom_original);
  view_tools_bar.addSeparator();
  view_tools_bar.addAction(action_show_rulers);
  view_tools_bar.addSeparator();
  view_tools_bar.addAction(action_show_arcs);
  view_tools_bar.addSeparator();
  view_tools_bar.addAction(action_show_info);
  view_tools_bar.setIconSize(QSize(24, 24));
  addToolBar(&view_tools_bar);

  operations_tools_bar.addAction(action_discretize);
  operations_tools_bar.addAction(action_build_cells);
  operations_tools_bar.addAction(action_build_quad_tree);
  operations_tools_bar.addAction(action_visibility_graph);
  operations_tools_bar.addSeparator();
  operations_tools_bar.addAction(action_select_mission);
  operations_tools_bar.addSeparator();
  operations_tools_bar.addAction(action_compute_min_path);
  operations_tools_bar.addSeparator();
  operations_tools_bar.addAction(action_clean);
  operations_tools_bar.setIconSize(QSize(32, 32));
  addToolBar(Qt::LeftToolBarArea, &operations_tools_bar);
}

void MapFrame::init_menu()
{

  file_menu.setTitle("&File");
  file_menu.addAction(action_open_map);
  file_menu.addAction(action_save_enviroment);
  file_menu.addAction(action_export_path);
  file_menu.addSeparator();
  file_menu.addAction(action_exit);
  menuBar()->addMenu(&file_menu);

  view_menu.setTitle("&View");
  view_menu.addAction(action_zoom_in);
  view_menu.addAction(action_zoom_out);
  view_menu.addAction(action_zoom_original);
  view_menu.addAction(action_show_rulers);
  view_menu.addAction(action_show_arcs);
  view_menu.addSeparator();
  QMenu * sub_menu = new QMenu("&Toolbars");
  sub_menu->addAction(action_show_file_tools_bar);
  sub_menu->addAction(action_show_view_tools_bar);
  sub_menu->addAction(action_show_operations_tools_bar);
  view_menu.addMenu(sub_menu);
  menuBar()->addMenu(&view_menu);

  operations_menu.setTitle("&Operations");
  operations_menu.addAction(action_discretize);
  operations_menu.addAction(action_build_cells);
  operations_menu.addAction(action_build_quad_tree);
  operations_menu.addAction(action_visibility_graph);
  operations_menu.addSeparator();
  operations_menu.addAction(action_select_mission);
  operations_menu.addSeparator();
  operations_menu.addAction(action_compute_min_path);
  operations_menu.addSeparator();
  operations_menu.addAction(action_clean);
  menuBar()->addMenu(&operations_menu);

  help_menu.setTitle("&Help");
  help_menu.addAction(action_show_info);
  help_menu.addSeparator();
  help_menu.addAction(action_about);
  help_menu.addAction(action_about_qt);
  menuBar()->addMenu(&help_menu);
}

void MapFrame::adjust_scroll_bar(QScrollBar * scroll_bar,
                                  const double & factor)
{
  scroll_bar->setValue(int(factor * scroll_bar->value() +
                          ((factor - 1) * scroll_bar->pageStep() / 2)));
}

void MapFrame::slot_zoom_in()
{
  double factor = panel->get_zoom_factor() + 0.1;
  panel->zoom(factor);
  action_zoom_in->setEnabled(factor <= 2);
  action_zoom_out->setEnabled(true);
  adjust_scroll_bar(scroll_area->horizontalScrollBar(), factor);
  adjust_scroll_bar(scroll_area->verticalScrollBar(), factor);
}

void MapFrame::slot_zoom_out()
{
  double factor = panel->get_zoom_factor() - 0.1;
  panel->zoom(factor);
  action_zoom_out->setEnabled(factor >= 0.3);
  action_zoom_in->setEnabled(true);
  adjust_scroll_bar(scroll_area->horizontalScrollBar(), factor);
  adjust_scroll_bar(scroll_area->verticalScrollBar(), factor);
}

void MapFrame::slot_normal_size()
{
  panel->zoom(1.0);
  action_zoom_out->setEnabled(true);
  action_zoom_in->setEnabled(true);
  adjust_scroll_bar(scroll_area->horizontalScrollBar(), 1.0);
  adjust_scroll_bar(scroll_area->verticalScrollBar(), 1.0);
}

void MapFrame::slot_show_rulers()
{
  panel->show_rulers(action_show_rulers->isChecked());
}

void MapFrame::slot_show_arcs()
{
  panel->show_arcs(action_show_arcs->isChecked());
}

void MapFrame::closeEvent(QCloseEvent *)
{

}

void MapFrame::slot_discretize()
{
  action_discretize->setEnabled(false);
  action_build_cells->setEnabled(false);
  action_build_quad_tree->setEnabled(false);
  action_visibility_graph->setEnabled(false);

  DiscretizeWindow * w = new DiscretizeWindow(panel, this);
  w->show();
  connect(w, SIGNAL(signal_closed()), this, SLOT(child_closed()));
  connect(w, SIGNAL(signal_accepted(QString)),
          this, SLOT(slot_child_accepted(QString)));
}

void MapFrame::slot_build_cells()
{
  action_discretize->setEnabled(false);
  action_build_cells->setEnabled(false);
  action_build_quad_tree->setEnabled(false);
  action_visibility_graph->setEnabled(false);
  OnefieldWindow * w = new OnefieldWindow(panel,
                                          Algorithm::Building_Square_Cells,
                                          this);
  w->show();
  connect(w, SIGNAL(signal_closed()), this, SLOT(child_closed()));
  connect(w, SIGNAL(signal_accepted(QString)),
          this, SLOT(slot_child_accepted(QString)));
}

void MapFrame::slot_build_quad_tree()
{
  action_discretize->setEnabled(false);
  action_build_cells->setEnabled(false);
  action_build_quad_tree->setEnabled(false);
  action_visibility_graph->setEnabled(false);
  OnefieldWindow * w = new OnefieldWindow(panel,
                                          Algorithm::Building_Quad_Tree, this);
  w->show();
  connect(w, SIGNAL(signal_closed()), this, SLOT(child_closed()));
  connect(w, SIGNAL(signal_accepted(QString)),
          this, SLOT(slot_child_accepted(QString)));
}

void MapFrame::slot_visibility_graph()
{
  action_discretize->setEnabled(false);
  action_build_cells->setEnabled(false);
  action_build_quad_tree->setEnabled(false);
  action_visibility_graph->setEnabled(false);
  OnefieldWindow * w = new OnefieldWindow(panel,
                                          Algorithm::Building_Visibility_Graph, this);
  w->show();
  connect(w, SIGNAL(signal_closed()), this, SLOT(child_closed()));
  connect(w, SIGNAL(signal_accepted(QString)),
          this, SLOT(slot_child_accepted(QString)));
}

void MapFrame::child_closed()
{
  action_discretize->setEnabled(true);
  action_build_cells->setEnabled(true);
  action_build_quad_tree->setEnabled(true);
  action_visibility_graph->setEnabled(true);
  bool test = panel->get_algorithm() != Algorithm::Num_Algorithms;
  action_compute_min_path->setEnabled(test);
  action_save_enviroment->setEnabled(test);
  action_select_mission->setEnabled(test);

}

void MapFrame::slot_open_map()
{
  QString file_name = QFileDialog::getOpenFileName(this, "Open map",
                                                   QDir::homePath(), "*");
  if (file_name.isEmpty())
    return;

  try
  {
    map.load_file(file_name.toStdString());
  }
  catch(...)
  {
    std::stringstream s;
    s << "Cannot open file " << file_name.toStdString();
    QMessageBox::critical(this, "Error", s.str().c_str());
    return;
  }

  panel = new MapPanel(map);
  scroll_area = new QScrollArea;
  scroll_area->setWidget(panel);
  scroll_area->setAlignment(Qt::AlignCenter);
  scroll_area->setBackgroundRole(QPalette::Dark);

  setCentralWidget(scroll_area);

  action_open_map->setEnabled(false);
  set_working_actions_enable(true);

  connect(panel, SIGNAL(to_status_bar(QString, int)),
          this, SLOT(slot_update_status_bar(QString, int)));
}

void MapFrame::slot_save_enviroment()
{
  QString file_name = QFileDialog::getSaveFileName(this, "Save file",
                                                   QDir::homePath(), "*");
  if (file_name.isEmpty())
    return;

  try
  {
    panel->save_enviroment(file_name);
  }
  catch(const std::exception & e)
  {
    QMessageBox::critical(this, "Error", e.what());
  }
}

void MapFrame::slot_show_tools_bars()
{
  file_tools_bar.setVisible(action_show_file_tools_bar->isChecked());
  view_tools_bar.setVisible(action_show_view_tools_bar->isChecked());
  operations_tools_bar.setVisible(
    action_show_operations_tools_bar->isChecked()
  );
}

void MapFrame::set_working_actions_enable(bool b)
{
  action_zoom_in->setEnabled(b);
  action_zoom_out->setEnabled(b);
  action_zoom_original->setEnabled(b);
  action_close->setEnabled(b);
  action_discretize->setEnabled(b);
  action_build_cells->setEnabled(b);
  action_build_quad_tree->setEnabled(b);
  action_visibility_graph->setEnabled(b);
  action_show_rulers->setEnabled(b);
  action_show_arcs->setEnabled(b);
}

void MapFrame::slot_close_current_work()
{
  action_open_map->setEnabled(true);
  action_clean->setEnabled(false);
  action_compute_min_path->setEnabled(false);
  action_show_info->setEnabled(false);
  set_working_actions_enable(false);
  action_compute_min_path->setEnabled(false);
  action_show_arcs->setChecked(false);
  action_show_rulers->setChecked(false);
  action_export_path->setEnabled(false);
  disconnect(panel, SIGNAL(to_status_bar(QString, int)),
             this, SLOT(slot_update_status_bar(QString, int)));
  panel->close();
  map.empty();
  scroll_area->close();
}

void MapFrame::slot_about()
{
  QString about_ropad_txt;

  about_ropad_txt.append("Robot Path Builder VersiOn 1.0\n\n");
  about_ropad_txt.append("Author: Alejandro Mujica\n");
  about_ropad_txt.append("Email: aledrums en gmail.com");

  QMessageBox::about(this, "About", about_ropad_txt);
}

void MapFrame::slot_about_qt()
{
  QMessageBox::aboutQt(this, "About Qt");
}

void MapFrame::slot_compute_min_path()
{
  try
  {
    panel->compute_min_path();
    action_compute_min_path->setEnabled(false);
    action_export_path->setEnabled(true);
  }
  catch(const std::exception & e)
  {
    QMessageBox::critical(this, "Error", e.what());
  }
}

void MapFrame::slot_export_min_path()
{
  if (not panel->is_there_min_path())
    {
      QMessageBox::critical(this, "Error",
                            "There is not computed min path yet");
      return;
    }

  QString file_name = QFileDialog::getSaveFileName(this, "Save file",
                                                   QDir::homePath(), "*");
  if (file_name.isEmpty())
    return;

  try
  {
    panel->save_min_path(file_name);
  }
  catch(const std::exception & e)
  {
    QMessageBox::critical(this, "Error", e.what());
  }
}

void MapFrame::slot_show_info()
{
  action_show_info->setEnabled(false);
  InfoWindow * w = new InfoWindow(this);
  w->plain_text_edit()->appendPlainText(panel->get_info());
  connect(w, SIGNAL(signal_closed(QString)), this,
          SLOT(slot_child_accepted(QString)));
  w->show();
}

void MapFrame::slot_child_accepted(const QString &)
{
  action_show_info->setEnabled(true);
  action_clean->setEnabled(true);
  action_save_enviroment->setEnabled(true);
}

void MapFrame::slot_start_select_mission()
{
  action_discretize->setEnabled(false);
  action_build_cells->setEnabled(false);
  action_build_quad_tree->setEnabled(false);
  action_visibility_graph->setEnabled(false);
  action_compute_min_path->setEnabled(false);
  action_select_mission->setEnabled(false);
  action_clean->setEnabled(false);
  connect(panel, SIGNAL(finish_select_mission()),
          this, SLOT(slot_finish_select_mission()));
  panel->start_select_mission();
}

void MapFrame::slot_finish_select_mission()
{
  action_discretize->setEnabled(true);
  action_build_cells->setEnabled(true);
  action_build_quad_tree->setEnabled(true);
  action_visibility_graph->setEnabled(true);
  action_compute_min_path->setEnabled(true);
  action_select_mission->setEnabled(true);
  action_clean->setEnabled(true);
  disconnect(panel, SIGNAL(finish_select_mission()),
             this, SLOT(slot_finish_select_mission()));
}

void MapFrame::slot_update_status_bar(const QString & msg, int timeout)
{
  statusBar()->showMessage(msg, timeout);
}

void MapFrame::slot_clean()
{
  panel->clean();
  action_clean->setEnabled(false);
  action_show_info->setEnabled(false);
  action_save_enviroment->setEnabled(false);
  action_select_mission->setEnabled(false);
  action_compute_min_path->setEnabled(false);
  action_export_path->setEnabled(false);
}

