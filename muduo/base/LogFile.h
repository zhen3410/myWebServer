#ifndef SERVER_BASE_LOGFILE_H
#define SERVER_BASE_LOGFILE_H

#include"Mutex.h"

#include<string>
#include<memory>

namespace server{

class LogFile{

public:
	LogFile(const LogFile&)=delete;
	void LogFile(const LogFile&)=delete;

	LogFile(const std::string& basename,
			//off_t用于指示文件的偏移量，long类型
			off_t rollSize,
			bool threadSafe=true,
			int flushInterval=3,
			int checkEveryN=1024
			);

	void append(const char* logline,int len);
	void flush();
	void rollFile();

private:
	void append_unlocked(const char* logline,int len);
	static std::string getLogFileName(const std::string& basename,time_t* now);

	const std::string basename;
	const off_t rollSize_;
	const int flushInterval_;
	const int checkEveryN_;

	int count_;

	std::unique_ptr<Mutex> mutex_;
	time_t startOfPeriod_;
	time_t lastRoll_;
	time_t lastFlush_;
	std::unique_ptr<AppendFile> file_;

	const static int kRollPerSeconds_=60*60*24;
};

}


#endif