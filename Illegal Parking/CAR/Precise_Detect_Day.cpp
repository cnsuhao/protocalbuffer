//#include <vld.h>

//#include "vld.h"
#include "Precise_Detect.h"
#include "main_detect.h"

#include <opencv2/ml/ml.hpp> 
#include <vector>
#include "cv.h"
#include "highgui.h"
//#include "vld.h"

using namespace std;
using namespace cv;

//#define Cy 0.55    //0.55
//#define Cx 0.82   //0.86 0.92

typedef struct pos{
	int i;
	int j;
}pos;

/**
	**函数功能说明：
	**由extraction函数调用，弹出顶值
	**参数说明：
	**s:栈
	**top：栈顶位置
*/
pos pull(pos* s, int &top)
{
	if(top>0)
		return s[--top];
	else
	{
		printf("stack pull wrong!\n");
		exit(1);
	}
}
/**
	**函数功能说明：
	**由extraction函数调用，将值压入栈
	**参数说明：
	**s:栈
	**top：栈顶位置
	**Pos：需压入的值
*/
void push(pos* s, int &top, pos Pos)
{
	s[top++] = Pos;
}
/**
	**函数功能说明：
	**去除圆点
	**参数说明：
	**img:待处理图
	**a：左边界
	**b：右边界
*/
int extraction(Mat  img, int a, int b)
{
	int w = b-a+1;
	int h = img.rows;
	int n=0;
	pos *X = new pos[w*h];
	pos *A = new pos[w*h*3];
	pos *P = new pos[w*h];
	int topX=0, topA=0, topP=0;
	pos tmp;
	for(int i=0; i<h; i++)
		for(int j=a; j<=b; j++)
		{
			if(img.at<uchar>(i, j) ==  255)
			{
				tmp.i = i;
				tmp.j = j;
				push(X, topX, tmp);
			}
		}
	while(topX > 0)
	{
		topA=0, topP=0;
		tmp = pull(X, topX);
		push(A, topA, tmp);
		while(topA > 0)
		{
			pos x = pull(A, topA);
			if(x.i>=0 && x.i<h && x.j>=a && x.j<=b&&(img.at<uchar>( x.i, x.j) == 255) )
			{
				int flag=0;
				for(int s=0; s<topP; s++)
				{
					if(x.i == P[s].i && x.j == P[s].j)
					{
						flag = 1;
						break;
					}
				}
				if(flag == 0)
				{
					for(int t=0; t<topX; t++)
					{
						if(x.i == X[t].i && x.j == X[t].j)
						{
							topX--;
							X[t].i = X[topX].i;
							X[t].j = X[topX].j;
							break;
						}
					}
					push(P, topP, x);
					tmp.i = x.i-1;
					tmp.j = x.j;
					push(A, topA, tmp);
					tmp.i = x.i+1;
					tmp.j = x.j;
					push(A, topA, tmp);
					tmp.i = x.i;
					tmp.j = x.j-1;
					push(A, topA, tmp);
					tmp.i = x.i;
					tmp.j = x.j+1;
					push(A, topA, tmp);
				}
			}
		}
		if(topP<w*h*0.125)
			while(topP>0)
			{
				tmp = pull(P, topP);
				img.at<uchar>(tmp.i, tmp.j) = 0;
			}
	}
	free(X);
	free(A);
	free(P);
	return 0;
}
/**
	**函数功能说明：
	**由Test_color函数调用，计算颜色的RGB特征值
	**参数说明：
	**img:待计算特征图像
	**feat：特征向量
*/
void RGBfeature(Mat img, vector<float>& feat)
{
	char sName[300] = {0};
	int width = img.cols;
	int height = img.rows;
	Mat rgb = img.clone();//cvCreateImage(cvGetSize(img), img->depth, img->nChannels);
	Mat gray = Mat(img.size(),CV_8UC1);
	//rgb.copyTo(img);
	//cvCopy(img,rgb,0);
	cvtColor(img, gray, CV_BGR2GRAY);
	
	int Bc=0,Br=0,Bg=0,Bb=0,Wc=0,Wr=0,Wg=0,Wb=0;//黑白点个数Bc，Wc，黑白点所在位置原图的RGB值累加值Br=0,Bg=0,Bb=0,Wr=0,Wg=0,Wb=0
	Mat thresh = Mat(img.size(),CV_8UC1);
	IplImage* igray = &IplImage(gray);
	int threshval = getThreshVal_Otsu_8u(igray);
	threshold(gray, thresh, threshval, 255, CV_THRESH_BINARY_INV);
	//int sz=thresh.cols*thresh.rows;
	for(int i = 0;i < thresh.rows;i ++)
	{
		uchar* data = thresh.ptr<uchar>(i); 
		int nc = thresh.cols*thresh.channels();
		for(int j = 0;j < nc;j ++)
		{
			if(data[j] == 0)
			{
				Bc++;
				uchar* data1 = img.ptr<uchar>(i); 
				Br += data1[3*j];
				Bg += data1[3*j+1];
				Bb += data1[3*j+2];
			}
			else
			{
				Wc++;
				uchar* data1 = img.ptr<uchar>(i); 
				Wr += data1[3*j];
				Wg += data1[3*j+1];
				Wb += data1[3*j+2];
			}
		}
	}
	//for(int i = 0; i < height; i++)
	//	{
	//		for(int j = 0; j < width; j++)
	//		{
	//			if(CV_IMAGE_ELEM(thresh, uchar, i, j)==0)
	//			{
	//				Bc++;
	//				CvScalar s = cvGet2D(img,i,j);
	//				Br += s.val[0];
	//				Bg += s.val[1];
	//				Bb += s.val[2];
	//			}
	//			if(CV_IMAGE_ELEM(thresh, uchar, i, j)==255)
	//			{
	//				Wc++;
	//				CvScalar s = cvGet2D(img,i,j);
	//				Wr += s.val[0];
	//				Wg += s.val[1];
	//				Wb += s.val[2];
	//			}
	//		}
	//	}
		feat.push_back((float)Br/(float)Bc); 
		feat.push_back((float)Bg/(float)Bc); 
		feat.push_back((float)Bb/(float)Bc); 
		feat.push_back((float)Wr/(float)Wc); 
		feat.push_back((float)Wg/(float)Wc); 
		feat.push_back((float)Wb/(float)Wc);
		//cvReleaseImage(&rgb);
		//cvReleaseImage(&gray);
		//cvReleaseImage(&thresh);
}
/**
	**函数功能说明：
	**由Test_color函数调用，计算颜色的HSV特征值
	**参数说明：
	**img:待计算特征图像
	**feat：特征向量
	**返回值：false：深底 true：浅底
*/
bool HSVfeature(Mat img, vector<float>& feat)
{
	char sName[300] = {0};
	int width = img.cols;
	int height = img.rows;
	Mat hsv = Mat(img.size(),CV_8UC3);
	Mat gray = Mat(img.size(),CV_8UC1);
	cvtColor(img, hsv, CV_BGR2HSV);
	cvtColor(img, gray, CV_BGR2GRAY);
	int Bc=0,Bh=0,Wc=0,Wh=0;//黑白点个数Bc，Wc，黑白点所在位置原图的HSV中H值累加值
	float Bs=0,Bv=0,Ws=0,Wv=0;//HSV中SV值累加值
	Mat  thresh = Mat(img.size(),CV_8UC1);
	IplImage* igray = &IplImage(gray);
	int threshval = getThreshVal_Otsu_8u(igray);
	threshold(gray, thresh, threshval, 255, CV_THRESH_BINARY_INV);
	for(int i = 0;i < thresh.rows;i ++)
	{
		uchar* data = thresh.ptr<uchar>(i); 
		for(int j=0;j<thresh.cols;j++)
		{
			if(data[j]==0)
			{
				Bc++;
				uchar* data1 = hsv.ptr<uchar>(i);
				Bh += data1[3*j]*2;
				Bs += (float)data1[3*j+1]/(float)255;
				Bv += (float)data1[3*j+2]/(float)255;
			}
			else
			{
				Wc++;
				uchar* data1 = hsv.ptr<uchar>(i);
				Wh += data1[j*3]*2;
				Ws += (float)data1[j*3+1]/(float)255;
				Wv += (float)data1[j*3+2]/(float)255;
			}
		}
	}
	//for(int i = 0; i < height; i++)
	//	{
	//		for(int j = 0; j < width; j++)
	//		{
	//			if(CV_IMAGE_ELEM(thresh, uchar, i, j)==0)
	//			{
	//				Bc++;
	//				CvScalar s = cvGet2D(hsv,i,j);
	//				Bh += s.val[0]*2;
	//				Bs += s.val[1]/255;
	//				Bv += s.val[2]/255;
	//			}
	//			if(CV_IMAGE_ELEM(thresh, uchar, i, j)==255)
	//			{
	//				Wc++;
	//				CvScalar s = cvGet2D(hsv,i,j);
	//				Wh += s.val[0]*2;
	//				Ws += s.val[1]/255;
	//				Wv += s.val[2]/255;
	//			}
	//		}
	//	}
		feat.push_back((float)Bh/(float)Bc); 
		feat.push_back((float)Bs/(float)Bc); 
		feat.push_back((float)Bv/(float)Bc); 
		feat.push_back((float)Wh/(float)Wc); 
		feat.push_back((float)Ws/(float)Wc); 
		feat.push_back((float)Wv/(float)Wc);
		if(Bc<=Wc) return false;//深底
		else return true;//浅底
}
/**
	**函数功能说明：
	**判断车牌颜，有彩图直方图均衡化
	**参数说明：
	**img1:待判断车牌图像
	返回值：0 黑 1 蓝 2 白 3 黄
*/
int Test_color(Mat img1)
{
		if(img1.empty())
			return -1;
		char sName[300] = {0};
		vector<Mat> channels;
		//==================hsv=======================
		Mat HSV=Mat(img1.size(), CV_8UC3);
		Mat img=img1.clone();
		cvtColor(img, HSV, CV_BGR2HSV);
		Mat H=Mat(img.size(), CV_8UC1);
		Mat S=Mat(img.size(), CV_8UC1);
		Mat V=Mat(img.size(), CV_8UC1);
		split(img,channels);
		H = channels.at(0);
		S = channels.at(1);
		V = channels.at(2);
		//equalizeHist(H,H);
		//equalizeHist(S,S);
		equalizeHist(V,V);
		channels[0] = H;
		channels[1] = S;
		channels[2] = V;
		
		merge(channels,img);
		//cvtColor(dd, img, CV_HSV2BGR);
		sprintf(sName, "simple_result-sp-col/e_img.jpg");//保存图像
		imwrite(sName, img);
		Mat thresh = Mat(img.size(),CV_8UC1);
		Mat gray = Mat(img.size(),CV_8UC1);
		Mat thresh_v = Mat(img.size(),CV_8UC1);
		Mat bw=gray.clone();
		cvtColor(img, gray, CV_BGR2GRAY);
		
		sprintf(sName, "simple_result-sp-col/gray.jpg");//保存图像
		imwrite(sName, gray);

		int ave=0;//====计算灰度图中的灰度均值
		for(int i = 0;i < gray.rows;i++)
		{
			for(int j = 0;j < gray.cols;j++)
			{
				ave += gray.at<uchar>(i,j);
			}
		}
		ave /= gray.rows*gray.cols;
		int num=0;//====统计大于灰度均值的像素点个数
		for(int i = 0;i < gray.rows;i++)
		{
			for(int j = 0;j < gray.cols;j++)
			{
				if(gray.at<uchar>(i,j)>ave)
					num++;
			}
		}
		float s = (float)num/(float)(gray.rows*gray.cols);//s<0.5是深底，否则是浅底
		//cout<<s<<endl;

		vector<float> charFeature1;
		Mat layerSizes=(Mat_<int>(1,3)<<12,12,4);
		CvANN_MLP Nnw(layerSizes, CvANN_MLP::SIGMOID_SYM,0, 0 );
		//Nnw.load("xml/mlp_color1201.xml");
		Nnw.load("xml/mlp_color0115.xml");
		RGBfeature(img,charFeature1);
		bool back=0;
		back=HSVfeature(img,charFeature1);
		Mat cf1=Mat(1,12,CV_32FC1);
		cf1=Mat(charFeature1);
		cf1=cf1.reshape(0,1);
		//std::cout<<"cfMat"<<cf1<<std::endl;
		Mat result1=Mat(1,4,CV_32FC1);
		Nnw.predict(cf1,result1);
		//std::cout<<"resultMat"<<result1;
		Point maxLoc1;
		minMaxLoc(result1, NULL, NULL, NULL, &maxLoc1);	 
		int re1=maxLoc1.x;
		
		if(re1>1)
		{
			//if(back == false)//深底
			if(s<0.57)
			{
				re1=0;
				if(result1.at<float>(0,0)<result1.at<float>(0,1))
					re1=1;
			}
		}
		else
		{
			if(s>=0.55)
			{
				re1=2;
				if(result1.at<float>(0,2)<result1.at<float>(0,3))
					re1=3;
			}
		}
		Nnw.clear() ;
		return re1;
}
/**
	**函数功能说明：
	**判断车牌颜色1，无直方图均衡化
	**参数说明：
	**img1:待判断车牌图像
	**返回值：0 黑 1 蓝 2 白 3 黄
*/
int Test_color1(Mat img)
{
		char sName[300] = {0};
		sprintf(sName, "simple_result-sp-col/color_img .jpg");//保存图像
		imwrite(sName, img);
		Mat gray = Mat(img.size(),CV_8UC1);
		cvtColor(img, gray, CV_BGR2GRAY);
		sprintf(sName, "simple_result-sp-col/gray_img .jpg");//保存图像
		imwrite(sName, gray);
		
		int ave=0;//====计算灰度图中的灰度均值
		for(int i = 0;i < gray.rows;i++)
		{
			for(int j = 0;j < gray.cols;j++)
			{
				ave += gray.at<uchar>(i,j);
			}
		}
		ave /= gray.rows*gray.cols;
		int num=0;//====统计大于灰度均值的像素点个数
		for(int i = 0;i < gray.rows;i++)
		{
			for(int j = 0;j < gray.cols;j++)
			{
				if(gray.at<uchar>(i,j)>ave)
					num++;
			}
		}
		float s = (float)num/(float)(gray.rows*gray.cols);//s<0.5是深底，否则是浅底
		vector<float> charFeature1;
		Mat layerSizes=(Mat_<int>(1,3)<<12,12,4);
		CvANN_MLP Nnw(layerSizes, CvANN_MLP::SIGMOID_SYM,0, 0 );
		//Nnw.load("xml/mlp_color1201.xml");
		Nnw.load("xml/mlp_color0115.xml");
		RGBfeature(img,charFeature1);
		bool back=0;
		back=HSVfeature(img,charFeature1);
		Mat cf1=Mat(1,12,CV_32FC1);
		cf1=Mat(charFeature1);
		cf1=cf1.reshape(0,1);
		//std::cout<<"cfMat"<<cf1<<std::endl;
		Mat result1=Mat(1,4,CV_32FC1);
		Nnw.predict(cf1,result1);
		//std::cout<<"resultMat"<<result1;
		Point maxLoc1;
		minMaxLoc(result1, NULL, NULL, NULL, &maxLoc1);	 
		int re1=maxLoc1.x;
		//if(back==false&&re1==2)
		//	re1=0;
		//if(back==true&&re1==0)
		//	re1=2;
		if(re1>1)
		{
			//if(back == false)//深底
			if(s<0.49)
			{
				re1=0;
				if(result1.at<float>(0,0)<result1.at<float>(0,1))
					re1=1;
			}
		}
		//else
		//{
		//	//if(back == true)
		//	if(s>=0.49)
		//	{
		//		re1=2;
		//		if(result1.at<float>(0,2)<result1.at<float>(0,3))
		//			re1=3;
		//	}
		//}
		return re1;
}
/**
	**函数功能说明：
	**涂黑字符之间间隙
	**参数说明：
	**Pimg:待判断车牌图像
	**thresh：车牌图像二值化图
	**Px：二值化图像的垂直投影
	**Cx：涂黑间隙时用到的阈值
*/
void GapBlack(Mat  Pimg,Mat  thresh,vector<int>& Px,float Cx)
{	
	vector<int> P;
	if(Px.empty()/*&&thresh*/)
	{
		for(int i=0; i<Pimg.cols; i++)//求对x轴的投影
		{
			int p=0;
			for(int j=0; j<Pimg.rows; j++)
			{
				p += thresh.at<uchar>(j, i);
			}
			P.push_back(p);
			//cout<<Px[i]<<endl;
		}
	}
	else
	{
		for(int i=0;i<Px.size();i++)
		{
			P.push_back(Px[i]);
		}
		Px.clear();
	}

	int max=0, n=0;//max：最大的峰值； n:每次求得的最大值的位置
	for(int i=0; i</*Pimg.cols*/P.size(); i++)//求最大值
	{
		if(P[i]>max)
		{
			max = P[i];
			n = i;
		}
	}
	
	int max_once;//每次的最大值

	while(1)//将字符间隙涂黑，数组清零
	{
		max_once = 0,n=0;
		for(int i=n; i</*Pimg.cols-1*/P.size(); i++)
		{
			if(P[i]>max_once)//找最大值
			{
				max_once = P[i];
				n = i;
				//break;
			}
		}
	if(max_once > max*0.92)//最大值满足要求
		{
			int iLeftSide=0,iRightSide =n;
			for(int i=n; i>=0; i--)//左边界
			{
				if(P[i]<max*Cx)
				{
					if(i<n)
						iLeftSide =i+1;
					else if(i==n)
						iLeftSide =i;
					break;
				}
			}
			for(int i=n; i</*Pimg.cols*/P.size(); i++)
			{
				if(P[i]<max*Cx)//右边界
				{
					if(i>n)
						iRightSide=i-1;
					else if(i==n)
						iRightSide=i;
					break;
				}
			}
			if(iRightSide-iLeftSide==0)                                        //
			{
				//for(int i=iUpFoot; i<=iDownFoot; i++)//将这一范围清零
				//{					
				if(!Pimg.empty()&&!thresh.empty())
				{
					for(int j=0; j<Pimg.rows; j++)
					{	
							Pimg.at<uchar>(j, iRightSide) = 0;
							thresh.at<uchar>(j, iRightSide) = 255;				
					}
				}
					P[iRightSide] = 0;
				//}
			}
			else{
				int s=(iRightSide+iLeftSide)/2;
				//for(int i=0;i<Pimg.cols;i++)
				//{	
				if(!Pimg.empty()&&!thresh.empty())
				{
					for(int j=0; j<Pimg.rows; j++)
					{	
							Pimg.at<uchar>(j, s) = 0;
							thresh.at<uchar>(j, s) = 255;							
	/*					cvSet2D(Pimg, j, i, cvScalar(0, 0, 0));
						cvSet2D(thresh, j, i, cvScalar(255, 255, 255));*/
					}
				}
					P[s] = 0;
				//}
			}
		}
		else
			break;
	}
	for(int i=0;i<P.size();i++)
	{
		Px.push_back(P[i]);
	}
}
/**
	**函数功能说明：
	**整理字符块序列中每个字符块的左右边界，和字符块数
	**参数说明：
	**Pimg:待判断车牌图像
	**charSegment[][2]：存储字符块左右边界的二维数组
	**count：字符块数
	**Px：9图像的垂直投影
*/
int CharSegment(Mat  Pimg,int charSegment[][2],int count,vector<int> &Px)
{
	int width=0;
	if(Pimg.empty()&&Px.size()!=0)
	   width=Px.size();
	else
		width=Pimg.cols;
	for(int n=0; n</*Pimg->*/width;)
	{	
		int flag=0;
		while(n</*Pimg->*/width&&Px[n] == 0)
			n++;
		if(n>=/*Pimg->*/width)
			break;
		//if((count != 0)&&((n-charSegment[count-1][1])>Pimg.rows/2))
		//	count=0;
		charSegment[count][0] = n;
		while(n</*Pimg->*/width &&Px[n] !=0 )
			n++;
		if(n>/*=*//*Pimg->*/width)
			break;
		charSegment[count][1] = n/*-1*/;
		if(charSegment[count][1]-charSegment[count][0]!=0)
			flag=1;
		if(flag==1)
			count++;
		if(count>50)
			break;
	}
	return count;
}
/**
	**函数功能说明：
	**细定位车牌，分割字符
	**参数说明：
	**plate_region：车牌灰度图
	**color：车牌彩色图
	**whitebody:车身深浅
	**colnum：车牌颜色
	**vnum：视频编号
	**num：单视频帧号
	**cons：车牌结构
	**st：车牌类型
	**retest：定位返回值
	**block：字符块序列向量
	**返回值:返回车牌颜色 或 错误信息
*/
int Precise_detect(Mat plate_region,Mat color,bool whitebody,int colnum,int vnum,int num,int snum,int cons,int &st,int retest,vector<Mat> &block)
{	


	char sName[300]={0};
	int chuanflag=0; //对于“川”的判断标志，对后面字符分两段的时候用
	int huflag=0;
	
	float verticalMatrix[] = {
            -1,0,1,
            -2,0,2,
            -1,0,1,
    };
	Mat M_vertical_sobel = Mat(3, 3, CV_32F, verticalMatrix);	
	Mat V_img = Mat(plate_region.size(), CV_8UC1);	

	filter2D(plate_region, V_img,CV_8U, M_vertical_sobel);//做X方向上的Sobel滤波
	
	sprintf(sName, "simple_result-pic-real/%d_%d_%d_V_img.jpg", vnum,num,snum);//调试用
	imwrite(sName,V_img);	
	//统计梯度图像对y轴的投影
	vector<int> Py;
	for(int i=0; i<plate_region.rows; i++)
	{
		int p = 0;
		for(int j=0; j<plate_region.cols; j++)
		{
			if(V_img.at<uchar>(i,j) != 0)
			{
				p += V_img.at<uchar>(i,j);
			}
		}
		Py.push_back(p);
	}
	int max, n=1, iUpFoot, iDownFoot;
	float Cy,Cx,scale;
	int times;
	
	if(cons == 1)//对单排结构确定上下边界
	{
		
		//Cy=0.55;    //y方向比例系数
		Cy=0.4;
		if(retest==1)
			Cy=0.45;
		Cx=0.82;//x方向比例系数（GapBlack中使用）		
		for(int nnn=0; nnn<3; nnn++)//band筛选
		{
			max=0;
			//找y方向投影的最大值
			for(int i=0; i<plate_region.rows; i++)
			{
				if(Py[i]>max)
				{
					max = Py[i];
					n = i;
				}
			}
			iUpFoot=0 ;
				iDownFoot=plate_region.rows-1;
			//在最大值附近根据阈值比例Cy找边界
			for(int i=n-1; i>=0; i--)
			{
				if(Py[i]<max*Cy)
				{
					iUpFoot = i;
					break;
				}
			}
			for(int i=n+1; i<plate_region.rows; i++)
			{
				if(Py[i]<max*Cy)
				{
					iDownFoot = i;
					break;
				}
			}
			if(plate_region.cols / (iDownFoot-iUpFoot) < 20)
				break;
			else//边界过小，则是干扰涂黑去除
			{
				for(int i=iUpFoot; i<=iDownFoot; i++)
					Py[i] = 0;
			}
		}
	}


	if(cons == 2)//双排结构确定上下边界
	{
		//Cy=0.38;    //0.55
		//Cy=0.45;
		Cy=0.38;
		Cx=0.92;
		for(int nnn=0; nnn<3; nnn++)//band筛选
		{
			max=0;
			for(int i=0; i<plate_region.rows; i++)
			{
				if(Py[i]>max)
				{
					max = Py[i];
					n = i;
				}
			}
			iUpFoot=0, iDownFoot=plate_region.rows-1;
			for(int i=0; i<=n-1; i++)
			{
				if(Py[i]>max*Cy)
				{
					iUpFoot = i;
					break;
				}
			}
			for(int i=plate_region.rows-1; i>n; i--)
			{
				if(Py[i]>max*Cy)
				{
					iDownFoot = i;
					break;
				}
			}
			int hormin=10000,minpos=0;
			for(int i=iUpFoot;i<iDownFoot;i++)
			{
				if(hormin>Py[i])
				{
					hormin=Py[i];
					minpos=i;
				}
			}
			if(iUpFoot-4>=0)
				iUpFoot = iUpFoot-4;
			scale=(float)(minpos-iUpFoot)/(float)(iDownFoot-iUpFoot);
			if(scale<=0||scale>=1)
			{
				return -1;
				printf("双排结构上下部分比例确定错误\n");
			}
			if(plate_region.cols / (iDownFoot-iUpFoot) < 20)
				break;
			else
			{
				for(int i=iUpFoot; i<=iDownFoot; i++)
					Py[i] = 0;
			}
		}
		//if(iDownFoot+2<plate_region.rows)
		//	iDownFoot=iDownFoot+2;
		if(scale<0.33)
		{
			int downl=(iDownFoot-iUpFoot)*(1-scale);
			iUpFoot=iDownFoot-(iDownFoot-iUpFoot)*(1-scale)*1.55;
			if(iUpFoot<0)
				iUpFoot=0;
			scale=(float)(iDownFoot-iUpFoot-downl)/(float)(iDownFoot-iUpFoot);
		}
	}
	Py.clear();



	//////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////


	/*****保存band结果*****/
	int x0=0;
	int y0=0;//记录切割后距离原图左上角点的xy方向距离
	int by=0;
	int bhgt=0;

	if(iUpFoot-1>=0)
		iUpFoot--;

	if(iDownFoot+2<plate_region.rows)
		iDownFoot=iDownFoot+2;

	Rect ROI_rect;
	ROI_rect.x = 0;
	ROI_rect.y = iUpFoot;
	ROI_rect.width = plate_region.cols;
	ROI_rect.height = iDownFoot - iUpFoot ;

	//为下面处理高误切多情况记录数据
	by=iUpFoot;
	bhgt=ROI_rect.height;
	
	x0 += ROI_rect.x;
	y0 += ROI_rect.y;
	Rect roi(ROI_rect.x,ROI_rect.y,ROI_rect.width,ROI_rect.height);
	if(ROI_rect.x<0||ROI_rect.y<0||ROI_rect.width<=0||ROI_rect.height<=0
		||ROI_rect.x+ROI_rect.width>plate_region.cols
		||ROI_rect.y+ROI_rect.height>plate_region.rows)
		return  -1;

	Mat  band = plate_region(roi);

	//imshow("www",band);
	//waitKey(1);


	Mat  img_band = plate_region(roi);
	Mat  band_backup = band.clone();
	//colnum = 1 ;
	if(colnum==2||colnum==3)//浅底需要反色
	{
		for(int i=0; i<band_backup.rows; i++)
			for(int j=0;j<band_backup.cols;j++)
			{
				band_backup.at<uchar>(i,j)=255-band_backup.at<uchar>(i,j);
 			}
	}
	//放大到统一高度====================0511
	//Mat bandt;
	//cv::resize(band,bandt,Size(150*(float)band.cols/(float)band.rows,150));
	//band=bandt.clone();
	//img_band=band.clone();
	//Mat color1;
	//cv::resize(color,color1,Size(band.cols,band.cols*(float)color.rows/(float)color.cols));
	//color=color1.clone();
	int y = 0 ;
	//============================
	//顶帽变换弱化背景
	/*int an = img_band.rows-1;*/
	//if(retest==1)
	//	an=img_band.rows-1;//an=img_band.rows/3
	if( img_band.rows-1 <=0)
		return -1;
	IplImage *iimg_band = &(IplImage(img_band));
	IplConvKernel *elem_close_vertical = cvCreateStructuringElementEx( img_band.rows-1,  img_band.rows-1, ( img_band.rows-1-1)/2, ( img_band.rows-1-1)/2, CV_SHAPE_CROSS/*CV_SHAPE_ELLIPSE*//*CV_SHAPE_RECT*/, 0);
	if(colnum==0||colnum==1||colnum==4)
	{
		IplImage *band_temp=cvCreateImage(cvSize(band.cols,band.rows),8,1);
		IplImage *iband=cvCreateImage(cvSize(band.cols, band.rows), 8, 1);
		cvMorphologyEx(/*carplate0*/iimg_band,iband,band_temp,elem_close_vertical,CV_MOP_TOPHAT,1);
		Mat t_band = Mat(iband);
		band=t_band.clone();
		cvReleaseImage(&iband);
		cvReleaseImage(&band_temp);
	}	
	//Mat  band_backup = band.clone();
	cvReleaseStructuringElement(&elem_close_vertical);
	ROI_rect.width=band.cols;
	ROI_rect.height=band.rows;

	Rect roi1(ROI_rect.x,ROI_rect.y,ROI_rect.width,ROI_rect.height);
	if(ROI_rect.x<0||ROI_rect.y<0||ROI_rect.width<=0||ROI_rect.height<=0||ROI_rect.x+ROI_rect.width>color.cols||ROI_rect.y+ROI_rect.height>color.rows)
		return  -1;
	Mat  color_band = color(roi1);	
	sprintf(sName, "simple_result-pic-real/%d_%d_%d_band.jpg", vnum,num,snum);//调试用111111111111111111111111
	imwrite(sName, band);
	
	bool dou_con=false;//dou_con表示是否为双层结构车牌
	if(cons==2)
		dou_con=true;
	if(colnum!=2&&colnum!=3&&dou_con==true)
	{
		dou_con=false;
	}
	
	Mat lrcut=band.clone();
	//若为白色车身，取HSV中的S通道，这样车牌会更显著
	if(whitebody == 1)
	{
		Mat H = Mat(band.size(),CV_8UC1);
		Mat S = H.clone();
		Mat V = H.clone();
		Mat hsv = Mat(band.size(),CV_8UC3);
		cvtColor(color_band, hsv,CV_BGR2HSV );
		vector <Mat> channels;
		split(hsv,channels);
		lrcut=channels[1];
	}
	sprintf(sName, "simple_result-pic-real/%d_%d_%d_lrcut.jpg", vnum,num,snum);

	int maxpix=0;
	int maxpix1=0;
	
	vector<int> Qx;
	vector<int> Ux;
	
	for(int i=0; i<band.cols; i++)
	{
		int q = 0;
		int u = 0;
		for(int j=0; j<band.rows; j++)
		{
			q += lrcut.at<uchar>(j,i);//去除背景后为lrcut
			u += band.at<uchar> (j,i);//背景去不干净的情况下用原图来保证准确分割
		}
		Qx.push_back(q);
		Ux.push_back(u);
	}
	for(int i=0;i<band.cols;i++)//选取Qx中最大值为maxpix
	{
		if(maxpix<Qx[i])
			maxpix=Qx[i];
		if(maxpix1<Ux[i])
			maxpix1=Ux[i];
	}


	int iLeftSide = 0;
	int llfoot=0;
	int iRightSide = band.cols-1;
	
	n=band.cols*0.5;
	float l=0.37;/*0.37*/
	int m;
	if(cons == 2) //处理双排结构的时候系数改变
	{
		l=0.6;
		m=0.5;
	}
	int t = /*band.rows**/maxpix;
	int t1 = /*band.rows**/maxpix1;
	for(int i=0; i<n; i++)//用lrcut确定左边界
	{
		if(Qx[i]>t*l)
		{
			iLeftSide =i+1;
			break;
		}
	}
	for(int i=0; i<n; i++)//用原图确定左边界
	{
		if(Ux[i]>t1*l)
		{
			llfoot=i+1;
			break;
		}
	}
	//去左边界较大项
	if(llfoot>iLeftSide)
		iLeftSide = llfoot;
	//双排结构取llfoot为左边界
	if(cons==2)
		iLeftSide = llfoot;
	
	m=0.2;
	for(int i=band.cols-1; i>n; i--)
	{
		if(Qx[i]>t*m)//确定右边界
		{
			iRightSide = i;
			break;
		}
	}
	if(iLeftSide-2>=0)
	{
		iLeftSide = iLeftSide - 2;
	}

	//初步切割左右边界
	ROI_rect.x = iLeftSide;
	ROI_rect.width = iRightSide - iLeftSide;
	ROI_rect.y = 0;
	ROI_rect.height = band.rows;
	x0 += ROI_rect.x;
	y0 += ROI_rect.y;
	Rect roi2(ROI_rect.x,ROI_rect.y,ROI_rect.width,ROI_rect.height);
	if(ROI_rect.x<0||ROI_rect.y<0||ROI_rect.width<=0
		||ROI_rect.height<=0||ROI_rect.x+ROI_rect.width>band.cols
		||ROI_rect.y+ROI_rect.height>band.rows)
		return  -1;

	Mat  plate0 = band(roi2);

	//imshow("ddd",plate0 ) ;
	//waitKey(1) ;

	Mat  color_plate0 = color_band(roi2);//初步切割彩图左右边界
	if(colnum==2||colnum==3)//浅底需要反色
	{
		for(int i=0; i<plate0.rows; i++)
			for(int j=0;j<plate0.cols;j++)
			{
				plate0.at<uchar>(i,j)=255-plate0.at<uchar>(i,j);
 			}
		int a=plate0.rows;	
		IplConvKernel *elem_close_vertical1 = cvCreateStructuringElementEx(a, a, a/2-1 ,a/2-1, CV_SHAPE_CROSS, 0);
		IplImage* clip_temp = cvCreateImage(cvSize(plate0.cols,plate0.rows), 8, 1);
		IplImage *iplate0 = &(IplImage(plate0));
		IplImage *t_plate0 = cvCloneImage(clip_temp);
		cvMorphologyEx(iplate0,t_plate0,clip_temp,elem_close_vertical1,CV_MOP_TOPHAT,1);
		Mat tt=Mat(t_plate0);
		plate0 = tt.clone();
		sprintf(sName, "simple_result-pic-real/%d_%d_%d_convert_color.jpg", vnum,num,snum);//调试用11111111111111111111
		imwrite(sName, plate0);
		cvReleaseStructuringElement(&elem_close_vertical1);		
		cvReleaseImage(&clip_temp);	
		cvReleaseImage(&t_plate0);	
	}

	//==========二值化去除边缘碎块=======================
	Mat  thresh1 = Mat(plate0.size(),CV_8UC1);
	IplImage *iplate0 = &(IplImage(plate0));
	int threshval = getThreshVal_Otsu_8u(iplate0);
	threshold(plate0, thresh1, threshval, 255, CV_THRESH_BINARY_INV);
	Mat threshcpy=thresh1.clone();
	
	float part1=0.3,part2=0.01;
	int insert=255;
	int sumpix=0;
	Mat mask=thresh1.clone();
	sprintf(sName, "simple_result-pic-real/%d_%d_%d_bf_reband.jpg", vnum,num,snum);//调试用11111111111111111111
	//imwrite(sName, mask);
	//计算二值化图中的黑点
	for(int i=0;i<thresh1.rows;i++)
	{
			uchar* s= thresh1.ptr<uchar>(i);
			for(int j=0;j<thresh1.cols;j++)
			{
				if(s[j] == 0)
				{
					sumpix++;
				}
			}
	}
	bool donotremove=false;
	//用漫水法检测上下边界是否有钉子，检测出将钉子去除
	for(int i=0;i<thresh1.cols;i++)
	{
		donotremove=false;
		uchar s = mask.at<uchar>(0,i);//上
		if(s == (255-insert))
		{
			Point seed = Point(i,0);
			int lo = 0;
			int up = 0;
			int flags = 8;
			Rect comp;
			Mat temp=thresh1.clone();
			floodFill( mask,seed,Scalar(insert,insert,insert),
				&comp,Scalar(lo, lo, lo),Scalar(up, up, up),flags);
			int area = floodFill( thresh1,seed, Scalar(insert,insert,insert),
				&comp,Scalar(lo, lo, lo),Scalar(up, up, up),flags);
			if((area>=part2*sumpix&&comp.height>thresh1.rows/6)||donotremove||comp.height>thresh1.rows/2)/*||(comp.area<part2*sumpix&&(comp.rect.height>0.3*thresh1.rows||!otherpix)))*/
			{
				thresh1=temp.clone();
			}
			else
			{
				sumpix=sumpix-area;
			}
			//imshow("ff",thresh1);
			//waitKey(0);
		}
		donotremove=false;
		s=mask.at<uchar>(thresh1.rows-1, i);//下
		if(s == (255-insert))
		{
			CvPoint seed = cvPoint( i,thresh1.rows-1);
			int lo = 0;
			int up = 0;
			int flags = 8;
			Rect comp;
			Mat temp=thresh1.clone();
			floodFill( mask,seed,Scalar(insert,insert,insert),
				&comp,Scalar(lo, lo, lo),Scalar(up, up, up),flags);
			int area = floodFill( thresh1,seed, Scalar(insert,insert,insert),
				&comp,Scalar(lo, lo, lo),Scalar(up, up, up),flags);
			if((area>=part2*sumpix&&comp.height>thresh1.rows/5)||donotremove||comp.height>thresh1.rows/2)/*||(comp.area<part2*sumpix&&(comp.rect.height>0.3*thresh1.rows||!otherpix)))*/
			{
				thresh1=temp.clone();
			}
			else
			{
				sumpix=sumpix-area;
			}
		}
	}
	
	sprintf(sName, "simple_result-pic-real/%d_%d_%d_band_thresh.jpg", vnum,num,snum);

	bool pixextra=0;//标记是否去除钉子
	for(int i=0;i<thresh1.cols;i++)
	{
		for(int j=0;j<thresh1.rows;j++)
		{
			uchar s1=thresh1.at<uchar>(j,i);
			uchar s2=threshcpy.at<uchar>(j,i);
			if(s1 == 255&&s2 == 0)
			{
				plate0.at<uchar>(j,i)=s2;
				pixextra=1;
			}
		}
	}
	Py.clear();
	//去除钉子后再次确定上下边界
	V_img = Mat(plate0.size(), CV_8UC1);
	filter2D(plate0, V_img,8, M_vertical_sobel, cvPoint(-1, -1));
	for(int i=0; i<plate0.rows; i++)
	{
		int p = 0;
		for(int j=0; j<plate0.cols; j++)
		{
			p += V_img.at<uchar>(i,j);
		}
		Py.push_back(p);
	}
	max=1; n=1; iUpFoot=0; iDownFoot=plate0.rows-1;	
	if(pixextra==true&&cons==1)
	{
		Cy=0.35;    //0.55
		//if(cons == 2)
		//{
		//	Cy=0.3;
		//}
		
		for(int nnn=0; nnn<3; nnn++)//band筛选
		{
			max=0;
			for(int i=0; i<plate0.rows; i++)
			{
				if(Py[i]>max)
				{
					max = Py[i];
					n = i;
				}
			}
			iUpFoot=0, iDownFoot=plate0.rows-1;
			for(int i=n-1; i>=0; i--)
			{
				if(Py[i]<max*Cy)
				{
					iUpFoot = i;
					break;
				}
			}
			for(int i=n+1; i<plate0.rows; i++)
			{
				if(Py[i]<max*Cy)
				{
					iDownFoot = i;
					break;
				}
			}
			if(plate0.cols / (iDownFoot-iUpFoot) < 20)
				break;
			else
			{
				for(int i=iUpFoot; i<=iDownFoot; i++)
					Py[i] = 0;
			}
		}
	}
	//双排修改
	//if (cons == 2)
	//{
	//	iUpFoot = 0;
	//}
	Rect ROI_rect2;
	ROI_rect2.x = 0;
	ROI_rect2.y = iUpFoot;
	ROI_rect2.width = plate0.cols;
	ROI_rect2.height = iDownFoot - iUpFoot ;

	Rect ROI_rectf;
	ROI_rectf.x = 0;
	ROI_rectf.y = iUpFoot;
	ROI_rectf.width = band.cols;
	ROI_rectf.height = iDownFoot - iUpFoot ;
	by += ROI_rect2.y;              //确定高切在img0的什么位置
	bhgt = ROI_rect2.height;
	
	int usim=ROI_rect2.y;
	if(ROI_rectf.x<0||ROI_rectf.y<0||ROI_rectf.width<=0||ROI_rectf.height<=0||ROI_rectf.x+ROI_rectf.width>band.cols||ROI_rectf.y+ROI_rectf.height>band.rows)
		return  -1;
	Mat finalcut = band_backup(ROI_rectf);//记录只切了上下边界的图，待后面头部多切时调用
	Mat ori_ud_edge=finalcut.clone();
	//x0 += ROI_rect2.x;
	//y0 += ROI_rect2.y;
	//Rect roii;
	//roii.x = 0;
	//roii.y = y0;
	//roii.width = img0.cols;
	//roii.width = ROI_rectf.height;

	//Mat finalcut = img0(roii);//记录只切了上下边界的图，待后面头部多切时调用
	//Mat ori_ud_edge=finalcut.clone();

	int plate_x=x0;
	int plate_width=ROI_rect2.width;
	//上下最终确定，左右初步确定后的结果（灰度，彩色）
	if(ROI_rect2.x<0||ROI_rect2.y<0||ROI_rect2.width<=0||ROI_rect2.height<=0||ROI_rect2.x+ROI_rect2.width>plate0.cols||ROI_rect2.y+ROI_rect2.height>plate0.rows)
		return  -1;
	Mat band2 = plate0(ROI_rect2);
	Mat color_plate2 = color_plate0(ROI_rect2);
	
	plate0=band2.clone();
	color_plate0=color_plate2.clone();
	Mat  plate = plate0.clone();

	int piece=plate.rows/2-1;
	int rebandY=y0;
	int rebandH=ROI_rect2.height;
	
	sprintf(sName, "simple_result-pic-real/%d_%d_%d_plate.jpg", vnum,num,snum);//调试用11111111111111111111
	imwrite(sName, plate);
	
	Qx.swap(vector<int> ());
  






//////////////////////////////////////////////////////////////////
//////////////////////////////双层结构处理////////////////////////
	if(dou_con==true)//双层结构车牌
	{
		Mat tplate = plate.clone();
		resize(tplate,plate,Size(tplate.cols,tplate.cols/2));
		Mat  thresh;
		IplImage* iplate= &(IplImage(plate));
		threshval = getThreshVal_Otsu_8u(iplate);
		int value=threshval;
		threshold(plate, thresh, threshval, 255, CV_THRESH_BINARY_INV);
		sprintf(sName, "simple_result-sp_loc1/%d_%d_%d_threshold.jpg", vnum,num,snum);
		//调试用11111111111111111
		imwrite(sName, thresh);
		//切出上半部分车牌
		Rect rect;
		rect.x = 0;
		rect.y = 0;
		rect.width =plate.cols;
		rect.height =plate.rows*scale;	
		if(rect.x<0||rect.y<0||rect.width<=0||rect.height<=0||rect.x+rect.width>plate.cols||rect.y+rect.height>plate.rows)
			return  -1;
		Mat Pimg_up = plate(rect);
		Mat thresh_up = thresh(rect);
		
		//sprintf(sName, "simple_result-sp_loc1/%d_%d_%d_double_1.jpg", vnum,num,snum);//调试用1111111111111111111
		//imwrite(sName, thresh_up);
		//切出上半部分车牌
		rect.x = 0;
		rect.y = plate.rows*scale;
		rect.width =plate.cols;
		rect.height =plate.rows*(1-scale);	
		if(rect.x<0||rect.y<0||rect.width<=0||rect.height<=0||rect.x+rect.width>plate.cols||rect.y+rect.height>plate.rows)
			return  -1;
		Mat Pimg_down = plate(rect);
		Mat thresh_down = thresh(rect);
		if(thresh_up.rows>0.6*thresh_down.rows)
		{
			rect.x=0;
			rect.y=thresh_up.rows-0.6*thresh_down.rows;
			rect.width=thresh_up.cols;
			rect.height=0.6*thresh_down.rows;
			Mat re_up=Pimg_up(rect);
			Mat re_up_th=thresh_up(rect);
			Pimg_up=re_up;
			thresh_up=re_up_th;
		}
		sprintf(sName, "simple_result-sp_loc1/%d_%d_%d_double_2 .jpg", vnum,num,snum);//调试用1111111111111111111
		imwrite(sName, thresh_down);
		sprintf(sName, "simple_result-sp_loc1/%d_%d_%d_double_1.jpg", vnum,num,snum);//调试用1111111111111111111
		imwrite(sName, thresh_up);
		vector<int> Px;//储存对x轴的投影

		//GapBlack(Pimg_up,thresh_up,Px,Cx);//切割上半部分
		//imshow("213",Pimg_up);
		//waitKey(0);
		int charSegment0[50][2], count0=0;
		memset(charSegment0[0],0,50*2);
		//count0=CharSegment(Pimg_up,charSegment0,count0,Px);//整理上半部分切割后字块
		//bool recut=false;
		//for(int i=0;i<count0;i++)
		//{
		//	if(charSegment0[i][1]-charSegment0[i][0]>0.6*Pimg_up.cols)
		//	{
		//		recut=true;
		//		break;
		//	}
		//}
		//if(count0<2||recut)
		{
			cout<<"up recut"<<endl;
			//charSegment0[0][0]=Pimg_up.cols/4-5;
			//if(charSegment0[0][0]<0)
				charSegment0[0][0]=0;
			charSegment0[0][1]=Pimg_up.cols/4+Pimg_up.cols*2/11;
			charSegment0[1][0]=Pimg_up.cols*25/44;
			charSegment0[1][1]=Pimg_up.cols*25/44+Pimg_up.cols*2/11;
			vector<int> PPx;
			int maxp=0;
			for(int i=0;i<charSegment0[0][1];i++)
			{
				int p=0;
				for(int j=0;j<Pimg_up.rows;j++)
				{
					p += Pimg_up.at<uchar>(j,i);
				}
				PPx.push_back(p);
				if(maxp<p)
					maxp=p;
			}
			//for(int i=0;i<charSegment0[0][0];i++)
			//{
			//	//Px[i]=0;
			//	for(int j=0;j<Pimg_up.rows;j++)
			//		Pimg_up.at<uchar>(j,i)=0;
			//}
			//int maxp=0;
			//int lf=0,rt=charSegment0[0][1];
			//for(int i=0;i<charSegment0[0][1];i++)
			//{
			//	if(maxp<Px[i])
			//		maxp=Px[i];
			//}
			int lf=0,rt=charSegment0[0][1]-1;
			for(int i=0;i<charSegment0[0][1];i++)
			{
				if(PPx[i]>maxp*0.45)
				{
					lf=i;
					break;
				}
			}
			for(int i=charSegment0[0][1]-1;i>=0;i--)
			{
				if(PPx[i]>maxp*0.37)
				{
					rt=i;
					break;
				}
			}
			if((rt-lf)>1.2*Pimg_up.rows)
			{
				lf=rt-1.2*Pimg_up.rows;
			}
			charSegment0[0][0]=lf;
			charSegment0[0][1]=rt;

			////Rect roi;
			////roi.x = charSegment0[0][0];
			////roi.width = charSegment0[0][1]-charSegment0[0][0];
			////roi.y = 0;
			////roi.height = Pimg_up.rows;
			////Mat tmp = Pimg_up(roi);
			////Mat thresh_tmp;
			////vector<int> Tx;
			////threshold(tmp,thresh_tmp,0,255,CV_THRESH_BINARY | CV_THRESH_OTSU);
			////GapBlack(tmp,thresh_tmp,Tx,0.8);
			////imshow("232",tmp);
			////waitKey(0);
			for(int i=0;i<charSegment0[0][0];i++)
			{
				//Px[i]=0;
				for(int j=0;j<Pimg_up.rows;j++)
					Pimg_up.at<uchar>(j,i)=0;
			}
			for(int i=charSegment0[0][1];i<charSegment0[1][0];i++)
			{
				//Px[i]=0;
				for(int j=0;j<Pimg_up.rows;j++)
					Pimg_up.at<uchar>(j,i)=0;
			}
			for(int i=charSegment0[1][1];i<Pimg_up.cols;i++)
			{
				//Px[i]=0;
				for(int j=0;j<Pimg_up.rows;j++)
					Pimg_up.at<uchar>(j,i)=0;
			}
			count0=2;
		}
		//imshow("23",Pimg_up);
		//waitKey(0);
		Px.clear();

		GapBlack(Pimg_down,thresh_down,Px,Cx);//切割下半部分
		int charSegment1[50][2], count1=0;
		memset(charSegment1[0],0,100*2);
		count1=CharSegment(Pimg_down,charSegment1,count1,Px);//整理下半部分切割后字块
		if(count1<5)
			return -1;
		int nleft=0,nright=count0-1,huflag=0,chuanflag=0;
		//判断汉字位是否被切开
		//处理川字沪字类型特殊汉字
		for(int i=nleft; i<nright/2; i++)
		{
			if((double)Pimg_up.rows * 0.25 > (charSegment0[i][1] - charSegment0[i][0]))
			{
				if((double)Pimg_up.rows * 0.25 > (charSegment0[i][1] - charSegment0[i][0])
				&& (double)Pimg_up.rows * 0.25 > (charSegment0[i+1][1] - charSegment0[i+1][0])
				&& (double)Pimg_up.rows * 0.25 > (charSegment0[i+2][1] - charSegment0[i+2][0])
				&& (double)Pimg_up.rows * 0.75 < (charSegment0[i+3][1] - charSegment0[i+3][0])
				&& Pimg_up.rows*1.5 > charSegment0[i+2][1] - charSegment0[i][0]
				&& Pimg_up.rows*0.75  < charSegment0[i+2][1] - charSegment0[i][0])
				{
					nleft=i;
					charSegment0[nleft+2][0] = charSegment0[nleft][0];
					nleft += 2;
					chuanflag = 1; //首字符是“川”
					break;
				}
			}
		}
			//=================对“沪，渝，津，皖等偏旁可能被分开”字的处理===============================
		for(int i=nleft; i<nright/2; i++)
		{
			if((double)Pimg_up.rows * 0.25 > (charSegment0[i][1] - charSegment0[i][0]))
			{
				if((double)Pimg_up.rows * 0.25 > (charSegment0[i][1] - charSegment0[i][0])
				&& (double)Pimg_up.rows * 0.5 > (charSegment0[i+1][1] - charSegment0[i+1][0])
				&& (double)Pimg_up.rows * 0.25 < (charSegment0[i+1][1] - charSegment0[i+1][0])
				&& Pimg_up.rows*1.5 > charSegment0[i+1][1] - charSegment0[i][0]
				&& Pimg_up.rows*0.75  < charSegment0[i+1][1] - charSegment0[i][0])
				{
					nleft=i;
					charSegment0[nleft+1][0] = charSegment0[nleft][0];
					nleft += 1;
					huflag = 1; //首字符是“沪”或类似字
				}
			}
		}
		for(int i=0;i<count0;i++)
		{				
			if(charSegment0[i][1]-charSegment0[i][0]>0.8*Pimg_up.rows /*&& charSegment0[i][1]-charSegment0[i][0]<=1.75*Pimg_up.rows*/)//确定双排左边界位置
			{
				nleft=i;   //nleft 汉字的位置
				break;
			}
		}
		ROI_rect.x = charSegment0[nleft][0];
		ROI_rect.width = charSegment0[nright][1] - ROI_rect.x + 1;
		ROI_rect.y = 0;
		ROI_rect.height = Pimg_up.rows;
		if(ROI_rect.x+ROI_rect.width>Pimg_up.cols)
			ROI_rect.width=Pimg_up.cols-ROI_rect.x;
		if(ROI_rect.x<0||ROI_rect.y<0||ROI_rect.width<=0||ROI_rect.height<=0||ROI_rect.x+ROI_rect.width>Pimg_up.cols||ROI_rect.y+ROI_rect.height>Pimg_up.rows)
			return  -1;
		
		Mat band_clip_up = Pimg_up(ROI_rect);
		sprintf(sName, "simple_result-sp_loc1/%d_%d_%d_band_clip_up.jpg", vnum,num,snum);//调试用1111111111111111111
		imwrite(sName, band_clip_up);
		
		Px.clear();
		for(int i=0; i<band_clip_up.cols; i++)
		{
			int p = 0;
			for(int j=0; j<band_clip_up.rows; j++)
			{
				p +=band_clip_up.at<uchar>(j, i);
			}
			Px.push_back(p);
		}
		int charSegment3[50][2], count3=0;
		memset(charSegment3[0],0,100*2);
		count3=CharSegment(band_clip_up,charSegment3,count3,Px);//整理字块
		if(count3<2)
			return -1;
		Mat PlateSeg[15];
		Mat Frt_resize;
		//各类汉字首字符确定位置
		if(chuanflag == 1)
		{
			ROI_rect.width = charSegment3[2][1]-charSegment3[0][0];
			nleft=2;
		}
		if(huflag==1)
		{
			ROI_rect.width = charSegment3[1][1]-charSegment3[0][0];
			nleft=1;
		}
		else 
		{
			ROI_rect.width = charSegment3[0][1]-charSegment3[0][0];
			nleft=0;
		}
		ROI_rect.x = charSegment3[0][0];
		ROI_rect.y = 0;
		ROI_rect.height = band_clip_up.rows;
		if(ROI_rect.x<0||ROI_rect.y<0||ROI_rect.width<=0||ROI_rect.height<=0||ROI_rect.x+ROI_rect.width>band_clip_up.cols||ROI_rect.y+ROI_rect.height>band_clip_up.rows)
			return  -1;
		
		PlateSeg[1] = band_clip_up(ROI_rect);
		sprintf(sName, "seg-each-real/%d_%d_%d_1.jpg", vnum,num,snum);//调试用
		imwrite(sName,PlateSeg[1]);
		//确定上半部分汉字后一位
		for(int i=nleft+1;i<count3;i++)
		{
			if(charSegment3[i][1]-charSegment3[i][0]>piece*0.72)//字符块宽度符合
			{
				ROI_rect.x = charSegment3[i][0];
				ROI_rect.y = 0;
				ROI_rect.width=charSegment3[i][1]-charSegment3[i][0];
				ROI_rect.height = band_clip_up.rows;
				if(ROI_rect.width<=0||ROI_rect.x<0)
				{
					printf("1382\n");
					return -1;
				}
				
				PlateSeg[2] = band_clip_up(ROI_rect);
				sprintf(sName, "seg-each-real/%d_%d_%d_2.jpg", vnum,num,snum);//调试用
				imwrite(sName,PlateSeg[2]);
				break;
			}
			else if(i>0&&charSegment3[i][0]-charSegment3[i-1][1] < piece*0.05&&charSegment3[i][1]-charSegment3[i-1][0]<piece)//合并多切的情况
			{
				ROI_rect.x = charSegment3[i-1][0];
				ROI_rect.y = 0;
				ROI_rect.width = charSegment3[i][1]-charSegment3[i-1][0];
				ROI_rect.height = band_clip_up.rows;
				if(ROI_rect.width<=0||ROI_rect.x<0)
				{
					printf("1382\n");
					return -1;
				}				
				PlateSeg[1] = band_clip_up(ROI_rect);
				sprintf(sName, "seg-each-real/%d_%d_%d_1.jpg", vnum,num,snum);//调试用
				imwrite(sName,PlateSeg[1]);
				//break;
				continue;
			}
		}
		int cnt=2;
		if(count1<5)
		{
			printf("长度不足\n");
			return -1;
		}
		//确定下半部分剩余五位位置
		for(int i=0;i<count1;i++)
		{			
			ROI_rect.x = charSegment1[i][0];
			ROI_rect.y = 0;
			ROI_rect.width = charSegment1[i][1]-charSegment1[i][0];
			ROI_rect.height = Pimg_down.rows;

			//去除高度不符合分片===========================
			Py.clear();
			for(int k=0; k<thresh_down.rows; k++)//求分片对y轴的投影

			{
				int p = 0;
				for(int j=charSegment1[i][0]; j<charSegment1[i][1]; j++)
				{
					p += 255-thresh_down.at<uchar>(k, j);
				}
				Py.push_back(p);
			}
			iUpFoot=0;iDownFoot=thresh_down.rows-1;			
			for(int k=0; k<thresh_down.rows/**0.5*/; k++)
			{
				if(Py[k]!=0)
				{
					iUpFoot = k;
					break;
				}
			}	
			for(int k=thresh_down.rows-1; k>=0/*thresh0.rows*0.5*/; k--)
			{
				
				if(Py[k]!=0)
				{
					iDownFoot = k;
					break;
				}
			}
			//去除高度不满足的分块
		
			if(iDownFoot-iUpFoot<0.6*thresh_down.rows)
			{
					//for(int k=charSegment1[count][0]; k<=charSegment1[count][1]; k++)
					//{
					//	for(int l=0; l<thresh_down.rows; l++)
					//	{
					//		band_clip.at<uchar>(l, k)= 0;
					//	}
					//	Wx[k]=0;
					//}
					continue;
			}
			if(i==0 && (charSegment1[i+1][1]+charSegment1[i+1][0])/2-(charSegment1[i][1]+charSegment1[i][0])/2 < 
				(charSegment1[i+2][1]-charSegment1[i+2][0])/2)//双排调整值
			{
				continue;
			}

			if(ROI_rect.width<0.1*Pimg_down.rows && cnt == 2)//双排调整值
				continue;
			if(ROI_rect.width<5)//双排调整值
				continue;
						
			if(ROI_rect.width<0.4*Pimg_down.rows &&i>0/*&& cnt != 2*/) //“1”左右都被切光//双排调整值
			{
				ROI_rect.x = charSegment1[i][0]-(charSegment1[i][0]-charSegment1[i-1][1])/2;
				if(i+1>=count1)
					ROI_rect.width=charSegment1[i][1]-(charSegment1[i][0]+charSegment1[i-1][1])/2;
				else
					ROI_rect.width=(charSegment1[i+1][0]+charSegment1[i][1])/2-(charSegment1[i][0]+charSegment1[i-1][1])/2;
			}
			if(ROI_rect.x<0||ROI_rect.y<0||ROI_rect.width<=0||ROI_rect.height<=0||ROI_rect.x+ROI_rect.width>Pimg_down.cols||ROI_rect.y+ROI_rect.height>Pimg_down.rows)
				return  -1;
			cnt++;
			PlateSeg[cnt] = Pimg_down(ROI_rect);
			
			sprintf(sName, "seg-each-real/%d_%d_%d_%d.jpg", vnum,num,snum,cnt);//调试用
			imwrite(sName,PlateSeg[cnt]);
			
			if(cnt==7)
				break;
		}
		int pl=0;
		//统一处理各字符，直方图均衡化后去噪
		for(int i=1;i<8;i++)
		{

			medianBlur(PlateSeg[i],PlateSeg[i],3);
			equalizeHist(PlateSeg[i],PlateSeg[i]);

			if(i>1)
			{
				if(PlateSeg[i].empty())
				{
					pl=1;
					break;
				}
				Mat Bck;

				IplImage *tep = &(IplImage(PlateSeg[i]));
				threshval = getThreshVal_Otsu_8u(tep);
				threshold(PlateSeg[i], Bck, threshval, 255, CV_THRESH_BINARY_INV);
				int sumpix=0;
				
				for(int a=0;a<PlateSeg[i].rows;a++)
				{
						uchar *s=Bck.ptr<uchar>(a);
						for(int b=0;b<PlateSeg[i].cols;b++)
						{

							if(s[b]==255)
							{
								s[0]=0;
								PlateSeg[i].ptr<uchar>(a)[b]=0;
							}
							if(s[b]==0)
							{
								sumpix++;
							}
						}
				}
				Rect roi;
				roi.x=1;
				roi.y=0;
				roi.width=PlateSeg[i].cols-1;
				if(roi.width<=0) return -1;
				roi.height=PlateSeg[i].rows;
				Mat pt=PlateSeg[i](roi);
				PlateSeg[i]=pt.clone();
				Mat Bt=Bck(roi);
				Bck=Bt.clone();
				Rect r;          //留取字母连通区域
				if(colnum!=2)
				{
					//imwrite("Bck1.jpg",Bck);
					for(int a=0;a<Bck.cols;a++)
					{
						int b=Bck.rows/2;
						int flagf=0;
						uchar s=Bck.at<uchar>(b,a);
						if(s==0)
						{
							Point seed = Point(a,b);
							int lo = 0;
							int up = 0;
							int flags = 8;
							Rect comp;
							Mat temp=Bck.clone();

							int area = floodFill(Bck, seed, Scalar::all(255),&comp,Scalar(lo, lo, lo),Scalar(up, up, up),flags );

							if(comp.width*comp.height<0.5*Bck.cols*Bck.rows)
							{
								continue;
							}

							r=comp;
							if(r.height/r.width<3&&r.height>0.7*Bck.rows)
							{	
								Mat ts1 = PlateSeg[i](r);
								
								PlateSeg[i]=ts1.clone();
							}
							break;
						}
					}
				}
			}	

				//block.push_back(PlateSeg[i]);
				//sprintf(sName, "seg/%d_%d_%d_%d.jpg", vnum,num,snum,i);//调试用
				//imwrite(sName, PlateSeg[i]);
		}
					//imshow("23",PlateSeg[1]);
					//waitKey(0);
		//block[0]=PlateSeg[1];
		//sprintf(sName, "seg/%d_%d_%d_1.jpg", vnum,num,snum);//调试用
		//imwrite(sName, PlateSeg[1]);
		int ave_wid_down1=0;
		for(int a=4;a<=7;a++)
		{
			ave_wid_down1 += PlateSeg[a].cols;
		}
		ave_wid_down1 /=4;
		int ave_wid_down2=0;
		for(int a=3;a<=6;a++)
		{
			ave_wid_down2 += PlateSeg[a].cols;
		}
		ave_wid_down2 /=4;
		if(PlateSeg[3].cols > ave_wid_down1)
		{
			Rect roi;
			roi.x = PlateSeg[3].cols-ave_wid_down1;
			roi.width = ave_wid_down1;
			roi.y = 0;
			roi.height = PlateSeg[3].rows;
			if(roi.x<0||roi.width<=0)
				return -1;
			Mat tmp = PlateSeg[3](roi);
			PlateSeg[3] = tmp.clone();
		}
		if(PlateSeg[7].cols > ave_wid_down2)
		{
			Rect roi;
			roi.x = 0;
			roi.width = ave_wid_down2;
			roi.y = 0;
			roi.height = PlateSeg[7].rows;
			if(roi.width<=0)
				return -1;
			Mat tmp = PlateSeg[7](roi);
			PlateSeg[7] = tmp.clone();
		}
		if(PlateSeg[1].cols > PlateSeg[1].rows*1.2)
		{
			Rect roi;
			roi.x = PlateSeg[1].cols - PlateSeg[1].rows*1.2;
			roi.width = PlateSeg[1].rows*1.2;
			roi.y = 0;
			roi.height = PlateSeg[1].rows;
			Mat tmp = PlateSeg[1](roi);
			PlateSeg[1] = tmp.clone();
		}
		if(PlateSeg[2].cols > PlateSeg[2].rows*1.2)
		{
			Rect roi;
			roi.x = 0;
			roi.width = PlateSeg[2].rows*1.2;
			roi.y = 0;
			roi.height = PlateSeg[2].rows;
			Mat tmp = PlateSeg[2](roi);
			PlateSeg[2] = tmp.clone();
		}
		if(PlateSeg[1].cols>PlateSeg[2].cols)
		{
			Rect roi;
			roi.x=PlateSeg[1].cols-PlateSeg[2].cols*1.1;
			if(roi.x<0)
				roi.x=0;
			roi.y=0;
			roi.width=PlateSeg[2].cols*1.1;
			roi.height=PlateSeg[1].rows;
			if(roi.x+roi.width>PlateSeg[1].cols)
			{
				roi.width = PlateSeg[1].cols-roi.x;
			}
			if(roi.x<0||roi.y<0||roi.width+roi.x>PlateSeg[1].cols
				||roi.height+roi.y>PlateSeg[1].rows)
				return -1;
			Mat pt=PlateSeg[1](roi);
			PlateSeg[1]=pt;
		}

		int ave_wid_final=0;
		for(int a=1;a<=7;a++)
		{
			if(PlateSeg[a].cols>0)
			{
				ave_wid_final += PlateSeg[a].cols;
			}
		}
		ave_wid_final /= 7;
		if(ave_wid_final<40) return -1;
		if(pl==1||cnt<7)
		{
			printf("长度不足\n");
			return -1;
		}
		for(int i=1;i<8;i++)
		{
			block.push_back(PlateSeg[i]);
			sprintf(sName, "seg/%d_%d_%d_%d.jpg", vnum,num,snum,i);//调试用
			imwrite(sName, PlateSeg[i]);
		}
		Px.swap(vector<int> ());
		Py.swap(vector<int> ());
		return colnum;
	}


	/////////////////////////////////////////////////////////////
	//单排结构车牌处理
	if(dou_con == false && cons == 1)
	{

		int lsim=0;//与plate左边界距离
		Mat  Pimg1 = plate.clone();

		//imshow("ccc",Pimg1);
		//waitKey(0);

		Mat  thresh;
		//得到二值图thresh
		IplImage *iPimg1 = &(IplImage(Pimg1));
		threshval = getThreshVal_Otsu_8u(iPimg1);
		int value=threshval;
		threshold(Pimg1, thresh, threshval, 255, CV_THRESH_BINARY_INV);
	
		sprintf(sName, "simple_result-pic-real/%d_%d_%d_threshold.jpg", vnum,num,snum);//调试用11111111111111111
		imwrite(sName, thresh);
		
		Mat  Pimg =Pimg1.clone();
		
		//imshow("qqq",Pimg);
		//waitKey(1);
		vector<int> Px;
		
		//Cx=0.85;
		Cx=0.85;
		/*imshow("sss", Pimg );
		imshow("black", thresh );
		waitKey(0);*/
		GapBlack(Pimg,thresh,Px,Cx);//涂黑字符间隙
		
		vector<int> R;//得垂直方向投影
		for(int i=0;i<Pimg.cols;i++)
		{
			int sum=0;
			for(int j=0;j<Pimg.rows;j++)
			{
				sum+=Pimg.at<uchar>(j, i);
			}
			R.push_back(sum);
		}
		char Name[30]={0};
		
		int charSegment0[100][2], count1=0, segwidth=0;
		memset(charSegment0[0],0,100*2);
		count1=CharSegment(Pimg,charSegment0,count1,R);	
		sprintf(sName, "simple_result-pic-real/%d_%d_%d_beforecut.jpg", vnum,num,snum);//调试用1111111111111111111
		imwrite(sName, Pimg); 

		int tempright=count1;
		//去除尾部冗余块
		if(count1>7)
		{
			for(int i=count1-1;i>6;i--)
			{
				if(charSegment0[i][1]-charSegment0[i][0]>Pimg.rows*0.3||charSegment0[i][0]-charSegment0[i-1][1]>Pimg.rows*0.5)
					break;
				else
					tempright--;					
			}
		}

		int charSegment[100][2], count=0;
		memset(charSegment[0],0,100*2);
		count=CharSegment(Pimg,charSegment,count,R);
		sprintf(sName, "simple_result-pic-real/%d_%d_%d_aftercut.jpg", vnum,num,snum);//调试用1111111111111111111
		imwrite(sName, Pimg); 
		
		int nleft=0, nright=count-1;
		for(int i=nleft; i<tempright-6; i++)
		{
				int next=charSegment[i+3][1] - charSegment[i+3][0];
				for(int j=i+3;j<nright;j++)
				{
					next=charSegment[j][1] - charSegment[j][0];
					if(next>Pimg.rows * 0.225)
						break;
				}
				if((double)Pimg.rows * 0.225 > (charSegment[i][1] - charSegment[i][0])//川第一片可能较大
				&& (double)Pimg.rows * 0.225 > (charSegment[i+1][1] - charSegment[i+1][0])
				&& (double)Pimg.rows * 0.225 > (charSegment[i+2][1] - charSegment[i+2][0])
				&& (double)Pimg.rows * 0.05 < (charSegment[i][1] - charSegment[i][0])
				&& (double)Pimg.rows * 0.05 < (charSegment[i+1][1] - charSegment[i+1][0])
				&& (double)Pimg.rows * 0.05 < (charSegment[i+2][1] - charSegment[i+2][0])
				&& (charSegment[i+2][0] - charSegment[i+1][1]) < (charSegment[i+3][0] - charSegment[i+2][1])
				&& (charSegment[i+1][0] - charSegment[i][1])<(charSegment[i+3][0] - charSegment[i+2][1])			
				&& next*1.2 > charSegment[i+2][1] - charSegment[i][0]
				&& next*0.8  < charSegment[i+2][1] - charSegment[i][0])
				{
					nleft=i;
					charSegment[nleft+2][0] = charSegment[nleft][0];
					nleft += 2;
					chuanflag = 1; //首字符是“川”
					break;
				}
		}


		//=================对“沪，渝，津，皖等偏旁可能被分开”字的处理===============================
		for(int i=nleft; i<tempright-6; i++)
		{
			if((double)Pimg.rows * 0.2 > (charSegment[i][1] - charSegment[i][0]))
			{
				if((double)Pimg.rows * 0.2 > (charSegment[i][1] - charSegment[i][0])
				&& (double)Pimg.rows * 0.375 > (charSegment[i+1][1] - charSegment[i+1][0])
				&& (double)Pimg.rows * 0.1 < (charSegment[i][1] - charSegment[i][0])
				&& (double)Pimg.rows * 0.1 < (charSegment[i+1][1] - charSegment[i+1][0])
				&& (double)Pimg.rows * 0.2 < (charSegment[i+1][1] - charSegment[i+1][0])
				&& Pimg.rows*0.67 > charSegment[i+1][1] - charSegment[i][0]
				&& Pimg.rows*0.4  < charSegment[i+1][1] - charSegment[i][0]
				&&(charSegment[i+1][1] - charSegment[i][0]<=(charSegment[i+2][1] - charSegment[i+2][0])+1)
				&&(charSegment[i+1][1] - charSegment[i][0]>=(charSegment[i+2][1] - charSegment[i+2][0])-1))//汉字与第一个字母宽度
				{
					nleft=i;
					charSegment[nleft+1][0] = charSegment[nleft][0];
					nleft += 1;
					huflag = 1; //首字符是“沪”或类似字
					break;
				}
			}
		}
		int re=0;
		if(huflag==0&&chuanflag==0) re= 0;
		if(huflag==0&&chuanflag==1) re= 1;
		if(huflag==1&&chuanflag==0) re= 2;

		ROI_rect.x = charSegment[nleft][0];	
		ROI_rect.width = charSegment[nright][1] - ROI_rect.x + 1;	
		ROI_rect.y = 0;
		ROI_rect.height = Pimg.rows;

		if(ROI_rect.x+ROI_rect.width>Pimg.cols)
			ROI_rect.width=Pimg.cols-ROI_rect.x;
		if(ROI_rect.width<=0||ROI_rect.x<0||ROI_rect.width+ROI_rect.x>Pimg.cols||ROI_rect.height<=0||ROI_rect.y<0||ROI_rect.height+ROI_rect.y>Pimg.rows)
		{
			printf("2030\n");
			return -1;
		}

		lsim += ROI_rect.x;
		usim += ROI_rect.y;
		x0 += ROI_rect.x;
		y0 += ROI_rect.y;

		Mat  band_clip = Pimg(ROI_rect);//大致确定首字块位置		
		Mat  thresh0;
		IplImage* iband_clip = &(IplImage(band_clip));
		threshval = getThreshVal_Otsu_8u(iband_clip);
		threshold(band_clip, thresh0, threshval, 255, CV_THRESH_BINARY);
		vector<int>Wx;
		for(int i=0; i<band_clip.cols; i++)
		{
			int w = 0;
			for(int j=0; j<band_clip.rows; j++)
			{
				w += band_clip.at<uchar>(j, i);
			}
			Wx.push_back(w);
		}
		int charSegment1[100][2];
		memset(charSegment1[0],0,100*2);	
		count=0;
		//遍历每个字块
		for(n=0; n<band_clip.cols;)
		{
			while(n<band_clip.cols&&Wx[n] == 0)
				n++;
			charSegment1[count][0] = n;
			while(n<band_clip.cols&&Wx[n] != 0  )
				n++;
			charSegment1[count][1] = n-1;
			Py.clear();
			int sumt=0;//分片像素总和
			for(int i=0; i<thresh0.rows; i++)//求分片对y轴的投影
			{
				int p = 0;
				for(int j=charSegment1[count][0]; j<=charSegment1[count][1]; j++)
				{
					p += thresh0.at<uchar>(i, j);
				}
				Py.push_back(p);
				sumt+=p;
			}
			
			//分片占比较大删去&&分片宽度过大删去
			if((float)sumt/(float)(thresh0.rows*(charSegment1[count][1]-charSegment1[count][0]+1)*255)>0.9
				&&charSegment1[count][1]-charSegment1[count][0]>0.3*thresh0.rows)
			{
					for(int k=charSegment1[count][0]; k<=charSegment1[count][1]; k++)
					{
						for(int l=0; l<thresh0.rows; l++)
						{
							band_clip.at<uchar>(l, k) = 0;
						}
						Wx[k]=0;
					}
					continue;
			}
			//利用二值图确定分块中的上下边界
			
			iUpFoot=0;
			iDownFoot = thresh0.rows-1;			
			
			for(int i=0; i<thresh0.rows/**0.5*/; i++)
			{
				if(Py[i]!=0)
				{
					iUpFoot = i;
					break;
				}
			}	
			for(int i=thresh0.rows-1; i>=0/*thresh0.rows*0.5*/; i--)
			{
				
				if(Py[i]!=0)
				{
					iDownFoot = i;
					break;
				}
			}
			//去除高度不满足的分块
		
			if(iDownFoot-iUpFoot<0.6*thresh0.rows)
			{
					for(int k=charSegment1[count][0]; k<=charSegment1[count][1]; k++)
					{
						for(int l=0; l<thresh0.rows; l++)
						{
							band_clip.at<uchar>(l, k)= 0;
						}
						Wx[k]=0;
					}
					continue;
			}
			//确定分片正常宽度
			int piece0=0;
			//取现有正常分片宽度均值
			if(count>0)
			{
				piece0=0;
				int cnt=0;
				for(int i=0;i<count;i++)
				{
					if(charSegment1[i][1]-charSegment1[i][0]+1>piece*0.5)
					{
						piece0 += charSegment1[i][1]-charSegment1[i][0]+1;
						cnt++;
					}
				}
				if(cnt!=0)
				{
					piece0 /= cnt;
				}
			}
			if(piece0==0)
				piece0=piece;
			if(charSegment1[count][1]-charSegment1[count][0]+1<piece0*0.33)//过窄分块，占比较小，涂黑去除
			{
				if(count!=0&&sumt<0.35*(charSegment1[count][1]-charSegment1[count][0]+1)*thresh0.rows*255)//调参
				{
					for(int k=charSegment1[count][0]; k<=charSegment1[count][1]; k++)
					{
						for(int l=0; l<thresh0.rows; l++)
						{
							band_clip.at<uchar>(l, k) = 0;
						}
						Wx[k]=0;
					}
					continue;
				}
			}
			count++;
		}
		//去除圆点杂项
		if(colnum==0||colnum==1)
		{
			for(int i=0; i<count; i++)
			{
				extraction(band_clip, charSegment1[i][0], charSegment1[i][1]);
			}
		}
		sprintf(sName, "simple_result-pic-real/%d_%d_%d_band_seg.jpg", vnum,num,snum);//调试用
		imwrite(sName, band_clip);	
			
		Wx.swap(vector<int> ());
		Px.clear();
		for(int i=0; i<band_clip.cols; i++)
		{
			int p = 0;
			for(int j=0; j<band_clip.rows; j++)
			{
				p += band_clip.at<uchar>(j, i);
			}
			Px.push_back(p);
		}

		int charSegment2[100][2] ;//记录每个分片的起始和终止位置
		memset(charSegment2[0],0,100*2);
		count=0;

		/*	imshow("tyurt", band_clip);
		waitKey(1) ;*/
		count=CharSegment(band_clip,charSegment2,count,Px);

	/*	charSegment2[7][1] =  0 ;
		charSegment2[7][2] =  0 ;
		int itemp[3] ;
		itemp[0] = charSegment2[1][0];
		itemp[1] = charSegment2[1][0] + ( charSegment2[1][1] - charSegment2[1][0])/2;
		itemp[2] = charSegment2[1][1] ;

		for ( int i = 6 ; i >2 	; i --	)
		{
			charSegment2[i][0]= charSegment2[i-1][0]  ;
			charSegment2[i][1]= charSegment2[i-1][1]  ;
		}
	    	charSegment2[1][0] =  itemp[0] ;
			charSegment2[1][1] =  itemp[1] ;
			charSegment2[2][0] =  itemp[1] ;
			charSegment2[2][1] =  itemp[2] ;



*/

		if (count>6)
		{
			Mat  charshow[20] ;


		for (int i = 0 ; i< count ; i++  )
		{
			Rect temp ;
			temp.x = charSegment2[i][0] ;
			temp.y = 0 ; 
			temp.width = charSegment2[i][1] -charSegment2[i][0] ;
			temp.height = band_clip.rows ;
			charshow[i] = band_clip(temp) ;
			char str1[20] ;
		//	itoa(i, str1 , 10) ;
			/*		imshow("11", charshow[i]  );
			waitKey(1);*/
			int x = 0 ;
		}

		}

		if(count<7)                                         //====·1！！@切割粘连字符
		{
			for(int i=0;i<count;i++)
			{
				if((float)(charSegment2[i][1]-charSegment2[i][0])/(float)band_clip.rows>0.8&&(float)(charSegment2[i][1]-charSegment2[i][0])/(float)band_clip.rows<1.25)
				{
					Px[(charSegment2[i][1]+charSegment2[i][0])/2]=0;//两个相连
					for(int l=0; l<band_clip.rows; l++)
						{
							band_clip.at<uchar>(l,(charSegment2[i][1]+charSegment2[i][0])/2) = 0;
						}
				}
				else if((float)(charSegment2[i][1]-charSegment2[i][0])/(float)band_clip.rows>=1.25)
				{
					float Cxx = 0.9;
					Rect roi =Rect(charSegment2[i][0],0,charSegment2[i][1]-charSegment2[i][0],band_clip.rows);
					Mat stick = band_clip(roi);
					IplImage* istick = &(IplImage(stick));
					int tv = getThreshVal_Otsu_8u(istick);
					Mat st_th;
					threshold(stick, st_th, tv, 255, CV_THRESH_BINARY_INV);
					dilate(st_th,st_th,Mat(),Point(-1,-1),1);
					vector <int> sP;
					sprintf(sName, "simple_result-pic-real_0/%d_%d_%d_st_th.jpg", vnum,num,snum);//调试用
					imwrite(sName, st_th);
					GapBlack(stick,st_th,sP,Cxx);
					int part_cnt = 0;
					int cs[100][2];
					memset(cs[0],0,100*2);
					part_cnt = CharSegment(stick,cs,part_cnt,sP);
					for(int a = 0;a < part_cnt;a++)
					{
						if((float)(cs[a][1]-cs[a][0])/(float)band_clip.rows>0.8&&(float)(cs[a][1]-cs[a][0])/(float)band_clip.rows<1.25)
						{
							//Px[(charSegment2[i][1]+charSegment2[i][0])/2]=0;//两个相连
							sP[(cs[a][1]+cs[a][0])/2]=0;
							for(int l=0; l<stick.rows; l++)
								{
									stick.at<uchar>(l,(cs[a][1]+cs[a][0])/2) = 0;
								}
						}
					}
					band_clip(roi).convertTo(stick, stick.type(), 1, 0); 
					sprintf(sName, "simple_result-pic-real_0/%d_%d_%d_stick.jpg", vnum,num,snum);//调试用
					imwrite(sName, stick);	
					for(int a = charSegment2[i][0];a < charSegment2[i][1];a++)
					{
						Px[a]=sP[a-charSegment2[i][0]];
					}
				}
			}
		}
		memset(charSegment2[0],0,100*2);
		sprintf(sName, "simple_result-pic-real/%d_%d_%d_band_seg2.jpg", vnum,num,snum);//调试用
		imwrite(sName, band_clip);
		count=0;
		count=CharSegment(band_clip,charSegment2,count,Px);
		//==========处理88888这种band时从中间切断的情况===============
		//计算平均宽度
		int ave=0;
		for(int i=count/2-2;i<count/2+2;i++)
		{
			ave += charSegment2[i][1]-charSegment2[i][0];
		}
		ave /= 4;
		if((float)ave/(float)band_clip.rows>0.8)//长宽比满足这样的条件为高度多切
		{
			//设字符应有高度为ave*2，exhgt表示在现在基础上需要增加的高度
			int exhgt=ave*2-bhgt;
			//如果增加后到顶
			if(by-exhgt>=0)
			{
				by=by-exhgt;
				bhgt=ave*2;
			}
			else
			{
				bhgt= by+bhgt;
				by=0;
			}

			Rect reband;
			reband.x=x0;
			reband.y=by;
			reband.width=band_clip.cols;
			reband.height=bhgt;
			if(reband.width<=0||reband.x<0||reband.width+reband.x>plate_region.cols
				||reband.height<=0||reband.y<0||reband.height+reband.y>plate_region.rows)
			{
				return -1;
			}
			Mat rb = plate_region(reband);
			band_clip=rb.clone();

			piece=band_clip.rows/2-1;
			Rect replate;

			replate.x=plate_x;
			replate.y=by;
			replate.width=plate_width;
			replate.height=bhgt;			
			
			rebandY=replate.y;
			rebandH=replate.height;		
			if(replate.width<=0||replate.x<0||replate.width+replate.x>plate_region.cols
				||replate.height<=0||replate.y<0||replate.height+replate.y>plate_region.rows)
			{
				return -1;
			}
			Mat o_plate = plate_region(replate);			
			plate=o_plate.clone();
			Rect r; 
			r.x=0;
			r.y=rebandY;
			r.width=plate_region.cols;
			r.height=rebandH;		

			if(r.width<=0||r.x<0||r.width+r.x>plate_region.cols
				||r.height<=0||r.y<0||r.height+r.y>plate_region.rows)
			{
				return -1;
			}
			Mat p = plate_region(r);
			//imshow("p",img0);
			//waitKey(0);
			if(colnum==2||colnum==3)//浅底需要反色
			{
				for(int i=0; i<p.rows; i++)
					for(int j=0;j<p.cols;j++)
					{
						p.at<uchar>(i,j) = 255-p.at<uchar>(i,j);
 					}
				equalizeHist(p,p);					
				finalcut=p.clone();
			}
			else
			{
					
				//int an=p.rows-1;
				if( p.rows-1 <=0)
					return -1;					
				Mat element = getStructuringElement( MORPH_CROSS, Size( p.rows-1,p.rows-1 ), Point( (p.rows-1-1)/2, (p.rows-1-1)/2) );
				if(colnum==0||colnum==1||colnum==4)
				{						
					finalcut=Mat(Size(r.width, r.height), CV_8UC1);
					morphologyEx(/*carplate0*/p,finalcut,MORPH_TOPHAT,element,Point(-1, -1),1);
				}
			}
		}	
		int sp=0;
		if(re==1)//川字，对X轴投影空隙补齐
		{
			sp=3;
			for(int i=0;i<2;i++)
			{
				for(int j=charSegment2[i][1];j<charSegment2[i+1][0];j++)
					Px[j]=1;
			}
		}
		if(re==2)//沪字，对X轴投影补齐
		{
			sp=2;
			for(int i=0;i<1;i++)
			{
				for(int j=charSegment2[i][1];j<charSegment2[i+1][0];j++)
					Px[j]=1;
			}
		}
		//==========================================================
		//确定合适的分片宽度
		//先和之前ave比较
		if(piece<ave*0.6)
			ave=piece;
		if(ave<piece*0.6)
			ave=piece;
		piece=ave;	
		for(int i=sp;i<count;i++)
		{
			int next=piece;
			int nxt_next=piece;
			if((i+1)==count-1)//汉字后一位为最后一位
			{
				next=charSegment2[i-1][1]-charSegment2[i-1][0];
				nxt_next=charSegment2[i-2][1]-charSegment2[i-2][0];
			}
			else if((i+2)==count-1)//汉字后两位为最后一位
			{
				next=charSegment2[i+2][1]-charSegment2[i+2][0];
				nxt_next=charSegment2[i-1][1]-charSegment2[i-1][0];
			}
			else 
			{
				next=charSegment2[i+2][1]-charSegment2[i+2][0];
				nxt_next=charSegment2[i+3][1]-charSegment2[i+3][0];
			}
			
			if(piece<next*0.6)
				next=piece;
			if(piece<nxt_next*0.6)
				nxt_next=piece;
			if(next<piece*0.6)
				next=piece;
			if(nxt_next<piece*0.6)
				nxt_next=piece;
			if(nxt_next<next&&i>=2)
				next=nxt_next;

			float sc=1.25;//1.2==========================0108
			//if(i+1==count-1||(i<2&&i>count-7))//若是头部或末位则宽度要放宽
			//	//sc=1.9;
			//	sc=1.5;
			if(charSegment2[i+1][1]-charSegment2[i][0]>=(int)(next*0.8)//i+1块和i块合并在合适范围内
				&&(charSegment2[i+1][1]-charSegment2[i][0]<=(int)(next*sc)/*||(charSegment2[i+1][1]-charSegment2[i][0]<=(int)(nxt_next*sc)*/)
				&&charSegment2[i+1][0]-charSegment2[i][1]<(int)(band_clip.rows/5))//合并的两块的间隙合适
			{
				if(!((charSegment2[i+2][0]-charSegment2[i+1][1]<charSegment2[i+1][0]-charSegment2[i][1])
					&&(charSegment2[i+2][1]-charSegment2[i+1][0]>=(int)(next*0.8))
					&&(charSegment2[i+2][1]-charSegment2[i+1][0]<=(int)(next*sc))))//若发现后面有更合适的合并块则不合并======0108
				{
					for(int j=charSegment2[i][1];j<charSegment2[i+1][0];j++)
					{
						Px[j]=1;
					}
				}
			}
		}

		memset(charSegment2[0],0,100*2);
		count=0;
		Mat y;
		y.release();
		count=CharSegment(y,charSegment2,count,Px);
		ROI_rect.y = 0;
		ROI_rect.height = band_clip.rows;
		int flag=0;
		ave=0;
		int cnt=0;
		//再次计算宽度均值
		for(int i=count/2-2;i<count/2+2;i++)
		{	
			if(charSegment2[i][1]-charSegment2[i][0]<piece*0.5)//除去1这种分片极窄的不考虑
			{
				continue;
			}
			ave += charSegment2[i][1]-charSegment2[i][0];
			cnt++;
		}
		if(cnt>0)
		{
			ave /= cnt;
		}
		else
		{
			for(int i=count/2-2;i<count/2+2;i++)
			{	
				ave += charSegment2[i][1]-charSegment2[i][0];
			}
			ave /=4;
		}
		//======================1001
		if(piece<ave*0.6)
			ave=piece;
		if(ave<piece*0.6)
			ave=piece;
		piece=ave; 
		int c;		
		vector <int> center(0),perseg(0),cetd(0);
		for(int i=0;i<count;i++)
		{
			int c=(charSegment2[i][0]+charSegment2[i][1])/2;
			center.push_back(c);                                    //中心点center
			perseg.push_back(charSegment2[i][1]-charSegment2[i][0]);//每段长perseg
		}
		for(int i=0;i<count-1;i++)
		{
			cetd.push_back(center[i+1]-center[i]);                 //中心点距离cetd
		}
		//FILE *fp=fopen("wrong list.csv","a");
		vector<float> v_scale;//记录每块中心距离左右相邻字块中心距离之比
		vector <int> v_pos;	//记录位置

		int vpos=-1;
		float maxs=0;
		int maxpos=0;
		for(int i=cetd.size()-1;i>=1;i--)
		{	
			float scale=(float)(cetd[i-1])/(float)(cetd[i]);//计算第i位的左长比右长
			v_scale.push_back(scale);
			if(perseg[i+1]<0.1*piece||perseg[i]<0.1*piece||perseg[i-1]<0.1*piece||cetd[i]<0.8*piece||cetd[i-1]<0.8*piece||cetd[i]>4*piece||cetd[i-1]>4*piece)
				continue;

			//cout<<i<<":";
			//cout<<scale<<endl;
			v_pos.push_back(i);
			if(maxs<scale&&scale<2)
			{
				maxs=scale;
				maxpos=i;
			}
		}		
	//cout<<"2410"<<endl;
		vector <int>temp_v_pos;
		//将符合条件的字块位置放入temp_v_pos中
		for(int i=0;i<v_pos.size();i++)
		{
			if(v_scale[v_scale.size()-v_pos[i]]<1.17&&v_scale[v_scale.size()-v_pos[i]]>0.78&&v_pos[i]!=maxpos)
			{
				temp_v_pos.push_back(v_pos[i]);
			}
		}
		int style=0;//0为一般车牌
		            //1为使馆车牌
		            //2为警车车牌
					//3为领馆车牌
		if(temp_v_pos.size()>=3)//一般车牌
		{
			if(temp_v_pos.size()>3)
			{
				for(int i=0;i<=temp_v_pos.size()-3;i++)
				{
					if(temp_v_pos[i]==temp_v_pos[i+1]+1
						&&temp_v_pos[i+1]==temp_v_pos[i+2]+1
						&&((i+3)==temp_v_pos.size()||temp_v_pos[i+2]!=temp_v_pos[i+3]+1)
						&&temp_v_pos[i+2]>2)
							vpos=temp_v_pos[i+2]-2;//汉字后第一位
				}
			}
			else if(temp_v_pos[0]==temp_v_pos[1]+1&&temp_v_pos[1]==temp_v_pos[2]+1&&temp_v_pos[2]>2)
						vpos=temp_v_pos[2]-2;
			//if(vpos<1)
			//{
			//	cout<<"普通字符不全"<<endl;
			//}
		}
		if(temp_v_pos.size()>=2&&colnum==0/*&&temp_v_pos[0]==temp_v_pos[1]+3*/)//使馆车牌
		{
			for(int i=0;i<temp_v_pos.size();i++)
			{
				for(int j=i+1;j<temp_v_pos.size();j++)
				{
					if(temp_v_pos[j]>=2&&temp_v_pos[i]==temp_v_pos[j]+3&&v_scale[v_scale.size()-temp_v_pos[j]]>0.85)
					{
						style=1;
						vpos=temp_v_pos[i]-4;//使馆数字的第一位
						break;
					}
				}
			}
			
	/*		if(vpos<0)
				cout<<"使馆字符不全"<<endl;*/
		}
		
		if(temp_v_pos.size()==4
				&&temp_v_pos[3]>1
				&&temp_v_pos[0]==temp_v_pos[1]+1
				&&temp_v_pos[1]==temp_v_pos[2]+1
				&&temp_v_pos[2]==temp_v_pos[3]+1
				&&colnum==2)
		{
			style=2;
			vpos=temp_v_pos[3]-1;//警车的第二位
	/*		if(vpos<1)
				cout<<"警车字符不全"<<endl;*/
		}
		if(temp_v_pos.size()==5)
		{
			int uncont=0;
			for(int i=0;i<temp_v_pos.size()-1;i++)
			{
				if(temp_v_pos[i]!=temp_v_pos[i+1]+1)
					uncont=1;
			}
			if(uncont==0)
			{
				style=0;
				vpos=temp_v_pos[temp_v_pos.size()-1]-1;
			}
		}
		if(temp_v_pos.size()>=2&&colnum==0)
		{
			for(int i=0;i<temp_v_pos.size();i++)
			{
				for(int j=i+1;j<temp_v_pos.size();j++)
				{
					if(temp_v_pos[j]>=3&&((i==0&&temp_v_pos[i]==temp_v_pos[j]+1)||(i>0&&temp_v_pos[i]==temp_v_pos[j]+1&&temp_v_pos[i-1]!=temp_v_pos[i]+1))&&v_scale[v_scale.size()-temp_v_pos[j]+1]>1.3)
					{
						
						style=3;
						vpos=temp_v_pos[j]-3;//领馆数字的第一位
						break;
					}
				}
			}
		}
		int errf=NULL;//0无错误
					  //1关键字符没选对
					  //2头被去掉

		if(vpos==-1)
		{
			//cout<<"vpos==-1"<<endl;
			//cout<<vnum<<"_"<<snum<<"errf=1"<<endl;
		//	waitKey(0);		
			errf=1;
		}
		if(style==0||style==2)
		{
			if(vpos<1)
			{
				//cout<<"字符不全"<<endl;
				//cout<<vnum<<"_"<<snum<<"errf=1"<<endl;
			//	waitKey(0);		
				errf=1;
			}
		}
		//if(style!=1&&count==6)
		//{
		//	flag=1;
		//	vpos=0;
		//}
		if(style!=1&&count<=6)
		{
			//waitKey(0);		
			return -1;
		}
		if(style==1&&count<6)
		{
		/*	cout<<vnum<<"_"<<snum<<" ";
			printf("count<6\n");*/
		//	waitKey(0);		
			return -1;
		}
 		if(errf==1)
		{
			if(style==1)
				return -1;
			if(colnum!=2)
			 {
				for(int i=0;i<count-6;i++)
				{
					int first=charSegment2[i+1][0]-charSegment2[i][1]+1;//1st间隙
					int second=charSegment2[i+2][0]-charSegment2[i+1][1]+1;//2nd间隙
					int third=charSegment2[i+3][0]-charSegment2[i+2][1]+1;//3rd间隙

					if((first>=1)
						&&(second>first+1)
						&&(third<second)
						&&first<band_clip.rows*0.3
						&&(float)second/(float)first<10)
						{
							if((int)((charSegment2[i+1][1]-charSegment2[i+1][0])*0.6)<=second
							&&charSegment2[i+1][1]-charSegment2[i+1][0]+1>=(int)(piece*0.3)
							&&charSegment2[i+1][1]-charSegment2[i+1][0]+1<(int)(piece*1.9/*2.8*/)//0109
							&&charSegment2[i+2][1]-charSegment2[i+2][0]+1<(int)(piece*1.9/*2.8*/)
							
							&&(charSegment2[i+2][1]+charSegment2[i+2][0])/2-(charSegment2[i+1][1]+charSegment2[i+1][0])/2<piece*2.1//中心距离相减
							)
							{
								vpos=i+1;							
								flag=1;
								break;
							}
						}
				}
			if(flag==0)
			{
				
				//for(int i=0;i<count-6;i++)
				//{
				//	if(charSegment2[i][1]-charSegment2[i][0]+1>piece*0.8
				//	&&charSegment2[i+1][1]-charSegment2[i+1][0]+1>piece*0.8
				//	&&charSegment2[i][1]-charSegment2[i][0]+1<piece*1.8
				//	&&charSegment2[i+1][1]-charSegment2[i+1][0]+1<piece*1.8)//非蓝牌中连续两个块和字符块相似
				//	{
				//		vpos=i+1;
				//		flag=1;
				//		break;
				//	}
				//	
				//}
				//if(flag==0||vpos>count-6)
				{
					//char mes[100]={0};
					//sprintf(mes,"定位不准删去");
					//fprintf(fp, "%d,%d,%s\n",vnum,snum,mes);
					//sprintf(sName, "wrong/%d_%d_%d_wrongloc.jpg", vnum,num,snum);//调试用
					//imwrite(sName,img0);		
					return -2;
				}
			}
		}		
			else 
			{
				return -2;
				//vpos=1;
			}
		}	
		int charSeg[7][2];
		memset(charSeg[0],0,7*2);
		int charSeg1[7][2];
		memset(charSeg1[0],0,7*2);
		//确定每位在band_clip中的位置
		if(style==0||style==2)
		{
			for(int i=vpos;i<vpos+6;i++)
			{
				charSeg[1-vpos+i][0]=charSegment2[i][0];
				charSeg[1-vpos+i][1]=charSegment2[i][1];
			}
			int firstlength=0;
			int charstart=0;
			for(int i=vpos-1;i>=0;i--)
			{
				firstlength += charSegment2[i][1]-charSegment2[i][0];
				if(firstlength>(charSeg[1][1]-charSeg[1][0])*0.9||i==0)
				{
					charSeg[0][1]=charSegment2[vpos-1][1];
					charSeg[0][0]=charSegment2[i][0];
					if(charSeg[0][1]-charSeg[0][0]</*0.7**/0.85*piece)//headcut//0423
					{
						errf=2;
						char mes[100]={0};
						sprintf(mes,"头部太窄");
						//fprintf(fp, "%d,%d,%s\n",vnum,snum,mes);
						charstart=1;
					//	waitKey(0);		
					}
					break;
				}
			}
		
			Mat  pre[7];
			int presize=0;
			
				for(int i=0;i<7;i++)
				{
					Rect r;
					r.x=charSeg[i][0];
					r.width=charSeg[i][1]-charSeg[i][0];
					if(r.x+r.width>band_clip.cols)
						r.width=band_clip.cols-r.x;
					r.y=0;
					r.height=band_clip.rows;
					if(r.x+r.width>band_clip.cols||r.x>band_clip.cols||r.x<0||r.width<=0)
					{
						break;
					}
					Mat p = band_clip(r);
					sprintf(sName, "seg-each-realp3/%d_%d_%d_%d.jpg", vnum,num,snum,i);//调试用
					imwrite(sName,p);
					pre[i]=p.clone();
					presize++;
				}
				if(presize<6)
					return -1;
				else if(presize==6)
				{
					errf=2;
					pre[6]=pre[5].clone();
					for(int i=5;i>=1;i--)
					{
						pre[i] = pre[i-1].clone();
					}
				}
				//fclose(fp);
				
				int c1=2;
				charSeg1[0][0]=charSeg[0][0];
				charSeg1[0][1]=charSeg[0][1];
				charSeg1[1][0]=charSeg[1][0];
				charSeg1[1][1]=charSeg[1][1];
				//宽度基准由首位汉字确定，若汉字后一位大于汉字宽的1.2倍则选汉字后一位宽度为宽度基准
				int segave=pre[1].cols;
				if(1.2*pre[1].cols<pre[2].cols)
				{
					segave=pre[2].cols;
				}
				//切分粘连字符
				for(int i=2;i<7/*presize*/;i++)
				{
					if(pre[i].cols>1.86*segave&&c1<6)
					{
						charSeg1[c1][0]=charSeg[i][0];
						charSeg1[c1][1]=charSeg[i][0]+(charSeg[i][1]-charSeg[i][0])/2;
						charSeg1[c1+1][0]=charSeg1[c1][1];
						charSeg1[c1+1][1]=charSeg[i][1];
						c1++;
					}
					else
					{
						charSeg1[c1][0]=charSeg[i][0];
						charSeg1[c1][1]=charSeg[i][1];
					}			
					c1++;
					if(c1>6)
						break;
				}
				if(c1<6)//分片不足7
				{
					return -1;
				}
			}
			else
			{
				for(int i=vpos;i<vpos+6;i++)
				{
					charSeg[i-vpos][0]=charSegment2[i][0];
					charSeg[i-vpos][1]=charSegment2[i][1];
					charSeg1[i-vpos][0]=charSegment2[i][0];
					charSeg1[i-vpos][1]=charSegment2[i][1];
				}
			}

		int nsize=7;
		if(style==1||style==3)
		{
			nsize=6;
		}


		Rect ROI_rect1;//判断颜色

		ROI_rect1.x = lsim+charSeg[0][0];
		ROI_rect1.y = 0;
		if(nsize-1<0) return -1;
		ROI_rect1.width = charSeg[nsize-1][1]-charSeg[0][0];

		if(ROI_rect1.width+ROI_rect1.x>color_plate0.cols)
			ROI_rect1.width=color_plate0.cols-ROI_rect1.x-1;
		ROI_rect1.height = color_plate0.rows;

		if(ROI_rect1.x+ROI_rect1.width>color_plate0.cols||ROI_rect1.x<0||ROI_rect1.width<=0||ROI_rect1.x>color_plate0.cols||ROI_rect1.y+ROI_rect1.height>color_plate0.rows)
		{
			//printf("3002ROI region illegal\n");
			return -1;
		}
		
		Mat  color_test = color_plate0(ROI_rect1);

		sprintf(sName, "simple_result-sp-col/%d_%d_%d_color_test3.jpg", vnum,num,snum);//调试用11111111111111111
		imwrite(sName, color_test);
		int secTest=Test_color(color_test);
		if(colnum == -1)
				//cout<<"4177"<<endl;
		if(secTest>1)
			secTest = colnum;

		//cout<<"Test_color3: "<<secTest<<endl; 
		

		int charSegment3[100][2] ;//记录每个分片在img0中的起始和终止位置	
		memset(charSegment3[0],0,100*2);
		count=0;
		int stn=0;//后续处理起始点
	
		if(errf==2)
		{
			stn=1;
		}
		for(int i=stn;i<nsize;i++)
		{
			charSegment3[i][0]=charSeg1[i][0]+x0;//i-stn=>i
			charSegment3[i][1]=charSeg1[i][1]+x0;
		}

		nleft=0;
		nright=count-1;
		//不用再次判断特殊汉字
		Mat PlateSeg[15],Frt_resize;//将要是别的车牌图片变成二值图片
		n=0;
		int temp=0;
		//===========处理头部汉字被误去除的情况========================================
		int fCuthead=0;
		if(errf==2)
		{
			//计算首字块与第二字块之间的间隙宽度
			int firstgap=((charSegment3[3][0]+charSegment3[3][1])/2-(charSegment3[2][0]+charSegment3[2][1])/2)*((float)12/(float)57);
			//估计首字块左右边界
			charSegment3[0][1]=charSegment3[1][0]-firstgap;
			charSegment3[0][0]=charSegment3[0][1]-(charSegment3[1][1]-charSegment3[1][0]);
			if(charSegment3[0][0]<0)
			{
				charSegment3[0][0]=0;
			}
			//处理原图图像不完整的情况
			if(charSegment3[0][1]<=0||charSegment3[0][1]-charSegment3[0][0]<0.5*(charSegment3[1][1]-charSegment3[1][0]))
			{
					//char mes[100]={0};
					//sprintf(mes,"原图无头");
					//fprintf(fp, "%d,%d,%s\n",vnum,snum,mes);
					//sprintf(sName, "wrong/%d_%d_%d_narrowhead.jpg", vnum,num,snum);//调试用
					//imwrite(sName,img0);
					return -1;
			}
		}
		else if(charSegment3[1][0]-charSegment3[0][1]>(charSegment3[2][0]-charSegment3[1][1])*1.2
			&&charSegment3[1][0]-charSegment3[0][1]>(charSegment3[3][0]-charSegment3[2][1])*1.2
			&&(secTest!=2 ))//除了白牌，判断一间隙比后两个间隙的1.2倍都大的情况为头部被多切的情况
		{
			for(int i=nsize-1;i>=0;i--)
			{
				charSegment3[1+i][0]=charSegment3[i][0];
				charSegment3[1+i][1]=charSegment3[i][1];
			}
			int firstgap=((charSegment3[3][0]+charSegment3[3][1])/2-(charSegment3[2][0]+charSegment3[2][1])/2)*((float)12/(float)57);
			charSegment3[0][1]=charSegment3[1][0]-firstgap;
			charSegment3[0][0]=charSegment3[0][1]-(charSegment3[1][1]-charSegment3[1][0]);
			if(charSegment3[0][0]<0)
			{
				charSegment3[0][0]=0;
			}
			if(charSegment3[0][1]<=0||charSegment3[0][1]-charSegment3[0][0]<0.5*(charSegment3[1][1]-charSegment3[1][0]))
			{
					//char mes[100]={0};
					//sprintf(mes,"原图无头");
					//fprintf(fp, "%d,%d,%s\n",vnum,snum,mes);
					return -1;
			}
		}
		//整理每个字块宽度
		int picseg=charSegment3[1][1]-charSegment3[1][0];
		if(charSegment3[1][1]-charSegment3[1][0]<charSegment3[2][1]-charSegment3[2][0])
			picseg=charSegment3[2][1]-charSegment3[2][0];
		int plate_length=charSegment3[nsize-1][1]-charSegment3[0][0];
		for(int i=0;i</*7*/nsize;i++)
		{
			if(charSegment3[i][0]<0)
				break;
			ROI_rect.x = charSegment3[i][0];//处理第一个字符
			ROI_rect.width=charSegment3[i][1]-charSegment3[i][0];
			//非使馆首字符宽度过大
			if(i==0&&style!=1)
			{
				if(charSegment3[0][1]-charSegment3[0][0]>picseg)
				{
					ROI_rect.x=charSegment3[0][1]-picseg;
					ROI_rect.width=picseg;
				}
				plate_length -= ROI_rect.x;
			}
			//非使领馆车末位过宽
			if(i==6&&style!=1&&style!=3)
			{
				if(charSegment3[6][1]-charSegment3[6][0]>picseg)
				{
					ROI_rect.width=picseg;
				}
				if(ROI_rect.x-1>charSegment3[5][1])
				{
					ROI_rect.x--;
				}
				if(ROI_rect.x+ROI_rect.width+3<finalcut.cols)
				{
					ROI_rect.width =ROI_rect.width+3;
				}
				if(charSegment3[nsize-1][1]-(ROI_rect.x+ROI_rect.width)>0)
				{
					plate_length -= charSegment3[nsize-1][1]-(ROI_rect.x+ROI_rect.width);
				}
			}
			//非使馆第二位过窄
			if(i==1&&style!=1)
			{
				if(charSegment3[i][1]-charSegment3[i][0]<picseg)
				{
					if(ROI_rect.x-(picseg-ROI_rect.width)/2>charSegment3[i-1][1])
						ROI_rect.x -= (picseg-ROI_rect.width)/2;
					else 
						ROI_rect.x=charSegment3[i-1][1]+1;
					if(ROI_rect.x+picseg<charSegment3[i+1][0])
						ROI_rect.width=picseg;
					else
						ROI_rect.width=charSegment3[i+1][1]-1;
				}
			}
			int extflag=0;
			
			ROI_rect.y=0;
			ROI_rect.height=band_clip.rows;

			if(ROI_rect.x<0||ROI_rect.y<0||ROI_rect.x+ROI_rect.width>finalcut.cols||ROI_rect.y+ROI_rect.height>finalcut.rows||ROI_rect.width<=0||ROI_rect.height<=0)
				return -1;
			
			PlateSeg[1+i] = finalcut(ROI_rect);
			//imshow("per",PlateSeg[1+i]);
			//waitKey(0);
			//数字1拓宽左右
			if((style!=1&&i>1&&(float)ROI_rect.height/(float)ROI_rect.width>4)||(style==1&&i>=0&&(float)ROI_rect.height/(float)ROI_rect.width>4))
			{
				Mat src=Mat(PlateSeg[1+i]);
				Mat dst = Mat(src.rows, src.cols, src.type(), Scalar(0, 0, 0));;
				Size ksize(src.cols * 3, src.rows);
				resize(dst, dst, ksize);
				Mat roi = dst(Rect(src.cols, 0, src.cols, src.rows));
				addWeighted(roi, 0.0, src, 1.0, 0.0, roi);			
				PlateSeg[1+i]=dst.clone();
			}
		}
		//若首位头到末位尾长度过长，删去
  		if((float)plate_length/(float)finalcut.rows>4.5)
 			return -1;
		//非使领馆末位去杂
		if(style!=1&&style!=3)
		{		
			vector<int> Sx;
			Mat threshseg;
			IplImage *itep = &(IplImage(PlateSeg[7]));
			threshval = getThreshVal_Otsu_8u(itep);
			threshold(PlateSeg[7], threshseg, threshval, 255, CV_THRESH_BINARY_INV);
			sprintf(sName, "simple_result-pic-real/%d_%d_%d_threshseg.jpg", vnum,num,snum);//调试用
			imwrite(sName, threshseg);		
			Sx.clear();
			int srt=threshseg.cols-1;
			int slft=0;
			//for(int k=threshseg.cols-1;k>=0;k--)
			for(int k=0;k<threshseg.cols;k++)
			{
				int p=0;					
				for(int j=0;j<band_clip.rows;j++)
				{
					p += threshseg.at<uchar>(j, k);
				}
				Sx.push_back(p);
			}
			for(int k=0;k<Sx.size();k++)
			{
				while(k<Sx.size()&&Sx[k]==255*threshseg.rows)
					k++;
				int lft=k;
				while(k<Sx.size()&&Sx[k]!=255*threshseg.rows)
					k++;
				int rt=k;
				while(k<Sx.size()&&Sx[k]==255*threshseg.rows)
					k++;
				if(k-rt>0.1*threshseg.cols)
				{
					srt=rt;
					slft=lft;
				}
				break;
			}
			Rect s7;
			s7.x=slft;
			s7.y=0;
			s7.width=srt-slft;
			s7.height=threshseg.rows;
			if(s7.width+2<PlateSeg[7].cols)
				s7.width+=2;
			
			if(s7.x+s7.width>PlateSeg[7].cols||s7.x<0||s7.width<=0||s7.x>PlateSeg[7].cols||s7.y+s7.height>PlateSeg[7].rows)
			{
				//printf("3335ROI region illegal\n");
				return -1;
			}
			
			Mat ts = PlateSeg[7](s7);
			PlateSeg[7] = ts.clone();

			sprintf(sName, "simple_result-pic-real/%d_%d_%d_PlateSeg[7].jpg", vnum,num,snum);//调试用
			imwrite(sName, PlateSeg[7]);		
			threshseg.release();
			itep = NULL;
			itep = &(IplImage(PlateSeg[7]));
			threshval = getThreshVal_Otsu_8u(itep);
			threshold(PlateSeg[7], threshseg, threshval, 255, CV_THRESH_BINARY_INV);
			Rect r;
			//处理“1”
			if((float)PlateSeg[7].rows/(float)PlateSeg[7].cols>3.5)
			{
				Mat src=Mat(PlateSeg[7]);
				Mat dst = Mat(src.rows, src.cols, src.type(), Scalar(0, 0, 0));;
				Size ksize(src.cols * 3, src.rows);
				resize(dst, dst, ksize);
				Mat roi = dst(Rect(src.cols, 0, src.cols, src.rows));
				addWeighted(roi, 0.0, src, 1.0, 0.0, roi);			
				PlateSeg[7] = dst.clone();
			}	
		}
		//直方图均衡化去噪
			for(int i=1;i<nsize+1;i++)
			{
				//erode(PlateSeg[i],PlateSeg[i],Mat(),Point(-1,-1),1);
				medianBlur(PlateSeg[i],PlateSeg[i],3);
				equalizeHist(PlateSeg[i],PlateSeg[i]);
				if(i>1)
				{
					Mat Bck;
					IplImage *itep = &(IplImage(PlateSeg[i]));
					threshval = getThreshVal_Otsu_8u(itep);
					threshold(PlateSeg[i], Bck, threshval, 255, CV_THRESH_BINARY_INV);
					int sumpix=0;
					for(int a=0;a<PlateSeg[i].cols;a++)
					{
						for(int b=0;b<PlateSeg[i].rows;b++)
						{
							uchar s = Bck.at<uchar>(b,a);
							if(s==255)
							{
								PlateSeg[i].at<uchar>(b,a) = 0;
							}
							if(s==0)
							{
								sumpix++;
							}
						}
					}
					Rect r;          //留取字母连通区域
					if(colnum!=2)
					{
						for(int a=0;a<Bck.cols;a++)
						{
							int b=Bck.rows/2;
							int flagf=0;
							uchar s = Bck.at<uchar>(b,a);
							if(s==0)
							{
									Point seed = Point(a,b);
									int lo = 0;
									int up = 0;
									int flags = 8;
									Rect comp;
									Mat temp;
									temp=Bck.clone();
									int area = floodFill( Bck,seed, Scalar(255,255,255),
						&comp,Scalar(lo, lo, lo),Scalar(up, up, up),flags);
								
									if(comp.width*comp.height<0.5*Bck.cols*Bck.rows)
									{
										continue;
									}
								
									r=comp;							
									if(r.height/r.width<3&&r.height>0.7*Bck.rows)
									{
										for(int c=0;c<Bck.cols;c++)//去除非连通域小块
										{
											for(int d=0;d<Bck.rows;d++)
											{
												uchar s = Bck.at<uchar>(d,c);
												if(s==0)
												{
													PlateSeg[i].at<uchar>(d,c)=0;
												}
											}
										}
									
										Mat ts1 = PlateSeg[i](r);
										PlateSeg[i]=ts1.clone();
									
									}
									break;
							}
						}
					}
			}				
		}
			//imshow("p7",PlateSeg[7]);
			//waitKey(0);
		//==========0127=17:25=================
		//统一字块宽度，去除上部多余项
		if(style==0)
		{
			float aveheight=0;
			float avewidth=0;
			for(int i=2;i<=7;i++)
			{
				aveheight += PlateSeg[i].rows;
			}
			for(int i=2;i<=7;i++)
			{
				avewidth += PlateSeg[i].cols;
			}
			aveheight /= (float)6;
			avewidth /= (float)6;
			int min=1000;
			int corheight=0;
			for(int i=2;i<=7;i++)
			{
				if(abs(PlateSeg[i].rows-aveheight)<min)
				{
					corheight = PlateSeg[i].rows;
					min = abs(PlateSeg[i].rows-aveheight);
				}
			}
			
			Rect r;
			for(int i=2;i<=7;i++)
			{
				r.x = 0;
				r.y = PlateSeg[i].rows - corheight;
				if(r.y <= 0)
					continue;
				r.width = PlateSeg[i].cols;
				r.height = corheight - 1;
				if(r.height <= 0)
					return - 1;
				Mat ts1 = PlateSeg[i](r);
				PlateSeg[i] = ts1.clone();

			}

			//汉字与后一位比较
			//if(PlateSeg[1].cols>PlateSeg[2].cols)
			//{
			//	r.x=PlateSeg[1].cols-PlateSeg[2].cols;
			//	r.y=0;
			//	r.width=PlateSeg[2].cols;
			//	r.height=PlateSeg[1].rows;
			//	
			//	Mat ts1 = PlateSeg[1](r);
			//	PlateSeg[1] = ts1.clone();
			//}
			//汉字与后六位平均宽度比较
			if(PlateSeg[1].cols >= avewidth)
			{
				r.x = PlateSeg[1].cols-avewidth;
				r.y = 0;
				r.width = avewidth;
				r.height = PlateSeg[1].rows;
				Mat ts1 = PlateSeg[1](r);
				PlateSeg[1] = ts1.clone();
			}
			if(PlateSeg[1].cols< avewidth)
			{
				r.x=charSegment3[0][1]-avewidth;
				r.y=0;
				r.width=avewidth;
				r.height=PlateSeg[1].rows;
				if(r.x<0)
				{
					r.x=0;
					r.width=charSegment3[0][1];
				}
				if(r.x+r.width>ori_ud_edge.cols||r.x<0||r.width<=0||r.x>ori_ud_edge.cols||r.y+r.height>ori_ud_edge.rows)
				{
					return -1;
				}
				Mat ts1 = ori_ud_edge(r);
				PlateSeg[1].release();
				PlateSeg[1] = ts1.clone();
				//erode(PlateSeg[1],PlateSeg[1],Mat(),Point(-1,-1),1);
				medianBlur(PlateSeg[1],PlateSeg[1],3);
				equalizeHist(PlateSeg[1],PlateSeg[1]);
			}
			//===================末位过窄补全==============================
			//if(PlateSeg[7].cols<avewidth&&PlateSeg[7].cols>0.4*avewidth)
			//{
			//	r.x=charSegment3[6][0];
			//	r.y=0;
			//	r.width=avewidth;
			//	r.height=PlateSeg[7].rows;
			//	if(r.x+r.width>finalcut.cols)
			//	{
			//		r.width=finalcut.cols-charSegment3[6][0];
			//	}
			//	if(r.x+r.width>ori_ud_edge.cols||r.x<0||r.width<=0||r.x>ori_ud_edge.cols||r.y+r.height>ori_ud_edge.rows)
			//	{
			//		return -1;
			//	}
			//	Mat ts1 = ori_ud_edge(r);
			//	PlateSeg[7] = ts1.clone();
			//	erode(PlateSeg[7],PlateSeg[7],Mat(),Point(-1,-1),1);
			//	medianBlur(PlateSeg[7],PlateSeg[7],3);
			//	equalizeHist(PlateSeg[7],PlateSeg[7]);
			//	Mat Bck;
			//	threshold(PlateSeg[7], Bck, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
			//	for(int a=0;a<PlateSeg[7].cols;a++)
			//	{
			//		for(int b=0;b<PlateSeg[7].rows;b++)
			//		{
			//			uchar s = Bck.at<uchar>(b,a);
			//			if(s==0)
			//			{
			//				PlateSeg[7].at<uchar>(b,a) = 0;
			//			}
			//		}
			//	}

			//}
			if(PlateSeg[7].cols>avewidth)
			{
				Rect roi;
				roi.x=0;
				roi.y=0;
				roi.width=avewidth;
				roi.height=PlateSeg[7].rows;
				Mat ts=PlateSeg[7](roi);
				PlateSeg[7]=ts.clone();
			}
			if(PlateSeg[1].rows>aveheight)
			{
				Mat Head;
				threshold(PlateSeg[1], Head, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
				//imshow("head",Head);
				//waitKey(0);
				vector<int> px1;
				for(int a=0;a<Head.rows;a++)
				{
					int p=0;
					for(int b=0;b<Head.cols;b++)
					{
						p += Head.at<uchar>(a,b);
					}
					px1.push_back(p);
				}
				int down=px1.size()-1;
				for(int a=px1.size()-1;a>=0;a--)
				{
					if(px1[a]!=0)
					{
						down=a;
						break;
					}
				}
				int up=0;
				for(int a=0;a<px1.size();a++)
				{
					if(px1[a]!=0)
					{
						up=a;
						break;
					}
				}
				//Rect roi;
				//roi.x=0;
				//roi.y=0;
				//roi.width=Head.cols;
				//roi.height=down;
				Rect roi;
				roi.x=0;
				roi.y=up;
				roi.width=Head.cols;
				roi.height=down-up+1;
				//if(down>aveheight)
				//{
				//	roi.y=down-aveheight;
				//	roi.height=aveheight;
				//}
				if(roi.y-2>=0)
				{
					roi.y -= 2;
					roi.height += 2;
				}
				if(roi.height+roi.y+2<=PlateSeg[1].rows)
				{
					roi.height += 2;
				}
				if(roi.y<0||roi.x+roi.width>PlateSeg[1].cols||roi.y+roi.height>PlateSeg[1].rows)
					return -1;
				Mat pt=PlateSeg[1](roi);
				PlateSeg[1]=pt.clone();
			}
		}
		//imshow("p7",PlateSeg[7]);
		//waitKey(0);
		//vector<int> P7;
		//for(int i=0;i<PlateSeg[7].rows;i++)
		//{
		//	int p=0;
		//	for(int j=0;j<PlateSeg[7].cols;j++)
		//	{
		//		p += PlateSeg[7].at<uchar>(i,j);
		//	}
		//	P7.push_back(p);
		//}
		//for(int i=0;i<P7.size();i++)
		//{

		//}
		//Mat Bck;
		//IplImage *itep = &(IplImage(PlateSeg[1]));
		//threshval = getThreshVal_Otsu_8u(itep);
		//threshold(PlateSeg[1], Bck, threshval, 255, CV_THRESH_BINARY_INV);
		//imshow("12",Bck);
		//waitKey(0);
		int ave_wid_final=0;
		for(int i=1;i<nsize+1;i++)
		{
			//erode(PlateSeg[i],PlateSeg[i],Mat(),Point(-1,-1),1);
			block.push_back(PlateSeg[i]);
			sprintf(sName, "seg/%d_%d_%d_%d.jpg", vnum,num,snum,i);
			imwrite(sName, PlateSeg[i]);
			ave_wid_final += PlateSeg[i].cols;
		}
		ave_wid_final /= nsize;
		//if(ave_wid_final<37) return -1;
			Px.swap(vector<int> ());
			Py.swap(vector<int> ());
			st=style;
			return secTest;
		}
		else return -1;
}




