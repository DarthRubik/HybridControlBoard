
#ifndef SPI_CONTROLLER_H
#define SPI_CONTROLLER_H
#include "stdint.h"
#include "SpiHardware.h"
#include "Queue.h"

typedef struct{
	volatile uint32_t* data;
	volatile uint8_t messageLength;
	SpiId_t id;
} SpiMessage_t;


typedef struct {
	SpiMessage_t* message;
	SpiPackage_t* package;
	Queue_t messageQueue;
	bool messageRunning;
} SpiController_t;

void InitializeSpiController(SpiController_t* spi, SpiPackage_t* package,void** buffer,size_t length);
void QueueSpiMessage(SpiController_t* spi,SpiMessage_t* message);  
void SpiChunkSet(SpiController_t* spi,uint32_t data);
#endif
