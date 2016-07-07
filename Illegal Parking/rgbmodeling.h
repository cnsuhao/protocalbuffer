#include <opencv2\opencv.hpp>

using namespace cv;

IplImage* RGBmodeling(IplImage* img, double a=-1.25)
{
	//TRACE("current img add: %d\n",img);
	IplImage* img_new = img;
	if(img == NULL)
		return NULL;
	int width=img->width;
	int height=img->height;
	double E=2.71828;;
	int i,j;
	double** R;
	double** G;
	double** B;
	double sumB = 0.0,sumG = 0.0, sumR = 0.0;
	double avg1,avg2,avg3;
	double temp1,temp2,temp3;
	
	try
	{
	R = new double*[height];
	G = new double*[height];
	B = new double*[height];

	for (i = 0; i < height; ++i) 
	{
		R[i] = new double[width];
		G[i] = new double[width];
		B[i] = new double[width];
	}
	//TRACE("before cvGet2D\n");
	for (i=0;i<height;i++)
	{
		for (j=0;j<width;j++)
		{
			CvScalar t=cvGet2D(img_new,i,j);
		    double   s0=t.val[0];//B
		    double   s1=t.val[1];//G
		    double   s2=t.val[2];//R
			B[i][j] = s0;
			G[i][j] = s1;
			R[i][j] = s2;
			sumB += s0;
			sumG += s1;
			sumR += s2;
		}
	}
	//TRACE("after cvGet2D\n");
	double temp = width*height;
	for (i=0;i<height;i++)
	{
		for (j=0;j<width;j++)
		{

			avg1=(a)*double(B[i][j]*temp)/double(sumB);     
			avg2=(a)*double(G[i][j]*temp)/double(sumG);     
			avg3=(a)*double(R[i][j]*temp)/double(sumR);

			temp1 = pow(E*1.0,double(avg1));
			temp2 = pow(E*1.0,double(avg2));
			temp3 = pow(E*1.0,double(avg3));

			B[i][j]=255*((1-temp1)/(1+temp1));
			G[i][j]=255*((1-temp2)/(1+temp2));
			R[i][j]=255*((1-temp3)/(1+temp3)); 
		}
	}
	img_new = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,3);
	img_new->origin = img->origin;
	//img_new->origin = 1;
	for (i=0;i<height;i++)
	{
		for (j=0;j<width;j++)
		{
			CvScalar m_BGR=cvScalar(B[i][j],G[i][j],R[i][j],0);
			cvSet2D(img_new,i,j,m_BGR);
		}
	}
	for (int i = 0; i < height;i++) 
	{
		delete[] R[i]; R[i] = NULL;
		delete[] G[i]; G[i]=NULL;
		delete[] B[i]; B[i]=NULL;
		
	}
	delete[] R; R = NULL;
	delete[] G; G = NULL;
	delete[] B; B = NULL;
	//cvReleaseImage(&img);
	}
	catch ( cv::Exception & excep )
	{
		
	}
	
	return img_new;
}
void log(char* filename,char* picname, vector<double>& vec)
{
	FILE * fp ;
	if((fp = fopen(filename,"a"))==NULL)
	{
		return;
	}
	fprintf(fp,"%s\t",picname);
	for(int i=0;i<vec.size()-1;i++)
		fprintf(fp,"%.2lf\t",vec[i]);
	if(vec.size()>0)
		fprintf(fp,"%.2lf\n",vec[vec.size()-1]);
	fclose(fp);

}