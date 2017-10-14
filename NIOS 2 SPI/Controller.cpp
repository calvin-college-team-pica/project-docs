/*
 * Controller.cpp
 *
 *  Created on: Mar 8, 2011
 *      Author: Kendrick Wiersma
 */

#include <altera_avalon_spi.h>
#include <altera_up_avalon_character_lcd.h>
#include <altera_up_avalon_rs232.h>
#include <unistd.h>
#include <sys/alt_stdio.h>
#include <system.h>

int main() {
  alt_up_character_lcd_dev* char_lcd_dev;
  char_lcd_dev = alt_up_character_lcd_open_dev( "/dev/up_avalon_character_lcd_0");
  alt_up_character_lcd_init(char_lcd_dev);
  
    alt_u8* myBuff;
    alt_u8* myWrite;
    &myWrite[0] = 0x16; // address of the thing we want to read
    
	for(;;){
		int myDumbValue = alt_avalon_spi_command(SPI_0_BASE, 0,
							 2,            // write 2 bytes 
                                                         myWrite,      // data write buffer
                                                         2,            // read 2 bytes 
                                                         myBuff,       // store read data here
                                                         0 );
		alt_up_character_lcd_write(char_lcd_dev, &myBuff, 1);
		usleep(2000000);
	}

}



