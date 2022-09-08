#include <cppunit/config/SourcePrefix.h>
#include <string>
#include<algorithm>
#include "test.h"
CPPUNIT_TEST_SUITE_REGISTRATION( Client );
void Client::setUp()
{
	ipadd = "127.0.0.1";
	filename = "hello.txt";
}
void Client::getFileName_UT() {
	std::string Fname = "hello.txt";
	std::string Fname2 = getFileName();
	CPPUNIT_ASSERT_EQUAL(Fname, Fname2);
}
void Client::getIpAdd_UT() {
	std::string IP = "127.0.0.0";
	std::string IP2 = getIpAdd();
	CPPUNIT_ASSERT_EQUAL(IP, IP2);
}
