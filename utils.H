/*
  Envmorobot.

  Author: Alejandro Mujica (aledrums@gmail.com)
*/

# ifndef UTILS_H
# define UTILS_H

# include <obstacle.H>

# include <QPointF>
# include <QRectF>
# include <QLineF>

# include <cmath>

# include <iostream>

# include <tpl_euclidian_graph.H>

# include <buffer.H>

# define A_ZERO 0
# define A_PI_2 M_PI_2
# define A_3_PI_2 (3 * A_PI_2)
# define A_PI_DEGREES 180
# define A_2_PI_DEGREES A_PI_DEGREES * 2
# define A_PI_2_DEGREES A_PI_DEGREES / 2
# define RADIANS_2_DEGREES(t) (((t) * A_PI_DEGREES) / M_PI)
# define DEGREES_2_RADIANS(t) (((t) * M_PI) / A_PI_DEGREES)

/**
  * Calcula el &aacute;ngulo entre dos puntos de tipo Point.
  */
inline double compute_angle_between(const Point & p1, const Point & p2)
{
  if (p1.get_x() == p2.get_x())
    if (p1.get_y() > p2.get_y())
      return RADIANS_2_DEGREES(A_3_PI_2);
    else if (p1.get_y() < p2.get_y())
      return RADIANS_2_DEGREES(A_PI_2);
    else
      return A_ZERO;

  double m = double(p2.get_y().get_d() - p1.get_y().get_d()) /
             double(p2.get_x().get_d() - p1.get_x().get_d());

  double angle = RADIANS_2_DEGREES(std::atan(m));

  if (p1.get_x() > p2.get_x())
    return A_PI_DEGREES + angle;

  return angle;
}


/** Construye el obst&aacute;culo extendido de un Obstacle.
  *
  * @param obstacle Obst&aacute;culo original
  * @param radius Radio del robot
  * @param e_obstacle Obst&aacute;culo extendido resultante.
  */
inline Obstacle build_extended_obstacle(Obstacle & obstacle,
                                        const double & radius)
{
  Obstacle e_obstacle;

  DynDlist<Segment> segments;

  int sign = obstacle.is_clockwise() ? 1 : -1;

  for (Obstacle::Segment_Iterator it(obstacle); it.has_current(); it.next())
    {
      const Segment & s = it.get_current_segment();
      Point p1 = s.get_src_point();
      Point p2 = s.get_tgt_point();

      double angle = compute_angle_between(p1, p2);
      const double s_angle = RADIANS_2_DEGREES(A_PI_2) - angle;

      Point _p1(p1.get_x().get_d() +
                sign * radius * std::cos(DEGREES_2_RADIANS(s_angle)),
                p1.get_y().get_d() -
                sign * radius * std::sin(DEGREES_2_RADIANS(s_angle)));
      Point _p2(p2.get_x().get_d() +
                sign * radius * std::cos(DEGREES_2_RADIANS(s_angle)),
                p2.get_y().get_d() -
                sign * radius * std::sin(DEGREES_2_RADIANS(s_angle)));
      segments.append(Segment(_p1, _p2));
    }

  Segment s1, s2;
  DynDlist<Segment>::Iterator it2(segments);
  s1 = it2.get_current();
  it2.next();

  for ( ; it2.has_current(); it2.next())
    {
      s2 = it2.get_current();
      Point p = s1.intersection_with(s2);
      e_obstacle.add_vertex(p);
      s1 = s2;
    }

  it2.reset_first();
  s2 = it2.get_current();
  e_obstacle.add_vertex(s1.intersection_with(s2));
  e_obstacle.close();

  return e_obstacle;
}

/** Construye la pared extendida de un Wall.
  *
  * @param wall Pared original
  * @param radius Radio del robot
  * @param e_wall Pared extendida resultante.
  */
inline Obstacle build_extended_wall(const Segment & wall,
                                    const double & radius)
{
  Obstacle e_wall;

  const Point & _p1 = wall.get_src_point();
  const Point & _p2 = wall.get_tgt_point();

  const double angle = compute_angle_between(_p1, _p2);
  const double s_angle = RADIANS_2_DEGREES(A_PI_2) - angle;

  Point p1(_p1.get_x().get_d() -
           radius * std::cos(DEGREES_2_RADIANS(angle)) -
           radius * std::cos(DEGREES_2_RADIANS(s_angle)),
           _p1.get_y().get_d() -
           radius * std::sin(DEGREES_2_RADIANS(angle)) +
           radius * std::sin(DEGREES_2_RADIANS(s_angle)));

  e_wall.add_vertex(p1);

  Point p2(_p2.get_x().get_d() +
           radius * std::cos(DEGREES_2_RADIANS(angle)) -
           radius * std::cos(DEGREES_2_RADIANS(s_angle)),
           _p2.get_y().get_d() +
           radius * std::sin(DEGREES_2_RADIANS(angle)) +
           radius * std::sin(DEGREES_2_RADIANS(s_angle)));

  e_wall.add_vertex(p2);

  Point p3(_p2.get_x().get_d() +
           radius * std::cos(DEGREES_2_RADIANS(angle)) +
           radius * std::cos(DEGREES_2_RADIANS(s_angle)),
           _p2.get_y().get_d() +
           radius * std::sin(DEGREES_2_RADIANS(angle)) -
           radius * std::sin(DEGREES_2_RADIANS(s_angle)));

  e_wall.add_vertex(p3);

  Point p4(_p1.get_x().get_d() -
           radius * std::cos(DEGREES_2_RADIANS(angle)) +
           radius * std::cos(DEGREES_2_RADIANS(s_angle)),
           _p1.get_y().get_d() -
           radius * std::sin(DEGREES_2_RADIANS(angle)) -
           radius * std::sin(DEGREES_2_RADIANS(s_angle)));

  e_wall.add_vertex(p4);

  e_wall.close();

  return e_wall;
}

