/******************************************************************************
* Class: Robotic Vision
* Project: Coin Counter
* Authors: James Swift, Luke Newmeyer
* Copyright 2017
******************************************************************************/

#include "coin_counter.h"

// coin_counter constructor
coin_counter::coin_counter() 
{
#ifdef DEBUG
	// Create named windows
	cv::namedWindow(DEBUG_WINDOW_1, CV_WINDOW_AUTOSIZE);
	cv::namedWindow(DEBUG_WINDOW_2, CV_WINDOW_AUTOSIZE);
	cv::setWindowTitle(DEBUG_WINDOW_1, DEBUG_WINDOW_1);
	cv::setWindowTitle(DEBUG_WINDOW_2, DEBUG_WINDOW_2);
	cv::createTrackbar(DEBUG_TRACKBAR_1, DEBUG_WINDOW_1, 0, NULL, 0);
	cv::createTrackbar(DEBUG_TRACKBAR_2, DEBUG_WINDOW_2, 0, NULL, 0);
	//cv::createTrackbar(DEBUG_TRACKBAR_1, DEBUG_WINDOW_1, 0, 255, NULL, 0);
#endif
}

// coin_counter destructor
coin_counter::~coin_counter() {}

// Set background image
void coin_counter::set_background(Mat image)
{
	Mat temp = image.clone();
	set_roi(temp, background);
	backgroundSet = true;
}

void coin_counter::count_coins(Mat image)
{
	// Check if background has been set
	if (!backgroundSet)
		set_background(image);

	// If DEBUG_TRACK draw lines on track
#ifdef DEBUG_TRACK
	Mat trackImage;
	draw_track(image, trackImage);
#endif

	// Find coins
	vector<coin> coins;
	coins = find_coins(image);

	// Remove coins
	coins = remove_previous_coins(coins);

	//add remaining coins to coins found
	add_new_coins(coins);

	// Draw coins
	draw_coins(image);

	//vector<coin> imgCoins = remove_previous_coins(imgCoins);

	update_coin_counts(image);
}

vector<coin> coin_counter::find_coins(Mat image)
{
	// Select region of interest
	Mat roiImage;// = image(Rect(Point(TRACK_LEFT, 0), Point(TRACK_RIGHT, image.rows)));
	set_roi(image, roiImage);

	// Take difference of image from background
	Mat diffImage;
	cv::absdiff(roiImage, background, diffImage);

	// Greyscale image
	Mat greyImage;
	cv::cvtColor(diffImage, greyImage, CV_BGR2GRAY);

	// Treshold image
	Mat thresholdImage;
	int threshold = THRESHOLD_LEVEL;
	//threshold = cv::getTrackbarPos(DEBUG_TRACKBAR_1, DEBUG_THRESHOLD);
	cv::threshold(greyImage, thresholdImage, threshold, 255, cv::THRESH_BINARY);
#ifdef DEBUG_THRESHOLD
	cv::imshow(DEBUG_THRESHOLD, thresholdImage);
#endif

	// Erode image
	Mat erodeImage;
	cv::erode(thresholdImage, erodeImage, erodeKernel);
#ifdef DEBUG_ERODE
	cv::imshow(DEBUG_ERODE, erodeImage);
#endif

	// Dilate image?
	/*Mat dilateImage;
	cv::dilate(erodeImage, dilateImage, dilateKernel);
#ifdef DEBUG_DILATE
	cv::imshow(DEBUG_DILATE, dilateImage);
#endif*/

	// Distance transform?

	// Find connected components
	Mat labelImage;
	Mat stats;
	Mat centroids;
	vector<coin> coinsInImg;
	cv::connectedComponentsWithStats(erodeImage, labelImage, stats, centroids, 8, 4);

	// for each connected component not in the background (0)
	for (int i = 1; i < stats.rows; i++) {
		//get area
		int area = stats.at<int>(i, cv::CC_STAT_AREA);
		std::cout << "area: " << area << "\n";
		int x = (int)centroids.at<double>(i, 0);
		int y = (int)centroids.at<double>(i, 1);
		//only look at coins that are fully in the image
		if (y < ((roiImage.rows * 3) / 4)) {
			//add found coins to this variable (like this)
			coin newCoin;
			newCoin.add_area(area);
			newCoin.set_location(Point(x, y));
			coinsInImg.push_back(newCoin);
			//cout << "area: " << area << "\n";
		}
	}

	return coinsInImg;
}

