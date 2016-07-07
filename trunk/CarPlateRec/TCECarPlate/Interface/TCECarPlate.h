#pragma once
#include "CarPlateCommon.h"
#include <vector>

/**
**���ܣ�����ʶ���㷨��ʼ����������ʼ��ʶ��ģ��
**������strDataDir:ʶ��ģ�Ϳ��ļ���·����Ĭ��Ϊ��ǰ����·��
**����ֵ��0��sucess -1��Cascade_Final_14����ʧ�� -2��Cascade_double����ʧ��
         -3��Cascade_New����ʧ�� -4��Cascade_600_lpb����ʧ��
*/
TCE_CARPLATE_API int Init(const std::string& strDataDir);

/**
**���ܣ�����ʶ���㷨�˳������������Դ
*/
TCE_CARPLATE_API void Finish();

/**
**���ܣ�����ʶ��,����ͼƬ·���ķ�ʽ
**������strFilename:ͼƬ·��
        vecPlateInfos:�������ʶ�����Ϣ
**����ֵ��0��sucess -1��ʶ����ʧ��
*/
TCE_CARPLATE_API int PlateDetect_PicByFile(const std::string& strFilename, /*out*/std::vector<tPlateInfo>& vecPlateInfos);

/**
**���ܣ�����ʶ��,����ͼƬ�����Ƶķ�ʽ
**������strFileData:ͼƬ�ļ�����
        nFileLen:�ļ�����
        vecPlateInfos:�������ʶ�����Ϣ
**����ֵ��0��sucess -1��ʶ����ʧ��
*/
TCE_CARPLATE_API int PlateDetect_PicByBinary(const char* strFileData, const int nFileLen, /*out*/std::vector<tPlateInfo>& vecPlateInfos);

/**
**���ܣ�����ʶ��,������Ƶ�ļ��ķ�ʽ
**������strFilename:��Ƶ�ļ�·��
       vecPlateInfos:�������ʶ�����Ϣ�����ճ��Ƶ�˳��
**����ֵ��0��sucess -1��ʶ����ʧ��
*/
TCE_CARPLATE_API int PlateDetect_VideoByFile(const std::string& strFilename, /*out*/std::vector<tPlateInfo>& vecPlateInfos);
