
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

//车辆信息类
class CarInfo
{
public:
	CarInfo();
	~CarInfo();

	Rect rectPos;  //车辆位置
	CvPoint ptCenter ; //车辆中心点位置
	Mat matCarPic;  //车辆图片
	string key; 

	string strCarPlate ; //车辆内容
	string strColor ;  //车牌颜色

	string strCarinfo[4] ; //四个图片的路径及文件名 0，2全景，1，3近景
	string strComboPic ; //组图路径及图片文件名
	char strSeq[8] ; //序列号
	bool bZoomed  ; //是否抓拍了
	bool bPhoto_flag; 
	bool bTakePhoto ;//是否抓拍了图片
	bool bFirstPhoto ;	//是否是抓拍了第一张图片
	bool bLastPhoto  ;  //是否是抓拍了最后一张图片
	bool bSendMessage ; //是否发了短信
	bool bMatch ;  // 是否匹配
	bool bWrongDetect ; //是否错误抓拍
    bool bDelete ; //是否应该删除

	bool bDetect  ;  //是否检测到
	int iFourInOne  ;  //四合一 0 表示没有抓拍完四张，1表示抓拍了四张，2表示四张合成了  
	int iStartTime; //违停开始计数时间
	int iCurrentTime; //目前的时间

	int iMissTime  ; //未匹配的次数

	int iColor; 
	int iLane;
	double dContourArea;

};

//这类里面只有 carData 有用，其他都是北邮写的，没意义
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
	
  vector<CarInfo>  carData  ;  //车辆信息

	unordered_map<string, CarNode*> umap;
	
	CarNode * Tail() ;
	CarNode * Head() ;

	void Insert(string key, Rect val, Mat bg) ;
	void Delete(string key) ;
	bool match(string key, Rect value) ;
	 bool PosMatch (  Rect  rectObject ) ;

};

#endif 