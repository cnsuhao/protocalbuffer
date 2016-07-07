#pragma once
#include <string>
#include "opencv2/opencv.hpp"
#include "../Interface/CarPlateCommon.h"

class CarPlateMain
{
protected:
    CarPlateMain();
public:
    ~CarPlateMain();
public:
    static CarPlateMain Instance();

    /**
    **功能：车牌识别算法初始化，包括初始化识别模型
    **参数：strDataDir:识别模型库文件夹路径，默认为当前工作路径
    **返回值：0：sucess -1：Cascade_Final_14加载失败 -2：Cascade_double加载失败
    **        -3：Cascade_New加载失败 -4：Cascade_600_lpb加载失败
    */
    int Init(const std::string& strDataDir = "");

    /**
    **功能：车牌识别算法退出，清理分配资源
    */
    void Finish();

    /**
    **功能：车牌识别,按照输入参数为opencv mat的方式
    **参数：picMat:图片内容，opencv mat
           vecPlateInfos:输出车牌识别的信息
    **返回值：0：sucess -1：识别车牌失败
    */
   int PlateDetect(const cv::Mat& picMat, /*out*/std::vector<tPlateInfo>& vecPlateInfos);

private:


private:
    CvHaarClassifierCascade* m_pCascade_Final_14;
    cv::CascadeClassifier m_Cascade_double;
    cv::CascadeClassifier m_Cascade_New;
    cv::CascadeClassifier m_Cascade_600_lpb;
};

