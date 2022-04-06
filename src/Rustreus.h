#pragma once

#include <stdint.h>
#include "bridge.h"
#include "kaleidoscope/KeyEvent.h"
#include "kaleidoscope/Runtime.h"
#include "kaleidoscope/event_handler_result.h"
#include "kaleidoscope/plugin.h"
#include "Kaleidoscope-FocusSerial.h"


static void hex_encode(const uint8_t *bytes, size_t bytes_len, char *out, size_t max_out_len) {
  const char *hex       = "0123456789ABCDEF";
  const uint8_t *reader = bytes;
  char *writer          = out;
  const int step        = 2;
  size_t written        = 0;

  for (; reader < bytes + bytes_len; writer += step, reader++) {
    written += step;
    if (written > max_out_len) {
      // Truncate rather than overflow buffer
      return;
    }

    writer[0] = hex[(*reader >> 4) & 0xF];
    writer[1] = hex[*reader & 0xF];
  }
}


static void send_key_state(RustreusBridge::KeyState key_state) {
  uint8_t max_len = 255;
  uint8_t bytes[max_len + 1];
  const auto len = serialize_key_state(&key_state, max_len, bytes);

  const size_t out_len = len * 2;
  char out[out_len + 1];
  hex_encode(bytes, len, out, out_len);
  out[out_len] = 0;

  Focus.send("rustreus.key_state", out, '\n');
}
static const uint8_t response_max_bytes = 255;

namespace kaleidoscope {
namespace plugin {
class RustreusPlugin : public Plugin {
 private:
  /* uint16_t response_counter = 0; */
  /* bool has_response         = false; */
  /* uint8_t response_id       = 0; */
  /* uint8_t response_bytes[response_max_bytes + 1]; */

 public:
  EventHandlerResult onKeyEvent(KeyEvent &event) {
    const auto key_state = RustreusBridge::build_key_state(event);

    RustreusBridge::Response res;
    rustreus_handle_message(&key_state, &res);
    /* send_key_state(key_state); */
    return handle_response(res);
  }

  /*
  EventHandlerResult beforeEachCycle() {
    if (has_response) {
      //Focus.send("rustreus.response", response.key_code, '\n');
      //Runtime.serialPort().flush();

      has_response = false;
      Runtime.serialPort().flush();
      RustreusBridge::Response response;
      uint8_t bytes[response_max_bytes + 1];
      for (int i = 0; i < 3; ++i) {
        bytes[i] = this->response_bytes[i];
      }
      // deserialize_response(bytes, &response);
      Focus.send("rustreus.response", response_id, response.key_code, '\n');

      //handle_response(response);
      //return EventHandlerResult::EVENT_CONSUMED;
      return EventHandlerResult::OK;
    } else {
      return EventHandlerResult::OK;
    }
  }
  */

  EventHandlerResult handle_response(RustreusBridge::Response res) {
  if (res.action == PassThroughAction) {
    return EventHandlerResult::OK;
  } else {
    handle_action((RustreusBridge::Action)res.action, res.key_code);
    return EventHandlerResult::EVENT_CONSUMED;
  }
}

void handle_action(RustreusBridge::Action action, uint8_t key_code) {
  switch (action) {
  case RustreusBridge::PressKey:
    Focus.send("Press", key_code, '\n');
    Runtime.handleKeyEvent(KeyEvent(KeyAddr::none(), INJECTED | IS_PRESSED, Key(key_code, KEY_FLAGS)));
    break;
  case RustreusBridge::ReleaseKey:
    Focus.send("Release", key_code, '\n');
    Runtime.handleKeyEvent(KeyEvent(KeyAddr::none(), INJECTED, Key(key_code, KEY_FLAGS)));
    break;
  case RustreusBridge::NoAction:
    break;
  }
}

/*
EventHandlerResult onFocusEvent(const char *command) {
  if (!LOCAL_CONTROL && strcmp_P(command, PSTR("rustreus.cmd")) == 0) {
    if (!has_response) {
      response_id         = Runtime.serialPort().read();
      const uint8_t count = Runtime.serialPort().read();

      if (count < response_max_bytes) {
        const auto read_count = Runtime.serialPort().readBytes(this->response_bytes, count);
        Focus.send("rustreus.response_read", read_count);
        has_response = true;
      }

      return EventHandlerResult::EVENT_CONSUMED;
    }
  }
  return EventHandlerResult::OK;
}
*/

};
}  // namespace plugin
}  // namespace kaleidoscope

kaleidoscope::plugin::RustreusPlugin Rustreus;
