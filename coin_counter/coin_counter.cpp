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
/** Debug: Window name to open */
#define WINDOW_NAME "Coin Counter"

/** Y locations for the region of interest (where the track is in the image) */
#define ROI_LEFT 130
#define ROI_RIGHT 510

/** Initial threshold value */
#define THRES_INIT 10

/** Maximim a coin can move in x and y */
#define MAX_DIFF_BETWEEN_FRAMES_X 20
#define MAX_DIFF_BETWEEN_FRAMES_Y -60

/****************************** Private Variables ****************************/
static Mat roiImg;
/****************************** Function Implementation **********************/

coin_counter::coin_counter()
{
}

coin_counter::~coin_counter()
{
}
	
void coin_counter::set_background(cv::Mat& bkgndImg)
{
	//blur image and set as background
	GaussianBlur(bkgndImg, m_backgroundImg, Size(5, 5), 2, 2);
	m_backgroundSet = true;
}

void coin_counter::count_coins(cv::Mat& inImg)
{
	if (m_backgroundSet) {
		//get coins found in image
		vector<coin> imgCoins = get_coins_in_img(inImg);

		//remove coins we've already found
		imgCoins = remove_previous_coins(imgCoins);

		//add remaining coins to coins found
		add_new_coins(imgCoins);

		//Draw coins on screen
		for (coin coin : m_countedCoins) {
			int hWidth = sqrt(coin.get_avg_area()) / 2; //half width
			Point tl = Point(
				coin.get_location().x - hWidth + ROI_LEFT,
				coin.get_location().y - hWidth);
			Point br = Point(
				coin.get_location().x + hWidth + ROI_LEFT + 1,
				coin.get_location().y + hWidth + 1);

			Scalar red = Scalar(0, 0, 255);
			Scalar green = Scalar(0, 255, 0);
			Scalar blue = Scalar(255, 0, 0);
			Scalar yellow = Scalar(0, 255, 255);
			Scalar purple = Scalar(255, 0, 255);
			Scalar color = Scalar(255, 255, 255);

			if (coin.get_coin_type() == COIN_TYPE_QUARTER)
				color = green;
			else if (coin.get_coin_type() == COIN_TYPE_NICKLE)
				color = yellow;
			else if (coin.get_coin_type() == COIN_TYPE_PENNY)
				color = blue;
			else if (coin.get_coin_type() == COIN_TYPE_DIME)
				color = purple;

			rectangle(inImg, Rect(tl, br),
				color,
				3, //thickness
				8, //connection type
				0);
		}

		//update coin count
		update_coin_counts(inImg);
	}
	else {
		//lets just go ahead and assume this is the first image
		set_background(inImg);
	}
}

std::vector<coin> coin_counter::get_coins_in_img(cv::Mat& inputImg)
{
    //create a vector of coins
    vector<coin> coinsInImg;
    
    //extract a specific section of the video
	Rect roi = Rect(Point(ROI_LEFT, 0), Point(ROI_RIGHT, inputImg.rows));
	//cout << roi << "\n";
	Mat roiImgi = inputImg(roi);
	Mat roiImgb = m_backgroundImg(roi);

	//subtract background
	absdiff(roiImgi, roiImgb, roiImg);

	//get red channel
	Mat grayImg;
	extractChannel(roiImg, grayImg, 2);

	//threshold
	Mat thresh;
	threshold(grayImg, grayImg, THRES_INIT, 255, THRESH_BINARY);
	//imshow("ThreshImg", grayImg);

	//distance transform
	int i = 20;
	Mat distImg;
	distanceTransform(grayImg, distImg, DIST_L2, DIST_MASK_5);
	threshold(distImg, distImg, i, 255, THRESH_BINARY);
	distImg.convertTo(distImg, CV_8U);
	//imshow("Dist Demo", distImg);
	//waitKey();

	//get connected components
	Mat labelImg;
	Mat stats;
	Mat centroids;
	cv::connectedComponentsWithStats(distImg, labelImg, stats, centroids, 8, 4);

	//for each connected component not in the background (0)
	for (int i = 1; i < stats.rows; i++) {
		//get area
		int area = stats.at<int>(i, CC_STAT_AREA);
		cout << "area: " << area << "\n";
		int x = (int)centroids.at<double>(i, 0);
		int y = (int)centroids.at<double>(i, 1);
		//only look at coins that are fully in the image
		if (y < ((roiImg.rows * 3)/4)) {
			//add found coins to this variable (like this)
			coin newCoin;
			newCoin.add_area(area);
			newCoin.set_location(Point(x, y));
			coinsInImg.push_back(newCoin);
			//cout << "area: " << area << "\n";
		}
	}

    //return all coins found
    return coinsInImg;
}

