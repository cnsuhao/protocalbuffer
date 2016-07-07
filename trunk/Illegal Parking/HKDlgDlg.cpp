// HKDlgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HKDlg.h"
#include "HKDlgDlg.h"
#include "RABDetection.h"
#include <cxcore.h>
#include <cv.h>
#include <highgui.h>
#include "opencv2/core/core.hpp"  
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "Thread.h"
#include "DlgPTZCruise.h"

#include "LogFile.h"
#include "MiniDump.h"
using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern "C" WINBASEAPI HWND WINAPI GetConsoleWindow ();
LONG lPort; //ȫ�ֵĲ��ſ�port��
LONG m_iPort;

//char  *imagesData[SIZE_OF_BUFFER] ={0};
char  *imagesDataShow[SIZE_OF_BUFFER] ={0};
bool bParkOpen = false ;
bool bSetRoi = false ;
bool  bSetCruise  = false; 
int icount = 0 ;

int  iLineNum11  ;

critical_section  CrtLsct    ;
//critical_section templock; 
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

/*�������ܣ�
����������ȡ��Ƶ��
*/
void CALLBACK DecCBFun(long nPort,char * pBuf,long nSize, FRAME_INFO * pFrameInfo, long nReserved1,long /*nReserved2*/) //YV12(YUV��һ��)-->IplImage
{
	//if(pFrameInfo->dwFrameNum>0)

		//TRACE("DecCBFun called! %d:%d\n",nReserved1,in[index]);
	if(bWaitForDeal == true )
	{
		Sleep(20) ;
		return ;
	}
		hik_width_org = pFrameInfo->nWidth;
		hik_height_org = pFrameInfo->nHeight;
		//char * imgData = NULL/*= new char[hik_width_org*hik_height_org*3/2]*/;
		char * imgData = new char[hik_width_org*hik_height_org*3/2];
		if (imgData == NULL)
		{
		 return ; //	AfxMessageBox("�ұ�������");
		}
		//	char * imgDatashow = new char[hik_width_org*hik_height_org*3/2];
		memcpy(imgData, pBuf, hik_width_org*hik_height_org*3/2);
		//memcpy(imgDatashow, pBuf, hik_width_org*hik_height_org*3/2);
	
		templock.lock();
		if (imagesData[FrameIn] != NULL)
		{
		delete	imagesData[FrameIn]  ;
			imagesData[FrameIn] = NULL  ;
		}
		imagesData[FrameIn] = imgData;
		//imagesDataShow[FrameShowIn] =  imgDatashow ;
		templock.unlock()  ;
		//templock.lock() ;
		//printf("����%d\n",FrameIn);
		//	printf("^^^^^^^^^^^^^^%d\n",FrameIn);
	
		//TRACE("��ͼ�񣡣�");

		templock.lock() ;
		RealIn ++ ;
		FrameIn =RealIn % SIZE_OF_BUFFER;
		//	FrameShowIn =(FrameShowIn+1)%SIZE_OF_BUFFER;
		if (RealOut>1000000 && FrameIn > FrameOut	)
		{
			RealOut =  RealOut % SIZE_OF_BUFFER ;
			RealIn = RealIn % SIZE_OF_BUFFER ;
		}
		templock.unlock() ;

	//	delete []   imgData ;

}
void CALLBACK g_RealDataCallBack_V30(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize,void* dwUser)
{
	HWND hWnd= GetConsoleWindow();

	switch (dwDataType)
	{
	case NET_DVR_SYSHEAD: //ϵͳͷ

		if (!PlayM4_GetPort(&lPort))  //��ȡ���ſ�δʹ�õ�ͨ����
		{
			break;
		}
		m_iPort = lPort; //��һ�λص�����ϵͳͷ������ȡ�Ĳ��ſ�port�Ÿ�ֵ��ȫ��port���´λص�����ʱ��ʹ�ô�port�Ų���
		if (dwBufSize > 0)
		{
			if (!PlayM4_SetStreamOpenMode(lPort, STREAME_REALTIME))  //����ʵʱ������ģʽ
			{
				break;
			}

			if (!PlayM4_OpenStream(lPort, pBuffer, dwBufSize, 1920*1920)) //�����ӿ�
			{
				break;
			}
			PlayM4_SetDecCallBack(lPort,DecCBFun);
			if (!PlayM4_Play(lPort, hWnd)) //���ſ�ʼ
			{
				break;
			}
		}
	case NET_DVR_STREAMDATA:   //��������
		if (dwBufSize > 0 && lPort != -1)
		{
		
			if (!PlayM4_InputData(lPort, pBuffer, dwBufSize))
			{
				break;
			}
		}
	}

}
/*
�������ܣ�
���Ѳ������

*/


DWORD WINAPI  CHKDlgDlg ::Cruise(void* aaa)
{
	CHKDlgDlg *p = (CHKDlgDlg*) aaa ;

	bool bInterrupt = false ;
	int iSeq = p->m_comboSeq.GetCurSel() ;  
	int  iTimeStart = 0  , iTimeEnd , i = 0;

	try{

	while(p->bCruise == true)
	{	
		if (arrCruiseInfo[iSeq][i].iStaytime< 0)
		{
			i = (i+1)%MAX_PRESET_V30 ;
			continue; 
		}
		//�������Ѳ����ԭ��ץ��
		if (arrCruiseInfo[iSeq][i].bTurned == false	)
		{			
			if (iParkingDetect == 1 && bParkOpen == true  )
			{
				iParkingDetect = 2 ;
			}

			p->Program_PTZCruise( p->m_lPlayHandle,  arrCruiseInfo[iSeq][i].iPresetNum  ) ;
			// Sleep(WAITTURNTIME) ;
			WrongPark->iPresentPoint = arrCruiseInfo[iSeq][i].iPresetNum ;
			arrCruiseInfo[iSeq][i].bTurned = true ;
			if (bParkOpen == true )
			{
				iParkingDetect = 1 ;
			}
			iTimeStart = GetTickCount() ; 
		}
		else{
			//�ﵽץ��
			if (WrongPark->iTurnToWhere == TURN_TO_WARNINGPOINT )
			{
				if (bParkOpen == true )
				{
				iParkingDetect = 2 ;	
				}
				
				p->Program_PTZCruise( p->m_lPlayHandle, WrongPark->iTurnToPointNum  ) ;
				iCaptureStartTime = GetTickCount() ;
				WrongPark->bRunState = STATE_IN_CAPTURE ;	
				WrongPark->iTurnToWhere = TURN_TO_NOWHERE ;		
				if (bParkOpen == true )
				{
				iParkingDetect = 1 ;
				}
			}
			
			if (WrongPark->iTurnToWhere == TURN_TO_CRUISEPOINT  )
			{
				if (bParkOpen == true )
				{
					iParkingDetect = 2 ;	
				}
					p->Program_PTZCruise( p->m_lPlayHandle, WrongPark->iPresentPoint  ) ;
					
					WrongPark->bRunState = STATE_NORMAL ;
					WrongPark->iTurnToWhere = TURN_TO_NOWHERE ;
					if (bParkOpen == true )
					{
					iParkingDetect = 1 ;
					}
			}

			if (WrongPark->bRunState == STATE_IN_CAPTURE )
			{
				continue; 
			}

			iTimeEnd = GetTickCount() ;		
			float fTimeGap = (iTimeEnd - iTimeStart + WAITTURNTIME) /1000 ;
		//�������ʱ��;�ֹץ��
			if (fTimeGap >(arrCruiseInfo[iSeq][i].iStaytime )&& bZoomed_Dynamic == false && bZoomed_Static == false  )
			{
				arrCruiseInfo[iSeq][i].bTurned = false ;

				i = (i+1)%MAX_PRESET_V30 ;
				if (bParkOpen == true )
				{
				iParkingDetect = 2 ;
				}
			}
			Sleep(200) ;	
		}
	}

	}
	catch(...){
		int x = 0 ;

	}

	return 0 ;
}

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHKDlgDlg dialog

CHKDlgDlg::CHKDlgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHKDlgDlg::IDD, pParent), m_bIsPlaying(FALSE), m_lPlayHandle(-1), m_bIsLogin(FALSE), m_pClientSocketThread(NULL)
{
	//{{AFX_DATA_INIT(CHKDlgDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_height = _T("");
	m_width = _T("");

	m_iCurChanIndex = 1;
}

void CHKDlgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHKDlgDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_IPADDRESS1, m_ip);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_height);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_width);
	DDX_Control(pDX, IDC_BUTTON_CameraUp, m_up);
	DDX_Control(pDX, IDC_BUTTON_CameraDown, m_down);
	DDX_Control(pDX, IDC_BUTTON3IDC_BUTTON_CameraLeft, m_left);
	DDX_Control(pDX, IDC_BUTTON_CameraRight, m_right);
	DDX_Control(pDX, IDC_BUTTON_ZoomIn, m_zoomin);
	DDX_Control(pDX, IDC_BUTTON_ZoomOut, m_zoomout);

	//DDX_Text(pDX, IDC_EDIT_Preset, m_preset);
	DDV_MinMaxInt(pDX, m_preset, 1, 255);
	//  DDX_Text(pDX, IDC_EDIT_PicAddr, m_picaddress);
	//  DDV_MaxChars(pDX, m_picaddress, 100);
	DDX_Text(pDX, IDC_EDIT_PicAddr, m_picaddress);


	//DDX_Text(pDX, IDC_EDIT_COORDINARY, m_offset);

	DDV_MaxChars(pDX, m_picaddress, 100);
	DDX_Control(pDX, IDC_LIST_Result, m_list);
	DDX_Text(pDX, IDC_FileName, fileNameNow);
	DDX_Text(pDX, IDC_FrameNum, frameNumber);
	DDX_Control(pDX, IDC_COMBO_PTZ_SPEED, m_comboPTZSpeed);
	DDX_Control(pDX, IDC_COMBO_SEQ, m_comboSeq);
	DDX_Control(pDX, IDC_COMBO_PRESET, m_comboPreset);

	//DDX_Text(pDX, IDC_CHECKBUG, strEditCheck);

	//	DDX_Control(pDX, IDC_EDIT1, strWarningMsg);
	//DDX_Text(pDX, IDC_EDIT1, strWarningMsg);
}

BEGIN_MESSAGE_MAP(CHKDlgDlg, CDialog)
	//{{AFX_MSG_MAP(CHKDlgDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(SHOW_HIKIMG, OnShowHikImgMessage) // ��Ϣӳ��
	ON_MESSAGE(SHOW_REPAINTIMG, showInitImage) // ��Ϣӳ��

	ON_MESSAGE(WM_MYUPDATEDATA, OnUpdateDateY) // ��Ϣӳ��  Yaojh
	ON_MESSAGE(WM_CFGDATA, OnCfgData) // ��Ϣӳ��  hb
	ON_BN_CLICKED(ID_LOGIN, OnLogin)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnButtonPlay)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_RMNIGHT, &CHKDlgDlg::OnBnClickedButtonRmnight)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CHKDlgDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CHKDlgDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_OUT, &CHKDlgDlg::OnBnClickedButtonOut)
	ON_WM_SIZE()
	ON_WM_SIZING()
//	ON_WM_LBUTTONDOWN()

ON_EN_CHANGE(IDC_EDIT_Preset, &CHKDlgDlg::OnEnChangeEditPreset)

ON_BN_CLICKED(IDC_BUTTON_SetPreset, &CHKDlgDlg::OnClickedButtonSetpreset)
ON_BN_CLICKED(IDC_BUTTON_GotoPreset, &CHKDlgDlg::OnClickedButtonGotopreset)
ON_BN_CLICKED(IDC_BUTTON_Capture, &CHKDlgDlg::OnClickedButtonCapture)

ON_BN_CLICKED(IDC_BUTTON_SetROI, &CHKDlgDlg::OnBnClickedButtonSetroi)
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_BN_CLICKED(IDC_BUTTON_PRESET_GOTO, &CHKDlgDlg::OnBnClickedButtonPresetGoto)
ON_BN_CLICKED(IDC_BUTTON_PRESET_SET, &CHKDlgDlg::OnBnClickedPresetSet)
ON_BN_CLICKED(IDC_BUTTON_PRESET_DEL, &CHKDlgDlg::OnBnClickedPresetDel)
ON_CBN_SELCHANGE(IDC_COMBO_PRESET, &CHKDlgDlg::OnCbnSelchangeComboPreset)
ON_BN_CLICKED(IDC_BUTTON_SEQ_SET, &CHKDlgDlg::OnBnClickedSeqSet)
ON_BN_CLICKED(IDC_BUTTON_SEQ_GOTO, &CHKDlgDlg::OnBnClickedButtonSeqGoto)
ON_BN_CLICKED(IDC_BUTTON_SEQ_GOTOR, &CHKDlgDlg::OnBnClickedButtonSeqGotor)
ON_BN_CLICKED(IDC_BUTTON_TRACK_STOP, &CHKDlgDlg::OnButtonTrackStop)
ON_BN_CLICKED(IDC_BtnFindVehicle, &CHKDlgDlg::OnBnClickedBtnfindvehicle)
//ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS1, &CHKDlgDlg::OnIpnFieldchangedIpaddress1)

ON_BN_CLICKED(IDC_BUTTON_ZoomOut, &CHKDlgDlg::OnBnClickedButtonZoomout)
ON_BN_CLICKED(IDC_BUTTON_CONFIRMROI, &CHKDlgDlg::OnBnClickedButtonConfirmroi)
//ON_EN_CHANGE(IDC_CHECKBUG, &CHKDlgDlg::OnEnChangeCheckbug)
ON_EN_CHANGE(IDC_EDIT_PicAddr, &CHKDlgDlg::OnEnChangeEditPicaddr)
ON_BN_CLICKED(IDC_BTN_BROWSER, &CHKDlgDlg::OnBnClickedBtnBrowser)
ON_NOTIFY(NM_DBLCLK, IDC_LIST_Result, &CHKDlgDlg::OnNMDblclkListResult)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CHKDlgDlg)
	EASYSIZE(IDC_IPADDRESS1,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_STATIC_IP,ES_KEEPSIZE,ES_BORDER,IDC_IPADDRESS1,ES_KEEPSIZE,0)

	EASYSIZE(IDC_EDIT_HEIGHT,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_STATIC_CHANG,ES_KEEPSIZE,ES_BORDER,IDC_EDIT_HEIGHT,ES_KEEPSIZE,0)
	EASYSIZE(IDC_EDIT_WIDTH,ES_KEEPSIZE,ES_BORDER,IDC_STATIC_CHANG,ES_KEEPSIZE,0)
	EASYSIZE(IDC_STATIC_WIDTH,ES_KEEPSIZE,ES_BORDER,IDC_STATIC_CHANG,ES_KEEPSIZE,0)
	//EASYSIZE(IDC_STATIC_SIZE,ES_KEEPSIZE,ES_BORDER,IDC_STATIC_CHANG,ES_KEEPSIZE,0)
	
	EASYSIZE(IDC_BUTTON_PLAY,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_OPEN,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_RMNIGHT,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_OUT,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_BUTTON_STOP,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)

	//EASYSIZE(IDCANCEL,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)

    EASYSIZE(IDC_STATIC_PLAY,ES_BORDER,ES_BORDER,IDC_STATIC_IP,ES_BORDER,0)

END_EASYSIZE_MAP
/////////////////////////////////////////////////////////////////////////////
// CHKDlgDlg message handlers


BOOL CHKDlgDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message==WM_LBUTTONDOWN )
	{
		//if (	GetDlgItem(IDC_PICTURE_MOUSE)->GetSafeHwnd()==pMsg->hwnd)
		//�ڴ˴��ݵ����λ�ڶԻ����е�����
		OnLeftButtonDown(MK_LBUTTON,pMsg->pt);
		 //PtInRect() ;
	}

	if (pMsg->message==WM_RBUTTONDOWN )
	{
		//if (	GetDlgItem(IDC_PICTURE_MOUSE)->GetSafeHwnd()==pMsg->hwnd)
		//�ڴ˴��ݵ����λ�ڶԻ����е�����
		 OnRightButtonDown(MK_RBUTTON,pMsg->pt);

	}
	
	//UpdateData(TRUE) ;
	if (!strMessagePlate.empty())
	{//"��AF0236|��ɫ|E:\\1\\1_0��̬16_22_50��������.jpg"
		//strWarningMsg = strMessagePlate.c_str();

		unsigned int loc = strMessagePlate.find('|');
		if (loc != string::npos)
		{
			string sub;
			int n = m_list.GetItemCount() ;
			char strn[4] = { 0 };
			sprintf_s(strn, "%d", n+1);
			m_list.InsertItem(n, strn);

			sub = strMessagePlate.substr(0, loc);
			strWarningMsg = sub.c_str();
			m_list.SetItemText(n, 1, sub.c_str());

			unsigned int loc1 = strMessagePlate.find('|', loc + 1);
			if (loc1 != string::npos)
			{
				sub = strMessagePlate.substr(loc + 1, loc1 - loc-1);
				m_list.SetItemText(n, 2, sub.c_str());
			}
			loc = strMessagePlate.rfind('|');
			if (loc != string::npos)
			{
				sub = strMessagePlate.substr(loc+1);
				m_list.SetItemText(n, 6, sub.c_str());
			}

			CTime timecurrent = CTime::GetCurrentTime();
			string strTime = timecurrent.Format("%Y-%m-%d_%H:%M:%S");
			m_list.SetItemText(n, 3, "Υͣ����");
			m_list.SetItemText(n, 4, strTime.c_str());
		}
		strMessagePlate.clear();
	}

	////m_strChekcBug = m_strCheck ;
	////m_GetFrame = m_strGetFrame  ;
	////m_PlatePos = m_strPlatePos  ;
	////m_PhotoDeal = m_strPhotoDeal ;
	//UpdateData(FALSE);
	
	//if(WM_MOUSEMOVE   ==   pMsg-> message) 
	//{
	//	if(pMsg-> hwnd   ==   GetDlgItem(IDC_PICTURE_MOUSE)-> GetSafeHwnd()) 
	//	{ 
	//		OnMouseMove(NULL,pMsg->pt);
	//	} 
	//}
	return CDialog::PreTranslateMessage(pMsg);
}

