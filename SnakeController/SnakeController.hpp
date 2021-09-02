#pragma once

#include <functional>
#include <list>
#include <memory>
#include <stdexcept>

#include "IEventHandler.hpp"
#include "SnakeInterface.hpp"
#include "Segments.hpp"
#include "World.hpp"

class Event;
class IPort;

namespace Snake {
struct ConfigurationError : std::logic_error {
    ConfigurationError();
};

struct UnexpectedEventException : std::runtime_error {
    UnexpectedEventException();
};

class Controller : public IEventHandler {
public:
    Controller(IPort& p_displayPort, IPort& p_foodPort, IPort& p_scorePort, std::string const& p_config,
                std::unique_ptr<World> world,  std::unique_ptr<Segments> segments);

    Controller(Controller const& p_rhs) = delete;
    Controller& operator=(Controller const& p_rhs) = delete;

    void receive(std::unique_ptr<Event> e) override;

private:
    std::unique_ptr<World> world_ = nullptr;
    std::unique_ptr<Segments> segments_ = nullptr;

    IPort& m_displayPort;
    IPort& m_foodPort;
    IPort& m_scorePort;

    Direction m_currentDirection;

    void handleTimeoutInd();
    void handleDirectionInd(std::unique_ptr<Event>);
    void handleFoodInd(std::unique_ptr<Event>);
    void handleFoodResp(std::unique_ptr<Event>);
    void handlePauseInd(std::unique_ptr<Event>);

    bool m_paused;
};

}  // namespace Snake
