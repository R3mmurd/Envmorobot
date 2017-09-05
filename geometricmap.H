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

# ifndef GEOMETRIC_MAP_H
# define GEOMETRICMAP_H

# include <tpl_dynDlist.H>
# include <point.H>

# include <obstacle.H>

# define CO "/*"
# define WA "WA"
# define OB "OB"
# define DO "DO"
# define NI "NI"
# define NF "NF"

/**
  * \brief Clase que modela un mapa geom&eacute;trico.
  *
  *  Mantiene informaci&oacute;n tal como las paredes, las puertas, los
  *  obst&aacute;culos, inicio y fin de misi&oacute;n.
  *
  * @author Alejandro Mujica
  */
class GeometricMap
{
  DynDlist<Segment> walls_list;

  DynDlist<Segment> doors_list;

  DynDlist<Obstacle> obstacles_list;

  Point mission_begin;

  Point mission_end;

  Geom_Number min_x;

  Geom_Number min_y;

  Geom_Number max_x;

  Geom_Number max_y;

public:
  GeometricMap();

  ~GeometricMap();

  /**
    * Crea un mapa descrito en un archivo con extensi&oacute;n .map.
    * @param file_name Nombre del archivo que se quiere cargar
    * @exception std::logic_error Si el archivo con nombre file_name no existe.
    */
  void load_file(const std::string & file_name);

  DynDlist<Segment> & get_walls_list();

  DynDlist<Segment> & get_doors_list();

  DynDlist<Obstacle> & get_obstacles_list();

  const Point & get_mission_begin();

  const Point & get_mission_end();

  /**
    * El valor de x m&aacute;s peque&ntilde;o de los puntos que conforman el
    * mapa.
    */
  const double get_min_x() const;

  /**
    * El valor de y m&aacute;s peque&ntilde;o de los puntos que conforman el
    * mapa.
    */
  const double get_min_y() const;

  /**
    * El valor de x m&aacute;s grande de los puntos que conforman el mapa.
    */
  const double get_max_x() const;

  /**
    * El valor de y m&aacute;s grande de los puntos que conforman el mapa.
    */
  const double get_max_y() const;

  /**
    * Ancho del mapa dado por la diferencia entre el m&aacute;ximo y el
    * m&iacute;nimo valor de x.
    */
  const double get_width() const;

  /**
    * Largo del mapa dado por la diferencia entre el m&aacute;ximo y el
    * m&iacute;nimo valor de y.
    */
  const double get_height() const;

  void empty();

  /**
    * Determina si un segmento intersecta a alguna de las paredes o a alguno de
    * los obst&aacute;culos.
    * @param s El segmento que se quiere evaluar
    */
  bool intersects_with_segment(const Segment & s);
};

# endif // GEOMETRICMAP_H

