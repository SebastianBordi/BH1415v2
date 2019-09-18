//

#include "hardware.h"

void main (){
    unsigned char index;
    __delay_ms(100);
    config();
    __delay_ms(100);
    beep(100);
    getData();
    beep(10);
    if(!btnMenu){                           //Si al iniciar se encuenta presionado
        __delay_ms(25);                     // el boton de menu. Entra en modo de
        if(!btnMenu) setFrequency();        // configuracion de frecuencia.
    }
    start();
    lockingScreen();
    writeFrequency(frequency);  //Se esccribe la frecuencia y se espera 
    
    //Muestra el mensaje por defecto
    principalScreen();
    while(1){
//        if(functionalStat == UN_BLOCKED){
//            setTransStat(1);
//        }else{
//            setTransStat(1);
//        }
        if(!btnMenu){                           
            __delay_ms(500);
            for(i = 0; i < 10; i++){
                __delay_ms(100);
            }
            if(!btnMenu){
                setFrequency();
                principalScreen();                    
            }
        }
    }
}
//Subrrutina de configuracion 
void config(){
    //Conf de interrupciones
    INTCON  = 0b01000000;
    PIE1    = 0b00100001;   //Interrupcion por timer y por recepcion 
    //Configuracion de los pines
    TRISA   = 0b00111000;
    TRISB   = 0b00000000;
    TRISC   = 0b10000000;
    PORTA   = 0x00;
    PORTB   = 0x00;
    PORTC   = 0x0E;
    //Configuracion Timer 1 (100ms)
    T1CON   = 0b00010001;
    //Configuracion del ADC
    ADCON0  = 0b00000000;
    ADCON1  = 0b00000000;
    ANSEL   = 0b00000000;
    ANSELH  = 0b00000000;
    //Configuracion del puerto serie
    TXSTA   = 0b00100100;   //Transmision y recepcion habilitadas
    RCSTA   = 0b10010000;   // 9600 bd 8N1
    BAUDCTL = 0b00001000;
    SPBRG   = 0b00011001;
    //Apaga el transmisor 
    transEn = 0;
    pllEn = 1;
    //Inicializacion del LCD
    initLCD();
    //Inicializacion de procesos 
    T1CONbits.TMR1ON = 1;   //Inicia timer 1
    INTCONbits.GIE = 1;
    //Bloqueando transmisor 
    setTransStat(0);
    return;
}
//Subrrutina de configuracion 
void start (){
    char i = 0;
    INTCONbits.GIE = 0;

    /////////////////////////       TESTING SUPPLY
    for(i = 0; i < 10; i++){
        sprintf(lineOne,"TESTING         ");
        sprintf(lineTwo,"          SUPPLY");
        SetDDRamAddr(0x00);
        putsXLCD(lineOne);
        SetDDRamAddr(0x40);
        putsXLCD(lineTwo);
        __delay_ms(100);
    }
    /////////////////////////       TESTING TEMPERATURE
    for(i = 0; i < 18; i++){
        sprintf(lineOne,"TESTING         ");
        sprintf(lineTwo,"     TEMPERATURE");
        SetDDRamAddr(0x00);
        putsXLCD(lineOne);
        SetDDRamAddr(0x40);
        putsXLCD(lineTwo);
        __delay_ms(100);
    }
    /////////////////////////       TESTING SWR
    for(i = 0; i < 15; i++){
        sprintf(lineOne,"TESTING         ");
        sprintf(lineTwo,"             SWR");
        SetDDRamAddr(0x00);
        putsXLCD(lineOne);
        SetDDRamAddr(0x40);
        putsXLCD(lineTwo);
        __delay_ms(100);
    }
    beep(250);
    INTCONbits.GIE = 1;
    __delay_ms(500);
    INTCONbits.GIE = 1;
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
        updateLCD();
        PIR1bits.TMR1IF = 0;
    }    
    if(PIR1bits.RCIF == 1)
        if(RCREG == '$'){
            
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
            if(minutes >= 60){
                minutes = 0;
                hours++;
                setData(0);
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
    SetDDRamAddr(0x00);
    putsXLCD(lineOne);
    beep(200);
    while(!btnMenu)continue;           //Espera a que se suelte el boton 

    while(1){
        if(!btnUp){                                     //Detecta boton de subir
            if(frequency <= 1081)frequency++;
            if(frequency > 1081)frequency = 875;
            beep(100);
            __delay_ms(150);
            while(!btnUp){                              //Detecta si aun se mantiene 
                if(frequency <= 1081)frequency++;        // presionado
                if(frequency > 1081)frequency = 875;
                __delay_ms(50);
            }
        }

        if(!btnDown){                                   //Detecta boton de bajar
            if(frequency >= 875)frequency--;
            if(frequency < 875)frequency = 1081;
            beep(100);
            __delay_ms(150);
            while(!btnDown){                            //Detecta si aun se mantiene
                if(frequency >= 875)frequency--;         // presionado
                if(frequency < 875)frequency = 1081;
                __delay_ms(50);
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

    isOnConfig = 0;
    lockingScreen();
    writeFrequency(frequency);  //Se esccribe la frecuencia y se espera 
    return;
}

void setTransStat (int stat){
    if(stat == 1){
        transEn = 1;
        pllEn = 0;
        __delay_ms(2);
    }else{
        transEn = 0;
        pllEn = 1;
        __delay_ms(2);
    }
}

void principalScreen(){
    SetDDRamAddr(0x00);
    putrsXLCD("AMPRO      EX-FM");
    unsigned char decim = frequency % 10;
    unsigned char integ = frequency / 10;
    sprintf(lineTwo,"FREQ.  %3d.%d MHz",integ, decim);
    SetDDRamAddr(0x40);
    putsXLCD(lineTwo);
}

void lockingScreen(){
    //Indica en la pantalla que se esta fijando la frecuencia
    sprintf(lineOne,"LOCKING         ");
    sprintf(lineTwo,"       FREQUENCY");
    SetDDRamAddr(0x00);
    putsXLCD(lineOne);
    SetDDRamAddr(0x40);
    putsXLCD(lineTwo);
    return;
}