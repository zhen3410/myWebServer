#include"../LogStream.h"

#include<limits>
#include<stdint.h>

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(testLogStreamBooleans){
    server::LogStream os;
    const server::LogStream::Buffer& buf=os.buffer();
    BOOST_CHECK_EQUAL(buf.toString(), string(""));
    os << true;
    BOOST_CHECK_EQUAL(buf.toString(), string("1"));
    os << '\n';
    BOOST_CHECK_EQUAL(buf.toString(), string("1\n"));
    os << false;
    BOOST_CHECK_EQUAL(buf.toString(), string("1\n0"));
}