extern "C"{
	#include "SpiHardware.h"
}
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/CommandLineTestRunner.h"

static SpiPackage_t spi;
static SpiHw_t hardware[3];
static const SpiId_t NumTargets = 7;
static SpiTarget_t targets[NumTargets] = {
	{
		&hardware[0],
		0
	},
	{
		&hardware[0],
		1
	},
	{
		&hardware[0],
		2
	},
	{
		&hardware[1],
		0
	},
	{
		&hardware[2],
		0
	},
	{
		&hardware[2],
		1
	},
	{
		&hardware[2],
		2
	}
};
Pio_t activeSelection;
TEST_GROUP(SpiHardware)
{
	void setup(){
		InitializeSpi(&spi,targets,&activeSelection,NumTargets);
		
	}
	void teardown() {
		mock().clear();
	}
};
void CheckStartSpiMessageSelectsTarget(SpiId_t id)
{
	StartSpiMessage(&spi,id);
	LONGS_EQUAL((1<<targets[id].slaveselect)
			,targets[id].hardware->slaveselect);
}
TEST(SpiHardware, StartSpiMessageSelectsTarget){
	for (SpiId_t x = 0; x < NumTargets; x++){
		CheckStartSpiMessageSelectsTarget(x);
	}
}

TEST(SpiHardware, IndexingOutOfRangeIsError)
{
	mock().expectOneCall("DeclareError");
	StartSpiMessage(&spi,NumTargets);
	mock().checkExpectations();
}

int main(int ac, char** av)
{
	return CommandLineTestRunner::RunAllTests(ac, av);
}
