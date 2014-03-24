#include <embUnit/embUnit.h>

static void setUp(void)
{
}

static void tearDown(void)
{
}

static void verify(TestCaseRef test)
{
	TestResult result = new_TestResult(NULL);

	test->isa->run(test,&result);

	if (result.failureCount == 0) {
		TEST_FAIL("fail");
	}
}

static void assert_equal_string_runTest(void)
{
	TEST_ASSERT_EQUAL_STRING("123","456");
}

static void assert_equal_int_runTest(void)
{
	TEST_ASSERT_EQUAL_INT(123,456);
}

static void assert_null_runTest(void)
{
	char *p="";
	TEST_ASSERT_NULL(p);
}

static void assert_not_null_runTest(void)
{
	char *p=NULL;
	TEST_ASSERT_NOT_NULL(p);
}

static void assert_message_runTest(void)
{
	TEST_ASSERT_MESSAGE(0,"0");
}

static void assert_runTest(void)
{
	TEST_ASSERT(0);
}

static void testASSERT_EQUAL_STRING(void)
{
	TestCase tcase = new_TestCase("assert_equal_string",NULL,NULL,assert_equal_string_runTest);
	verify(&tcase);
}

static void testASSERT_EQUAL_INT(void)
{
	TestCase tcase = new_TestCase("assert_equal_int",NULL,NULL,assert_equal_int_runTest);
	verify(&tcase);
}

static void testASSERT_NULL(void)
{
	TestCase tcase = new_TestCase("assert_null",NULL,NULL,assert_null_runTest);
	verify(&tcase);
}

static void testASSERT_NOT_NULL(void)
{
	TestCase tcase = new_TestCase("assert_not_null",NULL,NULL,assert_not_null_runTest);
	verify(&tcase);
}

static void testASSERT_MESSAGE(void)
{
	TestCase tcase = new_TestCase("assert_message",NULL,NULL,assert_message_runTest);
	verify(&tcase);
}

static void testASSERT(void)
{
	TestCase tcase = new_TestCase("assert",NULL,NULL,assert_runTest);
	verify(&tcase);
}

TestRef assertTest_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
		new_TestFixture("testASSERT_EQUAL_STRING",testASSERT_EQUAL_STRING),
		new_TestFixture("testASSERT_EQUAL_INT",testASSERT_EQUAL_INT),
		new_TestFixture("testASSERT_NULL",testASSERT_NULL),
		new_TestFixture("testASSERT_NOT_NULL",testASSERT_NOT_NULL),
		new_TestFixture("testASSERT_MESSAGE",testASSERT_MESSAGE),
		new_TestFixture("testASSERT",testASSERT),
	};
	EMB_UNIT_TESTCALLER(AssertTest,"AssertTest",setUp,tearDown,fixtures);

	return (TestRef)&AssertTest;
}
