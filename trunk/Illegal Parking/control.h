
#pragma once

#include <cstdio>   
#include <cstring>   
#include <iostream>   
#include "Windows.h"   
#include "HCNetSDK.h"   
#include "plaympeg4.h"   
#include <opencv2\opencv.hpp>   
#include "cv.h"   
#include "highgui.h"  
#include <cxcore.h>
#include "opencv2/core/core.hpp"  
#include "opencv2/imgproc/imgproc.hpp"
#include <time.h>  
using namespace std;  
using namespace cv;  

void DVR_Start_Control(LONG lUserID,LONG lPlayHandle);
void DVR_Stop_Control(LONG lUserID,LONG lPlayHandle);






