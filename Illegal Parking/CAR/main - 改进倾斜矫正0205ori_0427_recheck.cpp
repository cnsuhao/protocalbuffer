
#include "Precise_Detect.h"
#include <vector>
#include <string>
#include <sstream>
#include <windows.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/ml/ml.hpp> 
#include <algorithm>
#include<math.h>
//#include "vld.h"

using namespace std;
using namespace cv;
int WIDTH=800;
int HEIGHT=600;
#define IMG_WID				16
#define IMG_HEI				32
#define LEN                 1024
int  size=1;
bool isout=0;
int  cor=0;
//float scalew=0.5;
float scalew=1;
float scaleh=1;
static CvMemStorage* storage = 0;
static CvHaarClassifierCascade* cascade = 0;
//static CvHaarClassifierCascade* cascade1 = 0;
CascadeClassifier cascade1;
CascadeClassifier cascade2;
CascadeClassifier cascade3;
extern const char* cascade_name = "data/cascade_final_14.xml";//"data/cascade_final_14.xml";
extern const char* cascade_name1 = "data/cascade_double.xml";
extern const char* cascade_name2 = "data/cascade_new.xml";
extern const char* cascade_name3 = "data/cascade_6000_lbp.xml";

bool IniRead( char strfind[] ,char strget[]	)
{	
	FILE *fp = fopen("run.ini","r");
	if (fp == NULL)
	{
		return 0 ;
	}
	char  strnum[256] ;
	memset(strget,0,sizeof(strget));
	memset(strnum,0,sizeof(strnum));

	while(!feof(fp))
	{		
		char str[256] ;
		fgets(str ,256 ,fp);	
		if(	strstr(	str, strfind )	)
		{
			int aa,bb,j=0;
			bool givestr = false;
			aa = strlen(str);
			bb = strlen(strfind);
			for (int i = 0; i<aa-bb;i++)
			{	
				if (str[i+bb]==61)
				{	givestr = true ;
				continue;
				}
				if (str[i+bb]==32)
				{
					continue;
				}

				if (str[i+bb]==10)
				{
					continue;
				}
				if (givestr ==true)
				{	
					strnum[j] =	str[i+bb] ;
					j++ ;
				}
			}
			strcpy(strget , strnum);
			fclose(fp);
			return 1 ;	
		}	

	}
	fclose(fp);
}




/**
	**函数功能说明：
	**由AdaptiveFindThreshold调用
	**参数说明：
	**dx:x方向Sobel梯度图
	**dy:y方向Sobel梯度图
	**low:canny边缘检测下阈值指针
	**high:canny边缘检测上阈值指针
*/
void _AdaptiveFindThreshold(CvMat *dx, CvMat *dy, double *low, double *high)   
{                                                                              
	CvSize size;                                                           
	IplImage* imge=0;                                                      
	int i,j;                                                               
	CvHistogram *hist;                                                     
	int hist_size = 255;                                                   
	float range_0[]={0,256};                                               
	float* ranges[] = { range_0 };                                         
	double PercentOfPixelsNotEdges = 0.7;                                  
	size = cvGetSize(dx);                                                  
	imge = cvCreateImage(size, IPL_DEPTH_32F, 1);                          
	// 计算边缘的强度, 并存于图像中                                        
	float maxv = 0;                                                        
	for(i = 0; i < size.height; i++ )                                      
	{                                                                      
		const short* _dx = (short*)(dx->data.ptr + dx->step*i);        
		const short* _dy = (short*)(dy->data.ptr + dy->step*i);        
		float* _image = (float *)(imge->imageData + imge->widthStep*i);
		for(j = 0; j < size.width; j++)                                
		{                                                              
			_image[j] = (float)(abs(_dx[j]) + abs(_dy[j]));        
			maxv = maxv < _image[j] ? _image[j]: maxv;             
	                                                                       
		}                                                              
	}                                                                      
	if(maxv == 0){                                                         
		*high = 0;                                                     
		*low = 0;                                                      
		cvReleaseImage( &imge );                                       
		return;                                                        
	}  
		// 计算直方图                                                          
	range_0[1] = maxv;                                                     
	hist_size = (int)(hist_size > maxv ? maxv:hist_size);                  
	hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);          
	cvCalcHist( &imge, hist, 0, NULL );                                    
	int total = (int)(size.height * size.width * PercentOfPixelsNotEdges); 
	float sum=0;                                                           
	int icount = hist->mat.dim[0].size;                                    
                                                                               
	float *h = (float*)cvPtr1D( hist->bins, 0 );                           
	for(i = 0; i < icount; i++)                                            
	{                                                                      
		sum += h[i];                                                   
		if( sum > total )                                              
			break;                                                 
	}                                                                      
	// 计算高低门限                                                        
	*high = (i+1) * maxv / hist_size ;                                     
	*low = *high * 0.4;                                                    
	cvReleaseImage( &imge );                                               
	cvReleaseHist(&hist);                                                  
}        
/**
	**函数功能说明：
	**得出canny边缘检测上下阈值
	**参数说明：
	**image:待检测边缘的图
	**low:canny边缘检测下阈值指针
	**high:canny边缘检测上阈值指针
	**aperture_size:Sobel算子核大小
*/
void AdaptiveFindThreshold(const CvArr* image, double *low, double *high, int aperture_size=3)
{                                                                              
	Mat src = cvarrToMat(image);                                   
	const int cn = src.channels();                                         
	Mat dx(src.rows, src.cols, CV_16SC(cn));                           
	Mat dy(src.rows, src.cols, CV_16SC(cn));                           
                                                                               
	Sobel(src, dx, CV_16S, 1, 0, aperture_size, 1, 0, BORDER_DEFAULT);
	Sobel(src, dy, CV_16S, 0, 1, aperture_size, 1, 0, BORDER_DEFAULT);
                                                                               
	CvMat _dx = dx, _dy = dy;                                              
	_AdaptiveFindThreshold(&_dx, &_dy, low, high);                         
                                                                               
}   
/**
	**函数功能说明：
	**提取字块图像的HOG特征
	**参数说明：
	**imgSrc:待计算的图
	**feat:计算出的特征向量
*/
void calcHOGFeat(const Mat& imgSrc, vector<float>& feat)
{
	Mat image;
	resize(imgSrc, image, Size(IMG_WID, IMG_HEI));
	
	HOGDescriptor hog(Size(IMG_WID, IMG_HEI), Size(16, 16), Size(8, 8), Size(8, 8), 9);
	int hogFeatureLen = 36 * (IMG_WID / 8 - 1)*(IMG_HEI / 8 - 1);
	hog.compute(image, feat, cv::Size(8, 8));

}
/**
	**函数功能说明：
	**提取字块图像的xy方向梯度特征
	**参数说明：
	**imgSrc:待计算的图
	**feat:计算出的特征向量
*/
void calcGradientFeat1(const Mat& imgSrc, vector<float>& feat) 
{ 
	float sumMatValue(const Mat& image); // 计算图像中像素灰度值总和 
 
	Mat image; 
//	cvtColor(imgSrc,image,CV_BGR2GRAY); 
	resize(imgSrc,image,Size(16,32)); 
 
// 计算x方向和y方向上的滤波 
	float mask[3][3] = { { 1, 2, 1 }, { 0, 0, 0 }, { -1, -2, -1 } };

	Mat y_mask = Mat(3, 3, CV_32F, mask) / 8; 
	Mat x_mask = y_mask.t(); // 转置 
	Mat sobelX, sobelY;

	filter2D(image, sobelX, CV_32F, x_mask); 
	filter2D(image, sobelY, CV_32F, y_mask);

	sobelX = abs(sobelX); 
	sobelY = abs(sobelY);

	float totleValueX = sumMatValue(sobelX); 
	float totleValueY = sumMatValue(sobelY);

// 将图像划分为4*2共8个格子，计算每个格子里灰度值总和的百分比 
	for (int i = 0; i < image.rows; i = i + 4) 
	{ 
		for (int j = 0; j < image.cols; j = j + 4) 
		{ 
			Mat subImageX = sobelX(Rect(j, i, 4, 4)); 
			feat.push_back(sumMatValue(subImageX) / totleValueX); 
			Mat subImageY= sobelY(Rect(j, i, 4, 4)); 
			feat.push_back(sumMatValue(subImageY) / totleValueY); 
		} 
	} 
} 
/**
	**函数功能说明：
	**提取字块图像的xy+两对角线方向梯度特征
	**参数说明：
	**imgSrc:待计算的图
	**feat:计算出的特征向量
*/
void calcGradientFeat2(const Mat& imgSrc, vector<float>& feat) 
{ 
	float sumMatValue(const Mat& image); // 计算图像中像素灰度值总和 
 
	Mat image; 
//	cvtColor(imgSrc,image,CV_BGR2GRAY); 
	resize(imgSrc,image,Size(16,32)); 
 
// 计算x方向和y方向上的滤波 
	float mask[3][3] = { { 1, 2, 1 }, { 0, 0, 0 }, { -1, -2, -1 } };
	float qxMatrix[] = {
            0,-1,2,
            -1,0,1,
            -2,1,0,
    };
	float qx1Matrix[] = {
            -2,-1,0,
            -1,0,1,
            0,1,2,
    };
	Mat y_mask = Mat(3, 3, CV_32F, mask) / 8; 
	Mat xy_mask = Mat(3, 3, CV_32F, qxMatrix) / 8; 
	Mat yx_mask = Mat(3, 3, CV_32F, qx1Matrix) / 8; 
	Mat x_mask = y_mask.t(); // 转置 
	Mat sobelX, sobelY,sobelXY, sobelYX;

	filter2D(image, sobelX, CV_32F, x_mask); 
	filter2D(image, sobelY, CV_32F, y_mask);
	filter2D(image, sobelXY, CV_32F, xy_mask); 
	filter2D(image, sobelYX, CV_32F, yx_mask);
	sobelX = abs(sobelX); 
	sobelY = abs(sobelY);
	sobelXY = abs(sobelXY); 
	sobelYX = abs(sobelYX);
	float totleValueX = sumMatValue(sobelX); 
	float totleValueY = sumMatValue(sobelY);
	float totleValueXY = sumMatValue(sobelXY); 
	float totleValueYX = sumMatValue(sobelYX);
// 将图像划分为4*2共8个格子，计算每个格子里灰度值总和的百分比 
	for (int i = 0; i < image.rows; i = i + 4) 
	{ 
		for (int j = 0; j < image.cols; j = j + 4) 
		{ 
			Mat subImageX = sobelX(Rect(j, i, 4, 4)); 
			feat.push_back(sumMatValue(subImageX) / totleValueX); 
			Mat subImageY= sobelY(Rect(j, i, 4, 4)); 
			feat.push_back(sumMatValue(subImageY) / totleValueY); 
			Mat subImageXY = sobelXY(Rect(j, i, 4, 4)); 
			feat.push_back(sumMatValue(subImageXY) / totleValueXY); 
			Mat subImageYX= sobelYX(Rect(j, i, 4, 4)); 
			feat.push_back(sumMatValue(subImageYX) / totleValueYX); 
		} 
	} 
} 
/**
	**函数功能说明：
	**由计算梯度特征的函数调用，计算图像像素值总和
	**参数说明：
	**image:输入图像
*/
float sumMatValue(const Mat& image) 
{ 
	float sumValue = 0; 
	int r = image.rows; 
	int c = image.cols; 
	if (image.isContinuous()) 
	{ 
		 c = r*c; 
		 r = 1;    
	} 
	for (int i = 0; i < r; i++) 
	{ 
		const uchar* linePtr = image.ptr<uchar>(i); 
		for (int j = 0; j < c; j++) 
		{ 
			sumValue += linePtr[j]; 
		} 
	} 
	 return sumValue; 
}
/**
	**函数功能说明：
	**提取字块图像非归一化灰度特征
	**参数说明：
	**imgSrc:待计算的图
	**feat:计算出的特征向量
*/
void calcGrayFeat1(const Mat& imgSrc, vector<float>& feat) 
{
	Mat image; 
	//cvtColor(imgSrc,image,CV_BGR2GRAY); 
	resize(imgSrc,image,Size(8,16)); 
	//resize(imgSrc,image,Size(6,12)); 
	for (int i = 0; i < image.rows; i++) 
	{ 
		for (int j = 0; j < image.cols; j++) 
		{ 
			feat.push_back((int)image.at<uchar>(i,j));
			//feat.push_back((float)image.at<uchar>(i,j)/(float)255);
		}
	}
}
/**
	**函数功能说明：
	**提取字块图像归一化灰度特征
	**参数说明：
	**imgSrc:待计算的图
	**feat:计算出的特征向量
*/
void calcGrayFeat3(const Mat& imgSrc, vector<float>& feat) 
{
	Mat image; 
	//cvtColor(imgSrc,image,CV_BGR2GRAY); 
	resize(imgSrc,image,Size(8,16)); 
	//resize(imgSrc,image,Size(6,12)); 
	for (int i = 0; i < image.rows; i++) 
	{ 
		for (int j = 0; j < image.cols; j++) 
		{ 
			//feat.push_back((int)image.at<uchar>(i,j));
			feat.push_back((float)image.at<uchar>(i,j)/(float)255);
		}
	}
}

double cmp(double a,double b)
{
    return a>b;
}

