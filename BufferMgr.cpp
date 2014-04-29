/*
 * BufferMgr.cpp
 *
 *  Created on: 2014年4月28日
 *      Author: houbo
 */

#include "BufferMgr.h"

BufferMgr::BufferMgr():count(0),hit(0) {
	// TODO Auto-generated constructor stub
}

BufferMgr::~BufferMgr() {
	// TODO Auto-generated destructor stub
}

void BufferMgr::initLruCtrl()
{
    for(int i=0; i<LRUSIZE; i++)
    {
            ptof[i].head = NULL;
            ptof[i].tail = NULL;
            ptof[i].length = 0;
    }
    std::cout << "LruCtrl is initing...." << std::endl;
}

BCB* BufferMgr::isContain(_PageID pageID)
{
    int pos = pageID & mask;// %64
    int len = ptof[pos].length;
    if(len == 0)
            return NULL;

    BCB* phead = ptof[pos].head;
    int i = 1;
    while((i<len+1) && (phead->page_id != pageID))
    {
            i++;
            phead = phead->next;
    }
    if(i > len)
            return NULL;

    return phead;//frameID = phead->frame_id;
}

bool BufferMgr::isContainAndUpdate(_PageID pageID)
{
    BCB* bcb = isContain(pageID);
    if(NULL == bcb)
            return false;
    //update
    ++ bcb->count;
     ++ hit;
    return true;
}

bool BufferMgr::isFull(_PageID pageID)
{
    int pos = pageID & mask;
    int len = ptof[pos].length;
    return len==LISTSIZE ? true : false ;

}

BCB* BufferMgr::newBCB(_PageID pageID,int length)
{
    _FrameID frameID = pageID*LRUSIZE + length;//in the same time copy storage to buffer
    BCB* bcb = new BCB();
    bcb->count = 1;
    bcb->frame_id = frameID;
    bcb->page_id = pageID;
    bcb->pre =  bcb;
    bcb->next = bcb;
    return bcb;
}

BCB* BufferMgr::newBCB(_PageID pageID, BCB*old)
{
    old->count = 1;//in the same time copy storage to buffer overrite the old int buffer
    old->page_id = pageID;
    old->pre =  old;
    old->next = old;
    return old;
}

void BufferMgr::notFullAndInsert(_PageID pageID)
{
	 int pos = pageID & mask;// %64
	        BCB * phead = ptof[pos].head;
	        //new a bcb
	        BCB* bcb = newBCB(pageID,ptof[pos].length);

	        //update
	        ++ptof[pos].length;
	        //first node
	        if(ptof[pos].length == 1)
	        {
	                ptof[pos].head = bcb;
	        }
	        //update head point
	        else
	        {
	        phead->pre->next = bcb;
	        bcb->pre = phead->pre;

	        phead->pre = bcb;
	        bcb->next = phead;
	        ptof[pos].head = bcb;
	        }

	        //update tail point
	        if(HOTLEN+COLDLEN != ptof[pos].length)
	                return ;
	        //init tail point
	        int i = 1;
	        phead = ptof[pos].head;
	        while(i < HOTLEN+1)
	        {
	                i++;
	                phead = phead->next;
	        }
	        ptof[pos].tail = phead;

	        ++count;
}

BCB* BufferMgr::drop(_PageID pageID)
{
    int pos = pageID & mask;// %64
    //int count = ptof[pos].length;

    BCB* listTail = (ptof[pos].head)->pre;
    while(listTail != (ptof[pos].tail)->pre )
    {
            if(listTail->count < 2)//find victim
                    //victim = listTail;
            {
                    (ptof[pos].head)->pre = listTail->pre;
                    listTail->pre->next = ptof[pos].head;
                    //free(listTail);
                    //return listTail;
            }
            else
            {
                    listTail->count = 0;
                    ptof[pos].head = listTail;
                    ptof[pos].tail = (ptof[pos].tail)->pre;
            }
            listTail = listTail->pre;
    }

    return listTail;
}

void BufferMgr::fullAndInsert(_PageID pageID)
{
    BCB * _new = newBCB(pageID,drop(pageID));
    //insert
    int pos = pageID & mask;
    BCB* tail = ptof[pos].tail;
    tail->pre->next = _new;
    _new->pre = tail->pre;

    _new->next = tail;
    tail->pre = _new;
    //update
    ptof[pos].tail = _new;
    ++count;
}

long BufferMgr::printCount()
{
	std::cout << "count: " << count << std::endl;
	return count;
}

long BufferMgr::printHit()
{
	std::cout << "hit: " << hit << std::endl;
	return hit;
}
