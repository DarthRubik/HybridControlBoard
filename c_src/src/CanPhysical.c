#include "CanPhysical.h"
#include "SpiController.h"


void CANInitialize(CanHw_t* hw,SpiController_t* spi,
	SpiId_t msg8,
	SpiId_t msg16,
	SpiId_t msg24,
	SpiId_t msg32)
{
	hw->spi = spi;
	hw->msg8 = msg8;
	hw->msg16 = msg16;
	hw->msg24 = msg24;
	hw->msg32 = msg32;
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
					(*hw->messages[x].spiMessage_t.data)
						>> hw->messages[x].ret->offset;
				hw->messages[x].ret->isReady = 1;
				hw->messages[x].ret = 0;
			}
		}
	}
}
static void CANMessage(CanHw_t* hw,uint32_t data,
		SpiId_t id,CanDelayedReturn_t* ret,uint8_t offset)
{
	CanMessage_t* message = &hw->messages[hw->messageIndex];
	hw->messageIndex++;
	if (hw->messageIndex >= 10)
	{
		hw->messageIndex = 0;
	}

	message->spiMessage_t.data = message->buffer;
	message->buffer[0] = data;
	message->spiMessage_t.messageLength = 1;
	message->spiMessage_t.id = id;
	message->ret = ret;
	if (ret)
	{
		ret->isReady = 0;
		ret->offset = offset;
	}
	QueueSpiMessage(hw->spi,&message->spiMessage_t);
}

void CANWriteRegister(CanHw_t* hw,uint8_t add, uint8_t data)
{
	CANMessage(hw,(1<<6)|(add<<8)|(data<<16),hw->msg24,0,0);
}
void CANReadRegister(CanHw_t* hw, uint8_t add, CanDelayedReturn_t* data)
{
	CANMessage(hw,(1<<6)|(1<<7)|(add << 8) ,hw->msg24,data,16);
}
void CANReadRxRegister(CanHw_t* hw,uint8_t index,CanDelayedReturn_t* data)
{
	CANMessage(hw,(index<<5)|(1<<3)|(1<<0),hw->msg16,data,8);
}
void CanWriteTxRegister(CanHw_t* hw,uint8_t index, uint8_t data)
{
	CANMessage(hw,(index<<5)|(1<<1)|(data<<8),hw->msg16,0,0);
}
void CANRequestToSend(CanHw_t* hw,uint8_t index)
{
	CANMessage(hw,(index<<5)|(1<<0),hw->msg8,0,0);
}
void CANMutateRegisterBits(CanHw_t* hw,uint8_t address, uint8_t mask, uint8_t data)
{
	CANMessage(hw,(1<<5)|(1<<7)|(address<<8)|(mask<<16)|(data<<24),hw->msg32,0,0);
}
void CANReadStatusRegister(CanHw_t* hw, CanDelayedReturn_t* data)
{
	CANMessage(hw,(1<<2)|(1<<0),hw->msg16,data,8);
}
void CANReadRxStatusRegister(CanHw_t* hw, CanDelayedReturn_t* data)
{
	CANMessage(hw,(1<<0)|(2<<1)|(3<<1),hw->msg16,data,8);
}
