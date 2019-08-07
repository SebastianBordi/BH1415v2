//

#include "hardware.h"

void main (){
    char i = 0;
    transEn = 0;
    __delay_ms(100);
    config();
    getData();
    beep(10);
    if(!btnMenu){                           //Si al iniciar se encuenta presionado
        __delay_ms(25);                     // el boton de menu. Entra en modo de
        if(!btnMenu) setFrequency();        // configuracion de frecuencia.
    }
    beep(100);
    for(i; i < 50; i++){
        sprintf(lineOne,"Fijando Fcia.   ");
        __delay_ms(90);
    }
    //Bucle de repeticion infinita
    sprintf(lineOne,"AMPRO      EX-FM");
    while(1){
        if(functionalStat == UN_BLOCKED){
            transEn = 1;
        }else{
            transEn = 0;
        }
        writeFrequency(frequency);
        __delay_ms(10);
    }
}
//Subrrutina de configuracion 
void config(){
    //Conf del Oscilador
    OSCCON  = 0b00000000;
    //Conf de interrupciones
    INTCON  = 0b01100000;
    PIE1    = 0b00000011;
    //Configuracion de los pines
    TRISA   = 0b00011111;
    TRISB   = 0b00000000;
    TRISC   = 0b11111111;
    TRISD   = 0b00000000;
    TRISE   = 0b00001000;
    //Configuracion Timer 1 (100ms)
    T1CON   = 0b10110001;
    //Configuracion Timer 0 (25ms)
    T0CON   = 0b10000000;
    //Configuracion del ADC
    ADCON0  = 0b11000001;
    ADCON1  = 0b01001110;
    //Inicializacion del LCD
    initLCD();
    //Inicializacion de procesos 
    ADCON0bits.GO = 1;      //Inicia primera conversion AD
    T1CONbits.TMR1ON = 1;   //Inicia timer 1
    INTCONbits.GIE = 1;
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
        if(cntTmr1 == 3){     //cada 400ms
            cntTmr1 = 0;
            updateLCD();        //actualiza los valores en el display
        }
        PIR1bits.TMR1IF = 0;
    }
    //Testea si la interrupcion fue por el timer 0
    if(INTCONbits.TMR0IF == 1){
        TMR0H   = prTmr0H;
        TMR0L   = prTmr0L;
        vumeter(ADRESH);    //muestra el valor de entrada en el vumetro
        ADCON0bits.GO = 1;
        INTCONbits.TMR0IF = 0;
    }
    
    INTCONbits.GIE = 1;
    return;
}
//Muestreo del vumetro
void vumeter (unsigned char vumLevel){
    unsigned char level = 0;
    //Convierte el valor de 0 - 255 a una escala 0 - 8
    if      (vumLevel >= 200) level = 8;
    else if (vumLevel >= 125) level = 7;
    else if (vumLevel >= 75) level = 6;
    else if (vumLevel >= 60) level = 5;
    else if (vumLevel >= 45) level = 4;
    else if (vumLevel >= 15) level = 3;
    else if (vumLevel >= 5) level = 2;
    else if (vumLevel >= 1) level = 1; 

    vum = 0xFF80 >> level;
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
    sprintf(lineOne,"Seleccione Fcia.");
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
                beep(100);
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
                beep(100);
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
    return;
}