/** Determina si una pared se intersecta con una celda
  *
  * @param wall Pared a verificar
  * @param p Punto central de la celda
  * @param x_radius Radio horizontal de la celda
  * @param y_radius Radio vertical de la celda
  */
inline bool intersects_wall_with_cell(const Segment & wall, const Point & p,
                                      const double & x_radius,
                                      const double & y_radius)
{
  Obstacle square;

  square.add_vertex(Point(p.get_x() - x_radius, p.get_y() - y_radius));
  square.add_vertex(Point(p.get_x() + x_radius, p.get_y() - y_radius));
  square.add_vertex(Point(p.get_x() + x_radius, p.get_y() + y_radius));
  square.add_vertex(Point(p.get_x() - x_radius, p.get_y() + y_radius));

  square.close();

  return square.intersects_with(wall);
}

/** Determina si un obst&aacute;culo se intersecta con una celda o si la celda
  * est&aacute; dentro del obst&aacute;culo.
  *
  * @param obstacle Obst&aacute;culo a verificar
  * @param p Punto central de la celda
  * @param x_radius Radio horizontal de la celda
  * @param y_radius Radio vertical de la celda
  */
inline bool intersects_obstacle_with_cell(Obstacle & obstacle, const Point & p,
                                          const double & x_radius,
                                          const double & y_radius)
{
  Point p1(p.get_x() - x_radius, p.get_y() - y_radius);
  Point p2(p.get_x() + x_radius, p.get_y() - y_radius);
  Point p3(p.get_x() + x_radius, p.get_y() + y_radius);
  Point p4(p.get_x() - x_radius, p.get_y() + y_radius);

  if (obstacle.contains(p1) or obstacle.contains(p2) or
      obstacle.contains(p3) or obstacle.contains(p4))
    return true;

  Segment s1(p1, p2);
  Segment s2(p2, p3);
  Segment s3(p3, p4);
  Segment s4(p4, p1);

  if (obstacle.intersects_with(s1) or obstacle.intersects_with(s2) or
      obstacle.intersects_with(s3) or obstacle.intersects_with(s4))
    return true;

  return false;
}

/** Determina si dos celdas se intersectan.
  *
  * @param p1 Punto central de la celda 1
  * @param rx1 Radio horizontal de la celda 1
  * @param ry1 Radio vertical de la celda 1
  * @param p2 Punto central de la celda 2
  * @param rx2 Radio horizontal de la celda 2
  * @param ry2 Radio vertical de la celda 2
  */
inline bool intersect_cells(const Point & p1,
                            const double & rx1, const double & ry1,
                            const Point & p2,
                            const double & rx2, const double & ry2)
{
  Obstacle obstacle;
  obstacle.add_vertex(Point(p2.get_x() - rx2, p2.get_y() - ry2));
  obstacle.add_vertex(Point(p2.get_x() + rx2, p2.get_y() - ry2));
  obstacle.add_vertex(Point(p2.get_x() + rx2, p2.get_y() + ry2));
  obstacle.add_vertex(Point(p2.get_x() - rx2, p2.get_y() + ry2));
  obstacle.close();

  return intersects_obstacle_with_cell(obstacle, p1, rx1, ry1);
}

/** Determina si un punto est&aacute; dentro de alg&uacute;n pol&iacute;gono de
  * las listas.
  *
  * @param p Punto a probar
  * @param walls Lista de paredes
  * @param obstacles Lista de obst&aacute;culos
  * @param radius Radio del robot.
  */
inline bool is_point_inside_some_polygon(const Point & p,
                                         DynDlist<Segment> & walls,
                                         DynDlist<Obstacle> & obstacles,
                                         const double & radius)
{
  for (DynDlist<Segment>::Iterator it(walls); it.has_current(); it.next())
    {
      Segment & w = it.get_current();
      Obstacle & ew =
        const_cast<Obstacle &>(
          Buffer::get_instance()->get_extended_wall(w, radius)
        );
      if (ew.contains(p, false))
        return true;
    }

  for (DynDlist<Obstacle>::Iterator it(obstacles); it.has_current(); it.next())
    {
      Obstacle & o = it.get_current();
      Obstacle & eo =
        const_cast<Obstacle &>(
          Buffer::get_instance()->get_extended_obstacle(o, radius)
        );
      if (eo.contains(p, false))
        return true;
    }

  return false;
}

