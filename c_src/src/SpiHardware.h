#ifndef SPI_HARDWARE_H
#define SPI_HARDWARE_H
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


typedef uint8_t SpiId_t;

typedef struct
{
        volatile uint32_t data;
        volatile uint32_t direction;
        volatile uint32_t interruptMask;
        volatile uint32_t edgeCapture;
} __attribute__((packed)) Pio_t;


typedef struct{
	SpiHw_t* hardware;
	uint32_t activeMux;
	uint8_t slaveselect;
} SpiTarget_t;

typedef struct 
{
	Pio_t* mux;
	SpiTarget_t* targets;
	SpiHw_t* activeHw;
	SpiId_t count;
} SpiPackage_t;

void InitializeSpi(SpiPackage_t* spi, SpiTarget_t* hws, Pio_t* Active, SpiId_t count);
void StartSpiMessage(SpiPackage_t* spi, SpiId_t id);
void WriteWordToSpi(SpiPackage_t* spi, uint32_t word);
void EndSpiMessage(SpiPackage_t* spi);

#endif


