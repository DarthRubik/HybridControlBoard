project(HybridControlBoard)


# include CppUTest headers
include_directories($ENV{CPPUTEST_INCLUDE}/include)
include_directories(../src)

# add cpputest library
add_library(imp_cpputest STATIC IMPORTED)
add_library(imp_cpputestext STATIC IMPORTED)

set_property(TARGET imp_cpputest PROPERTY
	IMPORTED_LOCATION $ENV{CPPUTEST_BIN}/libCppUTest.a)
set_property(TARGET imp_cpputestext PROPERTY
	IMPORTED_LOCATION $ENV{CPPUTEST_BIN}/libCppUTestExt.a)


function(create_test COM LINKLIST)
	list(GET LINKLIST 0 MODULE_NAME)
	add_executable(${MODULE_NAME} ${MODULE_NAME}.cpp)
	target_link_libraries(${LINKLIST})
	add_custom_command(
		TARGET ${MODULE_NAME}
		COMMENT "${COM}"
		POST_BUILD
		COMMAND ${MODULE_NAME}
	)
endfunction()



add_library(MockSpiController MockSpiController.c)
add_library(MockSpiHardware MockSpiHardware.c)
add_library(MockError MockError.c)

create_test("Run spi controller tests" "SpiControllerTests;SpiController;Queue;MockError;MockSpiHardware;imp_cpputest;imp_cpputestext")	
create_test("Run queue tests" "QueueTests;Queue;MockError;imp_cpputest;imp_cpputestext")	
create_test("Run adc tests" "AdcTests;Adc;MockSpiController;MockError;imp_cpputest;imp_cpputestext")
