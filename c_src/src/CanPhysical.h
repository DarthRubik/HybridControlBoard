#ifndef CAN_PHYSICAL_H
#define CAN_PHYSICAL_H
#include "stdint.h"
#include "SpiHardware.h"
#include "SpiController.h"

typedef struct
{
	uint8_t data;
	uint8_t isReady:1;
	uint8_t offset:5;
}CanDelayedReturn_t;

typedef struct
{
	uint32_t buffer[1];
	SpiMessage_t spiMessage_t;
	CanDelayedReturn_t* ret;
}CanMessage_t;

typedef struct
{
	CanMessage_t messages[10];
	size_t messageIndex;

	//Different IDs for each message length
	SpiId_t msg8;
	SpiId_t msg16;
	SpiId_t msg24;
	SpiId_t msg32;
	SpiController_t* spi;

}CanHw_t;

void CANInitialize(CanHw_t* hw,SpiController_t* spi,
	SpiId_t msg8,
	SpiId_t msg16,
	SpiId_t msg24,
	SpiId_t msg32);
void UpdateCAN(CanHw_t* hw);

//General purpose r/w commands
void CANReadRegister(CanHw_t* hw, uint8_t add, CanDelayedReturn_t* data);
void CANWriteRegister(CanHw_t* hw, uint8_t add, uint8_t data);

//r/w commands for the Tx and Rx registers for fast(er) access
void CANReadRxRegister(CanHw_t* hw,uint8_t index,CanDelayedReturn_t* data);
void CanWriteTxRegister(CanHw_t* hw,uint8_t index, uint8_t data);

void CANRequestToSend(CanHw_t* hw,uint8_t index);

//Only certain registers can use this
void CANMutateRegisterBits(CanHw_t* hw,uint8_t address, uint8_t mask, uint8_t data);

void CANReadStatusRegister(CanHw_t* hw, CanDelayedReturn_t* data);
void CANReadRxStatusRegister(CanHw_t* hw, CanDelayedReturn_t* data);

#endif