/*
	��ȡEXE����Ŀ¼
*/
CString GetExePath()
{
	char pathbuf[260];
	int  pathlen = ::GetModuleFileName(NULL,pathbuf,260);
	
	// �滻������
	while(TRUE)
	{
		if(pathbuf[pathlen--]=='\\')
			break;
	}
	pathbuf[++pathlen]=   0x0;
	CString  fname = pathbuf;
	return   fname;
}
/*
	�쳣����ģ��
*/
LONG WINAPI CrashReportEx(LPEXCEPTION_POINTERS ExceptionInfo)
{
	char	szFileName[MAX_PATH] = {0x00};
	UINT	nRet = 0;

	// ��������
	::GetModuleFileName(NULL, szFileName, MAX_PATH);
	nRet = WinExec(szFileName, SW_SHOW);

	// ����DUMP�ļ�
	Create_Dump(ExceptionInfo,1,1);
	return EXCEPTION_EXECUTE_HANDLER;
}
BOOL CHKDlgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	char pathbuf[260];
	int  pathlen = ::GetModuleFileName(NULL,pathbuf,260) ;
	memset(&m_struPicPreviewInfo, 0, sizeof(m_struPicPreviewInfo));

	m_up.tag	  = 0;
	m_down.tag	  = 1;
	m_left.tag    = 2;
	m_right.tag   = 3;
	m_zoomin.tag  = 4;
	m_zoomout.tag = 5;

	m_list.InsertColumn(0,"���",LVCFMT_CENTER,40);  
    m_list.InsertColumn(1,"���ƺ���",LVCFMT_CENTER,70);
	m_list.InsertColumn(2,"������ɫ",LVCFMT_CENTER,60);
	m_list.InsertColumn(3,"Υ������",LVCFMT_CENTER,70);
	m_list.InsertColumn(4,"Υ��ʱ��",LVCFMT_CENTER,140);
	m_list.InsertColumn(5,"Υ��ͼƬ����·��",LVCFMT_CENTER,250);
	m_list.InsertColumn(6, "Υͣ����ͼƬ·��", LVCFMT_CENTER, 210);
	m_list.InsertColumn(7, "���Ͷ���", LVCFMT_CENTER, 75);
    m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT |LVS_EX_GRIDLINES);

	//m_list.InsertItem(0,"1");  
	//m_list.SetItemText(0,1,"��A 12345");
	//m_list.SetItemText(0,2,"Υ��ͣ��");
	//m_list.SetItemText(0,3,"20150101-1200");
	flag_drawline = 0;
	flag_drawlane = 0;
	/*if(!cascade.load("data\\cascade_car.xml")){
		AfxMessageBox("error loading cascade!");
	}*/
//	NET_DVR_SetPicViewDataCallBack(0,PicDataCallback,NULL);
	welcomeImage = cvLoadImage("res\\bg.jpg");
	char target[32] ;
	char Keyword[32] ="CamaraIP";
	IniRead(Keyword ,target )  ;

	DWORD   dwIP; 
	dwIP   =   inet_addr(target); 
	unsigned   char   *pIP   =   (unsigned   char*)&dwIP; 
	m_ip.SetAddress(*pIP,   *(pIP+1),   *(pIP+2),   *(pIP+3));

	CString strHeight, strWidth;
	strWidth.Format("%d",1920);
	strHeight.Format("%d",1080);
	m_height = strHeight;
	m_width = strWidth;
	m_preset = 1;

	char targetPath[MAX_PATH] = { 0 };
	IniRead("savepath", targetPath, MAX_PATH);
	m_picaddress = targetPath;
	

	UpdateData(FALSE);
	INIT_EASYSIZE;

	//mark0617
	CRect rect;
	GetDlgItem(IDC_STATIC_PLAY)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	SCREEN_LEFTTOP_X = rect.TopLeft().x;
	SCREEN_LEFTTOP_Y = rect.TopLeft().y;
	SCREEN_HEIGHT = rect.bottom - SCREEN_LEFTTOP_Y;
	SCREEN_WIDTH = rect.right - SCREEN_LEFTTOP_X;

	// auto start
	HKEY hKey;
	CString strRegPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	if(RegOpenKeyEx(HKEY_CURRENT_USER, strRegPath, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		TCHAR szModule[_MAX_PATH];
		GetModuleFileName(NULL, szModule, _MAX_PATH);
		RegSetValueEx(hKey, "HKDlg", 0, REG_SZ, (const BYTE*)(LPCSTR)szModule, strlen(szModule));
		RegCloseKey(hKey);

	}

	GetDlgItem(IDC_BUTTON_SetROI)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CONFIRMROI)->EnableWindow(FALSE);

	char strPrest[4] = { 0 };
	char strPrestValue[3] = { 0 };
	for (int i = 0; i< (MAX_CRUISE_POINT-1); i++)/*MAX_PRESET_V30*/
	{
		sprintf_s(strPrest, "%d", i+1);
		IniRead(strPrest, strPrestValue, 2, "Preset");
		if (strcmp(strPrestValue,"1")==0)
		{
			m_struDeviceInfo.struChanInfo[m_iCurChanIndex].struDecodercfg.bySetPreset[i] = 1;
		}
		else
		{
			m_struDeviceInfo.struChanInfo[m_iCurChanIndex].struDecodercfg.bySetPreset[i] = 0;
		}
	}

	//��̨�ٶ�
	m_comboPTZSpeed.SetCurSel(5);
	


	SetUnhandledExceptionFilter(CrashReportEx);
	HMODULE	hKernel32;

	// Try to get MiniDumpWriteDump() address.
	hDbgHelp = LoadLibrary("DBGHELP.DLL");
	MiniDumpWriteDump_ = (MINIDUMP_WRITE_DUMP)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
	//	d("hDbgHelp=%X, MiniDumpWriteDump_=%X", hDbgHelp, MiniDumpWriteDump_);

	// Try to get Tool Help library functions.
	hKernel32 = GetModuleHandle("KERNEL32");
	CreateToolhelp32Snapshot_ = (CREATE_TOOL_HELP32_SNAPSHOT)GetProcAddress(hKernel32, "CreateToolhelp32Snapshot");
	Module32First_ = (MODULE32_FIRST)GetProcAddress(hKernel32, "Module32First");
	Module32Next_ = (MODULE32_NEST)GetProcAddress(hKernel32, "Module32Next");

	//��ʼ�����ݿ�����by hb at2016.03.17
	IniRead("User", dbConfig.DbUser, 20, "Db");
	IniRead("Pwd", dbConfig.DbPwd, 20, "Db");
	IniRead("connect", dbConfig.DbConnect, MAX_PATH, "Db");

	//if( m_ado.Connect("Driver=MySQL ODBC 5.3 ANSI Driver;SERVER=192.168.1.76;UID=root;PWD=tce;DATABASE=test;PORT=3306;COLUMN_SIZE_S32=1", "root", "tce")!=TRUE )
	if (m_ado.Connect(dbConfig.DbConnect, dbConfig.DbUser, dbConfig.DbPwd) != TRUE)
	{
		MessageBox("�������ݿ����", "db����ʧ��", MB_ICONWARNING);
	}

// 	char striD[3] = { 0 };
// 	for (int i = 0; i < 1; i++)
// 	{
// 		sprintf_s(striD, "%d", i + 1);
// 		m_list.InsertItem(i, striD);
// 		m_list.SetItemText(i, 1, "��123456");
// 		m_list.SetItemText(i, 2, "��ɫ");
// 		m_list.SetItemText(i, 3, "Υ��ͣ��");
// 		m_list.SetItemText(i, 4, "2016-03-18 10:48:37");
// 		m_list.SetItemText(i, 5, "D:\\1\\2_2��AF0236_��ɫ_���.jpg");
// 		m_list.SetItemText(i, 6, "D:\\1\\2_2��̬09_34_26��������.jpg");
// 		m_list.SetItemText(i, 7, "δ");
// 	}

// 	m_list.InsertItem(1, "2");
// 	m_list.SetItemText(1, 1, "��123456");
// 	m_list.SetItemText(1, 4, "2016-03-18 10:48:37");
// 	m_list.SetItemText(1, 5, "C:\\Users\\tce\\Pictures\\QQͼƬ20160307105256.jpg");
// 	m_list.SetItemText(1, 6, "C:\\Users\\tce\\Pictures\\QQͼƬ20160308103045.jpg");
// 	m_list.SetItemText(1, 7, "δ");


