#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include "server.h"

int main()
{
	int server_port;
	Server s;
	char* ip_addr1 = "127.0.0.1";
	//s.getIp_addr((char*)"127.0.0.1");
	//s.getServer_port(server_port);
	s.display();


	CPPUNIT_NS::TestResult controller;
	CPPUNIT_NS::TestResultCollector result;
	controller.addListener( &result );        
	CPPUNIT_NS::BriefTestProgressListener progress;
	controller.addListener( &progress );      
	CPPUNIT_NS::TestRunner runner;
	runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );
	runner.run( controller );
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 
	return result.wasSuccessful() ? 0 : 1;
}
