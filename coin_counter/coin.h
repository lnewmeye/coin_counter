/******************************************************************************
* Class: Robotic Vision
* Author: James Swift
* Copyright 2017
******************************************************************************/
#ifndef __COIN_H__
#define __COIN_H__

#include "stdafx.h"

#include <opencv2/core.hpp>
//#include "opencv2\opencv.hpp"
//#include "opencv\highgui.h"
#include "typedefs.h"

/****************************** Definitions **********************************/
/*
#define LARGER_THAN_COIN 5100.0
#define QUARTER_SIZE_MIN 3900.0
#define NICKLE_SIZE_MIN 2900.0
#define PENNY_SIZE_MIN 1700.0
#define DIME_SIZE_MIN 700.0
*/
#define QUARTER_SIZE_MAX 750
#define QUARTER_SIZE_MIN 450
#define NICKLE_SIZE_MIN 240
#define PENNY_SIZE_MIN 120
#define DIME_SIZE_MIN 70

typedef enum COIN_TYPE_E {
	COIN_TYPE_QUARTER = 0,
	COIN_TYPE_DIME,
	COIN_TYPE_NICKLE,
	COIN_TYPE_PENNY,
	COIN_TYPE_UNKNOWN,
	COIN_TYPE_NUMS
} COIN_TYPE_T;

/****************************** Class Definition *****************************/
class coin
{
public:
	/** Constructor */
	coin();
	/** Destructor */
	~coin();

	/** Add coin prediction from circle*/
	void coin::add_area(int area);

	/** Get coin type */
	COIN_TYPE_T coin::get_coin_type();

	/** Set coin location */
	void coin::set_location(cv::Point newLocation );

	/** Get coin location */
	cv::Point coin::get_location();
	
	/*** Debug: get avg area */
	int coin::get_avg_area();

private:
	/** Current location of coin in image */
	cv::Point location;
	/** Coin Type */
	COIN_TYPE_T type;
	/** Coin area history */
	std::vector<int> areaHist;
};

#endif //__COIN_H__