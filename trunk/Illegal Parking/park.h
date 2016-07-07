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
#define MAX_X_STEP  10	//match����
#define MAX_Y_STEP  10	//match����
#define PI 3.1415926
#define angleWithDirection 90 

#define Max_aspect_ratio 1.9
#define Min_aspect_ratio 0.7
#define Min_KeyPointSize   20

#define Frame_Diff  8
#define Frame_Slow  10
#define objDetRectNumber 10


int nFrmNum = 0;//��¼֡��
int Width;//��¼֡�Ŀ��
int Height;//��¼֡�ĸ߶�
int FrameRate;//��¼��Ƶ֡��
double aspect_ratio;//�����߱�
int rect_area;//�������
int area_threhold; //���������ֵ

//�������������
int t=0;
CvPoint pt1 = cvPoint(100,100),
		pt2 = cvPoint(120,300); 
CvPoint roiPt1, roiPt2;
CvPoint tipCord = cvPoint(20,50);

bool match(CvRect rect1, CvRect rect2)	//ƥ���⵽������͸�����������ƶȣ�ͨ�����ĵ��ƶ��������ж�
{
	//���������ƶ�����
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

bool stop_detection(CvPoint pt1, CvPoint pt2)	//ֹͣ���
{
	//���������ƶ�����
	if (abs(pt1.x-pt2.x )< 2 && abs(pt1.y-pt2.y) < 2)
		return true;
	else
		return false;
}

void drawArrow(IplImage* img,CvPoint pt1,CvPoint pt2)
{   
	double alpha_angle;//����
	CvScalar line_color;
	line_color = CV_RGB(0,0,255);

	cvLine(img,pt1,pt2,CV_RGB(0,0,255),1,8,0);//�����յ�仭ֱ��


	float xx=pt2.x-pt1.x;
	float yy=pt2.y-pt1.y;

	if( xx == 0 && yy > 0 )
		alpha_angle = PI/2;
	else if ( xx == 0 && yy < 0 )
		alpha_angle = PI*3/2;
	else
		alpha_angle = abs(atan(yy/xx));

	//�����Ƕ�
	if(xx<0 && yy>0) alpha_angle = PI - alpha_angle ;
	if(xx<0 && yy<0) alpha_angle = PI + alpha_angle ;
	if(xx>0 && yy<0) alpha_angle = 2*PI - alpha_angle ;


	CvPoint p;//pt2�Ǽ�ͷ�ļ�Ƕ��㣬��p�Ǽ�ͷ���ߵĵ㡣��ͷ����p-pt2��pt2-p�������߶Σ�p�������β�ͬ��ֵ��ǰ����һ���㣩���Լ�pt1-pt2���ɵġ��ܹ��������߶Ρ�
	p.x = (int) (pt2.x + 6 * cos(alpha_angle - PI / 4*3));
	p.y = (int) (pt2.y + 6 * sin(alpha_angle - PI / 4*3));
	cvLine( img, p, pt2, line_color, 1, CV_AA, 0 );//p-pt2�߶�1
	p.x = (int) (pt2.x + 6 * cos(alpha_angle + PI / 4*3));
	p.y = (int) (pt2.y + 6 * sin(alpha_angle + PI / 4*3));
	cvLine( img, p, pt2, line_color, 1, CV_AA, 0 );//p-pt2�߶�2
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


