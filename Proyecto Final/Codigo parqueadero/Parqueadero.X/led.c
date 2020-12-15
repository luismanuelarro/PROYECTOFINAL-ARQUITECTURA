/*
 * File:   led.c
 * Author: Luis Manuel Arroyo - Luis Miguel Romero
 *
 * Creado: 11/12/2020
 */




//CONFIGURACION DE LOS BITS
#define RS PORTBbits.RB7
#define EN PORTBbits.RB6
#define D4 PORTBbits.RB5
#define D5 PORTBbits.RB4
#define D6 PORTBbits.RB3
#define D7 PORTBbits.RB2

// Se incluye las librerias necesarias para el funcionamiento del programa 
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "pic18f4550.h"
#define _XTAL_FREQ 8000000
#include "lcd.h"
#include "fuses.h"
//---
void main(void);
// VARIABLES 
unsigned char SelectTecla; //Variable que captura los numeros digitados
char SelectContra[4]; //Variable que almacena la contrasenia
char AlmContra[4] = {'1', '2', '3', '4'}; //Contrasenia definida para la seguridad
unsigned int retardo = 100; //Retardo del teclado
unsigned char MYcount = 0;
int mynum = 1;
//Estado de variables contrasenia 

enum Estado_Variable {
    N1, N2, N3, N4, VAL
};
//Estado inicial del teclado
char Estado_Teclado = N1;
//Variable para validar 
int contador = 0;
//Configuracion analogica

short getCad(char canal) {
    ADCON1 = 0b00001100;
    switch (canal) {
        case 0:
            ADCON1 = 0b00000010;
            break;
    }
    ADCON2 = 0b00101001;
    ADON = 1;
    GO_DONE = 1;
    return ADRESH;
}
//Inicializar teclado

void Keypad_Init() {
    TRISD = 0B00001111; //INICIA EL TECLADO
}
//Metodo para capturar letras

unsigned char Keypad_Key_Press(void) {
    const unsigned char keypad[17] = {'7', '8', '9', '/', '4', '5', '6', '*', '1', '2', '3', '-', 13, '0', '=', '+', 0};
    unsigned int tecla = 0, fila;

    for (fila = 0b00000001; fila < 0b00010000; fila <<= 1) {
        LATD = fila << 4; //lo traslada 4  posicones para sacar por las filas
        if (PORTDbits.RD0 == 1) break;
        tecla++;
        if (PORTDbits.RD1 == 1) break;
        tecla++;
        if (PORTDbits.RD2 == 1) break;
        tecla++;
        if (PORTDbits.RD3 == 1) break;
        tecla++;
    }
    PORTD = 0x00;
    return keypad[tecla]; //se cambia la tecla
}

int ContadorCarros(void) {
    //int sensor = 0;
    //sensor = TRISAbits.RA4; //lectura del sensor en le pin A0
    INTCONbits.GIE = 0;
    T0CONbits.T0CS = 1;
    T0CONbits.T0SE = 1;
    T0CONbits.T08BIT = 1;
    T0CONbits.TMR0ON = 1;
    TMR0L = 0; // Se inicializa el contador en 0
    TRISC = 0x00; // 
    LATC = 0x00; // 
    //TRISAbits.RA4 = 1;
    if (TRISAbits.RA4 == 1) { // Si hay un auto en el edificio aumenta el contador
        while (1) { // Aumenta el contador
            if (TMR0L == 10) { // Cuando llegue a 10 se sale
                TMR0L = MYcount; // Mostrar el numero 
            }
            MYcount = TMR0L;
            LATC = (1 << 5) | MYcount;
        }
    }
    return 1;
}
//VALIDA LA CONTRASENIA

