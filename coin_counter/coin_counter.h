/******************************************************************************
* Class: Robotic Vision
* Author: James Swift
* Copyright 2017
******************************************************************************/
#ifndef __COIN_COUNTER_H__
#define __COIN_COUNTER_H__

#define HOUGH_THRESH1 60
#define HOUGH_THRESH2 20

#include "opencv2\opencv.hpp"
#include "opencv\highgui.h"

#include "typedefs.h"
#include "coin.h"

class coin_counter
{
public:
	/****************************** Attributes ***********************************/
	int total = 0;
	int numQuarters = 0;
	int numDimes = 0;
	int numNickles = 0;
	int numPennies = 0;

	/****************************** Methods **************************************/
		/** Constructor */
	coin_counter();

	/** Destructor */
	~coin_counter();

	/** Set background (used for subtracing subsequent frames */
	void coin_counter::set_background(cv::Mat& bkgndImg);

	/** Function that counts coins. Images must be fed in 1 by 1.
	* set_background must have been previously called.
	*/
	void coin_counter::count_coins(cv::Mat& inImg);

private:
	/****************************** Private Attributes ***************************/
	/** Coins we have counted so far */
	std::vector<coin> m_countedCoins;

	/** Background image to subract all subsequent images with */
	cv::Mat m_backgroundImg;

	/** background is set */
	bool m_backgroundSet = false;

	/****************************** Private Methods ******************************/
	/** Get the coins in the image */
	std::vector<coin> coin_counter::get_coins_in_img(cv::Mat& inputImg);
	
	/** Remove found coins from 'coins' vector, update found coins new locations */
	std::vector<coin> coin_counter::remove_previous_coins(std::vector<coin>& coins);

	/** Add new coins to the counted coins */
	void coin_counter::add_new_coins(std::vector<coin>& coins);
	
	/** Re-count all coins in the m_countedCoins property */
	void coin_counter::update_coin_counts(cv::Mat& inImg);

	/** Add a 'string' to the 'img' of 'color' at 'xLoc' on text row 'row' */
	void coin_counter::addTitle(cv::Mat& img, std::string& title, cv::Scalar& color, int xLoc, int row);
};

#endif //__COIN_COUNTER_H__
