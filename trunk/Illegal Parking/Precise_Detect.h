
#pragma once
#ifndef PRECISE_DETECT_H
#define PRECISE_DETECT_H
#endif
#include "stdafx.h"
#include <Windows.h>
#include "cv.h"
#include "highgui.h"
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdlib.h>
#include <io.h>
#include <opencv2/ml/ml.hpp> 
//#include "exception_handler.h"
using namespace std;
using namespace cv;

bool CarPlateRecognition(char Filepath[], vector<string>&Carplates , vector<string>&Platecolor ,
	                                 vector<Rect> &Positions, bool model ) ;
bool IniRead(char strfind[], char strget[], int nStrgetSize = 32, char* section = "common");//modify by hb for 用api读取ini文件 at 2016.03.21
bool IniWrite(char strKey[], char strValue[], char* section = "common"); //add by hb for 增加写配置文件的功能 at 2016.03.22
void incrementRadon(double *pr, double pixel, double r);
static void radon(double *pPtr, double *iPtr, double *thetaPtr, int M, int N,int xOrigin, int yOrigin, int numAngles, int rFirst, int rSize);
void mexFunction(double* I, int rows, int cols, double *theta, int numAngles, int& rSize);
void imrotate(IplImage* src, int angle,Mat &dst0);
void correct(Mat src, Mat &dst);
int getThreshValue(IplImage* img);
int getThreshVal_Otsu_8u( IplImage* _src );
//int Test_color(IplImage *img);
int min(int a, int b, int c);
int BlueBased(IplImage* img);
int getThreshVal_Otsu_8u( IplImage* _src );
IplImage* myCanny(IplImage* img);
double candidate_plate_filter(IplImage* img);

int Precise_detect(/*Mat img,*/Mat gray,Mat color,bool whitebody,int colnum,int vnum,int count,int snum,int cons,int &st,int retest,vector<Mat> &block);
void RGBfeature(Mat img, vector<float>& feat);
bool HSVfeature(Mat img, vector<float>& feat);
int Test_color(Mat img);
int Test_color1(Mat img);

void CarColor(Mat &imgPlateGray ,bool &whitebody	) ;

int detect_pic(Mat matScr  ,vector<string>&Carplates , vector<string>&Platecolor, vector<Rect> &Positions  ) ;

 bool dotest1(int num ) ;