/****************************** Private Method Implementation ****************/

std::vector<coin> coin_counter::remove_previous_coins( std::vector<coin>& coins )
{
	vector<coin> newCoins;
	bool found;
	Point pixelsMoved = Point(0,0);
	int num = 0;

	//for all new coins
	for (coin newCoin : coins) {
		// see if any current coins are within 50 pixels
		found = false;
		for (coin cCoin : m_countedCoins) {
			//check distance bewteen the two (negative y is up)
			Point dist = newCoin.get_location() - cCoin.get_location();
			if ( (abs(dist.x) < MAX_DIFF_BETWEEN_FRAMES_X) &&
			( dist.y > MAX_DIFF_BETWEEN_FRAMES_Y ) &&
			( dist.y < 0 ) ) {
				//add to average distance it moved
				pixelsMoved += dist;
				//cout << "moved: " << dist << "\n";
				num++;
				//also update his area guess
				cCoin.add_area(newCoin.get_avg_area());
				found = true;
				break;
			}
		}
		if (!found)
			newCoins.push_back(newCoin);
	}

	if (num > 0) 
		//pixelsMoved /= (float)num;
		pixelsMoved /= (float)num;
	else 
		pixelsMoved = Point(0,-50); //total guess

	//update distance of all coins
	//for (coin cCoin : m_countedCoins) {
		for (vector<coin>::iterator it = m_countedCoins.begin();
			it != m_countedCoins.end(); ++it) {
	//for (int i = 0; i < m_countedCoins.size(); i++ ) {
		//coin cCoin = m_countedCoins.at(i);
		//only move in y
		Point currLoc = (*it).get_location();
		//cout << "Curr: " << currLoc << "\n";
		(*it).set_location(currLoc + pixelsMoved);
		currLoc = (*it).get_location();
		//cout << "New : " << currLoc << "\n";
	}
	/*
	for (coin cCoin : newCoins) {
		//only move in y
		Point currLoc = cCoin.get_location();
		//cout << "Curr: " << currLoc << "\n";
		cCoin.set_location(currLoc + pixelsMoved);
		//cout << "New : " << currLoc << "\n";
	}
	*/

	return newCoins;
}
    
void coin_counter::add_new_coins( std::vector<coin>& coins )
{
	//add new coins (if none, nothing will happen here)
	for (coin newCoin : coins ) {
		//add to set of current coins
		m_countedCoins.push_back(newCoin);
	}
}

void coin_counter::update_coin_counts(cv::Mat& inImg)
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
	
	//add number of coins to screen
	char text[255];
	sprintf(text, "Quarters %d", numQuarters);
	addTitle(inImg, (string) text, Scalar(0, 0, 255), 0, 0);
	sprintf(text, "Dimes %d", numDimes);
	addTitle(inImg, (string) text, Scalar(0, 0, 255), 0, 1);
	sprintf(text, "Nickles %d", numNickles);
	addTitle(inImg, (string) text, Scalar(0, 0, 255), 0, 2);
	sprintf(text, "Pennies %d", numPennies);
	addTitle(inImg, (string) text, Scalar(0, 0, 255), 0, 3);
	//add total to screen
	sprintf(text, "Total $%d.%2d", total / 100, total % 100);
	addTitle(inImg, (string) text, Scalar(0, 0, 255), inImg.cols / 2, 0);

    //TODO: show image (should this be here???)
	//imshow(WINDOW_NAME, inImg);
}

void coin_counter::addTitle(cv::Mat& img, std::string& title, cv::Scalar& color, int xLoc, int row)
{
	//setup parameters
	int fontFace = FONT_HERSHEY_COMPLEX_SMALL;
	double fontScale = 1;
	int thickness = 1;

	//find acceptable location
	int baseline = 0;
	Size textSize = getTextSize(title, fontFace,
		fontScale, thickness, &baseline);
	Point textOrg(xLoc, textSize.height*((row+1)*1.1));

	//put text on image
	putText(img, title, textOrg, fontFace, fontScale, color, thickness, 8);
}
