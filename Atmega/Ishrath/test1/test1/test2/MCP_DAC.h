#pragma once
//
//    FILE: MCP_DAC.h
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
//    DATE: 2021-02-03
// PURPOSE: Arduino library for MCP_DAC
//     URL: https://github.com/RobTillaart/MCP_DAC
//


#include "Arduino.h"
#include "SPI.h"


#define MCP_DAC_LIB_VERSION       (F("0.1.0"))


///////////////////////////////////////////////////////////////
//
// BASE CLASS
//
class MCP_DAC
{
public:
  MCP_DAC(uint8_t dataOut = 255, uint8_t clock = 255);
  // if only select is given ==> HW SPI
  void     begin(uint8_t select);

  
  //       0 or 1
  uint8_t  channels() { return _channels; };
  uint16_t maxValue() { return _maxValue; };

  //       gain = 1 or 2
  bool     setGain(uint8_t gain);
  uint8_t  getGain()  { return _gain; };

  bool     analogWrite(uint16_t value, uint8_t channel = 0);
  uint16_t lastValue(uint8_t channel = 0) { return _value[channel]; };
  void     fastWriteA(uint16_t value);
  void     fastWriteB(uint16_t value);
  //       convenience wrappers
  void     setPercentage(float perc, uint8_t channel = 0);  //  perc = 0..100.0%
  float    getPercentage(uint8_t channel);

  //       trigger LDAC = LatchDAC pin - if not set it does nothing
  void     setLatchPin( uint8_t latchPin);
  void     triggerLatch();

  //       shutDown - Page 21  ==> write will wake up.
  void     shutDown();
  bool     isActive()  { return _active; };

  //       speed in Hz
  void     setSPIspeed(uint32_t speed) { _SPIspeed = speed; };
  uint32_t getSPIspeed()               { return _SPIspeed; };

  //
  //       MCP49xxx series only
  //
  //       see page 20 ==> not functional for MCP48xx series.
  void     setBufferedMode(bool mode) { _buffered = mode; };
  bool     getBufferedMode()          { return _buffered; };


protected:
  uint8_t  _dataOut;
  uint8_t  _clock;
  uint8_t  _select;
  uint8_t  _latchPin = 255;;
  bool     _hwSPI;
  uint32_t _SPIspeed = 16000000;

  uint8_t  _channels;
  uint16_t _maxValue;
  uint16_t _value[2];
  uint8_t  _gain;
  bool     _buffered = false;
  bool     _active   = true;

  void     transfer(uint16_t data);
  uint8_t  swSPI_transfer(uint8_t d);
};


///////////////////////////////////////////////////////////////
//
// 4800 Series
//
class MCP4801 : public MCP_DAC
{
public:
  MCP4801(uint8_t dataOut = 255, uint8_t clock = 255);
};


class MCP4802 : public MCP_DAC
{
public:
  MCP4802(uint8_t dataOut = 255, uint8_t clock = 255);
};


class MCP4811 : public MCP_DAC
{
public:
  MCP4811(uint8_t dataOut = 255, uint8_t clock = 255);
};


class MCP4812 : public MCP_DAC
{
public:
  MCP4812(uint8_t dataOut = 255, uint8_t clock = 255);
};


class MCP4821 : public MCP_DAC
{
public:
  MCP4821(uint8_t dataOut = 255, uint8_t clock = 255);
};


class MCP4822 : public MCP_DAC
{
public:
  MCP4822(uint8_t dataOut = 255, uint8_t clock = 255);
};


///////////////////////////////////////////////////////////////
//
// 4900 Series
//
class MCP4901 : public MCP_DAC
{
public:
  MCP4901(uint8_t dataOut = 255, uint8_t clock = 255);
};


class MCP4902 : public MCP_DAC
{
public:
  MCP4902(uint8_t dataOut = 255, uint8_t clock = 255);
};


class MCP4911 : public MCP_DAC
{
public:
  MCP4911(uint8_t dataOut = 255, uint8_t clock = 255);
};


class MCP4912 : public MCP_DAC
{
public:
  MCP4912(uint8_t dataOut = 255, uint8_t clock = 255);
};


class MCP4921 : public MCP_DAC
{
public:
  MCP4921(uint8_t dataOut = 255, uint8_t clock = 255);
};


class MCP4922 : public MCP_DAC
{
public:
  MCP4922(uint8_t dataOut = 255, uint8_t clock = 255);
};


// -- END OF FILE --