/**
	**函数功能说明：
	**评判粗定位后图像的车牌相似度，根据图像Lab空间b通道图像特性有筛选
	**参数说明：
	**plate:车牌候选序列
	**gray：灰度后的图
	**cannyval:视频号
	**resize：原图
	**count：处理图号
	**nn：调试用，调试图号
	**graypic：是否是灰度图
*/
double realplate(CvSeq* plate,Mat gray,double *&cannyval,Mat resize,  vector<Rect> &vectPositios ,int count,int &nn,bool graypic    )
{
	double max=0;
	
	//cannyval=(double *)malloc(sizeof(double)*plate->total);
	int width=gray.cols;
	int height=gray.rows;
	int centx=0;
	int centy=0;
	for(int i=0; i<(plate? plate->total: 0); i++)
	{
		Rect* r = (Rect*) cvGetSeqElem(plate, i);
		centx = r->x+r->width/2;
		centy = r->y+r->height/2;
		Rect r1;
		r1.x=r->x;
		r1.y=r->y;
		r1.width=r->width;
		r1.height=r->height;
		if(r1.x - r1.width*0.125 > 0)
			r1.x = r1.x - r1.width*0.125;
		else
			r1.x = 0;
		if(r1.y - r1.height*0.125 > 0)
			r1.y = r1.y - r1.height*0.125;
		else
			r1.y = 0;
		if(r1.x + r1.width + r1.width*0.25 < width)
			r1.width = r1.width + r1.width*0.25;
		else
			r1.width = width - r1.x;
		//if(r->y + r->height + r->height< height)
		if(r1.y + 1.25*r1.height < height)
			r1.height = r1.height * 1.25;
		else
			r1.height = height - r1.y;
		

		Mat CandidatePlate = resize(r1);
		Mat CandidatePlate_gray = gray(*r);
		Mat CandidatePlate_thresh;

	//	imshow("aa", CandidatePlate );
		//imshow("bb", CandidatePlate_gray );
		//waitKey(1);
		char sName[200]={0};

		if(!graypic)
		{
		//===================取候选区域Lab颜色空间中的b通道========================//
	    //============若图像没有灰度突出部分，则非车牌候选区域，删除出plate序列===//
			Mat Lab,L,a,b;
			cvtColor(CandidatePlate,Lab,CV_BGR2Lab);
			vector<Mat> channels; 
			split(Lab,channels);
			L = channels[0];
			a = channels[1];
			b = channels[2];
		
			GaussianBlur( b, b, Size( 3, 3 ), 0, 0 ); 

			int ave=0;
			for(int p=0;p<b.rows;p++)
			{
				for(int q=0;q<b.cols;q++)
				{
					uchar s;
					s = b.at<uchar>(p,q);
					ave += s;
				}
			}
			ave = ave/(b.rows*b.cols);
			int sum=0;
			for(int p=0;p<b.rows;p++)
			{
				for(int q=0;q<b.cols;q++)
				{
					uchar s;
					s = b.at<uchar>(p,q);
					if((s > ave*1.1)||(s < ave*0.9))
					{
						s=0;
						b.at<uchar>(p,q) = s;
						sum++;
					}
					else
					{
						s=255;
						b.at<uchar>(p,q) = s;

					}
				}
			}
			if(sum<b.rows*b.cols*0.08/*&&scolorcnt!=5*/&&centy<gray.rows*0.4/*||centy<gray.rows*0.2*/||sum==0)//===========0110
			{
				cvSeqRemove(plate,i);
				i--;
				nn++;			
				continue;
			}
			
		}
	//	Rect recttemp ;


 		vectPositios.push_back(r1) ;


		IplImage *iCandidatePlate_gray = &(IplImage(CandidatePlate_gray));
		int threshval = getThreshVal_Otsu_8u(iCandidatePlate_gray);
		threshold(CandidatePlate_gray, CandidatePlate_thresh, threshval, 255, CV_THRESH_BINARY);
		IplConvKernel *elem_open_vertical = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT, 0);
		IplImage *iCandidatePlate_thresh = &(IplImage(CandidatePlate_thresh));
		IplImage *t_CandidatePlate_thresh = cvCreateImage(cvSize(CandidatePlate_thresh.cols,CandidatePlate_thresh.rows),8,1);
		cvMorphologyEx(iCandidatePlate_thresh, t_CandidatePlate_thresh, 0, elem_open_vertical, CV_MOP_OPEN, 1); //开运算：腐蚀+膨胀
		Mat tt = Mat(t_CandidatePlate_thresh);
		CandidatePlate_thresh = tt.clone();
		cvReleaseImage(&t_CandidatePlate_thresh);
		cvReleaseStructuringElement(&elem_open_vertical);
		sprintf(sName, "simple_result/%d_%d_CandidatePlate_thresh.jpg",count,i+nn);//调试用
		//imwrite(sName,CandidatePlate_thresh);
		IplImage* canny = NULL;
		IplImage *ict = &(IplImage(CandidatePlate_thresh));
		canny = myCanny(ict);
		sprintf(sName, "simple_result/%d_%d_myCanny.jpg",count,i+nn);//调试用
		//imwrite(sName,canny);	
		double n = candidate_plate_filter(canny);
		cannyval[i]=n;
		if(n>max)
		{
			max = n;
		}
		
		cvReleaseImage(&canny);
		nn++;
	}
	return max;
}
/**
	**函数功能说明：
	**评判粗定位后图像的车牌相似度，无筛选
	**参数说明：
	**plate:车牌候选序列
	**gray：灰度后的图
	**cannyval:记录候选车牌序列像车牌程度
	**resize：原彩图
	**count：处理图号
	**nn：调试用，调试图号
	**graypic：是否是灰度图
*/
double realplate_new(CvSeq* plate,Mat gray,double *&cannyval,Mat resize,int count,int &nn,bool graypic)
{
	double max=0;
	//cannyval=(double *)malloc(sizeof(double)*plate->total);
	int width=gray.cols;
	int height=gray.rows;
	int centx=0;
	int centy=0;
	for(int i=0; i<(plate? plate->total: 0); i++)
	{
		Rect* r = (Rect*) cvGetSeqElem(plate, i);
		centx = r->x+r->width/2;
		centy = r->y+r->height/2;
		Rect r1;
		r1.x=r->x;
		r1.y=r->y;
		r1.width=r->width;
		r1.height=r->height;
		if(r1.x - r1.width*0.125 > 0)
			r1.x = r1.x - r1.width*0.125;
		else
			r1.x = 0;
		if(r1.y - r1.height*0.125 > 0)
			r1.y = r1.y - r1.height*0.125;
		else
			r1.y = 0;
		if(r1.x + r1.width + r1.width*0.25 < gray.cols)
			r1.width = r1.width + r1.width*0.25;
		else
			r1.width = width - r1.x;
		if(r1.y + 1.25*r1.height < gray.rows)
			r1.height = r1.height * 1.25;
		else
			r1.height = height - r1.y;
		Mat CandidatePlate;
		Mat CandidatePlate_thresh;
		CandidatePlate = resize(r1);	
		Mat CandidatePlate_gray = gray(*r);

		char sName[200]={0};
		
		IplImage *iCandidatePlate_gray = &(IplImage(CandidatePlate_gray));
		int threshval = getThreshVal_Otsu_8u(iCandidatePlate_gray);
		threshold(CandidatePlate_gray, CandidatePlate_thresh, threshval, 255, CV_THRESH_BINARY);
		IplConvKernel *elem_open_vertical = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT, 0);
		IplImage *iCandidatePlate_thresh = &(IplImage(CandidatePlate_thresh));
		IplImage *t_CandidatePlate_thresh = cvCreateImage(cvSize(CandidatePlate_thresh.cols,CandidatePlate_thresh.rows),8,1);
		cvMorphologyEx(iCandidatePlate_thresh, t_CandidatePlate_thresh, 0, elem_open_vertical, CV_MOP_OPEN, 1); //开运算：腐蚀+膨胀
		Mat tt = Mat(t_CandidatePlate_thresh);
		CandidatePlate_thresh = tt.clone();
		cvReleaseImage(&t_CandidatePlate_thresh);
		cvReleaseStructuringElement(&elem_open_vertical);
		sprintf(sName, "simple_result/%d_%d_CandidatePlate_thresh.jpg",count,i+nn);//调试用
		//imwrite(sName,CandidatePlate_thresh);
		IplImage* canny = NULL;
		IplImage *ict = &(IplImage(CandidatePlate_thresh));
		canny = myCanny(ict);
		sprintf(sName, "simple_result/%d_%d_myCanny.jpg",count,i+nn);//调试用
		//imwrite(sName,canny);	
		double n = candidate_plate_filter(canny);
		cannyval[i]=n;
		if(n>max)
		{
			max = n;
		}
		cvReleaseImage(&canny);
		nn++;
	}
	return max;
}
/**
	**函数功能说明：
	**判断是否为灰度图
	**参数说明：
	**frame:输入图像
*/
bool graypicf(Mat frame)
{
	bool graypic=false;
	int scolorcnt=0;
	int pixcnt=0; 
	for(int i = 0;i < frame.rows;i += frame.rows/5)
	{
		uchar* data= frame.ptr<uchar>(i); 
		int nc= frame.cols * frame.channels();  
		for(int j = 0;j < nc;j += frame.channels()*frame.cols/5)
		{
			pixcnt++;					
			if(data[j] == data[j+1] && data[j+1] == data[j+2])
			{
				scolorcnt++;
			}
		}
	}
	if(scolorcnt==pixcnt)
		graypic=true;
	return graypic;
}
/**
	**函数功能说明：
	**倾斜校正前，得到边缘检测所需的已去除杂项的二值化图
	**参数说明：
	**carplate0:车牌灰度图
	**color_carplate0：车牌彩色图
	**colnum：车牌颜色
	**whitebody:车身是否为浅色
*/
Mat extractEdge(Mat carplate0,Mat color_carplate0,int colnum,bool &whitebody)
{
		char sName[100]={0};

		Mat B=Mat(color_carplate0.size(), CV_8UC1);
		Mat G=Mat(color_carplate0.size(), CV_8UC1);
		Mat R=Mat(color_carplate0.size(), CV_8UC1);
		Mat H=Mat(color_carplate0.size(), CV_8UC1);
		Mat S=Mat(color_carplate0.size(), CV_8UC1);
		Mat V=Mat(color_carplate0.size(), CV_8UC1);
		Mat hsv=Mat(color_carplate0.size(), CV_8UC3);
		Mat L=Mat(color_carplate0.size(), CV_8UC1);
		Mat a=Mat(color_carplate0.size(), CV_8UC1);
		Mat b=Mat(color_carplate0.size(), CV_8UC1);
		Mat Lab=Mat(color_carplate0.size(), CV_8UC3);
		if(colnum == 3||colnum == 2)
		{
			vector<Mat> channels; 
			split(color_carplate0,channels);  //车身为深色，分出图片中的B通道，以下对B通道进行操作
			Mat Y = Mat(color_carplate0.size(), CV_8UC3);
			B = channels.at(0);
			G = channels.at(1);
			R = channels.at(2);
			Mat bk = Mat(color_carplate0.size(), CV_8UC1);
			bk = Scalar(0); 
			channels[0]=bk;
			merge(channels,Y);			
			cvtColor( Y, B, CV_BGR2GRAY );	
		}
		else
		{
			cvtColor( color_carplate0, hsv,CV_BGR2HSV );			
			if(whitebody == 1)
			{
				vector<Mat> channels; 
				split(hsv,channels); //车身为浅色，分出图片中的S通道，以下对s通道进行操作
				B=channels.at(1);				
			}
			else
			{
				vector<Mat> channels; 
				split(color_carplate0,channels);
				B=channels.at(0);
			}
		}
		cvtColor( color_carplate0, Lab,CV_BGR2Lab );
		//分出图片中的b通道，以下对b通道进行操作
		vector<Mat> channels; 
		split(Lab,channels);
		b=channels[2];
		IplImage *iB=&(IplImage(B));
		int threshval = getThreshVal_Otsu_8u(iB);
		threshold(B, B, threshval, 255, CV_THRESH_BINARY_INV);

		//imshow("ttt",B ) ;
		//waitKey(1);

		Mat thresh0=B.clone();
		IplImage *ib=&(IplImage(b));
		
		threshval = getThreshVal_Otsu_8u(ib);		
		
		threshold(b, b, threshval, 255, CV_THRESH_BINARY_INV);

		Mat thresh00=b.clone();
		
		for(int i=0;i<thresh0.rows;i++)
		{
			for(int j=0;j<thresh0.cols;j++)
			{
				uchar s1 = thresh0.at<uchar>(i,j);
				uchar s2 = thresh00.at<uchar>(i,j);
				if(s1==s2)
				{
					s1=255;
					thresh0.at<uchar>(i,j)=s1;
				}
			}
		}
		
		
		int blanksum=0;//==================计算thresh0里空白块占多少，占太大则用b代替thresh0
		for(int i=0;i<thresh0.cols;i++)
		{
			for(int j=0;j<thresh0.rows;j++)
			{
				uchar s1=thresh0.at<uchar>(j,i);
				if(s1 == 255)
				{
					blanksum++;
				}
			}
		}
		if(blanksum>thresh0.rows*thresh0.cols*0.9)
		{
			thresh0=thresh00.clone();
		}
		
		int bbase=0;
		int colorflag=0;//0为黑底白牌，1为白底黑牌
		blanksum=0;

		for(int i=0;i<thresh0.rows;i++)
		{
			uchar s;
			s=thresh0.at<uchar>(i,0);
			if(s == 255)
				blanksum++;
		}

		if(blanksum<0.9*thresh0.rows)
			bbase++;
		blanksum=0;

		for(int i=0;i<thresh0.rows;i++)
		{
			uchar s;
			s=thresh0.at<uchar>(i,thresh0.cols-1);
			if(s == 255)
				blanksum++;
		}
		if(blanksum<0.9*thresh0.rows)
			bbase++;
		blanksum=0;
		for(int i=0;i<thresh0.cols;i++)
		{
			uchar s;
			s = thresh0.at<uchar>(thresh0.rows-1,i);
			if(s == 255)
				blanksum++;
		}
		if(blanksum<0.9*thresh0.cols)
			bbase++;
		blanksum=0;
		for(int i=0;i<thresh0.cols;i++)
		{
			uchar s;
			s=thresh0.at<uchar>(0,i);
			if(s == 255)
				blanksum++;
		}
		if(blanksum<0.9*thresh0.cols)
			bbase++;
		IplImage *ithresh0 = &(IplImage(thresh0));
		if(bbase==4)//车牌部分二值化为白色，则膨胀
		{
			dilate(thresh0,thresh0,Mat(),Point(-1,-1),2);
			colorflag=1;				//黑底白牌
		}
		else
		{
			erode(thresh0,thresh0,Mat(),Point(-1,-1),2);		
		}
		
		Mat mask=thresh0.clone();
		float part=0.4;
		
		int insert=255;
		if(colorflag==0)//白底黑牌
			insert=0;
		int sumpix=0;
		for(int i=0;i<thresh0.rows;i++)
		{
			for(int j=0;j<thresh0.cols;j++)
			{
				uchar s=mask.at<uchar>(i,j);
				if(s == insert)
					sumpix++;
			}
		}

		for(int i=0;i<thresh0.rows;i++)
		{
			uchar s=mask.at<uchar>(i,0);//上
			if(s == (insert))
			{
				Point  seed = Point (0,i);
				int lo = 0;
				int up = 0;
				int flags = 8;
				Rect comp;
				Mat temp=thresh0.clone();
				floodFill( mask,seed,Scalar(255-insert,255-insert,255-insert),
					&comp,Scalar(lo, lo, lo),Scalar(up, up, up),flags);
				int area = floodFill( thresh0,seed, Scalar(255-insert,255-insert,255-insert),
					&comp,Scalar(lo, lo, lo),Scalar(up, up, up),flags);
				if(area>part*sumpix)
				{
					thresh0=temp.clone();
				}
				else
				{
					sumpix=sumpix-area;
				}
			}
			s=mask.at<uchar>(i,thresh0.cols-1);//右
			if(s == insert)
			{
				Point seed = Point(thresh0.cols-1,i);
				int lo = 0;
				int up = 0;
				int flags = 8;
				Rect comp;
				Mat temp=thresh0.clone();
					floodFill( mask,seed,Scalar(255-insert,255-insert,255-insert),
				&comp,Scalar(lo, lo, lo),Scalar(up, up, up),flags);
				int area = floodFill( thresh0,seed, Scalar(255-insert,255-insert,255-insert),
					&comp,Scalar(lo, lo, lo),Scalar(up, up, up),flags);
				if(area>part*sumpix)
				{
					thresh0=temp.clone();
				}				
				else
				{
					sumpix=sumpix-area;
				}
			}
		}

		for(int i=0;i<thresh0.cols;i++)
		{
			uchar s=mask.at<uchar>(0,i);//左
			if(s == (insert))
			{
				Point seed = Point(i,0);
				int lo = 0;
				int up = 0;
				int flags = 8;
				Rect comp;
				Mat temp=thresh0.clone();
				floodFill( mask,seed,Scalar(255-insert,255-insert,255-insert),
					&comp,Scalar(lo, lo, lo),Scalar(up, up, up),flags);
				int area = floodFill( thresh0,seed, Scalar(255-insert,255-insert,255-insert),
					&comp,Scalar(lo, lo, lo),Scalar(up, up, up),flags);
				if(area>part*sumpix)
				{
					thresh0=temp.clone();
				}				
				else
				{
					sumpix=sumpix-area;
				}
			}
			s=mask.at<uchar>(thresh0.rows-1,i);//下
			if(s == (insert))
			{
				Point seed = Point(i,thresh0.rows-1);
				int lo = 0;
				int up = 0;
				int flags = 8;
				Rect comp;
				Mat temp=thresh0.clone();
				floodFill( mask,seed,Scalar(255-insert,255-insert,255-insert),
					&comp,Scalar(lo, lo, lo),Scalar(up, up, up),flags);
				int area = floodFill( thresh0,seed, Scalar(255-insert,255-insert,255-insert),
					&comp,Scalar(lo, lo, lo),Scalar(up, up, up),flags);
				if(area>part*sumpix)
				{
					thresh0=temp.clone();
				}				
				else
				{
					sumpix=sumpix-area;
				}
			}
		}
		part=0.1;
		for(int i=1;i<thresh0.cols-1;i++)
		{
			for(int j=1;j<thresh0.rows-1;j++)
			{
				uchar s=mask.at<uchar>(j,i);
				if(s == (insert))
				{
					Point seed = Point(i,j);
					int lo = 0;
					int up = 0;
					int flags = 8;
					Rect comp;
					Mat temp=thresh0.clone();
					floodFill( mask,seed,Scalar(255-insert,255-insert,255-insert),
						&comp,Scalar(lo, lo, lo),Scalar(up, up, up),flags);
					int area = floodFill( thresh0,seed, Scalar(255-insert,255-insert,255-insert),
						&comp,Scalar(lo, lo, lo),Scalar(up, up, up),flags);
					if(area>part*sumpix)
					{
						thresh0=temp.clone();
					}				
					else
					{
						sumpix=sumpix-area;
					}
				}
			}
		}

		
			if(colorflag==0)//白底黑牌
			{
				erode(thresh0,thresh0,Mat(),Point(-1,-1),1);
			}
		
			else
				dilate(thresh0,thresh0,Mat(),Point(-1,-1),1);
			part=0.5;//腐蚀或膨胀后再 漫水去除一次内部杂块
			if(colorflag==1)
				insert=0;
			else
				insert=255;
			mask=thresh0.clone();
			sumpix=0;
		for(int i=0;i<thresh0.rows;i++)
		{
			for(int j=0;j<thresh0.cols;j++)
			{
				uchar s=mask.at<uchar>(i,j);
				if(s == insert)
					sumpix++;
			}
		}
		for(int i=0;i<thresh0.rows;i++)
		{
			for(int j=0;j<thresh0.cols;j++)
			{
				uchar s=mask.at<uchar>(i,j);
				if(s == insert)
				{
					Point seed = Point(j,i);
					int lo = 0;
					int up = 0;
					int flags = 8;
					
					Rect comp;
					Mat temp=thresh0.clone();
					floodFill( mask,seed,Scalar(255-insert,255-insert,255-insert),
						&comp,Scalar(lo, lo, lo),Scalar(up, up, up),flags);
					int area = floodFill( thresh0,seed, Scalar(255-insert,255-insert,255-insert),
						&comp,Scalar(lo, lo, lo),Scalar(up, up, up),flags);
					if(area>part*sumpix)
					{
						thresh0=temp.clone();
					}				
					else
					{
						sumpix=sumpix-area;
					}
				}
			}
		}
		
		return thresh0;
	}
