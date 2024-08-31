#include "SPI.h"
//I thought the hardware SS pin meant I didn't have to do this, maybe that's for slave mode...
const int ss = 10;
SPISettings set595(200000, LSBFIRST, SPI_MODE0);
SPISettings set597(200000, MSBFIRST, SPI_MODE0);

int addr_pin[] = { 2, 3, 4 };
int ctrl_lock = 5;   //LED output
int shift_lock = 6;  //LED output
int address = 7;     //so it starts counting at 0

const int strobe = 7;
const int busy = 8;

const int shift_pin = A0;
const int control_pin = A1;
const int reset_pin = A2;
const int capslock_pin = A3;
const int apple_pin = A4;
const int option_pin = A5;

int temp = 0;
boolean shifted = 0;
boolean controled = 0;
boolean capsed = 0;

boolean prev_key_status[16][8] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
};

boolean key_status[16][8] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
};

const byte key_scancode[][8] = {
  { 0x00, 0x1b, 0xef, 0x61, 0x7a, 0x2f, 0x00, 0x2a },  //A0 "","esc","tab","a","z","num /","","*"
  { 0x00, 0x31, 0x71, 0x64, 0x78, 0x00, 0x1b, 0x00 },  //A1 "","1","q","d","x","","num esc",""
  { 0x00, 0x32, 0x77, 0x73, 0x63, 0x30, 0x34, 0x38 },  //A2 "","2","w","s","c","num 0","num 4","num 8"
  { 0x2d, 0x33, 0x65, 0x68, 0x76, 0x31, 0x35, 0x39 },  //A3 "num -","3","e","h","v","num 1","num 5","num 9"
  { 0x0a, 0x34, 0x72, 0x66, 0x62, 0x32, 0x36, 0x2e },  //A4 "num enter","4","r","f","b","num 2","num 6","num ."         right now num enter is just line feed, maybe macro later
  { 0x00, 0x36, 0x79, 0x67, 0x6e, 0x33, 0x37, 0x2b },  //A5 "","6","y","g","n","num 3","num 7","num +"
  { 0x7f, 0x35, 0x74, 0x6a, 0x6d, 0x5c, 0x60, 0x0d },  //A6 "del","5","t","j","m","\","`","return"
  { 0xfd, 0x37, 0x75, 0x6b, 0x2c, 0x3d, 0x70, 0xfe },  //A7 "down","7","u","k",",","=","p","up"
  { 0xfc, 0x38, 0x69, 0x3b, 0x2e, 0x30, 0x5b, 0x20 },  //A8 "left","8","i",";",".","0","[","space"
  { 0xfb, 0x39, 0x6f, 0x6c, 0x2f, 0x2d, 0x5d, 0x27 },  //A9 "right","9","o","l","/","-","]","'"
  { 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A10 "apple","","","","","","",""                                  injected into the table, not in the matrix
  { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A11 "caps","","","","","","",""                                   injected into the table, not in the matrix
  { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A12 "control","","","","","","",""                                injected into the table, not in the matrix
  { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A13 "shift","","","","","","",""                                  injected into the table, not in the matrix
  { 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A14 "option","","","","","","",""                                 injected into the table, not in the matrix
  { 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A15 "reset","","","","","","",""                                  injected into the table, not in the matrix
};

const byte caps_scancode[][8] = {
  { 0x00, 0x1b, 0xef, 0x41, 0x5a, 0x2f, 0x00, 0x2a },  //A0 "","esc","tab","A","Z","num /","","*"
  { 0x00, 0x31, 0x51, 0x44, 0x58, 0x00, 0x1b, 0x00 },  //A1 "","1","Q","D","X","","num esc",""
  { 0x00, 0x32, 0x57, 0x53, 0x43, 0x30, 0x34, 0x38 },  //A2 "","2","W","S","C","num 0","num 4","num 8"
  { 0x2d, 0x33, 0x45, 0x48, 0x56, 0x31, 0x35, 0x39 },  //A3 "num -","3","E","H","V","num 1","num 5","num 9"
  { 0x0a, 0x34, 0x52, 0x46, 0x42, 0x32, 0x36, 0x2e },  //A4 "num enter","4","R","F","B","num 2","num 6","num ."         right now num enter is just line feed, maybe macro later
  { 0x00, 0x36, 0x59, 0x47, 0x4e, 0x33, 0x37, 0x2b },  //A5 "","6","Y","G","N","num 3","num 7","num +"
  { 0x7f, 0x35, 0x54, 0x4a, 0x4d, 0x5c, 0x60, 0x0d },  //A6 "del","5","T","J","M","\","`","return"
  { 0xfd, 0x37, 0x55, 0x4b, 0x2c, 0x3d, 0x50, 0xfe },  //A7 "down","7","U","K",",","=","P","up"
  { 0xfc, 0x38, 0x49, 0x3b, 0x2e, 0x30, 0x5b, 0x20 },  //A8 "left","8","I",";",".","0","[","space"
  { 0xfb, 0x39, 0x4f, 0x4c, 0x2f, 0x2d, 0x5d, 0x27 },  //A9 "right","9","O","L","/","-","]","'"
  { 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A10 "apple","","","","","","",""                                  injected into the table, not in the matrix
  { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A11 "caps","","","","","","",""                                   injected into the table, not in the matrix
  { 0xfa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A12 "control","","","","","","",""                                injected into the table, not in the matrix
  { 0xfa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A13 "shift","","","","","","",""                                  injected into the table, not in the matrix
  { 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A14 "option","","","","","","",""                                 injected into the table, not in the matrix
  { 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A15 "reset","","","","","","",""                                  injected into the table, not in the matrix
};

//for caps lock, only letters have their shifted state, otherwise normal
const byte shifted_scancode[][8] = {
  { 0x00, 0x1b, 0xef, 0x41, 0x5a, 0x2f, 0x00, 0x2a },  //A0 "","esc","tab","A","Z","num /","","*"
  { 0x00, 0x21, 0x51, 0x44, 0x58, 0x00, 0x1b, 0x00 },  //A1 "","!","Q","D","X","","num esc",""
  { 0x00, 0x40, 0x57, 0x53, 0x43, 0x30, 0x34, 0x38 },  //A2 "","@","W","S","C","num 0","num 4","num 8"
  { 0x2d, 0x23, 0x45, 0x48, 0x56, 0x31, 0x35, 0x39 },  //A3 "num -","#","E","H","V","num 1","num 5","num 9"
  { 0x0a, 0x24, 0x52, 0x46, 0x42, 0x32, 0x36, 0x2e },  //A4 "num enter","$","R","F","B","num 2","num 6","num ."         right now num enter is just line feed, maybe macro later
  { 0x00, 0x5e, 0x59, 0x47, 0x4e, 0x33, 0x37, 0x2b },  //A5 "","^","Y","G","N","num 3","num 7","num +"
  { 0x7f, 0x25, 0x54, 0x4a, 0x4d, 0x7c, 0x7e, 0x0d },  //A6 "del","%","T","J","M","|","~","return"
  { 0xfd, 0x26, 0x55, 0x4b, 0x3c, 0x2b, 0x50, 0xfe },  //A7 "down","&","U","K","<","+","P","up"
  { 0xfc, 0x2a, 0x49, 0x3a, 0x3e, 0x29, 0x7b, 0x20 },  //A8 "left","*","I",":",">",")","{","space"
  { 0xfb, 0x28, 0x4f, 0x4c, 0x3f, 0x5f, 0x7d, 0x22 },  //A9 "right","(","O","L","?","_","}","""
  { 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A10 "apple","","","","","","",""                              injected into the table, not in the matrix
  { 0xfa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A11 "caps","","","","","","",""                               injected into the table, not in the matrix
  { 0xfa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A12 "control","","","","","","",""                            injected into the table, not in the matrix
  { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A13 "shift","","","","","","",""                              injected into the table, not in the matrix
  { 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A14 "option","","","","","","",""                             injected into the table, not in the matrix
  { 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A15 "reset","","","","","","",""                              injected into the table, not in the matrix
};
//implemented the first 32 ascii codes, some extra duplicate locations because this keyboard has raw kwys for some control codes and shifted codes are mapped differently (1C, 1E, 1F are mapped to 1,2,3)
const byte controled_scancode[][8] = {
  { 0x00, 0x1b, 0xef, 0x01, 0x1a, 0x2f, 0x00, 0x00 },  //A0 "","esc","tab","SOH","SUB","num /","","NUL"
  { 0x00, 0x1c, 0x11, 0x04, 0x18, 0x00, 0x1b, 0x00 },  //A1 "","FC","DC1","EOT","CAN","","num esc",""
  { 0x00, 0x1e, 0x17, 0x13, 0x03, 0x30, 0x34, 0x38 },  //A2 "","RS","ETB","DC3","ETX","num 0","num 4","num 8"
  { 0x2d, 0x1f, 0x05, 0x08, 0x16, 0x31, 0x35, 0x39 },  //A3 "num -","US","ENQ","BS","SYN","num 1","num 5","num 9"
  { 0x0a, 0x00, 0x12, 0x06, 0x02, 0x32, 0x36, 0x2e },  //A4 "num enter","NUL","DC2","ACK","STX","num 2","num 6","num ."         right now num enter is just line feed, maybe macro later
  { 0x00, 0x00, 0x19, 0x07, 0x0e, 0x33, 0x37, 0x2b },  //A5 "","NUL","EM","BEL","SO","num 3","num 7","num +"
  { 0x7f, 0x00, 0x14, 0x0a, 0x0d, 0x0c, 0x60, 0x0d },  //A6 "del","NUL","DC4","LF","CR","FF","`","return"
  { 0xfd, 0x00, 0x15, 0x0b, 0x00, 0x00, 0x10, 0xfe },  //A7 "down","NUL","NAK","VT","NUL","NUL","DLE","up"
  { 0xfc, 0x00, 0x09, 0x00, 0x00, 0x00, 0x1b, 0x20 },  //A8 "left","NUL","HT","NUL","NUL","NUL","ESC","space"
  { 0xfb, 0x00, 0x0f, 0x0c, 0x00, 0x00, 0x1d, 0x00 },  //A9 "right","NUL","SI","FF","NUL","NUL","GS","NUL"
  { 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A10 "apple","","","","","","",""                                  injected into the table, not in the matrix
  { 0xfa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A11 "caps","","","","","","",""                                   injected into the table, not in the matrix
  { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A12 "control","","","","","","",""                                injected into the table, not in the matrix
  { 0xfa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A13 "shift","","","","","","",""                                  injected into the table, not in the matrix
  { 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A14 "option","","","","","","",""                                 injected into the table, not in the matrix
  { 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  //A15 "reset","","","","","","",""                                  injected into the table, not in the matrix
};


void setup() {
  for (int i = 0; i < sizeof(addr_pin) / sizeof(addr_pin[0]); ++i)
    pinMode(addr_pin[i], OUTPUT);
  pinMode(ctrl_lock, OUTPUT);
  pinMode(shift_lock, OUTPUT);
  pinMode(ss, OUTPUT);
  pinMode(busy, INPUT);
  pinMode(strobe, OUTPUT);
  pinMode(shift_pin, INPUT);
  pinMode(control_pin, INPUT);
  pinMode(reset_pin, INPUT);
  pinMode(capslock_pin, INPUT);
  pinMode(apple_pin, INPUT);
  pinMode(option_pin, INPUT);
  digitalWrite(ctrl_lock, 0);  //redefined to 'unmapped blink'
  digitalWrite(shift_lock, 0);
  digitalWrite(ss, LOW);
  digitalWrite(strobe, LOW);
  SPI.begin();
  Serial.begin(115200);
}

void loop() {

  //set address line
  address += 1;
  if (address > 7)
    address = 0;
  for (int i = 0; i < sizeof(addr_pin) / sizeof(addr_pin[0]); ++i)
    digitalWrite(addr_pin[i], (address >> i) % 2);
  delay(1);

  SPI.beginTransaction(set597);
  digitalWrite(ss, LOW);
  digitalWrite(ss, HIGH);  //Latch inputs
  //read entire row of keys
  temp = SPI.transfer(0);
  temp += (SPI.transfer(0) << 8);
  SPI.endTransaction();

  for (int i = 0; i < 16; i++)  //iterate through each bit of the 16 lines we just read
  {
    if (!((temp >> i) % 2)) {
      key_status[i][address] = 1;
    } else {
      key_status[i][address] = 0;
    }
  }
  ReadOtherKeys();

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 16; j++) {
      if (prev_key_status[j][i] && !key_status[j][i]) {

/*
        Serial.print("key up");
        Serial.print(" :");
        Serial.print(" d");
        Serial.print(i);
        Serial.print(" a");
        Serial.println(j);
        if (!controled && !shifted) {
          Serial.println(key_scancode[j][i], HEX);
        }
*/
        if (i == 0 && j == 13)  //hardcoded shift
        {
          shifted = 0;
        }
        if (i == 0 && j == 12)  //hardcoded control
        {
          controled = 0;
        }
        if (i == 0 && j == 11)  //hardcoded caps
        {
          capsed = 0;
        }
      }
      if (!prev_key_status[j][i] && key_status[j][i])  //key went down
      {
/*
        Serial.print("key down");
        Serial.print(" :");
        Serial.print(" d");
        Serial.print(i);
        Serial.print(" a");
        Serial.println(j);
*/

        if (controled) {
          HandleKey(j, i, 0, 1, 0);  //controlled key
        } else if (shifted) {
          HandleKey(j, i, 1, 0, 0);  //shifted key
        } else if (capsed) {
          HandleKey(j, i, 0, 0, 1);  //caps key
        } else {
          HandleKey(j, i, 0, 0, 0);  //normal key
        }
      }
      prev_key_status[j][i] = key_status[j][i];
    }
  }
  //this handles edge cases where you unlock while holding down the other modifier key
  if (key_status[12][0] && !key_status[13][0] && !key_status[11][0])  //holding control and not holding others
  {
    controled = 1;
  }
  if (key_status[11][0] && !key_status[13][0] && !key_status[12][0])  //holding caps and not holding others
  {
    capsed = 1;
  }
  if (key_status[13][0] && !key_status[12][0] && !key_status[11][0])  //holding shift and not holding others
  {
    shifted = 1;
  }
}

void ReadOtherKeys() {
  key_status[13][0] = !digitalRead(shift_pin);

  key_status[12][0] = !digitalRead(control_pin);

  pinMode(control_pin, OUTPUT);
  digitalWrite(control_pin, LOW);
  key_status[15][0] = !digitalRead(reset_pin);
  pinMode(control_pin, INPUT);

  key_status[11][0] = !digitalRead(capslock_pin);

  key_status[10][0] = digitalRead(apple_pin);

  key_status[14][0] = digitalRead(option_pin);
}

void HandleKey(int j, int i, boolean shift, boolean control, boolean caps) {
  if (!shift && !control && !caps) {
    if (key_scancode[j][i] > 0x7f)
      HandleMacro(key_scancode[j][i]);
    else
      //Serial.println(key_scancode[j][i], HEX);
      SendKey(key_scancode[j][i]);
  }
  if (shift) {
    if (shifted_scancode[j][i] > 0x7f)
      HandleMacro(shifted_scancode[j][i]);
    else
      //Serial.println(shifted_scancode[j][i], HEX);
      SendKey(shifted_scancode[j][i]);
  }
  if (control) {
    if (controled_scancode[j][i] > 0x7f)
      HandleMacro(controled_scancode[j][i]);
    else
      //Serial.println(controled_scancode[j][i], HEX);
      SendKey(controled_scancode[j][i]);
  }
  if (caps) {
    if (caps_scancode[j][i] > 0x7f)
      HandleMacro(caps_scancode[j][i]);
    else
      //Serial.println(controled_scancode[j][i], HEX);
      SendKey(caps_scancode[j][i]);
  }
}
void HandleMacro(int macro) {
  //Macros can be any character from 0x80 to 0xFF, I started counting backward from 0xFF because
  //it's easy to see all the undefined 0xFF key behavior in the scancode tables.
  //This means that in my current implementation you cannot define extended ascii characters except
  //through single character macro sequences which is a bit cumbersome to read but I don't expect to
  //come up often, if at all.
  //I don't actually know how various terminals and computers will respond to getting 8-bit extended
  //ascii sequences over a parallel keyboard port, the 2376 controller chip won't generate them so maybe
  //they're just passed on to the operating system or program? maybe they're filtered out? maybe they just
  //ignore the high bit entirely.

  //These macros can be useful for talking to a specific terminal and automating escape or control sequences.
  //They can also be useful for talking through the terminal transparently and talking to the computer
  //operating system or program to execute a specific function that could take multiple keys.
  switch (macro) {
    case 0xff:
      //Serial.println("do nothing");
      break;
    case 0xfe:
      //Serial.println("up");  //from zrt-80 manual
      SendKey(0x1b);         //ESC
      SendKey(0x5b);         //[
      //SendKey(0x31); //1 might be unneeded, defaults to 1
      SendKey(0x41);  //A
      break;
    case 0xfd:
      //Serial.println("down");  //from zrt-80 manual
      SendKey(0x1b);           //ESC
      SendKey(0x5b);           //[
      //SendKey(0x31); //1 might be unneeded, defaults to 1
      SendKey(0x42);  //B
      break;
    case 0xfc:
      //Serial.println("left");  //from zrt-80 manual
      SendKey(0x1b);           //ESC
      SendKey(0x5b);           //[
      //SendKey(0x31); //1 might be unneeded, defaults to 1
      SendKey(0x44);  //D
      break;
    case 0xfb:
      //Serial.println("right");  //from zrt-80 manual
      SendKey(0x1b);            //ESC
      SendKey(0x5b);            //[
      //SendKey(0x31); //1 might be unneeded, defaults to 1
      SendKey(0x43);  //C
      break;
    case 0xfa:
      //Serial.println("wtf? no. stop.");  //you are mucking with control and shift at the same time, stop it!
      break;
    case 0xf9:
      //Serial.println("Erase in Display");  //from wikipedia ANSI_escape_code
      SendKey(0x1b);                       //ESC
      SendKey(0x5b);                       //[
      SendKey(0x32);                       //2
      SendKey(0x4a);                       //J
      break;
    case 0xf8:
      //Serial.println("home");  //from zrt-80 manual
      SendKey(0x1b);           //ESC
      SendKey(0x5b);           //[
      SendKey(0x48);           //H
      break;
    case 0xf7:
      //Serial.println("F1");  //from wikipedia ANSI_escape_code
      SendKey(0x1b);         //ESC
      SendKey(0x5b);         //[
      SendKey(0x31);         //1
      SendKey(0x50);         //P
      break;
    case 0xf6:
      //Serial.println("F2");  //from wikipedia ANSI_escape_code
      SendKey(0x1b);         //ESC
      SendKey(0x5b);         //[
      SendKey(0x31);         //1
      SendKey(0x51);         //Q
      break;
    case 0xf5:
      //Serial.println("F3");  //from wikipedia ANSI_escape_code
      SendKey(0x1b);         //ESC
      SendKey(0x5b);         //[
      SendKey(0x31);         //1
      SendKey(0x52);         //R
      break;
    case 0xf4:
      //Serial.println("F4");  //from wikipedia ANSI_escape_code
      SendKey(0x1b);         //ESC
      SendKey(0x5b);         //[
      SendKey(0x31);         //1
      SendKey(0x53);         //S
      break;
    case 0xf3:
      //Serial.println("normal charset");  //from zrt-80 manual
      SendKey(0x1b);                     //ESC
      SendKey(0x5b);                     //[
      SendKey(0x67);                     //g
      break;
    case 0xf2:
      //Serial.println("alt charset");  //from zrt-80 manual
      SendKey(0x1b);                  //ESC
      SendKey(0x5b);                  //[
      SendKey(0x66);                  //f
      break;
    case 0xf1:
      //Serial.println("clear display");  //from zrt-80 manual
      SendKey(0x1b);                    //ESC
      SendKey(0x5b);                    //[
      SendKey(0x45);                    //E
      break;
    case 0xf0:
      //Serial.println("clear");  //spare
      SendKey(0x1b);            //ESC
      SendKey(0x5b);            //[
      SendKey(0x32);            //2
      SendKey(0x4a);            //J
      break;
    case 0xef:
      //Serial.println("undefined yet, blink");  //spare
      digitalWrite(ctrl_lock, 1);              //blink
      delay(200);
      digitalWrite(ctrl_lock, 0);
      break;
    default:
      //Serial.println("undefined macro");
      break;
  }
}
void SendKey(byte code) {
  //set bits
  SPI.beginTransaction(set595);
  digitalWrite(ss, LOW);
  //set output bits
  SPI.transfer(code);
  digitalWrite(ss, HIGH);
  SPI.endTransaction();
  //Serial.println(code, HEX);
  //Serial.print(char(code));

  //settle 4.4ms + 88cycles @ 50khz = 6.16ms
  delayMicroseconds(6160);
  //set strobe
  digitalWrite(strobe, HIGH);
  //settle 0.5us + 30ms (way exceed human typing speed)
  delay(30);
  //look for busy signal
  while (!digitalRead(busy)) {  //blocks on busy signal being low
    //Serial.println("blocking for busy line");
  }
  //un-set strobe
  digitalWrite(strobe, LOW);
  //settle for more than 0.5us
  delayMicroseconds(1);
}
