/*
 * File name:EEPROM.c
 *
 * Created: 21/06/2017 10:24:56 AM
 * Author : Etiq Technologies
 * Description:  This Project interfaces AT24C512 EEPROM with ATmega32 microcontroller.
   AT24C512 has 512 pages of 128 bytes each.Program benefits writing to individual bytes and to a whole page.
   A single character can be written to any one byte numbered(0-127)  in any of the 
   512 pages numbered (0-511).A sentence or a paragraph (not more than 128 characters)
   can be written to any one page by selecting the page write option.the data in a single byte 
   or in a page can be read with byte read or page read option.It should be noted that selecting 
   an invalid page number and byte will not yield expected result.
 
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#include "uart.h"                             //uart library
#include "TWI.h"                              //TWI library
/****************************************EEPROM FUNCTIONS*****************************************/
/************eeprom in write mode*******************/
void EE_write_mode()
{
TWI_send(0xA6);	
}
/******************eeprom in read mode**************/
void EE_read_mode()
{
TWI_send(0xA7);
}

/***********************writes a character to a byte in a page*****************/
void EE_byte_write(unsigned int page,unsigned char byte,unsigned char data )
{
unsigned int byte_adress;                              //byte address   
byte_adress=(page*128)+byte;                           //generates  16 bit byte address from page num and byte number
TWI_start();                                           //start TWI
EE_write_mode();
TWI_send(byte_adress>>8);                              //MSB of byte address(data word address)
TWI_send(byte_adress);                                  //LSB of byte address(data word address)

TWI_send(data);                                        //input character to eeprom
TWI_stop();                                            //stops TWI

_delay_ms(100);	                                       //delay for internal write cycle of EEPROM
}
/*******************Reads data from single byte function returs read data*********************/
unsigned char EE_byte_read(unsigned int page,unsigned char byte)
{
unsigned int byte_adress;                             //byte address 
unsigned char data;
byte_adress=(page*128)+byte;                         //generates  16 bit byte address from page num and byte number
TWI_start();
EE_write_mode();                                     //access in write mode
TWI_send(byte_adress>>8);                            //MSB of byte address(data word address)
TWI_send(byte_adress);                              //LSB of byte address(data word address)
													
TWI_start();                                        //restart TWI
EE_read_mode();                                     //access in read mode
data=TWI_read_nack();                               //read with NACK
TWI_stop();	                                        //stops TWI
write_uart(data);                                   //writes the data to uart
return data;
}
/*************************receive data from keyboard and write to page until enter button pressed************/
void EE_page_write(unsigned int page)
{
unsigned int byte_adress;                            //byte address 
unsigned char i=0,data;
byte_adress=page*128;                                 //generates  16 bit byte address from page num
TWI_start();
EE_write_mode();                                     //access in write mode
TWI_send(byte_adress>>8);                            //MSB of byte address(data word address)
TWI_send(byte_adress);                               //LSB of byte address(data word address)
write_uart_strg("\n\rPress Enter after data entry\r\n"); 
while((data!='\r')&&(i<128))                        //enter data from keyboard until enter button pressed 
{                                                   //or number of characters reached 128
data=read_uart();
//write_uart(data);                                //uncommenting can check whether entered character is right
TWI_send(data);                                    //writes to eeprom
i++;	
}
TWI_stop();                                        //TWI stop
_delay_ms(100);	                                   //internal write cycle
}

/*********************reads data from a page until last valid data enterd or 128 characters reached ie,page ends***************/
void EE_page_read(unsigned int page)
{	
unsigned int byte_adress;                           //byte address 
unsigned char data,i=0;
byte_adress=(page*128);                              //generates  16 bit byte address from page num
TWI_start();
EE_write_mode();                                     //access in write mode
TWI_send(byte_adress>>8);                            //MSB of byte address(data word address)
TWI_send(byte_adress);                               //LSB of byte address(data word address)
TWI_start();                                          //restart TWI
EE_read_mode();                                       //access in read mode   
while((data!='\r')&&(i<128))                         //read data from eeprom until last data 
{                                                    //or number of characters reached 128
data=TWI_read_ack();
write_uart(data);                                   //writes the data to uart
i++;	
}
data=TWI_read_nack();                                //last read data
TWI_stop();                                          //stops TWI
}

/************************** MAIN ***************************/
int main(void)
{
	unsigned char choice,choice1,choice2,page,byte,data;
    uart_initialize();              //initializes the uart
	TWI_init();                     //initialize TWI
	while(1)
	{
		
		
	choice=0;choice1=0;choice2=0;	
		
	write_uart_strg("SELECT YOUR CHOICES ACCORDING TO THE PROMPT\n\r1.EEPROM Write\n\r2.EEPROM Read\r\n");                     //Enter whether write or read
	choice=read_uart();
	write_uart(choice);
	write_uart_strg(". Selected\r\n");
	if(choice=='1')                                              //if write selected
	{
		write_uart_strg("\n\r1.Byte write\n\r2.Page write\r\n");     //enter whether byte write or page write
		choice1=read_uart();
    	write_uart(choice1);
		write_uart_strg(". Selected\r\n");     //enter whether byte write or page write
		
		if(choice1=='1')                                        //if byte write selected
		{   
			write_uart_strg("\n\rEnter page no & byte\r\n");        //enter the page number and number of byte
			write_uart_strg("Page No:\t");
			page=read_uart();
			write_uart(page);
			write_uart_strg("\r\n");
			write_uart_strg("Byte:\t");
			byte=read_uart();                                   //read the page number and number of byte
			write_uart(byte);
			write_uart_strg("\r\n");
			write_uart_strg("\n\rEnter data\r\n");                  //enter data
			data=read_uart();
			EE_byte_write(page,byte,data);                      //function to write byte to the eeprom
		}
		
		else if(choice1=='2')                                    //if page write selected
		{
		
			write_uart_strg("\n\rEnter page no:\t");               //read the page number 
			page=read_uart();
			write_uart(page);
			write_uart_strg("\r\n");
			EE_page_write(page);                                 //function to write data from keyboad to the eeprom page
			write_uart_strg("\r\n");
		}
		
	}
	
	else if(choice=='2')                                              //if read selected
	{
		write_uart_strg("\n\r1.Byte read\n\r2.Page read\r\n");           //enter whether byte read or page read
		choice2=read_uart();
		write_uart(choice2);
		write_uart_strg(". Selected\r\n");
		
		if(choice2=='1')                                             //if byte read selected
		{
			write_uart_strg("\n\rEnter page no & byte\r\n");            //enter the page number and number of byte
			write_uart_strg("Page No:\t");
			page=read_uart();
			write_uart(page);
			write_uart_strg("\r\n");
			write_uart_strg("Byte:\t");
			byte=read_uart();                                   //read the page number and number of byte
			write_uart(byte);
			write_uart_strg("\r\nResult:");
			EE_byte_read(page,byte);                                  //function to read  byte from the eeprom
			write_uart_strg("\r\n");
		}
		else if(choice2=='2')                                        //if page read selected
		{   
			write_uart_strg("\n\rEnter page no:\t");
			page=read_uart();
			write_uart(page);
			write_uart_strg("\r\nResult: \t");
			EE_page_read(page);                                     //function to read data from eeprom  page
			write_uart_strg("\r\n");
		}
		
		
	}	
		
		
		
	}
	
}

