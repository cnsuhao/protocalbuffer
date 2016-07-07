#pragma once
#include"stdafx.h"
#include "cvaux.h"
#include <vector>
#include "hashmap.h"
#include<ctime>
#include    <fstream>
#include    <string>
#include	<opencv/highgui.h>
#include	<opencv/cvaux.h>
#include	<opencv/cxcore.h>
#include	"opencv2/core/core.hpp"
#include	"opencv2/imgproc/imgproc.hpp"
#include	"opencv2/video/background_segm.hpp"
#include	"opencv2/objdetect/objdetect.hpp"
#include	"opencv2/highgui/highgui.hpp"
#include    "log.h" // write logFile
#include    "Foreground_Model.h"
#include    "Precise_Detect.h"
#include	"HKDlgDlg.h"
#define SHOW_HIKIMG (WM_USER+102) 
#define WM_MYUPDATEDATA WM_USER+131 // 2015-07-07 by YaoJH
#define  SIZE_OF_BUFFER 200
#define  RATE 2 
//#define  MAX_CRUISE_POINT  8


using namespace cv;
using namespace std;
using namespace ATL;

typedef struct
{
	HWND hHwnd;
	int index;
	int procType;
	CString type;	//摄像头类型
	CString devicename; //摄像头名称
	LONG lPlayHandle;
	LONG lUserID;
	//CPoint p_origin,p_end;
	//CPoint p_roi_1,p_roi_2;
}threadParam;

typedef struct	//摄像头信息
{
	CString ip;
	CString type;
	CString username;
	CString deviceID;
	CString pwd;
	CString density_value;
	int xie_out_value;
	int xie_in_value;
	int chuizhi_in_value;
	int chuizhi_out_value;
	int alarm_area;
	int rfid;
	BOOL running;
	CString deviceName;
	int windowIndex;
	int currentFunction;	//1：不处理 2：去夜 3：人群检测 4：异常事件 5：斜视角 6：垂直视角 7:定位 8:定位_zigbee
	vector<CvPoint> location;
}CameraInfo;
//斜视角行人检测的结构体
typedef struct RectInfo
{
	int minX;
	int minY;
	int maxX;
	int maxY;
	int id;			//记录行人id
	int lost;		//1~5表示丢了几帧，-1表示空闲
	int depth;		//代表行人的深度
	int frameN;		//表示连续出现几帧
}RectInfo;	

typedef struct
{
	int m_port;
	int index;
}HikPort;

//巡航路径
typedef struct
{
	int iIndex;  //index
	int nPoint;  //total points
	CTime time;	 
	int procType;
	CString type;	//摄像头类型
	CString devicename; //摄像头名称
	LONG lPlayHandle;
	LONG lUserID;
	//CPoint p_origin,p_end;
	//CPoint p_roi_1,p_roi_2;
}CruiseParam;

class  CarPlateInfo
{
public:
	Mat  matOrigin ;
	vector<string> Carplates ;
	vector<string> Platecolor ;
	vector<Rect>  Positions ;
	vector<Rect>  rectRoi ;
	vector<Rect>  rectRoiCopy ;
	int iPresentP ;
//	string strSeq ;

}; 

class  CruiseState
{

public:
	CruiseState();
	~CruiseState();

	bool bRunState ; 
	int iTurnToWhere ;
	int iPresentPoint ;
	int iTurnToPointNum ; 
	int iParkSeq[MAX_CRUISE_POINT] ;
	vector<CvPoint> *vecPtRoi[MAX_CRUISE_POINT] ;
	Vehicle vecCar[MAX_CRUISE_POINT] ; 
	Vehicle vecCar_Static[MAX_CRUISE_POINT] ; 
} ;

class   VehicleViolation
{
public:
	VehicleViolation();
	~VehicleViolation();

char strSetSeq[8] ;  //车辆序号
Vehicle  vehSuspectCarStatic[4] ; //四个监测区域里面的车辆
Vehicle  vehSuspectCar ;
IplImage* oldimage ;  //草摄像头读取的原图像
	IplImage * ImageRoi ;//车辆特写图像
CascadeClassifier cascade_car; //车辆识别的分类器
LONG lPlayHandle ;
  vector<string> strSend ;  //发短信内容       ;
vector<CvRect> objDetRect;  //每帧检测的车辆

	NET_DVR_POINT_FRAME pStruPointFrame; //球机拉近的边框
	CvMat* pFrameMat ;

		vector<Rect> rectRoi ;  //车辆匡位置
			int iRoiNum ;
			int iNumSeq ;
			int iSequence ;
			bool bTakePic ; //是否拍照了
bool InitParking(); //初始化违停
int MisParking();  //违停程序

};

