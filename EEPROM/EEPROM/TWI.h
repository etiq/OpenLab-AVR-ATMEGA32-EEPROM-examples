#ifndef TWI_H
#define TWI_H

void TWI_init();                                      //TWI initialization with desired SCL frequency


unsigned char TWI_read_ack();                        //TWI recieve data with ack


unsigned char TWI_read_nack();                    //TWI recive last byte with NACK



void TWI_start();                                    //generate a start condition on the TWI bus



void TWI_stop();





void TWI_send(unsigned char data);                 //sends data into the slave



#endif



