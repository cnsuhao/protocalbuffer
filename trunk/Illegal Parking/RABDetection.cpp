//// RABDetection.cpp : 定义控制台应用程序的入口点。
////
//
#include "StdAfx.h"
#include "RABDetection.h"
#include "blockNode.h"
//
//#include"blockNode.h"
//using namespace std;
//
////划分警戒区域变量
//int t=0;
//int a[100][2];
/////////初始化字体格式
//CvFont font1;
//CvPoint ptext;
//int linetype=CV_AA;
//string msg[16]={"Alarm!!!","01","02","03","04","05","06","07","08","09","10","11","12","13","14","15"};
//
//const int CONTOUR_MAX_AERA = 16;
////CvFilter filter = CV_GAUSSIAN_5x5;
//int nFrmNum = 0;
//bool matchValue = false;
//const int MAX_X_STEP = 20;
//const int MAX_Y_STEP = 50;
//
//const double MHI_DURATION = 0.5;
//const double MATCH_RATIO = 0.5;
//const int N = 3;
//int last = 0;
//std::vector<CvRect> objDetRect;
//std::vector<blockTrack> trackBlock;
//IplImage *mhi = 0; // MHI: motion history image
//IplImage **buf = 0;
//
//void initRABDetection()
//{
//	if(mhi!=NULL)
//		cvReleaseImage( &mhi );
//	buf=NULL;
//	mhi = NULL;
//	objDetRect.clear();
//	trackBlock.clear();
//};
//void  m_Detect( IplImage* img, IplImage* dst, int diff_threshold )
//{
//	double timestamp = clock()/100.; // get current time in seconds
//	CvSize size = cvSize(img->width,img->height); // get current frame size
//	int i, idx1, idx2;
//	IplImage* silh;
//	IplImage* pyr = cvCreateImage( cvSize((size.width & -2)/2, (size.height & -2)/2), 8, 1 );
//	CvMemStorage *stor;
//	CvSeq *cont;
//
//	if( !mhi || mhi->width != size.width || mhi->height != size.height ) 
//	{
//		if( buf == 0 ) 
//		{
//			buf = (IplImage**)malloc(N*sizeof(buf[0]));
//			memset( buf, 0, N*sizeof(buf[0]));
//		}
//
//		for( i = 0; i < N; i++ ) 
//		{
//			cvReleaseImage( &buf[i] );
//			buf[i] = cvCreateImage( size, IPL_DEPTH_8U, 1 );
//			cvZero( buf[i] );
//		}
//		cvReleaseImage( &mhi );
//		mhi = cvCreateImage( size, IPL_DEPTH_32F, 1 );
//		cvZero( mhi ); // clear MHI at the beginning
//	} // end of if(mhi)
//
//	cvCvtColor( img, buf[last], CV_BGR2GRAY ); // convert frame to grayscale
//
//	idx1 = last;
//	idx2 = (last + 1) % N; // index of (last - (N-1))th frame 
//	last = idx2;
//
//	silh = buf[idx2];
//	cvAbsDiff( buf[idx1], buf[idx2], silh ); // 做帧差
//
//	cvThreshold( silh, silh, diff_threshold, 255, CV_THRESH_BINARY ); // 对差图像做二值化
//
//	cvUpdateMotionHistory( silh, mhi, timestamp, MHI_DURATION ); // update MHI
//	cvCvtScale( mhi, dst, 255./MHI_DURATION, 
//		(MHI_DURATION - timestamp)*255./MHI_DURATION );    
//	cvCvtScale( mhi, dst, 255./MHI_DURATION, 0 );    
//
//	cvSmooth( dst, dst, CV_MEDIAN, 3, 0, 0, 0 );// 中值滤波，消除小的噪声	
//	
//	cvPyrDown( dst, pyr, 7 );// 向下采样，去掉噪声
//	cvDilate( pyr, pyr, 0, 1 );  // 做膨胀操作，消除目标的不连续空洞
//	cvPyrUp( pyr, dst, 7 );
//
//	// Create dynamic structure and sequence.
//	stor = cvCreateMemStorage(0);
//	cont = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint) , stor);
//
//	cvFindContours( dst, stor, &cont, sizeof(CvContour), 
//		CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
//
//	// 直接使用CONTOUR中的矩形来画轮廓
//	for(;cont;cont = cont->h_next)
//	{
//		CvRect r = ((CvContour*)cont)->rect;
//		if(r.height * r.width > CONTOUR_MAX_AERA) // 去除噪声
//			objDetRect.push_back(r);
//	}
//
//	cvReleaseMemStorage(&stor);
//	cvReleaseImage( &pyr );
//} 
////中心坐标移动距离
//bool match(CvRect rect1, CvRect rect2)
//{
//	CvPoint pt1,pt2;
//	pt1 = cvPoint(rect1.x+rect1.width/2,rect1.y+rect1.height/2);
//	pt2 = cvPoint(rect2.x+rect2.width/2,rect2.y+rect2.height/2);
//	if (abs(pt1.x-pt2.x)<MAX_X_STEP&&abs(pt1.y-pt2.y)<MAX_Y_STEP)
//		return TRUE;		
//
//	return FALSE;
//}
//bool ptInPolygon(CvPoint** ptx,int t,CvPoint pt)
//{
//	int nCross = 0; 
//	for (int i = 0; i < t; i++) 
//	{ 
//		CvPoint p1 = ptx[0][i]; 
//		CvPoint p2 = ptx[0][(i + 1) % t]; 
//		// 求解 y=p.y 与 p1p2 的交点 
//		if ( p1.y == p2.y ) // p1p2 与 y=p0.y平行 
//			continue; 
//		if ( pt.y < min(p1.y, p2.y) ) // 交点在p1p2延长线上 
//			continue; 
//		if ( pt.y >= max(p1.y, p2.y) ) // 交点在p1p2延长线上 
//			continue; 
//		// 求交点的 X 坐标 
//		double x = (double)(pt.y - p1.y) * (double)(p2.x - p1.x) / (double)(p2.y - p1.y) + p1.x; 
//		if ( x > pt.x ) 
//			nCross++; // 只统计单边交点 
//	} 
//	// 单边交点为偶数，点在多边形之外
//	return (nCross % 2 == 1); 
//}
//int RABDetection(IplImage* img, CvPoint** ptx, int t)
//{
//	int flag = 0;
//	CvPoint ptm;
//	int k = 0;
//	IplImage* motion = NULL;
//
//	//objDetRect.clear();
//	//trackBlock.clear();
//
//	ptext = cvPoint(10,30);
//	cvInitFont(&font1, CV_FONT_HERSHEY_TRIPLEX , 0.5, 0.5, 0, 1, 8);
//	if( !motion )
//	{
//		motion = cvCreateImage( cvSize(img->width,img->height), IPL_DEPTH_8U, 1 );
//		cvZero( motion );
//		motion->origin = img->origin;
//	}
//
//	//******跟踪目标初始化
//	if (trackBlock.empty())
//	{
//		for(vector<CvRect>::iterator it=objDetRect.begin(); it!=objDetRect.end();++it)
//		{
//			blockTrack blockTemp;
//			blockTemp.flag = false;
//			blockTemp.crsAear = false;
//			blockTemp.pt[0] = cvPoint(it->x + it->width/2, it->y + it->height/2);
//			blockTemp.pt[MAX_STEP-1] = cvPoint(1, 1);  //
//			blockTemp.rect = *it;
//			blockTemp.step = 0;
//			trackBlock.push_back(blockTemp);
//		}
//	}
//	objDetRect.clear();
//
//	m_Detect(img,motion,30);    ////////////////////////////////运动目标检测主函数******
//
//	//******跟踪目标匹配
//	for(vector<CvRect>::iterator it=objDetRect.begin(); it!=objDetRect.end();++it)
//	{
//		ptm=cvPoint(it->x+ it->width/2, it->y + it->height/2);
//		for(vector<blockTrack>::iterator itt=trackBlock.begin(); itt!=trackBlock.end();++itt)
//		{
//			if (itt->flag)
//				continue;
//			if(match(*it,itt->rect))
//			{
//				itt->rect = *it;
//				itt->step = (itt->step+1)%MAX_STEP;
//				itt->pt[itt->step] = ptm;
//				itt->flag = true;
//				matchValue = true;
//				break;
//			}
//		}
//		if (!matchValue)
//		{
//			blockTrack temp;
//			temp.flag = true;
//			temp.crsAear = false;
//			temp.pt[0] = ptm;
//			temp.pt[MAX_STEP-1] = cvPoint(1, 1);  //
//			temp.rect = *it;
//			temp.step = 0;
//			trackBlock.push_back(temp);
//		}
//		matchValue = false;
//	}
//
//	//******目标更新
//	for(vector<blockTrack>::iterator ita=trackBlock.begin(); ita!=trackBlock.end();)
//	{
//		if(!ita->flag)
//		{
//			//ita = trackBlock.erase(ita);
//			if ( ita == trackBlock.begin() )
//			{
//			trackBlock.erase(ita);
//			ita = trackBlock.begin();
//			}
//			else
//			{
//				ita=trackBlock.erase(ita);    //擦除报到m的元素
//				ita--;
//			}
//		}
//		else								
//		{
//			CvRect rect = ita->rect;
//			ptm=cvPoint(rect.x+ rect.width/2, rect.y + rect.height/2);
//			if ( ptInPolygon(ptx,t,ptm)||ita->crsAear ) 
//			{
//				ita->crsAear =true;
//				cvRectangle( img, cvPoint(rect.x,rect.y), 
//					cvPoint(rect.x + rect.width, rect.y + rect.height),
//					CV_RGB(255,0,0), 1, CV_AA,0);
//				if (ita->pt[MAX_STEP-1].x == 1)
//					for(int i = 0;i<ita->step-1;i++)
//						cvLine(img,ita->pt[i],ita->pt[i+1],CV_RGB(255,0,0),1,8,0);
//				else
//					for(int i = 0;i<MAX_STEP-1;i++)
//						cvLine(img,ita->pt[(ita->step+i+1)%MAX_STEP],ita->pt[(ita->step+i+2)%MAX_STEP],CV_RGB(255,0,0),1,8,0);
//
//				cvPutText( img, msg[0].c_str(), ptext, &font1, CV_RGB(255,0,0));
//				flag = 1;
//			}
//			ita->flag = false;
//			ita++;
//		}
//	}
//	cvReleaseImage(&motion);
//	cvPolyLine(img,ptx,&t,1,1,CV_RGB(255,255,0),1);      //画警戒区
//	return flag;
//}