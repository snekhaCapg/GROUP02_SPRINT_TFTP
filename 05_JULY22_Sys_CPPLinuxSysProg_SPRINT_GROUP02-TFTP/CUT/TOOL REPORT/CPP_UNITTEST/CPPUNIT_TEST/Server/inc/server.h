#include <iostream>
#include <string>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cppunit/extensions/HelperMacros.h>

using namespace std;

class Server : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(Server );
	CPPUNIT_TEST( getServer_port_UT );
	CPPUNIT_TEST( getIp_addr_UT );
	CPPUNIT_TEST(testServerClass);
	CPPUNIT_TEST_SUITE_END();
	protected:
	int server_port;
	char* ip_addr;
	public:
	Server() { 
		server_port = 69;
		ip_addr = "127.0.0.1";
	}
	void setUp();
	void setIp_addr(char* ip) { ip_addr = ip; }
	int getServer_port() { return server_port; }
	char* getIp_addr() { return ip_addr; }
	int validate_number(char *str){
		while (*str) {
			if(!isdigit(*str)){ 
				false;
				return 0;
			}
			str++;
		}
		return 1;
	}
	bool validIp_addr(char* ip)
	{                           
		int i, num, dots = 0;
		char *ptr;
		if (ip == NULL)
			return 0;
		ptr = strtok(ip, "."); 
		if (ptr == NULL)
			return 0;
		while (ptr) 
		{
			if (!validate_number(ptr))
				return 0;
			num = atoi(ptr); 
			if (num >= 0 && num <= 255) 
			{
				ptr = strtok(NULL, "."); 
				if (ptr != NULL)
					dots++;
			} else
				return 0;
		}
		if (dots != 3) 
			return 0;
		return 1;
	}
	void display()
	{
		cout<<"server port number: "<<server_port<<endl;
		cout<<"IP address: "<<ip_addr<<endl;
	}
	protected:
	void getServer_port_UT();
	void getIp_addr_UT ();
	void validIp_addr_UT();
	void setIp_addr_UT();
	void testServerClass();
};
