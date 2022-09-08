#include <server.h>
#define DATESIZE sizeof("DD-MM-YYYY HH:MM:SS")

static FILE *logFp;

/*DEFAULT CONSTRUCTOR*/
Server::Server()
{
}

/*CONVERTS BLOCK NUMBER TO STRING OF LENGTH 2*/

void Server :: Blkno_to_String(char *temp, int n)
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
		char first = (n%10)+'0';
		char second = (n/10)+'0';
		temp[0] = first, temp[1] = second, temp[2] = '\0';
	}
	else 
	{
		temp[0] = '9', temp[1] = '9', temp[2] = '\0';
	}
}

/*MAKING DATAPACKETS*/

char* Server :: DataPacket(int blk, char *data){
	char *datapacket=NULL;
	char temp[3];
	Blkno_to_String(temp, blk);
	datapacket = (char*)malloc(4+strlen(data)+1);
	strcpy(datapacket, DATA_OPCODE);
	strcat(datapacket, temp);
	strcat(datapacket, data);
	return datapacket;
}


/*MAKING ACKNOWLEDGMENT*/

char* Server :: Acknowledgment(char* blk){
	char *datapacket=NULL;
	datapacket = (char*)malloc(2+strlen(blk)+1);
	strcpy(datapacket, ACK_OPCODE);
	strcat(datapacket, blk); 
	return datapacket;
}

/*MAKING ERROR PACKETS*/

char* Server :: Error(char *err_code, char* error_msg){
	char *datapacket=NULL;
	datapacket = (char*)malloc(4+strlen(error_msg)+1);
	size_t len=sizeof(datapacket);
	memset(datapacket, 0, len);
	strcat(datapacket, ERROR_OPCODE);
	strcat(datapacket, err_code);
	strcat(datapacket, error_msg);
	return datapacket;
} 

/*MAINTAINING THE LOG MESSAGE*/

void Server::logmsg(const char *format,...)
{
	va_list argList;
	const char *TIMEFORMAT = "%d-%m-%Y %X";	

	char timeStamp[DATESIZE];
	time_t t;
	struct tm *tmVar;
	t = time(NULL);
	tmVar = localtime(&t);
	if(tmVar == NULL || strftime(timeStamp,DATESIZE,TIMEFORMAT,tmVar) == 0)
	{
		fprintf(logFp, "***Unknown time***: ");
	}
	else
	{
		fprintf(logFp, "%s: ",timeStamp);
	}
	va_start(argList, format);
	vfprintf(logFp, format, argList);
	fprintf(logFp,"\n");	
	va_end(argList);
}

/*OPENNING A LOG FILE*/

void Server :: Openlog(const char *logfile)
{
	mode_t oldPerms;
	oldPerms = umask(777);
	logFp = fopen(logfile, "a");
	umask(oldPerms);
	if(logFp == NULL)
	{
		perror("logfile open");
		exit(EXIT_FAILURE);
	}
	setbuf(logFp, NULL);
	logmsg("opened log file");

}

/*CLOSING THE LOG FILE*/

void Server :: Closelog(void)
{
	logmsg("closing log file");
	fclose(logFp);
}

/*FUNCTION TO TYPECAST UNSPECIFIC ADDRESS TO IPV4 AND IPV6 */


void* Server :: getAddr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) 
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


/*CHECKING FOR THE TIMEOUT CONDITION*/

int Server :: Timeout(int sockfd, char *buf, struct sockaddr_storage server_addr, socklen_t len){
	fd_set fds;
	int val;
	struct timeval tv;
	FD_ZERO(&fds);
	FD_SET(sockfd, &fds);
	tv.tv_sec = TIME_OUT;
	tv.tv_usec = 0;
	val = select(sockfd+1, &fds, NULL, NULL, &tv);
	if (val == 0)
	{
		cout<<"timeout\n";
		logger("Warning log:: Server: Timeout");
		return -2;
	} 
	else if (val == -1)
	{
		cout<<"error\n";
		return -1;  
	}
	return recvfrom(sockfd, buf, MAXLEN-1 , 0, (struct sockaddr *)&server_addr, &len);
}

/*CHECKING FOR MAXIMUM TRIES*/