/**
	**函数功能说明：
	**得到骨架化图像
	**参数说明：
	**src:原图
	**dst：骨架化后图
*/
void rosenfeld(const Mat src, Mat& dst)
{
    Mat gray_img = Mat(src.size(), CV_8UC1);
	gray_img=src;
    // set border to 0
    for(int i = 0; i < gray_img.cols; i++)
    {
        gray_img.at<uchar>(0, i) = 0;
        gray_img.at<uchar>(gray_img.rows - 1, i) = 0;
    }
    for(int i = 0; i < gray_img.rows; i++)
    {
        gray_img.at<uchar>(i, 0) = 0;
        gray_img.at<uchar>(i, gray_img.cols - 1) = 0;
    }

    Mat erode_img;
    Mat kernel = (Mat_<uchar>(3, 3) << 255, 255, 255, 255, 255, 255, 255, 255, 255);
    erode(gray_img, erode_img, kernel, Point(-1, -1), 1);

    //imshow("erode", erode_img);
    threshold(erode_img, erode_img, 0, 255, THRESH_BINARY);
    erode_img.copyTo(dst);

    int width = gray_img.cols -1;   // avoid crossing the border
    int height = gray_img.rows -1;
    int step = gray_img.step;
    int dir[4] = {-step, step, 1, -1};

    uchar* img_data;
    bool is_end;
    Mat tmp_data;
    int  p2, p3, p4, p5, p6, p7, p8, p9;

    while(true)
    {
        is_end = false;
        for(int n = 0; n < 4; n++)
        {
            dst.copyTo(tmp_data);
            img_data = tmp_data.data;
            for(int i = 1; i < height; i++)
            {
                img_data += step;
                for(int j =1; j<width; j++)
                {
                    uchar* p = img_data + j;
                    if(p[0] == 0 || p[dir[n]] > 0)
                    {
                        continue;
                    }
                    p2 = p[-step] > 0 ? 1 : 0;
                    p3 = p[-step + 1] > 0 ? 1 : 0;
                    p4 = p[1] > 0 ? 1 : 0;
                    p5 = p[step + 1] > 0 ? 1 : 0;
                    p6 = p[step] > 0 ? 1 : 0;
                    p7 = p[step - 1] > 0 ? 1 : 0;
                    p8 = p[-1] > 0 ? 1 : 0;
                    p9 = p[-step -1 ]>0 ? 1 : 0;

                    int is_8_simple = 1;
                    if(p2 == 0 && p6 == 0)
                    {
                        if((p9 == 1 || p8 == 1 || p7 == 1) && (p3 == 1 || p4 == 1 || p5 == 1))
                        {
                            is_8_simple = 0;
                        }
                    }
                    if(p4 == 0 && p8 == 0)
                    {
                        if((p9 == 1 || p2 == 1 || p3 == 1) && (p5 == 1 || p6 == 1 || p7 == 1))
                        {
                            is_8_simple = 0;
                        }
                    }
                    if(p8 == 0 && p2 == 0)
                    {
                        if(p9 == 1 && (p3 == 1 || p4 == 1 || p5 == 1 || p6 == 1 || p7 == 1))
                        {
                            is_8_simple = 0;
                        }
                    }
                    if(p4 == 0 && p2 == 0)
                    {
                        if(p3 == 1 && (p5 == 1 || p6 == 1 || p7 == 1 || p8 == 1 || p9 == 1))
                        {
                            is_8_simple = 0;
                        }
                    }
                    if(p8 == 0 && p6 == 0)
                    {
                        if(p7 == 1 && (p3 == 9 || p2 == 1 || p3 == 1 || p4 == 1 || p5 == 1))
                        {
                            is_8_simple = 0;
                        }
                    }
                    if(p4 == 0 && p6 == 0)
                    {
                        if(p5 == 1 && (p7 == 1 || p8 == 1 || p9 == 1 || p2 == 1 || p3 == 1))
                        {
                            is_8_simple = 0;
                        }
                    }
                    int sum = p2 + p3 + p4+ p5 + p6 + p7 + p8 + p9;
                    if(sum != 1 && sum != 0 && is_8_simple== 1)
                    {
                        dst.at<uchar>(i,j) = 0;
                        is_end = true;
                    }

                }
            }
        }
        if(!is_end)
        {
            break;
        }
    }

    // modify skeleton
    bool is_skeleton = false;
    int skeleton_row = 0;
    for(int i = 0; i < dst.rows; i++)
    {
        is_skeleton = false;
        for(int j = 0; j < dst.cols; j++)
        {
            if(dst.at<uchar>(i, j) > 0)
            {
                is_skeleton = true;
                skeleton_row = i;
            }
        }
        if(is_skeleton)
        {
            break;
        }
    }
    for(int j = 0; j < dst.cols; j++)
    {
        if(dst.at<uchar>(skeleton_row, j) > 0)
        {
            dst.at<uchar>(skeleton_row + 1, j) = dst.at<uchar>(skeleton_row, j);
            dst.at<uchar>(skeleton_row, j) = 0;
        }
    }

    skeleton_row = dst.rows - 1;
    for(int i = dst.rows - 1; i >= 0; i--)
    {
        is_skeleton = false;
        for(int j = 0; j < dst.cols; j++)
        {
            if(dst.at<uchar>(i, j) > 0)
            {
                is_skeleton = true;
                skeleton_row = i;
            }
        }
        if(is_skeleton)
        {
            break;
        }
    }
    for(int j = 0; j < dst.cols; j++)
    {
        if(dst.at<uchar>(skeleton_row, j) > 0)
        {
            dst.at<uchar>(skeleton_row - 1, j) = dst.at<uchar>(skeleton_row, j);
            dst.at<uchar>(skeleton_row, j) = 0;
        }
    }
    // modify skeleton finish
}
/**
	**函数功能说明：
	**17易混淆再判断
	**参数说明：
	**src:需要判断的字符图像
*/
char distinguish17(Mat& src)
{
    Mat src_img = Mat(src.size(), CV_8UC1);
    threshold(src, src_img, 0, 255, THRESH_BINARY);
	imwrite( "rosen_Image.jpg", src );
    int count;
    int max_down = 0;
    int max_up = 0;
    int max_row1 = 0;
    int max_row2 = 0;
    for(int i = 1, j = src_img.rows /2; i < j; i++, j--)
    {
        count = 0;
        for(int k = 0; k < src_img.cols; k++)
        {
            if(src_img.at<uchar>(i, k) > 0)
            {
                count++;
            }
        }
        if(count > max_down)
        {
            max_down = count;
            max_row1 = i;
        }

        count = 0;
        for(int k = 0; k < src_img.cols; k++)
        {
            if(src_img.at<uchar>(j, k) > 0)
            {
                count++;
            }
        }
        if(count > max_up)
        {
            max_up = count;
            max_row2 = j;
        }
    }

    char res;
    if(max_down <=5)
    {
        res = '1';
        if(max_down >= float(src.cols) / 2.3)
        {
            line(src, Point(0, max_row1), Point(src.cols / 5, max_row1), Scalar(255, 0, 0));
        }
        if(max_up >= float(src.cols) / 2.3)
        {
            line(src, Point(0, max_row2), Point(src.cols / 5, max_row2), Scalar(255, 0, 0));
        }
    }
    else
    {
        res = '7';
        if(max_down < float(src.cols) / 2.3)
        {
            line(src, Point(src.cols - 1, max_row1), Point(src.cols - 1 - src.cols / 5, max_row1), Scalar(255, 0, 0));
        }
        if(max_up < float(src.cols) / 2.3)
        {
            line(src, Point(src.cols - 1, max_row2), Point(src.cols - 1 - src.cols / 5, max_row2), Scalar(255, 0, 0));
        }
    }
	imwrite( "Gray_Image.jpg", src );
    return res;
}
/**
	**函数功能说明：
	**1X易混淆再判断
	**参数说明：
	**src:需要判断的字符图像
*/
char distinguish1X(Mat& src)
{
	Mat src_img = Mat(src.size(), CV_8UC1);
    threshold(src, src_img, 0, 255, THRESH_BINARY);
		imwrite( "rosen_Image.jpg", src );
    int count;
    int max_down = 0;
    int max_up = 0;
    int max_row1 = 0;
    int max_row2 = 0;
	int flag=0;
	for(int i=0;i<src_img.rows;i++)
	{
		count=0;
		for(int k = 0; k < src_img.cols-1; k++)
		{
			if(src_img.at<uchar>(i, k) !=src_img.at<uchar>(i, k+1) )
			{
				count++;
				if(count==4)
				{
					flag=1;
					break;
				}
			}
		}
		if(flag==1)
			break;
	}
	char res;
    if(count >= 4)
    {
        res='X'; 
    }
	else
	{
		res='1';
	}
	imwrite( "Gray_Image.jpg", src );
    return res;
}
/**
	**函数功能说明：
	**车牌粗定位，倾斜校正，从候选项中挑选出最终项
	**参数说明：
	**finalout:保存最后车牌号字符串的向量
	**nvideo：视频编号
	**count：单个视频里的帧编号
	**exnum:单帧车牌编号
	**style：车牌类型，普通：0 使馆：1 警车：2 领馆：3
	**f：车牌颜色，黑色：0 蓝色：1 白色：2 黄色：3
	**q：候选车牌编号
	**showflag：是否显示
	**rete：定位返回值
	**block：待识别字符块向量
*/
int recogchar(vector<string> &finalout,int nvideo, int count,int &exnum,int cons,
	int style,int f,int q,char *color_flag,int showflag,int rete,vector<Mat> block)
	{
		//for (int i = 0 ; i< block.size(); i ++ )
		//{

		//	imshow("yyy",block[i]  );
		//	waitKey(1) ;	
		//	int x = 0 ;
		//}
		int ret=0;
		int fn=192;
		bool backchar=false;//判断最后一位是否为汉字
		int start=2;
		char sName[100]={0};
		char pn[7]={0};
		//char *out0;//储存最后一位汉字
		string out0;

		Mat layerSizesa=(Mat_<int>(1,3)<<192,192,25);
		CvANN_MLP Nnwa(layerSizesa, CvANN_MLP::SIGMOID_SYM,0, 0 );
		Nnwa.load("xml/25000_192_alpha.xml");//识别第二位字母的xml文件加载
		
		Mat layerSizes0=(Mat_<int>(1,3)<<192,192,34);
		CvANN_MLP Nnw0(layerSizes0, CvANN_MLP::SIGMOID_SYM,0, 0 );
		Nnw0.load("xml/34000_192.xml");
		
		Mat layerSizesd=(Mat_<int>(1,3)<<192,192,10);
		CvANN_MLP Nnwd(layerSizesd, CvANN_MLP::SIGMOID_SYM,0, 0 );	
		Nnwd.load("xml/10000_digit_192.xml");

		Mat layerSizes=(Mat_<int>(1,3)<<192,192,39);
		CvANN_MLP Nnw(layerSizes, CvANN_MLP::SIGMOID_SYM,0, 0 );//末位字
		int exflag=1;
		int charnum=7;
		if(style==3)
		{
			charnum=6;
			backchar=true;
			out0="领";
		}
		if(style!=1)
		{
			for(int i=start;i<=charnum;i++)
			{
				vector<float> charFeature;
				Mat img = block[i-1].clone();
				if(img.empty())
				{
					exflag=0;
					break;
				}
				if(cons==1&&((float)img.rows/(float)img.cols>5||(float)img.rows/(float)img.cols<1))
				{
					//cout<<"img empty"<<endl;
					//cout<<"1837"<<endl;
						exflag=0;
						break;
				}
				if(i==start&&cons==1&&(float)img.rows/(float)img.cols>3.5)//汉字后一位分片高比宽大于3.5去掉
				{
					//cout<<"1841"<<endl;
						exflag=0;
						break;
				}
				Mat cf;
				Mat result;		
				calcGradientFeat1(img,charFeature);
				calcGrayFeat1(img,charFeature);
				//img.release();
				cf=Mat(1,192,CV_32FC1);
				cf=Mat(charFeature);
				cf=cf.reshape(0,1);
				//std::cout<<"cfMat"<<cf<<endl;
	
				if(f!=1&&i==7&&f!=3)//非蓝牌末位
				{
					result=Mat(1,39,CV_32FC1);
					if(f == 0)
						Nnw.load("xml/mlp_blackback1215.xml");
					if(f == 2)
						Nnw.load("xml/mlp_whiteback1215.xml");
					if(f == 3)
						Nnw.load("xml/mlp_yellowback1215.xml");
					Nnw.predict(cf,result);
				}

				else if(i==2)//所有牌第二位
				{
					result=Mat(1,25,CV_32FC1);
					Nnwa.predict(cf,result);
				}	
				else //剩余：蓝牌非第二位、非蓝牌非第二位非末位
				{
					result=Mat(1,34,CV_32FC1);
					Nnw0.predict(cf,result);
				}
				cf.release();
				//std::cout<<"resultMat"<<result<<endl;
				Point maxLoc;
				minMaxLoc(result, NULL, NULL, NULL, &maxLoc);	
				result.release();
				int re=maxLoc.x;
				char ch;
				if(i!=2)
				{
					if(re>9&&re<18)
					{
						ch='A'+re-10;
					}
					else if(re>17&&re<23)
					{
						ch='A'+re-9;
					}
					else if(re>22&&re<34)
					{
						ch='A'+re-8;
					}
					else if(re>=34)
					{
						backchar=true;
						if(re == 34)
							out0="澳";
						if(re == 35)
							out0="警";
						/*if(re == 36)
							out0="领";*/
						if(re == 37)
							out0="港";
						if(re == 38)
							out0="学";
					}
					else 
					{
						ch='0'+re;
					}
				}
				if(i==2)
				{
					if(re<8)
						ch='A'+re;
					else
						ch='A'+re+1;
				}																	
					if(ch=='1'||ch=='7')
					{
						Mat dst;
						rosenfeld(img, dst);
						ch=distinguish17(dst);
					}
					if(ch=='X')
					{
						Mat dst;
						rosenfeld(img, dst);
						ch=distinguish1X(dst);
					}
					if(ch=='D'||ch=='0'||ch=='O')
					{
						ret=4;
						Mat dst;
						imwrite("src.jpg", img);
						correct(img, dst);
						imwrite("dst.jpg", dst);
						Mat layerSizes0d=(Mat_<int>(1,3)<<364,364,2);
						CvANN_MLP Nnw0d(layerSizes0d, CvANN_MLP::SIGMOID_SYM,0, 0 );
						Nnw0d.load("xml/0D_dis_364_2258.xml");
						vector<float> feat;
						calcHOGFeat(dst,feat);
						calcGrayFeat3(dst,feat);		
						calcGradientFeat2(dst,feat);
						Mat cf1;
						Mat result1;
						cf1=Mat(1,364,CV_32FC1);	
						cf1=Mat(feat);
						cf1=cf1.reshape(0,1);
						result1=Mat(1,2,CV_32FC1);
						Nnw0d.predict(cf1,result1);
						Point maxLoc1;
						minMaxLoc(result1, NULL, NULL, NULL, &maxLoc1);	
						int resu=maxLoc1.x;
						//cout<<result1<<endl;
						if(resu==0)
						{
							if(i==2)
								ch='O';
							else
								ch='0';
						}
						if(resu==1)
							ch='D';
					}
					pn[i-start]=ch;
			}	
		}
		if(style==1)
		{
			start=1;
			for(int i=1;i<=6;i++)
			{
				vector<float> charFeature;
				Mat img = block[i-1].clone();
				if(img.empty()||(float)img.rows/(float)img.cols>5)
				{
					//cout<<"img empty"<<endl;
					//cout<<"1989"<<endl;
						exflag=0;
						break;
				}
				Mat cf;
				Mat result;
				calcGradientFeat1(img,charFeature);
				calcGrayFeat1(img,charFeature);
				img.release();
				cf=Mat(1,192,CV_32FC1);
				cf=Mat(charFeature);
				cf=cf.reshape(0,1);
				result=Mat(1,10,CV_32FC1);
				Nnwd.predict(cf,result);
				cf.release();
				//std::cout<<"resultMat"<<result<<endl;
				Point maxLoc;
				minMaxLoc(result, NULL, NULL, NULL, &maxLoc);	
				result.release();
				int re=maxLoc.x;
				char ch;
				ch='0'+re;
				pn[i-start]=ch;
			}	
		}

		if(exflag==0)
		{
			return -1;
		}
			if(cons == 3)
			{
				pn[6]='\0';
				pn[5]='\0';
			}
			else
			{
				if(backchar == true)
				{
					pn[6]='\0';
					pn[5]='\0';
				}
				else
					pn[6]='\0';
			}
			//char *out;
			string out;
			if(style!=1)
			{
				vector<float> charFeature1;
				charFeature1.clear();
				Mat img1 = block[0].clone();
				if((float)img1.rows/(float)img1.cols>3.5&&rete==0)//汉字宽高比不合适踢掉
				{
					cout<<"2049"<<endl;
					return -1;
				}
				calcGradientFeat1(img1,charFeature1);
				calcGrayFeat1(img1,charFeature1);
				img1.release();
								
				Mat cf1=Mat(1,fn,CV_32FC1);
				cf1=Mat(charFeature1);
				cf1=cf1.reshape(0,1);
				//std::cout<<"cfMat"<<cf1;
				Mat result1;
				if(f==2)//白牌，有10个字母
				{
					result1=Mat(1,42,CV_32FC1);
					Mat layerSizes1=(Mat_<int>(1,3)<<fn,fn,42);
					CvANN_MLP Nnw1(layerSizes1, CvANN_MLP::SIGMOID_SYM,0, 0 );
					Nnw1.load("xml/mlpchar_white_head.xml");
					Nnw1.predict(cf1,result1);
				}													
				else//蓝，黄，黑
				{
					result1=Mat(1,32,CV_32FC1);
					Mat layerSizes1=(Mat_<int>(1,3)<<fn,fn,32);
					CvANN_MLP Nnw1(layerSizes1, CvANN_MLP::SIGMOID_SYM,0, 0 );
					Nnw1.load("xml/char_0207_v2.xml");//================================
					Nnw1.predict(cf1,result1);
				}
				//std::cout<<"resultMat"<<result1;
				cf1.release();
				Point maxLoc1;
				minMaxLoc(result1, NULL, NULL, NULL, &maxLoc1);				
				int re1=maxLoc1.x;
				if(re1==24&&f!=0)//如果在非黑牌中识别出了使，则找除使外的最大项
				{
					result1.at<int>(0,24)=-10;
					minMaxLoc(result1, NULL, NULL, NULL, &maxLoc1);	
					re1=maxLoc1.x;
				}
				float  y=	result1.at<float>(0 ,re1) ;
				printf("相似度为%f\n" , y);

				if (y<0.35)
				{
					re1 = -1 ;
				}

				result1.release();
				if ( re1== -1 )
				{
					char character[4];
					IniRead( "character", character ); 						
					out  = character ;  
				}


				if(re1==0)
				{
					out="皖";
				}
				if(re1==1)
				{
					out="京";
				}
				if(re1==2)
				{
					out="渝";
				}
				if(re1==3)
				{
					out="闽";
				}
				if(re1==4)
				{
					out="甘";
				}
				if(re1==5)
				{
					out="粤";
				}
				if(re1==6)
				{
					out="桂";
				}
				if(re1==7)
				{
					out="贵";
				}
				if(re1==8)
				{
					out="琼";
				}
				if(re1==9)
				{
					out="冀";
				}
				if(re1==10)
				{
					out="黑";
				}
				if(re1==11)
				{
					out="豫";
				}
				if(re1==12)
				{
					out="鄂";
				}
				if(re1==13)
				{
					out="湘";
				}
				if(re1==14)
				{
					out="赣";
				}
				if(re1==15)
				{
					out="吉";
				}
				if(re1==16)
				{
					out="辽";
				}
				if(re1==17)
				{
					out="蒙";
				}
				if(re1==18)
				{
					out="宁";
				}
				if(re1==19)
				{
					out="青";
				}
				if(re1==20)
				{
					out="陕";
				}
				if(re1==21)
				{
					out="鲁";
				}
				if(re1==22)
				{
					out="沪";
				}
				if(re1==23)
				{
					out="晋";
				}
				if(re1==24)
				{
					out="使";
				}
				if(re1==25)
				{
					out="川";
				}
				if(re1==26)
				{
					out="苏";
				}	
				if(re1==27)
				{
					out="津";
				}
				if(re1==28)
				{
					out="新";
				}
				if(re1==29)
				{
					out="藏";
				}
				if(re1==30)
				{
					out="云";
				}
				if(re1==31)
				{
					out="浙";
				}
				if(re1==32)
				{
					out="B";
				}
				if(re1==33)
				{
					out="C";
				}
				if(re1==34)
				{
					out="G";
				}
				if(re1==35)
				{
					out="H";
				}
				if(re1==36)
				{
					out="J";
				}	
				if(re1==37)
				{
					out="K";
				}
				if(re1==38)
				{
					out="L";
				}
				if(re1==39)
				{
					out="N";
				}
				if(re1==40)
				{
					out="S";
				}
				if(re1==41)
				{
					out="V";
				}
			}	
			if(style==1)
			{
				out="使";
			}
			char fname[50]={0};
			sprintf(fname,"多车牌识别(test0127_17Xdis).csv");
			if(fname==NULL)
			{
				cout<<"NULL fname"<<endl;
				return -1;
			}
			FILE *fp=fopen(fname,"a");
				
			char showfile[50]={0};
				
			char fout[20]={0};
			if(backchar == true)
			{
				if(out0=="澳"&&out!="粤"||out0=="港"&&out!="粤")
				{
					cout << "港澳车牌首末汉字不对应" << endl;
					return -1;
				}
				fprintf(fp, "%d,%d,%s,%s,%s%s\n",count,q,color_flag,out.c_str(),pn,out0.c_str());
				
				sprintf(fout, "%s %s%s",out.c_str(),pn,out0.c_str());
				string s=fout;
				if(s.size()==10)
				{
					finalout.push_back(s);
				}
				
				if(showflag==1)
				{
					sprintf(showfile,"show %d",q);
					
				}
			}
			else
			{
				fprintf(fp, "%d,%d,%s,%s,%s\n",count,q,color_flag,out.c_str(),pn);
				
				sprintf(fout, "%s %s",out.c_str(),pn);
				string s=fout;
				if(s.size()==9)
				{
					finalout.push_back(s);
				}
				if(showflag==1)
				{
					sprintf(showfile,"show %d",q);
				}					
			}
			fclose(fp);
			exnum++;
			return ret;
	}


