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

# include <mappanel.H>

# include <buffer.H>
# include <QDateTime>

MapPanel::MapPanel(GeometricMap & _map, QWidget * parent)
  : QWidget(parent), mission_status(MissionStatus::Waiting), map(_map),
    zoom_factor(1.0), dim(WIDTH, HEIGHT), __show_rulers(false),
    enviroment_graph(), algo(Algorithm::Num_Algorithms), __show_arcs(false)
{
  std::fstream file("debug.txt");
  if (not file)
    debug = false;
  else
    {
      file >> debug;
      file.close();
    }

  const double map_width = map.get_width();
  const double map_height = map.get_height();

  scale = std::min<double>((WIDTH - 2 * H_MARGIN) / map_width,
                           (HEIGHT - 2 * V_MARGIN) / map_height);

  resize(dim);

  x_init = W_CENTER - (scale * ((map_width / 2) ));
  y_init = H_CENTER + (scale * ((map_height / 2)));
}

void MapPanel::draw_mission_point(QPainter & painter, QColor color,
                                  QPointF center)
{
  QPen old_pen = painter.pen();
  QPen pen(color);
  pen.setWidth(2);
  painter.setPen(pen);
  painter.drawLine(center.x() - 5, center.y() - 5,
                   center.x() + 5, center.y() + 5);
  painter.drawLine(center.x() - 5, center.y() + 5,
                   center.x() + 5, center.y() - 5);
  painter.setPen(old_pen);
}

void MapPanel::paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.setRenderHint(QPainter::Antialiasing, true);

  painter.fillRect(0, 0, width(), height(), Qt::white);

  if (__show_rulers)
    draw_rulers(painter);
  if (debug)
    draw_margins(painter);

  painter.scale(zoom_factor, zoom_factor);

  QPen pen;
  QBrush brush;

  pen = painter.pen();
  brush = painter.brush();
  painter.setPen(QPen(Qt::black, 2));

  for (DynDlist<Segment>::Iterator it(map.get_walls_list()); it.has_current();
       it.next())
    {
      Segment & s = it.get_current();

      QPointF src = point2qpointf(s.get_src_point());
      QPointF tgt = point2qpointf(s.get_tgt_point());

      if (algo == Algorithm::Discretization or algo == Algorithm::Building_Visibility_Graph)
        {
          QPen pen = painter.pen();
          QBrush brush = painter.brush();
          Obstacle & e_wall =
            const_cast<Obstacle &>(
              Buffer::get_instance()->get_extended_wall(s, robot_radius)
            );
          painter.setPen(Qt::transparent);
          painter.setBrush(QBrush(Qt::lightGray, Qt::Dense5Pattern));
          draw_obstacle(e_wall, painter);
          painter.setPen(pen);
          painter.setBrush(brush);
        }
      painter.drawLine(src, tgt);
    }

  painter.setPen(pen);
  painter.setBrush(brush);

  pen = painter.pen();
  brush = painter.brush();

  DynDlist<Obstacle> & obstacles_list = map.get_obstacles_list();

  for (DynDlist<Obstacle>::Iterator it(obstacles_list); it.has_current();
       it.next())
    {
      Obstacle & obstacle = it.get_current();

      if (algo == Algorithm::Discretization or algo == Algorithm::Building_Visibility_Graph)
        {
          Obstacle & e_obstacle =
            const_cast<Obstacle &>(
              Buffer::get_instance()->get_extended_obstacle(obstacle,
                                                            robot_radius)
             );
          painter.setPen(Qt::transparent);
          painter.setBrush(QBrush(Qt::lightGray, Qt::Dense5Pattern));
          draw_obstacle(e_obstacle, painter);
        }
      painter.setPen(Qt::darkCyan);
      painter.setBrush(Qt::darkCyan);

      draw_obstacle(obstacle, painter);
    }

  painter.setPen(pen);
  painter.setBrush(brush);

  if (mission_status == MissionStatus::Start or
      mission_status == MissionStatus::End)
    draw_mission_point(painter, Qt::darkGray, mission_pos);

  if (enviroment_graph.beg != nullptr)
    draw_mission_point(painter, Qt::blue,
                       point2qpointf(enviroment_graph.beg->
                                     get_info().position));

  if (enviroment_graph.end != nullptr)
    draw_mission_point(painter, Qt::darkRed,
                       point2qpointf(enviroment_graph.end->
                                     get_info().position));

  draw_graph(painter);
}