void coin_counter::draw_coins(Mat& imageIn)
{
	//Draw coins on screen
	for (coin coin : m_countedCoins) {
		std::cout << "drawing coin" << std::endl;
		int hWidth = sqrt(coin.get_avg_area()) / 2; //half width
		Point tl = Point(
			coin.get_location().x - hWidth + TRACK_LEFT,
			coin.get_location().y - hWidth);
		Point br = Point(
			coin.get_location().x + hWidth + TRACK_LEFT + 1,
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

		rectangle(imageIn, Rect(tl, br),
			color,
			3, //thickness
			8, //connection type
			0);
	}
}

void coin_counter::add_new_coins(std::vector<coin>& coins)
{
	//add new coins (if none, nothing will happen here)
	for (coin newCoin : coins) {
		//add to set of current coins
		m_countedCoins.push_back(newCoin);
	}
}

vector<coin> coin_counter::remove_previous_coins(std::vector<coin>& coins)
{
	vector<coin> newCoins;
	bool found;
	Point pixelsMoved = Point(0, 0);
	int num = 0;

	//for all new coins
	for (coin newCoin : coins) {
		// see if any current coins are within 50 pixels
		found = false;
		for (coin cCoin : m_countedCoins) {
			//check distance bewteen the two (negative y is up)
			Point dist = newCoin.get_location() - cCoin.get_location();
			if ((abs(dist.x) < MAX_DIFF_BETWEEN_FRAMES_X) &&
				(dist.y > MAX_DIFF_BETWEEN_FRAMES_Y) &&
				(dist.y < 0)) {
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
		pixelsMoved = Point(0, -50); //total guess

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

void coin_counter::update_coin_counts(cv::Mat& inImg)
{
	numQuarters = 0;
	numDimes = 0;
	numNickles = 0;
	numPennies = 0;
	for (coin coin : m_countedCoins) {
		switch (coin.get_coin_type()) {
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
	total = numQuarters * 25 + numDimes * 10 + numNickles * 5 + numPennies;

	//add number of coins to screen
	char text[255];
	sprintf(text, "Quarters %d", numQuarters);
	addTitle(inImg, (string)text, Scalar(0, 0, 255), 0, 0);
	sprintf(text, "Dimes %d", numDimes);
	addTitle(inImg, (string)text, Scalar(0, 0, 255), 0, 1);
	sprintf(text, "Nickles %d", numNickles);
	addTitle(inImg, (string)text, Scalar(0, 0, 255), 0, 2);
	sprintf(text, "Pennies %d", numPennies);
	addTitle(inImg, (string)text, Scalar(0, 0, 255), 0, 3);
	//add total to screen
	sprintf(text, "Total $%d.%2d", total / 100, total % 100);
	addTitle(inImg, (string)text, Scalar(0, 0, 255), inImg.cols / 2, 0);

	//TODO: show image (should this be here???)
	//imshow(WINDOW_NAME, inImg);
}

void coin_counter::addTitle(cv::Mat& img, std::string& title, cv::Scalar& color, int xLoc, int row)
{
	//setup parameters
	int fontFace = cv::FONT_HERSHEY_COMPLEX_SMALL;
	double fontScale = 1;
	int thickness = 1;

	//find acceptable location
	int baseline = 0;
	Size textSize = cv::getTextSize(title, fontFace,
		fontScale, thickness, &baseline);
	Point textOrg(xLoc, textSize.height*((row + 1)*1.1));

	//put text on image
	putText(img, title, textOrg, fontFace, fontScale, color, thickness, 8);
}


void coin_counter::draw_track(Mat imageIn, Mat imageOut)
{
	// Clone image
	imageOut = imageIn.clone();
	
	// Define track line ends
	Point topLeft = Point(TRACK_LEFT, 0);
	Point bottomLeft = Point(TRACK_LEFT, imageIn.rows);
	Point topRight = Point(TRACK_RIGHT, 0);
	Point bottomRight = Point(TRACK_RIGHT, imageIn.rows);

	// Draw lines on image
	cv::line(imageOut, topLeft, bottomLeft, trackColor);
	cv::line(imageOut, topRight, bottomRight, trackColor);
}

void coin_counter::set_roi(Mat imageIn, Mat& imageOut)
{
	Rect roi = Rect(Point(TRACK_LEFT, 0), Point(TRACK_RIGHT, imageIn.rows));
	imageOut = imageIn(roi);
}
