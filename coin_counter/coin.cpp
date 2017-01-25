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
	
void coin::add_prediction(COIN_TYPE_T type)
{
	//update counts
	switch (type) {
	case COIN_TYPE_QUARTER:
		m_numQ++;
		break;
	case COIN_TYPE_DIME:
		m_numD++;
		break;
	case COIN_TYPE_NICKLE:
		m_numN++;
		break;
	case COIN_TYPE_PENNY:
		m_numP++;
		break;
	}
	
	//update type
	if ((m_numQ >= m_numD) &&
		(m_numQ >= m_numN) &&
		(m_numQ >= m_numP))
		type = COIN_TYPE_QUARTER;
	else if ((m_numD >= m_numQ) &&
		(m_numD >= m_numN) &&
		(m_numD >= m_numP))
		type = COIN_TYPE_DIME;
	else if ((m_numN >= m_numQ) &&
		(m_numN >= m_numD) &&
		(m_numN >= m_numP))
		type = COIN_TYPE_NICKLE;
	else if ((m_numP >= m_numQ) &&
		(m_numP >= m_numD) &&
		(m_numP >= m_numN))
		type = COIN_TYPE_PENNY;
	else 
		type = COIN_TYPE_UNKNOWN;
}

COIN_TYPE_T coin::get_coin_type()
{
	return type;
}
	
void coin::set_location(cv::Point newLocation )
{
	location = newLocation;
}

cv::Point coin::get_location()
{
	return location;
}
