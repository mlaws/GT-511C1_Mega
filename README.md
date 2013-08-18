##Overview

A sketch for the Arduino Mega to interface the GT-511C1 fingerprint scanner with a computer through the Arduino serial monitor.

Fingerprint scanner:
https://www.sparkfun.com/products/11836

Arduino Mega:
http://arduino.cc/en/Main/arduinoBoardMega

Based on FutureInventions snippet:

https://www.sparkfun.com/products/11836#comment-51f9ad61757b7f58291b6812

##Setup

###1. Flash sketh onto Arduino Mega

###2. Connect GT-511C1 fingerprint scanner to Mega

Pins from left to right:

1. 19 (RX1)
2. 18 (TX1)
3. GND
4. 3.3V or 5V

###3. Open serial monitor in Arduino IDE at 9600 baud

##Usage

Commands are sent to the Mega in two character packets. The first character is the command and the second is the value.

Where the command does not need a value, simply use a 0 (zero) as a place holder. Many of the inputs below have a zero as the second character in this fashion. Only inputs exactlt two characters long are processed.
I have not implemented all possible commands, but you should see from the template how to add more. You simply need to look up the DEC code for the character (eg. 0 = 48) as a basis.

When entering a number, only 0-9 can be used. The scanner can store 20 prints but this method only allows 10. A rewrite would be neccessary to accomodate IDs 10-19.

### Open Connection

```
O0
```

Opens a connection to the scanner. Should return an ack 30 code if successful. Doesn't seem to be needed to operate.

### Close Connection

```
X0
```

Close connection. Opposite of above.

### Toggle CMOS LED

```
L0 for LED off

L1 for LED on
```

Turns CMOS LED on or off. Must be turned on before any scanning. Should be turned off in battery powered installations to save power.

### Get Enrol Count

```
T0
```

Returns the amount of fingerprints stored in memory.

### Check enrol status for ID

```
t<number>
```

Checks if a specific ID has a fingerprint stored.

### Start enrolment

```
E<number>
```

Start finger print enrolment to the specified ID.

### Enrolment scan 1

```
10
```

Save scan number 1.

### Enrolment scan 2

```
20
```

Save scan number 2.

### Enrolment scan 3

```
30
```

Save scan number 3 which finalises and saves to internal database.

### Finger pressed?

```
F0
```

Checks if finger is pressed on scanner. Unknown on how exactly this should be used.

### Delete ID

```
d<number>
```

Wipes a stored fingerprint from the database.

### Delete all

```
D0
```

Deletes all fingerprints from the database.

### Verify scan

```
V<number>
```

Checks a finger scan against the database at a specified ID.

### Identify scan

```
I0
```

Checks to see if a finger scan matches ANY of the internally saved prints.

### Capture Finger

```
C0
```

Takes a scan of a finger. Required before most other functions.

### Send ACK

```
Y0
```

Sends acknowledgement signal to the scanner.

### Send NACK

```
N0
```

Sends non-acknowledgement signal to the scanner.

## Examples

### Store a fingerprint in the database

```
00 (to init)
L1 (to turn on LED)
E0 (start enrolment at ID 0)
*place finger on scanner*
C0 (capture print)
10 (store capture as scan 1)
*remove and replace finger on scanner*
C0 (capture print)
20 (store capture as scan 2)
*remove and replace finger on scanner*
C0 (capture print)
30 (store capture as scan 3, combine with previous two scans and save in database)
*remove finger*
```

Please note that in the above example, the ID is entered only once with E0. The other zeros as the second character are generic values. If storing at ID 6, the early command would be E6 and all other commands would be identical to the example for ID 0.

### Check a fingerprint in two different ways

To complete a basic test to see if a fingerprint is in the database, the following could be used:

```
L1 (led on)
*finger on scanner*
C0 (capture print)
I0 (identify print in the database)
ACK code returned if matched or error code if not
*finger off*
L0 (led off)
```

To match a finger scan to a specific ID, such as giving access to a specific user's property, the following could be used:

```
L1 (led on)
*finger on scanner*
C0 (capture print)
V<number> (try to match print to specific ID eg. V6 for match with ID 6)
ACK code returned if matched or error code if not
*finger off*
L0 (led off)
```

## Embedded Usage

It is advised to use the Mega to store the prints in the internal database. After this, a modified version of the code could be put on a smaller Arduino microcontroller to run the scanner.

A push button could be connected to the MCU which activated a sequence of calls to the communicate() function to check for scan matches.

Consider an example with a finger print protected safe. The safe uses a solenoid which actuates a latch which unlocks the safe handle. A smaller Arduino unit with only the Solenoid and a push button attached is houses inside the safe door.

The user should place their finger on the scanner and push the button.

The code (incomplete) might then go as follows:

```
communicate(48,48); // init
communicate(76,49); // LED on
communicate(67,48); // capture finger
if(!error){
  communicate(73,48); // check if finger print is stored
  if(match){
    digitalWrite(solenoid, HIGH); // activate solenoid to open safe
    delay(1000); // 1 second window to open safe
    digitalWrite(solenoid), LOW); // lock safe
  }
}
communicate(76,48); // LED off
```

## To Do

At this stage nothing is written to handle responses from the scanner. The raw HEX output is simply written to the console. Successfull code have a 30 value near the end. Where as a 31 usually means a problem/no match.

Ideally, the response could be written to a string and then specific substring searches applied to match the various codes.