inline bool is_segment_intersected_with_some_polygon(
  const Segment & s, DynDlist<Segment> & walls, DynDlist<Obstacle> & obstacles,
  const double & radius)
{
  for (DynDlist<Segment>::Iterator it(walls); it.has_current(); it.next())
    {
      Segment & w = it.get_current();
      Obstacle & ew =
        const_cast<Obstacle &>(
          Buffer::get_instance()->get_extended_wall(w, radius)
        );
      if (ew.intersects_properly_with(s) or w.intersects_properly_with(s))
        return true;
    }

  for (DynDlist<Obstacle>::Iterator it(obstacles); it.has_current(); it.next())
    {
      Obstacle & o = it.get_current();
      Obstacle & eo =
        const_cast<Obstacle &>(
          Buffer::get_instance()->get_extended_obstacle(o, radius)
        );
      if (eo.intersects_properly_with(s) or o.intersects_properly_with(s))
        return true;
    }

  return false;
}

inline bool is_segment_intersected_with_some_segment(const Segment & s,
                                                     DynDlist<Segment> & walls)
{
  for (DynDlist<Segment>::Iterator it(walls); it.has_current(); it.next())
    {
      Segment & w = it.get_current();
      if (w.intersects_properly_with(s))
        return true;
    }
  return false;
}

template <class GT>
struct DefaultOperationOnNode
{
  void operator () (GT &, typename GT::Node *, const size_t &, const size_t &)
  {
    // Empty
  }
};

template <class GT>
struct DefaultOperationOnArc
{
  void operator () (GT &, typename GT::Arc *, const size_t &, const size_t &)
  {
    // Empty
  }
};

 /** \brief Clase que construye un grafo en forma de Malla 2D con
   * topolog&iacute;a cuadrada
   *
   * La clase recibe los siguientes par&aacute;metros tipo:
   * <ol>
   * <li> GT: el tipo de grafo basado en List_Graph de la biblioteta <a href="http://webdelprofesor.ula.ve/ingenieria/lrleon/aleph/html/index.html">Aleph-w</a>
   * <li> Operation_On_Node: La clase de operaci&oacute;n sobre los nodos que
   * debe exportar lo siguiente
   *    <ul>
   *    <li> Operation_On_Node::operator()(GT & g, typename GT::Node * node,
  *          const size_t & i, const size_t & j) que retorna void.
   *    </ul>
   *    .
   * <li> Operation_On_Arc: La clase de operaci&oacute;n sobre los arcos que
   *      debe exportar lo siguiente
   *    <ul>
   *    <li> Operation_On_Arc::operator()(GT & g, typename GT::Arc * arc,
   *         const size_t & i, const size_t & j) que retorna void.
   *    </ul>
   * </ol>
   * @author Alejandro Mujica
   */
template <
    class GT,
    class OperationOnNode = DefaultOperationOnNode<GT>,
    class OperationOnArc = DefaultOperationOnArc<GT>
    >
class GridBuilder
{
  OperationOnNode & op_node;

  OperationOnArc & op_arc;

public:
  GridBuilder(OperationOnNode & _op_node, OperationOnArc & _op_arc)
    : op_node(_op_node), op_arc(_op_arc)
  {
    // Empty
  }

  GridBuilder(OperationOnNode && _op_node = OperationOnNode(),
               OperationOnArc && _op_arc = OperationOnArc())
    : op_node(_op_node), op_arc(_op_arc)
  {
    // Empty
  }

  GT operator () (const size_t & width, const size_t & height)
  {
    GT g;

    if (width < 2 or height < 2)
      throw std::length_error("The minimun size must be 2 x 2");

    typename GT::Node *** map = new typename GT::Node **[height];
    for (size_t i = 0; i < height; ++i)
      {
        try
        {
          map[i] = new typename GT::Node *[width];
          for (size_t j = 0; j < width; ++j)
            {
              typename GT::Node * n = g.insert_node(typename GT::Node_Type());
              op_node(g, n, i, j);
              map[i][j] = n;
              if (j > 0) //Conecta con el nodo a su izquierda
                {
                  typename GT::Arc * a = g.insert_arc(n, map[i][j - 1]);
                  op_arc(g, a, i, j);
                }
              if (i > 0) //Conecta con el nodo que esta arriba
                {
                  typename GT::Arc * a = g.insert_arc(n, map[i - 1][j]);
                  op_arc(g, a, i, j);
                }
            }
        }
        catch (...)
        {
          // Si se captura excepcion libero la memoria apartada hasta ahora y la
          // relanzo
          for (size_t k = i - 1; k >= 0; --k)
            delete [] map[k];
          delete [] map;
          clear_graph(g);
          throw;
        }
      }

    for (size_t i = 0; i < height; ++i)
      delete [] map[i];
    delete [] map;

    return g;
  }
};

# endif // UTILS_H

