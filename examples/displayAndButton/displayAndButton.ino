/*
  ------COMMANDS-------
  Display Mode Settings (0x00)
      MSB            LSB
      0 0 - - - - b1 b0
                  |__|
                     |_00: 4 Grids, 13 Segments
                     |_01: 5 Grids, 12 Segments
                     |_10: 6 Grids, 11 Segments
                     |_11: 7 Grids, 10 Segments

  Data Settings (0x40)
      MSB              LSB
      0 1 - - b3 b2 b1 b0
               |  |  |__| *Data Write/Read Mode Settings
               |  |     |_00: Write Data to DisplayMode
               |  |     |_10: Read Key data
               |  |
               |  | *Address Increment Mode setting (Display Mode)
               |  |_0: Increment address after data has been written
               |  |_1: Fixed Address
               |
               | *Mode Setting
               |_0: Normal Operation
               |_1: Test Mode

  Address Setting (0xC0)
      MSB              LSB
      1 1 - - b3 b2 b1 b0
               |________| *Address: 0x00 to 0x0D

  Display Control (0x80)
      MSB              LSB
      1 0 - - b3 b2 b1 b0
               |  |_____| *Dimming Quantity Settings
               |        |_000: Pulse width = 1/16
               |        |_001: Pulse width = 2/16
               |        |_010: Pulse width = 4/16
               |        |_011: Pulse width = 10/16
               |        |_100: Pulse width = 11/16
               |        |_101: Pulse width = 12/16
               |        |_110: Pulse width = 13/16
               |        |_111: Pulse width = 14/16
               |
               | *Display Settings
               |_0: Display OFF (Key Scan Continues)
               |_1: Display ON
  */

#include "CS1628.h"

CS1628 lcd(A0, A1, A2);

bool lastBtn0Pressed;
bool lastBtn1Pressed;
bool _colon = true;

int _c;
int _x;
void plus()
{
  _c++;
  if (_c > 9)
  {
    _c = 0;
    _x++;
    if (_x > 9)
      _x = 0;
  };
}

void minus()
{
  _c--;
  if (_c < 0)
  {
    _c = 9;
    _x--;
    if (_x < 0)
      _x = 9;
  };
}

void displayUpdate()
{
  //lcd.displayText(((_x * 10) + _c) + 0x20, '-', 48 + _x, 48 + _c, _colon);
  lcd.displayNumber(lastBtn0Pressed ? 1 : 0, lastBtn1Pressed ? 1 : 0, _x, _c, _colon);
}

uint8_t button;
unsigned long holdDelay;
unsigned long buttonDelay;
void buttonLoop()
{
  if (millis() - buttonDelay >= 60)
  {
    buttonDelay = millis();

    button = lcd.getButtons();
    bool curBtn0 = (button & 0x4) == 0x4;
    bool curBtn1 = (button & 0x10) == 0x10;
    if (lastBtn0Pressed != curBtn0)
    {
      lastBtn0Pressed = curBtn0;
      holdDelay = millis();
      if (curBtn0)
      {
        plus();
        displayUpdate();
      }
    }
    else
    {
      if (curBtn0)
      {
        if (millis() - holdDelay > 500)
        {
          plus();
          displayUpdate();
        }
      }
    }

    if (lastBtn1Pressed != curBtn1)
    {
      lastBtn1Pressed = curBtn1;
      holdDelay = millis();
      if (curBtn1)
      {
        minus();
        displayUpdate();
      }
    }
    else
    {
      if (curBtn1)
      {
        if (millis() - holdDelay > 500)
        {
          minus();
          displayUpdate();
        }
      }
    }
  }
}

unsigned long displayDelay;
void displayLoop()
{
  if (millis() - displayDelay >= 1000)
  {
    displayDelay = millis();
    _colon = !_colon;
    displayUpdate();
  }
}

void setup()
{
  Serial.begin(115200);
  lcd.setDisplayControl(true, 7);
}

void loop()
{
  buttonLoop();
  displayLoop();
}