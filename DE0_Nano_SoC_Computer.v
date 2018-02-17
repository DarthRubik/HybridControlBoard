
module DE0_Nano_SoC_Computer (
	////////////////////////////////////
	// FPGA Pins
	////////////////////////////////////

	// Clock pins
	input              CLOCK_50,
	input              CLOCK2_50,
	input              CLOCK3_50,
	
	// ADC
	output             ADC_CONVST,
	output             ADC_SCLK,
	output             ADC_SDI,
	input              ADC_SDO,
	
	// ARDUINO
	inout       [15:0] ARDUINO_IO,
	inout              ARDUINO_RESET_N,
	
	// GPIO
	inout       [35:0] GPIO_0,
	inout       [35:0] GPIO_1,
	
	// KEY
	input       [1:0]  KEY,
	
	// LED
	output      [7:0]  LED,
	
	// SW
	input       [3:0]  SW,
	
	////////////////////////////////////
	// HPS Pins
	////////////////////////////////////
	
	// DDR3 SDRAM
	output      [14:0] HPS_DDR3_ADDR,
	output      [2:0]  HPS_DDR3_BA,
	output             HPS_DDR3_CAS_N,
	output             HPS_DDR3_CKE,
	output             HPS_DDR3_CK_N,
	output             HPS_DDR3_CK_P,
	output             HPS_DDR3_CS_N,
	output      [3:0]  HPS_DDR3_DM,
	inout       [31:0] HPS_DDR3_DQ,
	inout       [3:0]  HPS_DDR3_DQS_N,
	inout       [3:0]  HPS_DDR3_DQS_P,
	output             HPS_DDR3_ODT,
	output             HPS_DDR3_RAS_N,
	output             HPS_DDR3_RESET_N,
	input              HPS_DDR3_RZQ,
	output             HPS_DDR3_WE_N,

	// Ethernet
	output             HPS_ENET_GTX_CLK,
	inout              HPS_ENET_INT_N,
	output             HPS_ENET_MDC,
	inout              HPS_ENET_MDIO,
	input              HPS_ENET_RX_CLK,
	input       [3:0]  HPS_ENET_RX_DATA,
	input              HPS_ENET_RX_DV,
	output      [3:0]  HPS_ENET_TX_DATA,
	output             HPS_ENET_TX_EN,

	// Accelerometer
	inout              HPS_GSENSOR_INT,

	// I2C
	inout              HPS_I2C0_SCLK,
	inout              HPS_I2C0_SDAT,
	inout              HPS_I2C1_SCLK,
	inout              HPS_I2C1_SDAT,
	
	// Pushbutton
	inout              HPS_KEY,

	// LED
	inout              HPS_LED,

	// LTC
	inout              HPS_LTC_GPIO,

	// SD Card
	output             HPS_SD_CLK,
	inout              HPS_SD_CMD,
	inout       [3:0]  HPS_SD_DATA,
	
	// SPI
	output             HPS_SPIM_CLK,
	input              HPS_SPIM_MISO,
	output             HPS_SPIM_MOSI,
	inout              HPS_SPIM_SS,
	
	// UART
	input              HPS_UART_RX,
	output             HPS_UART_TX,
	
	// USB
	inout              HPS_CONV_USB_N,
	input              HPS_USB_CLKOUT,
	inout       [7:0]  HPS_USB_DATA,
	input              HPS_USB_DIR,
	input              HPS_USB_NXT,
	output             HPS_USB_STP
);


//=======================================================
//  REG/WIRE declarations
//=======================================================
wire  hps_fpga_reset_n;

wire [7:0] servo_0_reg;

wire [7:0] servo_1_reg;

wire slowClock;

wire ADC_SPI_mosi;
wire ADC_SPI_miso;
wire ADC_SPI_clk;
wire CAN_SPI_mosi;
wire CAN_SPI_miso;
wire CAN_SPI_clk;
wire [7:0] SPI_SELECT_BUS;
wire REAL_SPI_mosi;
wire REAL_SPI_miso;
wire REAL_SPI_clk;


//Add more later if necessary
assign REAL_SPI_clk = SPI_SELECT_BUS[0] ? ADC_SPI_clk : CAN_SPI_clk; 
assign REAL_SPI_mosi = SPI_SELECT_BUS[0] ? ADC_SPI_mosi : CAN_SPI_mosi; 
assign ADC_SPI_miso = REAL_SPI_miso; 
assign CAN_SPI_miso = REAL_SPI_miso;

assign GPIO_0[32] = CAN_SPI_clk;
assign REAL_SPI_miso = GPIO_0[34];
assign GPIO_0[35] = CAN_SPI_mosi;



