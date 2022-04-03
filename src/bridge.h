#pragma once
#include "kaleidoscope/KeyEvent.h"

namespace RustreusBridge {
enum Action {
  NoAction   = 0,
  PressKey   = 1,
  ReleaseKey = 2
};

enum Toggled {
  ToggledUnknown = 0,
  ToggledOff     = 1,
  ToggledOn      = 2,
};

extern "C" {
struct Response {
  int8_t action;
  uint8_t key_code;
  int8_t other;
};

struct KeyState {
  uint8_t key_code;
  int8_t is_pressed;
  int8_t was_pressed;
  int8_t toggle;
};

void rustreus_handle_message(const KeyState *const, Response *res);
uint8_t serialize_key_state(KeyState *key_state, uint8_t max_len, uint8_t *bytes);
void deserialize_response(const uint8_t *bytes, Response *res);
}

KeyState build_key_state(KeyEvent &event) {
  KeyState state = {
    .key_code    = event.key.getKeyCode(),
    .is_pressed  = (int8_t)(keyIsPressed(event.state) ? 1 : 0),
    .was_pressed = (int8_t)(keyWasPressed(event.state) ? 1 : 0),
    .toggle      = (int8_t)ToggledUnknown};

  if keyToggledOn (event.state) {
    state.toggle = ToggledOn;
  } else if (keyToggledOff(event.state)) {
    state.toggle = ToggledOff;
  };

  return state;
}
}  // namespace RustreusBridge
