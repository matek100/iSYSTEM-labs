/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef SCREEN1VIEWBASE_HPP
#define SCREEN1VIEWBASE_HPP

#include <gui/common/FrontendApplication.hpp>
#include <mvp/View.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/widgets/TextAreaWithWildcard.hpp>
#include <touchgfx/widgets/TextureMapper.hpp>
#include <touchgfx/widgets/canvas/Circle.hpp>
#include <touchgfx/widgets/canvas/PainterRGB565.hpp>
#include <touchgfx/widgets/TextArea.hpp>

class Screen1ViewBase : public touchgfx::View<Screen1Presenter>
{
public:
    Screen1ViewBase();
    virtual ~Screen1ViewBase() {}
    virtual void setupScreen();

protected:
    FrontendApplication& application() {
        return *static_cast<FrontendApplication*>(touchgfx::Application::getInstance());
    }

    /*
     * Member Declarations
     */
    touchgfx::Box __background;
    touchgfx::Box box1;
    touchgfx::TextAreaWithOneWildcard Altitude;
    touchgfx::Box box2;
    touchgfx::TextAreaWithOneWildcard Time;
    touchgfx::TextAreaWithOneWildcard Voltage_1;
    touchgfx::TextAreaWithOneWildcard Voltage_2;
    touchgfx::TextAreaWithOneWildcard GPS_Stat;
    touchgfx::TextAreaWithOneWildcard Speed;
    touchgfx::TextAreaWithOneWildcard Direction;
    touchgfx::TextAreaWithOneWildcard Altitude_1;
    touchgfx::TextAreaWithOneWildcard Speed_1;
    touchgfx::TextAreaWithOneWildcard Direction_1;
    touchgfx::TextureMapper Kompas;
    touchgfx::TextureMapper Kazalec;
    touchgfx::TextAreaWithOneWildcard Velocity;
    touchgfx::TextAreaWithOneWildcard AoA;
    touchgfx::TextAreaWithOneWildcard Distance;
    touchgfx::Circle circle1;
    touchgfx::PainterRGB565 circle1Painter;
    touchgfx::TextArea textArea1;
    touchgfx::TextArea textArea1_1;

    /*
     * Wildcard Buffers
     */
    static const uint16_t ALTITUDE_SIZE = 100;
    touchgfx::Unicode::UnicodeChar AltitudeBuffer[ALTITUDE_SIZE];
    static const uint16_t TIME_SIZE = 10;
    touchgfx::Unicode::UnicodeChar TimeBuffer[TIME_SIZE];
    static const uint16_t VOLTAGE_1_SIZE = 10;
    touchgfx::Unicode::UnicodeChar Voltage_1Buffer[VOLTAGE_1_SIZE];
    static const uint16_t VOLTAGE_2_SIZE = 10;
    touchgfx::Unicode::UnicodeChar Voltage_2Buffer[VOLTAGE_2_SIZE];
    static const uint16_t GPS_STAT_SIZE = 10;
    touchgfx::Unicode::UnicodeChar GPS_StatBuffer[GPS_STAT_SIZE];
    static const uint16_t SPEED_SIZE = 12;
    touchgfx::Unicode::UnicodeChar SpeedBuffer[SPEED_SIZE];
    static const uint16_t DIRECTION_SIZE = 10;
    touchgfx::Unicode::UnicodeChar DirectionBuffer[DIRECTION_SIZE];
    static const uint16_t ALTITUDE_1_SIZE = 100;
    touchgfx::Unicode::UnicodeChar Altitude_1Buffer[ALTITUDE_1_SIZE];
    static const uint16_t SPEED_1_SIZE = 12;
    touchgfx::Unicode::UnicodeChar Speed_1Buffer[SPEED_1_SIZE];
    static const uint16_t DIRECTION_1_SIZE = 10;
    touchgfx::Unicode::UnicodeChar Direction_1Buffer[DIRECTION_1_SIZE];
    static const uint16_t VELOCITY_SIZE = 20;
    touchgfx::Unicode::UnicodeChar VelocityBuffer[VELOCITY_SIZE];
    static const uint16_t AOA_SIZE = 20;
    touchgfx::Unicode::UnicodeChar AoABuffer[AOA_SIZE];
    static const uint16_t DISTANCE_SIZE = 20;
    touchgfx::Unicode::UnicodeChar DistanceBuffer[DISTANCE_SIZE];

private:

    /*
     * Canvas Buffer Size
     */
    static const uint16_t CANVAS_BUFFER_SIZE = 7200;
    uint8_t canvasBuffer[CANVAS_BUFFER_SIZE];
};

#endif // SCREEN1VIEWBASE_HPP
