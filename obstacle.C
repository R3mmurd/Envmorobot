/*
  Envmorobot.

  Author: Alejandro Mujica (aledrums@gmail.com)
*/

# include <obstacle.H>

# include <cmath>

Obstacle::Obstacle()
{
  // Empty
}

Obstacle::~Obstacle()
{
  // Empty
}

bool Obstacle::contains(const Point & p, bool use_borders)
{
  if (size() < 3)
    throw std::domain_error("this has only 2 Vertexs");

  Segment_Iterator it(*this);
  Segment s = it.get_current_segment();
  it.next();
  bool test = p.is_to_right_from(s) or (p.is_inside(s) and use_borders);

  for( ; it.has_current(); it.next())
    {
      s = it.get_current_segment();
      if ((p.is_to_right_from(s) or (p.is_inside(s) and use_borders)) != test)
        return false;
    }

  return true;
}

std::string Obstacle::to_string()
{
  std::string ret_val;

  for(Vertex_Iterator it(*this); it.has_current(); it.next())
    ret_val += it.get_current_vertex().to_string() + ";";

  return ret_val;
}

double Obstacle::compute_area()
{
  if (size() < 3)
    return 0;

  double ret = 0;

  for (Segment_Iterator it(*this); it.has_current(); it.next())
    {
      Segment s = it.get_current_segment();

      const Vertex & v1 = s.get_src_point();
      const Vertex & v2 = s.get_tgt_point();

      ret += Geom_Number(v1.get_x() * v2.get_y() -
                         v1.get_y() * v2.get_x()).get_d();
    }

  return ret / 2;
}

double Obstacle::area()
{
  double area = compute_area();
  return std::abs(area);
}

Obstacle::Orientation Obstacle::orientation()
{
  double area = compute_area();
  if (area < 0)
    return Counter_Clockwise;
  if (area > 0)
    return Clockwise;
  return Collinear;
}

bool Obstacle::is_clockwise()
{
  return orientation() == Clockwise;
}

void Obstacle::intersections_with(Obstacle & obstacle, DynList<Point> & points)
{
  for (Segment_Iterator it1(*this); it1.has_current(); it1.next())
    {
      Segment s1 = it1.get_current_segment();
      for (Segment_Iterator it2(obstacle); it2.has_current(); it2.next())
        {
          Segment s2 = it2.get_current_segment();

          if (not s2.intersects_properly_with(s1))
            continue;
          try
          {
            Point p = s1.intersection_with(s2);
            points.append(p);
          }
          catch(...)
          {
            // Nothing to do
          }
        }
    }
}

bool Obstacle::intersects_properly_with(const Segment & segment)
{
  for (Segment_Iterator it(*this); it.has_current(); it.next())
    {
      const Segment & curr_segment = it.get_current_segment();
      if (segment.intersects_properly_with(curr_segment))
        return true;
    }
  return false;
}
