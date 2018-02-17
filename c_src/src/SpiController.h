
#ifndef SPI_CONTROLLER_H
#define SPI_CONTROLLER_H
#include "stdint.h"
#include "SpiHardware.h"

typedef struct{
	volatile uint32_t* data;
	volatile uint8_t messageLength;
} Message_t;

typedef struct{
	SpiId_t id;
	Message_t message;
} SpiMessage_t;

typedef struct {
} SpiController_t;

void QueueMessage(SpiController_t* spi,SpiId_t id, SpiMessage_t* message);  
void ChunkSet(SpiController_t* spi);
#endif
