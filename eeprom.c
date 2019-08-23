//

#include "hardware.h"


//Subrrutina que lee la EEPROM interna
unsigned char readEEPROM (unsigned char address){
    EEADR = address;
    EECON1bits.EEPGD = 0;
    EECON1bits.RD = 1;
    return EEDATA;
}
//Subrrutina que escribe la EEPROM interna
void writeEEPROM(unsigned char data, unsigned char address){
    EEADR = address;
    EEDATA = data;

    EECON1bits.EEPGD = 0;
    EECON1bits.WREN = 1;

    INTCONbits.GIE = 0;
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    INTCONbits.GIE = 1;
    
    while(PIR2bits.EEIF == 0) continue;
    return;
}
//Subrrutina de lectura de informacion 
void getData (){
    unsigned char aux0 = readEEPROM(0);
    unsigned char aux1 = readEEPROM(1);
    frequency = 0;
    frequency = (aux1 << 8) | aux0;
    months = readEEPROM(2);
    days = readEEPROM(3);
    hours = readEEPROM(4);
    minutes = readEEPROM(5);
    functionalStat = readEEPROM(6);
    return;
}
//subrrutina de escritura de informacion
void setData(unsigned char full){
    unsigned char data0 = frequency & 0x00FF;
    unsigned char data1 = frequency >> 8;
    writeEEPROM(data0 ,0);
    writeEEPROM(data1 ,1);
    writeEEPROM(months ,2);
    writeEEPROM(days ,3);
    writeEEPROM(hours ,4);
    writeEEPROM(minutes ,5);
    writeEEPROM(functionalStat ,6);
    return;
}