class Paramtran
{
public:
	Paramtran();
	~Paramtran();
	threadParam *param;
	CHKDlgDlg *CHKdlg;
};


extern CruiseState *WrongPark ;
extern  Vehicle vecCar_Temp[MAX_CRUISE_POINT] ;
extern char PicName[256];

extern CString  m_strCheck ;
extern CString  m_strGetFrame  ;
extern CString  m_strPlatePos  ;
extern CString  m_strPhotoDeal ;

extern  int  iCaptureStartTime ;                
extern 	int  iPNum  ; 
extern  int  iSequence ;
extern bool  bZoomed_Static ;
extern bool  bZoomed_Dynamic ;
extern bool  bSetCruise ;
extern bool  bShowInfo ;           ;

extern bool g_continue; //控制程序结束
extern bool g_bPreset; //判断预置点
extern bool g_bCruise; //球机巡航标志
extern IplImage* images[SIZE_OF_BUFFER];//缓冲区是个循环队列
extern IplImage* imageresize ;  
extern unsigned short in; //产品进缓冲区时的缓冲区下标
extern unsigned short out; //产品出缓冲区时的缓冲区下标
extern HANDLE g_hMutex; //用于线程间的互斥
extern HANDLE g_hFullSemaphore; //当缓冲区满时迫使生产者等待
extern HANDLE g_hEmptySemaphore; //当缓冲区空时迫使消费者等待

//extern HANDLE hThread[];
extern CWinThread* m_pThread;
extern DWORD WINAPI  CarPlateAnalysis(void* aaa) ;
extern UINT  ShowHikImg(Paramtran *paramdeal);
extern UINT  ProcessFile(LPVOID lpParameter);

extern void stopThread();

extern void init();

extern bool YV12_To_IplImage(IplImage* pImage,unsigned char* pYV12, int width, int height);
extern bool YV12_To_BGR24(unsigned char* pYV12, unsigned char* pRGB24,int width, int height);
extern int detectFace(Mat frame);
extern void log(char* filename,char* content);
extern void DeleteCar(  CruiseState *WrongPark ) ;

//斜视角使用的定义
#define WIN_WIDTH		640
#define WIN_HEIGHT		480
#define WIN_WIDTH2		320
#define WIN_HEIGHT2		240
#define WIN_WIDTH4		160
#define WIN_HEIGHT4		120

extern int hik_width_org;
extern int hik_height_org;

#define MAX_DEPTH		4000
#define MIN_DEPTH		800
#define WAIT_KEY		30
#define HOG_WIDTH	32
#define HOG_HEIGHT	32

extern char * imagesData[SIZE_OF_BUFFER];

extern bool isRemoving ;
extern bool bChangePoint ;
extern int iParkingDetect ;
extern int iPresetPNum ;
extern Rect ptRoi ;

extern vector<CvPoint> vecMousePoint ;

extern  vector<int>  vecPresetPoints ;

extern CarInfo InfoWrite ;
extern string fileName;
extern string strMessagePlate ;


extern void initUnusual();

extern bool isUnusualing;

extern bool flag_seting;
extern bool flag_setroi;
extern bool flag_setdirection;
extern bool flag_setingLane1; //by Yaojh
extern bool flag_setingLane2; //by Yaojh
extern bool flag_setingLane3; //by Yaojh
//extern CascadeClassifier  cascade;//harr 分类器
extern CvPoint pt1_dir;
extern CvPoint pt2_dir;
extern CvPoint pt1_roi;
extern CvPoint pt2_roi;
//for more lanes, By Yaojh,2015-0805
extern CvPoint pt3_roi;
extern CvPoint pt4_roi;
extern CvPoint pt5_roi;
extern CvPoint pt6_roi;

extern CvPoint pt7_roi;
extern CvPoint pt8_roi;

bool pedestrian(Mat image);

//MARK 0617
extern int SCREEN_WIDTH ;
extern int SCREEN_HEIGHT ;
//MARK 0624
extern int SCREEN_LEFTTOP_X;
extern int SCREEN_LEFTTOP_Y;
extern vector<string> AllFilePath;
extern string playRecord;
extern string AllplayRecord;

// 当前测试放文件名
extern CString fileNameNow;
extern std::string fileNameNows;
// 测试帧数
extern CString frameNumber;
extern std::string frameNumbers;

extern bool bBiggerOrNot ;
extern bool bBiggerOrNotShow ;
extern bool bWaitForDeal ; 

extern bool bSynCronize ;

extern int  FrameIn  ;
extern int  FrameOut  ;
extern int  RealIn ;
extern int  RealOut ;

extern int  FrameShowIn  ;
extern int  FrameShowOut ;
extern critical_section templock; 
extern critical_section lockThread; 
extern critical_section lockImage; 


extern bool bResetBG ;


