#include "Controller.hpp"

#import <Foundation/Foundation.h>
#import <IOKit/hid/IOHIDManager.h>

#include <iostream>
#include <string_view>

// Hat values for range 0 .. 7
#define HAT_N   0
#define HAT_S   4
#define HAT_E   2
#define HAT_W   6
#define HAT_NW  7
#define HAT_NE  1
#define HAT_SW  5
#define HAT_SE  3
#define HAT_MID 15

namespace
{

struct Devices
{
    IOHIDDeviceRef               ref;
    sf::Joystick::Identification ident;
    int                          hatValue;
    float                        X_axis;
    float                        Y_axis;
    float                        Z_axis;
    float                        U_axis;
    float                        V_axis;
    float                        R_axis;
    uint32_t                     buttons;
    
} devices[sf::Joystick::Count]{};

IOHIDManagerRef hidManager{nullptr};

void addDictionary(CFMutableArrayRef matcher, uint32_t page, uint32_t use)
{
    CFMutableDictionaryRef result =
        CFDictionaryCreateMutable(kCFAllocatorDefault,
                                  0,
                                  &kCFTypeDictionaryKeyCallBacks,
                                  &kCFTypeDictionaryValueCallBacks);
    
    if (result == nullptr)
        return;
    
    CFNumberRef pageNumber = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &page);
    CFDictionarySetValue(result, CFSTR(kIOHIDDeviceUsagePageKey), pageNumber);
    CFRelease(pageNumber);
    CFNumberRef useNumber = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &use);
    CFDictionarySetValue(result, CFSTR(kIOHIDDeviceUsageKey), useNumber);
    CFRelease(useNumber);
    CFArrayAppendValue(matcher, result);
    CFRelease(result);
}

void deviceInput(void* ctx, IOReturn result, void* sender, IOHIDValueRef value)
{
    auto* device = reinterpret_cast<Devices*>(ctx);
    
    auto element = IOHIDValueGetElement(value);
    auto type    = static_cast<int>(IOHIDElementGetType(element));
    auto page    = static_cast<int>(IOHIDElementGetUsagePage(element));
    auto usage   = static_cast<int>(IOHIDElementGetUsage(element));
    auto val     = static_cast<int>(IOHIDValueGetIntegerValue(value));
    
    auto  min  = IOHIDElementGetLogicalMin(element);
    auto  max  = IOHIDElementGetLogicalMax(element);
    float fval = 200.f * ((val - min) / static_cast<float>(max - min)) - 100.f;
    
    auto err = [&]() {
        std::cerr << std::hex << "Device  VID=" << device->ident.vendorId << " PID=";
        std::cerr << device->ident.productId << std::dec << std::endl;
        std::cerr << "Control t=" << type << " p=" << page << " u=";
        std::cerr << usage << " v=" << val << std::endl << std::endl;;
    };
    
    if (type == 1) // Type 1 == axes ???
    {
        if (page == kHIDPage_GenericDesktop)
        {
            switch (usage)
            {
                case kHIDUsage_GD_X:  device->X_axis = fval; break;
                case kHIDUsage_GD_Y:  device->Y_axis = fval; break;
                case kHIDUsage_GD_Z:  device->Z_axis = fval; break;
                case kHIDUsage_GD_Rx: device->V_axis = fval; break; // Not sure about this one
                case kHIDUsage_GD_Ry: device->R_axis = fval; break; // and this one
                case kHIDUsage_GD_Rz: device->U_axis = fval; break;
                case kHIDUsage_GD_Hatswitch:
                {
                    if (min == 0 && max == 7)
                        device->hatValue = val;
                    else if (min == 1 && max == 8)
                        device->hatValue = val - 1;
                    else
                    {
                        std::cerr << "Unhandled hat range " << min << " .. " << max << std::endl;
                        err();
                        device->hatValue = HAT_MID;
                    }
                }
                break;
                    
                default:
                    std::cerr << "Unhandled usage" << std::endl;
                    err();
            }
        }
        else if (page == kHIDPage_Simulation)
        {
            float fval = 200.f * ((val - min) / static_cast<float>(max - min)) - 100.f;
            switch (usage)
            {
                case kHIDUsage_Sim_Accelerator:  device->V_axis = fval; break;
                case kHIDUsage_Sim_Brake:        device->R_axis = fval; break;
                default:
                    std::cerr << "Unhandled usage" << std::endl;
                    err();
            }
        }
        else
        {
            std::cerr << "Unhandled page" << std::endl;
            err();
        }
    }
    else if (type == 2) // Type 2 == buttons ???
    {
        if (page == 0x09) // Buttons page
        {
            if (usage >= kHIDUsage_Button_1 && usage <= kHIDUsage_Button_32)
            {
                if (val)
                    device->buttons |= (1 << (usage-1));
                else
                    device->buttons &= ~(1 << (usage-1));
            }
            else
            {
                std::cerr << "Unhandled usage" << std::endl;
                err();
            }
        }
        else
        {
            std::cerr << "Unhandled page" << std::endl;
            err();
        }
    }
    else
    {
        std::cerr << "Unhandled type" << std::endl;
        err();
    }

}

