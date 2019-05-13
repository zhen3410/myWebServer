#ifndef SERVER_BASE_TIMESTAMP_H
#define SERVER_BASE_TIMESTAMP_H

#include<ctime>
#include<cstdint>

class Timestamp{
public:

Timestamp():microSecondsSinceEpoch_(0){}

explicit Timestamp(int64_t initTime):microSecondsSinceEpoch_(initTime){}

int64_t get(){return microSecondsSinceEpoch_;}

static Timestamp now();

static const int kMicroSecondsPreSecond=1000*1000;

private:
	int64_t microSecondsSinceEpoch_;
};

inline bool operator<(Timestamp lhs,Timestamp rhs){
	return lhs.get()<rhs.get();
}

inline bool operator==(Timestamp lhs,Timestamp rhs){
	return lhs.get()==rhs.get();
}

int Timestamp addTime(Timestamp time,double seconds){
	int64_t delta=static_cast<int64_t>(seconds*Timestamp::kMicroSecondsPreSecond);
	return Timestamp(time.get()+delta);
}

#endif