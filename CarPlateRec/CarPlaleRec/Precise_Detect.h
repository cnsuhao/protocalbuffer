
#pragma once
#ifndef PRECISE_DETECT_H
#define PRECISE_DETECT_H
#endif

#ifdef PRECISE_DETECT_EXPORT
#define PRECISE_DETECTDLL  _declspec(dllexport)
#else
#define PRECISE_DETECTDLL  _declspec(dllimport)
#endif

//#include "stdafx.h"
#include <Windows.h>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <fstream>
#include <vector>
#include <string>
#include "opencv2/opencv.hpp"
#include <iostream>
#include <stdlib.h>
#include <io.h>
#include "opencv2/ml/ml.hpp"
//#include "exception_handler.h"
#include "conf/ReadConfig.h"
using namespace std;
using namespace cv;

/**
**���ܣ�����ʶ���㷨��ʼ����������ʼ��ʶ��ģ��
**������strDataDir:ʶ��ģ�Ϳ��ļ���·����Ĭ��Ϊ��ǰ����·��
**����ֵ��0��sucess -1��Cascade_Final_14����ʧ�� -2��Cascade_double����ʧ��
**        -3��Cascade_New����ʧ�� -4��Cascade_600_lpb����ʧ��
*/
PRECISE_DETECTDLL int InitCarPlateDetect(const string& strDataCarDir);

/**
**���ܣ�����ʶ���㷨�˳������������Դ
*/
PRECISE_DETECTDLL void FinishCarPlateDetect();

PRECISE_DETECTDLL bool CarPlateRecognition(char Filepath[], vector<string>&Carplates , vector<string>&Platecolor ,
                                     vector<Rect> &Positions, bool model ) ;
PRECISE_DETECTDLL bool IniRead( char strfind[] ,char strget[]	) ;

PRECISE_DETECTDLL int detect_pic(Mat& matScr  ,vector<string>&Carplates , vector<string>&Platecolor, vector<Rect> &Positions  ) ;

//bool CarPlateRecognition(char Filepath[], vector<string>&Carplates , vector<string>&Platecolor ,
//	                                 vector<Rect> &Positions, bool model ) ;
//bool IniRead( char strfind[] ,char strget[]	) ;

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

int Precise_detect(/*Mat img,*/Mat gray,Mat color,bool whitebody,int colnum ,int snum,int cons,int &st,int retest,vector<Mat> &block);
void RGBfeature(Mat img, vector<float>& feat);
bool HSVfeature(Mat img, vector<float>& feat);
int Test_color(Mat img);
int Test_color1(Mat img);

void CarColor(Mat &imgPlateGray ,bool &whitebody	) ;

//int detect_pic(Mat matScr  ,vector<string>&Carplates , vector<string>&Platecolor, vector<Rect> &Positions  ) ;

 bool dotest1(int num ) ;


 const std::string& GetChineseCharacter(int nIndex);
