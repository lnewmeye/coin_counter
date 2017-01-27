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
#define MAX_DIFF_BETWEEN_FRAMES_X 20
#define MAX_DIFF_BETWEEN_FRAMES_Y -50

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
	imshow("Background", m_backgroundImg);
	//waitKey();
}

void coin_counter::count_coins(cv::Mat& inImg)
{
    //get coins found in image
    vector<coin> imgCoins = get_coins_in_img( inImg );
	
	//DEBUG: draw coins on screen
	for (coin coin : imgCoins) {
		int hWidth = 2;
		Point tl = Point(
			coin.get_location().x - hWidth,
			coin.get_location().y - hWidth);
		Point br = Point(
			coin.get_location().x + hWidth,
			coin.get_location().y + hWidth);

		Scalar color = Scalar(0,255,0);
		
		rectangle(roiImg, Rect(tl, br),
			color,
			1, //thickness
			8, //connection type
			0);
	}
	//END DEBUG: draw coins on screen

    //remove coins we've already found
	imgCoins = remove_previous_coins( imgCoins );
	
	//DEBUG: draw coins on screen
	for (coin coin : imgCoins) {
		int hWidth = 4;
		Point tl = Point(
			coin.get_location().x - hWidth,
			coin.get_location().y - hWidth);
		Point br = Point(
			coin.get_location().x + hWidth,
			coin.get_location().y + hWidth);

		Scalar color = Scalar(255,255,0);
		
		rectangle(roiImg, Rect(tl, br),
			color,
			1, //thickness
			8, //connection type
			0);
	}
	//END DEBUG: draw coins on screen

    //add remaining coins to coins found
    add_new_coins( imgCoins );
	
	//DEBUG: draw coins on screen
	for (coin coin : m_countedCoins) {
		int hWidth = sqrt(coin.get_avg_area())/2; //half width
		Point tl = Point(
			coin.get_location().x - hWidth,
			coin.get_location().y - hWidth);
		Point br = Point(
			coin.get_location().x + hWidth,
			coin.get_location().y + hWidth);

		Scalar red = Scalar(0, 0, 255);
		Scalar green = Scalar(0, 255, 0);
		Scalar blue = Scalar(255, 0, 0);
		Scalar yellow = Scalar(0, 255, 255);
		Scalar purple = Scalar(255, 0, 255);
		Scalar color = Scalar(255,255,255);

		if (coin.get_coin_type() == COIN_TYPE_QUARTER)
			color = green;
		else if( coin.get_coin_type() == COIN_TYPE_NICKLE )
			color = yellow;
		else if( coin.get_coin_type() == COIN_TYPE_PENNY )
			color = blue;
		else if( coin.get_coin_type() == COIN_TYPE_DIME )
			color = purple;
		
		rectangle(roiImg, Rect(tl, br),
			color,
			3, //thickness
			8, //connection type
			0);
	}
	//show img in
	imshow("imgIn", roiImg);
	//END DEBUG: draw coins on screen


    //update coin count
	update_coin_counts( inImg );
}

