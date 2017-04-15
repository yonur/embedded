#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"

#define SYSCTL_RCGC1_R          (*((volatile uint32_t *)0x400FE104))
#define SYSCTL_RCGC2_R          (*((volatile uint32_t *)0x400FE108))
	
void SendUartString(uint32_t nBase, char *nData)
{
	while(*nData != '\0')
	{
		if(UARTSpaceAvail(nBase))
		{
			while(!UARTCharPutNonBlocking(nBase, *nData));
			nData++;
		}
	}
}

void Int_Uart(void)
{
	char rcv_ch;
	uint32_t IntStatus;
	
	IntStatus = UARTIntStatus(UART0_BASE, true);
	UARTIntClear(UART0_BASE, IntStatus);
	
	rcv_ch = UARTCharGetNonBlocking(UART0_BASE);
	
	if((uint32_t)rcv_ch!=1)
	{
		while(!UARTCharPutNonBlocking(UART0_BASE, rcv_ch));
	}
}

int main(void)
{
	SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_5);
	
  SYSCTL_RCGC1_R |= 0x00000001;  // activate UART0
  SYSCTL_RCGC2_R |= 0x00000001;  // activate port A
	
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	
	IntDisable(INT_UART0);
	UARTDisable(UART0_BASE);
	UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
	UARTConfigSetExpClk(UART0_BASE, 16000000, 115200, UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE);
	UARTFIFODisable(UART0_BASE);
	UARTIntEnable(UART0_BASE, UART_INT_RX|UART_INT_RT);
	UARTIntRegister(UART0_BASE, Int_Uart);
	UARTEnable(UART0_BASE);
	
	IntEnable(INT_UART0);
	
	while(1)
	{
		//SendUartString(UART0_BASE, "\r\nDeneme");
		//SysCtlDelay((SysCtlClockGet()/3000)*500);
	}
}