int  PlatePosition(   Mat &resize,Mat &frame,Mat &gray,CvSeq **OutPlate,vector <int> &nPlate , vector<Rect>&vecPos ,   int count,
	int &pnum,int cons,int &rete ,    bool graypic  )
{
	
	double *cannyval;
	CvSeq *plate =NULL ;
	char sName[300] = {0};
	int flagi=1 ;
	
	double max;
	
	
	IplImage *igray = &IplImage(gray);
	//用cascade_final_14.xml分类器定位==============================
	if(rete==0)
	{
		if(cons == 1)
		{
			if(flagi==0)
			{
				plate = cvHaarDetectObjects(igray, cascade, storage, 1.2, 0, 8, cvSize(60, 15));//1.2,1,2
			}
			else
			{
				plate = cvHaarDetectObjects(igray, cascade, storage, 1.1, 2, 0, cvSize(60, 15));
			}
		}

		nPlate.clear();
		for(int i=0;i<=100;i++)
		{
			nPlate.push_back(-1);
		}
	
		//double *cannyval;
		//cannyval=(int *)malloc(sizeof(int)*plate->total); 
		cannyval=(double *)malloc(sizeof(double)*plate->total);
		int cc=0;


		max=realplate(plate,gray,cannyval,resize,vecPos ,count,cc,graypic);
		
		//
		//for(int i=0; i<(plate? plate->total: 0); i++)
		//{
		//	Rect* r = (Rect*) cvGetSeqElem(plate, i);
		//
		//	
		//	Mat  matshow = gray(*r	);
		//	imshow("111",matshow) ;
		//	waitKey(0);
		//	int x = 0 ;
		//
		//}
		




		//printf("%lf\n",max);

		//=========================================
		//在原型测试时
		//检测出多个车牌
		//如果车牌相似度第二大的车牌的相似值与最大的差距较大
		//直接设定flag_reloc=1
		//==========================================
		int flag_reloc=0;
		double *cval0;
		if(cons==1)
		{
			//cval=(int *)malloc(sizeof(int)*plate->total);  
			cval0=(double *)malloc(sizeof(double)*plate->total);   
			for(int i=0; i<(plate? plate->total: 0); i++)
			{
				cval0[i]=cannyval[i];

			}
			if(plate->total>1)
			{
				sort(cval0,cval0+plate->total,cmp);
				if(cval0[1]<cval0[0]*0.7)
					flag_reloc=1;
			}
			free(cval0);
		}
		//return 0;
		bool term=false;
		if(cons==1)
		{
			if(flagi==0)
			{
				term=max <18.5||plate->total <= 1||flag_reloc==1;
			}
			else
			{
				term=max <13;
			}
		}
		if(cons==2)
			term=max <18;


		//========================================================
		//满足term条件的
		//从-20°~20°旋转检测
		if(term)
		{
			free(cannyval);

			Mat dst = Mat(gray.size(), CV_8UC1);
			Mat dstc = Mat(gray.size(), CV_8UC3);

			vector<CvSeq*> v_plate;
			//vector<Mat> v_dst;
			v_plate.clear();
			//v_dst.clear();
			Mat dstf = Mat(gray.size(), CV_8UC1);
			Mat dstcf = Mat(gray.size(), CV_8UC3);
			int cnt=0;
			int pos=0;
			int angle0=0;
			double maxvalue=0;	
			for(int k=-20;k<=20;k+=2)
			{				
				if(k==0)
					continue;
				//printf("倾斜角度：%f\n\n",angle);
				Mat rot_mat = Mat(2,3,CV_32FC1);
				Point2f center = Point2f(gray.cols/2,gray.rows/2);
				double scale = 1.0;
				rot_mat=getRotationMatrix2D(center,k,scale);
				warpAffine(gray,dst,rot_mat,dst.size());
				warpAffine(resize,dstc,rot_mat,dstc.size());
				IplImage * idst = &IplImage(dst);
				if(cons==1)
				{
					if(flagi==0)
					{
						plate = cvHaarDetectObjects(idst, cascade, storage, 1.1, 1, 2/*CV_HAAR_DO_CANNY_PRUNING*/, cvSize(60,15));
					}
					else
					{
						plate = cvHaarDetectObjects(idst, cascade, storage, 1.1, 2, 0/*CV_HAAR_DO_CANNY_PRUNING*/, cvSize(60,15));
					}
				}

				cannyval=(double *)malloc(sizeof(double)*plate->total); 
				v_plate.push_back(plate);
				//v_dst.push_back(dst);
				cc++;
				double max2=realplate(plate,dst,cannyval, dstc,vecPos,count,cc,graypic);
				//printf("\n%d°%lf\n",k,max2);
				if(plate->total!=0/*&&max2>30*/)//==========
				{	
					if(max2>maxvalue)
					{
						maxvalue=max2;
						pos=cnt;
						angle0=k;
						dstf=dst.clone();
						dstcf=dstc.clone();
						max=max2;
					}
				}
				cnt++;
				free(cannyval);
			}	
			//printf("best: %d°\n",angle0);
			plate=v_plate[pos];
			v_plate.clear();
			//exnum++;
			//return 0;
			dst=dstf.clone();
			dstc=dstcf.clone();


			//还原最大车牌相似值的那个车牌
			cannyval=(double *)malloc(sizeof(double)*plate->total); 
			cc++;
			double mmax=realplate(plate,dst,cannyval,dstc,vecPos , count,cc,graypic);
			Mat rot_mat = Mat(2,3,CV_32FC1);
			CvPoint2D32f center = cvPoint2D32f(gray.cols/2,gray.rows/2);
			double scale = 1.0;
			rot_mat=getRotationMatrix2D(center,angle0,scale);
			Mat resize_t=resize.clone();
			Mat gray_t=gray.clone();

			warpAffine(resize,resize_t,rot_mat,resize_t.size());
			warpAffine(gray,gray_t,rot_mat,gray_t.size());

			resize=resize_t.clone();
			gray=gray_t.clone();
			if(plate->total==0)
			{
				sprintf(sName, "simple_result/frame_%d.jpg", count);
				sprintf(sName, "wrong/%d_nonloc.jpg", count);//调试用
				imwrite(sName,frame);
				printf("no car plate has been found!\n");//调试用
				cvClearSeq(plate);
				free(cannyval);
				cannyval=NULL;

				rete=1;
	
			}
		}
	}

	//用cascade_new.xml分类器定位==============================
	 if(rete==1)
	{
		vector<Rect> rects;  
		if(cons == 1)
		{
			if(flagi==0)
			{
				rects.clear(); 
				cascade3.detectMultiScale(gray,rects,1.1,2,0/*0|CV_HAAR_DO_CANNY_PRUNING*/,Size(60, 20));
				plate=cvCreateSeq(0, sizeof(CvSeq), sizeof(Rect), storage);
				for(int i=0;i<rects.size();i++)
					cvSeqPush(plate, &rects[i] );
				//plate = cvHaarDetectObjects(gray, cascade, storage, 1.2, 0, 8, cvSize(60, 15));//1.2,1,2
			}
			else
			{
				rects.clear(); 
				cascade3.detectMultiScale(gray,rects,1.1,2,0|CV_HAAR_DO_CANNY_PRUNING,Size(60, 20));
				plate=cvCreateSeq(0, sizeof(CvSeq), sizeof(Rect), storage);
				for(int i=0;i<rects.size();i++)
					cvSeqPush(plate, &rects[i] );
				//plate = cvHaarDetectObjects(gray, cascade, storage, 1.1, 2, 0, cvSize(60, 15));
			}
		}


		nPlate.clear();
		for(int i=0;i<=100;i++)
		{
			nPlate.push_back(-1);
		}

		//cannyval=(int *)malloc(sizeof(int)*plate->total); 
		cannyval=(double *)malloc(sizeof(double)*plate->total);
		int cc=0;
		max=realplate_new(plate,gray,cannyval,resize,count,cc,graypic);

		if(plate->total==0)
		{
			sprintf(sName, "simple_result/frame_%d.jpg", count);
			sprintf(sName, "wrong/%d_nonloc.jpg", count);//调试用
			imwrite(sName,frame);
			printf("no car plate has been found!\n");//调试用
			cvClearSeq(plate);
			free(cannyval);
			cannyval=NULL;

			rete=2;
	
			//sprintf(sName, "simple_result/frame_%d.jpg", count);//调试用
			////imwrite(sName,frame);
			///*cvReleaseImage(&dst);*/
			//sprintf(sName, "wrong/%d_nonloc.jpg", count);//调试用
			////imwrite(sName,frame);
			//printf("no car plate has been found!\n");//调试用
			//cvClearSeq(plate);
			//return 0;
		}
		else 
		{
			int num=plate->total;
			for(int i=1;i<num;i++)
				cvSeqRemove(plate,1);					
		}
	}

	 if(rete==2)//cascade_6000_lbp.xml
	{
		vector<Rect> rects;  
		if(cons == 1)
		{
			if(flagi==0)
			{
				rects.clear(); 
				cascade2.detectMultiScale(gray,rects,1.1,2,0/*0|CV_HAAR_DO_CANNY_PRUNING*/,Size(60, 20));//1 2
				plate=cvCreateSeq(0, sizeof(CvSeq), sizeof(Rect), storage);
				for(int i=0;i<rects.size();i++)
					cvSeqPush(plate, &rects[i] );
				//plate = cvHaarDetectObjects(gray, cascade, storage, 1.2, 0, 8, cvSize(60, 15));//1.2,1,2
			}
			else
			{
				rects.clear(); 
				cascade2.detectMultiScale(gray,rects,1.1,2,0|CV_HAAR_DO_CANNY_PRUNING,Size(60, 20));
				plate=cvCreateSeq(0, sizeof(CvSeq), sizeof(Rect), storage);
				for(int i=0;i<rects.size();i++)
					cvSeqPush(plate, &rects[i] );
				//plate = cvHaarDetectObjects(gray, cascade, storage, 1.1, 2, 0, cvSize(60, 15));
			}
		}

		nPlate.clear();
		for(int i=0;i<=100;i++)
		{
			nPlate.push_back(-1);
		}
		//cannyval=(int *)malloc(sizeof(int)*plate->total); 
		cannyval=(double *)malloc(sizeof(double)*plate->total);
		int cc=0;
		max=realplate_new(plate,gray,cannyval,resize,count,cc,graypic);
		//printf("%lf\n",max);

		if(plate->total==0)
		{
			sprintf(sName, "simple_result/frame_%d.jpg", count);//调试用
			//imwrite(sName,frame);
			/*cvReleaseImage(&dst);*/
			sprintf(sName, "wrong/%d_nonloc.jpg", count);//调试用
			//imwrite(sName,frame);
			printf("no car plate has been found!\n");//调试用
			cvClearSeq(plate);
			free(cannyval);
			cannyval=NULL;
			rete = 3 ;
			//rete=2;
	
		}
	}
	 if(rete == 3)//cascade_double.xml
	{
		vector<Rect> rects;
		rects.clear();
		cascade1.detectMultiScale(gray,rects,1.1,1,2,Size(60, 30));

		for (int i=0;i<rects.size();i++)
		{
			if(rects[i].y < gray.cols / 4)
				rects[i] = Rect(0,0,1,1);
		}

		plate = cvCreateSeq(0, sizeof(CvSeq), sizeof(Rect), storage);
		for(int i=0;i<rects.size();i++)
			cvSeqPush(plate, &rects[i] );
		//plate = cvHaarDetectObjects(gray, cascade1, storage, 1.1, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(60, 30));//检测双排结构车牌，尺寸待改
		nPlate.clear();
		for(int i=0;i<=100;i++)
		{
			nPlate.push_back(-1);
		}

		cannyval=(double *)malloc(sizeof(double)*plate->total);
		int cc=0;
		max=realplate_new(plate,gray,cannyval,resize,count,cc,graypic);
		//printf("%lf\n",max);
	}

	if(plate->total==1)
	{
		nPlate[0]=0;
	//	memcpy(OutPlate,plate, sizeof(CvSeq)*plate->total );
			*OutPlate = plate ;
		//cvClearSeq(plate) ;

		free(cannyval);
	 cannyval=NULL;
	 return 1 ;
	}

	//========================================================================
	//将用realplate算出的cannyval值排序
	//取前70%
	//将序号位置存在nPlate里

	if(plate->total>1)
	{
		int j=0;
		//int *cval;
		double *cval;
		//cval=(int *)malloc(sizeof(int)*plate->total);  
		cval=(double *)malloc(sizeof(double)*plate->total);   
		for(int i=0; i<(plate? plate->total: 0); i++)
		{
			cval[i]=cannyval[i];

		}
		sort(cannyval,cannyval+plate->total,cmp);
		for(int i=0;i<plate->total;i++)
		{
			if(cannyval[i]<0.7*max)
			{
				pnum=i;
				break;
			}
		}
		if(pnum<2)
			pnum=2;
		if(pnum>4)
			pnum=4;
		if(flagi==1)
			pnum=7;
		for(int j=0;j<pnum;j++)
		{
			for(int i=0; i<(plate? plate->total: 0); i++)
			{
				if(cval[i]==cannyval[j])//cval是未排序原集，cannyval是已排序集
				{
					nPlate[j]=i;
					//printf("%lf\n ",cannyval[j]);
				}
			}
		}

	/*	if (OutPlate->elem_size != plate->elem_size)
		{
			cvClearSeq(OutPlate);
			OutPlate = cvCreateSeq( 0, sizeof(CvSeq),plate->elem_size, storage );
		}*/
		*OutPlate = plate ;
	// memcpy(,, sizeof(plate->elem_size)*plate->total );
	 //	cvClearSeq(plate) ;
		//printf("\n");
		free(cval);
		free(cannyval);
		cannyval=NULL;

		 return 2 ;
	}
	if(cannyval)
	{
		free(cannyval);
		cannyval=NULL;
	}
		cvClearSeq(plate) ;
	return 0 ;
} 

 bool HorizontalRectify( Mat &imgEdge, Mat &imgPlateGray ,  Mat &imgPlatecolor   , Mat &thresh ,
	 Rect  &re_rect1 , Rect &rect , int &getnoline ,double &angle)
 {

 if( rect.width < 0.2*imgEdge.cols && rect.height > imgEdge.rows*0.8 )
	 getnoline=1;

 CvMemStorage* storage1 = cvCreateMemStorage(0);
 CvSeq* lines = 0;

 Mat carp = imgPlateGray.clone();//原始图
 Mat dst = imgPlateGray.clone();//结果图
 Mat dst_col = imgPlatecolor.clone();//结果彩图
 Rect  re_rect ;
 re_rect.x= rect.x;
 re_rect.y= rect.y;
 re_rect.width= rect.width;
 re_rect.height= rect.height;

 re_rect1.x=0;
 re_rect1.y=rect.y;
 re_rect1.width=imgEdge.cols;
 re_rect1.height= rect.height;



 int down = rect.y + rect.height - 1 ;

 if((rect.width>imgEdge.cols*0.8)&&(rect.width<0.47*imgEdge.rows)
	 ||(rect.height<0.3*imgEdge.rows)&&(down==imgEdge.rows-1)
	 ||(rect.height<0.3*imgEdge.rows)&&(rect.x==0))//顶部或底部被腐蚀掉
 {
	 re_rect1.y=0;
	 re_rect1.height=imgEdge.rows;
 }	



 Mat thresh1=thresh.clone();

 Mat color= Mat(imgEdge.size(),CV_8UC3);
 cvtColor( imgEdge, color, CV_GRAY2BGR );
 Mat bw_backup=imgEdge.clone();
 IplImage *ibw=&(IplImage(imgEdge));
 lines = cvHoughLines2( ibw, storage1, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180,rect.width/4 /*imgEdge.rows/4*/, rect.width/5+1,rect.width/5+1/*10, imgEdge.rows/10*/);
  int threshval;
 //====对于一些不能直接检测出正确水平直线的图====
 //====直接用水平梯度图来检测水平直线============
 if(lines->total==0||getnoline)
 {
	 Mat H_img=Mat (carp.size(),CV_8UC1);
	 float horizontalMatrix[] = {
		 1,2,1,
		 0,0,0,
		 -1,-2,-1,
	 };
	 Mat M_horizontal_sobel = Mat(3, 3, CV_32F, horizontalMatrix);
	 filter2D(carp, H_img, 8,M_horizontal_sobel,Point(-1, -1));
	 Mat thresh_h=carp.clone();
	 IplImage *iH_img = &(IplImage(H_img));
	 threshval = getThreshVal_Otsu_8u(iH_img);
	 threshold(H_img, thresh_h, threshval, 255, CV_THRESH_BINARY_INV);
	 imgEdge=thresh_h.clone();
     double low = 0.0 ;
	 double  high = 0.0; 
	 IplImage *ithresh_h=&(IplImage(thresh_h));
	 AdaptiveFindThreshold(ithresh_h, &low, &high,3);
	 Canny(thresh_h, imgEdge, low, high); 
	 cvClearSeq(lines);
	 ibw=NULL;
	 ibw=&(IplImage(imgEdge));
	 lines = cvHoughLines2( ibw, storage1, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180,rect.width/5 /*imgEdge.rows/4*/, rect.width/5+1,rect.width/5+1/*10, imgEdge.rows/10*/);
	 color= Mat(imgEdge.size(),CV_8UC3);
	 cvtColor( imgEdge, color, CV_GRAY2BGR );
 }
 Mat matTemp = carp.clone();
 double low = 0.0 , high = 0.0; 
 IplImage *imgtemp = &(IplImage(imgPlateGray));
 AdaptiveFindThreshold(imgtemp, &low, &high,3);
 Canny(imgPlateGray, matTemp, 20, 80); 
 //imshow("orgEdge" ,  matTemp );
 //waitKey(0) ;



 //==========水平角度有筛选==========//
 //========筛选横坐标差值最大的线
 vector<int> vline,vline1;
 int u=0;
 vector <int> Lcand;
 int mini=1000;
 int temp = 0 ;
 int maxi = 0 ;
 int j = 0 ;
 double anglea=0;

 for( int k = 0; k < lines->total; k++ )
 {
	 Point* line = (Point*)cvGetSeqElem(lines,k);
	 //cvLine( color, line[0], line[1], CV_RGB(255,0,0), 3, CV_AA, 0 );

	 CvPoint  pttemp0 , pttemp1 ;  
	 
	 pttemp0.x = line[0].x ;
	 pttemp0.y = line[0].y ;
	 pttemp1.x = line[1].x ;
	 pttemp1.y = line[1].y ;

	 temp =line[1].x-line[0].x;
	 if(temp > maxi)
	 {
		 maxi = temp;
		 j = k;
	 }
 }
 Point* line = (Point*)cvGetSeqElem(lines,j);
 

 if(lines->total>0)
 {
	 cv::line( color, line[0], line[1], CV_RGB(255,0,0), 3, CV_AA, 0 );
	 //imshow("line" ,color );
	 //waitKey(1) ;
	 //sprintf(filename2,"simple_result03cor2/%d_%d_%d_redline.jpg",nvideo,count,q+exnum);
	 ////sprintf(filename2,"simple_result-sp-sobel/%d_%d_%d_redline.jpg",nvideo,count,q);
	 //imwrite(filename2,color);
	 double k = (double)(-(line[1].y-line[0].y))/(double)(line[1].x-line[0].x);
	 double theta = atan(k);
	 angle=-theta*180/3.14;
	 //angle=theta*180/3.14;
 }

 //根据水平角度进行水平校正==============================================
 if(angle!=0&&angle<20&&angle>-20)
 {		
	 //printf("倾斜角度：%f\n\n",angle);
	 Mat rot_mat(2,3,CV_32FC1);
	 Point2f center = Point2f(imgPlateGray.cols/2,imgPlateGray.rows/2);
	 double scale = 1.0;
	 rot_mat=getRotationMatrix2D(center,angle,scale);
	 warpAffine(imgPlateGray,dst,rot_mat,imgPlateGray.size());
	 warpAffine(imgPlatecolor,dst_col,rot_mat,imgPlatecolor.size());//旋转彩色图像
	 //cvWarpAffine(bw,bw1,rot_mat);//旋转bw
	 warpAffine(thresh,thresh1,rot_mat,thresh.size());
	 imgPlateGray = dst.clone();
	 //cvCopy(dst,carplate0,0);
	 imgPlatecolor = dst_col.clone();

 } 
 cvReleaseMemStorage(&storage1);
		 return 0 ;
	 }

