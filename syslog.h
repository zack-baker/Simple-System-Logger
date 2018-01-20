#include <stdio.h>
#include <cstdlib>
#include <sqlite3.h>
#include <string.h>
#include <ctype.h>
#include <iostream>
#include <time.h>
#include <sstream>

const int _DB_CONN_ERROR = -1;
const int _BAD_ARGS = -2;
const int _HELP_MSG = -3;
const int _DB_QUERY_ERROR = -4;


static int callback(void*, int, char**, char**);
void print_usage(const char*);
int verify_args(int, const char*);
char* to_upper(char*);