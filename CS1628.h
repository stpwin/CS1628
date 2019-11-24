#ifndef CS1628_h
#define CS1628_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "TM16XXFonts.h"

class CS1628
{
public:
  CS1628(uint8_t _dio_pin, uint8_t _clk_pin, uint8_t _stb_pin);

  void begin(boolean active, uint8_t intensity = 4);
  void setDisplayMode(uint8_t mode);
  void setData(bool testMode, bool fixedAddress, bool readKey);
  void setAddress(uint8_t address);
  void setDisplayControl(bool displayOn, uint8_t intensity = 4);
  void displayText(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, bool colon = false);
  void displayNumber(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, bool colon = false);
  uint8_t getButtons();

protected:
  void sendCommand(uint8_t data);
  uint8_t receive();
  uint8_t _dio_pin;
  uint8_t _clk_pin;
  uint8_t _stb_pin;
};

#endif