//SLOWCLK U1(.RST(hps_fpga_reset_n),.CLK50MHZ(CLOCK_50),.CLK1HZ(slowClock));
//servo U2(.eightBitBus(servo_0_reg),.clk(slowClock),.rst(hps_fpga_reset_n),.outputSignal(LED[0]));
//servo U3(.eightBitBus(servo_1_reg),.clk(slowClock),.rst(hps_fpga_reset_n),.outputSignal(LED[1]));



//=======================================================
//  Structural coding
//=======================================================
Computer_System The_System (
	////////////////////////////////////
	// FPGA Side
	////////////////////////////////////

	// Global signals
	.system_pll_ref_clk_clk				(CLOCK_50),
	.system_pll_ref_reset_reset			(1'b0),

	// ADC
	.adc_sclk							(ADC_SCLK),
	.adc_cs_n							(ADC_CONVST),
	.adc_dout							(ADC_SDO),
	.adc_din							(ADC_SDI),

	// Arduino GPIO
	.arduino_gpio_export				(ARDUINO_IO),

	// Arduino Reset_n
	.arduino_reset_n_export				(ARDUINO_RESET_N),

	// Slider Switches
	.slider_switches_export				(SW),

	// Pushbuttons
	.pushbuttons_export					(~KEY),
	
	
	.ls_sw_external_connection_export({GPIO_1[25],GPIO_1[24],GPIO_1[27],GPIO_1[26],GPIO_1[29],GPIO_1[28],
												  GPIO_1[31],GPIO_1[30],GPIO_1[33],GPIO_1[32],GPIO_1[35],GPIO_1[34]}),
	

	// Expansion JP1
	//.expansion_jp1_export				({GPIO_0[35:19], GPIO_0[17], GPIO_0[15:3], GPIO_0[1]}),

	//.expansion_jp1_export            ({GPIO_0[29],GPIO_0[28],GPIO_0[27],GPIO_0[26],GPIO_0[34],GPIO_0[4],GPIO_0[3],GPIO_0[2],GPIO_0[1],GPIO_0[0],GPIO_0[35],GPIO_0[33],GPIO_0[25],GPIO_0[12],GPIO_0[11],GPIO_0[10],GPIO_0[24],GPIO_0[23],GPIO_0[22],GPIO_0[21],GPIO_0[20],GPIO_0[19],GPIO_0[18],GPIO_0[17],GPIO_0[16],GPIO_0[15],GPIO_0[14],GPIO_0[13],GPIO_0[5],GPIO_0[6],GPIO_0[7],GPIO_0[8]}),
	// Expansion JP7
	//.expansion_jp7_export				({GPIO_1[31:0]}),
	
	
	.adc_spi_external_SCLK({ADC_SPI_clk}),
	.adc_spi_external_MOSI({ADC_SPI_mosi}),
	.adc_spi_external_MISO({ADC_SPI_miso}),
	.adc_spi_external_SS_n({GPIO_0[29],GPIO_0[28]}),
	.can_spi_external_SCLK({CAN_SPI_clk}),
	.can_spi_external_MOSI({CAN_SPI_mosi}),
	.can_spi_external_MISO({CAN_SPI_miso}),
	.can_spi_external_SS_n({GPIO_0[33],GPIO_0[30],GPIO_0[31]}),
	.spi_select_external_connection_export({SPI_SELECT_BUS[7:0]}),
	
	//.spi_spi_clk                     ({GPIO_1[35]}),
	//.spi_spi_miso                    ({GPIO_1[34]}),
	//.spi_spi_mosi							({GPIO_1[33]}),
	//.spi_spi_cs_n								({GPIO_1[32:25]}),

	// LEDs
	//.leds_export						(LED),
	//.servo_0_export (servo_0_reg),
	//.servo_1_export (servo_1_reg),
	
	////////////////////////////////////
	// HPS Side
	////////////////////////////////////
	// DDR3 SDRAM
	.memory_mem_a			(HPS_DDR3_ADDR),
	.memory_mem_ba			(HPS_DDR3_BA),
	.memory_mem_ck			(HPS_DDR3_CK_P),
	.memory_mem_ck_n		(HPS_DDR3_CK_N),
	.memory_mem_cke			(HPS_DDR3_CKE),
	.memory_mem_cs_n		(HPS_DDR3_CS_N),
	.memory_mem_ras_n		(HPS_DDR3_RAS_N),
	.memory_mem_cas_n		(HPS_DDR3_CAS_N),
	.memory_mem_we_n		(HPS_DDR3_WE_N),
	.memory_mem_reset_n		(HPS_DDR3_RESET_N),
	.memory_mem_dq			(HPS_DDR3_DQ),
	.memory_mem_dqs			(HPS_DDR3_DQS_P),
	.memory_mem_dqs_n		(HPS_DDR3_DQS_N),
	.memory_mem_odt			(HPS_DDR3_ODT),
	.memory_mem_dm			(HPS_DDR3_DM),
	.memory_oct_rzqin		(HPS_DDR3_RZQ),
		  
	// Accelerometer
	.hps_io_hps_io_gpio_inst_GPIO61		(HPS_GSENSOR_INT),

	// Ethernet
	.hps_io_hps_io_gpio_inst_GPIO35		(HPS_ENET_INT_N),
	.hps_io_hps_io_emac1_inst_TX_CLK	(HPS_ENET_GTX_CLK),
	.hps_io_hps_io_emac1_inst_TXD0		(HPS_ENET_TX_DATA[0]),
	.hps_io_hps_io_emac1_inst_TXD1		(HPS_ENET_TX_DATA[1]),
	.hps_io_hps_io_emac1_inst_TXD2		(HPS_ENET_TX_DATA[2]),
	.hps_io_hps_io_emac1_inst_TXD3		(HPS_ENET_TX_DATA[3]),
	.hps_io_hps_io_emac1_inst_RXD0		(HPS_ENET_RX_DATA[0]),
	.hps_io_hps_io_emac1_inst_MDIO		(HPS_ENET_MDIO),
	.hps_io_hps_io_emac1_inst_MDC		(HPS_ENET_MDC),
	.hps_io_hps_io_emac1_inst_RX_CTL	(HPS_ENET_RX_DV),
	.hps_io_hps_io_emac1_inst_TX_CTL	(HPS_ENET_TX_EN),
	.hps_io_hps_io_emac1_inst_RX_CLK	(HPS_ENET_RX_CLK),
	.hps_io_hps_io_emac1_inst_RXD1		(HPS_ENET_RX_DATA[1]),
	.hps_io_hps_io_emac1_inst_RXD2		(HPS_ENET_RX_DATA[2]),
	.hps_io_hps_io_emac1_inst_RXD3		(HPS_ENET_RX_DATA[3]),

	// I2C
	.hps_io_hps_io_i2c0_inst_SDA		(HPS_I2C0_SDAT),
	.hps_io_hps_io_i2c0_inst_SCL		(HPS_I2C0_SCLK),
	.hps_io_hps_io_i2c1_inst_SDA		(HPS_I2C1_SDAT),
	.hps_io_hps_io_i2c1_inst_SCL		(HPS_I2C1_SCLK),

	// Pushbutton
	.hps_io_hps_io_gpio_inst_GPIO54		(HPS_KEY),

	// LED
	.hps_io_hps_io_gpio_inst_GPIO53		(HPS_LED),

	// LTC
	.hps_io_hps_io_gpio_inst_GPIO40		(HPS_LTC_GPIO),

	// SD Card
	.hps_io_hps_io_sdio_inst_CMD		(HPS_SD_CMD),
	.hps_io_hps_io_sdio_inst_D0			(HPS_SD_DATA[0]),
	.hps_io_hps_io_sdio_inst_D1			(HPS_SD_DATA[1]),
	.hps_io_hps_io_sdio_inst_CLK		(HPS_SD_CLK),
	.hps_io_hps_io_sdio_inst_D2			(HPS_SD_DATA[2]),
	.hps_io_hps_io_sdio_inst_D3			(HPS_SD_DATA[3]),

	// SPI
	.hps_io_hps_io_spim1_inst_CLK		(HPS_SPIM_CLK),
	.hps_io_hps_io_spim1_inst_MOSI		(HPS_SPIM_MOSI),
	.hps_io_hps_io_spim1_inst_MISO		(HPS_SPIM_MISO),
	.hps_io_hps_io_spim1_inst_SS0		(HPS_SPIM_SS),
	

	
	
	
	// UART
	.hps_io_hps_io_uart0_inst_RX		(HPS_UART_RX),
	.hps_io_hps_io_uart0_inst_TX		(HPS_UART_TX),

	// USB
	.hps_io_hps_io_gpio_inst_GPIO09		(HPS_CONV_USB_N),
	.hps_io_hps_io_usb1_inst_D0			(HPS_USB_DATA[0]),
	.hps_io_hps_io_usb1_inst_D1			(HPS_USB_DATA[1]),
	.hps_io_hps_io_usb1_inst_D2			(HPS_USB_DATA[2]),
	.hps_io_hps_io_usb1_inst_D3			(HPS_USB_DATA[3]),
	.hps_io_hps_io_usb1_inst_D4			(HPS_USB_DATA[4]),
	.hps_io_hps_io_usb1_inst_D5			(HPS_USB_DATA[5]),
	.hps_io_hps_io_usb1_inst_D6			(HPS_USB_DATA[6]),
	.hps_io_hps_io_usb1_inst_D7			(HPS_USB_DATA[7]),
	.hps_io_hps_io_usb1_inst_CLK		(HPS_USB_CLKOUT),
	.hps_io_hps_io_usb1_inst_STP		(HPS_USB_STP),
	.hps_io_hps_io_usb1_inst_DIR		(HPS_USB_DIR),
	.hps_io_hps_io_usb1_inst_NXT		(HPS_USB_NXT)
);

endmodule

