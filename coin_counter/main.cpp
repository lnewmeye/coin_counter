/******************************************************************************
* Copywright 2017
* Authors: James Swift, Luke Newmeyer
*****************************************************************************/

#include <iostream>
#include "opencv2\core.hpp"
#include "opencv2\highgui.hpp"

#include "coin_counter.h"
#include "typedefs.h"

using namespace cv;
using namespace std;

// Program options
#define LUKE_METHOD
#define VIDEO_INPUT "output.avi" // File name of video
#define DISPLAY_WINDOW "Coin Counter"
#define FRAME_PERIOD 100

// Function delcarations
static void usage_description(void);

// Main
int main(int argc, char **argv)
{

// Luke's method built off of James's method
#ifdef LUKE_METHOD

	// Open video caputre	
	VideoCapture video(VIDEO_INPUT);
	if (!video.isOpened()) {
		cout << "Error: failed to open video" << endl;
		return -1;
	}

	// Create display window
	namedWindow(DISPLAY_WINDOW, CV_WINDOW_AUTOSIZE);

	// Create core data structures
	coin_counter counter;
	Mat image;
	char keyvalue = 0;

	// Capture first frame of video and set as background
	video >> image;
	counter.set_background(image);

	// Display video options
	cout << "Press 'q' to quit" << endl << endl;

	// Loop through video, exit when complete or 'q' is pressed
	while (image.data && keyvalue != 'q') {

		// Count coins in image
		counter.count_coins(image);

		// Display image (processed)
		imshow(DISPLAY_WINDOW, image);

		// Get new image
		video >> image;

		// Get key press
		keyvalue = waitKey(FRAME_PERIOD);
	}

#endif LUKE_METHOD

#ifdef JAMES_METHOD
	//check number of arguments
	if (argc == (1)) {
		//define the input source
		VideoCapture camera("output.avi");
		//VideoCapture camera("pic.bmp");
		//VideoCapture camera;
		//camera.open(0);

		//create coin counter object
		coin_counter cc;

		//serve first image as background
		Mat inImg;
		camera >> inImg;
		//for (int i = 0; i < 5; i++) camera >> inImg;
		//inImg = imread("bkgnd.bmp", 1);
		cc.set_background(inImg);

		//inImg = imread("pic1.bmp", 1);
		imshow("inim", inImg);
		//for (int i = 0; i < 25; i++) camera >> inImg;
		//for (int i = 0; i < 10; i++) camera >> inImg;

		//serve remaining image into coin counter
		while (!inImg.empty()) {
			//count coins
			cc.count_coins(inImg);
			//break if user presses esc
			if (waitKey(100) == 27) break;
			imshow("inim", inImg);
			//get new image
			camera >> inImg;
			//break;
		}
		
		//output total
		cout << cc.total << "\n";
	}
	else {
		cout << "Error: Invalid number of arguments (" << argc << ")\n";
		usage_description();
	}
#endif // Method == James

    return 0;
}

static void usage_description(void)
{
	cout << "Usage: CoinCounter\n";
	cout << "  This will open the first camera and count coins on a conveyer\n";
	cout << "  belt. System still count and track coins passing by.\n";
}
