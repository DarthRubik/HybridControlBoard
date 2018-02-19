/*

The spi controller is the piece of software that controls the SPI hardware.

This spi controller is designed so that it should work with any
hardware.  To change the implementation details of the hardware
you need to link this with a module that implements the SpiHardware.h
interface.

*/



#ifndef SPI_CONTROLLER_H
#define SPI_CONTROLLER_H
#include "stdint.h"
#include "SpiHardware.h"
#include "Queue.h"


//This struct represents a message that is sent on the SPI
//The data pointer points to your data.  The data will be replaced
//with the incoming data because of how SPI works
//Note: the data and message length variables do need to be volatile
//because they are going to be modified in an interrupt
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

//Call this function to queue a SPI message to be sent at the SPI's
//earliest convinience
void QueueSpiMessage(SpiController_t* spi,SpiMessage_t* message);  

//This should be called from the ISR when the spi sent data and should be
//passed the data that the SPI got during the transaction
void SpiChunkSet(SpiController_t* spi,uint32_t data);
#endif
