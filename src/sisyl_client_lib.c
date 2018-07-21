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
		- A pointer to a queryparam struct, containing the requests level, title, description, the name of the user invoking the request, and a char* representation of the current UNIX timestamp
	OUTPUT:
		returns a char* holding the full request
*/
char* get_daemon_insert_query(QueryParams* qp){
	printf("In query former\n");
	size_t query_len = 8 + strlen(qp->level) + strlen(qp->title) + strlen(qp->description) + strlen(qp->user) + strlen(qp->timestr); //this variable holds the length of the char* to create for the full query. The total size is the size of each individual char*, plus 3 (for the constant string "SET") plus 5 spaces. 
	printf("Size is: %ld\n", query_len);
	char* query = malloc(sizeof(char) * query_len);//create the full query array
	printf("malloced query\n");
	sprintf(query,"SET %s %s %s %s %s", qp->level, qp->title, qp->description, qp->user, qp->timestr);//stick the individual arrays together
	printf("assigned query\n");
	printf("Full query is %s\n", query);
	return query;
}

/**
*	This method takes in the parameters passed to main (argc, argv) and both verifies that the proper arguments were passed, and if so, contains the logic for handling those arguments
*	INPUT:
*		- argc - an int holding the number of arguments in argv
*		- argv - an array of char*s which holds the arguments to the program
*	OUTPUT:
		- returns a pointer to a params struct containing the parsed arguments
			- contains an integer code related to the success of the argument handling
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

void* process_args(int argc, char** argv){
	printf("~~~~~~~~~~~~~~~~~~~~~~\n");
	

	int iter = 0;//variable used for getopt loops
	print_args(argc, argv);


	if(argc<2){//if the user didn't provide any arguments, show usage information and exit
		Params* p = malloc(sizeof(Params));
		p->return_code = 1;
		print_usage(2,argv[0]);
		return p;
	}
	if(strcmp(argv[1],"SET")==0){//if the first argument is SET, the user wishes to make a new log entry. 

		Params* p = malloc(sizeof(Params));
		p->title = NULL;
		p->description = NULL;
		p->level = NULL;
		//initialize the character arrays to hold the inputs
		
		p->request_type = "SET";
		optind = 1;//reset getopt index. Important when running tests, since they all run in the same environment. This ensures getopt will read the entire argv array from the beginning

		//use getopt to parse the input flags (TODO: ensure proper behavior for all input. Works on valid input, unsure about invalid input )
		printf("Processing args...\n");
		while((iter=getopt(argc, argv,"l:t:"))!=-1){
			printf("Iter: %c\n", iter);
			switch(iter){
				case 'l': p->level = optarg; break;				
				case 't': p->title = optarg; break;
			}
		}
		printf("args processed\n");
		//if there are more args than getopt saw (not counting SET), then there is a description provided
		if(optind<argc-1){
			p->description = get_description(argc, argv);
			if(p->description==NULL){
				p->return_code = 5;
				return p;
			}
		}else{
			p->description = "<no description>";
		}
		printf("Level is: %s\n", p->level);
		printf("Title is: %s\n", p->title);
		printf("Description is %s\n", p->description);
		if(p->level==NULL){//if the level is not provided, and kept as INT_MIN, notify and exit
			fprintf(stderr, "Please provide level\n");
			print_usage(0, argv[0]);
			p->return_code = 2;
			return p;
		}
		

		//if log title wasn't provided, print usage information
		if(p->title==NULL){
			fprintf(stderr, "Please provide title\n");
			print_usage(0, argv[0]);
			p->return_code = 3;
			return p;
		}

		printf("args good\n");
		/*
		

		query_daemon_insert(level, title, desc, user, secs_str);//create and send the query
		*/

		p->return_code = 0;
		return p;

	}else if(strcmp("GET",argv[1])==0){
		Params* p = malloc(sizeof(Params));
		printf("GETting db");
		p->return_code = 0;
		return p;
	}else{//if arg[1] is neither SET nor GET, print usage information
		Params* p = malloc(sizeof(Params));
		print_usage(2,argv[0]);
		p->return_code = 4;
		return p;
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

void print_args(int argc, char** argv){
	for(int i=0;i<argc;i++){
		printf("%s", argv[i]);
		if(i!=argc-1){
			printf(" ");
		}
	}
	printf("\n");
}