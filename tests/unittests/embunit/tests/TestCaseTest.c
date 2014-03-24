#include <embUnit/embUnit.h>
#include "MockTestCase.h"

static void setUp(void)
{
}

static void tearDown(void)
{
}

static void testName(void)
{
	TestCaseRef mock = MockTestCase_case();
	TEST_ASSERT_EQUAL_STRING("MockTestCase", mock->isa->name(mock));
}

static void testCountTestCases(void)
{
	TestCaseRef mock = MockTestCase_case();
	TEST_ASSERT_EQUAL_INT(1, mock->isa->countTestCases(mock));
}

static void success_runTest(void)
{
}

static void testSuccess(void)
{
	TestCase tcase = new_TestCase("success",NULL,NULL,success_runTest);
	TestResult result = new_TestResult(NULL);

	tcase.isa->run(&tcase,&result);

	TEST_ASSERT_EQUAL_INT(1, result.runCount);
	TEST_ASSERT_EQUAL_INT(0, result.failureCount);
}

static void failure_runTest(void)
{
	TEST_FAIL("");
}

static void testFailure(void)
{
	TestCase tcase = new_TestCase("failure",NULL,NULL,failure_runTest);
	TestResult result = new_TestResult(NULL);

	tcase.isa->run(&tcase,&result);

	TEST_ASSERT_EQUAL_INT(1, result.runCount);
	TEST_ASSERT_EQUAL_INT(1, result.failureCount);
}

TestRef TestCaseTest_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
		new_TestFixture("testName",testName),
		new_TestFixture("testCountTestCases",testCountTestCases),
		new_TestFixture("testSuccess",testSuccess),
		new_TestFixture("testFailure",testFailure),
	};
	EMB_UNIT_TESTCALLER(TestCaseTest,"TestCaseTest",setUp,tearDown,fixtures);

	return (TestRef)&TestCaseTest;
}
