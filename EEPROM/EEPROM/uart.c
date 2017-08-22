#include <avr/io.h>
void uart_initialize()
{
	unsigned int baudrate_value=103;
	UBRRL=baudrate_value;                             //Loading the baud rate generator registers
	UBRRH=(baudrate_value>>8);

	UCSRB=(1<<TXEN)|(1<<RXEN);                       //Enabling transmission and reception

	UCSRC=(1<<URSEL)|(3<<UCSZ0);                    //accessing the UCSRC register and selecting 8 bit data format
}

void write_uart(unsigned char data)
{
	while(!(UCSRA&(1<<UDRE)));
	UDR=data;
	while(!(UCSRA&(1<<TXC)));
}

unsigned char read_uart()
{
	unsigned char data;
	while(!(UCSRA&(1<<RXC)));
	data=UDR;
	return data;
}

void write_uart_strg(char *ptr)
{
	while(*ptr)
	{
		write_uart(*ptr);
		ptr++;
	}
}
