// DlgPTZCruise.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "HKDlg.h"
#include "DlgPTZCruise.h"
#include "afxdialogex.h"
#include "Thread.h"

CruiseINFO ::CruiseINFO()
{

	bRunning = false ;
	bTurned = false ;
	iPresetNum = -1 ;
	iStaytime = -1 ;
	iSpeed    = -1 ;
}

CruiseINFO :: ~CruiseINFO()
{

}


CruiseINFO arrCruiseInfo[8][CRUISE_MAX_PRESET_NUMS] ;

// CDlgPTZCruise �Ի���

IMPLEMENT_DYNAMIC(CDlgPTZCruise, CDialogEx)

CDlgPTZCruise::CDlgPTZCruise(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgPTZCruise::IDD, pParent)
	, m_iCruiseNum(1)
	, m_iSeqPoint(1)
	, m_iPresetNum(1)
	, m_iSeqDwell(2)
	, m_iSeqSpeed(30)
	, m_iChanNum(-1)
{

}

CDlgPTZCruise::~CDlgPTZCruise()
{
}

void CDlgPTZCruise::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CRUISE_ROUTE, m_comboCruiseRoute);
	DDX_Control(pDX, IDC_COMBO_CRUISE_POINT, m_comboPoint);
	DDX_Control(pDX, IDC_COMBO_CRUISE_PRESET, m_comboSeqPreset);
	DDX_Control(pDX, IDC_COMBO_CRUISE_TIME, m_comboSeqTime);
	DDX_Control(pDX, IDC_COMBO_CRUISE_SPEED, m_comboSeqSpeed);
}


BEGIN_MESSAGE_MAP(CDlgPTZCruise, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CDlgPTZCruise::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CDlgPTZCruise::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CDlgPTZCruise::OnBnClickedDel)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CDlgPTZCruise::OnBnClickedButtonOk)
	ON_CBN_SELCHANGE(IDC_COMBO_CRUISE_POINT, &CDlgPTZCruise::OnCbnSelchangeComboCruisePoint)
	ON_CBN_SELCHANGE(IDC_COMBO_CRUISE_ROUTE, &CDlgPTZCruise::OnCbnSelchangeComboCruiseRoute)
END_MESSAGE_MAP()


// CDlgPTZCruise ��Ϣ�������


void CDlgPTZCruise::OnBnClickedCancel()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	 bSetCruise = false ;
	CDialogEx::OnCancel();
}


BOOL CDlgPTZCruise::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CDialog::OnInitDialog();

	int i;
	CString tmp;
	for (i = 0; i<MAX_CRUISE_SEQ; i++)
	{
		tmp.Format("Ѳ��·��%d", i + 1);
		m_comboCruiseRoute.AddString(tmp);
	}
	m_comboCruiseRoute.SetCurSel(0);

	for (i = 0; i<MAX_CRUISE_POINT; i++)
	{
		tmp.Format("%d", i + 1);
		m_comboPoint.AddString(tmp);
	}
	m_comboPoint.SetCurSel(0);

	for (i = 0; i<MAX_CRUISE_PRESET; i++)
	{
		tmp.Format("%d", i + 1);
		m_comboSeqPreset.AddString(tmp);
	}
	m_comboSeqPreset.SetCurSel(0);

	for (i = 0; i<MAX_CRUISE_TIME; i++)
	{
		tmp.Format("%d", i + 1);
		m_comboSeqTime.AddString(tmp);
	}
	m_comboSeqTime.SetCurSel(10);

	for (i = 0; i<MAX_CRUISE_SPEED; i++)
	{
		tmp.Format("%d", i + 1);
		m_comboSeqSpeed.AddString(tmp);
	}
	m_comboSeqSpeed.SetCurSel(4);


	//CHKDlgDlg * p = g_pMainDlg;
//	int ii = p->m_iCurChanIndex;
	g_pMainDlg->m_iCurChanIndex = 1;

	m_iChanNum = g_pMainDlg->m_struDeviceInfo.struChanInfo[g_pMainDlg->m_iCurChanIndex].iChanIndex;

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

