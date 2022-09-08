#include "client.h"


User :: User() /*DEFAULT CONSTRUCTOR*/
{
}

/*Blkno_to_String:: CONVERTS BLOCK NUMBER TO STRING OF LENGTH 2*/

void User :: Blkno_to_String(char *temp, int n) 
{
	if(n==0)
	{
		temp[0] = '0', temp[1] = '0', temp[2] = '\0';
	}
	else if(n%10 > 0 && n/10 == 0)
	{

		char ch = n+'0';
		temp[0] = '0', temp[1] = ch, temp[2] = '\0';
	} 
	else if(n%100 > 0 && n/100 == 0)
	{
		char second = (n%10)+'0';
		char first = (n/10)+'0';
		temp[0] = first, temp[1] = second, temp[2] = '\0';
	}
	else 
	{
		temp[0] = '9', temp[1] = '9', temp[2] = '\0';
	}
}

/*ReadRequest:: MAKES PACKET FOR READ REQUEST*/

char* User :: ReadRequest(char *f_name)
{
	char *datapacket;
	datapacket = (char*)malloc(2+strlen(f_name)+1);
	size_t len = sizeof(datapacket);
	memset(datapacket,0,len);
	strcat(datapacket,READ_OPCODE);
	strcat(datapacket,f_name);
	return datapacket;
}

/*WriteRequest:: MAKES PACKET FOR WRITE REQUEST*/
char* User :: WriteRequest(char *f_name)
{
	char *datapacket;
	datapacket = (char*)malloc(2+strlen(f_name)+1);
	size_t len = sizeof(datapacket);
	memset(datapacket,0,len);
	strcat(datapacket, WRITE_OPCODE);
	strcat(datapacket, f_name);
	return datapacket;

}

/*DataPacket:: MAKES DATAPACKET*/

char* User :: DataPacket(int blk, char *data)
{
	char *datapacket;
	char temp[3];
	Blkno_to_String(temp,blk);
	datapacket = (char*)malloc(4+strlen(data)+1);
	size_t len = sizeof(datapacket);
	memset(datapacket,0,len);
	strcat(datapacket,DATA_OPCODE);
	strcat(datapacket,temp);
	strcat(datapacket,data);
	return datapacket;
}

/*Acknowledgment:: MAKES ACKNOWLEDGMENT PACKET*/

char* User :: Acknowledgment(char *blk)
{
	char *datapacket = NULL;
	datapacket = (char *)malloc(2+strlen(blk)+1);
	strcpy(datapacket, ACK_OPCODE);
	strcat(datapacket, blk);
	return datapacket;
}

/*Error:: MAKES ERROR PACKET*/

char* User :: Error(char *err_code,char* error_msg)
{
	char *datapacket;
	datapacket = (char*)malloc(4+strlen(error_msg)+1);
	size_t len=sizeof(datapacket);
	memset(datapacket, 0, len);
	strcat(datapacket, ERROR_OPCODE); // opcode 
	strcat(datapacket, err_code);
	strcat(datapacket, error_msg);
	return datapacket;
}

/*getAddr:: IT TYPECAST AN UNSPECIFIC ADDRESS INTO IPV4 OR IPV6*/

void *User :: getAddr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) 
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
	

}

/*Timeout:: CHECKS FOR TIMEOUT*/

int User :: Timeout(int sockfd, char *buf,struct sockaddr_storage *client_addr,socklen_t len)
{
	fd_set fds;
	int ret;
	struct timeval tv;

	FD_ZERO(&fds);
	FD_SET(sockfd, &fds);

	tv.tv_sec = TIME_OUT;
	tv.tv_usec = 0;

	ret = select(sockfd+1, &fds, NULL, NULL, &tv);
	if (ret == 0)
	{
		cout<<"timeout\n";
		logger("Warning log:: Client: Timeout");
		return -2; // timeout 
	}
	else if (ret == -1)
	{
		cout<<"Select error\n";
		return -1; /* error */
	}
	return recvfrom(sockfd, buf, MAXLEN-1 , 0, (struct sockaddr *)client_addr, &len);
}


/*Tries:: CHECKS THE MAXIMUM TRIES*/

int User :: Tries(int sockfd,char *buf,struct sockaddr_storage *client_addr,socklen_t len,struct addrinfo *result,char *end_msg)
{
	logger("Debug log:: Maximum Tries Function");
	int t;
	int bytes;
	for(t=0;t<TRY_NUM;++t)
	{
		if(t == TRY_NUM)
		{
			logger("Warning log:: Client: Maximum number of tries reached");
			cout<<"Client: Maximum number of tries reached"<<endl;
			exit(1);
		}
		logger("Debug log:: Client: Timeout check");
		bytes = Timeout(sockfd,buf,client_addr,len);

		if(bytes == -1)
		{
			logger("Fatal log:: Client: select() ");
			errorhandling(bytes,"CLIENT: recvfrom");
		}
		else if(bytes == -2)
		{
			logger("Warning log:: Client: timeout");
			cout<<"CLIENT: try no. "<<t+1<<endl;
			int temp_bytes;
			if((temp_bytes = sendto(sockfd, end_msg, strlen(end_msg), 0, result->ai_addr, result->ai_addrlen)) == -1)
			{
				logger("Fatal log:: Client: sendto" );
				errorhandling(bytes,"CLIENT ACK: sendto");
			}
			cout<<"Client: sent "<<temp_bytes<<"bytes again"<<endl;
			logger("Info log:: Client sent bytes again");
			continue;
		}
		else
		{
			break;
		}
	}
	return bytes;
}


