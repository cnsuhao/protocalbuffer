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
	CString type;	//����ͷ����
	CString devicename; //����ͷ����
	LONG lPlayHandle;
	LONG lUserID;
	//CPoint p_origin,p_end;
	//CPoint p_roi_1,p_roi_2;
}threadParam;

typedef struct	//����ͷ��Ϣ
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
	int currentFunction;	//1�������� 2��ȥҹ 3����Ⱥ��� 4���쳣�¼� 5��б�ӽ� 6����ֱ�ӽ� 7:��λ 8:��λ_zigbee
	vector<CvPoint> location;
}CameraInfo;
//б�ӽ����˼��Ľṹ��
typedef struct RectInfo
{
	int minX;
	int minY;
	int maxX;
	int maxY;
	int id;			//��¼����id
	int lost;		//1~5��ʾ���˼�֡��-1��ʾ����
	int depth;		//�������˵����
	int frameN;		//��ʾ�������ּ�֡
}RectInfo;	

typedef struct
{
	int m_port;
	int index;
}HikPort;

//Ѳ��·��
typedef struct
{
	int iIndex;  //index
	int nPoint;  //total points
	CTime time;	 
	int procType;
	CString type;	//����ͷ����
	CString devicename; //����ͷ����
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

char strSetSeq[8] ;  //�������
Vehicle  vehSuspectCarStatic[4] ; //�ĸ������������ĳ���
Vehicle  vehSuspectCar ;
IplImage* oldimage ;  //������ͷ��ȡ��ԭͼ��
	IplImage * ImageRoi ;//������дͼ��
CascadeClassifier cascade_car; //����ʶ��ķ�����
LONG lPlayHandle ;
  vector<string> strSend ;  //����������       ;
vector<CvRect> objDetRect;  //ÿ֡���ĳ���

	NET_DVR_POINT_FRAME pStruPointFrame; //��������ı߿�
	CvMat* pFrameMat ;

		vector<Rect> rectRoi ;  //������λ��
			int iRoiNum ;
			int iNumSeq ;
			int iSequence ;
			bool bTakePic ; //�Ƿ�������
bool InitParking(); //��ʼ��Υͣ
int MisParking();  //Υͣ����

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

extern bool g_continue; //���Ƴ������
extern bool g_bPreset; //�ж�Ԥ�õ�
extern bool g_bCruise; //���Ѳ����־
extern IplImage* images[SIZE_OF_BUFFER];//�������Ǹ�ѭ������
extern IplImage* imageresize ;  
extern unsigned short in; //��Ʒ��������ʱ�Ļ������±�
extern unsigned short out; //��Ʒ��������ʱ�Ļ������±�
extern HANDLE g_hMutex; //�����̼߳�Ļ���
extern HANDLE g_hFullSemaphore; //����������ʱ��ʹ�����ߵȴ�
extern HANDLE g_hEmptySemaphore; //����������ʱ��ʹ�����ߵȴ�

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

//б�ӽ�ʹ�õĶ���
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
//extern CascadeClassifier  cascade;//harr ������
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

// ��ǰ���Է��ļ���
extern CString fileNameNow;
extern std::string fileNameNows;
// ����֡��
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