bool ShearWarp(Mat &imgEdge ,Mat &imgPlateGray ,Mat &imgPlatecolor ,Rect &re_rect1 , double &angle ,int colnum , int getnoline )
{
	CvMemStorage* storage1 = cvCreateMemStorage(0);
	CvSeq* lines = 0;
	Mat  carp =imgPlateGray.clone() ;
	//bw1=cvCloneImage(bwv);
	Mat color1= Mat(imgEdge.size(),CV_8UC3);
	int temp = 0 ;
	int	maxi=0;
	int j=0;
	
	cvtColor( imgEdge, color1, CV_GRAY2BGR );
	IplImage *ibw1 = &(IplImage(imgEdge));
	//lines = cvHoughLines2( bw1, storage1, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180,20, 1,20  );//累计值最小值，最小线段长度，最大间隔值
	lines = cvHoughLines2( ibw1, storage1, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180,re_rect1.height/4,re_rect1.height/5+1,re_rect1.height/5+1);

	//选竖直坐标差值最大的线
	for( int k = 0; k < lines->total; k++ )
	{
		Point* line = (Point*)cvGetSeqElem(lines,k);

		temp =line[1].y-line[0].y;

		if(temp<0)
			temp=-temp;			
		if(temp > maxi)
		{
			maxi = temp;
			j = k;
		}
	}

	//检测不出竖直方向直线或y坐标差值过小=====
	//用竖直方向梯度图来检测直线==============
	if(lines->total==0||getnoline||maxi<re_rect1.height/3)
	{
		Mat V_img=Mat(carp.size(),CV_8UC1);
		float verticalMatrix[] = {
			-1,0,1,
			-2,0,2,
			-1,0,1,
		};
		Mat M_vertical_sobel = Mat(3, 3, CV_32F, verticalMatrix);
		filter2D(carp, V_img, 8,M_vertical_sobel, cvPoint(-1, -1));
		Mat thresh_v=carp.clone();
		IplImage *iV_img = &(IplImage(V_img));
		int  threshval = getThreshVal_Otsu_8u(iV_img);
		threshold(V_img, thresh_v, threshval, 255, CV_THRESH_BINARY_INV);
		imgEdge=thresh_v.clone();
		double low = 0.0; 
		double high = 0.0; 
		IplImage *ithresh_v=&(IplImage(thresh_v));

		AdaptiveFindThreshold(ithresh_v, &low, &high,3);
		Canny(thresh_v, imgEdge, low, high); 

		//imshow("zz",imgEdge);
		//waitKey(1);

		//sprintf(filename2,"simple_result03cor2/%d_%d_%d_bw1.jpg",nvideo,count,q+exnum);
		////imwrite(filename2,bw1);
		cvClearSeq(lines);
		ibw1 = NULL;
		ibw1 = &(IplImage(imgEdge));
		lines = cvHoughLines2( ibw1, storage1, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180,
			re_rect1.height/4,re_rect1.height/5+1,re_rect1.height/5+1);
		color1= Mat(imgEdge.size(),CV_8UC3 );
		cvtColor( imgEdge, color1, CV_GRAY2BGR );
	}
	vector<int> vline ;
	vector<int> vline1;
	vector <int> Lcand;
	maxi=0;
	j=0;
	vline.clear();
	vline1.clear();

	for( int k = 0; k < lines->total; k++ )
	{
		CvPoint* line = (CvPoint*)cvGetSeqElem(lines,k);

		temp =line[1].y-line[0].y;

		if(temp<0)
			temp=-temp;			
		vline.push_back(temp);
		vline1.push_back(temp);
		if(temp > maxi)
		{
			maxi = temp;
			j = k;
		}
	}
	sort(vline.begin(),vline.end(),cmp);
	int u=0;
	Lcand.clear();
	float scale1=0.5;
	if(colnum==0)
		scale1=0.8;
	//选前scale1*maxi=====================================
	while(u<lines->total&&vline[u]>=scale1*maxi)
	{
		for(int k = 0; k < lines->total; k++)
		{
			if(vline1[k]==vline[u])
			{
				Lcand.push_back(k);
				break;
			}
		}
		u++;
	}

	//对于竖直方向直线的检测
	//检测出多条直线
	//需进行二次筛选
	//选择和检测出的水平直线夹角最接近90d°的竖直方向直线
	//此为经验值==============================================
	int mini=1000;
	j=0;
	double 	anglea=0;
	double kpos=0;
	for(int k=0;k<Lcand.size();k++)
	{
		CvPoint* line1 = (CvPoint*)cvGetSeqElem(lines,Lcand[k]);
		double angle1=0, k1=0, o_angle=0;
		if(lines->total>0)
		{
			cv::line( color1, line1[0], line1[1], CV_RGB(255,0,0), 3, CV_AA, 0 );
			//sprintf(filename2,"simple_result03cor2/%d_%d_%d_redline1.jpg",nvideo,count,q+exnum);//============线错！！！！！！！！！！！
			////sprintf(filename2,"simple_result-sp-sobel/%d_%d_%d_redline1.jpg",nvideo,count,q);
			//imwrite(filename2,color1);


			/*		imshow("vertical",color1 );
			waitKey(1) ;
			*/
			k1 = (double)(-(line1[1].y-line1[0].y))/(double)(line1[1].x-line1[0].x);
			double theta = atan(k1);
			angle1=theta*180/3.14;				
			angle1=angle1+angle;
			o_angle=abs(angle1);//图中原始垂直角度
			k1=tan(angle1*3.14/180);
		}
		if(abs(o_angle-90)<mini)//选出图中原始垂直角度最竖直的一条
		{
			anglea=angle1;
			j=Lcand[k];
			mini=abs(o_angle-90);
			kpos=k1;
		}
	}

	//根据错切角度进行错切校正
	double angle1=anglea;
		double k1= kpos;

	if ( fabs( fabs(90 -  fabs (angle1 )) - fabs(angle) )>10 )
	{
		if (angle>0)
		{
			angle1 = 90 - angle ;
		}
		else
		{
			angle1 = -90 - angle ;
		}

		k1 = tan(angle1 * 3.14/180); 
	}	

	if(abs(angle1)>60&&abs(angle1)<120 && angle1 != 90 )
	{
		Mat dst1=imgPlateGray.clone();
		Mat dst_col1=imgPlatecolor.clone();
		//Mat warp_mat1 = Mat(2,3,CV_32FC1);  
		Mat warp_mat1( 2, 3, CV_32FC1 );  
		Point2f  srcTri[3];  
		Point2f  dstTri[3];
		srcTri[0] = Point2f( 0,0 );
		srcTri[1] = Point2f( imgPlateGray.cols - 1, 0 );
		srcTri[2] = Point2f( 0, imgPlateGray.rows - 1 );

		dstTri[0] = Point2f( -(double)imgPlateGray.rows/k1, 0 );
		dstTri[1] = Point2f( imgPlateGray.cols - 1, 0  );
		dstTri[2] = Point2f( 0, imgPlateGray.rows - 1 );
		warp_mat1=getAffineTransform(srcTri,dstTri);
		warpAffine(imgPlateGray,dst1,warp_mat1,imgPlateGray.size());
		warpAffine(imgPlatecolor,dst_col1,warp_mat1,imgPlatecolor.size());
		imgPlatecolor = dst_col1.clone();
		//sprintf(filename2,"simple_result03cor2/%d_%d_%d_warp.jpg",nvideo,count,q+exnum);
		////sprintf(filename2,"simple_result-sp-sobel/%d_%d_%d_warp.jpg",nvideo,count,q);
		//imwrite(filename2,dst1);
		imgPlateGray=dst1.clone();
	}
	if(re_rect1.x+re_rect1.width>imgPlateGray.cols
		||re_rect1.x<0||re_rect1.width<=0
		||re_rect1.x>imgPlateGray.cols
		||re_rect1.y+re_rect1.height>imgPlateGray.rows)
	{
		printf("2175ROI region illegal\n");
		//rete=true;
		//return -1;
		cvClearSeq(lines);
		cvReleaseMemStorage(&storage1);
		return 0 ;
	}
	cvClearSeq(lines);
	cvReleaseMemStorage(&storage1);
	return 0 ;
}

bool AngleRectify( Mat &imgPlateGray,Mat &imgPlatecolor, Rect  &re_rect1 ,
	int count, int exnum ,int colnum	,bool &whitebody)
{

	CarColor(	imgPlateGray , 	whitebody	) ;

	Mat thresh = extractEdge(imgPlateGray,imgPlatecolor,colnum,whitebody);//得到去杂后的车牌二值化的图
	//Mat matOrgGray = imgPlateGray.clone();//原始图
	//Mat matDstGray = imgPlateGray.clone();//结果图
	//Mat dst_col = imgPlatecolor.clone();//结果彩图
	Mat imgEdge = Mat(thresh.size(),CV_8UC1);	;//水平校正用边缘图

	//imshow("edge", thresh);
	//waitKey(1);

	//得边缘图=======================================

	double low = 0.0, high = 0.0; 
	IplImage *ithresh = &(IplImage(thresh));
	AdaptiveFindThreshold(ithresh, &low, &high,3);
	//low = 0 ;
	//high = 150 ;

	Canny(thresh, imgEdge, low, high); 
	//cvAdaptiveThreshold(thresh, bw, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 3, 5);
	//cvCanny(thresh, bw, 50, 150); 
	/*sprintf(filename2,"simple_result03cor2/%d_%d_%d_canny.jpg",nvideo,count,exnum);
	imwrite(filename2,bw);*/
	//==================================================

	//==========求边缘图白色像素的高和宽=======================
	int left=0,right=imgEdge.cols-1;
	int flagp=0;
	for(int w=0;w<imgEdge.cols;w++)
	{
		for(int h=0;h<imgEdge.rows;h++)
		{
			uchar s=imgEdge.at<uchar>(h,w);
			if(s == 255)
			{
				left=w;
				flagp=1;
				break;
			}
		}
		if(flagp==1)
		{
			flagp=0;
			break;
		}
	}

	for(int w=imgEdge.cols-1;w>0;w--)
	{
		for(int h=0;h<imgEdge.rows;h++)
		{
			uchar s=imgEdge.at<uchar>(h,w);
			if(s == 255)
			{
				right=w;
				flagp=1;
				break;
			}
		}
		if(flagp==1)
		{
			flagp=0;
			break;
		}
	}

	int t_width=right-left+1;
	int up=0,down=imgEdge.rows-1;
	for(int h=0;h<imgEdge.rows;h++)
	{
		for(int w=0;w<imgEdge.cols;w++)
		{
			uchar s= imgEdge.at<uchar>(h,w);
			if(s == 255)
			{
				up=h;
				flagp=1;
				break;
			}
		}
		if(flagp==1)
		{
			flagp=0;
			break;
		}
	}
	for(int h=imgEdge.rows-1;h>0;h--)
	{
		for(int w=0;w<imgEdge.cols;w++)
		{
			uchar s=imgEdge.at<uchar>(h,w);
			if(s ==255)
			{
				down=h;
				flagp=1;
				break;
			}
		}
		if(flagp==1)
		{
			flagp=0;
			break;
		}
	}
	int t_height=down-up+1;
	if(t_height<=4||t_width<=4)//边缘图结果无用，跳过
	{
		exnum++;
		return 0 ;
	}

	//if (t_height < imgEdge.rows * 0.75)
	//{
	//	t_height = imgEdge.rows * 0.75 ;
	//}
	//if ( t_height > imgEdge.rows - up - 1 )
	//{
	//	 t_height = imgEdge.rows - up - 1 ;
	//}



	//==============================================================
//	return 0 ;
	//==============================================
	//====水平校正开始==============================
	int getnoline = 0 ;
	Rect  rect ;
	double angle=0; 
	rect.x = left ;
	rect.y = up ;
	rect.width = t_width ;
	rect.height = t_height ;

	//imshow("edge", imgEdge);
	//waitKey(1);

	 HorizontalRectify( imgEdge , imgPlateGray ,  imgPlatecolor  , thresh , re_rect1 , rect ,getnoline,angle ) ;
	//return 0 ;
	 int x= 1 ;
	//=======水平校正结束===============================================

	//===========错切====================================================
	ShearWarp(imgEdge ,imgPlateGray ,imgPlatecolor ,re_rect1 , angle ,colnum ,  getnoline );
	return 1 ;
}

void CarColor(Mat &imgPlateGray ,bool &whitebody	)
{
	int st=0;
	for(int k=0;k<imgPlateGray.cols;k++)
	{
		for(int i=0;i<imgPlateGray.rows/8;i++)
		{			
			st += imgPlateGray.at<uchar>(i,k);
		}
	}
	if(st>255*(imgPlateGray.rows/8)*(imgPlateGray.cols)*0.5)//调参
		whitebody=1;
}



void InitRect(	Rect *r	,int width , int height, int rete ,int flagi	)
{
	if(flagi==1)
	{
		if(r->x - r->width*1/6 > 0)
			r->x = r->x - r->width*1/6;
		else
			r->x = 0;
		if(r->y - r->height*1/6 > 0)
			r->y = r->y - r->height*1/6;
		else
			r->y = 0;
		if(r->x + r->width + r->width*1/3 < width)
			r->width = r->width + r->width*1/3;
		else
			r->width = width - r->x;
		//if(r->y + r->width + r->width< height)
		if(r->y + 1.5*r->height < height)
			r->height = r->height * 1.5;
		else
			r->height = height - r->y;
	}
	else
	{
		if(rete==0)//ori
		{
			if(r->x - r->width*0.3 > 0)
				r->x = r->x - r->width*0.3;//0.2
			else
				r->x = 0;
			if(r->y - r->height*0.3 > 0)
				r->y = r->y - r->height*0.3;//0.2
			else
				r->y = 0;
			if(r->x + r->width + r->width*0.6 < width)//0.4
				r->width = r->width + r->width*0.6;
			else
				r->width = width - r->x;
			//if(r->y + r->width + r->width< height)
			if(r->y + 2*r->height < height)
				r->height = r->height * 2;
			else
				r->height = height - r->y;
		}
		else if(rete==1)//lbp
		{
			if(r->x - r->width*0.05 > 0)
				r->x = r->x - r->width*0.05;
			else
				r->x = 0;
			if(r->y - r->height*0.13 > 0)
				r->y = r->y - r->height*0.13;
			else
				r->y = 0;
			if(r->x + r->width + r->width*0.25 < width)
				r->width = r->width + r->width*0.25;
			else
				r->height = width - r->x;
			if(r->y + r->height + 0.3*r->height< height)
				r->height = r->height * 1.3;
			else
				r->height = height - r->y;
		}
		else if(rete==2)//3100
		{
			if(r->x - r->width*0.05 > 0)
				r->x = r->x - r->width*0.05;
			else
				r->x = 0;
			if(r->y - r->height*0.125 > 0)
				r->y = r->y - r->height*0.125;
			else
				r->y = 0;
			if(r->x + r->width + r->width*0.25 < width)//0.25
				r->width = r->width + r->width*0.25;
			else
				r->width = width - r->x;
			if(r->y + r->height + r->height< height)
				r->height = r->height * 1.25;
			else
				r->height = height - r->y;
		}
		else if (rete==3)//bus
		{
			if(r->x - r->width*0.05 > 0)
				r->x = r->x - r->width*0.05;
			else
				r->x = 0;
			if(r->y - r->height*0.125 > 0)
				r->y = r->y - r->height*0.125;
			else
				r->y = 0;
			if(r->x + r->width + r->width*0.15 < width)
				r->width = r->width + r->width*0.15;
			else
				r->width = width - r->x;
			if(r->y + r->height + r->height< height)
				r->height = r->height * 1.25;
			else
				r->height = height - r->y;
		}
	}

}

int RefinePlate(Mat &frame ,Mat &origin, vector<string> &finalout,vector<string> &colorout,vector<CvPoint> &cent ,
	vector<Rect> &rt, int &rete , vector<string>&Carplates, vector<string>&Platecolor, vector<Rect>&Positions )
{
	//==========识别出结果进行筛选==============================
	vector <bool> mark;
	vector <string> final;
	vector <string> final_pre;
	vector <Point> final_pre_cent;
	vector <Rect> final_pre_rt;
	vector <string> final_pre_clr;
	vector <string> final_color;
//这个for循环是为了去掉后五位字母个数大于等于4的候选字符串
	for(int i=0;i<finalout.size();i++)
	{
		mark.push_back(false);
		int alphacnt=0;
		for(int j=3;j<9;j++)
		{
			//finalout[i][j];
			if(finalout[i][j]>='A'&&finalout[i][j]<='Z')
				alphacnt++;
		}
		if(alphacnt>=4)
		{
			continue;
		}
		else
		{
			final_pre.push_back(finalout[i]);
			final_pre_cent.push_back(cent[i]);
			final_pre_rt.push_back(rt[i]);
			final_pre_clr.push_back(colorout[i]);
		}
	}
	//去掉明显错误字符串后重新赋回原向量
	if(final_pre.size()>0||rete==0)
	{
		finalout.clear();
		cent.clear();
		rt.clear();
		colorout.clear();
		for(int i=0;i<final_pre.size();i++)
		{
			finalout.push_back(final_pre[i]);
			cent.push_back(final_pre_cent[i]);
			rt.push_back(final_pre_rt[i]);
			colorout.push_back(final_pre_clr[i]);
		}
	}
//对位置相近的车牌进行去重处理==============
	for(int i=0;i<finalout.size();i++)
	{
		if(finalout[i].size()<8)
			mark.push_back(true);
		else
			mark.push_back(false);		
	}
	for(int i=0;i<finalout.size();i++)//处理得出结果
	{
		if(mark[i]==true)
			continue;
		vector<string> same;
		same.clear();
		same.push_back(finalout[i]);
		mark[i]=true;
		for(int j=i+1;j<finalout.size();j++)
		{
			int d=rt[j].width*rt[j].width+rt[j].height*rt[j].height;
			if((rt[i].width*rt[i].width+rt[i].height*rt[i].height)<(rt[j].width*rt[j].width+rt[j].height*rt[j].height))
				d=rt[i].width*rt[i].width+rt[i].height*rt[i].height;
			if(
				(cent[i].x-cent[j].x)*(cent[i].x-cent[j].x)+(cent[i].y-cent[j].y)*(cent[i].y-cent[j].y)<d/*/4150*150*/
				&&mark[j]==false)
			{
				same.push_back(finalout[j]);
				mark[j]=true;
			}
		}
		char showfile[100]={0};
		if(same.size()==1)
		{
			final.push_back(same[0]);
			final_color.push_back(colorout[0]);
			rectangle(origin,rt[i],Scalar(0,255,0));
			Mat show/*=cvCreateImage(cvSize(500*origin.rows/origin.cols,500),8,3)*/;
			cv::resize(origin,show,Size(500*origin.cols/origin.rows,500));
			frame=show.clone();
			Carplates = same ;
			Platecolor = colorout ;	
			if (Platecolor.size()>Carplates.size())
			{
				Platecolor.erase(Platecolor.end()-1);
			}
				printf("识别结果：%s %s\n",same[0].c_str(),colorout[0].c_str());
				sprintf(showfile,"show %d",final.size());			
		}
		else
		{
			int max=1;
			int pos=0;
			for(int k=0;k<same.size();k++)
			{
				int cnt=1;
				for(int n=k+1;n<same.size();n++)
				{
					if(!strcmp(same[k].c_str(),same[n].c_str()))
						cnt++;
				}
				if(cnt>max)
				{
					max=cnt;
					pos=k;
				}
			}
			final.push_back(same[pos]);
			final_color.push_back(colorout[pos]);
			rectangle(origin,rt[i],Scalar(0,255,0));
			Mat show/*=cvCreateImage(cvSize(500*origin.rows/origin.cols,500),8,3)*/;
			cv::resize(origin,show,Size(500*origin.cols/origin.rows,500));
			frame=show.clone();
				Carplates = same ;
				Platecolor = colorout ;
				printf("识别结果：%s %s\n",same[pos].c_str(),colorout[pos].c_str());
				sprintf(showfile,"show %d",final.size());
		}	
	}
	//=========整理得最终结果=======================
	Positions = rt;

	//FILE *fpp=fopen(fname,"a");
	bool uncor=0;
	for(int i=0;i<final.size();i++)
	{
		if(final[i].size()<8)
			uncor=1;
		char s1[3]={0};
		char s2[10]={0};
		s1[0]=final[i][0];
		s1[1]=final[i][1];
		s1[2]='\0';
		for(int j=0;j<final[i].size()-3;j++)
		{
			s2[j]=final[i][j+3];
		}
		s2[final[i].size()-1]='\0';

		//fprintf(fpp, "%d,%s,%s,%s\n",count,s1,s2,final_color[i].c_str()/*final[i].c_str()*/);
	}
	//fclose(fpp);
	if(final.size()==0||uncor==1)
	{
		if(rete==0)
			rete=1;
		else if(rete==1)
			rete=2;
		else if(rete==2)
			rete=3;
		return  0 ;
	}
	return 1 ;
}

