
//#include "lcd_pic16.c"

#include "hardware.h"

// Rutinas de tiempo auxiliares para la libreria XLCD

#define UPPER

#define DATA_PORT PORTB
#define TRIS_DATA_PORT  TRISB

#define TRIS_RS TRISBbits.TRISB1
#define TRIS_RW TRISBbits.TRISB2
#define TRIS_E  TRISBbits.TRISB3

#define RS_PIN  PORTBbits.RB1
#define RW_PIN  PORTBbits.RB2
#define E_PIN   PORTBbits.RB3


void DelayFor18TCY(void) 
{
   __delay_us(18);
}

void DelayPORXLCD(void)
{
   __delay_ms(20); //Delay de 15 ms
}

void DelayXLCD(void)
{
   __delay_ms(20); //Delay de 20 ms
}
/********************************************************************
*       Nombre de la funcion:  OpenXLCD                             *
*       Valor que retorna:   void                                   *
********************************************************************/
void OpenXLCD(unsigned char lcdtype)
{
#ifdef BIT8                             
        DATA_PORT = 0;
        TRIS_DATA_PORT = 0x00;
#else                                   
#ifdef UPPER                            
        DATA_PORT &= 0x0f;
        TRIS_DATA_PORT &= 0x0F;
#else                                   
        DATA_PORT &= 0xf0;
        TRIS_DATA_PORT &= 0xF0;
#endif
#endif
        TRIS_RW = 0;                    
        TRIS_RS = 0;
        TRIS_E = 0;
        RW_PIN = 0;                     
        RS_PIN = 0;                     
        E_PIN = 0;                      

        // Delay  15ms Power on reset
        DelayPORXLCD();
 //-------------------reset por medio del software----------------------       
       WriteCmdXLCD(0x30);
         __delay_ms(5);

       WriteCmdXLCD(0x30);
         __delay_ms(1);


       WriteCmdXLCD(0x32);
      while( BusyXLCD() );
//------------------------------------------------------------------------


        // Set data interface width, # lineas,  tipo de font
        while(BusyXLCD());              // espera si  LCD esta ocupado
        WriteCmdXLCD(lcdtype);          // Funcion para escribir un comando

        // enciende el LCD
        while(BusyXLCD());              // Espera si  LCD esta ocupado
        WriteCmdXLCD(DOFF&CURSOR_OFF&BLINK_OFF); // Display OFF/Blink OFF
        while(BusyXLCD());              // Espera si  LCD esta ocupado
        WriteCmdXLCD(DON&CURSOR_ON&BLINK_ON); // Display ON/Blink ON

        // Limpia display
        while(BusyXLCD());              // Espera si  LCD esta ocupado
        WriteCmdXLCD(0x01);             // Limpia display

        // Set entry mode inc, no shift
        while(BusyXLCD());              // Espera si  LCD esta ocupado
        WriteCmdXLCD(SHIFT_CUR_RIGHT);   // Entra el Modo
       
   while(BusyXLCD());              // Espera si  LCD esta ocupado
        WriteCmdXLCD(0x06);         // Modo Auto Incremento
 
        while(BusyXLCD());              // Espera si  LCD esta ocupado
        SetDDRamAddr(0x80);             // Setea el cursor en posición 0
        
        while(BusyXLCD());              // espera si  LCD esta ocupado
        WriteCmdXLCD(CURSOR_OFF);       // Cursor OFF
        return;
}


/********************************************************************
*       Nombre de la función:  WriteDataXLCD                        *
*       Valor que retorna:   void                                   *
********************************************************************/
void WriteDataXLCD(char data)
{
#ifdef BIT8                             // interface 8-bit 
        TRIS_DATA_PORT = 0;             // Configura el Puerto como salida
        DATA_PORT = data;               // Escribe el dato en el puerto
        RS_PIN = 1;                     // Setea bit RS
        RW_PIN = 0;
        DelayFor18TCY();
        E_PIN = 1;                      // Genera Enable
        DelayFor18TCY();
        E_PIN = 0;
        RS_PIN = 0;                     // Pone a cero RS
        TRIS_DATA_PORT = 0xff;          // Configura el Puerto como entrada
#else                                   // Interface 4-bit
#ifdef UPPER                            // Transfiere Nibble Alto
        TRIS_DATA_PORT &= 0x0f;
        DATA_PORT &= 0x0f;
        DATA_PORT |= data&0xf0;
#else                                   // transfiere Nibble Bajo
        TRIS_DATA_PORT &= 0xf0;
        DATA_PORT &= 0xf0;
        DATA_PORT |= ((data>>4)&0x0f);
#endif
        RS_PIN = 1;                     // Setea bit RS
        RW_PIN = 0;
        DelayFor18TCY();
        E_PIN = 1;                      // Genera el ENABLE
        DelayFor18TCY();
        E_PIN = 0;
#ifdef UPPER                            // Transfiere Nibble Alto
        DATA_PORT &= 0x0f;
        DATA_PORT |= ((data<<4)&0xf0);
#else                                   // Transfiere Nible Bajo
        DATA_PORT &= 0xf0;
        DATA_PORT |= (data&0x0f);
#endif
        DelayFor18TCY();
        E_PIN = 1;                      // Genera el ENABLE
        DelayFor18TCY();
        E_PIN = 0;
#ifdef UPPER                            // Transfiere Nibble Alto
        TRIS_DATA_PORT |= 0xf0;
#else                                   // Transfiere Nible Bajo
        TRIS_DATA_PORT |= 0x0f;
#endif
#endif
        return;
}

