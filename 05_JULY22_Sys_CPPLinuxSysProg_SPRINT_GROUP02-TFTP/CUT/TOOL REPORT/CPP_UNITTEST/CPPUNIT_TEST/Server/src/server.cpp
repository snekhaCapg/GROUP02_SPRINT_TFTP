#include <cppunit/config/SourcePrefix.h>
#include <string>
#include<algorithm>

#include "server.h"

CPPUNIT_TEST_SUITE_REGISTRATION(Server);

void Server::setUp()
{
	server_port = 69;
	ip_addr = "127.0.0.1" ;
}
void Server::getIp_addr_UT(){
	char* ip_addr1 = "127.0.0.1";
	char* ip_addr2 = getIp_addr();
	CPPUNIT_ASSERT_EQUAL(ip_addr1, ip_addr2);
}
void Server::getServer_port_UT() {
	CPPUNIT_ASSERT_EQUAL(69, getServer_port());
}
void Server::validIp_addr_UT(){
	char* ip_addr1 = "127.0.0.1";
	CPPUNIT_ASSERT_EQUAL(true, validIp_addr(ip_addr1));
}
void Server::testServerClass()
{
	Server s;
	char* ip_addr1 = "127.0.0.1";
	s.setUp();
	CPPUNIT_ASSERT_EQUAL(ip_addr1, s.getIp_addr());
}
