#include "SpiController.h"
#include "SpiHardware.h"

void InitializeSpiController(SpiController_t* spi, SpiPackage_t* package, void** buffer, size_t length)
{
	InitializeQueue(&spi->messageQueue,buffer,length);
	spi->package = package;
	spi->messageRunning = false;
}
static void KickOffMessage(SpiController_t* spi, SpiMessage_t* message)
{
	spi->messageRunning = true;
	spi->message = message;
	StartSpiMessage(spi->package,message->id);

	WriteWordToSpi(spi->package,*spi->message->data);
	spi->message->data++;
	spi->message->messageLength--;
}
void QueueSpiMessage(SpiController_t* spi, SpiMessage_t* message){
	if (spi->messageRunning)
	{
		AddToQueue(&spi->messageQueue,message);
	}
	else
	{
		KickOffMessage(spi,message);
	}
}
void SpiChunkSet(SpiController_t* spi){
	if (spi->message->messageLength)
	{
		WriteWordToSpi(spi->package,*spi->message->data);
		spi->message->data++;
		spi->message->messageLength--;
	}
	else{
		EndSpiMessage(spi->package);
		if (DoesQueueHaveStuff(&spi->messageQueue))
		{
			KickOffMessage(spi,(SpiMessage_t*)GetFromQueue(&spi->messageQueue));
		}
		else{
			spi->messageRunning = false;

		}
	}
}