/********************************************************************
*       Nombre de la función: WriteCmdXLCD                         *
*       Valor que retorna:   void                                   *
*       Parametros:     cmd: commando para enviar al LCD            *
********************************************************************/
void WriteCmdXLCD(unsigned char cmd){
#ifdef BIT8                             
        TRIS_DATA_PORT = 0;             
        DATA_PORT = cmd;                
        RW_PIN = 0;                     
        RS_PIN = 0;                     
        DelayFor18TCY();
        E_PIN = 1;                      
        DelayFor18TCY();
        E_PIN = 0;
        DelayFor18TCY();
        TRIS_DATA_PORT = 0xff;          
#else                                   
#ifdef UPPER                            
        TRIS_DATA_PORT &= 0x0f;
        DATA_PORT &= 0x0f;
        DATA_PORT |= cmd&0xf0;
#else                                   
        TRIS_DATA_PORT &= 0xf0;
        DATA_PORT &= 0xf0;
        DATA_PORT |= (cmd>>4)&0x0f;
#endif
        RW_PIN = 0;                     
        RS_PIN = 0;
        DelayFor18TCY();
        E_PIN = 1;                      
        DelayFor18TCY();
        E_PIN = 0;
#ifdef UPPER                            
        DATA_PORT &= 0x0f;
        DATA_PORT |= (cmd<<4)&0xf0;
#else                                   
        DATA_PORT &= 0xf0;
        DATA_PORT |= cmd&0x0f;
#endif
        DelayFor18TCY();
        E_PIN = 1;                      
        DelayFor18TCY();
        E_PIN = 0;
#ifdef UPPER                             
        TRIS_DATA_PORT |= 0xf0;
#else
        TRIS_DATA_PORT |= 0x0f;
#endif
#endif
        return;
}

/********************************************************************
*       Nombre de l Funcion:  SetDDRamAddr                          *
*       Valor que retorna:   void                                   *
*       Parametros:   CGaddr: direccion de la Ram de datos del LCD  *
********************************************************************/
void SetDDRamAddr(unsigned char DDaddr)
{
#ifdef BIT8                                     
        TRIS_DATA_PORT = 0;                     
        DATA_PORT = DDaddr | 0b10000000;        
        RW_PIN = 0;                             
        RS_PIN = 0;
        DelayFor18TCY();
        E_PIN = 1;                              
        DelayFor18TCY();
        E_PIN = 0;
        DelayFor18TCY();
        TRIS_DATA_PORT = 0xff;                  
#else                                           
#ifdef UPPER                                    
        TRIS_DATA_PORT &= 0x0f;                 
        DATA_PORT &= 0x0f;                      
        DATA_PORT |= ((DDaddr | 0b10000000) & 0xf0);
#else                                           
        TRIS_DATA_PORT &= 0xf0;                 
        DATA_PORT &= 0xf0;                      
        DATA_PORT |= (((DDaddr | 0b10000000)>>4) & 0x0f);
#endif
        RW_PIN = 0;                             
        RS_PIN = 0;
        DelayFor18TCY();
        E_PIN = 1;                              
        DelayFor18TCY();
        E_PIN = 0;
#ifdef UPPER                                    
        DATA_PORT &= 0x0f;                      
        DATA_PORT |= ((DDaddr<<4)&0xf0);
#else                                           
        DATA_PORT &= 0xf0;                      
        DATA_PORT |= (DDaddr&0x0f);
#endif
        DelayFor18TCY();
        E_PIN = 1;                              // Clock the cmd and address in
        DelayFor18TCY();
        E_PIN = 0;
#ifdef UPPER                                    // Upper nibble interface
        TRIS_DATA_PORT |= 0xf0;                 // Make port input
#else                                           // Lower nibble interface
        TRIS_DATA_PORT |= 0x0f;                 // Make port input
#endif
#endif
        return;
}

