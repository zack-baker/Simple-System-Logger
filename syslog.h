#include <stdio.h>
#include <cstdlib>
#include <sqlite3.h>
#include <string.h>
const int _DB_CONN_ERROR = -1;
const int _BAD_ARGS = -2;
const int _HELP_MSG = -3;
static int callback(void*, int, char**, char**);