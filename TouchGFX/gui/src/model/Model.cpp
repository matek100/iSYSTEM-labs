#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <stm32h750b_discovery.h>
#include "types.h"

extern "C"{
	extern nav_t Get_GPS_1_data(void);
	extern nav_t Get_XBEE_data(void);
}


Model::Model() : modelListener(0), Count(0)
{

}

void Model::tick()
{
#ifndef SIMULATOR
	nav_t GPS_1_data = Get_GPS_1_data();
	nav_t XBEE_data = Get_XBEE_data();
	modelListener->SetText(GPS_1_data, XBEE_data);
	//modelListener->SetText(Latitude, Longitude, Altitude, Tim);
#endif /*SIMULATOR*/
}
