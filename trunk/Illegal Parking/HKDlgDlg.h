// HKDlgDlg.h : header file
//

#if !defined(AFX_HKDLGDLG_H__0EA94231_D24E_420E_AB10_56018DDED5BB__INCLUDED_)
#define AFX_HKDLGDLG_H__0EA94231_D24E_420E_AB10_56018DDED5BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeneralDef.h"
#include	<opencv/highgui.h>
#include "CvvImage.h"
#include "easysize.h"
#include "afxwin.h"
#include "MyButton.h"
#include "resource.h"
#include "CAdo.h"
#include "PicInfo.h"
#include "ClientSocketThread.h"
/////////////////////////////////////////////////////////////////////////////
// CHKDlgDlg dialog

#define  WAITTURNTIME  3000
#define  TURN_TO_WARNINGPOINT  2
#define  TURN_TO_CRUISEPOINT   1 
#define  TURN_TO_NOWHERE   0 

#define  STATE_NORMAL 0 
#define  STATE_IN_CAPTURE   1


#define SHOW_REPAINTIMG					(WM_USER+100)
//#define WM_MYUPDATEDATA WM_USER+131 // 2015-07-07 by YaoJH

void CALLBACK g_RealDataCallBack_V30(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize,void* dwUser);
class DbConfig
{
public:
	//char DbIP[20];
	char DbUser[20];
	char DbPwd[20];
	//char DbPort[8];
	//char DbDataBase[20];
	char DbConnect[MAX_PATH];
};
class CHKDlgDlg : public CDialog
{
	DECLARE_EASYSIZE
// Construction
public:
	CHKDlgDlg(CWnd* pParent = NULL);	// standard constructor
	int flag_drawline;
	int flag_drawlane;
// 	CPoint Porigin,Pend;
// 	CPoint p_roi_1,p_roi_2;

public:
	LOCAL_DEVICE_INFO m_struDeviceInfo;
	BOOL m_bIsPlaying;
	BOOL m_bIsLogin;
	BOOL DoLogin();
	BOOL Login();

	LONG m_lPlayHandle;
	LONG m_lDecID;
	IplImage *welcomeImage;

	int m_iCurChanIndex;               //当前通道在数组中索引 2015-07-21

	void ShowImage(IplImage* img, UINT ID);
	bool bCruise ;

	NET_DVR_START_PIC_VIEW_INFO m_struPicPreviewInfo;
	LRESULT OnShowHikImgMessage(WPARAM wParam, LPARAM lParam); // Yaojh
	LRESULT OnUpdateDateY(WPARAM wParam, LPARAM lParam);
	LRESULT OnCfgData(WPARAM wParam, LPARAM lParam);//网络信息响应 hb
	void StopPlay();
	bool StartPlay(int iChanIndex);
	LRESULT showInitImage(WPARAM wParam, LPARAM lParam);
	void DoGetDeviceResoureCfg();

	void Program_PTZCruise(	LONG lRealHandle, DWORD dwPresetIndex ) ;

		BOOL PreTranslateMessage(MSG* pMsg) ;
	// Dialog Data
	//{{AFX_DATA(CHKDlgDlg)
	enum { IDD = IDD_HKDLG_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHKDlgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CHKDlgDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLogin();
	afx_msg void OnButtonPlay();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonRmnight();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonOut();
	CIPAddressCtrl m_ip;
	CString m_height;
	CString m_width;
	CString DeviceIp;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	MyButton m_up;
	MyButton m_down;
	MyButton m_left;
	MyButton m_right;
	MyButton m_zoomin;
	MyButton m_zoomout;

	int m_preset;
	afx_msg void OnEnChangeEditPreset();


	afx_msg void OnClickedButtonSetpreset();
	afx_msg void OnClickedButtonGotopreset();
//	CString m_picaddress;
	afx_msg void OnClickedButtonCapture();
	CString m_picaddress;

	CString m_coordinary ;
	
	CString m_offset ;

	CListCtrl m_list;
	
	afx_msg void OnBnClickedButtonSetroi();
	afx_msg void OnLeftButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRightButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	/* PTZ speed: 1,2,3,4,5 */
	// PTZ speed: 1,2,3,4,5
	CComboBox m_comboPTZSpeed;
	// cruise line
	CComboBox m_comboSeq;
	afx_msg void OnBnClickedButtonPresetGoto();
	afx_msg void OnBnClickedPresetSet();
	afx_msg void OnBnClickedPresetDel();
	// 预置点
	CComboBox m_comboPreset;
	afx_msg void OnCbnSelchangeComboPreset();
	afx_msg void OnBnClickedSeqSet();
	afx_msg void OnBnClickedButtonSeqGoto();
	afx_msg void OnBnClickedButtonSeqGotor();
	afx_msg void OnButtonTrackStop();
	afx_msg void OnBnClickedBtnfindvehicle();
	afx_msg void OnBnClickedButton2();

	static DWORD WINAPI  Cruise(void* aaa) ;



	afx_msg void OnBnClickedButtonZoomout();
	afx_msg void OnBnClickedButtonConfirmroi();

	afx_msg void OnEnChangeCheckbug();
	afx_msg void OnEnChangeEditPicaddr();

	CString m_strChekcBug;
	CString m_GetFrame;
	CString m_PlatePos;
	CString m_PhotoDeal;

	CString strWarningMsg;
	CAdo	m_ado; //新增数据操作by hb at 2016.03.17
	DbConfig dbConfig; //新增数据配置类by hb at 2016.03.18
	ClientSocketThread *m_pClientSocketThread;
	afx_msg void OnBnClickedBtnBrowser();
	afx_msg void OnNMDblclkListResult(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HKDLGDLG_H__0EA94231_D24E_420E_AB10_56018DDED5BB__INCLUDED_)
