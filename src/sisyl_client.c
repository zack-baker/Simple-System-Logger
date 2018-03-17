#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void print_usage(int mode, char* name){
	if(mode==0){
		fprintf(stderr, "USAGE: %s SET -l LEVEL -t TITLE [-d description]\n", name);
		return;
	}
	if(mode==2){
		fprintf(stderr, "USAGE: %s SET|GET [options]\n",name );
	}
}

/**
*	This function sends the char* query to the SiSyL daemon to process, and returns it's response
*	INPUT:
		- char* query - the char* which holds the query for the daemon
	OUTPUT:
		- the response from the daemon
*/
char* send_request(char* query){
	int socket_file_descriptor;//variable to hold the file descriptor of the communication socket 
	int port_num = 8585;//8585 is the default port
	struct sockaddr_in serv_addr; 
	struct hostent *server;
	
	int query_len = strlen(query);

	socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_file_descriptor<0){//if the socket cannot be created, exit
		fprintf(stderr,"[ERROR] Can't create socket");
		exit(1);		
	}
	server = gethostbyname("localhost");//get the hostent representation of localhost, since that is where the daemon is always located
	if(server==NULL){//if we can't find the host, exit
		fprintf(stderr,"[ERROR] Cannot find host at localhost\n");
		exit(1);
	}
	bzero((char*) &serv_addr, sizeof(serv_addr));//zero out the serv_addr struct
	serv_addr.sin_family = AF_INET;
	bcopy((char*) server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port_num);
	if(connect(socket_file_descriptor, (struct sockaddr*) &serv_addr, sizeof(serv_addr))<0){//if we can't connect to the socket, exit
		fprintf(stderr, "[ERROR] Cannot connect socket\n");
		exit(1);
	}
	ssize_t io_status = write(socket_file_descriptor, query, query_len);//write the query to the socket
	if(io_status<0){//if the write fails, exit
		fprintf(stderr,"[ERROR] Failed to write to socket\n");
		exit(1);
	}
	//read the response from the daemon (once it sends one)
	//initialize a new array, as query will likely not be large enough for the response
	close(socket_file_descriptor);
	return NULL;
}
/**
*	This function takes the consituent arguments from the user and constructs them into one char* to send to the daemon as a request. 
*	INPUT:
		- char* level - a char* holding the string representing the level of log entry
		- char* title - a char* holding the title of the log entry
		- char* description - a char* holding the longform description of the log entry
		- char* user - a char* holding the username of the user invoking the client
		- char* time - a char* holding a string representing the current UNIX timestamp
	OUTPUT:
		none
*/
void query_daemon_insert(char* level, char* title, char* description, char* user, char* time){
	size_t query_len = 8 + strlen(level) + strlen(title) + strlen(description) + strlen(user) + strlen(time); //this variable holds the length of the char* to create for the full query. The total size is the size of each individual char*, plus 3 (for the constant string "SET") plus 5 spaces. 
	char query[query_len];//create the full query array
	sprintf(query,"SET %s %s %s %s %s", level, title, description, user, time);//stick the individual arrays together
	printf("Full query is %s\n", query);

	send_request(query);//send the complete query to the daemon
}


int main(int argc, char** argv)
{
	int iter = 0;
	if(argc<2){
		print_usage(2,argv[0]);
		exit(1);
	}
	if(strcmp(argv[1],"SET")==0){
		char* level = NULL;
		char* title = NULL;
		char* desc = NULL;
		printf("SETting db\n");
		while((iter=getopt(argc, argv,"l:t:d::"))!=-1){
			switch(iter){
				case 'l': level = optarg;
				case 't': title = optarg;
				case 'd': desc = optarg;
			}
		}
		printf("Level is: %s\n", level);
		printf("Title is: %s\n", title);
		printf("Description is %s\n", desc);
		if(atoi(level)==0){
			fprintf(stderr, "Please provide level\n");
			print_usage(0, argv[0]);
		}
		if(title==NULL){
			fprintf(stderr, "Please provide title\n");
			print_usage(0, argv[0]);
		}

		char* user = (getpwuid(getuid()))->pw_name;
		printf("Username is %s\n", user);
		time_t secs = time(NULL);
		printf("Current timestamp is %ld\n", secs);
		char secs_str[64];
		sprintf(secs_str,"%ld", secs);
		printf("Time as str is: %s\n",secs_str);

		query_daemon_insert(level, title, desc, user, secs_str);

	}else if(strcmp("GET",argv[1])==0){
		printf("GETting db");
	}else{
		print_usage(2,argv[0]);
	}
	
	return 0;
}