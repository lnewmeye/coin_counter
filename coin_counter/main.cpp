/******************************************************************************
* Copywright 2017
* Authors: James Swift
*****************************************************************************/

#include <iostream>
#include "opencv2\opencv.hpp"
#include "opencv\highgui.h"

#include "coin_counter.h"
#include "typedefs.h"

using namespace cv;
using namespace std;

/****************************** Definitions **********************************/

/****************************** Private Variables ****************************/

/****************************** Private Functions ****************************/
static void usage_description(void);

/****************************** Function Implementation **********************/
int main(int argc, char **argv)
{
	//check number of arguments
	if (argc == (1)) {
		//define the input source
		VideoCapture camera("output.avi");
		//VideoCapture camera;
		//camera.open(0);

		//create coin counter object
		coin_counter cc;

		//serve first image as background
		Mat inImg;
		camera >> inImg;
		for (int i = 0; i < 5; i++) camera >> inImg;
		cc.set_background(inImg);

		for (int i = 0; i < 25; i++) camera >> inImg;
		//for (int i = 0; i < 10; i++) camera >> inImg;

		//serve remaining image into coin counter
		while (!inImg.empty()) {
			//count coins
			cc.count_coins(inImg);
			//break if user presses esc
			if (waitKey() == 27) break;
			//get new image
			camera >> inImg;
		}

		//output total
		cout << cc.total << "\n";
	}
	else {
		cout << "Error: Invalid number of arguments (" << argc << ")\n";
		usage_description();
	}

    return 0;
}

static void usage_description(void)
{
	cout << "Usage: CoinCounter\n";
	cout << "  This will open the first camera and count coins on a conveyer\n";
	cout << "  belt. System still count and track coins passing by.\n";
}
