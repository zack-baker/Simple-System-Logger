#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <limits.h>

void daemon_skeleton();

int process_get(char*,size_t);
int process_set(char*,size_t);
int process_request(char*, size_t);

int main();
