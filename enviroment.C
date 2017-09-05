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

# include <enviroment.H>

# include <geometricmap.H>
# include <buffer.H>

# include <Dijkstra.H>
# include <tpl_components.H>

const Distance::Distance_Type Distance::Max_Distance =
  Geom_Number(std::numeric_limits<double>::max());

const Distance::Distance_Type Distance::Zero_Distance = Geom_Number(0.0);

bool EnviromentGraph::node_belong_to_graph(EnviromentGraph::Node * n)
{
  for (Node_Iterator it(*this); it.has_current(); it.next())
    {
      Node * curr = it.get_current();
      if (curr == n)
        return true;
    }
  return false;
}

EnviromentGraph::Node * EnviromentGraph::get_closest_node(const Point & p)
{
  Node_Iterator it(*this);
  Node * closest = it.get_curr();
  it.next();

  for ( /* nothing */ ; it.has_curr(); it.next())
    {
      Node * curr = it.get_curr();

      if (curr->get_info().position.distance_with(p) <
          closest->get_info().position.distance_with(p))
        closest = curr;
    }

  return closest;
}

void EnviromentGraph::set_beg_node(const Point & p, bool new_node, double rad,
                                   GeometricMap & map)
{
  if (new_node)
    {
      if (beg != nullptr)
        remove_node(beg);

      beg = insert_node();
      beg->get_info().position = p;
      IndexArc<EnviromentGraph> idx(*this);
      beg->get_info().available =
          BuildingVisibilityGraphAlgorithm(map).connect_node(*this, beg, idx,
                                                             rad);
    }
  else
    beg = get_closest_node(p);
}

void EnviromentGraph::set_end_node(const Point & p, bool new_node, double rad,
                                   GeometricMap & map)
{
  if (new_node)
    {
      if (end != nullptr)
        remove_node(end);
      end = insert_node();
      end->get_info().position = p;
      IndexArc<EnviromentGraph> idx(*this);
      end->get_info().available =
          BuildingVisibilityGraphAlgorithm(map).connect_node(*this, end, idx,
                                                             rad);
    }
  else
    end = get_closest_node(p);
}

void EnviromentGraph::clear()
{
  beg = end = nullptr;
  clear_graph(*this);
}

void EnviromentGraph::save(ofstream & o)
{  
  size_t num_available_nodes = 0;

  for (Node_Iterator it(*this); it.has_curr(); it.next())
    if (it.get_curr()->get_info().available)
      ++num_available_nodes;

  o << num_available_nodes << std::endl;

  DynMapTree<Node *, size_t, Treap> map_node_pos;

  size_t i = 0;

  for (Node_Iterator it(*this); it.has_curr(); it.next())
    {
      Node * p = it.get_curr();

      if (not p->get_info().available)
        continue;

      map_node_pos[p] = i++;

      o << p->get_info().position.get_x().get_d() << ' '
        << p->get_info().position.get_y().get_d() << ' '
        << p->get_info().level_length_rel << std::endl;
    }

  o << get_num_arcs() << std::endl;

  for (Arc_Iterator it(*this); it.has_curr(); it.next())
    {
      Arc * a = it.get_curr();
      Node * s = get_src_node(a);
      Node * t = get_tgt_node(a);

      if (not s->get_info().available or not t->get_info().available)
        continue;

      o << map_node_pos[s] << ' ' << map_node_pos[t] << std::endl;
    }
}

DynList<Point> MinPathBuilder::operator ()()
{
  if (graph.beg == nullptr)
    throw std::logic_error("There is not selected start node");

  if (graph.end == nullptr)
    throw std::logic_error("There is not selected end node");

  Path<EnviromentGraph> temp_path(graph);

  auto c = Dijkstra_Min_Paths<EnviromentGraph, Distance>()(graph, graph.beg,
                                                           graph.end,
                                                           temp_path);

  if (c == 0)
    throw std::logic_error("There is not path between start and end node");

  DynList<Point> min_path;

  for (Path<EnviromentGraph>::Iterator it(temp_path); it.has_curr(); it.next())
    {
      Point cp = it.get_current_node()->get_info().position;
      min_path.append(cp);
    }

  return min_path;
}