/**
	**函数功能说明：
	**车牌粗定位，倾斜校正，从候选项中挑选出最终项
	**参数说明：
	**resize:有要切除上下监控信息文字需要时，切除后的图
	**frame：原帧图
	**gray：灰度后的图
	**nvideo:视频号
	**count：单个视频里的帧数
	**exnum：单帧车牌数
	**cons：车牌结构
	**rete：定位返回值
*/
int ExtractPlate(Mat &resize,Mat &frame,Mat &gray,int nvideo,int count,int exnum,int cons,
	int &rete , vector<string>&Carplates , vector<string>&Platecolor ,vector<Rect>&Positions)
{

	int showflag=1;
	char sName[300] = {0};
	//Mat layerSizes=(Mat_<int>(1,3)<<48,48,39);
	//CvANN_MLP Nnw(layerSizes, CvANN_MLP::SIGMOID_SYM,0, 0 );
	//Nnw.load("mlp0923.xml");
	//Nnw.load("mlp_yellow.xml");
	bool graypic=graypicf(frame);
	int fn=192;
	int flagi=1;
	int pnum = 2 ;
	int width=frame.cols;
	CvSeq* plate = NULL ;
	vector <int> nPlate ;
	
	float widths=(float)resize.cols/(float)width;
	float heights=widths;	
	int height = width*resize.rows/(resize.cols);
	Mat gray1;
	//Mat ogray=cvCloneImage(gray);
	cv::resize(gray,gray1,Size(width,height));
	gray=gray1.clone();
	Mat resize1;
	//Mat oresize=cvCloneImage(resize);
	cv::resize(resize,resize1,Size(width,height));
	resize=resize1.clone();

	//********************定位开始****************
	vector<Rect> vecPos ;
int iNumofPlate = PlatePosition(  resize,frame,gray,&plate  ,nPlate,vecPos ,
		 count,pnum,cons, rete , graypic  ) ;

//for(int i=0; i<(plate? plate->total: 0); i++)
//{
//	Rect* r = (Rect*) cvGetSeqElem(plate, i);
//
//
//	Mat  matshow = gray(*r	);
//	imshow("222",matshow) ;
//	waitKey(0);
//	int x = 0 ;
//
//}



if (rete == 3)
{
     cons = 2 ;
}
if (iNumofPlate == 0)
{
	return 0 ;
}
	//********************定位结束****************
		int oritest = rete;
	//****************候选车牌处理****************
	int q0=0;	
	Mat origin=resize.clone();
	int flags=0;
	vector <CvPoint> cent;
	vector <Rect> rt;
	vector <string> finalout;
	vector <string> colorout;
	cent.clear();
	rt.clear();
	Mat rectplate=Mat(Size(resize.rows/2,resize.cols/2),CV_8UC3);
	//widths=widths*1.3;//旋转后图放大1.3倍
	for(int q=q0;q<pnum+q0;q++)
	{
		if(nPlate[q-q0]==-1)
			break;

		int  temp11 = nPlate[q] ;
		Rect* r = (Rect*) cvGetSeqElem(plate, q);
		if(r->width<2||r->height<2)
			continue;
		//记录原始位置=============
	
		
		Rect ori_r;
		ori_r.x=r->x;
		ori_r.y=r->y;
		ori_r.width=r->width;
		ori_r.height=r->height;
		//=========================
		if(r==NULL)
			continue;
		//最终的车牌区域，并将大小扩大:y放大1/4，x放大1/4
		//为防止车牌框的不完整=====================================================
	InitRect(r ,width ,height , rete ,flagi);
		//将框出的车牌抠出来=================================================
		if(r->x+r->width>gray.cols||r->x<0||r->width<=0
			||r->x>gray.cols||r->y+r->height>gray.rows)
		{
			printf("1298ROI region illegal\n");
			continue;
		}
		Rect roi(r->x, r->y, r->width, r->height);
		Mat carplate = gray(roi);
		Mat color_carplate = origin(roi);
		Mat matColorTemp = origin(ori_r) ;

		//imshow("findroi",color_carplate) ;
		//waitKey(1);


		//检测ROI_rect1区域内的颜色
		//第一次判色===========================================================================
		Rect ROI_rect1;
		//ROI_rect1.x = color_carplate.cols*0.3;
		//ROI_rect1.y = color_carplate.rows*0.3;
		//ROI_rect1.width = color_carplate.cols*0.4;
		//ROI_rect1.height = color_carplate.rows*0.2;
		////Rect roi(ROI_rect1.x, ROI_rect1.y, ROI_rect1.width, ROI_rect1.height);
		//Mat color_test = color_carplate(ROI_rect1);

		ROI_rect1.x = matColorTemp.cols*0.2;
		ROI_rect1.y = matColorTemp.rows*0.2;
		ROI_rect1.width = matColorTemp.cols*0.4;
		ROI_rect1.height = matColorTemp.rows*0.3;
		Mat color_test = matColorTemp(ROI_rect1);

		//int colnum = 1 ;
		//imshow("fff",matColorTemp ) ;
		//imshow("rec",color_test) ;
		//waitKey(1);
		int colnum=Test_color(color_test);//0:黑  1:蓝  2:白  3:黄 初步测试车牌颜色

		if(cons == 2&&colnum!=3)
			continue;
		if(colnum == -1)
				cout<<"2851"<<endl;
		//cout<<"color_test1: "<<colnum<<endl;
		if(graypic)
			colnum=4;
		if(cons==2)
		{
			if(colnum==0||colnum==1||colnum==4)
			{
				continue;
			}
		}
//========================================================================================
			//	continue;
//抠出的车牌resize一下==========================================================
		Mat imgGray;
		Mat imgColor;
		cv::resize(carplate, imgGray,Size(color_carplate.cols*scalew,color_carplate.rows*scaleh),CV_INTER_LINEAR);
		cv::resize(color_carplate, imgColor,Size(color_carplate.cols*scalew,color_carplate.rows*scaleh),CV_INTER_LINEAR);

//在原图中画出车牌区域==========================================================		
		if(q==q0)
		{
			rectangle(resize,cvPoint(r->x,r->y),cvPoint(r->x+r->height,r->y+r->width),Scalar(0,255,0),2);	
			cv::resize(resize,rectplate,rectplate.size());
		}
		else
		{
			rectangle(rectplate,cvPoint(r->x/2,r->y/2),cvPoint(r->x/2+r->height/2,r->y/2+r->width/2),Scalar(0,255,0),2);
		}	

///////////////////倾斜校正开始=================================================================================
		Rect  re_rect1 ;
		bool  whitebody = 0  ;
		 AngleRectify( imgGray, imgColor, re_rect1 ,count, exnum , colnum ,whitebody);

		//imshow("recAngle",imgGray ) ;
		// waitKey(0);
///////////////////倾斜校正结束=================================================================================
		 if (re_rect1.width < 4 ||re_rect1.height<4)
		 {
			 continue; 
		 }
	//	 continue; 
       //根据上面边缘图得到的ROI对车牌区域进行进一步细定位==================
			

	//	 re_rect1.y = re_rect1.y + re_rect1.height * 0.1 ;
	//	 re_rect1.height = 0.8 * re_rect1.height ;
		 Mat carplate1 = imgGray(re_rect1);
		 Mat color_carplate1 = imgColor(re_rect1);	

		//=============================================================

		//对车牌图resize到适合分割的大小
		Mat cp1;
		cv::resize(carplate1,cp1,Size(carplate1.cols*2.5,carplate1.rows*2.5));
		carplate1=cp1.clone();
		Mat ccp1;//=cvCreateImage(cvSize(carplate1.rows,carplate1.cols), carplate1->depth, 3);
		cv::resize(color_carplate1,ccp1,Size(carplate1.cols,carplate1.rows));
		color_carplate1=ccp1.clone();
	
		Mat img0=carplate1.clone();
		
		if(colnum!=4)//非灰度图重新判色，第二次判色
		{
			Rect CT_rect;
		//	CT_rect.x = color_carplate1.cols*0.3;//0.3
		//	CT_rect.y = color_carplate1.rows*0.3;//0.1
		//	CT_rect.width = color_carplate1.cols*0.5;//0.4
		//	CT_rect.height = color_carplate1.rows*0.6;
		////	Rect roi(CT_rect.x,CT_rect.y,CT_rect.width,CT_rect.height);
		//	Mat color_test2 = color_carplate1(CT_rect);
			
			CT_rect.x = matColorTemp.cols*0.25;//0.3
			CT_rect.y = matColorTemp.rows*0.25;//0.1
			CT_rect.width = matColorTemp.cols*0.5;//0.4
			CT_rect.height = matColorTemp.rows*0.6;
			Mat color_test2 = matColorTemp(CT_rect);

			//imshow("eee",color_test2 );
			//waitKey(1);

			colnum=Test_color(color_test2);//0:黑  1:蓝  2:白  3:黄 
			if(colnum == -1)
				cout<<"3309"<<endl;
		}

		//==============车牌分割步骤======================
		int f =0;
		int style=0;
		vector<Mat> charblock;
		charblock.clear();

		//imshow("org",carplate1);
		//waitKey(1) ;

		//colnum = 1 ;
		f=Precise_detect(carplate1,color_carplate1,whitebody,colnum,nvideo,count,exnum,cons,style,oritest,charblock);
		if(graypic==true)//如果灰度图，则默认为蓝色车牌
		{
			f=1;
		}
		//==========对返回结果做处理=========================
		char *color_flag;
		if(f==0)
		{
			color_flag="黑色";
			
		}
		if(f==1)
		{
			color_flag="蓝色";
			
		}
		if(f==2)
		{
			color_flag="白色";
			
		}
		if(f==3)
		{
			color_flag="黄色";
		}	
		//CvFont font; 
		//cvInitFont(&font,CV_FONT_HERSHEY_COMPLEX, 0.5, 0.5, 1, 1, 8); 
		if(f==-1)
		{
			exnum++;
			//cvPutText(rectplate,"segnum<7",cvPoint(/*50,50*/r->x/4,r->y/2-10),&font,CV_RGB(255,255,255));
			//if(showflag==1)
			//{		
			///*	sprintf(showfile,"show %d",q);		
			//	
			//	waitKey(0);			*/
			//}
			continue;
		}
		if(f==-2)
		{
			exnum++;
			//sprintf(showfile,"show %d",q);
			//
			//if(showflag==1)
			//{
			//	
			////	waitKey(0);			
			//}
			continue;
		}
		if(f==-3)
		{
			exnum++;
			//sprintf(showfile,"show %d",q);
			//
			//if(showflag==1)
			//{
			//	
			//	//waitKey(0);			
			//}
			continue;
		}
		//=====================================================
		Point pctr;//图像中心点，为后面判断某两个车牌位置是否过近，是否是同一个车牌
		pctr.x = ori_r.x+ori_r.width/2;
		pctr.y = ori_r.y+ori_r.height/2;
		//for (int k = 0 ; k<charblock.size(); k++ )
		//{
		//	char  aaa[256] ;
		// itoa(k , aaa , 10);		
		// strcat(aaa, ".jpg");
	 //	imwrite(aaa,charblock[k]);
		//}
	int 	x = 1 ;
		//字符块识别部分
		int res;
		if(charblock.size() != 0)
		{
			res=recogchar(finalout, nvideo,count,exnum,cons,
				style,f,q,color_flag,showflag,oritest,charblock);
		}
		if(res==-1)
			continue;
		else
		{
			cent.push_back(pctr);
			rt.push_back(ori_r);
			colorout.push_back(color_flag);
		}
	}
	//==========识别出结果进行筛选==============================\
	
int iResult = RefinePlate(frame ,origin, finalout,colorout,cent ,
		rt, rete, Carplates, Platecolor,Positions);
	
	if ( iResult == 0 )
	{
          return -1 ;   
	}
  int   x =1 ;
	//===============================================
	cvClearSeq(plate);
return exnum;
}

/**
	**函数功能说明：
	**遍历图片文件夹中所有文件，识别图片中车牌号
	**参数说明：
	**SPath:文件夹路径
*/
int detect_pic(char SPath[],vector<string>&Carplates , vector<string>&Platecolor, vector<Rect> &Positions  )
{
	int i=0;
	int count=0;
	
	   	  // 构造完整路径
		char filename[LEN];
		int scale=6;
		//wsprintf(filename,"%s\\%s", SPath,FindData.cFileName);
		Mat img0 = imread(SPath/*,CV_LOAD_IMAGE_GRAYSCALE*/);

		if(!img0.data)        // 判断图片调入是否成功
		{
			return 0; 
		}
		Mat frame = img0.clone(); 
		    
		double t = (double)cvGetTickCount();
		Rect ROI_rect;
		ROI_rect.x = 0;
		ROI_rect.y = 0/*frame.cols*0.250.660.250.50.6*/;
		ROI_rect.width = frame.cols;
		ROI_rect.height = frame.rows/size;	
		
		Mat img = frame(ROI_rect);
		Mat resize=img.clone();
		Mat resize1=resize.clone();
					
		Mat gray;
		cvtColor(resize, gray, CV_BGR2GRAY);
		Mat gray1=gray.clone();
		Mat frame1=frame.clone();
			
	//	printf("\n处理第%d帧中\n", count);
		cvClearMemStorage( storage );
		char sName[200]={0};
		sprintf(sName, "simple_result-pic-real/%dimg.jpg", count);//调试用
		imwrite(sName,frame1);	
		Mat res(resize);
		Mat fra(frame);
		Mat gra(gray);
		int exnum=0;
		int retest=0;
		if(retest==0)//ori
		{
			exnum = ExtractPlate(res, fra, gra, i,count,exnum,1,retest , Carplates , Platecolor ,Positions );			
		}
		if( retest == 1 )//lbp
		{
			gra = gray1.clone();		
			res = resize1.clone();		
			fra=frame1.clone();
		//	exnum=ExtractPlate(res,fra,gra,i,count,exnum,1,retest,Carplates , Platecolor,Positions );
			
		}
		if(retest==2)//3100
		{
			gra=gray1.clone();
			res=resize1.clone();
			
			fra=frame1.clone();
			//exnum=ExtractPlate(res,fra,gra,i,count,exnum,1,retest,Carplates , Platecolor,Positions );
			
		}
		if(retest==3)//bus
		{		
			gra=gray1.clone(); 
			res=resize1.clone();
			
			fra=frame1.clone();
		//	exnum=ExtractPlate(res,fra,gra,i,count,exnum,2,retest,Carplates , Platecolor,Positions );
			
		}

		double t0 = (double)cvGetTickCount();
		res.release();
		fra.release();
		gra.release();
		resize.release();
		frame.release();
		gray.release();
		resize1.release();
		frame1.release();
		gray1.release();
		
	return 0;
}
/**
	**函数功能说明：
	**遍历视频文件夹中所有文件，识别视频中每一帧车牌号
	**参数说明：
	**SPath:文件夹路径
*/
int detect_video(LPCSTR SPath , vector<string>&Carplates , vector<string>&Platecolor ,vector<Rect> &Positions )
{
		int scale=6;
		int i = 0 ;
		//wsprintf(filename,"%s\\%s", SPath,FindData.cFileName);
		VideoCapture capture(SPath);
		if(!capture.isOpened())
		{
			cout<<"Cannot find video file!"<<endl;
			return 0;
		}
		Mat frame;
		int count=0;

////////////////////////////////////////////////////
		while(capture.read(frame))
		{
			count++;;
			double t = (double)cvGetTickCount();
			Rect ROI_rect;
			ROI_rect.x = 0;
			ROI_rect.y = 0/*frame.cols*0.250.660.250.50.6*/;
			ROI_rect.width = frame.cols;
			ROI_rect.height = frame.rows/size;	
		
			Mat img = frame(ROI_rect);
			Mat resize=img.clone();
			Mat resize1=resize.clone();
			Mat gray;
			cvtColor(resize, gray, CV_BGR2GRAY);
			Mat gray1=gray.clone();
			Mat frame1=frame.clone();			
		//	printf("\n处理第%d帧中\n", count);
			cvClearMemStorage( storage );

			Mat res(resize);
			Mat fra(frame);
			Mat gra(gray);
			int exnum=0;
			int retest=0;
			string fname;
			stringstream ss;
			ss << i;
			ss >> fname;
			if(retest==0)//ori
			{
				exnum=ExtractPlate(res,fra,gra,i,count,exnum,1,retest,Carplates , Platecolor , Positions);
			}
			if(retest==1)//lbp
			{
				gra=gray1.clone();	
				res=resize1.clone();	
				fra=frame1.clone();
				exnum=ExtractPlate(res,fra,gra,i,count,exnum,1,retest,Carplates , Platecolor, Positions );
			}
			if(retest==2)//3100
			{
				gra=gray1.clone();
				res=resize1.clone();
				fra=frame1.clone();
				exnum=ExtractPlate(res,fra,gra,i,count,exnum,1,retest,Carplates , Platecolor, Positions );
			}
			if(retest==3)//bus
			{
				//cout<<"bus:"<<endl;
				gra=gray1.clone(); 
				res=resize1.clone();
				fra=frame1.clone();
				exnum=ExtractPlate(res,fra,gra,i,count,exnum,2,retest,Carplates , Platecolor, Positions );
			}						
			double t0 = (double)cvGetTickCount();
			
			res.release();
			fra.release();
			gra.release();
			resize.release();
			frame.release();
			gray.release();
			resize1.release();
			frame1.release();
			gray1.release();
		}
	
	return 0;
}
/**
	**函数功能说明：
	**处理图片数据
	**参数说明：
	**无
*/
int fromPicture( char Filepath[],vector<string>&Carplates , vector<string>&Platecolor ,vector<Rect> &Positions  )
{
	cascade = (CvHaarClassifierCascade*)cvLoad(cascade_name, 0, 0, 0);
	if(!cascade)
	{
		printf("ERROR: Could not load classifier cascade\n");
		return 0;
	}
	if(!cascade1.load(cascade_name1))
	{
		printf("ERROR: Could not load classifier double cascade\n");
		return 0;
	}
	if(!cascade2.load(cascade_name2))
	{
		printf("ERROR: Could not load classifier double cascade\n");
		return 0;
	}
	if(!cascade3.load(cascade_name3))
	{
		printf("ERROR: Could not load classifier double cascade\n");
		return 0;
	}
	storage = cvCreateMemStorage(0);
	//IplConvKernel *elem_open_vertical = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT, 0);
	char filename[256] ;
	memset(filename , 0 ,256 * sizeof(char));
	strcpy(filename,Filepath ) ; 
	//Mat yest1 = imread(filename);
	//Mat test2 = imread("D:\\测试\\车牌稳定测试\\问题图片\\一.jpg");
	detect_pic(filename,Carplates , Platecolor,  Positions );
//	detect_pic("F:\\交通视频\\原型测试车牌\\辽");
	//printf("%s\n",Carplates);
	cvReleaseMemStorage(&storage);
	cvReleaseHaarClassifierCascade(&cascade);
	//printf("处理结束!\n");
	//system("pause");
	//float auc=(float)cor/(float)500;
	//cout<<auc<<endl;
	return 0;
	cvWaitKey(3);
}
/**
	**函数功能说明：
	**处理视频数据
	**参数说明：
	**无
*/
int fromVideo(  char Filepath[] ,vector<string>&Carplates , vector<string>&Platecolor ,vector<Rect> &Positions)
{
	cascade = (CvHaarClassifierCascade*)cvLoad(cascade_name, 0, 0, 0);
	if(!cascade)
	{
		printf("ERROR: Could not load classifier cascade\n");
		return 0;
	}
	if(!cascade1.load(cascade_name1))
	{
		printf("ERROR: Could not load classifier double cascade\n");
		return 0;
	}
	if(!cascade2.load(cascade_name2))
	{
		printf("ERROR: Could not load classifier double cascade\n");
		return 0;
	}
	if(!cascade3.load(cascade_name3))
	{
		printf("ERROR: Could not load classifier double cascade\n");
		return 0;
	}
	
	storage = cvCreateMemStorage(0);
	IplConvKernel *elem_open_vertical = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT, 0);
	char videoName[300] = {0};
	char sName[300] = {0};
	char filename[256]  ;
	char keyword1[] = "aviname" ;
	char target[256] ;

		strcpy(filename,Filepath ) ; 

	//IniRead(keyword1 ,target ) ;
	//strcpy(filename, target) ;
	
	detect_video(filename,Carplates , Platecolor, Positions);

	cvReleaseMemStorage(&storage);
	cvReleaseHaarClassifierCascade(&cascade);
	printf("处理结束!\n");
	system("pause");
	
	return 0;
	cvWaitKey(3);
}