// 	string strMessagePlate = "��AF0236|��ɫ|D:\\1\\2_2��̬09_34_26��������.jpg";
// 
// 	unsigned int loc = strMessagePlate.find('|');
// 	if (loc != string::npos)
// 	{
// 		string sub;
// 		int n = m_list.GetItemCount();
// 		char strn[4] = { 0 };
// 		sprintf_s(strn, "%d", n+1);
// 		m_list.InsertItem(n, strn);
// 
// 		sub = strMessagePlate.substr(0, loc);
// 		m_list.SetItemText(n, 1, sub.c_str());
// 
// 		unsigned int loc1 = strMessagePlate.find('|', loc + 1);
// 		if (loc1 != string::npos)
// 		{
// 			sub = strMessagePlate.substr(loc + 1, loc1 - loc - 1);
// 			m_list.SetItemText(n, 2, sub.c_str());
// 		}
// 		loc = strMessagePlate.rfind('|');
// 		if (loc != string::npos)
// 		{
// 			sub = strMessagePlate.substr(loc + 1);
// 			m_list.SetItemText(n, 6, sub.c_str());
// 		}
// 	}
	if (m_pClientSocketThread == NULL)
	{
		char CfgFromNet[3] = { 0 };
		IniRead("CfgFromNet", CfgFromNet, 3, "common");
		if (atoi(CfgFromNet)==1)
		{
			m_pClientSocketThread = new ClientSocketThread(this);
			m_pClientSocketThread->Start();
		}
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHKDlgDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHKDlgDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
		PostMessage(SHOW_REPAINTIMG);
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHKDlgDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CHKDlgDlg::OnLogin() 
{
	if(m_pThread!=NULL)
	{
		OnBnClickedButtonStop();
	}
	// TODO: Add your control notification handler code here
	if(!m_bIsLogin)
	{
		if(!DoLogin())
			return;
		DoGetDeviceResoureCfg();  //��ȡ�豸��Դ��Ϣ	
		//CreateDeviceTree();        //����ͨ����
		//GetDecoderCfg();                           //��ȡ��̨��������Ϣ
		//InitDecoderReferCtrl();         //��ʼ����������ؿؼ�      
		//GetDlgItem(IDC_BUTTON_LOGIN)->SetWindowText("Logout");
		m_bIsLogin = TRUE;
	}
}
BOOL CHKDlgDlg::Login()
{
	if(m_pThread!=NULL)
	{
		OnBnClickedButtonStop();
	}
	// TODO: Add your control notification handler code here
	if(!m_bIsLogin)
	{
		if(!DoLogin())
			return false;
		DoGetDeviceResoureCfg();  //��ȡ�豸��Դ��Ϣ	
		//CreateDeviceTree();        //����ͨ����
		//GetDecoderCfg();                           //��ȡ��̨��������Ϣ
		//InitDecoderReferCtrl();         //��ʼ����������ؿؼ�      
		//GetDlgItem(IDC_BUTTON_LOGIN)->SetWindowText("Logout");
		m_bIsLogin = TRUE;
	}
	return true;
}
BOOL CHKDlgDlg::DoLogin()
{
	UpdateData(TRUE);
	unsigned   char   *pIP; 
	DWORD   dwIP; 
	m_ip.GetAddress(dwIP); 
	pIP   =   (unsigned   char*)&dwIP; 
	DeviceIp.Format( "%u.%u.%u.%u ",*(pIP+3),   *(pIP+2),   *(pIP+1),   *pIP);

	CString m_csUser ;
	CString m_csPWD  ;
	hik_width_org = _ttoi(m_width);
	hik_height_org = _ttoi(m_height);
	char target[32] ;
	char Keyword1[32] = "CamaraID" ;
	char Keyword2[32] = "CamaraPassport" ;
	char Keyword3[32] = "Port" ;
	IniRead(  Keyword1 , target) ;
	
	m_csUser = target ;
	
	IniRead(  Keyword2 , target) ;
	
	m_csPWD = target ;

		IniRead(  Keyword3 , target) ;

	UINT	m_nDevPort  = atoi(target) ;


	NET_DVR_DEVICEINFO_V30 DeviceInfoTmp;
	memset(&DeviceInfoTmp,0,sizeof(NET_DVR_DEVICEINFO_V30));
	
	m_lDecID = NET_DVR_Login_V30(DeviceIp.GetBuffer(DeviceIp.GetLength()),m_nDevPort, 
		m_csUser.GetBuffer(m_csUser.GetLength()),m_csPWD.GetBuffer(m_csPWD.GetLength()),&DeviceInfoTmp);
	if(m_lDecID == -1)
	{
		MessageBox("Login to Device failed!\n");
		return FALSE;
	}
    m_struDeviceInfo.lLoginID = m_lDecID;
	m_struDeviceInfo.iDeviceChanNum = DeviceInfoTmp.byChanNum;
    m_struDeviceInfo.iIPChanNum = DeviceInfoTmp.byIPChanNum;
    m_struDeviceInfo.iStartChan  = DeviceInfoTmp.byStartChan;

	return TRUE;
}

/*************************************************
������:    	DoGetDeviceResoureCfg
��������:	��ȡ�豸��ͨ����Դ
�������:   
�������:   			
����ֵ:		
**************************************************/
void CHKDlgDlg::DoGetDeviceResoureCfg()
{
	NET_DVR_IPPARACFG IpAccessCfg;
	memset(&IpAccessCfg,0,sizeof(IpAccessCfg));	
	DWORD  dwReturned;

	m_struDeviceInfo.bIPRet = NET_DVR_GetDVRConfig(m_struDeviceInfo.lLoginID,NET_DVR_GET_IPPARACFG,0,&IpAccessCfg,sizeof(NET_DVR_IPPARACFG),&dwReturned);

	int i;
    if(!m_struDeviceInfo.bIPRet)   //��֧��ip����,9000�����豸��֧�ֽ���ģ��ͨ��
	{
		for(i=0; i<MAX_ANALOG_CHANNUM; i++)
		{
			if (i < m_struDeviceInfo.iDeviceChanNum)
			{
				sprintf(m_struDeviceInfo.struChanInfo[i].chChanName,"camera%d",i+m_struDeviceInfo.iStartChan);
				m_struDeviceInfo.struChanInfo[i].iChanIndex=i+m_struDeviceInfo.iStartChan;  //ͨ����
				m_struDeviceInfo.struChanInfo[i].bEnable = TRUE;
				
			}
			else
			{
				m_struDeviceInfo.struChanInfo[i].iChanIndex = -1;
				m_struDeviceInfo.struChanInfo[i].bEnable = FALSE;
				sprintf(m_struDeviceInfo.struChanInfo[i].chChanName, "");	
			}
		}
	}
	else        //֧��IP���룬9000�豸
	{
		for(i=0; i<MAX_ANALOG_CHANNUM; i++)  //ģ��ͨ��
		{
			if (i < m_struDeviceInfo.iDeviceChanNum)
			{
				sprintf(m_struDeviceInfo.struChanInfo[i].chChanName,"camera%d",i+m_struDeviceInfo.iStartChan);
				m_struDeviceInfo.struChanInfo[i].iChanIndex=i+m_struDeviceInfo.iStartChan;
				if(IpAccessCfg.byAnalogChanEnable[i])
				{
					m_struDeviceInfo.struChanInfo[i].bEnable = TRUE;
				}
				else
				{
					m_struDeviceInfo.struChanInfo[i].bEnable = FALSE;
				}
				
			}
			else//clear the state of other channel
			{
				m_struDeviceInfo.struChanInfo[i].iChanIndex = -1;
				m_struDeviceInfo.struChanInfo[i].bEnable = FALSE;
				sprintf(m_struDeviceInfo.struChanInfo[i].chChanName, "");	
			}		
		}

		//����ͨ��
		for(i=0; i<MAX_IP_CHANNEL; i++)
		{
			if(IpAccessCfg.struIPChanInfo[i].byEnable)  //ipͨ������
			{
				m_struDeviceInfo.struChanInfo[i+MAX_ANALOG_CHANNUM].bEnable = TRUE;
                m_struDeviceInfo.struChanInfo[i+MAX_ANALOG_CHANNUM].iChanIndex = IpAccessCfg.struIPChanInfo[i].byChannel;
				sprintf(m_struDeviceInfo.struChanInfo[i+MAX_ANALOG_CHANNUM].chChanName,"IP Camera %d",IpAccessCfg.struIPChanInfo[i].byChannel);

			}
			else
			{
               m_struDeviceInfo.struChanInfo[i+MAX_ANALOG_CHANNUM].bEnable = FALSE;
			    m_struDeviceInfo.struChanInfo[i+MAX_ANALOG_CHANNUM].iChanIndex = -1;
			}
		}
		
		
	}

}

void CHKDlgDlg::OnButtonPlay() 
{
	// TODO: Add your control notification handler code here
	CString sButtonCaption;
	GetDlgItemText(IDC_BUTTON_PLAY, sButtonCaption);
	if (sButtonCaption.Compare("��������ͷ")!=0)
	{
		//OnBnClickedButtonStop();
		ShowImage(welcomeImage, IDC_STATIC_PLAY);
		StopPlay();
		m_bIsLogin = FALSE;
		m_bIsPlaying = FALSE;
		GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText("��������ͷ");
		return;
	}
	if(!Login())
		return;

	GetDlgItem(IDC_BUTTON_SetROI)->EnableWindow(TRUE);


	//����Ԥ�õ�
	int i;
	CString tmp;
	for (i = 0; i<(MAX_CRUISE_POINT-1); i++)/*MAX_PRESET_V30 Ԥ�õ�8�ȽϺ���*/
	{
		tmp.Format("%d", i + 1);     //i+1
		m_comboPreset.AddString(tmp);
	}
	m_comboPreset.SetCurSel(0);
	GetDlgItem(IDC_BUTTON_PRESET_GOTO)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PRESET_DEL)->EnableWindow(FALSE);

	//Ѳ���켣
	for (i = 0; i<MAX_CRUISE_SEQ; i++)
	{
		tmp.Format("%d", i + 1);     //i+1
		m_comboSeq.AddString(tmp);
	}
	m_comboSeq.SetCurSel(0);

	//int m_iCurChanIndex = 2;
	if (m_iCurChanIndex == -1)
	{
		MessageBox("ѡ��һ��ͨ��");
		return;
	}
	if(!m_bIsPlaying)
	{
         if(StartPlay(m_iCurChanIndex))
		 {
			 m_bIsPlaying = TRUE;
			 GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText("�Ͽ�����");
		 }
	}
	//else
	//{
	//	StopPlay();
	//	m_bIsPlaying = FALSE;
	//	m_bIsLogin = false;
	//	GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText("��������ͷ");
	//}
}

