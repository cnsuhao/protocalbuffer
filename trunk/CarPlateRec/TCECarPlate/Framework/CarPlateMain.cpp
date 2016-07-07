#include "CarPlateMain.h"
#include <iostream>

#define CASCADE_FINAL_14_FILE ("data/cascade_final_14.xml")
#define  CASCADE_DOUBLE_FILE ("data/cascade_double.xml")
#define  CASCADE_NEW_FILE ("data/cascade_new.xml")
#define  CASCADE_6000_LPB_FILE ("data/cascade_6000_lbp.xml")

CarPlateMain::CarPlateMain()
{
}

CarPlateMain::~CarPlateMain()
{
}

CarPlateMain CarPlateMain::Instance()
{
    static CarPlateMain s_carPlateMain;
    return s_carPlateMain;
}

int CarPlateMain::Init(const std::string& strDataDir)
{
    m_pCascade_Final_14 = (CvHaarClassifierCascade*)cvLoad(CASCADE_FINAL_14_FILE, 0, 0, 0);
    if (!m_pCascade_Final_14)
    {
        std::cout << "ERROR: Could not load classifier " << CASCADE_FINAL_14_FILE << std::endl;
        return -1;
    }
    if (!m_Cascade_double.load(CASCADE_DOUBLE_FILE))
    {
        std::cout << "ERROR: Could not load classifier" << CASCADE_DOUBLE_FILE << std::endl;
        return -2;
    }
    if (!m_Cascade_New.load(CASCADE_NEW_FILE))
    {
        std::cout << "ERROR: Could not load classifier" << CASCADE_NEW_FILE << std::endl;
        return -3;
    }
    if (!m_Cascade_600_lpb.load(CASCADE_6000_LPB_FILE))
    {
        std::cout << "ERROR: Could not load classifier" << CASCADE_6000_LPB_FILE << std::endl;
        return -4;
    }
    return 0;
}

void CarPlateMain::Finish()
{
    cvReleaseHaarClassifierCascade(&m_pCascade_Final_14);
    m_pCascade_Final_14 = NULL;
}

int CarPlateMain::PlateDetect(const cv::Mat& picMat, /*out*/std::vector<tPlateInfo>& vecPlateInfos)
{
    return 0;
}
