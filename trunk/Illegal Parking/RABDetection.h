//#include "stdafx.h"
//#include <cv.h>
//
//int RABDetection(IplImage* img, CvPoint** ptx, int t);
//void initRABDetection();

#include <cv.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <assert.h> 
#include <math.h> 
#include <float.h> 
#include <limits.h> 
#include <time.h> 
#include <ctype.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;
using namespace std;

typedef struct{
	int point1_x;
	int point1_y;
	int point2_x;
	int point2_y;
	bool flag;
}PEOPLE_POINT;

bool detectAndDisplay(IplImage* image, CvPoint pt1, CvPoint pt2);