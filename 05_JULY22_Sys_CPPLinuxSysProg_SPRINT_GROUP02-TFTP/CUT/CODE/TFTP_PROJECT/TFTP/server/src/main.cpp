#include "server.h"

int sockfd;  /*SOCKET DESCRIPTOR*/  

Server S;
static char *LOGFILE = "request.log";   /*STORING CLIENT REQUEST*/ 

/*USED TO STOP SERVER*/

void signalHandler(int sig)
{
	if(sig==SIGINT)
	{
		cout<<"\n---------END OF SERVER SIDE--------\nEXIT\n";
		S.Closelog();
		close(sockfd);
		exit(EXIT_SUCCESS);
	}
}

/*SETTING UP SERVER CONFIGURATION*/

int main(void)
{
	struct addrinfo hints, *server_data, *result;
	int val;
	int bytes;
	struct sockaddr_storage server_addr;
	char buffer[MAXLEN];
	socklen_t len;
	char destination[INET6_ADDRSTRLEN];

	signal(SIGINT,signalHandler);

	/*CONF OF SERVER STARTS*/

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; 
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; 

	if ((val = getaddrinfo(NULL, PORTNO, &hints, &server_data)) != 0) 
	{
		S.logger("Fatal log:: Server: getaddrinfo");
		cout<<"SERVER: ERROR IN GETTING THE ADDRESS"<<endl;
		return 1;
	}

	/*LOOP THROUGH THE RESULTS AND STARTING THE BINDING*/

	for(result = server_data; result != NULL; result = result->ai_next) 
	{
		if ((sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == -1) 
		{
			S.logger("Warning log:: ---Server socket---");
			perror("---SERVER:: socket---");
			continue;
		}
		if (bind(sockfd, result->ai_addr, result->ai_addrlen) == -1) 
		{
			S.logger("Warning log:: ---Server socket---");
			close(sockfd);
			perror("---SERVER bind Success---");
			continue;
		}
		break;
	}
	if (result == NULL) 
	{   
		S.logger("fatal log:: ERROR ---> Server failed to bind");
		cout<<"ERROR---> SERVER failed to bind socket\n";
		return 2;
	}
	freeaddrinfo(server_data);
	S.Openlog(LOGFILE);
	cout<<"---------Server connected with the pid: "<<getpid()<<"------------"<<endl;

	cout<<"--------CONNECTED WITH SERVER-------------\n";
	cout<<"--------Getting data from client----------\n";


	/*CONFIGURATION OF SERVER ENDS*/



	/*ITERATIVE SERVER*/

	while(1)
	{
		memset(&server_addr,0,sizeof(server_addr));
		len = sizeof(server_addr);
		if ((bytes = recvfrom(sockfd, buffer, MAXLEN-1 , 0, (struct sockaddr *)&server_addr, &len)) == -1) 
		{
			
			S.logger("fatal log:: Server: recvfrom");
			S.errorhandling(bytes,"SERVER: recvfrom");
		}

		

		S.logger("Info log:: Server got the packet");

		cout<<"SERVER: got packet from "<<inet_ntop(server_addr.ss_family, S.getAddr((struct sockaddr *)&server_addr), destination, sizeof destination)<<endl;
		cout<<"SERVER: packet is "<<bytes<<" bytes long"<<endl;

		buffer[bytes] = '\0';

		cout<<"SERVER: Data packet contains "<<buffer<<endl;

		S.logger("Debug log:: Server: Read request");

		/*DOWNLOAD REQUEST*/
		if(buffer[0] == '0' && buffer[1] == '1')
		{
			val=S.Download_File(sockfd, buffer, server_addr, len, result);
			S.logmsg("Server received READ REQUEST(RRQ) from %s \n",inet_ntop(server_addr.ss_family, S.getAddr((struct sockaddr *)&server_addr), destination, sizeof destination));
			if(val == EXIT_FAILURE)
			{
				S.logger("Warning log:: Server: Read request unsuccesful");
				cout<<"ERROR---> READ REQUEST UNSUCCESSFUL\n";
			}	
		}
		/*UPLOAD REQUEST*/

		else if (buffer[0] == '0' && buffer[1] == '2')
		{
			S.logger("Debug log:: ---Server= Write request---");
			val=S.Upload_File(sockfd, buffer, server_addr,len);
			S.logmsg("Server received WRITE REQUEST(WRQ) from %s \n",inet_ntop(server_addr.ss_family, S.getAddr((struct sockaddr *)&server_addr), destination, sizeof (destination)));
			if(val == EXIT_FAILURE)
			{
				S.logger("Warning log:: ---Server: Write request unsuccesful---");
				cout<<"WRITE REQUEST UNSUCCESSFUL\n";
			}
		} 
		else 
		{
			cout<<" ---REQUEST INVALID---\n";
			S.logmsg("Server received an invalid request from %s \n",inet_ntop(server_addr.ss_family, S.getAddr((struct sockaddr *)&server_addr), destination, sizeof destination));
			S.logger("fatal log:: Server: Invalid request");
		}
	}
	return EXIT_SUCCESS;
}

