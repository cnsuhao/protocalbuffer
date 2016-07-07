
//#include <vld.h>
#include "Precise_Detect.h"
#include "vld.h"

static CvMemStorage* storage = 0;
static CvHaarClassifierCascade* cascade = 0;
static CvHaarClassifierCascade* cascade1 = 0;
static CvHaarClassifierCascade* cascade2 = 0;

using namespace cv;
int min(int a, int b, int c)
{
	int min = a;
	if(b<min)
		min = b;
	if(c<min)
		min = c;
	return min;	
}
/**
	**函数功能说明：
	**利用OTSU法得到二值化所需阈值
	**参数说明：
	**_src:需要得到阈值的图
*/
int getThreshVal_Otsu_8u( IplImage* _src )
{
	int width = _src->width;
	int height = _src->height;
    const int N = 256;
    int i, j, h[N] = {0};
    for( i = 0; i < height; i++ )
    {
        for( j = 0; j <= width - 4; j += 4 )
        {
            int v0 = CV_IMAGE_ELEM(_src, uchar, i, j), v1 = CV_IMAGE_ELEM(_src, uchar, i, j+1);
            h[v0]++; h[v1]++;
            v0 = CV_IMAGE_ELEM(_src, uchar, i, j+2); v1 = CV_IMAGE_ELEM(_src, uchar, i, j+3);
            h[v0]++; h[v1]++;
        }
        for( ; j < width; j++ )
            h[CV_IMAGE_ELEM(_src, uchar, i, j)]++;
    }

    double mu = 0, scale = 1./(width*height);
    for( i = 0; i < N; i++ )
        mu += i*(double)h[i];
    
    mu *= scale;
    double mu1 = 0, q1 = 0;
    double max_sigma = 0, max_val = 0;

    for( i = 0; i < N; i++ )
    {
        double p_i, q2, mu2, sigma;

        p_i = h[i]*scale;
        mu1 *= q1;
        q1 += p_i;
        q2 = 1. - q1;

        if( std::min(q1,q2) < FLT_EPSILON || std::max(q1,q2) > 1. - FLT_EPSILON )
            continue;

        mu1 = (mu1 + i*p_i)/q1;
        mu2 = (mu - q1*mu1)/q2;
        sigma = q1*q2*(mu1 - mu2)*(mu1 - mu2);
        if( sigma > max_sigma )
        {
            max_sigma = sigma;
            max_val = i;
        }
    }
    return max_val;
}
/**
	**函数功能说明：
	**计算车牌相似度时所用的边缘检测函数
	**参数说明：
	**img:候选车牌图
*/
IplImage* myCanny(IplImage* img)
{
	float verticalMatrix[] = {
            -1,0,1,
            -2,0,2,
            -1,0,1,
    };
	CvMat M_vertical = cvMat(3, 3, CV_32F, verticalMatrix);
	IplImage* V_img = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);
	cvFilter2D(img, V_img, &M_vertical);
	int hist[256] = {0};
	int sum = 0;
	for(int i=0; i<img->width; i++)
	{
		for(int j=0; j<img->height; j++)
		{
			hist[CV_IMAGE_ELEM(V_img, uchar, j, i)]++;
			sum++;
		}
	}
	int strong_edge_threshold = 0;
	for(int i=0; i<=255; i++)
	{
		strong_edge_threshold += hist[i];
		if(strong_edge_threshold > 0.8*sum)
		{
			strong_edge_threshold = i;
			break;
		}
	}
	int weak_edge_threshold = strong_edge_threshold*0.4;
	IplImage* canny = cvCreateImage(cvGetSize(img), img->depth, 1);
	cvSmooth(img, img);
	cvCanny(V_img, canny, weak_edge_threshold, strong_edge_threshold);
	cvReleaseImage(&V_img);
	return canny;
}
/**
	**函数功能说明：
	**计算由myCanny得到的边缘图的平均跳变数
	**参数说明：
	**_src:由myCanny得到的边缘图
*/
double candidate_plate_filter(IplImage* img)
{
	double num = 0;
	int h = img->height/**0.5*/ - img->height*0.125;
	for(int i=0; i<6&&h>=0; i++)
	//for(int i=0; i<img->height-1; i++)
	{
		for(int j=0; j<img->width-1; j++)
		{
			if(CV_IMAGE_ELEM(img, uchar, h, j) != CV_IMAGE_ELEM(img, uchar, h, j+1))
			{
				num++;
			}
		}
		h =h-(img->height*0.125);
	}
	num = (double)num / (double)6;	
	return num;
}



