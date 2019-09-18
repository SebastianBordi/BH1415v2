
#include "hardware.h"

void uartMenu (char inst){
    char buffer[10];
    int value = 0;
    switch(inst){
        case'd':
            while(!dataAvailable)continue;
            readUart(buffer);
            value = atoi(buffer);
            break;
            
        case 'f':
            while(!dataAvailable)continue;
            readUart(buffer);
            value = atoi(buffer);
            break;
            
    }
}

void putch(char data) {
    while( ! TXIF) // check buffer
        continue; // wait till ready
    TXREG = data; // send data
}

char dataAvailable(void){
    return 1;
}

void readUart(char *buff){
    
}