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

	/** Function that counts coins until user exits by pressing 'esc'.
	* This function will capture camera output, detect coins on a conveyor
	* belt and show the user which coins it is counting and the total amount
	* passed so far. 
	*/
	void coin_counter::count_coins(cv::Mat& inImg);

private:
/****************************** Private Attributes ***************************/
	std::vector<coin> m_countedCoins;
	cv::Mat m_backgroundImg;

/****************************** Private Methods ******************************/
    std::vector<coin> coin_counter::get_coins_in_img(cv::Mat& inputImg);
	std::vector<coin> coin_counter::remove_previous_coins(std::vector<coin>& coins);
    void coin_counter::add_new_coins( std::vector<coin>& coins );
    void coin_counter::update_coin_counts(cv::Mat& inImg);
	void coin_counter::addTitle(cv::Mat& img, std::string& title, cv::Scalar& color, int xLoc);
};

#endif //__COIN_COUNTER_H__
