#include"Timestamp.h"

#include<iostream>

using namespace server;

Timestamp Timestamp::now(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return Timestamp(tv.tv_sec*kMicroSecondsPreSecond+tv.tv_usec);
}