/*************************************************
������:    	StartPlay
��������:	��ʼһ·����
�������:   ChanIndex-ͨ����
�������:   			
����ֵ:		
**************************************************/
bool CHKDlgDlg::StartPlay(int iChanIndex)
{
	int m_iCurChanIndex = 0;
	NET_DVR_PREVIEWINFO ClientInfo;
	ClientInfo.hPlayWnd     = NULL;//GetDlgItem(IDC_STATIC_PLAY)->m_hWnd;
	ClientInfo.lChannel     = m_struDeviceInfo.struChanInfo[m_iCurChanIndex].iChanIndex;
	ClientInfo.dwStreamType    = 0;
    ClientInfo.dwLinkMode = NULL;
	m_lPlayHandle = NET_DVR_RealPlay_V40(m_struDeviceInfo.lLoginID,&ClientInfo,g_RealDataCallBack_V30,NULL);
	TRACE("*********%d",m_lPlayHandle);
	if(-1 == m_lPlayHandle)
	{
		DWORD err=NET_DVR_GetLastError();
		CString m_csErr;
        m_csErr.Format("���ų����������%d",err);
		MessageBox(m_csErr);
		return false;
	}

	m_bIsPlaying = TRUE;
	//GetDlgItem(IDC_BUTTON_PLAY)->SetWindowText("stop");

	init();
	Paramtran *paramthread = new Paramtran ;
	paramthread->param->hHwnd = GetSafeHwnd();
	paramthread->param->lUserID = m_struDeviceInfo.lLoginID;
	paramthread->param->lPlayHandle = this->m_lPlayHandle;
	paramthread->CHKdlg = this ;

	//pam->lUserID = m_struDeviceInfo.lLoginID;
	/*pam->p_origin = Porigin;
	pam->p_end = Pend;
	pam->p_roi_1 = p_roi_1;
	pam->p_roi_2 = p_roi_2;*/
	m_pThread = AfxBeginThread((AFX_THREADPROC)ShowHikImg, paramthread, 0, 0, NULL);
	m_pThread->m_bAutoDelete = FALSE;

	m_up.handle = this->m_lPlayHandle;
	m_down.handle = this->m_lPlayHandle;
	m_left.handle = this->m_lPlayHandle;
	m_right.handle = this->m_lPlayHandle;
	m_zoomin.handle = this->m_lPlayHandle;
	m_zoomout.handle = this->m_lPlayHandle;
	return true;
//	NET_DVR_StartPicPreview(m_iCurChanIndex,&ClientInfo,PicDataCallback,NULL);
}

/*************************************************
������:    	StopPlay
��������:	ֹͣ����
�������:   
�������:   			
����ֵ:		
**************************************************/
void CHKDlgDlg::StopPlay()
{
	if(m_lPlayHandle != -1)
	{
		NET_DVR_StopRealPlay(m_lPlayHandle);
		m_lPlayHandle=-1;
		m_bIsPlaying = FALSE;
		GetDlgItem(IDC_STATIC_PLAY)->Invalidate();
		stopThread();
	}
}


LRESULT CHKDlgDlg::OnShowHikImgMessage(WPARAM wParam, LPARAM lParam)
{
	int index = (int)wParam;
	int outindex = (int)lParam;
	if(g_continue)
	{
		//TRACE("index in DLG:%d\n",index);
		ShowImage(imageresize, IDC_STATIC_PLAY);
		//cvReleaseImage(&images[out]);	//�˴��ǵ��ͷţ����������ڴ�й©
		
	//	UpdateData(FALSE);
	}
	return 0;
}

LRESULT CHKDlgDlg::OnUpdateDateY(WPARAM wParam, LPARAM lParam) //updatedata
{
	UpdateData(FALSE);
	return 0;
}
LRESULT CHKDlgDlg::OnCfgData(WPARAM wParam, LPARAM lParam) //fanfa
{
	Factor *ff = (Factor*)wParam;
	MessageBox(ff->strCO, ff->strO3);
	OnButtonPlay();
	CvPoint pROI1, pROI2;
	pROI1.x = 253;
	pROI1.y = 82;
	pROI2.x = 737;
	pROI2.y = 478;
	WrongPark->vecPtRoi[iPNum]->push_back(pROI1);
	WrongPark->vecPtRoi[iPNum]->push_back(pROI2);
	OnBnClickedButtonRmnight();
	return 0;
}
//��opencv��IplImage������ʾ��ͼƬ�ؼ���
void CHKDlgDlg::ShowImage(IplImage* img, UINT ID)
{
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage* cimg = new CvvImage();
	lockImage.lock() ;
	cimg->CopyOf(img);
	lockImage.unlock() ;
	cimg->DrawToHDC(hDC, &rect);

	delete cimg;
	ReleaseDC(pDC);
	::DeleteDC(hDC);
}


//Υͣץ��
void CHKDlgDlg::OnBnClickedButtonRmnight() //Υͣץ��
{
	// TODO: Add your control notification handler code here
	try{
 if (bParkOpen == false )
 {
	iParkingDetect = 1 ;
	
	if (m_lPlayHandle >= 0)
	{
		if (!NET_DVR_PTZPreset(m_lPlayHandle, SET_PRESET, 1))
		{
//			MessageBox("����Ԥ�õ�ʧ��");
	//		return;
		}
	}
	else
	{
		if (!NET_DVR_PTZPreset_Other(m_struDeviceInfo.lLoginID, m_struDeviceInfo.struChanInfo[m_iCurChanIndex].iChanIndex, \
			SET_PRESET, 10))
		{
		//	MessageBox("����Ԥ�õ�ʧ��");
		//	return;
		}

	}



	bParkOpen = true ;
 }
 else{
   iParkingDetect = 0 ; 
   bParkOpen = false ;
	}

	if(iParkingDetect > 0)
	{
		GetDlgItem(IDC_BUTTON_RMNIGHT)->SetWindowTextA("ֹͣ���");
		bZoomed_Static = 0 ;
		bZoomed_Dynamic = 0 ;
	}
	if(iParkingDetect == 0)
	{
		GetDlgItem(IDC_BUTTON_RMNIGHT)->SetWindowTextA("Υͣץ��");
	}
	}
	catch(...){
		int x = 0 ;
	}


}

vector<string> filePath;
void GetFilePath(string filepath, vector<string> &AllFilePath){
	//mark 20150624
	Directory dir;
	string extenl = "*";//"*"  
	bool addPath = true;

	AllFilePath.clear();
	filePath.clear();
	filePath = dir.GetListFiles(filepath, extenl, addPath);
	
	vector<string>::iterator vit = filePath.begin();
	for (; vit != filePath.end(); vit++){
		string temp = *vit;
		if (temp.size() < 4)continue;
		string last4 = temp.substr(temp.size() - 4);
		if (last4 == ".mp4" || last4 == ".avi" || last4 == ".mpg")
		{
			AllFilePath.push_back(temp);
			
		}		
	}
	
}

// open file
void CHKDlgDlg::OnBnClickedButtonOpen()
{
	if(m_pThread!=NULL)
	{
		OnBnClickedButtonStop();
	}
	if(m_bIsPlaying)
	{
		OnButtonPlay();
	}
//	 TODO: Add your control notification handler code here
	char szFileFilter[]= 
		"AVI File(*.avi)|*.avi|"
		"Mp3 File(*.mp3)|*.mp4|"
		"All File(*.*)|*.*||";
	CFileDialog fileDialog( TRUE,NULL,NULL,NULL,szFileFilter,this);
	if (fileDialog.DoModal() == IDOK)
	{
		fileName=fileDialog.GetPathName();
		fileNameNow = fileName.c_str();  // Yaojh
		//fileName.MakeUpper();
		//m_player.put_URL(fileName);
	}
	if(!fileName.empty())
	{
		string::size_type len = fileName.find_last_of('\\');
		string homepath = fileName.substr(0, len);
		GetFilePath(homepath, AllFilePath);
		init();
		threadParam* pam = new threadParam();
		pam->hHwnd = GetSafeHwnd();//�õ�һ�����ڶ���
		CWinThread * p = AfxBeginThread(ProcessFile,(LPVOID)pam,0,0,NULL);
		//CWinThread * p = AfxBeginThread(ShowHikImg,(LPVOID)pam,0,0,NULL);
		p->m_bAutoDelete = FALSE;
		m_pThread = p;
	}
}

void CHKDlgDlg::OnBnClickedButtonStop()
{
	// TODO: Add your control notification handler code here
	stopThread();
	ShowImage(welcomeImage,IDC_STATIC_PLAY);
	if(isRemoving)
		OnBnClickedButtonRmnight();
	if(isUnusualing)
		OnBnClickedButtonOut();
}

