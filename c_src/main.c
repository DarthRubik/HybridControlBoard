#include "src/SpiController.h"
#include "src/Adc.h"
#include "stdio.h"
#include "src/address_map.h"
#include "src/defines.h"

#define CAN_SPI_PORT (((SpiHw_t*)0xff200220))
#define ADC_SPI_PORT (((SpiHw_t*)0xff200240))
#define SPI_SELECT      ((Pio_t*)0xff200200)
#define USER_INTERRUPT_BASE 72
#define CAN_CHIP_0 0
#define CAN_CHIP_1 1
#define CAN_CHIP_2 2
#define ADC_CHIP_0 3
#define ADC_CHIP_1 4

#define ADC_INTERRUPT (0+USER_INTERRUPT_BASE)


#define LS_SW_PORT (((Pio_t*)0xff200120))

#ifdef USE_INTERRUPTS
#define INTERRUPT __attribute__((interrupt))
#else
#define INTERRUPT
#endif
void set_A9_IRQ_stack(void);
void enable_A9_interrupts(void);
void config_GIC(void);
void config_interrupt (int N, int CPU_target);
static SpiTarget_t spiTargets[] =
{
	{
		CAN_SPI_PORT,
		0,
		0
	},
	{
		CAN_SPI_PORT,
		0,
		1,
	},
	{
		CAN_SPI_PORT,
		0,
		2,
	},
	{
		ADC_SPI_PORT,
		1,
		0,
	},
	{
		ADC_SPI_PORT,
		1,
		1,
	},
};

SpiController_t spi;
int main(void)
{

	set_A9_IRQ_stack();
	config_GIC();
	ADC_SPI_PORT->control |= 1<<7;
	SpiPackage_t spipackage;
	void* spiBuffer[10];
	InitializeSpi(&spipackage,spiTargets,SPI_SELECT,
			sizeof(spiTargets)/sizeof(spiTargets[0]));
	InitializeSpiController(&spi,&spipackage,spiBuffer,10);
	Adc_t adc;
	InitializeAdc(&adc,&spi,ADC_CHIP_0,1<<1);
	enable_A9_interrupts();

	while (1){
		UpdateAdc(&adc);
		printf("Reading: %i\n", GetAdcReading(&adc,1));
	}

}
void AdcSpiInterrupt(void)
{
	ADC_SPI_PORT->status &= ~(1<<7);
	SpiChunkSet(&spi,ADC_SPI_PORT->rxdata);
}
#ifdef USE_INTERRUPTS
void INTERRUPT __cs3_isr_dabort(void)
{
	int x = 1;
	while (x){}
	return;
}

// Define the IRQ exception handler
void INTERRUPT __cs3_isr_irq (void)
{
	// Read the ICCIAR from the processor interface 
	int address = MPCORE_GIC_CPUIF + ICCIAR; 
	int int_ID = *((volatile int *) address); 
   
	if (int_ID == ADC_INTERRUPT)			
		AdcSpiInterrupt();
	else while(1);

	// Write to the End of Interrupt Register (ICCEOIR)
	address = MPCORE_GIC_CPUIF + ICCEOIR;
	*((volatile int *) address) = int_ID;

	return;
} 
/* 
 * Initialize the banked stack pointer register for IRQ mode
*/
void set_A9_IRQ_stack(void)
{
	int stack, mode;
	stack = A9_ONCHIP_END - 7;		// top of A9 onchip memory, aligned to 8 bytes
	/* change processor to IRQ mode with interrupts disabled */
	mode = INT_DISABLE | IRQ_MODE;
	asm("msr cpsr, %[ps]" : : [ps] "r" (mode));
	/* set banked stack pointer */
	asm("mov sp, %[ps]" : : [ps] "r" (stack));

	/* go back to SVC mode before executing subroutine return! */
	mode = INT_DISABLE | SVC_MODE;
	asm("msr cpsr, %[ps]" : : [ps] "r" (mode));
}
/* 
 * Turn on interrupts in the ARM processor
*/
void enable_A9_interrupts(void)
{
	int status = SVC_MODE | INT_ENABLE;
	asm("msr cpsr, %[ps]" : : [ps]"r"(status));
}

/* 
 * Configure the Generic Interrupt Controller (GIC)
*/
void config_GIC(void)
{
	int address;	// used to calculate register addresses

	config_interrupt(ADC_INTERRUPT,1);

 	// Set Interrupt Priority Mask Register (ICCPMR). Enable interrupts of all priorities 
	address = MPCORE_GIC_CPUIF + ICCPMR;
  	*((volatile int *) address) = 0xFFFF;       

  	// Set CPU Interface Control Register (ICCICR). Enable signaling of interrupts
	address = MPCORE_GIC_CPUIF + ICCICR;
	*((volatile int *) address) = ENABLE;

	// Configure the Distributor Control Register (ICDDCR) to send pending interrupts to CPUs 
	address = MPCORE_GIC_DIST + ICDDCR;
	*((volatile int *) address) = ENABLE;   
}

/* 
 * Configure registers in the GIC for an individual interrupt ID
 * We configure only the Interrupt Set Enable Registers (ICDISERn) and Interrupt 
 * Processor Target Registers (ICDIPTRn). The default (reset) values are used for 
 * other registers in the GIC
*/
void config_interrupt (int N, int CPU_target)
{
	int reg_offset, index, value, address;
    
	/* Configure the Interrupt Set-Enable Registers (ICDISERn). 
	 * reg_offset = (integer_div(N / 32) * 4
	 * value = 1 << (N mod 32) */
	reg_offset = (N >> 3) & 0xFFFFFFFC; 
	index = N & 0x1F;
	value = 0x1 << index;
	address = MPCORE_GIC_DIST + ICDISER + reg_offset;
	/* Now that we know the register address and value, set the appropriate bit */
	*(volatile int *)address |= value;

	/* Configure the Interrupt Processor Targets Register (ICDIPTRn)
	 * reg_offset = integer_div(N / 4) * 4
	 * index = N mod 4 */
	reg_offset = (N & 0xFFFFFFFC);
	index = N & 0x3;
	address = MPCORE_GIC_DIST + ICDIPTR + reg_offset + index;
	/* Now that we know the register address and value, write to (only) the appropriate byte */
	*(volatile char *)address = (char) CPU_target;
}
#endif
