#include "stdafx.h"
#include "MyButton.h"
#include "GeneralDef.h"

MyButton::MyButton(void)
{
}


MyButton::~MyButton(void)
{
}
BEGIN_MESSAGE_MAP(MyButton, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


void MyButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
		
	switch(tag) {
	case 0: PTZControlAll(this->handle, TILT_UP,   0, 3); break;
	case 1: PTZControlAll(this->handle, TILT_DOWN, 0, 3); break;
	case 2: PTZControlAll(this->handle, PAN_LEFT,  0, 3); break;
	case 3: PTZControlAll(this->handle, PAN_RIGHT, 0, 3); break;
	case 4: PTZControlAll(this->handle, ZOOM_IN,   0, 3); break;
	case 5: PTZControlAll(this->handle, ZOOM_OUT,  0, 3); break;
	default: break;
	}
	
	CButton::OnLButtonDown(nFlags, point);
}


void MyButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	
	switch(tag) {
	case 0: PTZControlAll(this->handle, TILT_UP,   1, 3); break;
	case 1: PTZControlAll(this->handle, TILT_DOWN, 1, 3); break;
	case 2: PTZControlAll(this->handle, PAN_LEFT,  1, 3); break;
	case 3: PTZControlAll(this->handle, PAN_RIGHT, 1, 3); break;
	case 4: PTZControlAll(this->handle, ZOOM_IN,   1, 3); break;
	case 5: PTZControlAll(this->handle, ZOOM_OUT,  1, 3); break;
	
	default: break;
	}
	
	CButton::OnLButtonUp(nFlags, point);
}

/*************************************************
������:    	PTZControlAll
��������:	��̨���ƺ���
�������:   
�������:   			
����ֵ:		
*************************************************/
void MyButton::PTZControlAll(LONG lRealHandle, DWORD dwPTZCommand, DWORD dwStop, int Speed)
{
	if(lRealHandle>=0)
	{
		BOOL ret;
		if(Speed>=1)
		{
			ret = NET_DVR_PTZControlWithSpeed(lRealHandle,dwPTZCommand,dwStop,Speed);
			TRACE("%d\n", ret);
			if(!ret)
			{
				AfxMessageBox("��̨����ʧ��!");
				return;
			}
		}
		else
		{
			ret = NET_DVR_PTZControl(lRealHandle,dwPTZCommand,dwStop);
			if(!ret)
			{
				MessageBox("��̨����ʧ��!");
				return;
			}
		}
	}

}
