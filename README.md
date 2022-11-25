The code is alpha, please note!

What needs to be done to make PW1 work.
First you have to teach the PW1 the address, that's in the manual, except that with this solution you have to change the band until the LED on the remote controller lights up permanently.

Now you have to know how the PW1 checks whether the correct subscriber can be reached, I will try to explain this.

When the PW1 is switched on and a subscriber has been programmed, the PW1 sends 6 bytes consisting of 0xFE,0xFE, address of the subscriber, address PW1, 0x03,0xFD.
The PW1 uses different data rates, from 0k6, 1k2, 2k4, 4k8, 9k6 and 19k2, at each data rate the query byte is sent 2x, the TRX must answer within 20ms, if not, the next data rate is used, this is repeated 3x.
If there is no answer from the TRX, the CIV communication is stopped, this can be triggered again by switching between TRX1 and TRX2 on the remote controller, then the loop is restarted.

Now we assume that the first communication has worked, the first response from the TRX is structured as follows, 0xFE, 0xFE , address PW1, address TRX, 0x00 or 0x03, frequency data, 0xFD, then the PW1 switches to the received band.

Now the PW1 sends a new request every 10s, the CMD byte is now 0x04 (request for the modulation type), I assume because the response from the TRX is then shorter to relieve the interface.
If one does not react to this, it can happen that the connection to PW1 is interrupted, this is what happened to me from time to time, which is why I send different answers.

I have developed a small board, which fits into a SUBD15 connector housing, an Atmega8 with TQFP32 housing is used on the board, the Atmega 8 can of course be replaced by a pin-compatible type.

In the development phase I am working with an Arduino 2560 board.
