#include <opencv2/opencv.hpp>
#include "highgui.h"
#include "vibe.h"
#include "blockNode.h"
#include "hashmap.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;
using namespace cv;

#define MIN_AREA 200
#define MAX_X_STEP  10	//match距离
#define MAX_Y_STEP  10	//match距离
#define PI 3.1415926
#define angleWithDirection 90 

#define Max_aspect_ratio 1.9
#define Min_aspect_ratio 0.7
#define Min_KeyPointSize   20

#define Frame_Diff  8
#define Frame_Slow  10
#define objDetRectNumber 10


int nFrmNum = 0;//记录帧数
int Width;//记录帧的宽度
int Height;//记录帧的高度
int FrameRate;//记录视频帧率
double aspect_ratio;//检测框宽高比
int rect_area;//检测框面积
int area_threhold; //检测框面积阈值

//定义正方向变量
int t=0;
CvPoint pt1 = cvPoint(100,100),
		pt2 = cvPoint(120,300); 
CvPoint roiPt1, roiPt2;
CvPoint tipCord = cvPoint(20,50);

bool match(CvRect rect1, CvRect rect2)	//匹配检测到的物体和跟踪物体的相似度，通过质心的移动距离来判断
{
	//中心坐标移动距离
	CvPoint pt1,pt2;

	pt1 = cvPoint(rect1.x+rect1.width/2,rect1.y+rect1.height/2);
	pt2 = cvPoint(rect2.x+rect2.width/2,rect2.y+rect2.height/2);

	if (abs(pt1.x - pt2.x ) < MAX_X_STEP && 
		abs(pt1.y - pt2.y ) < MAX_Y_STEP &&
		abs(rect1.width - rect2.width) < 10 &&
		abs(rect1.height - rect2.height) < 10
		)
		return true;
	else
		return false;
}

bool stop_detection(CvPoint pt1, CvPoint pt2)	//停止检测
{
	//中心坐标移动距离
	if (abs(pt1.x-pt2.x )< 2 && abs(pt1.y-pt2.y) < 2)
		return true;
	else
		return false;
}

void drawArrow(IplImage* img,CvPoint pt1,CvPoint pt2)
{   
	double alpha_angle;//弧度
	CvScalar line_color;
	line_color = CV_RGB(0,0,255);

	cvLine(img,pt1,pt2,CV_RGB(0,0,255),1,8,0);//起点和终点间画直线


	float xx=pt2.x-pt1.x;
	float yy=pt2.y-pt1.y;

	if( xx == 0 && yy > 0 )
		alpha_angle = PI/2;
	else if ( xx == 0 && yy < 0 )
		alpha_angle = PI*3/2;
	else
		alpha_angle = abs(atan(yy/xx));

	//调整角度
	if(xx<0 && yy>0) alpha_angle = PI - alpha_angle ;
	if(xx<0 && yy<0) alpha_angle = PI + alpha_angle ;
	if(xx>0 && yy<0) alpha_angle = 2*PI - alpha_angle ;


	CvPoint p;//pt2是箭头的尖角顶点，而p是箭头两边的点。箭头是由p-pt2和pt2-p的两条线段（p赋了两次不同的值，前后不是一个点），以及pt1-pt2构成的。总共是三条线段。
	p.x = (int) (pt2.x + 6 * cos(alpha_angle - PI / 4*3));
	p.y = (int) (pt2.y + 6 * sin(alpha_angle - PI / 4*3));
	cvLine( img, p, pt2, line_color, 1, CV_AA, 0 );//p-pt2线段1
	p.x = (int) (pt2.x + 6 * cos(alpha_angle + PI / 4*3));
	p.y = (int) (pt2.y + 6 * sin(alpha_angle + PI / 4*3));
	cvLine( img, p, pt2, line_color, 1, CV_AA, 0 );//p-pt2线段2
}

bool checkNiXing(CvPoint sp1,CvPoint sp2)
{
	double a[2]={(pt2.x-pt1.x)*1.0,(pt2.y-pt1.y)*1.0},
		   b[2]={(sp2.x-sp1.x)*1.0,(sp2.y-sp1.y)*1.0};
	double ab,a1,b1,cosr;

	ab = a[0] * b[0] + a[1] * b[1];
	a1 = sqrt(a[0] * a[0] + a[1] * a[1]);
	b1 = sqrt(b[0] * b[0] + b[1] * b[1]);
	cosr = ab / a1 / b1;
	if(acos(cosr) * 180 / PI > angleWithDirection)
		return true;
	else
		return false;
}

void CalStraitLine(CvPoint* pt,int start,int end,double& a, double& b)
{
	double sum_x = 0,sum_y = 0,sum_xy = 0,sum_xx = 0;
	int N = end - start;

	for(int i = start;i < end;i ++)
	{
	/*	if ( pt[i].x - pt[i-1].x > 3)
		{
			pt[i].x = pt[i-1].x + 1;
		}
		else if ( pt[i].x - pt[i-1].x < -3)
		{
			pt[i].x = pt[i-1].x - 1;
		}*/

		sum_x += pt[i].x;
		sum_y += pt[i].y;
		sum_xy += pt[i].x * pt[i].y;
		sum_xx += pt[i].x * pt[i].x;
	}

	a = ( sum_xx * sum_y - sum_x * sum_xy ) / ( N * sum_xx - sum_x * sum_x );
	b = ( N * sum_xy - sum_x * sum_y) / ( N * sum_xx - sum_x * sum_x );
}


