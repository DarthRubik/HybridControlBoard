#ifndef AdcConverter_h
#define AdcConverter_h
#include "stdint.h"
#include "SpiController.h"


#define ADC_CHANNEL_MASK 0xf000
#define ADC_CHANNEL_OFFSET 12

#define ADC_DATA_MASK 0x0fff
#define ADC_DATA_OFFSET 0

#define ADC_CHANNELS 16

typedef uint16_t AdcResult_t;
typedef uint8_t AdcChannel_t;
typedef uint16_t AdcChannelMask_t;

typedef struct{
	uint32_t buffer[1];
	AdcResult_t readings[16];
	SpiMessage_t currentSpiMessage;
	SpiId_t id;
	uint8_t initializationIndex;
	AdcChannelMask_t channelMask;
	SpiController_t* spi;
}Adc_t;


void InitializeAdc(Adc_t* adc,SpiController_t* spi, SpiId_t spiID, AdcChannelMask_t enabledChannels);

// Calling this function will get the reading for the specified channel
AdcResult_t GetAdcReading(Adc_t* adc, AdcChannel_t channel);

// This function should be called in the main loop
void UpdateAdc(Adc_t* adc);

#endif
