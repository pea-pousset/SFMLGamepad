#include <SFML/Graphics.hpp>
#include <SFMLGamepad/Gamepad.hpp>

#ifdef __APPLE__
extern const std::string& resourcePath();
#else
const std::string resourcePath() { return {}; }
#endif

class ControlState final : public sf::Drawable, public sf::Transformable
{
public:
    static constexpr int analogWidth = 150;
    static constexpr int analogXorg = 120;
    static constexpr int booleanXorg = analogXorg + analogWidth + 7;
    static constexpr int height = 16;

    ControlState()
    {
        font.loadFromFile(resourcePath() + "coolvetica rg.otf");
        text.setFont(font);
        text.setCharacterSize(15);
        text.setFillColor(sf::Color::White);

        analog = sf::VertexArray(sf::Quads, 4);
        analog[0].position = sf::Vector2f(1 + analogXorg,               1);
        analog[1].position = sf::Vector2f(1 + analogXorg + analogWidth, 1);
        analog[2].position = sf::Vector2f(1 + analogXorg + analogWidth, 1 + height);
        analog[3].position = sf::Vector2f(1 + analogXorg,               1 + height);

        boolean = sf::VertexArray(sf::Quads, 4);
        boolean[0].position = sf::Vector2f(1 + booleanXorg,          1);
        boolean[1].position = sf::Vector2f(1 + booleanXorg + height, 1);
        boolean[2].position = sf::Vector2f(1 + booleanXorg + height, 1 + height);
        boolean[3].position = sf::Vector2f(1 + booleanXorg,          1 + height);

        analogContour = sf::VertexArray(sf::LinesStrip, 5);
        analogContour[0].position = sf::Vector2f(0 + analogXorg,               0);
        analogContour[1].position = sf::Vector2f(2 + analogXorg + analogWidth, 0);
        analogContour[2].position = sf::Vector2f(2 + analogXorg + analogWidth, 2 + height);
        analogContour[3].position = sf::Vector2f(0 + analogXorg,               2 + height);
        analogContour[4].position = analogContour[0].position;

        booleanContour = sf::VertexArray(sf::LinesStrip, 5);
        booleanContour[0].position = sf::Vector2f(0 + booleanXorg,          0);
        booleanContour[1].position = sf::Vector2f(2 + booleanXorg + height, 0);
        booleanContour[2].position = sf::Vector2f(2 + booleanXorg + height, 2 + height);
        booleanContour[3].position = sf::Vector2f(0 + booleanXorg,          2 + height);
        booleanContour[4].position = booleanContour[0].position;

        for (int i = 0; i < 4; i++)
        {
            analog[i].color = sf::Color::Green;
            boolean[i].color = sf::Color::Red;
            analogContour[i].color = sf::Color::White;
            booleanContour[i].color = sf::Color::White;
        }
    }

    void setValue(const std::string& txt, float analogValue, bool booleanValue)
    {
        text.setString(txt);
        analog[1].position = sf::Vector2f(1 + analogXorg + analogWidth * (analogValue / 100.f), 1);
        analog[2].position = sf::Vector2f(1 + analogXorg + analogWidth * (analogValue / 100.f), 1 + height);
        for (int i = 0; i < 4; i++)
            boolean[i].color = booleanValue ? sf::Color::Red : sf::Color::Transparent;
    }

private:
    sf::Text        text;
    sf::VertexArray analog;
    sf::VertexArray boolean;
    sf::VertexArray analogContour;
    sf::VertexArray booleanContour;
    sf::Font        font;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform *= getTransform();
        target.draw(text, states);
        target.draw(analog, states);
        target.draw(boolean, states);
        target.draw(analogContour, states);
        target.draw(booleanContour, states);
    }
};

struct ControlInfos
{
    const std::string name;
    sf::Gamepad::Control control;
    sf::Vector2f position;
};

