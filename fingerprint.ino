byte highbyte = 0;
byte lowbyte = 0;
byte command = 1;
word checksum = 0;
byte highcheck = 0;
byte lowcheck = 0;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character
boolean ledon = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Start");
  Serial1.begin(9600);
}
//Here comes the part you want to put in void loop that sends the command to the device

void loop(){
  if(Serial1.available()){
    index = 0;
    while(Serial1.available() > 0){
      inChar = Serial1.read(); // Read a character
      Serial.print(inChar, HEX);
    }
   
  }
  
  // read from port 0, send to port 1:
  if (Serial.available()==2) {
    int inByte1 = Serial.read();
    int inByte2 = Serial.read();
    communicate(inByte1, inByte2); 
  }
}

void communicate(int com, int val){
   Serial.println();
  int value;
  switch(val){
    case 48:
      value = 0;
    break;
    
    case 49:
      value = 1;
      break; 
      
    case 50:
      value = 2;
      break;
      
    case 51:
      value = 3;
      break; 
      
    case 52:
      value = 4;
      break;
      
    case 53:
      value = 5;
      break;
      
    case 54:
      value = 6;
      break;
      
    case 55:
      value = 7;
      break;
      
    case 56:
      value = 8;
      break;
      
    case 57:
      value = 9;
      break;
  }
  switch(com){
    // 0 > init > 01
    case 48:
    Serial.println("Open connection");
      command = 0x01;
      break;
    
    // X > close > 02
    case 88:
    Serial.println("Close connection");    
      command = 0x02;
      break;
      
    // L > LED change > 12  
    case 76:
      if(value == 0){
        Serial.println("LED off");
      } else {
        Serial.println("LED on");
      }
      command = 0x12;
      break;

    // T > Check total enrol count > 20 
    case 84:
      Serial.println("Check total enrol count");
      command = 0x20;
      break;
    
    // t > check enrol id  > 21
    case 116:
      Serial.print("Check enrol for ID ");
      Serial.println(value);
      command = 0x21;
      break;  
    
    // E > enrol1 > 22
    case 69:
      Serial.print("Begin enrol for ID ");
      Serial.println(value);      
      command = 0x22;
      break;
    
    // 1 > enrol1 > 23
    case 49:
      Serial.println("Enrol scan #1"); 
      command = 0x23;
      break;  
    
    // 2 > enrol2 > 24  
    case 50:
      Serial.println("Enrol scan #2"); 
      command = 0x24;
      break;
    
    // 3 > enrol3 > 25  
    case 51:
      Serial.println("Enrol scan #3");  
      command = 0x25;
      break;
    
    // F > isfingerpressed? > 26 
    case 70:
      Serial.println("Checking if finger pressed..."); 
      command = 0x26;
      break;
    
    // d > delete ID > 40
    case 100:
      Serial.print("Deleting scan ");
      Serial.println(value);
      command = 0x40;
      break;
    
    // D > delete all > 41
    case 68:
      Serial.println("Deleting all scans"); 
      command = 0x41;
      break;
    
    // V > verify ID > 50
    case 86:
      Serial.print("Verfifying ID ");
      Serial.println(value);
      command = 0x50;
      break;
    
    // I > identify finger > 51
    case 73:
      Serial.println("Checking for scan match...");
      command = 0x51;
      break;
     
    // C > capture finger > 60
    case 67: 
      Serial.println("Capturing finger");
      command = 0x60;
      break; 
    
   // Y > ack > 30
    case 89:
      Serial.println("Sending acknowledgement");
      command = 0x30;
      break;
     
    // N > nack > 31
    case 78:
      Serial.println("Sending Non-acknowledgement");
      command = 0x31;
      break; 

  }
  
  //command = 0x12; //The command goes here. This is the command for the LED.
  valueToWORD(value); //This value is the parameter being send to the device. 0 will turn the LED off, while 1 will turn it on.
  calcChecksum(command, highbyte, lowbyte); //This function will calculate the checksum which tells the device that it received all the data
  Serial1.write(0x55); //Command start code 1
  Serial1.write(0xAA); //Command start code 2
  Serial1.write(0x01); // This is the first byte for the device ID. It is the word 0x0001
  Serial1.write(0x00); // Second byte of Device ID. Notice the larger byte is first. I'm assuming this is because the datasheet says "Multi-byte item is represented as Little Endian"
  Serial1.write(lowbyte); //writing the largest byte of the Parameter
  Serial1.write(highbyte); //Writing the second largest byte of the Parameter
  Serial1.write(0x00); //The datasheet says the parameter is a DWORD, but it never seems to go over the value of a word
  Serial1.write(0x00); //so I'm just sending it a word of data. These are the 2 remaining bytes of the Dword
  Serial1.write(command); //write the command byte
  Serial1.write(0x00); //again, the commands don't go over a byte, but it is sent as a word, so I'm only sending a byte
  Serial1.write(lowcheck); //Writes the largest byte of the checksum
  Serial1.write(highcheck); //writes the smallest byte of the checksum
}
//Now for the functions

void valueToWORD(int v){ //turns the word you put into it (the paramter in the code above) to two bytes
  highbyte = highByte(v); //the high byte is the first byte in the word
  lowbyte = lowByte(v); //the low byte is the last byte in the word (there are only 2 in a word)
}

void calcChecksum(byte c, byte h, byte l){
  checksum = 256 + c + h + l; //adds up all the bytes sent
  highcheck = highByte(checksum); //then turns this checksum which is a word into 2 bytes
  lowcheck = lowByte(checksum);
}
