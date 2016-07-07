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
    **���ܣ�����ʶ���㷨��ʼ����������ʼ��ʶ��ģ��
    **������strDataDir:ʶ��ģ�Ϳ��ļ���·����Ĭ��Ϊ��ǰ����·��
    **����ֵ��0��sucess -1��Cascade_Final_14����ʧ�� -2��Cascade_double����ʧ��
    **        -3��Cascade_New����ʧ�� -4��Cascade_600_lpb����ʧ��
    */
    int Init(const std::string& strDataDir = "");

    /**
    **���ܣ�����ʶ���㷨�˳������������Դ
    */
    void Finish();

    /**
    **���ܣ�����ʶ��,�����������Ϊopencv mat�ķ�ʽ
    **������picMat:ͼƬ���ݣ�opencv mat
           vecPlateInfos:�������ʶ�����Ϣ
    **����ֵ��0��sucess -1��ʶ����ʧ��
    */
   int PlateDetect(const cv::Mat& picMat, /*out*/std::vector<tPlateInfo>& vecPlateInfos);

private:


private:
    CvHaarClassifierCascade* m_pCascade_Final_14;
    cv::CascadeClassifier m_Cascade_double;
    cv::CascadeClassifier m_Cascade_New;
    cv::CascadeClassifier m_Cascade_600_lpb;
};

