#include "sisyl_client.h"

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
		fprintf(stderr, "USAGE: %s SET -l LEVEL -t TITLE [description]\n", name);
		return;
	}
	if(mode==2){
		fprintf(stderr, "USAGE: %s SET|GET [options]\n",name );
	}
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
*	This method takes in the parameters passed to main (argc, argv) and both verifies that the proper arguments were passed, and if so, contains the logic for handling those arguments
*	INPUT:
*		- argc - an int holding the number of arguments in argv
*		- argv - an array of char*s which holds the arguments to the program
*	OUTPUT:
		- an integer code related to the success of the argument handling
			+-------+------------------------------------------------------------------------------+
			|  code |                                   status                                     |
			+=======+==============================================================================+
			|   0   |                  Success (no issues with provided arguments)                 |
			|   1   |   The user did not provide any arguments (there are no provided arguments)   |
			|   2   |    Using the SET parameter, the user did not provide the level argument      |
			|   3   |     Using the SET parameter, the user did not provide the title argument     |
			|   4   | The user provided a parameter in the SET|GET position, but is not SET or GET |
			|   5   |            Couldn't allocate memory to assemble description                  |
			+-------+------------------------------------------------------------------------------+
*/

int process_args(int argc, char** argv){

	int iter = 0;//variable used for getopt loops

	if(argc<2){//if the user didn't provide any arguments, show usage information and exit
		print_usage(2,argv[0]);
		return 1;
	}
	if(strcmp(argv[1],"SET")==0){//if the first argument is SET, the user wishes to make a new log entry. 

		//initialize the character arrays to hold the inputs
		char* level = NULL;
		char* title = NULL;
		char* desc = NULL;

		//use getopt to parse the input flags (TODO: ensure proper behavior for all input. Works on valid input, unsure about invalid input )
		while((iter=getopt(argc, argv,"l:t:"))!=-1){
			switch(iter){
				case 'l': level = optarg; break;
				case 't': title = optarg; break;
			}
		}
		//if there are more args than getopt saw (not counting SET), then there is a description provided
		if(optind<argc-1){
			desc = get_description(argc, argv);
			if(desc==NULL){
				return 5;
			}
		}else{
			desc = "<no description>";
		}
		printf("Level is: %s\n", level);
		printf("Title is: %s\n", title);
		printf("Description is %s\n", desc);

		//if log level wasn't provided, print usage information
		if(level==NULL || atoi(level)==0){
			fprintf(stderr, "Please provide level\n");
			print_usage(0, argv[0]);
			return 2;
		}
		//if log title wasn't provided, print usage information
		if(title==NULL){
			fprintf(stderr, "Please provide title\n");
			print_usage(0, argv[0]);
			return 3;
		}

		printf("args good\n");
		char* user = (getpwuid(getuid()))->pw_name;//get the username of the user who invoked the client
		time_t secs = time(NULL);//get the current UNIX time
		char secs_str[64];
		sprintf(secs_str,"%ld", secs);//convert the time into a character array for the query (TODO: Figure out proper size)

		query_daemon_insert(level, title, desc, user, secs_str);//create and send the query

		return 0;

	}else if(strcmp("GET",argv[1])==0){
		printf("GETting db");
	}else{//if arg[1] is neither SET nor GET, print usage information
		print_usage(2,argv[0]);
		return 4;
	}
}

/**
*	This method extracts the description from the provided arguments. This allows the description to be written as normal text on the command line, i.e. with spaces and without quotes. 
*	INPUT:
		- argc - an int holding the number of arguments passed to the program
		- argv - an array of char*s which holds the arguments passed to the program
*/
char* get_description(int argc, char** argv){
	int desc_args = argc-7;//figure out how many of the arguments provided are description arguments
	if(desc_args<0){//if there are fewer than the proper number of arguments, then exit out of the function, returning NULL. This should only happen if this function is called improperly
		return NULL;
	}
	int totalsize = 0;
	for(int i=argc-desc_args;i<argc;i++){
		totalsize += strlen(argv[i]);
		if(i!=argc-1){
			totalsize++;//if we aren't on the last part, add a byte for each space
		}
	}
	char* full_desc = malloc(totalsize);
	if(full_desc!=NULL){//if we successfully malloc'ed
		printf("Index starting at %d\t num args: %d\n", argc-(desc_args+1), argc);
		for(int i=argc-desc_args-1;i<argc;i++){
			//concatenate each piece of the description in full_desc
			printf("Concatenating %s\n", argv[i]);
			strcat(full_desc,argv[i]);
			if(i!=argc-1){strcat(full_desc, " ");}

		}
		printf("Full description: %s\n", full_desc);

		return full_desc;
	}
	else{
		fprintf(stderr,"ERROR: Couldn't allocate memory to assemble description\n");
		return NULL;
	}

}	

