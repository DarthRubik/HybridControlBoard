#include "SpiController.h"
#include "CppUTestExt/MockSupport_c.h"
#include "MockSpiController.h"

void InitializeSpiController(SpiController_t* spi, SpiPackage_t* package,void** buffer,size_t
 length){

}

//Pretends to receive data from the SPI
//The data pointer is the data to use
//The SpiMessage_t is the message to transmit to
//and the use variable say how much to use
void MockSpiFakeReceiveData(uint32_t* data, SpiMessage_t* message, size_t use)
{
	for (size_t x = 0; x < use; x++)
	{
		*data = *message->data;
		data++;
		message->data++;
	}
}
void QueueSpiMessage(SpiController_t* spi,SpiMessage_t* message){
	mock_c()->actualCall("QueueSpiMessage")->
		withPointerParameters("spi",spi)->
		withPointerParameters("message",message);



	
}

void SpiChunkSet(SpiController_t* spi,uint32_t data){
}