bool CarPlateRecognition(char Filepath[], vector<string>&Carplates , vector<string>&Platecolor, vector<Rect> &Positions, bool model )
{
	if (model == 0)
	{
	fromVideo(Filepath , Carplates ,Platecolor,Positions );
	}
	else
	{
	fromPicture(Filepath ,  Carplates ,Platecolor, Positions );
	}
	return 0 ;
}

 bool dotest()
	{ 
		bool n=1;
	//cout<<"video: 0\nPicture: 1"<<endl;
	//cin>>n;
	char path[256]  ;	
	char keyword1[] = "aviname" ;
	char keyword2[] = "filename" ;

	char target[256] ;
	vector<string>  Carplate ;
	vector<string>  ColorOfPlate ;
	vector<Rect>  Positions ;
	if (n == 0)
	{
	IniRead(keyword1 ,target ) ;
	strcpy(path, target) ;
	}
	else
	{
		IniRead(keyword2 ,target ) ;
		strcpy(path, target) ;
	}
	 CarPlateRecognition( path,Carplate ,ColorOfPlate ,Positions, 1 ) ;
	return 0;
}

 bool dotest1(int num )
 { 
	// printf("这是第%d次循环\n",num);
	 char keyword1[] = "aviname" ;
	 char keyword2[] = "filename" ;

	 char target[256] ;
	 vector<string>  Carplate ;
	 vector<string>  ColorOfPlate ;
	 vector<Rect>  Positions ;

	 _finddata_t file;
	 int k;
	 long HANDLE;
	  HANDLE = _findfirst( "C:\\Users\\Administrator\\Desktop\\*.jpg", &file );
	  int count = 1 ;
	  cout << file.name << endl;

	  char path[256] ="C:\\Users\\Administrator\\Desktop\\" ;
	  char savepath[256] ;
	  strcpy(savepath ,path);
	  strcat(savepath,"问题图片\\");

	  strcat(path , file.name);
	  strcat(savepath,file.name);
	 
	  CarPlateRecognition( path,Carplate ,ColorOfPlate ,Positions, 1 ) ;
	  printf("这是第%d次循环,这是第%d张图片\n",num,count);
	  Mat  matShow = imread(path);
	  Mat  matresize = Mat(matShow.rows/2 , matShow.cols/2 ,matShow.type() ) ;
		  resize(matShow, matresize, matresize.size(), 0, 0, 0);  
		 imshow("dad",matresize )  ;
	  waitKey(0);
	  Carplate.clear();
	  ColorOfPlate.clear();
	  Positions.clear();	

	 while( !_findnext( HANDLE, &file ) )
	 {    
		 count++ ;
		 cout << file.name << endl;
			
		 char path[256] ="C:\\Users\\111\\Desktop\\车牌识别\\未识别\\处理中\\" ;
		 char savepath[256] ;
		 strcpy(savepath ,path);
		 strcat(savepath,"问题图片\\");

		 strcat(path , file.name);
		  strcat(savepath,file.name);
		  
		 CarPlateRecognition( path,Carplate ,ColorOfPlate ,Positions, 1 ) ;
		 printf(" 这是第%d次循环,这是第%d张图片\n",num,count);
		 Mat  matShow = imread(path);
		 Mat  matresize = Mat(matShow.rows/2 , matShow.cols/2 ,matShow.type() ) ;
		 resize(matShow, matresize, matresize.size(), 0, 0, 0);  
		 imshow("dad",matresize )  ;
		 waitKey(0);
		 Carplate.clear();
		 ColorOfPlate.clear();
		 Positions.clear();	;
		 /* Mat  fine = imread(path);
		 imwrite( savepath  ,fine);
		 remove(path);*/
		 int x = 0 ;	
	 }
	 _findclose( HANDLE );
	 return 0;
 }

 bool dotest2(int num )
 { 
	 char keyword1[] = "aviname" ;
	 char keyword2[] = "filename" ;
	 char keyword3[] = "savepath" ;
	 char keyword4[] = "savepic" ;

	 char path[256]  ;
	 char savepath[256] ;
	 char pathfile[256] ;
	 char target[256] ;

	 bool bsave ;

	 vector<string>  Carplate ;
	 vector<string>  ColorOfPlate ;
	 vector<Rect>  Positions ;
	 memset( path , 0 , sizeof(char)*256  );
	 memset( savepath , 0 , sizeof(char)*256  );
	 memset( pathfile , 0 , sizeof(char)*256  );

	 IniRead( keyword2 , target );

	 strcpy( path , target );

	 IniRead( keyword4 , target );

	 bsave = atoi(target);
	 if (bsave == 1 )
	 {
		 IniRead( keyword3 , target );
		 strcpy( savepath ,target) ;
	 }
	 strcpy(pathfile,path);
	 strcat(pathfile , "\\"); 
	 strcat(pathfile,"*.jpg");
	 _finddata_t file;
	 int k;
	 long HANDLE;
	 HANDLE = _findfirst( pathfile, &file );
	 int count = 0 ;

	 while( !_findnext( HANDLE, &file ) )
	 {    
		 count++ ;
		 cout << file.name << endl;
		 char temppath[256]  ;
		 char tempsavepath[256] ;

		 strcpy(temppath,path);
		 strcat(temppath , "\\"); 
		 strcat(temppath , file.name );

		 CarPlateRecognition( temppath,Carplate ,ColorOfPlate ,Positions, 1 ) ;
		 printf(" 这是第%d次循环,这是第%d张图片\n",num,count);

		 if (bsave == 1 )
		 {		
			 strcpy(tempsavepath ,savepath );
			 strcat(tempsavepath , "\\"); 
			 string rec ;
			 Mat  fine = imread(temppath);
			 rec  = tempsavepath ;
			 if (Carplate.size()== 0 )
			 {
				 rec = rec + "无车牌";

				 char time[10] ;	
				 itoa(count ,time ,10 );

				 rec = rec + time ; 
			 }
			 else
			 {
				 for (int i = 0 ; i<Carplate.size() ; i++)
				 {
					 rec = rec + Carplate[i] + ColorOfPlate[i]+"II" ;
				 }
			 }
			 rec = rec + ".jpg";
			 imwrite( rec  ,fine);
			 // remove(path);
		 }
		 int x = 0 ;
		 Carplate.clear();
		 ColorOfPlate.clear();
		 Positions.clear();	;
	 }
	 _findclose( HANDLE );
	 return 0;
 }

 /*LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)
 {
	 ::MessageBox(0,_T("Error"),_T("error"),MB_OK);   
	 HANDLE lhDumpFile = CreateFile("DumpFile.dmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL ,NULL);
	 MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
	 loExceptionInfo.ExceptionPointers = ExceptionInfo;
	 loExceptionInfo.ThreadId = GetCurrentThreadId();
	 loExceptionInfo.ClientPointers = TRUE;

	 MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),lhDumpFile, MiniDumpNormal, &loExceptionInfo, NULL, NULL);


	 CloseHandle(lhDumpFile);

	 ::ShellExecuteW(NULL, NULL, L"BugReport.exe", NULL, NULL, SW_NORMAL);
	 TRACE("===========================");
	 printf("=====================");
	 Sleep(30000);

	 return EXCEPTION_CONTINUE_SEARCH;
 }*/
 int main()
 {
	// SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
	int  count = 0 ;
	while(1){
		 count++ ;

	 dotest1(count) ;
	 }
	  return 0 ;
 }

 //////////////////////车牌定位///////////////////////
 //  cvClearSeq(plate);
 //return 0 ;
 //double *cannyval;
 //double max;
 //IplImage *igray = &IplImage(gray);
 ////用cascade_final_14.xml分类器定位==============================
 //if(rete==0)
 //{
 //	if(cons == 1)
 //	{
 //		if(flagi==0)
 //		{
 //			plate = cvHaarDetectObjects(igray, cascade, storage, 1.2, 0, 8, cvSize(60, 15));//1.2,1,2
 //		}
 //		else
 //		{
 //			plate = cvHaarDetectObjects(igray, cascade, storage, 1.1, 2, 0, cvSize(60, 15));
 //		}
 //	}
 //	
 //	nPlate.clear();
 //	for(int i=0;i<=100;i++)
 //	{
 //		nPlate.push_back(-1);
 //	}
 //
 //	//double *cannyval;
 //	//cannyval=(int *)malloc(sizeof(int)*plate->total); 
 //	cannyval=(double *)malloc(sizeof(double)*plate->total);
 //	int cc=0;
 //	max=realplate(plate,gray,cannyval,resize,count,cc,graypic);
 //	//printf("%lf\n",max);

 //	//=========================================
 //	//在原型测试时
 //	//检测出多个车牌
 //	//如果车牌相似度第二大的车牌的相似值与最大的差距较大
 //	//直接设定flag_reloc=1
 //	//==========================================
 //	int flag_reloc=0;
 //	double *cval0;
 //	if(cons==1)
 //	{
 //		//cval=(int *)malloc(sizeof(int)*plate->total);  
 //		cval0=(double *)malloc(sizeof(double)*plate->total);   
 //		for(int i=0; i<(plate? plate->total: 0); i++)
 //		{
 //			cval0[i]=cannyval[i];

 //		}
 //		if(plate->total>1)
 //		{
 //			sort(cval0,cval0+plate->total,cmp);
 //			if(cval0[1]<cval0[0]*0.7)
 //				flag_reloc=1;
 //		}
 //		free(cval0);
 //	}


 //	//return 0;
 //	bool term=false;
 //	if(cons==1)
 //	{
 //		if(flagi==0)
 //		{
 //			term=max <18.5||plate->total <= 1||flag_reloc==1;
 //		}
 //		else
 //		{
 //			term=max <13;
 //		}
 //	}
 //	if(cons==2)
 //		term=max <18;


 //	//========================================================
 //	//满足term条件的
 //	//从-20°~20°旋转检测
 //	if(term)
 //	{
 //		free(cannyval);
 //	
 //		Mat dst = Mat(gray.size(), CV_8UC1);
 //		Mat dstc = Mat(gray.size(), CV_8UC3);
 //		
 //		vector<CvSeq*> v_plate;
 //		//vector<Mat> v_dst;
 //		v_plate.clear();
 //		//v_dst.clear();
 //		Mat dstf = Mat(gray.size(), CV_8UC1);
 //		Mat dstcf = Mat(gray.size(), CV_8UC3);
 //		int cnt=0;
 //		int pos=0;
 //		int angle0=0;
 //		double maxvalue=0;	
 //		for(int k=-20;k<=20;k+=2)
 //		{				
 //			if(k==0)
 //				continue;
 //			//printf("倾斜角度：%f\n\n",angle);
 //			Mat rot_mat = Mat(2,3,CV_32FC1);
 //			Point2f center = Point2f(gray.cols/2,gray.rows/2);
 //			double scale = 1.0;
 //			rot_mat=getRotationMatrix2D(center,k,scale);
 //			warpAffine(gray,dst,rot_mat,dst.size());
 //			warpAffine(resize,dstc,rot_mat,dstc.size());
 //			IplImage * idst = &IplImage(dst);
 //			if(cons==1)
 //			{
 //				if(flagi==0)
 //				{
 //					plate = cvHaarDetectObjects(idst, cascade, storage, 1.1, 1, 2/*CV_HAAR_DO_CANNY_PRUNING*/, cvSize(60,15));
 //				}
 //				else
 //				{
 //					plate = cvHaarDetectObjects(idst, cascade, storage, 1.1, 2, 0/*CV_HAAR_DO_CANNY_PRUNING*/, cvSize(60,15));
 //				}
 //			}
 //			
 //			cannyval=(double *)malloc(sizeof(double)*plate->total); 
 //			v_plate.push_back(plate);
 //			//v_dst.push_back(dst);
 //			cc++;
 //			double max2=realplate(plate,dst,cannyval,dstc,count,cc,graypic);
 //			//printf("\n%d°%lf\n",k,max2);
 //			if(plate->total!=0/*&&max2>30*/)//==========
 //			{	
 //				if(max2>maxvalue)
 //				{
 //					maxvalue=max2;
 //					pos=cnt;
 //					angle0=k;
 //					dstf=dst.clone();
 //					dstcf=dstc.clone();
 //					max=max2;
 //				}
 //			}
 //			cnt++;
 //			free(cannyval);
 //		}	
 //		//printf("best: %d°\n",angle0);
 //		plate=v_plate[pos];
 //		v_plate.clear();
 //		//exnum++;
 //		//return 0;
 //		dst=dstf.clone();
 //		dstc=dstcf.clone();
 //	
 //		
 //		//还原最大车牌相似值的那个车牌
 //		cannyval=(double *)malloc(sizeof(double)*plate->total); 
 //		cc++;
 //		double mmax=realplate(plate,dst,cannyval,dstc,count,cc,graypic);
 //		Mat rot_mat = Mat(2,3,CV_32FC1);
 //		CvPoint2D32f center = cvPoint2D32f(gray.cols/2,gray.rows/2);
 //		double scale = 1.0;
 //		rot_mat=getRotationMatrix2D(center,angle0,scale);
 //		Mat resize_t=resize.clone();
 //		Mat gray_t=gray.clone();
 //	
 //		warpAffine(resize,resize_t,rot_mat,resize_t.size());
 //		warpAffine(gray,gray_t,rot_mat,gray_t.size());
 //	
 //		resize=resize_t.clone();
 //		gray=gray_t.clone();
 //		if(plate->total==0)
 //		{
 //			sprintf(sName, "simple_result/frame_%d.jpg", count);
 //			sprintf(sName, "wrong/%d_nonloc.jpg", count);//调试用
 //			imwrite(sName,frame);
 //			printf("no car plate has been found!\n");//调试用
 //			cvClearSeq(plate);
 //			free(cannyval);
 //			cannyval=NULL;

 //			rete=1;
 //			return 0;
 //		}
 //	}
 //}

 ////用cascade_new.xml分类器定位==============================
 //else if(rete==1)
 //{
 //	vector<Rect> rects;  
 //	if(cons == 1)
 //	{
 //		if(flagi==0)
 //		{
 //			rects.clear(); 
 //			cascade3.detectMultiScale(gray,rects,1.1,2,0/*0|CV_HAAR_DO_CANNY_PRUNING*/,Size(60, 20));
 //			plate=cvCreateSeq(0, sizeof(CvSeq), sizeof(Rect), storage);
 //			for(int i=0;i<rects.size();i++)
 //				cvSeqPush(plate, &rects[i] );
 //			//plate = cvHaarDetectObjects(gray, cascade, storage, 1.2, 0, 8, cvSize(60, 15));//1.2,1,2
 //		}
 //		else
 //		{
 //			rects.clear(); 
 //			cascade3.detectMultiScale(gray,rects,1.1,2,0|CV_HAAR_DO_CANNY_PRUNING,Size(60, 20));
 //			plate=cvCreateSeq(0, sizeof(CvSeq), sizeof(Rect), storage);
 //			for(int i=0;i<rects.size();i++)
 //				cvSeqPush(plate, &rects[i] );
 //			//plate = cvHaarDetectObjects(gray, cascade, storage, 1.1, 2, 0, cvSize(60, 15));
 //		}
 //	}
 //	
 //
 //	nPlate.clear();
 //	for(int i=0;i<=100;i++)
 //	{
 //		nPlate.push_back(-1);
 //	}
 //
 //	//cannyval=(int *)malloc(sizeof(int)*plate->total); 
 //	cannyval=(double *)malloc(sizeof(double)*plate->total);
 //	int cc=0;
 //	max=realplate_new(plate,gray,cannyval,resize,count,cc,graypic);
 //	
 //		if(plate->total==0)
 //		{
 //			sprintf(sName, "simple_result/frame_%d.jpg", count);
 //			sprintf(sName, "wrong/%d_nonloc.jpg", count);//调试用
 //			imwrite(sName,frame);
 //			printf("no car plate has been found!\n");//调试用
 //			cvClearSeq(plate);
 //			free(cannyval);
 //			cannyval=NULL;

 //			rete=2;
 //			return 0;
 //			//sprintf(sName, "simple_result/frame_%d.jpg", count);//调试用
 //			////imwrite(sName,frame);
 //			///*cvReleaseImage(&dst);*/
 //			//sprintf(sName, "wrong/%d_nonloc.jpg", count);//调试用
 //			////imwrite(sName,frame);
 //			//printf("no car plate has been found!\n");//调试用
 //			//cvClearSeq(plate);
 //			//return 0;
 //		}
 //		else 
 //		{
 //			int num=plate->total;
 //			for(int i=1;i<num;i++)
 //				cvSeqRemove(plate,1);					
 //		}
 //}

 //else if(rete==2)//cascade_6000_lbp.xml
 //{
 //	vector<Rect> rects;  
 //	if(cons == 1)
 //	{
 //		if(flagi==0)
 //		{
 //			rects.clear(); 
 //			cascade2.detectMultiScale(gray,rects,1.1,2,0/*0|CV_HAAR_DO_CANNY_PRUNING*/,Size(60, 20));//1 2
 //			plate=cvCreateSeq(0, sizeof(CvSeq), sizeof(Rect), storage);
 //			for(int i=0;i<rects.size();i++)
 //				cvSeqPush(plate, &rects[i] );
 //			//plate = cvHaarDetectObjects(gray, cascade, storage, 1.2, 0, 8, cvSize(60, 15));//1.2,1,2
 //		}
 //		else
 //		{
 //			rects.clear(); 
 //			cascade2.detectMultiScale(gray,rects,1.1,2,0|CV_HAAR_DO_CANNY_PRUNING,Size(60, 20));
 //			plate=cvCreateSeq(0, sizeof(CvSeq), sizeof(Rect), storage);
 //			for(int i=0;i<rects.size();i++)
 //				cvSeqPush(plate, &rects[i] );
 //			//plate = cvHaarDetectObjects(gray, cascade, storage, 1.1, 2, 0, cvSize(60, 15));
 //		}
 //	}
 //
 //	nPlate.clear();
 //	for(int i=0;i<=100;i++)
 //	{
 //		nPlate.push_back(-1);
 //	}
 //
 //	//cannyval=(int *)malloc(sizeof(int)*plate->total); 
 //	cannyval=(double *)malloc(sizeof(double)*plate->total);
 //	int cc=0;
 //	max=realplate_new(plate,gray,cannyval,resize,count,cc,graypic);
 //	//printf("%lf\n",max);
 //	
 //		if(plate->total==0)
 //		{
 //			sprintf(sName, "simple_result/frame_%d.jpg", count);//调试用
 //			//imwrite(sName,frame);
 //			/*cvReleaseImage(&dst);*/
 //			sprintf(sName, "wrong/%d_nonloc.jpg", count);//调试用
 //			//imwrite(sName,frame);
 //			printf("no car plate has been found!\n");//调试用
 //			cvClearSeq(plate);
 //			free(cannyval);
 //			cannyval=NULL;

 //			//rete=2;
 //			return 0;
 //		}
 //}
 //
 //else if(rete == 3)//cascade_double.xml
 //{
 //		vector<Rect> rects;
 //		rects.clear();
 //		cascade1.detectMultiScale(gray,rects,1.1,1,2,Size(60, 30));

 //		for (int i=0;i<rects.size();i++)
 //		{
 //			if(rects[i].y < gray.cols / 4)
 //				rects[i] = Rect(0,0,1,1);
 //		}

 //		plate = cvCreateSeq(0, sizeof(CvSeq), sizeof(Rect), storage);
 //		for(int i=0;i<rects.size();i++)
 //			cvSeqPush(plate, &rects[i] );
 //		//plate = cvHaarDetectObjects(gray, cascade1, storage, 1.1, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(60, 30));//检测双排结构车牌，尺寸待改
 //		nPlate.clear();
 //		for(int i=0;i<=100;i++)
 //		{
 //			nPlate.push_back(-1);
 //		}
 //
 //		cannyval=(double *)malloc(sizeof(double)*plate->total);
 //		int cc=0;
 //		max=realplate_new(plate,gray,cannyval,resize,count,cc,graypic);
 //		//printf("%lf\n",max);
 //}

 //if(plate->total==1)
 //{
 //	nPlate[0]=0;
 //}

 ////========================================================================
 ////将用realplate算出的cannyval值排序
 ////取前70%
 ////将序号位置存在nPlate里
 //
 //if(plate->total>1)
 //{
 //	int j=0;
 //	//int *cval;
 //	double *cval;
 //	//cval=(int *)malloc(sizeof(int)*plate->total);  
 //	cval=(double *)malloc(sizeof(double)*plate->total);   
 //	for(int i=0; i<(plate? plate->total: 0); i++)
 //	{
 //		cval[i]=cannyval[i];

 //	}
 //	sort(cannyval,cannyval+plate->total,cmp);
 //	for(int i=0;i<plate->total;i++)
 //	{
 //		if(cannyval[i]<0.7*max)
 //		{
 //			pnum=i;
 //			break;
 //		}
 //	}
 //	if(pnum<2)
 //		pnum=2;
 //	if(pnum>4)
 //		pnum=4;
 //	if(flagi==1)
 //		pnum=7;
 //	for(int j=0;j<pnum;j++)
 //	{
 //		for(int i=0; i<(plate? plate->total: 0); i++)
 //		{
 //			if(cval[i]==cannyval[j])//cval是未排序原集，cannyval是已排序集
 //			{
 //				nPlate[j]=i;
 //				//printf("%lf\n ",cannyval[j]);
 //			}
 //		}
 //	}
 //	//printf("\n");
 //	free(cval);
 //	free(cannyval);
 //	cannyval=NULL;
 //}
 //if(cannyval)
 //{
 //	free(cannyval);
 //	cannyval=NULL;
 //}


