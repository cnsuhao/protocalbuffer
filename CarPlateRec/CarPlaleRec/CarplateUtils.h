#pragma once
#include "opencv2/opencv.hpp"

/**
**���������������ľ�ֵ
**������ mat:�������
**����ֵ������Ԫ�صľ�ֵ
*/
extern int MatAverage(const cv::Mat& mat);

/**
**������������ͼƬ�ĵ׵�ֵ
**������ mat:�������
**����ֵ������׵�ֵ
*/
extern float MatBackgroudValue(const cv::Mat& mat);

/**
**�����������������Ԫ��Ϊĳһ��ֵ������
**������ mat:�������
**      value:ָ����ֵ
**����ֵ��������Ԫ��Ϊĳһ��ֵ������
*/
extern int NumPixOfValue(const cv::Mat& mat, const uchar value);
