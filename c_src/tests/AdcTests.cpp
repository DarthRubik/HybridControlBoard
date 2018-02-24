extern "C"{
	#include "SpiController.h"
	#include "MockSpiController.h"
	#include "Adc.h"
}
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/CommandLineTestRunner.h"
#include "string.h"


static const SpiId_t SpiId = 12;
TEST_GROUP(Adc)
{
	static const size_t AdcChannels = ADC_CHANNELS;
	Adc_t adc;
	SpiController_t spi;
	void setup()
	{
		memcpy(&adc, (void*)memcpy, sizeof(adc));	
		mock().disable();
		InitializeAdc(&adc,&spi,SpiId,123);
		for (int x = 0; x < 3; x++)
		{
			UpdateAdc(&adc);
			adc.currentSpiMessage.messageLength = 0;
		}
		mock().enable();
	}
	void teardown(){
		mock().clear();
	}
};

TEST(Adc,InitiallyReturnsZero)
{	
	for (size_t x = 0; x < AdcChannels; x++)
	{
		LONGS_EQUAL(0,GetAdcReading(&adc,x));
	}
}
TEST(Adc,OutOfRangeThrowsError)
{
	mock().expectOneCall("DeclareError");
	GetAdcReading(&adc,AdcChannels);
	mock().checkExpectations();
}
static void CheckSentMessage(Adc_t* adc, uint32_t data)
{
	LONGS_EQUAL(1,adc->currentSpiMessage.messageLength);
	POINTERS_EQUAL(adc->buffer,adc->currentSpiMessage.data);
	LONGS_EQUAL(SpiId,adc->currentSpiMessage.id);
	LONGS_EQUAL(data,adc->buffer[0]); //We should send the ADC chip all zeros
}
TEST(Adc,QueuesSpiMessageWhenComplete)
{
	mock().expectOneCall("QueueSpiMessage").withPointerParameter("spi",&spi).withPointerParameter("message",&adc.currentSpiMessage);
	adc.currentSpiMessage.messageLength = 0; //When this is zero the message is sent

	adc.buffer[0] = 123;  //Simulate data in the buffer already
	UpdateAdc(&adc);
	CheckSentMessage(&adc,0);

		
	mock().checkExpectations();
}
TEST(Adc,DoesNotQueueSpiMessageWhenNotComplete)
{
	adc.currentSpiMessage.messageLength = 1;
	UpdateAdc(&adc);
	mock().checkExpectations();
}
static void CheckReading(Adc_t* adc, AdcResult_t desiredReading,AdcChannel_t channel){
	mock().disable();
	adc->currentSpiMessage.messageLength = 0;
	adc->buffer[0] = (channel<<ADC_CHANNEL_OFFSET) | desiredReading;
	UpdateAdc(adc);
	AdcResult_t reading = GetAdcReading(adc,channel);
	LONGS_EQUAL(desiredReading,reading);
	mock().enable();
}
TEST(Adc,ReceivingAMessageUpdatesAdcReading){
	CheckReading(&adc,123,1);
}
TEST(Adc,UpperRangeOfAdc)
{
	CheckReading(&adc,0xfff,2);
}
TEST(Adc,LowerRangeOfAdc)
{
	CheckReading(&adc,0,3);
}
TEST(Adc,IntializesProperly)
{
	mock().disable();
	InitializeAdc(&adc,&spi,12,321);

	UpdateAdc(&adc);
	CheckSentMessage(&adc,0x2840);
	adc.currentSpiMessage.messageLength = 0;
	UpdateAdc(&adc);
	CheckSentMessage(&adc,0x8000);
	adc.currentSpiMessage.messageLength = 0;
	UpdateAdc(&adc);
	CheckSentMessage(&adc,321);
	mock().enable();
}


int main(int ac, char** av)
{
        return CommandLineTestRunner::RunAllTests(ac, av);
}

