#include "sisyld.h"


/**
*	This function sets up the program as a daemon
*/
void daemon_skeleton(){
	pid_t pid; //the pid of the daemon fork

	pid = fork();//fork the current process, to be free of a TTY

	//if the fork() call returns an error
	if(pid==-1){
		exit(EXIT_FAILURE);//exit the daemon
	}

	//If the fork call was successful, the child is assigned a non-zero PID, and we can exit the parent process
	if(pid>0){
		exit(EXIT_SUCCESS);
	}


	//change directory to root
	int chdir_status = chdir("/");

	//if changing directories doesn't work, exit
	if(chdir_status==-1){
		exit(EXIT_FAILURE);
	}

	//close all file descriptors
	int fd;
	for(fd = sysconf(_SC_OPEN_MAX); fd>=0; fd--){
		close(fd);
	}

	//make a log in the official system log
	openlog("sysld",LOG_PID, LOG_DAEMON);

	return;
}



/**
*	This function handles SET requests. SET requests are structured as follows: SET $level $date $user $title $description 
*/
int process_set(char* request, size_t request_len){
	char* saveptr;
	char* res = strtok_r(request, " ", &saveptr);//process the request's first item. we don't care about it, since we already know the type of request. This helps streamline data extraction later in the function
	return 0;
}
int process_get(char* request, size_t request_len){
	return 0;
}
/*
*	This function handles the incoming request to the daemon
*	INPUT:
		- request - a char* which holds the request to process
		- request_len - the size of request
	OUTPUT:
		- returns an integer corresponding to the relative success of the execution of the request, as described below:
		|----------------------------------------------|
		|   code    |              status              |
		|----------------------------------------------|
		|      0    | request execution was successful |
		|     -1    | request type was null or empty   |
		|  INT_MIN  | unknown error                    |
		|----------------------------------------------|
*/
int process_request(char* request, size_t request_len){
	syslog(LOG_INFO, "[INFO] Request receieved: %s", request);
	//we want to make a copy of the request received because strtok_r, used to split the request to parse it's contents, modifies the given pointer
	char req_copy[request_len];
	strcpy(req_copy, request);
	char* req_type; //make a char* to hold the kind of request made. Request types are always 3 characters (GET, SET)
	char* save_ptr;
	req_type = strtok_r(req_copy, " ", &save_ptr);
	syslog(LOG_INFO, "[INFO] REQ type is %s", req_type);
	if(strcmp(req_type,"GET")!=0 && strcmp(req_type,"SET")!=0){//if the request type isn't SET or GET, alert the user and abort
		syslog(LOG_WARNING, "[WARN] Unrecognized request type");
		return -1; 
	}
	else if(strcmp(req_type,"GET")==0){// if the request is a "get" request, handle it here
		syslog(LOG_INFO,"[INFO] Request of type GET");
		return process_get(request, request_len);
	}	
	else if(strcmp(req_type,"SET")==0){// if the request is a "set" request, handle it here
		syslog(LOG_INFO, "[INFO] Request of type SET");
		return process_set(request, request_len);
	}else{
		//we should never get here. There must be a missed condition if we do.
		return INT_MIN;
	}

}


/**
*	This is the main method of the daemon. It will create the process as a daemon, and then start a server to listen to incoming requests (by default on port 8585). 
*/
int main(){

	//create the system's variables
	int socket_file_descriptor;//file descriptor for the server socket
	int client_socket_file_descriptor;//file descriptor for the client socket
	int port_num = 8585;//variable holding the port number 
	size_t mesg_buffer_len = 1024;
	char mesg_buffer[mesg_buffer_len];//char array to buffer messages back and forth between the client and the server
	socklen_t clilen;
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	
	daemon_skeleton();

	socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0); //we want to use a SOCK_STREAM socket, as it provides a rigourous socket structure for lossless communication. 
	if(socket_file_descriptor < 0){//if we can't create a socket, log the error in the system log and exit
		syslog(LOG_ALERT, "[FATAL] Cannot create socket for communication, exiting daemon");
		exit(EXIT_FAILURE);
	}
	bzero((char *)&serv_addr, sizeof(serv_addr)); //"initialize" the serv_addr structure by writing it's memory with all zeros
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port_num);

	if(bind(socket_file_descriptor, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0){//bind the socket to the given port. if it fails, log the error and exit
		syslog(LOG_ALERT, "[FATAL] Cannot bind socket to port %d, exiting daemon", port_num);
		exit(EXIT_FAILURE);
	}

	listen(socket_file_descriptor, 16);//have 16 potential requests in queue. We probably won't need many, as the system will not be queried often
	clilen = sizeof(cli_addr);
	// This is the main loop of the function. It waits and listens for an incoming connection, reads in a request from the client, and processes it.  
	while(1){
		client_socket_file_descriptor = accept(socket_file_descriptor, (struct sockaddr *)&cli_addr, &clilen);//accept an incoming connection
		if(client_socket_file_descriptor < 0){//if there's an error accepting a connection, close the connection and log the event, but don't panic
			close(client_socket_file_descriptor);
			syslog(LOG_WARNING, "[NOTICE] client unable to be accepted");
			continue;
		}
		pid_t pid = fork();//fork the process to handle the request
		if(pid<0){//if the fork fails log the event, but don't panic
			close(client_socket_file_descriptor);
			syslog(LOG_WARNING, "[NOTICE] Failed to fork process to handle client request");
			continue;
		}
		if(pid==0){//if the fork was successful and we are the child process, handle the current client's request
			close(socket_file_descriptor);//we are the child process, not the main process, so we don't need to listen on the server socket anymore
			bzero(mesg_buffer,mesg_buffer_len);//"empty" the message buffer by writing it with all zeros
			ssize_t io_status = read(client_socket_file_descriptor, mesg_buffer, mesg_buffer_len-1);//read into mesg_buffer from the client
			if(io_status<0){//if the read failed, log the incident and exit the child process
				close(client_socket_file_descriptor);
				syslog(LOG_ERR, "[ERROR] Unable to read from client, exiting child");
				exit(EXIT_FAILURE);
			}
			process_request(mesg_buffer, mesg_buffer_len);//handle the request from the client
			exit(EXIT_SUCCESS);//exit the child process
		}
		close(client_socket_file_descriptor);//close the current client's connection and prepare for the next incoming connection
	}
	closelog();
	return EXIT_SUCCESS;
}