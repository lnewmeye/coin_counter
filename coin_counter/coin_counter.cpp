/******************************************************************************
* Class: Robotic Vision
* Author: James Swift
* Copyright 2017
******************************************************************************/
#include "coin_counter.h"
#include "coin.h"

using namespace cv;
using namespace std;

/****************************** Definitions **********************************/
#define WINDOW_NAME "Coin Counter"
#define QUARTER_SIZE 25.0
#define DIME_SIZE 15.0
#define NICKLE_SIZE 21.0
#define PENNY_SIZE 18.0

/****************************** Private Variables ****************************/
/****************************** Function Implementation **********************/

coin_counter::coin_counter()
{
}

coin_counter::~coin_counter()
{
}
	
void coin_counter::set_background(cv::Mat bkgndImg)
{
	//blur image and set as background
	GaussianBlur(bkgndImg, m_backgroundImg, Size(5, 5), 2, 2);
	imshow("Coin Counter", m_backgroundImg);
	waitKey();
}

void coin_counter::count_coins(cv::Mat inImg)
{
    //get coins found in image
    vector<coin> imgCoins = get_coins_in_img( inImg );

    //remove coins we've already found
	remove_previous_coins( imgCoins );

    //add remaining coins to coins found
    add_new_coins( imgCoins );

    //update coin count
	update_coin_counts( inImg );
}

std::vector<coin> coin_counter::get_coins_in_img(cv::Mat inputImg)
{
    //create a vector of coins
    vector<coin> coinsInImg;
    
    //extract a specific section of the video
	Rect roi = Rect(Point(inputImg.cols / 6, 0), Point((inputImg.cols * 5) / 6, inputImg.rows));
	Mat roiImgi = inputImg(roi);
	Mat roiImgb = m_backgroundImg(roi);

	//subtract background
	Mat roiImg;
	absdiff(roiImgi, roiImgb, roiImg);
	imshow(WINDOW_NAME, roiImg);
	waitKey();

	//change to bw
	Mat grayImg;
	cvtColor(roiImg, grayImg, CV_BGR2GRAY);
	imshow(WINDOW_NAME, grayImg);
	waitKey();

	// Reduce the noise so we avoid false circle detection
	GaussianBlur(grayImg, grayImg, Size(5, 5), 10, 10);

	//threshold
	Mat thresh;
	threshold(grayImg, grayImg, 60, 255, THRESH_BINARY);
	imshow("ThreshImg", grayImg);
	waitKey();

    //add found coins to this variable (like this)
    coin newCoin;
    coinsInImg.push_back(newCoin);

    //return all coins found
    return coinsInImg;
}

/****************************** Private Method Implementation ****************/

void coin_counter::remove_previous_coins( std::vector<coin> coins )
{
    //TODO: Update previously found coins locations
	//TODO: Remove previously found coins from the 'coins' vector. 
}
    
void coin_counter::add_new_coins( std::vector<coin> coins )
{
	//add new coins (if none, nothing will happen here)
	for (coin newCoin : coins ) {
		//add to set of current coins
		m_countedCoins.push_back(newCoin);
	}
}

void coin_counter::update_coin_counts(cv::Mat inImg)
{
	numQuarters = 0;
	numDimes = 0;
	numNickles = 0;
	numPennies = 0;
	for (coin coin : m_countedCoins) {
		switch (coin.get_coin_type() ) {
		case COIN_TYPE_QUARTER:
			numQuarters++;
			break;
		case COIN_TYPE_DIME:
			numDimes++;
			break;
		case COIN_TYPE_NICKLE:
			numNickles++;
			break;
		case COIN_TYPE_PENNY:
			numPennies++;
			break;
		}
	}
	total = numQuarters*25 + numDimes*10 + numNickles*5 + numPennies;
	
    //add text to screen
	char text[255];
	sprintf(text, "Quarters %d", numQuarters);
	addTitle(inImg, text, Scalar(255, 255, 255), 0);
	sprintf(text, "Total $%d.%2d", total / 100, total % 100);
	addTitle(inImg, text, Scalar(255, 255, 255), inImg.cols / 2);

    //TODO: show image (should this be here???)
	imshow(WINDOW_NAME, inImg);
}

void coin_counter::addTitle(Mat img, string title, Scalar color, int xLoc)
{
	//setup parameters
	int fontFace = FONT_HERSHEY_COMPLEX_SMALL;
	double fontScale = 1;
	int thickness = 1;

	//find acceptable location
	int baseline = 0;
	Size textSize = getTextSize(title, fontFace,
		fontScale, thickness, &baseline);
	Point textOrg(xLoc, textSize.height);

	//put text on image
	putText(img, title, textOrg, fontFace, fontScale, color, thickness, 8);
}
