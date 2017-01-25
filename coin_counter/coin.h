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
	/** Current location of coin in image */
	cv::Point center;
	/** Coin Type */
	COIN_TYPE_T type;

	/** Constructor */
	coin();
	/** Destructor */
	~coin();

	/** Add coin prediction from circle*/
	void coin::addPrediction(cv::Vec3f circle);

	/** Get coin type */
	COIN_TYPE_T coin::getCoinType();

private:
	/** History of times this coin looked like a quarter */
	int numQ = 0;
	/** History of times this coin looked like a dime */
	int numD = 0;
	/** History of times this coin looked like a nickle */
	int numN = 0;
	/** History of times this coin looked like a penny */
	int numP = 0;
	/** Update type based on a newly detected type */
	void coin::updateType(COIN_TYPE_T newlyDetectedType);
};


#endif //__COIN_H__