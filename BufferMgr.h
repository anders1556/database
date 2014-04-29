/*
 * BufferMgr.h
 *
 *  Created on: 2014年4月28日
 *      Author: houbo
 */

#ifndef BUFFERMGR_H_
#define BUFFERMGR_H_

#include<iostream>
#include<stdlib.h>

const int FRAMESIZE = 4096;
const int DEFBUFSIZE = 1024;

struct bFrame
{
        char field[FRAMESIZE];
};//page
//bFrame buf[DEFBUFSIZE];//buffer

const int LISTSIZE = 16;
const int LRUSIZE = 64;//64 * 16 = 1024

const int mask = 0x1F;
const int HOTLEN = 8;
const int COLDLEN = 8;
typedef int _PageID;
typedef int _FrameID;
struct BCB
{
        _PageID page_id;
        _FrameID frame_id;
        int latch;
        int count;

        int ftime;//cycle size:1024
        int stime;

        bool dirty;
        BCB * next;
        BCB * pre;
};

struct LruCtrl
{
        BCB* head;//point the circle list hot head
        BCB* tail;//point the circle list could head
        int length;
};

class BufferMgr {
public:
	BufferMgr();
	virtual ~BufferMgr();
	void initLruCtrl();
	bool isContainAndUpdate(_PageID pageID);
	bool isFull(_PageID pageID);
	void notFullAndInsert(_PageID pageID);
	void fullAndInsert(_PageID pageID);
	long printCount();
	long printHit();

private:
	LruCtrl ptof[LRUSIZE];
	BCB* isContain(_PageID pageID);
	BCB* newBCB(_PageID pageID, int length);
	BCB* newBCB(_PageID pageID, BCB* old);
	BCB* drop(_PageID pageID);
	long count;
	long hit;
};

#endif /* BUFFERMGR_H_ */
