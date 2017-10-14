/************************************************************************************************ 
 * Copyright 2010 K. Wiersma, A. Sterk, A. Ball
 * ENGR 325L-A
 * Completed 21 October 2010
 * 
 * Purpose: Implements the login app, optimized for the NIOS2 processor.
 *
 * Requires: NIOS2, JTAG_UART, up_avalon_character_lcd, up_avalon_rs232
 *
 * Software: terminal communications app, serial settings: baud 9600, no flow control, no parity
 *
 * Libraries: loginApp.h, alt_stdio.h, unistd.h, system.h, altera_up_avalon_character_lcd.h, 
 *            altera_up_avalon_rs232.h, login_check.h, rot13.h
 *************************************************************************************************/

#include "loginApp.h"

int main(void)
{
  alt_up_character_lcd_dev * char_lcd_dev;                                         // create a pointer to the LCD display device
  // open the Character LCD port
  char_lcd_dev = alt_up_character_lcd_open_dev ("/dev/up_avalon_character_lcd_0"); // open the Character LCD Display
  /* Initialize the character display */
  alt_up_character_lcd_init (char_lcd_dev);
  alt_up_character_lcd_string(char_lcd_dev, "Greetings human...");                 // write the first row
  char second_row[] = "We come in peace\0";                                        // create an aray to hold the 2nd row
  alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 1);                         // adjust the cursor position
  alt_up_character_lcd_string(char_lcd_dev, second_row);                           // write the second row on the LCD
  usleep(2000000);                                                                 // briefly sleep to allow time for the user to see
                                                                                   // the message.
  alt_up_character_lcd_init (char_lcd_dev);                                        // clear the display
  alt_up_character_lcd_string(char_lcd_dev, "User:");                              // display the user prompt
  
  alt_up_rs232_dev* rs232_dev;                                                     // create a pointer to the rs232 device
  
  rs232_dev = alt_up_rs232_open_dev("/dev/up_avalon_rs232_0");                     // open the rs232 device
  
  alt_u8 storeUser[8] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };                // create a buffer to hold the entered username
  alt_u8* username = &storeUser[0];                                                // make a pointer to the head of the username buffer
  alt_u8 storePass[8] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };                // create a buffer to hold the entered password
  alt_u8* password = &storePass[0];                                                // make a pointer to the head of the password buffer
  alt_u8 readdata = 0;                                                             // create a variable to hold a newly read character
  alt_u8* read = &readdata;                                                        // make a pointer to the new character data
  alt_u8 CR = 0xD;  // the hex code for a carriage return
  alt_u8 LF = '\n'; // line feed
  alt_u8 AS = '*';  // asterisk for password masking
  unsigned readNum = 0; // holds the number of characters read
  unsigned namelen = 0; // holds the length of the read username
  unsigned passlen = 0; // holds the length of the password length
  unsigned goodlogin = 0; // holds a variable to indicate the login is valid
  unsigned validUser = 0; // variable to indicate a valid user
  
  alt_u8 userPrompt[5] = { 'U', 's', 'e', 'r', ':' }; // create an array to print user to the rs232
  echo( rs232_dev, &userPrompt, 5);                   // echo the prompt out the rs232 port

