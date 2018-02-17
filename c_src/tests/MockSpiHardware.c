#include "CppUTestExt/MockSupport_c.h"
#include "SpiHardware.h"

void StartSpiMessage(SpiPackage_t* spi, SpiId_t id){
	mock_c()->actualCall("StartSpiMessage")->
		withPointerParameters("spi",spi)->
		withIntParameters("id",id);
}
