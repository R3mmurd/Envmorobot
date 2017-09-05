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

# include <geometricmap.H>
# include <buffer.H>

# include <string>
# include <fstream>
# include <stdexcept>

# include <QString>

# define MAX_LINE 1024

GeometricMap::GeometricMap()
{
  min_x = min_y =  std::numeric_limits<double>::max();
  max_x = max_y = -std::numeric_limits<double>::max();
}

GeometricMap::~GeometricMap()
{
  // Empty
}

void GeometricMap::load_file(const std::string & file_name)
{
  std::ifstream file(file_name.c_str());

  if (not file)
    throw std::logic_error("Cannot open file");

  empty();

  char __line[MAX_LINE];

  bool there_is_mission_begin = false;
  bool there_is_mission_end = false;

  while (not file.eof())
    {
      file.getline(__line, MAX_LINE);
      if (file.eof())
        break;

      std::string line(__line);

      if (line.empty())
        continue;

      std::string command = line.substr(0, 2);

      if (command == CO)
        continue;
      else if (command == WA or command == DO)
        {
          size_t pos_1, pos_2;

          float x, y;

          pos_1 = line.find('(', 2);
          pos_2 = line.find(',', pos_1);

          x = QString(line.substr(pos_1 + 1,
                                  pos_2 - 1 - pos_1).c_str()).toFloat();
          pos_1 = pos_2;
          pos_2 = line.find(')', pos_1);
          y = QString(line.substr(pos_1 + 1,
                                  pos_2 - 1 - pos_1).c_str()).toFloat();

          Point p1(x, y);

          if (p1.get_x() < min_x)
            min_x = p1.get_x();
          if (p1.get_y() < min_y)
            min_y = p1.get_y();
          if (p1.get_x() > max_x)
            max_x = p1.get_x();
          if (p1.get_y() > max_y)
            max_y = p1.get_y();

          pos_1 = line.find('(', pos_2);
          pos_2 = line.find(',', pos_1);
          x = QString(line.substr(pos_1 + 1,
                                  pos_2 - 1 - pos_1).c_str()).toFloat();
          pos_1 = pos_2;
          pos_2 = line.find(')', pos_1);
          y = QString(line.substr(pos_1 + 1,
                                  pos_2 - 1 - pos_1).c_str()).toFloat();

          Point p2(x, y);

          if (p2.get_x() < min_x)
            min_x = p2.get_x();
          if (p2.get_y() < min_y)
            min_y = p2.get_y();
          if (p2.get_x() > max_x)
            max_x = p2.get_x();
          if (p2.get_y() > max_y)
            max_y = p2.get_y();

          Segment s(p1, p2);

          if (command == WA)
            walls_list.append(s);
          else
            doors_list.append(s);
        }
      else if (command == OB)
        {
          Obstacle o;
          size_t pos_1, pos_2 = 2;
          float x, y;
          while ((pos_1 = line.find('(', pos_2)) != std::string::npos)
            {
              pos_2 = line.find(',', pos_1);
              x = QString(line.substr(pos_1 + 1,
                                      pos_2 - 1 - pos_1).c_str()).toFloat();
              pos_1 = pos_2;
              pos_2 = line.find(')', pos_1);
              y = QString(line.substr(pos_1 + 1,
                                      pos_2 - 1 - pos_1).c_str()).toFloat();
              Point p(x, y);
              o.add_vertex(p);

              if (p.get_x() < min_x)
                min_x = p.get_x();
              if (p.get_y() < min_y)
                min_y = p.get_y();
              if (p.get_x() > max_x)
                max_x = p.get_x();
              if (p.get_y() > max_y)
                max_y = p.get_y();
            }
          o.close();
          obstacles_list.append(o);
        }
      else if (command == NI or command == NF)
        {
          size_t pos_1, pos_2;
          float x, y;
          pos_1 = line.find('(', 2);
          pos_2 = line.find(',', pos_1);
          x = QString(line.substr(pos_1 + 1,
                                  pos_2 - 1 - pos_1).c_str()).toFloat();
          pos_1 = pos_2;
          pos_2 = line.find(')', pos_1);
          y = QString(line.substr(pos_1 + 1,
                                  pos_2 - 1 - pos_1).c_str()).toFloat();

          Point p(x, y);

          if (p.get_x() < min_x)
            min_x = p.get_x();
          if (p.get_y() < min_y)
            min_y = p.get_y();
          if (p.get_x() > max_x)
            max_x = p.get_x();
          if (p.get_y() > max_y)
            max_y = p.get_y();

          if (command == NI)
            {
              if (not there_is_mission_begin)
                {
                  mission_begin = p;
                  there_is_mission_begin = true;
                }
              else
                throw std::logic_error("There is an mission begin already");
            }
          else
            {
              if (not there_is_mission_end)
                {
                  mission_end = p;
                  there_is_mission_end = true;
                }
              else
                throw std::logic_error("There is an mission end already");
            }
        }
      else
        throw std::logic_error("Invalid command");
    }
  file.close();
}

DynDlist<Segment> & GeometricMap::get_walls_list()
{
  return walls_list;
}

DynDlist<Segment> & GeometricMap::get_doors_list()
{
  return doors_list;
}

DynDlist<Obstacle> & GeometricMap::get_obstacles_list()
{
  return obstacles_list;
}

const Point & GeometricMap::get_mission_begin()
{
  return mission_begin;
}

const Point & GeometricMap::get_mission_end()
{
  return mission_end;
}

const double GeometricMap::get_min_x() const
{
  return min_x.get_d();
}

const double GeometricMap::get_min_y() const
{
  return min_y.get_d();
}

const double GeometricMap::get_max_x() const
{
  return max_x.get_d();
}

const double GeometricMap::get_max_y() const
{
  return max_y.get_d();
}

const double GeometricMap::get_width() const
{
  return get_max_x() - get_min_x();
}

const double GeometricMap::get_height() const
{
  return get_max_y() - get_min_y();
}

void GeometricMap::empty()
{
  min_x = min_y = std::numeric_limits<double>::max();
  max_x = max_y = -std::numeric_limits<double>::max();
  walls_list.empty();
  doors_list.empty();
  obstacles_list.empty();
  Buffer::get_instance()->clear();
}

bool GeometricMap::intersects_with_segment(const Segment & s)
{
  for (DynDlist<Segment>::Iterator it(walls_list); it.has_current(); it.next())
    {
      Segment & current_segment = it.get_current();
      if (s.intersects_properly_with(current_segment))
        return true;
    }

  for (DynDlist<Obstacle>::Iterator it(obstacles_list); it.has_current();
       it.next())
    {
      Obstacle & current_obstacle = it.get_current();
      if (current_obstacle.intersects_properly_with(s))
        return true;
    }

  return false;
}