void Validacion_Contra() {
    SelectTecla = Keypad_Key_Press(); //Captura la letra

    /*/*/
    //OJO AQUI PODEMOS CONECTAR LOS LEDS
    //
    //
    switch (Estado_Teclado) {
        case N1:
            if ((SelectTecla != 0) && (isdigit(SelectTecla))) {
                SelectContra[0] = SelectTecla; //Se guarda la tecla presionada
                Estado_Teclado = N2; //Se pasa al siguiente digito 
                Lcd_Set_Cursor(2, 1);
                Lcd_Write_String("               "); //Escribe
                Lcd_Set_Cursor(2, 8);
                Lcd_Write_Char(SelectTecla);
                __delay_ms(400);
            }
            break;

        case N2:
            if ((SelectTecla != 0) && (isdigit(SelectTecla))) {
                SelectContra[1] = SelectTecla; //Se guarda la tecla presionada
                Estado_Teclado = N3; //Se pasa al siguiente digito 
                Lcd_Set_Cursor(2, 9);
                Lcd_Write_Char(SelectTecla);
                __delay_ms(400);
            }
            break;
        case N3:
            if ((SelectTecla != 0) && (isdigit(SelectTecla))) {
                SelectContra[2] = SelectTecla; //Se guarda la tecla presionada
                Estado_Teclado = N4; //Se pasa al siguiente digito 
                Lcd_Set_Cursor(2, 10);
                Lcd_Write_Char(SelectTecla);
                __delay_ms(400);
            }
            break;
        case N4:
            if ((SelectTecla != 0) && (isdigit(SelectTecla))) {
                SelectContra[3] = SelectTecla; //Se guarda la tecla presionada
                Estado_Teclado = VAL; //Se pasa a validacion  
                Lcd_Set_Cursor(2, 11);
                Lcd_Write_Char(SelectTecla);
                __delay_ms(400);
            }
            break;
        case VAL:
            if ((SelectTecla != 0) && (SelectTecla != '=')) {// No se presiono el asterisco para validar
                Estado_Teclado = N1; // Vuelve al estado inicial
                Lcd_Set_Cursor(2, 9);
                Lcd_Write_String("               "); //Escribe
                __delay_ms(600);
            } else if ((SelectTecla != 0) && (SelectTecla == '=')) { //Si se presiono el =
                //Ciclo que recorre el vector de la contrasenia guardada
                for (int i = 0; i <= 4; i++) {
                    //Compara la clave guardada "1234" con la ingresada
                    if (AlmContra[i] == SelectContra[i]) {
                        contador++;
                    }
                }
                // Si la clave es correcta entra a hacer sus funciones
                if (contador == 4) {
                    TRISB = 0;
                    ADCON1 = 0b00001111;
                  
                    Lcd_Set_Cursor(2, 1);
                    Lcd_Write_String("CLAVE CORRECTA"); // mensaje mostrado en el lcd
                    PORTBbits.RB0 = 1; // Luces encendidad
                    ContadorCarros(); // Se activa el contador de carros
                    __delay_ms(600);
                    
                } else {
                    SelectContra[4] = 0; //
                    retardo = atoi(SelectContra);
                    Estado_Teclado = N1;
                    Lcd_Set_Cursor(2, 1);
                    Lcd_Write_String("CLAVE INCORRECTA"); // Mensaje mostrado en el lcd
                    __delay_ms(600);
                }
            };
            break;
    }
}

void RepetirOpcion(signed int valor) {
    while (valor > 0) {
        __delay_us(700);
        Validacion_Contra();
        valor--;
    }
}

void main(void) {
    // Configuramos los 0 como salidas y los 1 como entradas 
    TRISB = 0;
    ADCON1 = 0b00001111;

    Keypad_Init(); // Activar el teclado
    Lcd_Init(); // Activar el lcd
    Lcd_Clear();
    Lcd_Set_Cursor(1, 6);
    Lcd_Write_String("PARKING PLUS");// Mensaje a mostrar
    while (1) {
        RepetirOpcion(retardo);
    }

    //    if (PORTBbits.RB0 == 1) // Si presionamos el boton conectado a RB0 ubicado en el PORTB el led conectado prendera
    //    {
    //        PORTBbits.RB1 = 1; 
    //        PORTBbits.RB2 = 0;
    //    } 
    //     // en caso contrario el led estara apagado
    //    else {
    //        PORTBbits.RB1 = 0;
    //        PORTBbits.RB2 = 1;
    //    }
}

