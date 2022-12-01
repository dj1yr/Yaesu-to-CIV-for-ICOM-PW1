The code is alpha, please note!

This small project translates the BCD code from Yaesu to ICOM CIV for communication with the PW1.

What do you have to do to make the PW1 work? First you have to teach the PW1 the address, this is in the manual, only with this solution you have to change the band until the LED on the remote is permanently on.

Now you have to know how the PW1 checks if the right subscriber can be reached, I will try to explain that.

When the PW1 is switched on and a participant has been programmed, the PW1 sends 6 bytes consisting of 0xFE,0xFE, address of participant, address PW1, 0x03,0xFD. The PW1 uses different data rates, from 0k6, 1k2, 2k4, 4k8, 9k6 and 19k2, at each data rate the query byte is sent 2x, the TRX must respond within 20ms, if not the next data rate is used, this is repeated 3x. If the TRX does not answer, the CIV communication is stopped, this can be retriggered by switching between TRX1 and TRX2 on the remote control, then the loop is restarted.

Now let's assume that the first communication worked, the first response from the TRX is structured as follows, 0xFE, 0xFE , address PW1, address TRX, 0x00 or 0x03, frequency data, 0xFD, then the PW1 switches to the received band.

Now the PW1 sends a new request every 10s, the CMD byte is initially 0x03, when the corresponding response comes, the PW1 sends 0x04, i.e. request for modulation. If one does not react to this, it can happen that the connection to the PW1 is interrupted, this also happened to me from time to time, so I send other answers.

I have developed a small board that fits into a SUBD15 connector housing, on the board an Atmega8 with TQFP32 housing is used, the Atmega 8 can of course be replaced by a pin compatible type.

In the development phase I am working with an Arduino Mega 2560 board.
