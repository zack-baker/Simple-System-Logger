#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>

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

	//umake(0); //pretty sure we dont want this cause we are making db calls

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

void write_signal_receive(){
	syslog(LOG_NOTICE, "write request received\n");
}
void read_signal_receive(){
	syslog(LOG_NOTICE, "read request receieved\n");
}
int main(){
	//initialize the daemon
	daemon_skeleton();

	//set signal handlers
	signal(SIGUSR1,write_signal_receive);
	signal(SIGUSR2,read_signal_receive);
	while(1);

	syslog(LOG_NOTICE,"We done dawg");
	closelog();

	return EXIT_SUCCESS;
}