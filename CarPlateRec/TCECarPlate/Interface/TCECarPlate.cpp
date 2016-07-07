#include "TCECarplate.h"
#include "../Framework/CarPlateMain.h"

int Init(const std::string& strDataDir)
{
    return 0;
}

void Finish()
{
    return;
}

int PlateDetect_PicByFile(const std::string& strFilename, /*out*/std::vector<tPlateInfo>& vecPlateInfos)
{
    vecPlateInfos.clear();

    cv::Mat img = cv::imread(strFilename/*,CV_LOAD_IMAGE_GRAYSCALE*/);

    if (!img.data)        // 判断图片调入是否成功
        return -1;
    return CarPlateMain::Instance().PlateDetect(img,vecPlateInfos);
}

int PlateDetect_PicByBinary(const char* strFileData, const int nFileLen, /*out*/std::vector<tPlateInfo>& vecPlateInfos)
{
    vecPlateInfos.clear();

    cv::Mat img = cv::imdecode(cv::_InputArray(strFileData, nFileLen), CV_LOAD_IMAGE_COLOR | CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH);

    if (!img.data)        // 判断图片调入是否成功
        return -1;
    return CarPlateMain::Instance().PlateDetect(img,vecPlateInfos);
}

int PlateDetect_VideoByFile(const std::string& strFilename, /*out*/std::vector<tPlateInfo>& vecPlateInfos)
{
    vecPlateInfos.clear();

    cv::VideoCapture capture(strFilename);
    if (!capture.isOpened())
    {
        std::cout << "Cannot find video file!" << std::endl;
        return -1;
    }

    ////////////////////////////////////////////////////
    cv::Mat frame;
    while (capture.read(frame))
    {
        CarPlateMain::Instance().PlateDetect(frame,vecPlateInfos);
    }
    return 0;
}