int Server :: Tries(int sockfd, char *buf, struct sockaddr_storage server_addr, socklen_t len, struct addrinfo *result, char *t_msg){
	int t;
	int bytes;
	for(t=0;t<=TRY_NUM;++t)
	{
		if(t==TRY_NUM)
		{
			cout<<"SERVER: MAX NUMBER OF TRIES REACHED\n";
			logger("Fatal log:: Server: Max number of tries reached");
			return -1;
		}
		bytes = Timeout(sockfd, buf, server_addr,len);
		if(bytes == -1)
		{	
			logger("Fatal log:: Server: recvfrom");
			errorhandling(bytes,"SERVER: recvfrom");
		} 
		else if(bytes == -2)
		{	
			cout<<"SERVER: try no. "<<t+1<<endl;
			int temp_bytes;
			if((temp_bytes = sendto(sockfd, t_msg, strlen(t_msg), 0, result->ai_addr, result->ai_addrlen)) == -1)
			{
				logger("Fatal log:: Server: sendto");
				errorhandling(temp_bytes,"SERVER: ACK: sendto");
			}
			cout<<"SERVER: sent "<<temp_bytes<<" bytes AGAIN"<<endl;
			logger("Info log:: Server: sent bytes again");
			continue;
		} 
		else 
		{ 
			break;
		}
	}
	return bytes;
}


/*FUNCTION FOR THE DOWNLAOD REQUEST*/

int Server :: Download_File(int sockfd, char *buf, struct sockaddr_storage server_addr, socklen_t len, struct addrinfo *result)
{
	char filename[100];
	int bytes;
	char *t_msg;
	char destination[INET6_ADDRSTRLEN];
	strcpy(filename, buf+2);
	FILE *fp = fopen(filename, "rb"); 
	if(fp == NULL || access(filename, F_OK) == -1)
	{ 	
		cout<<"SERVER: file "<<filename<<" does not exist and sending error packet\n";
		char *e_msg = Error("01", "ERROR_FILE_NOT_FOUND");
		logger("Warning log:: Server: Error file not found");
		cout<<e_msg<<endl;
		logger("Debug log:: Server: Sendto()");
		sendto(sockfd, e_msg, strlen(e_msg), 0, (struct sockaddr *)&server_addr,len);
		free(e_msg);
		return EXIT_FAILURE;
	}
	logger("Debug log:: Starting to send file");
	int block = 1;
	fseek(fp, 0, SEEK_END);    
	int total = ftell(fp);   
	fseek(fp, 0, SEEK_SET);
	int remaining = total;
	if(remaining == 0)
		++remaining;
	else if(remaining%READ_LEN == 0)
		--remaining;
	while(remaining>0)
	{
		char temp[READ_LEN+5];
		if(remaining>READ_LEN)
		{
			fread(temp,READ_LEN, sizeof(char), fp);
			temp[READ_LEN] = '\0';
			remaining -= (READ_LEN);
		} 
		else 
		{
			fread(temp, remaining, sizeof(char), fp);
			temp[remaining] = '\0';
			remaining = 0;
		}
		t_msg = DataPacket(block, temp);

		if((bytes = sendto(sockfd, t_msg, strlen(t_msg), 0, (struct sockaddr *)&server_addr, len)) == -1)
		{
			logger("Fatal log:: Server: ACK sendto");
			errorhandling(bytes,"SERVER ACK: sendto");
		}
		cout<<"SERVER: sent "<<bytes<<" bytes\n";
		logger("Debug log:: Server: Maxtries");
		bytes=Tries(sockfd, buf, server_addr, len, result, t_msg);
		if(bytes==-1)
		{
			logger("Warning log:: Server: failure");
			free(t_msg);
			fclose(fp);
			return EXIT_FAILURE;
		}
		logger("Info log:: Server: Got packet from client");
		cout<<"SERVER: got packet from "<<inet_ntop(server_addr.ss_family, getAddr((struct sockaddr *)&server_addr), destination, sizeof destination)<<endl;
		cout<<"SERVER: packet is "<<bytes<<" bytes long\n";
		buf[bytes] = '\0';
		cout<<"SERVER: packet contains "<<buf<<endl;				
		++block;
		if(block>100)
			block = 1;
		free(t_msg);
	}
	fclose(fp);
	return EXIT_SUCCESS;
}


/*FUNCTION FOR UPLOAD REQUEST*/


