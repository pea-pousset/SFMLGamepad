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

#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFMLGamepad/Gamepad.hpp>

#include <SFML/Window/Joystick.hpp>

#include <string>
#include <map>
#include <vector>
#include <cstdint>

namespace sf
{
namespace priv
{

////////////////////////////////////////////////////////////
/// \brief Static class for gamepad mapping implementation
///
////////////////////////////////////////////////////////////
class GamepadImpl
{
public:
    ////////////////////////////////////////////////////////////
    /// \brief Describes a control's type as used in the ControlInfo struct
    ///
    ////////////////////////////////////////////////////////////
    enum ControlType
    {
        None,   //!< The control is not present
        Button, //!< The control is an axis
        Hat,    //!< The control is a hat
        Axis    //!< The control is an axis
    };

    ////////////////////////////////////////////////////////////
    /// \brief Describes a platform as used in the Infos struct
    ///
    ////////////////////////////////////////////////////////////
    enum class Platform
    {
        Linux,      //!< Platform: Linux
        Windows,    //!< Platform: Windows
        Mac,        //!< Platform: macOS
        Android,    //!< Platform: Android
        Ios         //!< Platform: iOS
    };

    ////////////////////////////////////////////////////////////
    /// \brief Describes a control
    ///
    ////////////////////////////////////////////////////////////
    struct ControlInfo
    {
        uint8_t type : 2; ///< One of the ControlType values
        uint8_t dir  : 1; ///< Direction for axis : 0 = positive, 1 = negative
        uint8_t id   : 5; ///< Button or axis number
    };

    ////////////////////////////////////////////////////////////
    /// \brief Contains the informations about a controller
    ///
    ////////////////////////////////////////////////////////////
    struct Infos
    {
        Platform platform;                  //!< Platform used by this controller
        std::string name;                   //!< Name of the controller
        ControlInfo a{0};                   //!< 'A' control description
        ControlInfo b{0};                   //!< 'B' control description
        ControlInfo x{0};                   //!< 'X' control description
        ControlInfo y{0};                   //!< 'Y' control description
        ControlInfo start{0};               //!< 'Start' control description
        ControlInfo back{0};                //!< 'Back' control description
        ControlInfo guide{0};               //!< 'Guide' control description
        ControlInfo dpdown{0};              //!< 'Dpad down' control description
        ControlInfo dpleft{0};              //!< 'Dpad left' control description
        ControlInfo dpright{0};             //!< 'Dpad right' control description
        ControlInfo dpup{0};                //!< 'Dpad up' control description
        ControlInfo leftshoulder{0};        //!< 'Left shoulder' control description
        ControlInfo rightshoulder{0};       //!< 'Right shoulder' control description
        ControlInfo lefttrigger{0};         //!< 'Left trigger' control description
        ControlInfo righttrigger{0};        //!< 'Right trigger' control description
        ControlInfo leftstick{0};           //!< Left stick as a button control description
        ControlInfo rightstick{0};          //!< Right stick as a button control description
        ControlInfo leftxpos{0};            //!< Left stick, X axis, positive direction control description
        ControlInfo leftxneg{0};            //!< Left stick, X axis, negative direction control description
        ControlInfo leftypos{0};            //!< Left stick, Y axis, positive direction control description
        ControlInfo leftyneg{0};            //!< Left stick, Y axis, negative direction control description
        ControlInfo rightxpos{0};           //!< Right stick, X axis, positive direction control description
        ControlInfo rightxneg{0};           //!< Right stick, X axis, negative direction control description
        ControlInfo rightypos{0};           //!< Right stick, Y axis, positive direction control description
        ControlInfo rightyneg{0};           //!< Right stick, Y axis, negative direction control description
        ControlInfo touchpad{0};            //!< Touchpad control description
        ControlInfo paddle1{0};             //!< Paddle 1 control description
        ControlInfo paddle2{0};             //!< Paddle 2 control description
        ControlInfo paddle3{0};             //!< Paddle 3 control description
        ControlInfo paddle4{0};             //!< Paddle 4 control description
        ControlInfo misc1{0};               //!< Miscellaneous control 1 description
    };

    ///////////////////////////////////////////////////////////
    /// \brief Load a mapping database from a file
    ///
    /// \param filename  Path of the database to load
    ///
    ///////////////////////////////////////////////////////////
    static void loadMappingFromFile(const std::string& filename);

    ///////////////////////////////////////////////////////////
    /// \brief Load a mapping database from a string
    ///
    /// \param db  String that contains the database to load
    ///
    ///////////////////////////////////////////////////////////
    static void loadMappingFromString(const std::string& db);

    ///////////////////////////////////////////////////////////
    /// \brief Check if a gamepad is available
    ///
    /// "Available" means that 1) the underlying sf::Joystick is connected
    /// and 2) the gamepad has been identified in a mapping table
    ///
    /// \param gamepad  Index of the gamepad to check
    ///
    /// \return True if the gamepad is available, false otherwise
    ///
    ///////////////////////////////////////////////////////////
    static bool isAvailable(unsigned int gamepad);

    ///////////////////////////////////////////////////////////
    /// \brief Get informations about a control
    ///
    /// \param control  Control type
    /// \param id       sf::Joystick identification
    ///
    /// \return ControlInfo struct for the control type
    ///
    ///////////////////////////////////////////////////////////
    static ControlInfo& getControlInfo(Gamepad::Control control, sf::Joystick::Identification& id);

private:
    ///////////////////////////////////////////////////////////
    /// \brief Splits a text string into tokens
    ///
    /// Leading and trailing whitespaces are removed
    ///
    /// \param text   String to split
    /// \param delim  Delimiter character
    ///
    /// \return Vector of token strings
    ///
    ///////////////////////////////////////////////////////////
    static std::vector<std::string> tokenize(const std::string& text, const char delim);

    ///////////////////////////////////////////////////////////
    /// \brief Parse a full line of a text database
    ///
    /// \param line Line to parse
    ///
    ///////////////////////////////////////////////////////////
    static void parseLine(const std::string& line);

    ///////////////////////////////////////////////////////////
    /// \brief Parse a GUID string
    ///
    /// \param guid  GUID to parse
    /// \param pid   Stores the extracted PID
    /// \param vid   Stores the extracted VID
    ///
    /// \return True if the GUID is valid, false otherwise
    ///
    ///////////////////////////////////////////////////////////
    static bool parseGUID(const std::string& guid, uint16_t& pid, uint16_t& vid);

    ///////////////////////////////////////////////////////////
    /// \brief Parse an attribute/value pair string, i.e. start:b10
    ///
    /// \param attrStr  String to parse
    /// \param infos    Gamepad infos structure to update
    ///
    /// \return True if the attribute/value pair string is valid, false otherwise
    ///
    ///////////////////////////////////////////////////////////
    static bool parseAttr(const std::string& attrStr, Infos& infos);

    using Products = std::map<uint16_t, Infos>;     //!< Maps gamepad infos to a PID
    using Vendors = std::map<uint16_t, Products>;   //!< Maps PIDs to a VID

    static Vendors      _db;        //!< Full mapping
    static unsigned int _lineCount; //!< Current line in the databse being parsed
};

}
}
