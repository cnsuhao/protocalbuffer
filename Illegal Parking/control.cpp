
#include "StdAfx.h"
#include "control.h"
#include "HKDlg.h"
#include "HKDlgDlg.h"



void DVR_Start_Control(LONG lUserID,LONG lPlayHandle)
{
	NET_DVR_PTZPOS PTZposition1,PTZposition2;
	memset(&PTZposition1, 0, sizeof(NET_DVR_PTZPOS));
	memset(&PTZposition2, 0, sizeof(NET_DVR_PTZPOS));
	DWORD lpreturned = 0;
	
	NET_DVR_GetDVRConfig(lUserID,NET_DVR_GET_PTZPOS,1,&PTZposition1,sizeof(NET_DVR_PTZPOS),&lpreturned);
    NET_DVR_GetDVRConfig(lUserID,NET_DVR_GET_PTZPOS,1,&PTZposition2,sizeof(NET_DVR_PTZPOS),&lpreturned);
    
	while (PTZposition1.wPanPos - PTZposition2.wPanPos < 0x0050)
    {
		NET_DVR_PTZControlWithSpeed(lPlayHandle,PAN_LEFT,0,1);
		NET_DVR_GetDVRConfig(lUserID,NET_DVR_GET_PTZPOS,1,&PTZposition2,sizeof(NET_DVR_PTZPOS),&lpreturned);
    }
	NET_DVR_PTZControlWithSpeed(lPlayHandle,PAN_LEFT,0,1);
}

void DVR_Stop_Control(LONG lUserID,LONG lPlayHandle)
{
	NET_DVR_PTZControl(lPlayHandle,PAN_LEFT,1);
	NET_DVR_PTZControl(lPlayHandle,TILT_UP,1);
	NET_DVR_PTZControl(lPlayHandle,ZOOM_IN,1);
}
