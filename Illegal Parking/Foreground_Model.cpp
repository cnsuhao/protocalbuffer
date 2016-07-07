//#include "StdAfx.h"

#include "StdAfx.h"
#include "Foreground_Model.h"
//#include "stdafx.h"
#include <math.h>

using namespace std;
using namespace cv;

static int c_xoff[9] = {-1,  0,  1, -1, 1, -1, 0, 1, 0};//x的邻居点
static int c_yoff[9] = {-1,  0,  1, -1, 1, -1, 0, 1, 0};//y的邻居点

float samples[1600][2048][defaultNbSamples+1];//保存每个像素点的样本值
RNG rng(0xFFFFFFFF);//随机生成器

Foreground_Model::Foreground_Model(void)
{
	pAfter=NULL;
	pAfterMat=NULL;//保存pFrame对应的灰度图像
	segMat=NULL;//保存处理后的信息
	nFrmNum=0;
}

Foreground_Model::~Foreground_Model(void)
{
	if (pAfter!=NULL)
	{
		cvReleaseImage(&pAfter);
		pAfter=NULL;
	}
	if (pAfterMat!=NULL)
	{
		cvReleaseMat(&pAfterMat);
		pAfterMat=NULL;
	}

	if (segMat!=NULL)
	{
		cvReleaseMat(&segMat);
		segMat=NULL;
	}
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
void Foreground_Model::ViBe_Model(IplImage* src,IplImage* dst , bool &bReset)
{

int i ,k=0 ;
if (bReset == true )
{
	nFrmNum = 0 ;
	bReset =false ;
}
nFrmNum++;

	cvSmooth(src,src,CV_MEDIAN,3,0,0,0);//11.11这个加入以后不错啊
			
	if(nFrmNum==1)
	{

		if (pAfter!=NULL)
		{
			cvReleaseImage(&pAfter);
			pAfter=NULL;
		}
		if (pAfterMat!=NULL)
		{
			cvReleaseMat(&pAfterMat);
			pAfterMat=NULL;
		}

		if (segMat!=NULL)
		{
			cvReleaseMat(&segMat);
			segMat=NULL;
		}
		segMat = cvCreateMat(src->height, src->width, CV_32FC1);
		//原始图像的灰度图
		pAfter=cvCreateImage(cvSize(src->width, src->height), 
			IPL_DEPTH_8U,1);
		// 原始图像灰度图矩阵
		pAfterMat=cvCreateMat(src->height, src->width, CV_32FC1);

		//转化成单通道图像再处理
		cvCvtColor(src, pAfter, CV_BGR2GRAY);
		cvConvert(pAfter, pAfterMat);

		Initialize(pAfterMat,rng);
	}
	else
	{
		if (src->width!=pAfter->width || src->height!=pAfter->height)
		{
			nFrmNum = 0 ;
			return ;
		}
	
		cvCvtColor(src ,pAfter, CV_BGR2GRAY);
		cvConvert(pAfter, pAfterMat);
	
		update(pAfterMat, segMat, rng);//更新背景
		
		cvConvert(segMat, dst);

	//	waitKey(1) ;
		//对得到的二值图像进行形态学操作
		cvDilate(dst, dst, 0, 2);
		
		//cvMorphologyEx();
		cvErode(dst, dst, 0, 1);

	}

}
/************************************************************************/
//初始化                                                                     */
/************************************************************************/
void Foreground_Model::Initialize(CvMat *pFrameMat, RNG rng)
{

	//记录随机生成的 行(r) 和 列(c)
	int rand, r, c;

	//对每个像素样本进行初始化
	for(int y = 0; y < pFrameMat->rows; y++)//Height
	{
		for(int x = 0; x < pFrameMat->cols; x++)//Width
		{
			for(int k = 0; k < defaultNbSamples; k++)
			{
				//随机获取像素样本值
				rand = rng.uniform( 0, 9 );
				//行
				r = y + c_yoff[rand];
				if(r < 0) 
				{
					r = 0; 
				}
				if(r >= pFrameMat->rows)
				{
					r = pFrameMat->rows - 1;
				}
				//列
				c = x + c_xoff[rand];
				if(c < 0) 
				{
					c = 0; 
				}
				if(c >= pFrameMat->cols) 
				{
					c = pFrameMat->cols - 1;
				}
				//存储像素样本值
				
				samples[y][x][k] = CV_MAT_ELEM(*pFrameMat, float, r, c);
			}
			samples[y][x][defaultNbSamples] = 0;
		}
	}
}


/************************************************************************/
/* //更新函数                                                                     */
/************************************************************************/
void Foreground_Model::update(CvMat* pFrameMat, CvMat* segMat, RNG rng)
{

	for(int y = 0; y < pFrameMat->rows; y++)//Height
	{	
		for(int x = 0;x < pFrameMat->cols; x++)//Width
		{	

			//用于判断一个点是否是背景点,index记录已比较的样本个数，count表示匹配的样本个数
			int count = 0;
			int index = 0;

			float dist = 0;
			
			while((count<defaultReqMatches) && (index<defaultNbSamples))
			{
				dist=CV_MAT_ELEM(*pFrameMat,float,y,x)-samples[y][x][index];
				if(dist<0) 
				{
					dist=-dist;
				}
				if(dist<defaultRadius)
				{	
					count++;
				}
				index++;
			}
			if(count>=defaultReqMatches)
			{

				//判断为背景像素,只有背景点才能被用来传播和更新存储样本值
				samples[y][x][defaultNbSamples]=0;

				*((float *)CV_MAT_ELEM_PTR(*segMat,y,x))=background;

				int rand=rng.uniform(0,defaultSubsamplingFactor);
				if(rand==0)
				{
					rand=rng.uniform(0,defaultNbSamples);
					samples[y][x][rand]=CV_MAT_ELEM(*pFrameMat,float,y,x);
				}
				rand=rng.uniform(0,defaultSubsamplingFactor);
				if(rand==0)
				{
					int xN,yN;
					rand=rng.uniform(0,9);
					yN=y+c_yoff[rand];
					if(yN<0) 
					{
						yN=0; 
					}
					if(yN>=pFrameMat->rows)
					{
						yN=pFrameMat->rows-1;
					}
					rand=rng.uniform(0,9);
					xN=x+c_xoff[rand];
					if(xN<0) 
					{
						xN=0; 
					}
					if(xN>=pFrameMat->cols) 
					{
						xN=pFrameMat->cols-1;
					}
					rand=rng.uniform(0,defaultNbSamples);
					samples[yN][xN][rand]=CV_MAT_ELEM(*pFrameMat,float,y,x);
				} 
			}
			else 
			{
				//判断为前景像素
				*((float *)CV_MAT_ELEM_PTR(*segMat,y,x))=foreground;

				samples[y][x][defaultNbSamples]++;
				if(samples[y][x][defaultNbSamples]>50)
				{
					int rand=rng.uniform(0,defaultNbSamples);
					if(rand==0)
					{
						rand=rng.uniform(0,defaultNbSamples);
						samples[y][x][rand]=CV_MAT_ELEM(*pFrameMat,float,y,x);
					}
				}
			}

		}
	}

}
