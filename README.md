# Tiny-Box-Fan
This is a simple project to turn on a fan when a box lid is opened and then closed. 
Power: 4x SR44 1.5V silver oxide coin cells in series, for ~6VDC
Logic: Using the 8-pin PIC 12F675
After configuration is complete, the PIC goes to sleep and waits for an interrupt on GP2 pin.
A momentary switch (magnetic reed) is held closed when the lid is closed, which keeps GP2 pulled low.
When the lid opens, GP2 is pulled high. This wakes the PIC and it enters the interrupt service routine.
The PIC checks the state of the GP2 pin. When the lid is closed again, the pin is pulled low.
Then the PIC sends GP0 high to turn on the fan (through an NPN transistor), and sends GP1 high to turn on an indicator LED.
After 25 seconds, the PIC turns off the fan and LED, does some housekeeping, and goes back to sleep.
How do I make the circuit and code more power efficient? Is there a better solution?