/*FUNCTION TO DOWNLOAD FILE FROM THE SERVER*/

int User :: Download_File(int sockfd, struct sockaddr_storage client_addr,struct addrinfo *result,char *file,char *server)
{
	char *msg = ReadRequest(file);
	int bytes;
	FILE *fp;
	char lst_msg[MAXLEN];
	char filename[100];
	char buf[MAXLEN];

	char *blk;
	char *trans_msg;

	
	char ack[10];
	char destination[INET6_ADDRSTRLEN];
	socklen_t len;
	len = sizeof(client_addr);
	strcpy(ack,"");

	if((bytes = sendto(sockfd, msg, strlen(msg),0,result->ai_addr,result->ai_addrlen)) == -1)
	{
		logger("Fatal log:: Client: sendto");
		errorhandling(bytes,"CLient: sendto");
	}
	cout<<"-------------Client connected with the pid: "<<getpid()<<"-------------"<<endl;
	cout<<"Send "<<bytes<<" bytes to "<<server<<endl;
	

	logger("Info log:: Client sent to server");

	int flag=1;
	logger("Debug log:: Receiving actual file");


	int client_msg;

	do
	{
		logger("Info log:: Receiving file data packet");
		if(bytes = recvfrom(sockfd,buf,MAXLEN-1,0,(struct sockaddr*)&client_addr, &len)==-1)
		{
			logger("Fatal log:: Client: recvfrom");
			errorhandling(bytes,"Client::recvfrom");
		}
		cout<<"Client: received packet from "<<inet_ntop(client_addr.ss_family, getAddr((struct sockaddr *)&client_addr),destination, INET6_ADDRSTRLEN)<<endl;
		logger("Info log:: Client: received packets from");
		cout<<"Client: packet is "<<bytes <<" bytes"<<endl;

		buf[bytes] = '\0';
		

		if(buf[0]=='0' && buf[1] == '5')
		{
			cout<<"Received error packets: "<<buf<<endl;
			logger("Warning log :: Received error packets");
			free(msg);
			exit(1);
		}

		if(flag == 1)
		{
			strcpy(filename, file);
			
			fp = fopen(filename, "wb");
			
			if(fp == NULL)
			{
				logger("Fatal log :: Error while openning the file");
				cout<<"Error while openning file: "<<filename<<endl;
				free(msg);
				exit(1);
			}
			flag=0;
		}

		if(strcmp(buf,lst_msg) == 0)
		{
			logger("Warning log:: Last acknowledgment has not reached");
			bytes = sendto(sockfd, ack,strlen(ack),0,(struct sockaddr *)&client_addr,len);


			if(bytes == -1)
			{
				logger("Fatal log:: Error while sending the system call\n");
				errorhandling(bytes,"sendto");
			}
			continue;
		}


		client_msg = strlen(buf+4);
		fwrite(buf+4,sizeof(char),client_msg,fp);
		strcpy(lst_msg,buf);

		logger("Debug log:: Sending ack data packet\n");
		blk = (char*)malloc(3*sizeof(char));
		strncpy(blk, buf+1,2);
		blk[2] = '\0';

		trans_msg = Acknowledgment(blk);

		if((bytes = sendto(sockfd, trans_msg, strlen(trans_msg),0,result->ai_addr,result->ai_addrlen))==-1)
		{
			logger("Fatal log:: Failed to send ack");
			errorhandling(bytes,"sendto:");
		}
		cout<<"CLIENT: sent "<<bytes<<" bytes"<<endl;
		
		logger("Info log:: Client sent data");
		
		strcpy(lst_msg, trans_msg);
		strcpy(buf,lst_msg);
		free(blk);
		free(trans_msg);
	} 
	while(client_msg == MAXLEN);
		cout<<" FILE SUCCESSFULLY MADE "<<filename<<endl;
		logger("Info log:: New file successfully made\n");
		free(msg);
		fclose(fp);
		return EXIT_SUCCESS;
}


/*FUNCTION TO UPLOAD FILE TO THE SERVER */

