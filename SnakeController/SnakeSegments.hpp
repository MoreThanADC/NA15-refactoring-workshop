#pragma once

#include <list>
#include <string>
#include "SnakeInterface.hpp"
#include "SnakePosition.hpp"

namespace Snake
{

class Segments
{
public:
    Segments(Direction direction);

    bool isCollision(Position position) const;
    void addSegment(Position position);
    void addHead(Position position);
    Position nextHead() const;
    Position removeTail();
    void updateDirection(Direction newDirection);
    unsigned size() const;
    std::string recivedPoints();
private:
    Direction m_headDirection;
    std::list<Position> m_segments;
};

} // namespace Snake
