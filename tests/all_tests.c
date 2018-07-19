#include "CuTest.h"
#include <stdio.h>
#include <stdlib.h>

CuSuite* get_description_get_suite();
CuSuite* process_args_get_suite();

int runSuite(CuSuite* suite){
	CuString* output = CuStringNew();

	CuSuiteRun(suite);

	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);

	printf("%s\n", output->buffer);

	return suite->failCount;
}
void runAllTests(){
	int totalFails = 0;
	totalFails += runSuite(get_description_get_suite());
	totalFails += runSuite(process_args_get_suite());
	if(totalFails>0){
		exit(1);
	}
}

int main()
{
	runAllTests();
	return 0;
}