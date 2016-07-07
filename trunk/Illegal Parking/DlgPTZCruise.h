#pragma once
#include "afxwin.h"
#include <vector>

// CDlgPTZCruise 对话框

class CruiseINFO
{
public:
   CruiseINFO();
   ~CruiseINFO() ;

   bool bRunning ;
   bool bTurned ;
   int iPresetNum ;
   int iStaytime ;
   int iSpeed ;
};

extern	CruiseINFO arrCruiseInfo[8][CRUISE_MAX_PRESET_NUMS] ;

class CDlgPTZCruise : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPTZCruise)

public:
	BOOL Set_NET_DVR_PTZCruise(int iChanIndex, DWORD dwPTZCruiseCmd, BYTE byCruiseRoute, BYTE byCruisePoint, WORD wInput);
	CDlgPTZCruise(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgPTZCruise();

	int m_iCruiseNum;
	int m_iSeqPoint;
	int m_iPresetNum;
	int m_iSeqDwell;//time interval
	int m_iSeqSpeed;
	int m_iChanNum;       //通道号
	
	std::vector<int> vecCruiseSeq ;

// 对话框数据
	enum { IDD = IDD_DLG_PTZ_CRUISE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	CComboBox m_comboCruiseRoute;
	CComboBox m_comboPoint;
	CComboBox m_comboSeqPreset;
	CComboBox m_comboSeqTime;
	CComboBox m_comboSeqSpeed;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDel();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnCbnSelchangeComboCruisePoint();
	afx_msg void OnCbnSelchangeComboCruiseRoute();
};


 

