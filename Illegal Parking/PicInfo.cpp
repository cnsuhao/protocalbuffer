// PicInfo.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "HKDlg.h"
#include "PicInfo.h"
#include "afxdialogex.h"


// CPicInfo �Ի���

IMPLEMENT_DYNAMIC(CPicInfo, CDialogEx)

CPicInfo::CPicInfo(DetailInfo & di, CWnd* pParent /*=NULL*/)
	: CDialog(CPicInfo::IDD, pParent)
{
	this->di = &di;
	m_cimg = nullptr;
}

CPicInfo::~CPicInfo()
{
	if (m_cimg)
	{
		delete m_cimg;
		m_cimg = nullptr;
	}
}

void CPicInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PIC, m_ctlPic);
}


BEGIN_MESSAGE_MAP(CPicInfo, CDialog)
	ON_BN_CLICKED(ID_BTN_SEND, &CPicInfo::OnBnClickedBtnSend)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CPicInfo ��Ϣ�������


void CPicInfo::OnBnClickedBtnSend()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	di->bSendSMS = TRUE;
	AfxMessageBox("�����ѳɹ����ͣ�", MB_OK);
}
BOOL CPicInfo::OnInitDialog()
{
	if (di==nullptr)
	{
		return TRUE;
	}
	IplImage * imgshow = NULL;
	imgshow = cvLoadImage(di->sPicPath.GetBuffer(di->sPicPath.GetLength()));
	di->sPicPath.ReleaseBuffer();
	if (imgshow != NULL)
	{
		if (di->bComboPic)
		{
			GetDlgItem(IDC_EDT_SMS)->ShowWindow(SW_HIDE);
			GetDlgItem(ID_BTN_SEND)->ShowWindow(SW_HIDE);
			GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_WARNING_PIC)->ShowWindow(SW_HIDE);
			ShowImage(imgshow, IDC_COMBO_PIC);
		}
		else
		{
			GetDlgItem(IDC_COMBO_PIC)->ShowWindow(SW_HIDE);
			ShowImage(imgshow, IDC_WARNING_PIC);
		}
		
		cvReleaseImage(&imgshow);
	}
	SetDlgItemText(IDC_EDT_SMS,di->sSMSInfo);
	this->SetWindowText( "ͼƬ��Ϣ��"+di->sPicPath );
	return TRUE;
}

//��opencv��IplImage������ʾ��ͼƬ�ؼ���
void CPicInfo::ShowImage(IplImage* img, UINT ID)
{
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();

	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);

	if (m_cimg==nullptr)
	{
		m_cimg = new CvvImage();
	}
	m_cimg->CopyOf(img);
	m_cimg->DrawToHDC(hDC, &rect);

	ReleaseDC(pDC);
	::DeleteDC(hDC);
}

void CPicInfo::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()
	if (di == nullptr)
	{
		return;
	}
	IplImage * imgshow = NULL;
	imgshow = cvLoadImage(di->sPicPath.GetBuffer(di->sPicPath.GetLength()));
	di->sPicPath.ReleaseBuffer();
	if (imgshow != NULL)
	{
		if (di->bComboPic)
		{
			ShowImage(imgshow, IDC_COMBO_PIC);
		}
		else
		{
			ShowImage(imgshow, IDC_WARNING_PIC);
		}
		cvReleaseImage(&imgshow);
	}
}
