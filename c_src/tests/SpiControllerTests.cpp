extern "C"{
	#include "SpiController.h"
}
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

TEST_GROUP(SpiController)
{

	SpiController_t spi;
	SpiMessage_t message;
	void setup(){
	}
	void teardown() {}
};


IGNORE_TEST(SpiController, VerifySender)
{
		
}
IGNORE_TEST(SpiController, VerifyShortMessage)
{
}
IGNORE_TEST(SpiController, VerifyLongMessage)
{
}
IGNORE_TEST(SpiController, QueueMultipleMessages)
{
}
IGNORE_TEST(SpiController, QueueMaxMessages)
{
}
IGNORE_TEST(SpiController, RunWithEmptyMessageQueue)
{
}
