#include "CuTest.h"
#include <stdio.h>

CuSuite* get_description_get_suite();

void runAllTests(){
	CuString* output = CuStringNew();
	CuSuite* suite = CuSuiteNew();

	CuSuiteAddSuite(suite, get_description_get_suite());

	CuSuiteRun(suite);
	CuSuiteSummary(suite,output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);

	if(suite->failCount>0){
		exit(1);
	}

}

int main()
{
	runAllTests();
	return 0;
}