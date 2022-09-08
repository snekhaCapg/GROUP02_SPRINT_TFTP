#pragma once

#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<time.h>

//MACROS

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

using namespace std;


class User
{
	int sockfd;

	public:
		
		User();  /*Constructor*/

		/*METHODS*/

		void Blkno_to_String(char *str, int n);
		char* ReadRequest(char *f_name);
		char* WriteRequest(char *f_name);
		char* DataPacket(int blk, char *data);
		char* Acknowledgment(char *blk);
		char* Error(char *err_code, char* error_msg);
		void *getAddr(struct sockaddr *sa);

		/*LOGGER METHODS*/
		int logger(char* msg);

		
		/*SOCKET METHODS*/

		int Tries(int sockfd,char *buf,struct sockaddr_storage *client_addr,socklen_t len,struct addrinfo *result,char *end_msg);
		int Timeout(int sockfd, char *buf,struct sockaddr_storage *client_addr,socklen_t len);
		int Download_File(int sockfd, struct sockaddr_storage client_addr,struct addrinfo *result,char *file,char *server);
		int Upload_File(int sockfd, struct sockaddr_storage client_addr,struct addrinfo *result,char *file,char *server);
		void errorhandling(int ret, const char *msg);

		~User(); /*DESTRUCTOR*/
	


	private:

		struct addrinfo hints, *server_data, *result;
		struct sockaddr_storage client_addr;
};	