int User :: Upload_File(int sockfd, struct sockaddr_storage client_addr,struct addrinfo *result,char *file,char *server)
{
	char *msg = WriteRequest(file);
	char *lst_msg;
	char destination[INET6_ADDRSTRLEN];
	char buf[MAXLEN];
	char *trans_msg;
	int bytes;
	if((bytes = sendto(sockfd, msg, strlen(msg),0,result->ai_addr,result->ai_addrlen))==-1)
	{
		logger("fatal log::Client: sento");
		errorhandling(bytes,"sento");
	}
	cout<<"Sent "<<bytes<<" bytes to "<<server<<endl;
	logger("Info log:: Send data to the server\n");
	lst_msg = msg;

	socklen_t len;

	len = sizeof(client_addr);
	logger("maximum tries function call\n");

	bytes = Tries(sockfd,buf,&client_addr,len,result,lst_msg);
	cout<<"CLIENT: got packet from "<<inet_ntop(client_addr.ss_family, getAddr((struct sockaddr *)&client_addr), destination, INET6_ADDRSTRLEN)<<endl;
	cout<<"CLIENT: packet is "<<bytes<<" bytes long"<<endl;

	buf[bytes] = '\0';
	cout<<"Packet Contains "<<buf<<endl;

	if(buf[0] == '0' && buf[1] == '4')
	{
		FILE *fp = fopen(file, "rb");
		if(fp == NULL || access(file, F_OK) == -1)
		{
			cout<<"CLIENT: file " <<file<< "does not exist"<<endl;
			char *e_msg = Error("01", "ERROR_FILE_NOT_FOUND");
			cout<<e_msg<<endl;
			sendto(sockfd, e_msg, strlen(e_msg), 0, result->ai_addr, result->ai_addrlen);
			free(e_msg);
			free(msg);
			logger("Warning log:: Client: File does not exist\n");
			exit(1);
		}
		logger("debug log:: Calculating the size of file");
		int blk = 1;
		fseek(fp, 0, SEEK_END);
		int total = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		int remaining = total;

		if(remaining == 0)
			++remaining;
		else if(remaining%READ_LEN == 0)
			--remaining;

		logger("Info log:: Reading file data packet\n");

		while(remaining>0)
		{
			char temp[520];
			if(remaining > READ_LEN)
			{
				fread(temp, READ_LEN, sizeof(char), fp);
				temp[READ_LEN] = '\0';
				remaining -= (READ_LEN);
			}
			else
			{
				fread(temp,remaining,sizeof(char),fp);
				temp[remaining] = '\0';
				remaining =0;
			}

			logger("debug log:: Sending file data packets\n");
			trans_msg = DataPacket(blk,temp);

			if((bytes = sendto(sockfd, trans_msg,strlen(trans_msg),0,result->ai_addr,result->ai_addrlen)) == -1)
			{
				logger("fatal log:: CLient: sendto\n");
				errorhandling(bytes,"Client: sendto");
			}

			cout<<"CLIENT: sent "<<bytes<< " bytes to "<<server<<endl;
			logger("info log:: Client: sent bytes to server\n");
			lst_msg = trans_msg;

			bytes=Tries(sockfd,buf, &client_addr,len,result,lst_msg);
			cout<<"CLIENT: got packet from "<<inet_ntop(client_addr.ss_family, getAddr((struct sockaddr *)&client_addr), destination, INET6_ADDRSTRLEN)<<endl; 
			logger("Info log:: Client: got packet\n");
			cout<<"CLIENT: packet is "<<bytes<<" bytes long"<<endl;
			buf[bytes] = '\0';
			cout<<"CLIENT: packet contains "<<buf<<endl;

			if(buf[0]=='0' && buf[1]=='5')	
			{
				logger("Warning Error:: Client: error packet received\n");
				cout<<"error while recving packet: "<<buf;
				free(trans_msg);
				free(msg);
				fclose(fp);
				exit(1);
			}
			++blk;
			if(blk>100)
				blk = 1;
			free(trans_msg);
		}
		free(msg);
		fclose(fp);
	}

	else
	{
		logger("Warning log:: Client: ack expecting but got\n");
		cout<<"CLIENT ACK: expecting but got: "<<buf<<endl;
		exit(1);
	}
	return EXIT_SUCCESS;
}


/*TO STORE 4 TYPES OF LOG MESSAGES IN SPECIFIC FILE*/

int User :: logger(char* msg)
{
		

		FILE *logfile;
		char filename[100]="data.log";
		time_t ltime = time(NULL);
		struct tm res;
		char TIMESTAMP[32];
		localtime_r(&ltime,&res);
		asctime_r(&res,TIMESTAMP);
		
		
		logfile = fopen(filename,"a+");
		fprintf(logfile,"\n~~%s\t%s\n----------\n",TIMESTAMP,msg);
		fclose(logfile);
		
		return EXIT_SUCCESS;
}

/*HANDLES THE ERROR OCCURS CAUSED BY SYSTEMCALL OR LIBRARY FUNCTIONS*/

void User :: errorhandling(int ret, const char *msg)
{
	if(ret == -1)
	{
		perror("message");
		exit(EXIT_FAILURE);
	}
}

/*DESTRUCTOR*/
User :: ~User()
{
}
