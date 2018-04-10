#include "CanPhysical.h"
#include "SpiController.h"
#include <string.h>


void CANInitialize(CanHw_t* hw,SpiController_t* spi,SpiId_t msg8)
{
	hw->spi = spi;
	hw->msg8 = msg8;
	hw->messageIndex = 0;
}
void UpdateCAN(CanHw_t* hw)
{
	for (size_t x = 0; x < 10; x++)
	{
		if (hw->messages[x].spiMessage_t.messageLength == 0)
		{
			if (hw->messages[x].ret)
			{
				hw->messages[x].ret->data = 
					hw->messages[x].spiMessage_t.data[
						hw->messages[x].ret->offset];
				hw->messages[x].ret->isReady = 1;
				hw->messages[x].ret = 0;
			}
		}
	}
}
static void CANMessage(CanHw_t* hw,const uint32_t* data,size_t length,
		CanDelayedReturn_t* ret)
{
	CanMessage_t* message = &hw->messages[hw->messageIndex];
	hw->messageIndex++;
	if (hw->messageIndex >= 10)
	{
		hw->messageIndex = 0;
	}

	message->spiMessage_t.data = message->buffer;
	memcpy(message->buffer,data,length*sizeof(uint32_t));
	message->spiMessage_t.messageLength = length;
	message->spiMessage_t.id = hw->msg8;
	message->ret = ret;
	if (ret)
	{
		ret->isReady = 0;
		ret->offset = length-1;
	}
	QueueSpiMessage(hw->spi,&message->spiMessage_t);
}

void CANWriteRegister(CanHw_t* hw,uint8_t add, uint8_t data)
{
	uint32_t message[] =
	{
		1<<6,
		add,
		data
	};
	CANMessage(hw,message,3,0);
}
void CANReadRegister(CanHw_t* hw, uint8_t add, CanDelayedReturn_t* data)
{
	uint32_t message[] =
	{
		(1<<6)|(1<<7),
		add,
		0
	};
	CANMessage(hw,message,3,data);
}
void CANReadRxRegister(CanHw_t* hw,uint8_t index,CanDelayedReturn_t* data)
{
	uint32_t message[] =
	{
		(index<<5)|(1<<3)|(1<<0),
		0
	};
	CANMessage(hw,message,2,data);
}
void CanWriteTxRegister(CanHw_t* hw,uint8_t index, uint8_t data)
{
	uint32_t message[] =
	{
		(index<<5)|(1<<2),
		0
	};
	CANMessage(hw,message,2,0);
}
void CANRequestToSend(CanHw_t* hw,uint8_t index)
{
	uint32_t message[] =
	{
		(index<<5)|(1<<0),
	};
	CANMessage(hw,message,1,0);
}
void CANMutateRegisterBits(CanHw_t* hw,uint8_t address, uint8_t mask, uint8_t data)
{
	uint32_t message[] =
	{
		(1<<5)|(1<<7),
		address,
		mask,
		data
	};
	CANMessage(hw,message,4,0);
}
void CANReadStatusRegister(CanHw_t* hw, CanDelayedReturn_t* data)
{
	uint32_t message[] =
	{
		(1<<2)|(1<<0),
		0
	};
	CANMessage(hw,message,2,data);
}
void CANReadRxStatusRegister(CanHw_t* hw, CanDelayedReturn_t* data)
{
	uint32_t message[] =
	{
		(1<<0)|(1<<2)|(1<<3),
		0
	};
	CANMessage(hw,message,2,data);
}
