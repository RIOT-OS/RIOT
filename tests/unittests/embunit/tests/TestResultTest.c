#include <embUnit/embUnit.h>

static void setUp(void)
{
}

static void tearDown(void)
{
}

static void testTestResult_result(void)
{
	TestResult result = new_TestResult(NULL);

	TEST_ASSERT_EQUAL_INT(0, result.runCount);
	TEST_ASSERT_EQUAL_INT(0, result.failureCount);
}

static void testTestResult_startTest(void)
{
	TestResult result = new_TestResult(NULL);

	TestResult_startTest(&result,NULL);

	TEST_ASSERT_EQUAL_INT(1, result.runCount);
	TEST_ASSERT_EQUAL_INT(0, result.failureCount);
}

static void testTestResult_endTest(void)
{
	TestResult result = new_TestResult(NULL);

	TestResult_endTest(&result,NULL);

	TEST_ASSERT_EQUAL_INT(0, result.runCount);
	TEST_ASSERT_EQUAL_INT(0, result.failureCount);
}

static void testTestResult_addFailure(void)
{
	TestResult result = new_TestResult(NULL);

	TestResult_addFailure(&result,NULL,"",0,"");

	TEST_ASSERT_EQUAL_INT(0, result.runCount);
	TEST_ASSERT_EQUAL_INT(1, result.failureCount);
}

TestRef TestResultTest_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
		new_TestFixture("testTestResult_result",testTestResult_result),
		new_TestFixture("testTestResult_startTest",testTestResult_startTest),
		new_TestFixture("testTestResult_endTest",testTestResult_endTest),
		new_TestFixture("testTestResult_addFailure",testTestResult_addFailure),
	};
	EMB_UNIT_TESTCALLER(TestResultTest,"TestResultTest",setUp,tearDown,fixtures);

	return (TestRef)&TestResultTest;
}
