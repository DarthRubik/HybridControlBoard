#include "CanPhysical.h"
#include "SpiController.h"
#include <string.h>
#include "CppUTestExt/MockSupport_c.h"

uint8_t CanRegisters[128];
void CANInitialize(CanHw_t* hw,SpiController_t* spi,SpiId_t msg8)
{
}
void UpdateCAN(CanHw_t* hw)
{
}

void CANWriteRegister(CanHw_t* hw,uint8_t add, uint8_t data)
{
	CanRegisters[add] = data;
}
void CANReadRegister(CanHw_t* hw, uint8_t add, CanDelayedReturn_t* data)
{
	data->data = CanRegisters[add];
	data->isReady = 1;
}
void CANReadRxRegister(CanHw_t* hw,uint8_t index,CanDelayedReturn_t* data)
{
}
void CANReadRxRange(CanHw_t* hw,uint8_t index,CanRangeReturn_0* data,uint8_t length)
{
	data->offset = 1;
	data->isReady = 1;
	for (uint8_t x = 0; x < length; x++)
	{
		data->data[x+1] = CanRegisters[0x60+0x10*index+x];
	}
}
void CanWriteTxRegister(CanHw_t* hw,uint8_t index, uint8_t data)
{
}
void CANRequestToSend(CanHw_t* hw,uint8_t index)
{
	CanRegisters[0x30 + 0x10*index] |= 1<<3;
}
void CANMutateRegisterBits(CanHw_t* hw,uint8_t add, uint8_t mask, uint8_t data)
{
	CanRegisters[add] = (CanRegisters[add] & ~mask) | data;
}
void CANReadStatusRegister(CanHw_t* hw, CanDelayedReturn_t* data)
{

}
void CANReadRxStatusRegister(CanHw_t* hw, CanDelayedReturn_t* data)
{
	mock_c()->setPointerData("rx_reg",data);
	data->isReady = 0;
}
