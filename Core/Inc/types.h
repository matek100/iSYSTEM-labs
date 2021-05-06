/*
 * types.h
 *
 *  Created on: 20 Apr 2021
 *      Author: domen
 */

#ifndef APPLICATION_USER_TYPES_H_
#define APPLICATION_USER_TYPES_H_


typedef struct{
	int Time;
	int Lat;
	int Long;
	int Alt;
	int Status;
	int SOG;
	int COG;
	double V1;
	double V2;
}nav_t;


#endif /* APPLICATION_USER_TYPES_H_ */
