#include "Error.h"
#include "CppUTestExt/MockSupport_c.h"


void DeclareError(const char* message)
{
	mock_c()->actualCall("DeclareError");
}
