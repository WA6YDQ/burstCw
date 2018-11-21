# burstCw
A cw burst encoder for the Arduino Uno

This is a cw burst encoder based on the AN/GRA-71
CW burst encoder, specifically the MX-4495 component.

 There are 4 buttons and an LED. The output to the 
 transmitter is an NPN transistor as an open collector
 that is tied to the KEY line. 

 Button 1:  Play/Record (Short press/Long press)
 Button 2:  DOT
 Button 3:  Space
 Button 4:  DASH

 LED: Show power on (3 flashes), Play (continous on),
 Record Start (2 flashes), Record Stop (flashes while 
 writing to EEPROM), Playback buffer empty (8 flashes).

The keying transistor can be any garden variety NPN. A 2N3904
will work just fine. Tie the base thru a 1K 1/4 resistor to pin 13
on the Uno. The base goes to ground and the collector will be tied
to the transmitter PTT/KEY line.

This burst encoder transmits a cw message at 300 WPM. The message is 
entered as a series of dots/dashes and spaces using the three buttons
(above). Pressing the DOT button in record mode saves a single dot.
Pressing the DASH button saves a single dash. Pressing the Space button
saves a space (3 bit lengths).

The message is saved in EEPROM in a running stream of 1's and 0's.
Only 1000 bytes are saved. Each byte can hold from 1 to 4 characters. The usage
is based on the character lengths.

To record a message the PLAY/RECORD button is pressed until the 
LED flashes twice (a long press). Then press the DOT/DASH/SPACE buttons until
your message is done. Press the PLAY/RECORD button. The LED will light
until the button is released. When released, the LED will flash while the 
message you just entered is copied from the RAM buffer to EEPROM.

The LED will go out when the data transfer is done. 

To play the message, a short press (less than .25 seconds) on the PLAY/RECORD 
button will light the LED and key the transmit line on and off as the message
goes out. The LED will extinguish when the message is finished being sent.


 k theis <theis.kurt@gmail.com> 11/2018
