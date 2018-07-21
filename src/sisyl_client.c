#include "sisyl_client.h"

/**
*	This is the main function of the client. 
*	INPUT:
		- argc - the number of command line arguments
		- argv - the arguments to the client.
			* argv[1] must be either "SET" or "GET", to specify the type of request to the daemon
			* the other arguments are some combination of -l $level, -t $title, and an optional $description. -l and -t are required. 
	OUTPUT
		- 0 if operation successful
		- 1 otherwise
*/
int main(int argc, char** argv)
{
	Params* p = (Params*)(process_args(argc, argv));
	printf("return_code: %d\n", p->return_code);
	if(p->return_code!=0){//if params are bad
		return 1;//return non-zero
	}

	//Do set stuff here
	char* user = (getpwuid(getuid()))->pw_name;//get the username of the user who invoked the client
	time_t secs = time(NULL);//get the current UNIX time
	char secs_str[64];
	sprintf(secs_str,"%ld", secs);//convert the time into a character array for the query (TODO: Figure out proper size)

	QueryParams* qp = malloc(sizeof(QueryParams));
	qp->title = p->title;
	qp->description = p->description;
	qp->user=user;
	qp->timestr=secs_str;
	qp->level=p->level;
	free(p);
	char* query = get_daemon_insert_query(qp);
	free(qp);


	//Do get stuff here, once params struct is fleshed out

	return 0;
}