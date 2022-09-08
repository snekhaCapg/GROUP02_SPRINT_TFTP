#pragma once 

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>


//DEFINING MACROS


#define PORTNO "8080" 		// Port to be opened on server //
#define MAXLEN 560		// GET sockaddr IPV4 or IPV6 //
#define READ_LEN 512		// MAX bytes //
#define TRY_NUM 5		// MAX NUMBER OF TRIES//
#define TIME_OUT 6		// MAX TIMEOUT IN sec//
#define READ_OPCODE "01"	// OPCODE FOR READREQUEST//
#define WRITE_OPCODE "02"	//OPCODE FOR WRITEREQUEST //
#define DATA_OPCODE "03"	//OPCODE FOR DATA PACKET //
#define ACK_OPCODE "04"		//OPCODE FOR ACKNOWLEDGEMENT PACKET //
#define ERROR_OPCODE "05"	//OPCODE FOR ERROR PACKET //
#define DATESIZE sizeof("DD-MM-YYYY HH:MM:SS")  //DEFINING DATESIZE FORMAT FOR LOGMSG//
#define WRQ_ACK "00"
using namespace std;


class Server{
	int sockfd;

	public:
		Server(); /*Constructor*/

		/*METHODS*/

		void Blkno_to_String(char *temp, int n);
		char* DataPacket(int blk, char *data);
		char* Acknowledgment(char *blk);
		char* Error(char *err_code,char* error_msg);

		void logmsg(const char *msg_format,...);
		void Openlog(const char *logfile);
		void Closelog(void);

		/*LOGGER METHODS*/
		int logger(char *msg);

		/*SOCKET METHODS*/

		void *getAddr(struct sockaddr *sa);
		int Tries(int sockfd,char *buf,struct sockaddr_storage server_addr,socklen_t len,struct addrinfo *result,char *t_msg);
		int Timeout(int sockfd, char *buf,struct sockaddr_storage server_addr,socklen_t len);
		int Download_File(int sockfd,char *buf, struct sockaddr_storage server_addr,socklen_t len,struct addrinfo *result);
		int Upload_File(int sockfd,char *buf, struct sockaddr_storage server_addr,socklen_t len);
		void errorhandling(int ret, const char *message);

		~Server();/*DESTRUCTOR*/

	private: 

		struct addrinfo hints, *server_data, *result;
		int bytes;
		struct sockaddr_storage server_addr;
		char buffer[MAXLEN];
		socklen_t len;
		char destination[INET6_ADDRSTRLEN];
};
