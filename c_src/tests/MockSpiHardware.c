#include "CppUTestExt/MockSupport_c.h"
#include "SpiHardware.h"

void IntializeSpi(SpiPackage_t* spi, SpiHw_t* hws, Pio_t* Active, SpiId_t count)
{
}
void StartSpiMessage(SpiPackage_t* spi, SpiId_t id){
	mock_c()->actualCall("StartSpiMessage")->
		withPointerParameters("spi",spi)->
		withIntParameters("id",id);
}
void WriteWordToSpi(SpiPackage_t* spi, uint32_t word)
{
	mock_c()->actualCall("WriteWordToSpi")->
		withPointerParameters("spi",spi)->
		withIntParameters("word",word);
}
void EndSpiMessage(SpiPackage_t* spi)
{
	mock_c()->actualCall("EndSpiMessage")->
		withPointerParameters("spi",spi);
}
