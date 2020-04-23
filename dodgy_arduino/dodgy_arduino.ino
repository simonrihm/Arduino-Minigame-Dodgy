#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins

int myCounter=0;  //declare your variable myCounter and set to 0

LiquidCrystal LCD(2, 3, 8, 9, 10, 11); /// REGISTER SELECT PIN,ENABLE PIN,D4 PIN,D5 PIN, D6 PIN, D7 PIN

const int SW_pin = 4;
const int X_pin = 1;
const int Y_pin = 0;

const int updatespeed = 10;
const int cursorspeed = 100;

byte lo[8] = {
  B11000,
  B11000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

byte ro[8] = {
  B00011,
  B00011,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

byte lm[8] = {
  B00000,
  B00000,
  B00000,
  B11000,
  B11000,
  B00000,
  B00000,
  B00000
};

byte rm[8] = {
  B00000,
  B00000,
  B00000,
  B00011,
  B00011,
  B00000,
  B00000,
  B00000
};

byte lu[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11000,
  B11000
};

byte ru[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00011,
  B00011
};

byte full[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

int x = 0;
int y = 0;

int block1 = 15;
int block2 = 15;
int level = 1;
int counter = 0;
int blockcounter1 = 0;
int blockcounter2 = 0;
int cursorcounter = 0;

int x_joystick, y_joystick,x_lcd, y_lcd, x_char, y_char, pos_char, x_delta, y_delta, blockspeed1, blockspeed2, SW;

void setup()
{
  Serial.begin(115200);

  LCD.createChar(0,lo);
  LCD.createChar(1,ro);
  LCD.createChar(2,lm);
  LCD.createChar(3,rm);
  LCD.createChar(4,lu);
  LCD.createChar(5,ru);

  LCD.createChar(7,full);
  
  LCD.begin(16,2); //Tell Arduino to start your 16 column 2 row LCD
}

void loop() {
  // RESET
  LCD.clear();
  LCD.setCursor(0,0);

  // LEVELSTART
  if (counter == 0) {
    LCD.write("START LEVEL ");
    LCD.print(level);
    for (int i = 3; i >= 0; i = i-1){
      LCD.setCursor(0,1);
      LCD.write("IN ");
      LCD.print(i);
      LCD.write(" SECONDS");
      delay(1000);
    }
    LCD.clear();
    x = 0;
    y = 0;
    block1 = 15;
    block2 = 15;
    blockspeed1 = 500 / level;
    blockspeed2 = 300 / level;
  }
  
  // JOYSTICK COMMAND
  x_joystick = analogRead(X_pin);
  y_joystick = analogRead(Y_pin);
  SW = analogRead(SW_pin);

  if (SW<100) {
    LCD.write("GAME PAUSED");
    LCD.setCursor(0,1);
    LCD.write("PRESS BUTTON");
    delay(500);
    while(true) {
      SW = analogRead(SW_pin);
      delay(updatespeed);
      if (SW<100) {
        LCD.clear();
        LCD.setCursor(0,0);
        LCD.write("RESUME IN");
        for (int i = 3; i >= 0; i = i-1){
          LCD.setCursor(0,1);
          LCD.print(i);
          LCD.write(" SECONDS");
          delay(1000);
        }
        break;
      }
    }
  }
  
  if (x_joystick < 400) {
    x_delta = 1;
  }
  else if (x_joystick > 600) {
    x_delta = -1;
  }
  else {
    x_delta = 0;
  }

  if (y_joystick < 400) {
    y_delta = -1;
  }
  else if (y_joystick > 600) {
    y_delta = 1;
  }
  else {
    y_delta = 0;
  }

  // CALCULATE CURSOR POSITION
  if (cursorcounter >= cursorspeed) {
    x = x + x_delta;
    y = y + y_delta;
    cursorcounter = 0;
  }
  
  switch (x) {
    case -1:
      x = 31;
      break;
    case 32:
      x = 0;
      break;
  }

  switch (y) {
    case -1:
      y = 5;
      break;
    case 6:
      y = 0;
      break;
  }
  
  x_lcd = x/2;
  x_char = x%2;
  y_lcd = y/3;
  y_char = y%3;

  // CALCULATE BLOCK POSITION
  if (blockcounter1 >= blockspeed1) {
    block1 = block1 -1;
    blockcounter1 = 0;
  }
  if (blockcounter2 >= blockspeed2) {
    block2 = block2 -1;
    blockcounter2 = 0;
  }
  if (block1 < 0) {
    block1 = 15;
  }

  if (block2 < 0) {
    block2 = 15;
  }
  
  // OUTPUT
  /*Serial.print("Block1 ");
  Serial.println(block1);
  Serial.print("Block2 ");
  Serial.println(block2);
  Serial.print("Cursor X ");
  Serial.println(x);
  Serial.print("Cursor Y ");
  Serial.println(y);
  Serial.print("SW ");
  Serial.println(SW);
  Serial.println("--");*/

  // SET CURSOR POSITION
  LCD.setCursor(x_lcd,y_lcd);
  pos_char = 2*y_char+x_char;
  LCD.write(byte(pos_char));

  // SET BLOCK POSITION
  LCD.setCursor(block1,0);
  LCD.write(byte(7));

  LCD.setCursor(block2,1);
  LCD.write(byte(7));

  
  // RETURN
  counter = counter + updatespeed;
  blockcounter1 = blockcounter1 + updatespeed;
  blockcounter2 = blockcounter2 + updatespeed;
  cursorcounter = cursorcounter + updatespeed;
  
  if (counter >= 15000) {
    level = level + 1;
    counter = 0;
  }
  delay(updatespeed);

  
  // CHECK LOSE
  if ((y_lcd == 0 && x_lcd == block1) || (y_lcd == 1 && x_lcd == block2)) {
    LCD.clear();
    LCD.setCursor(0,0);
    LCD.write("YOU LOSE!");
    level = 1;
    counter = 0;
    delay(2000);
    LCD.clear();
  }
}
