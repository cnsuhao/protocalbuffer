#ifndef MAIN_H
#define MAIN_H

#include <Windows.h>
#include "cv.h"
#include "highgui.h"
//#include "exception_handler.h"


int getThreshValue(IplImage* img);
int getThreshVal_Otsu_8u( IplImage* _src );
//int Test_color(IplImage *img);
int min(int a, int b, int c);
int BlueBased(IplImage* img);
int getThreshVal_Otsu_8u( IplImage* _src );
IplImage* myCanny(IplImage* img);
double candidate_plate_filter(IplImage* img);
#endif