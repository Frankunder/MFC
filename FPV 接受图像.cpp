#include <opencv2/core/utility.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "conio.h"
#include <opencv/opencv.hpp>
#include <iostream>
#include <opencv2/core.hpp>
#include <iostream>
#include <ctype.h>


using namespace cv;
using namespace std;

int main()
{
	VideoCapture capture(1);
	if (!capture.isOpened())
	{

		return -1;
	}
	cout << "°´ÏÂESCÍË³ö" << endl;

	while (1)
	{
		Mat frame;
		capture >> frame;
		imshow("fpv", frame);
		waitKey(30);

		char c = (char)waitKey(30);
		if (c == 27)
			break;
	}
	return (0);
}