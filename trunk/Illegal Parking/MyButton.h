#pragma once
#include "afxwin.h"
class MyButton :
	public CButton
{
public:
	int tag;
	LONG handle;
	MyButton(void);
	~MyButton(void);
	void PTZControlAll(LONG lRealHandle, DWORD dwPTZCommand, DWORD dwStop, int Speed);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);



};


