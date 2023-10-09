/*
 * File:   12F675_Fan.c
 * Author: marka
 *                  PIC12F675
 *              +---------:_:---------:
 *     5v0 -> 1 : VDD             VSS : 8 <- GND
 *         <> 2 : GP5         GP0/CIN : 7 <> PGD
 *         <> 3 : GP4/T1Gn    GP1/CIN : 6 <> PGC
 *     VPP -> 4 : GP3/MCLRn   GP2/INT : 5 <>
 *              +---------------------+
 *                       DIP-8
 *
 * Created on October 8, 2023, 10:39 AM
 */

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

// CONFIG
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSC oscillator: I/O function on GP4/OSC2/CLKOUT pin, I/O function on GP5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-Up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // GP3/MCLR pin function select (GP3/MCLR pin function is digital I/O, MCLR internally tied to VDD)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config CP = OFF         // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


#define _XTAL_FREQ 4000000UL //4MHz
#define FAN_PIN  GPIO0
#define INDICATOR GPIO1
#define SENSOR_PIN GPIO2

static uint8_t previous_state = 1;
static uint8_t current_state = 1;
static uint16_t timer = 0;

void __interrupt() myISR(void);

void main(void) {
    // Setup
    TRISIO = 0b00000100; // Configure GP0 as output for the fan, GP1 as output for LED and GP2 as input for the sensor
    ANSEL = 0;   //disable all analog ports
    CMCON = 0b00000111; //Turn off the comparator

    // Initialize fan and LED pins to low
    GPIO = 0b00000000;
    
    // Enable global interrupts
    INTCONbits.GIE = 1;
    
    // Clear any stray interrupt flags
    INTCONbits.INTF = 0;
        
    //Interrupt on rising edge
    OPTION_REGbits.INTEDG = 1; 

    // Enable GPIO Change Interrupt on GP2
    INTCONbits.INTE = 1; // Enable GP2 Interrupt
    INTCONbits.PEIE = 1; // Enable peripheral interrupt
    IOCbits.IOC2 = 1; // Enable interrupt on change for GP2
    
    GPIO;               /* Read GPIO to clear any mismatch for IOC */
    IOC &= (~IOC);      /* Clear IOC bits that changed */
    

    // Main loop
    while (1) {
        // Put the device to sleep. It will wake up on GPIO change.
        asm("SLEEP");
    }
    
}

void __interrupt() myISR(void) {
    // Disable interrupts temporarily
     INTCONbits.GIE = 0; //Disable global interrupts 
     IOCbits.IOC2 = 0; // Disable interrupt on change for GP2

    // Check if the interrupt is due to a GPIO change
    if (INTCONbits.INTF) {
        // Debouncing logic: wait 20 ms before re-reading the sensor state
        __delay_ms(20);

        // Read the current state of the sensor
        current_state = SENSOR_PIN;
        
        // Check state transition
        if ((previous_state == 0) && (current_state == 1)) {
            // Wait for box to close
            __delay_ms (20);
            while (current_state == 1){
                current_state = SENSOR_PIN; //Read the pin again to avoid getting stuck
                
            }
        }
        if (current_state == 0){
        // If the box is closed, turn on the fan for 25 seconds
            __delay_ms (20);
            GPIO = 0b00000011; //Writing the entire port to avoid read-modify-write issues

            // Wait 25 seconds
            for (timer = 1; timer <= 5000; ++timer) {
                __delay_ms(1);
            }

            // Turn off the fan
            GPIO = 0b00000000; //Clearing the entire port 
        }

                            
    }

    // Update the previous state
    previous_state = current_state;
    
    // Clear the interrupt flags
    INTCONbits.INTF = 0; // Clear the GP2 External interrupt flag bit
    INTCONbits.GPIF = 0; // Clear the port change interrupt flag bit
    INTCONbits.GIE = 1; // Re-Enable global interrupts
    IOCbits.IOC2 = 1; // Re-Enable interrupt on change for GP2
    
    GPIO;               /* Read GPIO to clear any mismatch for IOC */
    //IOC &= (~IOC);      /* Clear IOC bits that changed */
}
