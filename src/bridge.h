#pragma once
#include "kaleidoscope/KeyEvent.h"

namespace RustreusBridge {
enum Action {
  PassThroughAction   = 0,
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
  uint8_t action;
  uint8_t key_code;
};

struct KeyState {
  uint8_t key_code;
  uint8_t is_pressed;
  uint8_t was_pressed;
  uint8_t toggle;
};

void rustreus_handle_message(const KeyState *const, Response *res);
uint8_t serialize_key_state(KeyState *key_state, uint8_t max_len, uint8_t *bytes);
void deserialize_response(const uint8_t *bytes, Response *res);
}

KeyState build_key_state(KeyEvent &event) {
  KeyState state = {
    .key_code    = event.key.getKeyCode(),
    .is_pressed  = keyIsPressed(event.state) ? 1 : 0,
    .was_pressed = keyWasPressed(event.state) ? 1 : 0,
    .toggle      = ToggledUnknown};

  if keyToggledOn (event.state) {
    state.toggle = ToggledOn;
  } else if (keyToggledOff(event.state)) {
    state.toggle = ToggledOff;
  };

  return state;
}
}  // namespace RustreusBridge
