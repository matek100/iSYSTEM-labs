#include <gui/screen1_screen/Screen1View.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include "types.h"

Screen1Presenter::Screen1Presenter(Screen1View& v)
    : view(v)
{

}

void Screen1Presenter::activate()
{

}

void Screen1Presenter::deactivate()
{

}

/*void Screen1Presenter::SetText(uint32_t Lat, uint32_t Long, uint32_t Alt, uint32_t Tim)
{
	view.SetText(Lat, Long, Alt, Tim);
}*/

void Screen1Presenter::SetText(nav_t GPS_data, nav_t XBEE_data)
{
	view.SetText(GPS_data, XBEE_data);
}

