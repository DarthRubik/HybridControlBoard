project(HybridControlBoard)


# include CppUTest headers
include_directories($ENV{CPPUTEST_INCLUDE}/include)
include_directories(../src)

# add cpputest library
add_library(imp_cpputest STATIC IMPORTED)
add_library(imp_cpputestext STATIC IMPORTED)
add_library(SpiControllerTests SpiControllerTests.cpp)
add_library(MockSpiHardware MockSpiHardware.c)
add_library(QueueTests QueueTests.cpp)
add_library(MockError MockError.c)
set_property(TARGET imp_cpputest PROPERTY
	IMPORTED_LOCATION $ENV{CPPUTEST_BIN}/libCppUTest.a)
set_property(TARGET imp_cpputestext PROPERTY
	IMPORTED_LOCATION $ENV{CPPUTEST_BIN}/libCppUTestExt.a)



add_executable(RunAllTests RunAllTests.cpp)

target_link_libraries(RunAllTests 
	SpiControllerTests SpiController MockSpiHardware 
	QueueTests Queue 
	MockError
	imp_cpputestext imp_cpputest)

set(UNIT_TEST RunAllTests)
add_custom_command(
     TARGET ${UNIT_TEST}
     COMMENT "Run tests"
     POST_BUILD 
     COMMAND ${UNIT_TEST} -c
)

