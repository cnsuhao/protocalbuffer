#pragma once
#ifndef FOREGROUND_MODEL_H
#define FOREGROUND_MODEL_H

#include <opencv2/opencv.hpp>
#include <opencv2/highgui//highgui.hpp>
#include <iostream>


using namespace std;
using namespace cv;

//每个像素点的样本个数
#define defaultNbSamples 20

//#min指数
#define defaultReqMatches 2

//Sqthere半径
#define defaultRadius 20

//子采样概率
#define defaultSubsamplingFactor 16

//背景像素
#define background 0

//前景像素
#define foreground 255		

class Foreground_Model
{
public:

	Foreground_Model(void);
	~Foreground_Model(void);

	//函数名：ViBe_Model
	//函数功能：获取前景函数
	//参数说明：
	//IplImage* src：原始的多通道图像
	//IplImage* dst：更新后的前景图像
	//返回值：无返回值
	void ViBe_Model(IplImage* src,IplImage* dst, bool &bReset) ;

	//保存处理后的信息
	CvMat* segMat;

private:

	//输入视频帧数
	int nFrmNum;

	//保存pFrame对应的灰度图像
	IplImage* pAfter;

	//保存pFrame对应的灰度矩阵
	CvMat* pAfterMat;

	//函数名：Initialize
	//函数功能：初始化函数
	//参数说明：
	//IplImage* src：原始的多通道图像
	//RNG rng：产生的随机数
	//返回值：无返回值
	void Initialize(CvMat* pFrameMat,RNG rng);

	//函数名：update
	//函数功能：更新函数
	//参数说明：
	//CvMat* pFrameMat：原始的多通道图像
	//CvMat* segMat:分割后的前景图像
	//RNG rng：产生的随机数
	//int nFrmNum: 统计输入的视频帧数
	//返回值：无返回值
	void update(CvMat* pFrameMat,CvMat* segMat,RNG rng);

};
#endif