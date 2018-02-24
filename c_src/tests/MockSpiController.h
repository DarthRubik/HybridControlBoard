#ifndef MOCK_SPI_CONTROLLER_H
#define MOCK_SPI_CONTROLLER_H
#include "stdint.h"
#include "SpiController.h"

void MockSpiFakeReceiveData(uint32_t* data, SpiMessage_t* message, size_t use);

#endif
