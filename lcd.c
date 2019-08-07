//

#include "hardware.h"

void initLCD (){
    OpenXLCD(FOUR_BIT & LINES_5X7);
    __delay_ms(100);
    SendCMDLCD(CURSOR_OFF);
    SendCMDLCD(SHIFT_DISP_LEFT);
    __delay_ms(50);
    return;
}

void updateLCD(){
    unsigned char integ = frequency / 10;
    unsigned char decim = frequency % 10;
    sprintf(lineTwo,"Fcia.  %3d.%d MHz",integ, decim);

    SetDDRamAddr(0x00);
    putsXLCD(lineOne);
    SetDDRamAddr(0x40);
    putsXLCD(lineTwo);
    return;
}
void SendCMDLCD (unsigned char CMD){
    while(BusyXLCD())continue;
    WriteCmdXLCD(CMD);
}