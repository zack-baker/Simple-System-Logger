#include "syslog.h"
#include <sqlite3.h>

int main(int argc, char const *argv[])
{
	printf("Hello world\n");
	sqlite3* db;
	int rc = sqlite3_open("syslog.db",&db);
	if(rc){//if an error occured connecting to the database
		fprintf(stderr, "[ERROR] Cannot connect to database: %s\n", sqlite3_errmsg(db));
	}
	return 0;
}