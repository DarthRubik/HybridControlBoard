extern "C"{
	#include "Queue.h"
}
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

TEST_GROUP(Queue)
{
	static const size_t queueSize = 10;
	Queue_t queue;
	void* data[queueSize];
	void setup(){
		InitializeQueue(&queue, data, queueSize);
	}
	void teardown() 
	{
		mock().clear();
	}
};


TEST(Queue, CheckOneOperation)
{
	void* data = (void*)123;
	AddToQueue(&queue,data);
	
	CHECK_TRUE(DoesQueueHaveStuff(&queue));
	
	data = GetFromQueue(&queue);

	POINTERS_EQUAL((void*)123,data);
	
	CHECK_FALSE(DoesQueueHaveStuff(&queue));
}

TEST(Queue, CheckTwoOperations)
{
	void* data;


	AddToQueue(&queue, (void*)0xDEAD);
	AddToQueue(&queue, (void*)0xBEEF);
	
	CHECK_TRUE(DoesQueueHaveStuff(&queue));
	POINTERS_EQUAL((void*)0xDEAD, GetFromQueue(&queue));
	POINTERS_EQUAL((void*)0xBEEF, GetFromQueue(&queue));
	CHECK_FALSE(DoesQueueHaveStuff(&queue));
}

TEST(Queue, CheckWrapAround)
{
	for (int x = 0; x < queueSize*2; x++)
	{
		AddToQueue(&queue, (void*)(x+30));
		CHECK_TRUE(DoesQueueHaveStuff(&queue));
		POINTERS_EQUAL((void*)(x+30), GetFromQueue(&queue));
		CHECK_FALSE(DoesQueueHaveStuff(&queue));
	}
}

TEST(Queue, CheckOverflow)
{
	mock().expectOneCall("DeclareError");
	for (int x = 0; x < queueSize + 1; x++)
	{
		AddToQueue(&queue, (void*)(x+42));
		CHECK_TRUE(DoesQueueHaveStuff(&queue));
	}
	for (int x = 0; x < queueSize; x++)
	{
		void* ActualData = (void*)(x+42);
		void* gottenData = GetFromQueue(&queue);
		POINTERS_EQUAL(ActualData,gottenData);
	}
	mock().checkExpectations();
}
TEST(Queue, CheckUnderflow)
{
	mock().expectOneCall("DeclareError");
	AddToQueue(&queue, (void*)42);
	GetFromQueue(&queue);
	GetFromQueue(&queue);
	CHECK_FALSE(DoesQueueHaveStuff(&queue));
	mock().checkExpectations();
}
TEST(Queue, CheckFullRange)
{
	
	for (int x = 0; x < queueSize; x++)
	{
		AddToQueue(&queue, (void*)(x+100));
		CHECK_TRUE(DoesQueueHaveStuff(&queue));
	}
	for (int x = 0; x < queueSize; x++)
	{
		POINTERS_EQUAL((void*)(x+100), GetFromQueue(&queue));
	}
	CHECK_FALSE(DoesQueueHaveStuff(&queue));
}
TEST(Queue, CheckOverflowWithWrap)
{

	AddToQueue(&queue, (void*)123);
	AddToQueue(&queue, (void*)123);
	AddToQueue(&queue, (void*)123);
	
	GetFromQueue(&queue);
	GetFromQueue(&queue);
	GetFromQueue(&queue);

	for (int x = 0; x < queueSize; x++)
	{
		AddToQueue(&queue, (void*)(x+100));
		CHECK_TRUE(DoesQueueHaveStuff(&queue));
	}
	for (int x = 0; x < queueSize; x++)
	{
		POINTERS_EQUAL((void*)(x+100), GetFromQueue(&queue));
	}
	CHECK_FALSE(DoesQueueHaveStuff(&queue));
}

