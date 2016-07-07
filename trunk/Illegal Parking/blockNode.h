///////////跟踪模块数据结构
#ifndef _BLOCKNODE_H
#define _BLOCKNODE_H

const int MAX_STEP = 200;
typedef struct blockTrack
{
	bool flag;
	//bool crsAear;
	int step;
	CvPoint pt[MAX_STEP];	
	CvRect rect;
	int iLane; // lane number
}blockTrack;



#endif 