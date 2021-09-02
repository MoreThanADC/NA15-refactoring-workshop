#include "SnakeController.hpp"

#include <algorithm>
#include <sstream>

#include "EventT.hpp"
#include "IPort.hpp"

namespace Snake {
ConfigurationError::ConfigurationError()
    : std::logic_error("Bad configuration of Snake::Controller.") {}

UnexpectedEventException::UnexpectedEventException()
    : std::runtime_error("Unexpected event received!") {}

Controller::Controller(IPort& p_displayPort, IPort& p_foodPort, IPort& p_scorePort, std::string const& p_config, std::unique_ptr<World> world, std::unique_ptr<Segments> segments)
    : m_displayPort(p_displayPort),
      m_foodPort(p_foodPort),
      m_scorePort(p_scorePort),
      m_paused(false),
      world_(std::move(world)),
      segments_(std::move(segments)) {
    std::istringstream istr(p_config);
    char w, f, s, d;

    int width, height, length;
    int foodX, foodY;
    istr >> w >> width >> height >> f >> foodX >> foodY >> s;

    if (w == 'W' and f == 'F' and s == 'S') {
        m_mapDimension = std::make_pair(width, height);
        m_foodPosition = std::make_pair(foodX, foodY);

        istr >> d;
        switch (d) {
        case 'U':
            m_currentDirection = Direction_UP;
            break;
        case 'D':
            m_currentDirection = Direction_DOWN;
            break;
        case 'L':
            m_currentDirection = Direction_LEFT;
            break;
        case 'R':
            m_currentDirection = Direction_RIGHT;
            break;
        default:
            throw ConfigurationError();
        }
        istr >> length;

        while (length--) {
            Segment seg;
            istr >> seg.x >> seg.y;
            m_segments.push_back(seg);
        }
    } else {
        throw ConfigurationError();
    }
}

namespace {
bool isHorizontal(Direction direction) {
    return Direction_LEFT == direction or Direction_RIGHT == direction;
}

bool isVertical(Direction direction) {
    return Direction_UP == direction or Direction_DOWN == direction;
}

bool isPositive(Direction direction) {
    return (isVertical(direction) and Direction_DOWN == direction) or (isHorizontal(direction) and Direction_RIGHT == direction);
}

bool perpendicular(Direction dir1, Direction dir2) {
    return isHorizontal(dir1) == isVertical(dir2);
}
}  // namespace

void Controller::handleTimeoutInd() {
    updateSegmentsIfSuccessfullMove(calculateNewHead());
}

void Controller::handleDirectionInd(std::unique_ptr<Event> e) {
    auto direction = payload<DirectionInd>(*e).direction;

    if (perpendicular(m_currentDirection, direction)) {
        m_currentDirection = direction;
    }
}

void Controller::handleFoodInd(std::unique_ptr<Event> e) {
    auto receivedFood = payload<FoodInd>(*e);

    updateFoodPosition(receivedFood.x, receivedFood.y, std::bind(&Controller::sendClearOldFood, this));
}

void Controller::handleFoodResp(std::unique_ptr<Event> e) {
    auto requestedFood = payload<FoodResp>(*e);

    updateFoodPosition(requestedFood.x, requestedFood.y, [] {});
}

void Controller::handlePauseInd(std::unique_ptr<Event> e) {
    m_paused = not m_paused;
}

void Controller::receive(std::unique_ptr<Event> e) {
    switch (e->getMessageId()) {
    case TimeoutInd::MESSAGE_ID:
        if (!m_paused) {
            return handleTimeoutInd();
        }
        return;
    case DirectionInd::MESSAGE_ID:
        if (!m_paused) {
            return handleDirectionInd(std::move(e));
        }
        return;
    case FoodInd::MESSAGE_ID:
        return handleFoodInd(std::move(e));
    case FoodResp::MESSAGE_ID:
        return handleFoodResp(std::move(e));
    case PauseInd::MESSAGE_ID:
        return handlePauseInd(std::move(e));
    default:
        throw UnexpectedEventException();
    }
}

}  // namespace Snake