void MapPanel::mouseMoveEvent(QMouseEvent * evt)
{
  if (mission_status != MissionStatus::Start and
      mission_status != MissionStatus::End)
    return;

  QString msg;

  if (mission_status == MissionStatus::Start)
    msg.append("Mission start point: ");
  else
    msg.append("Mission end point: ");


  mission_pos = evt->pos();

  msg.append(qpoint2point(mission_pos).to_string().c_str());

  emit to_status_bar(msg, 1000);

  repaint();
}

void MapPanel::mousePressEvent(QMouseEvent * evt)
{
  if (mission_status == MissionStatus::Start)
    {
      enviroment_graph.set_beg_node(qpoint2point(evt->pos()),
                                    algo == Algorithm::Building_Visibility_Graph,
                                    robot_radius, map);
      mission_status = MissionStatus::End;
    }
  else if (mission_status == MissionStatus::End)
    {
      enviroment_graph.set_end_node(qpoint2point(evt->pos()),
                                    algo == Algorithm::Building_Visibility_Graph,
                                    robot_radius, map);
      mission_status = MissionStatus::Completed;
      emit finish_select_mission();
      setMouseTracking(false);
    }
  else
    return;

  repaint();
}

QPointF MapPanel::point2qpointf(const Point & p)
{
  qreal x = (p.get_x().get_d() - map.get_min_x()) * scale + x_init;
  qreal y = -((p.get_y().get_d() - map.get_min_y()) * scale) + y_init;
  return QPointF(x, y);
}

Point MapPanel::qpoint2point(const QPoint & p)
{
  double x = ((p.x() - x_init) / scale) + map.get_min_x();
  double y = (-(p.y() - y_init) / scale) + map.get_min_y();
  return Point(x, y);
}

void MapPanel::draw_rulers(QPainter & painter)
{
  QPen pen = painter.pen();
  QBrush brush = painter.brush();

  painter.setPen(QPen(Qt::black, 1));

  painter.setFont(QFont("Arial", 8 * zoom_factor));

  painter.drawLine(H_MARGIN * zoom_factor, 0,
                   H_MARGIN * zoom_factor, (V_MARGIN / 3) * zoom_factor);
  painter.drawLine(width() - H_MARGIN * zoom_factor, 0,
                   width() - H_MARGIN * zoom_factor,
                  (V_MARGIN / 3) * zoom_factor);
  painter.drawLine(H_MARGIN * zoom_factor, (V_MARGIN / 3) * zoom_factor,
                   width() - H_MARGIN * zoom_factor,
                  (V_MARGIN / 3) * zoom_factor);

  for (int i = map.get_min_x(); i <= map.get_max_x(); ++i)
    {
      int x = ((i - map.get_min_x()) * scale + x_init) * zoom_factor;
      int y = (V_MARGIN / 3) * zoom_factor;
      painter.setPen(Qt::yellow);
      painter.drawLine(x, y, x, height());
      painter.setPen(Qt::black);
      painter.drawLine(x, y - 5 * zoom_factor, x, y + 5 * zoom_factor);
      painter.drawText(x + 2 * zoom_factor, y + 10 * zoom_factor,
                       QString().setNum(i));
    }

  painter.drawLine(0, V_MARGIN * zoom_factor,
                   (H_MARGIN / 3) * zoom_factor,
                    V_MARGIN * zoom_factor);
  painter.drawLine(0, height() - V_MARGIN * zoom_factor,
                   (H_MARGIN / 3) * zoom_factor,
                    height() - V_MARGIN * zoom_factor);
  painter.drawLine((H_MARGIN / 3) * zoom_factor, V_MARGIN * zoom_factor,
                   (H_MARGIN / 3) * zoom_factor,
                    height() - V_MARGIN * zoom_factor);

  for (int j = map.get_min_y(); j <= map.get_max_y(); ++j)
    {
      int x = (H_MARGIN / 3) * zoom_factor;
      int y = (-(j - map.get_min_y()) * scale + y_init) * zoom_factor;
      painter.setPen(Qt::yellow);
      painter.drawLine(x, y, width(), y);
      painter.setPen(Qt::black);
      painter.drawLine(x - 5 * zoom_factor, y, x + 5 * zoom_factor, y);
      painter.drawText(x + 2 * zoom_factor, y + 10 * zoom_factor,
                       QString().setNum(j));
    }
  painter.setPen(pen);
  painter.setBrush(brush);
}

