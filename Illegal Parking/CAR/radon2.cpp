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
**pr������radon�任���������Ķ���һ���ض�theta�ǵ��е��׵�ַ 
**pixel��Ҫ����radon�任������ֵ����0.25�Ժ��ֵ������ÿһ�����ص�ȡ�������ĸ�����߾��ȣ����ڼ���ʱpixelҲҪ��Ӧ����0.25��������һ����ռ0.25�ı�����Ȼ���ĸ���պô���1�ķݶ 
**r������radon�任�ĸõ����ʼ��rֵ����rFirst֮��Ĳ� 
**/   
void incrementRadon(double *pr, double pixel, double r)
{     
	int r1;     
	double delta;     

	r1 = (int) r;   //����ÿһ���㣬rֵ��ͬ�����ԣ�ͨ�����ַ�ʽ�����԰���һ������Ӧ�е�Ԫ�ص�ֵ������  
	delta = r - r1;     
	pr[r1] += pixel * (1.0 - delta); //radon�任��������ͨ����¼Ŀ��ƽ����ĳһ��ı�ӳ����Ļ��ۺ��������ԭƽ���ֱ�ߵĴ����Եģ���Ϊ+=    
	pr[r1+1] += pixel * delta;  //�����㻥����ϣ���߾���   
}     
/*** 
**�������ͣ� 
**pPtr������radon�任�������һά���飬��һά��������ʵҪ��ԭ��һ��rSize*numAngles�ľ��� 
**iPtr����Ҫ����radon�任�ľ����һά����洢���� 
**thetaPtr��ָ������radon�任�Ļ��ȵ����飬�ýǶȾ��Ǽ�������ƫ��������ĽǶ� 
**M��Ҫ����radon�任�ľ�������� 
**N��Ҫ����radon�任�ľ�������� 
**xOrigin��Ҫ����radon�任�ľ���ĵ����ĵĺ����� 
**yOrigin��Ҫ����radon�任�ľ�������ĵ������� 
**numAngles��thetaPtr������Ԫ�صĸ��� 
**rFist���������г�ʼ����任ԭ��ľ��� 
**rSize������radon�任�м�����ĵ�֮�����Զ���� 
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
	xCosTable = (double *) calloc(2*N, sizeof(double));  //MATLAB���ڴ����뺯������ӦC���Կ��Ի���calloc����   
	ySinTable = (double *) calloc(2*M, sizeof(double));     

	for (k = 0; k < numAngles; k++)   
	{     
		//ÿһ��theta�ǣ�����radon�仯�󣬾ͻ����һ�����ݣ���һ�������У�����rSize������  
		angle = thetaPtr[k];     
		pr = pPtr + k*rSize;  /* pointer to the top of the output column */     
		cosine = cos(angle);      
		sine = sin(angle);        

		/* Radon impulse response locus:  R = X*cos(angle) + Y*sin(angle) */     
		/* Fill the X*cos table and the Y*sin table.                      */     
		/* x- and y-coordinates are offset from pixel locations by 0.25 */     
		/* spaced by intervals of 0.5. */     
		/* 
		**radon �任�У��������£���r���theta�Ǻ�ÿһ�����ص�ķֲ����Ƿ����Եģ����˴����õ�������radon�任�� 
		**Ϊ����߾��ȣ���ÿһ�����ص�ֳ��������ĸ����ڵ����ص������м��㣡x��y��������������0.25 
		*/  
		for (n = 0; n < N; n++)     
		{     
			x = n - xOrigin;     
			xCosTable[2*n]   = (x - 0.25)*cosine;   //�ɼ������֪ʶ֪��������ڱ任��ԭ�㣬������ǵõ��˸õ�ĺ�����  
			xCosTable[2*n+1] = (x + 0.25)*cosine;     
		}     
		for (m = 0; m < M; m++)     
		{     
			y = yOrigin - m;     
			ySinTable[2*m] = (y - 0.25)*sine;   //ͬ������ڱ任��ԭ�㣬�õ���������  
			ySinTable[2*m+1] = (y + 0.25)*sine;     
		}     

		pixelPtr = iPtr;     
		for (n = 0; n < N; n++) // ���������λ�ȡ��������ֵ������I�е�����Ӧ���������Ⱥ�ת��Ϊ��һά����    
		{     
			for (m = 0; m < M; m++)   //����ԭ�����е�ÿһ�����ص�  
			{     
				pixel = *pixelPtr++;     
				if (pixel != 0.0)   //����õ�����ֵ��Ϊ0��Ҳ��ͼ������  
				{     
					pixel *= 0.25;     

					//һ�����ص�ֽ���ĸ��ٽ������ص���м��㣬��߾�ȷ��  
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

	free((void *) xCosTable);   //MATLAB���ڴ��ͷź�������ӦC���Կ��Ի���free����  
	free((void *) ySinTable);     
}   

//nlhs���������������plhs����������Ĵ洢����  
//nrhs���������������prhs����������Ĵ洢����  

void mexFunction(double* I, int rows, int cols, double *theta, int numAngles, int& rSize)     
{     
	//���������ʵ������ô���ģ������Ҫ��MATLAB��C���Ի�ϱ��ʱ��һ���ӿں�����û��ʵ��radon�任�ĺ��Ĺ���  
	//radon�任�ĺ��Ĺ�����radon��incrementRadon������  
	//����radon�����Ĳ����ڶ࣬���Կ��Բο��������������Щ������ʲô��˼����ô��ֵ  
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
	thetaPtr = (double *) calloc(numAngles, sizeof(double));   //MATLAB���ڴ����뺯������ӦC���Կ��Ի���calloc����  
	pr1 = theta;   //��ȡ��������������������׵�ַ  
	pr2 = thetaPtr;     
	for (k = 0; k < numAngles; k++) // ������ĸ����Ƕ�ת��Ϊ���ȣ��洢��pr2��
		*(pr2++) = *(pr1++) * deg2rad;     

	M = rows;   //���ͼ����������  
	N = cols;   //���ͼ����������  

	/* ��ת���ĵ�Where is the coordinate system's origin? */     
	xOrigin = MAXX(0, (N-1)/2);     
	yOrigin = MAXX(0, (M-1)/2);     

	/* How big will the output be? */     
	temp1 = M - 1 - yOrigin;// ����һ��ĳ���     
	temp2 = N - 1 - xOrigin;// ����һ��ĳ���
	// �Խ���һ��ĳ���
	rLast = (int) ceil(sqrt((double) (temp1*temp1+temp2*temp2))) + 1;     
	rFirst = -rLast;     
	rSize = rLast - rFirst + 1;  // �Խ��ߵĳ���   

	R = new double[rSize];   //����һ��rSize��1�еľ�����ʵҲ����һ������  
	pr1 = R;     
	for (k = rFirst; k <= rLast; k++) {   
		*(pr1++) = (double) k;         
		//cout << *(pr1-1) << " ";
	}

	P = new double[rSize*numAngles];    //����rSize* numAngles��С�Ķ�ά����
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
	// dst ��ת���ͼ�񣬷���ֵ
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
