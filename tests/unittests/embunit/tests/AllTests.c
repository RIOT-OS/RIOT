#include <embUnit/embUnit.h>

extern TestRef assertTest_tests(void);
extern TestRef stdImplTest_tests(void);
extern TestRef TestCaseTest_tests(void);
extern TestRef TestCallerTest_tests(void);
extern TestRef TestResultTest_tests(void);
extern TestRef RepeatedTestTest_tests(void);

int main (int argc, const char* argv[])
{
	TestRunner_start();
		TestRunner_runTest(assertTest_tests());
		TestRunner_runTest(stdImplTest_tests());
		TestRunner_runTest(TestCaseTest_tests());
		TestRunner_runTest(TestCallerTest_tests());
		TestRunner_runTest(TestResultTest_tests());
		TestRunner_runTest(RepeatedTestTest_tests());
	TestRunner_end();
	return 0;
}
