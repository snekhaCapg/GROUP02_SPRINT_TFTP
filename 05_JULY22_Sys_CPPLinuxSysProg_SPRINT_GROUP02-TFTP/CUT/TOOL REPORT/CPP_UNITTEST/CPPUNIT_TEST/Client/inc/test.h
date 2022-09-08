#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <vector>
#include <algorithm>
#include <cppunit/extensions/HelperMacros.h>
using namespace std;
class Client : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(Client );
	CPPUNIT_TEST( getIpAdd_UT );
	CPPUNIT_TEST( getFileName_UT);
	CPPUNIT_TEST_SUITE_END();
	protected:
	string ipadd;
	string filename;
	public:
	void setUp();
	void setValues(string i,string f) {ipadd = i; filename = f;}
	string getIpAdd(){return ipadd;}
	string getFileName(){return filename;}
	protected:
	void getIpAdd_UT();
	void getFileName_UT();
};