while( goodlogin == 0 ) {                             // loop until we have a good login

    while( *read != CR ){                             // until we see a carriage return, loop
      readNum = alt_up_rs232_get_used_space_in_read_FIFO( rs232_dev ); // find out if there is anything in the rs232 fifo
      if( readNum != 0 ){                                              // once we find something...
        alt_up_rs232_read_data( rs232_dev, read);                      // read it from the rs232 devvice and store it in 'read'
        if( *read != CR ) {                                            // if it isn't a carriage return
          alt_up_rs232_write_data( rs232_dev , *read);                 // write the data back to the rs232 (echo to terminal)
          alt_up_character_lcd_write(char_lcd_dev, read, 1);           // write the character to the character LCD display
          username[namelen++] = *read;                                 // add the character to the username buffer and increment size
        }
      }
    }
    
//    echo(rs232_dev, &storeUser, namelen);
    
    validUser = check_user(username, namelen);                         // check if our user is valid
    alt_printf( "UID: %d", validUser );                                // print out the returned UID
    if( validUser != 0 ) {                                             // assuming we have a valid user, lets get a password
      alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 1);         // move cursor to the 2nd line of the LCD display
      alt_up_character_lcd_string(char_lcd_dev, "Pass:");              // write prompt to display
      alt_u8 passPrompt[5] = { 'P', 'a', 's', 's', ':' };              // create array to write prompt to rs232
      
      alt_up_rs232_write_data( rs232_dev , 0xA  );                     // move rs232 to next line
      
      echo( rs232_dev, &passPrompt, 5);                                // write password prompt to the rs232
      *read = 0;                                                       // NULL the incoming character variable
      while( *read != CR ){                                            // until we see a carriage return loop
       readNum = alt_up_rs232_get_used_space_in_read_FIFO( rs232_dev ); // check if there is anything in the FIFO for us to use
        if( readNum != 0 ){                                             // if we find something in the fifo
          alt_up_rs232_read_data( rs232_dev, read);                     // grab it
          if( *read != CR ){                                            // if it isn't a carriage return
            alt_up_rs232_write_data( rs232_dev , AS);                   // write out an asterisk
            alt_up_character_lcd_write(char_lcd_dev, &AS, 1);           // display an asterisk
            password[passlen++] =  rot13(read);                         // rot13 the incoming character and store it in the password
                                                                        // password buffer, incrementing the size of the password.
          } 
        }
      }
      unsigned validPass = 0;                                           // create memory to validate password
      validPass = check_pass(validUser, password, passlen);             // check if the password we have is valid
      if( validPass == 1 ){                                             // if the password is valid
          alt_up_character_lcd_init (char_lcd_dev);                     // clear the LCD display
          alt_up_character_lcd_string(char_lcd_dev, "WELCOME TO THE");  // print a welcome message
          alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 1);      // move the cursor to the next line of the display
          alt_up_character_lcd_string(char_lcd_dev, "MATRIX, ");        // write the next line of the display
          alt_up_character_lcd_write(char_lcd_dev, username, namelen);  // write the username to the display
          
          alt_u8 welPrompt[7] = { 'W', 'E', 'L', 'C', 'O','M','E' };    // print welcome message to the rs232
          alt_up_rs232_write_data( rs232_dev , 0xA  );
          echo( rs232_dev, &welPrompt, 7);
          
          goodlogin = 1;                                                // we have a good login and can break the loop
          usleep(2000000);                                              // sleep for a bit so the user can see the message
          
      } else {
        printFailMessage( char_lcd_dev );                               // if the password isn't valid, print an error message
      }
    } else {
      printFailMessage( char_lcd_dev );                                 // if the user isn't valid, print an error message
    }
  }
  printWelcomeMessage( char_lcd_dev, username, namelen);                // assuming that all works, print the welcome prompt

  alt_up_character_lcd_init (char_lcd_dev);                             // clear the display
  alt_up_character_lcd_string(char_lcd_dev, "MATRIX$:");                // print the matrix prompt
  alt_up_character_lcd_cursor_blink_on(char_lcd_dev);                   // start the cursor blinking on the display
}

/***************************************************************************************************************/
// HELPER FUNCTIONS
void printFailMessage( alt_up_character_lcd_dev * char_lcd_dev ){
    alt_up_character_lcd_init (char_lcd_dev);
    alt_up_character_lcd_string(char_lcd_dev, "YOU FAIL!!");
    alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 1);
    alt_up_character_lcd_string(char_lcd_dev, "Please try again");
    usleep(20000000);
}

void printWelcomeMessage( alt_up_character_lcd_dev * char_lcd_dev, alt_u8* username, unsigned namelen){
  alt_up_character_lcd_init (char_lcd_dev);
  alt_up_character_lcd_string(char_lcd_dev, "WELCOME TO THE");
  alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 1);
  alt_up_character_lcd_string(char_lcd_dev, "MATRIX, ");
  alt_up_character_lcd_write(char_lcd_dev, username, namelen);
  usleep(2000000);
}

void echo( alt_up_rs232_dev* rs232_dev, alt_u8* messageToPrint, unsigned length ){
    unsigned j = 0;
    for ( j = 0; j < length; j++ ) {
        alt_up_rs232_write_data( rs232_dev, messageToPrint[j]);
    }
}