BOOL CDlgPTZCruise::Set_NET_DVR_PTZCruise(int iChanNum, DWORD dwPTZCruiseCmd, BYTE byCruiseRoute, BYTE byCruisePoint, WORD wInput)
{
	if (g_pMainDlg->m_lPlayHandle >= 0)
	{
		if (!NET_DVR_PTZCruise(g_pMainDlg->m_lPlayHandle, dwPTZCruiseCmd, byCruiseRoute, byCruisePoint, wInput))
		{
			MessageBox("����ʧ��");
			return FALSE;
		}
	}
	else
	{
		if (!NET_DVR_PTZCruise_Other(g_pMainDlg->m_struDeviceInfo.lLoginID, m_iChanNum, dwPTZCruiseCmd, byCruiseRoute, byCruisePoint, wInput))
		{
			MessageBox("����ʧ��");
			return FALSE;
		}
	}

	return TRUE;
}

void CDlgPTZCruise::OnBnClickedAdd()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	BOOL bRet = FALSE;
	m_iCruiseNum = m_comboCruiseRoute.GetCurSel() ;
	m_iSeqPoint = m_comboPoint.GetCurSel() ;
	m_iPresetNum = m_comboSeqPreset.GetCurSel() + 1;
	m_iSeqDwell = m_comboSeqTime.GetCurSel() + 1;
	m_iSeqSpeed = m_comboSeqSpeed.GetCurSel() + 1;

	arrCruiseInfo[m_iCruiseNum][m_iSeqPoint].iPresetNum = m_iPresetNum ;
	arrCruiseInfo[m_iCruiseNum][m_iSeqPoint].iStaytime = m_iSeqDwell ;
	arrCruiseInfo[m_iCruiseNum][m_iSeqPoint].iSpeed = m_iSeqSpeed ;

	//bRet = Set_NET_DVR_PTZCruise(m_iChanNum, FILL_PRE_SEQ, (BYTE)m_iCruiseNum, (BYTE)m_iSeqPoint, m_iPresetNum);
	//if (bRet)
	//{
	//	bRet = Set_NET_DVR_PTZCruise(m_iChanNum, SET_SEQ_DWELL, (BYTE)m_iCruiseNum, (BYTE)m_iSeqPoint, m_iSeqDwell);
	//	if (bRet)
	//	{
	//		bRet = Set_NET_DVR_PTZCruise(m_iChanNum, SET_SEQ_SPEED, (BYTE)m_iCruiseNum, (BYTE)m_iSeqPoint, m_iSeqSpeed);
	//	}
	//}
}


void CDlgPTZCruise::OnBnClickedDel()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	BOOL bRet = FALSE;
	m_iCruiseNum = m_comboCruiseRoute.GetCurSel() + 1;
	m_iSeqPoint = m_comboPoint.GetCurSel() + 1;
	m_iPresetNum = m_comboSeqPreset.GetCurSel() + 1;

	arrCruiseInfo[m_iCruiseNum][m_iSeqPoint].iPresetNum = -1 ;
	arrCruiseInfo[m_iCruiseNum][m_iSeqPoint].iStaytime = -1 ;
	arrCruiseInfo[m_iCruiseNum][m_iSeqPoint].iSpeed = -1 ;
	//if (g_pMainDlg->m_lPlayHandle >= 0)
	//{
	//	if (!NET_DVR_PTZCruise(g_pMainDlg->m_lPlayHandle, CLE_PRE_SEQ, m_iCruiseNum, m_iSeqPoint, m_iPresetNum))
	//	{
	//		MessageBox("ɾ��ʧ��");
	//		return;
	//	}
	//}
	//else
	//{
	//	if (!NET_DVR_PTZCruise_Other(g_pMainDlg->m_struDeviceInfo.lLoginID, m_iChanNum, CLE_PRE_SEQ, m_iCruiseNum, m_iSeqPoint, m_iPresetNum))
	//	{
	//		MessageBox("ɾ��ʧ��");
	//		return;
	//	}
	//}
}


void CDlgPTZCruise::OnBnClickedButtonOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	UpdateData(FALSE);
	 bSetCruise = false ;

	CDialogEx::OnOK();
}


void CDlgPTZCruise::OnCbnSelchangeComboCruisePoint()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(WrongPark->vecPtRoi[iPNum]->size()!=0 )
	{
		int x = 0 ;

	}

}


void CDlgPTZCruise::OnCbnSelchangeComboCruiseRoute()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
