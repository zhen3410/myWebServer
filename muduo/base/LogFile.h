#ifndef SERVER_BASE_LOGFILE_H
#define SERVER_BASE_LOGFILE_H

namespace server{

class LogFile{

public:
	LogFile(const LogFile&)=delete;
	void LogFile(const LogFile&)=delete;

	LogFile(const string& basename,
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
	static string getLogFileName(const string& basename,time_t* now);

	const string basename;
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