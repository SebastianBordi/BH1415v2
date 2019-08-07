//

#include "hardware.h"

//Codificacion de la frecuencia al formato del BH1415
unsigned int codFreq (unsigned int f){
    if(stereoEnable == 1){
        f = f | 0b0100100000000000;
    }else{
        f = f | 0b0100000000000000;
    }
    return f;
}
//Escritura del integrado 
void writeFrequency (unsigned int f){
    unsigned int buffer;
    unsigned int r;
    unsigned char i;

    buffer = codFreq(f);

    bhEnab = 1;
    __delay_ms(1);
    for(i = 0; i < 16; i++){
        r = (buffer >> i) & 0x0001;
        if(r == 1){
            bhData = 1;
        }else{
            bhData = 0;
        }
        __delay_ms(1);
        bhClock = 1;
        __delay_ms(1);
        bhClock = 0;
    }
    __delay_ms(1);
    bhEnab = 0;
    return;
}

