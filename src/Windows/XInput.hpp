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
#include <Windows.h>
#include <Xinput.h>

#include <SFML/Window/Joystick.hpp>

#include <SFMLGamepad/Gamepad.hpp>

namespace sf
{
namespace priv
{

////////////////////////////////////////////////////////////
/// \brief Handes XInput controllers
///
////////////////////////////////////////////////////////////
class XInput
{
public:
    ////////////////////////////////////////////////////////////
    /// \brief Check if a gamepad supports XInput
    ///
    /// \param gamepad  Gamepad to check
    ///
    /// \return True if the gamepad supports XInput, false otherwise
    ///
    ////////////////////////////////////////////////////////////
    static bool isXInput(unsigned int gamepad);

    ////////////////////////////////////////////////////////////
    /// \brief Get the current position of a control
    ///
    /// \param gamepad  Gamepad to check
    /// \param control  Control to check (LeftTrigger or RightTrigger)
    ///
    /// \return Current position of the control, in range [0 .. 100]
    ///
    /// \warning isXinput() has to be called as least once for this function to work
    /// correctly.
    ///
    ////////////////////////////////////////////////////////////
    static float getPosition(unsigned int gamepad, Gamepad::Control control);

private:
    ////////////////////////////////////////////////////////////
    /// \brief Initialize the XInput module
    ///
    /// \return True if XInput had been successfully initialized, false otherwise
    ///
    ////////////////////////////////////////////////////////////
    static bool init();

    ////////////////////////////////////////////////////////////
    /// \brief Undocumented XInput struct that contains extra informations
    /// about a controller
    ///
    ////////////////////////////////////////////////////////////
    struct XINPUT_CAPABILITIES_EX
    {
        XINPUT_CAPABILITIES capabilities;   //!< Standard XInput capabilities struct
        WORD                vendorId;       //!< Vendor ID
        WORD                productId;      //!< Product ID
        WORD                revisionId;     //!< Revision ID
        DWORD               a4;             //!< Unknown use
    };

    ////////////////////////////////////////////////////////////
    /// \brief typedef for the XInputGetCapabilities function
    ///
    ////////////////////////////////////////////////////////////
    typedef DWORD(_stdcall* XInputGetCapabilitiesEx_t)(DWORD a1, DWORD dwUserIndex, DWORD dwFlags,
                                                       XINPUT_CAPABILITIES_EX* pCapabilities);

    ////////////////////////////////////////////////////////////
    /// \brief Undocumented XInput function that gives extra informations
    /// about a controller
    ///
    ////////////////////////////////////////////////////////////
    static XInputGetCapabilitiesEx_t XInputGetCapabilitiesEx;


    static HMODULE xinputHandle;            //!< XInput module handle
    static int joy2xinpID[Joystick::Count]; //!< Maps joystick IDs against XInput controller IDs
};

}
}