std::vector<coin> coin_counter::get_coins_in_img(cv::Mat& inputImg)
{
    //create a vector of coins
    vector<coin> coinsInImg;
    
    //extract a specific section of the video
	Rect roi = Rect(Point(inputImg.cols / 6, 0), Point((inputImg.cols * 5) / 6, inputImg.rows));
	Mat roiImgi = inputImg(roi);
	Mat roiImgb = m_backgroundImg(roi);

	//subtract background
	absdiff(roiImgi, roiImgb, roiImg);

	//change to bw
	Mat grayImg;
	cvtColor(roiImg, grayImg, CV_BGR2GRAY);

	// Reduce the noise so we avoid false circle detection
	//GaussianBlur(grayImg, grayImg, Size(5, 5), 10, 10);

	//threshold
	Mat thresh;
	threshold(grayImg, grayImg, 60, 255, THRESH_BINARY);
	imshow("ThreshImg", grayImg);
	//waitKey();

	//distance transform
	//threshold	
	int i = 20;
	//for (i = 0; i < 50; i=i+5) {
		Mat distImg;
		distanceTransform(grayImg, distImg, DIST_L2, DIST_MASK_5);
		threshold(distImg, distImg, i, 255, THRESH_BINARY);
		distImg.convertTo(distImg, CV_8U);
		imshow("Dist Demo", distImg);
		//waitKey();
	//}

	/* erosion
	//erode a few times
	for (int i = 0; i < 10; i++) {
		int erosion_size = 3;
		Mat element = getStructuringElement(MORPH_ELLIPSE,
		//Mat element = getStructuringElement(MORPH_RECT,
			Size(2 * erosion_size + 1, 2 * erosion_size + 1),
			Point(erosion_size, erosion_size));
		/// Apply the erosion operation
		erode(grayImg, grayImg, element);
		imshow("Erosion Demo", grayImg);
		waitKey();
	}
	*/

	//get connected components
	Mat labelImg;
	Mat stats;
	Mat centroids;
	//cv::connectedComponentsWithStats(grayImg, labelImg, stats, centroids, 8, 4);
	cv::connectedComponentsWithStats(distImg, labelImg, stats, centroids, 8, 4);

	//for each connected component not in the background (0)
	for (int i = 1; i < stats.rows; i++) {
		//get area
		int area = stats.at<int>(i, CC_STAT_AREA);
		//int x = stats.at<int>(i, CC_STAT_LEFT);
		//int y = stats.at<int>(i, CC_STAT_TOP);
		int x = (int)centroids.at<double>(i, 0);
		int y = (int)centroids.at<double>(i, 1);
		//ensure it can be a coin
		//if (area < LARGER_THAN_COIN && area > DIME_SIZE_MIN) {
			//add found coins to this variable (like this)
			coin newCoin;
			newCoin.add_area(area);
			newCoin.set_location(Point(x, y));
			coinsInImg.push_back(newCoin);
		//}
	}

    //return all coins found
    return coinsInImg;
}

/****************************** Private Method Implementation ****************/

std::vector<coin> coin_counter::remove_previous_coins( std::vector<coin>& coins )
{
    //TODO: Update previously found coins locations
	//TODO: Remove previously found coins from the 'coins' vector. 

	vector<coin> newCoins;
	bool found;
	Point pixelsMoved = Point(0,0);
	int num = 0;

	//for all new coins
	for (coin newCoin : coins) {
		// see if any current coins are within 50 pixels
		found = false;
		for (coin cCoin : m_countedCoins) {
			//check distance bewteen the two
			Point dist = newCoin.get_location() - cCoin.get_location();
			if ( (abs(dist.x) < MAX_DIFF_BETWEEN_FRAMES_X) &&
			( dist.y > MAX_DIFF_BETWEEN_FRAMES_Y ) &&
			( dist.y < 0 ) ) {
				//add to average distance it moved
				pixelsMoved += dist;
				cout << "moved: " << dist << "\n";
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
		cout << "Curr: " << currLoc << "\n";
		(*it).set_location(currLoc + pixelsMoved);
		currLoc = (*it).get_location();
		cout << "New : " << currLoc << "\n";
	}
	/*
	for (coin cCoin : newCoins) {
		//only move in y
		Point currLoc = cCoin.get_location();
		cout << "Curr: " << currLoc << "\n";
		cCoin.set_location(currLoc + pixelsMoved);
		cout << "New : " << currLoc << "\n";
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
	
    //add text to screen
	char text[255];
	sprintf(text, "Quarters %d", numQuarters);
	addTitle(inImg, (string) text, Scalar(0, 0, 255), 0);
	sprintf(text, "Total $%d.%2d", total / 100, total % 100);
	addTitle(inImg, (string) text, Scalar(0, 0, 255), inImg.cols / 2);

    //TODO: show image (should this be here???)
	imshow(WINDOW_NAME, inImg);
}

void coin_counter::addTitle(Mat& img, string& title, Scalar& color, int xLoc)
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