EnviromentGraph DiscretizationAlgorithm::operator () (double d, double radius)
{
  double map_width = map.get_width();
  double map_height = map.get_height();

  if (d > map_width or d > map_height)
    throw std::logic_error("Distance between points too large");

  DiscretizeNodeOp dop;
  dop.length = d;
  dop.min_x = map.get_min_x();
  dop.min_y = map.get_min_y();

  const size_t width = (map_width / dop.length) + 1;
  const size_t height = (map_height / dop.length) + 1;

  GridBuilder<EnviromentGraph, DiscretizeNodeOp> gb(std::move(dop));

  EnviromentGraph ret = gb(width, height);

  for (DynDlist<Segment>::Iterator w_it(map.get_walls_list());
       w_it.has_current(); w_it.next())
    {
      Segment & wall = w_it.get_current();
      Obstacle & e_wall = const_cast<Obstacle &>(
        Buffer::get_instance()->get_extended_wall(wall, radius)
      );

      for (EnviromentGraph::Node_Iterator n_it(ret); n_it.has_curr();
           n_it.next())
        {
          EnviromentGraph::Node * node = n_it.get_current();
          Point p = node->get_info().position;

          if (e_wall.contains(p))
            {
              for (EnviromentGraph::Node_Arc_Iterator it3(node);
                   it3.has_curr(); )
                {
                  EnviromentGraph::Arc * arc = it3.get_current();
                  it3.next();
                  ret.remove_arc(arc);
                }
              node->get_info().available = false;
            }
        }

      for (EnviromentGraph::Arc_Iterator a_it(ret); a_it.has_curr(); )
        {
          EnviromentGraph::Arc * arc = a_it.get_current();
          Segment s(ret.get_src_node(arc)->get_info().position,
                    ret.get_tgt_node(arc)->get_info().position);

          a_it.next();

          if (e_wall.intersects_with(s))
            ret.remove_arc(arc);
        }
    }

  for (DynDlist<Obstacle>::Iterator o_it(map.get_obstacles_list());
       o_it.has_current(); o_it.next())
    {
      Obstacle & obstacle = o_it.get_current();
      Obstacle & e_ostacle = const_cast<Obstacle &>(
        Buffer::get_instance()->get_extended_obstacle(obstacle, radius)
      );

      for (EnviromentGraph::Node_Iterator n_it(ret); n_it.has_curr();
           n_it.next())
        {
          EnviromentGraph::Node * node = n_it.get_current();
          Point p = node->get_info().position;

          if (e_ostacle.contains(p))
            {
              for (EnviromentGraph::Node_Arc_Iterator it3(node);
                  it3.has_curr(); )
                {
                  EnviromentGraph::Arc * arc = it3.get_current();
                  it3.next();
                  ret.remove_arc(arc);
                }
              node->get_info().available = false;
            }
        }
      for (EnviromentGraph::Arc_Iterator a_it(ret); a_it.has_current(); )
        {
          EnviromentGraph::Arc * arc = a_it.get_current();
          Segment s(ret.get_src_node(arc)->get_info().position,
                    ret.get_tgt_node(arc)->get_info().position);

          a_it.next();

          if (e_ostacle.intersects_with(s))
            ret.remove_arc(arc);
        }
    }

  return ret;
}

EnviromentGraph BuildingSquareCellsAlgorithm::operator () (double radius)
{
  double map_width = map.get_width();
  double map_height = map.get_height();
  double diameter = 2 * radius;

  if (diameter > map_width or diameter > map_height)
    throw std::logic_error("Radius too large");

  CellsNodeOp cop;
  cop.length = diameter;
  cop.min_x = map.get_min_x();
  cop.min_y = map.get_min_y();
  cop.radius = radius;

  size_t width = (map_width / cop.length) + 1;
  size_t height = (map_height / cop.length) + 1;

  GridBuilder<EnviromentGraph, CellsNodeOp> gb(std::move(cop));

  EnviromentGraph ret = gb(width, height);

  for (DynDlist<Segment>::Iterator w_it(map.get_walls_list());
       w_it.has_current(); w_it.next())
    {
      Segment & wall = w_it.get_current();
      for (EnviromentGraph::Node_Iterator n_it(ret);
           n_it.has_current(); n_it.next())
        {
          EnviromentGraph::Node * node = n_it.get_current();
          Point p = node->get_info().position;
          if (intersects_wall_with_cell(wall, p, radius, radius))
            {
              for (EnviromentGraph::Node_Arc_Iterator it3(node);
                   it3.has_current(); )
                {
                  EnviromentGraph::Arc * arc = it3.get_current();
                  it3.next();
                  ret.remove_arc(arc);
                }
              node->get_info().available = false;
            }
        }
    }

  for (DynDlist<Obstacle>::Iterator o_it(map.get_obstacles_list());
       o_it.has_current(); o_it.next())
    {
      Obstacle & obstacle = o_it.get_current();

      for (EnviromentGraph::Node_Iterator n_it(ret); n_it.has_current();
           n_it.next())
        {
          EnviromentGraph::Node * node = n_it.get_current();
          Point p = node->get_info().position;

          if (intersects_obstacle_with_cell(obstacle, p, radius, radius))
            {
              for (EnviromentGraph::Node_Arc_Iterator it3(node);
                   it3.has_current(); )
                {
                  EnviromentGraph::Arc * arc = it3.get_current();
                  it3.next();
                  ret.remove_arc(arc);
                }
              node->get_info().available = false;
            }
        }
    }

  return ret;
}