#include <iostream>
#include <filesystem>
int main()
{
    const ControlInfos controlInfos[] =
    {
        { "A",              sf::Gamepad::Control::A,             sf::Vector2f(10,   5) },
        { "B",              sf::Gamepad::Control::B,             sf::Vector2f(10,  30) },
        { "X",              sf::Gamepad::Control::X,             sf::Vector2f(10,  55) },
        { "Y",              sf::Gamepad::Control::Y,             sf::Vector2f(10,  80) },
        { "Back",           sf::Gamepad::Control::Back,          sf::Vector2f(10, 105) },
        { "Start",          sf::Gamepad::Control::Start,         sf::Vector2f(10, 130) },
        { "Guide",          sf::Gamepad::Control::Guide,         sf::Vector2f(10, 155) },
        { "Up",             sf::Gamepad::Control::Up,            sf::Vector2f(10, 180) },
        { "Down",           sf::Gamepad::Control::Down,          sf::Vector2f(10, 205) },
        { "Left",           sf::Gamepad::Control::Left,          sf::Vector2f(10, 230) },
        { "Right",          sf::Gamepad::Control::Right,         sf::Vector2f(10, 255) },
        { "Left stick",     sf::Gamepad::Control::LeftStick,     sf::Vector2f(10, 280) },
        { "Right stick",    sf::Gamepad::Control::RightStick,    sf::Vector2f(10, 305) },
        { "Left shoulder",  sf::Gamepad::Control::LeftShoulder,  sf::Vector2f(10, 330) },
        { "Right shoulder", sf::Gamepad::Control::RightShoulder, sf::Vector2f(10, 355) },

        { "Left trigger",   sf::Gamepad::Control::LeftTrigger,   sf::Vector2f(335,   5) },
        { "Right trigger",  sf::Gamepad::Control::RightTrigger,  sf::Vector2f(335,  30) },
        { "Left X-",        sf::Gamepad::Control::LeftXMinus,    sf::Vector2f(335,  55) },
        { "Left X+",        sf::Gamepad::Control::LeftXPlus,     sf::Vector2f(335,  80) },
        { "Left Y-",        sf::Gamepad::Control::LeftYMinus,    sf::Vector2f(335, 105) },
        { "Left Y+",        sf::Gamepad::Control::LeftYPlus,     sf::Vector2f(335, 130) },
        { "Right X-",       sf::Gamepad::Control::RightXMinus,   sf::Vector2f(335, 155) },
        { "Right X+",       sf::Gamepad::Control::RightXPlus,    sf::Vector2f(335, 180) },
        { "Right Y-",       sf::Gamepad::Control::RightYMinus,   sf::Vector2f(335, 205) },
        { "Right Y+",       sf::Gamepad::Control::RightYPlus,    sf::Vector2f(335, 230) },
        { "Touchpad",       sf::Gamepad::Control::Touchpad,      sf::Vector2f(335, 255) },
        { "Paddle 1",       sf::Gamepad::Control::Paddle1,       sf::Vector2f(335, 280) },
        { "Paddle 2",       sf::Gamepad::Control::Paddle2,       sf::Vector2f(335, 305) },
        { "Paddle 3",       sf::Gamepad::Control::Paddle3,       sf::Vector2f(335, 330) },
        { "Paddle 4",       sf::Gamepad::Control::Paddle4,       sf::Vector2f(335, 355) },
        { "Misc 1",         sf::Gamepad::Control::Misc1,         sf::Vector2f(335, 380) },
    };

    sf::Gamepad::loadMappingFromFile(resourcePath() + "gamecontrollerdb.txt");
    sf::RenderWindow window(sf::VideoMode(640, 480), "SFMLGamepad Test", sf::Style::Titlebar | sf::Style::Close);
    sf::Font font;
    sf::Text connectionStatus;
    sf::Text helpText;

    int gamepad = 0;

    window.setVerticalSyncEnabled(true);
    font.loadFromFile(resourcePath() + "coolvetica rg.otf");
    connectionStatus.setCharacterSize(15);
    connectionStatus.setPosition(10, 460);
    connectionStatus.setFont(font);

    helpText.setCharacterSize(15);
    helpText.setPosition(10, 440);
    helpText.setFont(font);
    helpText.setFillColor(sf::Color::White);
    helpText.setString("Use left/right arrows to choose the controller");

    ControlState state;
    while (window.isOpen())
    {
         sf::Event event;
         while (window.pollEvent(event))
         {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Right)
                    gamepad = (gamepad + 1) % 8;
                else if (event.key.code == sf::Keyboard::Left)
                    gamepad = (8 + gamepad - 1) % 8;
            }
         }

        if (sf::Gamepad::isAvailable(gamepad))
        {
            connectionStatus.setFillColor(sf::Color::Green);
            connectionStatus.setString(std::to_string(gamepad) + ": " + sf::Joystick::getIdentification(gamepad).name);
        }
        else
        {
            connectionStatus.setFillColor(sf::Color::Red);
            connectionStatus.setString("Gamepad " + std::to_string(gamepad) + " unavailable");
        }

        window.clear();
        for (auto& ci : controlInfos)
        {
            state.setValue(ci.name,
                           sf::Gamepad::getPosition(gamepad, ci.control),
                           sf::Gamepad::isPressed(gamepad, ci.control));
            state.setPosition(ci.position);
            window.draw(state);
        }
        window.draw(helpText);
        window.draw(connectionStatus);
        window.display();
    }

    return EXIT_SUCCESS;
}
