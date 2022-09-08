#include "client.h"

int sockfd;
User user;


/*GETTING INPUT FROM USER BY COMMAND LINE ARGUMENTS*/

int main(int argc, char* argv[])
{
	struct addrinfo hints, *server_data, *result;
	struct sockaddr_storage client_addr;
	int val;
	client_addr.ss_family = AF_UNSPEC;

	if(argc != 4)
	{
		//Checks if args is valid 
		user.logger("Warning log:: Invalid number of arguments");
		cout<<"ERROR---> DOWNLOAD/UPLOAD server filename\n";
		exit(1);
	}
						
	char server[100];
	strcpy(server, argv[2]);
	char file[100];
	strcpy(file,argv[3]);	
					
	/*BEGINNING OF CONFIGURATION OF CLIENT*/

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;



	if((val = getaddrinfo(server, PORTNO, &hints, &server_data))!= 0)
	{
		user.logger("Client :: Fatal log: getAddress_Information\n");
		cout<<"Address info error"<<endl;
		return 1;
	}

	/*LOOP THROUGH THE RESULTS AND MAKING SOCKET*/

	for(result = server_data; result != NULL; result = result->ai_next)
	{
		if((sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == -1)
		{
			user.logger("CLIENT: SOCKET\n");
			perror("Client: socket");
			continue;
		}
		break;
	}

	if(result == NULL)
	{
		user.logger("Client:: Fatal log:: failed to bind");
		cout<<"Client: failed to bind socket\n";

		return 2;
	}
	
	/*CONFIGURATION OF CLIENT ENDS*/
	

	/*CHECKING VALID COMMAND LINE ARGUMENTS*/

	if(strcmp(argv[1],"DOWNLOAD")==0 || strcmp(argv[1],"download")==0)
	{	
		/*DOWNLOADING DATA FROM THE SERVER*/

		user.logger("Client:: info log:: Requesting file from server\n");
		user.Download_File(sockfd,client_addr,result,file,server);
	}
	else if(strcmp(argv[1],"UPLOAD")==0 || strcmp(argv[1],"upload")==0)
	{	
		/*UPLOADING DATA TO THE SERVER*/

		user.logger("Client::info log::Writing file to server\n");
		user.Upload_File(sockfd,client_addr,result,file,server);
	}
	else
	{	/*INVALID REQUEST*/

		user.logger("Client:: Warning log::Invalid Input Format\n");
	       	cout<<"USAGE: tftp_c GET/PUT server filename\n";
       		exit(1);
	}
	
	freeaddrinfo(server_data);
	close(sockfd);
	return EXIT_SUCCESS;
}

