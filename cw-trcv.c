/*
 * MW-band CW transmitter
 *
 * Created: 2016/08/27
 * Update : 2017/07/20
 *  Author: jk1mly
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <pic12f615.h>         //Include header file

#define _XTAL_FREQ 8000000

#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on GP4/OSC2/CLKOUT pin, I/O function on GP5/OSC1/CLKIN)
//#pragma config FOSC = ECH       // Oscillator Selection (ECH, External Clock, High Power Mode (4-32 MHz): device clock supplied to CLKIN pin)
//#pragma config FOSC = HS       // Oscillator Selection (ECH, External Clock, High Power Mode (4-32 MHz): device clock supplied to CLKIN pin)
#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
//#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
//#pragma config IOSCFS = 4MHZ    // Internal Oscillator Frequency Select (4 MHz)
#pragma config IOSCFS = 8MHZ    // Internal Oscillator Frequency Select (8 MHz)
#pragma config BOREN = OFF       // Brown-out Reset Selection bits (BOR enabled)
//#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
//#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
//#pragma config IESO = ON        // Internal/External Switchover (Internal/External Switchover mode is enabled)
//#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)
//#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
//#pragma config PLLEN = ON       // PLL Enable (4x PLL enabled)
//#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
//#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
//#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

#define		DLY		50

#define		Key		GP0     //Pin 7
#define		LED		GP4     //Pin 6
#define		Ant		GP2     //Pin 6
#define		MRST	GP3     //Pin 4
#define		OSC2	GP4     //Pin 3
#define		OSC1	GP5     //Pin 2

#define		uint8_t		int
#define		uint16_t	int

void port_init(void) {

    /* CONFIGURE GPIO */ 
    ANSEL = 0;      // disable analog 
    TRISA   = 0b00000001;    //In(1) /Out(0)
    nGPPU   = 0;
    WPU     = 0b00001001;    //PupOn(1)
    INTCON  = 0b00000000;

    P1ASEL = 0;     // GP1 as AF > P1A (PWM)
    // Configure PWM
    T2CONbits.T2CKPS = 0b00;    // set prescaler as 1:1 (default)
    CCP1CONbits.P1M = 0;        // single / P1A modulated
    CCP1CONbits.CCP1M = 0b1100; // set PWM mode 
    // Timer configuration
    PR2    = 0b00000001;        // 2:668kHz, 1:1002kHz
    TMR2 = 0;       // reset TMR2 count
    TMR2ON = 1;     // enable TMR2, start PWM  
    CCPR1L = 0;
    CCP1CON &= 0xCF;
    CCP1CON |= 0x30;
}

void tm1k(uint16_t LOOP){
	for (volatile uint16_t k=0; k<LOOP; k++){
		for (volatile uint16_t l=0; l<DLY; l++){
			asm("NOP");
		}
	}
}

void tone(uint8_t freq){
    TMR2ON = 1;     // enable TMR2, start PWM  
    tm1k(freq);
    TMR2ON = 0;     // enable TMR2, stop  PWM  
    TMR2 = 0;       // reset TMR2 count
    tm1k(freq);
}

uint8_t twinkle(void){
	uint8_t mode=0;
    tm1k(100);
	for (uint8_t i = 0; i < 3; i++){
//Send High
        TRISA   = 0b00000001;    //In(1) /Out(0)
        LED = 0;
        for (uint8_t i = 0; i < 200; i++){
            tone(1);
        }
//Check
        if(Key == 0){
            mode = 1;
        }
//Recv
        TRISA   = 0b00000101;    //In(1) /Out(0)
        LED = 1;
        for (uint8_t i = 0; i < 30; i++){
            tm1k(2);
        }
//Check
        if(Key == 0){
            mode = 1;
        }
//Send Low
        TRISA   = 0b00000001;    //In(1) /Out(0)
        LED = 0;
        for (uint8_t i = 0; i < 200; i++){
            tone(2);
        }
//Check
        if(Key == 0){
            mode = 1;
        }
//Recv
        TRISA   = 0b00000101;    //In(1) /Out(0)
        LED = 1;
        for (uint8_t i = 0; i < 100; i++){
            tm1k(2);
        }
//Check
        if(Key == 0){
            mode = 1;
        }
	}
	return(mode);
}

uint8_t send_vvv(void){
	uint8_t mode=1;

//Wait
        for (uint8_t i = 0; i < 1400; i++){
            tm1k(2);
        }
//Check
        if(Key == 0){
            mode = 0;
        }
//VVV
        for (uint8_t k = 0; k < 3; k++){
//3 Dot
            for (uint8_t j = 0; j < 3; j++){
//Send Dot
                TRISA   = 0b00000001;    //In(1) /Out(0)
                LED = 0;
                for (uint8_t i = 0; i < 100; i++){
                    tone(1);
                }
//Recv
                TRISA   = 0b00000101;    //In(1) /Out(0)
                LED = 1;
                for (uint8_t i = 0; i < 100; i++){
                    tm1k(2);
                }
//Check
                if(Key == 0){
                    mode = 0;
                }
            }
//Send Dash
            TRISA   = 0b00000001;    //In(1) /Out(0)
            LED = 0;
            for (uint8_t i = 0; i < 300; i++){
                tone(1);
            }
//Recv
            TRISA   = 0b00000101;    //In(1) /Out(0)
            LED = 1;
            for (uint8_t i = 0; i < 300; i++){
                tm1k(2);
            }
//Check
            if(Key == 0){
                mode = 0;
            }
        }

	return(mode);
}


int main(void){
	uint8_t mode=0;

//Init
	port_init();
	mode = twinkle();

//Auto
    while(mode == 1){
    	mode = send_vvv();
    }

//Key
    TMR2ON = 0;             // enable TMR2, stop  PWM  
			
    while(1) {
        if (Key == 1) {
//Recv
            TRISA   = 0b00000101;    //In(1) /Out(0)
            LED = 1;
            tm1k(2);
        } else {
//Send
            TRISA   = 0b00000001;    //In(1) /Out(0)
            LED = 0;
            tone(1);
        }
    }
}