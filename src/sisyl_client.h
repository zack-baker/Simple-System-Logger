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
	int level;
	char* request_type;

	int return_code;
} Params;

void print_usage(int, char*);
char* send_request(char*);
void query_daemon_insert(char*, char*, char*, char*, char*);
char* get_description(int,char**);
Params* process_args(int, char**);
void print_args(int, char**);

