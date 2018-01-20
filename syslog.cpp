#include "syslog.h"


using namespace std;

static int callback(void* data, int argc, char** argv, char** azColName){
	int i=0;
	printf("Inn callback\n");
	for(int i=0;i<argc;i++){
		printf("%d %s",i, argv[i]);
	}
	return 0;
}

void print_usage(const char* bin_name){
	printf("USAGE: %s SET [log type]\n", bin_name);
	printf("       %s REC <number of logs>\n", bin_name);
}

/** This method verifies if the given input is valid and which method to employ
	INPUT:
		- argc: number of arguments provided to the program, including the binary name
		- argv: the arguments provided to the argument, including the binary name
	OUTPUT:
		This method returns 1 if an entry is being added to the db, 2 if logs are being recalled, 3 if logs are being recalled and a number of logs is explicitly specified

*/
int verify_args(int argc, char const* argv[]){
	int method = 0;
	if(argc<2){
		fprintf(stderr, "[ERROR] Incorrect usage - too few arguments\n");
		print_usage(argv[0]);
		exit(_BAD_ARGS);
	}
	if(strcmp(argv[1],"-h")==0){
		print_usage(argv[0]);
		exit(_HELP_MSG);
	}
	if(strcmp(argv[1],"SET")!=0 && strcmp(argv[1],"REC")!=0){
		fprintf(stderr,"[ERROR] Incorrect usage - not a proper keyword\n");
		print_usage(argv[0]);
		exit(_BAD_ARGS);
	}
	if(strcmp(argv[1],"SET")==0 && argc==2){
		fprintf(stderr, "[ERROR] Incorrect usage - too few arguments for SET\n");
		print_usage(argv[0]);
		exit(_BAD_ARGS);
	}
	if(strcmp(argv[1],"SET")==0){
		method = 1;
	}
	if(strcmp(argv[1],"REC")==0 && argc==2){
		method = 2;
	}
	if(strcmp(argv[1],"REC")==0 && argc==3){
		method = 3;
	}
	return method;
}
int main(int argc, char const *argv[])
{
	verify_args(argc, argv);
	sqlite3* db;
	int rc = sqlite3_open("syslog.db",&db);
	if(rc){//if an error occured connecting to the database
		fprintf(stderr, "[ERROR] Cannot connect to database: %s\n", sqlite3_errmsg(db));
		exit(_DB_CONN_ERROR);
	}
	const char* create_table_sql = "CREATE TABLE if not exists logs(ID INTEGER PRIMARY KEY AUTOINCREMENT, LOG_TIME DATETIME NOT NULL, LOG_TYPE NVARCHAR NOT NULL, COMMENTS VARCHAR)";
	rc = sqlite3_exec(db, create_table_sql, callback, 0, 0);

	return 0;
}