#ifndef CAN_LOGICAL_H
#define CAN_LOGICAL_H
#include "CanPhysical.h"

typedef void (*CanLogicalCallBack)(void* custom,uint32_t* data,uint8_t length);
CAN_RANGE_RETURN_MAKE(0x11,rx_reg_t);
typedef uint8_t CanSubscriberSize_t;
typedef enum 
{
	CAN_NORMAL_MODE = 0,
	CAN_SLEEP_MODE = 1,
	CAN_LOOP_BACK_MODE = 2,
	CAN_LISTEN_ONLY_MODE = 3,
	CAN_CONFIGURATION_MODE = 4,
}CanMode_t;
typedef struct
{
	CanHw_t* host;
	CanChipOffset_t base;
	CanOutputChannelIndex_t index;
} CanMaster_t;
typedef struct
{
	CanLogicalCallBack callback;
	CanIdentifier_t identifier; //Depends on the HW being little endian
	void* data;
} CanSubscriber_t;
typedef struct
{
	CanHw_t* host;
	rx_reg_t rx_reg[2];
	CanSubscriber_t* subs;
	CanSubscriberSize_t nsubs;
	CanDelayedReturn_t rxstatus;
} LogicalCan_t;


void InitializeCanMaster(CanMaster_t* master, 
			CanHw_t* host, 
			CanChipOffset_t base, 
			CanOutputChannelIndex_t index);
void InitializeLogicalCan(LogicalCan_t* can,
			CanHw_t* host,
			CanSubscriber_t* subs,
			CanSubscriberSize_t nsubs);
void TransmitMessage(CanMaster_t* master,uint8_t* data,CanMessageLength_t length);
void UpdateLogicalCan(LogicalCan_t* can);

//The following methods are blocking and are only designed
//to be called on startup
void EnterCanMode(LogicalCan_t* can, CanMode_t mode); 


#endif
