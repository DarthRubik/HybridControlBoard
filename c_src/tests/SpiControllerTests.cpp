extern "C"{
	#include "SpiController.h"
}
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/CommandLineTestRunner.h"

TEST_GROUP(SpiController)
{

	SpiController_t spi;
	SpiPackage_t package;
	SpiMessage_t message;
	static const size_t bufferSize = 10;
	void* buffer[bufferSize];
	void setup(){
		InitializeSpiController(&spi,&package,buffer,bufferSize);
		mock().strictOrder();
	}
	void teardown() {
		mock().clear();
	}
};


TEST(SpiController, VerifyShortMessage)
{
	mock().expectOneCall("StartSpiMessage").withParameter("spi",&package).withParameter("id",0);
	mock().expectOneCall("WriteWordToSpi").withParameter("spi",&package).withParameter("word",42);
	mock().expectOneCall("EndSpiMessage").withParameter("spi",&package);

	uint32_t data[] = { 42 };
	message.data = data;
	message.messageLength = 1;
	message.id = 0;
	QueueSpiMessage(&spi, &message);


	SpiChunkSet(&spi,23);
	LONGS_EQUAL(23,data[0]);
	mock().checkExpectations();

}
TEST(SpiController, VerifyLongMessage)
{
	mock().expectOneCall("StartSpiMessage").withParameter("spi",&package).withParameter("id",0);
	for (int x = 0; x < 10; x++)
		mock().expectOneCall("WriteWordToSpi").withParameter("spi",&package).withParameter("word",x+22);

	mock().expectOneCall("EndSpiMessage").withParameter("spi",&package);

		
	uint32_t data[] = { 22,23,24,25,26,27,28,29,30,31};
	message.data = data;
	message.messageLength = 10;
	message.id = 0;

	QueueSpiMessage(&spi,&message);

	for (int x = 0; x < 10; x++)
	{
		SpiChunkSet(&spi,x);
		LONGS_EQUAL(x,data[x]);
	}
	mock().checkExpectations();
}
TEST(SpiController, QueueMultipleMessages)
{
	uint32_t data[] = { 0x10,0x20,0x30 };
	SpiMessage_t messages[3];
	for (int x = 0; x < 3; x++)
	{
		messages[x].data = &data[x];
		messages[x].messageLength = 1;
		messages[x].id = x;
		mock().expectOneCall("StartSpiMessage").withParameter("spi",&package).withParameter("id",x);
		mock().expectOneCall("WriteWordToSpi").withParameter("spi",&package).withParameter("word",data[x]);
		mock().expectOneCall("EndSpiMessage").withParameter("spi",&package);
	}
	QueueSpiMessage(&spi,&messages[0]);
	QueueSpiMessage(&spi,&messages[1]);
	QueueSpiMessage(&spi,&messages[2]);

	for (int x = 0; x < 3; x++)
	{
		SpiChunkSet(&spi,x+100);
		LONGS_EQUAL(x+100,data[x]);

	}
	mock().checkExpectations();
}
TEST(SpiController, QueueMaxMessages)
{
	uint32_t data[bufferSize+1];
	SpiMessage_t messages[bufferSize+1];
	for (size_t x = 0; x < bufferSize+1; x++)
	{
		data[x] = x;
		messages[x].data = &data[x];
		messages[x].messageLength = 1;
		messages[x].id = x;
		mock().expectOneCall("StartSpiMessage").withParameter("spi",&package).withParameter("id",x);
		mock().expectOneCall("WriteWordToSpi").withParameter("spi",&package).withParameter("word",data[x]);
		mock().expectOneCall("EndSpiMessage").withParameter("spi",&package);
	}
	for (size_t x = 0; x < bufferSize+1; x++)
	{
		QueueSpiMessage(&spi,&messages[x]);
	}
	for (size_t x = 0; x < bufferSize+1; x++)
	{
		SpiChunkSet(&spi,x);
		LONGS_EQUAL(x,data[x]);
	}
	mock().checkExpectations();
}
TEST(SpiController, OverflowQueue)
{

	uint32_t data[bufferSize+2];
	SpiMessage_t messages[bufferSize+2];
	

	for (size_t x = 0; x < bufferSize+2; x++)
	{
		data[x] = x;
		messages[x].data = &data[x];
		messages[x].messageLength = 1;
		messages[x].id = x;
		if (x != bufferSize + 1)
		{
			mock().expectOneCall("StartSpiMessage").withParameter("spi",&package).withParameter("id",x);
			mock().expectOneCall("WriteWordToSpi").withParameter("spi",&package).withParameter("word",data[x]);
			if (x == 0) mock().expectOneCall("DeclareError");
			mock().expectOneCall("EndSpiMessage").withParameter("spi",&package);
		}
	}
	for (size_t x = 0; x < bufferSize+2; x++)
	{
		QueueSpiMessage(&spi,&messages[x]);
	}
	for (size_t x = 0; x < bufferSize+1; x++)
	{
		SpiChunkSet(&spi,x);
		LONGS_EQUAL(x,data[x]);
	}
	mock().checkExpectations();
}
TEST(SpiController, GetInterruptWithWithNoSend)
{
	SpiChunkSet(&spi,123);
}
IGNORE_TEST(SpiController, SendTimeout)
{
	
}

int main(int ac, char** av)
{
	return CommandLineTestRunner::RunAllTests(ac, av);
}