/********************************************************************
*       Function Name:  SetCGRamAddr                                *
*       Return Value:   void                                        *
*       Parameters:     CGaddr: character generator ram address     *
********************************************************************/
void SetCGRamAddr(unsigned char CGaddr)
{
#ifdef BIT8                                     // 8-bit interface
        TRIS_DATA_PORT = 0;                     // Make data port ouput
        DATA_PORT = CGaddr | 0b01000000;        // Write cmd and address to port
        RW_PIN = 0;                             // Set control signals
        RS_PIN = 0;
        DelayFor18TCY();
        E_PIN = 1;                              // Clock cmd and address in
        DelayFor18TCY();
        E_PIN = 0;
        DelayFor18TCY();
        TRIS_DATA_PORT = 0xff;                  // Make data port inputs
#else                                           // 4-bit interface
#ifdef UPPER                                    // Upper nibble interface
        TRIS_DATA_PORT &= 0x0f;                 // Make nibble input
        DATA_PORT &= 0x0f;                      // and write upper nibble
        DATA_PORT |= ((CGaddr | 0b01000000) & 0xf0);
#else                                           // Lower nibble interface
        TRIS_DATA_PORT &= 0xf0;                 // Make nibble input
        DATA_PORT &= 0xf0;                      // and write upper nibble
        DATA_PORT |= (((CGaddr |0b01000000)>>4) & 0x0f);
#endif
        RW_PIN = 0;                             // Set control signals
        RS_PIN = 0;
        DelayFor18TCY();
        E_PIN = 1;                              // Clock cmd and address in
        DelayFor18TCY();
        E_PIN = 0;
#ifdef UPPER                                    // Upper nibble interface
        DATA_PORT &= 0x0f;                      // Write lower nibble
        DATA_PORT |= ((CGaddr<<4)&0xf0);
#else                                           // Lower nibble interface
        DATA_PORT &= 0xf0;                      // Write lower nibble
        DATA_PORT |= (CGaddr&0x0f);
#endif
        DelayFor18TCY();
        E_PIN = 1;                              // Clock cmd and address in
        DelayFor18TCY();
        E_PIN = 0;
#ifdef UPPER                                    // Upper nibble interface
        TRIS_DATA_PORT |= 0xf0;                 // Make inputs
#else                                           // Lower nibble interface
        TRIS_DATA_PORT |= 0x0f;                 // Make inputs
#endif
#endif
        return;
}

/********************************************************************
*       Function Name:  ReadDataXLCD                                *
*       Return Value:   char: data byte from LCD controller         *
*       Parameters:     void                                        *
********************************************************************/
char ReadDataXLCD(void)
{
        char data;

#ifdef BIT8                             // 8-bit interface
        RS_PIN = 1;                     // Set the control bits
        RW_PIN = 1;
        DelayFor18TCY();
        E_PIN = 1;                      // Clock the data out of the LCD
        DelayFor18TCY();
        data = DATA_PORT;               // Read the data
        E_PIN = 0;
        RS_PIN = 0;                     // Reset the control bits
        RW_PIN = 0;
#else                                   // 4-bit interface
        RW_PIN = 1;
        RS_PIN = 1;
        DelayFor18TCY();
        E_PIN = 1;                      // Clock the data out of the LCD
        DelayFor18TCY();
#ifdef UPPER                            // Upper nibble interface
        data = DATA_PORT&0xf0;          // Read the upper nibble of data
#else                                   // Lower nibble interface
        data = (DATA_PORT<<4)&0xf0;     // read the upper nibble of data
#endif
        E_PIN = 0;                      // Reset the clock line
        DelayFor18TCY();
        E_PIN = 1;                      // Clock the next nibble out of the LCD
        DelayFor18TCY();
#ifdef UPPER                            // Upper nibble interface
        data |= (DATA_PORT>>4)&0x0f;    // Read the lower nibble of data
#else                                   // Lower nibble interface
        data |= DATA_PORT&0x0f;         // Read the lower nibble of data
#endif
        E_PIN = 0;                                      
        RS_PIN = 0;                     // Reset the control bits
        RW_PIN = 0;
#endif
        return(data);                   // Return the data byte
}

