
#include <avr/io.h>
void TWI_init()                                      //TWI initialization with desired SCL frequency
{
	TWBR=32;
	
}

unsigned char TWI_read_ack()                        //TWI recieve data with ack
{
	unsigned char data;
	TWCR=0xC4;                                          //clear flag and TWIE set
	while(!(TWCR&(1<<TWINT)));                          //wait until the flag sets ie end of recieve condition
	data=TWDR;                                           //read data
	return data;
}

unsigned char TWI_read_nack()                    //TWI recive last byte with NACK
{
	unsigned char data;
	TWCR=0x84;                                 //clear flag and TWIE set
	while(!(TWCR&(1<<TWINT)));                 //wait until the flag sets ie end of recieve condition
	data=TWDR;                                 //read data
	return data;
}

void TWI_start()                                    //generate a start condition on the TWI bus
{
	TWCR=0xA4;                                         //value to be loaded to clear TWINT flag and start START condition
	while(!(TWCR&(1<<TWINT)));                         //wait until the flag sets ie end of start condition
}

void TWI_stop()
{
	TWCR=0x94;                                         //value to be loaded to clear TWINT flag and start STOP condition
	//while(!(TWCR&(1<<TWINT)));                         //wait until the flag sets ie end of stop condition
}

void TWI_send(unsigned char data)                 //sends data into the slave
{
	TWDR=data;                                        //data to be sent
	TWCR=0x84;                                        //interrupt flag is cleared
	while(!(TWCR&(1<<TWINT)));	                      //wait until flag sets
	//TWCR=0x84;
}