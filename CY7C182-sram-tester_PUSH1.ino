// Pins for addresses SRAMA A0, A1, A2, ..., A12  -to Arduino Mega 2560 PINS  {22, 24, 26, 28, 32, 34, 36, 38, 40, 42, 44, 46, 48 };
const char ADDR_PINS[] = { 22, 24, 26, 28, 32, 34, 36, 38, 40, 42, 44, 46, 48 };

// Pins for I/O0, I/O1, I/O2, ..., I/O7
const char DATA_PINS[] = {50, 23, 25, 27, 29, 31, 33, 35, 37};

// Pins for !WE, !OE and !CS
const char WE_PIN = 2;
const char OE_PIN = 3;
const char CS1_PIN = 4; //negative CS
const char CS2_PIN = 30; //positive cs

 // ( pin 5 connected via resistor to +5V , and via pushbutton switch to gnd.)

// Amount of microseconds to wait in case the memory is slower than the arduino.
const char OP_WAIT = 0;

// Testing a 32KB memory
//const unsigned int MEMORY_SIZE = 32768; //1024 bytes * 32; 32768
const unsigned int MEMORY_SIZE = 36864; //256*32*9 = 36864

// Byte to be writen, read and compared
 unsigned char TESTING_BYTE = 0xff;    

 bool flag = false; //button press/release handler.( pin 5 connected via resistor to +5V , and via push button switch to gnd.)
 
void setup() {

pinMode(5 , INPUT_PULLUP);// Pin 5 for select TESTING BYTE 0xff or 0x00.
 
  
  // Configure all address pins to output
  for(int n = 0; n < 13; n += 1) {
    pinMode(ADDR_PINS[n], OUTPUT);
  }

// Byte to be writen, read and compared
const unsigned char TESTING_BYTE = 0xff;

  // Configure all control pins to output
  pinMode(WE_PIN, OUTPUT);
  pinMode(OE_PIN, OUTPUT);
  pinMode(CS1_PIN, OUTPUT);
  pinMode(CS2_PIN, OUTPUT);

  // Configure Write Enable, Output Enable and Chip Select to HIGHT.
  resetControlPins();  
  
  Serial.begin(57600);
  
  // Start testing
  test();
}

// Write a byte into memory at the address specified
void writeByte(unsigned char byte, unsigned int address) {
  // Configure all data pins to output
  for(int n = 0; n < 9; n += 1) {
    pinMode(DATA_PINS[n], OUTPUT);
  }
  
  setAddress(address);

  // Set CS to low, in order to select the chip
  digitalWrite(CS1_PIN, LOW);
  digitalWrite(CS2_PIN, HIGH);
  
  delayMicroseconds(OP_WAIT); // wait 

  
  // Set the data pins with the byte
  for(int b = 0; b < 9; b++) {
    if(byte & 1<<(b)) {    
      digitalWrite(DATA_PINS[b], HIGH);
    } else {
      digitalWrite(DATA_PINS[b], LOW);
    }
  }  

  // Set WE to low, in order to write the data
  digitalWrite(WE_PIN, LOW);
  delayMicroseconds(OP_WAIT); // wait 

  resetControlPins();
}

// Read a byte from memory at the address specified
unsigned char readByte(unsigned int address) {
  // Configure all data pins to input
  for(int n = 0; n < 9; n += 1) {
    pinMode(DATA_PINS[n], INPUT);
  }

  setAddress(address);
  
  // Set CS to low, in order to select the chip
  digitalWrite(CS1_PIN, LOW);
  digitalWrite(CS2_PIN, HIGH);
  // Set OE to low, in order to read the data
  digitalWrite(OE_PIN, LOW);

  delayMicroseconds(OP_WAIT); // wait 

  unsigned char byte = 0;
  
  // Read the data pins
  for(int b = 0; b < 9; b++) {
    if(digitalRead(DATA_PINS[b])) {
      byte = byte | 1<<(b);  
    }
  }

  resetControlPins();

  return byte;
}

// Set the address pins to represent the address passed
void setAddress(unsigned int address) {
  for(int b = 0; b < 13; b++) {
    if(address & 1<<(b)) {    
      digitalWrite(ADDR_PINS[b], HIGH);
    } else {
      digitalWrite(ADDR_PINS[b], LOW);
    }
  }  
}

// Configure Write Enable, Output Enable and Chip Select to HIGHT.
void resetControlPins() {
  digitalWrite(WE_PIN, HIGH);
  digitalWrite(OE_PIN, HIGH);
  digitalWrite(CS1_PIN, HIGH);
  digitalWrite(CS2_PIN, LOW);
}

void test() {

  // читаем инвертированное значение для удобства кнопки пин 5
 bool btnState = !digitalRead(5);
  if (btnState && !flag) {  // press handler
    flag = true;
   Serial.println(" PPPPreSSSS- 00");
    TESTING_BYTE = 0x00;
    }
  
  if (!btnState && !flag) {  // release handler
    flag = false;  
   Serial.println(" REEEleaSSSE- ff");
    TESTING_BYTE = 0xff;
  }

    
  
  Serial.print(" Testing byte - "); 
Serial.println(TESTING_BYTE);


  char output[100];
  for(unsigned int address = 0; address < MEMORY_SIZE; address++) {
   writeByte(TESTING_BYTE, address);
    delayMicroseconds(OP_WAIT); // wait 
    unsigned char byte = readByte(address);
    
    if(TESTING_BYTE != byte) {
      sprintf(output, "Error at address: %04X - written: %02X read: %02X", address, TESTING_BYTE, byte);
      Serial.println(output);
    }
    delayMicroseconds(OP_WAIT); // wait 

    if(address%1024==0) {
      Serial.print((address/1024)+1);
      Serial.print(" KB");
    }
  }
  Serial.println("End of test.");
  Serial.println(" Your CY7C182 is good if no errors have been reported");
}

void loop() {
}
