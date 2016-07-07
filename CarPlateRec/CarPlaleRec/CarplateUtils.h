#pragma once
#include "opencv2/opencv.hpp"

/**
**功能明：计算矩阵的均值
**参数： mat:输入矩阵
**返回值：矩阵元素的均值
*/
extern int MatAverage(const cv::Mat& mat);

/**
**功能明：计算图片的底的值
**参数： mat:输入矩阵
**返回值：矩阵底的值
*/
extern float MatBackgroudValue(const cv::Mat& mat);

/**
**功能明：计算矩阵中元素为某一个值的数量
**参数： mat:输入矩阵
**      value:指定的值
**返回值：矩阵中元素为某一个值的数量
*/
extern int NumPixOfValue(const cv::Mat& mat, const uchar value);
