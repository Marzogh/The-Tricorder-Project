  #include <pcd8544.h>
  #include <stdint.h>
  #include <Print.h>
  #include <stdint.h>
  
  /* PCD8544-specific defines: */
  #define LCD_COMMAND  0 
  #define LCD_DATA     1
  #define LCD_C     LOW
  #define LCD_D     HIGH
   
  /* 84x48 LCD Defines: */
  #define LCD_X   84 // Note: x-coordinates go wide
  #define LCD_Y  48 // Note: y-coordinates go high
  #define WHITE       0  // For drawing pixels. A 0 draws white.
  #define BLACK       1  // A 1 draws black.
  
  //LCD variables
  byte dc_pin = 10;    // Data/Command
  byte reset_pin = 9;
  byte cs_pin = 8;    // Chip select (SCE)
  byte sdin_pin = 11; //Serial Data In
  byte sclk_pin = 13; //Serial Clock
  const int LCD_LED=6;
  int scrollPosition = -10;
  
  static const byte ASCII[][5] =
  {
   {0x00, 0x00, 0x00, 0x00, 0x00} // 20
  ,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
  ,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
  ,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
  ,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
  ,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
  ,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
  ,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
  ,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
  ,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
  ,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
  ,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
  ,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
  ,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
  ,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
  ,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
  ,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
  ,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
  ,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
  ,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
  ,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
  ,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
  ,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
  ,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
  ,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
  ,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
  ,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
  ,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
  ,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
  ,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
  ,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
  ,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
  ,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
  ,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
  ,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
  ,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
  ,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
  ,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
  ,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
  ,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
  ,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
  ,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
  ,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
  ,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
  ,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
  ,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
  ,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
  ,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
  ,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
  ,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
  ,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
  ,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
  ,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
  ,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
  ,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
  ,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
  ,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
  ,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
  ,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
  ,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
  ,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c ¥
  ,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
  ,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
  ,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
  ,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
  ,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
  ,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
  ,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
  ,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
  ,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
  ,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
  ,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
  ,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
  ,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
  ,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j
  ,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
  ,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
  ,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
  ,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
  ,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
  ,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
  ,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
  ,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
  ,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
  ,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
  ,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
  ,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
  ,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
  ,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
  ,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
  ,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
  ,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
  ,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
  ,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
  ,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e ←
  ,{0x00, 0x06, 0x09, 0x09, 0x06} // 7f →
  };
  
  byte displayMap[LCD_X * LCD_Y / 8] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (0,0)->(11,7) ~ These 12 bytes cover an 8x12 block in the left corner of the display 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (12,0)->(23,7)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, // (24,0)->(35,7)
    0xF0, 0xF8, 0xFC, 0xFC, 0xFE, 0xFE, 0xFE, 0xFE, 0x1E, 0x0E, 0x02, 0x00, // (36,0)->(47,7)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (48,0)->(59,7)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (60,0)->(71,7)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (72,0)->(83,7)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (0,8)->(11,15)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (12,8)->(23,15)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, // (24,8)->(35,15)
    0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0xF8, // (36,8)->(47,15)
    0xF8, 0xF0, 0xF8, 0xFE, 0xFE, 0xFC, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00, // (48,8)->(59,15)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (60,8)->(71,15)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (72,8)->(83,15)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (0,16)->(11,23)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (12,16)->(23,23)
    0x00, 0x00, 0xF8, 0xFC, 0xFE, 0xFE, 0xFF, 0xFF, 0xF3, 0xE0, 0xE0, 0xC0, // (24,16)->(35,23)
    0xC0, 0xC0, 0xE0, 0xE0, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // (36,16)->(47,23)
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0x00, 0x00, 0x00, // (48,16)->(59,23)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (60,16)->(71,23)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (72,16)->(83,23)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (0,24)->(11,31)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (12,24)->(23,31)
    0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // (24,24)->(35,31)
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // (36,24)->(47,31)
    0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x1F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, // (48,24)->(59,31)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (60,24)->(71,31)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (72,24)->(83,31)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (0,32)->(11,39)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (12,32)->(23,39)
    0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x1F, // (24,32)->(35,39)
    0x0F, 0x0F, 0x0F, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x03, // (36,32)->(47,39)
    0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (48,32)->(59,39)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (60,32)->(71,39)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (72,32)->(83,39)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (0,40)->(11,47)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (12,40)->(23,47)
    0x00, 0x00, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, // (24,40)->(35,47)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (36,40)->(47,47)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (48,40)->(59,47)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (60,40)->(71,47)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (72,40)->(83,47) !!! The bottom right pixel!
  };
  
  
  
  // Create a pcd8544 object.
  // Hardware SPI will be used.
  // sdin (MOSI) is on pin 11 and sclk on pin 13.
  pcd8544 lcd(dc_pin, reset_pin, cs_pin);
  
  // If using soft SPU, use 5 arguments. Any pins can be choosen.
  // byte sdin_pin = 11, sclk_pin = 13;
  // pcd8544 lcd(dc_pin, reset_pin, cs_pin, sdin_pin, sclk_pin);
  
  
  
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ LCD Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
  
  
    //LCDWrite()
         void LCDWrite(byte data_or_command, byte data) 
          {
            //Tell the LCD that we are writing either to data or a command
            digitalWrite(dc_pin, data_or_command); 
           
            //Send the data
            digitalWrite(cs_pin, LOW);
            shiftOut(sdin_pin, sclk_pin, MSBFIRST, data);
            digitalWrite(cs_pin, HIGH);
          }
          
   
   //LCDInitialise()       
          void LCDInitialise(void)
          {
            pinMode(cs_pin,   OUTPUT);
            pinMode(reset_pin, OUTPUT);
            pinMode(dc_pin,    OUTPUT);
            pinMode(sdin_pin,  OUTPUT);
            pinMode(sclk_pin,  OUTPUT);
           
            digitalWrite(reset_pin, LOW);
            digitalWrite(reset_pin, HIGH);
           
            LCDWrite(LCD_COMMAND, 0x21);  // LCD Extended Commands.
            LCDWrite(LCD_COMMAND, 0xBf);  // Set LCD Vop (Contrast). //B1
            LCDWrite(LCD_COMMAND, 0x04);  // Set Temp coefficent. //0x04
            LCDWrite(LCD_COMMAND, 0x14);  // LCD bias mode 1:48. //0x13
            LCDWrite(LCD_COMMAND, 0x0C);  // LCD in normal mode. 0x0d for inverse
            LCDWrite(LCD_C, 0x20);
            LCDWrite(LCD_C, 0x0C);
            
            for (int i=0; i<100; i++)
            {
              analogWrite(LCD_LED, i);
              delay(10);
            }
          }
          
  //setContrast()
  
          void setContrast(byte contrast)
          {  
            LCDWrite(LCD_COMMAND, 0x21); //Tell LCD that extended commands follow
            LCDWrite(LCD_COMMAND, 0x80 | contrast); //Set LCD Vop (Contrast): Try 0xB1(good @ 3.3V) or 0xBF if your display is too dark
            LCDWrite(LCD_COMMAND, 0x20); //Set display mode
          }
  
  
  //gotoXY()
  
          void gotoXY(int x, int y)
          {
            LCDWrite(0, 0x80 | x);  // Column.
            LCDWrite(0, 0x40 | y);  // Row.  
          }
  
  
  //updateDisplay()
  
          void updateDisplay()
          {
            
            for (int i=0; i < (LCD_X * LCD_Y / 8); i++)
            {
              LCDWrite(LCD_DATA, displayMap[i]);
            }
          }
  
  
  
  //LCDClear()
  
          void LCDClear(void)
          {
            for (int index = 0; index < LCD_X * LCD_Y / 8; index++)
            {
              LCDWrite(LCD_D, 0x00);
            }
          }
  
  
  //drawBox()
  
          void drawBox(void)
          {
            int j;
            for(j = 0; j < 84; j++) // top
            {
              gotoXY(j, 0);
              LCDWrite(1, 0x01);
            }
           
            for(j = 0; j < 84; j++) //Bottom
            {
              gotoXY(j, 5);
              LCDWrite(1, 0x80);
            }
           
            for(j = 0; j < 6; j++) // Right
            {
              gotoXY(83, j);
              LCDWrite(1, 0xff);
            }
           
            for(j = 0; j < 6; j++) // Left
            {
              gotoXY(0, j);
              LCDWrite(1, 0xff);
            }
          }
  
  
  //LCDChar()
  
          void LCDChar(char character)
          {
            LCDWrite(LCD_D, 0x00);
            for (int index = 0; index < 5; index++)
            {
              LCDWrite(LCD_D, ASCII[character - 0x20][index]);
            }
            LCDWrite(LCD_D, 0x00);
          }
  
  
  
  //LCDString()
  
          void LCDString(char *characters)
          {
            while (*characters)
            {
              LCDChar(*characters++);
            }
          }
  
  
  /* //Unused functions
  
  //Scroll()
  
          void Scroll(String message)
          {
            for (int i = scrollPosition; i < scrollPosition + 11; i++)
            {
              if ((i >= message.length()) || (i < 0))
              {
                LCDChar(' ');
              }
              else
              {
                LCDChar(message.charAt(i));
              }
            }
            scrollPosition++;
            if ((scrollPosition >= message.length()) && (scrollPosition > 0))
            {
              scrollPosition = -10;
            }
          }
  
  
  
  
  //LCDBitmap
  
  void LCDBitmap(char my_array[])
  {
    for (int index = 0 ; index < (LCD_X * LCD_Y / 8) ; index++);
  
  }
  */
  
  
  //prepLCD()
  
  void prepLCD()
  {
    LCDClear(); 
    drawBox();
  }
  
  //screenBoot()
  void screenBoot()
  {
    gotoXY(15,1);
    LCDString("Moisture");
    gotoXY(22,4);
    LCDString("sensor");
    delay(3500);
  }

 //resample()
  void resample()
  {
    prepLCD();
    gotoXY(8,1);
    LCDString("Another ");
    gotoXY(15,4);
    LCDString("reading?");
    delay(500);
  }
  
  //LCDOff()
 // void LCDOff()
 // {
 //   digitalWrite(LCD_LED, LOW);
 // }