void BuildingQuadTreeAlgorithm::cut(const Point & p, double w, double h,
                                    Point ap[4])
{
  double w_2 = w / 2;
  double h_2 = h / 2;

  ap[0] = Point(p.get_x() - w_2, p.get_y() - h_2);
  ap[1] = Point(p.get_x() + w_2, p.get_y() - h_2);
  ap[2] = Point(p.get_x() - w_2, p.get_y() + h_2);
  ap[3] = Point(p.get_x() + w_2, p.get_y() + h_2);
}

void BuildingQuadTreeAlgorithm::insert_points_in_quad_tree(const Point & p,
                                                           double w, double h,
                                                           double d,
                                                           QuadTree & tree)
{
  if (w < d or h < d)
    return;

  tree.insert(p);

  const double w_2 = w / 2.0;
  const double h_2 = h / 2.0;

  if (not is_busy(p, w_2, h_2) or w_2 < d or h_2 < d)
    return;

  tree.remove(p);

  Point ap[4];
  cut(p, w_2, h_2, ap);
  insert_points_in_quad_tree(ap[0], w_2, h_2, d, tree);
  insert_points_in_quad_tree(ap[1], w_2, h_2, d, tree);
  insert_points_in_quad_tree(ap[2], w_2, h_2, d, tree);
  insert_points_in_quad_tree(ap[3], w_2, h_2, d, tree);
}

bool BuildingQuadTreeAlgorithm::is_busy(const Point & p, double w, double h)
{
  DynDlist<Segment> & walls = map.get_walls_list();

  for (DynDlist<Segment>::Iterator w_it(walls); w_it.has_current(); w_it.next())
    {
      Segment & wall = w_it.get_current();
      if (intersects_wall_with_cell(wall, p, w, h))
        return true;
    }

  DynDlist<Obstacle> & obstacles = map.get_obstacles_list();

  for (DynDlist<Obstacle>::Iterator o_it(obstacles); o_it.has_current();
       o_it.next())
    {
      Obstacle & obstacle = o_it.get_current();
      if (intersects_obstacle_with_cell(obstacle, p, w, h))
        return true;
    }
  return false;
}

EnviromentGraph BuildingQuadTreeAlgorithm::operator () (double radius)
{
  double diameter = radius * 2;

  double width = map.get_width();
  double height = map.get_height();

  if (diameter > width or diameter > height)
    throw std::logic_error("Radius too large");

  QuadTree tree(map.get_min_x(), map.get_max_x(),
                map.get_min_y(), map.get_max_y());

  double w_center = width / 2 + map.get_min_x();
  double h_center = height / 2 + map.get_min_y();

  Point p(w_center, h_center);

  insert_points_in_quad_tree(p, width, height, diameter, tree);

  EnviromentGraph ret;

  IndexArc<EnviromentGraph> idx(ret);

  MapGnodeQtreenode map_gnode_qtreenode;
  MapQtreenodeGnode map_qtreenode_gnode;

  tree.for_each([&](QuadTree::Node * qtreenode)
    {
      if (not qtreenode->is_leaf())
        return;

      const Point & p = qtreenode->get_points_set().get_first();

      EnviromentGraph::Node * gnode = ret.insert_node();

      gnode->get_info().position = p;

      const double level_length_rel = std::pow(2, LEVEL(qtreenode));

      gnode->get_info().level_length_rel = level_length_rel;

      gnode->get_info().available =
        not is_busy(p, width / (2 * level_length_rel),
                       height / (2 * level_length_rel));

      map_gnode_qtreenode.insert(gnode, qtreenode);
      map_qtreenode_gnode.insert(qtreenode, gnode);
    });

  for (EnviromentGraph::Node_Iterator it(ret); it.has_current(); it.next())
    {
      EnviromentGraph::Node * curr_node = it.get_current();

      if (not curr_node->get_info().available)
        continue;

      QuadTree::Node * qtree_node = map_gnode_qtreenode.find(curr_node);

      DynList<QuadTree::Node *> neighbors = qtree_node->get_neighbors();

      for (DynList<QuadTree::Node *>::Iterator qit(neighbors);
           qit.has_current(); qit.next())
        {
          QuadTree::Node * curr_qtree_node = qit.get_current();

          EnviromentGraph::Node * tgt =
              map_qtreenode_gnode.find(curr_qtree_node);

          if (not tgt->get_info().available)
            continue;

          if (idx.search(curr_node, tgt) != nullptr)
            continue;

          const Point & srcp = curr_node->get_info().position;
          const Point & tgtp = tgt->get_info().position;

          if (is_segment_intersected_with_some_segment(Segment(srcp, tgtp),
              map.get_walls_list()))
            continue;

          idx.insert(ret.insert_arc(curr_node, tgt));
        }
    }

  return ret;
}

