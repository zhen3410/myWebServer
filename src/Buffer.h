#ifndef SERVER_BUFFER_H
#define SERVER_BUFFER_H

#include<vector>
#include<assert.h>
#include<string>

#include<iostream>

namespace server{

class Buffer{

public:
	static const size_t kCheapPrepend=8;
	static const size_t kInitialSize=1024;

	Buffer()
		:buffer_(kCheapPrepend+kInitialSize),
		readerIndex_(kCheapPrepend),
		writerIndex_(kCheapPrepend)
	{
		assert(readableBytes()==0);
		assert(writableBytes()==kInitialSize);
		assert(prependableBytes()==kCheapPrepend);
	}


	size_t readableBytes()const{
		return writerIndex_-readerIndex_;
	}

	size_t writableBytes()const{
		return buffer_.size()-writerIndex_;
	}

	size_t prependableBytes()const{
		return readerIndex_;
	}

	const char* peek()const{
		return begin()+readerIndex_;
	}

	std::string retrieveAsString(){
		std::string str(peek(),readableBytes());
		std::cout<<"Buffer::retrieveAsString() str = "<<str.c_str()<<" , size() = "<<str.size()<<" , readerIndex = "<<readerIndex_<<" , writerIndex = "<<writerIndex_<<std::endl;
		readerIndex_=kCheapPrepend;
		writerIndex_=kCheapPrepend;
		return str;
	}

	void append(const char* data,size_t len){
		ensureWritableBytes(len);
		std::copy(data,data+len,(begin()+writerIndex_));
		writerIndex_+=len;
	}

	void prepend(const void* data,size_t len){
		assert(len<=prependableBytes());
		readerIndex_-=len;
		const char* d=static_cast<const char*>(data);
		std::copy(d,d+len,buffer_.begin()+readerIndex_);
	}

	void ensureWritableBytes(size_t len){
		if(writableBytes()<len){
			makeSpace(len);
		}
		assert(writableBytes()>=len);
	}

	size_t readFd(int fd,int* savedErrno);

private:
	char* begin(){
		return &*buffer_.begin();
	}
	const char* begin()const{
		return &*buffer_.begin();
	}

	void makeSpace(size_t len){
		if(writableBytes()+prependableBytes()<len+kCheapPrepend){
			buffer_.resize(writerIndex_+len);
		}else{
			assert(kCheapPrepend<readerIndex_);
			size_t readable=readableBytes();
			std::copy(buffer_.begin()+readerIndex_,
					  buffer_.begin()+writerIndex_,
					  buffer_.begin()+kCheapPrepend);
			readerIndex_=kCheapPrepend;
			writerIndex_=readerIndex_+readable;
			assert(readable==readableBytes());
		}
	}


	std::vector<char> buffer_;
	size_t readerIndex_;
	size_t writerIndex_;
};

}


#endif
