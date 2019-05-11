#include"Timestamp.h"

Timestamp Timestamp::now(){
	struct timeval now;
	gettimeofday(&tv,NULL);
	return Timestamp(tv.tv_sec*kMicroSecondsPreSecond+tv.tv_usec);
}