#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "CS1628.h"

uint8_t digits[7]; //Contains data to be written to display memory
uint8_t _intensity = 4;

CS1628::CS1628(uint8_t _dio_pin, uint8_t _clk_pin, uint8_t _stb_pin)
{
  this->_dio_pin = _dio_pin;
  this->_clk_pin = _clk_pin;
  this->_stb_pin = _stb_pin;

  pinMode(_dio_pin, OUTPUT);
  pinMode(_clk_pin, OUTPUT);
  pinMode(_stb_pin, OUTPUT);

  digitalWrite(_stb_pin, HIGH);
  digitalWrite(_clk_pin, HIGH);

  // setData(false, false, false); //command 2 //write data
  // setAddress(0x00);             //command 3 //clear display ram
  // setDisplayMode(0);            //command 1
  // setDisplayControl(true, 7);   //command 4 //display on
}

void CS1628::begin(boolean active = true, byte intensity = 4)
{
  sendCommand(0x80 | (active ? 8 : 0) | min(7, intensity)); //0x80
}

void CS1628::sendCommand(uint8_t data)
{
  digitalWrite(_stb_pin, LOW);
  shiftOut(_dio_pin, _clk_pin, LSBFIRST, data);
  digitalWrite(_stb_pin, HIGH);
}

void CS1628::setDisplayMode(uint8_t mode)
{
  sendCommand(min(3, mode));
}

void CS1628::setData(bool testMode, bool fixedAddress, bool readKey)
{
  sendCommand(0x40 | (testMode ? 8 : 0) | (fixedAddress ? 4 : 0) | (readKey ? 2 : 0));
}

void CS1628::setAddress(uint8_t address)
{
  sendCommand(0xC0 | min(13, address));
}

void CS1628::setDisplayControl(bool displayOn, uint8_t intensity = 4)
{
  _intensity = intensity;
  sendCommand(0x80 | (displayOn ? 8 : 0) | min(7, intensity));
}

void CS1628::displayNumber(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, bool colon = false)
{
  digits[6] = NUMBER_FONT[d3];
  digits[4] = NUMBER_FONT[d2];
  digits[2] = (colon ? 0x80 : 0) | NUMBER_FONT[d1];
  digits[0] = NUMBER_FONT[d0];

  digitalWrite(_stb_pin, LOW);
  shiftOut(_dio_pin, _clk_pin, LSBFIRST, 0xC0); //command 3
  for (int i = 0; i < 7; i++)
  {
    shiftOut(_dio_pin, _clk_pin, LSBFIRST, digits[i]); //command 3
  }
  digitalWrite(_stb_pin, HIGH);

  setDisplayMode(0x00);                //command 1
  setDisplayControl(true, _intensity); //command 4
}

void CS1628::displayText(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, bool colon = false)
{
  digits[6] = FONT_DEFAULT[d3 - 0x20];
  digits[4] = FONT_DEFAULT[d2 - 0x20];
  digits[2] = (colon ? 0x80 : 0) | FONT_DEFAULT[d1 - 0x20];
  digits[0] = FONT_DEFAULT[d0 - 0x20];

  digitalWrite(_stb_pin, LOW);
  shiftOut(_dio_pin, _clk_pin, LSBFIRST, 0xC0); //command 3
  for (int i = 0; i < 7; i++)
  {
    shiftOut(_dio_pin, _clk_pin, LSBFIRST, digits[i]); //command 3
  }
  digitalWrite(_stb_pin, HIGH);

  setDisplayMode(0x00);                //command 1
  setDisplayControl(true, _intensity); //command 4
}

uint8_t CS1628::receive()
{
  uint8_t temp;

  // Pull-up on
  pinMode(_dio_pin, INPUT);
  digitalWrite(_dio_pin, HIGH);

  for (int i = 0; i < 8; i++)
  {
    temp >>= 1;

    digitalWrite(_clk_pin, LOW);

    if (digitalRead(_dio_pin))
    {
      temp |= 0x80;
    }

    digitalWrite(_clk_pin, HIGH);
  }

  // Pull-up off
  pinMode(_dio_pin, OUTPUT);
  digitalWrite(_dio_pin, LOW);

  return temp;
}

uint8_t CS1628::getButtons()
{
  uint8_t keys = 0;

  digitalWrite(_stb_pin, LOW);
  shiftOut(_dio_pin, _clk_pin, LSBFIRST, 0x42); //command 2
  for (int i = 0; i < 5; i++)
  {
    keys |= receive() << i;
  }
  digitalWrite(_stb_pin, HIGH);
  setDisplayMode(0x00); //command 1
  //setDisplayControl(true, 7); //command 4
  return keys;
}