#include "SpiHardware.h"
#include "Error.h"



void InitializeSpi(SpiPackage_t* spi,
		SpiTarget_t* targets,
		Pio_t* active,
		SpiId_t count){
	spi->targets = targets;
	spi->mux = active;
	spi->activeHw = 0;
	spi->count = count;
}
void StartSpiMessage(SpiPackage_t* spi, SpiId_t id)
{
	if (spi->count > id)
	{
		spi->targets[id].hardware->slaveselect = 
			1 << spi->targets[id].slaveselect;
		spi->mux->data = spi->targets[id].activeMux;
		spi->activeHw = spi->targets[id].hardware;
	}
	else{
		DeclareError("Tried to index a spi device outside of range");
	}
}
void WriteWordToSpi(SpiPackage_t* spi, uint32_t word)
{
	spi->activeHw->txdata = word;
}
void EndSpiMessage(SpiPackage_t* spi)
{
	spi->activeHw->slaveselect = 0;
}

