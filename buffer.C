/*
  Envmorobot.

  Author: Alejandro Mujica (aledrums@gmail.com)
*/

# include <buffer.H>
# include <utils.H>

std::unique_ptr<Buffer> Buffer::instance = std::unique_ptr<Buffer>(nullptr);

Buffer::Buffer()
{
  // Empty
}

Buffer * Buffer::get_instance()
{
  if (instance == nullptr)
    instance = std::unique_ptr<Buffer>(new Buffer);
  return instance.get();
}

const Obstacle & Buffer::get_extended_obstacle(Obstacle & obstacle,
                                               const double & radius)
{
  Pair_Obs p(&obstacle, radius);

  auto ret = map_obstacle_extended_obstacle.search(p);

  if (ret != nullptr)
    return ret->second;

  Obstacle e_obstacle = build_extended_obstacle(obstacle, radius);

  return map_obstacle_extended_obstacle.insert(p, e_obstacle)->second;
}

const Obstacle & Buffer::get_extended_wall(Segment & wall,
                                           const double & radius)
{
  Pair_Seg p(&wall, radius);

  auto ret = map_wall_extended_wall.search(p);

  if (ret != nullptr)
    return ret->second;

  Obstacle e_wall = build_extended_wall(wall, radius);
  return map_wall_extended_wall.insert(p, e_wall)->second;
}

void Buffer::clear()
{
  map_obstacle_extended_obstacle.empty();
  map_wall_extended_wall.empty();
}

