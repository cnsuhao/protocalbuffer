//#include "StdAfx.h"

#include "StdAfx.h"
#include "Foreground_Model.h"
//#include "stdafx.h"
#include <math.h>

using namespace std;
using namespace cv;

static int c_xoff[9] = {-1,  0,  1, -1, 1, -1, 0, 1, 0};//x���ھӵ�
static int c_yoff[9] = {-1,  0,  1, -1, 1, -1, 0, 1, 0};//y���ھӵ�

float samples[1600][2048][defaultNbSamples+1];//����ÿ�����ص������ֵ
RNG rng(0xFFFFFFFF);//���������

Foreground_Model::Foreground_Model(void)
{
	pAfter=NULL;
	pAfterMat=NULL;//����pFrame��Ӧ�ĻҶ�ͼ��
	segMat=NULL;//���洦������Ϣ
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

	cvSmooth(src,src,CV_MEDIAN,3,0,0,0);//11.11��������Ժ󲻴�
			
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
		//ԭʼͼ��ĻҶ�ͼ
		pAfter=cvCreateImage(cvSize(src->width, src->height), 
			IPL_DEPTH_8U,1);
		// ԭʼͼ��Ҷ�ͼ����
		pAfterMat=cvCreateMat(src->height, src->width, CV_32FC1);

		//ת���ɵ�ͨ��ͼ���ٴ���
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
	
		update(pAfterMat, segMat, rng);//���±���
		
		cvConvert(segMat, dst);

	//	waitKey(1) ;
		//�Եõ��Ķ�ֵͼ�������̬ѧ����
		cvDilate(dst, dst, 0, 2);
		
		//cvMorphologyEx();
		cvErode(dst, dst, 0, 1);

	}

}
/************************************************************************/
//��ʼ��                                                                     */
/************************************************************************/
void Foreground_Model::Initialize(CvMat *pFrameMat, RNG rng)
{

	//��¼������ɵ� ��(r) �� ��(c)
	int rand, r, c;

	//��ÿ�������������г�ʼ��
	for(int y = 0; y < pFrameMat->rows; y++)//Height
	{
		for(int x = 0; x < pFrameMat->cols; x++)//Width
		{
			for(int k = 0; k < defaultNbSamples; k++)
			{
				//�����ȡ��������ֵ
				rand = rng.uniform( 0, 9 );
				//��
				r = y + c_yoff[rand];
				if(r < 0) 
				{
					r = 0; 
				}
				if(r >= pFrameMat->rows)
				{
					r = pFrameMat->rows - 1;
				}
				//��
				c = x + c_xoff[rand];
				if(c < 0) 
				{
					c = 0; 
				}
				if(c >= pFrameMat->cols) 
				{
					c = pFrameMat->cols - 1;
				}
				//�洢��������ֵ
				
				samples[y][x][k] = CV_MAT_ELEM(*pFrameMat, float, r, c);
			}
			samples[y][x][defaultNbSamples] = 0;
		}
	}
}


/************************************************************************/
/* //���º���                                                                     */
/************************************************************************/
void Foreground_Model::update(CvMat* pFrameMat, CvMat* segMat, RNG rng)
{

	for(int y = 0; y < pFrameMat->rows; y++)//Height
	{	
		for(int x = 0;x < pFrameMat->cols; x++)//Width
		{	

			//�����ж�һ�����Ƿ��Ǳ�����,index��¼�ѱȽϵ�����������count��ʾƥ�����������
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

				//�ж�Ϊ��������,ֻ�б�������ܱ����������͸��´洢����ֵ
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
				//�ж�Ϊǰ������
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
