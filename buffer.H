/*
  Envmorobot.

  Author: Alejandro Mujica (aledrums@gmail.com)
*/

# ifndef BUFFER_H
# define BUFFER_H

# include <memory>

# include <tpl_dynMapTree.H>

# include <obstacle.H>


/**
  * \brief Clase de tipo Singletone que mantiene en memoria mapeos de
  *  Obst&aacute;culos y Paredes con sus versiones extendidas
  *
  * Cada vez que se pide extended_wall o extended_obstacle lo busca en el mapeo
  * existente, si no existe lo agrega.
  *
  * @author Alejandro Mujica
  */

typedef std::pair<Obstacle *, double> Pair_Obs;

typedef std::pair<Segment *, double> Pair_Seg;

struct Compare_Obs
{
  bool operator () (const Pair_Obs & p1, const Pair_Obs & p2)
  {
    if (p1.first < p2.first)
      return true;

    return not (p1.first < p2.first) and (p1.second < p2.second);
  }
};

struct Compare_Seg
{
  bool operator () (const Pair_Seg & p1, const Pair_Seg & p2)
  {
    if (p1.first < p2.first)
      return true;

    return not (p1.first < p2.first) and (p1.second < p2.second);
  }
};

class Buffer
{
  DynMapTree<Pair_Obs, Obstacle, Treap, Compare_Obs>
  map_obstacle_extended_obstacle;

  DynMapTree<Pair_Seg, Obstacle, Treap, Compare_Seg>
  map_wall_extended_wall;

  Buffer();

  static std::unique_ptr<Buffer> instance;

public:
  /**
    * Retorna la instancia (&uacute;nica) del Buffer.
    */
  static Buffer * get_instance();

  /**
    * Retorna el extended_obstacle correspondiente a obstacle, si no existe lo
    * agrega.
    * @param obstacle Obst&aacute;culo al que se le quiere pedir el extendido
    * @param radius Radio del robot.
    */
  const Obstacle & get_extended_obstacle(Obstacle & obstacle,
                                         const double & raius);

  /**
    * Retorna el extended_wall correspondiente a wall, si no existe lo agrega.
    * @param wall Pared a la que se le quiere pedir la extendida
    * @param radius Radio del robot.
    */
  const Obstacle & get_extended_wall(Segment & wall, const double & radius);

  /**
    * Vac&iacute;a los mapas.
    */
  void clear();
};

# endif // BUFFER_H

