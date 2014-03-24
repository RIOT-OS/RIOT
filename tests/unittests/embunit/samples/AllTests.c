#include <embUnit/embUnit.h>

TestRef CounterTest_tests(void);
TestRef PersonTest_tests(void);

int main (int argc, const char* argv[])
{
	TestRunner_start();
		TestRunner_runTest(CounterTest_tests());
		TestRunner_runTest(PersonTest_tests());
	TestRunner_end();
	return 0;
}
