/**
*	This file includes all tests for the SiSyL client program
*/
#include "../src/sisyl_client.h"
#include "CuTest.h"


/***** get_description tests *****/

//basic test
void test_get_description_basic(CuTest* tc){
	char* args[8] = {"./sisyl_client", "SET", "-l", "1", "-t", "title", "test", "description"};
	int argc = 8;
	char* result = get_description(argc, args);
	CuAssertStrEquals(tc, "test description", result);
}
//one-word description test
void test_get_description_one_word(CuTest* tc){
	char* args[7] = {"./sisyl_client", "SET", "-l", "1", "-t", "title", "description"};
	int argc = 7;
	char* result = get_description(argc, args);
	CuAssertStrEquals(tc, "description", result);
}

//long description test
void test_get_description_long(CuTest* tc){
	char* args[19] = {"./sisyl_client", "SET", "-l", "1", "-t", "title", "this", "is", "a", "long", "description.", "It", "has", "multiple", "sentences.", "It", "also", "has", "punctuation!"};
	int argc = 19;
	char* result = get_description(argc, args);
	CuAssertStrEquals(tc, "this is a long description. It has multiple sentences. It also has punctuation!", result);
}

//empty description test
void test_get_description_empty(CuTest* tc){
	char* args[6] = {"./sisyl_client", "SET", "-l", "1", "-t" "title"};
	int argc = 6;
	char* result = get_description(argc, args);
	CuAssertPtrEquals(tc, NULL, result);
}

//unicode description test
void test_get_description_unicode(CuTest* tc){
	char* args[7] = {"./sisyl_client", "SET", "-l", "1", "-t", "title", "ѬՒ༂Δǣ"};
	int argc=7;
	char* result=  get_description(argc, args);
	CuAssertStrEquals(tc, "ѬՒ༂Δǣ", result);
}
//unicode with space description test
void test_get_description_unicode_long(CuTest* tc){
	char* args[8] = {"./sisyl_client", "SET", "-l", "1", "-t", "title", "⚑❹ⴰ", "ᢆᢆᢆᢆᢆᢆᘐᖠᖰ"};
	int argc = 8;
	char* result = get_description(argc, args);
	CuAssertStrEquals(tc, "⚑❹ⴰ ᢆᢆᢆᢆᢆᢆᘐᖠᖰ", result);
}
//Right-to-Left text description test
void test_get_description_RTL(CuTest* tc){
	char* args[7] = {"./sisyl_client", "SET", "-l", "1", "-t", "title", "وصف"};
	int argc = 7;
	char* result = get_description(argc, args);
	CuAssertStrEquals(tc, "وصف", result);
}
//Right-to-Left text with spaces description test
void test_get_description_RTL_long(CuTest* tc){
	char* args[8] = {"./sisyl_client", "SET", "-l", "1", "-t", "title","مرحبا","بالعالم"};
	int argc = 8;
	char* result = get_description(argc, args);
	CuAssertStrEquals(tc, "مرحبا بالعالم", result);
}

//Get description method test suite
CuSuite* get_description_get_suite(){
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, test_get_description_basic);
	SUITE_ADD_TEST(suite, test_get_description_one_word);
	SUITE_ADD_TEST(suite, test_get_description_long);
	SUITE_ADD_TEST(suite, test_get_description_empty);
	SUITE_ADD_TEST(suite, test_get_description_unicode);
	SUITE_ADD_TEST(suite, test_get_description_unicode_long);
	SUITE_ADD_TEST(suite, test_get_description_RTL);
	SUITE_ADD_TEST(suite, test_get_description_RTL_long);
	return suite;
}



/* process_args tests */

//proper usage test
void test_process_args_basic(CuTest* tc){
	char* args[10] = {"./sisyl_client", "SET", "-l", "1", "-t", "title", "this", "is", "a", "description"};
	int argc = 10;
	Params* result = process_args(argc, args);
	CuAssertIntEquals(tc, 0, result->return_code);
	free(result);
}

//no argument test
void test_process_args_no_args(CuTest* tc){
	char* args[1] = {"./sisyl_client"};
	int argc = 1;
	Params* result = process_args(argc, args);
	CuAssertIntEquals(tc, 1, result->return_code);
	free(result);
}

//only "SET" argument test
void test_process_args_only_set(CuTest* tc){
	char* args[2] = {"./sisyl_client", "SET"};
	int argc = 2;
	Params* result = process_args(argc, args);
	CuAssertIntEquals(tc, 2, result->return_code);
	free(result);
}

//no title test
void test_process_args_no_title(CuTest* tc){
	char* args[4] = {"./sisyl_client", "SET", "-l", "1"};
	int argc = 4;
	Params* result = process_args(argc, args);
	CuAssertIntEquals(tc, 3, result->return_code);
	free(result);
}

//level flag but no level arg test
void test_process_args_no_level_arg(CuTest* tc){
	char* args[3] = {"./sisyl_client", "SET", "-l"};
	int argc = 3;
	Params* result = process_args(argc, args);
	CuAssertIntEquals(tc, 2, result->return_code);
	free(result);
}
//title but no level test
void test_process_args_title_no_level(CuTest* tc){
	char* args[4] = {"./sisyl_client", "SET", "-t", "title"};
	int argc = 4;
	Params* result = process_args(argc, args);
	CuAssertIntEquals(tc, 2, result->return_code);
	free(result);
}
//no title argument test
void test_process_args_no_title_arg(CuTest* tc){
	printf("Test_process_args_no_title_arg_test\n");
	char* args[5] = {"./sisyl_client", "SET", "-l", "1", "-t"};
	int argc = 5;
	Params* result = process_args(argc, args);
	CuAssertIntEquals(tc, 3, result->return_code);
	free(result);
}
void test_process_args_bad_command(CuTest* tc){
	char* args[2] = {"./sisyl_client", "GREP"};
	int argc = 2;
	Params* result = process_args(argc, args);
	CuAssertIntEquals(tc, 4, result->return_code);
	free(result);
}
void test_process_args_command_typo(CuTest* tc){
	char* args[7] = {"./sisyl_client", "SEY", "-l", "1", "-t", "Title", "test"};
	int argc = 7;
	Params* result = process_args(argc, args);
	CuAssertIntEquals(tc, 4, result->return_code);
	free(result);
}
//arguments permuted test
void test_process_args_permuted_args(CuTest* tc){
	char* args[7] = {"./sisyl_client", "SET", "-t", "Title", "-l", "1", "test"};
	int argc = 7;
	Params* result = process_args(argc, args);
	CuAssertIntEquals(tc, 0, result->return_code);
	free(result);
}


// process_args method test suite
CuSuite* process_args_get_suite(){
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, test_process_args_no_args);
	SUITE_ADD_TEST(suite, test_process_args_only_set);
	SUITE_ADD_TEST(suite, test_process_args_no_title);
	SUITE_ADD_TEST(suite, test_process_args_title_no_level);
	SUITE_ADD_TEST(suite, test_process_args_no_level_arg);
	SUITE_ADD_TEST(suite, test_process_args_no_title_arg); //bizarre optarg issue
	SUITE_ADD_TEST(suite, test_process_args_bad_command);
	SUITE_ADD_TEST(suite, test_process_args_command_typo);
	SUITE_ADD_TEST(suite, test_process_args_basic);
	SUITE_ADD_TEST(suite, test_process_args_permuted_args);
	
	
	return suite;
}