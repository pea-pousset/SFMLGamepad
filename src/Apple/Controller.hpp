#pragma once

#include <SFML/Window/Joystick.hpp>

namespace sf
{
namespace priv
{

class Controller
{
public:
    static Controller* instance();
    bool isConnected(unsigned int gamepad);
    Joystick::Identification getIdentification(unsigned int gamepad);
    float getAxisPosition(unsigned int gamepad, Joystick::Axis axis);
    bool isButtonPressed(unsigned int gamepad, unsigned int button);
private:
    Controller();
    ~Controller();
    static Controller* _instance;
};

}
}