void MapPanel::draw_margins(QPainter & painter)
{
  QPen pen = painter.pen();
  QBrush brush = painter.brush();
  painter.setPen(QPen(Qt::red, 1));
  painter.drawLine(H_MARGIN * zoom_factor, V_MARGIN * zoom_factor,
                   width() - H_MARGIN * zoom_factor, V_MARGIN* zoom_factor);
  painter.drawLine(H_MARGIN * zoom_factor, height() - V_MARGIN * zoom_factor,
                   width() - H_MARGIN * zoom_factor,
                   height() - V_MARGIN * zoom_factor);
  painter.drawLine(H_MARGIN * zoom_factor, V_MARGIN * zoom_factor,
                   H_MARGIN * zoom_factor, height() - V_MARGIN * zoom_factor);
  painter.drawLine(width() - H_MARGIN * zoom_factor, V_MARGIN * zoom_factor,
                   width() - H_MARGIN * zoom_factor,
                   height() - V_MARGIN * zoom_factor);
  painter.setPen(pen);
  painter.setBrush(brush);
}

const double & MapPanel::get_zoom_factor() const
{
  return zoom_factor;
}

void MapPanel::zoom(const double & factor)
{
  zoom_factor = factor;
  resize(dim * zoom_factor);
  repaint();
}

void MapPanel::show_rulers(bool s)
{
  __show_rulers = s;
  repaint();
}

void MapPanel::show_arcs(bool s)
{
  __show_arcs = s;
  repaint();
}

