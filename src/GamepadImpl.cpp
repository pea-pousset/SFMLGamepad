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
#include "GamepadImpl.hpp"

#include <SFML/Config.hpp>

#include <functional>
#include <fstream>
#include <iostream>
#include <cctype>

#ifdef SFML_SYSTEM_MACOS
    #include "Apple/Controller.hpp"
#endif

#ifdef SFML_SYSTEM_WINDOWS
    #define POVY_UP_DIR     0
    #define POVY_DOWN_DIR   1
#else
    #define POVY_UP_DIR     1
    #define POVY_DOWN_DIR   0
#endif

namespace
{
using namespace std::placeholders;

#ifdef SFML_SYSTEM_MACOS
using ctrl = sf::priv::Controller;
auto getIdentification = std::bind(&ctrl::getIdentification, ctrl::instance(), _1);
auto isConnected       = std::bind(&ctrl::isConnected,       ctrl::instance(), _1);
#else
auto getIdentification = std::bind(sf::Joystick::getIdentification, _1);
auto isConnected       = std::bind(sf::Joystick::isConnected,       _1);
#endif

}


namespace sf
{
////////////////////////////////////////////////////////////
priv::GamepadImpl::Vendors priv::GamepadImpl::_db;
unsigned int priv::GamepadImpl::_lineCount;


////////////////////////////////////////////////////////////
void priv::GamepadImpl::loadMappingFromFile(const std::string& filename)
{
    std::ifstream db(filename);
    if (!db.is_open())
    {
        std::cerr << "Could not load '" << filename << "'" << std::endl;
        return;
    }

    _lineCount = 0;
    while (!db.eof())
    {
        std::string line;
        std::getline(db, line);
        parseLine(line);
    }

    db.close();
}


////////////////////////////////////////////////////////////
void priv::GamepadImpl::loadMappingFromString(const std::string& db)
{
    _lineCount = 0;
    const char* begin = db.c_str();
    const char* end   = begin;

    while (*begin)
    {
        while (*end && *end != '\n')
            end++;
        parseLine(std::string(begin, end));
        begin = end++;
    }
}


////////////////////////////////////////////////////////////
bool priv::GamepadImpl::isAvailable(unsigned int gamepad)
{
    if (gamepad >= sf::Joystick::Count)
        return false;

    if (!isConnected(gamepad))
        return false;

    auto id = getIdentification(gamepad);
    auto vid = _db.find(id.vendorId);
    if (vid == _db.end())
        return false;

    auto pid = vid->second.find(id.productId);
    if (pid == vid->second.end())
        return false;

    return true;
}


////////////////////////////////////////////////////////////
priv::GamepadImpl::ControlInfo& priv::GamepadImpl::getControlInfo(Gamepad::Control control,
    Joystick::Identification& id)
{
    static ControlInfo empty{0};

    auto vid = _db.find(id.vendorId);
    if (vid == _db.end())
        return empty;

    auto pid = vid->second.find(id.productId);
    if (pid == vid->second.end())
        return empty;

    auto& infos = pid->second;

    switch (control)
    {
        case Gamepad::Control::A:             return infos.a;
        case Gamepad::Control::B:             return infos.b;
        case Gamepad::Control::X:             return infos.x;
        case Gamepad::Control::Y:             return infos.y;
        case Gamepad::Control::Back:          return infos.back;
        case Gamepad::Control::Start:         return infos.start;
        case Gamepad::Control::Guide:         return infos.guide;
        case Gamepad::Control::Up:            return infos.dpup;
        case Gamepad::Control::Down:          return infos.dpdown;
        case Gamepad::Control::Left:          return infos.dpleft;
        case Gamepad::Control::Right:         return infos.dpright;
        case Gamepad::Control::LeftStick:     return infos.leftstick;
        case Gamepad::Control::RightStick:    return infos.rightstick;
        case Gamepad::Control::LeftShoulder:  return infos.leftshoulder;
        case Gamepad::Control::RightShoulder: return infos.rightshoulder;
        case Gamepad::Control::LeftTrigger:   return infos.lefttrigger;
        case Gamepad::Control::RightTrigger:  return infos.righttrigger;
        case Gamepad::Control::LeftXMinus:    return infos.leftxneg;
        case Gamepad::Control::LeftXPlus:     return infos.leftxpos;
        case Gamepad::Control::LeftYMinus:    return infos.leftyneg;
        case Gamepad::Control::LeftYPlus:     return infos.leftypos;
        case Gamepad::Control::RightXMinus:   return infos.rightxneg;
        case Gamepad::Control::RightXPlus:    return infos.rightxpos;
        case Gamepad::Control::RightYMinus:   return infos.rightyneg;
        case Gamepad::Control::RightYPlus:    return infos.rightypos;
        case Gamepad::Control::Touchpad:      return infos.touchpad;
        case Gamepad::Control::Paddle1:       return infos.paddle1;
        case Gamepad::Control::Paddle2:       return infos.paddle2;
        case Gamepad::Control::Paddle3:       return infos.paddle3;
        case Gamepad::Control::Paddle4:       return infos.paddle4;
        case Gamepad::Control::Misc1:         return infos.misc1;
        default:
            return empty;
    }
}


////////////////////////////////////////////////////////////
std::vector<std::string> priv::GamepadImpl::tokenize(const std::string& text, const char delim)
{
    if (text.empty())
        return {};

    std::vector<std::string> tokens;
    const char* str = text.c_str();

    do
    {
        while (isspace(*str))
            str++;
        const char* begin = str;

        while (*str && (*str != delim))
            str++;

        const char* end = str - 1;
        while ((end > begin) && (isspace(*end) || *end == '\0'))
            end--;

        if (*begin && (*begin != delim))
            tokens.push_back(std::string(begin, end+1));

    } while (*str++);

    return tokens;
}


////////////////////////////////////////////////////////////
void priv::GamepadImpl::parseLine(const std::string& line)
{
       _lineCount++;

        auto tokens = tokenize(line, ',');

        if (tokens.empty() || tokens[0][0] == '#') // Empty line or comment
            return;

        uint16_t    vid;
        uint16_t    pid;
        Infos       infos;
        auto&       guid = tokens[0];

        if (!parseGUID(guid, vid, pid))
        {
            std::cerr << "Invalid GUID '" << guid << "', line " << _lineCount << std::endl;
            return;
        }

        if (tokens.size() < 2)
        {
            std::cerr << "Missing name for GUID '" << guid << "', line " << _lineCount << std::endl;
            return;
        }
        infos.name = tokens[1];

        if (tokens.size() < 3)
        {
            std::cerr << "No attributes for GUID '" << guid << "', line " << _lineCount << std::endl;
            return;
        }

        bool error = false;
        for (int i = 2; i < tokens.size() && !error; i++)
        {
            if (!parseAttr(tokens[i], infos))
            {
                std::cerr << "Invalid argument or value for GUID '" << guid << "', argument '"
                    << tokens[i] << "', line " << _lineCount << std::endl;
                error = true;
            }
        }

        if (error)
            return;

#if defined(SFML_SYSTEM_WINDOWS)
        if (infos.platform != Platform::Windows)
            return;
#elif defined(SFML_SYSTEM_MACOS)
        if (infos.platform != Platform::Mac)
            return;
#elif defined(SFML_SYSTEM_ANDROID)
        if (infos.platform != Platform::Android)
            return;
#elif defined(SFML_SYSTEM_IOS)
        if (infos.platform != Platform::Ios)
            return;
#else
        if (infos.platform != Platform::Linux)
            return;
#endif
        _db[vid][pid] = infos;
}


////////////////////////////////////////////////////////////
bool priv::GamepadImpl::parseGUID(const std::string& guid, uint16_t& vid, uint16_t& pid)
{
    // Check GUID format : 32 hex digits
    if (guid.size() != 32)
        return false;

    for (int i = 0; i < 32; i++)
        if (!isxdigit(guid[i]))
            return false;

    // Hex digit to int conversion
    const auto x2d = [](char x) -> uint16_t {
        if (isdigit(x))
            return x - '0';
        if (x >= 'A' && x <= 'F')
            return (x - 'A') + 10;
        if (x >= 'a' && x <= 'f')
            return (x - 'a') + 10;
        return 0;
    };

    // Extract VID/PID from GUID
    vid = 0;
    vid |= x2d(guid[8])  * 0x0010;
    vid |= x2d(guid[9])  * 0x0001;
    vid |= x2d(guid[10]) * 0x1000;
    vid |= x2d(guid[11]) * 0x0100;

    pid = 0;
    pid |= x2d(guid[16]) * 0x0010;
    pid |= x2d(guid[17]) * 0x0001;
    pid |= x2d(guid[18]) * 0x1000;
    pid |= x2d(guid[19]) * 0x0100;

    return true;
}


////////////////////////////////////////////////////////////
bool priv::GamepadImpl::parseAttr(const std::string& attrStr, Infos& infos)
{
    auto tokens = tokenize(attrStr, ':');
    if (tokens.size() != 2)
        return false;

    auto& attr = tokens[0];
    auto& val  = tokens[1];

    if (attr == "platform")
    {
        if (val == "Windows")
            infos.platform = Platform::Windows;
        else if (val == "Mac OS X")
            infos.platform = Platform::Mac;
        else if (val == "Linux")
            infos.platform = Platform::Linux;
        else if (val == "Android")
            infos.platform = Platform::Android;
        else if (val == "iOS")
            infos.platform = Platform::Ios;
        else
            return false;
    }
    else
    {
        ControlInfo control{0};

        if (val[0] == 'b' && val.size() >= 2)
        {
            control.type = ControlType::Button;
            control.id = atoi(&val[1]);
        }
        else if (val[0] == 'h' && val.size() >= 4)
        {
            if (val[1] != '0' || val[2] != '.')
                return false;

            control.type = ControlType::Hat;
            switch(atoi(&val[3]))
            {
                case 1:
                    control.id = static_cast<int>(Joystick::PovY);
                    control.dir = POVY_UP_DIR;
                    break;
                case 2:
                    control.id = static_cast<int>(Joystick::PovX);
                    control.dir = 0;
                    break;
                case 4:
                    control.id = static_cast<int>(Joystick::PovY);
                    control.dir = POVY_DOWN_DIR;
                    break;
                case 8:
                    control.id = static_cast<int>(Joystick::PovX);
                    control.dir = 1;
                    break;
                default:
                    return false;
            }
        }
        else if (val[0] == 'a' && val.size() >= 2)
        {
            control.type = ControlType::Axis;
            switch(atoi(&val[1]))
            {
                case 0: control.id = static_cast<int>(Joystick::X); break;
                case 1: control.id = static_cast<int>(Joystick::Y); break;
                case 2: control.id = static_cast<int>(Joystick::Z); break;
                case 3: control.id = static_cast<int>(Joystick::U); break;
                case 4: control.id = static_cast<int>(Joystick::V); break;
                case 5: control.id = static_cast<int>(Joystick::R); break;
                default:
                    return false;
            }
        }
        else if ((val[0] == '-' || val[0] == '+') && val.size() >= 3 && val[1] == 'a')
        {
            control.type = ControlType::Axis;
            if (val[0] == '-')
                control.dir = 1;

            switch(atoi(&val[2]))
            {
                case 0: control.id = static_cast<int>(Joystick::X); break;
                case 1: control.id = static_cast<int>(Joystick::Y); break;
                case 2: control.id = static_cast<int>(Joystick::Z); break;
                case 3: control.id = static_cast<int>(Joystick::U); break;
                case 4: control.id = static_cast<int>(Joystick::V); break;
                case 5: control.id = static_cast<int>(Joystick::R); break;
                default:
                    return false;
            }
        }
        else
        {
            return false;
        }

        if (attr == "a")
            infos.a = control;
        else if (attr == "b")
            infos.b = control;
        else if (attr == "x")
            infos.x = control;
        else if (attr == "y")
            infos.y = control;
        else if (attr == "start")
            infos.start = control;
        else if (attr == "back")
            infos.back = control;
        else if (attr == "guide")
            infos.guide = control;
        else if (attr == "dpdown")
            infos.dpdown = control;
        else if (attr == "dpleft")
            infos.dpleft = control;
        else if (attr == "dpright")
            infos.dpright = control;
        else if (attr == "dpup")
            infos.dpup = control;
        else if (attr == "leftshoulder")
            infos.leftshoulder = control;
        else if (attr == "rightshoulder")
            infos.rightshoulder = control;
        else if (attr == "lefttrigger")
            infos.lefttrigger = control;
        else if (attr == "righttrigger")
            infos.righttrigger = control;
        else if (attr == "leftstick")
            infos.leftstick = control;
        else if (attr == "rightstick")
            infos.rightstick = control;
        else if (attr == "leftx")
        {
            ControlInfo controlNeg = control;
            controlNeg.dir = 1;
            infos.leftxpos = control;
            infos.leftxneg = controlNeg;
        }
        else if (attr == "lefty")
        {
            ControlInfo controlNeg = control;
            controlNeg.dir = 1;
            infos.leftypos = control;
            infos.leftyneg = controlNeg;
        }
        else if (attr == "rightx")
        {
            ControlInfo controlNeg = control;
            controlNeg.dir = 1;
            infos.rightxpos = control;
            infos.rightxneg = controlNeg;
        }
        else if (attr == "righty")
        {
            ControlInfo controlNeg = control;
            controlNeg.dir = 1;
            infos.rightypos = control;
            infos.rightyneg = controlNeg;
        }
        else if (attr == "+leftx")
            infos.leftxpos = control;
        else if (attr == "-leftx")
            infos.leftxneg = control;
        else if (attr == "+lefty")
            infos.leftypos = control;
        else if (attr == "-lefty")
            infos.leftyneg = control;
        else if (attr == "+rightx")
            infos.rightxpos = control;
        else if (attr == "-rightx")
            infos.rightxneg = control;
        else if (attr == "+righty")
            infos.rightypos = control;
        else if (attr ==  "-righty")
            infos.rightyneg = control;
        else if (attr == "touchpad")
            infos.touchpad = control;
        else if (attr == "paddle1")
            infos.paddle1 = control;
        else if (attr == "paddle2")
            infos.paddle2 = control;
        else if (attr == "paddle3")
            infos.paddle3 = control;
        else if (attr == "paddle4")
            infos.paddle4 = control;
        else if (attr == "misc1")
            infos.misc1 = control;
        else
            return false;
    }

    return true;
}

} // namespace sf
