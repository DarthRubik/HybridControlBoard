#include "src/SpiController.h"
#include "src/Adc.h"
#include "stdio.h"

#define CAN_SPI_PORT (((SpiHw_t*)0xff200220))
#define ADC_SPI_PORT (((SpiHw_t*)0xff200240))
#define SPI_SELECT      ((Pio_t*)0xff200200)

#define LS_SW_PORT (((Pio_t*)0xff200120))

#ifdef USE_INTERRUPTS
#define INTERRUPT __attribute__((interrupt))
#else
#define INTERRUPT
#endif

static SpiTarget_t spiTargets[] =
{
	{
		CAN_SPI_PORT,
		0,
		0
	},
	{
		CAN_SPI_PORT,
		0,
		1,
	},
	{
		CAN_SPI_PORT,
		0,
		2,
	},
	{
		ADC_SPI_PORT,
		1,
		0,
	},
	{
		ADC_SPI_PORT,
		1,
		1,
	},
};

int main(void)
{
	/*
	CAN_SPI_PORT->txdata = 0xaa;
	while(1)
	{
		if (CAN_SPI_PORT->status & (1<<7))
		{
			CAN_SPI_PORT->txdata = 0xaa;
		}
	}*/
	
	
	SpiPackage_t spipackage;
	SpiController_t spi;
	void* spiBuffer[10];
	InitializeSpi(&spipackage,spiTargets,SPI_SELECT,
			sizeof(spiTargets)/sizeof(spiTargets[0]));
	InitializeSpiController(&spi,&spipackage,spiBuffer,10);
	Adc_t adc;
	InitializeAdc(&adc,&spi,3,1<<1);

	while (1){

		UpdateAdc(&adc);
		printf("Reading: %i\n", GetAdcReading(&adc,1));
		if (ADC_SPI_PORT->status & (1<<7))
		{
			ADC_SPI_PORT->status &= ~(1<<7);
			uint64_t data = ADC_SPI_PORT->rxdata;
			SpiChunkSet(&spi,data);	
		}
	}

}

void INTERRUPT __cs3_isr_dabort(void)
{
	int x = 1;
	while (x){}
	return;
}