bool
BuildingVisibilityGraphAlgorithm::connect_node(EnviromentGraph & g,
                                               EnviromentGraph::Node * u,
                                               IndexArc<EnviromentGraph> & idx,
                                               double radius)
{
  DynDlist<Segment> & walls = map.get_walls_list();
  DynDlist<Obstacle> & obstacles = map.get_obstacles_list();

  if (is_point_inside_some_polygon(u->get_info().position, walls, obstacles,
                                   radius))
    return false;

  for (EnviromentGraph::Node_Iterator nit(g); nit.has_curr(); nit.next())
    {
      EnviromentGraph::Node * v = nit.get_curr();

      if (u == v)
        continue;

      if (is_point_inside_some_polygon(v->get_info().position, walls,
                                       obstacles, radius))
        continue;

      if (idx.search(u, v) != nullptr or idx.search(v, u) != nullptr)
        continue;

      if (is_segment_intersected_with_some_polygon(
            Segment(u->get_info().position, v->get_info().position), walls,
            obstacles, radius)
         )
        continue;

      idx.insert(g.insert_arc(u, v));
    }

  return true;
}

EnviromentGraph BuildingVisibilityGraphAlgorithm::operator () (double radius)
{
  EnviromentGraph ret;

  DynDlist<Segment> & walls = map.get_walls_list();
  DynDlist<Obstacle> & obstacles = map.get_obstacles_list();

  for (DynDlist<Segment>::Iterator w_it(walls); w_it.has_current(); w_it.next())
    {
      Segment & wall = w_it.get_current();

      Obstacle & e_wall = const_cast<Obstacle &>(
        Buffer::get_instance()->get_extended_wall(wall, radius + 0.01)
      );

      for (Obstacle::Vertex_Iterator it(e_wall); it.has_current(); it.next())
        {
          Point & p = it.get_current_vertex();

          if (p.get_x() < map.get_min_x() or p.get_x() > map.get_max_x() or
              p.get_y() < map.get_min_y() or p.get_y() > map.get_max_y())
            continue;

          EnviromentGraph::Node * n = ret.insert_node();
          n->get_info().position = p;
          n->get_info().available = true;
        }
    }

  for (DynDlist<Obstacle>::Iterator o_it(obstacles); o_it.has_current();
       o_it.next())
    {
      Obstacle & obstacle = o_it.get_current();

      Obstacle & e_obstacle = const_cast<Obstacle &>(
        Buffer::get_instance()->get_extended_obstacle(obstacle, radius + 0.01)
      );

      for (Obstacle::Vertex_Iterator it(e_obstacle); it.has_current();
           it.next())
        {
          Point & p = it.get_current_vertex();

          if (p.get_x() < map.get_min_x() or p.get_x() > map.get_max_x() or
              p.get_y() < map.get_min_y() or p.get_y() > map.get_max_y())
            continue;

          EnviromentGraph::Node * n = ret.insert_node();
          n->get_info().position = p;
          n->get_info().available = true;
        }

    }

  IndexArc<EnviromentGraph> idx(ret);

  for (EnviromentGraph::Node_Iterator it(ret); it.has_curr(); it.next())
    connect_node(ret, it.get_curr(), idx, radius);

  return ret;
}
