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

# ifndef ENVIROMENT_H
# define ENVIROMENT_H

# include <tpl_euclidian_graph.H>
# include <tpl_indexArc.H>
# include <tpl_dynMapTree.H>

# include <utils.H>
# include <quadtree.H>

class GeometricMap;

/** \brief Enumerados que contiene los posibles algoritmos a ejecutar para
  * modelar los entornos.
  *
  * @author Alejandro Mujica
  */
enum class Algorithm
{
  Discretization,
  Building_Square_Cells,
  Building_Quad_Tree,
  Building_Visibility_Graph,
  Num_Algorithms
};

/**
   * \brief Clase con la informaci&oacute;n que almacenan los nodos de los
   * grafos.
   *
   * Se majena un atributo de tipo booleano que indica si el nodo es accesible
   * o no y un atributo de tipo double que indica la longitud del nivel del
   * nodo cuando el grafo es construido mediante un Quad Tree.
   *
   * @author Alejandro Mujica
   */
class Node_Info
{
public:
  Point position;

  bool available;

  double level_length_rel;

  Node_Info(bool _available = false, const double & _level_length_rel = 1.0)
    : available(_available), level_length_rel(_level_length_rel)
  {
    // Empty
  }

  bool operator == (const Node_Info & another) const
  {
    return position == another.position and available == another.available
        and level_length_rel == another.level_length_rel;
  }
};


using EnviromentNode = Graph_Node<Node_Info>;
using EnviromentArc = Graph_Arc<Empty_Class>;
using BaseEnviromentGraph = List_Graph<EnviromentNode, EnviromentArc>;

/**
  *
  * @author Alejandro Mujica
  */
class EnviromentGraph : public BaseEnviromentGraph
{  
  using BaseEnviromentGraph::BaseEnviromentGraph;

public:
  Node * beg, * end;

  EnviromentGraph()
    : BaseEnviromentGraph(), beg(nullptr), end(nullptr)
  {
    /* empty */
  }

  bool node_belong_to_graph(Node *);

  Node * get_closest_node(const Point &);

  void set_beg_node(const Point &, bool, double, GeometricMap &);

  void set_end_node(const Point &, bool, double, GeometricMap &);

  void clear();

  void save(std::ofstream &);
};

class MinPathBuilder
{
  GeometricMap    & map;
  EnviromentGraph & graph;

public:
  MinPathBuilder(GeometricMap & m, EnviromentGraph & g)
    : map(m), graph(g)
  {
    // Empty
  }

  DynList<Point> operator () ();
};

/**
  * \brief Clase que contiene la operaci&oacute;n a ejecutar en cada nodo en
  * la construcci&oacute;n del grafo en el algoritmo de discretizaci&oacute;n
  *
  * @author Alejandro Mujica
  */
class DiscretizeNodeOp
{
public:
  double length;
  double min_x;
  double min_y;

  void operator () (EnviromentGraph & g, EnviromentNode * node,
                    const size_t & i, const size_t & j)
  {
    node->get_info().available = true;
    node->get_info().position = Point(j * length + min_x, i * length + min_y);
  }
};

class DiscretizationAlgorithm
{
  GeometricMap & map;

public:

  DiscretizationAlgorithm(GeometricMap & m) : map(m) { /* empty */ }

  EnviromentGraph operator () (double, double);
};

/**
  * \brief Clase que contiene la operaci&oacute;n a ejecutar en cada nodo en la
  *  construcci&oacute;n del grafo en el algoritmo construcci&oacute;n de celdas
  *
  * @author Alejandro Mujica
  */
class CellsNodeOp
{
public:
  double length;
  double min_x;
  double min_y;
  double radius;

  void operator () (EnviromentGraph &, EnviromentNode * node,
                    const size_t & i, const size_t & j)
  {
    node->get_info().available = true;
    node->get_info().position = Point(j * length + min_x + radius,
                                      i * length + min_y + radius);
  }
};

class BuildingSquareCellsAlgorithm
{
  GeometricMap & map;

public:
  BuildingSquareCellsAlgorithm(GeometricMap & m) : map(m) { /* empty */ }

  EnviromentGraph operator () (double);
};

class BuildingQuadTreeAlgorithm
{
  GeometricMap & map;

  void cut(const Point &, double, double, Point []);

  void insert_points_in_quad_tree(const Point &, double, double, double,
                                  QuadTree &);

  bool is_busy(const Point &, double, double);

public:
  using MapGnodeQtreenode =
  DynMapTreap<EnviromentGraph::Node *, QuadTree::Node *>;

  using MapQtreenodeGnode =
  DynMapTreap<QuadTree::Node *, EnviromentGraph::Node *>;

  BuildingQuadTreeAlgorithm(GeometricMap & m) : map(m) { /* empty */ }

  EnviromentGraph operator () (double);
};

class BuildingVisibilityGraphAlgorithm
{
  GeometricMap & map;

public:
  BuildingVisibilityGraphAlgorithm(GeometricMap & m) : map(m) { /* empty */ }

  bool connect_node(EnviromentGraph &, EnviromentGraph::Node *,
                    IndexArc<EnviromentGraph> &, double);

  EnviromentGraph operator () (double);
};

class Distance
{
public:  
  typedef Geom_Number Distance_Type;

  Distance_Type operator()(EnviromentGraph::Arc * arc)
  {
    EnviromentGraph::Node * src_node =
        reinterpret_cast<EnviromentGraph::Node *>(arc->src_node);
    EnviromentGraph::Node * tgt_node =
        reinterpret_cast<EnviromentGraph::Node *>(arc->tgt_node);
    return src_node->get_info().position.distance_with(
          tgt_node->get_info().position);
  }

  static const Distance_Type Max_Distance;

  static const Distance_Type Zero_Distance;
};

# endif // ENVIROMENT_H

