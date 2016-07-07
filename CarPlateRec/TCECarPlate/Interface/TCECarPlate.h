#pragma once
#include "CarPlateCommon.h"
#include <vector>

/**
**功能：车牌识别算法初始化，包括初始化识别模型
**参数：strDataDir:识别模型库文件夹路径，默认为当前工作路径
**返回值：0：sucess -1：Cascade_Final_14加载失败 -2：Cascade_double加载失败
         -3：Cascade_New加载失败 -4：Cascade_600_lpb加载失败
*/
TCE_CARPLATE_API int Init(const std::string& strDataDir);

/**
**功能：车牌识别算法退出，清理分配资源
*/
TCE_CARPLATE_API void Finish();

/**
**功能：车牌识别,按照图片路径的方式
**参数：strFilename:图片路径
        vecPlateInfos:输出车牌识别的信息
**返回值：0：sucess -1：识别车牌失败
*/
TCE_CARPLATE_API int PlateDetect_PicByFile(const std::string& strFilename, /*out*/std::vector<tPlateInfo>& vecPlateInfos);

/**
**功能：车牌识别,按照图片二进制的方式
**参数：strFileData:图片文件数据
        nFileLen:文件长度
        vecPlateInfos:输出车牌识别的信息
**返回值：0：sucess -1：识别车牌失败
*/
TCE_CARPLATE_API int PlateDetect_PicByBinary(const char* strFileData, const int nFileLen, /*out*/std::vector<tPlateInfo>& vecPlateInfos);

/**
**功能：车牌识别,按照视频文件的方式
**参数：strFilename:视频文件路径
       vecPlateInfos:输出车牌识别的信息，按照车牌的顺序
**返回值：0：sucess -1：识别车牌失败
*/
TCE_CARPLATE_API int PlateDetect_VideoByFile(const std::string& strFilename, /*out*/std::vector<tPlateInfo>& vecPlateInfos);