void MapPanel::draw_graph(QPainter & painter)
{
  QPen pen = painter.pen();
  QBrush brush = painter.brush();

  if (__show_arcs)
    {

      painter.setPen(QPen(Qt::darkGreen, 1));

      for (EnviromentGraph::Arc_Iterator it(enviroment_graph);
           it.has_current(); it.next())
        {
          EnviromentGraph::Arc * arc = it.get_current();
          QPointF src =
              point2qpointf(enviroment_graph.get_src_node(arc)->
                            get_info().position);
          QPointF tgt =
              point2qpointf(enviroment_graph.get_tgt_node(arc)->
                            get_info().position);
          painter.drawLine(src, tgt);
        }
    }

  for (EnviromentGraph::Node_Iterator it(enviroment_graph);
       it.has_current(); it.next())
    {
      EnviromentGraph::Node * node = it.get_current();

      switch(algo)
        {
        case Algorithm::Building_Quad_Tree:
          {
            const Point & node_position = node->get_info().position;
            QPointF q_node_position_f = point2qpointf(node_position);
            painter.setPen(QPen(Qt::lightGray, 1));
            painter.setBrush(Qt::lightGray);

            painter.drawEllipse(q_node_position_f, NODE_RADIUS, NODE_RADIUS);

            const double & lvl_len_rel = node->get_info().level_length_rel;

            const double w = scale * map.get_width() / lvl_len_rel;
            const double h = scale * map.get_height() / lvl_len_rel;

            painter.setPen(Qt::darkBlue);

            QBrush brush;
            brush.setStyle(Qt::Dense6Pattern);

            if (node->get_info().available)
              brush.setColor(Qt::green);
            else
              brush.setColor(Qt::red);

            painter.setBrush(brush);

            q_node_position_f -= QPointF(w / 2.0, h / 2.0);

            painter.drawRect(q_node_position_f.x(), q_node_position_f.y(),
                             w, h);
          }
          break;
        case Algorithm::Building_Visibility_Graph:
        case Algorithm::Discretization:
          {
            QPointF position = point2qpointf(node->get_info().position);
            painter.setPen(((node->get_info().available) ? Qt::green :
                                                           Qt::red));
            painter.setBrush(((node->get_info().available) ? Qt::green :
                                                             Qt::red));
            painter.drawEllipse(position, NODE_RADIUS, NODE_RADIUS);
          }
          break;
        case Algorithm::Building_Square_Cells:
          {
            const Point & __p = node->get_info().position;
            Point p(__p.get_x() - robot_radius, __p.get_y() + robot_radius);
            QPointF position = point2qpointf(p);
            painter.setPen(QPen(Qt::lightGray, 1));
            painter.setBrush(Qt::lightGray);
            painter.drawEllipse(point2qpointf(__p), NODE_RADIUS, NODE_RADIUS);
            QBrush brush;
            brush.setStyle(Qt::Dense6Pattern);

            if (node->get_info().available)
              brush.setColor(Qt::green);
            else
              brush.setColor(Qt::red);

            painter.setBrush(brush);
            painter.drawRect(position.x(), position.y(),
                             robot_radius * 2 * scale,
                             robot_radius * 2 * scale);
          }
          break;
        }
    }

  painter.setPen(QPen(Qt::black, 3));

  if (not min_path.is_empty())
    {
      Point p1;
      Point p2;

      DynList<Point>::Iterator it(min_path);

      p1 = it.get_current();
      it.next();

      for ( ; it.has_current(); it.next())
        {
          p2 = it.get_current();
          QPointF src_point = point2qpointf(p1);
          QPointF tgt_point = point2qpointf(p2);
          painter.drawLine(src_point, tgt_point);
          p1 = p2;
        }
    }

  painter.setPen(pen);
  painter.setBrush(brush);
}

void MapPanel::draw_obstacle(Obstacle & obstacle, QPainter & painter)
{
  const size_t & sz = obstacle.size();

  QPointF * points = new QPointF[sz];
  int i = 0;

  for (Obstacle::Vertex_Iterator it(obstacle); it.has_current(); it.next(), ++i)
    {
      Vertex & v = it.get_current_vertex();
      points[i] = point2qpointf(v);
    }

  painter.drawPolygon(points, sz);

  delete [] points;
}

void MapPanel::exec_discretize_algo(const double & length,
                                    const double & radius)
{
  DiscretizationAlgorithm disc_algo(map);
  enviroment_graph.clear();
  min_path.empty();
  emit to_status_bar("Discretizing enviroment...", 0);
  enviroment_graph = disc_algo(length, radius);
  emit to_status_bar("Dicretizing done!", 1000);
  robot_radius = radius;
  algo = Algorithm::Discretization;
  repaint();
}

void MapPanel::exec_build_cells_algo(const double & radius)
{
  BuildingSquareCellsAlgorithm bc_algo(map);
  enviroment_graph.clear();
  min_path.empty();
  emit to_status_bar("Building square cells...", 0);
  enviroment_graph = bc_algo(radius);
  emit to_status_bar("Square cells done!", 1000);
  robot_radius = radius;
  algo = Algorithm::Building_Square_Cells;
  repaint();
}

void MapPanel::exec_quad_tree_algo(const double & radius)
{
  BuildingQuadTreeAlgorithm qt_algo(map);
  enviroment_graph.clear();
  min_path.empty();
  emit to_status_bar("Building quad tree...", 0);
  enviroment_graph = qt_algo(radius);
  emit to_status_bar("Quad tree done!", 1000);
  robot_radius = radius;
  algo = Algorithm::Building_Quad_Tree;
  repaint();
}