LRESULT CHKDlgDlg::showInitImage(WPARAM wParam, LPARAM lParam)
{
	ShowImage(welcomeImage,IDC_STATIC_PLAY);
	return 0;
}

//����ץ��
void CHKDlgDlg::OnBnClickedButtonOut()
{
	// TODO: Add your control notification handler code here
	/*initRABDetection();
	initUnusual();*/
	isUnusualing = !isUnusualing;
	if(isUnusualing)
	{
		GetDlgItem(IDC_BUTTON_OUT)->SetWindowTextA("ֹͣ���");
		flag_seting = false;
		flag_setroi=false;
		flag_setdirection=false;

		flag_setingLane1 = false;
		flag_setingLane2 = false;
		flag_setingLane3 = false;

		flag_drawline = 0;
		flag_drawlane = 0;
		pt1_dir=cvPoint(0,0);
		pt2_dir=cvPoint(0,0);
		pt1_roi=cvPoint(0,0);
		pt2_roi=cvPoint(0,0);

		pt3_roi = cvPoint(0, 0);
		pt4_roi = cvPoint(0, 0);
		pt5_roi = cvPoint(0, 0);
		pt6_roi = cvPoint(0, 0);
		pt7_roi = cvPoint(0, 0);
		pt8_roi = cvPoint(0, 0);

	}
	else
	{
		flag_drawline = 0;
		flag_drawlane = 0;
		flag_seting = false;
		flag_setroi=false;
		flag_setdirection=false;

		flag_setingLane1 = false;
		flag_setingLane2 = false;
		flag_setingLane3 = false;

		GetDlgItem(IDC_BUTTON_OUT)->SetWindowTextA("����ץ��");
	}
}


void CHKDlgDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
	// TODO: Add your message handler code here
}


void CHKDlgDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);
	EASYSIZE_MINSIZE(280,250,fwSide,pRect);
	// TODO: Add your message handler code here
}

void CHKDlgDlg::OnEnChangeEditPreset()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
}

void CHKDlgDlg::OnClickedButtonSetpreset()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	NET_DVR_PTZPreset(m_lPlayHandle, SET_PRESET, m_preset);
}


void CHKDlgDlg::OnClickedButtonGotopreset()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	NET_DVR_PTZPreset(m_lPlayHandle, GOTO_PRESET, m_preset);
}


void CHKDlgDlg::OnClickedButtonCapture()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������


	if(m_lPlayHandle == -1)
	{
        MessageBox("����ѡ��һ��ͨ������");
		return;
	}

	char PicName[256] = {0};

	CTime CurTime = CTime::GetCurrentTime();;
	sprintf(PicName,"D:\\%04d%02d%02d%02d%02d%02d.bmp",CurTime.GetYear(),CurTime.GetMonth(),CurTime.GetDay(), CurTime.GetHour(),CurTime.GetMinute(),CurTime.GetSecond());
    
	if(NET_DVR_CapturePicture(m_lPlayHandle,PicName))
	{
		MessageBox("ץͼ�ɹ�!");
	}
	else
	{
		TRACE("%d\n",NET_DVR_GetLastError());
		MessageBox("ץͼʧ��!");
	}
}


//��������
void CHKDlgDlg::OnBnClickedButtonSetroi()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	/*CClientDC dc(this);
    OnPrepareDC(&dc);

    dc.DPtoLP(&point);*/
	//if (iParkingDetect == 1 )
	//{
	//	AfxMessageBox("����Υͣ����ر�ʱ�������������");
	//	return ;
	//}


	GetDlgItem(IDC_BUTTON_SetROI)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_CONFIRMROI)->EnableWindow(TRUE);

		bSetRoi = true   ;
	
		if(flag_drawline == 0)
		flag_drawline = 1;

		//AfxMessageBox("flag_drawline = 1;");
	
}

void CHKDlgDlg::OnLeftButtonDown(UINT nFlags, CPoint point)
{
	CRect rect_ctr;
	CRect rect_ctr222;
	CvPoint ptTemp ;
	CvPoint ptTemp1 ;
	//��ȡPicture�ؼ������Ļ���Ͻǵ����꣬/�洢��rect_ctr��
	(this->GetDlgItem(IDC_STATIC_PLAY))->GetWindowRect(&rect_ctr);

  bool bPtInRect =  PtInRect(rect_ctr ,point )  ; 

  if (bPtInRect == true && bSetRoi == true && bSetCruise ==false )
  {	  
	//  ScreenToClient(rect_ctr222); 

	  if (  WrongPark->vecPtRoi[iPNum]->size()>7 )
	  {
		  MessageBox("һ��Ԥ��λ�ļ�������ܳ����ĸ�����") ;
			return ;
	  }


	  ptTemp1.x = point.x - rect_ctr.left  ;
	  ptTemp1.y = point.y - rect_ctr.top   ;
	  ptRoi.width =rect_ctr.right - rect_ctr.left  ;
	  ptRoi.height = rect_ctr.bottom - rect_ctr.top ;
	  ptTemp.x = imageresize->width *  ptTemp1.x / ptRoi.width ;
	  ptTemp.y = imageresize->height * ptTemp1.y / ptRoi.height ;
	  	CrtLsct.lock() ;
	  WrongPark->vecPtRoi[iPNum]->push_back(ptTemp) ;
	  	CrtLsct.unlock() ;
	//  vecMousePoint.push_back(ptTemp) ;
  }


	//��ȡPicture�ؼ���ԶԻ���ͻ������Ͻǵ�����
//	CDialog::OnLButtonDown(nFlags, point);
}

void CHKDlgDlg::OnRightButtonDown(UINT nFlags, CPoint point)
{
	if ( bSetRoi == true && bSetCruise ==false )
	{
		if (WrongPark->vecPtRoi[iPNum]->size() == 0 )
		{
			return ;
		}
		CrtLsct.lock() ;
		WrongPark->vecPtRoi[iPNum]->pop_back() ;
			CrtLsct.unlock() ;
	}
			
}

void CHKDlgDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	
	if(isUnusualing==1)
	{
		if(flag_drawline==1)
		{
			if (point.x>SCREEN_LEFTTOP_X&&point.y>SCREEN_LEFTTOP_Y && point.x<(SCREEN_LEFTTOP_X + SCREEN_WIDTH) && point.y<(SCREEN_LEFTTOP_Y + SCREEN_HEIGHT))
			{				
				pt1_dir = cvPoint(point.x - SCREEN_LEFTTOP_X, point.y - SCREEN_LEFTTOP_Y);
				flag_drawline=2;
			}else
			{
				flag_drawline=1;
				return;
			}
		}
		else if(flag_drawline==2)
		{
			if (point.x>SCREEN_LEFTTOP_X&&point.y>SCREEN_LEFTTOP_Y && point.x<(SCREEN_LEFTTOP_X + SCREEN_WIDTH) && point.y<(SCREEN_LEFTTOP_Y + SCREEN_HEIGHT))
			{
				pt2_dir = cvPoint(point.x - SCREEN_LEFTTOP_X, point.y - SCREEN_LEFTTOP_Y);
				//AfxMessageBox("2");
				flag_setdirection=true;
				flag_drawline=3;
			}else
			{
				flag_drawline=2;
				return;
			}
		}
		else if(flag_drawline==3)
		{
			if (point.x>SCREEN_LEFTTOP_X&&point.y>SCREEN_LEFTTOP_Y && point.x<(SCREEN_LEFTTOP_X + SCREEN_WIDTH) && point.y<(SCREEN_LEFTTOP_Y + SCREEN_HEIGHT))
			{
				pt1_roi = cvPoint(point.x - SCREEN_LEFTTOP_X, point.y - SCREEN_LEFTTOP_Y);
				flag_drawline=4;
				
			}else
			{
				flag_drawline=3;
				return;
			}
			
		}
		else if(flag_drawline==4)
		{
			if (point.x>SCREEN_LEFTTOP_X&&point.y>SCREEN_LEFTTOP_Y && point.x<(SCREEN_LEFTTOP_X + SCREEN_WIDTH) && point.y<(SCREEN_LEFTTOP_Y + SCREEN_HEIGHT))
			{
				pt2_roi = cvPoint(point.x - SCREEN_LEFTTOP_X, point.y - SCREEN_LEFTTOP_Y);
				flag_seting = true;
				flag_drawline=0;
				
			}
			else
			{
				flag_drawline = 4;
				return;
			}
		}
		else
		{
			;
		}

			//first lane
		if (flag_drawlane == 1)
		{
			if (point.x > SCREEN_LEFTTOP_X&&point.y > SCREEN_LEFTTOP_Y && point.x<(SCREEN_LEFTTOP_X + SCREEN_WIDTH) && point.y<(SCREEN_LEFTTOP_Y + SCREEN_HEIGHT))
			{
				pt3_roi = cvPoint(point.x - SCREEN_LEFTTOP_X, point.y - SCREEN_LEFTTOP_Y);
				flag_drawlane = 2;

			}
			else
			{
				flag_drawlane = 1;
				return;
			}
		}
		if (flag_drawlane == 2)
		{
			if (point.x>SCREEN_LEFTTOP_X&&point.y>SCREEN_LEFTTOP_Y && point.x<(SCREEN_LEFTTOP_X + SCREEN_WIDTH) && point.y<(SCREEN_LEFTTOP_Y + SCREEN_HEIGHT))
			{
				pt4_roi = cvPoint(point.x - SCREEN_LEFTTOP_X, point.y - SCREEN_LEFTTOP_Y);
				flag_drawlane = 3;
				flag_setingLane1=true;

			}
			else
			{
				flag_drawlane = 2;
				return;
			}
		}

			//second lane
		if (flag_drawlane == 3)
		{
			if (point.x>SCREEN_LEFTTOP_X&&point.y>SCREEN_LEFTTOP_Y && point.x<(SCREEN_LEFTTOP_X + SCREEN_WIDTH) && point.y<(SCREEN_LEFTTOP_Y + SCREEN_HEIGHT))
			{
				pt5_roi = cvPoint(point.x - SCREEN_LEFTTOP_X, point.y - SCREEN_LEFTTOP_Y);
				flag_drawlane = 4;

			}
			else
			{
				flag_drawlane = 3;
				return;
			}
		}
		if (flag_drawlane == 4)
		{
			if (point.x>SCREEN_LEFTTOP_X&&point.y>SCREEN_LEFTTOP_Y && point.x<(SCREEN_LEFTTOP_X + SCREEN_WIDTH) && point.y<(SCREEN_LEFTTOP_Y + SCREEN_HEIGHT))
			{
				pt6_roi = cvPoint(point.x - SCREEN_LEFTTOP_X, point.y - SCREEN_LEFTTOP_Y);
				//flag_drawline = 8;
				flag_drawlane = 3;
				flag_setingLane2=true;
				//flag_seting = true;
			}
			else
			{
				flag_drawlane = 4;
				return;
			}
			
		}

		//third lane
		if (flag_drawlane == 5)
		{
			if (point.x>SCREEN_LEFTTOP_X&&point.y>SCREEN_LEFTTOP_Y && point.x<(SCREEN_LEFTTOP_X + SCREEN_WIDTH) && point.y<(SCREEN_LEFTTOP_Y + SCREEN_HEIGHT))
			{
				pt7_roi = cvPoint(point.x - SCREEN_LEFTTOP_X, point.y - SCREEN_LEFTTOP_Y);
				flag_drawlane = 6;

			}
			else
			{
				flag_drawlane = 5;
				return;
			}
		}

		//cvPointPolygonTest(const CvArr* contour, CvPoint2D32f pt, int measure_dist);measure_dist  ���Ϊ�棬�򷵻ص�double�����ǵ��������������ľ��롣
		if (flag_drawlane == 6)
		{
			if (point.x>SCREEN_LEFTTOP_X&&point.y>SCREEN_LEFTTOP_Y && point.x<(SCREEN_LEFTTOP_X + SCREEN_WIDTH) && point.y<(SCREEN_LEFTTOP_Y + SCREEN_HEIGHT))
			{
				pt8_roi = cvPoint(point.x - SCREEN_LEFTTOP_X, point.y - SCREEN_LEFTTOP_Y);
				flag_drawlane = 0;
				flag_setingLane3=true;
				//flag_seting = true;
			}
			else
			{
				flag_drawlane = 6;
				return;
			}

		}

	}
	CDialog::OnLButtonUp(nFlags, point);
}

