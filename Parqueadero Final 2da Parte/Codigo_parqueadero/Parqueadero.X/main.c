/*
 * File:   main.c
 * Author: Luis Manuel Arroyo - Luis Miguel Romero
 *
 * Creado: 11/12/2020
 */

// Se incluye las librerias necesarias para el funcionamiento del programa 
#include <xc.h>
#include "pic18f4550.h"
#include "fuses.h"

void main(void) {
    ADCON1 = 0x0F;// Se ponen todas los puertos 
    CMCON = 0x07; //de forma digital
    CCP1CON = 0;// no se utiliza pwm
    // Se configuran las entradas/salidas respectivas del puerto b
    TRISB=0b00011111; // RB0~RB4 como entradas y RB5~RB7 como salidas
    LATB =0; // Se inicializa el puerto b con un valor de bajo
    // Se inicializan el resto de puertos
    TRISA = 0;
    LATA = 0;
    TRISC = 0;
    LATC = 0;    
    TRISD = 0;
    LATD = 0;    
    while(1){ // ciclo principal de ejecucion
        // Se verifica el estado del boton de las luces
        if (PORTBbits.RB2 == 1){
            LATBbits.LATB5 = 1; // se activan las luces
        } 
        else {
            LATBbits.LATB5 = 0; // se desactivan las luces
        }
        // Se verifica el estado del boton de alarma
        if (PORTBbits.RB3 == 1){
            LATBbits.LATB6 = 1; // se activa la alarma
        } 
        else {
            LATBbits.LATB6 = 0; // se desactiva la alarma
        }
        // Se verifica el estado del boton de control de puerta
        if (PORTBbits.RB4 == 1){
            LATBbits.LATB7 = 1; // se activa la puerta
        } 
        else {
            LATBbits.LATB7 = 0; // se desactiva la puerta
        }
        
        __delay_ms(200);
    }
}
