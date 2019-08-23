
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// PIC16F886 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator: Crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = ON          // Code Protection bit (Program memory code protection is enabled)
#pragma config CPD = ON         // Data Code Protection bit (Data memory code protection is enabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
//Definiciones de hardware
#define _XTAL_FREQ 4000000        //Oscilador principal 12MHz

#define transEn PORTCbits.RC0
 
#define pllEn   PORTCbits.RC4


#define btnMenu PORTAbits.RA4
#define btnUp   PORTAbits.RA3
#define btnDown PORTAbits.RA5

#define outBuzz PORTBbits.RB0

#define LED3    PORTCbits.RC2
#define LED2    PORTCbits.RC3
#define LED1    PORTCbits.RC1

#define bhEnab  PORTCbits.RC5
#define bhClock PORTCbits.RC6
#define bhData  PORTCbits.RC7


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
__EEPROM_DATA(0xD3, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00);

//Instancias de subrrutinas de main.c
void main (void);                   //Subrrutina principal
void start (void);
void config (void);                 //Subrrutina de configuracion del hardware
void __interrupt() inter (void);     //Subrrutina de interrupciones
void vumeter (unsigned char val);   //Subrrutina que muetra el valor en el vumetro
void time (void);
void beep (int ms);
void setFrequency (void);
void setTransStat (int stat);
void principalScreen(void);
void lockingScreen(void);

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
unsigned char prTmr1L = 0xB0;   //Preloads del Timer 1 
unsigned char prTmr1H = 0x3C;

unsigned char cntTmr1 = 0;      //Registro de postscaler del Timer1

unsigned int frequency = 111;     //Frecuencia a programar en el BH1514
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
unsigned char i;

char lineOne[17];
char lineTwo[17];

//Constantes 
const char FINAL_MONTH = 6;
const char FINAL_DAY = 13;
const char BLOCKED = 0;
const char UN_BLOCKED = 1;

