#pragma once
#include "afxwin.h"

class DetailInfo
{
public:
	DetailInfo() :sPicPath(""), sSMSInfo("this is a test SMS"), bComboPic(FALSE), bSendSMS(FALSE){}
	CString sPicPath;
	CString sSMSInfo;
	BOOL	bComboPic;
	BOOL	bSendSMS;
};
// CPicInfo 对话框

class CPicInfo : public CDialog
{
	DECLARE_DYNAMIC(CPicInfo)

public:
	CPicInfo(DetailInfo & di,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPicInfo();

// 对话框数据
	enum { IDD = IDD_DLG_PICINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnSend();
	CStatic m_ctlPic;
	BOOL OnInitDialog();
	void ShowImage(IplImage* img, UINT ID);
	DetailInfo* di;
	CvvImage* m_cimg;
	afx_msg void OnPaint();
};
