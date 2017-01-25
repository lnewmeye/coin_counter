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
	GaussianBlur(bkgndImg, backgroundImg, Size(5, 5), 2, 2);
	imshow("Coin Counter", backgroundImg);
	waitKey();
}

void coin_counter::count_coins(cv::Mat inImg)
{
	string windowName = "Coin Counter";

	//extract a specific section of the video
	Rect roi = Rect(Point(inImg.cols / 6, 0), Point((inImg.cols * 5) / 6, inImg.rows));
	Mat roiImgi = inImg(roi);
	Mat roiImgb = backgroundImg(roi);

	//subtract background
	Mat roiImg;
	absdiff(roiImgi, roiImgb, roiImg);
	imshow(windowName, roiImg);
	waitKey();

	//change to bw
	Mat grayImg;
	cvtColor(roiImg, grayImg, CV_BGR2GRAY);
	imshow(windowName, grayImg);
	waitKey();

	// Reduce the noise so we avoid false circle detection
	GaussianBlur(grayImg, grayImg, Size(5, 5), 10, 10);

	//threshold
	Mat thresh;
	threshold(grayImg, grayImg, 60, 255, THRESH_BINARY);
	imshow("ThreshImg", grayImg);
	waitKey();

	//show canny filter
	Mat imgCanny;
	Canny(grayImg, imgCanny, HOUGH_THRESH1, HOUGH_THRESH2, 3);
	//convert to color
	Mat colorCanny;
	cvtColor(imgCanny, colorCanny, CV_GRAY2BGR);
	imshow("CannyImg", colorCanny);
	waitKey();

	//find circles in image
	vector<Vec3f> circles;
	HoughCircles(grayImg, circles, CV_HOUGH_GRADIENT, 1, grayImg.rows / 12,
		HOUGH_THRESH1, HOUGH_THRESH2, 0, 0);

	//draw circles in img
	for (int i = 0; i < circles.size(); i++) {
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		cout << "  center: " << center.x << "," << center.y << "\n";
		cout << "  radius: " << radius << "\n";
		circle(roiImgi, center, radius, Scalar(0, 0, 255), 3, 8, 0);
	}

	//process circles
	processCircles(circles);

	//add text to screen
	char text[255];
	sprintf(text, "Quarters %d", numQuarters);
	addTitle(roiImgi, text, Scalar(255, 255, 255), 0);
	sprintf(text, "Total $%d.%2d", total / 100, total % 100);
	addTitle(roiImgi, text, Scalar(255, 255, 255), roiImg.cols / 2);

	//imshow(windowName, inImg);
	imshow(windowName, roiImgi);
}

/****************************** Private Method Implementation ****************/

void coin_counter::processCircles(vector<Vec3f> circles)
{
	//remove circles if already associated with a coin
	removePreviousCoins(circles, m_countedCoins);
	//add new coins (if none, nothing will happen here)
	for (Vec3f circle : circles) {
		//create new coin
		coin newCoin;
		//add prediction (TODO: add actual pixel guesses)
		newCoin.addPrediction(circle);
		//add to set of current coins
		m_countedCoins.push_back(newCoin);
	}
	//update coin counts
	updateCoinCounts();
}

void coin_counter::removePreviousCoins( vector<Vec3f> coins, vector<coin> countedCoins)
{
	//TODO: Remove previously found coints from the 'coins' vector. Use the 
	//      'countedCoint' to know what's been counted.
	//Also, you may want to update everyone elses location as well in case they weren't detected but will be later.
}

void coin_counter::updateCoinCounts()
{
	numQuarters = 0;
	numDimes = 0;
	numNickles = 0;
	numPennies = 0;
	for (coin coin : m_countedCoins) {
		switch (coin.getCoinType() ) {
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
