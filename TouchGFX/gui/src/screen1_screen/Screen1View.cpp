#include <gui/screen1_screen/Screen1View.hpp>
#include "types.h"
#include "math.h"
int Flick = 0;
#define RADIUS_EARTH 6371000

Screen1View::Screen1View()
{

}

double radian(double degree){
    return  degree * (M_PI /180.0);
}

double haversine(double lon1, double lat1, double lon2, double lat2){ //po haversine formuli izracuna razdaljo med dvema tockama
    double phi_1 = radian(lat1);
    double phi_2 = radian(lat2);

    double delt_phi = radian(lat2-lat1);
    double delta_lambda = radian(lon2-lon1);

    double a = pow(sin(delt_phi/2.0),2) + cos(phi_1)*cos(phi_2) * pow(sin(delta_lambda/2.0), 2);

    double c = 2*atan2(sqrt(a), sqrt(1-a));

    return RADIUS_EARTH*c; //vrne razdaljo v metrih
}

double kompas_1(double lon1, double lat1, double lon2, double lat2){
	lon1 = radian(lon1);
	lat1 = radian(lat1);

	lon2 = radian(lon2);
	lat2 = radian(lat2);
	double delta_lon = lon2-lon1;

	double y = cos(lat2) * sin(delta_lon);
	double x = cos(lat1)*sin(lat2)-sin(lat1)*cos(lat2)*cos(delta_lon);

	double kompas = atan2(y, x);
	kompas = kompas*(180/M_PI) + 360;
	kompas = fmod(kompas, 360);
	return kompas; // bi moglo vrnet tako kot si hotu
}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::SetText(nav_t GPS_data, nav_t XBEE_data)
{
	/*int Izh_LAT = 460331829;
	int Izh_LON = 145277514;
	float Lat_to_px = 0.0190476;
	float Long_to_px = 0.0131355;
	int Delta_LAT, Delta_LON;

	Delta_LAT = Izh_LAT - Lat;
	Delta_LON = Izh_LON - Long;
	int m_x = Delta_LON*Long_to_px;
	int m_y = Delta_LAT*Lat_to_px;


	int ura[5] = {0};*/
	//Tim = 1312;

	int Lat = GPS_data.Lat;
	int Long = GPS_data.Long;
	int Alt = GPS_data.Alt;
	int Tim = GPS_data.Time;
	double V1 = GPS_data.V1; //GPS_data.V1;
	double V2 = GPS_data.V2; //GPS_data.V2;
	int SOG = GPS_data.SOG;
	int COG = GPS_data.COG;


	Unicode::UnicodeChar a[] = {0x00B0,0};

	//Unicode::snprintfFloat(LatBuffer, LAT_SIZE, "%f", (float(Lat))/10000000);
	//Unicode::snprintfFloat(LatitudeBuffer, LATITUDE_SIZE, "%f", (float(Lat))/10000000);
	//Latitude.resizeToCurrentText();
	//Latitude.invalidate();

	//Unicode::snprintfFloat(LongitudeBuffer, LONGITUDE_SIZE, "%f", (float(Long))/10000000);
	//Longitude.resizeToCurrentText();
	//Longitude.invalidate();

	Unicode::snprintfFloat(AltitudeBuffer, ALTITUDE_SIZE, "%.1f m", (float(Alt))/1000);
	//Altitude.resizeToCurrentText();
	Altitude.invalidate();


	Unicode::snprintf(TimeBuffer, TIME_SIZE, "%02d:%02d", (int)(Tim/100), (int)(Tim%100));
	//Time.resizeToCurrentText();
	Time.invalidate();

	Unicode::snprintfFloat(Voltage_1Buffer, VOLTAGE_1_SIZE, "%.1f", V1);
	Voltage_1.resizeToCurrentText();
	Voltage_1.invalidate();

	Unicode::snprintfFloat(Voltage_2Buffer, VOLTAGE_2_SIZE, "%.1f", V2);
	Voltage_2.resizeToCurrentText();
	Voltage_2.invalidate();

	if(GPS_data.Status == 1111){
		Unicode::snprintf(GPS_StatBuffer, GPS_STAT_SIZE, "NO GPS");
	}
	else if(GPS_data.Status == 1010){
		Unicode::snprintf(GPS_StatBuffer, GPS_STAT_SIZE, "GPS OK");
	}
	GPS_Stat.resizeToCurrentText();
	GPS_Stat.invalidate();

	Unicode::snprintf(SpeedBuffer, SPEED_SIZE, "%d km/h", SOG);
	//Speed.resizeToCurrentText();
	Speed.invalidate();

	Unicode::snprintf(DirectionBuffer, DIRECTION_SIZE, "%d%s", COG, a);
	//Direction.resizeToCurrentText();
	Direction.invalidate();


	//XBEE_data
	//Unicode::snprintfFloat(Latitude_1Buffer, LATITUDE_SIZE, "%f", (float(XBEE_data.Lat))/10000000);
	//Latitude.resizeToCurrentText();
	//Latitude_1.invalidate();

	//Unicode::snprintfFloat(Longitude_1Buffer, LONGITUDE_SIZE, "%f", (float(XBEE_data.Long))/10000000);
	//Longitude.resizeToCurrentText();
	//Longitude_1.invalidate();

	Unicode::snprintfFloat(Altitude_1Buffer, ALTITUDE_SIZE, "%0.1f m", (float(XBEE_data.Alt))/1000);
	//Altitude.resizeToCurrentText();
	Altitude_1.invalidate();

	//
	/*DODAJ PREVERJANJE ALI SO XBEE PODATKI OK*/
	//

	float Angle_GPS = kompas_1(Long/10000000.0, Lat/10000000.0, XBEE_data.Long/10000000.0, XBEE_data.Lat/10000000.0);

	Unicode::snprintf(Speed_1Buffer, SPEED_SIZE, "%d km/h", XBEE_data.SOG);
	//Speed_1.resizeToCurrentText();
	Speed_1.invalidate();

	Unicode::snprintf(Direction_1Buffer, DIRECTION_SIZE, "%d%s", (int)Angle_GPS, a);
	//Direction_1.resizeToCurrentText();
	Direction_1.invalidate();

	float Heading_angle = COG*0.0174532925;

	Kompas.updateAngles(0.000f, 0.000f, -Heading_angle);
	Kompas.invalidate();

	float AoA_angle = Angle_GPS*0.0174532925 - COG*0.0174532925;//kompas_1(Long/10000000.0, Lat/10000000.0, XBEE_data.Long/10000000.0, XBEE_data.Lat/10000000.0);//

	Kazalec.updateAngles(0.000f, 0.000f, AoA_angle);
	Kazalec.invalidate();

	double Distance_nm = 0;
	Distance_nm = haversine(Long/10000000.0, Lat/10000000.0, XBEE_data.Long/10000000.0, XBEE_data.Lat/10000000.0);
	Unicode::snprintfFloat(DistanceBuffer, DISTANCE_SIZE, "%0.2f m", (float)Distance_nm);
	Distance.invalidate();

	Unicode::snprintf(AoABuffer, AOA_SIZE, "%d%s", (int)(AoA_angle/0.0174532925), a);
	AoA.invalidate();

	Unicode::snprintf(VelocityBuffer, VELOCITY_SIZE, "%d km/h", XBEE_data.SOG);
	Velocity.invalidate();

	if(Flick == 1){
		circle1.setVisible(false);
		Flick = 0;
	}
	else{
		circle1.setVisible(true);
		Flick = 1;
	}
	circle1.invalidate();


	//image1.setXY((m_x+240), -(m_y-136));
	//image1.invalidate();




}
