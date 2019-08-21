//

#include "hardware.h"

void main (){
    char i = 0;
    __delay_ms(100);
    config();
    beep(100);
    getData();
    beep(10);
    if(!btnMenu){                           //Si al iniciar se encuenta presionado
        __delay_ms(25);                     // el boton de menu. Entra en modo de
        if(!btnMenu) setFrequency();        // configuracion de frecuencia.
    }
    __delay_ms(100);
    writeFrequency(frequency);
    beep(100);
    INTCONbits.GIE = 0;
    for(i; i < 50; i++){
        sprintf(lineOne,"LOCKING         ");
        sprintf(lineTwo,"       FREQUENCY");
        SetDDRamAddr(0x00);
        putsXLCD(lineOne);
        SetDDRamAddr(0x40);
        putsXLCD(lineTwo);
        __delay_ms(90);
    }
    
    INTCONbits.GIE = 1;
    //Bucle de repeticion infinita
    sprintf(lineOne,"AMPRO      EX-FM");
    updateLCD();
    while(1){
        if(functionalStat == UN_BLOCKED){
            setTransStat(1);
        }else{
            setTransStat(0);
        }
        //writeFrequency(frequency);
        __delay_ms(50);
    }
}
//Subrrutina de configuracion 
void config(){
    //Conf de interrupciones
    INTCON  = 0b01000000;
    PIE1    = 0b00000001;
    //Configuracion de los pines
    TRISA   = 0b00111000;
    TRISB   = 0b00000000;
    TRISC   = 0b00000000;
    PORTA   = 0x00;
    PORTB   = 0x00;
    PORTC   = 0x00;
    //Configuracion Timer 1 (100ms)
    T1CON   = 0b00010001;
    //Configuracion del ADC
    ADCON0  = 0b00000000;
    ADCON1  = 0b00000000;
    ANSEL   = 0b00000000;
    ANSELH  = 0b00000000;
    //Inicializacion del LCD
    initLCD();
    //Inicializacion de procesos 
    T1CONbits.TMR1ON = 1;   //Inicia timer 1
    INTCONbits.GIE = 1;
    //Bloqueando transmisor 
    setTransStat(0);
    return;
}
//Subrrutina de interruociones 
void __interrupt() inter (){
    //Deshabilita interrupciones
    INTCONbits.GIE = 0;
    //Testea si la interrupcion fue por el timer 1
    if(PIR1bits.TMR1IF == 1){
        TMR1H   = prTmr1H;
        TMR1L   = prTmr1L;
        cntTmr1++;
        time();
        if(cntTmr1 == 10){     //cada 400ms
            cntTmr1 = 0;
            //beep(10);
            updateLCD();        //actualiza los valores en el display
        }
        PIR1bits.TMR1IF = 0;
    }    
    INTCONbits.GIE = 1;
    return;
}

//Conteo de tiempo transcurrido de funcionamiento
void time (){
    hundredMiliSeconds++;
    if(hundredMiliSeconds >= 10){
        hundredMiliSeconds = 0;
        seconds++;
        if(seconds >= 60){
            seconds = 0;
            minutes++;
            setData(0);
            if(minutes >= 60){
                minutes = 0;
                hours++;
                if(hours >= 24){
                    hours = 0;
                    days++;
                    if(days >= 30){
                        days = 0;
                        months++;
                    }
                    if(days == FINAL_DAY && months == FINAL_MONTH){
                        functionalStat = BLOCKED;
                    }else{
                        functionalStat = UN_BLOCKED;
                    }
                }
            }
        }
    }
}
//Subrrutina de pitidos 5KHz
void beep (int ms){
    ms *= 5;
    for(ms; ms > 0; ms--){
        outBuzz = 1;
        __delay_us(100);
        outBuzz = 0;
        __delay_us(100);
    }
    return;
}
//Seteo de frecuencia 
void setFrequency (){
    isOnConfig = 1;
    sprintf(lineOne,"SET   FREQUENCY ");
    beep(200);
    while(!btnMenu)continue;           //Espera a que se suelte el boton 

    while(1){
        if(!btnUp){                                     //Detecta boton de subir
            if(frequency < 1080)frequency++;
            if(frequency > 1080)frequency = 1080;
            beep(100);
            __delay_ms(500);
            while(!btnUp){                              //Detecta si aun se mantiene 
                if(frequency < 1080)frequency++;        // presionado
                if(frequency > 1080)frequency = 1080;
                __delay_ms(100);
            }
        }

        if(!btnDown){                                   //Detecta boton de bajar
            if(frequency > 880)frequency--;
            if(frequency < 880)frequency = 880;
            beep(100);
            __delay_ms(500);
            while(!btnDown){                            //Detecta si aun se mantiene
                if(frequency > 880)frequency--;         // presionado
                if(frequency < 880)frequency = 880;
                __delay_ms(100);
            }
        }

        if(!btnMenu){                                   //Detecta boton Menu
            __delay_ms(500);
            if(!btnMenu){                               //Si se mantiene por mas de 500ms
                setData(1);                             //Guarda la data de las variables
                beep(1000);                                 
                break;                                  //Y Sale
            }
        }
    }
    writeFrequency(frequency);
    isOnConfig = 0;
    return;
}

void setTransStat (int stat){
    if(stat == 1){
        transEn = 1;
        pllEn = 0;
        __delay_ms(2);
        writeFrequency(frequency);
    }else{
        transEn = 0;
        pllEn = 1;
        __delay_ms(2);
        writeFrequency(730);
    }
}