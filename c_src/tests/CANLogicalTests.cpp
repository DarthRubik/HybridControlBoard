extern "C"{
	#include "CanLogical.h"
}
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/CommandLineTestRunner.h"
#include <vector>

extern uint8_t CanRegisters[128];
extern void (*UpdateCanCallback)(CanHw_t*);
TEST_GROUP(CAN)
{
	CanMaster_t master;
	LogicalCan_t can;
	CanHw_t hw;

	std::vector<uint32_t> input_buffer[2];
	CanSubscriber_t subscribers[2] =
	{
		{
			FakeCallBack,
			0x11223344,
			&input_buffer[0]
		},
		{
			FakeCallBack,
			0x44332211,
			&input_buffer[1]
		},
	};
	static void FakeCallBack(void* src,uint32_t* data,uint8_t length)
	{
		std::vector<uint32_t>* ptr = (std::vector<uint32_t>*)src;
		mock().actualCall("FakeCallBack").withPointerParameter("data",src);
		LONGS_EQUAL(ptr->size(),length);
		for (uint8_t x = 0; x < length; x++)
		{
			ptr->push_back(data[x]);
			LONGS_EQUAL((*ptr)[x],data[x]);
		}
	}
	template<typename... T>
	static void SetMessage(uint8_t base,uint32_t id, T... t)
	{
		union
		{
			uint32_t id;
			uint8_t array[4];
		}identifier;
		identifier.id = id;
		for (uint8_t x = 0; x < 4; x++)
		{
			CanRegisters[base+1+x] = identifier.array[x];
		}

		CanRegisters[base+5] = sizeof...(T);
		uint32_t array[] = { (uint32_t)t... };
		for (uint8_t x = 0; x < sizeof...(T); x++)
		{
			CanRegisters[base+6+x] = array[x];
		}
	}
	void setup()
	{
		InitializeCanMaster(&master,&hw,0x30,0);
		InitializeLogicalCan(&can,&hw,subscribers,2);
	}
	void teardown()
	{
		mock().checkExpectations();
		mock().clear();
	}
};

TEST(CAN, SendData)
{
	uint8_t data[] = 
	{
		123,
		156,
		0xaa
	};
	uint8_t before = CanRegisters[0x30];
	TransmitMessage(&master,data,sizeof(data));
	uint8_t difference = (before ^ CanRegisters[0x30]);
	LONGS_EQUAL(1<<3,difference);
	BITS_EQUAL(1<<3,CanRegisters[0x30],1<<3);

	BITS_EQUAL(3,CanRegisters[0x35],0xf);

	for (size_t x = 0; x < sizeof(data); x++)
	{
		LONGS_EQUAL(data[x],CanRegisters[0x36+x]);
	}
}
TEST(CAN, ReceiveChannel0)
{
	CanDelayedReturn_t* ret;
	UpdateLogicalCan(&can);
	ret = (CanDelayedReturn_t*)mock().getData("rx_reg").getPointerValue();
	ret->isReady = 1;
	ret->data = 1<<6; //Say there is a message in RXB0


	SetMessage(0x60,0x11223344,0xAA,0xBB,0xCC);
	this->input_buffer[0] = {0xAA,0xBB,0xCC}; //Set our expectation
	mock().expectOneCall("FakeCallBack").
		withParameter("data",&this->input_buffer[0]);
	for (int x = 0; x < 10; x++)
		UpdateLogicalCan(&can); //Give the CAN a chance to get the data
}
TEST(CAN, ReceiveChannel1)
{
	CanDelayedReturn_t* ret;
	UpdateLogicalCan(&can);
	ret = (CanDelayedReturn_t*)mock().getData("rx_reg").getPointerValue();
	ret->isReady = 1;
	ret->data = 1<<7; //Say there is a message in RXB1


	SetMessage(0x70,0x44332211,0x11,0x22,0x33);
	this->input_buffer[1] = {0x11,0x22,0x33}; //Set our expectation
	mock().expectOneCall("FakeCallBack").
		withParameter("data",&this->input_buffer[1]);
	for (int x = 0; x < 10; x++)
		UpdateLogicalCan(&can); //Give the CAN a chance to get the data
}
TEST(CAN, ReceiveBoth)
{
	CanDelayedReturn_t* ret;
	UpdateLogicalCan(&can);
	ret = (CanDelayedReturn_t*)mock().getData("rx_reg").getPointerValue();
	ret->isReady = 1;
	ret->data = (1<<7)|(1<<6); //Say there is a message in both buffers

	SetMessage(0x70,0x11223344,0x12,0x34,0x56);
	SetMessage(0x60,0x44332211,0xab,0xcd,0xef);
	this->input_buffer[0] = {0x12,0x34,0x56};
	this->input_buffer[1] = {0xab,0xcd,0xef}; //Set our expectation
	mock().expectOneCall("FakeCallBack").
		withParameter("data",&this->input_buffer[0]);
	mock().expectOneCall("FakeCallBack").
		withParameter("data",&this->input_buffer[1]);
	for (int x = 0; x < 10; x++)
		UpdateLogicalCan(&can); //Give the CAN a chance to get the data
}
TEST(CAN, FullSizedMessage)
{
	CanDelayedReturn_t* ret;
	UpdateLogicalCan(&can);
	ret = (CanDelayedReturn_t*)mock().getData("rx_reg").getPointerValue();
	ret->isReady = 1;
	ret->data = (1<<7); //Say there is a message in RXB0

	SetMessage(0x70,0x11223344,0x12,0x34,0x56,0x89,0xab,0xcd,0xef,0x00);
	this->input_buffer[0] = {0x12,0x34,0x56,0x89,0xab,0xcd,0xef,0x00};
	mock().expectOneCall("FakeCallBack").
		withParameter("data",&this->input_buffer[0]);
	for (int x = 0; x < 10; x++)
		UpdateLogicalCan(&can); //Give the CAN a chance to get the data
}
TEST(CAN, EmptyMessage)
{
	CanDelayedReturn_t* ret;
	UpdateLogicalCan(&can);
	ret = (CanDelayedReturn_t*)mock().getData("rx_reg").getPointerValue();
	ret->isReady = 1;
	ret->data = (1<<7); //Say there is a message in RXB0

	SetMessage(0x70,0x11223344);
	this->input_buffer[0] = {};
	mock().expectOneCall("FakeCallBack").
		withParameter("data",&this->input_buffer[0]);
	for (int x = 0; x < 10; x++)
		UpdateLogicalCan(&can); //Give the CAN a chance to get the data
}
TEST(CAN, Idle)
{
	for (int x = 0; x < 10; x++)
	{
		UpdateLogicalCan(&can);
	}
}
TEST(CAN, EnterCanMode)
{
	CanRegisters[0xe] = 0xf0;
	CANYieldFunction = [](CanHw_t* hw)
	{
		static int x = 0;
		switch(x)
		{
			case 3:
				CanRegisters[0xe] = 0;
				CANYieldFunction = 0;
				break;
		}
		x++;
	};
	EnterCanMode(&can,CAN_NORMAL_MODE);
}

int main(int ac, char** av)
{
        return CommandLineTestRunner::RunAllTests(ac, av);
}

