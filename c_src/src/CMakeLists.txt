project(HybridControlBoard)


include_directories(.)


add_library(Queue Queue.c)
add_library(SpiController SpiController.c)
add_library(Adc Adc.c)