///////////////////////////////////倾斜矫正////////////////////////////
//		int whitebody=0;
//		Mat thresh = extractEdge(carplate0,color_carplate0,colnum,whitebody);//得到去杂后的车牌二值化的图
//
//		Mat carp = carplate0.clone();//原始图
//		Mat dst = carplate0.clone();//结果图
//		Mat dst_col = color_carplate0.clone();//结果彩图
//		Mat bw;//水平校正用边缘图
//		Mat bw1;//错切校正用边缘图
//		
//	
//
//		//得边缘图=======================================
//		char filename2[100]="";	
//		CvMemStorage* storage1 = cvCreateMemStorage(0);
//		CvSeq* lines = 0;
//		int temp,maxi=0;
//		int k,j=0;
//		bw = Mat(thresh.size(),CV_8UC1);	
//		double low = 0.0, high = 0.0; 
//		IplImage *ithresh = &(IplImage(thresh));
//		AdaptiveFindThreshold(ithresh, &low, &high,3);
//		Canny(thresh, bw, low, high); 
//		//cvAdaptiveThreshold(thresh, bw, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 3, 5);
//		//cvCanny(thresh, bw, 50, 150); 
//		sprintf(filename2,"simple_result03cor2/%d_%d_%d_canny.jpg",nvideo,count,q+exnum);
//		imwrite(filename2,bw);
//		//==================================================
//		
//		//==========求边缘图白色像素的高和宽=======================
//		int left=0,right=bw.cols-1;
//		int flagp=0;
//		for(int w=0;w<bw.cols;w++)
//		{
//			for(int h=0;h<bw.rows;h++)
//			{
//				uchar s=bw.at<uchar>(h,w);
//				if(s == 255)
//				{
//					left=w;
//					flagp=1;
//					break;
//				}
//			}
//			if(flagp==1)
//			{
//				flagp=0;
//				break;
//			}
//		}
//
//		for(int w=bw.cols-1;w>0;w--)
//		{
//			for(int h=0;h<bw.rows;h++)
//			{
//				uchar s=bw.at<uchar>(h,w);
//				if(s == 255)
//				{
//					right=w;
//					flagp=1;
//					break;
//				}
//			}
//			if(flagp==1)
//			{
//				flagp=0;
//				break;
//			}
//		}
//
//		int t_width=right-left+1;
//		int up=0,down=bw.rows-1;
//		for(int h=0;h<bw.rows;h++)
//		{
//			for(int w=0;w<bw.cols;w++)
//			{
//				uchar s= bw.at<uchar>(h,w);
//				if(s == 255)
//				{
//					up=h;
//					flagp=1;
//					break;
//				}
//			}
//			if(flagp==1)
//			{
//				flagp=0;
//				break;
//			}
//		}
//		for(int h=bw.rows-1;h>0;h--)
//		{
//			for(int w=0;w<bw.cols;w++)
//			{
//				uchar s=bw.at<uchar>(h,w);
//				if(s ==255)
//				{
//					down=h;
//					flagp=1;
//					break;
//				}
//			}
//			if(flagp==1)
//			{
//				flagp=0;
//				break;
//			}
//		}
//		int t_height=down-up+1;
//		if(t_height<=4||t_width<=4)//边缘图结果无用，跳过
//		{
//			exnum++;
//			continue;
//		}
//		//==============================================================
//	
//	//	continue; 
//
//		//==============================================
//		//====水平校正开始==============================
//		
//		int getnoline=0;
//		if(t_width<0.2*bw.cols&&t_height>bw.rows*0.8)
//			getnoline=1;
//		Rect re_rect;
//		re_rect.x=left;
//		re_rect.y=up;
//		re_rect.width=t_width;
//		re_rect.height=t_height;
//		Rect re_rect1;
//		re_rect1.x=0;
//		re_rect1.y=up;
//		re_rect1.width=bw.cols;
//		re_rect1.height=t_height;
//		if((t_width>bw.cols*0.8)&&(t_height<0.47*bw.rows)||(t_height<0.3*bw.rows)&&(down==bw.rows-1)||(t_height<0.3*bw.rows)&&(up==0))//顶部或底部被腐蚀掉
//		{
//			re_rect1.y=0;
//			re_rect1.height=bw.rows;
//		}	
//		
//		Mat thresh1=thresh.clone();
//		
//		Mat color= Mat(bw.size(),CV_8UC3);
//		cvtColor( bw, color, CV_GRAY2BGR );
//		Mat bw_backup=bw.clone();
//		IplImage *ibw=&(IplImage(bw));
//		lines = cvHoughLines2( ibw, storage1, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180,t_width/4 /*bw.rows/4*/, t_width/5+1,t_width/5+1/*10, bw.rows/10*/);
//		int threshval;
//
//
//		//====对于一些不能直接检测出正确水平直线的图====
//		//====直接用水平梯度图来检测水平直线============
//		if(lines->total==0||getnoline)
//		{
//			Mat H_img=Mat (carp.size(),CV_8UC1);
//			float horizontalMatrix[] = {
//				1,2,1,
//				0,0,0,
//				-1,-2,-1,
//			 };
//			Mat M_horizontal_sobel = Mat(3, 3, CV_32F, horizontalMatrix);
//			filter2D(carp, H_img, 8,M_horizontal_sobel,Point(-1, -1));
//			Mat thresh_h=carp.clone();
//			IplImage *iH_img = &(IplImage(H_img));
//			threshval = getThreshVal_Otsu_8u(iH_img);
//			threshold(H_img, thresh_h, threshval, 255, CV_THRESH_BINARY_INV);
//			bw=thresh_h.clone();
//			low = 0.0, high = 0.0; 
//			IplImage *ithresh_h=&(IplImage(thresh_h));
//			AdaptiveFindThreshold(ithresh_h, &low, &high,3);
//			Canny(thresh_h, bw, low, high); 
//			cvClearSeq(lines);
//			ibw=NULL;
//			ibw=&(IplImage(bw));
//			lines = cvHoughLines2( ibw, storage1, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180,t_width/5 /*bw.rows/4*/, t_width/5+1,t_width/5+1/*10, bw.rows/10*/);
//			color= Mat(bw.size(),CV_8UC3);
//			cvtColor( bw, color, CV_GRAY2BGR );
//		}
//
//		//==========水平角度有筛选==========//
//		//========筛选横坐标差值最大的线
//		vector<int> vline,vline1;
//		int u=0;
//		vector <int> Lcand;
//		int mini=1000;
//		double anglea=0;
//
//		for( k = 0; k < lines->total; k++ )
//		{
//			Point* line = (Point*)cvGetSeqElem(lines,k);
//			//cvLine( color, line[0], line[1], CV_RGB(255,0,0), 3, CV_AA, 0 );
//			temp =line[1].x-line[0].x;
//			if(temp > maxi)
//			{
//				maxi = temp;
//				j = k;
//			}
//		}
//		Point* line = (Point*)cvGetSeqElem(lines,j);
//		double angle=0;
//
//		if(lines->total>0)
//		{
//			cv::line( color, line[0], line[1], CV_RGB(255,0,0), 3, CV_AA, 0 );
//			sprintf(filename2,"simple_result03cor2/%d_%d_%d_redline.jpg",nvideo,count,q+exnum);
//			//sprintf(filename2,"simple_result-sp-sobel/%d_%d_%d_redline.jpg",nvideo,count,q);
//			imwrite(filename2,color);
//			double k = (double)(-(line[1].y-line[0].y))/(double)(line[1].x-line[0].x);
//			double theta = atan(k);
//			angle=-theta*180/3.14;
//			//angle=theta*180/3.14;
//		}
//
//		//根据水平角度进行水平校正==============================================
//		if(angle!=0&&angle<20&&angle>-20)
//		{		
//			//printf("倾斜角度：%f\n\n",angle);
//			Mat rot_mat;
//			rot_mat.create(2,3,CV_32FC1);
//			Point2f center = Point2f(carplate0.cols/2,carplate0.rows/2);
//			double scale = 1.0;
//			rot_mat=getRotationMatrix2D(center,angle,scale);
//			warpAffine(carplate0,dst,rot_mat,carplate0.size());
//			warpAffine(color_carplate0,dst_col,rot_mat,color_carplate0.size());//旋转彩色图像
//			//cvWarpAffine(bw,bw1,rot_mat);//旋转bw
//			warpAffine(thresh,thresh1,rot_mat,thresh.size());
//			carplate0 = dst.clone();
//			//cvCopy(dst,carplate0,0);
//			color_carplate0 = dst_col.clone();
//			//cvCopy(dst_col,color_carplate0,0);
//			sprintf(filename2,"simple_result03cor2/%d_%d_%d_origin.jpg",nvideo,count,q+exnum);
//			//sprintf(filename2,"simple_result-sp-sobel/%d_%d_%d_origin.jpg",nvideo,count,q);
//			imwrite(filename2,carp);
//			sprintf(filename2,"simple_result03cor2/%d_%d_%d_hough.jpg",nvideo,count,q+exnum);
//			//sprintf(filename2,"simple_result-sp-sobel/%d_%d_%d_hough.jpg",nvideo,count,q);
//			imwrite(filename2,dst);
//			
//		}
//		//=======水平校正结束===============================================
//
//
//
//	    //===========错切====================================================
//		lines = 0;
//		
//		bw1=bw_backup.clone(); 
//		//bw1=cvCloneImage(bwv);
//		Mat color1= Mat(bw1.size(),CV_8UC3);
//		cvtColor( bw1, color1, CV_GRAY2BGR );
//		IplImage *ibw1 = &(IplImage(bw1));
//		//lines = cvHoughLines2( bw1, storage1, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180,20, 1,20  );//累计值最小值，最小线段长度，最大间隔值
//		lines = cvHoughLines2( ibw1, storage1, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180,t_height/4,t_height/5+1,t_height/5+1);
//		maxi=0;
//		j=0;
//		//选竖直坐标差值最大的线
//		for( int k = 0; k < lines->total; k++ )
//		{
//			Point* line = (Point*)cvGetSeqElem(lines,k);
//			
//			temp =line[1].y-line[0].y;
//
//			if(temp<0)
//				temp=-temp;			
//			if(temp > maxi)
//			{
//				maxi = temp;
//				j = k;
//			}
//		}
//
//		//检测不出竖直方向直线或y坐标差值过小=====
//		//用竖直方向梯度图来检测直线==============
//		if(lines->total==0||getnoline||maxi<t_height/3)
//		{
//			Mat V_img=Mat(carp.size(),CV_8UC1);
//			float verticalMatrix[] = {
//					-1,0,1,
//					-2,0,2,
//					-1,0,1,
//			};
//			Mat M_vertical_sobel = Mat(3, 3, CV_32F, verticalMatrix);
//			filter2D(carp, V_img, 8,M_vertical_sobel, cvPoint(-1, -1));
//			Mat thresh_v=carp.clone();
//			IplImage *iV_img = &(IplImage(V_img));
//			threshval = getThreshVal_Otsu_8u(iV_img);
//			threshold(V_img, thresh_v, threshval, 255, CV_THRESH_BINARY_INV);
//			bw1=thresh_v.clone();
//			low = 0.0, high = 0.0; 
//			IplImage *ithresh_v=&(IplImage(thresh_v));
//			
//			AdaptiveFindThreshold(ithresh_v, &low, &high,3);
//			Canny(thresh_v, bw1, low, high); 
//
//			sprintf(filename2,"simple_result03cor2/%d_%d_%d_bw1.jpg",nvideo,count,q+exnum);
//			//imwrite(filename2,bw1);
//			cvClearSeq(lines);
//			ibw1 = NULL;
//			ibw1 = &(IplImage(bw1));
//			lines = cvHoughLines2( ibw1, storage1, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180,t_height/4,t_height/5+1,t_height/5+1);
//			color1= Mat(bw1.size(),CV_8UC3 );
//			cvtColor( bw1, color1, CV_GRAY2BGR );
//		}
//
//		maxi=0;
//		j=0;
//		vline.clear();
//		vline1.clear();
//		for( int k = 0; k < lines->total; k++ )
//		{
//			CvPoint* line = (CvPoint*)cvGetSeqElem(lines,k);
//			
//			temp =line[1].y-line[0].y;
//
//			if(temp<0)
//				temp=-temp;			
//			vline.push_back(temp);
//			vline1.push_back(temp);
//			if(temp > maxi)
//			{
//				maxi = temp;
//				j = k;
//			}
//		}
//		sort(vline.begin(),vline.end(),cmp);
//		u=0;
//		Lcand.clear();
//		float scale1=0.5;
//		if(colnum==0)
//			scale1=0.8;
//
//
//		//选前scale1*maxi=====================================
//		while(u<lines->total&&vline[u]>=scale1*maxi)
//		{
//			for(k = 0; k < lines->total; k++)
//			{
//				if(vline1[k]==vline[u])
//				{
//					Lcand.push_back(k);
//					break;
//				}
//			}
//			u++;
//		}
//
//		//对于竖直方向直线的检测
//		//检测出多条直线
//		//需进行二次筛选
//		//选择和检测出的水平直线夹角最接近90d°的竖直方向直线
//		//此为经验值==============================================
//		mini=1000;
//		j=0;
//		anglea=0;
//		double kpos=0;
//		for(int k=0;k<Lcand.size();k++)
//		{
//			CvPoint* line1 = (CvPoint*)cvGetSeqElem(lines,Lcand[k]);
//			double angle1=0,k1=0,o_angle=0;
//			if(lines->total>0)
//			{
//				cv::line( color1, line1[0], line1[1], CV_RGB(255,0,0), 3, CV_AA, 0 );
//				sprintf(filename2,"simple_result03cor2/%d_%d_%d_redline1.jpg",nvideo,count,q+exnum);//============线错！！！！！！！！！！！
//				//sprintf(filename2,"simple_result-sp-sobel/%d_%d_%d_redline1.jpg",nvideo,count,q);
//				imwrite(filename2,color1);
//				k1 = (double)(-(line1[1].y-line1[0].y))/(double)(line1[1].x-line1[0].x);
//				double theta = atan(k1);
//				angle1=theta*180/3.14;				
//				angle1=angle1+angle;
//				o_angle=abs(angle1);//图中原始垂直角度
//				k1=tan(angle1*3.14/180);
//			}
//			if(abs(o_angle-90)<mini)//选出图中原始垂直角度最竖直的一条
//			{
//				anglea=angle1;
//				j=Lcand[k];
//				mini=abs(o_angle-90);
//				kpos=k1;
//			}
//		}
//		
//		//根据错切角度进行错切校正
//		double angle1=anglea;
//		double k1=kpos;
//		if(abs(angle1)>60&&abs(angle1)<120)
//		{
//			Mat dst1=carplate0.clone();
//			Mat dst_col1=color_carplate0.clone();
//			//Mat warp_mat1 = Mat(2,3,CV_32FC1);  
//			Mat warp_mat1( 2, 3, CV_32FC1 );  
//			Point2f  srcTri[3];  
//			Point2f  dstTri[3];
//			srcTri[0] = Point2f( 0,0 );
//			srcTri[1] = Point2f( carplate0.cols - 1, 0 );
//			srcTri[2] = Point2f( 0, carplate0.rows - 1 );
//		
//			dstTri[0] = Point2f( -(double)carplate0.rows/k1, 0 );
//			dstTri[1] = Point2f( carplate0.cols - 1, 0  );
//			dstTri[2] = Point2f( 0, carplate0.rows - 1 );
//			warp_mat1=getAffineTransform(srcTri,dstTri);
//			warpAffine(carplate0,dst1,warp_mat1,carplate0.size());
//			warpAffine(color_carplate0,dst_col1,warp_mat1,color_carplate0.size());
//			color_carplate0 = dst_col1.clone();
//			sprintf(filename2,"simple_result03cor2/%d_%d_%d_warp.jpg",nvideo,count,q+exnum);
//			//sprintf(filename2,"simple_result-sp-sobel/%d_%d_%d_warp.jpg",nvideo,count,q);
//			imwrite(filename2,dst1);
//			carplate0=dst1.clone();
//		}
////=============================错切校正结束=============================================================
//		cvClearSeq(lines);
//		cvReleaseMemStorage(&storage1);



//int main()
//{
//	cascade = (CvHaarClassifierCascade*)cvLoad(cascade_name, 0, 0, 0);
//	if(!cascade)
//	{
//		printf("ERROR: Could not load classifier cascade\n");
//		return 0;
//	}
//	if(!cascade1.load(cascade_name1))
//	{
//		printf("ERROR: Could not load classifier double cascade\n");
//		return 0;
//	}
//	if(!cascade2.load(cascade_name2))
//	{
//		printf("ERROR: Could not load classifier double cascade\n");
//		return 0;
//	}
//	if(!cascade3.load(cascade_name3))
//	{
//		printf("ERROR: Could not load classifier double cascade\n");
//		return 0;
//	}
//	
//	storage = cvCreateMemStorage(0);
//	IplConvKernel *elem_open_vertical = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT, 0);
//	CvCapture* capture;
//	char videoName[300] = {0};
//	char sName[300] = {0};
//	
//	int count;
//	cout<<"start from:"<<endl;
//	cin>>count;
//	int sum=0;
//	int picnum=2000/*530*/;
//	bool isstart=true;
//	for(int i=count+1; i<=picnum; i++)
//	{	
//		count++;
//		sprintf(sName, "origin/test (%d).jpg",i);
//		Mat img0 = imread(sName/*,CV_LOAD_IMAGE_GRAYSCALE*/);
//		if(!img0.data)        // 判断图片调入是否成功
//			continue;  
//		Mat frame = img0.clone(); 
//		double t = (double)cvGetTickCount();
//		Rect ROI_rect;
//		ROI_rect.x = 0;
//		ROI_rect.y = 0;
//		ROI_rect.width = frame.cols;
//		ROI_rect.height = frame.rows/size;	
//			
//		Mat img = frame(ROI_rect);
//		Mat resize=img.clone();
//		Mat resize1=resize.clone();
//			
//			
//		Mat gray;
//		cvtColor(resize, gray, CV_BGR2GRAY);
//		Mat gray1=gray.clone();
//		Mat frame1=frame.clone();
//		char filename2[200]={0};
//			
//
//		printf("\n处理第%d帧中\n", count);
//		cvClearMemStorage( storage );
//
//		Mat res(resize);
//		Mat fra(frame);
//		Mat gra(gray);
//		int exnum=0;
//		int retest=0;
//		if(retest==0)//ori
//		{
//			exnum=ExtractPlate(res,fra,gra,i,count,exnum,1,retest);
//		}
//		if(retest==1)//lbp
//		{
//			gra=gray1.clone();
//			res=resize1.clone();
//			fra=frame1.clone();
//			exnum=ExtractPlate(res,fra,gra,i,count,exnum,1,retest);
//		}
//		if(retest==2)//3100
//		{
//			gra=gray1.clone();
//			res=resize1.clone();
//			fra=frame1.clone();
//			exnum=ExtractPlate(res,fra,gra,i,count,exnum,1,retest);
//		}
//		if(retest==3)//bus
//		{
//			gra=gray1.clone();
//			res=resize1.clone();
//			fra=frame1.clone();
//			exnum=ExtractPlate(res,fra,gra,i,count,exnum,2,retest);
//		}
//			
//		double t0 = (double)cvGetTickCount();
//		res.release();
//		fra.release();
//		gra.release();
//		resize.release();
//		frame.release();
//		gray.release();
//		resize1.release();
//		frame1.release();
//		gray1.release();
//
//	}
//	cvReleaseMemStorage(&storage);
//	printf("处理结束!\n");
//	system("pause");
//	
//	return 0;
//	cvWaitKey(3);
//}
