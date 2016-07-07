#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdlib.h>
#include <io.h>
//#include "vld.h"
using namespace cv;
using namespace std;

#define MAXX(x,y) ((x) > (y) ? (x) : (y))     

/* Input Arguments */     
/*#define I      (prhs[0])     
#define THETA  (prhs[1])     
#define OPTION (prhs[2])  */   

/* Output Arguments */     
double* P;    
double* R; 

/** 
**pr：进行radon变换后输出矩阵的对于一个特定theta角的列的首地址 
**pixel：要进行radon变换的像素值乘以0.25以后的值（由于每一个像素点取了相邻四个点提高精度，故在计算时pixel也要相应乘以0.25，类似于一个点占0.25的比例，然后四个点刚好凑足1的份额） 
**r：进行radon变换的该点与初始的r值——rFirst之间的差 
**/   
void incrementRadon(double *pr, double pixel, double r)
{     
	int r1;     
	double delta;     

	r1 = (int) r;   //对于每一个点，r值不同，所以，通过这种方式，可以把这一列中相应行的元素的值给赋上  
	delta = r - r1;     
	pr[r1] += pixel * (1.0 - delta); //radon变换本来就是通过记录目标平面上某一点的被映射后点的积累厚度来反推原平面的直线的存在性的，故为+=    
	pr[r1+1] += pixel * delta;  //两个点互相配合，提高精度   
}     
/*** 
**参数解释： 
**pPtr：经过radon变换后输出的一维数组，该一维数组是其实要还原成一个rSize*numAngles的矩阵 
**iPtr：需要进行radon变换的矩阵的一维数组存储形势 
**thetaPtr：指定进行radon变换的弧度的数组，该角度就是极坐标中偏离正方向的角度 
**M：要进行radon变换的矩阵的行数 
**N：要进行radon变换的矩阵的列数 
**xOrigin：要进行radon变换的矩阵的的中心的横坐标 
**yOrigin：要进行radon变换的矩阵的中心的纵坐标 
**numAngles：thetaPtr数组中元素的个数 
**rFist：极坐标中初始点与变换原点的距离 
**rSize：整个radon变换中极坐标的点之间的最远距离 
***/   
static void radon(double *pPtr, double *iPtr, double *thetaPtr, int M, int N,      
	int xOrigin, int yOrigin, int numAngles, int rFirst, int rSize)     
{     
	int k, m, n;              /* loop counters */     
	double angle;             /* radian angle value */     
	double cosine, sine;      /* cosine and sine of current angle */     
	double *pr;               /* points inside output array */     
	double *pixelPtr;         /* points inside input array */     
	double pixel;             /* current pixel value */     
	double *ySinTable, *xCosTable;     
	/* tables for x*cos(angle) and y*sin(angle) */     
	double x,y;     
	double r, delta;     
	int r1;     

	/* Allocate space for the lookup tables */     
	xCosTable = (double *) calloc(2*N, sizeof(double));  //MATLAB的内存申请函数，对应C语言可以换成calloc函数   
	ySinTable = (double *) calloc(2*M, sizeof(double));     

	for (k = 0; k < numAngles; k++)   
	{     
		//每一个theta角，经过radon变化后，就会产生一列数据，这一列数据中，共有rSize个数据  
		angle = thetaPtr[k];     
		pr = pPtr + k*rSize;  /* pointer to the top of the output column */     
		cosine = cos(angle);      
		sine = sin(angle);        

		/* Radon impulse response locus:  R = X*cos(angle) + Y*sin(angle) */     
		/* Fill the X*cos table and the Y*sin table.                      */     
		/* x- and y-coordinates are offset from pixel locations by 0.25 */     
		/* spaced by intervals of 0.5. */     
		/* 
		**radon 变换中，极坐标下，沿r轴的theta角和每一个像素点的分布都是非线性的，而此处采用的是线性radon变换， 
		**为了提高精度，把每一个像素点分成其四周四个相邻的像素点来进行计算！x、y坐标的误差是正负0.25 
		*/  
		for (n = 0; n < N; n++)     
		{     
			x = n - xOrigin;     
			xCosTable[2*n]   = (x - 0.25)*cosine;   //由极坐标的知识知道，相对于变换的原点，这个就是得到了该点的横坐标  
			xCosTable[2*n+1] = (x + 0.25)*cosine;     
		}     
		for (m = 0; m < M; m++)     
		{     
			y = yOrigin - m;     
			ySinTable[2*m] = (y - 0.25)*sine;   //同理，相对于变换的原点，得到了纵坐标  
			ySinTable[2*m+1] = (y + 0.25)*sine;     
		}     

		pixelPtr = iPtr;     
		for (n = 0; n < N; n++) // 列优先依次获取各个像素值，所以I中的内容应该是列优先后转化为了一维数组    
		{     
			for (m = 0; m < M; m++)   //便利原矩阵中的每一个像素点  
			{     
				pixel = *pixelPtr++;     
				if (pixel != 0.0)   //如果该点像素值不为0，也即图像不连续  
				{     
					pixel *= 0.25;     

					//一个像素点分解成四个临近的像素点进行计算，提高精确度  
					r = xCosTable[2*n] + ySinTable[2*m] - rFirst;     
					incrementRadon(pr, pixel, r);     

					r = xCosTable[2*n+1] + ySinTable[2*m] - rFirst;     
					incrementRadon(pr, pixel, r);     

					r = xCosTable[2*n] + ySinTable[2*m+1] - rFirst;     
					incrementRadon(pr, pixel, r);     

					r = xCosTable[2*n+1] + ySinTable[2*m+1] - rFirst;     
					incrementRadon(pr, pixel, r);     
				}     
			}     
		}     
	}     

	free((void *) xCosTable);   //MATLAB的内存释放函数，对应C语言可以换成free函数  
	free((void *) ySinTable);     
}   

