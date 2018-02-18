#include "stdint.h"
typedef struct{
	volatile uint32_t rxdata;
	volatile uint32_t txdata;
	volatile uint32_t status;
	volatile uint32_t control;
	volatile uint32_t reserved;
	volatile uint32_t slaveselect;
	volatile uint32_t eop_value;
} __attribute__((packed)) SpiHw_t;

typedef struct
{
	volatile uint32_t data;
	volatile uint32_t direction;
	volatile uint32_t interruptMask;
	volatile uint32_t edgeCapture;
} __attribute__((packed)) Pio_t;

#define CAN_SPI_PORT (*((SpiHw_t*)0xff200220))
#define LS_SW_PORT (((Pio_t*)0xff200120)->data)
#define DO_SPI_TEST




int main(void)
{

	
	#ifdef DO_SPI_TEST
	while (1)
	{
		CAN_SPI_PORT.txdata = 0xAA;
		
		while (!(CAN_SPI_PORT.status & (1<<6)));
	}
	#else
	while (1)
	{
		LS_SW_PORT ^= 1;
		
		volatile uint64_t x = 0;
		for (x = 0; x < 4000000; x++)
		{
			
		}
		
		
	}
	#endif
}