void MapPanel::exec_build_visibility_graph_algo(const double & radius)
{
  BuildingVisibilityGraphAlgorithm vg_algo(map);
  enviroment_graph.clear();
  min_path.empty();
  emit to_status_bar("Building visibility graph...", 0);
  enviroment_graph = vg_algo(radius);
  emit to_status_bar("Visibility graph done!", 1000);
  robot_radius = radius;
  algo = Algorithm::Building_Visibility_Graph;
  repaint();
}

const Algorithm & MapPanel::get_algorithm() const
{
  return algo;
}

void MapPanel::compute_min_path()
{
  min_path.empty();
  min_path = MinPathBuilder(map, enviroment_graph)();
  repaint();
}

bool MapPanel::is_there_min_path()
{
  return not min_path.is_empty();
}

void MapPanel::save_enviroment(const QString & fn)
{
  std::string file_name = fn.toStdString();

  std::ofstream file(file_name.c_str());

  if (not file)
    {
      std::stringstream s;
      s << "Cannot create file " << file_name;
      throw std::logic_error(s.str());
    }

  enviroment_graph.save(file);

  file.close();
}

void MapPanel::save_min_path(const QString & fn)
{
  assert(not min_path.is_empty());
  assert(not fn.isEmpty());

  std::string file_name = fn.toStdString();

  std::ofstream file(file_name.c_str());

  if (not file)
    {
      std::stringstream s;
      s << "Cannot create file " << file_name;
      throw std::logic_error(s.str());
    }

  for (DynList<Point>::Iterator it(min_path); it.has_current(); it.next())
    {
      const Point & p = it.get_current();
      file << p.get_x().get_d() << " " << p.get_y().get_d() << std::endl;
    }

  file.close();
}

QString MapPanel::get_info()
{
  assert(algo != Algorithm::Num_Algorithms);
  QString ret_val;

  ret_val.append("Enviroment modeling system for movil robots\n");
  ret_val.append("Generated at: ");
  ret_val.append(QDateTime::currentDateTime().toString());
  ret_val.append("\n\n");
  ret_val.append("Map scale: ");
  ret_val.append(QString().setNum(scale));
  ret_val.append(" pixels / meter\n\n");

  ret_val.append("Robot radius: ");
  ret_val.append(QString().setNum(robot_radius));
  ret_val.append(" m\n\nModeling technique: ");

  switch (algo)
    {
    case Algorithm::Discretization:
      ret_val.append("Discretization\n\n");
      break;
    case Algorithm::Building_Square_Cells:
      ret_val.append("Building square cells\n\n");
      break;
    case Algorithm::Building_Quad_Tree:
      ret_val.append("Building quad tree\n\n");
      break;
    case Algorithm::Building_Visibility_Graph:
      ret_val.append("Visibility graph\n\n");
      break;
    }

  if (min_path.is_empty())
    {
      if (enviroment_graph.beg == nullptr)
        {
          assert(enviroment_graph.end == nullptr);

          ret_val.append("There is not selected mission yet\n");
        }
      else
        {
          assert(enviroment_graph.beg != nullptr and
                 enviroment_graph.end != nullptr);

          ret_val.append("Mission start point: ");
          ret_val.append(enviroment_graph.beg->
                         get_info().position.to_string().c_str());
          ret_val.append("\nMission end point: ");
          ret_val.append(enviroment_graph.end->
                         get_info().position.to_string().c_str());

          ret_val.append("\n\nThere is not computed min path yet\n");
        }
    }
  else
    {
      ret_val.append("Min path for mission (points sequence)\n");

      for (DynList<Point>::Iterator it(min_path); it.has_current(); it.next())
        {
          ret_val.append(it.get_current().to_string().c_str());
          ret_val.append(("\n"));
        }
    }

  return ret_val;
}

void MapPanel::clean()
{
  enviroment_graph.clear();
  min_path.empty();
  algo = Algorithm::Num_Algorithms;
  repaint();
}

void MapPanel::start_select_mission()
{
  mission_status = MissionStatus::Start;
  setMouseTracking(true);
  min_path.empty();
  repaint();
}

