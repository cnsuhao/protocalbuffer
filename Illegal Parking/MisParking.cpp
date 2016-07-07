#pragma once

#include "stdafx.h"
#include "Thread.h"
#include <windows.h>
#include <time.h>
//#include <vector>
#include "RGB.h"
#include <string>
#include "rgbmodeling.h"
#include "RABDetection.h"
#include "control.h"
#include "blockNode.h"
#include "park.h"
#include "HKDlgDlg.h"
#include "tceudp.h"
#include "LogFile.h"
#define _CRTDBG_MAP_ALLOC

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_PEOPLE	10
#define ROI_WIDTH	320
#define	ROI_HEIGHT	180
#define dWH_RatioL  0.8  // Little 
#define dWH_RatioG  3.0   // Great
#define Area_Threhold  1000

CruiseState ::CruiseState()
{
	bRunState = 0 ; 
	iTurnToWhere = TURN_TO_NOWHERE ;
	iPresentPoint = 0 ;
	iTurnToPointNum = 0  ;
	//iParkSeq  ;
	for (int i = 0 ; i <MAX_CRUISE_POINT ; i++)
	{	vecPtRoi[i] = new  vector<CvPoint> ; 
	}

	memset(iParkSeq ,0 , sizeof(int)*MAX_CRUISE_POINT ) ;
} 

CruiseState :: ~CruiseState()
{
	for (int i = 0; i < MAX_CRUISE_POINT; i++)
	{
		vecPtRoi[i]->clear();
		delete[] vecPtRoi[i];
		vecPtRoi[i]=nullptr;
	}
} 


Paramtran :: Paramtran()
{
	 param = new threadParam ;
	 CHKdlg = NULL ;
}

Paramtran :: ~Paramtran()
{
	if (param)
		delete param,param=NULL ;
	if (CHKdlg)
		CHKdlg =NULL;
}

//MARK 0617
int SCREEN_WIDTH ;
int SCREEN_HEIGHT ;
//MARK 0624
int SCREEN_LEFTTOP_X;
int SCREEN_LEFTTOP_Y;

char PicName[256];//2015-07-30
string sPicName;
Mat toSave;

vector<string> AllFilePath;
string playRecord;
string AllplayRecord;

CvRect  roi = cvRect(0, 0, ROI_WIDTH, ROI_HEIGHT);
//const unsigned short SIZE_OF_BUFFER = 100; //缓冲区长度
bool g_continue; //控制程序结束

bool g_bPreset; //判断是否在预置点
bool g_bCruise; //球机巡航标志

int iStayFrame = 20;

int iStayTime = 30;  //违停抓拍间隔
int  iPNum  = 1 ; 
int iFirstTake = 1 ;

int iZoomTimeStart = 0 ;
int  iSequence = 0 ;;
bool bWaitForDeal = false ; 
bool  bBiggerOrNot = 1 ;
bool  bBiggerOrNotShow = 1 ;
bool  bSynCronize = false ;
bool  bShowInfo = false ;

bool  bZoomed_Static  = false   ;  //是否已经拉近
bool  bZoomed_Dynamic  = false   ;

bool  bTakeThdPic = false ;

int  FrameIn = 0 ;  //从摄像头读取的图片序列号
int  FrameOut = 0 ;  //程序处理的图片系列号
int  RealIn = 0 ;
int  RealOut = 0 ;
int  FrameShowIn = 0 ;
int  FrameShowOut = 0 ;

int hik_width_org = 1280;
int hik_height_org = 720;

IplImage* images[SIZE_OF_BUFFER];//缓冲区是个循环队列
IplImage *back;
IplImage* imageresize ; 
IplImage* imageFront ;

unsigned short in = 0; //产品进缓冲区时的缓冲区下标
unsigned short out = 0; //产品出缓冲区时的缓冲区下标
HANDLE g_hMutex; //用于线程间的互斥
HANDLE g_hFullSemaphore; //当缓冲区满时迫使生产者等待
HANDLE g_hEmptySemaphore; //当缓冲区空时迫使消费者等待

HikPort g_CamOperate;
CWinThread* m_pThread;

char * imagesData[SIZE_OF_BUFFER] = {0};  //视频序列
//CascadeClassifier face_cascade;
vector<Rect> rect_cascade;
bool isRemoving;
bool bChangePoint = false ;
char strSeq[32] ;
CString  m_strCheck = "rr" ;
CString  m_strGetFrame = "tt" ;
CString  m_strPlatePos  = "yy" ;
CString  m_strPhotoDeal = "uu";

string  strSaveDir ;
string  strSavePath ;
string strPicPath ;
string strMessagePlate ;



int iLineNum = 0 ;
int iParkingDetect = 0 ;
int iPresetPNum = 0 ;

vector<int>  vecPresetPoints ;
int  iStatTimeThrd = 100 ;

string fileName;
bool isUnusualing;
bool flag_seting = false;
bool flag_setingLane1=false;
bool flag_setingLane2=false;
bool flag_setingLane3=false;
bool flag_setroi=false;
bool flag_setdirection=false;

Rect ptRoi ;
vector<CvPoint> vecMousePoint ;

CvPoint pt1_dir;
CvPoint pt2_dir;
CvPoint pt1_roi;
CvPoint pt2_roi;

// for more lanes
//first
CvPoint pt3_roi; 
CvPoint pt4_roi;

//second
CvPoint pt5_roi;
CvPoint pt6_roi;

//third lane
CvPoint pt7_roi;
CvPoint pt8_roi;

CascadeClassifier  *cascade_nixing  =  new CascadeClassifier;   //车型分类器


int pointNum = 4;
CvPoint **area = new CvPoint*[1];
CvSVM svm_16;
string filename = "forCarlogo/";
Size degreeScale(600,300);//Size degreeScale(500,250);

// 当前测试放文件名
CString fileNameNow;
std::string fileNameNows;
// 测试帧数
CString frameNumber;
std::string frameNumbers;

TceUdp  myUdp;
string name;
CTime CurTime;

critical_section templock; 
critical_section lockThread; 
critical_section lockImage; 

CruiseState *WrongPark = new CruiseState ;

CarInfo InfoWrite ;

//vector<CarInfo> vecThreadCar  ; 
Vehicle vecCar_Temp[MAX_CRUISE_POINT] ;  //临时车辆类

CarPlateInfo *CarPositions = new CarPlateInfo  ;

Foreground_Model *vibeFore = new Foreground_Model ;

bool  bCarPlateRec = false ; //是否在车牌识别
bool  bPhotoDeal = false ;	//是否在车牌识别
bool  bDeleteCar  = false ; //是否删除车辆信息
bool  bSynCronizing = false ;  //是否在需要同步

bool bPictureDeal = false ;//是否进行图片处理
bool bSendMsg = false ;  //是否发短信

bool bWaitCount = false ;
int  iWCount = 0 ;
int iCaptureStartTime = 0 ;
int iRelativeTime = 0 ;

bool bResetBG = true  ;

void delay(int sec)
{
	time_t start_time, cur_time; // 变量声明
	time(&start_time);
	do 
	{ 
		time(&cur_time);
	} 
	while((cur_time - start_time) < sec );
}

void initUnusual()
{
	area[0] = new CvPoint[4];
	area[0][0] = cvPoint(90,140);
	area[0][1] = cvPoint(171,140);
	area[0][2] = cvPoint(223,164);
	area[0][3] = cvPoint(120,218);
}
//针对特定的thread进行初始化
void init()
{
	m_pThread = NULL;
	g_continue = true;
	//创建各个互斥信号
	//g_hMutex = CreateMutex(NULL,FALSE,NULL);
	//g_hEmptySemaphore = CreateSemaphore(NULL,SIZE_OF_BUFFER-1,SIZE_OF_BUFFER-1,NULL);
	//g_hFullSemaphore = CreateSemaphore(NULL,0,SIZE_OF_BUFFER-1,NULL);
	in = 0;
	out = 0;
	isRemoving = false;
	isUnusualing = false;

	//if( !face_cascade.load( "haarcascade_frontalface_alt.xml" ) ){ TRACE("--(!)Error loading\n"); return ; };

	svm_16.load( "Car_16.xml" );
};


//初始化参数
void InitParam()
{
	char  strTarget[256] ;
	char  KeyWord1[32] ="savepath" ;
	char  KeyWord2[32] ="waittime" ;

	IniRead(KeyWord1, strTarget, sizeof(strTarget));
	if (!PathFileExists(strTarget))//文件夹不存在则创建by hb at 2016.03.18
	{
		SHCreateDirectoryEx(NULL, strTarget, NULL);
	}
	strSaveDir = strTarget ;
 
	IniRead( KeyWord2 ,strTarget) ;
    iStayTime = atoi( strTarget ) ;

}

bool pedestrian(Mat image)
{
	int ImgWidht1  = 16;
	int ImgHeight1 = 16;
	int ImgWidht2  = 32;
	int ImgHeight2 = 32;
	int n=0;

	//CvSVM svm_16, svm_32;

	//vector<float>descriptors_32;//结果数组 
	//svm_16.load( "Car_16.xml" );
	//svm_32.load( "Car_32.xml" );
	vector<float>descriptors_16;
	descriptors_16.resize(100);
	descriptors_16.clear();
	resize(image, image, cv::Size(ImgWidht1,ImgHeight1), 0, 0, INTER_CUBIC);
	HOGDescriptor *hog_16 = new HOGDescriptor(cvSize(ImgWidht1,ImgHeight1),cvSize(16,16),cvSize(8,8),cvSize(8,8),9);

	hog_16->compute(image, descriptors_16, Size(1,1), Size(0,0)); //调用计算函数开始计算 

	Mat SVMtrainMat =  Mat::zeros(1,descriptors_16.size(),CV_32FC1);

	for(vector<float>::iterator iter=descriptors_16.begin();iter!=descriptors_16.end();iter++)  
	{  
		SVMtrainMat.at<float>(0,n) = *iter;  
		n++;  
	}  
	int ret_16 = svm_16.predict(SVMtrainMat);
	delete hog_16;
	hog_16 = NULL;
	bool res = ret_16;
	SVMtrainMat.release();
	return res;
	//if ( ret_16 == 1)
	//{
	//	/*n = 0;
	//	resize(image, image, cv::Size(ImgWidht2,ImgHeight2), 0, 0, INTER_CUBIC);
	//	HOGDescriptor *hog_32 = new HOGDescriptor(cvSize(ImgWidht2,ImgHeight2),cvSize(16,16),cvSize(8,8),cvSize(8,8),9);
	//	hog_32->compute(image, descriptors_32, Size(1,1), Size(0,0));
	//	Mat SVMtrainMat1 =  Mat::zeros(1,descriptors_32.size(),CV_32FC1);
	//	for(vector<float>::iterator iter=descriptors_32.begin();iter!=descriptors_32.end();iter++)  
	//	{  
	//		SVMtrainMat1.at<float>(0,n) = *iter;  
	//		n++;  
	//	} 
	//	int ret_32 = svm_32.predict(SVMtrainMat1);
	//	if( ret_32 == 1 )*/
	//		return true;
	//	/*else
	//		return false;*/
	//}
	//	
	//else
	//	return false;
}

void DrawTag(IplImage* frame,vector<cv::Rect>found_filtered)
{
	vector<cv::Rect>::iterator it;
	for(it = found_filtered.begin();it!=found_filtered.end();it++)
	{
		cvRectangle(frame,cvPoint(it->x, it->y),cvPoint(it->x+it->width,it->y+it->height),cvScalar(0x00,0xff,0x00),2);
	}
}

/**
	**函数功能说明：
	**远景车牌识别
*/
DWORD WINAPI  CarPlateAnalysis(void* aaa)
{
	TRACE("CarPlateAnalysis!!\n") ;

	CarPlateInfo  *p = (CarPlateInfo*)aaa ; 
	char tempseq[12] ;
	strcpy(tempseq ,strSeq  ) ;

	for (int i = 0 ; i< p->rectRoiCopy.size() ; i++	)
	{
		if (p->rectRoiCopy[i].x < 0  )
		{
			p->rectRoiCopy[i].x = 0  ;
		}
		if (p->rectRoiCopy[i].y < 0  )
		{
			p->rectRoiCopy[i].y = 0  ;
		}
		if (p->rectRoiCopy[i].width > p->matOrigin.cols - p->rectRoiCopy[i].x - 1 )
		{
			p->rectRoiCopy[i].width = p->matOrigin.cols - p->rectRoiCopy[i].x - 1 ;
		}
		if (p->rectRoiCopy[i].height > p->matOrigin.rows - p->rectRoiCopy[i].y - 1 )
		{
			p->rectRoiCopy[i].height = p->matOrigin.rows - p->rectRoiCopy[i].y - 1 ;
		}
		int iFrom = p->Positions.size() ;
		Mat matTemp(  p->matOrigin , p->rectRoiCopy[i]) ;
	
		memset( strSeq ,0, sizeof(char)* 32 ) ;
	
		char temp[8] ;
		itoa(i ,temp , 10 ) ;
		strcpy( strSeq , tempseq ) ;
		strcat(temp , ".jpg") ;
		strcat(strSeq , "_") ;
		strcat(strSeq , temp) ;
	//	imwrite( "gdf.jpg" , p->matOrigin) ;
		imwrite( strSeq  , matTemp  ) ;
		try{
			imshow("rrr" ,matTemp	);
			waitKey(1) ;

		lockThread.lock();
			//string strTempPlate = "桂A53217" ;
			//string strTempColor = "黄色" ;
			//Rect rectTemp(0,0,300,400) ;
			//p->Carplates.push_back(strTempPlate) ;
			//p->Platecolor.push_back(strTempColor) ;
			//p->Positions.push_back(rectTemp) ;
		detect_pic( matTemp , p->Carplates , p->Platecolor , p->Positions ) ; 	
		lockThread.unlock() ;
		
		}
		catch(...)
		{
		//	AfxMessageBox("车牌识别  ！！");
		}

		int iTo =  p->Positions.size() ;
		for (int  j = iFrom ; j < iTo ; j++ )
		{
			p->Positions[j].x = p->Positions[j].x + p->rectRoiCopy[i].x  ;
			p->Positions[j].y = p->Positions[j].y + p->rectRoiCopy[i].y  ;
		}
	}

	p->rectRoiCopy.clear() ;
	p->rectRoi.clear() ;

	bCarPlateRec = false ;
	return 0 ;
}

void InitialRoi( CruiseState *WrongPark  ,vector<Rect> &vecRoi  )
{	
		 CvPoint ptTemp[2] ;
		 Rect  rectTemp ;
	     int iRectNum = WrongPark->vecPtRoi[iPNum]->size()/2 ;

		 for( int i = 0 ;i < iRectNum ; i ++ )
		 {
			 ptTemp[0].x = min( WrongPark->vecPtRoi[iPNum]->at(2*i).x ,WrongPark->vecPtRoi[iPNum]->at(2*i+1).x ) ;
			 ptTemp[0].y = min( WrongPark->vecPtRoi[iPNum]->at(2*i).y ,WrongPark->vecPtRoi[iPNum]->at(2*i+1).y ) ;
			 ptTemp[1].x = max( WrongPark->vecPtRoi[iPNum]->at(2*i).x ,WrongPark->vecPtRoi[iPNum]->at(2*i+1).x ) ;
			 ptTemp[1].y = max( WrongPark->vecPtRoi[iPNum]->at(2*i).y ,WrongPark->vecPtRoi[iPNum]->at(2*i+1).y ) ;

			 rectTemp.x =  ptTemp[0].x ;
			 rectTemp.y =  ptTemp[0].y ;
			 rectTemp.width =  ptTemp[1].x - ptTemp[0].x ;
			 rectTemp.height =  ptTemp[1].y - ptTemp[0].y ;

			 vecRoi.push_back(rectTemp) ;
	//		cvRectangle( imageresize ,ptTemp[0] ,ptTemp[1] , cvScalar(0,255,0) ,3 ,8,0 ) ; 
		 }
	
}
/**
	**函数功能说明：
	**显示检测区域
	**参数说明：
	**WrongPark: 车辆基础类
*/
void ShowRoi( CruiseState *WrongPark )
{	
	CvPoint ptTemp[2] ;
/*	Rect  rectTemp ;*/
	int iRectNum = WrongPark->vecPtRoi[iPNum]->size()/2 ;

	try{
	for (int i = 0 ; i < WrongPark->vecPtRoi[iPNum]->size() ; i++ )					
	{
		cvCircle( imageresize ,WrongPark->vecPtRoi[iPNum]->at(i) , 3 , cvScalar(40,100,255) ,3 ,8,0  );
	}
	}
	catch(...)
	{
/*int x = 0 ;*/
	}

	try{
	for( int i = 0 ;i < iRectNum ; i ++ )
	{
// 	int xmin = WrongPark->vecPtRoi[iPNum]->at(2*i).x ;
// 	int ymin = WrongPark->vecPtRoi[iPNum]->at(2*i).y ;
// 	int num = WrongPark->vecPtRoi[iPNum]->size() ;
		ptTemp[0].x = min( WrongPark->vecPtRoi[iPNum]->at(2*i).x ,WrongPark->vecPtRoi[iPNum]->at(2*i+1).x ) ;
		ptTemp[0].y = min( WrongPark->vecPtRoi[iPNum]->at(2*i).y ,WrongPark->vecPtRoi[iPNum]->at(2*i+1).y ) ;
		ptTemp[1].x = max( WrongPark->vecPtRoi[iPNum]->at(2*i).x ,WrongPark->vecPtRoi[iPNum]->at(2*i+1).x ) ;
		ptTemp[1].y = max( WrongPark->vecPtRoi[iPNum]->at(2*i).y ,WrongPark->vecPtRoi[iPNum]->at(2*i+1).y ) ;

// 		rectTemp.x =  ptTemp[0].x ;
// 		rectTemp.y =  ptTemp[0].y ;
// 		rectTemp.width =  ptTemp[1].x - ptTemp[0].x ;
// 		rectTemp.height =  ptTemp[1].y - ptTemp[0].y ;
		cvRectangle( imageresize ,ptTemp[0] ,ptTemp[1] , cvScalar(0,255,0) ,3 ,8,0 ) ; 
	}
}
catch(...)
{
/*	int x = 0 ;*/
}
	//	ImageRoi = cvCreateImage(cvSize(iWidth , iHeight), imageresize->depth , imageresize->nChannels ) ; 
}

