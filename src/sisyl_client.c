#include "sisyl_client.h"

/**
*	This function prints usage information if incorrect arguments are provided, to help inform the user as to it's expected input
*	INPUT:
		- mode - the format of usage information:
			* mode==0 - user mishandled SET operation
			* mode==1 - user mishandled GET operation
			* mode==2 - user did not provide either SET or GET
		- name - the name of the binary, i.e. argv[0]
	OUTPUT:
		- none
*
*/
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
		- query - the char* which holds the query for the daemon
	OUTPUT:
		- the response from the daemon
*
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
		- level - a char* holding the string representing the level of log entry
		- title - a char* holding the title of the log entry
		- description - a char* holding the longform description of the log entry
		- user - a char* holding the username of the user invoking the client
		- time - a char* holding a string representing the current UNIX timestamp
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

/**
*	This is the main function of the client. 
*	INPUT:
		- argc - the number of command line arguments
		- argv - the arguments to the client.
			* argv[1] must be either "SET" or "GET", to specify the type of request to the daemon
			* the other arguments are some combination of -l $level, -t $title, and [-d $description]. -l and -t are required. -d is optional. 
	OUTPUT
		- 0 if operation successful
		- 1 otherwise
*/
int main(int argc, char** argv)
{
	int iter = 0;
	if(argc<2){//if the user didn't provide any arguments, show usage information and exit
		print_usage(2,argv[0]);
		exit(1);
	}
	if(strcmp(argv[1],"SET")==0){//if the first argument is SET, the user wishes to make a new log entry. 
		//initialize the character arrays to hold the inputs
		char* level = NULL;
		char* title = NULL;
		char* desc = NULL;
		//use getopt to parse the input flags (TODO: ensure proper behavior for all input. Works on valid input, unsure about invalid input )
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
		//if log level wasn't provided, print usage information
		if(atoi(level)==0){
			fprintf(stderr, "Please provide level\n");
			print_usage(0, argv[0]);
			exit(1);
		}
		//if log title wasn't provided, print usage information
		if(title==NULL){
			fprintf(stderr, "Please provide title\n");
			print_usage(0, argv[0]);
			exit(1);
		}

		char* user = (getpwuid(getuid()))->pw_name;//get the username of the user who invoked the client
		time_t secs = time(NULL);//get the current UNIX time
		char secs_str[64];
		sprintf(secs_str,"%ld", secs);//convert the time into a character array for the query (TODO: Figure out proper size)

		query_daemon_insert(level, title, desc, user, secs_str);//create and send the query

		exit(1);

	}else if(strcmp("GET",argv[1])==0){
		printf("GETting db");
	}else{//if arg[1] is neither SET nor GET, print usage information
		print_usage(2,argv[0]);
		exit(1);
	}
	
	return 0;
}