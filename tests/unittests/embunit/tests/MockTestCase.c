#include <embUnit/embUnit.h>
#include "MockTestCase.h"

static void runTest(void)
{
}

TestCaseRef MockTestCase_case(void)
{
	EMB_UNIT_TESTCASE(MockTestCase,"MockTestCase",NULL,NULL,runTest);
	return (TestCaseRef)&MockTestCase;
}