int Server :: Upload_File(int sockfd, char *buf, struct sockaddr_storage server_addr, socklen_t len)
{
	char *message = Acknowledgment(WRQ_ACK);
	int bytes;

	int flag=1;

	char *t_msg=NULL;

	FILE *fp;
	char destination[INET6_ADDRSTRLEN];
	char filename[100];
	char lst_msg[MAXLEN];
	strcpy(lst_msg, buf);
	char lst_ack[10];

	strcpy(lst_ack, message);
	if((bytes = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&server_addr, len)) == -1)
	{
		logger("Server: ACK sendto");
		errorhandling(bytes,"SERVER ACK: sendto");
	}
	strcpy(filename, buf+2);

	int server_msg;
	do
	{
		logger("Debug log:: Server: Receiving packet data");
		if ((bytes = recvfrom(sockfd, buf, MAXLEN-1 , 0, (struct sockaddr *)&server_addr, &len)) == -1) 
		{
			logger("Fatal log:: Server: recvfrom");
			errorhandling(bytes,"SERVER: recvfrom");
		}
		logger("Info log:: Server got packet from");
		cout<<"SERVER: got packet from  "<<inet_ntop(server_addr.ss_family, getAddr((struct sockaddr *)&server_addr), destination, sizeof destination)<<endl;
		cout<<"SERVER: packet is "<<bytes<<" bytes long\n";
		buf[bytes] = '\0';
		cout<<"SERVER: packet contains "<<buf<<endl;
		if(flag==1)
		{
			if(buf[0]=='0' && buf[1]=='5')
			{
				cout<<"SERVER: got error packet: "<<buf<<endl;
				logger("Warning log:: Server: got error packet");
				free(message);
				return EXIT_FAILURE;
			}
			if(access(filename, F_OK) != -1)
			{ 
				logger("Debug log:: Server: Sending error packet");
				cout<<"SERVER: file "<<filename<<" already exists, sending error packet\n";
				char *e_msg = Error("06", "ERROR_FILE_ALREADY_EXISTS");
				if((bytes=sendto(sockfd, e_msg, strlen(e_msg), 0, (struct sockaddr *)&server_addr, len))==-1){
					logger("Fatal log:: Server: sendto system call failed");
					errorhandling(bytes,"SERVER ERROR MESSAGE: sendto");
				}
				free(e_msg);
				free(message);
				return EXIT_FAILURE;
			}
			fp = fopen(filename, "wb");
			if(fp == NULL || access(filename, W_OK) == -1)
			{ 
				logger("Warning log :: Server: Access denied");
				cout<<"SERVER: file "<<filename<<" access denied, sending error packet\n";
				char *e_msg = Error("02", "ERROR_ACCESS_DENIED");
				logger("Debug log:: Server: sendto");
				if((bytes=sendto(sockfd, e_msg, strlen(e_msg), 0, (struct sockaddr *)&server_addr, len))==-1)
				{
					logger("Fatal log:: Server: sendto system call failed");
					errorhandling(bytes,"SERVER ERROR MESSAGE: sendto");
				}
				free(e_msg);
				free(message);
				fclose(fp);
				return EXIT_FAILURE;
			}
			flag=0;
		}
		if(strcmp(buf, lst_msg) == 0)
		{
			logger("Info log:: Server: last ack sending");
			if((bytes=sendto(sockfd, lst_ack, strlen(lst_ack), 0, (struct sockaddr *)&server_addr, len))==-1){
				logger("Fatal log:: Server: sendto system call failed");
				errorhandling(bytes,"SERVER LAST SENT ACK: sendto");
			}
			continue;
		}
		server_msg = strlen(buf+4);
		fwrite(buf+4, sizeof(char), server_msg, fp);
		strcpy(lst_msg, buf);
		char block[3];
		strncpy(block, buf+2, 2);
		block[2] = '\0';
		t_msg = Acknowledgment(block);
		if((bytes = sendto(sockfd, t_msg, strlen(t_msg), 0, (struct sockaddr *)&server_addr, len)) == -1)
		{
			logger("Fatal log:: Server: ack sendto");
			errorhandling(bytes,"SERVER ACK: sendto");
		}
		logger("Info log:: Server sent to client");
		cout<<"SERVER: sent "<<bytes<<"bytes\n";
		strcpy(lst_ack, t_msg);
		free(t_msg);
	}
	while(server_msg == READ_LEN);
	logger("Info log:: Server: New file made");
	cout<<"FILE "<<filename<<" SUCCESSFULLY MADE\n";
	free(message);
	fclose(fp);
	return EXIT_SUCCESS;
}


/*TO STORE 4 TYPES LOG IN SPECIFIC FILE*/

int Server :: logger(char* msg)
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


/*HANDLES ERROR CAUSED BY SYSTEM CALL/LIBRARY FUNCTION */

void Server :: errorhandling(int ret, const char *message)
{
	if(ret == -1)
	{
		perror(message);
		exit(EXIT_FAILURE);
	}
}


/*DESTRUCTOR*/
Server::~Server()
{
}
