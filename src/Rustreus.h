#pragma once

#include <stdint.h>
#include "bridge.h"
#include "kaleidoscope/KeyEvent.h"
#include "kaleidoscope/Runtime.h"
#include "kaleidoscope/event_handler_result.h"
#include "kaleidoscope/plugin.h"
#include "Kaleidoscope-FocusSerial.h"

namespace kaleidoscope {
namespace plugin {
class RustreusPlugin : public Plugin {
 public:
  EventHandlerResult onKeyEvent(KeyEvent &event) {
    /* Focus.send(key_code, is_pressed, was_pressed, toggle_state, '\n'); */

    const auto key_state = RustreusBridge::build_key_state(event);

    RustreusBridge::Response res;
    rustreus_handle_message(&key_state, &res);
    if (res.action == RustreusBridge::PressKey) {
      Focus.send("Press", res.key_code, res.other, '\n');
    } else if (res.action == RustreusBridge::ReleaseKey) {
      Focus.send("Release", res.key_code, res.other, '\n');
    }

    return EventHandlerResult::OK;
  }

  EventHandlerResult onFocusEvent(const char *command) {
    /* int action = 0; */

    /* if (strcmp_P(command, PSTR("rustreus.pressKey")) == 0) { */
    /*   action = PRESS; */
    /* } else if (strcmp_P(command, PSTR("rustreus.releaseKey")) == 0) { */
    /*   action = RELEASE; */
    /* } */

    /* if (action != 0) { */
    /*   Runtime.serialPort().println(command); */
    /*   handle_action(action); */
    /*   return EventHandlerResult::EVENT_CONSUMED; */
    /* } */

    return EventHandlerResult::OK;
  }

  /* void handle_action(int action) { */
  /*   if (action == PRESS || action == RELEASE) { */
  /*     const uint8_t key_code = Runtime.serialPort().read(); */
  /*     Runtime.serialPort().println(key_code); */

  /*     if (action == PRESS) { */
  /*       Runtime.handleKeyEvent(KeyEvent(KeyAddr::none(), INJECTED | IS_PRESSED, Key(key_code, KEY_FLAGS))); */
  /*     } else if (action == RELEASE) { */
  /*       Runtime.handleKeyEvent(KeyEvent(KeyAddr::none(), INJECTED, Key(key_code, KEY_FLAGS))); */
  /*     } */
  /*   } */
  /* } */
};
}  // namespace plugin
}  // namespace kaleidoscope

kaleidoscope::plugin::RustreusPlugin Rustreus;
