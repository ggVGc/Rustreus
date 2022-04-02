#pragma once

#include <stdint.h>

extern "C" int32_t rustreus_handle_message(int8_t, int8_t, int8_t, int8_t);


#include "kaleidoscope/KeyEvent.h"
#include "kaleidoscope/Runtime.h"
#include "kaleidoscope/event_handler_result.h"
#include "kaleidoscope/plugin.h"
#include "Kaleidoscope-FocusSerial.h"


namespace kaleidoscope {
namespace plugin {
  class RustreusPlugin : public Plugin {
    public:
      EventHandlerResult onKeyEvent(KeyEvent &event){
        Focus.send("KeyEvent: ");
        int8_t toggle_state = 0;
        if keyToggledOn(event.state){
          toggle_state = 1;
        }else if(keyToggledOff(event.state)){
          toggle_state = 2;
        };

        int8_t was_pressed = !keyWasPressed(event.state);
        int8_t is_pressed = !keyIsPressed(event.state);

        const auto key_code = event.key.getKeyCode();
        const auto ret = rustreus_handle_message(key_code, is_pressed, was_pressed, toggle_state);
        char buf[64];
        sprintf(buf, "%li\n", ret);
        Focus.send(buf);

        return EventHandlerResult::OK;
      }
  };
}}

kaleidoscope::plugin::RustreusPlugin Rustreus;
