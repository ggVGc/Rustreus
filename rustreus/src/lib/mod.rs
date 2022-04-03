pub enum Action {
  None,
  PressKey(u8),
  ReleaseKey(u8),
}

pub enum Toggled {
  On,
  Off,
  Unknown,
}

pub struct Response {
  pub action: Action,
  pub other: i8,
}

pub struct KeyState {
  pub key_code: u8,
  pub is_pressed: bool,
  pub was_pressed: bool,
  pub toggle: Toggled,
}

pub fn handle_key(key_state: KeyState) -> Response {
  Response {
    other: if let Toggled::On = key_state.toggle {
      22
    } else {
      99
    },
    action: if key_state.is_pressed {
      Action::PressKey(key_state.key_code)
    } else {
      Action::ReleaseKey(key_state.key_code)
    },
  }
}
