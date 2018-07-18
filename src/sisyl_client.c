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
	process_args(argc, argv);
	return 0;
}