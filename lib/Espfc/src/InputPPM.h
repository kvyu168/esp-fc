#ifndef _INPUT_PPM_H_
#define _INPUT_PPM_H_

#include "Arduino.h"
#include "InputDevice.h"

namespace Espfc {

class InputPPM: public InputDevice
{
  public:
    void begin(uint8_t pin, int mode = RISING);
    void handle() ICACHE_RAM_ATTR;
    static void handle_isr() ICACHE_RAM_ATTR;

    bool fail()
    {
      // sometimes micros() is lower than _last_tick,
      // probably ISR occured before comparision,
      // small time compensation allows to avoid false detection
      uint32_t now = micros() + 1000;
      uint32_t delta = now - _last_tick;
      return delta > BROKEN_LINK_US;
    }

    uint16_t get(uint8_t i) const override
    {
      return _channels[i];
    }

    InputStatus update() override
    {
      if(_new_data)
      {
        _new_data = false;
        return INPUT_RECEIVED;
      }
      if(fail()) return INPUT_FAILED;
      return INPUT_IDLE;
    }

    static const size_t CHANNELS = 16;
    static const uint32_t BROKEN_LINK_US = 100000UL; // 100ms

  private:
    volatile uint16_t _channels[CHANNELS];
    volatile uint32_t _last_tick;
    volatile uint8_t  _channel;
    volatile bool     _new_data;
    uint8_t _pin;
    static InputPPM * _instance;
};

}

#endif
