use desse::{Desse, DesseSized};

#[derive(Desse, DesseSized)]
pub enum Action {
  PassThrough,
  PressKey(u8),
  ReleaseKey(u8),
}

#[derive(Desse, DesseSized)]
pub enum Toggled {
  On,
  Off,
  Unknown,
}

#[derive(Desse, DesseSized)]
pub struct Response {
  pub action: Action,
}

#[derive(Desse, DesseSized)]
pub struct KeyState {
  pub key_code: u8,
  pub is_pressed: bool,
  pub was_pressed: bool,
  pub toggle: Toggled,
}

pub fn handle_key(key_state: KeyState) -> Response {
  Response {
    action: if key_state.is_pressed {
      Action::PressKey(key_state.key_code)
    } else {
      Action::ReleaseKey(key_state.key_code)
    },
  }
}
