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
	CuAssertStrEquals(tc, "testdescription", result);
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

void test_get_description_unicode_long(CuTest* tc){
	char* args[8] = {"./sisyl_client", "SET", "-l", "1", "-t", "title", "⚑❹ⴰ", "ᢆᢆᢆᢆᢆᢆᘐᖠᖰ"};
	int argc = 8;
	char* result = get_description(argc, args);
	CuAssertStrEquals(tc, "⚑❹ⴰ ᢆᢆᢆᢆᢆᢆᘐᖠᖰ", result);
}
void tess_get_description_RTL(CuTest* tc){
	char* args[7] = {"./sisyl_client", "SET", "-l", "1", "-t", "title", "وصف"};
	int argc = 7;
	char* result = get_description(argc, args);
	CuAssertStrEquals(tc, "وصف", result);
}
void test_get_description_RTL_long(CuTest* tc){
	char* args[8] = {"./sisyl_client", "SET", "-l", "1", "-t", "title","مرحبا","بالعالم"};
	int argc = 8;
	char* result = get_description(argc, args);
	CuAssertStrEquals(tc, "مرحبا بالعالم", result);
}
CuSuite* get_description_get_suite(){
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, test_get_description_basic);
	SUITE_ADD_TEST(suite, test_get_description_one_word);
	SUITE_ADD_TEST(suite, test_get_description_long);
	SUITE_ADD_TEST(suite, test_get_description_empty);
	SUITE_ADD_TEST(suite, test_get_description_unicode);
	SUITE_ADD_TEST(suite, test_get_description_unicode_long);
	SUITE_ADD_TEST(suite, tess_get_description_RTL);
	SUITE_ADD_TEST(suite, test_get_description_RTL_long);
	return suite;
}



/* process_args tests */

// 