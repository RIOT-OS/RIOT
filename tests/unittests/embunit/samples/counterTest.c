#include <embUnit/embUnit.h>
#include "counter.h"

CounterRef counterRef;

static void setUp(void)
{
	counterRef = Counter_counter();
}

static void tearDown(void)
{
	Counter_dealloc(counterRef);
}

static void testInit(void)
{
	TEST_ASSERT_EQUAL_INT(0, Counter_value(counterRef));
}

static void testSetValue(void)
{
	Counter_setValue(counterRef,1);
	TEST_ASSERT_EQUAL_INT(1, Counter_value(counterRef));

	Counter_setValue(counterRef,-1);
	TEST_ASSERT_EQUAL_INT(-1, Counter_value(counterRef));
}

static void testInc(void)
{
	Counter_inc(counterRef);
	TEST_ASSERT_EQUAL_INT(1, Counter_value(counterRef));

	Counter_inc(counterRef);
	TEST_ASSERT_EQUAL_INT(2, Counter_value(counterRef));
}

static void testDec(void)
{
	Counter_dec(counterRef);
	TEST_ASSERT_EQUAL_INT(-1, Counter_value(counterRef));

	Counter_dec(counterRef);
	TEST_ASSERT_EQUAL_INT(-2, Counter_value(counterRef));
}

static void testClr(void)
{
	Counter_inc(counterRef);
	TEST_ASSERT_EQUAL_INT(1, Counter_value(counterRef));

	Counter_clr(counterRef);
	TEST_ASSERT_EQUAL_INT(0, Counter_value(counterRef));
}

TestRef CounterTest_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
		new_TestFixture("testInit",testInit),
		new_TestFixture("testSetValue",testSetValue),
		new_TestFixture("testInc",testInc),
		new_TestFixture("testDec",testDec),
		new_TestFixture("testClr",testClr),
	};
	EMB_UNIT_TESTCALLER(CounterTest,"CounterTest",setUp,tearDown,fixtures);

	return (TestRef)&CounterTest;
}