/*
float bbOverlap(const BoundingBox& box1,const BoundingBox& box2)
{
if (box1.x > box2.x+box2.width) { return 0.0; }
if (box1.y > box2.y+box2.height) { return 0.0; }
if (box1.x+box1.width < box2.x) { return 0.0; }
if (box1.y+box1.height < box2.y) { return 0.0; }
float colInt =  min(box1.x+box1.width,box2.x+box2.width) - max(box1.x, box2.x);
float rowInt =  min(box1.y+box1.height,box2.y+box2.height) - max(box1.y,box2.y);
float intersection = colInt * rowInt;
float area1 = box1.width*box1.height;
float area2 = box2.width*box2.height;
return intersection / (area1 + area2 - intersection);
}

��ƪ������Դ�� Linux������վ(www.linuxidc.com)  
*/


void CHKDlgDlg::OnBnClickedButtonPresetGoto()// goto
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int iPreset = m_comboPreset.GetCurSel() + 1;

	/*	if (vecPresetPoints.size() == 0 )
	{
	vecPresetPoints.push_back(iPreset) ;
	}
	else
	{
	bool bExist = false ;
	for ( int i = 0 ; i < vecPresetPoints.size(); i++ )
	{
	if (vecPresetPoints[i] == 	iPreset	)
	{
	bExist = true ;
	break; 
	}
	}
	if (bExist == false)
	{
	vecPresetPoints.push_back(iPreset) ;
	}
	}*/

	if (m_lPlayHandle >= 0)
	{
		if (!NET_DVR_PTZPreset(m_lPlayHandle, GOTO_PRESET, iPreset))
		{
			MessageBox("����Ԥ�õ�ʧ��");
			return;
		}
	}
	else
	{
		if (!NET_DVR_PTZPreset_Other(m_struDeviceInfo.lLoginID, m_struDeviceInfo.struChanInfo[m_iCurChanIndex].iChanIndex, \
			GOTO_PRESET, iPreset))
		{
			MessageBox("����Ԥ�õ�ʧ��");
			return;
		}
	}
}

void CHKDlgDlg::OnBnClickedPresetSet() //set
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int iPreset = m_comboPreset.GetCurSel() + 1; 

	if (vecPresetPoints.size() == 0 )
	{
		vecPresetPoints.push_back(iPreset) ;
	}
	else
	{
		bool bExist = false ;
		for ( int i = 0 ; i < vecPresetPoints.size(); i++ )
		{
			if (vecPresetPoints[i] == iPreset	)
			{
				bExist = true ;
				break; 
			}
		}
		if (bExist == false)
		{
			vecPresetPoints.push_back(iPreset) ;
		}
	}

	if (m_lPlayHandle >= 0)
	{
		if (!NET_DVR_PTZPreset(m_lPlayHandle, SET_PRESET, iPreset))
		{
			MessageBox("����Ԥ�õ�ʧ��");
			return;
		}
	}
	else
	{
		if (!NET_DVR_PTZPreset_Other(m_struDeviceInfo.lLoginID, m_struDeviceInfo.struChanInfo[m_iCurChanIndex].iChanIndex, \
			SET_PRESET, iPreset))
		{
			MessageBox("����Ԥ�õ�ʧ��");
			return;
		}
	}

	//��ӵ�Ԥ�õ���Ϣ
	m_struDeviceInfo.struChanInfo[m_iCurChanIndex].struDecodercfg.bySetPreset[iPreset - 1] = TRUE;
	//add Ԥ�õ㱣�������ļ��� by hb at 2016.04.08
	char strPrest[3] = { 0 };
	sprintf_s(strPrest, "%d", iPreset);
	IniWrite(strPrest, "1", "Preset");
	//���°�ť״̬
	OnCbnSelchangeComboPreset();
}


void CHKDlgDlg::OnBnClickedPresetDel()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int iPreset = m_comboPreset.GetCurSel() + 1;    //+1
	if (m_lPlayHandle >= 0)
	{
		if (!NET_DVR_PTZPreset(m_lPlayHandle, CLE_PRESET, iPreset))
		{
			MessageBox("ɾ��Ԥ�õ�ʧ��");
			return;
		}
	}
	else
	{
		if (!NET_DVR_PTZPreset_Other(m_struDeviceInfo.lLoginID, m_struDeviceInfo.struChanInfo[m_iCurChanIndex].iChanIndex, \
			CLE_PRESET, iPreset))
		{
			MessageBox("ɾ��Ԥ�õ�ʧ��");
			return;
		}

	}

	//��ӵ�Ԥ�õ���Ϣ
	m_struDeviceInfo.struChanInfo[m_iCurChanIndex].struDecodercfg.bySetPreset[iPreset - 1] = FALSE;
	//add Ԥ�õ㱣�������ļ��� by hb at 2016.04.08
	char strPrest[3] = { 0 };
	sprintf_s(strPrest, "%d", iPreset);
	IniWrite(strPrest, "0", "Preset");
	//���°�ť״̬
	OnCbnSelchangeComboPreset();
}

void CHKDlgDlg::OnCbnSelchangeComboPreset()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int iIndex = m_comboPreset.GetCurSel() ;

    bool  bSwich = m_struDeviceInfo.struChanInfo[m_iCurChanIndex].struDecodercfg.bySetPreset[iIndex] ;
	
	iPNum = iIndex + 1;
	vecPresetPoints.clear() ;
	if (bSwich)
	{
		bChangePoint = false ;
		GetDlgItem(IDC_BUTTON_PRESET_GOTO)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_PRESET_DEL)->EnableWindow(TRUE);
	}
	else
	{
		bChangePoint = true ;
		GetDlgItem(IDC_BUTTON_PRESET_GOTO)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_PRESET_DEL)->EnableWindow(FALSE);
	}
}

//����Ԥ�õ�
void CHKDlgDlg::OnBnClickedSeqSet()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	  bSetCruise = true ;

	CDlgPTZCruise Dlg;
	Dlg.DoModal();
}

/*
�������ܣ�
ת��ָ��Ԥ�õ�
����˵����
 **lRealHandle�� ������
 **dwPresetIndex �� Ԥ�õ�λ�����
*/

