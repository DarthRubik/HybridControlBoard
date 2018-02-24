#include "Adc.h"
#include "Error.h"

void InitializeAdc(Adc_t* adc, SpiController_t* spi, SpiId_t id,AdcChannelMask_t channelMask){
	adc->spi = spi;
	adc->currentSpiMessage.id = id;
	adc->currentSpiMessage.messageLength = 0;
	adc->initializationIndex = 0;
	adc->channelMask = channelMask;
	for (AdcChannel_t x = 0; x < ADC_CHANNELS; x++)
	{
		adc->readings[x] = 0;
	}
	

}

#define ADC_INIT_LENGTH 2
static const uint16_t adcInitializaitionArray[ADC_INIT_LENGTH] = 
{
	0x2840,
	0x8000
};
// Calling this function will get the reading for the specified channel
AdcResult_t GetAdcReading(Adc_t* adc, AdcChannel_t channel){
	if (channel >= ADC_CHANNELS)
	{
		DeclareError("Out of range ADC access");
		return 0;
	}
	return adc->readings[channel];
}

static void SendAdcMessage(Adc_t* adc, uint16_t data)
{

	adc->currentSpiMessage.data = adc->buffer;
	adc->currentSpiMessage.messageLength = 1;
	adc->buffer[0] = data;
	QueueSpiMessage(adc->spi, &adc->currentSpiMessage);
}
// This function should be called in the main loop
void UpdateAdc(Adc_t* adc){
	//Wait for our message to have sent already
	if (adc->currentSpiMessage.messageLength == 0)
	{
		if (adc->initializationIndex < ADC_INIT_LENGTH)
		{
			SendAdcMessage(adc,
				adcInitializaitionArray[adc->initializationIndex]);
			adc->initializationIndex++;
		}
		else if (adc->initializationIndex == ADC_INIT_LENGTH)
		{
			SendAdcMessage(adc,adc->channelMask);
			adc->initializationIndex++;
		}
		else
		{

			AdcChannel_t channel = (adc->buffer[0] & ADC_CHANNEL_MASK) >> ADC_CHANNEL_OFFSET;
			AdcResult_t result = (adc->buffer[0] & ADC_DATA_MASK) >> ADC_DATA_OFFSET;
			adc->readings[channel] = result;
			SendAdcMessage(adc,0);
		}
	}
}

