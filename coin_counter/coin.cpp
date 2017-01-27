/******************************************************************************
* Class: Robotic Vision
* Author: James Swift
* Copyright 2017
******************************************************************************/

#include "coin.h"

/****************************** Definitions **********************************/
/****************************** Private Variables ****************************/
/****************************** Private Functions ****************************/
/****************************** Function Implementation **********************/

coin::coin()
{
}

coin::~coin()
{
}
	
void coin::add_area(int area)
{
	areaHist.push_back(area);
}

COIN_TYPE_T coin::get_coin_type()
{
	//get average area over all predictions
	double area = 0;
	for (int a : areaHist) {
		area += (double)a;
	}
	area /= (double)areaHist.size();

	if (area > QUARTER_SIZE_MIN)
		type = COIN_TYPE_QUARTER;
	else if (area > NICKLE_SIZE_MIN)
		type = COIN_TYPE_NICKLE;
	else if (area > PENNY_SIZE_MIN)
		type = COIN_TYPE_PENNY;
	else if (area > DIME_SIZE_MIN)
		type = COIN_TYPE_DIME;
	else
		type = COIN_TYPE_UNKNOWN;

	return type;
}
	
int coin::get_avg_area()
{
	//get average area over all predictions
	double area = 0;
	for (int a : areaHist) {
		area += (double)a;
	}
	area /= (double)areaHist.size();

	return (int)area;
}
	
void coin::set_location(cv::Point newLocation )
{
	location = newLocation;
}

cv::Point coin::get_location()
{
	return location;
}
