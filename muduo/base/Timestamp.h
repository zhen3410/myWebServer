#ifndef SERVER_BASE_TIMESTAMP_H
#define SERVER_BASE_TIMESTAMP_H

#include<sys/time.h>
#include<cstdint>

namespace server{

class Timestamp{
public:

Timestamp():microSecondsSinceEpoch_(0){}

explicit Timestamp(int64_t initTime):microSecondsSinceEpoch_(initTime){}

string get()const{
  char buf[64] = {0};
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
  struct tm tm_time;
  gmtime_r(&seconds, &tm_time);

  if (showMicroseconds)
  {
    int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
             microseconds);
  }
  else
  {
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
  }
  return buf;
}

static Timestamp now();

static const int kMicroSecondsPerSecond=1000*1000;

private:
	int64_t microSecondsSinceEpoch_;
};

inline bool operator<(Timestamp lhs,Timestamp rhs){
	return lhs.get()<rhs.get();
}

inline bool operator==(Timestamp lhs,Timestamp rhs){
	return lhs.get()==rhs.get();
}

inline Timestamp addTime(Timestamp time,double seconds){
	int64_t delta=static_cast<int64_t>(seconds*Timestamp::kMicroSecondsPerSecond);
	return Timestamp(time.get()+delta);
}

}

#endif
