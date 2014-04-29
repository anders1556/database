/*
 * main.cpp
 *
 *  Created on: 2014年4月27日
 *      Author: houbo
 */

#include "BufferMgr.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <pthread.h>
#include <unistd.h>

#include <time.h>

const int DEFAULTSIZE = 1024;
bool full = false;
pthread_mutex_t mutex;
struct timespec slptm;
bool gg = false;

const char* path = "/home/houbo/database/data-5w-50w-zipf.txt";

int pageArray[DEFAULTSIZE];

void* reader_function(void * )
{
	slptm.tv_sec = 0;
	slptm.tv_nsec = 500000000;
	BufferMgr* bufferMgr = new BufferMgr();
	bufferMgr->initLruCtrl();
	while(!gg)
	{
		pthread_mutex_lock(&mutex);
		if(full)
		{
			//consume_item(buffer);
			for(int i=0; i<DEFAULTSIZE; i++)
			{

				if(!(bufferMgr->isContainAndUpdate(pageArray[i])))
				{
					//std::cout << "***********" << pageArray[i] << std::endl;
					if(bufferMgr->isFull(pageArray[i]))
						bufferMgr->fullAndInsert(pageArray[i]);
					else
						bufferMgr->notFullAndInsert(pageArray[i]);
				}
				bufferMgr->printCount();
				std::cout << "######################################" << std::endl;
				bufferMgr->printHit();
			}
			full = false;
		}
		pthread_mutex_unlock(&mutex);
		if(nanosleep(&slptm,NULL) == -1)
			{
			perror("Failed to nanosleep");
			return NULL;//return 1;
			}
	}
	return NULL;
}

void write_function(void)
{
	std::ifstream rfile(path,std::ios::in);


	if(!rfile)
	{
		std::cout << "can not open the file!" << std::endl;
		exit(1);
	}
	while(!rfile.eof())
	{
		pthread_mutex_lock(&mutex);
		if(!full)
		{
			int i = 0;
			char str[10];
			do{
				rfile.getline(str,10);
				int rw = (str[0] - '0') ? 1 : 1;
				pageArray[i++] = rw * atoi(&str[2]);
			}while(i<DEFAULTSIZE );

			full = true;
		}
		pthread_mutex_unlock(&mutex);
		if(nanosleep(&slptm,NULL) == -1)
			{
			perror("Failed to nanosleep");
			return ;//return 1;
			}
	}
	rfile.close();
	pthread_mutex_lock(&mutex);
	gg = true;
	pthread_mutex_unlock(&mutex);
}

int main()
{
	pthread_t reader;
	pthread_mutex_init(&mutex,NULL);
	pthread_create(&reader,NULL,reader_function,NULL);
	write_function();
	return 0;

}

