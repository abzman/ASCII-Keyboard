#include "SPI.h"
//I thought the hardware SS pin meant I didn't have to do this, maybe that's for slave mode...
const int ss = 10;
SPISettings set595(200000, LSBFIRST, SPI_MODE0);
SPISettings set597(200000, MSBFIRST, SPI_MODE0);

int addr_pin[] = {2, 3, 4};
int ctrl_lock = 5; //LED output
int shift_lock = 6; //LED output
int address = 7; //so it starts counting at 0

const int strobe = 7;
const int busy = 8;

int temp = 0;
boolean shifted = 0;
boolean shift_locked = 0;
boolean controled = 0;
boolean control_locked = 0;

boolean prev_key_status[16][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
};

boolean key_status[16][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
};

const byte key_scancode[][8] = {
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                  //A0 not connected to a row
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                  //A1 not connected to a row
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                  //A2 not connected to a row
  {0xff, 0xf7, 0xf6, 0xf5, 0xf4, 0xff, 0xff, 0xff},                  //A3 "wildcard","(I)","(II)","(III)","(IV)","(V)","(VI)","undo"
  {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38},                  //A4 "1","2","3","4","5","6","7","8"
  {0x09, 0x71, 0x77, 0x65, 0x72, 0x74, 0x79, 0x75},                  //A5 "Htab","q","w","e","r","t","y","u"
  {0x61, 0x73, 0x64, 0x66, 0x67, 0x68, 0x6a, 0x6b},                  //A6 "a","s","d","f","g","h","j","k"
  {0x7a, 0x78, 0x63, 0x76, 0x62, 0x6e, 0x6d, 0x2c},                  //A7 "z","x","c","v","b","n","m",","
  {0x39, 0x30, 0x2d, 0x2b, 0x5e, 0x6c, 0x3b, 0x27},                  //A8 "9","0","-","+","^","l",";","'"
  {0x69, 0x6f, 0x70, 0x5b, 0x5d, 0x2e, 0x2f, 0x0d},                  //A9 "i","o","p","[","]",".","/","CR"
  {0x5c, 0x1b, 0x00, 0x00, 0x20, 0xff, 0xff, 0xf1},                  //A10 "\","escape","","","space","move","store","clear"
  {0xfe, 0xfb, 0xfd, 0xfc, 0x08, 0xff, 0xff, 0xff},                  //A11 "up","right","down","left","backspace","insert","print","delete"
  {0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                  //A12 "control","","","","","","",""
  {0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                  //A13 "shift","","","","","","",""
  {0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                  //A14 "lock","","","","","","",""
  {0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                  //A15 "home","","","","","","",""
};

const byte shifted_scancode[][8] = {
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                  //A0 not connected to a row
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                  //A1 not connected to a row
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                  //A2 not connected to a row
  {0xff, 0xf7, 0xf6, 0xf5, 0xf4, 0xff, 0xff, 0xff},                  //A3 "wildcard","(I)","(II)","(III)","(IV)","(V)","(VI)","undo"
  {0x21, 0x40, 0x23, 0x24, 0x25, 0x5f, 0x26, 0x2a},                  //A4 "!","@","#","$","%","_","&","*"
  {0x0b, 0x51, 0x57, 0x45, 0x52, 0x54, 0x59, 0x55},                  //A5 "Vtab","Q","W","E","R","T","Y","U"
  {0x41, 0x53, 0x44, 0x46, 0x47, 0x48, 0x4a, 0x4b},                  //A6 "A","S","D","F","G","H","J","K"
  {0x5a, 0x58, 0x43, 0x56, 0x42, 0x4e, 0x4d, 0x3c},                  //A7 "Z","X","C","V","B","N","M","<"
  {0x28, 0x29, 0x60, 0x3d, 0x7e, 0x4c, 0x3a, 0x22},                  //A8 "(",")","`","=","~","L",":","""
  {0x49, 0x4f, 0x50, 0x7b, 0x7d, 0x3e, 0x3f, 0x0a},                  //A9 "I","O","P","{","}",">","?","LF"
  {0x7c, 0xff, 0x00, 0x00, 0x20, 0xff, 0xff, 0xf9},                  //A10 "|","WP","","","space","copy","get","clear"
  {0xfe, 0xfb, 0xfd, 0xfc, 0x08, 0xff, 0xff, 0xff},                  //A11 "up","right","down","left","backspace","insert","print","delete"
  {0xfa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                  //A12 "control","","","","","","",""
  {0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                  //A13 "shift","","","","","","",""
  {0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                  //A14 "lock","","","","","","",""
  {0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                  //A15 "home","","","","","","",""
};
//implemented the first 32 ascii codes, some extra duplicate locations because this keyboard has raw kwys for some control codes and shifted codes are mapped differently
const byte controled_scancode[][8] = {
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                  //A0 not connected to a row
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                  //A1 not connected to a row
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                  //A2 not connected to a row
  {0xff, 0xf7, 0xf6, 0xf5, 0xf4, 0xff, 0xff, 0xff},                  //A3 "wildcard","(I)","(II)","(III)","(IV)","(V)","(VI)","undo"
  {0xff, 0x00, 0xff, 0xff, 0xff, 0x1f, 0xff, 0xff},                  //A4 "1","2","3","4","5","6","7","8"
  {0x09, 0x11, 0x17, 0x05, 0x12, 0x14, 0x19, 0x15},                  //A5 "tab","q","w","e","r","t","y","u"
  {0x01, 0x13, 0x04, 0x06, 0x07, 0x08, 0x0a, 0x0b},                  //A6 "a","s","d","f","g","h","j","k"
  {0x1a, 0x18, 0x03, 0x16, 0x02, 0x0e, 0x0d, 0xff},                  //A7 "z","x","c","v","b","n","m",","
  {0xff, 0xff, 0xff, 0xff, 0x1e, 0x0c, 0xff, 0xff},                  //A8 "9","0","-","+","^","l",";","'"
  {0x09, 0x0f, 0x10, 0x1b, 0x1d, 0xff, 0x1f, 0x0d},                  //A9 "i","o","p","[","]",".","/","CR"
  {0x1c, 0x1b, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff},                  //A10 "\","escape","","","space","move","store","clear"
  {0xfe, 0xfb, 0xfd, 0xfc, 0xff, 0xff, 0xff, 0x1f},                  //A11 "up","right","down","left","backspace","insert","print","delete"
  {0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                  //A12 "control","","","","","","",""
  {0xfa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                  //A13 "shift","","","","","","",""
  {0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                  //A14 "lock","","","","","","",""
  {0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                  //A15 "home","","","","","","",""
};


void setup() {
  for (int i = 0; i < sizeof(addr_pin) / sizeof(addr_pin[0]); ++i)
    pinMode(addr_pin[i], OUTPUT);
  pinMode (ctrl_lock, OUTPUT);
  pinMode (shift_lock, OUTPUT);
  pinMode (ss, OUTPUT);
  pinMode (busy, INPUT);
  pinMode (strobe, OUTPUT);
  digitalWrite (ctrl_lock, control_locked);
  digitalWrite (shift_lock, shift_locked);
  digitalWrite (ss, LOW);
  digitalWrite (strobe, LOW);
  SPI.begin();
  //Serial.begin(115200);
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
  digitalWrite (ss, LOW);
  digitalWrite (ss, HIGH); //Latch inputs
  //read entire row of keys
  temp = SPI.transfer(0);
  temp += (SPI.transfer(0) << 8);
  SPI.endTransaction();

  for (int i = 0; i < 16; i++) //iterate through each bit of the 16 lines we just read
  {
    if (!((temp >> i) % 2))
    {
      key_status[i][address] = 1;
    }
    else
    {
      key_status[i][address] = 0;
    }
  }


  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 16; j++)
    {
      if (prev_key_status[j][i] && !key_status[j][i])
      {

        //Serial.print("key up");
        //Serial.print(" :");
        //Serial.print(" d");
        //Serial.print(i);
        //Serial.print(" a");
        //Serial.println(j);

        if (!controled && !shifted)
        {
          //Serial.println(key_scancode[j][i], HEX);
        }
        if (i == 0 && j == 13 && !shift_locked)//hardcoded shift
          shifted = 0; //only stop shifting if the lock isn't set
        if (i == 0 && j == 12 && !control_locked)//hardcoded control
          controled = 0; //only stop controling if the lock isn't set
      }
      if (!prev_key_status[j][i] && key_status[j][i]) //key went down
      {
        /*
                Serial.print("key down");
                Serial.print(" :");
                Serial.print(" d");
                Serial.print(i);
                Serial.print(" a");
                Serial.println(j);
        */

        if (controled)
        {
          if (i == 0 && j == 14 && !control_locked)//hardcoded lock
          {
            control_locked = 1; //set control lock
            digitalWrite (ctrl_lock, control_locked);
            //Serial.println("set control lock");
          }
          else if (i == 0 && j == 14 && control_locked)//hardcoded lock
          {
            control_locked = 0; //un-set control lock
            digitalWrite (ctrl_lock, control_locked);
            //Serial.println("un-set control lock");
          }
          else
          {
            HandleKey(j, i, 0, 1);//controlled key
          }
        }
        else if (shifted)
        {
          if (i == 0 && j == 14 && !shift_locked)//hardcoded lock
          {
            shift_locked = 1; //set shift lock
            digitalWrite (shift_lock, shift_locked);
            //Serial.println("set shift lock");
          }
          else if (i == 0 && j == 14 && shift_locked)//hardcoded lock
          {
            shift_locked = 0; //un-set shift lock
            digitalWrite (shift_lock, shift_locked);
            //Serial.println("un-set shift lock");
          }
          else
          {
            HandleKey(j, i, 1, 0);//shifted key
          }
        }
        else if (!controled && !shifted)
        {
          // modifier keys handled state triggered now, not edge triggered
          //if (i == 0 && j == 13)//hardcoded shift
          //{
          //  shifted = 1;
          //  Serial.println("shift down");
          //}
          //else if (i == 0 && j == 12)//hardcoded control
          //{
          //  controled = 1;
          //  Serial.println("control down");
          //}
          //else
          //{
          HandleKey(j, i, 0, 0);//normal key
          //}
        }
      }
      prev_key_status[j][i] = key_status[j][i];
    }
  }
  //this handles edge cases where you unlock while not holding down the modifier key
  if (!key_status[13][0] && !shift_locked)
    shifted = 0;
  if (!key_status[12][0] && !control_locked)
    controled = 0;


  //this handles edge cases where you unlock while holding down the other modifier key
  if (key_status[12][0] && !key_status[13][0] && !shift_locked) //holding control and not holding shift and not shift locked
    controled = 1;
  if (key_status[13][0] && !key_status[12][0] && !control_locked) //holding shift and not holding control and not control locked
    shifted = 1;


}


void HandleKey(int j, int i, boolean shift, boolean control)
{
  if (!shift && !control)
  {
    if (key_scancode[j][i] > 0x7f)
      HandleMacro(key_scancode[j][i]);
    else
      //Serial.println(key_scancode[j][i], HEX);
      SendKey(key_scancode[j][i]);
  }
  if (shift)
  {
    if (shifted_scancode[j][i] > 0x7f)
      HandleMacro(shifted_scancode[j][i]);
    else
      //Serial.println(shifted_scancode[j][i], HEX);
      SendKey(shifted_scancode[j][i]);
  }
  if (control)
  {
    if (controled_scancode[j][i] > 0x7f)
      HandleMacro(controled_scancode[j][i]);
    else
      //Serial.println(controled_scancode[j][i], HEX);
      SendKey(controled_scancode[j][i]);
  }

}
void HandleMacro(int macro)
{
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
      //Serial.println("up"); //from zrt-80 manual
      SendKey(0x1b); //ESC
      SendKey(0x5b); //[
      //SendKey(0x31); //1 might be unneeded, defaults to 1
      SendKey(0x41); //A
      break;
    case 0xfd:
      //Serial.println("down"); //from zrt-80 manual
      SendKey(0x1b); //ESC
      SendKey(0x5b); //[
      //SendKey(0x31); //1 might be unneeded, defaults to 1
      SendKey(0x42); //B
      break;
    case 0xfc:
      //Serial.println("left"); //from zrt-80 manual
      SendKey(0x1b); //ESC
      SendKey(0x5b); //[
      //SendKey(0x31); //1 might be unneeded, defaults to 1
      SendKey(0x44); //D
      break;
    case 0xfb:
      //Serial.println("right"); //from zrt-80 manual
      SendKey(0x1b); //ESC
      SendKey(0x5b); //[
      //SendKey(0x31); //1 might be unneeded, defaults to 1
      SendKey(0x43); //C
      break;
    case 0xfa:
      //Serial.println("wtf? no. stop."); //you are mucking with control and shift at the same time, stop it!
      break;
    case 0xf9:
      //Serial.println("Erase in Display"); //from wikipedia ANSI_escape_code
      SendKey(0x1b); //ESC
      SendKey(0x5b); //[
      SendKey(0x32); //2
      SendKey(0x4a); //J
      break;
    case 0xf8:
      //Serial.println("home"); //from zrt-80 manual
      SendKey(0x1b); //ESC
      SendKey(0x5b); //[
      SendKey(0x48); //H
      break;
    case 0xf7:
      //Serial.println("F1"); //from wikipedia ANSI_escape_code
      SendKey(0x1b); //ESC
      SendKey(0x5b); //[
      SendKey(0x31); //1
      SendKey(0x50); //P
      break;
    case 0xf6:
      //Serial.println("F2"); //from wikipedia ANSI_escape_code
      SendKey(0x1b); //ESC
      SendKey(0x5b); //[
      SendKey(0x31); //1
      SendKey(0x51); //Q
      break;
    case 0xf5:
      //Serial.println("F3"); //from wikipedia ANSI_escape_code
      SendKey(0x1b); //ESC
      SendKey(0x5b); //[
      SendKey(0x31); //1
      SendKey(0x52); //R
      break;
    case 0xf4:
      //Serial.println("F4"); //from wikipedia ANSI_escape_code
      SendKey(0x1b); //ESC
      SendKey(0x5b); //[
      SendKey(0x31); //1
      SendKey(0x53); //S
      break;
    case 0xf3:
      //Serial.println("normal charset"); //from zrt-80 manual
      SendKey(0x1b); //ESC
      SendKey(0x5b); //[
      SendKey(0x67); //g
      break;
    case 0xf2:
      //Serial.println("alt charset"); //from zrt-80 manual
      SendKey(0x1b); //ESC
      SendKey(0x5b); //[
      SendKey(0x66); //f
      break;
    case 0xf1:
      //Serial.println("clear display"); //from zrt-80 manual
      SendKey(0x1b); //ESC
      SendKey(0x5b); //[
      SendKey(0x45); //E
      break;
    case 0xf0:
      //Serial.println("clear"); //spare
      SendKey(0x1b); //ESC
      SendKey(0x5b); //[
      SendKey(0x32); //2
      SendKey(0x4a); //J
      break;
    default:
      //Serial.println("undefined macro");
      break;
  }
}
void SendKey(byte code)
{
  //set bits
  SPI.beginTransaction(set595);
  digitalWrite (ss, LOW);
  //set output bits
  SPI.transfer(code);
  digitalWrite (ss, HIGH);
  SPI.endTransaction();
  Serial.println(code, HEX);

  //settle 4.4ms + 88cycles @ 50khz = 6.16ms
  delayMicroseconds(6160);
  //set strobe
  digitalWrite (strobe, HIGH);
  //settle 0.5us + 30ms (way exceed human typing speed)
  delay(30);
  //look for busy signal
  while (!digitalRead(busy)) { //blocks on busy signal being low
    //Serial.println("blocking for busy line");
  }
  //un-set strobe
  digitalWrite (strobe, LOW);
  //settle for more than 0.5us
  delayMicroseconds(1);
}
