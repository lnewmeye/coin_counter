/******************************************************************************
* Class: Robotic Vision
* Author: James Swift, Luke Newmeyer
* Copyright 2017
******************************************************************************/
#ifndef __COIN_COUNTER_H__
#define __COIN_COUNTER_H__

#include "stdafx.h"

#include <vector>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "coin.h"

using std::vector;
using std::string;
using cv::Mat;
using cv::Scalar;
using cv::Point;
using cv::Rect;
using cv::Size;

// Debug windows
#define DEBUG_WINDOW_1 "Debug Window 1"
#define DEBUG_WINDOW_2 "Debug Window 2"
#define DEBUG_TRACKBAR_1 "Option 1"
#define DEBUG_TRACKBAR_2 "Option 2"

// Debug options: provide window debug should be written to
#define DEBUG // This should be uncommented if any of the options below are
//#define DEBUG_TRACK DEBUG_WINDOW_2
//#define DEBUG_THRESHOLD DEBUG_WINDOW_1
//#define DEBUG_ERODE DEBUG_WINDOW_1
//#define DEBUG_DILATE DEBUG_WINDOW_2
#define DEBUG_DISTANCE DEBUG_WINDOW_2

// Include iostream for debug (when defined)
#ifdef DEBUG
#include <iostream>
#endif

#define TRACK_LEFT 160 // Track edge left
#define TRACK_RIGHT 500 // Track edge Right

#define THRESHOLD_LEVEL 20 // Value between 0 and 255

#define MAX_DIFF_BETWEEN_FRAMES_X 20
#define MAX_DIFF_BETWEEN_FRAMES_Y -60

class coin_counter
{
public:

	int total = 0;
	int numQuarters = 0;
	int numDimes = 0;
	int numNickles = 0;
	int numPennies = 0;

	coin_counter();
	~coin_counter();

	void coin_counter::set_background(Mat background);
	void coin_counter::count_coins(Mat image);

private:
	// Private constatns
	const Scalar trackColor = Scalar(0, 0, 255);
	const Size erodeKernelSize = Size(30, 30);
	const Mat erodeKernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, erodeKernelSize);
	const Size dilateKernelSize = Size(5, 5);
	const Mat dilateKernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, dilateKernelSize);

	// Private variables
	Mat background; // Background image
	bool backgroundSet = false; // Background is set
	std::vector<coin> m_countedCoins; // Coins counted so far

	// Private functions
	vector<coin> coin_counter::find_coins(Mat image);
	vector<coin> coin_counter::remove_previous_coins(vector<coin>& coins);
	void coin_counter::draw_coins(Mat& imageIn);
	void coin_counter::set_roi(Mat imageIn, Mat& imageOut);
	void coin_counter::draw_track(Mat imageIn, Mat imageOut);
	void coin_counter::add_new_coins(vector<coin>& coins);

	/** Re-count all coins in the m_countedCoins property */
	void coin_counter::update_coin_counts(cv::Mat& inImg);

	/** Add a 'string' to the 'img' of 'color' at 'xLoc' on text row 'row' */
	void coin_counter::addTitle(cv::Mat& img, std::string& title, cv::Scalar& color, int xLoc, int row);
};


#endif //__COIN_COUNTER_H__