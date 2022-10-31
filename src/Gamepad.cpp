////////////////////////////////////////////////////////////
//
// SFMLGamepad - Gamepad mapping for SFML
// Copyright (C) 2022 Pierre-Alexandre Pousset <pea.pousset@gmail.com>
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <GamepadImpl.hpp>
#include <SFMLGamepad/Gamepad.hpp>

#include <SFML/Window/Joystick.hpp>
#include <SFML/Config.hpp>

#ifdef SFML_SYSTEM_WINDOWS
#include <Windows/XInput.hpp>
#endif

#include <cmath>

namespace sf
{
////////////////////////////////////////////////////////////
using impl = priv::GamepadImpl;


////////////////////////////////////////////////////////////
void Gamepad::loadMappingFromFile(const std::string& filename)
{
    impl::loadMappingFromFile(filename);
}


////////////////////////////////////////////////////////////
void Gamepad::loadMappingFromString(const std::string& db)
{
    impl::loadMappingFromString(db);
}


////////////////////////////////////////////////////////////
bool Gamepad::isAvailable(unsigned int gamepad)
{
    return impl::isAvailable(gamepad);
}


////////////////////////////////////////////////////////////
bool Gamepad::isPressed(unsigned int gamepad, Control control, unsigned int deadzone)
{
    if (gamepad >= Joystick::Count || control == Control::None)
        return false;

    if (deadzone > 100)
        deadzone = 100;

    auto id = Joystick::getIdentification(gamepad);
    auto& info = impl::getControlInfo(control, id);

    if (info.type == impl::ControlType::Button)
    {
        return Joystick::isButtonPressed(gamepad, info.id);
    }
    else if (info.type == impl::ControlType::Axis || info.type == impl::ControlType::Hat)
    {
#ifdef SFML_SYSTEM_WINDOWS
        if ((control == Control::LeftTrigger || control == Control::RightTrigger)
            && priv::XInput::isXInput(gamepad))
        {
            return priv::XInput::getPosition(gamepad, control) >= deadzone;
        }
#endif

        auto val = Joystick::getAxisPosition(gamepad, static_cast<Joystick::Axis>(info.id));
        if (info.dir)
            return val <= -static_cast<int>(deadzone);
        else
            return val >= deadzone;
    }

    return false;
}


////////////////////////////////////////////////////////////
float Gamepad::getPosition(unsigned int gamepad, Control control)
{
    if (gamepad >= Joystick::Count || control == Control::None)
        return 0.f;

    auto id = Joystick::getIdentification(gamepad);
    auto& info = impl::getControlInfo(control, id);

    if (info.type == impl::ControlType::Button)
    {
        return Joystick::isButtonPressed(gamepad, info.id) ? 100.f : 0.f;
    }
    else if (info.type == impl::ControlType::Axis || info.type == impl::ControlType::Hat)
    {
#ifdef SFML_SYSTEM_WINDOWS
        if ((control == Control::LeftTrigger || control == Control::RightTrigger)
            && priv::XInput::isXInput(gamepad))
        {
            return priv::XInput::getPosition(gamepad, control);
        }
#endif
        auto val = Joystick::getAxisPosition(gamepad, static_cast<Joystick::Axis>(info.id));
        if (info.dir)
        {
            if (val > 0.f)
                return 0.f;
            return fabs(val);
        }
        else
        {
            if (val < 0.f)
                return 0.f;
            return val;
        }
    }

    return 0.f;
}


////////////////////////////////////////////////////////////
bool Gamepad::hasControl(unsigned int gamepad, Control control)
{
    if (gamepad >= Joystick::Count || control == Control::None)
        return false;

    auto id = Joystick::getIdentification(gamepad);
    auto& info = impl::getControlInfo(control, id);

    return info.type != impl::ControlType::None;
}

}
