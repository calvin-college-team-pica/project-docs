/*************************************************************** 
 * Copyright 2010 K. Wiersma, A. Sterk, A. Ball
 * ENGR 325L-A
 * Completed 21 October 2010
 * 
 * Purpose: header file for the loginApp.c; provides libraries
 *          and function prototypes.
 ***************************************************************/

#ifndef LOGINAPP_H_
#define LOGINAPP_H_

#include "sys/alt_stdio.h"

#include "unistd.h"
#include "system.h"
#include "altera_up_avalon_character_lcd.h"
#include "altera_up_avalon_rs232.h"
#include "login_check.h"
#include "rot13.h"

void printFailMessage( alt_up_character_lcd_dev * );
void printWelcomeMessage( alt_up_character_lcd_dev *, alt_u8*, unsigned );
void echo( alt_up_rs232_dev*, alt_u8*, unsigned );

#endif /*LOGINAPP_H_*/
