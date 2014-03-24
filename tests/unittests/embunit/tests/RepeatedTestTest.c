#include <embUnit/embUnit.h>
#include "MockTestCase.h"

static void setUp(void)
{
}

static void tearDown(void)
{
}

static void testRepeatedOnce(void)
{
	RepeatedTest test = new_RepeatedTest(MockTestCase_case(),1);
	TestResult result = new_TestResult(NULL);

	test.isa->run(&test,&result);

	TEST_ASSERT_EQUAL_INT(1, result.runCount);
	TEST_ASSERT_EQUAL_INT(1, test.isa->countTestCases(&test));
}

static void testRepeatedMoreThanOnce(void)
{
	RepeatedTest test = new_RepeatedTest(MockTestCase_case(),100);
	TestResult result = new_TestResult(NULL);

	test.isa->run(&test,&result);


	TEST_ASSERT_EQUAL_INT(100, result.runCount);
	TEST_ASSERT_EQUAL_INT(100, test.isa->countTestCases(&test));
}

static void testRepeatedZero(void)
{
	RepeatedTest test = new_RepeatedTest(MockTestCase_case(),0);
	TestResult result = new_TestResult(NULL);

	test.isa->run(&test,&result);


	TEST_ASSERT_EQUAL_INT(0, result.runCount);
	TEST_ASSERT_EQUAL_INT(0, test.isa->countTestCases(&test));
}

TestRef RepeatedTestTest_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
		new_TestFixture("testRepeatedOnce",testRepeatedOnce),
		new_TestFixture("testRepeatedMoreThanOnce",testRepeatedMoreThanOnce),
		new_TestFixture("testRepeatedZero",testRepeatedZero),
	};
	EMB_UNIT_TESTCALLER(RepeatedTestTest,"RepeatedTestTest",setUp,tearDown,fixtures);

	return (TestRef)&RepeatedTestTest;
}
