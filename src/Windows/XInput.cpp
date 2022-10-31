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
#include <Windows/XInput.hpp>

#include <iostream>

namespace sf
{
////////////////////////////////////////////////////////////
priv::XInput::XInputGetCapabilitiesEx_t priv::XInput::XInputGetCapabilitiesEx = NULL;
priv::XInput::XInputGetState_t priv::XInput::XInputGetState_ = NULL;
HMODULE priv::XInput::xinputHandle = 0;
int priv::XInput::joy2xinpID[Joystick::Count] = { -1 };


////////////////////////////////////////////////////////////
bool priv::XInput::isXInput(unsigned int gamepad)
{
    if (!init())
        return false;

    XINPUT_STATE state;
    XINPUT_CAPABILITIES_EX caps;

    for (int i = 0; i < sf::Joystick::Count; i++)
        joy2xinpID[i] = -1;

    int joyIdx = 0;
    for (DWORD xinpIdx = 0; xinpIdx < XUSER_MAX_COUNT; xinpIdx++)
    {
        DWORD ret = XInputGetState_(xinpIdx, &state);
        if (ret == ERROR_SUCCESS)
        {
            XInputGetCapabilitiesEx(1, xinpIdx, 0, &caps);

            for (int j = joyIdx; j < Joystick::Count; j++)
            {
                auto id = Joystick::getIdentification(j);
                if ((id.vendorId == caps.vendorId) &&
                    (id.productId == caps.productId || caps.productId == 0))
                {
                    joy2xinpID[j] = xinpIdx;
                    joyIdx = j + 1;
                    break;
                }
            }
        }
    }

    return joy2xinpID[gamepad] != -1;
}


////////////////////////////////////////////////////////////
float priv::XInput::getPosition(unsigned int gamepad, Gamepad::Control control)
{
    XINPUT_STATE state;
    DWORD ret;

    ret = XInputGetState_(joy2xinpID[gamepad], &state);
    if (ret != ERROR_SUCCESS)
        return 0.f;

    if (control == Gamepad::Control::LeftTrigger)
        return state.Gamepad.bLeftTrigger * 100.f / 255.f;
    else if (control == Gamepad::Control::RightTrigger)
        return state.Gamepad.bRightTrigger * 100.f / 255.f;
    else if (control == Gamepad::Control::Up)
        return state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ? 100.f : 0.f;
    else if (control == Gamepad::Control::Down)
        return state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ? 100.f : 0.f;
    else if (control == Gamepad::Control::Left)
        return state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ? 100.f : 0.f;
    else if (control == Gamepad::Control::Right)
        return state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ? 100.f : 0.f;

    return 0.f;
}


////////////////////////////////////////////////////////////
bool priv::XInput::init()
{
    static bool alreadyTried = false;

    if (xinputHandle == 0)
    {
        if (alreadyTried)
            return false;

        alreadyTried = true;

        xinputHandle = LoadLibrary(TEXT("XInput1_4.dll"));
        if (isXInput == 0)
        {
            std::cerr << "XInput: Unable to load module 'XInput1_4.Dll'" << std::endl;
            return false;
        }

        XInputGetState_ = reinterpret_cast<XInputGetState_t>(GetProcAddress(xinputHandle, "XInputGetState"));
        if (XInputGetState_ == NULL)
        {
            std::cerr << "XInput: Unable to get 'XInputGetState' function" << std::endl;
            return false;
        }

        XInputGetCapabilitiesEx = reinterpret_cast<XInputGetCapabilitiesEx_t>(
                                        GetProcAddress(xinputHandle, reinterpret_cast<char*>(108)));
        if (XInputGetCapabilitiesEx == NULL)
        {
            std::cerr << "XInput: Unable to get 'XInputGetCapabilitiesEx' function" << std::endl;
            return false;
        }
    }

    return true;
}

}
