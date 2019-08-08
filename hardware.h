
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// PIC18F452 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1H
#pragma config OSC = HS
#pragma config OSCS = OFF

// CONFIG2L
#pragma config PWRT = ON
#pragma config BOR = OFF
#pragma config BORV = 20

// CONFIG2H
#pragma config WDT = OFF
#pragma config WDTPS = 128

// CONFIG3H
#pragma config CCP2MUX = OFF

// CONFIG4L
#pragma config STVR = ON
#pragma config LVP = OFF

// CONFIG5L
#pragma config CP0 = OFF
#pragma config CP1 = OFF
#pragma config CP2 = OFF
#pragma config CP3 = OFF

// CONFIG5H
#pragma config CPB = OFF
#pragma config CPD = OFF

// CONFIG6L
#pragma config WRT0 = OFF
#pragma config WRT1 = OFF
#pragma config WRT2 = OFF
#pragma config WRT3 = OFF

// CONFIG6H
#pragma config WRTC = OFF   
#pragma config WRTB = OFF   
#pragma config WRTD = OFF 

// CONFIG7L
#pragma config EBTR0 = OFF
#pragma config EBTR1 = OFF
#pragma config EBTR2 = OFF
#pragma config EBTR3 = OFF

// CONFIG7H
#pragma config EBTRB = OFF      

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


//Definiciones de hardware
#define _XTAL_FREQ 12000000        //Oscilador principal 12MHz

#define transEn PORTAbits.RA5

#define vum     PORTB

#define btnMenu PORTCbits.RC0
#define btnUp   PORTCbits.RC1
#define btnDown PORTCbits.RC2

#define outBuzz PORTDbits.RD0

#define bhEnab  PORTEbits.RE0
#define bhClock PORTEbits.RE1
#define bhData  PORTEbits.RE2


/* Display ON/OFF Control defines */
#define DON         0b00001111  /* Display on      */
#define DOFF        0b00001011  /* Display off     */
#define CURSOR_ON   0b00001111  /* Cursor on       */
#define CURSOR_OFF  0b00001101  /* Cursor off      */
#define BLINK_ON    0b00001111  /* Cursor Blink    */
#define BLINK_OFF   0b00001110  /* Cursor No Blink */

/* Cursor or Display Shift defines */
#define SHIFT_CUR_LEFT    0b00000100  /* Cursor shifts to the left   */
#define SHIFT_CUR_RIGHT   0b00000101  /* Cursor shifts to the right  */
#define SHIFT_DISP_LEFT   0b00000110  /* Display shifts to the left  */
#define SHIFT_DISP_RIGHT  0b00000111  /* Display shifts to the right */

/* Function Set defines */
#define FOUR_BIT   0b00101100  /* 4-bit Interface               */
#define EIGHT_BIT  0b00111100  /* 8-bit Interface               */
#define LINE_5X7   0b00110000  /* 5x7 characters, single line   */
#define LINE_5X10  0b00110100  /* 5x10 characters               */
#define LINES_5X7  0b00111000  /* 5x7 characters, multiple line */


//Valores en programacion de la EEPROM 
__EEPROM_DATA(0x7E, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00);

//Instancias de subrrutinas de main.c
void main (void);                   //Subrrutina principal
void config (void);                 //Subrrutina de configuracion del hardware
void __interrupt() inter (void);     //Subrrutina de interrupciones
void vumeter (unsigned char val);   //Subrrutina que muetra el valor en el vumetro
void time (void);
void beep (int ms);
void setFrequency (void);

//Instancias de subrrutinas de lcd.c
void initLCD (void);            //Subrrutina que inicializa el LCD
void updateLCD (void);          //Subrrutina que actualiza el LCD
void SendCMDLCD (unsigned char CMD);  
void lcdVumeter (unsigned char lvl);

//Instancias de subrrutinas de eeprom.c
unsigned char readEEPROM (unsigned char address);
void writeEEPROM(unsigned char data, unsigned char address);
void getData (void);
void setData (unsigned char full);

//Instancias de subrrutinas de bh1415.c
unsigned int codFreq (unsigned int f);
void writeFrequency (unsigned int f);


void DelayFor18TCY(void);
void DelayPORXLCD(void);
void DelayXLCD(void);
void WriteDataXLCD(char data);
void WriteCmdXLCD(unsigned char cmd);
void SetDDRamAddr(unsigned char DDaddr);
unsigned char BusyXLCD(void);
void OpenXLCD(unsigned char lcdtype);
void putsXLCD(char *buffer);
void putrsXLCD(const  char *buffer);

//Instancias de variables globales
unsigned char prTmr1L = 0x84;   //Preloads del Timer 1 
unsigned char prTmr1H = 0x6D;
unsigned char prTmr0L = 0x68;   //Preloads del Timer 0
unsigned char prTmr0H = 0xC5;

unsigned char cntTmr1 = 0;      //Registro de postscaler del Timer1

unsigned int frequency = 0;     //Frecuencia a programar en el BH1514
unsigned char months = 3;            // Meses de funcionamiento
unsigned char days = 19;             // Dias de funcionamiento
unsigned char hours = 22;            // Horas de funcionamiento
unsigned char minutes = 15;          // Minutos de funcionamiento
unsigned char seconds = 0;           // Segundos de funcionamiento
unsigned char hundredMiliSeconds = 0;// Decimas de funcionamiento 
unsigned char functionalStat = 0;    // Estado de funcionamiento (bloqueado o no)
unsigned char stereoEnable = 1;      // Funcion de codificacion estereo
unsigned char isOnConfig = 0;
unsigned char level;

char lineOne[17];
char lineTwo[17];

//Constantes 
const char FINAL_MONTH = 6;
const char FINAL_DAY = 13;
const char BLOCKED = 0;
const char UN_BLOCKED = 1;

