/*
  Envmorobot.

  Author: Alejandro Mujica (aledrums@gmail.com)
*/

# ifndef OBSTACLE_H
# define OBSTACLE_H

# include <htlist.H>

# include <polygon.H>

/**
  * \brief Clase que modela un obst&aacute;culo geom&eacute;trico.
  *
  * Basada en la clase Polygon de la biblioteca
  * <a href="http://webdelprofesor.ula.ve/ingenieria/lrleon/aleph/html/index.html" target="_blank">Aleph-w</a>
  */
class Obstacle : public Polygon
{
public:
  using Polygon::Polygon;

  /**
    * \brief Tipos de orientaci&oacute;n en los que se puede haber formado un
    * pol&iacute;gono.
    * @author Alejandro Mujica
    */
  enum Orientation
  {
    Clockwise,
    Counter_Clockwise,
    Collinear,
    Num_Orientantions
  };

  double compute_area();

public:
  Obstacle();

  ~Obstacle();

  /**
    * Determina si un punto esta contenido dentro del pol&iacute;gono.
    * @param p Punto a verificar
    * @param use_borders Se coloca true si se quiere considerar dentro a un
    * punto que est&eacute; en el borde y false en caso contrario.
    */
  bool contains(const Point & p, bool use_borders = true);

  /**
    * Retorna una representaci&oacute;n en cadena del pol&icute;gono.
    */
  std::string to_string();

  /**
    * Retorna el &aacute;rea del pol&iacute;gono. NOTA: Solamente funciona para
    * pol&iacute;gonos convexos.
    */
  double area();

  /**
    * Retorna la orientaci&oacute;n en la que fue formado el pol&iacute;gono.
    * NOTA: Solamente funciona para pol&iacute;gonos convexos.
    */
  Orientation orientation();

  /**
    * Retorna true si el pol&iacute;gono fue formado en el sentido de las agujas
    * del reloj. NOTA: Solamente funciona para pol&iacute;gonos convexos.
    */
  bool is_clockwise();

  /**
    * Determina los puntos de intersecci&oacute;n de dos pol&iacute;gonos.
    * @param polygon El pol&iacute;gono con el que se intersecta.
    * @param points Lista en la que quedan almacenados los puntos de
    * intersecci&oacute;n.
    */
  void intersections_with(Obstacle & polygon, DynList<Point> & points);

  /**
    * Retorna true si el pol&iacute;gono se intersecta propiamente con el
    * segmemento s.
    */
  bool intersects_properly_with(const Segment & s);
};

# endif // OBSTACLE_H

