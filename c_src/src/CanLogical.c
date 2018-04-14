#include "CanLogical.h"
#include "Error.h"
#include "stdio.h"

#define RXB0FULL (1<<6)
#define RXB1FULL (1<<7)

//It looks like the TX and RX registers are the same
//so these defines work for both
#define CONTROL_OFFSET 0
#define IDENTIFIER_OFFSET 1
#define LENGTH_OFFSET 5
#define DATA_OFFSET 6

void InitializeCanMaster(CanMaster_t* master, 
			CanHw_t* host, 
			CanChipOffset_t base, 
			CanOutputChannelIndex_t index)
{
	master->base = base;
	master->index = index;
}
void InitializeLogicalCan(LogicalCan_t* can,
			CanHw_t* host,
			CanSubscriber_t* subs,
			CanSubscriberSize_t nsubs)
{
	can->host = host;
	can->subs = subs;
	can->nsubs = nsubs;
	CANReadRxStatusRegister(can->host,&can->rxstatus);
}
void TransmitMessage(CanMaster_t* master,uint8_t* data,CanMessageLength_t length)
{
	if (length > 8)
	{
		DeclareError("Message cannot be longer than 8 bytes");
	}
	CANWriteRegister(master->host,master->base+LENGTH_OFFSET,length);
	for (CanMessageLength_t x = 0; x < length; x++)
	{
		CANWriteRegister(master->host,master->base+DATA_OFFSET+x,data[x]);
	}
	CANRequestToSend(master->host,master->index);
}
void HandleInputEvent(LogicalCan_t* can,rx_reg_t* reg)
{
	CanMessageLength_t length = reg->data[reg->offset+LENGTH_OFFSET] & 0xf;
	union
	{
		CanIdentifier_t id;
		uint8_t array[sizeof(CanIdentifier_t)];
	}identifier;
	
	for (uint8_t y = 0; y < sizeof(CanIdentifier_t); y++)
	{
		identifier.array[y] = reg->data[reg->offset+y+IDENTIFIER_OFFSET];
	}
	for (uint8_t y = 0; y < can->nsubs; y++)
	{
		CanSubscriber_t* sub = &can->subs[y];
		if (sub->identifier == identifier.id)
		{
			sub->callback(sub->data,
					&reg->data[reg->offset+DATA_OFFSET],
					length);
		}
	}
	reg->isReady = 0;
}
void UpdateLogicalCan(LogicalCan_t* can)
{
	for (uint8_t x = 0; x < 2; x++)
	{
		rx_reg_t* reg = &can->rx_reg[x];
		if (reg->isReady)
		{
			HandleInputEvent(can,reg);
		}
	}
	if (can->rxstatus.isReady)
	{
		if (can->rxstatus.data & RXB0FULL)
			CANReadRxRange(can->host,0,
					(CanRangeReturn_0*)&can->rx_reg[0],0x10);
		if (can->rxstatus.data & RXB1FULL)
			CANReadRxRange(can->host,1,
					(CanRangeReturn_0*)&can->rx_reg[1],0x10);
		CANReadRxStatusRegister(can->host,&can->rxstatus);
	}
}
void EnterCanMode(LogicalCan_t* can, CanMode_t mode)
{
	CANMutateRegisterBits(can->host,0xf,0x7<<5,mode<<5);
	uint8_t data;
	do
	{
		data = CANReadRegisterBlocking(can->host,0xe);
	}
	while ((data & (0x7<<5)) == (mode<<5));
}