//nlhs是输出变量个数，plhs是输出变量的存储数组  
//nrhs是输入变量个数，prhs是输入变量的存储数组  

void mexFunction(double* I, int rows, int cols, double *theta, int numAngles, int& rSize)     
{     
	//这个函数其实不用怎么看的，这个主要是MATLAB与C语言混合编程时的一个接口函数，没有实现radon变换的核心功能  
	//radon变换的核心功能在radon和incrementRadon函数中  
	//由于radon函数的参数众多，所以可以参考这个函数，看那些参数是什么意思，怎么赋值  
	//int numAngles;          /* number of theta values */     
	double *thetaPtr;       /* pointer to theta values in radians */     
	double *pr1, *pr2;      /* double pointers used in loop */     
	double deg2rad;         /* conversion factor */     
	double temp;            /* temporary theta-value holder */     
	int k;                  /* loop counter */     
	int M, N;               /* input image size */     
	int xOrigin, yOrigin;   /* center of image */     
	int temp1, temp2;       /* used in output size computation */     
	int rFirst, rLast;      /* r-values for first and last row of output */     
	//int rSize;              /* number of rows in output */     


	/* Get THETA values */     
	deg2rad = 3.14159265358979 / 180.0;       
	thetaPtr = (double *) calloc(numAngles, sizeof(double));   //MATLAB的内存申请函数，对应C语言可以换成calloc函数  
	pr1 = theta;   //获取矩阵的数组表达，返回数组的首地址  
	pr2 = thetaPtr;     
	for (k = 0; k < numAngles; k++) // 将输入的各个角度转换为弧度，存储于pr2中
		*(pr2++) = *(pr1++) * deg2rad;     

	M = rows;   //获得图像矩阵的行数  
	N = cols;   //获得图像矩阵的列数  

	/* 旋转中心点Where is the coordinate system's origin? */     
	xOrigin = MAXX(0, (N-1)/2);     
	yOrigin = MAXX(0, (M-1)/2);     

	/* How big will the output be? */     
	temp1 = M - 1 - yOrigin;// 纵向一半的长度     
	temp2 = N - 1 - xOrigin;// 横向一半的长度
	// 对角线一半的长度
	rLast = (int) ceil(sqrt((double) (temp1*temp1+temp2*temp2))) + 1;     
	rFirst = -rLast;     
	rSize = rLast - rFirst + 1;  // 对角线的长度   

	R = new double[rSize];   //创建一个rSize行1列的矩阵，其实也就是一个数组  
	pr1 = R;     
	for (k = rFirst; k <= rLast; k++) {   
		*(pr1++) = (double) k;         
		//cout << *(pr1-1) << " ";
	}

	P = new double[rSize*numAngles];    //创建rSize* numAngles大小的二维数组
	for (int i = 0; i < rSize*numAngles; i++) {
		P[i] = 0;
	}
	
	radon(P, I, thetaPtr, M, N, xOrigin, yOrigin, numAngles, rFirst, rSize);
	free(thetaPtr);
}

