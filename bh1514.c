//

#include "hardware.h"

//Codificacion de la frecuencia al formato del BH1415
unsigned int codFreq (unsigned int f){
    unsigned int fr;
        fr = f | 0b0100100000000000;
    return fr;
}
//Escritura del integrado 
void writeFrequency (unsigned int f){
    unsigned int buffer;
    unsigned int r;
       
    pllEn = 0;
    transEn = 0;
    
    buffer = codFreq(f);
    bhClock = 0;
    bhData = 0;
    bhEnab = 1;
    __delay_ms(1);
    
    for(i = 0; i < 16; i++){
        r = (buffer >> i) & 0x0001;
        __delay_ms(1);
        bhClock = 0;
        if(r == 1){
            bhData = 1;
        }else{
            bhData = 0;
        }
        __delay_ms(1);
        bhClock = 1;
    }
    
    __delay_ms(1);
    bhEnab = 0;
    
    beep(20);
    __delay_ms(500);
    beep(20);
    __delay_ms(500);
    for(i = 0; i < 40; i++){
        __delay_ms(100);
    }
    
    pllEn = 0;
    transEn = 1;
    
    return;
}


