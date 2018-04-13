project(HybridControlBoard)


include_directories(.)


add_library(Queue Queue.c)
add_library(SpiController SpiController.c)
add_library(Adc Adc.c)
add_library(SpiHardware SpiHardware.c)
add_library(Error Error.c)
add_library(CanPhysical CanPhysical.c)
add_library(CanLogical CanLogical.c)