/*********************************************************************
*       Function Name:  ReadAddrXLCD                                 *
*       Return Value:   char: address from LCD controller            *
*       Parameters:     void                                         *
*********************************************************************/
unsigned char ReadAddrXLCD(void)
{
        char data;                      // Holds the data retrieved from the LCD

#ifdef BIT8                             // 8-bit interface
        RW_PIN = 1;                     // Set control bits for the read
        RS_PIN = 0;
        DelayFor18TCY();
        E_PIN = 1;                      // Clock data out of the LCD controller
        DelayFor18TCY();
        data = DATA_PORT;               // Save the data in the register
        E_PIN = 0;
        RW_PIN = 0;                     // Reset the control bits
#else                                   // 4-bit interface
        RW_PIN = 1;                     // Set control bits for the read
        RS_PIN = 0;
        DelayFor18TCY();
        E_PIN = 1;                      // Clock data out of the LCD controller
        DelayFor18TCY();
#ifdef UPPER                            // Upper nibble interface
        data = DATA_PORT&0xf0;          // Read the nibble into the upper nibble of data
#else                                   // Lower nibble interface
        data = (DATA_PORT<<4)&0xf0;     // Read the nibble into the upper nibble of data
#endif
        E_PIN = 0;                      // Reset the clock
        DelayFor18TCY();
        E_PIN = 1;                      // Clock out the lower nibble
        DelayFor18TCY();
#ifdef UPPER                            // Upper nibble interface
        data |= (DATA_PORT>>4)&0x0f;    // Read the nibble into the lower nibble of data
#else                                   // Lower nibble interface
        data |= DATA_PORT&0x0f;         // Read the nibble into the lower nibble of data
#endif
        E_PIN = 0;
        RW_PIN = 0;                     // Reset the control lines
#endif
        return (data&0x7f);             // Return the address, Mask off the busy bit
}





/********************************************************************
*       Function Name:  putsXLCD
*       Return Value:   void
*       Parameters:     buffer: pointer to string
********************************************************************/
void putsXLCD(char *buffer)
{
        while(*buffer)                  // Write data to LCD up to null
        {
                while(BusyXLCD());      // Wait while LCD is busy
                WriteDataXLCD(*buffer); // Write character to LCD
                buffer++;               // Increment buffer
        }
        return;
}

/********************************************************************
*       Function Name:  putrsXLCD
*       Return Value:   void
*       Parameters:     buffer: pointer to string
********************************************************************/
void putrsXLCD(const  char *buffer)
{
        while(*buffer)                  // Write data to LCD up to null
        {
                while(BusyXLCD());      // Wait while LCD is busy
                WriteDataXLCD(*buffer); // Write character to LCD
                buffer++;               // Increment buffer
        }
        return;
}

/********************************************************************
*       Function Name:  BusyXLCD                                    *
*       Return Value:   char: busy status of LCD controller         *
*       Parameters:     void                                        *
********************************************************************/
unsigned char BusyXLCD(void)
{
#ifdef BUSY_LCD
        RW_PIN = 1;                     // Set the control bits for read
        RS_PIN = 0;
        DelayFor18TCY();
        E_PIN = 1;                      // Clock in the command
        DelayFor18TCY();
#ifdef BIT8                             // 8-bit interface
        if(DATA_PORT&0x80)                      // Read bit 7 (busy bit)
        {                               // If high
                E_PIN = 0;              // Reset clock line
                RW_PIN = 0;             // Reset control line
                return 1;               // Return TRUE
        }
        else                            // Bit 7 low
        {
                E_PIN = 0;              // Reset clock line
                RW_PIN = 0;             // Reset control line
                return 0;               // Return FALSE
        }




#else                                   // 4-bit interface
#ifdef UPPER                            // Upper nibble interface
        if(DATA_PORT&0x80)

#else                                   // Lower nibble interface
        if(DATA_PORT&0x08)
#endif
        {
                E_PIN = 0;              // Reset clock line
                DelayFor18TCY();
                E_PIN = 1;              // Clock out other nibble
                DelayFor18TCY();
                E_PIN = 0;
                RW_PIN = 0;             // Reset control line
                return 1;               // Return TRUE
        }
        else                            // Busy bit is low
        {
                E_PIN = 0;              // Reset clock line
                DelayFor18TCY();
                E_PIN = 1;              // Clock out other nibble
                DelayFor18TCY();
                E_PIN = 0;
                RW_PIN = 0;             // Reset control line
                return 0;               // Return FALSE
        }
#endif
#else
        __delay_ms(5);
        return 0;
#endif

}

