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
// CPicInfo �Ի���

class CPicInfo : public CDialog
{
	DECLARE_DYNAMIC(CPicInfo)

public:
	CPicInfo(DetailInfo & di,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPicInfo();

// �Ի�������
	enum { IDD = IDD_DLG_PICINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
