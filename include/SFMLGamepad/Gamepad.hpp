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
#include <string>
#include <SFMLGamepad/Config.hpp>

namespace sf
{

////////////////////////////////////////////////////////////
/// \brief Add mappings to sf::Joystick
///
////////////////////////////////////////////////////////////
class SFML_GAMEPAD_API Gamepad
{
public:
    enum class Control
    {
        None,           //!< No control
        A,              //!< A / Cross button
        B,              //!< B / Circle button
        X,              //!< X / Square button
        Y,              //!< Y / Triangle button
        Back,           //!< Back / Select button
        Start,          //!< Start button
        Guide,          //!< Guilde / Home button
        Up,             //!< Directional pad, up direction
        Down,           //!< Directional pad, down direction
        Left,           //!< Directional pad, left direction
        Right,          //!< Directional pad, right direction
        LeftStick,      //!< Left stick pressed
        RightStick,     //!< Right stick pressed
        LeftShoulder,   //!< Left shoulder button
        RightShoulder,  //!< Right shoulder button
        LeftTrigger,    //!< Left trigger
        RightTrigger,   //!< Right trigger
        LeftXPlus,      //!< Left stick, X axis, positive direction
        LeftXMinus,     //!< Left stick, X axis, negative direction
        LeftYPlus,      //!< Left stick, Y axis, positive direction
        LeftYMinus,     //!< Left stick, Y axis, negative direction
        RightXPlus,     //!< Right stick, X axis, positive direction
        RightXMinus,    //!< Right stick, X axis, negative direction
        RightYPlus,     //!< Right stick, Y axis, positive direction
        RightYMinus,    //!< Right stick, Y axis, negative direction
        Touchpad,       //!< Touchpad (PS gamepads)
        Paddle1,        //!< Paddle 1
        Paddle2,        //!< Paddle 2
        Paddle3,        //!< Paddle 3
        Paddle4,        //!< Paddle 4
        Misc1           //!< Miscellaneous control #1
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

    ////////////////////////////////////////////////////////////
    /// \brief Check if a gamepad control is pressed
    ///
    /// If the control is an analog one, its position is converted to
    /// a boolean according to a threshold
    ///
    /// \param gamepad   Index of the gamepad
    /// \param control   Control to check
    /// \param deadzone  Threshold value [0 .. 100] for axis to boolean conversion
    ///
    /// \return True if the control is pressed, false otherwise
    ///
    ////////////////////////////////////////////////////////////
    static bool isPressed(unsigned int gamepad, Control control, unsigned int deadzone = 50);

    ////////////////////////////////////////////////////////////
    /// \brief Get the current position of a control
    ///
    /// If the control is a button, the returned value will be
    /// 0 when released and 100 when pressed
    ///
    /// \param gamepad  Index of the gamepad
    /// \param control  Control to check
    ///
    /// \return Current position of the control, in range [0 .. 100]
    ///
    ////////////////////////////////////////////////////////////
    static float getPosition(unsigned int gamepad, Control control);

    ////////////////////////////////////////////////////////////
    /// \brief Check if a gamepad supports a given control
    ///
    /// \param gamepad  Index of the gamepad to check
    /// \param control  Control to check
    ///
    /// \return True if the gamepad supports the control, false otherwise
    ///
    ////////////////////////////////////////////////////////////
    static bool hasControl(unsigned int gamepad, Control control);
};

}
