#include "StdAfx.h"
#include "hashmap.h"


 CarInfo::  CarInfo()
{
	 iStartTime = -1;
	 iCurrentTime = -1;
	 dContourArea = -1;
	 bZoomed = false ;
	 bFirstPhoto = false ;
	 bPhoto_flag = false;
	 bTakePhoto = false;
	 bLastPhoto  = false;
	 bSendMessage = false ;
	 bMatch = true ;
	 bDetect = false ;
	 bDelete = false ;
	 bWrongDetect = false ;
	 iFourInOne = 0 ;
	 iMissTime = 0 ;
	 iColor;
	 iLane;
	 for(int i = 0 ; i < 4 ; i++) 
	 {
			strCarinfo[i].clear() ;
	 }
}

 CarInfo :: ~CarInfo()
{


}

Vehicle	::Vehicle(){
		head = new CarNode();
		tail = new CarNode();
		head->next = tail;
		tail->pre  = head;
	}

	Vehicle	::~Vehicle(){

		while (tail != head && tail->pre != head)
		{
			CarNode* p;
			p = tail;
			tail = tail->pre;
			delete p;
		}
		delete head;
		delete tail;
	}

	CarNode* Vehicle:: Tail()
	{
		return tail;
	}

	CarNode* Vehicle::  Head()
	{
		return head;
	}

	void Vehicle :: Insert (string key, Rect val, Mat bg){
		if (umap.find(key) != umap.end()) {
			return;
		}

		CarNode* insert = new CarNode(key, val, bg);
		insert->pre = tail->pre;
		insert->next = tail;
		insert->pre->next = insert;
		tail->pre = insert;
		umap[key] = insert;
	}

	void Vehicle::Delete (string key){
		if (umap.find(key) == umap.end()) {
			return;
		}

		CarNode* temp;
		temp = umap[key];
		umap.erase(key);
		temp->pre->next = temp->next;
		temp->next->pre = temp->pre;
		delete temp;
	}


	bool  Vehicle::match (string key, Rect value)
	{
		CarNode *p;
		p = head->next;

		while(p != tail)
		{
			Rect val = p->val;
			Point pt;
			pt.x = value.x + value.width/2;
			pt.y = value.y + value.height/2;

			if (pt.x > val.x && pt.x < (val.x + val.width) &&
				pt.y > val.y && pt.y < (val.y + val.height))
				return true;
			p = p->next;
		}	
		return false;
	}

	bool  Vehicle::PosMatch ( Rect rectObject )
	{

		for (int i = 0 ; i < carData.size(); i++ )
		{
			Rect  rectTemp = carData[i].rectPos;

 			Point pt;
			pt.x = rectObject.x + rectObject.width/2;
			pt.y = rectObject.y + rectObject.height/2;

			if (pt.x > rectTemp.x && pt.x < (rectTemp.x + rectTemp.width) &&
				pt.y > rectTemp.y && pt.y < (rectTemp.y + rectTemp.height))
				return true;
		}
		return false;
	}


