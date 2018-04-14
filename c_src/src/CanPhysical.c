#include "CanPhysical.h"
#include "SpiController.h"
#include <string.h>


void CANInitialize(CanHw_t* hw,SpiController_t* spi,SpiId_t msg8)
{
	hw->spi = spi;
	hw->msg8 = msg8;
	hw->messageIndex = 0;
	for (size_t x = 0; x < 10; x++)
	{
		hw->messages[x].spiMessage_t.messageLength = 0;
		hw->messages[x].ret.isFull = 0;
	}
}
void UpdateCAN(CanHw_t* hw)
{
	for (size_t x = 0; x < 10; x++)
	{
		if (hw->messages[x].spiMessage_t.messageLength == 0)
		{
			if (hw->messages[x].ret.isFull)
			{
				if (!hw->messages[x].ret.isRange)
				{
					hw->messages[x].ret.single->data = 
						hw->messages[x].spiMessage_t.data[
						hw->messages[x].ret.single->offset];
					hw->messages[x].ret.single->isReady = 1;
				}
				else
				{
					hw->messages[x].ret.range->isReady = 1;
				}
				hw->messages[x].ret.isFull = 0;
			}
		}
	}
}
uint8_t CANIsBusy(CanHw_t* hw)
{
	for (size_t x = 0; x < 10; x++)
	{
		if (hw->messages[x].spiMessage_t.messageLength)
		{
			return 1;
		}
	}
	return 0;
}
static CanGenericReturn_t getGenericRange(CanRangeReturn_0* range,
		CanChipOffset_t offset)
{
	CanGenericReturn_t ret;
	ret.isRange = 1;
	ret.isFull = 1;
	ret.range = range;
	ret.range->offset = offset;
	return ret;
}
static CanGenericReturn_t getGenericSingle(CanDelayedReturn_t* single)
{
	CanGenericReturn_t ret;
	ret.isRange = 0;
	ret.isFull = 1;
	ret.single = single;
	return ret;
}
static CanGenericReturn_t getGenericNull(void)
{
	CanGenericReturn_t ret;
	ret.isFull = 0;
	return ret;
}
static void CANMessage(CanHw_t* hw,const uint32_t* data,size_t length,
		CanGenericReturn_t ret)
{
	CanMessage_t* message = &hw->messages[hw->messageIndex];
	hw->messageIndex++;
	if (hw->messageIndex >= 10)
	{
		hw->messageIndex = 0;
	}

	if (!ret.isRange)
	{
		message->spiMessage_t.data = message->buffer;
		memcpy(message->buffer,data,length*sizeof(uint32_t));
	}
	else
	{
		message->spiMessage_t.data = ret.range->data;
		memcpy(ret.range->data,data,length*sizeof(uint32_t));
	}
	message->spiMessage_t.messageLength = length;
	message->spiMessage_t.id = hw->msg8;
	message->ret = ret;
	if (ret.isFull)
	{
		if (ret.isRange)
		{
			ret.range->isReady = 0;
		}
		else
		{
			ret.single->isReady = 0;
			ret.single->offset = length-1;
		}
	}
	QueueSpiMessage(hw->spi,&message->spiMessage_t);
}

void CANWriteRegister(CanHw_t* hw,CanChipOffset_t add, uint8_t data)
{
	uint32_t message[] =
	{
		1<<6,
		add,
		data
	};
	CANMessage(hw,message,3,getGenericNull());
}
void CANReadRegister(CanHw_t* hw, CanChipOffset_t add, CanDelayedReturn_t* data)
{
	uint32_t message[] =
	{
		(1<<6)|(1<<7),
		add,
		0
	};
	CANMessage(hw,message,3,getGenericSingle(data));
}
void CANReadRange(CanHw_t* hw, 
		CanChipOffset_t add,
		CanRangeReturn_0* data,
		CanMessageLength_t length)
{
	uint32_t message[] =
	{
		(1<<6)|(1<<7),
		add,
	};
	CANMessage(hw,message,2+length,getGenericRange(data,2));
}
void CANReadRxRegister(CanHw_t* hw,
		CanInputChannelIndex_t index,
		CanDelayedReturn_t* data)
{
	uint32_t message[] =
	{
		(index<<5)|(1<<3)|(1<<0),
		0
	};
	CANMessage(hw,message,2,getGenericSingle(data));
}
void CANReadRxRange(CanHw_t* hw,
		CanInputChannelIndex_t index,
		CanRangeReturn_0* data,
		CanMessageLength_t length)
{
	uint32_t message[] =
	{
		(index<<5)|(1<<3)|(1<<0),
	};
	CANMessage(hw,message,1+length,getGenericRange(data,1));
}
void CanWriteTxRegister(CanHw_t* hw,
		CanOutputChannelIndex_t index, 
		uint8_t data)
{
	uint32_t message[] =
	{
		(index<<5)|(1<<2),
		0
	};
	CANMessage(hw,message,2,getGenericNull());
}
void CANRequestToSend(CanHw_t* hw,uint8_t index)
{
	uint32_t message[] =
	{
		(index<<5)|(1<<0),
	};
	CANMessage(hw,message,1,getGenericNull());
}
void CANMutateRegisterBits(CanHw_t* hw,
		CanChipOffset_t address,
		uint8_t mask,
		uint8_t data)
{
	uint32_t message[] =
	{
		(1<<5)|(1<<7),
		address,
		mask,
		data
	};
	CANMessage(hw,message,4,getGenericNull());
}
void CANReadStatusRegister(CanHw_t* hw, CanDelayedReturn_t* data)
{
	uint32_t message[] =
	{
		(1<<2)|(1<<0),
		0
	};
	CANMessage(hw,message,2,getGenericSingle(data));
}
void CANReadRxStatusRegister(CanHw_t* hw, CanDelayedReturn_t* data)
{
	uint32_t message[] =
	{
		(1<<0)|(1<<2)|(1<<3),
		0
	};
	CANMessage(hw,message,2,getGenericSingle(data));
}

void (*CANYieldFunction)(CanHw_t*) = 0;

uint8_t CANReadRegisterBlocking(CanHw_t* hw, CanChipOffset_t offset)
{
	CanDelayedReturn_t reg;
	CANReadRegister(hw,offset,&reg);
	while (!reg.isReady)
	{
		//This code is here for debugging
		//in the real application it should never do anything
		if (CANYieldFunction)
			CANYieldFunction(hw);

		
		UpdateCAN(hw);
	}
	return reg.data;
}