void GetForegroundInfo( IplImage *dst , vector<CvRect> &vecObject )
{
	cvAdaptiveThreshold(dst, dst, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 3, 5);    //二值化
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contours = 0;

	//从二值图像中检索轮廓，并返回检测到的轮廓的个数
	int cnt = cvFindContours(dst, storage, &contours, sizeof(CvContour),2, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

	Mat segMapRoi(dst) ;
	vector<KeyPoint> keypoints;
	for(CvSeq* p = contours;p!=NULL;p=p->h_next)//依次画出各个区域
	{
		double area = cvContourArea(p); 
		if(area < MIN_AREA)
			continue;

		//计算点集的最外面（up-right）矩形边界,将结果写入到轮廓头的 rect 域中 header
		CvRect rect1 = cvBoundingRect(p,1);
		CvRect rect;
		Mat imageroi(segMapRoi,rect1);

		//角点检测
		FastFeatureDetector fast(40,true);
		fast.detect(imageroi,keypoints);

		aspect_ratio = ((double)rect1.width) / ((double)rect1.height);
		rect_area = rect1.width * rect1.height;

		//Mat image_pedestrian(pFrameMat,rect1);
		if(aspect_ratio > 0.7 && aspect_ratio < 2 && rect_area > area_threhold
			&& rect_area < dst->width * dst->height / 2)// && keypoints.size() > 20) //&& pedestrian(image_pedestrian))
		{
			rect = rect1;
			rect.x = rect1.x;
			rect.y = rect1.y;
			vecObject.push_back(rect);
			cvRectangle(dst,cvPoint(rect.x,rect.y),cvPoint(rect.x+rect.width,rect.y+rect.height),CV_RGB(255,255,255));
			//	cvRectangle(pFrame,cvPoint(rect.x,rect.y),cvPoint(rect.x+rect.width,rect.y+rect.height),CV_RGB(0,0,255));
			keypoints.clear();
		}
	}
	cvReleaseMemStorage(&storage);
}

void TargetMatch( IplImage *imgSrc ,Mat &matBackGround , Vehicle &vehCar , 
   vector<blockTrack>&trackBlock ,vector<CvRect> &vecObject ,bool &bMatch )
{
	string  strKey ;
	Rect   rectObject ;
	stringstream   sstrTemp ; 

	/////////////////******跟踪目标初始化////////////////////////

		if (trackBlock.empty())	//如果跟踪为空，把上一帧检测到的物体rect保存为跟踪物体
			{
				for(vector<CvRect>::iterator it = vecObject.begin();it!=vecObject.end();++it)	//检测到有物体，执行跟踪
				{
					blockTrack blockTemp;
					blockTemp.flag = false;	//是否在当前帧检测到
					blockTemp.pt[0] = cvPoint(it->x + it->width/2, it->y + it->height/2);	//初始位置，取矩形中心作为质心
					blockTemp.pt[MAX_STEP-1] = cvPoint(1, 1);  //最后位置
					blockTemp.rect = *it;	//位置rect
					blockTemp.step = 0;		//步数
					trackBlock.push_back(blockTemp);	//作为新发现的跟踪目标插入
				}
			}
			//******跟踪目标匹配，从当前帧检测到的物体，匹配是否有之前的跟踪记录，有则更新，没有则加上
			for(vector<CvRect>::iterator it = vecObject.begin(); it!=vecObject.end(); ++it)	//当前帧检测到的物体
			{
				CvPoint ptm = cvPoint(it->x+ it->width/2, it->y + it->height/2);	//取中心点

				for(vector<blockTrack>::iterator itt = trackBlock.begin(); itt != trackBlock.end(); ++itt) //跟踪目标
				{
					if (itt->flag)
						continue;
					if(match(*it,itt->rect))  //做两重循环，匹配检测到的物体和之前跟踪到的物体，并更新跟踪物体信息
					{
						itt->rect = *it;
						itt->step = (itt->step+1)%MAX_STEP;	//如果超过了阈值则返回0记录
						itt->pt[itt->step] = ptm;	//更新路径
						itt->flag = true;	//在当前帧检测到
						bMatch = true;
						break;	//匹配成功，找到跟踪物体的新路径，跳出循环
					}
				}

				if (!bMatch)	//如果没有找到之前有跟踪的记录，创建新的跟踪记录
				{
					blockTrack temp;
					temp.flag = true;	//当前帧检测到
					temp.pt[0] = ptm;	//起点
					temp.pt[MAX_STEP-1] = cvPoint(1, 1);//终点	
					temp.rect = *it;
					temp.step = 0;
					trackBlock.push_back(temp);
				}

				bMatch = false;
			}

			//******目标更新，判断是否进入了警戒区，划线
			for(vector<blockTrack>::iterator ita=trackBlock.begin(); ita!=trackBlock.end();)	//跟踪物体循环
			{
				CvScalar lineColor = CV_RGB(0,255,0);

				if(!ita->flag)	//在当前没有检测到该物体，去除检测物体					
					ita = trackBlock.erase(ita);
				else								
				{
					CvRect rect = ita->rect;
					CvPoint ptm = cvPoint(rect.x+ rect.width/2, rect.y + rect.height/2);	//取得中心点

					if(ita->step > 10)
					{
						if( stop_detection(ita->pt[ita->step-10],ita->pt[ita->step]))
						{
							rectObject = ita->rect;
							sstrTemp<<rectObject.x + rectObject.width/2<<" "<<rectObject.y + rectObject.height/2;  
							strKey = sstrTemp.str();
							sstrTemp.str("");

							if (! vehCar.PosMatch(rectObject))
							{				
								Mat weiting(imgSrc, rectObject);
								matBackGround = weiting;
								cvtColor(matBackGround,matBackGround,CV_BGR2GRAY);
								cvtColor(weiting,weiting,CV_BGR2GRAY);

								cvThreshold(&IplImage(weiting), &IplImage(weiting), 100, 255, CV_THRESH_BINARY);
								CvMemStorage* storage = cvCreateMemStorage(0);
								CvSeq* contours = 0;
								int cnt = cvFindContours(&IplImage(weiting), storage, &contours, sizeof(CvContour),2, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
								double area_max = 0;
								for(CvSeq* q = contours;q != NULL;q = q->h_next)
								{
									double area = cvContourArea(q); 
									if(area < 1000)
										continue;
									if(area > area_max)
										area_max = area;
								}

								if ( area_max != 0)
								{
									// char  temp[8] ;
									CarInfo carinfoTemp ;
								//	itoa(WrongPark->iParkSeq[iPNum] ,  temp , 10  ) ;
								//	WrongPark->vecCar[iPNum].Insert(strKey, rectObject, matBackGround);	
									carinfoTemp.rectPos = rectObject ;
									carinfoTemp.matCarPic = matBackGround ;
									carinfoTemp.iStartTime = GetTickCount();
									carinfoTemp.dContourArea = area_max;
								//	strcpy(carinfoTemp.strSeq , temp ) ;
									vehCar.carData.push_back(carinfoTemp) ;
								//	WrongPark->iParkSeq[iPNum] ++ ; 
								}								
								cvReleaseMemStorage(&storage);
								//cout<<car.umap[key]->contour_area<<" "<<car.umap[key]->frame_s<<endl;
							}
						}
					}

					if (ita->pt[MAX_STEP-1].x == 1)	 //画前一个路径到当前路径的线
						for(int i = 0;i<ita->step-1;i++)
							cvLine(imgSrc,ita->pt[i],ita->pt[i+1],lineColor,1,8,0);
					else
						for(int i = 0;i<MAX_STEP-1;i++)
							cvLine(imgSrc,ita->pt[(ita->step+i+1)%MAX_STEP],ita->pt[(ita->step+i+2)%MAX_STEP],lineColor,1,8,0);
					ita->flag = false;
					ita++;
				}
			}
}

/**
	**函数功能说明：
	**车辆检测
	**参数说明：
	**imgSrc: 原图
	**vehCarStatic：选定的车辆候选区域
	**vehCar：初步检测到的车辆区域
	**cascade_car：车型分类器
	**CvPoint ptOffSet：车型图片相对原图的偏移量
*/
void StaticCarDetect( IplImage *imgSrc , Vehicle &vehCarStatic ,
	Vehicle &vehCar , CascadeClassifier &cascade_car  ,CvPoint ptOffSet )
{
	TRACE("StaticCarDetect!!\n") ;
	vector<Rect> rectTemp;
 
	string  strKey ;
	Rect   rectObject ;
	stringstream   sstrTemp ; 
	
	//	rects1.clear(); 
	//cvSetImageROI(pFrame,roi);
	IplImage* pFrame_gray = cvCreateImage(cvSize(imgSrc->width, imgSrc->height), imgSrc->depth, 1);
	cvCvtColor(imgSrc, pFrame_gray, CV_BGR2GRAY);
	cascade_car.detectMultiScale(pFrame_gray, rectTemp, 1.1, 2, 0, Size(64, 64));
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* objects1;
	objects1 = cvCreateSeq(0, sizeof(CvSeq), sizeof(Rect), storage);
	for(int i=0; i < rectTemp.size(); i++)  
	{  
		cvSeqPush(objects1, &rectTemp[i] );  
	}
	////match(car_static,car)
	
	iLineNum = 637 ;
	 m_strCheck.Format("目前帧数是%d,第%d行", nFrmNum , iLineNum ) ; 
	
	for(int i=0;i<(objects1? objects1->total:0);++i) 
	{ 
		CvRect* r=(CvRect*)cvGetSeqElem(objects1,i); //返回索引指定的元素指针
		Point r_mid;
		r_mid.x = r->x + r->width/2;
		r_mid.y = r->y + r->height/2;
	//	CarNode *p_m;
		bool match_1 = true, match_2 = true;
	//	Rect val;
		Rect rectTemp ;

		for(int j = 0; j <vehCarStatic.carData.size(); j++ )
		{
			rectTemp = vehCarStatic.carData[j].rectPos ;
			if (r_mid.x >( rectTemp.x ) && r_mid.x < (rectTemp.x  + rectTemp.width) 
				&& r_mid.y > ( rectTemp.y - ptOffSet.y ) && r_mid.y < (rectTemp.y - ptOffSet.y + rectTemp.height))
			{
				match_1 = false;
				break;
			}
			else
			{
				match_1 = true;
			}	
		}

		for(int j = 0; j < vehCar.carData.size(); j++ )
		{
			rectTemp = vehCar.carData[j].rectPos ;
			if (r_mid.x > rectTemp.x && r_mid.x < (rectTemp.x + rectTemp.width) 
				&& r_mid.y > rectTemp.y && r_mid.y < (rectTemp.y + rectTemp.height))
			{
				match_2 = false;
				break;
			}
			else
			{
				match_2 = true;
			}	
		}

		if ( match_1 && match_2)
		{
			//cout<<r_mid.x<<"============================================="<<r_mid.y<<endl;
			CvFont font;
			cvInitFont( &font, CV_FONT_HERSHEY_PLAIN ,2, 2, 0, 2, 8); 	
			cvPutText(imgSrc,"Park",cvPoint(r_mid.x,r_mid.y),&font,CV_RGB(0,255,0));
			sstrTemp<<r_mid.x<<" "<<r_mid.y;  
			strKey = sstrTemp.str();
			sstrTemp.str("");
			Rect car_ignore;
			car_ignore.x = r->x;
			car_ignore.y = r->y;
			car_ignore.width = r->width;
			car_ignore.height = r->height;
			Mat bg_ignore(imgSrc, car_ignore);
			cvtColor(bg_ignore,bg_ignore,CV_BGR2GRAY);
			//cvThreshold(&IplImage(bg_static), &IplImage(bg_static), 100, 255, CV_THRESH_BINARY);
			cvAdaptiveThreshold(&IplImage(bg_ignore), &IplImage(bg_ignore), 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 3, 5);
		//	storage = cvCreateMemStorage(0);
			CvSeq* contours = 0;
			int cnt = cvFindContours(&IplImage(bg_ignore), storage, &contours, sizeof(CvContour),2, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
			double area_max = 0;
			for(CvSeq* q = contours;q != NULL;q = q->h_next)
			{
				double area = cvContourArea(q); 
				if(area < 1000)
					continue;
				if(area > area_max)
					area_max = area;
			}

			if ( area_max != 0)
			{

				CarInfo carinfoTemp ;
				//char  temp[8] ;
			//	itoa(WrongPark->iParkSeq[iPNum] ,  temp , 10  ) ;
				carinfoTemp.iStartTime  = GetTickCount();
				carinfoTemp.rectPos = car_ignore ;
				carinfoTemp.matCarPic =  bg_ignore   ;
				carinfoTemp.dContourArea = area_max;
			//	strcpy( carinfoTemp.strSeq ,temp ) ;	
				vehCarStatic.carData.push_back(carinfoTemp) ;
			//	WrongPark->iParkSeq[iPNum] ++ ;
			//	cout<< WrongPark->vecCar_Static[iPNum].umap[strKey]->contour_area<<" "<< WrongPark->vecCar_Static[iPNum].umap[strKey]->frame_s<<endl;
			}
		}
	}

	cvReleaseImage(&pFrame_gray); 
	cvReleaseMemStorage(&storage);
}

/**
	**函数功能说明：
	**车辆检测
	**参数说明：
	**imgSrc: 原图
	**vehCarStatic：选定的车辆候选区域
	**rectRoi；车牌位置的大致区域
	** ptOffSet：车型图片相对原图的偏移量
*/
void ParkCarCount(IplImage *imgSrc , Vehicle &vehCarStatic ,vector<Rect> &rectRoi  ,CvPoint ptOffSet )
{
	TRACE("ParkCarCount!!\n") ;
		CvFont font;
		cvInitFont( &font, CV_FONT_HERSHEY_PLAIN ,2, 2, 0, 2, 8); 	

		//Vehicle  * vehTemp  ;
		CString  strType     ;
		bool  bSkip = false    ;		
	
		if (vehCarStatic.carData.size() == 0)
		{
			return ;
		}	
		try{
			for ( vector<CarInfo>::iterator it = vehCarStatic.carData.begin() ;  it != vehCarStatic.carData.end() ; )
			{
				TRACE("ParkCarCount 825 !!\n") ;

					Mat matBg_ori = it->matCarPic;
						TRACE("ParkCarCount 828 !!\n") ;
					Mat matBg_cur(imgSrc,it->rectPos);
			TRACE("ParkCarCount 830 !!\n") ;

					cvtColor(matBg_cur,matBg_cur,CV_BGR2GRAY);
					//cvThreshold(&IplImage(bg_cur), &IplImage(bg_cur), 100, 255, CV_THRESH_BINARY);
					cvAdaptiveThreshold(&IplImage(matBg_cur), &IplImage(matBg_cur), 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 3, 5);
					CvMemStorage* storage = cvCreateMemStorage(0);
					CvSeq* contours = 0;
					int cnt = cvFindContours(&IplImage(matBg_cur), storage, &contours, sizeof(CvContour),2, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
					double area_max = 0;

	TRACE("ParkCarCount 840 !!\n") ;
				for(CvSeq* q_sub = contours;q_sub != NULL;q_sub = q_sub->h_next)
					{
						double area = cvContourArea(q_sub); 
						if(area < 1000)
							continue;
						if(area > area_max)
							area_max = area;
					}

					cvReleaseMemStorage(&storage);

					double contour_thre = 0.0;
					if(	it->dContourArea < it->rectPos.width * it->rectPos.height * 0.88 )
						contour_thre = 0.92;
					else
						contour_thre = 0.99;
					TRACE("ParkCarCount 854 !!\n") ;
					if( area_max >= it->dContourArea / 6 && area_max < it->dContourArea * 2.5
						&& area_max < it->rectPos.width * it->rectPos.height * contour_thre	)//判断车在,参数调整
					{
						if ( it->bLastPhoto == false )
						{
							CvPoint  pttemp ;
							pttemp.x = it->rectPos.x + it->rectPos.width / 3 ;
							pttemp.y = it->rectPos.y + it->rectPos.height / 3 ;

							cvPutText(imgSrc,"Park",cvPoint(pttemp.x , pttemp.y),&font,CV_RGB(0,255,0));
						}
						it->iCurrentTime = GetTickCount()  ;
						int iTimeGap = (it->iCurrentTime - it->iStartTime)/1000 ;

					//	try{
						TRACE("ParkCarCount 870 !!\n") ;
 						if( (iTimeGap >= iStayFrame  && it->bPhoto_flag == false )
							||(iTimeGap >= iFirstTake  && it->bFirstPhoto == false ) )//判断超时否
						{
						//	*nodeCarPicture = p_s ;	
							iRelativeTime = iTimeGap ;

							CvPoint al;
							al.x = it->rectPos.x + it->rectPos.width/2;
							al.y = it->rectPos.y + it->rectPos.height/2;

							TRACE("ParkCarCount 881 !!\n") ;
							if (it->bDetect == false )
							{
						
							Rect rectTemp ;
							rectTemp.x = ( ptOffSet.x + it->rectPos.x - it->rectPos.width/4 ) * RATE ;
							rectTemp.y = ( ptOffSet.y + it->rectPos.y - it->rectPos.height/4 ) * RATE ;
							rectTemp.width = it->rectPos.width * 1.5 * RATE  ;
							rectTemp.height = it->rectPos.height * 1.5 * RATE  ;

							/*rectTemp.x = ( ptOffSet.x + it->rectPos.x - it->rectPos.width/4 )  ;
							rectTemp.y = ( ptOffSet.y + it->rectPos.y - it->rectPos.height/4 )  ;
							rectTemp.width = it->rectPos.width * 1.5   ;
							rectTemp.height = it->rectPos.height * 1.5   ;*/

							rectRoi.push_back(rectTemp) ;
								it->bDetect = true ;
							}
							TRACE("ParkCarCount 899 !!\n") ;
						}	
					}
					else
					{
					//	cout<<area_max<<",  "<<p_s->val.height*p_s->val.width<<".   "<<p_s->key<<" deleted."<<endl;
							TRACE("ParkCarCount 905 !!\n") ;
						cvPutText(imgSrc,"Deleted",cvPoint(it->rectPos.x + it->rectPos.width / 2,
							it->rectPos.y + it->rectPos.height / 2),&font,CV_RGB(255,0,0));	
							it =vehCarStatic.carData.erase(it);
						matBg_ori.release();
						matBg_cur.release();
						continue;
					}
					matBg_cur.release();
					matBg_ori.release();	

					it++ ;
				}	
			}catch(...)
			{
		int x  = 0 ;
	//	AfxMessageBox("这里错了！！");

			}
}

/**
	**函数功能说明：
	**将车型和车牌检测通过的区域认为是有车，推进WrongPark列表
	**参数说明：
	**carPos：
	**WrongPark：车辆违章基础类 
	**iPNum：预置点序号
*/

void CandidateCar( CarPlateInfo  *carPos , CruiseState *WrongPark , int iPNum )
{
	TRACE("CandidateCar!!\n");
	int iThresh = 30 ;
	for ( int i = 0 ; i< carPos->Carplates.size() ; i++ )
	{	
		bool bMatched = false ;
		Rect rectTemp ;
		CvPoint  ptCenter ;		
		int y = carPos->Positions.size();
		TRACE( " Carnum is = %d ,value is %d  ", y , i );	
		if (i >= carPos->Positions.size() || i <0 )
		{
			int x = 0 ;
		}

		rectTemp.x = carPos->Positions[i].x/RATE ;
		rectTemp.y = carPos->Positions[i].y/RATE ;
		rectTemp.width = carPos->Positions[i].width/RATE ; 
		rectTemp.height = carPos->Positions[i].height/RATE ;
		ptCenter.x  =  rectTemp.x + rectTemp.width /2 ;
		ptCenter.y  =  rectTemp.x + rectTemp.height /2 ;

		for ( int i1 = 0 ; i1< WrongPark->vecCar_Static[iPNum].carData.size(); i1++	)
		{
			if(WrongPark->vecCar_Static[iPNum].carData[i1].iFourInOne == 1 )
			{
					bPictureDeal = true  ;  
			}
			//判断中心点距离，排除掉同一辆车反复推进的情况

			int iXdiff = abs(ptCenter.x	- WrongPark->vecCar_Static[iPNum].carData[i1].ptCenter.x ) ;
			int iYdiff = abs(ptCenter.y - WrongPark->vecCar_Static[iPNum].carData[i1].ptCenter.y ) ;

			if ( WrongPark->vecCar_Static[iPNum].carData[i1].bMatch==false 
				||(iXdiff < iThresh  && iYdiff < iThresh)   )
			{
				WrongPark->vecCar_Static[iPNum].carData[i1].bMatch = true ;
				WrongPark->vecCar_Static[iPNum].carData[i1].strCarPlate =  carPos->Carplates[i] ;
				WrongPark->vecCar_Static[iPNum].carData[i1].strColor = carPos->Platecolor[i] ;

				bMatched = true  ;
				break ;  
			}
			else
			{
				int x = 0 ;
			}
		}

		if (bMatched == false)
		{
			CarInfo carinfoTemp ;
			char  temp[8] ;
			//if (iPNum == 2  )
			//{
			//	imshow("uytut",carPos.matOrigin ) ;
			//	waitKey(1) ;
			//	int x = 0 ;
			//}
			itoa(WrongPark->iParkSeq[iPNum] ,  temp , 10  ) ;
			carinfoTemp.iStartTime  = GetTickCount();
			carinfoTemp.rectPos = rectTemp ;
			carinfoTemp.ptCenter.x = ptCenter.x ;
			carinfoTemp.ptCenter.y = ptCenter.y ;
			carinfoTemp.strCarPlate = carPos->Carplates[i] ;
			carinfoTemp.strColor = carPos->Platecolor[i] ;
			//carinfoTemp.matCarPic =  bg_ignore   ;
			strcpy( carinfoTemp.strSeq ,temp ) ;	
			WrongPark->vecCar_Static[iPNum].carData.push_back(carinfoTemp) ;
			WrongPark->iParkSeq[iPNum] ++ ;
		}
	}

	DeleteCar(WrongPark ) ;
}

/**
	**函数功能说明：
	**将球机按照车辆的框大小拉近，拉近之前拍一张全景照片并保存
	**参数说明：
	**imgSrc： 进行处理的图片 
	**imgOrgin：  原始图片用于保存
	**vehCarStatic： 违停车辆列表 
	**pStruPointFrame：海康镜头缩放专用类
	**lPlayHandle：海康摄像机句柄
	**strSetSeq：要保存图片的序列号
	**bTakePic：是否要拍照
*/

void CamaraZoomIn(IplImage* imgSrc , IplImage *imgOrgin , Vehicle &vehCarStatic , 
	NET_DVR_POINT_FRAME &pStruPointFrame ,LONG &lPlayHandle ,string strSetSeq  ,bool &bTakePic)
	{
		CvFont font;
		cvInitFont( &font, CV_FONT_HERSHEY_PLAIN ,2, 2, 0, 2, 8); 	
		
		CString  strType  = "静态"   ;
		bool  bSkip = false    ;

			for ( vector<CarInfo>::iterator it = vehCarStatic.carData.begin() ;  it != vehCarStatic.carData.end() ; )
			{
					//Mat matBg_ori = it->matCarPic;
					//Mat matBg_cur(imgSrc,it->rectPos);
					//cvtColor(matBg_cur,matBg_cur,CV_BGR2GRAY);
					////cvThreshold(&IplImage(bg_cur), &IplImage(bg_cur), 100, 255, CV_THRESH_BINARY);
					//cvAdaptiveThreshold(&IplImage(matBg_cur), &IplImage(matBg_cur), 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 3, 5);
					//CvMemStorage* storage = cvCreateMemStorage(0);
					//CvSeq* contours = 0;
					//int cnt = cvFindContours(&IplImage(matBg_cur), storage, &contours, sizeof(CvContour),2, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
					//double area_max = 0;
					//for(CvSeq* q_sub = contours;q_sub != NULL;q_sub = q_sub->h_next)
					//{
					//	double area = cvContourArea(q_sub); 
					//	if(area < 1000)
					//		continue;
					//	if(area > area_max)
					//		area_max = area;
					//}
					///*		if(area_max != 0)
					//{
					//cout<<p_s->key<<" "<<area_max<<"  "<<p_s->val.width*p_s->val.height<<"   ";

					//}*/
					//cvReleaseMemStorage(&storage);

					//double contour_thre = 0.0;
					//if(	it->dContourArea < it->rectPos.width * it->rectPos.height * 0.88 )
					//	contour_thre = 0.92;
					//else
					//	contour_thre = 0.99;

					//if( area_max >= it->dContourArea / 6 && area_max < it->dContourArea * 2.5
					//	&& area_max < it->rectPos.width * it->rectPos.height * contour_thre	)//判断车在,参数调整
					//{
				
						it->iCurrentTime = GetTickCount()  ;
						int iTimeGap = (it->iCurrentTime - it->iStartTime)/1000 ;

 						if( (iTimeGap >= iStayTime  && it->bPhoto_flag == false )||(iTimeGap >= iFirstTake  && it->bFirstPhoto == false )
							&& bZoomed_Static == false && bZoomed_Dynamic == false )//判断超时否
						{
						    TRACE("CamaraZoomIn!!\n");

						//	*nodeCarPicture = p_s ;	
							iRelativeTime = iTimeGap ;

							CvPoint al;
							al.x = it->rectPos.x + it->rectPos.width/2;
							al.y = it->rectPos.y + it->rectPos.height/2;
						//	cvPutText(,"park",al,&font,CV_RGB(255,0,0));
							TRACE( "width = %d and height = %d", imgSrc->width, imgSrc->height );	
						//这个值是海康固定的 255 
							int iFactor = 255 ;
							pStruPointFrame.xTop    = ( it->rectPos.x - it->rectPos.width *1.3 ) * iFactor  / imgSrc->width  ;
							pStruPointFrame.yTop    = ( it->rectPos.y - it->rectPos.width *1.3 ) * iFactor  / imgSrc->height;
							pStruPointFrame.xBottom = ( it->rectPos.x + it->rectPos.width + it->rectPos.width * 1.3 )  * iFactor / imgSrc->width;
							pStruPointFrame.yBottom = (it->rectPos.y + it->rectPos.height + it->rectPos.width * 1.3 )  * iFactor / imgSrc->height;
							
							if (pStruPointFrame.xTop < 0)
							{
								pStruPointFrame.xTop  = 0 ;
							}
							if (pStruPointFrame.yTop < 0)
							{
								pStruPointFrame.yTop  = 0 ;
							}
							if (pStruPointFrame.xBottom > 254 )
							{
								pStruPointFrame.xBottom  = 254  ;
							}
							if (pStruPointFrame.yBottom > 254 )
							{
								pStruPointFrame.yBottom  = 254  ;
							}

							CTime  temptime = CTime :: GetCurrentTime() ;
							CString  strSaveTime  ;
							CString  strCarPlate = it->strCarPlate.c_str() ;

						//	 p_s->strSeq 
							if (it->bFirstPhoto == false  )
							{  
								
								strSaveTime = strSetSeq.c_str()  ;
						    	strSaveTime =  strSaveTime +"_"+ it->strSeq  + strType ;
 								strSaveTime +=  temptime.Format("%H_%M_%S")+ "初次_" + strCarPlate + ".jpg";
								it->strCarinfo[0] = strSaveTime ;
								iWCount = GetTickCount() ;
								    bTakePic = false ;
									bWaitCount = true ;	
							}

							if ( it->bFirstPhoto == true && it->bPhoto_flag == false )
							{ 
								strSaveTime = strSetSeq.c_str()  ;
							    strSaveTime = strSaveTime +"_"+ it->strSeq  + strType ;
								strSaveTime += temptime.Format("%H_%M_%S")+ "再次_"+ strCarPlate +".jpg";
								it->strCarinfo[2] = strSaveTime ;
								 bTakePic = false ;
								bTakeThdPic = true ;
								bWaitCount = true ;	
								iWCount = GetTickCount() ;
									
							}	

							bZoomed_Static = true ;		
							iZoomTimeStart = GetTickCount() ;
							cvSaveImage(strSaveTime,  imgOrgin);

							LPNET_DVR_POINT_FRAME LpnetPointFrame = &pStruPointFrame;

							//云台图象区域选择放大或缩小
							NET_DVR_PTZSelZoomIn(lPlayHandle,LpnetPointFrame);
						
							if ( it->bFirstPhoto == true  )
							{	
								it->bPhoto_flag = true;	
							}
							it->bFirstPhoto = true ;
							it->bTakePhoto = false ;	
							it->bZoomed = true ;	
						}			
				
					if ( bZoomed_Static == true ||bZoomed_Dynamic == true )
					{
						bSkip = true ;
						break; 
					}
					it++ ;
				}	
				if (bSkip == true )
				{
				  return ;
				//	break ;  
				}
	}

/**
	**函数功能说明：
	**将球机拉回，并保存一张近景图片
	**参数说明：
	**imgOrgin：  原始图片
	**vehCarStatic： 违停车辆列表 
	**lPlayHandle：海康摄像机句柄
	**iPNum:预置点位置
	**strSetSeq：要保存图片的序列号
*/

void CamaraReturn ( IplImage * imgOrgin , Vehicle &vehSuspectCar , LONG lPlayHandle ,int iPNum ,string  strSetSeq )
{
	for (int i = 0 ; i <vehSuspectCar.carData.size(); i ++ )
	{
		if (vehSuspectCar.carData[i].bZoomed == false )
		{
		continue ;
		}
		int iTimeEnd = GetTickCount() ;
		int iTimeGap = (iTimeEnd - vehSuspectCar.carData[i].iStartTime )/1000 ;
//满足条件的就会拉回摄像头
	if(iTimeGap>= iRelativeTime + 6  && (vehSuspectCar.carData[i].bPhoto_flag == true || vehSuspectCar.carData[i].bFirstPhoto == true   ))
	{		
		if ( vehSuspectCar.carData[i].bTakePhoto == false  )
		{
			TRACE("CamaraReturn!!\n");
			CTime  temptime = CTime :: GetCurrentTime() ;

			CString  strSaveTime ;
			if (bZoomed_Static == true && vehSuspectCar.carData[i].bPhoto_flag == false	)
			{
				strSaveTime = strSetSeq.c_str()  ;
				strSaveTime = strSaveTime +"_"+ vehSuspectCar.carData[i].strSeq ;
				strSaveTime += "静态" + temptime.Format("%H_%M_%S") + "初次拉近" + ".jpg";
			     vehSuspectCar.carData[i].strCarinfo[1] = strSaveTime ;
			}
			if (bZoomed_Static == true	&& vehSuspectCar.carData[i].bPhoto_flag == true )
			{
				strSaveTime = strSetSeq.c_str()  ;
				strSaveTime = strSaveTime +"_"+ vehSuspectCar.carData[i].strSeq ;
				 vehSuspectCar.carData[i].strComboPic = strSaveTime ;
				strSaveTime += "静态" + temptime.Format("%H_%M_%S") + "再次拉近" + ".jpg";
				 vehSuspectCar.carData[i].strCarinfo[3] = strSaveTime ;
				 vehSuspectCar.carData[i].iFourInOne = 1 ;
			}

			if (bZoomed_Dynamic == true	&& vehSuspectCar.carData[i].bPhoto_flag == false )
			{
				strSaveTime = strSetSeq.c_str()  ;
				strSaveTime = strSaveTime +"_"+ vehSuspectCar.carData[i].strSeq ;
				strSaveTime += "动态" + temptime.Format("%H_%M_%S") + "初次拉近" + ".jpg";
			}
			if (bZoomed_Dynamic == true	&& vehSuspectCar.carData[i].bPhoto_flag == true	)
			{
				strSaveTime = strSetSeq.c_str()  ;
				strSaveTime = strSaveTime +"_"+ vehSuspectCar.carData[i].strSeq ;
				strSaveTime += "动态" + temptime.Format("%H_%M_%S") + "再次拉近" + ".jpg";
			}
			cvSaveImage(strSaveTime,imgOrgin);

			NET_DVR_PTZPreset(lPlayHandle,GOTO_PRESET,iPNum);	

			vehSuspectCar.carData[i].bZoomed = false ;
			vehSuspectCar.carData[i].bTakePhoto = true ;
		}				
		//	Sleep(3000) ;
	}
	}
}

/**
	**函数功能说明：
	**在图片整合后的车辆列表信息同步
	**参数说明：
	**vecCar：车辆类列表
*/
void SynCronizing( Vehicle vecCar[]  )
{
	for (int j = 0 ; j < MAX_CRUISE_POINT ; j++)
	{
		for (int i = 0 ; i < vecCar_Temp[j].carData.size() ; i ++  )
		{
		if (  vecCar_Temp[j].carData[i].bWrongDetect == true ||vecCar_Temp[j].carData[i].iFourInOne == 2 )
		{
			for ( int k = 0 ; k <vecCar[j].carData.size() ;  k++)
			{
				if ( vecCar[j].carData[k].rectPos.x == vecCar_Temp[j].carData[i].rectPos.x 
					&& vecCar[j].carData[k].rectPos.width ==  vecCar_Temp[j].carData[i].rectPos.width
					)
				{
					vecCar[j].carData[k].iFourInOne = vecCar_Temp[j].carData[i].iFourInOne ;
					vecCar[j].carData[k].bWrongDetect = vecCar_Temp[j].carData[i].bWrongDetect ;
				}
			}
		}
		}
	}
}

/**
	**函数功能说明：
	**删除掉已抓拍，车辆移走，错误检测等情况的车辆信息
	**参数说明：
	** WrongPark: 要寻找的的字符串
*/
void DeleteCar(  CruiseState *WrongPark )
{
	TRACE("DeleteCar!!\n");
	vector<CarInfo> ::iterator it ;
	for (it = WrongPark->vecCar_Static[iPNum].carData.begin() ; it != WrongPark->vecCar_Static[iPNum].carData.end()	;	)
	{
		if (it->bMatch == false )
		{
			it->iMissTime ++ ;
		}
		else
		{
			it->iMissTime = 0 ;
			it->bMatch = false ; 	
		}

			int iCountTime = GetTickCount() ;
			int iTimeGap = (iCountTime - it->iStartTime)/1000 ;
		//已抓拍一段时间后，可以删除
			if (iTimeGap >( 2* iStayTime + iStayTime) )
			{
				it->bDelete = true ;
			}
		//三次以上抓不到就删除，错误检测删除
		if ((it->iMissTime > 3 ||it->bWrongDetect == true) &&it->iFourInOne != 1 )
		{
			it->bDelete = true ;
		}

		if (it->bDelete == true )
		{	
			it->matCarPic.release() ;
			it = WrongPark->vecCar_Static[iPNum].carData.erase(it) ;
		}
		else
		{
			it ++ ;
		}
	}
}


/**
	**函数功能说明：
	**初次发现车辆，发短信模块
	**参数说明：
	** aaa: 要发送的字符串
*/
DWORD WINAPI SendShortMassege(void *aaa)
{
	vector<string> *p = (vector<string>*)aaa ;
	for (int i = 0 ; i < (p->size()) ; i++ )
	{
		CarPlateInfo temp ;
		string strFileName = (*p)[i];
		temp.matOrigin = imread(strFileName);
		if (temp.matOrigin.empty() == false	)
		{
			/*	imshow("gg",temp.matOrigin   ) ;
			waitKey(1);*/
			lockThread.lock() ;
			detect_pic(	temp.matOrigin , temp.Carplates ,temp.Platecolor ,temp.Positions );
			  lockThread.unlock() ;
			  
			if ( temp.Carplates.size()> 0 )
		   {
			  CString strTemp ;
			//strTemp.Format("车牌号为 %s 的车主请注意，您的机动车已经违停，请您迅速将车移开，否则交通部门将进行罚款。",temp.Carplates[0].c_str()	);
			  strTemp.Format("%s|%s|%s", temp.Carplates[0].c_str(), temp.Platecolor[0].c_str(), strFileName.c_str());
			//  sprintf(tempsend,"车牌号为%s的车主已经违停，请您迅速将车移开，否则将按违停进行罚款。",temp.Carplates[0]) ;
		  strMessagePlate = strTemp ;
			}
		}
	}
	p->clear();
	
	bSendMsg = false ;
	return 0 ;
}

/**
	**函数功能说明：
	**将已经抓拍完四张的图片进行处理
*/
DWORD WINAPI PictureProccess(void *aaa)
{
	TRACE("!!PictureProccess\n");
	vector<CarInfo> :: iterator  it ;
	for(int i = 0 ; i < MAX_CRUISE_POINT ; i++)
	{	
		for (  it = vecCar_Temp[i].carData.begin() ; it != vecCar_Temp[i].carData.end(); it++)
		{
			vector<string> Carplates ;
			vector<string> Platecolor ;
			vector<Rect>  Positions ;

			//判断是否已经抓拍完四张图片
			if (it->iFourInOne == 1 )
			{
				IplImage   *imgFour[4] ={0};
				IplImage   *imgFourShrink[4] = {0} ;
				//创建组合图片
				IplImage  *imgCombo = cvCreateImage( cvSize(1960 , 1120) ,8 ,3) ;
				Rect  rectRoi ;  

				bool bSaveFail = false ; 
				string strFinalPlate ;
				string strFinalColor ;
			
				//两张特写图片进行车牌识别，并进行内容比较
					for (int i = 1 ;i<4 ;i+=2)
				{

				char  sTemp[256] ;
				strcpy(sTemp , it->strCarinfo[i].c_str() ) ;

				TRACE( "I的值是%d ,文件名是%s " , i ,sTemp ) ;

				imgFour[i] = cvLoadImage( sTemp ) ;	

				if (imgFour[i] == NULL	)
				{
				continue ; 
				}
				try
				{
					lockThread.lock();
					
			/*		string strTempPlate = "桂A53217" ;
					string strTempColor = "黄色" ;
					Rect rectTemp(0,0,300,400) ;
					Carplates.push_back(strTempPlate) ;
					Platecolor.push_back(strTempColor) ;
					Positions.push_back(rectTemp) ;*/
					
				detect_pic( imgFour[i] , Carplates , Platecolor , Positions  );	
				lockThread.unlock() ;	
				cvReleaseImage(&imgFour[i]);
			
				}
				catch(...)
				{
					int x = 0 ;	//	AfxMessageBox("我崩溃 图片处理  ！！");
				}	
				}
				//无车牌，或者超过两个，视为错误，特写图片只有一个车牌，两个都不太可能
				if (Carplates.size() == 0 ||Carplates.size() > 2 )
				{
				bSaveFail = true ;
				}
				else if (Carplates.size() == 1)
				{
					strFinalPlate = Carplates.at(0) ;
					strFinalColor = Platecolor.at(0) ;
				}
				else
				{		
					try{
					char plate1[16] ;
					char plate2[16] ;
					int count = 0 ;
					strcpy(	plate1	, Carplates.at(0).c_str());
					strcpy(	plate2	, Carplates.at(1).c_str());

					//比较车牌信息
					for (int ii = 0 ; ii <7 ; ii++ )
					{
					if ( plate1[ii]== plate2[ii])
					{
					count ++;
					}
					}
					//超过四个字符相同就认为是同一个车牌
					if (count >4 )
					{
					strFinalPlate = Carplates.at(0) ;
					strFinalColor = Platecolor.at(0) ;
					}
					else
					{
					bSaveFail = true ;
					}
					}
					catch(...)
					{
					int x = 0;
					}
					}
				
				//将四张图片合成一张
				for(int i = 0 ; i < 4 ; i ++ ) 
				{	
					if (bSaveFail == true )
					{
						break ;
					}
					
					char  sTemp[256] ;
					strcpy(sTemp , it->strCarinfo[i].c_str() ) ;

					imgFour[i] = cvLoadImage( sTemp ) ;	
					if (imgFour[i] ==NULL )
					{
						bSaveFail = true ;
						break ;
					}
					imgFourShrink[i] = cvCreateImage(cvSize(imgFour[i]->width/2 , imgFour[i]->height/2 ) ,imgFour[i]->depth , imgFour[i]->nChannels ) ;

					cvResize(imgFour[i] , imgFourShrink[i]	 ) ;

					rectRoi.width = imgFour[i]->width /2 ;
					rectRoi.height = imgFour[i]->height /2 ;

					if (i == 0)
					{
						rectRoi.x = 10 ;
						rectRoi.y = 10 ;
					}
					if (i == 1 )
					{
						rectRoi.x = 30 + imgFour[i]->width /2;
						rectRoi.y = 10 ;
					}
					if (i == 2 )
					{
						rectRoi.x = 10 ;
						rectRoi.y = 30 +  imgFour[i]->height/ 2 ;
					}
					if (i == 3 )
					{
						rectRoi.x = 30 + imgFour[i]->width /2 ;
						rectRoi.y = 30 +  imgFour[i]->height / 2 ;
					}
					
					cvSetImageROI(imgCombo , rectRoi) ;
					cvCopy(imgFourShrink[i] , imgCombo	) ;
					cvResetImageROI(imgCombo	) ;	
					//   cvShowImage("222" , imgCombo ) ;
					//waitKey(1);	
	
					cvReleaseImage(& imgFour[i]) ;
					cvReleaseImage(& imgFourShrink[i] );		
				}
				
			//	bSaveFail = true ;

				if (bSaveFail == false)
				{
					//可以合成的就保存图片
					it->strComboPic = strSavePath +"_"+ it->strSeq;
					it->strComboPic = it->strComboPic + strFinalPlate + "_"+ strFinalColor +"_"+"组合" + ".jpg"   ;
					cvSaveImage(it->strComboPic.c_str()	,imgCombo 	) ; 
					
					InfoWrite.strCarinfo[0] = it->strCarinfo[0] ;
					InfoWrite.strCarinfo[1] = it->strCarinfo[1] ;
					InfoWrite.strCarinfo[2] = it->strCarinfo[2] ;
					InfoWrite.strCarinfo[3] = it->strCarinfo[3] ;
					InfoWrite.strComboPic = it->strComboPic ;
					 InfoWrite.strCarPlate = strFinalPlate;
				     InfoWrite.strColor = strFinalColor;
					 iSequence ++ ;
					 bShowInfo = true ;
					
					 // cvsaveimage
					//imwrite(it->strComboPic ,matCombo) ;
					it->iFourInOne = 2 ;				
				}
				else
				{
					//合成不了的，就把图片都删掉
					it->bWrongDetect = true ;
					it->iFourInOne = 0 ;	
				
					for ( int i	= 0 ; i < 4 ; i++	)
					{
						try{
						char  sTemp[256] ;
						strcpy(sTemp , it->strCarinfo[i].c_str() ) ;
						remove(sTemp) ;
						}
						catch(...)
						{
							int x = 0 ;
						}
					 }		
				}
				cvReleaseImage(&imgCombo );
			}
		}
}
	bPhotoDeal = false ;
	bPictureDeal = false ;
	return 0 ;
}

void PictureProccess( CruiseState *WrongPark ,CString strSavePath )
{
	vector<CarInfo> :: iterator  it ;
	for (int j = 0 ; j < MAX_CRUISE_POINT ; j ++  )
	{
    for (  it = WrongPark->vecCar_Static[j].carData.begin() ; it != WrongPark->vecCar_Static[j].carData.end(); it++)
    {
		if (it->iFourInOne == 1 )
		{
			IplImage   *imgFour[4] ={0};
			IplImage   *imgFourShrink[4] = {0} ;
			
			IplImage  *imgCombo = cvCreateImage( cvSize(1960 , 1120) ,8 ,3) ;
			Rect  rectRoi ;  

		   bool	bSaveFail = false ; 

			for(int i = 0 ; i < 4 ; i ++ ) 
			{
				char  sTemp[256] ;
				strcpy(sTemp , it->strCarinfo[i].c_str() ) ;

				imgFour[i] = cvLoadImage( sTemp ) ;	
				if (imgFour[i] ==NULL )
				{
					bSaveFail = true ;
					break ;
				}
				
				imgFourShrink[i] = cvCreateImage(cvSize(imgFour[i]->width/2 , imgFour[i]->height/2 ) ,imgFour[i]->depth , imgFour[i]->nChannels ) ;
				
				cvResize(imgFour[i] , imgFourShrink[i]	 ) ;
				
				rectRoi.width = imgFour[i]->width /2 ;
				rectRoi.height = imgFour[i]->height /2 ;

				if (i == 0)
				{
					rectRoi.x = 10 ;
					rectRoi.y = 10 ;
				}
				if (i == 1 )
				{
					rectRoi.x = 30 + imgFour[i]->width /2;
					rectRoi.y = 10 ;
				}
				if (i == 2 )
				{
					rectRoi.x = 10 ;
					rectRoi.y = 30 +  imgFour[i]->height/ 2 ;
				}
				if (i == 3 )
				{
					rectRoi.x = 30 + imgFour[i]->width /2 ;
					rectRoi.y = 30 +  imgFour[i]->height / 2 ;
				}
					cvSetImageROI(imgCombo , rectRoi) ;
					cvCopy(imgFourShrink[i] , imgCombo	) ;
					cvResetImageROI(imgCombo	) ;	
				//   cvShowImage("222" , imgCombo ) ;
				//waitKey(1);

				cvReleaseImage(& imgFour[i]) ;
				cvReleaseImage(& imgFourShrink[i] );
			}

			if (bSaveFail == false)
			{
			
			it->strComboPic = it->strComboPic +"组合" + ".jpg"   ;
			  cvSaveImage(it->strComboPic.c_str() ,imgCombo ) ; 
				// cvsaveimage
				 //imwrite(it->strComboPic ,matCombo) ;
		
			it->iFourInOne = 2 ;
			}
			
			cvReleaseImage(&imgCombo );

		}
    }
	}
}

/**
	**函数功能说明：
	**球机巡航判断
	**参数说明：
	** aaa: 要发送的字符串
*/
void CruiseProsess(	CruiseState *WrongPark  , int iPNum   , bool &bGetPhoto )
{
	//TRACE("CruiseProsess!!\n");
	//球机抓拍期间，进行保护，抓拍完以后才能调用
	if (WrongPark->bRunState == STATE_IN_CAPTURE)
	{
		int iTempIime = GetTickCount() ;
		int iTimeGap = (iTempIime - iCaptureStartTime) ;
		if ( bGetPhoto == true&&iTimeGap ||iTimeGap > 25000 )
		{
			WrongPark->iTurnToWhere = TURN_TO_CRUISEPOINT ;
			bGetPhoto = false ;
		}
	}
	else
	{
		for ( int i = 0 ; i < MAX_CRUISE_POINT ; i++)
		{
			if ( i == iPNum	)
			{
				continue ; 
			}
			//判断是否有车辆达到违停时间，是否需要转到该预置位抓拍

			for (int i1 = 0 ; i1 < WrongPark->vecCar_Static[i].carData.size() ; i1++ )
			{
				    WrongPark->vecCar_Static[i].carData[i1].iCurrentTime  = GetTickCount() ;
					int iBias =(WrongPark->vecCar_Static[i].carData[i1].iCurrentTime - WrongPark->vecCar_Static[i].carData[i1].iStartTime)/1000 ;
				
					if (iBias >iStayTime && WrongPark->vecCar_Static[i].carData[i1].bPhoto_flag == false 
					&& WrongPark->vecCar_Static[i].carData[i1].iStartTime > 0)
				{  
					WrongPark->iTurnToPointNum = i ;
					WrongPark->iTurnToWhere = TURN_TO_WARNINGPOINT  ;
					break; 
					//	WrongPark->vecCar	
				}
			}
		}
	}

}

VehicleViolation ::VehicleViolation()
{
	 strSetSeq[8] ;
	
	 oldimage = NULL ;
     pFrameMat = NULL;
	ImageRoi = NULL ;
	iRoiNum = 0 ;
	 iNumSeq = -1;
	 iSequence = 0 ;
	bTakePic = false;

};

VehicleViolation ::~VehicleViolation()
{
if ( oldimage !=NULL)
{
	cvReleaseImage(&oldimage);
}
if (ImageRoi != NULL)
{
	cvReleaseImage(&ImageRoi);
}
if (  pFrameMat != NULL)
{
	cvReleaseData(&pFrameMat);
}


};


/**
	**函数功能说明：
	**初始化违停类
*/
bool VehicleViolation :: InitParking()
{
	cascade_car.load("data/cascade_car.xml");

	return 1 ;
}


/**
	**函数功能说明：
	**违停抓拍
*/
int VehicleViolation :: MisParking()
{
		if (strSaveDir.size() == 0)
		{
			strSavePath = strSaveDir   ;
		}
		else{
			strSavePath = strSaveDir + "\\" ;
			} 

		    itoa( iPNum , strSetSeq,10 ) ;	
			strSavePath = strSavePath + strSetSeq ;
			if ( iPNum != iNumSeq )
			{
				iNumSeq = iPNum ;
				for(int ii = 0 ; ii < 4 ; ii++)
				{
					vehSuspectCarStatic[ii].carData.clear() ;
				}
			}
		//		area_threhold = size.width * size.height / 1000;
			//逐帧读取视频并进行处理
			if (bZoomed_Dynamic == false && bZoomed_Static == false)
			{	
			////////////////////////////////////////背景建模//////////////////////////////	  
				if (iRoiNum != WrongPark->vecPtRoi[iPNum]->size()/2 )
				{		
						rectRoi.clear() ;
					InitialRoi(	WrongPark , rectRoi ); 
					iRoiNum = rectRoi.size() ;
				}
			int iNumofRoi  ;
			CvPoint ptOffset ;

			if (rectRoi.size() > 0 )
			{
			 iNumofRoi = rectRoi.size() ;
			}
			else{
			iNumofRoi = 1 ;
			}
			for( int i = 0 ; i< iNumofRoi ; i++  ) 
			{
				if (rectRoi.size()> 0 )
				{
					ImageRoi = cvCreateImage(cvSize(rectRoi[i].width , rectRoi[i].height), imageresize->depth , imageresize->nChannels ) ; 
					 lockImage.lock() ; 
					cvSetImageROI(	imageresize , rectRoi[i])  ;
					cvCopy( imageresize   , ImageRoi  ) ;
					cvResetImageROI(imageresize) ;
					 lockImage.unlock() ; 
				ptOffset.x =  rectRoi[i].x ;
				ptOffset.y =  rectRoi[i].y ;
				}
				else
				{
					ImageRoi = cvCreateImage(cvSize(imageresize->width , imageresize->height), imageresize->depth , imageresize->nChannels ) ; 
					 lockImage.lock() ; 
					cvCopy( imageresize  , ImageRoi  ) ;
					 lockImage.unlock() ; 
					ptOffset.x =  0 ;
					ptOffset.y =  0 ;
				}
			//	vibeFore->ViBe_Model(  imageresize ,  imageFront ,  bResetBG  ) ;
				/*	cvShowImage("111",imageFront) ;
				waitKey(1) ;*/
			//	GetForegroundInfo(	imageFront , objDetRect	) ;
    		//	TargetMatch( imageresize, bg, vehSuspectCar , trackBlock ,objDetRect  ,matchValue    )  ;		
				///////////////////////////////////静止车辆检测////////////////////////////			
				//遮挡后出现
			if ( nFrmNum % 150 == 0)
			{	
				/*		cvShowImage("111",ImageRoi) ;
				waitKey(1) ;*/
				StaticCarDetect( ImageRoi  , vehSuspectCarStatic[i] ,vehSuspectCar ,cascade_car , ptOffset  )  ;  	
			}

			/////////////////每隔一段加1 ////////////////////////
			if ( nFrmNum % 30 == 0 )
			{     try {
					ParkCarCount( ImageRoi , vehSuspectCarStatic[i] , CarPositions->rectRoi , ptOffset ) ;
			}
			catch(...)
			{
              int x = 0 ;
				//	AfxMessageBox("ParkCarCount 出错了!!" ) ;
			}
			}

			if (ImageRoi != NULL)
			{	
				cvReleaseImage(&ImageRoi) ;
				ImageRoi = NULL ; 
			}
			}
			//////////////////////////////////////////
		//开启线程，将远景图片进行车牌识别
			if ( nFrmNum % 150 == 0)
			{
				Mat  matSrc(oldimage) ; 
				CarPositions->matOrigin.release() ;
				CarPositions->matOrigin = matSrc ;
				
				if (CarPositions->rectRoi.size() != 0  && bCarPlateRec == false )
				{
					bCarPlateRec = true ;
					bSynCronizing = false ;
					cvSaveImage("0011.jpg" , oldimage  );
					strcpy(strSeq  , strSetSeq   ) ; 
					CarPositions->iPresentP = iPNum ;
					CarPositions->rectRoiCopy =  CarPositions->rectRoi ; 
					HANDLE fff = CreateThread(NULL,0, CarPlateAnalysis, CarPositions,NULL,NULL)  ; 
					for (int i = 0 ; i < 4 ; i++)
					{
						vehSuspectCarStatic[i].carData.clear() ;	
					}
				}
			}
		    }
///////////////////////////////////////////////////////////		
	//如果能识别出车牌就把图片推进WrongPark类里面
			if ( CarPositions->Carplates.size() != 0 && bCarPlateRec == false )
			{
					if (CarPositions->iPresentP == iPNum )
					{
						CandidateCar( CarPositions ,WrongPark ,iPNum ) ;
					}
	
				CarPositions->Carplates.clear() ;
				CarPositions->Platecolor.clear() ;
				CarPositions->Positions.clear() ;
			}

			if (  WrongPark->vecCar_Static[iPNum].carData.size()!=0 && bCarPlateRec == false  )
			{	
				  CamaraZoomIn(  imageresize , oldimage , WrongPark->vecCar_Static[iPNum]  , pStruPointFrame ,lPlayHandle , strSavePath ,bTakePic ) ;
			if (bTakeThdPic == true)
			{
			int 	x = 0 ;
			}
				  if (bZoomed_Static == true||bZoomed_Dynamic == true )	//拍照
				  {   	
					  if (bTakeThdPic == true)
					  {
						int   x = 0 ;
					  }

		    	  CamaraReturn ( oldimage, WrongPark->vecCar_Static[iPNum] , lPlayHandle ,iPNum ,strSavePath ) ;
				  }
				//  bSendMsg = true ;
				  if (bSendMsg == false)
				  {
					  bSendMsg = true ;
					
					  for (int i = 0 ; i <WrongPark->vecCar_Static[iPNum].carData.size() ; i++ )
					  {
						  if (WrongPark->vecCar_Static[iPNum].carData[i].bTakePhoto == true &&
							  WrongPark->vecCar_Static[iPNum].carData[i].bLastPhoto == false&&
							  WrongPark->vecCar_Static[iPNum].carData[i].bSendMessage == false 
							  )
						  {
								strSend.push_back(WrongPark->vecCar_Static[iPNum].carData[i].strCarinfo[1])	;  
							  WrongPark->vecCar_Static[iPNum].carData[i].bSendMessage = true ;
						  }
					  }
					
					  HANDLE fff = CreateThread(NULL,0,  SendShortMassege, &strSend,NULL,NULL)  ; 
				  }

			  if ( bWaitCount == true && iWCount > 0 )
			  {
				  int iEnd = GetTickCount()  ;
				  int iBias = iEnd - iWCount ;
				  if (iBias > 10000)
				  {
					  bWaitCount = false  ;
					  bZoomed_Dynamic = false ;
					  bZoomed_Static = false ;
					  bTakePic = true ;
				  }
			  }
			}
			
			if (bZoomed_Dynamic == true || bZoomed_Static == true)
			{
				int iTimeCur = GetTickCount() ;

				int iTimeGap11 = iTimeCur - iZoomTimeStart  ;
				if (iTimeGap11 > 25000	)
				{
					NET_DVR_PTZPreset(lPlayHandle,GOTO_PRESET,iPNum); 
					bZoomed_Static = false ;
					bZoomed_Dynamic = false ;
				}
			}
			else{
					ShowRoi(WrongPark) ;
				}
			
			if ( bPictureDeal == true && bPhotoDeal == false && bCarPlateRec == false )
			{
				CString temp ;
				bPhotoDeal = true ;
				bSynCronizing = false ;
				for (int i = 0 ; i < MAX_CRUISE_POINT ; i++	)
				{ 
					vecCar_Temp[i].carData.clear() ; 
					vecCar_Temp[i].carData = WrongPark->vecCar_Static[i].carData ; 
				}
						 
			 HANDLE ggg = CreateThread(NULL,0, PictureProccess, &vecCar_Temp,NULL,NULL)  ; 	
		 //	PictureProccess( WrongPark ,temp) ;
			}
			if (bSynCronizing == false && bPhotoDeal == false && bCarPlateRec == false)
			{
				/*	for (int i = 0 ; i < 16 ; i ++	)
				{ */
					SynCronizing(WrongPark->vecCar_Static ) ;
			//	}
				
				//DeleteCar(	WrongPark) ;
				bSynCronizing = true ;
			}
			objDetRect.clear();		//把从上一帧检测到的物体清除,为下一帧检测做好准备
			cvReleaseMat(&pFrameMat);
			//	images[out] = pFrame;	
			 CruiseProsess(	WrongPark  , iPNum  , bTakePic ) ;
return 0 ;

}

//违章检测--摄像头
UINT  ShowHikImg(Paramtran *paramdeal)
{
	bool backtag=TRUE;
	float gme=0.0f;
	float gmm = 0.0f;

	threadParam *param = (threadParam *)paramdeal->param;  // 转化参数
	CHKDlgDlg *chkdlgtemp = (CHKDlgDlg *)paramdeal->CHKdlg ;
	
	VehicleViolation *incident = new VehicleViolation ;
	
	HWND hHwnd = param->hHwnd;
	incident->lPlayHandle = param->lPlayHandle;
	LONG lUserID = param->lUserID;\

	//delete paramdeal;

	NET_DVR_PTZPOS PTZposition1,PTZposition2;
	memset(&PTZposition1, 0, sizeof(NET_DVR_PTZPOS));
	memset(&PTZposition2, 0, sizeof(NET_DVR_PTZPOS));
	DWORD lpreturned = 0;
	
	NET_DVR_GetDVRConfig(lUserID,NET_DVR_GET_PTZPOS,1,&PTZposition1,sizeof(NET_DVR_PTZPOS),&lpreturned);
	NET_DVR_GetDVRConfig(lUserID,NET_DVR_GET_PTZPOS,1,&PTZposition2,sizeof(NET_DVR_PTZPOS),&lpreturned);
	NET_DVR_PTZPreset_Other(lUserID,1,SET_PRESET,10);

	int zoom_flag = 0;

	vector<CvRect> objDetRect;	//矩形区域，保存当前帧检测到的物体的区域
	vector<blockTrack> trackBlock;	 //跟踪物体对象	
	IplImage* pFrame = NULL;
	IplImage* pAfter = NULL;
	IplImage* segMap = NULL;
	CvMat* pFrameMat = NULL;//pFrame对象
	CvMat* pAfterMat = NULL;//保存pFrame对应的灰度图像
	CvMat* segMat = NULL;//保存处理后的信息

	bool matchValue = false;
	objDetRect.clear();	//目标检测和跟踪向量，可转换成为数组
	trackBlock.clear();
	CvSize size;
	//创建一个随机数生成器
	RNG rng(0xFFFFFFFF);
	//定义字体
	CvFont font;
	//cvInitFont( &font, CV_FONT_HERSHEY_COMPLEX_SMALL ,1, 1, 0, 1, 8);CV_FONT_HERSHEY_PLAIN 
	cvInitFont( &font, CV_FONT_HERSHEY_PLAIN ,2, 2, 0, 2, 8); 
	string key;
	stringstream sstr; 
	Rect val;
	Mat bg;
	CarNode node(key,val,bg);

	nFrmNum = 0;    

	//**********************************************************
	//分类器目录
	//
	//**********************************************************
	const char* cascade_nixing_name = "xml/cascade1.xml";
	//const char* cascade_nixing_name = "xml/cascade4.xml";
	cascade_nixing->load(cascade_nixing_name);

	//char strSetSeq[8] = {0} ;
	int  flag=0;
	imageresize = cvCreateImage(cvSize(hik_width_org/RATE,hik_height_org/RATE),IPL_DEPTH_8U,3);  
	imageFront = cvCreateImage(cvSize(hik_width_org/RATE,hik_height_org/RATE),IPL_DEPTH_8U,1);  
	incident->oldimage = cvCreateImage(cvSize( hik_width_org,hik_height_org )  , imageresize->depth ,imageresize->nChannels   ) ;
	incident->InitParking();
	
	CTime  time ;
	int iStartTime = GetTickCount() ;
	int iEndTime ;
	
	InitParam() ;
	
	while(g_continue)
	{
		if (RealOut >= RealIn  )
		{
			Sleep(50) ;
			continue;
		}
		if(	RealIn - RealOut >100 )
		{
			templock.lock() ;
			delete imagesData[FrameOut] ;
			imagesData[FrameOut] =NULL;
			bWaitForDeal = true ;
			RealOut ++ ;
			FrameOut = RealOut%SIZE_OF_BUFFER;	
			templock.unlock() ;
			//	iNumframe++ ;
			nFrmNum++ ;
			continue;
	}

		if(RealIn - RealOut <90 )
		{
			bWaitForDeal = false ;
		}
		char *tempchar =NULL;
		//delete	imagesData[FrameOut] ;
		templock.lock() ;
		tempchar = imagesData[FrameOut] ;
		imagesData[FrameOut] = NULL ;	
		if (tempchar == NULL)
		{
			RealOut ++ ;
			FrameOut = RealOut%SIZE_OF_BUFFER;	
			nFrmNum++ ;
			continue;
		}
		templock.unlock() ;
			
	bool bTrans = YV12_To_IplImage(incident->oldimage,(unsigned char *)tempchar,hik_width_org,hik_height_org);		
	
		cvResize(incident->oldimage,imageresize,CV_INTER_NN);	
	
		if (WrongPark->bRunState == STATE_IN_CAPTURE )
		{
			if (WrongPark->iTurnToPointNum > 0)
			{
				iPNum = WrongPark->iTurnToPointNum ;
			}
		}
		else
		{
			if (WrongPark->iPresentPoint > 0 )
			{
				iPNum = WrongPark->iPresentPoint ;
			}
		}

		if (iParkingDetect == 0	)
		{
			ShowRoi( WrongPark ) ;
		}
			nFrmNum++;
		if (nFrmNum >5000)
		{
			iEndTime = GetTickCount() ;

			int iTimeSpend =  iEndTime - iStartTime ;
			double dFps =1000* nFrmNum/iTimeSpend ;
			printf("目前的处理速度是%lf帧每秒",dFps ) ;
		}
	//	bool bTakePic = false ;

		if(iParkingDetect == 1 ) //违停
		{
		incident->MisParking() ;

		if(bShowInfo == true)
		{
			//列表框最多显示500条记录 超过清空列表 add by hb at 2016.03.23
			if (chkdlgtemp->m_list.GetItemCount() > 500)
			{
				LOG("记录已达500条！清空之");
				chkdlgtemp->m_list.DeleteAllItems();
			}
			
			itoa((incident->iSequence+1) , incident->strSetSeq	,10);
			CTime timecurrent = CTime::GetCurrentTime();
			string strTime = timecurrent.Format("%Y-%m-%d_%H:%M:%S");
			int n = chkdlgtemp->m_list.GetItemCount();
			char strn[4] = { 0 };
			sprintf_s(strn, "%d", n + 1);
			chkdlgtemp->m_list.InsertItem(n, strn);
			//chkdlgtemp->m_list.InsertItem(incident->iSequence	,incident->strSetSeq);  
			chkdlgtemp->m_list.SetItemText(n,1,InfoWrite.strCarPlate.c_str());
			chkdlgtemp->m_list.SetItemText(n,2,InfoWrite.strColor.c_str());
			chkdlgtemp->m_list.SetItemText(n,3,"违章停车");
			chkdlgtemp->m_list.SetItemText(n,4,strTime.c_str());
			chkdlgtemp->m_list.SetItemText(n,5,InfoWrite.strComboPic.c_str()) ;

			//违章数据入库
			if (chkdlgtemp->m_ado.isConn == TRUE)
			{
				CString sql(""); 
				sql.Format("INSERT INTO IllegalInfo (plate,color,type,preset,picname,picname1,picname2,picname3,picname4,time) \
												VALUES('%s','%s',1,1,'%s','%s','%s','%s','%s',NOW());",
												InfoWrite.strCarPlate.c_str(), InfoWrite.strColor.c_str(), InfoWrite.strComboPic.c_str(), 
												InfoWrite.strCarinfo[0].c_str(), InfoWrite.strCarinfo[1].c_str(), InfoWrite.strCarinfo[2].c_str(), InfoWrite.strCarinfo[3].c_str());
				
				chkdlgtemp->m_ado.ExecuteNonQuery(sql.GetBuffer(sql.GetLength()));
				sql.ReleaseBuffer();
				LOG("违停记录:CarId[%s] pic[%s]", InfoWrite.strCarPlate.c_str(),InfoWrite.strComboPic.c_str());
			}
			/*取消界面上合图的显示，结果列表双击直接看大图 modify by hb at 2016.03.24
			IplImage * imgshow = NULL ;
			imgshow	= cvLoadImage(InfoWrite.strComboPic.c_str()) ;
			if (imgshow != NULL)
			{
				chkdlgtemp->ShowImage( imgshow ,IDC_STATIC_PicResult2);
			     cvReleaseImage(&imgshow) ;
			}
			*/
			incident->iSequence++ ; 
			bShowInfo = false ;
		}
	}
	/////////////////////////////////逆行检测/////////////////////////////
		if(isUnusualing)//逆行
		{
			// 等待设定方向和检测区域
			if(!flag_seting){
				IplImage* pFrame = imageresize;
				if(!flag_setdirection)
				{
					//drawArrow(pFrame,pt1,pt2);
					cvPutText(imageresize,"set cars' normal direction with mouse",tipCord,&font,CV_RGB(255,0,0));//在图片中输出字符
				}else 
				{
					pt1 = pt1_dir;
					pt2 = pt2_dir;
					drawArrow(pFrame,pt1,pt2);
					//cvShowImage("pFrame",pFrame);
					//waitKey(5);
					//cvRectangle(pFrame,pt1_roi,pt2_roi,CV_RGB(255,0,0));
					//cvLine(pFrame,roiPt1,roiPt2,cvScalar(0,255,255));
					cvPutText(imageresize,"set roi's topleft & bottomright point",cvPoint(tipCord.x,tipCord.y),&font,CV_RGB(255,0,0));//在图片中输出字符
				}
				cvReleaseImage(&imageresize);
				//delete [] imagesData[out];

				::PostMessage(hHwnd, SHOW_HIKIMG, (WPARAM)FrameOut, (LPARAM)FrameOut);   // 向hHwnd句柄PostMessage
				//	out = (out+1)%SIZE_OF_BUFFER;
				continue;
			}
			roiPt1 = pt1_roi;
			roiPt2 = pt2_roi;
			IplImage* Frame = imageresize;
			CvSize sizefame = degreeScale;
			pFrame = cvCreateImage(sizefame,images[out]->depth,3);
			//***************************************************
			//resize pFrame (400,200)
			cvResize(Frame,pFrame);
			//
			//***************************************************
			//IplImage* pFrame = images[out];
			//IplImage* pFrame = cvCreateImage(cvSize(519,361)
			size.width  = pFrame->width;
			size.height = pFrame->height;
			area_threhold = size.width * size.height / 1000;

			//逐帧读取视频并进行处理
			nFrmNum++;

			if(nFrmNum %1 == 0)
			{
				if(nFrmNum == 1){
					segMap =    cvCreateImage(size, IPL_DEPTH_8U,1);
					segMat =    cvCreateMat(size.height, size.width, CV_32FC1);
					pAfter =    cvCreateImage(size, IPL_DEPTH_8U,1);
					pAfterMat = cvCreateMat(size.height, size.width, CV_32FC1);

					//转化成单通道图像再处理
					cvCvtColor(pFrame, pAfter, CV_BGR2GRAY);
					cvConvert(pAfter, pAfterMat);
					Initialize(pAfterMat,rng);
				}
				else {
					cvCvtColor(pFrame, pAfter, CV_BGR2GRAY);
					cvConvert(pAfter,pAfterMat);

					update(pAfterMat,segMat,rng,nFrmNum);//更新背景
					cvConvert(segMat,segMap);
					cvDilate (segMap, segMap);
					cvErode  (segMap, segMap);
					cvErode  (segMap, segMap);
					cvDilate (segMap, segMap);
					Mat segMapRoi(segMap);
					Mat pFrameMat(pFrame);
					cvThreshold(segMap, segMap, 0, 255, CV_THRESH_OTSU);
					CvMemStorage* storage = cvCreateMemStorage(0);
					CvSeq* contours = 0;
					int cnt = cvFindContours(segMap, storage, &contours, sizeof(CvContour),CV_RETR_TREE,CV_CHAIN_APPROX_TC89_L1, cvPoint(0,0));
					//vector<KeyPoint> keypoints;
					for(CvSeq* p = contours;p!=NULL;p=p->h_next)//依次画出各个区域
					{
						double area = cvContourArea(p); 
						if(area < MIN_AREA)
							continue;
						CvRect rect = cvBoundingRect(p,1);

						Mat imageroi(segMapRoi,rect);

						//角点检测
						//FastFeatureDetector fast(40,true);
						//fast.detect(imageroi,keypoints);

						aspect_ratio = ((double)rect.width) / ((double)rect.height);
						rect_area = rect.width * rect.height;

						Mat image_pedestrian(pFrameMat,rect);
						area_threhold = size.width * size.height / 400;
						if(aspect_ratio > 0.8 && aspect_ratio < 3 && rect_area > area_threhold && pedestrian(image_pedestrian))///&& keypoints.size() > 20 )
						{								
							////////复制图像
							//Mat imgs(pFrameMat,rect);
							////imshow("imgs",imgs);
							//rect_cascade.clear();
							//cascade_nixing.detectMultiScale(imgs,rect_cascade,1.1,2,0,Size(25,25));
							//////add
							//for(int i=0;i<rect_cascade.size();i++){
							//	Rect r = rect_cascade[i];
							//	objDetRect.push_back(cvRect(r.x+rect.x,r.y+rect.y,r.width,r.height));
							//	cvRectangle(pFrame,cvPoint(r.x+rect.x,r.y+rect.y),cvPoint(r.x+r.width+rect.x,r.y+r.height+rect.y),CV_RGB(255,0,0));					
							//	
							//}
							//origin code
							objDetRect.push_back(rect);
							cvRectangle(pFrame,cvPoint(rect.x,rect.y),cvPoint(rect.x+rect.width,rect.y+rect.height),CV_RGB(255,0,0));

						}
						image_pedestrian.release();
					}
					cvReleaseMemStorage(&storage);
				}
				/*******************************************************************************************************************************************************************************************/
				//******跟踪目标初始化
				if (trackBlock.empty())	//如果跟踪为空，把上一帧检测到的物体rect保存为跟踪物体
				{
					for(vector<CvRect>::iterator it=objDetRect.begin();it!=objDetRect.end();++it)	//检测到有物体，执行跟踪
					{
						blockTrack blockTemp;
						blockTemp.flag = false;	//是否在当前帧检测到
						blockTemp.pt[0] = cvPoint(it->x + it->width/2, it->y + it->height/2);	//初始位置，取矩形中心作为质心
						blockTemp.pt[MAX_STEP-1] = cvPoint(1, 1);  //最后位置
						blockTemp.rect = *it;	//位置rect
						blockTemp.step = 0;		//步数
						trackBlock.push_back(blockTemp);	//作为新发现的跟踪目标插入
					}
				}

				//******跟踪目标匹配，从当前帧检测到的物体，匹配是否有之前的跟踪记录，有则更新，没有则加上
				for(vector<CvRect>::iterator it=objDetRect.begin(); it!=objDetRect.end();++it)	//当前帧检测到的物体
				{
					CvPoint ptm=cvPoint(it->x+ it->width/2, it->y + it->height/2);	//取中心点
					for(vector<blockTrack>::iterator itt=trackBlock.begin(); itt!=trackBlock.end();++itt) //跟踪目标
					{
						if (itt->flag)
							continue;
						if(match(*it,itt->rect))  //做两重循环，匹配检测到的物体和之前跟踪到的物体，并更新跟踪物体信息
						{
							itt->rect = *it;
							itt->step = (itt->step+1)%MAX_STEP;	//如果超过了阈值则返回0记录
							itt->pt[itt->step] = ptm;	//更新路径
							itt->flag = true;	//在当前帧检测到
							matchValue = true;
							break;	//匹配成功，找到跟踪物体的新路径，跳出循环
						}
					}
					if (!matchValue)	//如果没有找到之前有跟踪的记录，创建新的跟踪记录
					{
						blockTrack temp;
						temp.flag = true;	//当前帧检测到
						temp.pt[0] = ptm;	//起点
						temp.pt[MAX_STEP-1] = cvPoint(1, 1);//终点	
						temp.rect = *it;
						temp.step = 0;
						trackBlock.push_back(temp);
					}
					matchValue = false;
				}

				//******目标更新，判断是否进入了警戒区，划线
				int pminx,pminy,pmaxx,pmaxy;
				if(roiPt1.x>roiPt2.x){
					pminx = roiPt2.x;
					pmaxx = roiPt1.x;
				}else{
					pminx = roiPt1.x;
					pmaxx = roiPt2.x;
				}
				if(roiPt1.y>roiPt2.y){
					pminy = roiPt2.y;
					pmaxy = roiPt1.y;
				}else{
					pminy = roiPt1.y;
					pmaxy = roiPt2.y;
				}
				//***********************************
				//
				pminx = sizefame.width*(float)pminx/(float)Frame->width;
				pmaxx = sizefame.width*(float)pmaxx/(float)Frame->width;
				pminy = sizefame.height*(float)pminy/(float)Frame->height;
				pmaxy = sizefame.height*(float)pmaxy/(float)Frame->height;
				cvRectangle(pFrame,Point(pminx,pminy),Point(pmaxx,pmaxy),cvScalar(0,0,255));

				string text = "";
				char ch[100];
				itoa(pminx,ch,10);
				text+=ch;
				text+=",";
				itoa(pminy,ch,10);
				text+=ch;
				//cvPutText(pFrame,text.c_str(),cvPoint(pminx,pminy),&font,CV_RGB(255,0,0));
				cvPutText(imageresize,text.c_str(),tipCord,&font,CV_RGB(255,125,0));

				text = "";
				itoa(pmaxx,ch,10);
				text+=ch;
				text+=",";
				itoa(pmaxy,ch,10);
				text+=ch;
				cvPutText(imageresize,text.c_str(),cvPoint(pmaxx,pmaxy),&font,CV_RGB(255,0,0));

				for(vector<blockTrack>::iterator ita=trackBlock.begin(); ita!=trackBlock.end();)	//跟踪物体循环
				{
					CvScalar lineColor = CV_RGB(0,255,0);
					if(!ita->flag)	//在当前没有检测到该物体，去除检测物体					
						ita = trackBlock.erase(ita);
					else								
					{
						CvRect rect = ita->rect;
						CvPoint ptm = cvPoint(rect.x+ rect.width/2, rect.y + rect.height/2);	//取得中心点

						if(ptm.x<pminx||ptm.x>pmaxx||ptm.y<pminy||ptm.y>pmaxy){
							ita->flag = false;
							ita++;
							continue;
						}
						//验证是否逆行
						int frame_diff = 8;
						int frame_slow = 10;

						if(ita->step > frame_slow)
						{
							double a,b;
							CalStraitLine(ita->pt,ita->step - frame_diff,ita->step,a,b);

							CvPoint sp1;
							CvPoint sp2;
							int pt_s = ita->step - frame_diff;
							int pt_s_2 = ita->step - frame_slow;
							int pt_e = ita->step;

							if(-1 <= b <= 1 && ita->pt[pt_e].x >= ita->pt[pt_s].x)
							{
								sp1.x = 50;
								sp1.y = a+b*50;
								sp2.x = 100;
								sp2.y = a+b*100;
							}

							else if(-1 <= b <= 1 && ita->pt[pt_e].x < ita->pt[pt_s].x)
							{
								sp1.x = 100;
								sp1.y = a+b*100;
								sp2.x = 50;
								sp2.y = a+b*50;
							}
							else if((b > 1 || b < -1) && ita->pt[pt_e].y >= ita->pt[pt_s].y)
							{
								sp1.x = (50 - a) / b;
								sp1.y = 50;
								sp2.x = (100 - a) / b;
								sp2.y = 100;
							}
							else
							{
								sp1.x = (100 - a) / b;
								sp1.y = 100;
								sp2.x = (50 - a) / b;
								sp2.y = 50;
							}

							if(stop_detection(ita->pt[pt_s_2],ita->pt[pt_e]))
							{
								//cout<<abs(ita->pt[pt_e].y - ita->pt[pt_s].y)<<"慢行:"<<congestion_num<<endl;
								//cvResetImageROI(pFrame);
								lineColor = CV_RGB(255,0,0);
								//cvSetImageROI(pFrame,roi);
							}

							else if(checkNiXing(sp1,sp2))// && (ita->pt[pt_e].y - ita->pt[pt_s].y) * (pt2.y - pt1.y) < 0 && abs(ita->pt[pt_e].y - ita->pt[pt_s].y) > 6)
							{
								//cout<<ita->pt[pt_e].x<<"   "<<pt1.x-roiPt1.x<<endl;
								//	if ( (pt1.y>pt2.y && ita->pt[pt_e].x < (pt1.x-roiPt1.x)) || (pt1.y<pt2.y && ita->pt[pt_s].x > (pt1.x-roiPt1.x)) )
								//	{
								//cvResetImageROI(pFrame);
								cvPutText(imageresize,"ALARM!!!",ita->pt[pt_e],&font,CV_RGB(255,0,0));
								lineColor = CV_RGB(255,255,0);
								//**********************************************
								//截图并保存；
								//**********************************************
								Rect tmpR;
								Mat frame = Mat(imageresize);
								tmpR.x = frame.cols*(float)rect.x/(float)pFrame->width;
								tmpR.y = frame.rows*(float)rect.y/(float)pFrame->height;
								tmpR.width = frame.cols*(float)rect.width/(float)pFrame->width;
								tmpR.height = frame.rows*(float)rect.height/(float)pFrame->height;
								Mat tosave = frame(tmpR);
								CTime CurTime = CTime::GetCurrentTime();
								string name = "";
								char ch[10];
								int val = CurTime.GetYear();
								itoa(val,ch,10);
								name+=ch;

								val = CurTime.GetMonth();
								itoa(val,ch,10);
								name+=ch;

								val = CurTime.GetDay();
								itoa(val,ch,10);
								name+=ch;

								val = CurTime.GetHour();
								itoa(val,ch,10);
								name+=ch;

								val = CurTime.GetMinute();
								itoa(val,ch,10);
								name+=ch;

								val = CurTime.GetSecond();
								itoa(val,ch,10);
								name+=ch;

								name+=".jpg";
								name = filename+name;
								//imwrite(name,tosave);
								//cvSetImageROI(pFrame,roi);
								//	}
							}
						}

						if (ita->pt[MAX_STEP-1].x == 1)	 //画前一个路径到当前路径的线
							for(int i = 0;i<ita->step-1;i++)
								cvLine(pFrame,ita->pt[i],ita->pt[i+1],lineColor,1,8,0);

						else
							for(int i = 0;i<MAX_STEP-1;i++)
								cvLine(pFrame,ita->pt[(ita->step+i+1)%MAX_STEP],ita->pt[(ita->step+i+2)%MAX_STEP],lineColor,1,8,0);

						ita->flag = false;
						ita++;
					}
				}
				objDetRect.clear();		//把从上一帧检测到的物体清除,为下一帧检测做好准备

				cvResize(pFrame,Frame);
				drawArrow(Frame,pt1,pt2);
				//drawArrow(pFrame,pt1,pt2);
				cvRectangle(Frame,pt1_roi,pt2_roi,CV_RGB(0,0,255));
			}
			cvReleaseImage(&pFrame);
		}
		//	cvReleaseImage(&oldimage);
//loop : int x = 0 ;
		if ( tempchar != NULL )
		{
			delete [] tempchar;
			tempchar = NULL;
		}	
	templock.lock();
		RealOut++ ;
		FrameOut = RealOut%SIZE_OF_BUFFER;
	templock.unlock() ;
		//	cvReleaseImage(&imageresize);
		//	delete [] imagesData[out];
		::PostMessage(hHwnd, SHOW_HIKIMG, (WPARAM)FrameOut, (LPARAM)FrameOut);// 向hHwnd句柄PostMessage
	//	out = (out+1)%SIZE_OF_BUFFER;
	if (nFrmNum > 10000000)
	{
     nFrmNum = 1 ;
	}
}    
	cvReleaseImage( &imageresize );
	cvReleaseImage( &imageFront );	
	/*delete param ;
	delete chkdlgtemp ;*/
	delete incident ;
	delete paramdeal;
	//cvReleaseVideoWriter(&video); 
	return 0;
}

void stopThread()
{
// 	if(!g_continue)
// 		return;
	if (g_continue)
		g_continue = false;

	if(m_pThread)
	{
		WaitForSingleObject( m_pThread->m_hThread, INFINITE );
		CloseHandle( m_pThread->m_hThread );        
		m_pThread->m_hThread = NULL;
		//delete m_pThread;
		m_pThread = NULL;
	}
};

//YV12转为BGR24数据
bool YV12_To_BGR24(unsigned char* pYV12, unsigned char* pRGB24,int width, int height)
{
	if(!pYV12 || !pRGB24)
	{
		return false;
	}

	const long nYLen = long(height * width);
	const int halfWidth = (width>>1);

	if(nYLen<1 || halfWidth<1) 
	{
		return false;
	}

	// yv12's data structure
	// |WIDTH |
	// y......y--------
	// y......y   HEIGHT
	// y......y
	// y......y--------
	// v..v
	// v..v
	// u..u
	// u..u
	unsigned char* yData = pYV12;
	unsigned char* vData = &yData[nYLen];
	unsigned char* uData = &vData[nYLen>>2];

	if(!uData || !vData)
	{
		return false;
	}

	// Convert YV12 to RGB24
	int rgb[3];
	int i, j, m, n, x, y;
	m = -width;
	n = -halfWidth;
	for(y=0; y<height;y++)
	{
		m += width;
		if(!(y % 2))
			n += halfWidth;
		for(x=0; x<width;x++)   
		{
			i = m + x;
			j = n + (x>>1);
			rgb[2] = yv2r_table[yData[i]][vData[j]];
			rgb[1] = yData[i] - uv2ig_table[uData[j]][vData[j]];
			rgb[0] = yu2b_table[yData[i]][uData[j]];
			//rgb[2] = int(yData[i] + 1.370705 * (vData[j] - 128)); // r
			//rgb[1] = int(yData[i] - 0.698001 * (uData[j] - 128)  - 0.703125 * (vData[j] - 128));   // g
			//rgb[0] = int(yData[i] + 1.732446 * (uData[j] - 128)); // b		

			//j = nYLen - iWidth - m + x;
			//i = (j<<1) + j;    //图像是上下颠倒的

			j = m + x;
			i = (j<<1) + j;

			for(j=0; j<3; j++)
			{
				if(rgb[j]>=0 && rgb[j]<=255)
					pRGB24[i + j] = rgb[j];
				else
					pRGB24[i + j] = (rgb[j] < 0)? 0 : 255;
			}
		}
	}

	if (pRGB24 == NULL)
	{
		return false;
	}

	return true;
}
bool YV12ToBGR24_OpenCV(unsigned char* pYUV,unsigned char* pBGR24,int width,int height)
{
	if (width < 1 || height < 1 || pYUV == NULL || pBGR24 == NULL)
		return false;
	Mat dst(height,width,CV_8UC3,pBGR24);
	Mat src(height + height/2,width,CV_8UC1,pYUV);
	cvtColor(src,dst,CV_YUV2BGR_YV12);
	return true;
}
bool YV12_To_IplImage(IplImage* pImage,unsigned char* pYV12, int width, int height)
{
	if (!pYV12)
	{
		return NULL;
	}

	int sizeRGB = width* height *3;
	unsigned char* pRGB24 = new unsigned char[sizeRGB];
	memset(pRGB24, 0, sizeRGB);

	//if(YV12_To_BGR24(pYV12, pRGB24 ,width, height) == false || (!pRGB24))
	//{
	//	return NULL;
	//}

	if(YV12ToBGR24_OpenCV(pYV12, pRGB24 ,width, height) == false || (!pRGB24))
	{
		delete [] pRGB24;
		return NULL;
	}

	//pImage = cvCreateImage(cvSize(width, height), 8, 3);
	pImage->origin = 0;
	if(!pImage)
	{
		delete [] pRGB24;
		return NULL;
	}

	memcpy(pImage->imageData, pRGB24, sizeRGB);
	if (!(pImage->imageData))
	{
		delete [] pRGB24;
		return NULL;
	}

	delete [] pRGB24;
	return true ;
	//	return pImage;
}

bool YV12_To_Gray(IplImage* pImage,unsigned char* pYV12, int width, int height)
{
	if (!pYV12)
	{
		return NULL;
	}

	int sizeGRAY = width* height ;

	pImage->origin = 1;
	if(!pImage)
	{
		return NULL;
	}

	memcpy(pImage->imageData, pYV12, sizeGRAY);
	if (!(pImage->imageData))
	{
		return NULL;
	}
	return 1 ;
	//	return pImage;
}

//IplImage* YV12_To_IplImage(IplImage* pImage,unsigned char* pYV12, int width, int height)
//{
//	if (!pYV12)
//	{
//		return NULL;
//	}
//
//	int sizeRGB = width* height *3;
//	unsigned char* pRGB24 = new unsigned char[sizeRGB];
//	memset(pRGB24, 0, sizeRGB);
//
//	if(YV12_To_BGR24(pYV12, pRGB24 ,width, height) == false || (!pRGB24))
//	{
//		return NULL;
//	}
//
//	pImage = cvCreateImage(cvSize(width, height), 8, 3);
//	pImage->origin = 1;
//	if(!pImage)
//	{
//		return NULL;
//	}
//
//	memcpy(pImage->imageData, pRGB24, sizeRGB);
//	if (!(pImage->imageData))
//	{
//		return NULL;
//	}
//
//	delete [] pRGB24;
//	return pImage;
//}

//int detectFace(Mat frame)
//{
//
//	char FrameFilename[53];
//	char FaceFilename[48];
//	char LogContent[70];
//	SYSTEMTIME sys;
//	std::vector<Rect> faces;
//	Mat frame_gray;
//	if(face_cascade.empty())
//		return -1;
//	cvtColor( frame, frame_gray, CV_BGR2GRAY );
//	cvWaitKey(1);
//	equalizeHist( frame_gray, frame_gray );
//	IplImage temp(frame);
//	//-- Detect faces
//	face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(80, 80) );
//	TRACE("face size:%d\n",faces.size());
//	if(faces.size() >0)
//		GetLocalTime( &sys );
//	for( int i = 0; i < faces.size(); i++ )
//	{ 
//		sprintf(FaceFilename,"F:\\HikGrab\\faceshot\\%4d%02d%02d_%02d%02d%02d_%03d_%02d.bmp",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,i);
//		sprintf(LogContent,"%4d%02d%02d %02d:%02d:%02d:%03d\t%s\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,FaceFilename);
//		log("F:\\HikGrab\\log.txt",LogContent);
//		Mat face = frame(faces[i]);
//		imwrite(FaceFilename, face);
//
//		Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
//		ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
//	}
//	if(faces.size()>0)
//	{
//		sprintf(FrameFilename,"F:\\HikGrab\\snapshot\\%4d%02d%02d_%02d%02d%02d_%03d.bmp",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds);
//		imwrite(FrameFilename,frame);
//	}
//	imshow("result",frame);
//	//cvReleaseMat(frame);
//	//-- Show what you got
//	//imshow("result",frame);
//	//imshow( window_name, frame );
//	return faces.size(); 
//}

void log(char* filename,char* content)
{
	FILE * fp ;
	if((fp = fopen(filename,"a"))==NULL)
	{
		return;
	}
	fputs(content,fp);
	fclose(fp);

}

void getSystemTime(string & str, CTime CurTime){
	string name = "";
	char ch[100];
	int val = CurTime.GetYear();
	itoa(val,ch,10);
	name += "";
	name+=ch;

	val = CurTime.GetMonth();
	itoa(val,ch,10);
	name += "--";
	name+=ch;


	val = CurTime.GetDay();
	itoa(val,ch,10);
	name += "--";
	name+=ch;


	val = CurTime.GetHour();
	itoa(val,ch,10);
	name += "--";
	name+=ch;


	val = CurTime.GetMinute();
	itoa(val,ch,10);
	name += "--";
	name+=ch;


	val = CurTime.GetSecond();
	itoa(val,ch,10);
	name += "--";
	name += ch;
	name += "\n";
	str += name;
}

//违章检测——视频
int iFrameCount = 0;
UINT  ProcessFile(LPVOID lpParameter)
{
	//int iFrameCount;
	//bool backtag=TRUE;
	//float gme=0.0f;
	//float gmm = 0.0f;

	threadParam* param= (threadParam *)lpParameter;    // 转化参数
	HWND hHwnd = param->hHwnd;
	delete lpParameter;

	cv::VideoCapture vc;
	vc.open(fileName);
	int count=0;

	vector<CvRect> objDetRect(objDetRectNumber);	//矩形区域，保存当前帧检测到的物体的区域
	objDetRect.clear();
	vector<blockTrack> trackBlock(objDetRectNumber);	 //跟踪物体对象	
	trackBlock.clear();
	IplImage* pFrame = NULL;
	CvMat* pFrameMat = NULL;//pFrame对象
	IplImage* pAfter = NULL;CvMat* pAfterMat = NULL;//保存pFrame对应的灰度图像
	IplImage* segMap = NULL;CvMat* segMat = NULL;//保存处理后的信息
	bool matchValue = false;
	objDetRect.clear();	//目标检测和跟踪向量，可转换成为数组
	trackBlock.clear();
	//创建一个随机数生成器
	RNG rng(0xFFFFFFFF);
	//定义字体
	CvFont font;
	cvInitFont( &font, CV_FONT_HERSHEY_COMPLEX_SMALL ,1, 1, 0, 1, 8);
	string key;
	stringstream sstr; 
	Rect val;
	Mat bg;
	CarNode node(key,val,bg);
	Vehicle car;
	Vehicle car_static;
	
	
	nFrmNum = 0;
	char strSetSeq[8] = {0} ;
//	imageresize = cvCreateImage(cvSize(hik_width_org/RATE,hik_height_org/RATE),IPL_DEPTH_8U,3);  
	imageFront = cvCreateImage(cvSize(hik_width_org/RATE,hik_height_org/RATE),IPL_DEPTH_8U,1);  
//	IplImage* oldimage = cvCreateImage(cvSize( hik_width_org,hik_height_org )  , imageresize->depth ,imageresize->nChannels   ) ;
	IplImage * ImageRoi = NULL ;
	vector<Rect> rectRoi ; 
	int  iNumSeq  = -1 ;
	nFrmNum = 0;    
	Vehicle  vehSuspectCarStatic[4] ;
	Vehicle  vehSuspectCar ;
	bool bTakePic = false ;
	cv::Mat frame;
	vc >> frame;

	IplImage  imagetemp11(frame) ;

	imageresize = cvCreateImage( cvSize(imagetemp11.width , imagetemp11.height) , imagetemp11.depth , imagetemp11.nChannels );  
	IplImage *oldimage = cvCreateImage( cvSize(imagetemp11.width , imagetemp11.height) , imagetemp11.depth , imagetemp11.nChannels );  

	CascadeClassifier cascade_car; //Opencv的级联分类器
	const char* cascade_name = "data/cascade_car.xml";
	cascade_car.load(cascade_name);

	//**********************************************************
	//分类器目录
	//
	//**********************************************************
	//const char* cascade_nixing_name = "xml/cascade1.xml";
	//const char* cascade_nixing_name = "xml/cascade2.xml";
	const char* cascade_nixing_name = "xml/cascade3.xml";
	//const char* cascade_nixing_name = "xml/cascade4.xml";
	cascade_nixing->load(cascade_nixing_name);
	CTime CurTime = CTime::GetCurrentTime();//intialize
	playRecord += "********************************************\nfile name : ";
	playRecord += fileName +" \n start at: ";
	getSystemTime(playRecord, CurTime);

	//fileNameNow = fileName.c_str();
	//	GetDlgItem(IDC_BUTTON_OUT)->SetWindowTextA("停止检测");
	int AllFilePathcount = 0;
	while(g_continue)
	{
		//HANDLE h[2] = {g_hFullSemaphore,g_hMutex};
		//if(WaitForMultipleObjects(2,h,TRUE,100)==WAIT_TIMEOUT)
		//	continue;
		if ( vc.isOpened() )
		{
			//转换YV12到RGB
			//IplImage* imageresize = cvCreateImage(cvSize(640,360),IPL_DEPTH_8U,3);  
			// imageresize = cvCreateImage(cvSize(SCREEN_WIDTH,SCREEN_HEIGHT),IPL_DEPTH_8U,3);  
			/** 读取当前视频帧 */
			cv::Mat frame;
			vc >> frame;

			//fileNameNow = fileName.c_str();
			iFrameCount += 1;
			frameNumber.Format("%d", iFrameCount);
			//	AfxUpdateData(FALSE);
			::PostMessage(hHwnd, WM_MYUPDATEDATA, (WPARAM)out, (LPARAM)out);
			/** 若视频读取完毕，跳出循环 */
			if ( frame.empty() )
			{
				//重新读取视频，将帧序号设定在0处
				/*vc.set(CV_CAP_PROP_POS_FRAMES, 0);
				vc >> frame;*/
				/*g_continue=0;
				break;*/
				//mark 20150624
				//gettime
				CurTime = CTime::GetCurrentTime();
				playRecord += "end at: ";
				getSystemTime(playRecord, CurTime);

				playRecord += "总帧数: ";
				playRecord += frameNumber ; 
				playRecord += "\n";
				//ofstream playRecordfile("data//playRecord.txt");
				fstream playRecordFile("data//playRecord.txt",fstream::app);
				playRecordFile << playRecord;
				playRecordFile.flush();
				playRecordFile.close();
				iFrameCount = 0;
				//循环读取AllFilePath
				AllFilePathcount = (AllFilePathcount + 1) % AllFilePath.size();
				string path = AllFilePath[AllFilePathcount];

				CurTime = CTime::GetCurrentTime();
				playRecord += "********************************************\nfile name :";
				playRecord += path + "\nstart at : ";
				getSystemTime(playRecord, CurTime);

				//fileName = path;
				fileNameNow = path.c_str();
				::PostMessage(hHwnd, WM_MYUPDATEDATA, (WPARAM)out, (LPARAM)out);

				if (!vc.open(path))continue;

				vc >> frame;
			}

		//	imageresize(frame) ;

		 IplImage imagetemp(frame);
			//oldimage->origin = 1;

	
			cvResize(&imagetemp,imageresize,CV_INTER_LINEAR);		//重新定义图像的大小
			cvCopy(	imageresize , 	oldimage) ;
			
			
		//	cvReleaseImage(&images[out]);//只是将变量值赋为NULL，而这个变量本身还是存在的
		//	images[out] = imageresize;

			if(iParkingDetect)//违停
			{
				try{
				    itoa( iPNum , strSetSeq,10 ) ;		
			if ( iPNum != iNumSeq )
			{
				iNumSeq = iPNum ;
				for(int ii = 0 ; ii < 4 ; ii++)
				{
					vehSuspectCarStatic[ii].carData.clear() ;
				}
			}
		//		area_threhold = size.width * size.height / 1000;
			//逐帧读取视频并进行处理
			nFrmNum++;
			if (bZoomed_Dynamic == false && bZoomed_Static == false)
			{	
			////////////////////////////////////////背景建模//////////////////////////////
			 try{
			InitialRoi(	WrongPark , rectRoi ); 
			 }
			 catch(...)
			 {
				 int x = 0 ;
			 }

			int iNumofRoi  ;
			CvPoint ptOffset ;

		if (rectRoi.size() > 0 )
		{
			 iNumofRoi = rectRoi.size() ;
		}
		else{
			iNumofRoi = 1 ;
		}
			for( int i = 0 ; i< iNumofRoi ; i++  ) 
			{
				try{
				if (rectRoi.size()> 0 )
				{
					ImageRoi = cvCreateImage(cvSize(rectRoi[i].width , rectRoi[i].height), imageresize->depth , imageresize->nChannels ) ; 
					cvSetImageROI(	imageresize , rectRoi[i])  ;
					cvCopy( imageresize   , ImageRoi  ) ;
					cvResetImageROI(imageresize) ;
				ptOffset.x =  rectRoi[i].x ;
				ptOffset.y =  rectRoi[i].y ;
				}
				else
				{
					ImageRoi = cvCreateImage(cvSize(imageresize->width , imageresize->height), imageresize->depth , imageresize->nChannels ) ; 
					cvCopy( imageresize  , ImageRoi  ) ;
					ptOffset.x =  0 ;
					ptOffset.y =  0 ;
				}
				}
				catch(...){
					int x = 0 ;
				}
			//	vibeFore->ViBe_Model(  imageresize ,  imageFront ,  bResetBG  ) ;
				/*	cvShowImage("111",imageFront) ;
				waitKey(1) ;*/
			//	GetForegroundInfo(	imageFront , objDetRect	) ;
    		//	TargetMatch( imageresize, bg, vehSuspectCar , trackBlock ,objDetRect  ,matchValue    )  ;		
	
				///////////////////////////////////静止车辆检测////////////////////////////			
			//遮挡后出现
			if ( nFrmNum % 150 == 0)
			{	
				/* cvShowImage("111",ImageRoi) ;
				waitKey(1) ;*/
				try{
				StaticCarDetect( ImageRoi  , vehSuspectCarStatic[i] ,vehSuspectCar ,cascade_car , ptOffset  )  ;  	
				}
				catch(...)
				{
				int x  = 0 ;
				}		

			}
			///////////////////每隔一段加1 ////////////////////////
			if ( nFrmNum % 30 == 0 )
			{
				try
				{
						ParkCarCount( ImageRoi , vehSuspectCarStatic[i] , CarPositions->rectRoi , ptOffset ) ;
				}
				catch (...)
				{
					int x= 0 ;
				}
			}
			if (vehSuspectCarStatic[i].carData.size() == 0 )
			{
				Sleep(50) ; 
			}
			if (ImageRoi != NULL)
			{
				cvReleaseImage(&ImageRoi) ;
			}
			}
			if ( nFrmNum % 150 == 0 )
			{
			Mat  matSrc(oldimage) ; 
			CarPositions->matOrigin = matSrc ;
			if (CarPositions->rectRoi.size() != 0  && bCarPlateRec == false )
			  {
				bCarPlateRec = true ;
				cvSaveImage("0011.jpg" , oldimage  );
				strcpy(strSeq  , strSetSeq   ) ; 
				CarPositions->iPresentP = iPNum ;
				CarPositions->rectRoiCopy =  CarPositions->rectRoi ; 
				HANDLE fff = CreateThread(NULL,0, CarPlateAnalysis, CarPositions,NULL,NULL)  ; 
				for (int i = 0 ; i < 4 ; i++)
				{
					vehSuspectCarStatic[i].carData.clear() ;	
				}
				
			    }
			 }
			rectRoi.clear() ;
		    }
///////////////////////////////////////////////////////////
			if ( CarPositions->Carplates.size() != 0 && bCarPlateRec == false )
			{
				try{
					if (CarPositions->iPresentP == iPNum )
					{
					CandidateCar( CarPositions ,WrongPark ,iPNum ) ;
					}
				}
				catch(...)
				{
					int x = 0 ;
				}
					CarPositions->Carplates.clear() ;
					CarPositions->Platecolor.clear() ;
					CarPositions->Positions.clear() ;
			}

			if (  WrongPark->vecCar_Static[iPNum].carData.size()!=0 && bCarPlateRec == false  )
			{
				try{
			//	  CamaraZoomIn(  imageresize , oldimage , WrongPark->vecCar_Static[iPNum]  , pStruPointFrame ,lPlayHandle , strSetSeq ) ;
				}
				catch(...)
				{
					int x = 0 ;
				}	  
				  if (bZoomed_Static == true||bZoomed_Dynamic == true )	//拍照
				{   
				try{
		    	//  CamaraReturn ( oldimage, WrongPark->vecCar_Static[iPNum] , lPlayHandle ,iPNum ,strSetSeq ) ;
				}
				  catch(...)
				  {
					  int x = 0 ;

				  }
				  }
			  if ( bWaitCount == true && iWCount > 0 )
			  {
				  int iEnd = GetTickCount()  ;
				  int iBias = iEnd - iWCount ;
				  if (iBias > 10000)
				  {
					  bWaitCount = false  ;
					  bZoomed_Dynamic = false ;
					  bZoomed_Static = false ;
					  bTakePic = true ;
				  }
			  }
			}
			if (bZoomed_Dynamic == true || bZoomed_Static == true)
			{
			int iTimeCur = GetTickCount() ;

			int iTimeGap11 = iTimeCur - iZoomTimeStart  ;
			if (iTimeGap11 > 25000	)
			{
	//			NET_DVR_PTZPreset(lPlayHandle,GOTO_PRESET,iPNum); 
				bZoomed_Static = false ;
				bZoomed_Dynamic = false ;
			}
			}
			else{
				try{
				ShowRoi(WrongPark) ;
				}
				catch(...)
				{
					int x = 0 ;

				}
			}
			if (nFrmNum	%200==0 )
			{
				CString temp ;

			try{
			PictureProccess( WrongPark ,temp) ;
			}
			catch(...)
			{
				int x = 0 ;

			}		
			}

			objDetRect.clear();		//把从上一帧检测到的物体清除,为下一帧检测做好准备
			cvReleaseMat(&pFrameMat);
			//	images[out] = pFrame;	
			 CruiseProsess(	WrongPark  , iPNum  , bTakePic ) ;
		}
			catch(...){
				int x = 0 ;
			}

				//Mat mat = Mat(images[out]).clone();
				//IplImage* output = RGBmodeling(&IplImage(mat));
				//images[out] = output;
				/*int alarm_area = RABDetection(images[out],area,pointNum);*/

				//IplImage* pFrame = images[out];
				//size.width  = pFrame->width;
				//size.height = pFrame->height;
				//area_threhold = size.width * size.height / 1000;

				////逐帧读取视频并进行处理
				//nFrmNum++;

				//if(nFrmNum == 1)
				//{
				//	segMap =    cvCreateImage(size, IPL_DEPTH_8U,1);
				//	segMat =    cvCreateMat(size.height, size.width, CV_32FC1);
				//	pAfter =    cvCreateImage(size, IPL_DEPTH_8U,1);
				//	pAfterMat = cvCreateMat(size.height, size.width, CV_32FC1);

				//	//转化成单通道图像再处理
				//	cvCvtColor(pFrame, pAfter, CV_BGR2GRAY);
				//	cvConvert(pAfter, pAfterMat);
				//	Initialize(pAfterMat,rng);//初始化
				//}

				//else 
				//{
				//	cvCvtColor(pFrame, pAfter, CV_BGR2GRAY);//get gray image:pAfter
				//	cvConvert(pAfter,pAfterMat);//线性变换转换

				//	update(pAfterMat,segMat,rng,nFrmNum);//更新背景
				//	cvConvert(segMat,segMap);//?

				//	//cvErode()腐蚀,cvDilate()膨胀，先开运算再闭运算
				//	//先腐烛后膨胀的过程称为开启运算,这样图像中的小像素块和孤立的点状就会被去掉
				//	cvErode  (segMap, segMap);
				//	cvDilate (segMap, segMap);

				//	//先膨胀后腐烛的过程称为关闭运算,可以使得图像中连续区域内的孔洞愈合
				//	cvDilate (segMap, segMap);
				//	cvErode  (segMap, segMap);

				//	Mat segMapRoi(segMap);
				//	Mat pFrameMat(pFrame);

				//	//cvThreshold(segMap, segMap, 100, 255, CV_THRESH_BINARY);//此阈值大小影响到是否连通
				//	cvAdaptiveThreshold(segMap, segMap, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 3, 5);    
				//	CvMemStorage* storage = cvCreateMemStorage(0);
				//	CvSeq* contours = 0;

				//	//从二值图像中检索轮廓，并返回检测到的轮廓的个数
				//	int cnt = cvFindContours(segMap, storage, &contours, sizeof(CvContour),2, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

				//	vector<KeyPoint> keypoints;

				//	for(CvSeq* p = contours;p!=NULL;p=p->h_next)//依次画出各个区域
				//	{
				//		double area = cvContourArea(p); 
				//		if(area < MIN_AREA)
				//			continue;

				//		//点集的最外面矩形边界
				//		CvRect rect1 = cvBoundingRect(p,1);
				//		CvRect rect;
				//		Mat imageroi(segMapRoi,rect1);

				//		//角点检测 Features from Accelerated Segment Test
				//		FastFeatureDetector fast(40,true);//检测的阈值为40
				//		fast.detect(imageroi,keypoints);

				//		aspect_ratio = ((double)rect1.width) / ((double)rect1.height);
				//		rect_area = rect1.width * rect1.height;

				//		//Mat image_pedestrian(pFrameMat,rect1);
				//		if (aspect_ratio > Min_aspect_ratio && 
				//			aspect_ratio < Max_aspect_ratio && 
				//			rect_area > area_threhold && 
				//			rect_area < size.width * size.height / 2 &&
				//			keypoints.size() > Min_KeyPointSize) //&& pedestrian(image_pedestrian))
				//		{
				//			rect = rect1;
				//			/*  rect.x = rect.x;
				//			rect.y = rect.y;*/
				//			rect.x = rect1.x;
				//			rect.y = rect1.y;
				//			objDetRect.push_back(rect);
				//			cvRectangle(segMap,cvPoint(rect.x,rect.y),cvPoint(rect.x+rect.width,rect.y+rect.height),CV_RGB(255,255,255));
				//			cvRectangle(pFrame,cvPoint(rect.x,rect.y),cvPoint(rect.x+rect.width,rect.y+rect.height),CV_RGB(0,0,255));
				//			keypoints.clear();
				//		}

				//	}
				//	cvReleaseMemStorage(&storage);
				//}

				///*******************************************************************************************************************************************************************************************/
				////******跟踪目标初始化
				//if (trackBlock.empty())	//如果跟踪为空，把上一帧检测到的物体rect保存为跟踪物体
				//{
				//	for(vector<CvRect>::iterator it = objDetRect.begin();it!=objDetRect.end();++it)	//检测到有物体，执行跟踪
				//	{
				//		blockTrack blockTemp;
				//		blockTemp.flag = false;	//是否在当前帧检测到
				//		blockTemp.pt[0] = cvPoint(it->x + it->width/2, it->y + it->height/2);	//初始位置，取矩形中心作为质心
				//		blockTemp.pt[MAX_STEP-1] = cvPoint(1, 1);  //最后位置
				//		blockTemp.rect = *it;	//位置rect
				//		blockTemp.step = 0;		//步数
				//		trackBlock.push_back(blockTemp);	//作为新发现的跟踪目标插入
				//	}
				//}

				////******跟踪目标匹配，从当前帧检测到的物体，匹配是否有之前的跟踪记录，有则更新，没有则加上
				//for(vector<CvRect>::iterator it = objDetRect.begin(); it!=objDetRect.end(); ++it)	//当前帧检测到的物体
				//{
				//	CvPoint ptm = cvPoint(it->x+ it->width/2, it->y + it->height/2);	//取中心点

				//	for(vector<blockTrack>::iterator itt = trackBlock.begin(); itt != trackBlock.end(); ++itt) //跟踪目标
				//	{
				//		if (itt->flag)
				//			continue;
				//		if(match(*it,itt->rect))  //做两重循环，匹配检测到的物体和之前跟踪到的物体，并更新跟踪物体信息
				//		{
				//			itt->rect = *it;
				//			itt->step = (itt->step+1)%MAX_STEP;	//如果超过了阈值则返回0记录
				//			itt->pt[itt->step] = ptm;	//更新路径
				//			itt->flag = true;	//在当前帧检测到
				//			matchValue = true;
				//			break;	//匹配成功，找到跟踪物体的新路径，跳出循环
				//		}
				//	}

				//	if (!matchValue)	//如果没有找到之前有跟踪的记录，创建新的跟踪记录
				//	{
				//		blockTrack temp;
				//		temp.flag = true;	//当前帧检测到
				//		temp.pt[0] = ptm;	//起点
				//		temp.pt[MAX_STEP-1] = cvPoint(1, 1);//终点	
				//		temp.rect = *it;
				//		temp.step = 0;
				//		trackBlock.push_back(temp);
				//	}

				//	matchValue = false;
				//}

				////******目标更新，判断是否进入了警戒区，划线
				//for(vector<blockTrack>::iterator ita=trackBlock.begin(); ita!=trackBlock.end();)	//跟踪物体循环
				//{
				//	CvScalar lineColor = CV_RGB(0,255,0);

				//	if(!ita->flag)	//在当前没有检测到该物体，去除检测物体					
				//		ita = trackBlock.erase(ita);
				//	else								
				//	{
				//		CvRect rect = ita->rect;
				//		CvPoint ptm = cvPoint(rect.x+ rect.width/2, rect.y + rect.height/2);	//取得中心点

				//		if(ita->step > 10)
				//		{
				//			if( stop_detection(ita->pt[ita->step-10],ita->pt[ita->step]))
				//			{
				//				val = ita->rect;
				//				sstr<<val.x + val.width/2<<" "<<val.y + val.height/2;  
				//				key = sstr.str();
				//				sstr.str("");

				//				if (!car.match(key,val))
				//				{				
				//					Mat weiting(pFrame, val);
				//					bg = weiting;
				//					cvtColor(bg,bg,CV_BGR2GRAY);
				//					cvtColor(weiting,weiting,CV_BGR2GRAY);

				//					cvThreshold(&IplImage(weiting), &IplImage(weiting),
				//						100, 255, CV_THRESH_BINARY);
				//					CvMemStorage* storage = cvCreateMemStorage(0);
				//					CvSeq* contours = 0;
				//					int cnt = cvFindContours(&IplImage(weiting), storage, &contours, 
				//						sizeof(CvContour),2, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

				//					double area_max = 0;
				//					for(CvSeq* q = contours;q != NULL;q = q->h_next)
				//					{
				//						double area = cvContourArea(q); 
				//						if (area < Area_Threhold)
				//							continue;
				//						if(area > area_max)
				//							area_max = area;
				//					}

				//					if ( area_max != 0)
				//					{
				//						car.Insert(key, val, bg);
				//						cout<<key<<" created."<<endl;
				//						//cvPutText(pFrame,"Start",cvPoint(val.x,val.y),&font,CV_RGB(255,0,0));

				//						car.umap[key]->frame_s = GetTickCount();;
				//						car.umap[key]->contour_area = area_max;
				//						car.umap[key]->photo_flag = false;
				//						car.umap[key]->bFirstPhoto = 0 ;
				//						car.umap[key]->bTakePhoto = 0 ;
				//						car.umap[key]->bTakePhoto = 0 ;
				//						//imshow("weiting",car.umap[key]->bg);
				//						//cout<<car.umap[key]->contour_area<<" "<<car.umap[key]->frame_s<<endl;
				//						//cvWaitKey(0);
				//					}
				//					cvReleaseMemStorage(&storage);
				//					//cout<<car.umap[key]->contour_area<<" "<<car.umap[key]->frame_s<<endl;
				//				}
				//			}
				//		}

				//		if (ita->pt[MAX_STEP-1].x == 1)	 //画前一个路径到当前路径的线
				//			for(int i = 0;i<ita->step-1;i++)
				//				cvLine(pFrame,ita->pt[i],ita->pt[i+1],lineColor,1,8,0);
				//		else
				//			for(int i = 0;i<MAX_STEP-1;i++)
				//				cvLine(pFrame,ita->pt[(ita->step+i+1)%MAX_STEP],ita->pt[(ita->step+i+2)%MAX_STEP],lineColor,1,8,0);

				//		ita->flag = false;
				//		ita++;
				//	}
				//}

				//vector<Rect> rects1;
				////遮挡后出现
				//if ( nFrmNum % 200 == 0)
				//{
				//	cout<<nFrmNum<<"!!!!!!!!!!"<<endl;

				//	//car detection
				//	rects1.clear(); 
				//	//cvSetImageROI(pFrame,roi);
				//	IplImage* pFrame_gray = cvCreateImage(cvSize(size.width,size.height), pFrame->depth, 1);
				//	cvCvtColor(pFrame, pFrame_gray, CV_BGR2GRAY);
				//	cascade_car.detectMultiScale(pFrame_gray, rects1, 1.1, 2, 0, Size(64, 64));
				//	CvMemStorage* storage = cvCreateMemStorage(0);
				//	CvSeq* objects1;
				//	objects1 = cvCreateSeq(0, sizeof(CvSeq), sizeof(Rect), storage);
				//	for(int i=0; i < rects1.size(); i++)  
				//	{  
				//		cvSeqPush(objects1, &rects1[i] );  
				//	}

				//	//match(car_static,car)
				//	for(int i=0;i<(objects1? objects1->total:0);++i) 
				//	{ 
				//		CvRect* r=(CvRect*)cvGetSeqElem(objects1,i); //返回索引指定的元素指针
				//		Point r_mid;
				//		r_mid.x = r->x + r->width/2;
				//		r_mid.y = r->y + r->height/2;
				//		CarNode *p_m;
				//		bool match_1 = true, match_2 = true;
				//		Rect val;

				//		p_m = car_static.Head()->next;
				//		while(p_m != car_static.Tail())
				//		{
				//			val = p_m->val;
				//			if (r_mid.x > val.x && r_mid.x < (val.x + val.width) && r_mid.y > val.y && r_mid.y < (val.y + val.height))
				//			{
				//				match_1 = false;
				//				break;
				//			}
				//			else
				//			{
				//				match_1 = true;
				//				p_m = p_m->next;
				//			}
				//		}

				//		p_m = car.Head()->next;
				//		while(p_m != car.Tail())
				//		{
				//			val = p_m->val;
				//			if (r_mid.x > val.x && r_mid.x < (val.x + val.width) && r_mid.y > val.y && r_mid.y < (val.y + val.height))
				//			{
				//				match_2 = false;
				//				break;
				//			}
				//			else
				//			{
				//				match_2 = true;
				//				p_m = p_m->next;
				//			}
				//		}

				//		if ( match_1 && match_2)
				//		{
				//			//cout<<r_mid.x<<"============================================="<<r_mid.y<<endl;
				//		//	cvPutText(imageresize,"Park",cvPoint(r_mid.x,r_mid.y),&font,CV_RGB(0,255,0));
				//			sstr<<r_mid.x<<" "<<r_mid.y;  
				//			key = sstr.str();
				//			sstr.str("");
				//			Rect car_ignore;
				//			car_ignore.x = r->x;
				//			car_ignore.y = r->y;
				//			car_ignore.width = r->width;
				//			car_ignore.height = r->height;
				//			Mat bg_ignore(pFrame, car_ignore);
				//			cvtColor(bg_ignore,bg_ignore,CV_BGR2GRAY);
				//			//cvThreshold(&IplImage(bg_static), &IplImage(bg_static), 100, 255, CV_THRESH_BINARY);
				//			cvAdaptiveThreshold(&IplImage(bg_ignore), &IplImage(bg_ignore), 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 3, 5);
				//			storage = cvCreateMemStorage(0);
				//			CvSeq* contours = 0;
				//			int cnt = cvFindContours(&IplImage(bg_ignore), storage, &contours, sizeof(CvContour),2, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
				//			double area_max = 0;
				//			for(CvSeq* q = contours;q != NULL;q = q->h_next)
				//			{
				//				double area = cvContourArea(q); 
				//				if(area < 1000)
				//					continue;
				//				if(area > area_max)
				//					area_max = area;
				//			}

				//			if ( area_max != 0)
				//			{
				//				car_static.Insert(key, car_ignore, bg_ignore);
				//				cout<<key<<" created."<<endl;

				//				car_static.umap[key]->frame_s = nFrmNum;
				//				car_static.umap[key]->contour_area = area_max;
				//				car_static.umap[key]->photo_flag = false;
				//				car_static.umap[key]->bFirstPhoto = 0;
				//				car_static.umap[key]->bLastPhoto = 0;
				//				car_static.umap[key]->bTakePhoto = false;
				//				cout<<car_static.umap[key]->contour_area<<" "<<car_static.umap[key]->frame_s<<endl;
				//			}
				//		}

				//	}
				//	cvReleaseImage(&pFrame_gray); 
				//	cvReleaseMemStorage(&storage);
				//}

				//if( nFrmNum % 30 == 0 )
				//{
				//	CarNode *p;
				//	p = car.Head()->next;

				//	CarNode *p_s;
				//	p_s = car_static.Head()->next;			

				//	while(p_s != car_static.Tail())
				//	{
				//		Mat bg_ori = p_s->bg;
				//		Mat bg_cur(pFrame, p_s->val);
				//		cvtColor(bg_cur,bg_cur,CV_BGR2GRAY);

				//		//得到二值图像
				//		//cvThreshold(&IplImage(bg_cur), &IplImage(bg_cur), 100, 255, CV_THRESH_BINARY);

				//		cvAdaptiveThreshold(&IplImage(bg_cur), &IplImage(bg_cur), 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 3, 5);
				//		CvMemStorage* storage = cvCreateMemStorage(0);
				//		CvSeq* contours = 0;
				//		int cnt = cvFindContours(&IplImage(bg_cur), storage, &contours, 
				//			sizeof(CvContour),2, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

				//		double area_max = 0;
				//		for(CvSeq* q_sub = contours;q_sub != NULL;q_sub = q_sub->h_next)
				//		{
				//			double area = cvContourArea(q_sub); 
				//			if(area < 1000)
				//				continue;
				//			if(area > area_max)
				//				area_max = area;
				//		}

				//		if(area_max != 0)
				//		{
				//			cout<<p_s->key<<" "<<area_max<<"  "<<p_s->val.width*p_s->val.height<<"   ";
				//			cvPutText(imageresize,"Park",cvPoint(p_s->val.x + p_s->val.width / 3,p_s->val.y + p_s->val.height / 3),&font,CV_RGB(0,255,0));

				//		}
				//		cvReleaseMemStorage(&storage);

				//		double contour_thre = 0.0;

				//		if(p_s->contour_area < p_s->val.height*p_s->val.width*0.88)
				//			contour_thre = 0.92;
				//		else
				//			contour_thre = 0.99;

				//		if(area_max >= p_s->contour_area / 6 && area_max < p_s->contour_area * 2.5 &&
				//			area_max < p_s->val.height * p_s->val.width * contour_thre)//判断车在,参数调整
				//		{
				//			p_s->frame_e = nFrmNum;
				//			int frame = p_s->frame_e - p_s->frame_s;
				//			cout<<frame<<endl;

				//			if( frame > 1000 && p_s->photo_flag == false)//判断超时否
				//			{
				//				p_s->photo_flag = true;
				//				CurTime = CTime::GetCurrentTime();
				//				sprintf(PicName, "./savedImage/%04d%02d%02d%02d%02d%02d.jpg", CurTime.GetYear(), CurTime.GetMonth(), CurTime.GetDay(), \
				//					CurTime.GetHour(), CurTime.GetMinute(), CurTime.GetSecond());
				//				Mat toSave(pFrame);
				//				imwrite(sPicName, toSave);

				//				cout<<p_s->key<<" photo taken."<<endl;//take photo
				//				cvPutText(imageresize,"Photo",cvPoint(p_s->val.x + p_s->val.width / 2,p_s->val.y + p_s->val.height / 2),&font,CV_RGB(255,0,0));
				//			}

				//		}
				//		else
				//		{
				//			cout<<area_max<<",  "<<p_s->val.height*p_s->val.width<<".   "<<p_s->key<<" deleted."<<endl;
				//			cvPutText(imageresize,"Deleted",cvPoint(p_s->val.x + p_s->val.width / 2,p_s->val.y + p_s->val.height / 2),&font,CV_RGB(255,0,0));
				//			CarNode *del_p;
				//			del_p = p_s->next;
				//			car_static.Delete(p_s->key);
				//			p_s = del_p;
				//			bg_ori.release();
				//			bg_cur.release();
				//			continue;
				//		}
				//		bg_ori.release();
				//		bg_cur.release();
				//		p_s = p_s->next;
				//	}

				//	while(p != car.Tail())
				//	{
				//		Mat bg_ori = p->bg;
				//		Mat bg_cur(pFrame, p->val);
				//		cvtColor(bg_cur,bg_cur,CV_BGR2GRAY);

				//		//imshow("sub",bg_cur);
				//		//cvAdaptiveThreshold(&IplImage(bg_cur), &IplImage(bg_cur), 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 3, 5);
				//		cvThreshold(&IplImage(bg_cur), &IplImage(bg_cur), 100, 255, CV_THRESH_BINARY);

				//		CvMemStorage* storage = cvCreateMemStorage(0);
				//		CvSeq* contours = 0;
				//		int cnt = cvFindContours(&IplImage(bg_cur), storage, &contours, sizeof(CvContour),2, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
				//		double area_max = 0;
				//		for(CvSeq* q_sub = contours;q_sub != NULL;q_sub = q_sub->h_next)
				//		{
				//			double area = cvContourArea(q_sub); 
				//			if(area < 1000)
				//				continue;
				//			if(area > area_max)
				//				area_max = area;
				//		}

				//		if(area_max != 0)
				//		{
				//			cout<<p->key<<" "<<area_max<<"  "<<p->val.width*p->val.height<<"   ";
				//			cvPutText(imageresize,"Park",cvPoint(p->val.x + p->val.width / 3,p->val.y + p->val.height / 3),&font,CV_RGB(0,255,0));

				//		}
				//		cvReleaseMemStorage(&storage);

				//		if(area_max >= p->contour_area / 6 && area_max < p->contour_area * 2.5 && area_max < p->val.height*p->val.width*0.85)//判断车在,参数调整
				//		{
				//			p->frame_e = nFrmNum;
				//			int frame = p->frame_e - p->frame_s;
				//			cout<<frame<<endl;

				//			if( frame > 1000 && p->photo_flag == false)//判断超时否
				//			{
				//				p->photo_flag = true;
				//				CurTime = CTime::GetCurrentTime();
				//				sprintf(PicName, "./savedImage/%04d%02d%02d%02d%02d%02d.jpg", CurTime.GetYear(), CurTime.GetMonth(), CurTime.GetDay(), \
				//					CurTime.GetHour(), CurTime.GetMinute(), CurTime.GetSecond());
				//				Mat toSave(pFrame);
				//				imwrite(sPicName, toSave);
				//				cout<<p->key<<" photo taken."<<endl;//take photo
				//				cvPutText(imageresize,"Photo",cvPoint(p->val.x + p->val.width / 2,p->val.y + p->val.height / 2),&font,CV_RGB(255,0,0));
				//			}					
				//		}
				//		else
				//		{
				//			cout<<p->key<<" deleted."<<endl;
				//			cvPutText(imageresize,"Deleted",cvPoint(p->val.x + p->val.width / 2,p->val.y + p->val.height / 2),&font,CV_RGB(255,0,0)); //第一个字母左下角的坐标
				//			CarNode *del_p;
				//			del_p = p->next;
				//			car.Delete(p->key);
				//			p = del_p;
				//			bg_ori.release();
				//			bg_cur.release();
				//			continue;
				//		}
				//		bg_ori.release();
				//		bg_cur.release();
				//		p = p->next;
				//	}
				//}		
				//objDetRect.clear();		//把从上一帧检测到的物体清除,为下一帧检测做好准备
				//cvReleaseMat(&pFrameMat);
				//images[out] = pFrame;

			}


			//////////////////////////////////////////////////////////////////
			//////////////////////////逆行//////////////////////////
			////////////////////////////////////////////////////////////////////
			if(isUnusualing)//逆行
			{

				if(!flag_seting)
				{
					IplImage* pFrame = images[out];
					if(!flag_setdirection)
					{
						//drawArrow(pFrame,pt1,pt2);
						cvPutText(imageresize,"Please select cars' normal direction with mouse",tipCord,&font,CV_RGB(255,0,0));//在图片中输出字符
					}else 
					{
						pt1 = pt1_dir;
						pt2 = pt2_dir;


						drawArrow(pFrame,pt1,pt2);
						//cvShowImage("pFrame",pFrame);
						//waitKey(5);
						//cvRectangle(pFrame,pt1_roi,pt2_roi,CV_RGB(255,0,0));
						//cvLine(pFrame,roiPt1,roiPt2,cvScalar(0,255,255));
						cvPutText(imageresize,"Please select roi's topleft & bottomright point",cvPoint(tipCord.x,tipCord.y),&font,CV_RGB(255,0,0));//在图片中输出字符

					}
					//cvReleaseImage(&pFrame);
					//delete  imagesData[out];

					::PostMessage(hHwnd, SHOW_HIKIMG, (WPARAM)out, (LPARAM)out);   // 向hHwnd句柄PostMessage

					out = (out+1)%SIZE_OF_BUFFER;
					ReleaseMutex(g_hMutex);
					ReleaseSemaphore(g_hEmptySemaphore,1,NULL);
					Sleep(25);
					continue;
				}
				roiPt1 = pt1_roi;
				roiPt2 = pt2_roi;
				IplImage* Frame = images[out];
				//缩小视频尺寸 degreeScale
				CvSize sizefame = degreeScale;
				pFrame = cvCreateImage(sizefame,images[out]->depth,3);

				//***************************************************
				//resize pFrame (400,200)
				cvResize(Frame,pFrame);
				//
				//***************************************************
				//mark0617
				//size.width  = pFrame->width;
				//size.height = pFrame->height;
				area_threhold = sizefame.width * sizefame.height / 1000;

				//逐帧读取视频并进行处理
				nFrmNum++;

				if(nFrmNum %1 == 0)
				{
					if(nFrmNum == 1)
					{
						segMap =    cvCreateImage(sizefame, IPL_DEPTH_8U,1);
						segMat =    cvCreateMat(sizefame.height, sizefame.width, CV_32FC1);
						pAfter =    cvCreateImage(sizefame, IPL_DEPTH_8U,1);
						pAfterMat = cvCreateMat(sizefame.height, sizefame.width, CV_32FC1);

						//转化成单通道图像再处理
						cvCvtColor(pFrame, pAfter, CV_BGR2GRAY);
						cvConvert(pAfter, pAfterMat);
						Initialize(pAfterMat,rng);
					}
					else 
					{
						cvCvtColor(pFrame, pAfter, CV_BGR2GRAY);
						cvConvert(pAfter,pAfterMat);//把IplImage转为矩阵,可以进行更自由的计算

						update(pAfterMat,segMat,rng,nFrmNum);//更新背景
						cvConvert(segMat,segMap);

						//先腐烛后膨胀的过程称为开启运算,这样图像中的小像素块和孤立的点状就会被去掉
						cvDilate (segMap, segMap);//开操作,对输入图像使用指定的结构元进行膨胀(src,dst)
						cvErode  (segMap, segMap);//腐蚀，闭操作，那些断裂处就被缝合

						//先膨胀后腐烛的过程称为关闭运算,可以使得图像中连续区域内的孔洞愈合
						cvErode  (segMap, segMap);
						cvDilate (segMap, segMap);
						//cvShowImage("vibebg",segMap);//在指定窗口中显示图像
						//waitKey(1);
						Mat segMapRoi(segMap);
						Mat pFrameMat(pFrame);
						cvThreshold(segMap, segMap, 0, 255, CV_THRESH_OTSU);
						CvMemStorage* storage = cvCreateMemStorage(0);
						CvSeq* contours = 0;
						int cnt = cvFindContours(segMap, storage, &contours, sizeof(CvContour),CV_RETR_TREE,CV_CHAIN_APPROX_TC89_L1, cvPoint(0,0));
						//vector<KeyPoint> keypoints;

						for(CvSeq* p = contours;p!=NULL;p=p->h_next)//依次画出各个区域
						{
							double area = cvContourArea(p); 
							if(area < MIN_AREA)
								continue;
							CvRect rect = cvBoundingRect(p,1);//计算点集的最外面矩形边界

							Mat imageroi(segMapRoi,rect);

							////角点检测
							//FastFeatureDetector fast(40,true);
							//fast.detect(imageroi,keypoints);

							aspect_ratio = ((double)rect.width) / ((double)rect.height);
							rect_area = rect.width * rect.height;

							Mat image_pedestrian(pFrameMat,rect);
							//area_threhold = 1000;//size.width * size.height / 400;
							area_threhold = Area_Threhold;
							//------------------------------------------------------------


							//--------------------------------------------------------------
							if (pedestrian(image_pedestrian)) //pedestrian函数中压缩到16*16
							{
								//aspect_ratio: 0.8---3.0  Yaojh
								if (aspect_ratio > dWH_RatioL && aspect_ratio < dWH_RatioG && rect_area > area_threhold)///&& keypoints.size() > 20 )
								{
									int a = 0;


									////////复制图像
									//Mat imgs(pFrameMat,rect);
									////imshow("imgs",imgs);
									//rect_cascade.clear();
									//cascade_nixing.detectMultiScale(imgs,rect_cascade,1.1,2,0,Size(25,25));
									//////add
									//for(int i=0;i<rect_cascade.size();i++){
									//	Rect r = rect_cascade[i];
									//	
									//	objDetRect.push_back(cvRect(r.x+rect.x,r.y+rect.y,r.width,r.height));
									//	cvRectangle(pFrame,cvPoint(r.x+rect.x,r.y+rect.y),cvPoint(r.x+r.width+rect.x,r.y+r.height+rect.y),CV_RGB(255,0,0));
									//	
									//	
									//}
									//origin code
									objDetRect.push_back(rect);//当前帧检测到的物体的区域

									//通过对角线上的两个顶点绘制简单、指定粗细或者带填充的矩形
									cvRectangle(pFrame, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height), CV_RGB(255, 0, 0));

									// cal the 

								}
							}

							//image_pedestrian.release();
						}
						cvReleaseMemStorage(&storage);
					}
					/*******************************************************************************************************************************************************************************************/
					//******跟踪目标初始化
					if (trackBlock.empty())	//如果跟踪为空，把上一帧检测到的物体rect保存为跟踪物体
					{
						for(vector<CvRect>::iterator it=objDetRect.begin();it!=objDetRect.end();++it)	//检测到有物体，执行跟踪
						{
							blockTrack blockTemp;
							blockTemp.flag = false;	//是否在当前帧检测到
							blockTemp.pt[0] = cvPoint(it->x + it->width/2, it->y + it->height/2);	//初始位置，取矩形中心作为质心
							blockTemp.pt[MAX_STEP-1] = cvPoint(1, 1);  //最后位置
							blockTemp.rect = *it;	//位置rect
							blockTemp.step = 0;		//步数
							trackBlock.push_back(blockTemp);	//!!!作为新发现的跟踪目标插入
						}
					}
					//******跟踪目标匹配，从当前帧检测到的物体，匹配是否有之前的跟踪记录，有则更新，没有则加上
					for(vector<CvRect>::iterator it=objDetRect.begin(); it!=objDetRect.end();++it)	//当前帧检测到的物体
					{
						CvPoint ptm=cvPoint(it->x+ it->width/2, it->y + it->height/2);	//取中心点
						for(vector<blockTrack>::iterator itt=trackBlock.begin(); itt!=trackBlock.end();++itt) //跟踪目标
						{
							if (itt->flag)
								continue;
							if(match(*it,itt->rect))  //做两重循环，匹配检测到的物体和之前跟踪到的物体，并更新跟踪物体信息
							{
								itt->rect = *it;
								itt->step = (itt->step+1)%MAX_STEP;	//如果超过了阈值则返回0记录
								itt->pt[itt->step] = ptm;	//更新路径
								itt->flag = true;	//在当前帧检测到
								matchValue = true;
								break;	//匹配成功，找到跟踪物体的新路径，跳出循环
							}
						}
						if (!matchValue)	//如果没有找到之前有跟踪的记录，创建新的跟踪记录
						{
							blockTrack temp;
							temp.flag = true;	//当前帧检测到
							temp.pt[0] = ptm;	//起点
							temp.pt[MAX_STEP-1] = cvPoint(1, 1);//终点	
							temp.rect = *it;
							temp.step = 0;
							trackBlock.push_back(temp);
						}
						matchValue = false;
					}

					//******目标更新，判断是否进入了警戒区，划线
					//AfxMessageBox("here");
					int pminx,pminy,pmaxx,pmaxy;
					if(roiPt1.x>roiPt2.x)
					{
						pminx = roiPt2.x;
						pmaxx = roiPt1.x;
					}
					else
					{
						pminx = roiPt1.x;
						pmaxx = roiPt2.x;
					}

					if(roiPt1.y>roiPt2.y)
					{
						pminy = roiPt2.y;
						pmaxy = roiPt1.y;
					}
					else
					{
						pminy = roiPt1.y;
						pmaxy = roiPt2.y;
					}

					//***********************************
					//
					pminx = sizefame.width*pminx/Frame->width;
					pmaxx = sizefame.width*pmaxx/Frame->width;
					pminy = sizefame.height*pminy/Frame->height;
					pmaxy = sizefame.height*pmaxy/Frame->height;
					//cvRectangle(pFrame,Point(pminx,pminy),Point(pmaxx,pmaxy),cvScalar(0,0,255));
					//***********************************

					//string text = "";
					//char ch[20];
					//itoa(pminx,ch,10);
					//text+=ch;
					//text+=",";
					//itoa(pminy,ch,10);
					//text+=ch;
					//cvPutText(pFrame,text.c_str(),cvPoint(pminx,pminy),&font,CV_RGB(255,0,0));
					////cvPutText(pFrame,text.c_str(),tipCord,&font,CV_RGB(255,125,0));

					//text = "";
					//itoa(pmaxx,ch,10);
					//text+=ch;
					//text+=",";
					//itoa(pmaxy,ch,10);
					//text+=ch;
					//cvPutText(pFrame,text.c_str(),cvPoint(pmaxx,pmaxy),&font,CV_RGB(255,0,0));
					//cvShowImage("pFrame",pFrame);
					//waitKey(5);
					for(vector<blockTrack>::iterator ita=trackBlock.begin(); ita!=trackBlock.end();)	//跟踪物体循环
					{
						CvScalar lineColor = CV_RGB(0,255,0);
						if(!ita->flag)	//在当前没有检测到该物体，去除检测物体					
							ita = trackBlock.erase(ita);
						else								
						{
							CvRect rect = ita->rect;
							CvPoint ptm = cvPoint(rect.x+ rect.width/2, rect.y + rect.height/2);	//取得中心点
							if(ptm.x<pminx||ptm.x>pmaxx||ptm.y<pminy||ptm.y>pmaxy)
							{
								ita->flag = false;
								ita++;
								continue;
							}
							//验证是否逆行
							/*int frame_diff = 8;
							int frame_slow = 10;*/
							int frame_diff = Frame_Diff;
							int frame_slow = Frame_Slow;


							if(ita->step > frame_slow)
							{
								double a,b;
								CalStraitLine(ita->pt,ita->step - frame_diff,ita->step,a,b);

								CvPoint sp1;
								CvPoint sp2;
								int pt_s = ita->step - frame_diff;
								int pt_s_2 = ita->step - frame_slow;
								int pt_e = ita->step;

								if(-1 <= b <= 1 && ita->pt[pt_e].x >= ita->pt[pt_s].x)
								{
									sp1.x = 50;
									sp1.y = a+b*50;
									sp2.x = 100;
									sp2.y = a+b*100;
								}

								else if(-1 <= b <= 1 && ita->pt[pt_e].x < ita->pt[pt_s].x)
								{
									sp1.x = 100;
									sp1.y = a+b*100;
									sp2.x = 50;
									sp2.y = a+b*50;
								}
								else if((b > 1 || b < -1) && ita->pt[pt_e].y >= ita->pt[pt_s].y)
								{
									sp1.x = (50 - a) / b;
									sp1.y = 50;
									sp2.x = (100 - a) / b;
									sp2.y = 100;
								}
								else
								{
									sp1.x = (100 - a) / b;
									sp1.y = 100;
									sp2.x = (50 - a) / b;
									sp2.y = 50;
								}

								if(stop_detection(ita->pt[pt_s_2],ita->pt[pt_e]))
								{
									//cout<<abs(ita->pt[pt_e].y - ita->pt[pt_s].y)<<"慢行:"<<congestion_num<<endl;
									//cvResetImageROI(pFrame);
									lineColor = CV_RGB(255,0,0);
									//cvSetImageROI(pFrame,roi);

								}

								//angle great than 90 degree
								else if(checkNiXing(sp1,sp2))// && (ita->pt[pt_e].y - ita->pt[pt_s].y) * (pt2.y - pt1.y) < 0 && abs(ita->pt[pt_e].y - ita->pt[pt_s].y) > 6)
								{
									//cout<<ita->pt[pt_e].x<<"   "<<pt1.x-roiPt1.x<<endl;
									//if ( (pt1.y>pt2.y && ita->pt[pt_e].x < (pt1.x-roiPt1.x)) || (pt1.y<pt2.y && ita->pt[pt_s].x > (pt1.x-roiPt1.x)) )
									//	{
									//cvResetImageROI(pFrame);
									cvPutText(imageresize,"ALARM!!!",ita->pt[pt_e],&font,CV_RGB(255,0,0));
									lineColor = CV_RGB(255,255,0);

									//**********************************************
									//截图并保存；
									//**********************************************
									//MARK0617									
									Rect tmpR;

									tmpR.x = frame.cols*(float)rect.x/(float)pFrame->width;
									tmpR.y = frame.rows*(float)rect.y/(float)pFrame->height;
									tmpR.width = frame.cols*(float)rect.width/(float)pFrame->width;
									tmpR.height = frame.rows*(float)rect.height/(float)pFrame->height;
									Mat tosave = frame(tmpR);
									CurTime = CTime::GetCurrentTime();
									name = "";
									char ch[100];
									int val = CurTime.GetYear();
									itoa(val,ch,10);
									name+=ch;

									val = CurTime.GetMonth();
									itoa(val,ch,10);
									name+=ch;


									val = CurTime.GetDay();
									itoa(val,ch,10);
									name+=ch;

									val = CurTime.GetHour();
									itoa(val,ch,10);
									name+=ch;

									val = CurTime.GetMinute();
									itoa(val,ch,10);
									name+=ch;


									val = CurTime.GetSecond();
									itoa(val,ch,10);
									name+=ch;

									name+=".jpg";
									//name = fileNameNow.GetString() + name;
									name = "./savedImage/" + name;  

									sprintf(PicName, "./savedImage/%04d%02d%02d%02d%02d%02d.jpg", CurTime.GetYear(), CurTime.GetMonth(), CurTime.GetDay(), \
										CurTime.GetHour(), CurTime.GetMinute(), CurTime.GetSecond());

									//imwrite(name,tosave);
									imwrite(PicName, tosave);

									char sendBuf[200] = { 0 };
									strcpy_s(sendBuf, name.c_str());
									myUdp.SendUdp(sendBuf, strlen(sendBuf));
									//	delete[] sendBuf;


									//	imshow("tosave",tosave);
									//AfxMessageBox(name.c_str());
									//	waitKey(0);

									//基于给定的矩形设置图像的ROI（感兴趣区域，region of interesting）
									//cvSetImageROI(pFrame,roi);

									//	}
								}

							}

							if (ita->pt[MAX_STEP-1].x == 1)	 //画前一个路径到当前路径的线
								for(int i = 0;i<ita->step-1;i++)
									cvLine(pFrame,ita->pt[i],ita->pt[i+1],lineColor,1,8,0);

							else
								for(int i = 0;i<MAX_STEP-1;i++)
									cvLine(pFrame,ita->pt[(ita->step+i+1)%MAX_STEP],ita->pt[(ita->step+i+2)%MAX_STEP],lineColor,1,8,0);

							ita->flag = false;
							ita++;

						}

					}
					objDetRect.clear();		//把从上一帧检测到的物体清除,为下一帧检测做好准备

					cvResize(pFrame,Frame);
					drawArrow(Frame,pt1,pt2);
					//drawArrow(pFrame,pt1,pt2);
					cvRectangle(Frame,pt1_roi,pt2_roi,CV_RGB(0,0,255));

					if (flag_setingLane1)
						cvRectangle(Frame,pt3_roi,pt4_roi,CV_RGB(0,0,255));
					if (flag_setingLane2)
						cvRectangle(Frame, pt5_roi, pt6_roi, CV_RGB(0, 0, 255));
					if (flag_setingLane3)
						cvRectangle(Frame, pt7_roi, pt8_roi, CV_RGB(0, 0, 255));
				}


				cvReleaseImage(&pFrame);

			}// if(isUnusualing)

			//*********************************
			//
			//
			//*********************************

			//delete [] imagesData[out];
			int z = imageresize->width ;
			::PostMessage(hHwnd, SHOW_HIKIMG, (WPARAM)out, (LPARAM)out);   // 向hHwnd句柄PostMessage
			
			out = (out+1)%SIZE_OF_BUFFER;
			//if(!isRemoving)
			//	Sleep(25);
			//ReleaseMutex(g_hMutex);
			//ReleaseSemaphore(g_hEmptySemaphore,1,NULL);
		}//--->vc.isOpened()
	}
	fileName = "";
	cvReleaseImage(&oldimage);
	cvReleaseImage(&imageresize);


	CloseHandle(g_hMutex);
	CloseHandle(g_hEmptySemaphore);
	CloseHandle(g_hFullSemaphore);
	vc.release();
	return 0;
}