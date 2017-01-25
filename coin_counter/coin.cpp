/******************************************************************************
* Class: Robotic Vision
* Author: James Swift
* Copyright 2017
******************************************************************************/

#include "coin.h"

/****************************** Definitions **********************************/
#define COIN_QUARTER_SIZE 25.0
#define COIN_DIME_SIZE 15.0
#define COIN_NICKLE_SIZE 21.0
#define COIN_PENNY_SIZE 18.0

/****************************** Private Variables ****************************/
/****************************** Private Functions ****************************/
/****************************** Function Implementation **********************/


coin::coin()
{
}

coin::~coin()
{
}
	
void coin::addPrediction(cv::Vec3f circle)
{
	//get circle info
	cv::Point circleCenter(cvRound(circle[0]), cvRound(circle[1]));
	int radius = cvRound(circle[2]);

	//make prediction (TODO: add function to make prediction)
	COIN_TYPE_T coinType = COIN_TYPE_QUARTER;

	//update center location
	center = circleCenter;

	//update if different 
	updateType(coinType);
}

COIN_TYPE_T coin::getCoinType()
{
	return type;
}

void coin::updateType(COIN_TYPE_T newlyDetectedType)
{
	//update counts
	switch (type) {
	case COIN_TYPE_QUARTER:
		numQ++;
		break;
	case COIN_TYPE_DIME:
		numD++;
		break;
	case COIN_TYPE_NICKLE:
		numN++;
		break;
	case COIN_TYPE_PENNY:
		numP++;
		break;
	}
	
	//update type
	if ((numQ >= numD) &&
		(numQ >= numN) &&
		(numQ >= numP))
		type = COIN_TYPE_QUARTER;
	else if ((numD >= numQ) &&
		(numD >= numN) &&
		(numD >= numP))
		type = COIN_TYPE_DIME;
	else if ((numN >= numQ) &&
		(numN >= numD) &&
		(numN >= numP))
		type = COIN_TYPE_NICKLE;
	else if ((numP >= numQ) &&
		(numP >= numD) &&
		(numP >= numN))
		type = COIN_TYPE_PENNY;
	else 
		type = COIN_TYPE_UNKNOWN;
}