void CHKDlgDlg:: Program_PTZCruise(	LONG lRealHandle, DWORD dwPresetIndex )
{
	if (m_lPlayHandle >= 0)
	{
		if (!NET_DVR_PTZPreset(lRealHandle, GOTO_PRESET, dwPresetIndex))
		{
			MessageBox("����Ԥ�õ�ʧ��");
			return;
		}
	}
	else
	{
		if (!NET_DVR_PTZPreset_Other(m_struDeviceInfo.lLoginID, m_struDeviceInfo.struChanInfo[m_iCurChanIndex].iChanIndex, \
			GOTO_PRESET, dwPresetIndex))
		{
			MessageBox("����Ԥ�õ�ʧ��");
			return;
		}
	}
	Sleep(WAITTURNTIME) ;
}

//���Ѳ��
void CHKDlgDlg::OnBnClickedButtonSeqGoto()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	bCruise = true ;
	
		 HANDLE fff = CreateThread(NULL,0, Cruise,this,NULL,NULL);
	int iSeq = m_comboSeq.GetCurSel() + 1;  

	//+1
	/*if (!g_bCruise)
	{
		if (m_lPlayHandle >= 0)
		{
			if (!NET_DVR_PTZCruise(m_lPlayHandle, RUN_SEQ, iSeq, 0, 0))
			{
				MessageBox("����Ѳ��ʧ��");
				return;
			}
		}
		else
		{
			if (!NET_DVR_PTZCruise_Other(m_struDeviceInfo.lLoginID, m_struDeviceInfo.struChanInfo[m_iCurChanIndex].iChanIndex, \
				RUN_SEQ, iSeq, 0, 0))
			{
				MessageBox("����Ѳ��ʧ��");
				return;
			}

		}
		g_bCruise = TRUE;
		GetDlgItem(IDC_BUTTON_SEQ_GOTO)->SetWindowText("ֹͣ");
	}
	else
	{
		if (m_lPlayHandle >= 0)
		{
			if (!NET_DVR_PTZCruise(m_lPlayHandle, STOP_SEQ, iSeq, 0, 0))
			{
				MessageBox("ֹͣѲ��ʧ��");
				return;
			}
		}
		else
		{
			if (!NET_DVR_PTZCruise_Other(m_struDeviceInfo.lLoginID, m_struDeviceInfo.struChanInfo[m_iCurChanIndex].iChanIndex, \
				STOP_SEQ, iSeq, 0, 0))
			{
				MessageBox("ֹͣѲ��ʧ��");
				return;
			}

		}*/
/*		g_bCruise = FALSE;
;
}*/
	GetDlgItem(IDC_BUTTON_SEQ_GOTO)->SetWindowText("����") ;
}

//���ѭ��Ѳ��
void CHKDlgDlg::OnBnClickedButtonSeqGotor()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//while (1)
	//{
	//	int iSeq = m_comboSeq.GetCurSel() + 1;    //+1
	//	if (!g_bCruise)
	//	{
	//		if (m_lPlayHandle >= 0)
	//		{
	//			if (!NET_DVR_PTZCruise(m_lPlayHandle, RUN_SEQ, iSeq, 0, 0))
	//			{
	//				MessageBox("����Ѳ��ʧ��");
	//				return;
	//			}
	//		}
	//		else
	//		{
	//			if (!NET_DVR_PTZCruise_Other(m_struDeviceInfo.lLoginID, m_struDeviceInfo.struChanInfo[m_iCurChanIndex].iChanIndex, \
	//				RUN_SEQ, iSeq, 0, 0))
	//			{
	//				MessageBox("����Ѳ��ʧ��");
	//				return;
	//			}

	//		}
	//		g_bCruise = TRUE;
	//		GetDlgItem(IDC_BUTTON_SEQ_GOTO)->SetWindowText("ֹͣ");
	//	}
	//	else
	//	{
	//		if (m_lPlayHandle >= 0)
	//		{
	//			if (!NET_DVR_PTZCruise(m_lPlayHandle, STOP_SEQ, iSeq, 0, 0))
	//			{
	//				MessageBox("ֹͣѲ��ʧ��");
	//				return;
	//			}
	//		}
	//		else
	//		{
	//			if (!NET_DVR_PTZCruise_Other(m_struDeviceInfo.lLoginID, m_struDeviceInfo.struChanInfo[m_iCurChanIndex].iChanIndex, \
	//				STOP_SEQ, iSeq, 0, 0))
	//			{
	//				MessageBox("ֹͣѲ��ʧ��");
	//				return;
	//			}

	//		}
	//		g_bCruise = FALSE;
	//		GetDlgItem(IDC_BUTTON_SEQ_GOTO)->SetWindowText("����");
	//	}

	//}
}


void CHKDlgDlg::OnButtonTrackStop()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	bCruise = false ;


}


void CHKDlgDlg::OnBnClickedBtnfindvehicle()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	// dotest1(1 ) ;

	DWORD dwReturned;
	NET_DVR_DECODERCFG m_pStru485DecoderCfg;
	memset(&m_pStru485DecoderCfg, 0, sizeof(NET_DVR_DECODERCFG));
	BOOL bRet = NET_DVR_GetDVRConfig(m_struDeviceInfo.lLoginID, NET_DVR_GET_DECODERCFG, 1, &m_pStru485DecoderCfg, sizeof(NET_DVR_DECODERCFG), &dwReturned);



	LOG("this is[test]");

}

//
//void CHKDlgDlg::OnIpnFieldchangedIpaddress1(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	*pResult = 0;
//}

void CHKDlgDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CHKDlgDlg::OnBnClickedButtonZoomout()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CHKDlgDlg::OnBnClickedButtonConfirmroi()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	GetDlgItem(IDC_BUTTON_SetROI)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CONFIRMROI)->EnableWindow(FALSE);
	bSetRoi = false ;

}


void CHKDlgDlg::OnEnChangeCheckbug()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CHKDlgDlg::OnEnChangeEditPicaddr()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CHKDlgDlg::OnBnClickedBtnBrowser()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	char sDir[MAX_PATH] = { 0 };
	BROWSEINFO bi;
	ITEMIDLIST *pIdl = nullptr;
	bi.hwndOwner = this->m_hWnd;
	bi.pidlRoot = nullptr;
	bi.pszDisplayName = sDir;
	bi.lpszTitle = "��ѡ��Ŀ¼";
	bi.ulFlags = BIF_STATUSTEXT | BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
	bi.lpfn = nullptr;
	bi.lParam = 0;
	bi.iImage = 0;
	pIdl = SHBrowseForFolder(&bi);
	if (pIdl == nullptr)return;
	if (!SHGetPathFromIDList(pIdl, sDir))return;
	else m_picaddress = sDir;
	UpdateData(FALSE);
	IniWrite("savepath", sDir);
}


void CHKDlgDlg::OnNMDblclkListResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//1 ȡ��ѡ���к�
	int i = 0;
	for (; i < m_list.GetItemCount(); i++)
	{
		if (m_list.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
			break;
	}

	//2 ��ȡ����
	int nColumnCount = m_list.GetHeaderCtrl()->GetItemCount();

	//3 ȡ�õ�ǰ�����һ���ֵ
	TCHAR sPicPath[MAX_PATH] = TEXT(""); //��ȡͼƬ·��
	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));
	lvi.iItem = i;	//�к�
	lvi.iSubItem = 5; //�к�
	lvi.mask = LVIF_TEXT;
	lvi.cchTextMax = MAX_PATH;
	lvi.pszText = sPicPath;
	m_list.GetItem(&lvi);
	BOOL bComboPic = FALSE;
	if (strlen(sPicPath) < 1) //���û�� �ϳ�ͼ����ȡ�ڶ������ѵ�ͼ�����򷵻�
	{
		ZeroMemory(&lvi, sizeof(LVITEM));
		lvi.iItem = i;	//�к�
		lvi.iSubItem = nColumnCount-2; //�к�
		lvi.mask = LVIF_TEXT;
		lvi.cchTextMax = MAX_PATH;
		lvi.pszText = sPicPath;
		m_list.GetItem(&lvi);
		if (strlen(sPicPath) < 1)
			return;
	}
	else
	{
		bComboPic = TRUE;
	}

	TCHAR sCarId[20] = TEXT("");	//��ȡ���ƺ�
	lvi.iSubItem = 1; //�к�
	lvi.cchTextMax = sizeof(sCarId);
	lvi.pszText = sCarId;
	m_list.GetItem(&lvi);
	//AfxMessageBox(sCarId);

	//4 ����Ϣ���ݸ� ͼƬ��Ϣչʾ�� �Ա㷢�Ͷ���
	DetailInfo di;
	di.sPicPath = sPicPath;
	di.sSMSInfo.Format("���ƺ�[%s]�ĳ�����ע�⣬���Ļ������Ѿ�Υͣ������Ѹ�ٽ����ƿ�������ͨ���Ž����з��", sCarId);
	if (bComboPic)
		di.bComboPic = bComboPic;

	CPicInfo Dlg(di);
	Dlg.DoModal();
	if (di.bSendSMS)
	{
		m_list.SetItemText(i, 7, "�ѷ�");
	}
	*pResult = 0;
}
