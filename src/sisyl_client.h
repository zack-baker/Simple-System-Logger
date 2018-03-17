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

void print_usage(int, char*);
char* send_request(char*);
void query_daemon_insert(char*, char*, char*, char*, char*);
int main(argc, argv);