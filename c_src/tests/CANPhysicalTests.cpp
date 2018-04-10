extern "C"{
	#include "SpiController.h"
	#include "MockSpiController.h"
	#include "CanPhysical.h"
}
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/CommandLineTestRunner.h"
#include <vector>
#include <algorithm>


TEST_GROUP(CAN)
{
	SpiController_t spi;
	CanHw_t hw;

	void setup()
	{
		CANInitialize(&hw,&spi,8);
	}
	void teardown(){
		mock().clear();
	}

	void PreExpectSpiMessage()
	{
		mock().expectOneCall("QueueSpiMessage")
			.withPointerParameter("spi",&spi)
			.ignoreOtherParameters();
	}
	template<typename... T>
	SpiMessage_t* ExpectSpiMessage(T... data)
	{
		uint32_t array[] = { (uint32_t)data ... };
		SpiMessage_t* message = (SpiMessage_t*)mock().getData("messagePointer")
			.getPointerValue();
		LONGS_EQUAL(sizeof...(data),message->messageLength);
		for (size_t x = 0; x < sizeof...(data); x++)
		{
			LONGS_EQUAL(array[x],message->data[x]);
		}
		LONGS_EQUAL(8,message->id);
		mock().checkExpectations();
		return message;
	}
};


TEST(CAN,TestWriteRegister)
{
	PreExpectSpiMessage();
	CANWriteRegister(&hw,0xaa,123);
	ExpectSpiMessage(1<<6,0xAA,123);
}
TEST(CAN,TestUniquePointers)
{
	std::vector<SpiMessage_t*> ptrs;
	for (int x = 0; x < 10; x++)
	{
		PreExpectSpiMessage();
		CANWriteRegister(&hw,0xaa,123);
		ptrs.push_back(ExpectSpiMessage(1<<6,0xAA,123));
	}
	for (auto x : ptrs)
	{
		LONGS_EQUAL(1,std::count_if(ptrs.begin(),ptrs.end(), 
					[&](auto y){ return y == x;}
				));
	}
	for (auto x : ptrs)
	{
		LONGS_EQUAL(1,std::count_if(ptrs.begin(),ptrs.end(), 
					[&](auto y){ return y->data == x->data;}
				));
	}
}
TEST(CAN,TestOverflowOfQueue)
{
	std::vector<SpiMessage_t*> ptrs;
	for (int x = 0; x < 10; x++)
	{
		PreExpectSpiMessage();
		CANWriteRegister(&hw,0xaa,123);
		ptrs.push_back(ExpectSpiMessage(1<<6,0xAA,123));
	}
	ptrs[0]->messageLength = 0; //Tell the CAN that we are done with this message
	ptrs[1]->messageLength = 0; 
	ptrs[2]->messageLength = 0; 
	UpdateCAN(&hw);
	for (int x = 0; x < 3; x++)
	{
		PreExpectSpiMessage();
		CANWriteRegister(&hw,0xaa,123);
		ptrs.push_back(ExpectSpiMessage(1<<6,0xAA,123));
	}
}
IGNORE_TEST(CAN,ThrowErrorOnOverflow)
{
}
TEST(CAN,ReadRegister)
{
	CanDelayedReturn_t data;
	PreExpectSpiMessage();
	CANReadRegister(&hw,0xAA,&data);
	LONGS_EQUAL(0,data.isReady);
	SpiMessage_t* msg = ExpectSpiMessage((1<<6)|(1<<7),0xAA,0);
	msg->messageLength = 0;
	msg->data[2] = 0x42;
	UpdateCAN(&hw);
	LONGS_EQUAL(1,data.isReady);
	LONGS_EQUAL(0x42,data.data);
}
IGNORE_TEST(CAN,DontCareAboutReturn)
{
}
IGNORE_TEST(CAN,ChecksMaxMessageLength)
{
}
int main(int ac, char** av)
{
        return CommandLineTestRunner::RunAllTests(ac, av);
}

