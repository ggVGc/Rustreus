#pragma once

#include <stdint.h>

extern "C" int32_t rustreus_handle_message(int8_t, int8_t, int8_t, int8_t);


#include "kaleidoscope/KeyEvent.h"
#include "kaleidoscope/Runtime.h"
#include "kaleidoscope/event_handler_result.h"
#include "kaleidoscope/plugin.h"
#include "Kaleidoscope-FocusSerial.h"

const int PRESS   = 1;
const int RELEASE = 2;


namespace kaleidoscope {
namespace plugin {
class RustreusPlugin : public Plugin {
 public:
  EventHandlerResult onKeyEvent(KeyEvent &event) {
    uint8_t toggle_state = 2;
    if keyToggledOn (event.state) {
      toggle_state = 1;
    } else if (keyToggledOff(event.state)) {
      toggle_state = 0;
    };

    int8_t was_pressed = keyWasPressed(event.state);
    int8_t is_pressed  = keyIsPressed(event.state);

    const auto key_code = event.key.getKeyCode();
    Focus.send(key_code, is_pressed, was_pressed, toggle_state, '\n');

    /* const auto ret      = rustreus_handle_message(key_code, is_pressed, was_pressed, toggle_state); */

    return EventHandlerResult::OK;
  }

  EventHandlerResult onFocusEvent(const char *command) {
    int action = 0;

    if (strcmp_P(command, PSTR("rustreus.pressKey")) == 0) {
      action = PRESS;
    } else if (strcmp_P(command, PSTR("rustreus.releaseKey")) == 0) {
      action = RELEASE;
    }

    if (action != 0) {
      Runtime.serialPort().println(command);
      handle_action(action);
      return EventHandlerResult::EVENT_CONSUMED;
    }

    return EventHandlerResult::OK;
  }

  void handle_action(int action) {
    if (action == PRESS || action == RELEASE) {
      const uint8_t key_code = Runtime.serialPort().read();
      Runtime.serialPort().println(key_code);

      if (action == PRESS) {
        Runtime.handleKeyEvent(KeyEvent(KeyAddr::none(), INJECTED | IS_PRESSED, Key(key_code, KEY_FLAGS)));
      } else if (action == RELEASE) {
        Runtime.handleKeyEvent(KeyEvent(KeyAddr::none(), INJECTED, Key(key_code, KEY_FLAGS)));
      }
    }
  }
};
}  // namespace plugin
}  // namespace kaleidoscope

kaleidoscope::plugin::RustreusPlugin Rustreus;
