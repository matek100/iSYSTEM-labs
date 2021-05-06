#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include "types.h"

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    //virtual void SetText(uint32_t Lat, uint32_t Long, uint32_t Alt, uint32_t Tim);
    virtual void SetText(nav_t GPS_data, nav_t XBEE_data);

    protected:
};

#endif // SCREEN1VIEW_HPP
