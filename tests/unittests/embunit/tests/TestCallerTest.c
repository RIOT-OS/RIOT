#include <embUnit/embUnit.h>

static void setUp(void)
{
}

static void tearDown(void)
{
}

static void testOneFixture(void)
{
	TestFixture	fixtures[] = {
		new_TestFixture(NULL,NULL),
	};
	TestCaller caller = new_TestCaller(NULL,NULL,NULL,1,fixtures);
	TestResult result = new_TestResult(NULL);

	caller.isa->run(&caller,&result);

	TEST_ASSERT_EQUAL_INT(1, result.runCount);
	TEST_ASSERT_EQUAL_INT(1, caller.isa->countTestCases(&caller));
}

static void testMoreThanOne(void)
{
	TestFixture	fixtures[] = {
		new_TestFixture(NULL,NULL),
		new_TestFixture(NULL,NULL),
		new_TestFixture(NULL,NULL),
		new_TestFixture(NULL,NULL),
		new_TestFixture(NULL,NULL),
	};
	TestCaller caller = new_TestCaller(NULL,NULL,NULL,5,fixtures);
	TestResult result = new_TestResult(NULL);

	caller.isa->run(&caller,&result);

	TEST_ASSERT_EQUAL_INT(5, result.runCount);
	TEST_ASSERT_EQUAL_INT(5, caller.isa->countTestCases(&caller));
}

static void testZeroFixture(void)
{
	TestCaller caller = new_TestCaller(NULL,NULL,NULL,0,NULL);
	TestResult result = new_TestResult(NULL);

	caller.isa->run(&caller,&result);

	TEST_ASSERT_EQUAL_INT(0, result.runCount);
	TEST_ASSERT_EQUAL_INT(0, caller.isa->countTestCases(&caller));
}

TestRef TestCallerTest_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
		new_TestFixture("testOneFixture",testOneFixture),
		new_TestFixture("testMoreThanOne",testMoreThanOne),
		new_TestFixture("testZeroFixture",testZeroFixture),
	};
	EMB_UNIT_TESTCALLER(TestCallerTest,"TestCallerTest",setUp,tearDown,fixtures);

	return (TestRef)&TestCallerTest;
}
