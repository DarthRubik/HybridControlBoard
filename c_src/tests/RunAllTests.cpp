#include "CppUTest/CommandLineTestRunner.h"


IMPORT_TEST_GROUP(SpiController);
IMPORT_TEST_GROUP(Queue);


int main(int ac, char** av)
{
	return CommandLineTestRunner::RunAllTests(ac, av);
}
