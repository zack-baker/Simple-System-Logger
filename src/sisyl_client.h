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
#include <limits.h>

typedef struct{
	char* title;
	char* description;
	char* level;//might make char* so we dont need to convert unneccesarily from char* to int, then back from int to char* when creating the daemon inserter struct
	char* request_type;

	int return_code;
} Params;

typedef struct{
	char* title;
	char* description;
	char* level;
	char* user;
	char* timestr;

} QueryParams;	

void print_usage(int, char*);
char* send_request(char*);
char* get_daemon_insert_query(QueryParams*);
char* get_description(int,char**);
void* process_args(int, char**);
void print_args(int, char**);

