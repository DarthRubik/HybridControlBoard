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
#include <cstring>
#include <stddef.h>

#define container_of(ptr, type, member) ((type *)((char *)(1 ? (ptr) : &((type *)0)->member) - offsetof(type, member)))


SpiMessage_t* lastMessage;
TEST_GROUP(CAN)
{
	SpiController_t spi;
	CanHw_t hw;

	void setup()
	{
		std::memcpy(&hw,(void*)std::memcpy,sizeof(hw));
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
	SpiMessage_t* ExpectPartialSpiMessage(size_t length,T... data)
	{
		uint32_t array[] = { (uint32_t)data ... };
		SpiMessage_t* message = (SpiMessage_t*)mock().getData("messagePointer")
			.getPointerValue();
		LONGS_EQUAL(sizeof...(data),message->messageLength);
		for (size_t x = 0; x < length; x++)
		{
			LONGS_EQUAL(array[x],message->data[x]);
		}
		LONGS_EQUAL(8,message->id);
		mock().checkExpectations();
		lastMessage = message;
		return message;
	}
	template<typename... T>
	SpiMessage_t* ExpectSpiMessage(T... data)
	{
		ExpectPartialSpiMessage(sizeof...(data),data...);
	}
};


TEST(CAN,TestWriteRegister)
{
	PreExpectSpiMessage();
	CANWriteRegister(&hw,0xaa,123);
	ExpectSpiMessage(1<<6,0xAA,123)->messageLength = 0;
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
TEST(CAN,ReadRange)
{
	CAN_RANGE_RETURN_MAKE(6,local_data);
	local_data data;
	PreExpectSpiMessage();
	CANReadRange(&hw,0xAA,(CanRangeReturn_0*)&data,4);
	LONGS_EQUAL(0,data.isReady);
	SpiMessage_t* msg = ExpectPartialSpiMessage(2,(1<<6)|(1<<7),0xAA,0,0,0,0);
	msg->messageLength = 0;

	msg->data[2] = 0x42;
	msg->data[3] = 0x52;
	msg->data[4] = 0x62;
	msg->data[5] = 0x72;

	UpdateCAN(&hw);
	LONGS_EQUAL(1,data.isReady);
	LONGS_EQUAL(0x42,data.data[data.offset+0]);
	LONGS_EQUAL(0x52,data.data[data.offset+1]);
	LONGS_EQUAL(0x62,data.data[data.offset+2]);
	LONGS_EQUAL(0x72,data.data[data.offset+3]);
}
IGNORE_TEST(CAN,DontCareAboutReturn)
{
}
IGNORE_TEST(CAN,ChecksMaxMessageLength)
{
}
TEST(CAN,NotBusyInitialy)
{
	CHECK_FALSE(CANIsBusy(&hw));
}
TEST(CAN,BusyAfterWrite)
{
	PreExpectSpiMessage();
	CANWriteRegister(&hw,0xaa,123);
	CHECK_TRUE(CANIsBusy(&hw));
	ExpectSpiMessage(1<<6,0xAA,123)->messageLength = 0;
	CHECK_FALSE(CANIsBusy(&hw));
}
#define _this(x) container_of(x,TEST_GROUP_CppUTestGroupCAN,hw)

TEST(CAN,BlockingRead)
{
	CANYieldFunction = [](CanHw_t* hw)
	{
		static void (*steps[])(CanHw_t*) =
		{
			[](CanHw_t* hw) {
				_this(hw)->ExpectSpiMessage((1<<6)|(1<<7),0xAA,0);
			},
			[](CanHw_t* hw) {
			},
			[](CanHw_t* hw) {
			},
			[](CanHw_t* hw) {
				lastMessage->data[2] = 0x12;
				lastMessage->messageLength = 0;
			},
		};
		static size_t index = 0;
		steps[index](hw);
		index++;
		if (index >= sizeof(steps)/sizeof(steps[0]))
		{
			CANYieldFunction = 0;
		}
	};
	PreExpectSpiMessage();
	uint8_t data = CANReadRegisterBlocking(&hw,0xAA);
	CANYieldFunction = 0;
	LONGS_EQUAL(0x12,data);
}

int main(int ac, char** av)
{
        return CommandLineTestRunner::RunAllTests(ac, av);
}

