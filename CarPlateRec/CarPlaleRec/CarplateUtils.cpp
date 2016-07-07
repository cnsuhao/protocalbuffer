#include "CarplateUtils.h"

int MatAverage(const cv::Mat& mat)
{
    int sum = 0, i = 0, j = 0;
    for (i = 0; i < mat.rows; i++)
    {
        for (j = 0; j < mat.cols; j++)
            sum += mat.at<uchar>(i, j);
    }
    return (sum / (mat.rows*mat.cols));
}

float MatBackgroudValue(const cv::Mat& mat)
{
    int ave = MatAverage(mat);
    int num = 0, i = 0, j = 0;//====统计大于灰度均值的像素点个数
    for (i = 0; i < mat.rows; i++)
    {
        for (j = 0; j < mat.cols; j++)
        {
            if (mat.at<uchar>(i, j) > ave)
                num++;
        }
    }
    return (float)num / (float)(mat.rows*mat.cols);
}

int NumPixOfValue(const cv::Mat& mat, const uchar value)
{
    int i = 0, j = 0, sumpix = 0;
    for (i = 0; i<mat.rows; i++)
    {
        for (j = 0; j<mat.cols; j++)
        {
            if (mat.at<uchar>(i, j) == value)
                sumpix++;
        }
    }
    return sumpix;
}