void deviceAttached(void* ctx, IOReturn result, void* sender, IOHIDDeviceRef device)
{
    for (int i = 0; i < sf::Joystick::Count; i++)
    {
        if (devices[i].ref == nullptr)
        {

            auto* name = reinterpret_cast<NSString*>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductKey)));
            auto* vid  = reinterpret_cast<NSNumber*>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDVendorIDKey)));
            auto* pid  = reinterpret_cast<NSNumber*>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductIDKey)));
            
            devices[i].ref = device;
            devices[i].ident.name = sf::String([name UTF8String]);
            devices[i].ident.vendorId = static_cast<int>([vid integerValue]);
            devices[i].ident.productId = static_cast<int>([pid integerValue]);
                        
            IOHIDDeviceOpen(device, kIOHIDOptionsTypeNone);
            IOHIDDeviceScheduleWithRunLoop(device, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);
            IOHIDDeviceRegisterInputValueCallback(device, deviceInput, &devices[i]);
            
            // TODO force read values if possible rather than using defaults
            devices[i].hatValue = HAT_MID;

            break;
        }
    }
}

void deviceDetached(void* ctx, IOReturn result, void* sender, IOHIDDeviceRef device)
{
    for (int i = 0; i < sf::Joystick::Count; i++)
    {
        if (devices[i].ref == device)
        {
            IOHIDDeviceClose(device, kIOHIDOptionsTypeNone);
            devices[i].ref = nullptr;
            devices[i].ident = {};
            break;
        }
    }
}

void init()
{
    hidManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    if (hidManager == nullptr) {
        std::cerr << "Could not initialize Gamepads" << std::endl;
    }
    auto matcher = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
    if (matcher == nullptr) {
        std::cerr << "Could not initialize Gamepads" << std::endl;
    }
    
    addDictionary(matcher, kHIDPage_GenericDesktop, kHIDUsage_GD_Joystick);
    addDictionary(matcher, kHIDPage_GenericDesktop, kHIDUsage_GD_GamePad);
    
    IOHIDManagerSetDeviceMatchingMultiple(hidManager, matcher);
    CFRelease(matcher);
    
    IOHIDManagerRegisterDeviceMatchingCallback(hidManager, deviceAttached, nullptr);
    IOHIDManagerRegisterDeviceRemovalCallback(hidManager, deviceDetached, nullptr);
    
    IOHIDManagerScheduleWithRunLoop(hidManager, CFRunLoopGetMain(), kCFRunLoopCommonModes);
    IOHIDManagerOpen(hidManager, kIOHIDOptionsTypeNone);
}

void release()
{
    for (int i = 0; i < sf::Joystick::Count; i++)
        IOHIDDeviceClose(devices[i].ref, kIOHIDOptionsTypeNone);
    
    if (hidManager)
    {
        IOHIDManagerUnscheduleFromRunLoop(hidManager, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);
        IOHIDManagerClose(hidManager, kIOHIDOptionsTypeNone);
        CFRelease(hidManager);
    }
}

} // namespace

namespace sf
{

priv::Controller* priv::Controller::_instance{nullptr};


priv::Controller* priv::Controller::instance()
{
    if (_instance == nullptr)
    {
        init();
        _instance = new Controller();
    }
    
    return _instance;
}

bool priv::Controller::isConnected(unsigned int gamepad)
{
    return devices[gamepad].ref != nullptr;
}

Joystick::Identification priv::Controller::getIdentification(unsigned int gamepad)
{
    if (gamepad < Joystick::Count)
        return devices[gamepad].ident;
    return {};
}

float priv::Controller::getAxisPosition(unsigned int gamepad, Joystick::Axis axis)
{
    auto& dev = devices[gamepad];
    if (dev.ref == nullptr)
        return 0.f;
    
    if (axis == Joystick::Axis::PovX)
    {
        switch (dev.hatValue)
        {
            case HAT_W:
            case HAT_NW:
            case HAT_SW:
                return -100.f;
            case HAT_E:
            case HAT_NE:
            case HAT_SE:
                return 100.f;
        }
    }
    else if (axis == Joystick::Axis::PovY)
    {
        switch (dev.hatValue)
        {
            case HAT_N:
            case HAT_NW:
            case HAT_NE:
                return -100.f;
            case HAT_S:
            case HAT_SW:
            case HAT_SE:
                return 100.f;
            default:
                return 0.f;
        }
    }
    else
    {
        switch (axis)
        {
            case Joystick::Axis::X: return dev.X_axis;
            case Joystick::Axis::Y: return dev.Y_axis;
            case Joystick::Axis::Z: return dev.Z_axis;
            case Joystick::Axis::U: return dev.U_axis;
            case Joystick::Axis::V: return dev.V_axis;
            case Joystick::Axis::R: return dev.R_axis;
            default:
                return 0.f;
        }
    }
    
    return 0.f;
}

bool priv::Controller::isButtonPressed(unsigned int gamepad, unsigned int button)
{
    auto& dev = devices[gamepad];
    if (dev.ref == nullptr)
        return false;
    
    if (button < 32)
        return dev.buttons & (1 << button);

    return false;
}

priv::Controller::Controller()
{
}

priv::Controller::~Controller()
{
    if (_instance != nullptr)
    {
        delete _instance;
        _instance = nullptr;
        release();
    }
}

}
