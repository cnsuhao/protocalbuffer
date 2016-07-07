#pragma once
#ifndef FOREGROUND_MODEL_H
#define FOREGROUND_MODEL_H

#include <opencv2/opencv.hpp>
#include <opencv2/highgui//highgui.hpp>
#include <iostream>


using namespace std;
using namespace cv;

//ÿ�����ص����������
#define defaultNbSamples 20

//#minָ��
#define defaultReqMatches 2

//Sqthere�뾶
#define defaultRadius 20

//�Ӳ�������
#define defaultSubsamplingFactor 16

//��������
#define background 0

//ǰ������
#define foreground 255		

class Foreground_Model
{
public:

	Foreground_Model(void);
	~Foreground_Model(void);

	//��������ViBe_Model
	//�������ܣ���ȡǰ������
	//����˵����
	//IplImage* src��ԭʼ�Ķ�ͨ��ͼ��
	//IplImage* dst�����º��ǰ��ͼ��
	//����ֵ���޷���ֵ
	void ViBe_Model(IplImage* src,IplImage* dst, bool &bReset) ;

	//���洦������Ϣ
	CvMat* segMat;

private:

	//������Ƶ֡��
	int nFrmNum;

	//����pFrame��Ӧ�ĻҶ�ͼ��
	IplImage* pAfter;

	//����pFrame��Ӧ�ĻҶȾ���
	CvMat* pAfterMat;

	//��������Initialize
	//�������ܣ���ʼ������
	//����˵����
	//IplImage* src��ԭʼ�Ķ�ͨ��ͼ��
	//RNG rng�������������
	//����ֵ���޷���ֵ
	void Initialize(CvMat* pFrameMat,RNG rng);

	//��������update
	//�������ܣ����º���
	//����˵����
	//CvMat* pFrameMat��ԭʼ�Ķ�ͨ��ͼ��
	//CvMat* segMat:�ָ���ǰ��ͼ��
	//RNG rng�������������
	//int nFrmNum: ͳ���������Ƶ֡��
	//����ֵ���޷���ֵ
	void update(CvMat* pFrameMat,CvMat* segMat,RNG rng);

};
#endif