void imrotate(IplImage* src, int angle,Mat &dst0) {
    size_t found1,found2;
    
    IplImage* dst;
    dst = cvCloneImage( src );
    //int angle = atoi(angleStr.c_str());
    CvMat* rot_mat = cvCreateMat(2,3,CV_32FC1);
    CvPoint2D32f center = cvPoint2D32f(
        src->width/2,
        src->height/2
    );
    double scale = 1;
    cv2DRotationMatrix( center, angle, scale, rot_mat );
    cvWarpAffine( src, dst, rot_mat);
    //char angStr[4];
    //sprintf(angStr,"%d",angle);
	// dst 旋转后的图像，返回值
    //cvSaveImage(outPath.c_str(),dst);
//    cvReleaseImage(&src);
    //cvReleaseImage(&dst);
	dst0=Mat(dst);
    cvReleaseMat( &rot_mat );
}

void correct(Mat src, Mat &dst) {
	double* I;
	double* theta;
	Mat img, imgSrc, edge, gray;
	int numAngles = 180;
	int rSize = 0;
	
	img = src;
	gray = src;
	blur( gray, edge, Size(3,3) ); 
	Canny( edge, imgSrc, 2, 19, 3 );  
	//imshow("edge", imgSrc);
	//waitKey();

	int h = imgSrc.rows;
	int w = imgSrc.cols;

	I = new double[w*h];
	int count = 0;
	for (int j = 0; j < w; j++)  
	{    
		for (int i = 0; i < h; i++){    
			I[count++] =  (double)(* imgSrc.row(i).col(j).data) > 0.0001  ? 1.0 : 0.0;  
		}  
	} 
	
	theta = new double[numAngles];
	for (int i = 0; i < numAngles; i++) {
		theta[i] = i + 1;
	}
	mexFunction(I, h, w, theta, numAngles, rSize);

	//FILE *fp = fopen("res.txt", "w");
	double max_val = 0;
	int max_row = 0, max_col = 0;
	for (int i = 0; i < rSize; i++){
		for (int j = 0; j < numAngles; j++) {
			//fprintf(fp, "%.5f\t", P[j*rSize + i]);
			if(max_val <  P[j*rSize + i]) {
				max_val = P[j*rSize + i];
				max_row = i;
				max_col = j;
			}
		}
		//fprintf(fp, "\n");
	}
	//cout <<endl<<" maxVal: " << max_val << " maxRow: "<< max_row <<" maxCol: "<< max_col<<endl;
	//fclose(fp);
	int qingxiejiao;
	if(max_col < 90)
        qingxiejiao = 360 - (max_col + 1);
    else 
        qingxiejiao=180 - (max_col + 1);
	imrotate(&IplImage(img), qingxiejiao, dst); 
	delete[] P;
	delete[] R;
	delete[] I;
	delete[] theta;
}
