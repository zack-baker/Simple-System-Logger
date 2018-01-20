#include "syslog.h"


using namespace std;

static int callback(void* data, int argc, char** argv, char** azColName){
	int i=0;
	printf("In callback\n");
	for(int i=0;i<argc;i++){
		printf("%d %s",i, argv[i]);
	}
	return 0;
}

/*This function prints the usage information of the program*/
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
/**
* This function converts a string to its uppercase equivalent
*/
char* to_upper(const char* arg){
	int size = strlen(arg);
	char* upper_str = new char[size];
	for(int i=0;i<size;i++){
		upper_str[i] = toupper(arg[i]);
	}
	printf("New str is %s\n", upper_str);
	return upper_str;
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
	/* SET action */
	if(strcmp(argv[1],"SET")==0){
		char* log_type = to_upper(argv[2]);
		printf("Log Type: %s\n", log_type);
		printf("Additional comment - ");
		char* comment = new char[1024];
		scanf("%1024[^\n]",comment);
		time_t cur_time = time(NULL);
		struct tm tm = *localtime(&cur_time);
		stringstream insert_sql_ss;
		insert_sql_ss << "INSERT INTO logs(LOG_TIME, LOG_TYPE, COMMENTS) VALUES (\"" << tm.tm_year+1900 << "-" << tm.tm_mon+1 << "-" << tm.tm_mday << " " << tm.tm_hour << ":" << tm.tm_min << ":" << tm.tm_sec << "\",\"" << log_type << "\",\"" << comment << "\");";
		string insert_sql_str = insert_sql_ss.str();
		printf("SQL statement: %s",insert_sql_str.c_str());
		const char* insert_sql_cstr = insert_sql_str.c_str();
		rc = sqlite3_exec(db, insert_sql_cstr, callback,0,0);
		if(rc==SQLITE_ABORT){
			fprintf(stderr, "[ERROR] Error inserting into db, exiting\n");
			exit(_DB_QUERY_ERROR);
		}

	}
	/*REC action */

	return 0;
}