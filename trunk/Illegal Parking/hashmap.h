
#ifndef _HASHMAP_H
#define _HASHMAP_H
#pragma once 
#include <unordered_map>
#include <string>
#include	<opencv/highgui.h>
#include	<opencv/cvaux.h>
#include	<opencv/cxcore.h>
#include	"opencv2/core/core.hpp"
#include	"opencv2/imgproc/imgproc.hpp"
#include	"opencv2/video/background_segm.hpp"
#include	"opencv2/objdetect/objdetect.hpp"
#include	"opencv2/highgui/highgui.hpp"
using namespace  std;
using namespace cv;

typedef struct CarNode {
	string key;
	char strSeq[8] ;
	Rect val;
	Mat bg;
	int frame_s;
	int frame_e;
	double contour_area;
	int bFirstPhoto ;
	bool photo_flag;
	bool bTakePhoto ;
	int  bLastPhoto  ;
	int color;
	int iLane;
	CarNode* pre;
	CarNode* next;
	CarNode(string key, Rect val, Mat bg) : key(key), frame_s(-1), val(val), bg(bg), pre(NULL), next(NULL){};
	CarNode() : pre(NULL), next(NULL) {};
	
}CarNode;

//������Ϣ��
class CarInfo
{
public:
	CarInfo();
	~CarInfo();

	Rect rectPos;  //����λ��
	CvPoint ptCenter ; //�������ĵ�λ��
	Mat matCarPic;  //����ͼƬ
	string key; 

	string strCarPlate ; //��������
	string strColor ;  //������ɫ

	string strCarinfo[4] ; //�ĸ�ͼƬ��·�����ļ��� 0��2ȫ����1��3����
	string strComboPic ; //��ͼ·����ͼƬ�ļ���
	char strSeq[8] ; //���к�
	bool bZoomed  ; //�Ƿ�ץ����
	bool bPhoto_flag; 
	bool bTakePhoto ;//�Ƿ�ץ����ͼƬ
	bool bFirstPhoto ;	//�Ƿ���ץ���˵�һ��ͼƬ
	bool bLastPhoto  ;  //�Ƿ���ץ�������һ��ͼƬ
	bool bSendMessage ; //�Ƿ��˶���
	bool bMatch ;  // �Ƿ�ƥ��
	bool bWrongDetect ; //�Ƿ����ץ��
    bool bDelete ; //�Ƿ�Ӧ��ɾ��

	bool bDetect  ;  //�Ƿ��⵽
	int iFourInOne  ;  //�ĺ�һ 0 ��ʾû��ץ�������ţ�1��ʾץ�������ţ�2��ʾ���źϳ���  
	int iStartTime; //Υͣ��ʼ����ʱ��
	int iCurrentTime; //Ŀǰ��ʱ��

	int iMissTime  ; //δƥ��Ĵ���

	int iColor; 
	int iLane;
	double dContourArea;

};

//��������ֻ�� carData ���ã��������Ǳ���д�ģ�û����
class Vehicle {
private:
	CarNode* head; 
	CarNode* tail;
	/*void moveToTail(CarNode* cur) {
	cur->pre = tail->pre;
	cur->next = tail;
	cur->pre->next = cur;
	tail->pre = cur;
	}*/
public:

	Vehicle();
	~Vehicle() ;
	
  vector<CarInfo>  carData  ;  //������Ϣ

	unordered_map<string, CarNode*> umap;
	
	CarNode * Tail() ;
	CarNode * Head() ;

	void Insert(string key, Rect val, Mat bg) ;
	void Delete(string key) ;
	bool match(string key, Rect value) ;
	 bool PosMatch (  Rect  rectObject ) ;

};

#endif 