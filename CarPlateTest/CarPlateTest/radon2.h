#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdlib.h>
#include <io.h>
#include "exception_handler.h"
using namespace cv;
using namespace std;

void incrementRadon(double *pr, double pixel, double r);
static void radon(double *pPtr, double *iPtr, double *thetaPtr, int M, int N,int xOrigin, int yOrigin, int numAngles, int rFirst, int rSize);
void mexFunction(double* I, int rows, int cols, double *theta, int numAngles, int& rSize);
void imrotate(IplImage* src, int angle,Mat &dst0);
void correct(Mat src, Mat &dst);
