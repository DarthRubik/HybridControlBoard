#ifndef CAN_PHYSICAL_H
#define CAN_PHYSICAL_H
#include "stdint.h"
#include "SpiHardware.h"
#include "SpiController.h"

typedef uint8_t CanMessageLength_t;
typedef uint8_t CanOutputChannelIndex_t;
typedef uint8_t CanInputChannelIndex_t;
typedef uint32_t CanIdentifier_t;
typedef uint8_t CanChipOffset_t;

typedef struct
{
	uint8_t isReady:1;
	CanMessageLength_t offset;
	uint8_t data;
}CanDelayedReturn_t;

#define CAN_RANGE_RETURN_MAKE(x,name) \
typedef struct\
{\
	uint8_t isReady:1;\
	CanMessageLength_t offset;\
	uint32_t data[x];\
}name


CAN_RANGE_RETURN_MAKE(0,CanRangeReturn_0);

typedef struct
{
	uint8_t isRange:1;
	uint8_t isFull:1;
	
	union
	{
		CanRangeReturn_0* range;
		CanDelayedReturn_t* single;
	};
	
} CanGenericReturn_t;

typedef struct
{
	uint32_t buffer[5];
	SpiMessage_t spiMessage_t;
	CanGenericReturn_t ret;
}CanMessage_t;

typedef struct
{
	CanMessage_t messages[10];
	size_t messageIndex;

	//Different IDs for each message length
	SpiId_t msg8;
	SpiController_t* spi;
}CanHw_t;

void CANInitialize(CanHw_t* hw,SpiController_t* spi, SpiId_t msg8);
void UpdateCAN(CanHw_t* hw);

//General purpose r/w commands
void CANReadRegister(CanHw_t* hw, CanChipOffset_t add, CanDelayedReturn_t* data);
void CANReadRange(CanHw_t* hw, 
		CanChipOffset_t add,
		CanRangeReturn_0* data,
		CanMessageLength_t length);
void CANWriteRegister(CanHw_t* hw, 
		CanChipOffset_t add, 
		uint8_t data);

//r/w commands for the Tx and Rx registers for fast(er) access
void CANReadRxRegister(CanHw_t* hw,
		CanInputChannelIndex_t index,
		CanDelayedReturn_t* data);
void CANReadRxRange(CanHw_t* hw,
		CanInputChannelIndex_t index,
		CanRangeReturn_0* data,
		CanMessageLength_t length);
void CanWriteTxRegister(CanHw_t* hw,CanOutputChannelIndex_t index, uint8_t data);

void CANRequestToSend(CanHw_t* hw,uint8_t index);

//Only certain registers can use this
void CANMutateRegisterBits(CanHw_t* hw,
		CanChipOffset_t address,
		uint8_t mask, uint8_t data);

void CANReadStatusRegister(CanHw_t* hw, CanDelayedReturn_t* data);
void CANReadRxStatusRegister(CanHw_t* hw, CanDelayedReturn_t* data);

#endif
