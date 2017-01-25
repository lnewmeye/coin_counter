/******************************************************************************
* Class: Robotic Vision
* Author: James Swift
* Copyright 2017
******************************************************************************/
#ifndef __COIN_H__
#define __COIN_H__

#include "opencv2\opencv.hpp"
#include "opencv\highgui.h"
#include "typedefs.h"

typedef enum COIN_TYPE_E {
	COIN_TYPE_QUARTER = 0,
	COIN_TYPE_DIME,
	COIN_TYPE_NICKLE,
	COIN_TYPE_PENNY,
	COIN_TYPE_UNKNOWN,
	COIN_TYPE_NUMS
} COIN_TYPE_T;

class coin
{
public:
	/** Constructor */
	coin();
	/** Destructor */
	~coin();

	/** Add coin prediction from circle*/
	void coin::add_prediction(COIN_TYPE_T type);

	/** Get coin type */
	COIN_TYPE_T coin::get_coin_type();

	/** Set coin location */
	void coin::set_location(cv::Point newLocation );

	/** Get coin location */
	cv::Point coin::get_location();

private:
	/** Current location of coin in image */
	cv::Point location;
	/** Coin Type */
	COIN_TYPE_T type;
	/** History of times this coin looked like a quarter */
	int m_numQ = 0;
	/** History of times this coin looked like a dime */
	int m_numD = 0;
	/** History of times this coin looked like a nickle */
	int m_numN = 0;
	/** History of times this coin looked like a penny */
	int m_numP = 0;
};

#endif //__COIN_H__