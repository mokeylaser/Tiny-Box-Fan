# Tiny-Box-Fan
This is what should be a simple project to turn on a fan when a box lid is opened and then closed.
Power: 2x CR2032 3V lithium coin cells in series, for ~6VDC
Logic: Using the 8-pin PIC 12F675
After configuration is complete, the PIC goes to sleep and waits for an interrupt on GP2 pin.
A momentary switch is held closed when the lid is closed, which keeps GP2 pulled low.
When the lid opens, GP2 is pulled high. This wakes the PIC and it enters the interrupt service routine.
The PIC checks the state of the GP2 pin. When the lid is closed, the pin is pulled low.
The PIC sends GP0 high to turn on the fan (through an NPN transistor), and sends GP1 high to turn on an indicator LED.
After 25 seconds, the PIC turns off the fan and LED, does some housekeeping, and goes back to sleep.

